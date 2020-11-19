#include <string>
#include <vector>
#include <regex>
#include <stdexcept>
#include <map>

#include <iostream>
using std::cout;
using std::endl;

using std::string;
using std::vector;
using std::map;
using std::regex_match;
using std::regex;
using std::runtime_error;


class Token {
public:
    string type;
    string value;
    Token(string type, string value) :type(type), value(value) {}
};

class AstNode {
public:
    string type;
    string name;
    string value;

    AstNode* callee = nullptr;
    AstNode* expression = nullptr;

    vector<AstNode> body;
    vector<AstNode> params;
    vector<AstNode> arguments;
    vector<AstNode> context;

    AstNode(string type) :type(type) {}
    AstNode(string type, vector<AstNode> body) :type(type), body(body) {}
    AstNode(string type, string name) :type(type), name(name) {}
    AstNode(string type, string name, vector<AstNode> params) :type(type), name(name), params(params) {}
    AstNode(string type, AstNode* callee, vector<AstNode> arguments) :type(type), callee(callee), arguments(arguments) {}
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

/* TODO */
AstNode walk(vector<Token>& tokens, int& current) {
    Token token = tokens[current];

    /* NUMBER */
    if (token.type == "number") {
        ++current;
        return AstNode("NumberLiteral", token.value);
    }

    /* STRING */
    if (token.type == "string") {
        ++current;
        return AstNode("StringLiteral", token.value);
    }

    /* TODO */
    /* PAREN */
    if (token.type == "paren" && token.value == "(") {
        token = tokens[++current];
        AstNode node = AstNode("CallExpression", token.value, vector<AstNode>({}));
        token = tokens[++current];
        while (
            (token.type != "paren") ||
            (token.type == "paren" && token.value != ")")
            ) {
            node.params.push_back(walk(tokens, current));
            token = tokens[current];
        }
        ++current;
        return node;
    }
    throw runtime_error("Unknown Type: " + token.type);
}

AstNode parser(vector<Token> tokens) {
    int current = 0;
    AstNode ast = AstNode("Program");
    while (current < tokens.size()) {
        ast.body.push_back(walk(tokens, current));
    }
    return ast;
};


class Visitor {
    AstNode* node;
    AstNode* parent;
    bool exit;
};

class transformer {
private:
    map<string, Visitor> visitors;
public:
    void traverser(AstNode ast) {
        
    }
};



void printTokens(vector<Token> tokens) {
    cout << "========= Tokens: ==========" << endl;
    for (auto to : tokens) {
        cout << to.type << " " << to.value << endl;
    }
    cout << "========== End Of Tokens ==========" << endl;
}
void printAST(AstNode ast) {
}

int main() {
    cout << "Test run" << endl;
    string rawcode = "(add 4 (subtract 23 8))";

    vector<Token> tokens = tokenizer(rawcode);
    printTokens(tokens);

    AstNode ast = parser(tokens);
    // printAST(ast);

    AstNode newast = transformer(ast);

    return 0;
}