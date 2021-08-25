#include "Timestamp.h"

Timestamp::Timestamp(int64_t microSeconds)
    : microSecondsSinceEpoch_(microSeconds) {
    // cout << "[CONS] Timestamp ..." << endl;
}

Timestamp::~Timestamp() {
    // cout << "[DECO] ~Timestamp ..." << endl;
}

bool Timestamp::Valid() {
    return microSecondsSinceEpoch_ > 0;
}

int64_t Timestamp::MicroSecondsSinceEpoch() {
    return microSecondsSinceEpoch_;
}

string Timestamp::ToString() const {
    int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
    int64_t microSeconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
    return to_string(seconds) + '.' + to_string(microSeconds);
}

Timestamp Timestamp::Now() {
    return Timestamp(Timestamp::NowMicroSeconds());
}

Timestamp Timestamp::NowAfter(double seconds) {
    return Timestamp(Timestamp::NowMicroSeconds() + seconds * kMicroSecondsPerSecond);
}

int64_t Timestamp::NowMicroSeconds() {
    timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t seconds = tv.tv_sec;
    return seconds * kMicroSecondsPerSecond + tv.tv_usec;
}

bool operator<(Timestamp l, Timestamp r) {
    return l.MicroSecondsSinceEpoch() < r.MicroSecondsSinceEpoch();
}

bool operator==(Timestamp l, Timestamp r) {
    return l.MicroSecondsSinceEpoch() == r.MicroSecondsSinceEpoch();
}
