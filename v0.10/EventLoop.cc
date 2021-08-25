#include "EventLoop.h"
#include "Channel.h"

EventLoop::EventLoop()
    : quit_(false)
    , pPoller_(new Epoll())
    , conns_(nullptr)
    , pTimerQueue_(new TimerQueue(this)) {
    eventFd_ = CreateEventFd();
    pEventFdChannel_ = new Channel(this, eventFd_);
    pEventFdChannel_->SetCallBack(this);
    pEventFdChannel_->EnableReading();
    // cout << "[CONS] EventLoop ..." << endl;
}

EventLoop::~EventLoop() {
    if (pPoller_) {
        delete pPoller_;
        pPoller_ = nullptr;
    }
    if (pEventFdChannel_) {
        delete pEventFdChannel_;
        pEventFdChannel_ = nullptr;
    }
    if (pTimerQueue_) {
        delete pTimerQueue_;
        pTimerQueue_ = nullptr;
    }
    // cout << "[DECO] ~EventLoop ..." << endl;
}

void EventLoop::Loop() {
    while (!quit_) {
        vector<Channel*> pChannels;//先把指针存起来，最后再一个一个调HandleEvent
        pPoller_->Poll(pChannels);
        for (Channel* pChannel : pChannels) {
            pChannel->HandleEvent();
            if (pChannel->IsClosed()) {//处理完事件后，如果该fd 已经close 了就回收内存
                int fd = pChannel->GetFd();
                if (conns_->count(fd)) {
                    if ((*conns_)[fd]) {
                        delete (*conns_)[fd];
                        (*conns_)[fd] = nullptr;
                    }
                    conns_->erase(fd);
                }
                cout << "[INFO] EventLoop::Loop cur connection: " << conns_->size() << endl;
            }
        }
        //处理OnWriteComplete 事件
        //以及现在的timer 事件
        DoPendingFunctors();
    }
}

void EventLoop::Update(Channel* pChannel, int op) {
    pPoller_->Update(pChannel, op);
}

void EventLoop::QueueLoop(IRun* pRun, void* param) {
    // cout << "[DEBUG] EventLoop::QueueLoop: " << pendingFunctors_.size() << endl;
    vPendingFunctors_.push_back({pRun, param});
    //说实话，感觉这里有没有都行啊，确实，好像是多线程用的
    // WakeUp();
}

void EventLoop::SetConns(unordered_map<int, TcpConnection*>* conns) {
    conns_ = conns;
}

long EventLoop::RunAt(Timestamp when, IRun* pRun) {
    return pTimerQueue_->AddTimer(when, pRun, 0);
}

//delay是seconds
long EventLoop::RunAfter(double delay, IRun* pRun) {
    // cout << "[DEBUG] EventLoop::RunAfter" << endl;
    return pTimerQueue_->AddTimer(Timestamp::NowAfter(delay), pRun, 0);
}

//interval也是seconds?
long EventLoop::RunEvery(double interval, IRun* pRun) {
    return pTimerQueue_->AddTimer(Timestamp::NowAfter(interval), pRun, interval);
}

void EventLoop::CancelTimer(long timerId) {
    pTimerQueue_->CancelTimer(timerId);
}

void EventLoop::HandleRead() {
    uint64_t one = 1;
    int n = read(eventFd_, &one, sizeof(one));
    if (n < 1)
        ERR_EXIT("[ERRO] EventLoop::HandleRead read");
}

//暂时没用，因为不需要监听eventFd_ 的EPOLLOUT 事件
void EventLoop::HandleWrite() {}

int EventLoop::CreateEventFd() {
    int eventFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (eventFd < 0)
        ERR_EXIT("[ERRO] EventLoop::CreateEventFd eventfd");
    return eventFd;
}

void EventLoop::DoPendingFunctors() {
    vector<Runner> runners;
    runners.swap(vPendingFunctors_);
    for (Runner runner : runners)
        runner.DoRun();
}

void EventLoop::WakeUp() {
    uint64_t one = 1;
    int n = write(eventFd_, &one, sizeof(one));
    if (n < 1)
        ERR_EXIT("[ERRO] EventLoop::WakeUp write");
}
