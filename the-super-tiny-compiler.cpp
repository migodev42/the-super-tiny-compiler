#include <string>
#include <vector>


using std::string;
using std::vector;

class Token {
public:
    string type;
    char value;
};

class AST {
public:
};

vector<Token> tokenizer(string);
AST parser(vector<Token>);
AST transformer(AST);
string codeGenerator(AST);

vector<Token> tokenizer(string input) {
    int current = 0;
    vector<Token> tokens;
    while (current < input.size()) {
        // char curr_char = input[current];
        ++current;
    }
}



