#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
/*
* 这个是面向对象编程（多态，封装，继承）
* 面向对象风格：
*   用一个EchoServer继承TcpServer（抽象类），实现三个接口 OnConnection,OnMessage,OnClose
* 基于对象风格：（效率更高）
*   用一个EchoServer包含一个TcpServer（具体类）对象，在构造函数中用bind来注册三个成员函数 OnConnection,OnMessage,OnClose
*/
class Thread {
public:
    Thread();
    virtual ~Thread();

    void Start();
    void Join();
    void SetAutoDelete(bool autoDelete);

private:
    static void* ThreadRoutine(void* argv);
    virtual void Run() = 0;
    pthread_t threadId_;
    bool autoDelete_;//是否自动销毁
};


#endif//_THREAD_H_