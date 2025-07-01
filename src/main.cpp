#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <format>
#include <vector>

const int EXIT_LEXICAL_ERROR = 65;

struct Token {
    enum class Type {
        LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, STAR,
        DOT, COMMA, PLUS, MINUS, SEMICOLON, RETURN, EQUAL, EQUAL_EQUAL,
        BANG, BANG_EQUAL, LESS, LESS_EQUAL, GREATER, GREATER_EQUAL,
        SLASH,
        EOF_TOKEN
    };
    Type type;
    std::string lexeme;
    int line;
};

struct Error {
    int line;
    std::string message;
};

std::string read_file_contents(const std::string& filename);
std::vector<Token> lexer(const std::string& content, bool& err);
std::string type_to_string(Token::Type t);
const Token* prev_token(const std::vector<Token>& tokens);
void replace_token(std::vector<Token>& tokens, const Token& token);

int main(int argc, char *argv[]) {
    // Disable output buffering
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    
    if (argc < 3) {
        std::cerr << "Usage: ./your_program tokenize <filename>" << std::endl;
        return EXIT_FAILURE;
    }
    
    const std::string command = argv[1];
    
    if (command == "tokenize") {
        bool err = false;
        std::string file_contents = read_file_contents(argv[2]);
        
        if (!file_contents.empty()) {
            std::vector<Token> tokens = lexer(file_contents, err);

            for (const Token& token : tokens) {
                std::cout << std::format("{} {} null", type_to_string(token.type), token.lexeme) << std::endl;
            }

            if (err) return EXIT_LEXICAL_ERROR;
        } else {
            std::cout << "EOF  null" << std::endl;
        }
        
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

std::vector<Token> lexer(const std::string& source, bool& err) {
    std::vector<Token> tokens;
    int line = 1;
    bool connected = true;
    bool skip_line = false;
    for (const char c : source) {
        if(skip_line && c != '\n') continue;
        switch(c) {
            case '(': tokens.push_back(Token{Token::Type::LEFT_PAREN, "(", line}); break;
            case ')': tokens.push_back(Token{Token::Type::RIGHT_PAREN, ")", line}); break;
            case '{': tokens.push_back(Token{Token::Type::LEFT_BRACE, "{", line}); break;
            case '}': tokens.push_back(Token{Token::Type::RIGHT_BRACE, "}", line}); break;
            case '*': tokens.push_back(Token{Token::Type::STAR, "*", line}); break;
            case '.': tokens.push_back(Token{Token::Type::DOT, ".", line}); break;
            case ',': tokens.push_back(Token{Token::Type::COMMA, ",", line}); break;
            case '+': tokens.push_back(Token{Token::Type::PLUS, "+", line}); break;
            case '-': tokens.push_back(Token{Token::Type::MINUS, "-", line}); break;
            case ';': tokens.push_back(Token{Token::Type::SEMICOLON, ";", line}); break;
            case '=': { 
                auto prev_tok = prev_token(tokens);
                if (connected && prev_tok != nullptr && prev_tok->line == line) {
                    if (prev_tok->type == Token::Type::EQUAL) { replace_token(tokens, Token{Token::Type::EQUAL_EQUAL, "==", line}); break; }
                    if (prev_tok->type == Token::Type::BANG) { replace_token(tokens, Token{Token::Type::BANG_EQUAL, "!=", line}); break; }
                    if (prev_tok->type == Token::Type::LESS) { replace_token(tokens, Token{Token::Type::LESS_EQUAL, "<=", line}); break; }
                    if (prev_tok->type == Token::Type::GREATER) { replace_token(tokens, Token{Token::Type::GREATER_EQUAL, ">=", line}); break; }
                } 
                tokens.push_back(Token{Token::Type::EQUAL, "=", line}); connected = true; break;
            }
            case '!': tokens.push_back(Token{Token::Type::BANG, "!", line}); break;
            case '<': tokens.push_back(Token{Token::Type::LESS, "<", line}); break;
            case '>': tokens.push_back(Token{Token::Type::GREATER, ">", line}); break;
            case '/': {
                auto prev_tok = prev_token(tokens);
                if (connected && prev_tok != nullptr && prev_tok->line == line)
                    if(prev_tok->type == Token::Type::SLASH) { 
                        tokens.pop_back();
                        skip_line = true; break; 
                    }
                tokens.push_back(Token{Token::Type::SLASH, "/", line}); connected = true; break;
            }
            case '\n': connected = skip_line = false; line++; break;
            case ' ': connected = false; break;
            case '\t': connected = false; break;
            default: std::cerr << std::format("[line {}] Error: Unexpected character: {}", line, c) << std::endl; err = true; break;
        }
    }
    tokens.push_back(Token{Token::Type::EOF_TOKEN, ""});
    return tokens;
}

void replace_token(std::vector<Token>& tokens, const Token& token) {
    if (!tokens.empty()) {
        tokens.pop_back();
        tokens.push_back(token);
    }
}

const Token* prev_token(const std::vector<Token>& tokens) {
    if (!tokens.empty()) {
        return &tokens.back();
    }
    return nullptr;
}

std::string read_file_contents(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error reading file: " << filename << std::endl;
        std::exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

std::string type_to_string(Token::Type t) {
    switch (t) {
        case Token::Type::LEFT_PAREN: return "LEFT_PAREN";
        case Token::Type::RIGHT_PAREN: return "RIGHT_PAREN";
        case Token::Type::LEFT_BRACE: return "LEFT_BRACE";
        case Token::Type::RIGHT_BRACE: return "RIGHT_BRACE";
        case Token::Type::STAR: return "STAR";
        case Token::Type::DOT: return "DOT";
        case Token::Type::COMMA: return "COMMA";
        case Token::Type::PLUS: return "PLUS";
        case Token::Type::MINUS: return "MINUS";
        case Token::Type::SEMICOLON: return "SEMICOLON";
        case Token::Type::EQUAL: return "EQUAL";
        case Token::Type::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case Token::Type::BANG: return "BANG";
        case Token::Type::BANG_EQUAL: return "BANG_EQUAL";
        case Token::Type::LESS: return "LESS";
        case Token::Type::LESS_EQUAL: return "LESS_EQUAL";
        case Token::Type::GREATER: return "GREATER";
        case Token::Type::GREATER_EQUAL: return "GREATER_EQUAL";
        case Token::Type::SLASH: return "SLASH";
        //case Token::Type::RETURN: return "RETURN";
        case Token::Type::EOF_TOKEN: return "EOF";
        default: return "UNKNOWN";
    }
}


