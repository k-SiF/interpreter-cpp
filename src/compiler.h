#pragma once
#include "libraries.h"
#include "parser.h"

enum OP_CODE : lib::Byte {
    CON, ADD, SUB, DIV, MUL,
    GRT, LSS, GRTE, LSSE, EQEQ, BEQ,
    BNG, NEG,
    PRINT, VAR,
    RETURN
};

class Compiler {
    private:
        std::vector<uint8_t> bytecode;
        const std::vector<std::unique_ptr<Expr>>& ast;
        std::vector<std::variant<double, std::string, bool>> constant_pool;
        std::vector<std::string> variable_pool;
        std::unordered_map<size_t, size_t> variable_map;
        size_t constant_index;

    public:
        Compiler(const std::vector<std::unique_ptr<Expr>>& ast);
        ~Compiler();

        std::vector<uint8_t> compile();
        size_t add_constant(const std::variant<std::monostate, double, std::string, bool>&  c);
        size_t add_variable(std::string& c);
        std::vector<std::variant<double, std::string, bool>> get_constant_pool();
        std::vector<std::string> get_variable_pool();
        std::unordered_map<size_t, size_t> get_variable_map();
        
        void handler(Expr *_ast);
        void literal_handler(Literal *expr);
        void binary_handler(Binary *expr);
        void unary_handler(Unary *expr);
        void function_handler(Function *expr);
        void variable_handler(Variable *expr);

        void print_bytecode();
};