/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include "lexer.h"
#include <string>

class Parser {
  public:
    void ConsumeAllInput();
    void parse_input();
    void parse_program();
    void parse_tasks();
    void parse_poly();
    void parse_execute();
    void parse_inputs();

  private:
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);
};

#endif
