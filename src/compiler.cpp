#include "compiler.h"

// Takes the AST created by the parser and turns it into bytecode for the VM to process.

Compiler::Compiler(const std::vector<std::unique_ptr<Expr>>& ast) : ast(ast) {}

Compiler::~Compiler() {}

std::vector<uint8_t> Compiler::compile() {
    for (size_t i = 0; i < ast.size(); i++)
    {
        auto _ast = ast[i].get();
        if (auto expr = dynamic_cast<Literal*>(_ast)) {
            literal_handler(expr);
        } else if (auto expr = dynamic_cast<Binary*>(_ast)) {
            binary_handler(expr);
        } else if (auto expr = dynamic_cast<Unary*>(_ast)) {
            unary_handler(expr);
        }
        
        bytecode.push_back(RETURN);
    }
    
    return bytecode;
}

void Compiler::literal_handler(Literal *expr)
{
    size_t constant_index = add_constant(expr->value);

    // Emit OP_CODE CONSTANT with index
    bytecode.push_back(CON);
    bytecode.push_back(static_cast<uint8_t>(constant_index));
}

void Compiler::binary_handler(Binary *expr)
{
    // Left expression
    if (auto _expr = dynamic_cast<Literal*>(expr->left.get())) {
        literal_handler(_expr);
    } else if (auto _expr = dynamic_cast<Binary*>(expr->left.get())) {
        binary_handler(_expr);
    } else if (auto _expr = dynamic_cast<Unary*>(expr->left.get())) {
        unary_handler(_expr);
    }

    // Right expression
    if (auto _expr = dynamic_cast<Literal*>(expr->right.get())) {
        literal_handler(_expr);
    } else if (auto _expr = dynamic_cast<Binary*>(expr->right.get())) {
        binary_handler(_expr);
    } else if (auto _expr = dynamic_cast<Unary*>(expr->right.get())) {
        unary_handler(_expr);
    }

    // Operator
    char op = expr->op[0];
    switch (op)
    {
        case '+': bytecode.push_back(ADD); break;
        case '-': bytecode.push_back(SUB); break;
        case '*': bytecode.push_back(MUL); break;
        case '/': bytecode.push_back(DIV); break;
        default: throw std::runtime_error("Operator mismatch"); break;
    }
}

void Compiler::unary_handler(Unary *expr) {
}

size_t Compiler::add_constant(const std::variant<std::monostate, double, std::string, bool>& constant) {
    return std::visit([&](auto&& c) -> size_t {
        using T = std::decay_t<decltype(c)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            throw std::runtime_error("Tried to add empty literal to constant pool");
        } else {
            // Check if constant exists in constant pool
            for (size_t i = 0; i < constant_pool.size(); i++)
            {
                // Handle different variant typing. I hate this.
                if (std::holds_alternative<T>(constant_pool[i]) &&
                std::get<T>(constant_pool[i]) == c) return i;
            }
            // Not found, add it to constant_pool
            constant_pool.push_back(c);
            return constant_pool.size() - 1;
        }
    }, constant);
}

std::vector<std::variant<double, std::string, bool>> Compiler::get_constant_pool()
{
    return constant_pool;
}

void Compiler::print_bytecode() {
    std::cout << "Bytecode: ";
    for (uint8_t b : bytecode) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(b) << " ";
    }
    std::cout << std::dec << std::endl; // reset to decimal
}