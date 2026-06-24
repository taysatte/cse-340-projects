/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include "lexer.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Rule {
    string lhs;
    vector<string> rhs;
};

LexicalAnalyzer lexer;

vector<Rule> rules;
vector<string> nt_order;
vector<string> terminal_order;
vector<string> appearance_order;
set<string> non_terminal_set;
string start_symbol;

unordered_map<string, set<string>> FIRST_sets;
unordered_map<string, set<string>> FOLLOW_sets;
set<string> nullable_set;

void syntax_error() {
    cout << "SYNTAX ERROR !!!!!!!!!!!!!!\n";
    exit(1);
}

Token expect(TokenType t) {
    Token tok = lexer.GetToken();
    if (tok.token_type != t)
        syntax_error();
    return tok;
}

vector<string> parse_id_list() {
    vector<string> ids;
    Token t = lexer.peek(1);
    if (t.token_type == ID) {
        ids.push_back(expect(ID).lexeme);
        vector<string> rest = parse_id_list();
        ids.insert(ids.end(), rest.begin(), rest.end());
    }
    return ids;
}

vector<vector<string>> parse_rhs() {
    vector<string> alt = parse_id_list();
    vector<vector<string>> alts;
    alts.push_back(alt);
    Token t = lexer.peek(1);
    if (t.token_type == OR) {
        expect(OR);
        vector<vector<string>> rest = parse_rhs();
        alts.insert(alts.end(), rest.begin(), rest.end());
    }
    return alts;
}

void parse_rule() {
    string lhs = expect(ID).lexeme;
    expect(ARROW);
    vector<vector<string>> alts = parse_rhs();
    expect(STAR);
    for (const vector<string> &alt : alts) {
        Rule r;
        r.lhs = lhs;
        r.rhs = alt;
        rules.push_back(r);
    }
}

void parse_rule_list() {
    parse_rule();
    Token t = lexer.peek(1);
    if (t.token_type == ID)
        parse_rule_list();
}

void record_symbol(const string &sym) {
    if (find(appearance_order.begin(), appearance_order.end(), sym) ==
        appearance_order.end())
        appearance_order.push_back(sym);
}

void build_symbol_orders() {
    set<string> lhs_symbols;
    for (const Rule &r : rules)
        lhs_symbols.insert(r.lhs);
    non_terminal_set = lhs_symbols;

    set<string> seen_nt;
    set<string> seen_term;
    for (const Rule &r : rules) {
        auto process = [&](const string &sym) {
            if (lhs_symbols.count(sym)) {
                if (!seen_nt.count(sym)) {
                    seen_nt.insert(sym);
                    nt_order.push_back(sym);
                }
                record_symbol(sym);
            } else {
                if (!seen_term.count(sym)) {
                    seen_term.insert(sym);
                    terminal_order.push_back(sym);
                }
                record_symbol(sym);
            }
        };
        process(r.lhs);
        for (const string &sym : r.rhs)
            process(sym);
    }
}

void ReadGrammar() {
    parse_rule_list();
    expect(HASH);
    if (rules.empty())
        syntax_error();
    start_symbol = rules[0].lhs;
    build_symbol_orders();
}

bool is_non_terminal(const string &sym) { return non_terminal_set.count(sym); }

bool set_union_into(set<string> &dest, const set<string> &src) {
    bool changed = false;
    for (const string &s : src) {
        if (!dest.count(s)) {
            dest.insert(s);
            changed = true;
        }
    }
    return changed;
}

bool is_nullable_string(const vector<string> &symbols) {
    if (symbols.empty())
        return true;
    for (const string &s : symbols) {
        if (!nullable_set.count(s))
            return false;
    }
    return true;
}

set<string> first_of_string(const vector<string> &symbols) {
    set<string> result;
    for (size_t i = 0; i < symbols.size(); i++) {
        const string &sym = symbols[i];
        if (!is_non_terminal(sym)) {
            result.insert(sym);
            return result;
        }
        set_union_into(result, FIRST_sets[sym]);
        if (!nullable_set.count(sym))
            return result;
    }
    return result;
}

void compute_nullable() {
    bool changed = true;
    while (changed) {
        changed = false;
        for (const Rule &r : rules) {
            if (nullable_set.count(r.lhs))
                continue;
            if (is_nullable_string(r.rhs)) {
                nullable_set.insert(r.lhs);
                changed = true;
            }
        }
    }
}

void compute_first() {
    for (const string &t : terminal_order)
        FIRST_sets[t].insert(t);
    for (const string &nt : nt_order)
        FIRST_sets[nt] = set<string>();

    bool changed = true;
    while (changed) {
        changed = false;
        for (const Rule &r : rules) {
            set<string> first_rhs = first_of_string(r.rhs);
            if (set_union_into(FIRST_sets[r.lhs], first_rhs))
                changed = true;
        }
    }
}

void compute_follow() {
    for (const string &nt : nt_order)
        FOLLOW_sets[nt] = set<string>();
    FOLLOW_sets[start_symbol].insert("$");

    bool changed = true;
    while (changed) {
        changed = false;
        for (const Rule &r : rules) {
            for (size_t i = 0; i < r.rhs.size(); i++) {
                const string &B = r.rhs[i];
                if (!is_non_terminal(B))
                    continue;
                vector<string> beta(r.rhs.begin() + i + 1, r.rhs.end());
                set<string> first_beta = first_of_string(beta);
                if (set_union_into(FOLLOW_sets[B], first_beta))
                    changed = true;
                if (is_nullable_string(beta)) {
                    if (set_union_into(FOLLOW_sets[B], FOLLOW_sets[r.lhs]))
                        changed = true;
                }
            }
        }
    }
}

void print_set_first_follow(const set<string> &s, bool follow) {
    cout << "{";
    bool first_elem = true;
    bool any = false;
    if (follow && s.count("$")) {
        cout << " $";
        first_elem = false;
        any = true;
    }
    for (const string &sym : appearance_order) {
        if (sym == "$")
            continue;
        if (s.count(sym)) {
            if (!first_elem)
                cout << ", ";
            else
                cout << " ";
            cout << sym;
            first_elem = false;
            any = true;
        }
    }
    if (!any)
        cout << "  ";
    cout << " }";
}

void Task1() {
    bool first = true;
    for (const string &t : terminal_order) {
        if (!first)
            cout << " ";
        cout << t;
        first = false;
    }
    for (const string &nt : nt_order) {
        if (!first)
            cout << " ";
        cout << nt;
        first = false;
    }
}

void Task2() {
    compute_nullable();
    cout << "Nullable = {";
    bool first = true;
    bool any = false;
    for (const string &nt : nt_order) {
        if (!nullable_set.count(nt))
            continue;
        if (!first)
            cout << " , ";
        else
            cout << " ";
        cout << nt;
        first = false;
        any = true;
    }
    if (!any)
        cout << " ";
    cout << " }";
}

void Task3() {
    compute_nullable();
    compute_first();
    for (size_t i = 0; i < nt_order.size(); i++) {
        cout << "FIRST(" << nt_order[i] << ") = ";
        print_set_first_follow(FIRST_sets[nt_order[i]], false);
        if (i + 1 < nt_order.size())
            cout << "\n";
    }
}

void Task4() {
    compute_nullable();
    compute_first();
    compute_follow();
    for (size_t i = 0; i < nt_order.size(); i++) {
        cout << "FOLLOW(" << nt_order[i] << ") = ";
        print_set_first_follow(FOLLOW_sets[nt_order[i]], true);
        if (i + 1 < nt_order.size())
            cout << "\n";
    }
}

int lex_compare_seq(const vector<string> &a, const vector<string> &b) {
    size_t n = min(a.size(), b.size());
    for (size_t i = 0; i < n; i++) {
        if (a[i] < b[i])
            return -1;
        if (a[i] > b[i])
            return 1;
    }
    if (a.size() < b.size())
        return -1;
    if (a.size() > b.size())
        return 1;
    return 0;
}

vector<string> rule_sequence(const Rule &r) {
    vector<string> seq;
    seq.push_back(r.lhs);
    seq.insert(seq.end(), r.rhs.begin(), r.rhs.end());
    return seq;
}

bool rule_less(const Rule &a, const Rule &b) {
    return lex_compare_seq(rule_sequence(a), rule_sequence(b)) < 0;
}

int common_prefix_len(const vector<string> &a, const vector<string> &b) {
    int len = 0;
    while (len < (int)a.size() && len < (int)b.size() && a[len] == b[len])
        len++;
    return len;
}

vector<string> prefix_of(const vector<string> &rhs, int len) {
    return vector<string>(rhs.begin(), rhs.begin() + len);
}

vector<string> suffix_after(const vector<string> &rhs, int len) {
    return vector<string>(rhs.begin() + len, rhs.end());
}

int longest_match_rule(const Rule &r, const vector<Rule> &group) {
    int best = 0;
    for (const Rule &other : group) {
        if (r.lhs != other.lhs)
            continue;
        if (r.rhs == other.rhs && &r == &other)
            continue;
        best = max(best, common_prefix_len(r.rhs, other.rhs));
    }
    return best;
}

bool same_prefix(const vector<string> &a, const vector<string> &b, int len) {
    return common_prefix_len(a, b) >= len;
}

void print_rules(const vector<Rule> &out_rules) {
    vector<Rule> sorted = out_rules;
    sort(sorted.begin(), sorted.end(), rule_less);
    for (const Rule &r : sorted) {
        cout << r.lhs << " ->";
        for (const string &sym : r.rhs)
            cout << " " << sym;
        cout << " #\n";
    }
}

void Task5() {
    vector<Rule> G = rules;
    vector<Rule> Gprime;
    map<string, int> factor_count;

    while (true) {
        map<string, vector<int>> indices_by_lhs;
        for (size_t i = 0; i < G.size(); i++)
            indices_by_lhs[G[i].lhs].push_back((int)i);

        int best_len = 0;
        vector<string> best_prefix;
        string best_lhs;

        for (auto &kv : indices_by_lhs) {
            const string &lhs = kv.first;
            vector<Rule> group;
            for (int idx : kv.second)
                group.push_back(G[idx]);

            if (group.size() < 2)
                continue;

            for (size_t i = 0; i < group.size(); i++) {
                for (size_t j = i + 1; j < group.size(); j++) {
                    int plen = common_prefix_len(group[i].rhs, group[j].rhs);
                    if (plen == 0)
                        continue;
                    vector<string> prefix = prefix_of(group[i].rhs, plen);
                    if (plen > best_len ||
                        (plen == best_len && plen > 0 &&
                         (best_prefix.empty() ||
                          lex_compare_seq(prefix, best_prefix) < 0))) {
                        best_len = plen;
                        best_prefix = prefix;
                        best_lhs = lhs;
                    }
                }
            }
        }

        if (best_len == 0)
            break;

        string new_name = best_lhs + to_string(++factor_count[best_lhs]);

        vector<Rule> new_G;
        vector<Rule> factored_rules;
        vector<Rule> suffix_rules;

        for (const Rule &r : G) {
            if (r.lhs == best_lhs && (int)r.rhs.size() >= best_len &&
                same_prefix(r.rhs, best_prefix, best_len)) {
                factored_rules.push_back(r);
            } else {
                new_G.push_back(r);
            }
        }

        Rule factored;
        factored.lhs = best_lhs;
        factored.rhs = best_prefix;
        factored.rhs.push_back(new_name);
        new_G.push_back(factored);

        for (const Rule &r : factored_rules) {
            Rule nr;
            nr.lhs = new_name;
            nr.rhs = suffix_after(r.rhs, best_len);
            suffix_rules.push_back(nr);
        }

        bool suffix_done = true;
        map<string, vector<Rule>> by_lhs;
        for (const Rule &r : suffix_rules)
            by_lhs[r.lhs].push_back(r);
        for (auto &kv : by_lhs) {
            const vector<Rule> &grp = kv.second;
            if (grp.size() < 2)
                continue;
            for (size_t i = 0; i < grp.size(); i++) {
                for (size_t j = i + 1; j < grp.size(); j++) {
                    if (common_prefix_len(grp[i].rhs, grp[j].rhs) > 0) {
                        suffix_done = false;
                        break;
                    }
                }
                if (!suffix_done)
                    break;
            }
            if (!suffix_done)
                break;
        }

        if (suffix_done) {
            Gprime.insert(Gprime.end(), suffix_rules.begin(),
                          suffix_rules.end());
            G = new_G;
        } else {
            G = new_G;
            G.insert(G.end(), suffix_rules.begin(), suffix_rules.end());
        }
    }

    Gprime.insert(Gprime.end(), G.begin(), G.end());
    print_rules(Gprime);
}

void eliminate_immediate_left_recursion(const string &A, vector<Rule> &rulesA,
                                        map<string, vector<Rule>> &all_rules) {
    vector<Rule> recursive;
    vector<Rule> non_recursive;
    for (const Rule &r : rulesA) {
        if (!r.rhs.empty() && r.rhs[0] == A)
            recursive.push_back(r);
        else
            non_recursive.push_back(r);
    }
    if (recursive.empty())
        return;

    string A1 = A + "1";
    vector<Rule> newA;
    for (const Rule &r : non_recursive) {
        Rule nr;
        nr.lhs = A;
        nr.rhs = r.rhs;
        nr.rhs.push_back(A1);
        newA.push_back(nr);
    }
    vector<Rule> newA1;
    for (const Rule &r : recursive) {
        Rule nr;
        nr.lhs = A1;
        nr.rhs = vector<string>(r.rhs.begin() + 1, r.rhs.end());
        nr.rhs.push_back(A1);
        newA1.push_back(nr);
    }
    Rule eps;
    eps.lhs = A1;
    eps.rhs = {};
    newA1.push_back(eps);

    all_rules[A] = newA;
    all_rules[A1] = newA1;
}

void Task6() {
    vector<string> sorted_nt(nt_order.begin(), nt_order.end());
    sort(sorted_nt.begin(), sorted_nt.end());

    map<string, vector<Rule>> all_rules;
    for (const Rule &r : rules)
        all_rules[r.lhs].push_back(r);

    for (size_t i = 0; i < sorted_nt.size(); i++) {
        string Ai = sorted_nt[i];
        for (size_t j = 0; j < i; j++) {
            string Aj = sorted_nt[j];
            vector<Rule> updated;
            for (const Rule &r : all_rules[Ai]) {
                if (!r.rhs.empty() && r.rhs[0] == Aj) {
                    vector<string> gamma(r.rhs.begin() + 1, r.rhs.end());
                    for (const Rule &rj : all_rules[Aj]) {
                        Rule nr;
                        nr.lhs = Ai;
                        nr.rhs = rj.rhs;
                        nr.rhs.insert(nr.rhs.end(), gamma.begin(), gamma.end());
                        updated.push_back(nr);
                    }
                } else {
                    updated.push_back(r);
                }
            }
            all_rules[Ai] = updated;
        }
        eliminate_immediate_left_recursion(Ai, all_rules[Ai], all_rules);
    }

    vector<Rule> result;
    for (auto &kv : all_rules) {
        for (const Rule &r : kv.second)
            result.push_back(r);
    }
    print_rules(result);
}

int main(int argc, char *argv[]) {
    int task;

    if (argc < 2) {
        cout << "Error: missing argument\n";
        return 1;
    }

    task = atoi(argv[1]);

    ReadGrammar();

    switch (task) {
    case 1:
        Task1();
        break;
    case 2:
        Task2();
        break;
    case 3:
        Task3();
        break;
    case 4:
        Task4();
        break;
    case 5:
        Task5();
        break;
    case 6:
        Task6();
        break;
    default:
        cout << "Error: unrecognized task number " << task << "\n";
        break;
    }
    return 0;
}
