#include "vm.h"

VM::VM(const std::vector<uint8_t>& bytecode, std::vector<std::variant<double, std::string, bool>>& constant_pool,
       const std::vector<std::string>& variable_pool, std::unordered_map<size_t, size_t>& variable_map) 
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
    bool store_var = false;
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
                constant_pool.push_back(std::get<double>(a) + std::get<double>(b));
                if (bytecode[i+1] == VAR) {
                    store_var = true;
                    stack.push_back(static_cast<double>(constant_pool.size() - 1));
                } else {
                    stack.push_back(std::get<double>(constant_pool.back()));
                }
                break;
            }
            case SUB: {
                auto b = stack.back(); stack.pop_back();
                auto a = stack.back(); stack.pop_back();
                constant_pool.push_back(std::get<double>(a) - std::get<double>(b));
                if (bytecode[i+1] == VAR) {
                    store_var = true;
                    stack.push_back(static_cast<double>(constant_pool.size() - 1));
                } else {
                    stack.push_back(std::get<double>(constant_pool.back()));
                }
                break;
            }
            case MUL: {
                auto b = stack.back(); stack.pop_back();
                auto a = stack.back(); stack.pop_back();
                constant_pool.push_back(std::get<double>(a) * std::get<double>(b));
                if (bytecode[i+1] == VAR) {
                    store_var = true;
                    stack.push_back(static_cast<double>(constant_pool.size() - 1));
                } else {
                    stack.push_back(std::get<double>(constant_pool.back()));
                }
                break;
            }
            case DIV: {
                auto b = stack.back(); stack.pop_back();
                auto a = stack.back(); stack.pop_back();
                constant_pool.push_back(std::get<double>(a) / std::get<double>(b));
                if (bytecode[i+1] == VAR) {
                    store_var = true;
                    stack.push_back(static_cast<double>(constant_pool.size() - 1));
                } else {
                    stack.push_back(std::get<double>(constant_pool.back()));
                }
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
                if (store_var) {
                    auto index =  stack.back(); stack.pop_back();
                    variable_map[bytecode[i+1]] = static_cast<size_t>(std::get<double>(index));
                    store_var = false;
                }
                uint8_t var_index = bytecode[++i];
                uint8_t con_index = variable_map.at(static_cast<size_t>(var_index));
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

