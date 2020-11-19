#include <string>
#include <vector>
#include <regex>
#include <stdexcept>


#include <iostream>
using std::cout;
using std::endl;

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

class AstNode {
public:
    string type;
    vector<Token> body;
    string name;

    AstNode(string t) :type(t) {}
    AstNode(string t, vector<Token> b) :type(t), body(b) {}
};






vector<Token> tokenizer(string);
AstNode parser(vector<Token>);
AstNode transformer(AstNode);
string codeGenerator(AstNode);

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
        if (regex_match(curr_char, regex("\\s"))) {
            ++current;
            continue;
        }

        /* NUMBERS  */
        if (regex_match(curr_char, regex("[0-9]"))) {
            string value;
            while (regex_match(curr_char, regex("[0-9]"))) {
                value += curr_char;
                curr_char = string(1, input[++current]);
            }

            tokens.push_back(Token("number", value));
            continue;
        }

        /* STRINGS */
        if (curr_char == "\"") {
            string value;
            curr_char = string(1, input[++current]);

            while (curr_char != "\"") {
                value += curr_char;
                curr_char = string(1, input[++current]);
            }

            curr_char = string(1, input[++current]);
            tokens.push_back(Token("string", value));
            continue;
        }

        /* LETTERS */
        if (regex_match(curr_char, regex("[a-z]"))) {
            string value;
            while (regex_match(curr_char, regex("[a-z]"))) {
                value += curr_char;
                curr_char = string(1, input[++current]);
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
        // AstNode node = AstNode("CallExpression",token.value,vector<AstNode>({}) );
        // 
        // {
        // type: 'CallExpression',
        // name : token.value,
        // params : [],
        // };
    }
    throw runtime_error(token.type);
}

AstNode parser(vector<Token> tokens) {
    int current = 0;
    AstNode ast = AstNode("Program");
    while (current < tokens.size()) {
        ast.body.push_back(walk(tokens, current));
    }
    return ast;
};




void printTokens(vector<Token> tokens) {
    cout << "========= Tokens: ==========" << endl;
    for (auto to : tokens) {
        cout << to.type << " " << to.value << endl;
    }
    cout << "========== End Of Tokens ==========" << endl;
}

int main() {
    cout << "Test run" << endl;
    vector<Token> tokens = tokenizer("(add 2 2)");
    printTokens(tokens);
}