#include <iostream>
#include <functional>
#include <string>
#include <vector>

struct Test {
template <typename Func>
void callFunction(Func&& func, const std::string &name) {
    // // 根据参数的数量和类型，调用传入的函数
    // if (args.size() == 2 && args[0] == "int" && args[1] == "int") {
    //     int a = std::stoi(args[2]);
    //     int b = std::stoi(args[3]);
    //     int result = func(a, b);
    //     std::cout << "Function called with int, int arguments. Result: " << result << std::endl;
    // } else if (args.size() == 3 && args[0] == "int" && args[1] == "int" && args[2] == "int") {
    //     int a = std::stoi(args[2]);
    //     int b = std::stoi(args[3]);
    //     int c = std::stoi(args[4]);
    //     // 假设func接受三个int参数并返回void
    //     func(a, b, c);
    //     std::cout << "Function called with int, int, int arguments." << std::endl;
    // } else {
    //     std::cout << "Invalid number or type of arguments." << std::endl;
    // }
    std::cout << name << std::endl;
}
};

int main() {
    // 定义一些函数
    int (*add)(int, int) = [](int a, int b) { return a + b; };
    void (*printSum)(int, int, int) = [](int a, int b, int c) {
        std::cout << "Sum: " << (a + b + c) << std::endl;
    };

    Test t;
    // 调用函数模板
    t.callFunction(add, "add"); // 应该输出: Function called with int, int arguments. Result: 7
    t.callFunction(printSum, "print"); // 应该输出: Sum: 12

    return 0;
}