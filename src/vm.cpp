#include "vm.h"

VM::VM(const std::vector<uint8_t>& bytecode, const std::vector<std::variant<double, std::string, bool>>& constant_pool) 
    : bytecode(bytecode), constant_pool(constant_pool) {}

VM::~VM() {}

void print_value(const std::variant<double, std::string, bool>& v) {
    std::visit([](auto&& val){
        std::cout << val;
    }, v);
}

void print_stack(const std::vector<std::variant<double, std::string, bool>>& stack) {
    for (const auto& v : stack) {
        print_value(v);
        std::cout << " ";
    }
    std::cout << "\n";
}

void VM::execute() {
    std::vector<std::variant<double, std::string, bool>> stack;

    for (size_t i = 0; i < bytecode.size(); i++)
    {
        switch (bytecode[i])
        {
            case CON: {
                uint8_t index = bytecode[++i];
                stack.push_back(constant_pool[index]);
                break;
            }
            case ADD: {
                auto b = stack.back(); stack.pop_back();
                auto a = stack.back(); stack.pop_back();
                stack.push_back(std::get<double>(a) + std::get<double>(b));
                break;
            }
            case SUB: {
                auto b = stack.back(); stack.pop_back();
                auto a = stack.back(); stack.pop_back();
                stack.push_back(std::get<double>(a) - std::get<double>(b));
                break;
            }
            case MUL: {
                auto b = stack.back(); stack.pop_back();
                auto a = stack.back(); stack.pop_back();
                stack.push_back(std::get<double>(a) * std::get<double>(b));
                break;
            }
            case DIV: {
                auto b = stack.back(); stack.pop_back();
                auto a = stack.back(); stack.pop_back();
                stack.push_back(std::get<double>(a) / std::get<double>(b));
                break;
            }
            case RETURN: {
                print_value(stack.back());
                std::cout << "\n";
                break;
            }
            default: break;
        }
    }   
}

