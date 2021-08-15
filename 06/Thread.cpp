#include "Thread.h"

#include <iostream>

using namespace std;

Thread::Thread() : autoDelete_(false) {
    cout << "Thread ..." << endl;
}

Thread::~Thread() {
    cout << "~Thread ..." << endl;    
}

void Thread::Start() {
    pthread_create(&threadId_, nullptr, ThreadRoutine, this);
}

void Thread::Join() {
    pthread_join(threadId_, nullptr);
}

void* Thread::ThreadRoutine(void* argv) {
    /*
    * argv 实际上是派生类指针，让基类指针指向
    * 基类指针指向的派生类对象，调用的是派生类的虚函数
    * 使用到了虚函数的多态，虚函数具有回调功能
    */
    Thread* thread = static_cast<Thread*>(argv);
    thread->Run();
    if (thread->autoDelete_)
        delete thread;
    return nullptr;
}

void Thread::SetAutoDelete(bool autoDelete) {
    autoDelete_ = autoDelete;
}
