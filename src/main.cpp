#include <sstream>
#include <cstring>
#include <fstream>
#include "libraries.h"
#include "lexer.h"
#include "parser.h"
#include "compiler.h"

const int EXIT_LEXICAL_ERROR = 65;
const int EXIT_PARSING_ERROR = 40;

struct LexerResult {
    int status;
    std::vector<Token> tokens;
};

struct ParserResult {
    int status;
    std::vector<std::unique_ptr<Expr>> ast;
};

struct CompilerResult {
    int status;
    std::vector<uint8_t> bytecode;
};

std::string read_file_contents(const std::string& filename);
void tokenizer(char *argv[], LexerResult& lexer_r, bool debug_mode = false);
void parser(char *argv[], LexerResult& lexer_r, ParserResult& parser_r, bool debug_mode = false);
void compile(char *argv[], LexerResult& lexer_r, ParserResult& parser_r, CompilerResult& compiler_r, bool debug_mode = false);

int main(int argc, char *argv[]) {
    // Disable output buffering
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    
    if (argc < 3) {
        std::cerr << "Usage: ./your_program [tokenize | parse] <filename>" << std::endl;
        return EXIT_FAILURE;
    }

    LexerResult lexer_r;
    ParserResult parser_r;
    CompilerResult compiler_r;
    
    const std::string command = argv[1];
    
    // LEXER
    if (command == "tokenize") {
        if (argc > 3 && std::string(argv[3]) == "debug") {
            std::cout << "[DEBUG MODE]" << std::endl;
            tokenizer(argv, lexer_r, true);
        }
        else tokenizer(argv, lexer_r);
    } 
    // PARSER
    else if (command == "parse") {
        if (argc > 3 && std::string(argv[3]) == "debug") { 
            std::cout << "[DEBUG MODE]" << std::endl;
            parser(argv, lexer_r, parser_r, true);
        }
        else parser(argv, lexer_r, parser_r);
    }
    // COMPILER
    else if (command == "compile") {
        if (argc > 3 && std::string(argv[3]) == "debug") { 
            std::cout << "[DEBUG MODE]" << std::endl;
            compile(argv, lexer_r, parser_r, compiler_r, true);
        }
        else compile(argv, lexer_r, parser_r, compiler_r);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void tokenizer(char *argv[], LexerResult& lexer_r, bool debug_mode) {
    std::string file_contents = read_file_contents(argv[2]);
    
    if (!file_contents.empty()) {
        Lexer lexer;
        lexer_r.tokens = lexer.lexer(file_contents);

        if (debug_mode) {
            for (Token& token : lexer_r.tokens) {
                if (std::holds_alternative<std::monostate>(token.literal)) token.literal = std::string("null");
                //else if (std::holds_alternative<double>(token.literal)) token.literal = std::to_string(std::get<double>(token.literal));
                std::cout << std::format("{} {} {}", lexer.type_to_string(token.type), token.lexeme, std::get<std::string>(token.literal)) << std::endl;
            }
        }

        if (lexer.error_check()) lexer_r.status = EXIT_LEXICAL_ERROR;

    } else {
        std::cout << "EOF  null" << std::endl;
    }
    lexer_r.status = EXIT_SUCCESS;
}

void parser(char *argv[], LexerResult& lexer_r, ParserResult& parser_r, bool debug_mode) {
    bool err = false;
    std::string file_contents = read_file_contents(argv[2]);

    if (!file_contents.empty()) {
        tokenizer(argv, lexer_r);
        Parser parser(lexer_r.tokens);
        parser_r.ast = parser.parse();
        if (debug_mode) parser.print_program(parser_r.ast);
        std::cout << std::endl;
        if (err) parser_r.status = EXIT_PARSING_ERROR;
    }
    parser_r.status = EXIT_SUCCESS;
}

void compile(char *argv[], LexerResult& lexer_r, ParserResult& parser_r, CompilerResult& compiler_r, bool debug_mode) {
    bool err = false;
    std::string file_contents = read_file_contents(argv[2]);

    if (!file_contents.empty()) {
        parser(argv, lexer_r, parser_r, true);
        Compiler compiler(parser_r.ast);
        compiler_r.bytecode = compiler.compile();
        if (debug_mode) compiler.print_bytecode();
        std::cout << std::endl;
    }
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