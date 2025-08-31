#include "compiler.h"

// Takes the AST created by the parser and turns it into bytecode for the VM to process.

Compiler::Compiler(const std::vector<std::unique_ptr<Expr>>& ast) : ast(ast) {}

Compiler::~Compiler() {}

std::vector<uint8_t> Compiler::compile() {
    for (size_t i = 0; i < ast.size(); i++)
    {
        auto _ast = ast[i].get();
        handler(_ast);
        bytecode.push_back(RETURN);
    }
    
    return bytecode;
}

void Compiler::handler(Expr *_ast)
{
    if (auto expr = dynamic_cast<Literal*>(_ast)) {
        literal_handler(expr);
    } else if (auto expr = dynamic_cast<Binary*>(_ast)) {
        binary_handler(expr);
    } else if (auto expr = dynamic_cast<Unary*>(_ast)) {
        unary_handler(expr);
    } else if (auto expr = dynamic_cast<Function*>(_ast)) {
        function_handler(expr);
    } else if (auto expr = dynamic_cast<Variable*>(_ast)) {
        variable_handler(expr);
    }
}

void Compiler::literal_handler(Literal *expr)
{
    if (std::holds_alternative<std::string>(expr->value)) {
        for (size_t i = 0; i < variable_pool.size(); i++)
        {
            if (variable_pool[i] == std::get<std::string>(expr->value)) {
                bytecode.push_back(VAR);
                bytecode.push_back(static_cast<uint8_t>(i));
                return;
            }
        }
        
    }
    constant_index = add_constant(expr->value);

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
        case '>': {
            if (expr->op.size() > 1 && expr->op[1] == '=') bytecode.push_back(GRTE);
            else bytecode.push_back(GRT); 
            break;
        }
        case '<': {
            if (expr->op.size() > 1 && expr->op[1] == '=') bytecode.push_back(LSSE);
            else bytecode.push_back(LSS); 
            break;
        }
        case '!': {
            if (expr->op.size() > 1 && expr->op[1] == '=') bytecode.push_back(BEQ);
            break;
        }
        default: throw std::runtime_error("Operator mismatch"); break;
    }
}

void Compiler::unary_handler(Unary *expr) {
}

void Compiler::function_handler(Function *expr)
{
    if (expr->type == Token::Type::PRINT) {
        handler(expr->expr.get());
        bytecode.push_back(PRINT);
    }
}

void Compiler::variable_handler(Variable *expr)
{
    // Right side
    if (auto _expr = dynamic_cast<Literal*>(expr->right.get())) {
        literal_handler(_expr);
    } else if (auto _expr = dynamic_cast<Binary*>(expr->right.get())) {
        binary_handler(_expr);
    } else if (auto _expr = dynamic_cast<Unary*>(expr->right.get())) {
        unary_handler(_expr);
    }   

    // Left side
    size_t var_index = add_variable(expr->left.lexeme);

    // Declaration
    if (expr->op == "=") {
        variable_map[var_index] = constant_index;
        bytecode.push_back(VAR);
        bytecode.push_back(static_cast<uint8_t>(var_index));
    }
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

size_t Compiler::add_variable(std::string &c)
{
    for (size_t i = 0; i < variable_pool.size(); i++)
    {
        if (variable_pool[i] == c) return i;
    }
    
    variable_pool.push_back(c);
    return variable_pool.size() - 1;
}

std::vector<std::variant<double, std::string, bool>> Compiler::get_constant_pool()
{
    return constant_pool;
}

std::vector<std::string> Compiler::get_variable_pool()
{
    return variable_pool;
}

std::unordered_map<size_t, size_t> Compiler::get_variable_map()
{
    return variable_map;
}

void Compiler::print_bytecode() {
    std::cout << "Bytecode: ";
    for (uint8_t b : bytecode) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(b) << " ";
    }
    std::cout << std::dec << std::endl; // reset to decimal
}