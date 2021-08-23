#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include "Epoll.h"
#include "TcpConnection.h"
#include "TimerQueue.h"
#include "Headers.h"
#include "Define.h"

class EventLoop : public IChannelCallBack {
public:
    class Runner {
    public:
        Runner(IRun* run, void* param)
            : run_(run)
            , param_(param) {
            // cout << "[CONS] Runner ..." << endl;
        }
        virtual ~Runner() {
            // cout << "[DECO] ~Runner ..." << endl;
        }
        void DoRun() {
            run_->Run(param_);
        }
    private:
        IRun* run_;//如果是一个定时任务，run_是一个
        //如果是一个定时任务，param_是一个Timer，否则为nullptr
        void* param_;
    };
    EventLoop();
    virtual ~EventLoop();
    void Loop();
    void Update(Channel* channel, int op);
    void QueueLoop(IRun* run, void* param);
    void SetConnections(unordered_map<int, TcpConnection*>* connections);
    int64_t RunAt(Timestamp when, IRun* run);
    int64_t RunAfter(double delay, IRun* run);
    int64_t RunEvery(double interval, IRun* run);
    void CancelTimer(int64_t timerId);

    void HandleRead() override;
    void HandleWrite() override;

private:
    int CreateEventFd();
    void DoPendingFunctors();
    void WakeUp();

    bool quit_;
    int eventFd_;
    Channel* channel_;
    Epoll* poller_;
    unordered_map<int, TcpConnection*>* connections_;
    vector<Runner> pendingFunctors_;
    TimerQueue* timerQueue_;
};

#endif //_EVENT_LOOP_H_
