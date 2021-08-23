#include "EventLoop.h"
#include "Channel.h"

EventLoop::EventLoop()
    : quit_(false)
    , poller_(new Epoll())
    , connections_(nullptr)
    , timerQueue_(new TimerQueue(this)) {
    eventFd_ = CreateEventFd();
    channel_ = new Channel(this, eventFd_);
    channel_->SetCallBack(this);
    channel_->EnableReading();
    // cout << "[CONS] EventLoop ..." << endl;
}

EventLoop::~EventLoop() {
    if (poller_) {
        delete poller_;
        poller_ = nullptr;
    }
    if (channel_) {
        delete channel_;
        channel_ = nullptr;
    }
    if (timerQueue_) {
        delete timerQueue_;
        timerQueue_ = nullptr;
    }
    // cout << "[DECO] ~EventLoop ..." << endl;
}

void EventLoop::Loop() {
    while (!quit_) {
        vector<Channel*> channels;//先把指针存起来，最后再一个一个调HandleEvent
        poller_->Poll(channels);
        for (Channel* pChannel : channels) {
            pChannel->HandleEvent();
            if (pChannel->IsClosed()) {//处理完事件后，如果该fd 已经close 了就回收内存
                int fd = pChannel->GetSockFd();
                if (connections_->count(fd)) {
                    if ((*connections_)[fd]) {
                        delete (*connections_)[fd];
                        (*connections_)[fd] = nullptr;
                    }
                    connections_->erase(fd);
                }
                cout << "[INFO] EventLoop::Loop cur connection: " << connections_->size() << endl;
            }
        }
        //处理OnWriteComplete 事件
        //以及现在的timer 事件
        DoPendingFunctors();
    }
}

void EventLoop::Update(Channel* channel, int op) {
    poller_->Update(channel, op);
}

void EventLoop::QueueLoop(IRun* run, void* param) {
    // cout << "[DEBUG] EventLoop::QueueLoop: " << pendingFunctors_.size() << endl;
    pendingFunctors_.push_back({run, param});
    //说实话，感觉这里有没有都行啊
    // WakeUp();
}

void EventLoop::SetConnections(unordered_map<int, TcpConnection*>* connections) {
    connections_ = connections;
}

int64_t EventLoop::RunAt(Timestamp when, IRun* run) {
    return timerQueue_->AddTimer(when, run, 0);
}

//delay是seconds
int64_t EventLoop::RunAfter(double delay, IRun* run) {
    // cout << "[DEBUG] EventLoop::RunAfter" << endl;
    return timerQueue_->AddTimer(Timestamp::NowAfter(delay), run, 0);
}

//interval也是seconds?
int64_t EventLoop::RunEvery(double interval, IRun* run) {
    return timerQueue_->AddTimer(Timestamp::NowAfter(interval), run, interval);
}

void EventLoop::CancelTimer(int64_t timerId) {
    timerQueue_->CancelTimer(timerId);
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
    runners.swap(pendingFunctors_);
    for (Runner runner : runners)
        runner.DoRun();
}

void EventLoop::WakeUp() {
    uint64_t one = 1;
    int n = write(eventFd_, &one, sizeof(one));
    if (n < 1)
        ERR_EXIT("[ERRO] EventLoop::WakeUp write");
}
