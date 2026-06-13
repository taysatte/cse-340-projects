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
#include <algorithm>
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

bool Parser::is_declared_poly(const string &name) {
    for (const PolyDecl &decl : poly_table) {
        if (decl.header.name.lexeme == name)
            return true;
    }
    return false;
}

int Parser::poly_param_count(const string &name) {
    for (const PolyDecl &decl : poly_table) {
        if (decl.header.name.lexeme == name)
            return decl.param_count;
    }
    return -1;
}

bool Parser::is_valid_poly_param(const string &lexeme) {
    for (const string &param : current_poly_params) {
        if (param == lexeme)
            return true;
    }
    return false;
}

void Parser::record_error(int code, int line_no) {
    switch (code) {
    case 1:
        semantic_error_1.push_back(line_no);
        break;
    case 2:
        semantic_error_2.push_back(line_no);
        break;
    case 3:
        semantic_error_3.push_back(line_no);
        break;
    case 4:
        semantic_error_4.push_back(line_no);
        break;
    }
}

void Parser::report_semantic_errors() {
    sort(semantic_error_1.begin(), semantic_error_1.end());
    sort(semantic_error_2.begin(), semantic_error_2.end());
    sort(semantic_error_3.begin(), semantic_error_3.end());
    sort(semantic_error_4.begin(), semantic_error_4.end());

    if (!semantic_error_1.empty()) {
        cout << "Semantic Error Code 1:";
        for (int line : semantic_error_1)
            cout << " " << line;
        cout << "\n";
        exit(1);
    }
    if (!semantic_error_2.empty()) {
        cout << "Semantic Error Code 2:";
        for (int line : semantic_error_2)
            cout << " " << line;
        cout << "\n";
        exit(1);
    }
    if (!semantic_error_3.empty()) {
        cout << "Semantic Error Code 3:";
        for (int line : semantic_error_3)
            cout << " " << line;
        cout << "\n";
        exit(1);
    }
    if (!semantic_error_4.empty()) {
        cout << "Semantic Error Code 4:";
        for (int line : semantic_error_4)
            cout << " " << line;
        cout << "\n";
        exit(1);
    }
}

void Parser::parse_input() {
    // input -> program END_OF_FILE
    parse_program();
    expect(END_OF_FILE);
    report_semantic_errors();
}

void Parser::parse_program() {
    // program -> tasks_section poly_section execute_section inputs_section
    parse_tasks_section();
    parse_poly_section();
    parse_execute_section();
    parse_inputs_section();
}

void Parser::parse_tasks_section() {
    // tasks_section -> TASKS num_list
    expect(TASKS);
    parse_num_list();
}

void Parser::parse_num_list() {
    // num_list -> NUM
    // num_list -> NUM num_list
    expect(NUM);
    Token t = lexer.peek(1);
    if (t.token_type == NUM)
        parse_num_list();
}

void Parser::parse_poly_section() {
    // poly_section -> POLY poly_decl_list
    expect(POLY);
    parse_poly_decl_list();
}

void Parser::parse_poly_decl_list() {
    // poly_decl_list -> poly_decl
    // poly_decl_list -> poly_decl poly_decl_list
    parse_poly_decl();
    Token t = lexer.peek(1);
    if (t.token_type == ID)
        parse_poly_decl_list();
}

void Parser::parse_poly_decl() {
    // poly_decl -> poly_header EQUAL poly_body SEMICOLON
    PolyHeader header = parse_poly_header();

    if (is_declared_poly(header.name.lexeme))
        record_error(1, header.name.line_no);

    PolyDecl decl;
    decl.header = header;
    decl.param_count = header.params.size();
    poly_table.push_back(decl);

    current_poly_params = header.params;

    expect(EQUAL);
    parse_poly_body();
    expect(SEMICOLON);

    current_poly_params.clear();
}

PolyHeader Parser::parse_poly_header() {
    // poly_header -> poly_name
    // poly_header -> poly_name LPAREN id_list RPAREN
    PolyHeader header;
    header.name = parse_poly_name();
    Token t = lexer.peek(1);
    if (t.token_type == LPAREN) {
        expect(LPAREN);
        header.params = parse_id_list();
        expect(RPAREN);
    } else {
        header.params.push_back("x");
    }
    return header;
}

Token Parser::parse_poly_name() {
    // poly_name -> ID
    return expect(ID);
}

vector<string> Parser::parse_id_list() {
    // id_list -> ID
    // id_list -> ID COMMA id_list
    Token id = expect(ID);
    vector<string> ids;
    ids.push_back(id.lexeme);
    Token t = lexer.peek(1);
    if (t.token_type == COMMA) {
        expect(COMMA);
        vector<string> rest = parse_id_list();
        ids.insert(ids.end(), rest.begin(), rest.end());
    }
    return ids;
}

void Parser::parse_poly_body() {
    // poly_body -> term_list
    parse_term_list();
}

void Parser::parse_term_list() {
    // term_list -> term
    // term_list -> term add_operator term_list
    parse_term();
    Token t = lexer.peek(1);
    if (t.token_type == PLUS || t.token_type == MINUS) {
        parse_add_operator();
        parse_term_list();
    }
}

void Parser::parse_term() {
    // term -> monomial_list
    // term -> coefficient monomial_list
    // term -> coefficient
    Token t1 = lexer.peek(1);
    if (t1.token_type == ID || t1.token_type == LPAREN) {
        parse_monomial_list();
    } else if (t1.token_type == NUM) {
        Token t2 = lexer.peek(2);
        if (t2.token_type == ID || t2.token_type == LPAREN) {
            parse_coefficient();
            parse_monomial_list();
        } else if (t2.token_type == PLUS || t2.token_type == MINUS ||
                   t2.token_type == RPAREN || t2.token_type == SEMICOLON ||
                   t2.token_type == COMMA || t2.token_type == END_OF_FILE) {
            parse_coefficient();
        } else {
            syntax_error();
        }
    } else {
        syntax_error();
    }
}

void Parser::parse_monomial_list() {
    // monomial_list -> monomial
    // monomial_list -> monomial monomial_list
    parse_monomial();
    Token t = lexer.peek(1);
    if (t.token_type == ID || t.token_type == LPAREN)
        parse_monomial_list();
}

void Parser::parse_monomial() {
    // monomial -> primary
    // monomial -> primary exponent
    parse_primary();
    Token t = lexer.peek(1);
    if (t.token_type == POWER)
        parse_exponent();
}

void Parser::parse_primary() {
    // primary -> ID
    // primary -> LPAREN term_list RPAREN
    Token t = lexer.peek(1);
    if (t.token_type == ID) {
        Token id = expect(ID);
        if (!current_poly_params.empty() && !is_valid_poly_param(id.lexeme))
            record_error(2, id.line_no);
    } else if (t.token_type == LPAREN) {
        expect(LPAREN);
        parse_term_list();
        expect(RPAREN);
    } else {
        syntax_error();
    }
}

void Parser::parse_exponent() {
    // exponent -> POWER NUM
    expect(POWER);
    expect(NUM);
}

void Parser::parse_add_operator() {
    // add_operator -> PLUS
    // add_operator -> MINUS
    Token t = lexer.peek(1);
    if (t.token_type == PLUS)
        expect(PLUS);
    else if (t.token_type == MINUS)
        expect(MINUS);
    else
        syntax_error();
}

void Parser::parse_coefficient() {
    // coefficient -> NUM
    expect(NUM);
}

void Parser::parse_execute_section() {
    // execute_section -> EXECUTE statement_list
    expect(EXECUTE);
    parse_statement_list();
}

void Parser::parse_statement_list() {
    // statement_list -> statement
    // statement_list -> statement statement_list
    parse_statement();
    Token t = lexer.peek(1);
    if (t.token_type == INPUT || t.token_type == OUTPUT || t.token_type == ID)
        parse_statement_list();
}

void Parser::parse_statement() {
    // statement -> input_statement
    // statement -> output_statement
    // statement -> assign_statement
    Token t = lexer.peek(1);
    if (t.token_type == INPUT)
        parse_input_statement();
    else if (t.token_type == OUTPUT)
        parse_output_statement();
    else if (t.token_type == ID)
        parse_assign_statement();
    else
        syntax_error();
}

void Parser::parse_input_statement() {
    // input_statement -> INPUT ID SEMICOLON
    expect(INPUT);
    expect(ID);
    expect(SEMICOLON);
}

void Parser::parse_output_statement() {
    // output_statement -> OUTPUT ID SEMICOLON
    expect(OUTPUT);
    expect(ID);
    expect(SEMICOLON);
}

void Parser::parse_assign_statement() {
    // assign_statement -> ID EQUAL poly_evaluation SEMICOLON
    expect(ID);
    expect(EQUAL);
    parse_poly_evaluation();
    expect(SEMICOLON);
}

void Parser::parse_poly_evaluation() {
    // poly_evaluation -> poly_name LPAREN argument_list RPAREN
    Token name = parse_poly_name();

    if (!is_declared_poly(name.lexeme))
        record_error(3, name.line_no);

    expect(LPAREN);
    int arg_count = parse_argument_list();
    expect(RPAREN);

    if (is_declared_poly(name.lexeme)) {
        int expected_count = poly_param_count(name.lexeme);
        if (arg_count != expected_count)
            record_error(4, name.line_no);
    }
}

int Parser::parse_argument_list() {
    // argument_list -> argument
    // argument_list -> argument COMMA argument_list
    parse_argument();
    int count = 1;
    Token t = lexer.peek(1);
    if (t.token_type == COMMA) {
        expect(COMMA);
        count += parse_argument_list();
    }
    return count;
}

void Parser::parse_argument() {
    // argument -> ID
    // argument -> NUM
    // argument -> poly_evaluation
    Token t1 = lexer.peek(1);
    Token t2 = lexer.peek(2);
    if (t1.token_type == ID && t2.token_type == LPAREN)
        parse_poly_evaluation();
    else if (t1.token_type == ID)
        expect(ID);
    else if (t1.token_type == NUM)
        expect(NUM);
    else
        syntax_error();
}

void Parser::parse_inputs_section() {
    // inputs_section -> INPUTS num_list
    expect(INPUTS);
    parse_num_list();
}

int main() {
    Parser parser;
    parser.parse_input();
    return 0;
}
