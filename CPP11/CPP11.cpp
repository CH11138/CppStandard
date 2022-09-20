#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <numeric>  //accumulate
#include <thread>

#include <windows.h>

//右值引用  可移动对象在<需要拷贝且被拷贝者之后不再被需要>的场景，建议使用std::move触发移动语义，提升性能。
namespace cpp11 {
    namespace rvalue {
        void f(int& x) { std::cout << "void f(int& x)\n"; }
        void f(int&& x) { std::cout << "void f(int&& x)\n"; }
        void rvalue_reference() {
            int x = 0; // `x` is an lvalue of type `int`
            int& xl = x; // `xl` is an lvalue of type `int&`
            //int&& xr = x; // compiler error -- `x` is an lvalue
            int&& xr2 = 0; // `xr2` is an lvalue of type `int&&` -- binds to the rvalue temporary, `0`,xr2是左值

            int&& xr3 = std::move(x);

            f(x);  // calls f(int&)
            f(xl); // calls f(int&)
            f(3);  // calls f(int&&)
            f(std::move(x)); // calls f(int&&)

            f(xr2);           // calls f(int&)
            f(std::move(xr2)); // calls f(int&& x)  std::move移动不了什么，唯一的作用就是把左值强制转化为右值；

            //int&& right_ref_a = 5;
            //right_ref_a = 6;
            // 上面的代码等价于
            int tmp = 5;
            int&& right_ref_a = std::move(tmp);
            right_ref_a = 50;
            std::cout << "tmp: " << tmp << std::endl; // tmp=??? 5还是50，答案是：50


            std::string str1 = "aacasxs";
            std::vector<std::string> vec;
            vec.push_back(std::move(str1)); // 调用移动语义的push_back方法，避免拷贝，str1会失去原有值，变成空字符串
            std::cout << "str1: " << str1 << std::endl;
        }
    }

    //完美转发
    namespace forwarding_references {

    }

    //可变参数模板
    namespace Variadic_templates {

        template <typename... T>
        struct arity {
            constexpr static int value = sizeof...(T);
        };
        //Static assertions
        static_assert(arity<>::value == 0,"arity<>::value != 0");
        static_assert(arity<char, short, int>::value == 3, "arity<char, short, int>::value != 3");

        template <typename First, typename... Args>
        auto add(const First first, const Args... args) -> decltype(first) {
            const auto values = { first, args... };
            return std::accumulate(values.begin(), values.end(), First{ 0 });
        }
        void test_Variadic_templates() {
            std::cout << add(1, 2, 3, 4, 5) << "\n"; // 15
            std::cout << add(1, 2, 3) << "\n";       // 6               
            std::cout << add(1.5, 2.0, 3.7) << "\n"; // 7.2
        }
    }

    namespace Lambda_expressions {

        void test_Lambda_expression(){

            int x = 1;
            auto getX = [=] { return x; };  //捕获局部变量
            std::cout << getX() << "\n"; // == 1

            auto addX = [=](int y) { return x + y; };
            std::cout << addX(1) << "\n"; // == 2

            auto getXRef = [&]() -> int& { return x; };
            std::cout << getXRef() << "\n"; // int& to `x`

            auto f1 = [&x] { x = 2; }; // OK: x is a reference and modifies the original

            //auto f2 = [x] { x = 2; }; // ERROR: the lambda can only perform const-operations on the captured value
            //mutable允许修改值捕获
            auto f3 = [x]() mutable { x = 2; }; // OK: the lambda can perform any operations on the captured value
        }
        //使用using 代替typedef
        template <typename T>
        using Vec = std::vector<T>;
        void test_Type_aliases() {
            Vec<int> v; // std::vector<int>
            using String = std::string;
            String s{ "foo" };
            std::cout << s << "\n";
        }
        // `noreturn` attribute indicates `f` doesn't return.
        [[ noreturn ]] void f() {
            throw "error";
        }
    }
    //凡是表达“只读”语义的场景都使用 const，表达“常量”语义的场景都使用 constexpr。
    namespace test_constexpr {
        constexpr int square(int x) {
            return x * x;
        }
        int square2(int x) {
            return x * x;
        }
        int a = square(2);  // mov DWORD PTR [rbp-4], 4
        int b = square2(2); // mov edi, 2
                            // call square2(int)
                            // mov DWORD PTR [rbp-8], eax
    }
    namespace test_constructors {
        struct A {
            std::string s;
            A() : s{ "test" } { std::cout <<"A()\n"; }
            A(const A& o) : s{ o.s } { std::cout << "A(const A& o)\n"; }
            A(A&& o) : s{ std::move(o.s) } { 
                std::cout << &o << "\n";
                std::cout << "A(A&& o)\n"; }
            A& operator=(A&& o) {
                s = std::move(o.s);
                std::cout << "A& operator=(A&& o)\n";
                return *this;
            }
            //转换函数
            operator bool() const { return true; }
        };
        A f(A a) {
            printf("%p,\n", &a);
            return a;
        }
        void testA() {
            A a1 = f(A{}); // move-constructed from rvalue temporary
            printf("%p,\n", &a1);
            A a2 = std::move(a1); // move-constructed using std::move
            A a3 = A{};
            a2 = std::move(a3); // move-assignment using std::move
            a1 = f(A{}); // move-assignment from rvalue temporary
        }
        //转换构造函数
        struct B {
            B(int) {}
            B(int, int) {}
            B(int, int, int) {}
            B(std::initializer_list<int>) {}
            //转换函数
            explicit operator bool() const { return true; }
        };
        void testB() {
            B a{ 0, 0 }; // calls A::A(int, int)
            B b(0, 0); // calls A::A(int, int)
            B c = { 0, 0 }; // calls A::A(int, int)
            B d{ 0, 0, 0 }; // calls A::A(int, int, int)

            B a11(1.1); // OK
            //大括号列表语法不允许缩小范围：
            //B b{ 1.1 }; // Error narrowing conversion from double to int

            B a1{ 0, 0 }; // calls A::A(std::initializer_list<int>)
            B b1(0, 0); // calls A::A(int, int)
            B c1 = { 0, 0 }; // calls A::A(std::initializer_list<int>)
            B d1{ 0, 0, 0 }; // calls A::A(std::initializer_list<int>)

        }
        void func1() noexcept {};        // does not throw
        void func2() noexcept(true) {};  // does not throw
        void func3() throw() {};         // does not throw

        void func4() noexcept(false) {  // may throw
            std::unique_ptr<int> p1{ new int{0} };  // in practice, use std::make_unique
            //std::unique_ptr<int> p2 = p1; // error -- cannot copy unique pointers
            //move `p1` into `p3`，now unsafe to dereference object held by `p1`  
            std::unique_ptr<int> p3 = std::move(p1); 
        };
    }

    namespace test_thread {
        void foo(bool clause) { /* do something... */ }

        std::vector<std::thread> threadsVector;
        void test(){

            threadsVector.emplace_back([]() {
                // Lambda function that will be invoked    
                });
            threadsVector.emplace_back(foo, true);  // thread will run foo(true)
            for (auto& thread : threadsVector) {
                thread.join(); // Wait for threads to finish
            }
        }

        void test_smart_pointers() noexcept{

        }

        void test_chrono() {
            std::chrono::time_point<std::chrono::steady_clock> start, end;
            start = std::chrono::steady_clock::now();
            // Some computations...
            end = std::chrono::steady_clock::now();
            Sleep(100);
            std::chrono::duration<double> elapsed_seconds = end - start;
            double t = elapsed_seconds.count(); // t number of seconds, represented as a `double`
            std::cout << " t: " << t << std::endl;
        }

        void test_tuples() {
            auto playerProfile = std::make_tuple(51, "Frans Nielsen", "NYI");
            std::get<0>(playerProfile); // 51
            std::get<1>(playerProfile); // "Frans Nielsen"
            std::get<2>(playerProfile); // "NYI"
        }
    }


}

int main()
{
    cpp11::rvalue::rvalue_reference();
    cpp11::Variadic_templates::test_Variadic_templates();
    cpp11::Lambda_expressions::test_Lambda_expression();
    cpp11::Lambda_expressions::test_Type_aliases();
    cpp11::test_constructors::testA();
    cpp11::test_thread::test_chrono();
    cpp11::test_thread::func1(5.5);


}

