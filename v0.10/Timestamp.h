#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#include "Define.h" 
#include "Headers.h"

class Timestamp {
public:
    Timestamp(int64_t microSeconds = 0);
    virtual ~Timestamp();
    bool Valid();
    int64_t MicroSecondsSinceEpoch();
    string ToString() const;

    static Timestamp Now();
    static Timestamp NowAfter(double seconds);
    static int64_t NowMicroSeconds();
    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t microSecondsSinceEpoch_;
};

bool operator<(const Timestamp l, const Timestamp r);
bool operator==(const Timestamp l, const Timestamp r);

#endif //_TIMESTAMP_H_
