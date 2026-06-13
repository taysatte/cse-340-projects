/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include "lexer.h"
#include <string>
#include <vector>

struct PolyHeader {
    Token name;
    std::vector<std::string> params;
};

struct PolyDecl {
    PolyHeader header;
    int param_count;
};

class Parser {
  public:
    void parse_input();

  private:
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);

    std::vector<PolyDecl> poly_table;
    std::vector<int> semantic_error_1;
    std::vector<int> semantic_error_2;
    std::vector<int> semantic_error_3;
    std::vector<int> semantic_error_4;
    std::vector<std::string> current_poly_params;

    bool is_declared_poly(const std::string &name);
    int poly_param_count(const std::string &name);
    bool is_valid_poly_param(const std::string &lexeme);
    void record_error(int code, int line_no);
    void report_semantic_errors();

    void parse_program();
    void parse_tasks_section();
    void parse_num_list();
    void parse_poly_section();
    void parse_poly_decl_list();
    void parse_poly_decl();
    PolyHeader parse_poly_header();
    Token parse_poly_name();
    std::vector<std::string> parse_id_list();
    void parse_poly_body();
    void parse_term_list();
    void parse_term();
    void parse_monomial_list();
    void parse_monomial();
    void parse_primary();
    void parse_exponent();
    void parse_add_operator();
    void parse_coefficient();
    void parse_execute_section();
    void parse_statement_list();
    void parse_statement();
    void parse_input_statement();
    void parse_output_statement();
    void parse_assign_statement();
    void parse_poly_evaluation();
    int parse_argument_list();
    void parse_argument();
    void parse_inputs_section();
};

#endif
