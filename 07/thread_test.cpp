#include "Thread.h"

#include <unistd.h>

#include <iostream>

using namespace std;

void func(int count) {
    while (count--) {
        cout << "this is a test 1 ..." << endl;
        sleep(1);
    }
}

struct sct {
    void func(int count) {
        while (count--) {
            cout << "this is a test 2 ..." << endl;
            sleep(1);
        }
    }
};

int main() {
    //bind用来转换（适配器
    Thread* t1 = new Thread(bind(&func, 5));
    t1->SetAutoDelete(true);
    t1->Start();
    t1->Join();

    sct st;
    Thread* t2 = new Thread(bind(&sct::func, &st, 5));
    t2->SetAutoDelete(true);
    t2->Start();
    t2->Join();

    // for (;;) pause();

    return 0;
}