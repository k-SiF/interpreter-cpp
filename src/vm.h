#pragma once
#include "compiler.h"

class VM {
    private:
        const std::vector<uint8_t>& bytecode;
        const std::vector<std::variant<double, std::string, bool>>& constant_pool;

    public:
        VM(const std::vector<uint8_t>& bytecode, const std::vector<std::variant<double, std::string, bool>>& constant_pool);
        ~VM();

        void execute();
};