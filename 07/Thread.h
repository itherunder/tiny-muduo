#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>

#include <functional>

using namespace std;

/*
* 这个是基于对象编程，回调成员函数（并不使用继承、多态
* 通过注册全局函数，对象再通过函数指针进行回调
* 面向对象风格：
*   用一个EchoServer继承TcpServer（抽象类），实现三个接口 OnConnection,OnMessage,OnClose
* 基于对象风格：（效率更高）
*   用一个EchoServer包含一个TcpServer（具体类）对象，在构造函数中用bind来注册三个成员函数 OnConnection,OnMessage,OnClose
*/
class Thread {
public:
    typedef function<void()> ThreadFunc;
    //阻止隐式的转换构造
    explicit Thread(const ThreadFunc& func);
    ~Thread();

    void Start();
    void Join();

    void SetAutoDelete(bool autoDelete);
    static void* ThreadRoutine(void* argv);

private:
    void Run();
    pthread_t threadId_;
    ThreadFunc func_;
    bool autoDelete_;
};

#endif //_THREAD_H_