#include "TimerQueue.h"

TimerQueue::TimerQueue(EventLoop* pLoop)
    : timerFd_(CreateTimerFd())
    , pLoop_(pLoop)
    , pTimerFdchannel_(new Channel(pLoop, timerFd_))
    , pAddTimerWrapper_(new AddTimerWrapper(this))
    , pCancelTimerWrapper_(new CancelTimerWrapper(this)) {
    pTimerFdchannel_->SetCallBack(this);
    pTimerFdchannel_->EnableReading();
    // cout << "[CONS] TimerQueue ..." << endl;
}

TimerQueue::~TimerQueue() {
    if (pTimerFdchannel_) {
        delete pTimerFdchannel_;
        pTimerFdchannel_ = nullptr;
    }
    if (pAddTimerWrapper_) {
        delete pAddTimerWrapper_;
        pAddTimerWrapper_ = nullptr;
    }
    if (pCancelTimerWrapper_) {
        delete pCancelTimerWrapper_;
        pCancelTimerWrapper_ = nullptr;
    }
    for (auto it = pTimers_.begin(); it != pTimers_.end(); ++it) {
        if (it->second) {
            delete it->second;
        }
    }
    // cout << "[DECO] ~TimerQueue ..." << endl;
}

void TimerQueue::DoAddTimer(void* param) {
    // cout << "[DEBUG] TimerQueue::DoAddTimer" << endl;
    Timer* pTimer = static_cast<Timer*>(param);
    bool earliestChanged = Insert(pTimer);
    if (earliestChanged)
        ResetTimerFd(pTimer->GetStamp());
}

void TimerQueue::DoCancelTimer(void* param) {
    Timer* pTimer = static_cast<Timer*>(param);
    //避免取消错误的Timer
    if (pTimer == nullptr) return;
    Entry entry(pTimer->GetStamp(), pTimer);
    for (auto it = pTimers_.begin(); it != pTimers_.end(); ++it) {
        if (it->second == pTimer) {
            pTimers_.erase(it);
            delete pTimer; //回收Timer 内存
            //另外，在执行完一个interval = 0 的Timer 任务后也应该回收
            break;
        }
    }
}

long TimerQueue::AddTimer(Timestamp when, IRun* pRun, double interval) {
    // cout << "[DEBUG] TimerQueue::AddTimer" << endl;
    Timer* pTimer = new Timer(when, pRun, interval);
    pLoop_->QueueLoop(pAddTimerWrapper_, pTimer);
    return (long)pTimer;
}

void TimerQueue::CancelTimer(long timerId) {
    pLoop_->QueueLoop(pCancelTimerWrapper_, (void*)timerId);
}

void TimerQueue::HandleRead() {
    ReadTimerFd();
    
    vector<Entry> expired = GetExpired();
    for (auto entry : expired)
        entry.second->Run();
    Reset(expired);
}

//暂时没用
void TimerQueue::HandleWrite() {}

int TimerQueue::CreateTimerFd() {
    timerFd_ = timerfd_create(CLOCK_MONOTONIC,
        TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerFd_ < 0)
        ERR_EXIT("[ERRO] TimerQueue::CreateTimerFd timerfd_create");
    return timerFd_;
}

vector<TimerQueue::Entry> TimerQueue::GetExpired() {
    vector<Entry> expired;
    Entry entry(Timestamp::Now(), reinterpret_cast<Timer*>(UINTPTR_MAX));
    auto end = pTimers_.lower_bound(entry);
    //追加到expired 后面
    copy(pTimers_.begin(), end, back_inserter(expired));
    pTimers_.erase(pTimers_.begin(), end);
    return expired;
}

void TimerQueue::ReadTimerFd() {
    uint64_t one = 1;
    ssize_t n = read(timerFd_, &one, sizeof(one));
    if (n != sizeof(one))
        ERR_EXIT("[ERRO] TimerQueue::ReadTimerFd read");
}

void TimerQueue::Reset(const vector<Entry>& expired) {
    for (auto entry : expired) {
        if (entry.second->IsRepeat()) {
            entry.second->MoveToNext();
            Insert(entry.second);
        } else {
            //如果不是循环任务，直接回收吧
            // cout << "[DEBUG] TimerQueue::Reset " << entry.second->IsRepeat() << endl;
            delete entry.second;
        }
    }
    Timestamp nextExpire;
    if (!pTimers_.empty())
        nextExpire = pTimers_.begin()->second->GetStamp();
    if (nextExpire.Valid())
        ResetTimerFd(nextExpire);
}

void TimerQueue::ResetTimerFd(Timestamp stamp) {
    itimerspec newValue;
    itimerspec oldValue;
    bzero(&newValue, sizeof(newValue));
    bzero(&oldValue, sizeof(newValue));
    newValue.it_value = HowMuchTimeFromNow(stamp);
    int ret = timerfd_settime(timerFd_, 0, &newValue, &oldValue);
    if (ret)
        ERR_EXIT("[ERRO] TimerQueue::ResetTimerFd timerfd_settime");
}

bool TimerQueue::Insert(Timer* pTimer) {
    bool earliestChanged = false;
    Timestamp when = pTimer->GetStamp();
    if (pTimers_.empty() || when < pTimers_.begin()->first)
        earliestChanged = true;
    pair<TimerSet::iterator, bool> result
        = pTimers_.insert(Entry(when, pTimer));
    if (!result.second)
        ERR_EXIT("[ERRO] TimerQueue::Insert pTimers_.insert error");
    return earliestChanged;
}

timespec TimerQueue::HowMuchTimeFromNow(Timestamp when) {
    int64_t microSeconds = when.MicroSecondsSinceEpoch()
        - Timestamp::Now().MicroSecondsSinceEpoch();
    if (microSeconds < 100)
        microSeconds = 100;
    timespec ts;
    ts.tv_sec = static_cast<int64_t>(
        microSeconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<int64_t>(
        (microSeconds % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}
