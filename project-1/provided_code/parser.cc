#include "parser.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>

using namespace std;

Parser::Parser() {
    for (int i = 0; i < 10; i++)
        tasks[i] = false;
    program = nullptr;
    next_location = 0;
    next_input_index = 0;
}

void Parser::syntax_error() {
    cout << "SYNTAX ERROR !!!!!&%!!\n";
    exit(1);
}

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

int Parser::poly_table_index(const string &name) {
    for (const PolyDecl &decl : poly_table) {
        if (decl.header.name.lexeme == name)
            return decl.table_index;
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

int Parser::param_index(const string &lexeme) {
    for (size_t i = 0; i < current_poly_params.size(); i++) {
        if (current_poly_params[i] == lexeme)
            return (int)i;
    }
    return 0;
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

void Parser::record_warning(int code, int line_no) {
    if (code == 1)
        warning_1.push_back(line_no);
    else if (code == 2)
        warning_2.push_back(line_no);
}

bool Parser::has_semantic_errors() {
    return !semantic_error_1.empty() || !semantic_error_2.empty() ||
           !semantic_error_3.empty() || !semantic_error_4.empty();
}

void Parser::print_semantic_errors() {
    sort(semantic_error_1.begin(), semantic_error_1.end());
    sort(semantic_error_2.begin(), semantic_error_2.end());
    sort(semantic_error_3.begin(), semantic_error_3.end());
    sort(semantic_error_4.begin(), semantic_error_4.end());

    if (!semantic_error_1.empty()) {
        cout << "Semantic Error Code 1:";
        for (int line : semantic_error_1)
            cout << " " << line;
        cout << " \n";
        return;
    }
    if (!semantic_error_2.empty()) {
        cout << "Semantic Error Code 2:";
        for (int line : semantic_error_2)
            cout << " " << line;
        cout << "\n";
        return;
    }
    if (!semantic_error_3.empty()) {
        cout << "Semantic Error Code 3:";
        for (int line : semantic_error_3)
            cout << " " << line;
        cout << "\n";
        return;
    }
    if (!semantic_error_4.empty()) {
        cout << "Semantic Error Code 4:";
        for (int line : semantic_error_4)
            cout << " " << line;
        cout << " \n";
    }
}

int Parser::get_location(const string &name) {
    if (location_table.count(name))
        return location_table[name];
    int loc = next_location++;
    location_table[name] = loc;
    if ((int)mem.size() <= loc)
        mem.resize(loc + 1, 0);
    return loc;
}

void Parser::parse_input() {
    parse_program();
    expect(END_OF_FILE);

    if (has_semantic_errors()) {
        if (tasks[1])
            print_semantic_errors();
        exit(0);
    }

    if (tasks[2])
        run_execution();
    if (tasks[3])
        print_warning_code_1();
    if (tasks[4])
        print_warning_code_2();
    if (tasks[5])
        print_polynomial_degrees();
}

void Parser::parse_program() {
    parse_tasks_section();
    parse_poly_section();
    parse_execute_section();
    parse_inputs_section();
}

void Parser::parse_tasks_section() {
    expect(TASKS);
    parse_task_num_list();
}

void Parser::parse_task_num_list() {
    Token num = expect(NUM);
    int task = atoi(num.lexeme.c_str());
    if (task >= 1 && task <= 9 && !tasks[task])
        tasks[task] = true;
    Token t = lexer.peek(1);
    if (t.token_type == NUM)
        parse_task_num_list();
}

void Parser::parse_num_list() {
    expect(NUM);
    Token t = lexer.peek(1);
    if (t.token_type == NUM)
        parse_num_list();
}

void Parser::parse_poly_section() {
    expect(POLY);
    parse_poly_decl_list();
}

void Parser::parse_poly_decl_list() {
    parse_poly_decl();
    Token t = lexer.peek(1);
    if (t.token_type == ID)
        parse_poly_decl_list();
}

void Parser::parse_poly_decl() {
    PolyHeader header = parse_poly_header();

    if (is_declared_poly(header.name.lexeme))
        record_error(1, header.name.line_no);

    PolyDecl decl;
    decl.header = header;
    decl.param_count = (int)header.params.size();
    decl.table_index = (int)poly_table.size();
    poly_table.push_back(decl);

    current_poly_params = header.params;

    expect(EQUAL);
    poly_table.back().body = parse_poly_body();
    expect(SEMICOLON);

    current_poly_params.clear();
}

PolyHeader Parser::parse_poly_header() {
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

Token Parser::parse_poly_name() { return expect(ID); }

vector<string> Parser::parse_id_list() {
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

TermList Parser::parse_poly_body() { return parse_term_list(); }

TermList Parser::parse_term_list() {
    TermList tl;
    tl.terms.push_back(parse_term());
    Token t = lexer.peek(1);
    if (t.token_type == PLUS || t.token_type == MINUS) {
        tl.ops.push_back(parse_add_operator());
        TermList rest = parse_term_list();
        tl.terms.insert(tl.terms.end(), rest.terms.begin(), rest.terms.end());
        tl.ops.insert(tl.ops.end(), rest.ops.begin(), rest.ops.end());
    }
    return tl;
}

Term Parser::parse_term() {
    Term term;
    term.coefficient = 1;
    term.has_coefficient = false;

    Token t1 = lexer.peek(1);
    if (t1.token_type == ID || t1.token_type == LPAREN) {
        term.monomials = parse_monomial_list();
    } else if (t1.token_type == NUM) {
        Token t2 = lexer.peek(2);
        if (t2.token_type == ID || t2.token_type == LPAREN) {
            term.coefficient = parse_coefficient();
            term.has_coefficient = true;
            term.monomials = parse_monomial_list();
        } else if (t2.token_type == PLUS || t2.token_type == MINUS ||
                   t2.token_type == RPAREN || t2.token_type == SEMICOLON ||
                   t2.token_type == COMMA || t2.token_type == END_OF_FILE) {
            term.coefficient = parse_coefficient();
            term.has_coefficient = true;
        } else {
            syntax_error();
        }
    } else {
        syntax_error();
    }
    return term;
}

vector<Monomial> Parser::parse_monomial_list() {
    vector<Monomial> ml;
    ml.push_back(parse_monomial());
    Token t = lexer.peek(1);
    if (t.token_type == ID || t.token_type == LPAREN) {
        vector<Monomial> rest = parse_monomial_list();
        ml.insert(ml.end(), rest.begin(), rest.end());
    }
    return ml;
}

Monomial Parser::parse_monomial() {
    Monomial m;
    m.exponent = 1;
    m.nested = nullptr;
    m.primary_kind = parse_primary_kind(m.var_index, &m.nested);
    Token t = lexer.peek(1);
    if (t.token_type == POWER)
        m.exponent = parse_exponent();
    return m;
}

PrimaryKind Parser::parse_primary_kind(int &var_index, TermList **nested) {
    Token t = lexer.peek(1);
    if (t.token_type == ID) {
        Token id = expect(ID);
        if (!current_poly_params.empty() && !is_valid_poly_param(id.lexeme))
            record_error(2, id.line_no);
        var_index = param_index(id.lexeme);
        *nested = nullptr;
        return PRIMARY_VAR;
    }
    if (t.token_type == LPAREN) {
        expect(LPAREN);
        TermList *tl = new TermList(parse_term_list());
        expect(RPAREN);
        var_index = 0;
        *nested = tl;
        return PRIMARY_TERM_LIST;
    }
    syntax_error();
    return PRIMARY_VAR;
}

int Parser::parse_exponent() {
    expect(POWER);
    Token num = expect(NUM);
    return atoi(num.lexeme.c_str());
}

TokenType Parser::parse_add_operator() {
    Token t = lexer.peek(1);
    if (t.token_type == PLUS)
        return expect(PLUS).token_type;
    if (t.token_type == MINUS)
        return expect(MINUS).token_type;
    syntax_error();
    return PLUS;
}

int Parser::parse_coefficient() {
    Token num = expect(NUM);
    return atoi(num.lexeme.c_str());
}

void Parser::parse_execute_section() {
    expect(EXECUTE);
    program = nullptr;
    parse_statement_list();
}

void Parser::parse_statement_list() {
    Stmt *stmt = parse_statement();
    if (!program)
        program = stmt;
    else {
        Stmt *cur = program;
        while (cur->next)
            cur = cur->next;
        cur->next = stmt;
    }
    Token t = lexer.peek(1);
    if (t.token_type == INPUT || t.token_type == OUTPUT || t.token_type == ID)
        parse_statement_list();
}

Stmt *Parser::parse_statement() {
    Token t = lexer.peek(1);
    if (t.token_type == INPUT)
        return parse_input_statement();
    if (t.token_type == OUTPUT)
        return parse_output_statement();
    if (t.token_type == ID)
        return parse_assign_statement();
    syntax_error();
    return nullptr;
}

Stmt *Parser::parse_input_statement() {
    Stmt *s = new Stmt();
    s->kind = STMT_INPUT;
    s->eval = nullptr;
    s->next = nullptr;
    expect(INPUT);
    Token id = expect(ID);
    s->var_name = id.lexeme;
    s->var_loc = get_location(id.lexeme);
    s->line_no = id.line_no;
    expect(SEMICOLON);
    return s;
}

Stmt *Parser::parse_output_statement() {
    Stmt *s = new Stmt();
    s->kind = STMT_OUTPUT;
    s->eval = nullptr;
    s->next = nullptr;
    expect(OUTPUT);
    Token id = expect(ID);
    s->var_name = id.lexeme;
    s->var_loc = get_location(id.lexeme);
    s->line_no = id.line_no;
    expect(SEMICOLON);
    return s;
}

Stmt *Parser::parse_assign_statement() {
    Stmt *s = new Stmt();
    s->kind = STMT_ASSIGN;
    s->next = nullptr;
    Token id = expect(ID);
    s->var_name = id.lexeme;
    s->var_loc = get_location(id.lexeme);
    s->line_no = id.line_no;
    expect(EQUAL);
    s->eval = parse_poly_evaluation();
    expect(SEMICOLON);
    return s;
}

PolyEval *Parser::parse_poly_evaluation() {
    PolyEval *pe = new PolyEval();
    Token name = parse_poly_name();
    pe->line_no = name.line_no;

    if (!is_declared_poly(name.lexeme))
        record_error(3, name.line_no);

    pe->poly_index = poly_table_index(name.lexeme);

    expect(LPAREN);
    pe->args = parse_argument_list();
    expect(RPAREN);

    if (is_declared_poly(name.lexeme)) {
        int expected_count = poly_param_count(name.lexeme);
        if ((int)pe->args.size() != expected_count)
            record_error(4, name.line_no);
    }

    return pe;
}

vector<PolyEvalArg> Parser::parse_argument_list() {
    vector<PolyEvalArg> args;
    args.push_back(parse_argument());
    Token t = lexer.peek(1);
    if (t.token_type == COMMA) {
        expect(COMMA);
        vector<PolyEvalArg> rest = parse_argument_list();
        args.insert(args.end(), rest.begin(), rest.end());
    }
    return args;
}

PolyEvalArg Parser::parse_argument() {
    PolyEvalArg arg;
    Token t1 = lexer.peek(1);
    Token t2 = lexer.peek(2);
    if (t1.token_type == ID && t2.token_type == LPAREN) {
        arg.kind = ARG_POLY;
        arg.nested = parse_poly_evaluation();
        arg.line_no = arg.nested->line_no;
    } else if (t1.token_type == ID) {
        Token id = expect(ID);
        arg.kind = ARG_ID;
        arg.var_name = id.lexeme;
        arg.var_loc = get_location(id.lexeme);
        arg.line_no = id.line_no;
        arg.nested = nullptr;
    } else if (t1.token_type == NUM) {
        Token num = expect(NUM);
        arg.kind = ARG_NUM;
        arg.num_value = atoi(num.lexeme.c_str());
        arg.line_no = num.line_no;
        arg.nested = nullptr;
    } else {
        syntax_error();
    }
    return arg;
}

void Parser::parse_inputs_section() {
    expect(INPUTS);
    parse_input_num_list();
}

void Parser::parse_input_num_list() {
    Token num = expect(NUM);
    input_values.push_back(atoi(num.lexeme.c_str()));
    Token t = lexer.peek(1);
    if (t.token_type == NUM)
        parse_input_num_list();
}

int Parser::ipow(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++)
        result *= base;
    return result;
}

int Parser::eval_monomial(const Monomial &m, const vector<int> &arg_values) {
    int val;
    if (m.primary_kind == PRIMARY_VAR)
        val = arg_values[m.var_index];
    else
        val = eval_term_list(*m.nested, arg_values);
    return ipow(val, m.exponent);
}

int Parser::eval_term(const Term &term, const vector<int> &arg_values) {
    int coeff = term.has_coefficient ? term.coefficient : 1;
    if (term.monomials.empty())
        return coeff;
    int product = 1;
    for (const Monomial &m : term.monomials)
        product *= eval_monomial(m, arg_values);
    return coeff * product;
}

int Parser::eval_term_list(const TermList &tl, const vector<int> &arg_values) {
    int result = eval_term(tl.terms[0], arg_values);
    for (size_t i = 0; i < tl.ops.size(); i++) {
        int val = eval_term(tl.terms[i + 1], arg_values);
        if (tl.ops[i] == PLUS)
            result += val;
        else
            result -= val;
    }
    return result;
}

int Parser::eval_poly_body(const TermList &body,
                           const vector<int> &arg_values) {
    return eval_term_list(body, arg_values);
}

int Parser::eval_arg(const PolyEvalArg &arg, const vector<int> &mem_vals) {
    if (arg.kind == ARG_NUM)
        return arg.num_value;
    if (arg.kind == ARG_ID)
        return mem_vals[arg.var_loc];
    return eval_poly_eval(arg.nested, mem_vals);
}

int Parser::eval_poly_eval(PolyEval *pe, const vector<int> &mem_vals) {
    vector<int> arg_values;
    for (const PolyEvalArg &arg : pe->args)
        arg_values.push_back(eval_arg(arg, mem_vals));
    return eval_poly_body(poly_table[pe->poly_index].body, arg_values);
}

void Parser::execute_program() {
    next_input_index = 0;
    if ((int)mem.size() < next_location)
        mem.resize(next_location, 0);
    for (int i = 0; i < next_location; i++)
        mem[i] = 0;

    for (Stmt *s = program; s; s = s->next) {
        if (s->kind == STMT_INPUT) {
            if (next_input_index < (int)input_values.size())
                mem[s->var_loc] = input_values[next_input_index++];
        } else if (s->kind == STMT_OUTPUT) {
            cout << mem[s->var_loc] << " \n";
        } else if (s->kind == STMT_ASSIGN) {
            mem[s->var_loc] = eval_poly_eval(s->eval, mem);
        }
    }
}

void Parser::run_execution() { execute_program(); }

void Parser::check_poly_eval_warning1(PolyEval *pe,
                                      const set<string> &initialized) {
    for (const PolyEvalArg &arg : pe->args) {
        if (arg.kind == ARG_ID) {
            if (!initialized.count(arg.var_name))
                record_warning(1, arg.line_no);
        } else if (arg.kind == ARG_POLY) {
            check_poly_eval_warning1(arg.nested, initialized);
        }
    }
}

void Parser::analyze_warning_code_1() {
    set<string> initialized;
    for (Stmt *s = program; s; s = s->next) {
        if (s->kind == STMT_ASSIGN)
            check_poly_eval_warning1(s->eval, initialized);
        if (s->kind == STMT_INPUT)
            initialized.insert(s->var_name);
        if (s->kind == STMT_ASSIGN)
            initialized.insert(s->var_name);
    }
}

void Parser::print_warning_code_1() {
    analyze_warning_code_1();
    sort(warning_1.begin(), warning_1.end());
    if (warning_1.empty())
        return;
    cout << "Warning Code 1:";
    for (int line : warning_1)
        cout << " " << line;
    cout << "\n";
}

bool Parser::poly_eval_uses_var(PolyEval *pe, const string &name) {
    for (const PolyEvalArg &arg : pe->args) {
        if (arg.kind == ARG_ID && arg.var_name == name)
            return true;
        if (arg.kind == ARG_POLY && poly_eval_uses_var(arg.nested, name))
            return true;
    }
    return false;
}

bool Parser::stmt_defines(Stmt *s, const string &name) {
    if (s->kind == STMT_INPUT && s->var_name == name)
        return true;
    if (s->kind == STMT_ASSIGN && s->var_name == name)
        return true;
    return false;
}

bool Parser::stmt_uses(Stmt *s, const string &name) {
    if (s->kind == STMT_OUTPUT && s->var_name == name)
        return true;
    if (s->kind == STMT_ASSIGN && poly_eval_uses_var(s->eval, name))
        return true;
    return false;
}

bool Parser::is_useless_assign(Stmt *assign, Stmt *next) {
    string x = assign->var_name;
    if (!next)
        return true;
    if (stmt_uses(next, x))
        return false;
    if (stmt_defines(next, x))
        return true;
    return is_useless_assign(assign, next->next);
}

void Parser::analyze_warning_code_2() {
    for (Stmt *s = program; s; s = s->next) {
        if (s->kind == STMT_ASSIGN && is_useless_assign(s, s->next))
            record_warning(2, s->line_no);
    }
}

void Parser::print_warning_code_2() {
    analyze_warning_code_2();
    sort(warning_2.begin(), warning_2.end());
    if (warning_2.empty())
        return;
    cout << "Warning Code 2:";
    for (int line : warning_2)
        cout << " " << line;
    cout << "\n";
}

int Parser::degree_primary_kind(PrimaryKind kind, int var_index,
                                TermList *nested) {
    (void)var_index;
    if (kind == PRIMARY_VAR)
        return 1;
    return degree_term_list(*nested);
}

int Parser::degree_monomial(const Monomial &m) {
    return degree_primary_kind(m.primary_kind, m.var_index, m.nested) *
           m.exponent;
}

int Parser::degree_monomial_list(const vector<Monomial> &ml) {
    int sum = 0;
    for (const Monomial &m : ml)
        sum += degree_monomial(m);
    return sum;
}

int Parser::degree_term(const Term &term) {
    if (term.monomials.empty())
        return 0;
    return degree_monomial_list(term.monomials);
}

int Parser::degree_term_list(const TermList &tl) {
    int max_deg = degree_term(tl.terms[0]);
    for (size_t i = 1; i < tl.terms.size(); i++) {
        int d = degree_term(tl.terms[i]);
        if (d > max_deg)
            max_deg = d;
    }
    return max_deg;
}

void Parser::print_polynomial_degrees() {
    for (const PolyDecl &decl : poly_table) {
        int deg = degree_term_list(decl.body);
        cout << decl.header.name.lexeme << ": " << deg << " \n";
    }
}

int main() {
    Parser parser;
    parser.parse_input();
    return 0;
}
