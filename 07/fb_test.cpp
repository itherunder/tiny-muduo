
#include <iostream>
#include <functional>

using namespace std;

class Foo {
public:
    void memberFunc(double d, int i, int j) {
        cout << d << endl;//0.5
        cout << i << endl;//100
        cout << j << endl;//10
    }
};

int main() {
    Foo foo;
    //第一个参数为函数的地址
    auto fp1 = bind(&Foo::memberFunc, &foo, 0.5, placeholders::_1, 10);
    fp1(100);
    auto fp2 = bind(&Foo::memberFunc, &foo, 0.5, placeholders::_1, placeholders::_2);
    fp2(1, 2);
    //这里第一个参数传的是函数的引用（一样的）
    auto fp3 = bind(&Foo::memberFunc, ref(foo), 0.5, placeholders::_1, placeholders::_2);
    fp3(3, 4);
    return 0;
}