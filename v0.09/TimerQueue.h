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
        Timer(Timestamp stamp, IRun* run, double interval)
            : stamp_(stamp)
            , id_((int64_t)&stamp)
            , run_(run)
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
            run_->Run(this);
        }
        bool IsRepeat() {
            return interval_ > 0;
        }
        void MoveToNext() {
            stamp_ = Timestamp::NowAfter(interval_);
        }
    private:
        Timestamp stamp_;
        int64_t id_;
        IRun* run_;
        double interval_;//seconds;
    };

    class AddTimerWrapper : public IRun {
    public:
        AddTimerWrapper(TimerQueue* queue)
            : queue_(queue) {
            // cout << "[CONS] AddTimerWrapper ..." << endl;
        }
        virtual ~AddTimerWrapper() {
            // cout << "[DECO] ~AddTimerWrapper ..." << endl;
        }
        void Run(void* param) override {
            queue_->DoAddTimer(param);
        }
    private:
        TimerQueue* queue_;
    };

    class CancelTimerWrapper : public IRun {
    public:
        CancelTimerWrapper(TimerQueue* queue)
            : queue_(queue) {
            // cout << "[CONS] CancelTimerWrapper ..." << endl;
        }
        virtual ~CancelTimerWrapper() {
            // cout << "[DECO] ~CancelTimerWrapper ..." << endl;
        }
        void Run(void* param) override {
            queue_->DoCancelTimer(param);
        }
    private:
        TimerQueue* queue_;
    };

    TimerQueue(EventLoop* loop);
    virtual ~TimerQueue();
    void DoAddTimer(void* param);
    void DoCancelTimer(void* param);
    int64_t AddTimer(Timestamp when, IRun* run, double interval);
    void CancelTimer(int64_t timerId);
    void HandleRead() override;
    void HandleWrite() override;

private:
    typedef pair<Timestamp, Timer*> Entry;
    typedef set<Entry> TimerSet;

    int CreateTimerFd();
    vector<TimerQueue::Entry> GetExpired(Timestamp now);
    void ReadTimerFd(int timerFd, Timestamp now);
    void Reset(const vector<TimerQueue::Entry>& expired, Timestamp now);
    void ResetTimerFd(int timerFd, Timestamp stamp);
    bool Insert(Timer* timer);
    timespec HowMuchTimeFromNow(Timestamp when);

    int timerFd_;
    TimerSet timers_;
    EventLoop* loop_;
    Channel* channel_;
    AddTimerWrapper* addTimerWrapper_;
    CancelTimerWrapper* cancelTimerWrapper_;
};

#endif //_TIMER_QUEUE_H_
