#ifndef __PARSER_H__
#define __PARSER_H__

#include "lexer.h"
#include <map>
#include <set>
#include <string>
#include <vector>

struct PolyHeader {
    Token name;
    std::vector<std::string> params;
};

enum PrimaryKind { PRIMARY_VAR, PRIMARY_TERM_LIST };

struct TermList;

struct Monomial {
    PrimaryKind primary_kind;
    int var_index;
    TermList *nested;
    int exponent;
};

struct Term {
    int coefficient;
    bool has_coefficient;
    std::vector<Monomial> monomials;
};

struct TermList {
    std::vector<Term> terms;
    std::vector<TokenType> ops;
};

struct PolyDecl {
    PolyHeader header;
    int param_count;
    TermList body;
    int table_index;
};

struct PolyEval;

enum ArgKind { ARG_NUM, ARG_ID, ARG_POLY };

struct PolyEvalArg {
    ArgKind kind;
    int num_value;
    int var_loc;
    std::string var_name;
    int line_no;
    PolyEval *nested;
};

struct PolyEval {
    int poly_index;
    std::vector<PolyEvalArg> args;
    int line_no;
};

enum StmtKind { STMT_INPUT, STMT_OUTPUT, STMT_ASSIGN };

struct Stmt {
    StmtKind kind;
    int var_loc;
    std::string var_name;
    int line_no;
    PolyEval *eval;
    Stmt *next;
};

class Parser {
  public:
    Parser();
    void parse_input();

  private:
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);

    bool tasks[10];
    std::vector<PolyDecl> poly_table;
    std::vector<int> semantic_error_1;
    std::vector<int> semantic_error_2;
    std::vector<int> semantic_error_3;
    std::vector<int> semantic_error_4;
    std::vector<int> warning_1;
    std::vector<int> warning_2;
    std::vector<std::string> current_poly_params;

    Stmt *program;
    std::map<std::string, int> location_table;
    int next_location;
    std::vector<int> mem;
    std::vector<int> input_values;
    int next_input_index;

    bool is_declared_poly(const std::string &name);
    int poly_param_count(const std::string &name);
    int poly_table_index(const std::string &name);
    bool is_valid_poly_param(const std::string &lexeme);
    int param_index(const std::string &lexeme);
    void record_error(int code, int line_no);
    void record_warning(int code, int line_no);
    bool has_semantic_errors();
    void print_semantic_errors();
    int get_location(const std::string &name);

    void parse_program();
    void parse_tasks_section();
    void parse_task_num_list();
    void parse_num_list();
    void parse_poly_section();
    void parse_poly_decl_list();
    void parse_poly_decl();
    PolyHeader parse_poly_header();
    Token parse_poly_name();
    std::vector<std::string> parse_id_list();
    TermList parse_poly_body();
    TermList parse_term_list();
    Term parse_term();
    std::vector<Monomial> parse_monomial_list();
    Monomial parse_monomial();
    PrimaryKind parse_primary_kind(int &var_index, TermList **nested);
    int parse_exponent();
    TokenType parse_add_operator();
    int parse_coefficient();
    void parse_execute_section();
    void parse_statement_list();
    Stmt *parse_statement();
    Stmt *parse_input_statement();
    Stmt *parse_output_statement();
    Stmt *parse_assign_statement();
    PolyEval *parse_poly_evaluation();
    std::vector<PolyEvalArg> parse_argument_list();
    PolyEvalArg parse_argument();
    void parse_inputs_section();
    void parse_input_num_list();

    int ipow(int base, int exp);
    int eval_monomial(const Monomial &m, const std::vector<int> &arg_values);
    int eval_term(const Term &term, const std::vector<int> &arg_values);
    int eval_term_list(const TermList &tl, const std::vector<int> &arg_values);
    int eval_poly_body(const TermList &body,
                       const std::vector<int> &arg_values);
    int eval_arg(const PolyEvalArg &arg, const std::vector<int> &mem);
    int eval_poly_eval(PolyEval *pe, const std::vector<int> &mem);
    void execute_program();
    void run_execution();

    void check_poly_eval_warning1(PolyEval *pe,
                                  const std::set<std::string> &initialized);
    void analyze_warning_code_1();
    void print_warning_code_1();
    bool poly_eval_uses_var(PolyEval *pe, const std::string &name);
    bool stmt_defines(Stmt *s, const std::string &name);
    bool stmt_uses(Stmt *s, const std::string &name);
    bool is_useless_assign(Stmt *assign, Stmt *next);
    void analyze_warning_code_2();
    void print_warning_code_2();

    int degree_primary_kind(PrimaryKind kind, int var_index, TermList *nested);
    int degree_monomial(const Monomial &m);
    int degree_monomial_list(const std::vector<Monomial> &ml);
    int degree_term(const Term &term);
    int degree_term_list(const TermList &tl);
    void print_polynomial_degrees();
};

#endif
