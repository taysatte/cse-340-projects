/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include "parser.h"
#include <cstdlib>
#include <iostream>

using namespace std;

void Parser::syntax_error() {
    cout << "SYNTAX ERROR !!!!!&%!!\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type) {
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

void Parser::parse_input() {
    parse_program();
    expect(END_OF_FILE);
}

void Parser::parse_program() {
    parse_tasks();
    parse_poly();
    parse_execute();
    parse_inputs();
}

void Parser::parse_tasks() { cout << "parse_tasks" << endl; }

void Parser::parse_poly() { cout << "parse_poly" << endl; }

void Parser::parse_execute() { cout << "parse_execute" << endl; }

void Parser::parse_inputs() { cout << "parse_inputs" << endl; }

int main() {
    // note: the parser class has a lexer object instantiated in it. You should
    // not be declaring a separate lexer object. You can access the lexer object
    // in the parser functions as shown in the example method
    // Parser::ConsumeAllInput If you declare another lexer object, lexical
    // analysis will not work correctly
    Parser parser;

    cout << "Parser created" << endl;

    parser.parse_input();
}
