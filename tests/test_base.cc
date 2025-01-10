#include <iostream>
#include <string_view>
#include <source_location>
 
void log(std::string_view message,
         const std::source_location& location = std::source_location::current())
{
    std::cout << "info:"
              << location.function_name() << "\n";
}

int main()
{
  log("Hello world!");
}
// info:main.cpp:15 Hello world!
