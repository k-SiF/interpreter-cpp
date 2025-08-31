#pragma once
#include "compiler.h"

class VM {
    private:
        const std::vector<uint8_t>& bytecode;
        std::vector<std::variant<double, std::string, bool>>& constant_pool;
        const std::vector<std::string>& variable_pool;
        std::unordered_map<size_t, size_t>& variable_map;

    public:
        VM(const std::vector<uint8_t>& bytecode, std::vector<std::variant<double, std::string, bool>>& constant_pool,
           const std::vector<std::string>& variable_pool, std::unordered_map<size_t, size_t>& variable_map);
        ~VM();

        void execute();
};