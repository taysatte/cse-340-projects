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

  private:
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);
};

#endif
