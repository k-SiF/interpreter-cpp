#include "vm.h"

VM::VM(const std::vector<uint8_t>& bytecode, const std::vector<std::variant<double, std::string, bool>>& constant_pool,
       const std::vector<std::string>& variable_pool, const std::unordered_map<size_t, size_t>& variable_map) 
    : bytecode(bytecode), constant_pool(constant_pool), variable_pool(variable_pool), variable_map(variable_map) {}

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
            case GRT: {
                auto b = stack.back(); stack.pop_back();
                auto a = stack.back(); stack.pop_back();
                stack.push_back(std::get<double>(a) > std::get<double>(b));
                break;
            }
            case GRTE: {
                auto b = stack.back(); stack.pop_back();
                auto a = stack.back(); stack.pop_back();
                stack.push_back(std::get<double>(a) >= std::get<double>(b));
                break;
            }
            case LSS: {
                auto b = stack.back(); stack.pop_back();
                auto a = stack.back(); stack.pop_back();
                stack.push_back(std::get<double>(a) < std::get<double>(b));
                break;
            }
            case LSSE: {
                auto b = stack.back(); stack.pop_back();
                auto a = stack.back(); stack.pop_back();
                stack.push_back(std::get<double>(a) <= std::get<double>(b));
                break;
            }
            case PRINT: {
                auto literal = stack.back(); stack.pop_back();
                if (auto v = std::get_if<std::string>(&literal)) {
                    print_value(*v);
                } else if (auto v = std::get_if<double>(&literal)){
                    print_value(std::to_string(*v));
                } else if (auto v = std::get_if<bool>(&literal)) {
                    print_value(std::to_string(*v));
                }
                break;
            }
            case VAR: {
                uint8_t var_index = bytecode[++i];
                uint8_t con_index = variable_map.at(static_cast<size_t>(var_index));
                std::cout << con_index;
                stack.push_back(constant_pool[con_index]);
                break;
            }
            case RETURN: {
                //print_value(stack.back());
                std::cout << "\n";
                break;
            }
            default: break;
        }
    }   
}

