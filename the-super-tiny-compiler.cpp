#include <string>
#include <vector>
#include <regex>
#include <stdexcept>

using std::string;
using std::vector;
using std::regex_match;
using std::regex;
using std::runtime_error;


class Token {
public:
    string type;
    string value;
    Token(string t, string v) :type(t), value(v) {}
};

class AST {
public:
    string type;
    vector<Token> body;
    string name;

    AST(string t) :type(t) {}
    AST(string t, vector<Token> b) :type(t), body(b) {}
};

vector<Token> tokenizer(string);
AST parser(vector<Token>);
AST transformer(AST);
string codeGenerator(AST);

vector<Token> tokenizer(string input) {
    int current = 0;
    vector<Token> tokens;
    while (current < input.size()) {
        string curr_char = string(1, input[current]);

        if (curr_char == "(") {
            tokens.push_back(Token("paren", "("));
            ++current;
            continue;
        }

        if (curr_char == ")") {
            tokens.push_back(Token("paren", ")"));
            ++current;
            continue;
        }

        /* WHITESPACE  */
        if (regex_match(curr_char, regex("\s"))) {
            current++;
            continue;
        }

        /* NUMBERS  */
        if (regex_match(curr_char, regex("[0-9]"))) {
            string value;
            while (regex_match(curr_char, regex("[0-9]"))) {
                value += curr_char;
                curr_char = input[++current];
            }

            tokens.push_back(Token("number", value));
            continue;
        }

        /* STRINGS */
        if (curr_char == "\"") {
            string value;
            curr_char = input[++current];

            while (curr_char != "\"") {
                value += curr_char;
                curr_char = input[++current];
            }

            curr_char = input[++current];
            tokens.push_back(Token("string", value));
            continue;
        }

        /* LETTERS */
        if (regex_match(curr_char, regex("[a-z]"))) {
            string value;
            curr_char = input[++current];

            while (regex_match(curr_char, regex("[a-z]"))) {
                value += curr_char;
                curr_char = input[++current];
            }

            tokens.push_back(Token("name", value));
            continue;
        }

        throw runtime_error("I dont know what this character is: " + curr_char);
    }
    return tokens;
}


Token walk(vector<Token>& tokens, int& current) {
    Token token = tokens[current];

    /* NUMBER */
    if (token.type == "number") {
        ++current;
        return Token("NumberLiteral", token.value);
    }

    /* STRING */
    if (token.type == "string") {
        ++current;
        return Token("StringLiteral", token.value);
    }

    /* TODO */
    /* PAREN */
    if (token.type == "paren" && token.value == "(") {
        token = tokens[++current];
        // AST node = AST("CallExpression",token.value,vector<AST>({}) );
        // 
        // {
        // type: 'CallExpression',
        // name : token.value,
        // params : [],
        // };
    }
    throw runtime_error(token.type);
}

AST parser(vector<Token> tokens) {
    int current = 0;
    AST ast = AST("Program");
    while (current < tokens.size()) {
        ast.body.push_back(walk(tokens, current));
    }
    return ast;
};

