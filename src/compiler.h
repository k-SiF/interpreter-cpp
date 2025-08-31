#pragma once
#include "libraries.h"
#include "parser.h"

enum OP_CODE : lib::Byte {
    CON, ADD, SUB, DIV, MUL,
    GRT, LSS, GRTE, LSSE, EQEQ, BEQ,
    BNG, NEG,
    PRINT,
    RETURN
};

class Compiler {
    private:
        std::vector<uint8_t> bytecode;
        const std::vector<std::unique_ptr<Expr>>& ast;
        std::vector<std::variant<double, std::string, bool>> constant_pool;
        int index = 0;

    public:
        Compiler(const std::vector<std::unique_ptr<Expr>>& ast);
        ~Compiler();

        std::vector<uint8_t> compile();
        size_t add_constant(const std::variant<std::monostate, double, std::string, bool>&  c);
        std::vector<std::variant<double, std::string, bool>> get_constant_pool();
        
        void handler(Expr *_ast);
        void literal_handler(Literal *expr);
        void binary_handler(Binary *expr);
        void unary_handler(Unary *expr);
        void function_handler(Function *expr);

        void print_bytecode();
};