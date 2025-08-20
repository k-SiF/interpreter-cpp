#include "parser.h"

// The parser constructs an Abstract Syntax Tree (AST) using the tokens from the buffer created by the lexer.
// Given the input 2 + 3 * 4, the parser should give us an tree following this structure: 
//                              (2 + (3 * 4)) 
// The tree would look something like this (not literally this is just a visualisation of it):
//                                  +
//                                 / \
//                                2   *
//                                   / \
//                                  3   4
//
// With how abstract this is it becomes quite confusing, 
// I needed a bit of time to try processing the sequence and how each step would translate into code.

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

Parser::~Parser() {}

std::unique_ptr<Expr> Parser::parse(){
    size_t buffer_size = tokens.size();   
    return op();
}

bool Parser::match(Token::Type type) {
    if (peek().type == type) {
        consume();
        return true;
    } else {
        return false;
    }
}

Token Parser::expected(Token::Type type) {
    if (peek().type == type) {
        return consume();
    } else {
        err = true;
        std::ostringstream oss;
        oss << "Unexpected token: " << peek(0).lexeme;
        throw std::runtime_error(oss.str());
    }
}

Token Parser::expect_any(int offset, std::initializer_list<Token::Type> types) {
    Token t = peek(offset);
    for (auto ty : types) if (t.type == ty) return consume();
    std::ostringstream oss;
    oss << "Expected operator, got '" << t.lexeme;
    throw std::runtime_error(oss.str());
}

bool Parser::check_any(int offset, std::initializer_list<Token::Type> types) {
    Token t = peek(offset);
    for (auto ty : types) if (t.type == ty) return true;
    return false;
}

Token Parser::consume() {
    if (pos >= tokens.size()) {
        current_token = { Token::Type::EOF_TOKEN, "" };
        return current_token;
    }
    current_token = tokens[pos++];
    return current_token;
}

Token Parser::peek(int offset) {
    size_t i = pos + static_cast<size_t>(offset);
    if (i >= tokens.size()) return { Token::Type::EOF_TOKEN, "" };
    return tokens[i];
}

std::unique_ptr<Expr> Parser::op() {
    auto left = term();

    while (peek().type == Token::Type::PLUS || peek().type == Token::Type::MINUS) {
        Token op = consume();
        auto right = term();
        left = std::make_unique<Binary>(std::move(left), op.lexeme, std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::term() {
    auto left = factor();

    while (peek().type == Token::Type::STAR || peek().type == Token::Type::SLASH) {
        Token op = consume();
        auto right = term();
        left = std::make_unique<Binary>(std::move(left), op.lexeme, std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::factor() {
    if (peek().type == Token::Type::NUMBER) {
        double value = std::stod(consume().lexeme);
        return std::make_unique<Literal>(value);
    }
    throw std::runtime_error("Expected number or '('");
}

// Utility printer for variant
static void print_value(const std::variant<std::monostate, double, std::string, bool>& v) {
    std::visit([](auto const& x) {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            std::cout << "null";
        } else if constexpr (std::is_same_v<T, bool>) {
            std::cout << (x ? "true" : "false");
        } else {
            std::cout << x; // works for double and std::string
        }
    }, v);
}

// Function to print the AST (for debugging purposes)
void Parser::print_ast(const Expr* expr) {
    if (auto lit = dynamic_cast<const Literal*>(expr)) {
        print_value(lit->value);
    } else if (auto bin = dynamic_cast<const Binary*>(expr)) {
        std::cout << "(";
        print_ast(bin->left.get());
        std::cout << " " << bin->op << " ";
        print_ast(bin->right.get());
        std::cout << ")";
    }
}

bool Parser::error_check() {
    return err;
}

// int main() {
//     // Manually building the AST for "2 + 3 * 4"
//     auto left = std::make_unique<Literal>(2);
//     auto right = std::make_unique<Binary>(
//         std::make_unique<Literal>(3),
//         "*",
//         std::make_unique<Literal>(4)
//     );
//     auto ast = std::make_unique<Binary>(std::move(left), "+", std::move(right));

//     // Printing the AST
//     print_ast(ast.get()); // Should print: (2 + (3 * 4))
//     std::cout << std::endl;

//     return 0;
// }

