#include <cstring>
#include <fstream>
#include <iostream>
#include "lexer.h"

const int EXIT_LEXICAL_ERROR = 65;
const int EXIT_PARSING_ERROR = 40;

struct Error {
    int line;
    std::string message;
};

std::string read_file_contents(const std::string& filename);
int tokenize(char *argv[]);
int parser(char *argv[]);

int main(int argc, char *argv[]) {
    // Disable output buffering
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    
    if (argc < 3) {
        std::cerr << "Usage: ./your_program tokenize <filename>" << std::endl;
        return EXIT_FAILURE;
    }
    
    const std::string command = argv[1];
    
    // LEXER
    if (command == "tokenize") {
        return tokenize(argv);
    } 
    // PARSER
    // else if (command == "parse") {
    //     bool err = false;
    //     std::string file_contents = read_file_contents(argv[2]);

    //     if (!file_contents.empty()) {

    //         if (err) return EXIT_PARSING_ERROR;
    //     }
    // } else {
    //     std::cerr << "Unknown command: " << command << std::endl;
    //     return EXIT_FAILURE;
    // }
    return EXIT_SUCCESS;
}

int tokenize(char *argv[]) {
    bool err = false;
    std::string file_contents = read_file_contents(argv[2]);
    
    if (!file_contents.empty()) {
        Lexer lexer;
        std::vector<Token> tokens = lexer.lexer(file_contents, err);

        for (Token& token : tokens) {
            if (std::holds_alternative<std::monostate>(token.literal)) token.literal = std::string("null");
            //else if (std::holds_alternative<double>(token.literal)) token.literal = std::to_string(std::get<double>(token.literal));
            std::cout << std::format("{} {} {}", lexer.type_to_string(token.type), token.lexeme, std::get<std::string>(token.literal)) << std::endl;
        }

        if (err) return EXIT_LEXICAL_ERROR;
    } else {
        std::cout << "EOF  null" << std::endl;
    }

    return EXIT_SUCCESS;
}

int parser(char *argv[]) {
    bool err = false;
    
    return EXIT_SUCCESS;
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