#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <any>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 解析 JSON 数组元素
std::any parseJsonElement(const json& element) {
    if (element.is_string()) {
        // 字符串类型
        return element.get<std::string>();
    } else if (element.is_array()) {
        // 嵌套的数组类型
        std::vector<std::any> nestedArray;
        for (const auto& nestedElement : element) {
            nestedArray.push_back(parseJsonElement(nestedElement));
        }
        return nestedArray;
    } else if (element.is_number_integer()) {
        // 整数类型
        return element.get<int>();
    } else if (element.is_number_float()) {
        // 浮点数类型
        return element.get<double>();
    } else if (element.is_object()) {
        // 对象类型
        std::map<std::string, std::any> nestedObject;
        for (auto it = element.begin(); it != element.end(); ++it) {
            nestedObject[it.key()] = parseJsonElement(it.value());
        }
        return nestedObject;
    } else if (element.is_discarded()) {
        // 丢弃的类型（例如元组）
        return std::any();
    } else {
        // 其他类型
        return nullptr;
    }
}

int main() {
    std::string jsonString = R"([0, "corr-ff32ff9f-7281-4fc5-a502-a2e94acf37d2", "echo", [1, "ss", [1, 2, "ss", 4, 2.3], 5.7], {"v": 5, "s": "aa"}])";

    // 解析 JSON 字符串
    json jsonData = json::parse(jsonString);

    // 解析数组元素
    std::vector<std::any> parsedArray;
    for (const auto& element : jsonData) {
        parsedArray.push_back(parseJsonElement(element));
    }

    // 输出解析结果
    for (const auto& parsedElement : parsedArray) {
        if (parsedElement.type() == typeid(int)) {
            std::cout << "整数值: " << std::any_cast<int>(parsedElement) << std::endl;
        } else if (parsedElement.type() == typeid(double)) {
            std::cout << "浮点数值: " << std::any_cast<double>(parsedElement) << std::endl;
        } else if (parsedElement.type() == typeid(std::string)) {
            std::cout << "字符串值: " << std::any_cast<std::string>(parsedElement) << std::endl;
        } else if (parsedElement.type() == typeid(std::vector<std::any>)) {
            std::cout << "嵌套的数组值: ";
            const std::vector<std::any>& nestedArray = std::any_cast<std::vector<std::any>>(parsedElement);
            for (const auto& nestedElement : nestedArray) {
                if (nestedElement.type() == typeid(int)) {
                    std::cout << std::any_cast<int>(nestedElement) << " ";
                } else if (nestedElement.type() == typeid(double)) {
                    std::cout << std::any_cast<double>(nestedElement) << " ";
                } else if (nestedElement.type() == typeid(std::string)) {
                    std::cout << std::any_cast<std::string>(nestedElement) << " ";
                } else if (nestedElement.type() == typeid(std::tuple<std::any, std::any, std::any>)) {
                    std::cout << "( ";
                    const auto& nestedTuple = std::any_cast<std::tuple<std::any, std::any, std::any>>(nestedElement);
                    std::cout << std::any_cast<int>(std::get<0>(nestedTuple)) << " ";
                    std::cout << std::any_cast<int>(std::get<1>(nestedTuple)) << " ";
                    std::cout << std::any_cast<int>(std::get<2>(nestedTuple)) << " ";
                    std::cout << ") ";
                }
            }
            std::cout << std::endl;
        } else if (parsedElement.type() == typeid(std::map<std::string, std::any>)) {
            std::cout << "嵌套的对象值: ";
            const std::map<std::string, std::any>& nestedObject = std::any_cast<std::map<std::string, std::any>>(parsedElement);
            for (const auto& pair : nestedObject) {
                std::cout << pair.first << ": ";
                if (pair.second.type() == typeid(int)) {
                    std::cout << std::any_cast<int>(pair.second) << " ";
                } else if (pair.second.type() == typeid(double)) {
                    std::cout << std::any_cast<double>(pair.second) << " ";
                } else if (pair.second.type() == typeid(std::string)) {
                    std::cout << std::any_cast<std::string>(pair.second) << " ";
                }
            }
            std::cout << std::endl;
        } else {
            std::cout << "未知类型" << std::endl;
        }
    }

    return 0;
}
