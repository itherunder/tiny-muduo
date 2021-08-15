
#include "muduo/base/Timestamp.h"

#include <assert.h>

#include <iostream>

using namespace std;

static_assert(sizeof(muduo::Timestamp) == sizeof(int64_t));
//这里会在编译的时候就进行检查，而不是main 函数中的assert
// static_assert(sizeof(int) == sizeof(short));

int main() {
    //运行时检查
    // assert(sizeof(int) == sizeof(short));
    cout << "this is a test ..." << endl;
    return 0;
}
