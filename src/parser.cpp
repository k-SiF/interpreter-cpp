#include "parser.h"

// The parser constructs an Abstract Syntax Tree (AST) using the tokens from the buffer created by the lexer.
// Given the input 2 + 3 * 4, the parser should give us a tree following this structure: 
//                              (2 + (3 * 4)) 
// The tree would look something like this:
//                                  +
//                                 / \
//                                2   *
//                                   / \
//                                  3   4
//

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

Parser::~Parser() {}

std::unique_ptr<Expr> Parser::parse(){
    size_t buffer_size = tokens.size();   
    return expression();
}

std::unique_ptr<Expr> Parser::expression() {
    auto left = equality();
    expected(Token::Type::SEMICOLON, ";");
    return left;
}

std::unique_ptr<Expr> Parser::equality() {
    auto left = comparison();
    
    while (peek().type == Token::Type::EQUAL_EQUAL || peek().type == Token::Type::BANG_EQUAL) {
        Token op = consume();
        auto right = comparison();
        left = std::make_unique<Binary>(std::move(left), op.lexeme, std::move(right));
    }

    return left;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto left = term();
    
    while (peek().type == Token::Type::GREATER || peek().type == Token::Type::GREATER_EQUAL || peek().type == Token::Type::LESS || peek().type == Token::Type::LESS_EQUAL) {
        Token op = consume();
        auto right = term();
        left = std::make_unique<Binary>(std::move(left), op.lexeme, std::move(right));
    }
    
    return left;
}

std::unique_ptr<Expr> Parser::term() {
    auto left = factor();
    
    while (peek().type == Token::Type::PLUS || peek().type == Token::Type::MINUS) {
        Token op = consume();
        auto right = factor();
        left = std::make_unique<Binary>(std::move(left), op.lexeme, std::move(right));
    }
    
    return left;
}

std::unique_ptr<Expr> Parser::factor() {
    auto left = unary();
    
    while (peek().type == Token::Type::STAR || peek().type == Token::Type::SLASH) {
        Token op = consume();
        auto right = unary();
        left = std::make_unique<Binary>(std::move(left), op.lexeme, std::move(right));
    }
    
    return left;
}

std::unique_ptr<Expr> Parser::unary() {
    if (peek().type == Token::Type::BANG || peek().type == Token::Type::MINUS) {
        Token op = consume();
        auto expr = unary();
        auto left = std::make_unique<Unary>(op.lexeme, std::move(expr));
        return left;
    }
    
    return primary();
}

std::unique_ptr<Expr> Parser::primary() {
    if (peek().type == Token::Type::NUMBER) {
        double value = std::stod(std::get<std::string>(consume().literal));
        return std::make_unique<Literal>(value);
    }
    if (peek().type == Token::Type::LEFT_PAREN) {
        consume();
        auto expr = expression();
        expected(Token::Type::RIGHT_PAREN, ")");
        return expr;
    }
    if (peek().type == Token::Type::STRING) return std::make_unique<Literal>(std::get<std::string>(consume().literal));
    if (peek().type == Token::Type::TRUE) { consume(); return std::make_unique<Literal>(true); }
    if (peek().type == Token::Type::FALSE) { consume(); return std::make_unique<Literal>(false); }
    if (peek().type == Token::Type::NIL) { consume(); return std::make_unique<Literal>(std::monostate{}); }
    
    throw std::runtime_error("Expected number | ')' | string | boolean");
}

bool Parser::match(Token::Type type) {
    if (peek().type == type) {
        consume();
        return true;
    } else {
        return false;
    }
}

Token Parser::expected(Token::Type type, const char* type_s) {
    if (peek().type == type) {
        return consume();
    } else {
        err = true;
        std::ostringstream oss;
        oss << "Expected a '" << type_s << "'.";
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

// Utility printer for variant
static void print_value(const std::variant<std::monostate, double, std::string, bool>& v) {
    std::visit([](auto const& x) {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            std::cout << "nil";
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
    } else if (auto bin = dynamic_cast<const Unary*>(expr)) {
        std::cout << "(" << bin->op << " ";
        print_ast(bin->expr.get());
        std::cout << ")";
    }
}

bool Parser::error_check() {
    return err;
}