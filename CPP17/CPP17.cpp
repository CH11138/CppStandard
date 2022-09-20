#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <string>
#include <filesystem>

namespace cpp17 {
    //1.构造函数模板推导
    void test_1() {
        std::pair<int, double> p1(1, 2.2); // before c++17
        std::pair p(1, 2.2); // c++17 自动推导
        std::vector v = { 1, 2, 3 }; // c++17
    }

    //2.结构化绑定
    std::tuple<int, double> func() {
        return std::tuple(1, 2.2);
    }

    void f() {
        auto [i, d] = func(); //是C++11的tie吗？更高级
        std::cout << i << std::endl;
        std::cout << d << std::endl;

        std::map<int, std::string> m = {
          {0, "a"},
          {1, "b"},
        };
        for (const auto& [i, s] : m) {
            std::cout << i << " " << s << std::endl;
        }

        //3.if-switch语句初始化 c++17以后
        auto GetValue = [](auto a) {return a; };
        if (int a = GetValue(5); a < 101) {
            std::cout << a;
        }

        std::string str1 = "Hi World";
        if (auto [pos, size] = std::pair(str1.find("Hi"), str1.size()); pos != std::string::npos) {
            std::cout << pos << " Hello, size is " << size;
        }
        //4.内联变量
        //// header file
        //struct A {
        //    static const int value;
        //};
        //inline int const A::value = 10;

    }
    //5.折叠表达式
    template <typename ... Ts>
    auto sum(Ts ... ts) {
        return (ts + ...);
    }
    void test_5() {
        int a{ sum(1, 2, 3, 4, 5) }; // 15
        std::cout << a << std::endl;
        std::string a1{ "hello " };
        std::string b{ "world" };
        std::cout << sum(a1, b) << std::endl; // hello world

        //6.C++17引入了constexpr lambda表达式，可以用于在编译期进行计算。
        constexpr auto lamb = [](int n) { return n * n; };
        static_assert(lamb(3) == 9, "constexpr lambda");
    }
    //7.namespace嵌套
    namespace A::B::C {
        void func() { printf("void func()\n"); }
    }

    //8.__has_include预处理表达式,可以判断是否有某个头文件

    //9.在lambda表达式用*this捕获对象副本

    //10.新增Attribute c++17新增3个

    // [[carries_dependency]] 让编译期跳过不必要的内存栅栏指令
    // [[noreturn]] 函数不会返回
    // [[deprecated(("use new func instead"))]] 函数将弃用的警告
    // c++17新增3个
    // [[fallthrough]]，用在switch中提示可以直接落下去，不需要break，让编译期忽略警告
    // [[nodiscard]] ：表示修饰的内容不能被忽略，可用于修饰函数，标明返回值一定要被处理
    // [[maybe_unused]] ：提示编译器修饰的内容可能暂时没有使用，避免产生警告

    //11.文件系统，C++17正式将file_system纳入标准中
    void test_fileSystem() {
        namespace fs = std::filesystem;
        std::string str = "D:\BaiduNetdiskDownload\C++\C++11-14-17\cpp\CppStandard\CPP17\a";
        fs::create_directory(str);
        //fs::copy_file(src, dst, fs::copy_options::skip_existing);
        //fs::exists(filename);
        //fs::current_path(err_code);

    }




}





int main()
{
    cpp17::f();
    cpp17::test_5();
    cpp17::A::B::C::func();
    cpp17::test_fileSystem();
}
