#include "TimerQueue.h"

TimerQueue::TimerQueue(EventLoop* loop)
    : timerFd_(CreateTimerFd())
    , loop_(loop)
    , channel_(new Channel(loop, timerFd_))
    , addTimerWrapper_(new AddTimerWrapper(this))
    , cancelTimerWrapper_(new CancelTimerWrapper(this)) {
    channel_->SetCallBack(this);
    channel_->EnableReading();
    // cout << "[CONS] TimerQueue ..." << endl;
}

TimerQueue::~TimerQueue() {
    if (channel_) {
        delete channel_;
        channel_ = nullptr;
    }
    if (addTimerWrapper_) {
        delete addTimerWrapper_;
        addTimerWrapper_ = nullptr;
    }
    if (cancelTimerWrapper_) {
        delete cancelTimerWrapper_;
        cancelTimerWrapper_ = nullptr;
    }
    for (auto it = timers_.begin(); it != timers_.end(); ++it) {
        if (it->second) {
            delete it->second;
        }
    }
    // cout << "[DECO] ~TimerQueue ..." << endl;
}

void TimerQueue::DoAddTimer(void* param) {
    // cout << "[DEBUG] TimerQueue::DoAddTimer" << endl;
    Timer* timer = static_cast<Timer*>(param);
    bool earliestChanged = Insert(timer);
    if (earliestChanged)
        ResetTimerFd(timerFd_, timer->GetStamp());
}

void TimerQueue::DoCancelTimer(void* param) {
    Timer* timer = static_cast<Timer*>(param);
    //避免取消错误的Timer
    if (timer == nullptr) return;
    Entry entry(timer->GetStamp(), timer);
    for (auto it = timers_.begin(); it != timers_.end(); ++it) {
        if (it->second == timer) {
            timers_.erase(it);
            delete timer; //回收Timer 内存
            //另外，在执行完一个interval = 0 的Timer 任务后也应该回收
            break;
        }
    }
}

int64_t TimerQueue::AddTimer(Timestamp when, IRun* run, double interval) {
    // cout << "[DEBUG] TimerQueue::AddTimer" << endl;
    Timer* timer = new Timer(when, run, interval);
    loop_->QueueLoop(addTimerWrapper_, timer);
    return (int64_t)timer;
}

void TimerQueue::CancelTimer(int64_t timerId) {
    loop_->QueueLoop(cancelTimerWrapper_, (void*)timerId);
}

void TimerQueue::HandleRead() {
    Timestamp now(Timestamp::Now());
    ReadTimerFd(timerFd_, now);
    
    vector<Entry> expired = GetExpired(now);
    for (auto entry : expired)
        entry.second->Run();
    Reset(expired, now);
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

vector<TimerQueue::Entry> TimerQueue::GetExpired(Timestamp now) {
    vector<Entry> expired;
    Entry entry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    auto end = timers_.lower_bound(entry);
    //追加到expired 后面
    copy(timers_.begin(), end, back_inserter(expired));
    timers_.erase(timers_.begin(), end);
    return expired;
}

void TimerQueue::ReadTimerFd(int timerFd, Timestamp now) {
    uint64_t howMany;
    ssize_t n = read(timerFd_, &howMany, sizeof(howMany));
    if (n != sizeof(howMany))
        ERR_EXIT("[ERRO] TimerQueue::ReadTimerFd read");
}

void TimerQueue::Reset(const vector<Entry>& expired, Timestamp now) {
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
    if (!timers_.empty())
        nextExpire = timers_.begin()->second->GetStamp();
    if (nextExpire.Valid())
        ResetTimerFd(timerFd_, nextExpire);
}

void TimerQueue::ResetTimerFd(int timerFd, Timestamp stamp) {
    itimerspec newValue;
    itimerspec oldValue;
    bzero(&newValue, sizeof(newValue));
    bzero(&oldValue, sizeof(newValue));
    newValue.it_value = HowMuchTimeFromNow(stamp);
    int ret = timerfd_settime(timerFd_, 0, &newValue, &oldValue);
    if (ret)
        ERR_EXIT("[ERRO] TimerQueue::ResetTimerFd timerfd_settime");
}

bool TimerQueue::Insert(Timer* timer) {
    bool earliestChanged = false;
    Timestamp when = timer->GetStamp();
    if (timers_.empty() || when < timers_.begin()->first)
        earliestChanged = true;
    pair<TimerSet::iterator, bool> result
        = timers_.insert(Entry(when, timer));
    if (!result.second)
        ERR_EXIT("[ERRO] TimerQueue::Insert timers_.insert error");
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
