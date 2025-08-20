#pragma once
#include "libraries.h"
#include "lexer.h"

struct Expr {
    virtual ~Expr() = default;
    //std::unique_ptr<Expr> expr;
};

// Terminal expressions, Leaf nodes (e.g., 2, 3, 4)
struct Literal : public Expr {
    std::variant<std::monostate, double, std::string, bool> value;
    explicit Literal(std::variant<std::monostate, double, std::string, bool> val)
        : value(std::move(val)) {}
};

// Non-Terminal expressions, Operations nodes (e.g. 2 + 3, 3 * 4)
struct Binary : public Expr {
    std::unique_ptr<Expr> left;
    std::string op;
    std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> left, std::string op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {} 
};

class Parser {
    private:
        std::unique_ptr<Expr> ast;
        std::vector<Token> tokens;
        Token current_token;
        std::vector<Token::Type> operators = {Token::Type::PLUS, Token::Type::MINUS, Token::Type::SLASH, Token::Type::STAR};

        size_t pos = 0;
        bool err = false;

    public:
        Parser(const std::vector<Token>& tokens);
        ~Parser();

        std::unique_ptr<Expr> parse();
        bool match(Token::Type type);
        bool check_any(int offset, std::initializer_list<Token::Type> types);
        std::unique_ptr<Expr> op();
        std::unique_ptr<Expr> term();
        std::unique_ptr<Expr> factor();
        void print_ast(const Expr* expr);
        
        Token consume();
        Token peek(int offset = 0);
        Token expected(Token::Type type);
        Token expect_any(int offset, std::initializer_list<Token::Type> types);

        bool error_check();
};