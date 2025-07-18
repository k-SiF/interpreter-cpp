#include <iostream>
#include <sstream>
#include <string>
#include <format>
#include <vector>
#include <variant>
#include <unordered_map>
#include <memory>

using Literal = std::variant<std::monostate, std::string, double, bool>;

enum class ScanState {
    NORMAL, STRING, COMMENT, NUMBER, IDENTIFIER
};

struct Token {
    enum class Type {
        LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, STAR,
        DOT, COMMA, PLUS, MINUS, SEMICOLON, ENTER, EQUAL, EQUAL_EQUAL,
        BANG, BANG_EQUAL, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,
        SLASH, STRING, NUMBER, IDENTIFIER, AND, CLASS, ELSE, FALSE, FOR, 
        FUN, IF, NIL, OR, PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,
        EOF_TOKEN
    };
    Type type;
    std::string lexeme;
    Literal literal = std::monostate{};
    int line;

    Token(Type type, std::string lexeme, int line)
        : type(type), lexeme(std::move(lexeme)), line(line) {}

    Token(Type type, std::string lexeme, Literal literal, int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

};

class Lexer {
    private:
        std::vector<Token> tokens;
        ScanState scan_state = ScanState::NORMAL;
        int line = 1;
        bool connected = true;
        int source_size;

    public:
        Lexer();
        ~Lexer();

        std::vector<Token> lexer(const std::string& content, bool& err);
        std::string type_to_string(Token::Type t);
        Token* prev_token(std::vector<Token>& tokens);
        void replace_token(std::vector<Token>& tokens, const Token& token);
        char next_token(const std::string& tokens, const int index);
        const std::unordered_map<std::string, Token::Type>& get_keywords();
};