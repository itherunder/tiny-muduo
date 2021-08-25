#ifndef _TIMER_QUEUE_H_
#define _TIMER_QUEUE_H_

#include "Timestamp.h"
#include "IChannelCallBack.h"
#include "IRun.h"
#include "Channel.h"
#include "Define.h"
#include "Headers.h"

class TimerQueue : public IChannelCallBack {
public:
    class Timer {
    public:
        Timer(Timestamp stamp, IRun* pRun, double interval)
            : stamp_(stamp)
            , id_((int64_t)&stamp)
            , pRun_(pRun)
            , interval_(interval) {
            cout << "[CONS] Timer ..." << endl;
        }
        virtual ~Timer() {
            cout << "[DECO] ~Timer ..." << endl;
        }
        Timestamp GetStamp() {
            return stamp_;
        }
        int64_t GetId() {
            return id_;
        }
        void Run() {
            pRun_->Run(this);
        }
        bool IsRepeat() {
            return interval_ > 0;
        }
        void MoveToNext() {
            stamp_ = Timestamp::NowAfter(interval_);
        }
    private:
        Timestamp stamp_;
        long id_;
        IRun* pRun_;
        double interval_;//seconds;
    };

    class AddTimerWrapper : public IRun {
    public:
        AddTimerWrapper(TimerQueue* pTimerQueue)
            : pTimerQueue_(pTimerQueue) {
            // cout << "[CONS] AddTimerWrapper ..." << endl;
        }
        virtual ~AddTimerWrapper() {
            // cout << "[DECO] ~AddTimerWrapper ..." << endl;
        }
        void Run(void* param) override {
            pTimerQueue_->DoAddTimer(param);
        }
    private:
        TimerQueue* pTimerQueue_;
    };

    class CancelTimerWrapper : public IRun {
    public:
        CancelTimerWrapper(TimerQueue* pTimerQueue)
            : pTimerQueue_(pTimerQueue) {
            // cout << "[CONS] CancelTimerWrapper ..." << endl;
        }
        virtual ~CancelTimerWrapper() {
            // cout << "[DECO] ~CancelTimerWrapper ..." << endl;
        }
        void Run(void* param) override {
            pTimerQueue_->DoCancelTimer(param);
        }
    private:
        TimerQueue* pTimerQueue_;
    };

    TimerQueue(EventLoop* pLoop);
    virtual ~TimerQueue();
    void DoAddTimer(void* param);
    void DoCancelTimer(void* param);
    long AddTimer(Timestamp when, IRun* pRun, double interval);
    void CancelTimer(long pTimerId);
    void HandleRead() override;
    void HandleWrite() override;

private:
    typedef pair<Timestamp, Timer*> Entry;
    typedef set<Entry> TimerSet;

    int CreateTimerFd();
    vector<TimerQueue::Entry> GetExpired();
    void ReadTimerFd();
    void Reset(const vector<TimerQueue::Entry>& expired);
    void ResetTimerFd(Timestamp stamp);
    bool Insert(Timer* pTimer);
    timespec HowMuchTimeFromNow(Timestamp when);

    int timerFd_;
    TimerSet pTimers_;
    EventLoop* pLoop_;
    Channel* pTimerFdchannel_;
    AddTimerWrapper* pAddTimerWrapper_;
    CancelTimerWrapper* pCancelTimerWrapper_;
};

#endif //_TIMER_QUEUE_H_
