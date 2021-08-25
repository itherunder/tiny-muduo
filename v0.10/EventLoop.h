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
        Runner(IRun* pRun, void* param)
            : pRun_(pRun)
            , param_(param) {
            // cout << "[CONS] Runner ..." << endl;
        }
        virtual ~Runner() {
            // cout << "[DECO] ~Runner ..." << endl;
        }
        void DoRun() {
            pRun_->Run(param_);
        }
    private:
        IRun* pRun_;//如果是一个定时任务，pRun_是一个
        //如果是一个定时任务，param_是一个Timer*，否则为nullptr
        void* param_;
    };
    EventLoop();
    virtual ~EventLoop();
    void Loop();
    void Update(Channel* pChannel, int op);
    void QueueLoop(IRun* pRun, void* param);
    void SetConns(unordered_map<int, TcpConnection*>* conns);
    long RunAt(Timestamp when, IRun* pRun);
    long RunAfter(double delay, IRun* pRun);
    long RunEvery(double interval, IRun* pRun);
    void CancelTimer(long timerId);

    void HandleRead() override;
    void HandleWrite() override;

private:
    int CreateEventFd();
    void DoPendingFunctors();
    void WakeUp();

    bool quit_;
    int eventFd_;
    Channel* pEventFdChannel_;
    Epoll* pPoller_;
    unordered_map<int, TcpConnection*>* conns_;
    vector<Runner> vPendingFunctors_;
    TimerQueue* pTimerQueue_;
};

#endif //_EVENT_LOOP_H_
