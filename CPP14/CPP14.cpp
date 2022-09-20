// CPP14.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <shared_mutex>
#include <string>

namespace cpp14 {
    //1.变量模板
    namespace args_template {
        template<class T>
        constexpr T pi = T(3.1415926535897932385L);
        
        template<class T>
        T circular_area(T r)
        {
            return pi<T> * r * r;
        }

        void test() {
            std::cout << circular_area(5) << "\n";
            std::cout << circular_area(5.5) << "\n";
        }
        //2.返回值类型优化
        auto func(int i) {
            std::cout << " auto func(int i)" << "\n";
            return i;
        }
        template<typename T> 
        auto func1(T t) { printf("auto func1(T t)\n"); return t; }
        //3.lambda参数auto C++11写法
        auto f1 = [](int a) { return a; };
        //4.lambda参数auto C++14写法
        auto f2 = [](auto a) { return a; };

        //5.别名模板 c++14
        template<typename T, typename U>
        struct A {
            T t;
            U u;
        };
        template<typename T>
        using B = A<T, int>;

        //6.constexpr函数在C++14中可以使用局部变量和循环
        constexpr int factorial(int n) { // C++11中不可，C++14中可以
            if (n == 10) return 1;  //C++11只能有一个return
            int ret = 0;
            for (int i = 0; i < n; ++i) ret += i;
            return ret;
        }

        //7.[[deprecated]]标记 当程序中使用到了被其修饰的代码时，编译会产生警告
        //用户提示开发者该标记修饰的内容将来可能会被丢弃，尽量不要使用。
        struct [[deprecated]] useless { int a; };
        //8.二进制字面量与整形字面量分隔符
        int bina = 0b0001'0011'1010;
        double test_bina = 3.14'1234'1234'1234;

        //9.C++11中有std::make_shared，c++14新增std::make_unique，
        std::unique_ptr<useless> ptr = std::make_unique<useless>();

        //10.std::shared_timed_mutex与std::shared_lock实现读写锁
        struct ThreadSafe {
            mutable std::shared_timed_mutex mutex_;
            int value_;

            ThreadSafe() {
                value_ = 0;
            }

            int get() const {
                std::shared_lock<std::shared_timed_mutex> loc(mutex_);
                return value_;
            }

            void increase() {
                std::unique_lock<std::shared_timed_mutex> lock(mutex_);
                value_ += 1;
            }
        };
        //11.std::quoted
        void test_quoted() {
            std::string str = "hello world";
            std::cout << str << std::endl;
            std::cout << std::quoted(str) << std::endl;
        }
    }
    

}






int main()
{
    cpp14::args_template::test();
    cpp14::args_template::func(5);
    std::cout << cpp14::args_template::func1(5.5) << "\n";

    cpp14::args_template::useless a;

    std::cout << cpp14::args_template::bina << "\n";
    std::cout << cpp14::args_template::test_bina << "\n";
}

