
  // /// local-declaration:
  // ///   variable-definition
  // Declaration*
  // Parser::parse_local_declaration()
  // {
  //   switch (lookahead()) {
  //   case Token::val_kw:
  //   case Token::var_kw:
  //     return parse_variable_definition();
  //   default:
  //     break;
  //   }
  //   throw std::runtime_error("expected local-declaration");
  // } 


  // -------------------------------------------------------------------------- //
  // Expressions

  /// expression:
  ///   assignment-expression
  Expression*
  Parser::parse_expression()
  {
    return parse_assignment_expression();
  }

  /// assignment-expression:
  ///   conditional-expression '=' assignment-expression
  ///   conditional-expression
  Expression*
  Parser::parse_assignment_expression()
  {
    Expression* e1 = parse_conditional_expression();
    if (Token op = match_if(Token::equal)) {
      Expression* e2 = parse_assignment_expression();
      return m_act.on_assignment_expression(e1, e2, op);
    }
    return e1;
  }

  /// conditional-expression:
  ///   logical-or-expression '?' expression ':' conditional-expression
  ///   logical-or-expression
  Expression*
  Parser::parse_conditional_expression()
  {
    Expression* e1 = parse_logical_or_expression();
    if (Token tok1 = match_if(Token::question)) {
      Expression* e2 = parse_expression();
      Token tok2 = match(Token::colon);
      Expression* e3 = parse_conditional_expression();
      return m_act.on_conditional_expression(e1, e2, e3, tok1, tok2);
    }
    return e1;
  }

  /// logical-or-expression:
  ///   logical-or-expression '||' logical-and-expression
  ///   logical-and-expression
  Expression*
  Parser::parse_logical_or_expression()
  {
    Expression* e1 = parse_logical_and_expression();
    while (Token op = match(Token::bar_bar)) {
      Expression* e2 = parse_logical_and_expression();
      e1 = m_act.on_logical_expression(e1, e2, op);
    }
    return e1;
  }

  /// logical-and-expression:
  ///   logical-and-expression '&&' bitwise-or-expression
  ///   bitwise-or-expression
  Expression*
  Parser::parse_logical_and_expression()
  {
    Expression* e1 = parse_bitwise_or_expression();
    while (Token op = match(Token::ampersand_ampersand)) {
      Expression* e2 = parse_bitwise_or_expression();
      e1 = m_act.on_logical_expression(e1, e2, op);
    }
    return e1;
  }

  /// bitwise-or-expression:
  ///   bitwise-or-expression '|' bitwise-xor-expression
  ///   bitwise-xor-expression
  Expression*
  Parser::parse_bitwise_or_expression()
  {
    Expression* e1 = parse_bitwise_xor_expression();
    while (Token op = match(Token::bar)) {
      Expression* e2 = parse_bitwise_xor_expression();
      e1 = m_act.on_bitwise_expression(e1, e2, op);
    }
    return e1;
  }

  /// bitwise-xor-expression:
  ///   bitwise-xor-expression '^' bitwise-and-expression
  ///   bitwise-and-expression
  Expression*
  Parser::parse_bitwise_xor_expression()
  {
    Expression* e1 = parse_bitwise_and_expression();
    while (Token op = match(Token::caret)) {
      Expression* e2 = parse_bitwise_and_expression();
      e1 = m_act.on_bitwise_expression(e1, e2, op);
    }
    return e1;
  }

  /// bitwise-and-expression:
  ///   bitwise-and-expression '&' equality-expression
  ///   equality-expression
  Expression*
  Parser::parse_bitwise_and_expression()
  {
    Expression* e1 = parse_equality_expression();
    while (Token op = match(Token::ampersand)) {
      Expression* e2 = parse_equality_expression();
      e1 = m_act.on_bitwise_expression(e1, e2, op);
    }
    return e1;
  }

  /// equality-expression:
  ///   equality-expression '==' relational-expression
  ///   equality-expression '!=' relational-expression
  ///   relational-expression
  Expression*
  Parser::parse_equality_expression()
  {
    Expression* e1 = parse_relational_expression();
    while (Token op = match_if_equality_operator()) {
      Expression* e2 = parse_relational_expression();
      e1 = m_act.on_equality_expression(e1, e2, op);
    }
    return e1;
  }

  /// relational-expression:
  ///   relational-expression '<' shift-expression
  ///   relational-expression '>' shift-expression
  ///   relational-expression '<=' shift-expression
  ///   relational-expression '>=' shift-expression
  ///   shift-expression
  Expression*
  Parser::parse_relational_expression()
  {
    Expression* e1 = parse_shift_expression();
    while (Token op = match_if_relational_operator()) {
      Expression* e2 = parse_shift_expression();
      e1 = m_act.on_relational_expression(e1, e2, op);
    }
    return e1;
  }

  /// shift-expression:
  ///   shift-expression '<<' additive-expression
  ///   shift-expression '>>' additive-expression
  ///   additive-expression
  Expression*
  Parser::parse_shift_expression()
  {
    Expression* e1 = parse_additive_expression();
    while (Token op = match_if_shift_operator()) {
      Expression* e2 = parse_additive_expression();
      e1 = m_act.on_shift_expression(e1, e2, op);
    }
    return e1;
  }

  /// additive-expression:
  ///   additive-expression '+' multiplicative-expression
  ///   additive-expression '-' multiplicative-expression
  ///   multiplicative-expression
  Expression*
  Parser::parse_additive_expression()
  {
    Expression* e1 = parse_multiplicative_expression();
    while (Token op = match_if_additive_operator()) {
      Expression* e2 = parse_multiplicative_expression();
      e1 = m_act.on_additive_expression(e1, e2, op);
    }
    return e1;
  }

  /// multiplicative-expression:
  ///   multiplicative-expression '*' additive-expression
  ///   multiplicative-expression '/' additive-expression
  ///   multiplicative-expression '%' additive-expression
  ///   additive-expression
  Expression*
  Parser::parse_multiplicative_expression()
  {
    Expression* e1 = parse_cast_expression();
    while (Token op = match_if_multiplicative_operator()) {
      Expression* e2 = parse_cast_expression();
      e1 = m_act.on_multiplicative_expression(e1, e2, op);
    }
    return e1;
  }

  Expression*
  Parser::parse_cast_expression()
  {
    // Expression* e = parse_unary_expression();
    // if (match_if(kw_as)) {
    //   type* t = parse_type();
    //   return m_act.on_cast_expression(e, t);
    // }
    // return e;

    return parse_unary_expression();
  }

  /// unary-expression:
  ///   '+' unary-expression
  ///   '-' unary-expression
  ///   '~' unary-expression
  ///   '!' unary-expression
  ///   postfix-expression  
  Expression*
  Parser::parse_unary_expression()
  {
    while (Token op = match_if_unary_operator()) {
      Expression* e = parse_unary_expression();
      return m_act.on_unary_expression(e, op);
    }
    return parse_postfix_expression();
  }

  /// postfix-expression:
  ///   postfix-expression '(' argument-list ')'
  ///   postfix-expression '[' subscript-list ']'
  ///   primary-expression
  Expression*
  Parser::parse_postfix_expression()
  {
    Expression* e = parse_primary_expression();
    while (true) {
      if (lookahead() == Token::lparen)
        return parse_call_expression(e);
      else if (lookahead(Token::lbracket))
        return parse_subscript_expression(e);
      else
        break;
    }
    return e;
  }

  /// postfix-expression:
  ///   postfix-expression '(' argument-list ')'
  Expression*
  Parser::parse_call_expression(Expression* e)
  {
    Token lparen = require(Token::lparen);
    Expression_seq args = parse_argument_list();
    Token rparen = match(Token::rparen);
    return m_act.on_call_expression(e, args, lparen, rparen); 
  }

  /// argument-list:
  ///   argument-list ',' argument
  ///   argument
  Expression_seq
  Parser::parse_argument_list()
  {
    Expression_seq args;
    while (true) {
      Expression* arg = parse_expression();
      if (match_if(Token::comma))
        continue;
      if (lookahead() == Token::rparen)
        break;
    }
    return args;
  }

  /// argument:
  ///   expression
  Expression*
  Parser::parse_argument()
  {
    return parse_expression();
  }

  /// postfix-expression
  ///   postfix-expression '[' subscript-list ']'
  Expression*
  Parser::parse_subscript_expression(Expression* e)
  {
    Token lbracket = require(Token::lbracket);
    Expression_seq args = parse_subscript_list();
    Token rbracket = match(Token::rbracket);
    return m_act.on_call_expression(e, args, lbracket, rbracket); 
  }

  /// subscript-list:
  ///   subscript-list ',' subscript
  ///   subscript
  Expression_seq
  Parser::parse_subscript_list()
  {
    Expression_seq args;
    while (true) {
      Expression* arg = parse_expression();
      if (match_if(Token::comma))
        continue;
      if (lookahead() == Token::rbracket)
        break;
    }
    return args;
  }

  /// subscript:
  ///   expression
  Expression*
  Parser::parse_subscript()
  {
    return parse_expression();
  }

  /// primary-expression:
  ///   literal
  ///   id-expression
  ///   '(' expression ')'
  Expression*
  Parser::parse_primary_expression()
  {
    switch (lookahead()) {
    case Token::binary_integer:
    case Token::decimal_integer:
    case Token::hexadecimal_integer:
      return m_act.on_integer_literal(accept());
    
    case Token::unit_kw:
      return m_act.on_unit_literal(accept());

    case Token::true_kw:
    case Token::false_kw:
      return m_act.on_boolean_literal(accept());
    
    case Token::decimal_float:
    case Token::hexadecimal_float:
      return m_act.on_float_literal(accept());
    
    case Token::character:
      return m_act.on_character_literal(accept());

    case Token::string:
      return m_act.on_character_literal(accept());
    
    case Token::identifier:
      return parse_id_expression();
    
    case Token::lparen: {
      Token lparen = match(Token::lparen);
      Expression* e = parse_expression();
      Token rparen = match(Token::rparen);
      return m_act.on_paren_expression(e, lparen, rparen);
    }

    default:
      break;
    }
    
    throw std::runtime_error("expected primary-expression");
  }

  /// id-expression:
  ///   identifier
  ///
  /// \todo Allow unqualified and qualified ids, not just identifiers.
  Expression*
  Parser::parse_id_expression()
  {
    Token id = require(Token::identifier);
    return m_act.on_id_expression(id);
  }

  // ---------------------------------------------------------------------------//
  // Statements

  
  // ---------------------------------------------------------------------------//
  // Declarations

  /// declaration:
  ///   function-definition
  ///   variable-definition
  Declaration* 
  Parser::parse_declaration()
  {
    switch (lookahead()) {
    case Token::func_kw:
      return parse_function_definition();
    case Token::val_kw:
    case Token::var_kw:
      return parse_variable_definition();
    default:
      break;
    }
    throw std::runtime_error("expected declaration");
  }

  /// local-declaration:
  ///   variable-definition
  Declaration*
  Parser::parse_local_declaration()
  {
    switch (lookahead()) {
    case Token::val_kw:
    case Token::var_kw:
      return parse_variable_definition();
    default:
      break;
    }
    throw std::runtime_error("expected local-declaration");
  } 

  /// variable-definition:
  ///   variable-head identifier ':' type-specifier initializer ';'
  ///   variable-head identifier ':' type-specifier ';'
  ///   variable-head identifier initializer ';'
  ///
  /// variable-head:
  ///   'val' | 'var'
  Declaration*
  Parser::parse_variable_definition()
  {
    assert(lookahead() == Token::val_kw || lookahead() == Token::var_kw);
    Token kw = accept();

    // Match the declaration name.
    Token id = match(Token::identifier);
    
    // Match the optional type specifier.
    Type_specifier* type = nullptr;
    Token colon = match_if(Token::colon);
    if (colon)
      type = parse_type_specifier();

    // Point of declaration
    Declaration* decl;
    if (type)
      decl = m_act.on_variable_declaration(id, type, kw, colon);
    else
      decl = m_act.on_variable_declaration(id, kw);

    // Match the optional initializer.
    //
    // FIXME: If this is a top-level declaration.
    Initializer* init = nullptr;
    if (lookahead() != Token::semicolon)
      init = parse_initializer();
    
    Token semi = match(Token::semicolon);

    if (init)
      return m_act.on_variable_definition(decl, init, semi);
    else
      return m_act.on_variable_definition(decl, semi);
  }

  /// initializer:
  ///   '=' expression
  ///
  /// \todo Support paren and brace initialization.
  Initializer*
  Parser::parse_initializer()
  {
    Token eq = require(Token::equal);
    Expression *e = parse_expression();
    return m_act.on_equal_initializer(e, eq);
  }

  Declaration*
  Parser::parse_function_definition()
  {
    Token kw = require(Token::func_kw);

    // Match the function name.
    Token id = match(Token::identifier);

    // Match the parameter list.
    Token lparen = match(Token::lparen);
    Parameter_seq parms;
    if (lookahead() != Token::rparen)
      parms = parse_parameter_clause();
    Token rparen = match(Token::rparen);
    
    // FIXME: Make return type specifier option?
    Token arrow = match(Token::arrow);
    Type_specifier* type = parse_type_specifier();

    // Point of declaration.
    Declaration* d = m_act.on_function_declaration(
      id, parms, type, kw, lparen, rparen, arrow
    );

    // FIXME: We want a semantic inside the braces so that we can declare
    // parameters. Maybe parse this differently than block statement.
    Statement* s = parse_block_statement();
    
    // End of definition
    return m_act.on_function_definition(d, s);
  }

  /// parameter-clause -> parameter-list
  ///                   | parameter-list ',' ...
  ///
  /// \todo Support variadic parameters.
  Parameter_seq
  Parser::parse_parameter_clause()
  {
    Parameter_seq parms = parse_parameter_list();
    return parms;
  }


  /// parameter-list:
  ///    parameter-list ',' function-parameter
  ///    function-parameter
  ///
  /// For simplicity sake, we parse ellipses here and then generate semantic
  /// errors if they aren't the last parameter.
  Parameter_seq
  Parser::parse_parameter_list()
  {
    Parameter_seq parms;
    while (true) {
      Parameter* parm;
      if (lookahead() == Token::ellipsis)
        parm = parse_variadic_parameter();
      else
        parm = parse_function_parameter();
      parms.push_back(parm);
      
      if (match_if(Token::comma))
        continue;
      else
        break;
    }
    return parms;
  }

  /// function-parameter:
  ///   identifier ':' type-specifier
  ///
  /// \todo Allow for unnamed parameters
  /// \todo Support default arguments.
  Parameter*
  Parser::parse_function_parameter()
  {
    Token id = match(Token::identifier);
    Token colon = match(Token::colon);
    Type_specifier* t = parse_type_specifier();
    return m_act.on_function_parameter(id, t, colon);
  }

  Parameter*
  Parser::parse_variadic_parameter()
  {
    Token ell = require(Token::ellipsis);
    return m_act.on_variadic_parameter(ell);
  }

  Declaration_seq
  Parser::parse_declaration_seq()
  {
    Declaration_seq ds;
    while (peek()) {
      Declaration* d = parse_declaration();
      ds.push_back(d);
    }
    return ds;
  }

  Declaration*
  Parser::parse_program()
  {
    m_act.enter_global_scope();
    Declaration_seq decls = parse_declaration_seq();
    m_act.leave_scope();
    return m_act.on_program(decls);
  }
