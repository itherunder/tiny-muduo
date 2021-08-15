#include "Thread.h"

#include <unistd.h>

#include <iostream>

using namespace std;

class TestThread : public Thread {
public:
    TestThread(int count) : count_(count) {
        cout << "TestThread ..." << endl;
    }
    ~TestThread() {
        cout << "~TestThread ..." << endl;
    }

private:
    void Run() {
        while (count_--) {
            cout << "this is a test ..." << endl;
            sleep(1);
        }
    }
    int count_;
};

int main() {
    // TestThread t1(5);
    // t1.Start();
    // t1.Join();

    //这里不能用直接创建（需要new 动态创建）否则delete 会删除到不对的内存
    TestThread* t2 = new TestThread(5);
    t2->SetAutoDelete(true);
    t2->Start();
    t2->Join();

    for (;;) pause();

    return 0;
}
