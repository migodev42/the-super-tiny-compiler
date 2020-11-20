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
using std::function;
using std::to_string;


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

    vector<AstNode*> body;
    vector<AstNode*> params;
    vector<AstNode*>* arguments;
    vector<AstNode*>* context;

    AstNode(string type) :type(type) {}
    AstNode(string type, vector<AstNode*> body) :type(type), body(body) {}
    AstNode(string type, string name) :type(type), name(name) {}
    AstNode(string type, string name, vector<AstNode*> params) :type(type), name(name), params(params) {}
    AstNode(string type, AstNode* callee, vector<AstNode*>* arguments) :type(type), callee(callee), arguments(arguments) {}
};

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


AstNode* walk(vector<Token>& tokens, int& current) {
    Token token = tokens[current];

    /* NUMBER */
    if (token.type == "number") {
        ++current;
        cout << "NumberLiteral" << endl;
        return new AstNode("NumberLiteral", token.value);
    }

    /* STRING */
    if (token.type == "string") {
        ++current;
        cout << "StringLiteral" << endl;
        return new AstNode("StringLiteral", token.value);
    }

    /* PAREN */
    if (token.type == "paren" && token.value == "(") {
        token = tokens[++current];
        cout << "CallExpression start" << endl;
        AstNode* node = new AstNode("CallExpression", token.value, vector<AstNode*>({}));
        token = tokens[++current];
        while (
            (token.type != "paren") ||
            (token.type == "paren" && token.value != ")")
            ) {
            node->params.push_back(walk(tokens, current));
            token = tokens[current];
        }
        cout << "CallExpression end" << endl;
        ++current;
        return node;
    }
    throw runtime_error("Unknown Type: " + token.type);
}

AstNode* parser(vector<Token> tokens) {
    int current = 0;
    cout << endl;
    cout << "========== Walk ============" << endl;
    AstNode* ast = new AstNode("Program");
    while (current < tokens.size()) {
        ast->body.push_back(walk(tokens, current));
    }
    cout << "========== End of Walk ============" << endl;
    cout << endl;
    return ast;
};


void NumberLiteral_Visitor(AstNode* node, AstNode* parent, bool exit) {
    if (!exit) {
        (*parent->context).push_back(new AstNode("NumberLiteral", node->name));
    }
}
void StringLiteral_Visitor(AstNode* node, AstNode* parent, bool exit) {
    if (!exit) {
        (*parent->context).push_back(new AstNode("StringLiteral", node->name));
    }
}
void CallExpression_Visitor(AstNode* node, AstNode* parent, bool exit) {
    if (!exit) {
        AstNode* callee = new AstNode("Identifier", node->name);
        AstNode* callExpr = new AstNode("CallExpression", callee, new vector<AstNode*>());
        /* context 和 expression 的arguments指向同一地址，以便(*parent->context) push的时候添加调用参数到arguments*/
        node->context = callExpr->arguments;
        if (parent != nullptr) {
            if (parent->type != "CallExpression") {
                AstNode* expr = new AstNode("ExpressionStatement");
                expr->expression = callExpr;
                callExpr = expr;
            }
            (*parent->context).push_back(callExpr);
        }
    }
}
class transformer {
private:
    map<string, function<void(AstNode*, AstNode*, bool)>> visitors;
    AstNode* AST;
public:
    transformer(AstNode* AST) : AST(AST) {
        // 对应类型传递一个函数指针
        visitors["NumberLiteral"] = &NumberLiteral_Visitor;
        visitors["StringLiteral"] = &StringLiteral_Visitor;
        visitors["CallExpression"] = &CallExpression_Visitor;
        visitors["Program"] = &CallExpression_Visitor;
    }
    void traverseNode(AstNode* node, AstNode* parent) {
        cout << node->type << endl;
        auto visitor = visitors[node->type];
        visitor(node, parent, false);

        if (node->type == "Program") {
            traverseArray(&(node->body), node);
        }
        else if (node->type == "CallExpression") {
            traverseArray(&(node->params), node);
        }
        else if (node->type == "NumberLiteral" || node->type == "StringLiteral") {}
        else {
            throw runtime_error("unknown node type " + node->type);
        }
        visitor(node, parent, true);
    }
    void traverseArray(vector<AstNode*>* nodes, AstNode* parent) {
        for (int i = 0; i < (*nodes).size();++i) {
            traverseNode((*nodes)[i], parent);
        }
    }
    AstNode* traversal() {
        cout << "========== Traversal ============" << endl;
        AstNode* newast = new AstNode("Program");
        traverseNode(AST, nullptr);
        newast->body = (*AST->context);
        cout << "========== End of Traversal ============" << endl;
        return newast;
    }

};

string codeGenerator(AstNode* node) {
    string gencode;
    if (node->type == "Program") {
        for (auto item : node->body) {
            gencode += codeGenerator(item) + "\n";
        }
        return gencode;
    }
    else if (node->type == "ExpressionStatement") {
        return codeGenerator(node->expression) + ";";
    }
    else if (node->type == "CallExpression") {
        gencode += codeGenerator(node->callee) + "(";
        auto arguments = (*node->arguments);
        for (int i = 0; i < arguments.size();++i) {
            if (i == arguments.size() - 1) {
                gencode += codeGenerator(arguments[i]);
            }
            else {
                gencode += codeGenerator(arguments[i]) + ",";
            }
        }
        gencode += ")";
        return gencode;
    }
    else if (node->type == "Identifier") {
        return node->name;
    }
    else if (node->type == "NumberLiteral") {
        return node->name;
    }
    else if (node->type == "StringLiteral") {
        return "\"" + node->name + "\"";
    }
    else {
        throw runtime_error("codeGenerator: unknow astnode type" + node->type);
    }
}


void printTokens(vector<Token> tokens) {
    cout << "========= Tokens: ==========" << endl;
    for (auto to : tokens) {
        cout << to.type << " " << to.value << endl;
    }
    cout << "========== End Of Tokens ==========" << endl;
}
void checkArguments() {
    // auto p = (*(*newast->body[0]).expression);
    // auto args = (*p.arguments);
}

int main() {
    cout << "Compile (add 4 (subtract 23 8))" << endl;
    string rawcode = "(add 4 (subtract 23 8))";

    vector<Token> tokens = tokenizer(rawcode);
    printTokens(tokens);

    AstNode* ast = parser(tokens);

    transformer trans(ast);
    AstNode* newast = trans.traversal();

    string gencode = codeGenerator(newast);
    
    cout << endl;
    cout << "========= Code generated! ====" << endl;
    cout << gencode << endl;
    cout << "========= End of Code Generate ====" << endl;
    return 0;
}