#include "Thread.h"

#include <iostream>

using namespace std;

//初始化的顺序一般需要按照类中定义的顺序
Thread::Thread(const ThreadFunc& func) : func_(func), autoDelete_(false) {
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

void Thread::Run() {
    func_();
}

void* Thread::ThreadRoutine(void* argv) {
    Thread* thread = (Thread*)argv;
    thread->Run();
    if (thread->autoDelete_)
        delete thread;
    return nullptr;
}

void Thread::SetAutoDelete(bool autoDelete) {
    autoDelete_ = autoDelete;
}
