/* Copyright (c) 2024 by Peter Gulutzan. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

/*
  The routines that start with "hparse_*" are a predictive recursive-descent
  recognizer for MySQL, generally assuming LL(1) grammar but allowing for a few quirks.
  (A recognizer does all the recursive-descent parser stuff except that it generates no tree.)
  Generally recursive-descent parsers or recognizers are reputed to be good
  because they're simple and can produce good -- often predictive -- error messages,
  but bad because they're huge and slow, and that's certainly the case here.
  The intent is to make highlight and hover look good.
  If any comparison fails, the error message will say:
  what we expected, token number + offset + value for the token where comparison failed.
  Allowed syntaxes are: MySQL_5.7, MariaDB 10.2, Tarantool (like SQLite), or an ocelotgui client statement.
*/

/*
  Todo: DEFAULT doesn't have to be followed by a literal, it can be an expression
  Todo: PERCENTILE_CONT and PERCENTILE_DISC can have WITHIN GROUP (ORDER BY ...)
  Todo: MEDIAN should only allow OVER (PARTITION ...) but not OVER (ORDER BY ...)
  Todo: More support of MariaDB 10.3 system versioning -- https://mariadb.com/kb/en/system-versioned-tables/
  Todo: other stuff in MariaDB 10.3.3 e.g. FOR ... END FOR loops
*/

/*
  Todo: Actually identifier length maximum isn't always 64.
  See http://dev.mysql.com/doc/refman/5.7/en/identifiers.html
*/
#define MYSQL_MAX_IDENTIFIER_LENGTH 64

/*
  Currently allow_flags only tries to detect parenthesized expressions
  with multiple operands inside (which are only allowed for comp-ops).
  And whether we're inside "on duplicate key".
  Todo: expand to check for whether subqueries are allowed
  (currently we depend on hparse_subquery_is_allowed)
  Todo: expand to check data type e.g. no string literal after << operator.
  Todo: expand to check whether declared variables are allowed.
  Todo: we allow "set @a = (1,2,3)" because we turn on allow_flag_is_multi
        when we see "(", but if we forbid that then we will be forbidding
        "set @a = (1,2,3) = (4,5,6)" which ought to be legal.
*/
#define ALLOW_FLAG_IS_MULTI 1
#define ALLOW_FLAG_IS_IN_DUPLICATE_KEY 2

/* conflict clauses are temporarily disabled */
//#define ALLOW_CONFLICT_CLAUSES 0

// create virtual is disabled, probably permanently
//#define ALLOW_CREATE_VIRTUAL

void MainWindow::hparse_f_nexttoken()
{
  if (hparse_errno > 0) return;
  for (;;)
  {
    ++hparse_i;
    hparse_token_type= main_token_types[hparse_i];
    if ((hparse_token_type == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE)
     && (hparse_sql_mode_ansi_quotes == true))
    {
      hparse_token_type= main_token_types[hparse_i]= TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE;
    }
    else if ((hparse_token_type == TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE)
     && (hparse_sql_mode_ansi_quotes == false))
    {
      hparse_token_type= main_token_types[hparse_i]= TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE;
    }
    if ((hparse_token_type != TOKEN_TYPE_COMMENT_WITH_SLASH)
     && (hparse_token_type != TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
     && (hparse_token_type != TOKEN_TYPE_COMMENT_WITH_MINUS))
      break;
  }
  hparse_prev_token= hparse_token;
  hparse_token= hparse_text_copy.mid(main_token_offsets[hparse_i], main_token_lengths[hparse_i]);
}

/*
  Lookahead.
  Call this if you want to know what the next symbol is, but don't want to get it.
  This is used in only five places, to see whether ":" follows (which could indicate a label),
  and to see whether next is "." and next_next is "*" (as in a select-list),
  and to see whether NOT is the beginning of NOT LIKE,
  and to see whether the word following GRANT ROLE is TO,
  and to see whether the word following DATE|TIME|TIMESTAMP is a literal.
  Todo: no conversion for identifier|literal with double quotes,
        as is done for hparse_f_nexttoken(), maybe it doesn't matter.
*/
void MainWindow::hparse_f_next_nexttoken()
{
  hparse_next_token= hparse_next_next_token= "";
  hparse_next_next_next_token= "";
  hparse_next_next_next_next_token= "";
  int saved_hparse_i= hparse_i;
  int saved_hparse_token_type= hparse_token_type;
  QString saved_hparse_token= hparse_token;
  if (main_token_lengths[hparse_i] != 0)
  {
    hparse_f_nexttoken();
    hparse_next_token= hparse_token;
    hparse_next_token_type= hparse_token_type;
    if (main_token_lengths[hparse_i] != 0)
    {
      hparse_f_nexttoken();
      hparse_next_next_token= hparse_token;
      hparse_next_next_token_type= hparse_token_type;
    }
    if (main_token_lengths[hparse_i] != 0)
    {
      hparse_f_nexttoken();
      hparse_next_next_next_token= hparse_token;
      hparse_next_next_next_token_type= hparse_token_type;
    }
    if (main_token_lengths[hparse_i] != 0)
    {
      hparse_f_nexttoken();
      hparse_next_next_next_next_token= hparse_token;
      hparse_next_next_next_next_token_type= hparse_token_type;
    }
  }
  hparse_i= saved_hparse_i;
  hparse_token_type= saved_hparse_token_type;
  hparse_token= saved_hparse_token;
}

void MainWindow::hparse_f_error()
{
  if (hparse_errno > 0) return;
  assert(hparse_i >= 0);
  assert(hparse_i < (int)main_token_max_count);
  main_token_flags[hparse_i] |= TOKEN_FLAG_IS_ERROR;
  QString q_errormsg= "The latest token is: ";
  if (hparse_token.length() > 40)
  {
    q_errormsg.append(hparse_token.left(40));
    q_errormsg.append("...");
  }
  else q_errormsg.append(hparse_token);
  q_errormsg.append("  (token #");
  q_errormsg.append(QString::number(hparse_i + 1));
  q_errormsg.append(", offset ");
  q_errormsg.append(QString::number(main_token_offsets[hparse_i] + 1));
  q_errormsg.append(") ");
  q_errormsg.append(". The list of expected tokens is: ");
  q_errormsg.append(hparse_expected);
  /* Todo: this looks a bit odd. why a loop rather than a single assignment? */
  while ((unsigned) q_errormsg.toUtf8().length() >= (unsigned int) sizeof(hparse_errmsg) - 1)
    q_errormsg= q_errormsg.left(q_errormsg.length() - 1);
  assert(q_errormsg.length() < MAX_HPARSE_ERRMSG_LENGTH);
  strcpy(hparse_errmsg, q_errormsg.toUtf8());
  hparse_errno= 10400;
}

/*
  Merely saying "if (hparse_token == 'x') ..." till we saw delimiter usually is not =.
  Warning: We can call this without going via hparse_f_multi_block()
*/
bool MainWindow::hparse_f_is_equal(QString hparse_token_copy, QString token)
{
  if (hparse_token_copy == hparse_delimiter_str) return false;
  if (hparse_token_copy == token) return true;
  return false;
}

/*
  Tarantool only -- after WITH, and within CREATE TRIGGER,
  only certain verbs are legal.
*/
bool MainWindow::hparse_f_is_special_verb(int outer_verb)
{
  QString s= hparse_token.toUpper();
  if ((s== "DELETE") || (s == "INSERT") || (s == "REPLACE")
   || (s== "SELECT") || (s == "UPDATE") || (s == "VALUES"))
    return true;
  if ((outer_verb == TOKEN_KEYWORD_TRIGGER) && (s == "WITH")) return true;
  hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DELETE, "DELETE");
  hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INSERT, "INSERT");
  hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REPLACE, "REPLACE");
  hparse_f_accept(FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_2_2, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SELECT, "SELECT");
  hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_UPDATE, "UPDATE");
  hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_VALUES, "VALUES");
  if (outer_verb == TOKEN_KEYWORD_TRIGGER)
    hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WITH, "WITH");
  hparse_f_error();
  return false;
}

/*
  accept means: if current == expected then clear list of what was expected, get next, and return 1,
                else add to list of what was expected, and return 0
*/
int MainWindow::hparse_f_accept(unsigned int flag_version, unsigned char reftype, int proposed_type, QString token)
{
  if (hparse_errno > 0) return 0;
  if ((hparse_dbms_mask & flag_version) == 0) return 0;
  bool equality= false;
  if (token == "[eof]")
  {
    if (hparse_token.length() == 0)
    {
      equality= true;
    }
  }
  else if ((hparse_token == hparse_delimiter_str) && (hparse_delimiter_str != ";"))
  {
    if ((hparse_token == token) && (proposed_type == TOKEN_TYPE_DELIMITER)) equality= true;
    else equality= false;
  }
  else if (hparse_text_copy.mid(main_token_offsets[hparse_i], 2).toUpper() == "\\G")
  {
    /* \G and \g can act somewhat like delimiters. No change to hparse_expected list. */
    if (proposed_type == TOKEN_TYPE_DELIMITER)
    {
      //main_token_types[hparse_i]= proposed_type;
      //main_token_types[hparse_i + 1]= proposed_type;
      hparse_expected= "";
      completer_widget->clear_wrapper();
      hparse_f_nexttoken();
      hparse_i_of_last_accepted= hparse_i;
      hparse_f_nexttoken();
      ++hparse_count_of_accepts;
      return 1;
    }
    return 0;
  }
  else if (token == "[identifier]")
  {
    /* todo: stop checking if it's "[identifier]" when reftype is always passed. */
    if ((hparse_dbms_mask & FLAG_VERSION_LUA) != 0)
    {
      if ((main_token_flags[hparse_i] & TOKEN_FLAG_IS_MAYBE_LUA) != 0)
      {
        main_token_flags[hparse_i] |= TOKEN_FLAG_IS_RESERVED;
      }
      else
      {
        main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_RESERVED);
        if ((hparse_token_type >= TOKEN_TYPE_OTHER)
         || (hparse_token_type == TOKEN_TYPE_IDENTIFIER)) /* specific, so don't use hparse_f_is_identifier() */
          equality= true;
      }
    }
    else
    {
      if (hparse_token_type == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK)
      {
        if ((hparse_token.size() == 1) || (hparse_token.right(1) != "`"))
        {
          /* Starts with ` but doesn't end with ` so identifier required but not there yet. */
          int ret= hparse_f_expected_exact(reftype);
          if (ret == 1) equality= true;
          if (ret == 2) return 0;
        }
        else equality= true; /* starts and ends with ` */
      }
      if (hparse_token_type == TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE)
      {
        if ((hparse_token.size() == 1) || (hparse_token.right(1) != "\""))
        {
          /* Starts with " but doesn't end with " so identifier required but not there yet. */
          int ret= hparse_f_expected_exact(reftype);
          if (ret == 1) equality= true;
          if (ret == 2) return 0;
        }
        else equality= true; /* starts and ends with " */
      }
      // if ((hparse_token_type == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK)
      // || (hparse_token_type == TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE)
      if ((hparse_token_type == TOKEN_TYPE_IDENTIFIER_WITH_AT)
       || ((hparse_token_type >= TOKEN_TYPE_OTHER)
        && ((main_token_flags[hparse_i] & TOKEN_FLAG_IS_RESERVED) == 0)))
      {
        if (hparse_f_expected_exact(reftype) > 0) equality= true;
      }
    }
  }
  else if (token == "[literal]")
  {
    if (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
    {
      if ((hparse_token.size() == 1) || (hparse_token.right(1) != "'"))
      {
        /* Starts with ' but doesn't end with ' so literal required but not there yet. */
        hparse_expected= token;
        return 0;
      }
    }
    if (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE)
    {
      if ((hparse_token.size() == 1) || (hparse_token.right(1) != "\""))
      {
        /* Starts with ' but doesn't end with " so literal required but not there yet. */
        hparse_expected= token;
        return 0;
      }
    }
    if (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_BRACKET)
    {
      if (hparse_token.right(2) != "]]")
      {
        /* Starts with [[ but doesn't end with ]] so literal required but not there yet. */
        hparse_expected= token;
        return 0;
      }
    }
    if ((hparse_token_type == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
     || (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE)
     || (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_DIGIT)
     || (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_BRACKET)
     /* literal_with_brace == literal */
     || (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_BRACE)) /* obsolete? */
    {
      equality= true;
    }
  }
  else if (token == "[introducer]")
  {
    if ((hparse_token_type >= TOKEN_KEYWORD__ARMSCII8)
     && (hparse_token_type <= TOKEN_KEYWORD__UTF8MB4))
    {
      equality= true;
    }
  }
  else if (token == "[reserved function]")
  {
    if (((main_token_flags[hparse_i] & TOKEN_FLAG_IS_RESERVED) != 0)
     && ((main_token_flags[hparse_i] & TOKEN_FLAG_IS_FUNCTION) != 0))
      equality= true;
  }
#ifdef DBMS_TARANTOOL
  else if (token == "[field identifier]")
  {
    int base_size= strlen(TARANTOOL_FIELD_NAME_BASE);
    bool ok= false;
    int field_integer= 0;
    int field_integer_length= hparse_token.length() - (base_size + 1);
    if (field_integer_length > 0) field_integer= hparse_token.right(field_integer_length).toInt(&ok);
    if ((hparse_token.left(base_size) == TARANTOOL_FIELD_NAME_BASE)
     && (hparse_token.mid(base_size, 1) == "_")
     && (field_integer > 0)
     && (ok == true)
     && (hparse_token.length() < TARANTOOL_MAX_FIELD_NAME_LENGTH))
    {
      equality= true;
    }
  }
#endif
  else
  {
    if ((hparse_dbms_mask & FLAG_VERSION_LUA) != 0)
    {
      if (QString::compare(hparse_token, token, Qt::CaseSensitive) == 0)
      {
        equality= true;
      }
    }
    else if (QString::compare(hparse_token, token, Qt::CaseInsensitive) == 0)
    {
      equality= true;
    }
  }
  if (equality == true)
  {
    /*
      Change the token type now that we're sure what it is.
      But for keyword: if it's already more specific, leave it.
      TODO: that exception no longer works because we moved TOKEN_TYPE_KEYWORD to the end
      But for literal: if it's already more specific, leave it
    */
    if ((proposed_type == TOKEN_TYPE_KEYWORD)
     && (main_token_types[hparse_i] >= TOKEN_KEYWORDS_START)) {;}
    else if ((proposed_type == TOKEN_TYPE_LITERAL)
     && (main_token_types[hparse_i] < TOKEN_TYPE_LITERAL)) {;}
    else
    {
      /* See comment preceding hparse_f_is_identifier */
      if (hparse_f_is_identifier(main_token_types[hparse_i]) == false) main_token_types[hparse_i]= proposed_type;
    }
    main_token_reftypes[hparse_i]= reftype;
    hparse_f_expected_clear();
    hparse_i_of_last_accepted= hparse_i;
    hparse_f_nexttoken();
    ++hparse_count_of_accepts;
    return 1;
  }
  hparse_f_expected_append(token, reftype, proposed_type);
  return 0;
}

///*
//  I had a theory that using  hparse_f_accept_key(TOKEN_KEYWORD_PAD)
//  rather than hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PAD, "PAD")
//  would save a bit of space. In a quick test, it didn't. But test again someday.
//*/
//int MainWindow::hparse_f_accept_key(int key)
//{
//  return hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,key, QString(strvalues[key].chars));
//}


/*
  We tracked "what is expected" with global QString hparse_expected and with a  private QListWidget in class completer_widget.
  * hparse_expect = the possible error message if syntax checker == 3,
    just keywords and [identifiers] separated by " or ", all possibilities even if something has been typed
     ... we wwill call token_reftype so it's more specific about the possibilities
  * completer_widget =   the autocomplete helper, [identifiers] established if REHASH happened,
    wipe out entries if something has been typed and won't match
  * when entering, clear them with hparse_f_expected_initialize()
  * When accept succeeds i.e. equality == true, clear them with hparse_f_expected_clear()
  * When accept fails and accept() is about to return 0, add to them with hparse_f_expected_append().
  todo: maybe I should call hparse_f_token_to_appendee for completer_widget too
  todo: there are other places where we might append a token
*/

/*
  Removed 2024-07-23. Duplicated by hparse_f_expected_clear() and nobody calls it now.
void MainWindow::hparse_f_expected_initialize()
{
  hparse_expected= "";
  completer_widget->clear_wrapper();
}
*/

void MainWindow::hparse_f_expected_clear()
{
  hparse_expected= "";
  completer_widget->clear_wrapper();
}

/*
  If there was no REHASH, then anything that looks like a valid identifier is considered equal, return true.
  If there was a REHASH, then return true iff the string, not just partial string, is found by rehash_search().
  This can result in false if rehash_scan() was not done recently, so we add:
  even if it is not found, if there is something ahead i.e. user kept typing anyway, regard it as true.
  This means we can call rehash_search twice -- once for expected_exact, once for expected_append.
  Todo: there are various places where we do QString-to-char* conversions that are less neat than this.
  Todo: if hparse_f_is_rehash_searchable() == false, shouldn't we check anyway for missing " or `?
  Return 0 = false, 1 = true, 2 = true but missing terminating " or `
*/
int MainWindow::hparse_f_expected_exact(int reftype)
{
  if (hparse_f_is_rehash_searchable(reftype) == false) return 1;
  if (main_token_lengths[hparse_i + 1] != 0) return 1;
  QByteArray ba= hparse_token.toUtf8();
  char *tmp= ba.data();
  QString rehash_search_result= rehash_search(hparse_f_pre_rehash_search(reftype), tmp, reftype,
                                              hparse_token,
                                              true,
                                              hparse_specified_schema,
                                              hparse_specified_list);
  if (rehash_search_result > "")
  {
    if ((hparse_token.left(1) == "\"") && (hparse_token.right(1) != "\""))
    {
      hparse_f_expected_append_endquote("\"");
      return 2;
    }
    if ((hparse_token.left(1) == "`") && (hparse_token.right(1) != "`"))
    {
      hparse_f_expected_append_endquote("`");
      return 2;
    }
    return 1;
  }
  return 0;
}

/*
  But do not append ; if prev was ; -- a kludge of a kludge, as in hparse_f_create_package() .
*/
void MainWindow::hparse_f_expected_append(QString token, unsigned char reftype, int proposed_type)
{
  if (hparse_expected > "") hparse_expected.append(" or ");
  hparse_expected.append(hparse_f_token_to_appendee(token, hparse_i, reftype));
  if (token == ";")
  {
    if ((hparse_i > 0) && (main_token_lengths[hparse_i - 1] == 1) && (main_token_types[hparse_i - 1] == TOKEN_TYPE_OPERATOR))
    {
      if (hparse_text_copy.mid(main_token_offsets[hparse_i - 1], main_token_lengths[hparse_i - 1]) == ";") return;
    }
  }
  if (hparse_f_is_rehash_searchable(reftype) == true)
  {
    if (token.contains("identifier]") == true)
    {
      QString rehash_search_result= rehash_search(hparse_f_pre_rehash_search(reftype), (char*)"", reftype,
                                                  hparse_token,
                                                  false,
                                                  hparse_specified_schema,
                                                  hparse_specified_list);
    }
  }
//    {
//      QString s;
//      char search_string[512];
//      int search_string_len;
//      s= text.mid(main_token_offsets[hparse_i], main_token_lengths[hparse_i]);
//      if (s.left(1) == "`") s= s.right(s.size() - 1);
//      else if (s.left(1) == "\"") s= s.right(s.size() - 1);
//      search_string_len= s.toUtf8().size();
//      memcpy(search_string, s.toUtf8().constData(), search_string_len);
//      search_string[search_string_len]= '\0';
//      QString rehash_search_result= rehash_search(hparse_f_pre_rehash_search(), search_string, main_token_reftypes[hparse_i]);
//      if (rehash_search_result > "")
//      {
//        expected_list= "Expecting: ";
//        expected_list.append(rehash_search_result);
//        unfinished_identifier_seen= true;
//      }
//    }
//  }
  hparse_f_variables_append(hparse_i, hparse_text_copy, reftype); /* this is in ocelotgui.cpp */

  /* If user has typed DROP TABLE " then possibilities should not include IF */
  if ((hparse_token_type== TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK) || (hparse_token_type == TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE))
  {
    if (token != "[identifier]") return;
  }
  /* This was passing main_token_types[hparse_i], which would be the type of the unfinished input */
  if (proposed_type == TOKEN_TYPE_OPERATOR)   completer_widget->append_wrapper(token, hparse_token, proposed_type, main_token_flags[hparse_i], "O");
  else completer_widget->append_wrapper(token, hparse_token, proposed_type, main_token_flags[hparse_i], "K");
}

/*
  E.g. call this after DROP table "x -- because we should finish the identifier so it's DROP TABLE "x"
  An endquote is " or ` but maybe somewhere it's useful to call this for '
  Todo: really the type is not "K", we would like the tooltip to say "O" operator, or better still "terminating quote"
*/
void MainWindow::hparse_f_expected_append_endquote(QString token)
{
  if (hparse_expected > "") hparse_expected.append(" or ");
  hparse_expected= token;
  completer_widget->clear_wrapper();
  completer_widget->append_wrapper(token, "", TOKEN_TYPE_OPERATOR, TOKEN_REFTYPE_ANY, "K");
}

/*
  Replace [identifier] with something more specific.
  Todo: there are some problems with what-is-an-identifier calculation:
  TOKEN_REFTYPE_ANY can be for @ within a user
  TOKEN_REFTYPE_HOST and TOKEN_REFTYPE_USER can be for literals
  "*" can be TOKEN_REFTYPE_DATABASE and TOKEN_TYPE_IDENTIFIER
*/
QString MainWindow::hparse_f_token_to_appendee(QString token, int i, char reftype)
{
  QString appendee= token;
  if (token != "[identifier]") return appendee;
  return token_reftype(i, false, TOKEN_TYPE_IDENTIFIER, reftype);
}

/* A variant of hparse_f_accept for debugger keywords which can be shortened to n letters */
/* TODO: are you checking properly for eof or ; ??? */
int MainWindow::hparse_f_acceptn(int proposed_type, QString token, int n)
{
  if (hparse_errno > 0) return 0;
  QString token_to_compare;
  int len= hparse_token.length();
  if ((len >= n) && (len < token.length())) token_to_compare= token.left(len);
  else token_to_compare= token;
  if (QString::compare(hparse_token, token_to_compare, Qt::CaseInsensitive) == 0)
  {
    main_token_types[hparse_i]= proposed_type;
    hparse_f_expected_clear();
    hparse_f_nexttoken();
    return 1;
  }
  hparse_f_expected_append(token, 0, TOKEN_TYPE_KEYWORD);
  return 0;
}

/* A variant of hparse_f_accept for statement_format_rule words e.g. [keyword] [operator] */
/* TODO: are you checking properly for eof or ; ??? */
/* We won't match , or ; so there will have to be some other way to specify them. */
/* We won't match comments. */
int MainWindow::hparse_f_acceptf(int pass_number, QString replacee)
{
  if (hparse_errno > 0) return 0;
  bool is_accepted= false;
  for (int i= 0; i < 5; ++i)
  {
    QString k;
    if (i == 0) k= "COMMENT";
    if (i == 1) k= "IDENTIFIER";
    if (i == 2) k= "KEYWORD";
    if (i == 3) k= "LITERAL";
    if (i == 4) k= "OPERATOR";
    if ((pass_number == 1) || (k == replacee))
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, k) == 1)
      {
        is_accepted= true;
        break;
      }
    }
  }
  if (is_accepted == false)
  {
    int i= main_token_types[hparse_i];
    if ((hparse_token != ",") && (hparse_token != ";"))
    {
       if (((i >= TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK) && (i <= TOKEN_TYPE_IDENTIFIER_WITH_AT))
       || (i >= TOKEN_KEYWORDS_START)
       || ((i >= TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE) && (i <= TOKEN_TYPE_LITERAL_WITH_BRACE))
       || (i == TOKEN_TYPE_OPERATOR))
      {
        hparse_f_expected_clear();
        hparse_f_nexttoken();
        is_accepted= true;
      }
    }
  }
  if (is_accepted == true)
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "-") == 1)
      {
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOWER") == 0)
          hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UPPER");
        if (hparse_errno > 0) return 0;
      }
    return 1;
  }
  /* guaranteed to fail */
  hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "[identifier]");
  hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "[keyword]]");
  hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "[literal]");
  hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "[operator]");
  return 0;
}

/*
  A variant of hparse_f_accept during SET target=value [, ...] so we skip target if we've already done it.
  Todo: this is okay for simple situations but wouldn't work for ... SET a=(x=y), x=z.
  The idea is that the caller will remove the matching item for tokens.
  An alternative is to search backward as far as the SET and not try to accept if it's seen before.
  Actually we no longer call this for SET, but we do call for REQUIRE.
*/
int MainWindow::hparse_f_accept_in_set(unsigned int flag_version, QStringList tokens, int *i_of_matched)
{
  for (int i= 0; i < tokens.size(); ++i)
  {
    QString token= tokens.at(i);
    if (hparse_f_accept(flag_version, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, token) == 1)
    {
      *i_of_matched= i;
      return 1;
    }
  }
  return 0;
}

/*
  A variant of hparse_f_accept for the word "SLAVE" or "REPLICA" e.g. for START|STOP REPLICA|SLAVE
  In MySQL 8.4 only REPLICA is okay. In MariaDB or before MySQL 8.4 SLAVE is okay too.
  Often flag_version == FLAG_VERSION_MYSQL_8_0 | FLAG_VERSION_MARIADB_10_7, and for "REPLICAS" it must be
  Same goes for  "SLAVES" or "REPLICAS"
*/
int MainWindow::hparse_f_accept_slave_or_replica(unsigned int flag_version)
{
  if ((hparse_dbms_mask & flag_version) == 0) return 0;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0 | FLAG_VERSION_MARIADB_10_7,TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REPLICA, "REPLICA") == 1) return 1;
  if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_4) != 0) return 0;
  return hparse_f_accept(flag_version,TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SLAVE");
}
int MainWindow::hparse_f_accept_slaves_or_replicas(unsigned int flag_version)
{
  if ((hparse_dbms_mask & flag_version) == 0) return 0;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0 | FLAG_VERSION_MARIADB_10_7,TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REPLICAS, "REPLICAS") == 1) return 1;
  if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_4) != 0) return 0;
  return hparse_f_accept(flag_version,TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SLAVES");
}

/* expect means: if current == expected then get next and return 1; else error */
int MainWindow::hparse_f_expect(unsigned int flag_version, unsigned char reftype,int proposed_type, QString token)
{
  if (hparse_errno > 0) return 0;
  if ((hparse_dbms_mask & flag_version) == 0) return 0;
  if (hparse_f_accept(flag_version, reftype,proposed_type, token)) return 1;
  hparse_f_error();
  return 0;
}

/* [literal] or _introducer [literal], return 1 if true */
/* todo: this is also accepting {ODBC junk} or NULL, sometimes when it shouldn't. */
/* todo: in fact it's far far too lax, you should pass what's acceptable data type */
/* todo: user mode should affect whether you accept "..." as a literal */
/*
   Warning; we had a trick, that if reftype ==
   TOKEN_REFTYPE_USER | HOST | CHARACTER_SET | COLLATION |ALIAS,
   we would pass TOKEN_TYPE_IDENTIFIER, "[literal]" instead of
   TOKEN_TYPE_LITERAL, "[literal]" to hparse_f_accept().
   (So that hovering would show them as identifiers not literals.)
   But that meant that if ocelot_statement_syntax_checker=3 then
   hparse_f_accept() would change the type on the first pass, and
   then fail on the second pass because the type was no longer literal.
   Hovering should accept that it's literal and look harder at reftype?
*/
int MainWindow::hparse_f_literal(unsigned char reftype, unsigned int flag_version,int token_literal_flags)
{
  if (hparse_errno > 0) return 0;
  if ((hparse_dbms_mask & flag_version) == 0) return 0;
  if (token_literal_flags & TOKEN_LITERAL_FLAG_INTRODUCER)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_INTRODUCER,TOKEN_TYPE_KEYWORD, "[introducer]") == 1)
    {
      if (hparse_f_literal(reftype, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      return 1;
    }
  }
  if (token_literal_flags & TOKEN_LITERAL_FLAG_ODBC)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "{") == 1)
    {
      /* I can't imagine how {oj ...} could be valid if we're looking for a literal */
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "D") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "T") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TS") == 1))
      {
        if (hparse_f_literal(reftype, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();

        if (hparse_errno > 0) return 0;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "}");
        if (hparse_errno > 0) return 0;
        return 1;
      }
      else hparse_f_error();
      return 0;
    }
  }
  if (token_literal_flags & TOKEN_LITERAL_FLAG_MAP)
  {
    if (hparse_f_accept(FLAG_VERSION_TARANTOOL_2_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "{") == 1)
    {
      hparse_f_opr_1(0, 0); /* hmm, since {} contains expressions it's not really a literal, eh? */
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_TARANTOOL_2_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ":");
      if (hparse_errno > 0) return 0;
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_TARANTOOL_2_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "}");
      if (hparse_errno > 0) return 0;
      return 1;
    }
  }
  if (token_literal_flags & TOKEN_LITERAL_FLAG_CONSTANT)
  {
    if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_NULL, "NULL") == 1)
         || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TRUE, "TRUE") == 1)
         || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FALSE, "FALSE") == 1)
         || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_UNKNOWN, "UNKNOWN") == 1))
    {
      return 1;
    }
  }
  if (token_literal_flags & TOKEN_LITERAL_FLAG_DATE)
  {
    /* DATE '...' | TIME '...' | TIMESTAMP '...' are literals, but DATE|TIME|TIMESTAMP are not. */
    QString hpu= hparse_token.toUpper();
    if ((hpu == "DATE") || (hpu == "TIME") || (hpu == "TIMESTAMP"))
    {
      hparse_f_next_nexttoken();
      if ((hparse_next_token.mid(0,1) == "\"") || (hparse_next_token.mid(0,1) == "'"))
      {
        if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATE") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TIME") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TIMESTAMP") == 1))
        {
          if (hparse_f_literal(reftype, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
          if (hparse_errno > 0) return 0;
          return 1;
        }
      }
    }
  }
  /* Allow for -integer, but not +integer. */
  if (token_literal_flags & TOKEN_LITERAL_FLAG_SIGNED_INTEGER)
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "-") == 1)
    {
      if (hparse_f_literal(reftype, flag_version, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      return 1;
    }
  }

  bool possible= false;
  if (token_literal_flags & TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER)
  {
    if (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_DIGIT)
    {
      if ((hparse_token.contains("E",Qt::CaseInsensitive) == false)
       && (hparse_token.contains(".") == false))
        possible= true;
    }
  }
  if (token_literal_flags & TOKEN_LITERAL_FLAG_SIGNED_INTEGER)
  {
    if (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_DIGIT)
    {
      if ((hparse_token.contains("E",Qt::CaseInsensitive) == false)
       && (hparse_token.contains(".") == false))
        possible= true;
    }
  }
  if (token_literal_flags & TOKEN_LITERAL_FLAG_FLOAT)
  {
    if (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_DIGIT)
    {
      possible= true;
    }
  }
  if (token_literal_flags & (TOKEN_LITERAL_FLAG_STRING | TOKEN_LITERAL_FLAG_USER_STRING | TOKEN_LITERAL_FLAG_HOST_STRING))
  {
    if ((hparse_token_type == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
     || (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE))
    {
      possible= true;
    }
    if ((hparse_token_type == TOKEN_TYPE_LITERAL_WITH_DIGIT)
     && (hparse_token.left(2) == "0x"))
    {
      possible= true;
    }
  }
  if ((token_literal_flags & TOKEN_LITERAL_FLAG_ID_STRING) || (token_literal_flags & TOKEN_LITERAL_FLAG_LIBRARY_STRING)
   || (token_literal_flags & TOKEN_LITERAL_FLAG_MENU_TITLE_STRING)
   || (token_literal_flags & TOKEN_LITERAL_FLAG_MENU_ITEM_STRING) || (token_literal_flags & TOKEN_LITERAL_FLAG_ACTION_STRING)
   || (token_literal_flags & TOKEN_LITERAL_FLAG_KEYSEQUENCE_STRING))
  {
    if (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
      possible= true;
  }
  if (possible)
  {
    if (hparse_f_accept(flag_version, reftype, TOKEN_TYPE_LITERAL, "[literal]") == 1) return 1;
  }
  else
  {
    /* guaranteed to fail */
    char expectation[128];
    if ((token_literal_flags & TOKEN_LITERAL_FLAG_INTEGER)
     && ((token_literal_flags & TOKEN_LITERAL_FLAG_STRING) == 0)
     && ((token_literal_flags & TOKEN_LITERAL_FLAG_FLOAT) == 0))
      strcpy(expectation, "[integer]");
    else if ((token_literal_flags & TOKEN_LITERAL_FLAG_NUMBER)
     && (token_literal_flags & TOKEN_LITERAL_FLAG_STRING))
      strcpy(expectation, "[number/string]");
    else if (token_literal_flags & TOKEN_LITERAL_FLAG_NUMBER)
      strcpy(expectation, "[number]");
    else if (token_literal_flags & TOKEN_LITERAL_FLAG_STRING)
      strcpy(expectation, "[string]");
    else if (token_literal_flags & TOKEN_LITERAL_FLAG_USER_STRING)
      strcpy(expectation, "[user-string]");
    else if (token_literal_flags & TOKEN_LITERAL_FLAG_HOST_STRING)
      strcpy(expectation, "[host-string]");
    else if (token_literal_flags & TOKEN_LITERAL_FLAG_ID_STRING)
      strcpy(expectation, "[id-string]");
    else if (token_literal_flags & TOKEN_LITERAL_FLAG_LIBRARY_STRING)
      strcpy(expectation, "[library-string]");
    else if (token_literal_flags & TOKEN_LITERAL_FLAG_MENU_TITLE_STRING)
      strcpy(expectation, "[menu_title-string]");
    else if (token_literal_flags & TOKEN_LITERAL_FLAG_MENU_ITEM_STRING)
      strcpy(expectation, "[menu_item-string]");
    else if (token_literal_flags & TOKEN_LITERAL_FLAG_ACTION_STRING)
      strcpy(expectation, "[action-string]");
    else if (token_literal_flags & TOKEN_LITERAL_FLAG_KEYSEQUENCE_STRING)
      strcpy(expectation, "[keysequence-string]");
    else
      strcpy(expectation, "[non-number/non-string]");
    if (hparse_f_accept(flag_version, reftype, TOKEN_TYPE_LITERAL, expectation) == 1) return 1;
  }
  return 0;
}

/*
  DEFAULT is a reserved word which, as an operand, might be
  () the right side of an assignment for INSERT/REPLACE/UPDATE
  () the beginning of DEFAULT(col_name)
  () the right side of an assignment for SET server-variable = DEFAULT.
  Not to be confused with hparse_f_default_clause.
*/
int MainWindow::hparse_f_default(int who_is_calling, bool server_variable_seen)
{
  if (who_is_calling == TOKEN_KEYWORD_SET)
  {
    if (server_variable_seen)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DEFAULT, "DEFAULT") == 1) return 1;
    }
    return 0;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DEFAULT, "DEFAULT") == 1)
  {
    bool parenthesis_seen= false;
    if ((who_is_calling == TOKEN_KEYWORD_INSERT)
     || (who_is_calling == TOKEN_KEYWORD_UPDATE)
     || (who_is_calling == TOKEN_KEYWORD_REPLACE))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1) parenthesis_seen= true;
    }
    else
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return 0;
      parenthesis_seen= true;
    }
    if (parenthesis_seen == true)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return 0;
    }
    return 1;
  }
  return 0;
}

/*
  MySQL + MariaDB both allow user names like 'a'@'b' or 'a' or a.
  MySQL 8.0 allows role names the same way, MariaDB only allows 'a' or a.
  token_reftype = TOKEN_REFTYPE_USER or (MySQL 8.0) TOKEN_REFTYPE_ROLE
  Beware: we treat @ as a separator so 'a' @ 'b' is a user name.
  MySQL doesn't expect spaces. But I'm thinking it won't cause ambiguity.
  Todo: Something bizarre happens when I enter "a"@" (it doesn't say it expects more)
        or "a"` ... AHA, it thinks @ is the start of an identifier,
        but doesn't look for the closing ".
*/
int MainWindow::hparse_f_user_or_role_name(int token_reftype)
{
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, token_reftype,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
   || (hparse_f_literal(token_reftype, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_USER_STRING) == 1))
  {
    if ((token_reftype == TOKEN_REFTYPE_ROLE)
     && ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0)) return 1;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "@") == 1)
    {
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_HOST,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
       || (hparse_f_literal(TOKEN_REFTYPE_HOST, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_HOST_STRING) == 1)) {;}
    }
    else if ((hparse_token.mid(0, 1) == "@")
          && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_HOST,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1))
    {
      ;
    }
    return 1;
  }
  if (token_reftype == TOKEN_REFTYPE_USER)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURRENT_USER") == 1)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return 0;
      }
      return 1;
    }
  }
  return 0;
}

/*
  It's possible for a user to define a new character set, but
  we only check the official names. An undocumented "feature"
  is that users can pass a string literal, we won't check it.
*/
int MainWindow::hparse_f_character_set_name()
{
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "ARMSCII8") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "ASCII") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "BIG5") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "BINARY") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "CP1250") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "CP1251") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "CP1256") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "CP1257") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "CP850") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "CP852") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "CP866") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "CP932") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "DEC8") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "EUCJPMS") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "EUCKR") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "FILENAME") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "GB2312") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "GBK") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "GEOSTD8") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "GREEK") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "HEBREW") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "HP8") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "KEYBCS2") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "KOI8R") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "KOI8U") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "LATIN1") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "LATIN2") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "LATIN5") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "LATIN7") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "MACCE") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "MACROMAN") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "SJIS") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "SWE7") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "TIS620") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "UCS2") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "UJIS") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "UTF16") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "UTF16LE") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "UTF32") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "UTF8") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "UTF8MB3") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "UTF8MB4") == 1))
    return 1;
  if (hparse_f_literal(TOKEN_REFTYPE_CHARACTER_SET, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 1) return 1;
  return 0;
}

/* Todo: someday check collation names the way we check character set names. */
int MainWindow::hparse_f_collation_name()
{
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_COLLATION,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) return 1;
//  if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
//  {
//    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_COLLATION,TOKEN_TYPE_KEYWORD, "BINARY") == 1) return 1;
//  }
  if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0)
  {
    if (hparse_f_literal(TOKEN_REFTYPE_COLLATION, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 1) return 1;
  }
  return 0;
}

/*
  Routines starting with hparse_f_table... are based on
  https://dev.mysql.com/doc/refman/5.5/en/join.html
*/

/*
  In the following explanatory lists, ID means IDENTIFIER,
  BLANK means end-of-input, OTHER means non-blank-non-dot.
  qualified_name_of_object:
    "." ID                . object (MySQL/MariaDB tables only)
    ID "." ID             database . object
    ID "." BLANK          database . expected-object
    ID BLANK              database|object
    ID OTHER              object
  qualified_name_of_column: see hparse_f_qualified_name_of_operand(0, )
  qualified_name_of_star:
    "*"                   column
    ID "." "*"            table . column
    ID "." ID "." "*"     database . table . column
*/

/*
  For names which might be qualified by [database_name]." namely:
  event function procedure table trigger view. not index. not column.
  e.g. we might pass TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE
  Warning: If you ever add a search for really existing identifiers,
           remember that with PLSQL dbms_output is an implicit database
           and dual is an implicit table.
*/
int MainWindow::hparse_f_qualified_name_of_object(unsigned int token_flags, int database_or_object_identifier, int object_identifier)
{
  if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0)
  {
    if ((object_identifier == TOKEN_REFTYPE_TABLE)
      && (hparse_token == "."))
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ".");
      if (hparse_errno > 0) return 0;
      main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_RESERVED);
      main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_FUNCTION);
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      return 1;
    }
    hparse_f_next_nexttoken();
    if (hparse_next_token == ".")
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_DATABASE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ".");
      if (hparse_errno > 0) return 0;
      if (object_identifier == TOKEN_REFTYPE_TABLE)
      {
        main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_RESERVED);
        main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_FUNCTION);
      }
      hparse_f_expect(FLAG_VERSION_ALL, object_identifier,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      return 1;
    }
    if (hparse_next_token == "")
    {
      main_token_flags[hparse_i]|= token_flags;
      if (hparse_f_accept(FLAG_VERSION_ALL, database_or_object_identifier,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
        return 1;
      }
      return 0;
    }
  }
  main_token_flags[hparse_i]|= token_flags;
  if (hparse_f_accept(FLAG_VERSION_ALL, object_identifier,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    return 1;
  }
  return 0;
}

/*
  Variant of hparse_f_qualified_name_of_object,"*" is acceptable.
  For GRANT and REVOKE.
*/
int MainWindow::hparse_f_qualified_name_of_object_with_star(int database_or_object_identifier, int object_identifier)
{
  hparse_f_next_nexttoken();
  if (hparse_next_token == ".")
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE,TOKEN_TYPE_IDENTIFIER, "*") == 0)
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return 0;
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ".");
    if (hparse_errno > 0) return 0;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, object_identifier,TOKEN_TYPE_IDENTIFIER, "*") == 0)
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, object_identifier,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return 0;
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    return 1;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, object_identifier,TOKEN_TYPE_IDENTIFIER, "*") == 1)
  {
    return 1;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, database_or_object_identifier,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    return 1;
  }
  return 0;
}

/*
  These are the flags for possible parts of a qualified name.
  Translate them to equivalent TOKEN_REFTYPE_... values.
  We don't try to be precise and say TOKEN_REFTYPE_PARAMETER_REFER.
  We assume TOKEN_REFTYPE_VARIABLE_REFER not TOKEN_REFTYPE_VARIABLE.
*/
#define F_DB    1
#define F_TBL   2
#define F_ROW   4
#define F_FUNC  8
#define F_COL  16
#define F_VAR  32
int MainWindow::hparse_f_e_to_reftype(int e)
{
  if (e == F_DB) return TOKEN_REFTYPE_DATABASE;
  if (e == F_TBL) return TOKEN_REFTYPE_TABLE;
  if (e == F_FUNC) return TOKEN_REFTYPE_FUNCTION;
  if (e == F_ROW) return TOKEN_REFTYPE_ROW;
  if (e == F_COL) return TOKEN_REFTYPE_COLUMN;
  if (e == F_VAR) return TOKEN_REFTYPE_VARIABLE_REFER;
  if (e == (F_DB | F_FUNC)) return TOKEN_REFTYPE_DATABASE_OR_FUNCTION;
  if (e == (F_DB | F_FUNC | F_VAR)) return TOKEN_REFTYPE_DATABASE_OR_FUNCTION_OR_VARIABLE;
  if (e == (F_DB | F_TBL)) return TOKEN_REFTYPE_DATABASE_OR_TABLE;
  if (e == (F_DB | F_TBL | F_ROW)) return TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_ROW;
  if (e == (F_DB | F_TBL | F_COL)) return TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN;
  if (e == (F_DB | F_TBL | F_FUNC | F_COL)) return TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN_OR_FUNCTION;
  if (e == (F_DB | F_TBL | F_FUNC | F_VAR)) return TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_VARIABLE_OR_FUNCTION;
  if (e == (F_DB | F_TBL | F_FUNC | F_COL | F_VAR)) return TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN_OR_FUNCTION_OR_VARIABLE;
  if (e == (F_DB | F_TBL | F_ROW | F_FUNC | F_COL)) return TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_ROW_OR_FUNCTION_OR_COLUMN;
  if (e == (F_DB | F_TBL | F_ROW | F_FUNC | F_VAR)) return TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_ROW_OR_FUNCTION_OR_VARIABLE;
  if (e == (F_DB | F_TBL | F_ROW | F_FUNC | F_COL | F_VAR)) return TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN_OR_FUNCTION_OR_VARIABLE;
  if (e == (F_TBL | F_COL | F_FUNC)) return TOKEN_REFTYPE_TABLE_OR_COLUMN_OR_FUNCTION;
  if (e == (F_TBL | F_COL | F_FUNC | F_VAR)) return TOKEN_REFTYPE_TABLE_OR_COLUMN_OR_FUNCTION_OR_VARIABLE;
  if (e == (F_TBL | F_ROW)) return TOKEN_REFTYPE_TABLE_OR_ROW;
  if (e == (F_TBL | F_COL)) return TOKEN_REFTYPE_TABLE_OR_COLUMN;
  if (e == (F_ROW | F_VAR)) return TOKEN_REFTYPE_ROW_OR_VARIABLE;
  if (e == (F_COL | F_VAR)) return TOKEN_REFTYPE_COLUMN_OR_VARIABLE;
  if (e == (F_FUNC | F_VAR)) return TOKEN_REFTYPE_FUNCTION_OR_VARIABLE;
  /* TODO: More combinations possible, especially if rows are possible */
  /* If it fails, the hovering display will just be "identifier" */
  log("hparse_f_e_to_reftype failed", 80);
  return 0;
}

/* In MySQL/MariaDB a qualified identifier can be a reserved word. */
int MainWindow::hparse_f_accept_qualifier(unsigned int flag_version, unsigned char reftype, int proposed_type, QString token)
{
  if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) == 0)
    return hparse_f_expect(flag_version, reftype, proposed_type, token);
  int saved_token_flags= main_token_flags[hparse_i];
  main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_RESERVED);
  main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_FUNCTION);
  if (hparse_f_accept(flag_version, reftype, proposed_type, token) == 0)
  {
    main_token_flags[hparse_i]= saved_token_flags;
    return 0;
  }
  return 1;
}

/*
  Return true if passed token_type is any of TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK= 6,
  TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE= 7, TOKEN_TYPE_IDENTIFIER_WITH_AT = 8, TOKEN_TYPE_IDENTIFIER = 8.
  Reason: in hparse_f_accept() there was "else main_token_types[hparse_i]= proposed_type;" but passed proposed_type
  could be TOKEN_TYPE_IDENTIFIER, so after SET OCELOT_EXPLORER_VISIBLE = 'yes'; USE `w` + control-E we didn't reach `w`
  because it's not TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK.
  Todo: Check whether there should be a similar check for the specific varieties of literals.
*/
bool MainWindow::hparse_f_is_identifier(int token_type)
{
  if ((token_type >= TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK) && (token_type <= TOKEN_TYPE_IDENTIFIER)) return true;
  return false;
}

/*
  If an identifier has not been declared earlier, then it can't be
  a row variable or a variable.
  This doesn't look for system variables, only for defined variables.
  If keyword_row == TOKEN_KEYWORD_ROW, we look for row variables.
  Assume: it is a row variable if the word ROW follows, or if
          it's in FOR ... END FOR. We do not check "what are the
          fields of the row", which often depends on table lookup.
          Exception: in FOR i IN x .. y DO, i is not a row variable.
  Luckily row-variables and scalar-variables can't have same names.
  Assume: If it's not within begin...end, or following is|as in plsql,
          there can't have been any variable declarations, so jump
          out fast if hparse_f_is_in_compound() is false.
  Warning: in plsql we count sqlcode + sqlerrm as variables, but
           main_token_pointers is 0 (if it was > 0 it would be a
           shadowing declared variable). Maybe they're really system
           variables, or functions that happen to lack ()s.
*/
bool MainWindow::hparse_f_is_variable(int i, int keyword_row)
{
  if (hparse_f_is_in_compound() == false) return false;
  int i_of_define;
  bool true_or_false= false;
  int saved_hparse_i= hparse_i;
  hparse_i= i;
  if (hparse_f_variables(&i_of_define) == 0) true_or_false= false;
  else true_or_false= true;
  hparse_i= saved_hparse_i;
  if (true_or_false == true)
  {
    bool is_row_variable= false;
    int next_word, prev_word;
     next_word= next_i(i_of_define, +1);
     if (main_token_types[next_word] == TOKEN_KEYWORD_ROW)
    {
      is_row_variable= true;
    }
    else if (main_token_types[next_word] == TOKEN_KEYWORD_IN)
    {
      prev_word= next_i(i_of_define, -1);
      is_row_variable= true;
      if (main_token_types[prev_word] == TOKEN_KEYWORD_FOR_IN_FOR_STATEMENT)
      {
        QString k_token;
        for (int k= next_word + 1;; ++k)
        {
          k_token= hparse_text_copy.mid(main_token_offsets[k], main_token_lengths[k]);
          if ((k_token == "")
           || (k_token == ";")
           || (k_token == hparse_delimiter_str)
           || (k_token.toUpper() == "DO"))
            break;
          if (k_token == "..")
          {
            is_row_variable= false;
            break;
          }
        }
      }
    }
    if (keyword_row == TOKEN_KEYWORD_ROW)
    {
      if (is_row_variable) return true;
      else return false;
    }
    if (keyword_row != TOKEN_KEYWORD_ROW)
    {
      if (is_row_variable) return false;
      else return true;
    }
  }
  else
  {
    if (hparse_dbms_mask & FLAG_VERSION_PLSQL)
    {
      QString token= hparse_text_copy.mid(main_token_offsets[saved_hparse_i], main_token_lengths[saved_hparse_i]).toUpper();
      if ((token == "SQLCODE") || (token == "SQLERRM")) return true;
    }
  }
  return true_or_false;
}

/*
  We're looking for a site identifier, but wow it gets complicated.
  In this chart:
  (MySQL) means MySQL or MariaDB as opposed to SQLite/Tarantool,
  (v) means a variable is possible because we're in a routine,
  (s) means a column is possible because we're inside DML.
  "Situation" is what's in the next few tokens, X being identifier.
  "Expect" is what we pass to hparse_f_expect() if Situation is true.
  Situation                                Expect
  ---------                                ------
  EOF                                      ?? not handled
  .           (MySQL)                 (s)  . table . column
  @@X EOF     (MySQL) (operand) (v)        variable
  @@X . EOF   (MySQL) (operand) (v)        variable . variable
  @@X . X .   (MySQL) (operand) (v)        variable . variable . variable
  @@X . X     (MySQL) (operand) (v)        variable . variable
  @X          (MySQL) (operand) (v)        variable
  X EOF       (MySQL) (operand) (v)(f)(s)  database|table|column|function|variable
  X EOF       (MySQL) (operand) (v)(f)     function|variable
  X EOF               (operand)    (f)(s)  database|table|column|function
  X EOF                               (s)  database|table|column
  X EOF       (MySQL) (operand)    (f)     function
  X (                 (operand)    (f)     function
  X . EOF     (MySQL) (operand)    (f)(s)  database|table . table|column|function
  X . EOF                             (s)  database|table . table|column
  X . X EOF   (MySQL) (operand)    (f)(s)  database|table . table|column|function
  X . X EOF                           (s)  database|table . table|column
  X . X .                             (s)  database . table . column
  X . X (                          (f)     database . function
  X . X other                         (s)  table . column
  X other     (MySQL) (operand) (v)   (s)  column|variable
  X other     (MySQL) (operand) (v)        variable
  X other     (MySQL) (operand)       (s)  column
  X other                             (s)  column
  And in MySQL, if X is qualified, it's okay even if it's reserved.
  Todo: "drop table .www;" is legal but you don't allow it (that's for object names).
  We set v iff MySQL/MariaDB AND there might be parameters or variables.
  If we find a declaration then it's a declared variable but if we don't
  then we aren't sure: it might be either column or server server variable.
  We change main_token_pointers[] if SET ...=...variable_name... so that
  get_sql_mode will know if SET sql_mode=ORACLE is to a variable or not.
  Todo: We can call this while we're looking at an unqualified literal.
        Probably it's harmless (slight time waste?), but make sure.
  Todo: The above table doesn't show effect of MariaDB 10.3 row.x vars.
  Todo: one too many kludges. rewrite?
  TODO: We need a different kind of accept() if "." follows:
        (if MySQL|MariaDB)
        save
        main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_RESERVED);
        main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_FUNCTION);
        hparse_f_accept()
        if fails anyway, restore
        ... Use it when you know you're looking at MySQL qualifier.
  TODO: Check: why do we call this, not hparse_f_qualified_name_of_object,
        when adding column etc. in CREATE|ALTER TABLE?
  TODO: We avoid this when looking for some <simple value specification>s because searches for variables
        are just asking for "[identifier]", I think it's because hparse_f_is_variables() returns false
        even if a variable could follow. If/when this is fixed, hparse_f_simple_value() might be redundant.
*/
int MainWindow::hparse_f_qualified_name_of_operand(unsigned int token_flags, bool v, bool f, bool s)
{
  bool is_variable_possible= false;
  bool m= false;
  if (hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) m= true;
  if (v & f & s)
  {
    /* hparse_f_opr_18() passes true,true,true = anything goes,
       but statements that don't have tables don't have columns.
       Todo: look again at all legal commands to be sure of this. */
    if ((hparse_is_in_subquery)
     || (hparse_statement_type == TOKEN_KEYWORD_INSERT)
     || (hparse_statement_type == TOKEN_KEYWORD_DELETE)
     || (hparse_statement_type == TOKEN_KEYWORD_UPDATE)
     || (hparse_statement_type == TOKEN_KEYWORD_REPLACE)
     || (hparse_statement_type == TOKEN_KEYWORD_CREATE)
     || (hparse_statement_type == TOKEN_KEYWORD_ALTER)
     || (hparse_statement_type == TOKEN_KEYWORD_HANDLER)
     || (hparse_statement_type == TOKEN_KEYWORD_LOAD)
     || (hparse_statement_type == TOKEN_KEYWORD_TABLE)
     || (hparse_statement_type == TOKEN_KEYWORD_SELECT)) {;}
    else s= false;
  }
  /* plsql triggers can have new|old . column-name */
  /* todo: fix: I had to say variable_refer so it works with assignments */
  if (((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0)
   && ((v) || (s))
   && (hparse_create_trigger_seen))
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ":") == 1)
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ROW,TOKEN_TYPE_IDENTIFIER, "NEW") == 0)
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ROW,TOKEN_TYPE_IDENTIFIER, "OLD");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ".");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_VARIABLE_REFER,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      return 1;
    }
  }

  if (m & v)
  {
    if ((hparse_token.left(2) == "@@")
     && (hparse_next_token == ""))
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      return 1;
    }
  }
  if (m & v)
  {
    if ((hparse_token.left(2) == "@@")
     && (hparse_next_token == ".")
     && (hparse_next_next_token != "")
     && (hparse_next_next_next_token == "."))
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ".");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ".");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      return 1;
    }
  }
  if (m & v)
  {
    if ((hparse_token.left(2) == "@@")
     && (hparse_next_token == ".")
     && (hparse_next_next_token != ""))
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ".");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      return 1;
    }
  }
  if (m & v)
  {
    if (hparse_token.left(1) == "@")
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      return 1;
    }
  }
  int i_plus[5]= {0,0,0,0,0};
  i_plus[0]= hparse_i;
  bool q_plus_1_is_percent= false;
  QString q_plus[5];
  {
    int token_type;
    QString token= "So far nothing has been seen";
    for (int i= 0; i < 5; ++i)
    {
      if (i == 0)   i_plus[0]= hparse_i;
      else i_plus[i]= next_i(i_plus[i - 1], +1);
      if ((token != "/*Other*/") && (token != ""))
      {
        if (main_token_lengths[i_plus[i]] == 0) token= "";
        else
        {
          token_type= main_token_types[i_plus[i]];
          token= hparse_text_copy.mid(main_token_offsets[i_plus[i]], main_token_lengths[i_plus[i]]);
          if ((token != ".")
           && (token != "(")
           && ((token_type == TOKEN_TYPE_OPERATOR) || (i == 1) || (i == 3)))
          {
            if ((i == 1) && (token == "%")) q_plus_1_is_percent= true;
            token= "/*Other*/";
          }
        }
      }
      q_plus[i]= token;
    }
  }
  /*
    Todo: We did not anticipate that MariaDB 10.3 would support PLSQL
    with cursor-name % attribute. There's no F_CUR. So before we see
    "%" we fail to hint that it might be a cursor. So someday rewrite
    all that follows. For now we have a short circuit.
  */
  if (((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0)
   && (v)
   && (q_plus_1_is_percent))
  {
    hparse_f_find_define(hparse_i_of_statement, TOKEN_REFTYPE_CURSOR_DEFINE, TOKEN_REFTYPE_CURSOR_REFER, true);
    if (hparse_errno > 0) return 0;
    /* guaranteed to succeed */
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "%");
    if (hparse_errno > 0) return 0;
    if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ATTRIBUTE,TOKEN_TYPE_KEYWORD, "FOUND") == 0)
     && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ATTRIBUTE,TOKEN_TYPE_KEYWORD, "NOTFOUND") == 0)
     && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ATTRIBUTE,TOKEN_TYPE_KEYWORD, "ROWCOUNT") == 0)
     && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ATTRIBUTE,TOKEN_TYPE_KEYWORD, "ISOPEN") == 0))
      hparse_f_error();
    if (hparse_errno > 0) return 0;
    main_token_types[hparse_i_of_last_accepted]= TOKEN_TYPE_IDENTIFIER;
    return 1;
  }
  /* Start with: possibles are database|table|column|var|func
  Eliminate as you go along.
  We never use e[1] or e[3] they would correspond to "."s. */
  int e[5]= {0,0,0,0,0};

  int qualifiers= 0;

  if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_3) != 0)
  {
    if (f) { e[0]|= F_DB|F_FUNC; e[2]= F_FUNC; }
    if (v) { e[0]|= F_ROW | F_VAR; e[2]|= F_VAR; }
    if (s) { e[0]|= F_DB|F_TBL|F_COL; e[2]|= F_TBL|F_COL; e[4]|= F_COL; }
  }
  else if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0)
  {
    if (f) { e[0]|= F_DB|F_FUNC; e[2]|= F_FUNC; }
    if (v) { e[0]|= F_VAR; e[2]|= F_VAR; }
    if (s) { e[0]|= F_DB|F_TBL|F_COL; e[2]|= F_TBL|F_COL; e[4]|= F_COL; }
  }
  else /* if Tarantool */
  {
    if (f) { e[0]|= F_FUNC; }
    if ((v) && ((hparse_dbms_mask & FLAG_VERSION_LUA_OUTPUT) != 0)) { e[0]= F_VAR; }
    if (s) { e[0]|= F_TBL | F_COL; e[2]|= F_COL; }
  }
  if (q_plus[0] == ".")
  {
    /* hparse_f_accept the . !! but redo! MOVE THIS UP! */
    e[0]&= F_TBL;
    e[0]&= F_COL;
  }
  /* Todo: Don't look for "." if e[2]&(F_VAR|F_COL|F_FUNC) == 0 */
  if (q_plus[1] == ".")
  {
    ++qualifiers;
    e[0]&= (~(F_FUNC|F_VAR|F_COL));
    /* Todo: don't look for "." if e[4]&F_COL == 0 */
    if (q_plus[3] == ".")
    {
      ++qualifiers;
      e[0]&= F_DB; e[2]&= F_TBL; e[4]&= F_COL;
    }
    else if (q_plus[3] == "(") e[2]&= F_FUNC;
    else if (q_plus[3] == "") e[4]= 0;
    else /* other */ { e[2]&= (~F_FUNC); }
  }
  else if (q_plus[1] == "(") e[0]&= F_FUNC;
  else if (q_plus[1] == "") { e[2]= 0; e[4]= 0; }
  else /* other */ e[0]&= (~F_FUNC);
  for (int i= 0; i < 5; ++i)
  {
    /* TEST!!!! I'm baffled by these two lines, I'm replacing them. */
    //if ((q_plus[i] == "") || (q_plus[i] == "/*Other*/"))
    //  e[i]= 0;
    if (q_plus[i] == "") break;
    if (q_plus[i] == "/*Other*/") e[i]= 0;
  }
  for (int i= 0; i < 4; ++i)
  {
    if (q_plus[i+1] == "/*Other*/")
    {
      e[i]&= (~(F_DB | F_TBL | F_ROW | F_FUNC));
    }
    else if (q_plus[i+1] == "(")
    {
      e[i]&= F_FUNC;
      if (i == 2) e[0]&= F_DB;
    }
  }
  if (q_plus[3] == "/*Other*/") e[0]&= (~F_DB);

  /*
     Looking for var, or row.member (where we label member as var)
     Todo: We don't distinguish var from var-member-of-row
     Todo: Last time I looked, row-var-name trumps table-name
           so if you find row-var you can turn off F_TBL.
           But I'm not sure whether that's a permanent feature.
     Todo: We're not looking for system variables, which could
           exist without starting with @@ in SET statements (targets).
           Ideally: select them when you connect, binary-search them
           here. Currently for SET sql_mode='' we say sql_mode is
           a generic identifier.
  */
  if ((e[0]&F_VAR) != 0)
  {
    is_variable_possible= true;
    if (hparse_f_is_variable(i_plus[0], 0) == false) e[0]&= (~F_VAR);
    else  e[0]&= (~F_COL);
  }

  if ((e[0]&F_ROW) != 0)
  {
    if (hparse_f_is_variable(i_plus[0], TOKEN_KEYWORD_ROW) == false)
    {
      e[0]&= (~F_ROW);
    }
  }

  if ((e[2]&F_VAR) != 0)
  {
    if ((e[0] & F_ROW) == 0)
    {
      e[2]&= (~F_VAR);
    }
  }
  if (e[2] == F_COL) e[0]= F_TBL; /* kludge. What turned off F_TBL? */

  if (q_plus[0] == "")
  {
    /* Immediate EOF or token_type is operator other than "." or "(".
     . Therefore this accept() will fail, okay. */
    if (hparse_f_accept(FLAG_VERSION_ALL, hparse_f_e_to_reftype(e[0]),TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0) return 0;
    /* maybe assert if you get here */
  }
  /* Perhaps e[0] == 0 at this point i.e. nothing is legal.
     But accept an identifier anyway, if it's followed by a ".".
     (The problem is the "." not the identifier.)
     Use a saved copy of e[0].
     Same applies if qualifiers == 2. */
  /* Todo: I dunno, maybe hparse_f_expect is better sometimes? */
  if (qualifiers == 2)
  {
    if (hparse_f_accept_qualifier(FLAG_VERSION_ALL, hparse_f_e_to_reftype(e[0]),TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0) return 0;
    if (hparse_f_accept(FLAG_VERSION_ALL,TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ".") == 0) return 0;
    if (hparse_f_accept_qualifier(FLAG_VERSION_ALL, hparse_f_e_to_reftype(e[2]),TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0) return 0;
    if (hparse_f_accept(FLAG_VERSION_ALL,TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ".") == 0) return 0;
    if (hparse_f_accept(FLAG_VERSION_ALL, hparse_f_e_to_reftype(e[4]),TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
    {
      hparse_f_error();
      return 0;
    }
    return 1;
  }
  if (qualifiers == 1)
  {
    if (hparse_f_accept_qualifier(FLAG_VERSION_ALL, hparse_f_e_to_reftype(e[0]),TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0) return 0;
    if (hparse_f_accept(FLAG_VERSION_ALL,TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ".") == 0) return 0;
    if (hparse_f_accept(FLAG_VERSION_ALL, hparse_f_e_to_reftype(e[2]),TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
    {
      hparse_f_error();
      return 0;
    }
    return 1;
  }
  if (qualifiers == 0)
  {
    /* if ep[2]==0 and F_VAR|F_COL, see if F_COL is declared earlier.
       if it is, eliminate F_COL. if it isn't, eliminate F_VAR
       (?? but maybe it is a system variable?) */
    ; /* Todo: eliminate F_ROW if it isn't declared earlier */
    ; /* Todo: eliminate F_VAR if it isn't declared earlier */
    ; /*       but if it IS declared earlier, eliminate F_COL */
    if (q_plus[1] == "/*Other*/") e[0]&= (~(F_DB | F_TBL | F_ROW));
    if (is_variable_possible == true) hparse_variable_is_allowed= true;
    main_token_flags[hparse_i]|= token_flags;
    if (hparse_f_accept(FLAG_VERSION_ALL, hparse_f_e_to_reftype(e[0]),TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0) return 0;
    hparse_variable_is_allowed= false;
    return 1;
  }
  return 0;
}

int MainWindow::hparse_f_qualified_name_with_star() /* like hparse_f_qualified_name but may end with * */
{
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ".") == 1)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "*") == 1) return 1;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ".") == 1)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "*") == 1) return 1;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return 0;
      }
    }
    return 1;
  }
  return 0;
}

/* escaped_table_reference [, escaped_table_reference] ... */
int MainWindow::hparse_f_table_references()
{
  int saved_hparse_i= hparse_i;
  do
  {
    hparse_f_table_escaped_table_reference();
    if (hparse_errno > 0) return 0;
  } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  if (hparse_i == saved_hparse_i) return 0;
  return 1;
}

/* table_reference | { OJ table_reference } */
void MainWindow::hparse_f_table_escaped_table_reference()
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "{") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OJ");
    if (hparse_errno > 0) return;
    if (hparse_f_table_reference(0) == 0)
    {
      hparse_f_error();
      return;
    }
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "}");
    if (hparse_errno > 0) return;
    return;
  }
  if (hparse_f_table_reference(0) == 1) return;
  if (hparse_errno > 0) return;
}

/* table_factor | join_table
   Since join_table might start with table_factor, we might have to back up and redo.
*/
int MainWindow::hparse_f_table_reference(int who_is_calling)
{
  {
    int saved_hparse_i= hparse_i;
    int saved_hparse_token_type= hparse_token_type;
    QString saved_hparse_token= hparse_token;

    if (hparse_f_table_factor() == 1)
    {
      /* todo: figure out whether the word OUTER is on this list correctly */
      if (who_is_calling == TOKEN_KEYWORD_JOIN) return 1;
      if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INNER, "INNER") == 1)
       || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CROSS, "CROSS") == 1)
       || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_JOIN, "JOIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_STRAIGHT_JOIN, "STRAIGHT_JOIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LEFT, "LEFT") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_RIGHT, "RIGHT") == 1)
       || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_OUTER, "OUTER") == 1)
       || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_NATURAL, "NATURAL") == 1))
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
        hparse_i= saved_hparse_i;
        hparse_token_type= saved_hparse_token_type;
        hparse_token= saved_hparse_token;
        if (hparse_f_table_join_table() == 1)
        {
          /* Despite the BNF MySQL accepts a series of LEFTs and RIGHTs */
          /* todo: check for other cases where MySQL accepts a series */
          for (;;)
          {
            if ((QString::compare(hparse_token, "LEFT", Qt::CaseInsensitive) == 0)
             || (QString::compare(hparse_token, "RIGHT", Qt::CaseInsensitive) == 0)
             || (QString::compare(hparse_token, "NATURAL", Qt::CaseInsensitive) == 0))
            {
              if (hparse_f_table_join_table() == 0) break;
              if (hparse_errno > 0) return 0;
            }
            else break;
          }
          return 1;
        }
        hparse_f_error();
        return 0;
      }
    }
    return 1;
  }
  hparse_f_error();
  return 0;
}

/* tbl_name [PARTITION (partition_names)]
        [[AS] alias] [index_hint_list]
   | table_subquery [AS] alias
   | ( table_references ) */
/* Undocumented detail: alias can be a literal instead of an identifier. Ugly. */
int MainWindow::hparse_f_table_factor()
{
  if (hparse_f_accept(FLAG_VERSION_TARANTOOL_2_11, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SEQSCAN, "SEQSCAN") == 1) {;}
  if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE,TOKEN_REFTYPE_TABLE) == 1)
  {
    hparse_f_partition_list(false, false);
    if (hparse_errno > 0) return 0;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_AS, "AS") == 1)
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ALIAS_OF_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
        if (hparse_f_literal(TOKEN_REFTYPE_ALIAS_OF_TABLE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else
    {
      if (((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
       && (QString::compare(hparse_token, "INDEXED", Qt::CaseInsensitive) == 0))
      {
        ;
      }
      else
      {
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ALIAS_OF_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
          hparse_f_literal(TOKEN_REFTYPE_ALIAS_OF_TABLE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING);
      }
    }
    hparse_f_table_index_hint_list();
    if (hparse_errno > 0) return 0;
    return 1;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
  {
    if (hparse_f_query(0, false, false, false) == 1)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return 0;
      bool is_alias_of_table_compulsory= true;
      if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
        is_alias_of_table_compulsory= false;
      {
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_AS, "AS") == 1)
          is_alias_of_table_compulsory= true;
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ALIAS_OF_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
        {
          if (is_alias_of_table_compulsory == true) hparse_f_error();
        }
      }
      if (hparse_errno > 0) return 0;
      return 1;
    }
    else
    {
      if (hparse_errno > 0) return 0;
      hparse_f_table_references();
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return 0;
      return 1;
    }
  }
  return 0;
}

/*
  table_reference [INNER | CROSS] JOIN table_factor [join_condition]
  | table_reference STRAIGHT_JOIN table_factor
  | table_reference STRAIGHT_JOIN table_factor ON conditional_expr
  | table_reference {LEFT|RIGHT} [OUTER] JOIN table_reference join_condition
  | table_reference NATURAL [{LEFT|RIGHT} [OUTER]] JOIN table_factor
  ...  we've changed the first choice to
  table_reference { [INNER | CROSS] JOIN table_reference [join_condition] ... }
*/
int MainWindow::hparse_f_table_join_table()
{
  if (hparse_f_table_reference(TOKEN_KEYWORD_JOIN) == 1)
  {
    bool inner_or_cross_or_join_seen= false;
    for (;;)
    {
      bool inner_seen= false;
      bool cross_seen= false;
      bool join_seen= false;
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INNER, "INNER") == 1)
        inner_seen= true;
      else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INNER, "CROSS") == 1)
        cross_seen= true;
      else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INNER, "JOIN") == 1)
        join_seen= true;
      if (inner_seen | cross_seen | join_seen)
      {
        inner_or_cross_or_join_seen= true;
        if (inner_seen | cross_seen)
          hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_JOIN, "JOIN");
        if (hparse_errno > 0) return 0;
        if (hparse_f_table_factor() == 0)
        {
           hparse_f_error();
           return 0;
        }
        hparse_f_table_join_condition();
        if (hparse_errno > 0) return 0;
      }
      else break;
    }
    if (inner_or_cross_or_join_seen == true) return 1;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_STRAIGHT_JOIN, "STRAIGHT_JOIN") == 1)
    {
      if (hparse_f_table_factor() == 0)
      {
         hparse_f_error();
         return 0;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ON, "ON") == 1)
      {
        hparse_f_opr_1(0, 0);
        if (hparse_errno > 0) return 0;
      }
      return 1;
    }
    if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LEFT, "LEFT") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_RIGHT, "RIGHT") == 1))
    {
      main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_OUTER, "OUTER");
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_JOIN, "JOIN");
      if (hparse_errno > 0) return 0;
      if (hparse_f_table_reference(0) == 0)
      {
         hparse_f_error();
         return 0;
      }
      if (hparse_f_table_join_condition() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      return 1;
    }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_NATURAL, "NATURAL"))
    {
      if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LEFT, "LEFT") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_RIGHT, "RIGHT") == 1))
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
        hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_OUTER, "OUTER");
      }
      else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INNER, "INNER") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CROSS, "CROSS") == 1) {;}
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_JOIN, "JOIN");
      if (hparse_errno > 0) return 0;
      if (hparse_f_table_factor() == 0)
      {
        hparse_f_error();
        return 0;
      }
      return 1;
    }
    hparse_f_error();
    return 0;
  }
  return 0;
}

int MainWindow::hparse_f_table_join_condition()
{
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ON, "ON") == 1)
  {
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return 0;
    return 1;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_USING, "USING") == 1)
  {
    hparse_f_column_list(1, 0);
    if (hparse_errno > 0) return 0;
    return 1;
  }
  return 0;
}

/*  index_hint [, index_hint] ... */
void MainWindow::hparse_f_table_index_hint_list()
{
  if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
  {
    if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "INDEXED") == 1)
    {
      hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_INDEX, TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "NOT") == 1)
    {
      hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "INDEXED");
      if (hparse_errno > 0) return;
    }
    return;
  }
  do
  {
    if (hparse_f_table_index_hint() == 0) return;
  } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
}

/* USE    {INDEX|KEY} [FOR {JOIN|ORDER BY|GROUP BY}] ([index_list])
 | IGNORE {INDEX|KEY} [FOR {JOIN|ORDER BY|GROUP BY}] (index_list)
 | FORCE  {INDEX|KEY} [FOR {JOIN|ORDER BY|GROUP BY}] (index_list) */
int MainWindow::hparse_f_table_index_hint()
{
  bool use_seen= false;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_USE, "USE") == 1) use_seen= true;
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORE") == 1)  {;}
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FORCE") == 1)  {;}
  else return 0;
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX") == 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY") == 0))
  {
    hparse_f_error();
    return 0;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR") == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "JOIN") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ORDER") == 1)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY") == 0)
      {
        hparse_f_error();
        return 0;
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GROUP") == 1)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY") == 0)
      {
        hparse_f_error();
        return 0;
      }
    }
  }
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return 0;
  if (hparse_f_table_index_list() == 0)
  {
    if (hparse_errno > 0) return 0;
    if (use_seen == false)
    {
      hparse_f_error();
      return 0;
    }
  }
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return 0;
  return 1;
}

/* index_name [, index_name] ... */
int MainWindow::hparse_f_table_index_list()
{
  int return_value= 0;
  do
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_INDEX,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
    if (hparse_errno > 0) return 0;
  } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  return return_value;
}

/*
  Operators, in order of precedence as in
  http://dev.mysql.com/doc/refman/5.7/en/operator-precedence.html
  Todo: take into account: PIPES_AS_CONCAT, HIGH_NOT_PRECEDENCE (but those are server options!)
  For unary operators: eat the operator and call the upper level.
  For binary operators: call the upper level, then loop calling the upper level.
  Call hparse_f_opr_1 when you want an "expression", hparse_f_opr_18 for an "operand".
*/

/*
  TODO: I'm not sure about this, it seems to allow a := b := c
*/
void MainWindow::hparse_f_opr_1(int who_is_calling, int allow_flags) /* Precedence = 1 (bottom) */
{
  hparse_f_opr_2(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ":=") == 1) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=") == 1))
  {
    hparse_f_opr_2(who_is_calling, allow_flags);
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_2(int who_is_calling, int allow_flags) /* Precedence = 2 */
{
  hparse_f_opr_3(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OR") == 1) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "||") == 1))
  {
    hparse_f_opr_3(who_is_calling, allow_flags);
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_3(int who_is_calling, int allow_flags) /* Precedence = 3 */
{
  hparse_f_opr_4(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "XOR") == 1)
  {
    hparse_f_opr_4(who_is_calling, allow_flags);
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_4(int who_is_calling, int allow_flags) /* Precedence = 4 */
{
  hparse_f_opr_5(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AND") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "&&") == 1))
  {
    hparse_f_opr_5(who_is_calling, allow_flags);
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_5(int who_is_calling, int allow_flags) /* Precedence = 5 */
{
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOT") == 1) {;}
  hparse_f_opr_6(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
}

/*
  Re MATCH ... AGAINST: unfortunately IN is an operator but also a clause-starter.
  So if we fail because "IN (" was expected, this is the one time when we have to
  override and set hparse_errno back to zero and carry on.
  Re CASE ... END: we change the token types, trying to avoid confusion with CASE statement.
*/
void MainWindow::hparse_f_opr_6(int who_is_calling, int allow_flags) /* Precedence = 6 */
{
  if (hparse_errno > 0) return;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CASE_IN_CASE_EXPRESSION, "CASE") == 1)
  {
    int when_count= 0;
    bool is_case_type_1= false;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WHEN") == 0)
    {
      hparse_f_opr_1(who_is_calling, allow_flags);
      if (hparse_errno > 0) return;
    }
    else
    {
      when_count= 1;
      is_case_type_1= true;
    }
    for (;;)
    {
      if (((when_count == 1) && (is_case_type_1 == true))
       || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WHEN") == 1))
      {
        ++when_count;
        hparse_f_opr_1(who_is_calling, allow_flags);
        if (hparse_errno > 0) return;
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "THEN") == 1)
        {
          hparse_f_opr_1(who_is_calling, allow_flags);
          if (hparse_errno > 0) return;
        }
        else hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else
      {
        break;
      }
    }
    if (when_count == 0)
    {
      hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ELSE") == 1)
    {
      hparse_f_opr_1(who_is_calling, allow_flags);
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END_IN_CASE_EXPRESSION, "END") == 1)
    {
      return;
    }
    else hparse_f_error();
    if (hparse_errno > 0) return;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MATCH") == 1)
  {
    hparse_f_column_list(1, 1);
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AGAINST");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return;
    hparse_f_opr_1(who_is_calling, allow_flags);
    bool in_seen= false;
    if (hparse_errno > 0)
    {
      if (QString::compare(hparse_prev_token, "IN", Qt::CaseInsensitive) != 0) return;
      hparse_errno= 0;
      in_seen= true;
    }
    else
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IN") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
        in_seen= true;
      }
    }
    if (in_seen == true)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BOOLEAN") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MODE");
        return;
      }
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NATURAL");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LANGUAGE");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MODE");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITH") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "QUERY");
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXPANSION");
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITH") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "QUERY");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXPANSION");
      if (hparse_errno > 0) return;
    }
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
    return;
  }

//  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BETWEEN") == 1)
//   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CASE") == 1)
//   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WHEN") == 1)
//   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "THEN") == 1)
//   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ELSE") == 1)
//          )
//    {;}
  hparse_f_opr_7(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
}

int MainWindow::hparse_f_comp_op()
{
  if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ">=") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ">") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "<=") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "<") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "<>") == 1)
   || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "==") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "!=") == 1))
    return 1;
  return 0;
}

/* Most comp-ops can be chained e.g. "a <> b <> c", but not LIKE or IN. */
void MainWindow::hparse_f_opr_7(int who_is_calling, int allow_flags) /* Precedence = 7 */
{
  if ((hparse_subquery_is_allowed == true) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS") == 1))
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return;
    if (hparse_f_query(0, false, false, false) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
    return;
  }
  if (hparse_f_is_equal(hparse_token, "(")) hparse_f_opr_8(who_is_calling, allow_flags | ALLOW_FLAG_IS_MULTI);
  else hparse_f_opr_8(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
  if (hparse_errno > 0) return;
  for (;;)
  {
    /* If we see "NOT", the only comp-ops that can follow are "LIKE" and "IN". */
    bool not_seen= false;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOT") == 1)
    {
      not_seen= true;
    }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LIKE") == 1)
    {
      hparse_like_seen= true;
      hparse_f_opr_8(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
      hparse_like_seen= false;
      break;
    }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IN") == 1)
    {
      hparse_f_opr_8(who_is_calling, allow_flags | ALLOW_FLAG_IS_MULTI);
      if (hparse_errno > 0) return;
      break;
    }
    /* The manual says BETWEEN has a higher priority than this */
    else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BETWEEN") == 1)
    {
      hparse_f_opr_8(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AND");
      if (hparse_errno > 0) return;
      hparse_f_opr_8(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
      if (hparse_errno > 0) return;
      return;
    }
    if (not_seen == true)
    {
      hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "->") == 1) /* MySQL 5.7.9 JSON-colum->path operator */
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "<=>") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REGEXP") == 1))
    {
      hparse_f_opr_8(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
      if (hparse_errno > 0) return;
      continue;
    }
    if (hparse_f_comp_op() == 1) /* = or >= or > or <= or < or <> or == or != */
    {
      if (hparse_subquery_is_allowed == true)
      {
        if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SOME") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ANY") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1))
        {
          /* todo: what if some mad person has created a function named any or some? */
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
          if (hparse_errno > 0) return;
          if (hparse_f_query(0, false, false, false) == 0) hparse_f_error();
          if (hparse_errno > 0) return;
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
          if (hparse_errno > 0) return;
          continue;
        }
      }
      if (hparse_f_is_equal(hparse_token, "(")) hparse_f_opr_8(who_is_calling, allow_flags | ALLOW_FLAG_IS_MULTI);
      else hparse_f_opr_8(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
      if (hparse_errno > 0) return;
      continue;
    }
    else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IS") == 1)
    {
      hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOT");
      if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NULL") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TRUE") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FALSE") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNKNOWN") == 1))
        {;}
      else hparse_f_error();
      if (hparse_errno > 0) return;
      continue;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SOUNDS") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LIKE");
      if (hparse_errno > 0) return;
      hparse_f_opr_8(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
      if (hparse_errno > 0) return;
      continue;
    }
    break;
  }
}

void MainWindow::hparse_f_opr_8(int who_is_calling, int allow_flags) /* Precedence = 8 */
{
  if (hparse_errno > 0) return;
  hparse_f_opr_9(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "|") == 1)
  {
    hparse_f_opr_9(who_is_calling, allow_flags);
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_9(int who_is_calling, int allow_flags) /* Precedence = 9 */
{
  hparse_f_opr_10(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "&") == 1)
  {
    hparse_f_opr_10(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_10(int who_is_calling, int allow_flags) /* Precedence = 10 */
{
  hparse_f_opr_11(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "<<") == 1) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ">>") == 1))
  {
    hparse_f_opr_11(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_11(int who_is_calling, int allow_flags) /* Precedence = 11 */
{
  hparse_f_opr_12(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "-") == 1) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "+") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_BINARY_PLUS_OR_MINUS;
    hparse_f_opr_12(who_is_calling, allow_flags);
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_12(int who_is_calling, int allow_flags) /* Precedence = 12 */
{
  if (hparse_errno > 0) return;
  hparse_f_opr_13(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "*") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "/") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "DIV") == 1)
   || (((hparse_dbms_mask&FLAG_VERSION_PLSQL) == 0) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "%") == 1))
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "MOD") == 1))
  {
    hparse_f_opr_13(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_13(int who_is_calling, int allow_flags) /* Precedence = 13 */
{
  hparse_f_opr_14(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "^") == 1)
  {
    hparse_f_opr_14(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_14(int who_is_calling, int allow_flags) /* Precedence = 14 */
{
  if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "-") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "+") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "~") == 1))
  {
    hparse_f_opr_15(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
  }
  else hparse_f_opr_15(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_opr_15(int who_is_calling, int allow_flags) /* Precedence = 15 */
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "!") == 1)
  {
    hparse_f_opr_16(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
  }
  else hparse_f_opr_16(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
}

/* Actually I'm not sure what ESCAPE precedence is, as long as it's higher than LIKE. */
void MainWindow::hparse_f_opr_16(int who_is_calling, int allow_flags) /* Precedence = 16 */
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BINARY") == 1)
  {
    hparse_f_opr_17(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
  }
  else hparse_f_opr_17(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  if (hparse_like_seen == true)
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ESCAPE") == 1)
    {
      hparse_like_seen= false;
      hparse_f_opr_17(who_is_calling, allow_flags & (~ALLOW_FLAG_IS_MULTI));
      if (hparse_errno > 0) return;
      return;
    }
  }
  while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
  {
    if (hparse_f_collation_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
}

/* todo: disallow INTERVAL unless we've seen + or - */
void MainWindow::hparse_f_opr_17(int who_is_calling, int allow_flags) /* Precedence = 17 */
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INTERVAL") == 1)
  {
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT) == 0) hparse_f_error();

    if (hparse_errno > 0) return;
    hparse_f_interval_quantity(TOKEN_KEYWORD_INTERVAL);
    if (hparse_errno > 0) return;
    return;
  }
  hparse_f_opr_18(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
}

/*
  Final level is operand.
  factor = identifier | number | "(" expression ")" .
*/
void MainWindow::hparse_f_opr_18(int who_is_calling, int allow_flags) /* Precedence = 18, top */
{
  if (hparse_errno > 0) return;
  QString opd= hparse_token.toUpper();
  bool identifier_seen= false;
  /* Check near the start for all built-in functions that happen to be reserved words */
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CHAR, "CHAR") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CONVERT, "CONVERT") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXPRESSION, "IF") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INSERT, "INSERT") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LEFT, "LEFT") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LOCALTIME, "LOCALTIME") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LOCALTIMESTAMP, "LOCALTIMESTAMP") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_MOD, "MOD") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REPEAT_IN_REPEAT_EXPRESSION, "REPEAT") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REPLACE, "REPLACE") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_RIGHT, "RIGHT") == 1))
  {
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")") == 0)
    {
      hparse_f_function_arguments(opd);
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    return;
  }
  /* This should only work for INSERT ... ON DUPLICATE KEY UPDATE */
  if ((hparse_statement_type == TOKEN_KEYWORD_INSERT)
   && ((allow_flags & ALLOW_FLAG_IS_IN_DUPLICATE_KEY) != 0))
  {
    bool is_value_or_values= false;
    if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_3) != 0)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALUE") == 1)
        is_value_or_values= true;
    }
    else
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALUES") == 1)
        is_value_or_values= true;
    }
    if (is_value_or_values == true)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
      return;
    }
  }
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATABASE") == 1)
#if (FLAG_VERSION_MARIADB_12_0 != 0)
        || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CATALOG") == 1)
#endif
        || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SCHEMA") == 1))
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
    return;
  }
  /* todo: restrict so not all these functions accept an optional argument */
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURRENT_DATE") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURRENT_TIME") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURRENT_USER") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURRENT_ROLE") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURRENT_TIMESTAMP") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UTC_DATE") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UTC_TIME") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UTC_TIMESTAMP") == 1))
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
    {
      hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT);
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    return;
  }
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATE") == 1) /* DATE 'x', else DATE is not reserved so might be an id */
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TIME") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TIMESTAMP") == 1))
  {
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 1) return;
    identifier_seen= true;
  }
  int saved_hparse_i= hparse_i;
  hparse_f_next_nexttoken();

  if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_3) != 0)
  {
    if ((hparse_token.toUpper() == "NEXT")
     || (hparse_token.toUpper() == "PREVIOUS"))
    {
      if (hparse_next_token.toUpper() == "VALUE")
      {
        if (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NEXT") == 0)
          hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PREVIOUS");
        hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALUE");
        hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FOR, "FOR");
        if (hparse_errno > 0) return;
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_SEQUENCE,TOKEN_REFTYPE_SEQUENCE) == 0)
          hparse_f_error();
        if (hparse_errno > 0) return;
        return;
      }
    }
  }
  if (hparse_next_token == "(")
  {
    if ((main_token_flags[hparse_i] & TOKEN_FLAG_IS_FUNCTION) != 0)
    {
      int saved_token= main_token_types[hparse_i];
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
      {
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "[reserved function]");
        if (hparse_errno > 0) return;
      }
      identifier_seen= true;
      main_token_types[saved_hparse_i]= saved_token;
    }
  }
  if ((identifier_seen == true)
   || (hparse_f_qualified_name_of_operand(0, true,true,true) == 1))
  {
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1) /* identifier followed by "(" must be a function name */
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")") == 0)
      {
        hparse_f_function_arguments(opd);
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
      hparse_f_over(saved_hparse_i, who_is_calling);
      if (hparse_errno > 0) return;
    }
    return;
  }
  if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ANY) == 1)
  {
    if (hparse_errno > 0) return;
    return;
  }
  else if (hparse_errno > 0) return;
  if (hparse_f_default(TOKEN_KEYWORD_SELECT, false) == 1)
  {
    return;
  }
  else if (hparse_errno > 0) return;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "["))
  {
    if (hparse_errno > 0) return;
    hparse_f_bracketed_multi_expression();
    //if (hparse_errno > 0) return;
    return;
  }
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "("))
  {
    if (hparse_errno > 0) return;
    /* if subquery is allowed, check for "(SELECT ...") */
    if ((hparse_subquery_is_allowed == true)
     && (hparse_f_is_query(false) == true))
    {
      hparse_f_query(0, true, false, false);
      if (hparse_errno > 0) return;
    }
    else if ((allow_flags & ALLOW_FLAG_IS_MULTI) != 0)
    {
      int expression_count= 0;
      hparse_f_parenthesized_multi_expression(&expression_count);
    }
    else hparse_f_opr_1(who_is_calling, allow_flags);
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
    return;
  }
  hparse_f_error();
  return;
}

/*
  Check for OVER () if MariaDB 10.2, and in select-list or in order-by list.
  After ROW_NUMBER() it is compulsory. After AVG() it is optional.
  TODO: this will have to be checked again when MariaDB 10.2 is released.
*/
void MainWindow::hparse_f_over(int saved_hparse_i, int who_is_calling)
{
  if ((hparse_dbms_mask & (FLAG_VERSION_MARIADB_10_2_2 | FLAG_VERSION_MYSQL_8_0)) == 0) return;
  if (who_is_calling != TOKEN_KEYWORD_SELECT) return;
  bool function_is_aggregate= false;
  if ((main_token_types[saved_hparse_i] == TOKEN_KEYWORD_CUME_DIST)
   || (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_DENSE_RANK)
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_FIRST_VALUE))
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_LAST_VALUE))
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_LEAD))
   || ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_3) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_MEDIAN))
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_NTH_VALUE))
   || (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_NTILE)
   || (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_PERCENT_RANK)
   || (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_RANK)
   || (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_ROW_NUMBER))
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_OVER, "OVER");
    if (hparse_errno > 0) return;
  }
  else if ((main_token_types[saved_hparse_i] == TOKEN_KEYWORD_AVG)
   || (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_BIT_AND)
   || (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_BIT_OR)
   || (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_BIT_XOR)
   || (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_COUNT)
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_MAX))
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_MIN))
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_NTH_VALUE))
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_STD))
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_STDDEV))
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_STDDEV_POP))
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_STDDEV_SAMP))
   || (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_SUM)
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_VARIANCE))
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_VAR_POP))
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) && (main_token_types[saved_hparse_i] == TOKEN_KEYWORD_VAR_SAMP)))
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_OVER, "OVER") == 0) return;
    function_is_aggregate= true;
  }
  else return;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_WINDOW_REFER,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
  {
    hparse_f_window_spec(function_is_aggregate);
    if (hparse_errno > 0) return;
  }
  return;
}

int MainWindow::hparse_f_over_start(int who_is_calling)
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNBOUNDED") == 1)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PRECEDING");
    if (hparse_errno > 0) return 0;
    return 1;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURRENT") == 1)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROW");
    if (hparse_errno > 0) return 0;
    return 1;
  }
  if (who_is_calling != TOKEN_KEYWORD_BETWEEN) return 0;
  if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PRECEDING") == 0)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOLLOWING");
    }
    if (hparse_errno > 0) return 0;
    return 1;
  }
  return 0;
}

int MainWindow::hparse_f_over_end()
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNBOUNDED") == 1)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOLLOWING");
    if (hparse_errno > 0) return 0;
    return 1;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURRENT") == 1)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROW");
    if (hparse_errno > 0) return 0;
    return 1;
  }
  if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PRECEDING") == 0)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOLLOWING");
    }
    if (hparse_errno > 0) return 0;
    return 1;
  }
  return 0;
}


/*
  TODO: Recognize all 400+ built-in functions.
  Until then, we'll assume any function has a generalized comma-delimited expression list.
  But we still have to handle the ones that don't have simple lists.
  Todo: Tarantool accepts MAX(ALL opd), but that's probably a mistake.
*/
void MainWindow::hparse_f_function_arguments(QString opd)
{
  if ((hparse_f_is_equal(opd, "AVG"))
   || (hparse_f_is_equal(opd, "SUM"))
   || (hparse_f_is_equal(opd, "MIN"))
   || (hparse_f_is_equal(opd, "MAX")))
  {
    bool distinct_seen= false;
    if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISTINCT") == 1)
     || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1))
      distinct_seen= true;
    if ((hparse_f_is_equal(opd, "AVG"))
     || (hparse_f_is_equal(opd, "SUM"))
     || ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) == 0)
     || (distinct_seen == true))
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
    }
    else do
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  }
  else if (hparse_f_is_equal(opd, "CAST"))
  {
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS");
    if (hparse_errno > 0) return;
    if (hparse_f_data_type(TOKEN_KEYWORD_CAST) == -1) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0) && hparse_f_is_equal(opd, "CHAR"))
  {
    do
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USING") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
        break;
      }
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  }
  else if (((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0) && hparse_f_is_equal(opd, "CONVERT"))
  {
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1)
    {
      if (hparse_f_data_type(TOKEN_KEYWORD_CAST) == -1) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USING");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
  }
  else if (((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0) && hparse_f_is_equal(opd, "IF"))
  {
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",");
    if (hparse_errno > 0) return;
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",");
    if (hparse_errno > 0) return;
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_is_equal(opd, "COUNT"))
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISTINCT") == 1) hparse_f_opr_1(0, 0);
    else
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "*") == 1) {;}
      else hparse_f_opr_1(0, 0);
    }
    if (hparse_errno > 0) return;
  }
  else if ((hparse_f_is_equal(opd, "SUBSTR")) || (hparse_f_is_equal(opd, "SUBSTRING")))
  {
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
    if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM") == 1))
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
      if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR") == 1))
      {
        hparse_f_opr_1(0, 0);
        if (hparse_errno > 0) return;
      }
    }
  }
  else if (hparse_f_is_equal(opd, "TRIM"))
  {
    if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BOTH") == 1)
     || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LEADING") == 1)
     || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TRAILING") == 1)) {;}
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM") == 1)
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
    }
  }
  else if (((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0) && hparse_f_is_equal(opd, "WEIGHT_STRING"))
  {
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS") == 1)
    {
      int hparse_i_of_char= hparse_i;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHAR") == 0)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BINARY");
        if (hparse_errno > 0) return;
      }
      main_token_flags[hparse_i_of_char] &= (~TOKEN_FLAG_IS_FUNCTION);
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
    }
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LEVEL") == 1)
    {
      do
      {
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ASC") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DESC") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REVERSE") == 1)) {;}
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
  }
  else if (hparse_f_is_equal(opd, "GROUP_CONCAT"))
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISTINCT") == 0)
      hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL");
    do
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0) return;
    hparse_f_order_by(0);
    if (hparse_errno > 0) return;
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SEPARATOR") == 1))
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_3) != 0)
    {
      hparse_f_limit(TOKEN_KEYWORD_GROUP_CONCAT);
      if (hparse_errno > 0) return;
    }
  }
  else do
  {
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
}

/*
  Expression list within INSERT, VALUES, SELECT, RETURNING, or HANDLER.
  (Added later: or CALL).
  column_count = number of identifiers at same level i.e. not within a
                 subquery, with reftype = column.
  Todo: I think TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN  etc. is
        impossible if we've succeeded with hparse_f_opr_1 and statement
        is complete. But TOKEN_REFTYPE_COLUMN_OR_VARIABLE might occur.
*/
int MainWindow::hparse_f_expression_list(int who_is_calling)
{
  bool comma_is_seen;
  int i_of_opd;
  int column_count= 0;
  main_token_flags[hparse_i] |= TOKEN_FLAG_IS_START_IN_COLUMN_LIST;
  do
  {
    i_of_opd= hparse_i;
    comma_is_seen= false;
    if (who_is_calling == TOKEN_KEYWORD_SELECT) hparse_f_next_nexttoken();
    if (hparse_errno > 0) return 0;
    if (hparse_f_default(who_is_calling, false) == 1) {;}
    else if ((who_is_calling == TOKEN_KEYWORD_SELECT) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "*") == 1)) {;}
    else if ((who_is_calling == TOKEN_KEYWORD_SELECT)
          && (hparse_f_is_equal(hparse_next_token, "."))
          && (hparse_f_is_equal(hparse_next_next_token, "*"))
          && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "[identifier]")))
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ".");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "*");
    }
    else
    {
      hparse_f_opr_1(who_is_calling, 0);
    }
    if (hparse_errno > 0) return 0;
    for (;i_of_opd < hparse_i; ++i_of_opd)
    {
      if (main_token_types[i_of_opd] == TOKEN_KEYWORD_SELECT)
      {
        int parentheses_count= 0;
        QString token_in_select;
        for (;i_of_opd < hparse_i; ++i_of_opd)
        {
          token_in_select= hparse_text_copy.mid(main_token_offsets[i_of_opd], main_token_lengths[i_of_opd]);
          if (token_in_select == "(") ++parentheses_count;
          if (token_in_select == ")")
          {
            if (parentheses_count == 0) break;
            --parentheses_count;
          }
        }
      }
      if (hparse_f_is_identifier(main_token_types[i_of_opd]) == true)
      {
        if (hparse_text_copy.mid(main_token_offsets[i_of_opd], main_token_lengths[i_of_opd]) == "*")
          ++column_count;
        if ((main_token_reftypes[i_of_opd] == TOKEN_REFTYPE_COLUMN)
         || (main_token_reftypes[i_of_opd] == TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN)
         || (main_token_reftypes[i_of_opd] == TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN_OR_FUNCTION))
            ++column_count;
      }
    }
    if (who_is_calling == TOKEN_KEYWORD_SELECT)
    {
      bool as_seen= false;
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS") == 1) as_seen= true;
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ALIAS_OF_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) {;}
      else if (hparse_f_literal(TOKEN_REFTYPE_ALIAS_OF_COLUMN, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 1) {;}
      else if (as_seen == true) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","))
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_END_IN_COLUMN_LIST;
      comma_is_seen= true;
    }
  } while (comma_is_seen);
  return column_count;
}

/* e.g. (1,2,3) or ( (1,1), (2,2), (3,3) ) i.e. two parenthesization levels are okay */
void MainWindow::hparse_f_parenthesized_value_list()
{
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  do
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
    {
      do
      {
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
}

/*
  routine_type = procedure, function, lua, or row, or cursor
*/
void MainWindow::hparse_f_parameter_list(int routine_type)
{
  if ((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0)
  {
    if (routine_type == TOKEN_KEYWORD_CURSOR)
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") != 1)
        return;
      do
      {
        /* todo: should this be hparse_f_expect? */
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_PARAMETER_DEFINE,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
        {
          if (hparse_f_data_type(routine_type) == -1) hparse_f_error();
          if (hparse_errno > 0) return;
        }
      } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
      return;
    }
  }

  if ((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0)
  {
    if ((routine_type == TOKEN_KEYWORD_PROCEDURE) || (routine_type == TOKEN_KEYWORD_FUNCTION))
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") != 1)
        return;
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")") == 1)
        return;
      do
      {
        hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_PARAMETER_DEFINE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IN") == 1)
          hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OUT");
        else hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OUT");
        if (hparse_errno > 0) return;
        if (hparse_f_data_type(routine_type) == -1) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
      return;
    }
  }
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  do
  {
    bool in_seen= false;
    if (routine_type == TOKEN_KEYWORD_PROCEDURE)
    {
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OUT") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INOUT") == 1))
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
        in_seen= true;
      }
    }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_PARAMETER_DEFINE,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
    {
      if (routine_type != TOKEN_KEYWORD_LUA)
      {
        if (hparse_f_data_type(routine_type) == -1) hparse_f_error();
        if (hparse_errno > 0) return;
      }
    }
    else if (in_seen == true)
    {
      hparse_f_error();
      return;
    }
  } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_parenthesized_expression()
{
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  hparse_f_opr_1(0, 0);
  if (hparse_errno > 0) return;
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
}

/*
  Re int *expression_count:
  The point is: if there is more than 1, then this is only legal for comparisons,
  and both sides of the comparison should have the same count.
  But we aren't actually using this knowlede yet, because we don't count selection columns.
  Counting expressions in the select list is feasible, but "select *" causes difficulty.
*/
void MainWindow::hparse_f_parenthesized_multi_expression(int *expression_count)
{
  *expression_count= 0;
  //hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
  //if (hparse_errno > 0) return;
  if ((hparse_subquery_is_allowed == true)
   && (hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL)
   && (hparse_f_is_query(false) == true))
  {
    hparse_f_query(0, true, false, false);
    if (hparse_errno > 0) return;
    (*expression_count)= 2;          /* we didn't really count, but guess it's more than 1 */
  }
  else
  {
    do
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
      ++(*expression_count);
    } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  }
  //hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
  //if (hparse_errno > 0) return;
}

/* We've seen [ and with Tarantool that can mean array, possibly nested */
void MainWindow::hparse_f_bracketed_multi_expression()
{
  do
  {
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "]");
  //if (hparse_errno > 0) return;
}

/* For assignments we need to know if global|persist|etc. was stated. */
void MainWindow::hparse_f_is_global_or_persist(bool *global_seen, bool *persist_seen)
{
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GLOBAL") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SESSION") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCAL") == 1))
  {
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "="); /* guaranteed to fail */
    *global_seen= true;
  }
  else if ((hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PERSIST") == 1)
        || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PERSIST_ONLY") == 1))
  {
    *persist_seen= true;
  }
}


/* TODO: if statement_type <> TOKEN_KEYWORD_SET, disallow assignment to @@ or @ variables. */
void MainWindow::hparse_f_assignment(int statement_type,
                                     int clause_type,
                                     bool global_seen_earlier,
                                     bool persist_seen_earlier)
{
  bool comma_is_seen;
  bool global_seen;
  bool persist_seen;
  bool server_variable_seen;
  main_token_flags[hparse_i] |= TOKEN_FLAG_IS_START_IN_COLUMN_LIST;
  int counter= 0;
  do
  {
    ++counter;
    comma_is_seen= false;
    global_seen= false;
    persist_seen= false;
    server_variable_seen= false;
    if (statement_type == TOKEN_KEYWORD_SET)
    {
      {
        if (counter == 1)
        {
          global_seen= global_seen_earlier;
          persist_seen= persist_seen_earlier;
        }
        else hparse_f_is_global_or_persist(&global_seen, &persist_seen);
      }
    }
    if ((global_seen == false) && (persist_seen == false))
    {
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "@@SESSION") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "@@GLOBAL") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "@@LOCAL") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "@@PERSIST") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "@@PERSIST_ONLY") == 1))
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ".");
        if (hparse_errno > 0) return;
      }
    }
    if (hparse_errno > 0) return;

    /* Todo: Find out why sometimes we only look for identifier, because that won't set main_token_pointers[] */
    if ((statement_type == TOKEN_KEYWORD_SET)
     && ((hparse_dbms_mask & (FLAG_VERSION_MARIADB_10_3 | FLAG_VERSION_LUA_OUTPUT)) == 0))
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    }
    else if (statement_type == TOKEN_KEYWORD_PRAGMA)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    }
    else if (statement_type == TOKEN_KEYWORD_SET)
    {
      if (hparse_f_qualified_name_of_operand(0, true,false,false) == 0) hparse_f_error();
    }
    else /* TOKEN_KEYWORD_INSERT | UPDATE | LOAD */
    {
      if (hparse_f_qualified_name_of_operand(0, true,false,true) == 0) hparse_f_error();
    }
    if (hparse_errno > 0) return;

    if (statement_type == TOKEN_KEYWORD_SET)
    {
      if (global_seen|persist_seen) server_variable_seen= true;
      else
      {
        int j= hparse_i_of_last_accepted;
        QString t= hparse_text_copy.mid(main_token_offsets[j], main_token_lengths[j]);
        if (t.left(2) == "@@") server_variable_seen= true;
      }
    }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ":=");
    if (hparse_errno > 0) return;
    /* TODO: DEFAULT and ON and OFF shouldn't always be legal. */
    bool is_special_word_seen= false;
    if (hparse_f_default(statement_type, server_variable_seen) == 1)
      is_special_word_seen= true;
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON") == 1)
      is_special_word_seen= true;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OFF") == 1)
      is_special_word_seen= true;
    /* VALUE|VALUES should only be legal for INSERT ... ON DUPLICATE KEY */
    if (is_special_word_seen == false)
    {
      if (clause_type == TOKEN_KEYWORD_DUPLICATE) hparse_f_opr_1(0, ALLOW_FLAG_IS_IN_DUPLICATE_KEY);
      else hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","))
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_END_IN_COLUMN_LIST;
      comma_is_seen= true;
    }
  } while (comma_is_seen);
}

void MainWindow::hparse_f_alter_specification()
{
  hparse_f_table_or_partition_options(TOKEN_KEYWORD_TABLE);
  if (hparse_errno > 0) return;
  bool default_seen= false;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    default_seen= true;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ADD") == 1))
  {
    bool column_name_is_expected= false;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_TARANTOOL_2_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLUMN") == 1) column_name_is_expected= true;
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PARTITION") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return;
      /* todo: check that hparse_f_partition_or_subpartition_definition does as expected */
      hparse_f_partition_or_subpartition_definition(TOKEN_KEYWORD_PARTITION);
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_create_definition(TOKEN_KEYWORD_ALTER) == 3)
    {
      /* If "ADD", but not constraint, and MySQL|MariaDB, then column */
      if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0) hparse_f_error();
      column_name_is_expected= true;
    }
    if (hparse_errno > 0) return;
    if (column_name_is_expected == true)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        do
        {
          if (hparse_f_qualified_name_of_operand(TOKEN_FLAG_IS_NEW, false,false,true) == 0) hparse_f_error();
          if (hparse_errno > 0) return;
          hparse_f_column_definition(0);
          if (hparse_errno > 0) return;
        } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
      else
      {
        if (hparse_f_qualified_name_of_operand(TOKEN_FLAG_IS_NEW, false,false,true) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_column_definition(0);
        if (hparse_errno > 0) return;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIRST") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AFTER") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
          if (hparse_errno > 0) return;
        }
      }
    }
    return;
  }
  if (default_seen == false)
  {
    if (hparse_f_algorithm_or_lock() == 1) return;
    if (hparse_errno > 0) return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALTER") == 1))
  {
    if (hparse_f_index_if_exists(FLAG_VERSION_MARIADB_10_6 | FLAG_VERSION_MYSQL_8_0) == 1) /* ... alter ... alter index|key */
    {
      hparse_f_not_ignored(true);
      if (hparse_errno > 0) return;
    }
    else
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLUMN");
      if (hparse_f_qualified_name_of_operand(0, false,false,true) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SET") == 1)
      {
        if (hparse_f_default_clause(TOKEN_KEYWORD_ALTER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DROP") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT");
        if (hparse_errno > 0) return;
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      }
      else hparse_f_error();
    }
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ANALYZE") == 1))
  {
    if (hparse_f_partition_list(false, true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHANGE") == 1))
  {
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLUMN");
    if (hparse_f_qualified_name_of_operand(0, false,false,true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if (hparse_f_qualified_name_of_operand(0, false,false,true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    hparse_f_column_definition(0);
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIRST") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AFTER") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    return;
  }
  /* Todo: Following is useless code. CHARACTER SET is a table_option. Error in manual? */
  if ((hparse_f_character_set() == 1))
  {
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
    if (hparse_f_character_set_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_collation_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    return;
  }
  if (hparse_errno > 0) return;
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHECK") == 1))
  {
    if (hparse_f_partition_list(false, true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  /* "LOCK" is already handled by hparse_f_algorithm_or_lock() */
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COALESCE") == 1))
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PARTITION");
    if (hparse_errno > 0) return;
    if (hparse_f_literal(TOKEN_REFTYPE_PARTITION_NUMBER, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONVERT") == 1))
  {
    if (hparse_f_accept(FLAG_VERSION_MARIADB_10_7, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PARTITION, "PARTITION") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MARIADB_10_7, TOKEN_REFTYPE_PARTITION,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MARIADB_10_7, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TO, "TO");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MARIADB_10_7, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TABLE, "TABLE");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MARIADB_10_7, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      return;
    }
    if (hparse_f_accept(FLAG_VERSION_MARIADB_10_7, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TABLE, "TABLE") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MARIADB_10_7, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MARIADB_10_7, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TO, "TO");
      if (hparse_errno > 0) return;
      hparse_f_partition_or_subpartition_definition(TOKEN_KEYWORD_PARTITION);
      return;
    }
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TO");
    if (hparse_errno > 0) return;
    hparse_f_character_set();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 0)
    {
      if (hparse_f_character_set_name() == 0) hparse_f_error();
    }
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
    {
      if (hparse_f_collation_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISABLE") == 1))
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEYS");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISCARD") == 1))
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLESPACE") == 1) return;
    if (hparse_f_partition_list(false, true) == 0)
    {
      hparse_f_error();
      if (hparse_errno > 0) return;
    }
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLESPACE");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DROP") == 1))
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PRIMARY") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_index_if_exists(FLAG_VERSION_MYSQL_OR_MARIADB_ALL) == 1) /* alter ... drop index index|key */
    {
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOREIGN") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY");
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_CONSTRAINT, TOKEN_REFTYPE_CONSTRAINT) == 0) hparse_f_error(); /* parse_f_symbol */
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PARTITION") == 1)
    {
      /* todo: maybe use if (hparse_f_partition_list(true, false) == 0) hparse_f_error(); */
      do
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_PARTITION,TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
    else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONSTRAINT") == 1)
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_CONSTRAINT, TOKEN_REFTYPE_CONSTRAINT) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLUMN");
      if (hparse_f_qualified_name_of_operand(0, false,false,true) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENABLE") == 1))
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEYS");
    if (hparse_errno > 0) return;
    return;
  }
  if ((hparse_f_accept(FLAG_VERSION_TARANTOOL_2_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ENABLE, "ENABLE") == 1)
  || (hparse_f_accept(FLAG_VERSION_TARANTOOL_2_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISABLE") == 1))
  {
    hparse_f_expect(FLAG_VERSION_TARANTOOL_2_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CHECK, "CHECK");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_TARANTOOL_2_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CONSTRAINT, "CONSTRAINT");
    if (hparse_errno > 0) return;
    if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_CONSTRAINT, TOKEN_REFTYPE_CONSTRAINT) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXCHANGE") == 1))
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PARTITION");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_PARTITION,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITH");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE");
    if (hparse_errno > 0) return;
    if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_TABLE, TOKEN_TYPE_IDENTIFIER) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITH") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITHOUT") == 1))
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALIDATION");
      if (hparse_errno > 0) return;
    }
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FORCE") == 1))
  {
    return;
  }
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IMPORT") == 1))
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLESPACE") == 1) return;
    if (hparse_f_partition_list(false, true) == 0)
    {
      hparse_f_error();
      if (hparse_errno > 0) return;
    }
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLESPACE");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MODIFY") == 1))
  {
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLUMN");
    if (hparse_f_qualified_name_of_operand(0, false,false,true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    hparse_f_column_definition(0);
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIRST") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AFTER") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    return;
  }
  /* "LOCK" is already handled by hparse_f_algorithm_or_lock() */
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OPTIMIZE") == 1))
  {
    if (hparse_f_partition_list(false, true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ORDER") == 1)) /* todo: could use modified hparse_f_order_by */
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY");
    if (hparse_errno > 0) return;
    do
    {
      if (hparse_f_qualified_name_of_operand(0, false,false,true) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ASC") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DESC") == 1)) {;}
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REBUILD") == 1))
  {
    if (hparse_f_partition_list(false, true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REMOVE") == 1))
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PARTITIONING");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RENAME") == 1))
  {
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY") == 1))
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_INDEX, TOKEN_TYPE_IDENTIFIER,"[identifier]");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TO");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_INDEX, TOKEN_TYPE_IDENTIFIER,"[identifier]");
      if (hparse_errno > 0) return;
    }
    else
    {
      if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TO") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS") == 1)) {;}
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REORGANIZE") == 1))
  {
    if (hparse_f_partition_list(false, false) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INTO");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return;
    do
    {
      hparse_f_partition_or_subpartition_definition(TOKEN_KEYWORD_PARTITION);
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPAIR") == 1))
  {
    if (hparse_f_partition_list(false, true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TRUNCATE") == 1))
  {
    if (hparse_f_partition_list(false, true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UPGRADE") == 1))
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PARTITIONING");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITH") == 1))
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALIDATION");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITHOUT") == 1))
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALIDATION");
    if (hparse_errno > 0) return;
    return;
  }
}

/*
  accept "CHARACTER SET"
  but surprisingly often "CHARSET" can be used instead
*/
int MainWindow::hparse_f_character_set()
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHARACTER") == 1)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SET");
    if (hparse_errno > 0) return 0;
    return 1;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHARSET") == 1) return 1;
  else return 0;
}

/*
  For alter table ... alter|drop INDEX|KEY [IF EXISTS] index_name
  Return 1 i.e. accepted even if error. The caller must add if hparse_errno > 0
*/
int MainWindow::hparse_f_index_if_exists(unsigned int flag_version)
{
 if ((hparse_f_accept(flag_version, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX") == 1)
  || (hparse_f_accept(flag_version, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY") == 1))
 {
  if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_6) != 0)
  {
     hparse_f_if_exists();
     if (hparse_errno > 0) return 1;
  }
   hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_INDEX, TOKEN_TYPE_IDENTIFIER, "[identifier]");
   return 1;
 }
 return 0;
}

/* For alter table ... alter index ... [NOT] IGNORED if MariaDB, VISIBLE|INVISIBLE if MySQL */
int MainWindow::hparse_f_not_ignored(bool is_compulsory)
{
  if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_6) != 0)
  {
    int not_was_seen= hparse_f_accept(FLAG_VERSION_MARIADB_10_6, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_NOT, "NOT");
    if (hparse_f_accept(FLAG_VERSION_MARIADB_10_6, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORED") == 0)
    {
      if ((not_was_seen == 1) || (is_compulsory == true)) hparse_f_error();
      return 0;
    }
    return 1;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VISIBLE") == 1) return 1;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INVISIBLE") == 1) return 1;
  if (is_compulsory == true) hparse_f_error();
  return 0;
}

void MainWindow::hparse_f_alter_database()
{
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
  if (hparse_errno > 0) return;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UPGRADE") == 1)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATA");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DIRECTORY");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NAME");
    if (hparse_errno > 0) return;
  }
  else
  {
    bool character_seen= false, collate_seen= false;
    for (;;)
    {
      if ((character_seen == true) && (collate_seen == true)) break;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT"))
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      if ((character_seen == false) && (hparse_f_character_set() == 1))
      {
        character_seen= true;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=")) {;}
        if (hparse_f_character_set_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        continue;
      }
      if (hparse_errno > 0) return;
      if ((collate_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATE")))
      {
        collate_seen= true;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=")) {;}
      if (hparse_f_collation_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        continue;
      }
      if ((character_seen == false) && (collate_seen == false))
      {
        hparse_f_error();
        return;
      }
      break;
    }
  }
}

void MainWindow::hparse_f_characteristics(int object_type)
{
  bool comment_seen= false, language_seen= false, contains_seen= false, sql_seen= false;
  bool deterministic_seen= false;
  if (object_type == TOKEN_KEYWORD_PACKAGE) /* packages have fewer characteristics so consider some to be already seen */
  {
    language_seen= contains_seen= deterministic_seen= true;
  }
  for (;;)
  {
    if ((comment_seen) && (language_seen) && (contains_seen) && (sql_seen)) break;
    if ((comment_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COMMENT")))
    {
      comment_seen= true;
      if (hparse_f_literal(TOKEN_REFTYPE_COMMENT, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      continue;
    }
    else if ((language_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LANGUAGE")))
    {
      language_seen= true;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL");
      if (hparse_errno > 0) return;
      continue;
    }
    else if ((deterministic_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOT")))
    {
      deterministic_seen= true;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DETERMINISTIC");
      if (hparse_errno > 0) return;
      continue;
    }
    else if ((deterministic_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DETERMINISTIC")))
    {
      deterministic_seen= true;
      continue;
    }
    else if ((contains_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONTAINS")))
    {
      contains_seen= true;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL");
      if (hparse_errno > 0) return;
      continue;
    }
    else if ((contains_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NO")))
    {
      contains_seen= true;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL");
      if (hparse_errno > 0) return;
      continue;
    }
    else if ((contains_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "READS")))
    {
      contains_seen= true;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATA");
      if (hparse_errno > 0) return;
      continue;
     }
    else if ((contains_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MODIFIES")))
    {
      contains_seen= true;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATA");
      if (hparse_errno > 0) return;
      continue;
    }
    else if ((sql_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL")))
    {
      sql_seen= true;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SECURITY");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFINER") == 1) continue;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INVOKER");
      if (hparse_errno > 0) return;
      continue;
    }
    break;
  }
}

int MainWindow::hparse_f_algorithm_or_lock()
{
  bool algorithm_seen= false, lock_seen= false;
  for (;;)
  {
    if ((algorithm_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALGORITHM") == 1))
    {
      algorithm_seen= true;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=") == 1) {;}
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
        break;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INPLACE") == 1) break;
      if (hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COPY") == 1) break;
      if (hparse_errno > 0) return 0;
    }
    if ((lock_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCK") == 1))
    {
      lock_seen= true;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=") == 1) {;}
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
        break;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NONE") == 1) break;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SHARED") == 1) break;
      if (hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXCLUSIVE") == 1) break;
      if (hparse_errno > 0) return 0;
    }
    break;
  }
  if ((algorithm_seen == true) || (lock_seen == true)) return 1;
  return 0;
}

void MainWindow::hparse_f_definer()
{
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
  if (hparse_errno > 0) return;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURRENT_USER") == 1) {;}
  else if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 1) {;}
  else hparse_f_error();
}

/* IF EXISTS clause. Todo: Maybe we could fit qualified_name_of_object or accept(identifier) into this. */
void MainWindow::hparse_f_if_exists()
{
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
  }
  /* caller should add if (hparse_errno > 0) return; */
}

void MainWindow::hparse_f_if_not_exists()
{
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOT");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
    if (hparse_errno > 0) return;
  }
}

int MainWindow::hparse_f_analyze_or_optimize(int who_is_calling,int *table_or_view)
{
  if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0) *table_or_view= TOKEN_KEYWORD_TABLE;
  else
  {
    *table_or_view= 0;
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NO_WRITE_TO_BINLOG") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCAL") == 1)) {;}
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE") == 1) *table_or_view= TOKEN_KEYWORD_TABLE;
    else if ((who_is_calling == TOKEN_KEYWORD_REPAIR)
          && ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0)
          && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VIEW") == 1))
      *table_or_view= TOKEN_KEYWORD_VIEW;
    else return 0;
  }
  do
  {
    if (*table_or_view == TOKEN_KEYWORD_TABLE)
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0)
        hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_VIEW, TOKEN_REFTYPE_VIEW) == 0)
        hparse_f_error();
    }
    if (hparse_errno > 0) return 0;
  } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  return 1;
}

/*
  INSTALL (in which case we know it's MySQL|MariaDB, because client install uses INSERT INTO plugins)
*/
void MainWindow::hparse_f_install()
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PLUGIN") == 1)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_PLUGIN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
  }
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SONAME");
  if (hparse_errno > 0) return;
  if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_character_set_or_collate()
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ASCII") == 1) {;}
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BINARY") == 1) {;}
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNICODE") == 1) {;}
  else if (hparse_f_character_set() == 1)
  {
    if (hparse_f_character_set_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_errno > 0) return;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
  {
    if (hparse_f_collation_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
}

/* Used for data type length. Might be useful for any case of "(" integer ")" */
/* Return 1 if there was a length. This also checks for character set and collate clauses. */
int MainWindow::hparse_f_length(bool is_ok_if_decimal, bool is_ok_if_unsigned, bool is_ok_if_binary)
{
  int return_value= 0;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_NOT_AFTER_SPACE;
    if (hparse_f_literal(TOKEN_REFTYPE_LENGTH, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    if (is_ok_if_decimal)
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1)
      {
        if (hparse_f_literal(TOKEN_REFTYPE_SCALE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
    }
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return 0;
    return_value= 1;
  }
  if (is_ok_if_unsigned)
  {
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNSIGNED") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SIGNED") == 1)) {;}
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ZEROFILL");
  }
  if (is_ok_if_binary)
  {
    hparse_f_character_set_or_collate();
    if (hparse_errno > 0) return 0;
  }
  return return_value;
}

void MainWindow::hparse_f_enum_or_set()
{
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  do
  {
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
  hparse_f_character_set_or_collate();
  if (hparse_errno > 0) return;
}

/*
  Called for column_definition (context=TOKEN_KEYWORD_COLUMN), casting
  (TOKEN_KEYWORD_CAST), declared-variable definition (TOKEN_KEYWORD_DECLARE),
  function-return (TOKEN_KEYWORD_RETURNS), and parameter-definition
  in either create-function of create-procedure (TOKEN_KEYWORD_PROCEDURE).
  Todo: we are not distinguishing for the different data-type syntaxes,
  for example in CAST "UNSIGNED INT" is okay but "INT UNSIGNED" is illegal,
  while in CREATE "UNSIGNED INT" is illegal but "UNSIGNED INT" is okay.
  We allow any combination.
  Todo: we no longer need to allow "UNSIGNED INT" for non-CAST.
  Todo: notice how CAST checks are clean and ordered? Do the same for non-CAST.
  Todo: with MariaDB DATE and DATETIME can be followed by (n), both for CAST and for non-CAST.
  Todo: we check for length if VARCHAR, but don't check for variant spellings
*/
int MainWindow::hparse_f_data_type(int context)
{
  if ((context == TOKEN_KEYWORD_CAST) && ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0))
  {
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BINARY, "BINARY") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CHAR, "CHAR") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CHAR, "CHARACTER") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DATE, "DATE") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DATETIME, "DATETIME") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DECIMAL, "DEC") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DECIMAL, "DECIMAL") == 1)
     || (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DOUBLE, "DOUBLE") == 1)
     || (hparse_f_accept(FLAG_VERSION_MARIADB_10_11, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INET4, "INET4") == 1)
     || (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INTEGER, "INT") == 1)
     || (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INTEGER, "INTEGER") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_JSON, "JSON") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_NCHAR, "NCHAR") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SIGNED, "SIGNED") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TIME, "TIME") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_UNSIGNED, "UNSIGNED") == 1)
     || (hparse_f_accept(FLAG_VERSION_MARIADB_10_11, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_UUID, "UUID") == 1))
    {
      int k= main_token_types[hparse_i_of_last_accepted];
      main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      if ((k == TOKEN_KEYWORD_SIGNED) || (k == TOKEN_KEYWORD_UNSIGNED))
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INTEGER, "INT") == 0)
        hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INTEGER, "INTEGER");
      }
      if ((k == TOKEN_KEYWORD_BINARY) || (k == TOKEN_KEYWORD_CHAR) || (k == TOKEN_KEYWORD_NCHAR)
       || ((k == TOKEN_KEYWORD_DATETIME) && ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0))
       || ((k == TOKEN_KEYWORD_TIME) && ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0)))
      {
        hparse_f_length(false, false, false);
        if (hparse_errno > 0) return 0;
        if (k == TOKEN_KEYWORD_CHAR)
        {
          if (hparse_f_character_set() == 1)
          {
            hparse_f_character_set_name();
          }
          if (hparse_errno > 0) return 0;
        }
      }
      if (k == TOKEN_KEYWORD_DECIMAL)
      {
        hparse_f_length(true, true, false);
      }
      if (hparse_errno > 0) return 0;
      return k;
    }
    return -1;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BIT") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_BIT;
  }
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TINYINT") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BOOLEAN") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INT1") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_TINYINT;
  }
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SMALLINT") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INT2") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_SMALLINT;
  }
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MEDIUMINT") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INT3") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MIDDLEINT") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_MEDIUMINT;
  }
  if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INT") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INT4") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_INT;
  }
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INTEGER") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0)
    {
      hparse_f_length(false, true, false);
      if (hparse_errno > 0) return 0;
    }
    return TOKEN_KEYWORD_INTEGER;
  }
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BIGINT") == 1)|| (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INT8") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_BIGINT;
  }
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REAL") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(true, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_REAL;
  }
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DOUBLE") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PRECISION");
    hparse_f_length(true, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_DOUBLE;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FLOAT8") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(true, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_FLOAT8;
  }
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FLOAT") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FLOAT4") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(true, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_FLOAT4;
  }
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_TARANTOOL_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DECIMAL") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_TARANTOOL_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEC") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIXED") == 1)
   || (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DECIMAL, "NUMBER") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0)
    {
      hparse_f_length(true, true, false);
      if (hparse_errno > 0) return 0;
    }
    return TOKEN_KEYWORD_DECIMAL;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NUMERIC") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(true, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_NUMERIC;
  }
  /* Todo: looks like this might return unsigned if we say "signed" */
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_TARANTOOL_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNSIGNED") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SIGNED") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INT") == 0) hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INTEGER");
      hparse_f_length(false, true, false);
      if (hparse_errno > 0) return 0;
    }
    return TOKEN_KEYWORD_UNSIGNED;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SERIAL") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_SERIAL;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATE") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_DATE;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TIME") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_TIME;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TIMESTAMP") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_TIMESTAMP;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATETIME") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_DATETIME;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "YEAR") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_YEAR;
  }
  int hparse_i_of_char= hparse_i;
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHAR") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHARACTER") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    main_token_flags[hparse_i_of_char] &= (~TOKEN_FLAG_IS_FUNCTION);
    bool byte_seen= false, varying_seen= false;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BYTE") == 1)
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
      byte_seen= true;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARYING") == 1)
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
      varying_seen= true;
    }
    if (byte_seen == false) hparse_f_length(false, false, true);
    if (hparse_errno > 0) return 0;
    if (varying_seen == true) return TOKEN_KEYWORD_VARCHAR;
    return TOKEN_KEYWORD_CHAR;
  }
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARCHAR") == 1)
   || (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_VARCHAR, "VARCHAR2") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    if (hparse_f_length(false, false, true) == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_VARCHAR;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARCHARACTER") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, false, true);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_VARCHARACTER;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NCHAR") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARYING");
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
      if (hparse_f_collation_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    return TOKEN_KEYWORD_CHAR;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NVARCHAR") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
      if (hparse_f_collation_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    return TOKEN_KEYWORD_CHAR;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NATIONAL") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    bool varchar_seen= false;
    hparse_i_of_char= hparse_i;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHAR") == 1)
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
      main_token_flags[hparse_i_of_char] &= (~TOKEN_FLAG_IS_FUNCTION);
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHARACTER") == 1)
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARCHAR") == 1) varchar_seen= true;
    else hparse_f_error();
    if (hparse_errno > 0) return 0;
    if (varchar_seen == false) hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARYING");
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
      if (hparse_f_collation_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    return TOKEN_KEYWORD_CHAR;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LONG") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARBINARY") == 1)
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
      return TOKEN_KEYWORD_MEDIUMBLOB;
    }
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARCHAR") == 1)
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    }
    hparse_f_character_set_or_collate();
    return TOKEN_KEYWORD_MEDIUMTEXT; /* LONG and LONG VARCHAR are synonyms for MEDIUMTEXT */
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BINARY") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_BINARY;
  }
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARBINARY") == 1)
   || (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_VARBINARY, "RAW") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_VARBINARY;
  }
  if (hparse_f_accept(FLAG_VERSION_MARIADB_11_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VECTOR") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_VECTOR;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TINYBLOB") == 1) return TOKEN_KEYWORD_TINYBLOB;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BLOB") == 1) return TOKEN_KEYWORD_BLOB;
  if (hparse_f_accept(FLAG_VERSION_TARANTOOL_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARBINARY") == 1) return TOKEN_KEYWORD_VARBINARY;
  if (hparse_f_accept(FLAG_VERSION_TARANTOOL_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NUMBER") == 1) return TOKEN_KEYWORD_NUMBER;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MEDIUMBLOB") == 1) return TOKEN_KEYWORD_MEDIUMBLOB;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LONGBLOB") == 1) return TOKEN_KEYWORD_LONGBLOB;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TINYTEXT") == 1)
  {
    hparse_f_character_set_or_collate();
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_TINYTEXT;
  }
  if ((hparse_f_accept(FLAG_VERSION_TARANTOOL_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STRING") == 1)
     || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SCALAR") == 1)
     || (hparse_f_accept(FLAG_VERSION_TARANTOOL_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TEXT") == 1)
     || (hparse_f_accept(FLAG_VERSION_TARANTOOL_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARCHAR") == 1))
  {
    int data_type= main_token_types[hparse_i_of_last_accepted];
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    if (data_type == TOKEN_KEYWORD_VARCHAR)
    {
      if (hparse_f_length(false, false, false) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    hparse_f_character_set_or_collate();
    if (hparse_errno > 0) return 0;
    return data_type;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TEXT") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_length(false, false, true);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_MEDIUMTEXT; /* todo: figure out: should this be TOKEN_KEYWORD_TEXT? */
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MEDIUMTEXT") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_character_set_or_collate();
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_MEDIUMTEXT;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LONGTEXT") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_character_set_or_collate();
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_LONGTEXT;
  }
  if (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LONGTEXT, "CLOB") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_LONGTEXT;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENUM") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_enum_or_set();
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_ENUM;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SET") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    hparse_f_enum_or_set();
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_SET;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "JSON") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return 0; /* todo: token_keyword */
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GEOMETRY") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_GEOMETRY;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "POINT") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_POINT;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LINESTRING") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_LINESTRING;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "POLYGON") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_POLYGON;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MULTIPOINT") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_MULTIPOINT;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MULTIPOLYGON") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_MULTIPOLYGON;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GEOMETRYCOLLECTION") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_GEOMETRYCOLLECTION;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LINESTRING") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_LINESTRING;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "POLYGON") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_POLYGON;
  }
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BOOL") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_BOOL;
  }
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BOOLEAN") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_BOOLEAN;
  }

  if (hparse_f_accept(FLAG_VERSION_MARIADB_10_7|FLAG_VERSION_TARANTOOL_2_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UUID") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_UUID;
  }

  if (hparse_f_accept(FLAG_VERSION_MARIADB_10_11, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INET4") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return TOKEN_KEYWORD_INET4;
  }

  /*
    Todo: The idea here -- return main_token_types[hparse_i_of_last_accepted] instead of a TOKEN_KEYWORD
          literal -- is applicable to some other cases in hparse_f_data_type, and would make code a bit shorter.
    Todo: these are really Tarantool 2.10 types, but see comments before "#define TARANTOOL_DATETIMES".
    Todo: currently this isn't just for CAST it can also be for column type, but check context again someday.
  */
  if ((hparse_f_accept(FLAG_VERSION_TARANTOOL_2_10, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ANY, "MAP") == 1)
   || (hparse_f_accept(FLAG_VERSION_TARANTOOL_2_10, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ARRAY, "ARRAY") == 1)
   || (hparse_f_accept(FLAG_VERSION_TARANTOOL_2_10, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_MAP, "ANY") == 1)
   || (hparse_f_accept(FLAG_VERSION_TARANTOOL_2_10*TARANTOOL_DATETIMES, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DATETIME, "DATETIME") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
    return main_token_types[hparse_i_of_last_accepted];
  }

  if (context == TOKEN_KEYWORD_DECLARE)
  {
    bool is_row_seen= false;
    bool is_type_seen= false;
    if (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ROW, "ROW") == 1)
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
      is_row_seen= true;
    }
    if (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TYPE, "TYPE") == 1)
    {
      if (is_row_seen == false) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
      is_type_seen= true;
    }
    if (is_type_seen == true)
    {
      hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OF");
      if (hparse_errno > 0) return -1;
      if (is_row_seen == true)
      {
        /* Actually, following could be either table or cursor */
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE,TOKEN_REFTYPE_TABLE) == 0)
        {
          hparse_f_error();
          return -1;
        }
        return TOKEN_KEYWORD_ROW;
      }
      else /* is_row_seen == false */ /* todo: actually at least one qualifier should be compulsory here */
      {
        if (hparse_f_qualified_name_of_operand(0, false,false,true) == 0) { hparse_f_error(); return -1; }
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
        return TOKEN_KEYWORD_VARCHAR; /* todo: VARCHAR is just filler, try to see what column data type really is */
      }
    }
    if ((is_row_seen == true) && (is_type_seen == false))
    {
      hparse_f_parameter_list(TOKEN_KEYWORD_ROW);
      if (hparse_errno > 0) return -1;
      return TOKEN_KEYWORD_ROW;
    }
  }

  /* If SQLite-style column definition, anything unreserved is acceptable. */
  /* So there was special handling here, until 2018-11-05. */
  /* But Tarantool has changed that, so we can look for real data types now. */
  return -1; /* -1 means error unless SQLite-style column definition */
}

/* In fact InnoDB won't accept SET DEFAULT but MySQL/MariaDB parsers accept it, so we do. */
void MainWindow::hparse_f_reference_option()
{
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RESTRICT") == 1) {;}
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CASCADE") == 1) {;}
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SET") == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 0)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NULL");
    }
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NO") == 1)
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ACTION");
    if (hparse_errno > 0) return;
  }
  else hparse_f_error();
  if (hparse_errno > 0) return;
}

/* todo: Check re Tarantool: last I saw it accepts but ignores MATCH, that will probably change */
int MainWindow::hparse_f_reference_definition()
{
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REFERENCES") == 0)
    return 0;
  if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
  if (hparse_errno > 0) return 0;
  hparse_f_column_list(0, 0);
  if (hparse_errno > 0) return 0;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MATCH") == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FULL") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PARTIAL") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SIMPLE") == 1) {;}
    else hparse_f_error();
    if (hparse_errno > 0) return 0;
  }
  bool on_delete_seen= false, on_update_seen= false;
  while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON") == 1)
  {
    if ((on_delete_seen == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DELETE") == 1))
    {
      hparse_f_reference_option();
      if (hparse_errno > 0) return 0;
      on_delete_seen= true;
    }
    else if ((on_update_seen == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UPDATE") == 1))
    {
      hparse_f_reference_option();
      if (hparse_errno > 0) return 0;
      on_update_seen= true;
    }
    else hparse_f_error();
  }
  return 1;
}

/*
     {INDEX|KEY}                    [index_name] [index_type] (index_col_name,...) [index_option] ...
     {FULLTEXT|SPATIAL} [INDEX|KEY] [index_name]              (index_col_name,...) [index_option] ...
  [] PRIMARY KEY                    [index_name  [index_type] (index_col_name,...) [index_option] ...
  [] UNIQUE             [INDEX|KEY] [index_name] [index_type] (index_col_name,...) [index_option] ...
  [] FOREIGN KEY                    [index_name]              (index_col_name,...) reference_definition
  [] CHECK (expression)
  In the above chart, [] is short for [CONSTRAINT x].
  The manual says [] i.e. [CONSTRAINT x] is not allowed for CHECK; actually it is; ignored.
  The manual says [index_name] is not allowed for PRIMARY KEY, actually it is, ignored.
  Return 1 if valid constraint definition, 2 if error, 3 if nothing (probably data type).
*/
int MainWindow::hparse_f_create_definition(int keyword)
{
  bool constraint_seen= false;
  bool fulltext_seen= false, foreign_seen= false;
  bool unique_seen= false, check_seen= false, primary_seen= false, vector_seen= false;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONSTRAINT") == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_CONSTRAINT,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
    {
      if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0) hparse_f_error();
      if (hparse_errno > 0) return 2;
    }
    constraint_seen= true;
  }
  else
  {
    if ((keyword == TOKEN_KEYWORD_ALTER) && ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)) return 3;
  }
  if ((constraint_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX") == 1)) {;}
  else if ((constraint_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY") == 1)) {;}
  else if ((constraint_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FULLTEXT") == 1)) fulltext_seen= true;
  else if ((constraint_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SPATIAL") == 1)) fulltext_seen= true;
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PRIMARY") == 1)
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY");
    if (hparse_errno > 0) return 2;
    primary_seen= true;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNIQUE") == 1) unique_seen= true;
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOREIGN") == 1)
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY");
    if (hparse_errno > 0) return 2;
    foreign_seen= true;
  }
  else if (((keyword != TOKEN_KEYWORD_ALTER) || (hparse_dbms_mask & FLAG_VERSION_TARANTOOL) == 0)
        && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHECK") == 1)) check_seen= true;
  else if (((keyword == TOKEN_KEYWORD_ALTER) && (hparse_dbms_mask & FLAG_VERSION_TARANTOOL_2_2) != 0)
        && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHECK") == 1)) check_seen= true;
  else if (hparse_f_accept(FLAG_VERSION_MARIADB_11_7, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_VECTOR, "VECTOR") == 1)vector_seen= true;
  else return 3;
  if (check_seen == true)
  {
    hparse_f_parenthesized_expression();
    if (hparse_errno > 0) return 2;
#ifdef ALLOW_CONFLICT_CLAUSES
    if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
    {
      hparse_f_conflict_clause();
      if (hparse_errno > 0) return 2;
    }
#endif
    return 1;
  }
  if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0)
  {
    if (vector_seen == true)
    {
      hparse_f_expect(FLAG_VERSION_MARIADB_11_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX");
      if (hparse_errno > 0) return 2;
    }
    if ((fulltext_seen == true) || (unique_seen == true))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX") == 1) {;}
      else hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY");
    }
    hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_CONSTRAINT, TOKEN_REFTYPE_CONSTRAINT);
    if (hparse_errno > 0) return 2;
  }
  hparse_f_index_columns(TOKEN_KEYWORD_TABLE, fulltext_seen, foreign_seen, primary_seen, vector_seen);
  if (hparse_errno > 0) return 2;
  return 1;
}

/*
  For column or variable definition: DEFAULT literal | (expression)
  Subqueries not allowed. Variables not allowed.
  Multi-operand e.g. 1+1 not allowed unless parenthesized.
  Sometimes function allowed. But preferably literal.
  Return 1 if "default" seen, even if subsequent error. Else return 0.
  Don't confuse with hparse_f_default().
  Todo: bug: sometimes we haven't picked up data_type so we pass 0
             (but actually we never use data_type anyway)
  Todo: Check: if PLSQL, is it true both := and DEFAULT are acceptable?
  Todo: hparse_f_opr_18 allows variables, it shouldn't
*/
int MainWindow::hparse_f_default_clause(int who_is_calling)
{
  if ((who_is_calling == TOKEN_KEYWORD_DECLARE)
   && (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ":=") == 1))
    {;}
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 1)
    {;}
  else return 0;
  main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
  bool saved_subquery_is_allowed= hparse_subquery_is_allowed;
  hparse_subquery_is_allowed= false;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
  {
    hparse_f_opr_1(who_is_calling, 0);
    if (hparse_errno > 0) goto e;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) goto e;
  }
  else
  {
    /* if (data_type == data_type) */
    {
      hparse_f_opr_18(who_is_calling, 0);
      if (hparse_errno > 0) goto e;
    }
  }
e:
  hparse_subquery_is_allowed= saved_subquery_is_allowed;
  return 1;
}

/*
  In column_definition, after datetime|timestamp default|on update,
  current_timestamp or one of its synonyms might appear. Ugly.
  Asking for 0-6 may be too fussy, MySQL accepts 9 but ignores it.
*/
int MainWindow::hparse_f_current_timestamp()
{
  int keyword;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURRENT_TIMESTAMP") == 1) keyword= TOKEN_KEYWORD_CURRENT_TIMESTAMP;
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCALTIME") == 1) keyword= TOKEN_KEYWORD_LOCALTIME;
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCALTIMESTAMP") == 1) keyword= TOKEN_KEYWORD_LOCALTIMESTAMP;
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOW") == 1) keyword= TOKEN_KEYWORD_NOW;
  else return 0;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "0") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "1") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "2") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "3") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "4") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "5") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "6") == 1) {;}
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, ")");
    if (hparse_errno > 0) return 0;
    return 1;
  }
  else if (keyword == TOKEN_KEYWORD_NOW) hparse_f_error();
  if (hparse_errno > 0) return 0;
  return 1;
}

/*
  The clause order for column definitions is what MySQL 5.7
  accepts, which differs from what the MySQL 5.7 manual says.
*/
void MainWindow::hparse_f_column_definition(int last_word)
{
  int data_type= hparse_f_data_type(TOKEN_KEYWORD_COLUMN);
  if (data_type == -1) hparse_f_error();
  if (hparse_errno > 0) return;
  bool generated_seen= false;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GENERATED") == 1)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALWAYS");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS");
    if (hparse_errno > 0) return;
    generated_seen= true;
  }
  else if (last_word == TOKEN_KEYWORD_START) hparse_f_error();
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS") == 1)
  {
    generated_seen= true;
  }
  if (hparse_errno != 0) return;
  if (generated_seen == true)
  {
    if (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ROW, "ROW") == 1)
    {
      if ((last_word == 0) && (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_START, "START") == 1))
        {;}
      else if ((last_word == TOKEN_KEYWORD_START) && (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "END") == 1))
        {;}
      else hparse_f_error();
    }
    else if (last_word == TOKEN_KEYWORD_START) hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "ROW");
    else
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return;
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
      if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VIRTUAL") == 0)
          hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PERSISTENT");
      }
      if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_ALL) != 0)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VIRTUAL") == 0)
          hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STORED");
      }
    }
  }
  if (hparse_errno != 0) return;
  bool null_seen= false, default_seen= false, auto_increment_seen= false;
  bool unique_seen= false, primary_seen= false, comment_seen= false, column_format_seen= false;
  bool on_seen= false, invisible_seen= false, constraint_seen= false;
  for (;;)
  {
    constraint_seen= false;
    if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONSTRAINT") == 1)
    {
      hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_CONSTRAINT,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      constraint_seen= true;
    }
    if ((null_seen == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOT") == 1))
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NULL");
      if (hparse_errno > 0) return;
      null_seen= true;
#ifdef ALLOW_CONFLICT_CLAUSES
      hparse_f_conflict_clause();
      if (hparse_errno > 0) return;
#endif
    }
    else if ((null_seen == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NULL") == 1))
    {
      null_seen= true; /* constraint can = NULL = silliness. For Tarantool it's undocumented. */
    }
    else if ((generated_seen == false) && (default_seen == false) && (hparse_f_default_clause(TOKEN_KEYWORD_CREATE) == 1))
    {
      if (hparse_errno > 0) return;
      default_seen= true;
    }
    else if ((generated_seen == false) && (auto_increment_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AUTO_INCREMENT") == 1))
    {
      auto_increment_seen= true;
    }
    else if ((generated_seen == false) && (auto_increment_seen == false) /* && (primary_seen == true) */ && ((data_type == TOKEN_KEYWORD_INTEGER || data_type == TOKEN_KEYWORD_INT || data_type == TOKEN_KEYWORD_UNSIGNED)) && (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AUTOINCREMENT") == 1))
    {
      auto_increment_seen= true;
    }
    else if ((invisible_seen == false) && (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INVISIBLE") == 1))
    {
      invisible_seen= true;
    }
    else if ((unique_seen == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNIQUE") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY");
      unique_seen= true;
    }
    else if ((primary_seen == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PRIMARY") == 1))
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY");
      if (hparse_errno > 0) return;
      primary_seen= true;
    }
    else if ((primary_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY") == 1))
    {
      primary_seen= true;
    }
    else if ((comment_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COMMENT") == 1))
    {
      if (hparse_f_literal(TOKEN_REFTYPE_COMMENT, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      comment_seen= true;
    }
    else if ((generated_seen == false) && (column_format_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLUMN_FORMAT") == 1))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIXED") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DYNAMIC") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 1)
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      else hparse_f_error();
      if (hparse_errno > 0) return;
      column_format_seen= true;
    }
    else if ((on_seen == false) && (generated_seen == false)
             && ((data_type == TOKEN_KEYWORD_TIMESTAMP) || (data_type == TOKEN_KEYWORD_DATETIME))
             && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON") == 1))
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UPDATE");
      if (hparse_errno > 0) return;
      if (hparse_f_current_timestamp() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      on_seen= true;
    }
    else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHECK") == 1)
    {
      hparse_f_parenthesized_expression();
      if (hparse_errno > 0) return;
    }
    else break;
  }
  if (((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0) && (constraint_seen == true))
  {
    hparse_f_error();
    return;
  }
  if (generated_seen == false)
  {
    hparse_f_reference_definition();
    if (hparse_errno > 0) return;
  }
}

/* Actually this could take COMMENT 'x' or (if extra) ATTRIBUTE 'x', and allow for other such simple clauses */
void MainWindow::hparse_f_comment(int extra)
{
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COMMENT") == 1)
   || ((extra == TOKEN_KEYWORD_ATTRIBUTE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ATTRIBUTE") == 1)))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    if (hparse_f_literal(TOKEN_REFTYPE_COMMENT, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
  }
}

void MainWindow::hparse_f_column_list(int is_compulsory, int is_maybe_qualified)
{
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 0)
  {
    if (is_compulsory == 1) hparse_f_error();
    return;
  }
  do
  {
    if (is_maybe_qualified == 0)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    else
    {
      if (hparse_f_qualified_name_of_operand(0, false,false,true) == 0) hparse_f_error();
    }
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
}

/*
  engine = engine_name part of either CREATE TABLE or CREATE TABLESPACE.
  Usually it will be a standard engine like MyISAM or InnoDB, but with MariaDB
  there are usually more choices ... in the end, we allow any identifier.
  Although it's undocumented, ENGINE = 'literal' is okay too.
*/
void MainWindow::hparse_f_engine()
{
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=") == 1) {;}
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "ARCHIVE") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "CSV") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "EXAMPLE") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "FEDERATED") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "HEAP") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "INNODB") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "MEMORY") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "MERGE") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "MYISAM") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "NDB") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_LITERAL, "[literal]") == 1) {;}
    else hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
}

void MainWindow::hparse_f_table_or_partition_options(int keyword)
{
  bool comma_seen= false;
  for (;;)
  {
    if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AUTO_INCREMENT") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_literal(TOKEN_REFTYPE_AUTO_INCREMENT, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();

      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AVG_ROW_LENGTH") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_literal(TOKEN_REFTYPE_LENGTH, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();

      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_character_set() == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      hparse_f_character_set_name();
      if (hparse_errno > 0) return;
    }
    else if (hparse_errno > 0) return;
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHECKSUM") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "0") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "1");
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATE") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_collation_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COMMENT") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_literal(TOKEN_REFTYPE_COMMENT, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();

      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COMPRESSION")) == 1)
    {
      /* todo: should be: 'zlib', 'lz4', or 'none' but some other things e.g. NO seem to be accepted */
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();

      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONNECTION")) == 1)
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();

      if (hparse_errno > 0) return;
    }
    else if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATA") == 1))
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DIRECTORY");
      if (hparse_errno > 0) return;
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_literal(TOKEN_REFTYPE_DIRECTORY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();

      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 1))
    {
      main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      if (hparse_f_character_set() == 1)
      {
        hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
        hparse_f_character_set_name();
        if (hparse_errno > 0) return;
      }
      else if (hparse_errno > 0) return;
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
      {
        hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
        if (hparse_f_collation_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DELAY_KEY_WRITE") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "0") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "1");
      if (hparse_errno > 0) return;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENCRYPTED") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "YES") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NO");
      if (hparse_errno > 0) return;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MYSQL_ALL) != 0) && (keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENCRYPTION") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENCRYPTION_KEY_ID") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();

      if (hparse_errno > 0) return;
    }
    else if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENGINE") == 1))
    {
      hparse_f_engine();
      if (hparse_errno > 0) return;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IETF_QUOTES") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "YES") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NO");
      if (hparse_errno > 0) return;
    }
    else if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX") == 1))
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DIRECTORY");
      if (hparse_errno > 0) return;
      {
        hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
        if (hparse_f_literal(TOKEN_REFTYPE_DIRECTORY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();

        if (hparse_errno > 0) return;
      }
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INSERT_METHOD") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NO") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIRST") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LAST") == 1)) {;}
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY_BLOCK_SIZE") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_literal(TOKEN_REFTYPE_LENGTH, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();

      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MAX_ROWS") == 1)
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();

      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MIN_ROWS") == 1)
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();

      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PACK_KEYS") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "0") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "1") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 1))
         main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      else hparse_f_error();
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PAGE_CHECKSUM") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "0") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "1");
      else hparse_f_error();
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PASSWORD") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_literal(TOKEN_REFTYPE_PASSWORD, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();

      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROW_FORMAT") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DYNAMIC") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIXED") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COMPRESSED") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REDUNDANT") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COMPACT") == 1)
       || (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PAGE") == 1)))
         main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      else hparse_f_error();
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATS_AUTO_RECALC") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "0") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "1") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 1))
         main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      else hparse_f_error();
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATS_PERSISTENT") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "0") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "1") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 1))
         main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      else hparse_f_error();
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATS_SAMPLE_PAGES") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DEFAULT, "DEFAULT") == 0)
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_PARTITION) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STORAGE") == 1))
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENGINE");
      if (hparse_errno > 0) return;
      hparse_f_engine();
      if (hparse_errno > 0) return;
    }
    else if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLESPACE") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_TABLESPACE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TRANSACTIONAL") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "0") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "1");
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNION") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return;
      do
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WITH, "WITH") == 1))
    {
      hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SYSTEM, "SYSTEM");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_VERSIONING, "VERSIONING");
      if (hparse_errno > 0) return;
    }
    else
    {
      if (comma_seen == false) break;
      hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if (keyword == TOKEN_KEYWORD_TABLE)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1) comma_seen= true;
      else comma_seen= false;
    }
  }
}

void MainWindow::hparse_f_partition_options()
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PARTITION") == 1)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY");
    if (hparse_errno > 0) return;
    hparse_f_partition_or_subpartition(TOKEN_KEYWORD_PARTITION);
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PARTITIONS") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SUBPARTITION") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return;
      hparse_f_partition_or_subpartition(0);
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SUBPARTITIONS") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
    {
      do
      {
        hparse_f_partition_or_subpartition_definition(TOKEN_KEYWORD_PARTITION);
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
  }
}

void MainWindow::hparse_f_partition_or_subpartition(int keyword)
{
  bool linear_seen= false;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LINEAR") == 1) linear_seen= true;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HASH") == 1)
  {
    hparse_f_parenthesized_expression();
     if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY") == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALGORITHM") == 1)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL_WITH_DIGIT, "1") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL_WITH_DIGIT, "2") == 1) {;}
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
    hparse_f_column_list(1, 0);
    if (hparse_errno > 0) return;
  }
  else if (((linear_seen == false) && (keyword == TOKEN_KEYWORD_PARTITION))
        && ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RANGE") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LIST") == 1)))
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLUMNS") == 1)
    {
       hparse_f_column_list(1, 0);
       if (hparse_errno > 0) return;
    }
    else
    {
       hparse_f_parenthesized_expression();
       if (hparse_errno > 0) return;
    }
  }
  else hparse_f_error();
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_partition_or_subpartition_definition(int keyword)
{
  if (keyword == TOKEN_KEYWORD_PARTITION)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PARTITION");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_PARTITION, TOKEN_TYPE_IDENTIFIER, "[identifier]");
  }
  else
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SUBPARTITION");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_SUBPARTITION, TOKEN_TYPE_IDENTIFIER, "[identifier]");
  }
  if (hparse_errno > 0) return;
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALUES") == 1))
  {
    /* Todo: LESS THAN only for RANGE; IN only for LIST. Right? */
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LESS") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "THAN");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MAXVALUE") == 1) {;}
      else
      {
        /* todo: supposedly this can be either expression or value-list. we take expression-list. */
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
        if (hparse_errno > 0) return;
        do
        {
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MAXVALUE") == 1) {;}
          else hparse_f_opr_1(0, 0);
          if (hparse_errno > 0) return;
        } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      }
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IN") == 1)
    {
      hparse_f_parenthesized_value_list();
      if (hparse_errno > 0) return;
    }
    hparse_f_table_or_partition_options(TOKEN_KEYWORD_PARTITION);
    if (hparse_errno > 0) return;
    if ((keyword == TOKEN_KEYWORD_PARTITION) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1))
    {
      hparse_f_partition_or_subpartition_definition(0);
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
  }
}

int MainWindow::hparse_f_partition_list(bool is_parenthesized, bool is_maybe_all)
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PARTITION") == 1)
  {
    if (is_parenthesized)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return 0;
    }
    if ((is_maybe_all) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1)) {;}
    else
    {
      do
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_PARTITION,TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return 0;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
    if (is_parenthesized)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return 0;
    }
    return 1;
  }
  return 0;
}

/* "ALGORITHM" seen, which must mean we're in ALTER VIEW or CREATE VIEW */
void MainWindow::hparse_f_algorithm()
{
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
  if (hparse_errno > 0) return;
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNDEFINED") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MERGE") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TEMPTABLE") == 1))
     {;}
  else hparse_f_error();
}

/* "SQL" seen, which must mean we're in ALTER VIEW or CREATE VIEW */
void MainWindow::hparse_f_sql()
{
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SECURITY");
  if (hparse_errno > 0) return;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFINER") == 1) {;}
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INVOKER") == 1) {;}
  else hparse_f_error();
}

/*
  Todo: MySQL's manual seems  bit vague here -- should the channel always be a string literal or can it be an identifier?
  I'm allowing both until I' sure, but prefer string.
*/
void MainWindow::hparse_f_for_channel()
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR"))
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHANNEL");
    if (hparse_errno > 0) return;  
    if (hparse_f_literal(TOKEN_REFTYPE_CHANNEL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHANNEL,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
  }
}

void MainWindow::hparse_f_interval_quantity(int interval_or_event)
{
  if (((interval_or_event == TOKEN_KEYWORD_INTERVAL) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MICROSECOND") == 1))
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SECOND") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MINUTE") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HOUR") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DAY") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WEEK") == 1)
   || ((interval_or_event == TOKEN_KEYWORD_INTERVAL) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MONTH") == 1))
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "QUARTER") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "YEAR") == 1)
   || ((interval_or_event == TOKEN_KEYWORD_INTERVAL) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SECOND_MICROSECOND") == 1))
   || ((interval_or_event == TOKEN_KEYWORD_INTERVAL) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MINUTE_MICROSECOND") == 1))
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MINUTE_SECOND") == 1)
   || ((interval_or_event == TOKEN_KEYWORD_INTERVAL) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HOUR_MICROSECOND") == 1))
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HOUR_SECOND") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HOUR_MINUTE") == 1)
   || ((interval_or_event == TOKEN_KEYWORD_INTERVAL) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DAY_MICROSECOND") == 1))
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DAY_SECOND") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DAY_MINUTE") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DAY_HOUR") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "YEAR_MONTH") == 1)) {;}
else hparse_f_error();
}

void MainWindow::hparse_f_alter_or_create_event(int statement_type)
{
  if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_EVENT, TOKEN_REFTYPE_EVENT) == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  bool on_seen= false, on_schedule_seen= false;
  if (statement_type == TOKEN_KEYWORD_CREATE)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON");
    if (hparse_errno > 0) return;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    on_seen= true;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SCHEDULE");
    if (hparse_errno > 0) return;
    on_schedule_seen= true;
  }
  else /* if statement_type == TOKEN_KEYWORD_ALTER */
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON") == 1)
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
      on_seen= true;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SCHEDULE") == 1)
      {
        on_schedule_seen= true;
      }
    }
  }
  if (on_schedule_seen == true)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AT") == 1)
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EVERY") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_interval_quantity(TOKEN_KEYWORD_EVENT);
      if (hparse_errno > 0) return;
    }
    else hparse_f_error();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STARTS") == 1)
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENDS") == 1)
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
    }
    on_seen= on_schedule_seen= false;
  }
  if (on_seen == false)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON") == 1) on_seen= true;
  }
  if (on_seen == true)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COMPLETION");
    if (hparse_errno > 0) return;
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOT");
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PRESERVE");
    if (hparse_errno > 0) return;
  }
  if (statement_type == TOKEN_KEYWORD_ALTER)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RENAME") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TO");
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_EVENT, TOKEN_REFTYPE_EVENT) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENABLE") == 1) {;}
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISABLE") == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON") == 1)
    {
      if (hparse_f_accept_slave_or_replica(FLAG_VERSION_MYSQL_OR_MARIADB_ALL) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
  }
  hparse_f_comment(0);
  if (hparse_errno > 0) return;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DO") == 1)
  {
    hparse_f_block(TOKEN_KEYWORD_EVENT, hparse_i);
    if (hparse_errno > 0) return;
  }
  else if (statement_type == TOKEN_KEYWORD_CREATE) hparse_f_error();
}

void MainWindow::hparse_f_create_database()
{
  hparse_f_if_not_exists();
  if (hparse_errno > 0) return;
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
  if (hparse_errno > 0) return;
  bool character_seen= false, collate_seen= false;
  for (int i=0; i < 2; ++i)
  {
    bool default_seen= false;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 1)
    {
      main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      default_seen= true;
    }
    if ((character_seen == false) && (hparse_f_character_set() == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_character_set_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      character_seen= true;
    }
    else if (hparse_errno > 0) return;
    else if ((collate_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATE") == 1))
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_collation_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      collate_seen= true;
    }
    else
    {
      if (default_seen == true) hparse_f_error();
    }
  }
}

/*
  We've seen CREATE PACKAGE [BODY] ... IS|AS with FLAG_VERSION_PLSQL. Or CREATE PACKAGE with FLAG_VERSION_MARIADB_11_4.
  Expect procedure|function declarations and|or definitions.
  Problem: hparse_f_block() calls hparse_f_semicolon_and_or_delimiter(),
           due to a kludge. This will eat the ";", so the do-loop ends.
           We could try to stop the kludge by passing a flag to
           hparse_f_block(). But we chose to kludge to negate the kludge
           by saying: if last-accepted was ; then continue.
*/
void MainWindow::hparse_f_create_package(bool is_body)
{
  int i_of_start_of_create_package= hparse_i;
  bool is_end_seen= false;
  int first_word= 0;
  if (is_body) first_word= hparse_f_declare_plsql(TOKEN_KEYWORD_PACKAGE);
  if (hparse_errno > 0) return;
  for (;;)
  {
    if ((first_word == TOKEN_KEYWORD_FUNCTION)
     || ((first_word == 0)
      && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FUNCTION, "FUNCTION") == 1)))
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION, TOKEN_REFTYPE_FUNCTION) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_create_function_clauses();
      if (hparse_errno > 0) return;
      if (is_body)
      {
        /* Todo: this was TOKEN_KEYWORD_PROCEDURE, we need to re-test with sql_mode='oracle' soon. */
        hparse_f_block(TOKEN_KEYWORD_FUNCTION, hparse_i);
        if (hparse_errno > 0) return;
      }
      first_word= 0;
    }
    else if ((first_word == TOKEN_KEYWORD_PROCEDURE)
          || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PROCEDURE, "PROCEDURE") == 1))
    {
      hparse_f_create_procedure_clauses();
      if (hparse_errno > 0) return;
      if (is_body)
      {
        hparse_f_block(TOKEN_KEYWORD_PROCEDURE, hparse_i);
        if (hparse_errno > 0) return;
      }
      first_word= 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END_IN_CREATE_STATEMENT, "END") == 1)
    {
      --hparse_flow_count;
      is_end_seen= true;
      break;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_PLSQL) == 0) && (is_body != 0)) /* statement in CREATE PACKAGE BODY? */
    {
      hparse_f_statement(i_of_start_of_create_package); /* todo: check if this is the right block_top now */
      if (hparse_errno > 0) return;
    }
    else if (hparse_token.toUpper() == "BEGIN")
    {
      hparse_f_block(TOKEN_KEYWORD_PROCEDURE, i_of_start_of_create_package);
      is_end_seen= true;
      break;
    }
    else
    {
      /* Guaranteed to fail */
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BEGIN, "BEGIN");
      hparse_f_error();
    }
    if (hparse_errno > 0) return;

    if (is_body)
    {
      QString last_accepted= hparse_text_copy.mid(main_token_offsets[hparse_i_of_last_accepted], main_token_lengths[hparse_i_of_last_accepted]);
      if (last_accepted == ";") continue;
    }
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";")) continue;
    break;
  }
  if (is_end_seen == false) hparse_f_error();
  /* This kludge occurs more than once. */
//  if ((hparse_prev_token != ";") && (hparse_prev_token != hparse_delimiter_str))
//  {
//    {
//      if (hparse_f_semicolon_and_or_delimiter(TOKEN_KEYWORD_PROCEDURE) == 0)
//      {
//        hparse_f_error();
//      }
//    }
//  }
}

void MainWindow::hparse_f_create_function_clauses()
{
  hparse_f_parameter_list(TOKEN_KEYWORD_FUNCTION);
  if (hparse_errno > 0) return;
  if ((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0)
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RETURN");
  else
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RETURNS");
  if (hparse_errno > 0) return;
  if (hparse_f_data_type(TOKEN_KEYWORD_RETURNS) == -1) hparse_f_error();
  if (hparse_errno > 0) return;
  hparse_f_characteristics(TOKEN_KEYWORD_FUNCTION);
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_create_procedure_clauses()
{
  if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_PROCEDURE, TOKEN_REFTYPE_PROCEDURE) == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  hparse_f_parameter_list(TOKEN_KEYWORD_PROCEDURE);
  if (hparse_errno > 0) return;
  hparse_f_characteristics(TOKEN_KEYWORD_PROCEDURE);
  if (hparse_errno > 0) return;
}


/*
  During CREATE TABLE ... (..., PRIMARY|UNIQUE(...)) we might know what
  the preceding column names are, so expect them.
  In Tarantool this is compulsory. ?? Well, it used to be compulsory.
  In MySQL|MariaDB this is optional.
  If ALTER this won't happen.
  Return: hparse_i of the column's data type word, or 0
  TODO: This stops showing expecteds as soon as I type the first letter.
  TODO: The comparison to "INTEGER" is redundant if data type flag is okay.
*/
int MainWindow::hparse_f_index_column_expecter()
{
  int i= hparse_i;
  for (;;)
  {
    if (i == 0) break; /* Actually this must be an error */
    if ((main_token_flags[i] & TOKEN_FLAG_IS_START_IN_COLUMN_LIST) != 0) break;
    --i;
  }
  if (i > 0)
  {
    for (;i < hparse_i; ++i)
    {
      if (hparse_errno > 0) return 0;
      if (main_token_reftypes[i] == TOKEN_REFTYPE_COLUMN)
      {
        QString column_name;
        column_name= hparse_text_copy.mid(main_token_offsets[i], main_token_lengths[i]);
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, column_name) == 1)
        {
          QString token;
          for (;;)
          {
            if (i == hparse_i) return 0;
            if ((main_token_flags[i] & TOKEN_FLAG_IS_DATA_TYPE) != 0)
            {
              return i;
            }
            token= hparse_text_copy.mid(main_token_offsets[i], main_token_lengths[i]);
            if (token == ",")
            {
              return 0;
            }
//            if (QString::compare(token, "INTEGER", Qt::CaseInsensitive) == 0) return i;
            ++i;
          }
          return 0;
        }
      }
    }
  }
  //if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0) hparse_f_error();
  //else
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
  return 0;
}

/* (index_col_name,...) [index_option] for both CREATE INDEX and CREATE TABLE */
/*
   Todo Re Tarantool:
   * Currently Tarantool requires that primary key constraint must follow column definition, but this
     might be a temporary bug, so check that it's still true.
   * alter table ... add primary (...) autoincrement will probably never happen but we should allow
*/
void MainWindow::hparse_f_index_columns(int index_or_table,
                                        bool fulltext_seen, bool foreign_seen, bool primary_seen, bool vector_seen)
{
  if ((fulltext_seen == false) && (foreign_seen == false))
  {
    /* index_type */
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USING") == 1)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BTREE") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HASH") == 1) {;}
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
  }
  if (index_or_table == TOKEN_KEYWORD_INDEX)
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON");    /* ON tbl_name */
    if (hparse_errno > 0) return;
    if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  int index_column_number= 0;
  do                                                             /* index_col_name, ... */
  {
    ++index_column_number;
    if (index_or_table == TOKEN_KEYWORD_TABLE)
    {
      /* i_of_data_type will only come in if it's CREATE not ALTER, and constraints after all columns */
      int i_of_data_type= hparse_f_index_column_expecter();
      if (((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
       /* && (index_column_number == 1) */
       && (primary_seen == true))
      {
        int data_type= main_token_types[i_of_data_type];
        if (data_type == TOKEN_KEYWORD_INT || data_type == TOKEN_KEYWORD_INTEGER || data_type == TOKEN_KEYWORD_UNSIGNED)
        {
          hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AUTOINCREMENT");
          if (hparse_errno > 0) return;
        }
      }
    }
    else hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    if (vector_seen == true) break;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_LENGTH, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    if ((foreign_seen == true) && ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)) {;}
    else
    {
      if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATE"))
      {
        if (hparse_f_collation_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ASC") != 1) hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DESC");
    }
  } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;

  if (foreign_seen == true)
  {
    if (hparse_f_reference_definition() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (vector_seen == false) /* if (foreign_seen == false) */
  {
    /* MySQL doesn't check whether these clauses are repeated, but we do. */
    bool key_seen= false, using_seen= false, comment_seen= false, with_seen= false, not_ignored_seen= false;
    for (;;)                                                             /* index_options */
    {
      if ((key_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY_BLOCK_SIZE") == 1))
      {
        key_seen= true;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=") == 1) {;}
        if (hparse_f_literal(TOKEN_REFTYPE_LENGTH, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        continue;
      }
      if ((using_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USING") == 1))
      {
        using_seen= true;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BTREE") == 1) {;}
        else hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HASH");
        if (hparse_errno > 0) return;
        continue;
      }
      if ((with_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITH") == 1))
      {
        with_seen= true;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PARSER");
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_PARSER,TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
        continue;
      }
      if ((comment_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COMMENT") == 1))
      {
        comment_seen= true;
        if (hparse_f_literal(TOKEN_REFTYPE_COMMENT, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        continue;
      }
      if ((not_ignored_seen == false) && (hparse_f_not_ignored(false) == 1))
      {
        not_ignored_seen= true;
        if (hparse_errno > 0) return;
        continue;
      }


      break;
    }
  }
  if (vector_seen == true)
  {
    if (hparse_f_accept(FLAG_VERSION_MARIADB_11_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "M") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_PARSER,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_errno > 0) return;
      if (hparse_f_literal(TOKEN_REFTYPE_COMMENT, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
  }
  if (hparse_f_accept(FLAG_VERSION_MARIADB_11_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISTANCE") == 1)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_PARSER,TOKEN_TYPE_OPERATOR, "=");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MARIADB_11_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EUCLIDEAN") == 0)
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_PARSER,TOKEN_TYPE_KEYWORD, "COSINE");
  }
}

void MainWindow::hparse_f_alter_or_create_sequence(int statement_type)
{
  if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_SEQUENCE,TOKEN_REFTYPE_SEQUENCE) == 0)
  {
    hparse_f_error();
    return;
  }
  bool is_start_seen= false, is_minvalue_seen= false, is_maxvalue_seen= false;
  bool is_increment_seen= false, is_cycle_seen= false, is_cache_seen= false;
  bool is_restart_seen= false; bool is_as_seen= false;
  if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_11_5) == 0) is_as_seen= true; /* i.e. don't look for AS if < 11.5 */
  for (;;)
  {
    if (is_as_seen == false)
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_AS, "AS") == 1)
      {
        /* MariaDB manual is false, "INT3" etc. would be accepted, but we'll only take what's documented. */
        if ((hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TINYINT, "TINYINT") == 1)
         || (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SMALLINT, "SMALLINT") == 1)
         || (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_MEDIUMINT, "MEDIUMINT") == 1)
         || (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INT, "INT") == 1)
         || (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INTEGER, "INTEGER") == 1)
         || (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BIGINT, "BIGINT") == 1))
        {
          main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DATA_TYPE;
          if ((hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SIGNED, "SIGNED") == 1)
           || (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_UNSIGNED, "UNSIGNED") == 1))
            {;}
        }
        else hparse_f_error();
        if (hparse_errno > 0) return;
        is_as_seen= true;
        continue;
      }
    }
    if (is_start_seen == false)
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_START, "START") == 1)
      {
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WITH, "WITH") == 0)
          hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_SIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        is_start_seen= true;
        continue;
      }
    }
    if (is_minvalue_seen== false)
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_MINVALUE, "MINVALUE") == 1)
      {
        hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_SIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        is_minvalue_seen= true;
        continue;
      }
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOMINVALUE") == 1)
      {
        is_minvalue_seen= true;
        continue;
      }
    }
    if (is_maxvalue_seen== false)
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_MAXVALUE, "MAXVALUE") == 1)
      {
        hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_SIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        is_maxvalue_seen= true;
        continue;
      }
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOMAXVALUE") == 1)
      {
        is_maxvalue_seen= true;
        continue;
      }
    }
    if (is_increment_seen == false)
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INCREMENT") == 1)
      {
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BY, "BY") == 0)
          hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_SIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        is_increment_seen= true;
        continue;
      }
    }
    if (is_cycle_seen == false)
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CYCLE, "CYCLE") == 1)
      {
        is_cycle_seen= true;
        continue;
      }
    }
    if (is_cache_seen == false)
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CACHE") == 1)
      {
        hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_SIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        is_cache_seen= true;
        continue;
      }
    }
    if ((is_restart_seen == false)
     && (statement_type == TOKEN_KEYWORD_ALTER))
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RESTART") == 1)
      {
        bool is_with_seen= false;
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WITH, "WITH") == 1) is_with_seen= true;
        else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=") == 1) is_with_seen= true;
        if (is_with_seen == true)
        {
          if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_SIGNED_INTEGER) == 0) hparse_f_error();
        }
        else
        {
          hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_SIGNED_INTEGER);
        }
        if (hparse_errno > 0) return;
        is_restart_seen= true;
        continue;
      }
    }
    if ((is_minvalue_seen == false)
     || (is_maxvalue_seen == false)
     || (is_cycle_seen== false))
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_NO, "NO") == 1)
      {
        if ((is_minvalue_seen == false)
         && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_MINVALUE, "MINVALUE") == 1))
        {
          is_minvalue_seen= true;
          continue;
        }
        if ((is_maxvalue_seen == false)
         && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_MAXVALUE, "MAXVALUE") == 1))
        {
          is_maxvalue_seen= true;
          continue;
        }
        if ((is_cycle_seen == false)
         && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CYCLE, "CYCLE") == 1))
        {
          is_cycle_seen= true;
          continue;
        }
      }
    }
    break;
  }
  /* TODO: table options */
  return;
}

void MainWindow::hparse_f_alter_or_create_server(int statement_type)
{
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_SERVER,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
    if (hparse_f_literal(TOKEN_REFTYPE_SERVER, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  if (statement_type == TOKEN_KEYWORD_CREATE)
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FOREIGN, "FOREIGN");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATA");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WRAPPER");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_WRAPPER,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
      if (hparse_f_literal(TOKEN_REFTYPE_WRAPPER, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OPTIONS");
  if (hparse_errno > 0) return;
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  do
  {
    if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HOST") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATABASE") == 1)
     || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USER") == 1)
     || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PASSWORD") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SOCKET") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OWNER") == 1)
     || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PORT") == 1))
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT) == 0) hparse_f_error();
    }
    else hparse_f_error();
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
}

/*
  REQUIRE tsl_option is allowed in GRANT, and in CREATE USER (+ALTER?) after MySQL 5.7.6 | MariaDB 10.2.
  WITH resource_option is allowed in GRANT, and in CREATE USER (+ALTER?) after MySQL 5.7.6 | MariaDB 10.2.
  password_option and lock_option are allowed in CREATE or ALTER after MySQL 5.7.6
  CIPHER + ISSUER + SUBJECT can be combined, but not NONE | SSL | X509.
*/
void MainWindow::hparse_f_require(int who_is_calling, bool proxy_seen, bool role_name_seen)
{
  bool is_grant_and_mysql_8= false;
  if ((who_is_calling == TOKEN_KEYWORD_GRANT) && ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) != 0))
    is_grant_and_mysql_8= true;

  if ((who_is_calling == TOKEN_KEYWORD_GRANT)
   || ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_2_2) != 0)
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_5_7) != 0))
  {
    bool requirement_seen= false;
    if ((is_grant_and_mysql_8 == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REQUIRE") == 1))
    {
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NONE") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SSL")== 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "X509")== 1))
         requirement_seen= true;
      else
      {
        /* Maybe we should be using something like hparse_pick_from_list here. */
        QStringList tokens= (QStringList() << "CIPHER" << "ISSUER" << "SUBJECT");
        for (;;)
        {
          int i_of_matched;
          if (hparse_f_accept_in_set(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, tokens, &i_of_matched) == 0) break;
          if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0)
          {
            hparse_f_error();
            return;
           }
          tokens.removeAt(i_of_matched);
          requirement_seen= true;
          if (tokens.size() == 0) break;
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AND") == 1)
            requirement_seen= false;
        }
      }
      if (requirement_seen == false) hparse_f_error();
      if (hparse_errno > 0) return;
    }
  }
  if ((who_is_calling == TOKEN_KEYWORD_GRANT)
   || ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_2_2) != 0)
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_5_7) != 0))
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITH") == 1)
    {
      bool option_seen= false;
      if (who_is_calling == TOKEN_KEYWORD_GRANT)
      {
        int x= 0;
        if (role_name_seen == false)
          x= hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GRANT");
        else
          x= hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ADMIN");
        if (x == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OPTION");
          if (hparse_errno > 0) return;
          option_seen= true;
        }
      }

      if ((option_seen == false) && (proxy_seen == false) && (is_grant_and_mysql_8 == false))
      {
        QStringList tokens;
        tokens << "MAX_QUERIES_PER_HOUR" << "MAX_UPDATES_PER_HOUR" << "MAX_CONNECTIONS_PER_HOUR" << "MAX_USER_CONNECTIONS";
        if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_2_2) != 0) tokens.append("MAX_STATEMENT_TIME");
        for (;;)
        {
          int i_of_matched;
          if (hparse_f_accept_in_set(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, tokens, &i_of_matched) == 0) break;
          QString matched_value= tokens.at(i_of_matched);
          if ((matched_value == "GRANT") || (matched_value == "ADMIN"))
          {
            option_seen= true;
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OPTION");
            break;
          }
          else
          {
            if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();
          }
          if (hparse_errno > 0) return;
          tokens.removeAt(i_of_matched);
          option_seen= true;
          if (tokens.size() == 0) break;
        }
      }
      if (option_seen == false) hparse_f_error();
      if (hparse_errno > 0) return;
    }
  }
  if ((who_is_calling == TOKEN_KEYWORD_CREATE) || (who_is_calling == TOKEN_KEYWORD_ALTER))
  {
    if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_2_2) != 0)
     || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_5_7) != 0))
    {
      /* password_option and lock_option can be mixed together and can be repeated, the MySQL manual is wrong */
      bool is_password_expire_seen= false;       /* MariaDB, MySQL 5.7, MySQL 8 */
      bool is_password_history_seen= false;      /* MySQL 8 */
      bool is_password_reuse_seen= false;        /* MySQL 8 */
      bool is_password_require_seen= false;      /* MySQL 8 */
      bool is_failed_login_attempts_seen= false; /* MySQL 8 */
      bool is_password_lock_time_seen= false;    /* MySQL 8 */
      bool is_account_seen= false;               /* MariaDB, MySQL 5.7, MySQL 8 */
      if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) == 0)
      {
        is_password_history_seen= is_password_reuse_seen= true; is_password_require_seen= true;
        is_failed_login_attempts_seen= is_password_lock_time_seen= true;
      }
      for (;;)
      {
        if (hparse_errno > 0) return;
        if ((is_password_expire_seen == false) || (is_password_history_seen == false)
         || (is_password_reuse_seen == false) || (is_password_require_seen == false))
        {
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PASSWORD, "PASSWORD") == 1)
          {
            if ((is_password_expire_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXPIRE") == 1))
            {
              is_password_expire_seen= true;
              if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DEFAULT, "DEFAULT") == 1)
                main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
              else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NEVER") == 1) {;}
              else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INTERVAL, "INTERVAL") == 1)
              {
                if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();
                if (hparse_errno > 0) return;
                hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DAY, "DAY");
              }
              continue;
            }
            if ((is_password_history_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HISTORY") == 1))
            {
              is_password_history_seen= true;
              if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DEFAULT, "DEFAULT") == 1)
                main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
              else if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_8_0, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();
              continue;
            }
            if ((is_password_reuse_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REUSE") == 1))
            {
              is_password_reuse_seen= true;
              hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INTERVAL, "INTERVAL");
              if (hparse_errno > 0) return;
              if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DEFAULT, "DEFAULT") == 1)
                main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
              else
              {
                if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_8_0, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();
                if (hparse_errno > 0) return;
                hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DAY, "DAY");
              }
              continue;
            }
            if ((is_password_require_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REQUIRE, "REQUIRE") == 1))
            {
              is_password_require_seen= true;
              hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CURRENT, "CURRENT");
              if (hparse_errno > 0) return;
              if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DEFAULT, "DEFAULT") == 1)
                main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
              else hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OPTIONAL");
              continue;
            }
            hparse_f_error(); /* because none of the password possibilities was accepted */
            continue;
          }
        }
        if ((is_failed_login_attempts_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FAILED_LOGIN_ATTEMPTS") == 1))
        {
          is_failed_login_attempts_seen= true;
          if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_8_0, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();
          continue;
        }
        if ((is_password_lock_time_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PASSWORD_LOCK_TIME") == 1))
        {
          is_password_lock_time_seen= true;
          if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_UNBOUNDED, "UNBOUNDED") == 0)
          {
            if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_8_0, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();
          }
          continue;
        }
        if ((is_account_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ACCOUNT") == 1))
        {
          is_account_seen= true;
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LOCK, "LOCK") == 0)
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_UNLOCK, "UNLOCK");
          continue;
        }
        break; /* because no new password_option was seen */
      }
    }
    if (((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) != 0) && (who_is_calling != TOKEN_KEYWORD_GRANT))
      hparse_f_comment(TOKEN_KEYWORD_ATTRIBUTE);
  }
}


void MainWindow::hparse_f_user_specification_list(int who_is_calling)
{
  do
  {
    if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 0) hparse_f_error();
    if (hparse_errno > 0) return;

    if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) != 0)
    {
      if (who_is_calling == TOKEN_KEYWORD_GRANT) continue;
      for (int auth_option_number= 1;; ++auth_option_number)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IDENTIFIED") == 1)
        {
          if (hparse_f_identified() == TOKEN_KEYWORD_BY) {;}
          else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WITH, "WITH") == 1)
          {
            hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_PLUGIN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
            if (hparse_errno > 0) return;
            if (hparse_f_identified() == TOKEN_KEYWORD_BY) {;} /* [BY ...] */
            else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_AS, "AS") == 1)
            {
              if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
              if (hparse_errno > 0) return;
            }
            else if ((auth_option_number == 1) && (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INITIAL") == 1))
            {
              hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AUTHENTICATION");
              if (hparse_errno > 0) return;
              hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IDENTIFIED");
              if (hparse_errno > 0) return;
              if (hparse_f_identified() == TOKEN_KEYWORD_BY) {;}
              else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_AS, "AS") == 1)
              {
                if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
                if (hparse_errno > 0) return;
              }
              else hparse_f_error();
            }
          }
          else hparse_f_error();
          if (hparse_errno > 0) return;
          if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REPLACE, "REPLACE") == 1)
          {
            if (hparse_f_literal(TOKEN_REFTYPE_PASSWORD, FLAG_VERSION_MYSQL_8_0, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
          }
          else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RETAIN") == 1)
          {
            hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CURRENT, "CURRENT");
            hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PASSWORD, "PASSWORD");
          }
          if (hparse_errno > 0) return;
          if ((auth_option_number < 3) && (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_AND, "AND") == 1)) continue;
        }
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISCARD") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OLD");
          hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PASSWORD, "PASSWORD");
          if (hparse_errno > 0) return;
        }
        break;
      }
    }
    else
    {
      /* The rest of this function is for anything that's not MySQL 8.0, i.e. MySQL 5.7 or MariaDB */
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IDENTIFIED") == 1)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY") == 1)
        {
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PASSWORD") == 1)
          {
            if (hparse_f_literal(TOKEN_REFTYPE_PASSWORD, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
            if (hparse_errno > 0) return;
          }
          else
          {
            if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
            if (hparse_errno > 0) return;
          }
        }
        else if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WITH, "WITH") == 1)
          || (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VIA") == 1))
        {
          for (;;)
          {
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_PLUGIN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
            if (hparse_errno > 0) return;
            if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_AS, "AS") == 1)
             || (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_USING, "USING") == 1)
             || (hparse_f_accept(FLAG_VERSION_MYSQL_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BY, "BY") == 1))
            {
              if (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PASSWORD, "PASSWORD") == 1)
              {
                hparse_f_expect(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
                if (hparse_errno > 0) return;
                if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
                hparse_f_expect(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
              }
              else if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
            }
            if (hparse_errno > 0) return;
            if (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_OR, "OR") == 1) continue;
            break;
          }
        }
        else hparse_f_error();
      }
      if (hparse_errno > 0) return;
    }
  } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
}

/* Called from hparse_f_user_specification_list() after IDENTIFIED for BY, which occurs a few times */
int MainWindow::hparse_f_identified()
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BY, "BY") == 1)
  {
    if (hparse_f_literal(TOKEN_REFTYPE_PASSWORD, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_RANDOM, "RANDOM");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PASSWORD, "PASSWORD");
      if (hparse_errno > 0) return 0;
    }
    return TOKEN_KEYWORD_BY;
  }
  return 0;
}

void MainWindow::hparse_f_alter_or_create_view()
{
  if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_VIEW, TOKEN_REFTYPE_VIEW) == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  hparse_f_column_list(0, 0);
  if (hparse_errno > 0) return;
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS");
  if (hparse_errno > 0) return;
  if (hparse_f_query(0, false, false, false) == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITH") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CASCADED") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCAL") == 1)) {;}
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHECK");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OPTION");
    if (hparse_errno > 0) return;
  }
}

/* For CALL statement or for PROCEDURE clause in SELECT */
void MainWindow::hparse_f_call()
{
  if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_PROCEDURE, TOKEN_REFTYPE_PROCEDURE) == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  hparse_f_call_arguments();
}

/* Todo: hparse_f_function_arguments() and hparse_f_expression_list()
   and hparse_f_parenthesized_value_list() are rather similar -- there
   ought to be some generalized function. Maybe there already is one
   but I've forgotten it.
*/
void MainWindow::hparse_f_call_arguments()
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")") == 1) return;
    do
    {
      hparse_f_opr_1(0, 0);
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_commit_or_rollback()
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AND") == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NO") == 1) {;}
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHAIN");
    if (hparse_errno > 0) return;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NO") == 1)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RELEASE");
    if (hparse_errno > 0) return;
  }
  else
  {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RELEASE") == 1) {;}
  }
  if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TRANSACTION") == 1)
  {
    hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_TRANSACTION,TOKEN_TYPE_IDENTIFIER, "[identifier]");
  }
}

/*
  Warning: we call hparse_f_qualified_name_of_object(0, ) while assuming
           that any statement-starting verb in Tarantool is reserved.
*/
void MainWindow::hparse_f_explain_or_describe(int block_top)
{
  bool explain_type_seen= false;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXTENDED") == 1)
  {
    explain_type_seen= true;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PARTITIONS") == 1)
  {
    explain_type_seen= true;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FORMAT") == 1)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TRADITIONAL") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TREE") == 1) {;}
    else hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "JSON");
    if (hparse_errno > 0) return;
    explain_type_seen= true;
  }
  if (explain_type_seen == false)
  {
    if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 1)
    {
      /* DESC table_name wild ... wild can contain '%' and be unquoted. Ugly. */
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 1) return;
      for (;;)
      {
        if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, ".") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "%") == 1)) continue;
        break;
      }
      return;
    }
  }
  if (((hparse_dbms_mask & FLAG_VERSION_MYSQL_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR") == 1))
  {
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATABASE") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SCHEMA") == 1))
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      goto explainable;
    }
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONNECTION");
    if (hparse_errno > 0) return;
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();
    return;
  }
explainable:
  hparse_f_explainable_statement(block_top);
  if (hparse_errno > 0) return;
}

/*
  If the token we're looking at is a keyword for a privilege, return 2.
  If it might be for a privilege, return 1.
   (There are some words that are probably privileges but might be
   identifiers because they're not reserved, affected non-reserved words
   are: event, execute, file, proxy, reload, replication, shutdown,
   super. In MySQL 8.0 you cannot use those as role names), but you can
   use backup_admin and other new MySQL-8.0 privileges.)
  Otherwise, return 0.
*/
int MainWindow::is_token_priv(int token)
{
  if ((token == TOKEN_KEYWORD_ALL)
   || (token == TOKEN_KEYWORD_ALTER)
   || (token == TOKEN_KEYWORD_CREATE)
   || (token == TOKEN_KEYWORD_DELETE)
   || (token == TOKEN_KEYWORD_DROP)
   || (token == TOKEN_KEYWORD_GRANT)
   || (token == TOKEN_KEYWORD_INDEX)
   || (token == TOKEN_KEYWORD_INSERT)
   || (token == TOKEN_KEYWORD_LOCK)
   || (token == TOKEN_KEYWORD_PROCESS)
   || (token == TOKEN_KEYWORD_REFERENCES)
   || (token == TOKEN_KEYWORD_SELECT)
   || (token == TOKEN_KEYWORD_SHOW)
   || (token == TOKEN_KEYWORD_SUPER)
   || (token == TOKEN_KEYWORD_TRIGGER)
   || (token == TOKEN_KEYWORD_UPDATE)
   || (token == TOKEN_KEYWORD_USAGE))
    return 2;
  if ((token == TOKEN_KEYWORD_EVENT)
   || (token == TOKEN_KEYWORD_EXECUTE)
   || (token == TOKEN_KEYWORD_FILE)
   || (token == TOKEN_KEYWORD_PROXY)
   || (token == TOKEN_KEYWORD_RELOAD)
   || (token == TOKEN_KEYWORD_REPLICATION)
   || (token == TOKEN_KEYWORD_SHUTDOWN))
  {
   if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) != 0) return 2;
   return 1;
  }
  if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) != 0)
  {
    if ((token == TOKEN_KEYWORD_BACKUP_ADMIN)
     || (token == TOKEN_KEYWORD_BINLOG_ADMIN)
     || (token == TOKEN_KEYWORD_CONNECTION_ADMIN)
     || (token == TOKEN_KEYWORD_ENCRYPTION_KEY_ADMIN)
     || (token == TOKEN_KEYWORD_FIREWALL_ADMIN)
     || (token == TOKEN_KEYWORD_FIREWALL_USER)
     || (token == TOKEN_KEYWORD_GROUP_REPLICATION_ADMIN)
     || (token == TOKEN_KEYWORD_PERSIST_RO_VARIABLES_ADMIN)
     || (token == TOKEN_KEYWORD_REPLICATION_SLAVE_ADMIN)
     || (token == TOKEN_KEYWORD_RESOURCE_GROUP_ADMIN)
     || (token == TOKEN_KEYWORD_RESOURCE_GROUP_USER)
     || (token == TOKEN_KEYWORD_ROLE_ADMIN)
     || (token == TOKEN_KEYWORD_SET_USER_ID)
     || (token == TOKEN_KEYWORD_SYSTEM_VARIABLES_ADMIN)
     || (token == TOKEN_KEYWORD_VERSION_TOKEN_ADMIN)
     || (token == TOKEN_KEYWORD_XA_RECOVER_ADMIN))
      return 1;
  }
  return 0;
}


/*
   With GRANT|REVOKE, first we check for identifiers (which could be
   role names) (only MariaDB or MySQL 8.0) and if they're there then
   everything must be role names, if they're not there then everything
   must not be role names.
   Todo: I'm unsure about GRANT|REVOKE PROXY
   is_maybe_all is for check of REVOKE_ALL_PRIVILEGES,GRANT OPTION

   We need lookahead here to check for GRANT token TO|ON, but if token
   is a role, for MariaDB, we didn't need to worry about
   GRANT role [, role...] -- see https://jira.mariadb.org/browse/MDEV-5772.
   But MySQL 8.0 allows multiple roles, so look far ahead.

   Todo: We're not checking that lists of privileges are compatible e.g. GRANT UPDATE,EXECUTE.
         We're not checking that privileges are compatible with objects e.g. GRANT UPDATE ON PACKAGE.
*/
void MainWindow::hparse_f_grant_or_revoke(int who_is_calling, bool *role_name_seen)
{
  bool is_role_ok= false;
  bool is_privilege_ok= true;
  /* Lookahead: find an indication whether this grant|revoke
     is for roles or for privileges. As soon as you have an
     indication, then you know all that follows must be too.
     + make sure you're showing that you're looking for role or looking for privilege
     e.g. token_reftype_privilege_or_role, token_reftype_privilege_refer, etc */
  if ((hparse_dbms_mask & (FLAG_VERSION_MARIADB_ALL | FLAG_VERSION_MYSQL_8_0)) != 0)
  {
    is_role_ok= true;
    int token_type;
    {
      int i;
      QString token;
      for (i= hparse_i; main_token_lengths[i] != 0; ++i)
      {
        int is_priv;
        token_type= main_token_types[i];
        if (token_type == TOKEN_KEYWORD_ON) {is_role_ok= false; break; }
        if (token_type == TOKEN_KEYWORD_TO) {is_privilege_ok= false; break; }
        if (token_type == TOKEN_KEYWORD_FROM) {is_privilege_ok= false; break; }
        is_priv= is_token_priv(token_type);
        if (is_priv == 2) {is_role_ok= false; break; }
        if (is_priv == 0) {is_privilege_ok= false; break; }
        token= hparse_text_copy.mid(main_token_offsets[i], main_token_lengths[i]);
        if (token == ";") break;
        if (token == hparse_delimiter_str) break;
      }
    }
  }
  bool is_maybe_all= false;
  int count_of_grants= 0;
  do
  {
    int priv_type= 0;
    if (is_privilege_ok)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1)
      {
        priv_type= TOKEN_KEYWORD_ALL;
        is_maybe_all= true;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PRIVILEGES") == 1) {;}
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALTER") == 1)
      {
        priv_type= TOKEN_KEYWORD_ALTER;
        is_maybe_all= false;
        hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROUTINE");
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CREATE") == 1)
      {
        priv_type= TOKEN_KEYWORD_CREATE;
        is_maybe_all= false;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROUTINE") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLESPACE") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TEMPORARY") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLES");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USER") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VIEW") == 1) {;}
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DELETE") == 1)
      {
        priv_type= TOKEN_KEYWORD_DELETE;
        is_maybe_all= false;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DROP") == 1)
      {
        priv_type= TOKEN_KEYWORD_DROP;
        is_maybe_all= false;
      }
      else if ((is_role_ok == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EVENT") == 1))
      {
        priv_type= TOKEN_KEYWORD_EVENT;
        is_maybe_all= false;
      }
      else if ((is_role_ok == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXECUTE") == 1))
      {
        priv_type= TOKEN_KEYWORD_EXECUTE;
        is_maybe_all= false;
      }
      else if ((is_role_ok == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FILE") == 1))
      {
        priv_type= TOKEN_KEYWORD_FILE;
        is_maybe_all= false;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GRANT") == 1)
      {
        priv_type= TOKEN_KEYWORD_GRANT;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OPTION");
        if (hparse_errno > 0) return;
        if ((is_maybe_all == true) && (who_is_calling == TOKEN_KEYWORD_REVOKE)) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX") == 1)
      {
        priv_type= TOKEN_KEYWORD_INDEX;
        is_maybe_all= false;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INSERT") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
        priv_type= TOKEN_KEYWORD_INSERT;
        is_maybe_all= false;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCK") == 1)
      {
        priv_type= TOKEN_KEYWORD_LOCK;
        is_maybe_all= false;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLES");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROCESS") == 1)
      {
        priv_type= TOKEN_KEYWORD_PROCESS;
        is_maybe_all= false;
      }
      else if ((is_role_ok == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROXY") == 1))
      {
        priv_type= TOKEN_KEYWORD_PROXY;
        is_maybe_all= false;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REFERENCES") == 1)
      {
        priv_type= TOKEN_KEYWORD_REFERENCES;
        is_maybe_all= false;
      }
      else if ((is_role_ok == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RELOAD") == 1))
      {
        priv_type= TOKEN_KEYWORD_RELOAD;
        is_maybe_all= false;
      }
      else if ((is_role_ok == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLICATION") == 1))
      {
        priv_type= TOKEN_KEYWORD_REPLICATION;
        is_maybe_all= false;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CLIENT") == 1) {;}
        else if (hparse_f_accept_slave_or_replica(FLAG_VERSION_MYSQL_OR_MARIADB_ALL) == 1) {;}
        else hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SELECT") == 1)
      {
        priv_type= TOKEN_KEYWORD_SELECT;
        is_maybe_all= false;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SHOW") == 1)
      {
        priv_type= TOKEN_KEYWORD_SHOW;
        is_maybe_all= false;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATABASES") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VIEW") == 1) {;}
        else if ((hparse_f_accept(FLAG_VERSION_MARIADB_11_4, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CREATE") == 1)
             && (hparse_f_accept(FLAG_VERSION_MARIADB_11_4, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROUTINE") == 1)) {;}
        else hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if ((is_role_ok == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SHUTDOWN") == 1))
      {
        priv_type= TOKEN_KEYWORD_SHUTDOWN;
        is_maybe_all= false;
      }
      else if ((is_role_ok == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SUPER") == 1))
      {
        priv_type= TOKEN_KEYWORD_SUPER;
        is_maybe_all= false;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TRIGGER") == 1)
      {
        priv_type= TOKEN_KEYWORD_TRIGGER;
        is_maybe_all= false;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UPDATE") == 1)
      {
        priv_type= TOKEN_KEYWORD_UPDATE;
        is_maybe_all= false;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USAGE") == 1)
      {
        priv_type= TOKEN_KEYWORD_USAGE;
        is_maybe_all= false;
      }
      else if ((hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BACKUP_ADMIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BINLOG_ADMIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONNECTION_ADMIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENCRYPTION_KEY_ADMIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIREWALL_ADMIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIREWALL_USER") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GROUP_REPLICATION_ADMIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PERSIST_RO_VARIABLES_ADMIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLICATION_SLAVE_ADMIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RESOURCE_GROUP_ADMIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RESOURCE_GROUP_USER") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROLE_ADMIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SET_USER_ID") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SYSTEM_VARIABLES_ADMIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VERSION_TOKEN_ADMIN") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "XA_RECOVER_ADMIN") == 1))
      {
        priv_type= main_token_types[hparse_i_of_last_accepted];
        is_maybe_all= false;
      }
    }
    if ((priv_type == 0) && (is_role_ok))
    {
      priv_type= TOKEN_KEYWORD_ROLE;
      if (hparse_f_user_or_role_name(TOKEN_REFTYPE_ROLE) == 1)
      {
        *role_name_seen= true;
        if ((hparse_dbms_mask & (FLAG_VERSION_MARIADB_ALL)) != 0) break;
      }
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if (hparse_errno > 0) return;
    ++count_of_grants;
    if ((priv_type == TOKEN_KEYWORD_SELECT)
     || (priv_type == TOKEN_KEYWORD_INSERT)
     || (priv_type == TOKEN_KEYWORD_UPDATE))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "("))
      {
        do
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
          if (hparse_errno > 0) return;
        } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
    }
  } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));

  if (is_privilege_ok == false) return;

  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON");
  if (hparse_errno > 0) return;
  main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROCEDURE") == 1)
  {
    if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_PROCEDURE, TOKEN_REFTYPE_PROCEDURE) == 0)
      hparse_f_error();
    return;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FUNCTION") == 1)
  {
    if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION, TOKEN_REFTYPE_FUNCTION) == 0)
      hparse_f_error();
    return;
  }
  if (hparse_f_accept(FLAG_VERSION_PLSQL | FLAG_VERSION_MARIADB_11_4, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PACKAGE") == 1)
  {
    hparse_f_accept(FLAG_VERSION_PLSQL | FLAG_VERSION_MARIADB_11_4, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BODY");
    if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_PACKAGE, TOKEN_REFTYPE_PACKAGE) == 0)
      hparse_f_error();
    return;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE") == 1)
    {;}
  if (hparse_f_qualified_name_of_object_with_star(TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0)
    hparse_f_error();
  if (hparse_errno > 0) return;
}

/* Todo: VALUES(...) is now called from hparse_f_query(). Redundancy? */
void MainWindow::hparse_f_insert_or_replace()
{
#ifdef ALLOW_CONFLICT_CLAUSES
  if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OR") == 1)
  {
    hparse_f_conflict_algorithm();
    if (hparse_errno > 0) return;
  }
#endif
  if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INTO");
    if (hparse_errno > 0) return;
  }
  else
    hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INTO");
  if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  hparse_f_partition_list(true, false);
  if (hparse_errno > 0) return;
  bool col_name_list_seen= false;
  /* MySQL/MariaDB would allow (query) here, Tarantool would not. */
  if (((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
   || (hparse_f_is_query(false) == false))
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
    {
      do
      {
        if (hparse_f_qualified_name_of_operand(0, false,false,true) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
      col_name_list_seen= true;
    }
  }
  if ((col_name_list_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SET") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    hparse_f_assignment(TOKEN_KEYWORD_INSERT, 0, false, false);
    if (hparse_errno > 0) return;
  }
  else if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALUES") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALUE")) == 1)
  {
    /* 2017-04-30: "VALUES ()" is legal */
    for (;;)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return;
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR,")") == 0)
      {
        hparse_f_expression_list(TOKEN_KEYWORD_INSERT);
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 0) break;
    }
  }
  else if (hparse_f_query(0, false, false, false) == 1)
  {
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DEFAULT, "DEFAULT") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_VALUES, "VALUES");
    if (hparse_errno > 0) return;
  }
  else hparse_f_error();
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_conflict_clause()
{
#ifdef ALLOW_CONFLICT_CLAUSES
  if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONFLICT");
    if (hparse_errno > 0) return;
    hparse_f_conflict_algorithm();
    if (hparse_errno > 0) return;
  }
#endif
}

void MainWindow::hparse_f_conflict_algorithm()
{
#ifdef ALLOW_CONFLICT_CLAUSES
  if ((hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROLLBACK") == 1)
   || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ABORT") == 1)
   || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FAIL") == 1)
   || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORE") == 1)
   || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLACE") == 1))
  {
    return;
  }
  hparse_f_error();
#endif
}

void MainWindow::hparse_f_condition_information_item_name()
{
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CLASS_ORIGIN") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SUBCLASS_ORIGIN") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RETURNED_SQLSTATE") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MESSAGE_TEXT") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MYSQL_ERRNO") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONSTRAINT_CATALOG") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONSTRAINT_SCHEMA") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONSTRAINT_NAME") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CATALOG_NAME") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SCHEMA_NAME") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE_NAME") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLUMN_NAME") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURSOR_NAME") == 1)
   || (hparse_f_accept(FLAG_VERSION_MARIADB_10_10, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROW_NUMBER") == 1)) {;}
  else hparse_f_error();
}

int MainWindow::hparse_f_signal_or_resignal(int who_is_calling, int block_top)
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQLSTATE") == 1)
  {
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALUE");
    if (hparse_f_literal(TOKEN_REFTYPE_SQLSTATE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
  }
  else if (hparse_f_find_define(block_top, TOKEN_REFTYPE_CONDITION_DEFINE, TOKEN_REFTYPE_CONDITION_REFER, false) > 0) {;}
  else if (who_is_calling == TOKEN_KEYWORD_SIGNAL) return 0;
  if (hparse_errno > 0) return 0;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SET") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    do
    {
      hparse_f_condition_information_item_name();
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_errno > 0) return 0;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ANY) == 0)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      }
      if (hparse_errno > 0) return 0;
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  }
  return 1;
}

/* An INTO clause may appear in two different places within a SELECT. */
/* Todo: Use TOKEN_REFTYPE_VARIABLE with hparse_f_qualified_name_of_operand() when it works correctly again. */
int MainWindow::hparse_f_into()
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INTO"))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OUTFILE") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      /* CHARACTER SET character-set-name and export_options */
      hparse_f_infile_or_outfile();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DUMPFILE") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else
    {
      do
      {
        if (hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
        if (hparse_errno > 0) return 0;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
    return 1;
  }
  return 0;
}

/*
  Todo: A problem with WITH is that it makes it hard to know what the true
  statement type is. Perhaps we should change main_token_flags?
  See also get_statement_type().
  For the statement that follows WITH, turn off TOKEN_FLAG_IS_START_STATEMENT
  because that wrecks the debugger. But it starts a clause, perhaps.
*/
void MainWindow::hparse_f_with_clause(int block_top, bool is_statement)
{
  hparse_f_accept(FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RECURSIVE");
  hparse_f_expect(FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_2_2, TOKEN_REFTYPE_WITH_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
  if (hparse_errno > 0) return;
  hparse_f_column_list(0, 0);
  if (hparse_errno > 0) return;
  hparse_f_expect(FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS");
  if (hparse_errno > 0) return;
  hparse_f_expect(FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  if (hparse_f_accept(FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALUES") == 1)
  {
    hparse_f_values(false);
    if (hparse_errno > 0) return;
  }
  else
  {
    hparse_f_query(0, false, false, false);
    if (hparse_errno > 0) return;
  }
  hparse_f_expect(FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
  if (is_statement)
  {
    if (hparse_f_is_special_verb(TOKEN_KEYWORD_WITH) == false) return;
  }
  else
  {
    if (QString::compare(hparse_token, "SELECT", Qt::CaseInsensitive) != 0)
    {
      /* guaranteed to fail */
      hparse_f_expect(FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_2_2, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SELECT");
      if (hparse_errno > 0) return;
    }
  }
  hparse_f_pseudo_statement(block_top);
  return;
}

/*
  Todo: check VALUES(...) ORDER BY etc. again
  (last time I tried, it wasn't legal despite SQLite diagrams).
*/
int MainWindow::hparse_f_values(bool is_top)
{
  hparse_statement_type= TOKEN_KEYWORD_VALUES;
  hparse_f_expression_list(TOKEN_KEYWORD_VALUES);
  int late_result= hparse_f_late_select_clauses(is_top, TOKEN_KEYWORD_VALUES);
  if (hparse_errno != 0) return 0;
  if (late_result == 0)
  {
    hparse_f_unionize(is_top);
    if (hparse_errno > 0) return 0;
  }
  return 0;
}

int MainWindow::hparse_f_unionize(bool is_top)
{
  bool is_union= false;
  bool is_except= false;
  bool is_intersect= false;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNION") == 1)
    is_union= true;
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0_31|FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXCEPT") == 1)
    is_except= true;
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0_31|FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INTERSECT") == 1)
    is_intersect= true;
  if ((is_union == false) && (is_except == false) && (is_intersect == false))
  {
    return 1;
  }
  main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
  if ((is_union == true)
   || ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_1) != 0)
   || ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0_31) != 0))
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1)
      {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISTINCT") == 1)
      {;}
  }
  else /* must be except|intersect with mariadb 10.3 */
  {
    if (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISTINCT") == 1)
      {;}
  }
//  if (hparse_f_accept(FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALUES") == 1)
//  {
//    hparse_f_values(false);
//    if (hparse_errno > 0) return 0;
//  }
//  else
  if (hparse_f_query(0, false, false, is_top) == 0)
  {
    hparse_f_error();
    return 0;
  }
  if (hparse_errno > 0) return 0;
  return 1;
}

#if (OCELOT_EXTENDER == 1)
/*
  Re hparse_flow_count: This should go up for compound-statement starters BEGIN CASE FOR IF LOOP REPEAT WHILE
  (and CREATE PACKAGE because it has an END), and go down for END. Therefore if it's > 0 we're inside a compound statement.
  Now we're only using hparse_flow_count within hparse_f_is_extender_ok() but it could be useful elsewhere too.
  Alternatives hparse_begin_count and block_top are less dependable.
*/
/*
  extender can't fix semiselects within statements if:
  Within compound statement: simple if we've done the counting correctly for hparse_flow_count.
  Within CREATE EVENT|FUNCTION|PROCEDURE|TRIGGER|VIEW: only a bit harder because of course we'll encounter start-of-statement,
  but we have to go back before it check that it's preceded by a delimiter. It's assumed that it's enough to find EVENT etc.,
  since (I think) anything before it will be CREATE or ALTER or SHOW. Todo: check that out.
  Within a semiselect: this can only happen in a semiselect's WHERE clause, I could handle that but it would be hard.
*/
bool MainWindow::hparse_f_is_extender_ok()
{
  if ((ocelot_statement_syntax_checker.toInt() & 7) != 7) return false;
  if (hparse_flow_count > 0) return false;
  for (int i_of_statement= hparse_i - 1; i_of_statement >= 0; --i_of_statement)
  {
    int t= main_token_types[i_of_statement];
    /* !!!! make sure we're within CREATE !!!! */
    if ((t == TOKEN_KEYWORD_EVENT) || (t == TOKEN_KEYWORD_FUNCTION)
     || (t == TOKEN_KEYWORD_PROCEDURE) || (t == TOKEN_KEYWORD_TRIGGER) || (t == TOKEN_KEYWORD_VIEW))
       return false;
    //if (main_token_types[i_of_statement] == TOKEN_KEYWORD_UNION) return true; /* todo: beware maybe UNION within subquery? */
    if ((main_token_flags[i_of_statement]&TOKEN_FLAG_IS_START_STATEMENT) != 0)
    {
      if ((main_token_flags[i_of_statement]&TOKEN_FLAG_IS_SEMISELECT_ALL) == TOKEN_FLAG_IS_SEMISELECT) return false;
      int i_of_prev= next_i(i_of_statement, -1);
      if (i_of_prev == 0) break;
      QString s_of_prev= hparse_text_copy.mid(main_token_offsets[i_of_prev], main_token_lengths[i_of_prev]);
      if ((s_of_prev == ";") || (s_of_prev == hparse_delimiter_str) || (s_of_prev == "\\G")) break;
    }
  }
  return true;
}
#endif

/*
  Return true if following looks like start of query|subquery.
  If false, do accept() anyway so it appears in the expected list.
  Warn: we only do the accept() anysay for keywords, not for (, so ... UNION ( looks error but UNION (WITH is ok
  Warn: sometimes block_top == -1 when we don't happen to know it, that should be fixed someday.
*/
bool MainWindow::hparse_f_is_query(bool is_statement)
{
  /* Check if starts with (repeatedly) "(" | "SELECT" | "VALUES" | "WITH" | "TABLE" */
  QString s= hparse_token;
  int i= hparse_i;
  int parentheses_count= 0;
  while (s == "(")
  {
    i= next_i(i, +1);
    s= hparse_text_copy.mid(main_token_offsets[i], main_token_lengths[i]);
    ++parentheses_count;
  }
  bool is_semiselect_ok= false;
  if ((is_statement == true) && (parentheses_count == 0)) is_semiselect_ok= true;
#if (OCELOT_EXTENDER == 1)
  else is_semiselect_ok= hparse_f_is_extender_ok();
#endif
  s= s.toUpper();
  bool is_query= false;
  if (s == "SELECT") is_query= true;
  if (s == "TABLE")
  {
    if (hparse_dbms_mask & (FLAG_VERSION_TARANTOOL|FLAG_VERSION_MYSQL_8_0))
      is_query= true;
  }
  if (s == "VALUES")
  {
    if (is_statement == false) is_query= true;
    else if (hparse_dbms_mask & (FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_3))
      is_query= true;
  }
  if (s == "WITH")
  {
    if (hparse_dbms_mask & (FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_2_2))
      is_query= true;
  }
  if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
  {
    if ((is_statement == true) && (parentheses_count == 0))
    {
      if (s == "EXPLAIN")
        is_query= true;
    }
  }
#if (OCELOT_MYSQL_INCLUDE == 1)
  if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0)
  {
    if (is_semiselect_ok == true)
    {
       if ((s == "ANALYZE") || (s == "CHECK") || (s == "CHECKSUM") || (s == "DESC") || (s == "DESCRIBE")
           || (s == "EXPLAIN") || (s == "HELP") || (s == "SHOW"))
      {
        is_query= true;
        main_token_flags[i] |= TOKEN_FLAG_IS_SEMISELECT;
      }
    }
  }
#endif
  if (is_query == false)
  {
    /* guaranteed to fail */
    /* todo: find out why I'm not saying "if (s != "SELECT" here */
    hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "SELECT");
    /* guaranteed to fail */
    if (s != "TABLE")
      hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "TABLE");
    /* guaranteed to fail */
    if (s != "VALUES")
      hparse_f_accept(FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "VALUES");
    /* guaranteed to fail */
    if (s != "WITH")
      hparse_f_accept(FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_2_2, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "WITH");
    if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
    {
      /* guaranteed to fail */
      if (s != "EXPLAIN")
        hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "EXPLAIN");
    }
    if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0)
    {
      if (is_semiselect_ok == true)
      {
        /* all guaranteed to fail */
        if (s != "ANALYZE")
           hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "ANALYZE");
        if (s != "CHECK")
           hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "CHECK");
        if (s != "CHECKSUM")
           hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "CHECKSUM");
        if (s != "DESC")
           hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "DESC");
        if (s != "DESCRIBE")
          hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "DESCRIBE");
        if (s != "EXPLAIN")
          hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "EXPLAIN");
        if (s != "HELP")
          hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "HELP");
        if (s != "SHOW")
          hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "SHOW");
      }
    }
  }
  return is_query;
}

/*
  Queries | subqueries can start with SELECT or VALUES or WITH or TABLE.
  May be parenthesized. We don't distinguish table versus scalar.
  Return 1 if you ate a query. Return 0 if not-a-query or error.
  "SELECT ..." or "(SELECT ...)" -- ditto for VALUES + WITH
  Todo: For a SELECT statement (not a subquery) that starts with "(",
        TOKEN_FLAG_IS_START_STATEMENT should go on for the "(" rather
        than for the "SELECT". Otherwise the debugger might choke (?).
  Todo: BUG: block_top is okay for calling from hparse_f_statement(),
        but below that we're passing 0, which is garbage.
*/
int MainWindow::hparse_f_query(int block_top, bool query_is_already_checked, bool is_statement, bool is_top)
{
  if (query_is_already_checked == false)
  {
    if (hparse_f_is_query(is_statement) == false) return 0;
  }
  if (is_statement)
  {
    /* We might be flagging "(" rather than the statement first word */
    main_token_flags[hparse_i] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
  }
  if (is_top == false) hparse_is_in_subquery= true;
  int i= hparse_f_deep_query(block_top, is_statement, is_top);
  hparse_is_in_subquery= false;
  return i;
}

int MainWindow::hparse_f_deep_query(int block_top, bool is_statement, bool is_top)
{
  /* todo: find out what this statement is for */
  if (hparse_subquery_is_allowed == false) hparse_subquery_is_allowed= true;
  /* SELECT|VALUES|WITH, even as statements, can be parenthesized. */
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
  {
    if (hparse_f_query(block_top, true, is_statement, false) == 0)
    {
      hparse_f_error();
      return 0;
    }
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return 0;
    /* allow for (select ....) union select ... as opposed to (select ... union select ...) */
    /* allow for (select ...) order by 1 */
    /* so this allows too much too often after ) but might be better than allowing nothing */
    int late_result= hparse_f_late_select_clauses(is_top, TOKEN_KEYWORD_VALUES);
    if (hparse_errno != 0) return 0;
    if (late_result == 0)
    {
      hparse_f_unionize(is_top);
      if (hparse_errno > 0) return 0;
    }
    return 1;
  }
  /* todo: we aren't checking if VALUES is legal here */
int i_of_start_of_query= -1;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "SELECT") == 1)
  {
    if (is_statement) hparse_statement_type= TOKEN_KEYWORD_SELECT;
    hparse_f_select(is_top, is_statement);
    if (hparse_errno > 0) return 0;
  }
#if (OCELOT_MYSQL_INCLUDE == 1)
  /* Todo: TABLE statement is new in MySQL 8.0.19. I haven't checked it works in exotic situations. */
  /* e.g. https://dev.mysql.com/doc/refman/8.0/en/table.html says it should work with UNION and INTO. */
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TABLE, "TABLE"))
  {
    if (is_statement) hparse_statement_type= TOKEN_KEYWORD_TABLE;
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0)
    {
      hparse_f_error();
      return 0;
    }
    int late_result= hparse_f_late_select_clauses(is_top, TOKEN_KEYWORD_TABLE);
    if (hparse_errno != 0) return 0;
    if (late_result == 0)
    {
      hparse_f_unionize(is_top);
      if (hparse_errno > 0) return 0;
    }
  }
#endif
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "VALUES") == 1)
  {
    if (is_statement) hparse_statement_type= TOKEN_KEYWORD_VALUES;
    hparse_f_values(is_top);
    if (hparse_errno > 0) return 0;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "WITH") == 1)
  {
    if (is_statement) hparse_statement_type= TOKEN_KEYWORD_WITH;
    hparse_f_with_clause(block_top, is_statement);
    if (hparse_errno > 0) return 0;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ANALYZE") == 1)
  {
    if (is_statement) hparse_statement_type= TOKEN_KEYWORD_ANALYZE;
    /* main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE; */
    i_of_start_of_query= hparse_i_of_last_accepted;
    if (((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
     && (hparse_token == ";"))
    {
      ; /* it's just "analyze;" */
    }
    else
    {
      int table_or_view;
      if (hparse_f_analyze_or_optimize(TOKEN_KEYWORD_ANALYZE, &table_or_view) == 1)
      {
        if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0)
        {
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PERSISTENT") == 1)
          {
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR");
            if (hparse_errno > 0) return 0;
            if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1) goto return_1;
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLUMNS");
            if (hparse_errno > 0) return 0;
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
            if (hparse_errno > 0) return 0;
            for (;;)
            {
              if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
              {
                if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1) continue;
              }
              break;
            }
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
            if (hparse_errno > 0) return 0;
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEXES");
            if (hparse_errno > 0) return 0;
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
            if (hparse_errno > 0) return 0;
            for (;;)
            {
              if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_INDEX,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
              {
                if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1) continue;
              }
              break;
            }
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
            if (hparse_errno > 0) return 0;
          }
        }

        if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) != 0)
        {
          bool is_update= false;
          bool is_drop= false;
          if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UPDATE") == 1)
            is_update= true;
          else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DROP") == 1)
            is_drop= true;
          if ((is_update) || (is_drop))
          {
            hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HISTOGRAM");
            if (hparse_errno > 0) return 0;
            hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON");
            if (hparse_errno > 0) return 0;
            do
            {
              hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
              if (hparse_errno > 0) return 0;
            } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));

            if (is_update)
            {
              if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITH") == 1)
              {
                if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
                if (hparse_errno > 0) return 0;
                hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BUCKETS");
                if (hparse_errno > 0) return 0;
              }
            }
          }
        }
        /* todo: this return was in the unmoved part. are we actually expected to return 1? if so just fall through */
        goto return_1;
      }
      if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FORMAT") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return 0;
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TRADITIONAL") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "JSON");
          if (hparse_errno > 0) return 0;
        }
        if (hparse_f_explainable_statement(block_top) == 1) goto return_1;
        if (hparse_errno > 0) return 0;
      }
      hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHECK") == 1)
  {
    if (is_statement) hparse_statement_type= TOKEN_KEYWORD_CHECK;
    i_of_start_of_query= hparse_i_of_last_accepted;
    /* main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE; */
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE") == 1)
    {
      if (hparse_errno > 0) return 0;
      do
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      for (;;)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UPGRADE");
          if (hparse_errno > 0) return 0;
        }
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "QUICK") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FAST") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MEDIUM") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXTENDED") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHANGED") == 1) {;}
        else break;
      }
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VIEW") == 1))
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_VIEW) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else hparse_f_error();
    if (hparse_errno > 0) return 0;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHECKSUM") == 1)
  {
    /* Todo: find out why this was commented out */
    //hparse_statement_type= TOKEN_KEYWORD_CHECKSUM;
    i_of_start_of_query= hparse_i_of_last_accepted;
    //if (is_statement) hparse_statement_type= TOKEN_KEYWORD_CHECKSUM;
    /* main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE; */
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE");
    if (hparse_errno > 0) return 0;
    do
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "QUICK") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXTENDED") == 1)) {;}
    else hparse_f_error();
    if (hparse_errno > 0) return 0;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DESC, "DESC") == 1)
  {
    if (is_statement) hparse_statement_type= TOKEN_KEYWORD_DESC;
    i_of_start_of_query= hparse_i_of_last_accepted;
    /* main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE; */
    hparse_f_explain_or_describe(block_top);
    if (hparse_errno > 0) return 0;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DESCRIBE, "DESCRIBE"))
  {
    if (is_statement) hparse_statement_type= TOKEN_KEYWORD_DESCRIBE;
    i_of_start_of_query= hparse_i_of_last_accepted;
    /* main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE; */
    hparse_f_explain_or_describe(block_top);
    if (hparse_errno > 0) return 0;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_EXPLAIN, "EXPLAIN"))
  {
    if (is_statement) hparse_statement_type= TOKEN_KEYWORD_EXPLAIN;
    i_of_start_of_query= hparse_i_of_last_accepted;
    /* main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE; */
    if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_EXPLAIN, "QUERY") == 1)
    {
      hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_EXPLAIN, "PLAN");
      if (hparse_errno > 0) return 0;
    }
    hparse_f_explain_or_describe(block_top);
    if (hparse_errno > 0) return 0;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_HELP, "HELP"))
  {
    /*
      Elsewhere we decide that HELP alone or HELP delimiter is a client
      statement. Here we see if it's HELP 'string' i.e. a server
      statement. MariaDB accepts HELP identifier|reserved-word but it's
      not documented, we would treat it as an error.
    */
    if (is_statement) hparse_statement_type= TOKEN_KEYWORD_HELP;
    i_of_start_of_query= hparse_i_of_last_accepted;
    /* main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE; */
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SHOW, "SHOW") == 1)
  {
    if (is_statement) hparse_statement_type= TOKEN_KEYWORD_SHOW;
    i_of_start_of_query= hparse_i_of_last_accepted;
    /* main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE; */
    if (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1)
    {
      if (hparse_f_accept_slaves_or_replicas(FLAG_VERSION_MARIADB_ALL) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS");
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_9, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ANALYZE") == 1)
    {
      /* Todo: this is duplicated for SHOW EXPLAIN. Merge. */
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FORMAT") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
        if (hparse_errno > 0) return 0;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "JSON");
        if (hparse_errno > 0) return 0;
      }
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR");
      if (hparse_errno > 0) return 0;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AUTHORS") == 1) {;} /* removed in MySQL 5.6.8 */
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BINARY") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOGS");
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BINLOG") == 1) /* show binlog */
    {
      if (hparse_f_accept(FLAG_VERSION_MARIADB_10_1, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_STATUS, "STATUS") == 0)
        {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EVENTS");
        if (hparse_errno > 0) return 0;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IN") == 1)
        {
          main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
          if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
          if (hparse_errno > 0) return 0;
        }
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM") == 1)
        {
          if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();
          if (hparse_errno > 0) return 0;
        }
        hparse_f_limit(TOKEN_KEYWORD_SHOW);
        if (hparse_errno > 0) return 0;
      }
    }
    else if (hparse_f_character_set() == 1) /* show character set */
    {
      hparse_f_like_or_where();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_errno > 0) return 0;
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CLIENT_STATISTICS") == 1))
    {
      ;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATION") == 1) /* show collation */
    {
      hparse_f_like_or_where();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLUMNS") == 1) /* show columns */
    {
      hparse_f_show_columns();
      if (hparse_errno > 0) return 0;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONTRIBUTORS") == 1))
    {
      ;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COUNT") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "*");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return 0;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ERRORS") == 1) ;
      else
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WARNINGS");
        if (hparse_errno > 0) return 0;
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CREATE") == 1) /* show create ... */
    {
#if (FLAG_VERSION_MARIADB_12_0 != 0)
      if (hparse_f_accept(FLAG_VERSION_MARIADB_12_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CATALOG") == 1)
      {
        hparse_f_if_not_exists();
        if (hparse_errno > 0) return 0;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return 0;
      }
      else
#endif
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATABASE") == 1)
      {
        hparse_f_if_not_exists();
        if (hparse_errno > 0) return 0;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EVENT") == 1)
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_EVENT, TOKEN_REFTYPE_EVENT) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FUNCTION") == 1)
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION, TOKEN_REFTYPE_FUNCTION) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PACKAGE") == 1)
      {
        /* There will be an END so consider this an implicit BEGIN. Todo: should mark TOKEN_FLAG_IS_FLOW_CONTROL too? */
        ++hparse_flow_count;
        hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BODY, "BODY");
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_PACKAGE, TOKEN_REFTYPE_PACKAGE) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROCEDURE") == 1)
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_PROCEDURE, TOKEN_REFTYPE_PROCEDURE) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SCHEMA") == 1)
      {
        hparse_f_if_not_exists();
        if (hparse_errno > 0) return 0;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SEQUENCE") == 1)
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_SEQUENCE, TOKEN_REFTYPE_SEQUENCE) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE") == 1)
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TRIGGER") == 1)
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TRIGGER, TOKEN_REFTYPE_TRIGGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      else if (((hparse_dbms_mask & (FLAG_VERSION_MYSQL_5_7|FLAG_VERSION_MARIADB_10_2_2)) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USER") == 1))
      {
        if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VIEW") == 1)
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_VIEW, TOKEN_REFTYPE_VIEW) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATABASES") == 1) /* show databases */
    {
      hparse_f_like_or_where();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENGINE") == 1) /* show engine */
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS") == 0)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MUTEX");
        if (hparse_errno > 0) return 0;
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENGINES") == 1) /* show engines */
    {
      ;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ERRORS") == 1) /* show errors */
    {
      hparse_f_limit(TOKEN_KEYWORD_SHOW);
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EVENTS") == 1) /* show events */
    {
      hparse_f_from_or_like_or_where();
      if (hparse_errno > 0) return 0;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXPLAIN") == 1))
    {
      if (hparse_f_accept(FLAG_VERSION_MARIADB_10_9, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FORMAT") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
        if (hparse_errno > 0) return 0;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "JSON");
        if (hparse_errno > 0) return 0;
      }
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR");
      if (hparse_errno > 0) return 0;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXTENDED") == 1) /* show columns */
    {
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLUMNS") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIELDS") == 1))
      {
        hparse_f_show_columns();
        if (hparse_errno > 0) return 0;
      }
      else hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIELDS") == 1) /* show columns */
    {
      hparse_f_show_columns();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FULL") == 1) /* show full [columns|tables|etc.] */
    {
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLUMNS") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIELDS") == 1))
      {
        hparse_f_show_columns();
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLES") == 1)
      {
        hparse_f_from_or_like_or_where();
        if (hparse_errno > 0) return 0;
      }
      else hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROCESSLIST");
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FUNCTION") == 1) /* show function [code] */
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CODE") == 1)
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION, TOKEN_REFTYPE_FUNCTION) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS") == 1)
      {
        hparse_f_like_or_where();
        if (hparse_errno > 0) return 0;
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GLOBAL") == 1) /* show global ... */
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS") == 1)
      {
        hparse_f_like_or_where();
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARIABLES") == 1)
      {
        hparse_f_like_or_where();
        if (hparse_errno > 0) return 0;
      }
      else hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GRANTS") == 1) /* show grants */
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR") == 1)
      {
        if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USING") == 1)
      {
        do
        {
          if (hparse_f_user_or_role_name(TOKEN_REFTYPE_ROLE) == 0) hparse_f_error();
          if (hparse_errno > 0) return 0;
        } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX") == 1) /* show index */
    {
      hparse_f_indexes_or_keys();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEXES") == 1) /* show indexes */
    {
      hparse_f_indexes_or_keys();
      if (hparse_errno > 0) return 0;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX_STATISTICS") == 1))
    {
      ;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEYS") == 1) /* show keys */
    {
      hparse_f_indexes_or_keys();
      if (hparse_errno > 0) return 0;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCALES") == 1))
    {
      ;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER") == 1) /* show master [status|logs\ */
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS") == 0)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOGS");
      }
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OPEN") == 1) /* show open [tables] */
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLES");
      if (hparse_errno > 0) return 0;
      hparse_f_from_or_like_or_where();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_PLSQL | FLAG_VERSION_MARIADB_11_4, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PACKAGE") == 1)
    {
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BODY");
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS");
      if (hparse_errno > 0) return 0;
      hparse_f_like_or_where();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PLUGINS") == 1) /* show plugins */
    {
      if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SONAME") == 1))
      {
        if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 1) {;}
        else hparse_f_from_or_like_or_where();
        if (hparse_errno > 0) return 0;
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PRIVILEGES") == 1) /* show privileges */
    {
      ;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROCEDURE") == 1) /* show procedure [code|status] */
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CODE") == 1)
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_PROCEDURE, TOKEN_REFTYPE_PROCEDURE) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS") == 1)
      {
        hparse_f_like_or_where();
        if (hparse_errno > 0) return 0;
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROCESSLIST") == 1) /* show processlist */
    {
      ;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROFILE") == 1) /* show profile */
    {
      for (;;)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BLOCK") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IO");
          if (hparse_errno > 0) return 0;
        }
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BLOCK_IO") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONTEXT") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SWITCHES");
          if (hparse_errno > 0) return 0;
        }
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CPU") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IPC") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MEMORY") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PAGE") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FAULTS");
          if (hparse_errno > 0) return 0;
        }
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SOURCE") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SWAPS") == 1) {;}
        else break;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1) continue;
        break;
      }
      if (hparse_errno > 0) return 0;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "QUERY");
        if (hparse_errno > 0) return 0;
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      hparse_f_limit(TOKEN_KEYWORD_SHOW);
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROFILES") == 1) /* show profiles */
    {
      ;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "QUERY_RESPONSE_TIME") == 1))
    {
      ;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RELAYLOG") == 1) /* show relaylog */
    {
      if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING);
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EVENTS");
      if (hparse_errno > 0) return 0;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IN") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
        if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM") == 1)
      {
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      hparse_f_limit(TOKEN_KEYWORD_SHOW);
      if (hparse_errno > 0) return 0;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR") == 1)
      {
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SCHEMAS") == 1) /* show schemas */
    {
      hparse_f_like_or_where();
      if (hparse_errno > 0) return 0;
    }
#if (FLAG_VERSION_MARIADB_12_0 != 0)
    /* todo: also SHOW CATALOG STATUS, eh? */
    else if (hparse_f_accept(FLAG_VERSION_MARIADB_12_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SERVER") == 1) /* show server ... */
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS");
      if (hparse_errno > 0) return 0;
    }
#endif
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SESSION") == 1) /* show session ... */
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS") == 1)
      {
        hparse_f_like_or_where();
        if (hparse_errno > 0) return 0;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARIABLES") == 1)
      {
        hparse_f_like_or_where();
        if (hparse_errno > 0) return 0;
      }
      else hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept_slave_or_replica(FLAG_VERSION_MYSQL_OR_MARIADB_ALL) == 1) /* show slave */
    {
      if (((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_4) == 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HOSTS") == 1)) {;}
      else
      {
        if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING);
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS");
        if (hparse_errno > 0) return 0;
        if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_ALL) != 0)
        {
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NONBLOCKING") == 1) {;}
          hparse_f_for_channel();
          if (hparse_errno > 0) return 0;
        }
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REPLICAS, "REPLICAS") == 1) /* show replicas */
    {
      ;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS") == 1) /* show status */
    {
      hparse_f_like_or_where();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STORAGE") == 1) /* show storage */
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENGINES");
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE") == 1) /* show table */
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS");
      if (hparse_errno > 0) return 0;
      hparse_f_from_or_like_or_where();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLES") == 1) /* show tables */
    {
      hparse_f_from_or_like_or_where();
      if (hparse_errno > 0) return 0;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE_STATISTICS") == 1))
    {
      ;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TRIGGERS") == 1) /* show triggers */
    {
      hparse_f_from_or_like_or_where();
      if (hparse_errno > 0) return 0;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USER_STATISTICS") == 1))
    {
      ;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARIABLES") == 1) /* show variables */
    {
      hparse_f_like_or_where();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WARNINGS") == 1) /* show warnings */
    {
      hparse_f_limit(TOKEN_KEYWORD_SHOW);
      if (hparse_errno > 0) return 0;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WSREP_MEMBERSHIP") == 1))
    {
      ;
    }
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WSREP_STATUS") == 1))
    {
      ;
    }
    else hparse_f_error();
    if (hparse_errno > 0) return 0;
  }
  if ((hparse_statement_type == 0)
   || (hparse_statement_type == TOKEN_KEYWORD_WITH))
  {
    hparse_statement_type= TOKEN_KEYWORD_SELECT;
  }
//  {
//    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNION") == 1)
//    {
//      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
//      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISTINCT") == 1)) {;}
//      int return_value= hparse_f_query(0, false, is_statement, false);
//      if (hparse_errno > 0) return 0;
//      if (return_value == 0)
//      {
//        hparse_f_error();
//        return 0;
//      }
//    }
//    hparse_f_order_by(0);
//    if (hparse_errno > 0) return 0;
//    hparse_f_limit(TOKEN_KEYWORD_SELECT);
//    if (hparse_errno > 0) return 0;
//    return 1;
//  }
return_1:
#if (OCELOT_EXTENDER == 1)
/* TODO: !! MAKE SURE THIS IS ONLY FOR SEMISELECT! */
  if (((ocelot_statement_syntax_checker.toInt() & 7) == 7)
   && ((main_token_flags[i_of_start_of_query]&TOKEN_FLAG_IS_SEMISELECT_ALL) == TOKEN_FLAG_IS_SEMISELECT))
  {
    int i_of_select_part_2= hparse_i;
    main_token_flags[hparse_i] |= TOKEN_FLAG_IS_SEMISELECT_MID;
    int select_part_2_return= hparse_f_select_part_2(is_top);
    main_token_flags[hparse_i]|= TOKEN_FLAG_IS_SEMISELECT_END;
    if (select_part_2_return != 0)
    {
      if ((main_token_flags[i_of_start_of_query]&TOKEN_FLAG_IS_START_STATEMENT) != 0)
      {
        int i_of_prev= next_i(i_of_start_of_query, -1);
        QString s_of_prev= hparse_text_copy.mid(main_token_offsets[i_of_prev], main_token_lengths[i_of_prev]);
        if (s_of_prev != "(")
        {
          /* If it's a statement and we didn't add extra clauses, then server can do it all, so turn off semiselect. */
          if (hparse_i == i_of_select_part_2)
          {
            main_token_flags[i_of_start_of_query] &= (~TOKEN_FLAG_IS_SEMISELECT_ALL);
            main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_SEMISELECT_ALL);
          }
        }
      }
    }

    //hparse_f_unionize(is_top);
    if (hparse_errno > 0) return 0;
    return select_part_2_return;;
  }
#endif

  return 1;
}
int MainWindow::hparse_f_select(bool is_top, bool is_statement)
{
  /* We've already seen the word SELECT */
  /*
    The formatter would be happy if we considered this SELECT to be
    a statement start, but the debugger would get horribly confused,
    unless of course it really is a SELECT statement not part of one.
  */
  if (is_top == false)
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
  for (;;)
  {
    if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1)
     || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISTINCT") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DISTINCTROW") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HIGH_PRIORITY") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STRAIGHT_JOIN") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL_SMALL_RESULT") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL_BIG_RESULT") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL_BUFFER_RESULT") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL_CACHE") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL_NO_CACHE") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL_CALC_FOUND_ROWS") == 1)
     || (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNIQUE") == 1))
    {
      ;
    }
    else break;
  }
  int column_count=  hparse_f_expression_list(TOKEN_KEYWORD_SELECT);
  if (hparse_errno > 0) return 0;
  hparse_f_into();
  if (hparse_errno > 0) return 0;
  /* FROM + some subsequent clauses are optional if no columns seen */
  bool from_seen= false;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM") == 1)
    from_seen= true;
  if ((is_statement == true) && (from_seen == false) && (column_count > 0)) hparse_f_error();
  if (hparse_errno > 0) return 0;
  if (from_seen)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    /* DUAL is a reserved word, perhaps the only one that could ever be an identifier */
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_KEYWORD, "DUAL") != 1)
    {
      if (hparse_f_table_references() == 0) hparse_f_error();
    }
    if (hparse_errno > 0) return 0;
  }
  return hparse_f_select_part_2(is_top);
}

int MainWindow::hparse_f_select_part_2(bool is_top)
{
  hparse_f_where();
  if (hparse_errno > 0) return 0;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GROUP"))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY");
    if (hparse_errno > 0) return 0;
    do
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return 0;
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ASC") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DESC") == 1)) {;}
    } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITH") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROLLUP");
      if (hparse_errno > 0) return 0;
    }
  }
  /* No need to check if GROUP BY was seen, now everybody accepts HAVING without it */
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HAVING"))
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return 0;
    }
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WINDOW"))
  {
    do
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_WINDOW_DEFINE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS");
      if (hparse_errno > 0) return 0;
      /* todo: "true" is just a hope, we must know if func is aggregate */
      hparse_f_window_spec(true);
      if (hparse_errno > 0) return 0;
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  }
  int late_result= hparse_f_late_select_clauses(is_top, TOKEN_KEYWORD_SELECT);
  if (late_result == 0)
  {
    hparse_f_unionize(is_top);
    if (hparse_errno > 0) return 0;
  }
  return 1;
}

/*
  After SELECT ... WHERE clause we might have ORDER BY, LIMIT, PROCEDURE, INTO, FOR, LOCK
  Some of these clauses might be in MariaDB VALUES or MySQL TABLE statements, though not always documented.
  (We are accepting the syntax even when it's not documented, hoping that it will be supported in future.)
  Tarantool allows SELECT ... ORDER BY, and SELECT ... LIMIT, and VALUES ... LIMIT, that's all.
  In a subquery the INTO and FOR and LOCK won't be allowed.
  Todo: For VALUES (...) ORDER BY args; the args must be ordinals
  Todo: although all UNIONed statements can be is_top==true, only the last one can have INTO.
        therefore "return 1" if INTO seen -- and if it's 1, unionize() won't be called
  Todo: MySQL 8 allows INTO both before and after FOR
  Todo: MySQL 8 allows ORDER BY ... WITH ROLLUP (?)
  Todo: During "select * from (select * from t into dumpfile 'x') as x;" is_top==false but it's illegal.
  Todo: MySQL 8 doesn't allow PROCEDURE, although MySQL 5.7 allowed it.
  Todo: MariaDB disallows PROCEDURE not only before UNION but also after UNION.
  Todo: PROCEDURE needs an argument list, even if empty.
  Todo: Call for things other than SELECT and VALUES, e.g. DELETE, but in that case only a few things work
        and probably not for Tarantool
*/
int MainWindow::hparse_f_late_select_clauses(bool is_top, int who_is_calling)
{
  if ((who_is_calling == TOKEN_KEYWORD_VALUES) && ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)) return 0;
  bool is_union_illegal_later= false;
  {
    int order_by_result= hparse_f_order_by(TOKEN_KEYWORD_SELECT); /* even if who_is_calling is something else */
    if (hparse_errno > 0) return 0;
    if (order_by_result == 1) is_union_illegal_later= true;
  }
  {
    int limit_result= hparse_f_limit(TOKEN_KEYWORD_SELECT); /* even if who_is_calling is something else */
    if (hparse_errno > 0) return 0;
    if (limit_result == 1) is_union_illegal_later= true;
  }
  if ((is_top == true)
   && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROCEDURE")))
  {
    is_union_illegal_later= true;
    hparse_f_call();
    if (hparse_errno > 0) return 0;
  }
  if ((is_top == true)
   && (hparse_f_into() == 1))
  {
    is_union_illegal_later= true;
    if (hparse_errno > 0) return 0;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR") == 1)
  {
    is_union_illegal_later= true;
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UPDATE") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SHARE") == 1))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OF") == 1)
      {
        do
        {
          if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE,TOKEN_REFTYPE_TABLE) == 0)
          {
            hparse_f_error();
            if (hparse_errno > 0) return 0;
          }
        } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      }
      if (hparse_f_accept(FLAG_VERSION_MARIADB_10_1, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WAIT") == 1)
      {
        if (hparse_f_literal(TOKEN_REFTYPE_PARTITION_NUMBER, FLAG_VERSION_MARIADB_10_1, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      else
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_MARIADB_10_1, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOWAIT") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SKIP") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCKED");
          if (hparse_errno > 0) return 0;
        }
      }
    }
    else
    {
      hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCK") == 1)
  {
    is_union_illegal_later= true;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IN");
    if (hparse_errno > 0) return 0;
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SHARE");
    if (hparse_errno > 0) return 0;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MODE");
    if (hparse_errno > 0) return 0;
    if (hparse_f_accept(FLAG_VERSION_MARIADB_10_1, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WAIT") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_PARTITION_NUMBER, FLAG_VERSION_MARIADB_10_1, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else hparse_f_accept(FLAG_VERSION_MARIADB_10_1, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOWAIT");
  }
  if (is_union_illegal_later == true) return 1;
  return 0;
}


void MainWindow::hparse_f_where()
{
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WHERE"))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    hparse_f_opr_1(0, 0);
  }
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_window_spec(bool function_is_aggregate)
{
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PARTITION, "PARTITION") == 1)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_OVER, "BY");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "("))
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    else
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
    }
  }
  if ((function_is_aggregate == false)
   && (QString::compare(hparse_token, "ORDER", Qt::CaseInsensitive) != 0))
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ORDER");
    if (hparse_errno > 0) return;
  }
  if ((hparse_f_order_by(0) == 1)
   && (function_is_aggregate == true))
  {
    /* window frame */
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RANGE") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROWS") == 1))
    {
      if (hparse_f_over_start(0) == 1) {;}
      else if (hparse_errno > 0) return;
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BETWEEN") == 1)
      {
        if (hparse_f_over_start(TOKEN_KEYWORD_BETWEEN) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AND");
        if (hparse_errno > 0) return;
        if (hparse_f_over_end() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
  }
  if (hparse_errno > 0) return;
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
}

int MainWindow::hparse_f_order_by(int who_is_calling)
{
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ORDER") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY");
    if (hparse_errno > 0) return 0;
    do
    {
      hparse_f_opr_1(who_is_calling, 0);
      if (hparse_errno > 0) return 0;
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ASC") == 0) hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DESC");
    } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    return 1;
  }
  else return 0;
}

/* LIMIT 1 or LIMIT 1,0 or LIMIT 1 OFFSET 0 from SELECT, DELETE, UPDATE, SHOW, or GROUP_CONCAT() */
/* Todo: disallow identifier unless we're inside a routine */
/* Todo: allow '?' if someday you parse within a prepared statement */
/* Iff MariaDB 10.6+ there can be a more standard OFFSET clause */
/* Note: Although MariaDB manual has "[count] ROWS" it seems that count is not optional */
int MainWindow::hparse_f_limit(int who_is_calling)
{
  if ((who_is_calling == TOKEN_KEYWORD_SELECT) && ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_10) != 0))
  {
    bool is_offset_seen= false;
    bool is_fetch_seen= false;
    if (hparse_f_accept(FLAG_VERSION_MARIADB_10_10, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_OFFSET, "OFFSET") == 1)
    {
      is_offset_seen= true;
      if (hparse_f_simple_value(true, true) == 0) return 0; /* expecting "integer {ROW|ROWS}" */
    }
    if (hparse_f_accept(FLAG_VERSION_MARIADB_10_10, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FETCH, "FETCH") == 1)
    {
      is_fetch_seen= true;
      if (hparse_f_accept(FLAG_VERSION_MARIADB_10_10, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FIRST, "FIRST") != 1)
        hparse_f_expect(FLAG_VERSION_MARIADB_10_10, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_NEXT, "NEXT");
      if (hparse_errno != 0) return 0;
      hparse_f_simple_value(false, true); /* accepting "integer {ROW|ROWS}" */
      if (hparse_f_accept(FLAG_VERSION_MARIADB_10_10, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ONLY, "ONLY") != 1)
      {
        /* WITH TIES is only valid if there was an earlier ORDER BY clause */
        /* This could be fooled by a subquery in an ORDER BY, or an ORDER BY within a subquery or join */
        bool is_order_by_in_select= false;
        for (int i= hparse_i_of_last_accepted;;)
        {
          i= next_i(i, -1);
          if (i == 0) break;
          if ((main_token_flags[i]&TOKEN_FLAG_IS_START_STATEMENT) != 0) break;
          if (main_token_types[i] == TOKEN_KEYWORD_ORDER) {is_order_by_in_select= true; break; }
        }
        if (is_order_by_in_select == false) hparse_f_error();
        if (hparse_errno != 0) return 0;
        hparse_f_expect(FLAG_VERSION_MARIADB_10_10, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WITH, "WITH");
        if (hparse_errno != 0) return 0;
        hparse_f_expect(FLAG_VERSION_MARIADB_10_10, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TIES, "TIES");
        if (hparse_errno != 0) return 0;
      }
    }
    if ((is_offset_seen == true) || (is_fetch_seen == true)) return 1;
  }
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LIMIT") == 1)
  {
    bool is_simple_value_seen= false;
    if (hparse_f_simple_value(false, false) == 1) /* expecting "integer" */
    {
      is_simple_value_seen= true;
      if ((who_is_calling == TOKEN_KEYWORD_DELETE) || (who_is_calling == TOKEN_KEYWORD_UPDATE) || (who_is_calling == TOKEN_KEYWORD_GROUP_CONCAT))
        return 1;
      if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1)
       || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OFFSET") == 1))
      {
        if (hparse_f_simple_value(true, false) == 0) return 0; /* expecting "integer" */
      }
    }
    bool is_rows_seen= false;
    if (who_is_calling == TOKEN_KEYWORD_SELECT)
    {
      if (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ROWS, "ROWS") == 1)
      {
        is_rows_seen= true;
        hparse_f_expect(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_EXAMINED, "EXAMINED");
        if (hparse_errno != 0) return 0;
        if (hparse_f_simple_value(true, false) == 0) return 0; /* expecting "integer" */
      }
    }
    if ((is_simple_value_seen == false) && (is_rows_seen == false)) hparse_f_error();
    return 1; /* because limit seen */
  }
  return 0;
}

/*
  Sometimes <simple value specification> might be an integer literal, or integer declared variable.
  E.g. LIMIT <simple value specification>, OFFSET <simple value specification> {ROW|ROWS}
  If error, say hparse_f_error() and return 0. If value seen, return 1.
  Todo: using hparse_f_is_variable or something higher we could do better than ask if it's an identifier.
  Todo: look for row|rows first before you look for an identifier
  Todo: for literals, only accepted values are unsigned integer -- 1E0 or 1.0 or +1 won't do.
  Todo: for variables, defined type must be integer or (dunno why) BIT. DECIMAL won't do.
        -- probably you could somehow get the restriction into setup_determine_what_variables_are_in_scope()
        -- also defined variables should not be among the choices if they're named ROW or ROWS
  Todo: this should be called from several other places, search for cases where we pass
        TOKEN_REFTYPE_VARIABLE_REFER to hparse_f_accept() and decide whether they require unsigned integers
  Todo: we're using this because hparse_f_qualified_name_of_operand() isn't working properly for variables,
        if or when that' fixed we can do otherwise
  Warn: In standard SQL ROW and ROWS are reserved, in MariaDB they're not -- but would cause errors if used.
        So act as if they are reserved.
*/
int MainWindow::hparse_f_simple_value(bool is_value_required, bool is_row_required)
{
  bool is_value_seen= false;
  if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 1) is_value_seen= true;
  if (is_value_seen == false)
  {
    if (hparse_f_is_in_compound() == true)
    {
      QString s= hparse_token.toUpper();
      if ((s == "ROW") || (s == "ROWS")) main_token_flags[hparse_i] |= TOKEN_FLAG_IS_RESERVED;
      hparse_variable_is_allowed= true;
      hparse_variable_must_be_int= true;
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_VARIABLE_REFER,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) is_value_seen= true;
      hparse_variable_must_be_int= false;
      hparse_variable_is_allowed= false;
    }
  }
  if (is_value_seen == false)
  {
    if (is_value_required)
    {
      hparse_f_error();
      return 0;
    }
  }
  if (is_row_required == true)
  {
    if (hparse_f_accept(FLAG_VERSION_MARIADB_10_10, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ROW, "ROW") != 1)
      hparse_f_expect(FLAG_VERSION_MARIADB_10_10, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ROWS, "ROWS");
    if (hparse_errno != 0) return 0;
  }
  return 1;
}

void MainWindow::hparse_f_like_or_where()
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LIKE") == 1)
  {
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WHERE") == 1)
  {
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_from_or_like_or_where()
{
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IN") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
  }
  hparse_f_like_or_where();
}

/* SELECT ... INTO OUTFILE and LOAD DATA INFILE have a similar clause. */
/* Now we'll call it for SET ocelot_import|ocelot_export too, so it's no longer strictly MySQL|MariaDB. */
void MainWindow::hparse_f_infile_or_outfile()
{
  if (hparse_f_character_set() == 1)
  {
    if (hparse_f_character_set_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  if (hparse_errno > 0) return;

  if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIELDS") == 1) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLUMNS") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TERMINATED") == 1)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    bool enclosed_seen= false;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OPTIONALLY") == 1)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENCLOSED");
      if (hparse_errno > 0) return;
      enclosed_seen= true;
    }
    else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENCLOSED") == 1)
    {
      enclosed_seen= true;
    }
    if (enclosed_seen == true)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ESCAPED") == 1)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
  }
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LINES") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STARTING") == 1)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TERMINATED") == 1)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
  }
}

void MainWindow::hparse_f_show_columns()
{
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM") == 0)
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IN");
    if (hparse_errno > 0) return;
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
  }
  if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  hparse_f_from_or_like_or_where();
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_indexes_or_keys() /* for SHOW {INDEX | INDEXES | KEYS} */
{
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IN") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IN") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WHERE") == 1)
  {
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
  }
}

/*
   For CREATE/ALTER: some clauses precede the object type, so e.g. we don't know yet
   whether it's a table, a view, an index, or whatever.
   We'll take such clauses in any order, but won't allow duplicates or impossibles.
   We'll return hparse_flags, which determines what can follow -- e.g. after CREATE UNIQUE
   we won't expect TABLE.
   schema=database, function+procedure+trigger+event=routine
*/
#define HPARSE_FLAG_DATABASE   1
#define HPARSE_FLAG_ROUTINE    2
#define HPARSE_FLAG_INDEX      8
#define HPARSE_FLAG_SERVER     32
#define HPARSE_FLAG_TABLE      64
#define HPARSE_FLAG_TABLESPACE 128
#define HPARSE_FLAG_TRIGGER    512
#define HPARSE_FLAG_USER       1024
#define HPARSE_FLAG_VIEW       2048
#define HPARSE_FLAG_INSTANCE   4096
#define HPARSE_FLAG_SEQUENCE   8192
#define HPARSE_FLAG_PACKAGE    16384
#define HPARSE_FLAG_ANY        65535

void MainWindow::hparse_f_alter_or_create_clause(int who_is_calling, unsigned int *hparse_flags,
                                                 bool *fulltext_seen, bool *vector_seen)
{
  bool algorithm_seen= false, definer_seen= false, sql_seen= false, temporary_seen= false;
  bool unique_seen= false, or_seen= false, ignore_seen= false, online_seen= false;
  bool aggregate_seen= false;
  *fulltext_seen= false;
  *vector_seen= false;
  (*hparse_flags)= HPARSE_FLAG_ANY;

  /* CREATE LUA FUNCTION|PROCEDURE, a special client sttement for displaying a Lua function, excludes all other flags. */
  if (who_is_calling == TOKEN_KEYWORD_CREATE)
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LUA, "LUA") == 1)
    {
      hparse_dbms_mask |= FLAG_VERSION_LUA_OUTPUT;
        (*hparse_flags)= HPARSE_FLAG_ROUTINE;
      return;
    }
  }

  /* in MySQL OR REPLACE is only for views, in MariaDB OR REPLACE is for all creates */
  int or_replace_flags;
  if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) or_replace_flags= HPARSE_FLAG_ANY;
  else or_replace_flags= HPARSE_FLAG_VIEW;

  if (who_is_calling == TOKEN_KEYWORD_CREATE)
  {
    ignore_seen= true;
    if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_ALL) != 0) online_seen= true;
  }
  else
  {
    temporary_seen= true; (*fulltext_seen)= true, unique_seen= true, or_seen= true;
    aggregate_seen= true;
  }
  for (;;)
  {
    if ((((*hparse_flags) & or_replace_flags) != 0) && (or_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OR") == 1))
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLACE");
      if (hparse_errno > 0) return;
      or_seen= true; (*hparse_flags) &= or_replace_flags;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_VIEW) != 0) && (algorithm_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALGORITHM") == 1))
    {
      hparse_f_algorithm();
      if (hparse_errno > 0) return;
      algorithm_seen= true; (*hparse_flags) &= HPARSE_FLAG_VIEW;
    }
    else if ((((*hparse_flags) & (HPARSE_FLAG_VIEW + HPARSE_FLAG_ROUTINE)) != 0) && (definer_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFINER") == 1))
    {
      hparse_f_definer();
      if (hparse_errno > 0) return;
      definer_seen= true; (*hparse_flags) &= (HPARSE_FLAG_VIEW + HPARSE_FLAG_ROUTINE + HPARSE_FLAG_PACKAGE);
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_VIEW) != 0) && (sql_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL") == 1))
    {
      hparse_f_sql();
      if (hparse_errno > 0) return;
      sql_seen= true;  (*hparse_flags) &= HPARSE_FLAG_VIEW;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_TABLE) != 0) && (ignore_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORE") == 1))
    {
      ignore_seen= true; (*hparse_flags) &= HPARSE_FLAG_TABLE;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_TABLE) != 0) && (temporary_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TEMPORARY") == 1))
    {
      temporary_seen= true; (*hparse_flags) &= (HPARSE_FLAG_TABLE | HPARSE_FLAG_SEQUENCE);
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_TABLE) != 0) && ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ONLINE") == 1))
    {
      online_seen= true; (*hparse_flags) &= (HPARSE_FLAG_INDEX | HPARSE_FLAG_TABLE);
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_TABLE) != 0) && ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OFFLINE") == 1))
    {
      online_seen= true; (*hparse_flags) &= HPARSE_FLAG_INDEX;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_INDEX) != 0) && (unique_seen == false) && ((*fulltext_seen) == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FULLTEXT") == 1))
    {
      (*fulltext_seen)= true; (*hparse_flags) &= HPARSE_FLAG_INDEX;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_INDEX) != 0) && ((*fulltext_seen) == false) && (unique_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SPATIAL") == 1))
    {
      (*fulltext_seen)= true; (*hparse_flags) &= HPARSE_FLAG_INDEX;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_INDEX) != 0) && (unique_seen == false) && ((*fulltext_seen) == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNIQUE") == 1))
    {
      unique_seen= true; (*hparse_flags) &= HPARSE_FLAG_INDEX;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_INDEX) != 0) && (unique_seen == false) && ((*fulltext_seen) == false) && (hparse_f_accept(FLAG_VERSION_MARIADB_11_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VECTOR") == 1))
    {
      unique_seen= true; (*vector_seen)= true; (*hparse_flags) &= HPARSE_FLAG_INDEX;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_ROUTINE) != 0) && (aggregate_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AGGREGATE") == 1))
    {
      aggregate_seen= true; (*hparse_flags) &= HPARSE_FLAG_ROUTINE;
    }
    else break;
  }
}

/*
  ; or (; + delimiter) or delimiter or \G or \g
  Re calling_statement_type:
    It might be it might be TOKEN_KEYWORD_PROCEDURE | TOKEN_KEYWORD_EVENT | TOKEN_KEYWORD_TRIGGER |  TOKEN_KEYWORD_FUNCTION | 0
    so something to do with blocks, I suppose. But we no longer use it.
  Re semicolon:
    We want to check for ; before checking for \G so that ; comes first in the Expecteds list.
    It's silly but possible to have delimiter // and end with ;// in which case we'll accept // but not add it to the Expecteds list.
*/
int MainWindow::hparse_f_semicolon_and_or_delimiter(int calling_statement_type)
{
  (void) calling_statement_type;  /* to avoid "unused parameter" warning */
  if (hparse_f_accept(FLAG_VERSION_ALL_OR_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";") == 1)
  {
    hparse_f_accept(FLAG_VERSION_ALL_OR_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_DELIMITER, hparse_delimiter_str);
    return 1;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_DELIMITER, "\\G") == 1)
  {
    return 1;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL_OR_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_DELIMITER, hparse_delimiter_str) == 1) return 1;
  return 0;
}

/*
  For EXPLAIN and perhaps for ANALYZE, we want to accept only a
  statement that would be legal therein. So check if that's what follows,
  if it is then call hparse_f_statement, if it's not then call
  hparse_f_accept which is guaranteed to fail.
  Return 1 if it was a statement, else return 0 (which might also mean error).
  Note: Actually MariaDB accepts INSERT and REPLACE, but doesn't document, so we don't accept.
*/
int MainWindow::hparse_f_explainable_statement(int block_top)
{
  QString hparse_token_upper= hparse_token.toUpper();
  if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_ALL) != 0)
  {
    if ((hparse_token_upper == "DELETE")
     || (hparse_token_upper == "INSERT")
     || (hparse_token_upper == "REPLACE")
     || (hparse_token_upper == "SELECT")
     || (hparse_token_upper == "TABLE") /* todo: actually I guess this is only okay in MySQL 8.0+ */
     || (hparse_token_upper == "UPDATE"))
    {
      hparse_f_pseudo_statement(block_top);
      if (hparse_errno > 0) return 0;
      return 1;
    }
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DELETE");
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INSERT");
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLACE");
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SELECT");
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE");
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UPDATE");
    return 0;
  }
  else if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0)
  {
    if ((hparse_token_upper == "DELETE")
     || (hparse_token_upper == "SELECT")
     || (hparse_token_upper == "UPDATE"))
    {
      hparse_f_pseudo_statement(block_top);
      if (hparse_errno > 0) return 0;
      return 1;
    }
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DELETE");
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SELECT");
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UPDATE");
    return 0;
  }
  else if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
  {
    hparse_f_pseudo_statement(block_top);
    if (hparse_errno > 0) return 0;
    return 1;
  }
  return 0;
}

/* TODO: I THINK I'M FORGETTING TO SAY return FOR A LOT OF MAIN STATEMENTS! */
/*
statement =
    "connect" "create" "drop" etc. etc.
    The idea is to parse everything that's described in the MySQL 5.7 manual.
    Additionally, depending on flags, parse MariaDB or Tarantool statements.
*/
void MainWindow::hparse_f_statement(int block_top)
{
  log("hparse_f_statement start", 80);
  if (hparse_errno > 0) return;
  hparse_statement_type= 0;
  hparse_subquery_is_allowed= 0;

  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALTER"))
  {
    hparse_statement_type= TOKEN_KEYWORD_ALTER;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    unsigned int hparse_flags; bool fulltext_seen; bool vector_seen;
    hparse_f_alter_or_create_clause(TOKEN_KEYWORD_ALTER, &hparse_flags, &fulltext_seen, &vector_seen);
    if ((((hparse_flags) & HPARSE_FLAG_DATABASE) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATABASE") == 1))
    {
      hparse_f_alter_database();
    }
    else if ((((hparse_flags) & HPARSE_FLAG_ROUTINE) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EVENT") == 1))
    {
      hparse_f_alter_or_create_event(TOKEN_KEYWORD_ALTER);
      if (hparse_errno > 0) return;
    }
    else if ((((hparse_flags) & HPARSE_FLAG_ROUTINE) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FUNCTION") == 1))
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION, TOKEN_REFTYPE_FUNCTION) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_characteristics(TOKEN_KEYWORD_FUNCTION);
    }
    else if ((((hparse_flags) & HPARSE_FLAG_INSTANCE) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INSTANCE") == 1))
    {
      /* Todo: This statement appears to have disappeared. */
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROTATE");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INNODB");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY");
      if (hparse_errno > 0) return;
    }
    /* TODO: ALTER LOGFILE GROUP is not supported */
    else if ((((hparse_flags) & HPARSE_FLAG_ROUTINE) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROCEDURE") == 1))
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_PROCEDURE, TOKEN_REFTYPE_PROCEDURE) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_characteristics(TOKEN_KEYWORD_PROCEDURE);
    }
    else if ((((hparse_flags) & HPARSE_FLAG_DATABASE) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SCHEMA") == 1))
    {
      hparse_f_alter_database();
    }
    else if ((((hparse_flags) & HPARSE_FLAG_SEQUENCE) != 0) && (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SEQUENCE") == 1))
    {
      hparse_f_alter_or_create_sequence(TOKEN_KEYWORD_ALTER);
      if (hparse_errno > 0) return;
    }
    else if ((((hparse_flags) & HPARSE_FLAG_SERVER) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SERVER, "SERVER") == 1))
    {
      hparse_f_alter_or_create_server(TOKEN_KEYWORD_ALTER);
      if (hparse_errno > 0) return;
    }
    else if ((((hparse_flags) & HPARSE_FLAG_TABLE) != 0) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE") == 1))
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      do
      {
        hparse_f_alter_specification();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      hparse_f_partition_options();
    }
    else if ((((hparse_flags) & HPARSE_FLAG_TABLESPACE) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_5_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLESPACE") == 1))
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_5_7, TOKEN_REFTYPE_TABLESPACE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_5_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RENAME");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_5_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TO");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_5_7, TOKEN_REFTYPE_TABLESPACE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_5_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENGINE") == 1))
      {
        hparse_f_engine();
        if (hparse_errno > 0) return;
      }
      if (hparse_errno > 0) return;
    }
    else if ((((hparse_flags) & HPARSE_FLAG_USER) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USER") == 1))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      hparse_f_user_specification_list(TOKEN_KEYWORD_ALTER);
      if (hparse_errno > 0) return;
      hparse_f_require(TOKEN_KEYWORD_ALTER, false, false);
      if (hparse_errno > 0) return;
    }
    else if ((((hparse_flags) & HPARSE_FLAG_VIEW) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VIEW") == 1))
    {
      hparse_f_alter_or_create_view();
      if (hparse_errno > 0) return;
    }
    else hparse_f_error();
  }
  /* "ANALYZE" is checked in hparse_f_query subroutines */
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BEGIN_WORK, "BEGIN") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_BEGIN_WORK; /* don't confuse with BEGIN for compound */
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WORK") == 1) {;}
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BINLOG") == 1)
  {
    //hparse_statement_type= TOKEN_KEYWORD_BINLOG;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CACHE") == 1)
  {
    //hparse_statement_type= TOKEN_KEYWORD_CACHE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX");
    if (hparse_errno > 0) return;
    do
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      bool parenthesis_is_seen= false;
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY") == 1))
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
        if (hparse_errno > 0) return;
        parenthesis_is_seen= true;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        parenthesis_is_seen= true;
      }
      if (parenthesis_is_seen == true)
      {
        do
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_INDEX, TOKEN_TYPE_IDENTIFIER, "[identifier]");
          if (hparse_errno > 0) return;
        } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    /* todo: partition clause should not be legal in MariaDB */
    /* todo: I think ALL is within parentheses? */
    if (hparse_f_partition_list(true, true) == 0)
    {
      if (hparse_errno > 0) return;
    }
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IN");
    if (hparse_errno > 0) return;
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_KEY_CACHE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CALL") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_CALL;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_f_call();
    if (hparse_errno > 0) return;
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHANGE") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_CHANGE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER") == 1)
    {
      if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING);
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TO");
      if (hparse_errno > 0) return;
      do
      {
        if ((((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DO_DOMAIN_IDS") == 1))
         || (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORE_DOMAIN_IDS") == 1))
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORE_SERVER_IDS") == 1))
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
          if (hparse_errno > 0) return;
          do
          {
            hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ANY); /* this allows "()" */
            if (hparse_errno > 0) return;
          } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_AUTO_POSITION")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_BIND")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_CONNECT_RETRY")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();}
        else if (((hparse_dbms_mask & FLAG_VERSION_MYSQL_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_DELAY"))) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_2_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_DELAY")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_HEARTBEAT_PERIOD")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_HOST")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_HOST,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_LOG_FILE")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_FILE,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_LOG_POS")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_PASSWORD")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_PASSWORD,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_PORT")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();}
        else if (((hparse_dbms_mask & FLAG_VERSION_MYSQL_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_RETRY_COUNT"))) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_SSL"))
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "0") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "1");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_SSL_CA")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_SSL_CAPATH")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DIRECTORY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_SSL_CERT")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_SSL_CIPHER")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_SSL_CRL")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_SSL_CRLPATH")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DIRECTORY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_SSL_KEY")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_SSL_VERIFY_SERVER_CERT"))
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "0") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "1");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_USER")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();}
        else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_USE_GTID"))) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();}
        else if (((hparse_dbms_mask & FLAG_VERSION_MYSQL_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_TLS_VERSION"))) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RELAY_LOG_FILE")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RELAY_LOG_POS")) {hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "="); if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();}
        else hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      hparse_f_for_channel();
      if (hparse_errno > 0) return;
      return;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLICATION") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FILTER");
      if (hparse_errno > 0) return;
      do
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLICATE_DO_DB") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLICATE_IGNORE_DB") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLICATE_DO_TABLE") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLICATE_IGNORE_TABLE") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLICATE_WILD_DO_TABLE") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLICATE_WILD_IGNORE_TABLE") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLICATE_REWRITE_DB") == 1) {;}
        else hparse_f_error();
        if (hparse_errno > 0) return;
        /* TODO: Following is garbage. We need lists of databases or tables */
        hparse_f_column_list(1, 0); /* Todo: take into account what kind of list it should be */
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
    else hparse_f_error();
  }
  /* "CHECK" is checked in hparse_f_query subroutines */
  /* "CHECKSUM" is checked in hparse_f_query subroutines */
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COMMIT") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_COMMIT;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WORK") == 1) {;}
    hparse_f_commit_or_rollback();
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONNECT"))
  {
    hparse_statement_type= TOKEN_KEYWORD_CONNECT;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CREATE, "CREATE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_CREATE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    unsigned int hparse_flags;
    bool fulltext_seen; bool vector_seen;
    hparse_f_alter_or_create_clause(TOKEN_KEYWORD_CREATE, &hparse_flags, &fulltext_seen, &vector_seen);
    if (hparse_errno > 0) return;
#if (FLAG_VERSION_MARIADB_12_0 != 0)
    if (((hparse_flags & HPARSE_FLAG_DATABASE) != 0) && (hparse_f_accept(FLAG_VERSION_MARIADB_12_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CATALOG") == 1))
    {
      hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_INDEX, TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;                                    /* catalog_name */
    }
    else
#endif
    if (((hparse_flags & HPARSE_FLAG_DATABASE) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATABASE") == 1))
    {
      hparse_f_create_database();
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_ROUTINE) != 0) && ((hparse_dbms_mask & FLAG_VERSION_LUA_OUTPUT) == 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_EVENT, "EVENT") == 1))
    {
      hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      hparse_f_alter_or_create_event(TOKEN_KEYWORD_CREATE);
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_ROUTINE) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FUNCTION, "FUNCTION") == 1))
    {
      if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name_of_object(TOKEN_FLAG_IS_NEW, TOKEN_REFTYPE_DATABASE_OR_FUNCTION, TOKEN_REFTYPE_FUNCTION) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      /* If (parameter_list) isn't there, it might be a UDF */
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RETURNS") == 1)
      {
        /* Manual doesn't mention INT or DEC. I wonder what else it doesn't mention. */
        if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STRING") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INTEGER") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INT") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REAL") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DECIMAL") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEC") == 1)) {;}
        else hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SONAME");
        if (hparse_errno > 0) return;
        if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();

        if (hparse_errno > 0) return;
      }
      else
      {
        hparse_f_create_function_clauses();
        if (hparse_errno > 0) return;
        hparse_f_block(TOKEN_KEYWORD_FUNCTION, hparse_i);
        if (hparse_errno > 0) return;
      }
      hparse_dbms_mask &= (~FLAG_VERSION_LUA_OUTPUT);
    }
    else if (((hparse_flags & HPARSE_FLAG_INDEX) != 0) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX") == 1))
    {
      if ((hparse_dbms_mask & (FLAG_VERSION_MARIADB_ALL | FLAG_VERSION_TARANTOOL)) != 0) hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      main_token_flags[hparse_i]|= TOKEN_FLAG_IS_NEW;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_INDEX, TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;                                    /* index_name */
      hparse_f_index_columns(TOKEN_KEYWORD_INDEX, fulltext_seen, false, false, vector_seen);
      if (hparse_errno > 0) return;
      hparse_f_algorithm_or_lock();
    }
    else if (((hparse_flags & HPARSE_FLAG_PACKAGE) != 0) && (hparse_f_accept(FLAG_VERSION_PLSQL | FLAG_VERSION_MARIADB_11_4, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PACKAGE") == 1))
    {
      bool is_body= false;
      if (hparse_f_accept(FLAG_VERSION_PLSQL | FLAG_VERSION_MARIADB_11_4, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BODY, "BODY") == 1)
        is_body= true;
      hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name_of_object(TOKEN_FLAG_IS_NEW, TOKEN_REFTYPE_DATABASE_OR_PACKAGE, TOKEN_REFTYPE_PACKAGE) == 0) hparse_f_error();
      hparse_f_characteristics(TOKEN_KEYWORD_PACKAGE);
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IS, "IS") == 0)
        hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_AS, "AS");
      if (hparse_errno > 0) return;
      hparse_f_create_package(is_body);
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_ROUTINE) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PROCEDURE, "PROCEDURE") == 1))
    {
      if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      hparse_f_create_procedure_clauses();
      if (hparse_errno > 0) return;
      hparse_f_block(TOKEN_KEYWORD_PROCEDURE, hparse_i);
      if (hparse_errno > 0) return;
      hparse_dbms_mask &= (~FLAG_VERSION_LUA_OUTPUT);
    }
    else if (((hparse_dbms_mask & (FLAG_VERSION_MARIADB_ALL | FLAG_VERSION_MYSQL_8_0)) != 0) && ((hparse_flags & HPARSE_FLAG_USER) != 0) && (hparse_f_accept(FLAG_VERSION_MARIADB_ALL | FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ROLE, "ROLE") == 1))
    {
      if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0) hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      if (QString::compare(hparse_token, "NONE", Qt::CaseInsensitive) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      do
      {
        if (hparse_f_user_or_role_name(TOKEN_REFTYPE_ROLE) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        if (hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) break;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      if (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITH") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ADMIN");
        if (hparse_errno > 0) return;
        if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
    }
    else if (((hparse_flags & HPARSE_FLAG_DATABASE) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SCHEMA, "SCHEMA") == 1))
    {
      hparse_f_create_database();
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_SEQUENCE) != 0) && (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SERVER, "SEQUENCE") == 1))
    {
      hparse_f_alter_or_create_sequence(TOKEN_KEYWORD_CREATE);
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_SERVER) != 0) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SERVER, "SERVER") == 1))
    {
      hparse_f_alter_or_create_server(TOKEN_KEYWORD_CREATE);
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_TABLE) != 0) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TABLE, "TABLE") == 1))
    {
      hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name_of_object(TOKEN_FLAG_IS_NEW, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      bool element_is_seen= false;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LIKE") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        return;
      }
      else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SERVER, "SERVER") == 1)
      {
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_SERVER,TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LUA, "LUA") == 1)
        {
          if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        }
        else
        {
          /* Todo: Better to call hparse_f_statement, someday */
          hparse_f_lua_blocklist(0, hparse_i);
          if (hparse_errno > 0) return;
        }
        return;
      }
      else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LIKE") == 1)
        {
          if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
          if (hparse_errno > 0) return;
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
          if (hparse_errno > 0) return;
          return;
        }
        main_token_flags[hparse_i] |= TOKEN_FLAG_IS_START_IN_COLUMN_LIST;
        bool comma_is_seen;
        bool item_is_seen;
        int last_word= 0;
        do
        {
          comma_is_seen= false;
          item_is_seen= false;
          if (last_word == TOKEN_KEYWORD_END)
          {
            if (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PERIOD, "PERIOD"))
            {
              hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FOR, "FOR");
              if (hparse_errno != 0) return;
              hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SYSTEM_TIME, "SYSTEM_TIME");
              if (hparse_errno != 0) return;
              hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
              if (hparse_errno != 0) return;
              if (hparse_f_qualified_name_of_operand(TOKEN_FLAG_IS_NEW, false,false,true) != 1) hparse_f_error();
              if (hparse_errno != 0) return;
              hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",");
              if (hparse_errno != 0) return;
              if (hparse_f_qualified_name_of_operand(TOKEN_FLAG_IS_NEW, false,false,true) != 1) hparse_f_error();
              if (hparse_errno != 0) return;
              hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
              if (hparse_errno != 0) return;
              item_is_seen= true;
            }
          }
          else if (hparse_f_qualified_name_of_operand(TOKEN_FLAG_IS_NEW, false,false,true) == 1)
          {
            hparse_f_column_definition(last_word);
            if (hparse_errno > 0) return;
            item_is_seen= true;
          }
          else
          {
            if (hparse_errno > 0) return;
            if (hparse_f_create_definition(TOKEN_KEYWORD_CREATE) != 3) item_is_seen= true;
            if (hparse_errno > 0) return;
          }
          if (item_is_seen == false)
          {
            hparse_f_error();
            return;
          }
          /* Did the last item end with ROW START or ROW END? */
          last_word= main_token_types[hparse_i_of_last_accepted];
          if ((last_word == TOKEN_KEYWORD_START) || (last_word == TOKEN_KEYWORD_END))
          {
            int hparse_i_of_before_last_word= next_i(hparse_i_of_last_accepted, -1);
            int before_last_word= main_token_types[hparse_i_of_before_last_word];
            if (before_last_word != TOKEN_KEYWORD_ROW) last_word= 0;
          }
          else last_word= 0;
          if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","))
          {
            main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_END_IN_COLUMN_LIST;
            comma_is_seen= true;
          }
        } while (comma_is_seen);
        element_is_seen= true;
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
      if ((element_is_seen == true)
       && ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0))
      {
        /* we used to check for WITHOUT ROWID here. Now the only table option is WITH ENGINE='' */
        if (hparse_f_accept(FLAG_VERSION_TARANTOOL_2_3, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_WITH, "WITH") == 1)
        {
          hparse_f_expect(FLAG_VERSION_TARANTOOL_2_3, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_ENGINE, "ENGINE");
          if (hparse_errno > 0) return;
          hparse_f_expect(FLAG_VERSION_TARANTOOL_2_3, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_TARANTOOL_2_3, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        }
      }
      else
      {
        hparse_f_table_or_partition_options(TOKEN_KEYWORD_TABLE);
        if (hparse_errno > 0) return;
        hparse_f_partition_options();
        if (hparse_errno > 0) return;
        bool ignore_or_as_seen= false;
        if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORE") || hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLACE") == 1))
        {
          main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
          ignore_or_as_seen= true;
        }
        if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
        {
          /* Tarantool no longer acceps AS SELECT here */
          //hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS");
          //if (hparse_errno > 0) return;
          //ignore_or_as_seen= true;
          hparse_f_error();
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS") == 1) ignore_or_as_seen= true;
        bool select_is_seen= false;
        if (ignore_or_as_seen == true)
        {
          if (hparse_f_query(0, false, false, false) == 0)
          {
            hparse_f_error();
            return;
          }
          select_is_seen= true;
        }
        else if (hparse_f_query(0, false, false, false) != 0) select_is_seen= true;
        if (hparse_errno > 0) return;
        if ((element_is_seen == false) && (select_is_seen == false))
        {
          hparse_f_error();
          return;
        }
      }
    }
    else if (((hparse_flags & HPARSE_FLAG_TABLESPACE) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TABLESPACE, "TABLESPACE") == 1))
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_TABLESPACE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ADD");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATAFILE");
      if (hparse_errno > 0) return;
      if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FILE_BLOCK_SIZE") == 1)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=") == 1) {;}
        if (hparse_f_literal(TOKEN_REFTYPE_LENGTH, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENGINE") == 1)
      {
        hparse_f_engine();
        if (hparse_errno > 0) return;
      }
    }
    else if (((hparse_flags & HPARSE_FLAG_ROUTINE) != 0)  && ((hparse_dbms_mask & FLAG_VERSION_LUA_OUTPUT) == 0) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TRIGGER, "TRIGGER") == 1))
    {
      if ((hparse_dbms_mask & (FLAG_VERSION_MARIADB_ALL | FLAG_VERSION_TARANTOOL)) != 0) hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TRIGGER, TOKEN_REFTYPE_TRIGGER) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BEFORE") == 1)
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
      else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INSTEAD") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OF");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AFTER") == 1)
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
      else
      {
        hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INSERT") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      }
      else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UPDATE") == 1)
      {
        if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OF") == 1)
        {
          do
          {
            hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
            if (hparse_errno > 0) return;
          } while (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
        }
      }
      else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DELETE") == 1) {;}
      else hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON");
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR") == 1)
      {
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EACH");
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROW");
        if (hparse_errno > 0) return;
      }
      else
      {
        if ((hparse_dbms_mask & FLAG_VERSION_ALL) != 0)
        {
          hparse_f_error();
          if (hparse_errno > 0) return;
        }
      }
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_MARIADB_10_2_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOLLOWS") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PRECEDES")) == 1)
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TRIGGER, TOKEN_REFTYPE_TRIGGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
      {
        if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WHEN") == 1)
        {
          main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
          /* Todo: subqueries are allowed here, but maybe this is the wrong place to say so. */
          hparse_subquery_is_allowed= true;
          hparse_f_opr_1(0, 0);
          if (hparse_errno > 0) return;
        }
        hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BEGIN, "BEGIN");
        if (hparse_errno > 0) return;
        ++hparse_flow_count;
        bool statement_is_seen= false;
        for (;;)
        {
          if (statement_is_seen)
          {
            if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "END") == 1) break;
          }
          /* check first word of statement is okay, else return error */
          if (hparse_f_is_special_verb(TOKEN_KEYWORD_TRIGGER) == false) return;
          hparse_f_statement(block_top);
          if (hparse_errno > 0) return;
          /* This kludge occurs more than once. */
          if ((hparse_prev_token != ";") && (hparse_prev_token != hparse_delimiter_str))
          {
            if (hparse_f_semicolon_and_or_delimiter(TOKEN_KEYWORD_TRIGGER) == 0) hparse_f_error();
          }
          if (hparse_errno > 0) return;
          statement_is_seen= true;
        }
        --hparse_flow_count;
      }
      else
      {
        hparse_create_trigger_seen= true;
        hparse_f_block(TOKEN_KEYWORD_TRIGGER, hparse_i);
        hparse_create_trigger_seen= false;
        if (hparse_errno > 0) return;
      }
    }
    else if (((hparse_flags & HPARSE_FLAG_USER) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_USER, "USER") == 1))
    {
      hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      hparse_f_user_specification_list(TOKEN_KEYWORD_CREATE);
      if (hparse_errno > 0) return;
      if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) != 0)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_KEYWORD_DEFAULT, "DEFAULT"))
        {
          main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
          hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ROLE, "ROLE");
          if (hparse_errno > 0) return;
          /* This occurs more than once, should we have a function hparse_f_user_or_role_name_list? */
          do
          {
            if (hparse_f_user_or_role_name(TOKEN_REFTYPE_ROLE) == 0) hparse_f_error();
            if (hparse_errno > 0) return;
          } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
        }
      }
      hparse_f_require(TOKEN_KEYWORD_CREATE, false, false);
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_VIEW) != 0) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_VIEW, "VIEW") == 1))
    {
      if ((hparse_dbms_mask & (FLAG_VERSION_MARIADB_ALL | FLAG_VERSION_TARANTOOL)) != 0) hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      hparse_f_alter_or_create_view();
      if (hparse_errno > 0) return;
    }
#ifdef ALLOW_CREATE_VIRTUAL
    else if ((((hparse_flags) & HPARSE_FLAG_TABLE) != 0) && (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VIRTUAL") == 1))
    {
      hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE");
      if (hparse_errno > 0) return;
      hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE);
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USING");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_PLUGIN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        for (;;)
        {
          if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_PARAMETER_DEFINE,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) {;}
          if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1) continue;
          else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")") == 1) break;
          else hparse_f_error();
          if (hparse_errno > 0) return;
        }
      }
    }
#endif
    else hparse_f_error();
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DEALLOCATE, "DEALLOCATE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_DEALLOCATE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PREPARE");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_STATEMENT,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DELETE, "DELETE"))
  {
    /* todo: look up how partitions are supposed to be handled */
    hparse_statement_type= TOKEN_KEYWORD_DELETE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_subquery_is_allowed= true;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOW_PRIORITY")) {;}
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "QUICK")) {;}
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORE")) {;}
    bool is_from_seen= false;
    if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM");
      if (hparse_errno > 0) return;
      is_from_seen= true;
    }
    if ((is_from_seen == true)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM") == 1))
    {
      bool multi_seen= false;
      if (hparse_f_qualified_name_with_star() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
      {
        hparse_f_table_index_hint_list();
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","))
      {
        multi_seen= true;
        do
        {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      }
      if (multi_seen == true) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USING");
      if (hparse_errno > 0) return;
      if ((multi_seen == true) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USING") == 1))
      {
        /* DELETE ... tbl_name[.*] [, tbl_name[.*]] ... FROM table_references [WHERE ...] */
        if (hparse_f_table_references() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_where();
        if (hparse_errno > 0) return;
        return;
      }
      /* DELETE ... FROM tbl_name [WHERE] [ORDER BY] LIMIT] */
      hparse_f_where();
      if (hparse_errno > 0) return;
      if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0)
      {
        hparse_f_order_by(0);
        if (hparse_errno > 0) return;
        hparse_f_limit(TOKEN_KEYWORD_DELETE);
        if (hparse_errno > 0) return;
      }
      if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RETURNING") == 1)
        {
          hparse_f_expression_list(TOKEN_KEYWORD_SELECT);
          if (hparse_errno > 0) return;
        }
      }
      return;
    }
    if (hparse_errno > 0) return;
    /* DELETE tbl_name[.*] [, tbl_name[.*]] ... FROM table_references [WHERE ...] */
    do
    {
      if (hparse_f_qualified_name_with_star() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM");
    if (hparse_errno > 0) return;
    if (hparse_f_table_references() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    hparse_f_where();
    if (hparse_errno > 0) return;
    return;
  }
  /* "DESC" is checked in hparse_f_query subroutines */
  /* "DESCRIBE" is checked in hparse_f_query subroutines */
  else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DETACH, "DETACH") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_DETACH;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATABASE") == 1) {;}
    hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_DATABASE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DO, "DO"))
  {
    hparse_statement_type= TOKEN_KEYWORD_DO;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_subquery_is_allowed= true;
    do
    {
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DROP, "DROP"))         /* drop database/event/etc. */
  {
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_DROP;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    bool temporary_seen= false, online_seen= false;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TEMPORARY") == 1) temporary_seen= true;
    if ((temporary_seen == false) && ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ONLINE") == 1) online_seen= true;
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OFFLINE") == 1) online_seen= true;
    }
#if (FLAG_VERSION_MARIADB_12_0 != 0)
    if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_MARIADB_12_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CATALOG")))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    else
#endif
    if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATABASE")))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EVENT")))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_EVENT, TOKEN_REFTYPE_EVENT) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FUNCTION")))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION, TOKEN_REFTYPE_FUNCTION) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX")))
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_INDEX,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON");
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0)
      {
        hparse_f_algorithm_or_lock();
      }
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_PLSQL | FLAG_VERSION_MARIADB_11_4, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PACKAGE")))
    {
      hparse_f_accept(FLAG_VERSION_PLSQL | FLAG_VERSION_MARIADB_11_4, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BODY, "BODY");
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_PACKAGE, TOKEN_REFTYPE_PACKAGE) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PREPARE")))
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_STATEMENT,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROCEDURE")))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_PROCEDURE, TOKEN_REFTYPE_PROCEDURE) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && ((hparse_dbms_mask & (FLAG_VERSION_MARIADB_ALL | FLAG_VERSION_MYSQL_8_0)) != 0) && (hparse_f_accept(FLAG_VERSION_MARIADB_ALL | FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROLE")== 1))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      do
      {
        if (hparse_f_user_or_role_name(TOKEN_REFTYPE_ROLE) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SCHEMA")))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SERVER")))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_SERVER, TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    else if ((online_seen == false) && (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SEQUENCE")))
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      do
      {
        if (hparse_errno > 0) return;
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_SEQUENCE, TOKEN_REFTYPE_SEQUENCE) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      if (hparse_errno > 0) return;
    }
    else if ((online_seen == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE")))
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      do
      {
        if (hparse_errno > 0) return;
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      if (hparse_errno > 0) return;
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RESTRICT");
      if (hparse_errno > 0) return;
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CASCADE");
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLESPACE")))
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_TABLESPACE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENGINE"))
      {
        hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      }
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TRIGGER")))
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TRIGGER, TOKEN_REFTYPE_TRIGGER) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USER")))
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      do
      {
        if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
    else if (((temporary_seen == false || (hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)) && (online_seen == false) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VIEW")))
    {
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF_IN_IF_EXISTS, "IF") == 1)
      {
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      do
      {
        if (hparse_errno > 0) return;
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_VIEW, TOKEN_REFTYPE_VIEW) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      if (hparse_errno > 0) return;
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RESTRICT");
      if (hparse_errno > 0) return;
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CASCADE");
      if (hparse_errno > 0) return;
    }
    else hparse_f_error();
  }
  else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "END"))
  {
    hparse_statement_type= TOKEN_KEYWORD_COMMIT;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_f_commit_or_rollback();
    return;
  }
  /* Todo: EXECUTE IMMEDIATE expression is okay in MariaDB */
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_EXECUTE, "EXECUTE") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_EXECUTE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_accept(FLAG_VERSION_MARIADB_10_2_3, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_IMMEDIATE, "IMMEDIATE") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MARIADB_10_2_3, TOKEN_LITERAL_FLAG_STRING) == 0)
      hparse_f_expect(FLAG_VERSION_MARIADB_10_2_3, TOKEN_REFTYPE_USER_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    }
    else hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_STATEMENT,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USING"))
    {
      do
      {
       hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_USER_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
       if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
  }
  /* "EXPLAIN" is checked in hparse_f_query subroutines */
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FLUSH, "FLUSH") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_FLUSH;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NO_WRITE_TO_BINLOG") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCAL") == 1)) {;}
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLES") == 1)
      || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0 | FLAG_VERSION_MARIADB_11_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE") == 1))
    {
      int table_name_count= 0;
      for (;;)
      {
        if (hparse_prev_token != ",")
        {
          if ((hparse_token == ";") || (hparse_next_token == hparse_delimiter_str)) break;
        }
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0)
        {
          if (hparse_errno > 0) return;
          if (table_name_count > 0) { hparse_f_error(); if (hparse_errno > 0) return; }
          break;
        }
        ++table_name_count;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 0) break;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITH") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "READ");
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCK");
        if (hparse_errno > 0) return;
      }
      else if ((table_name_count > 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR") == 1))
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXPORT");
        if (hparse_errno > 0) return;
      }
      /* else probably "flush table;" */
    }
    else do
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BINARY") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOGS");
        if (hparse_errno > 0) return;
        if (hparse_f_accept(FLAG_VERSION_MARIADB_10_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DELETE_DOMAIN_ID") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
          if (hparse_errno > 0) return;
          do
          {
            if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
            if (hparse_errno > 0) return;
          } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
          if (hparse_errno > 0) return;
        }
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DES_KEY_FILE") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ENGINE") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOGS");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ERROR") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOGS");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GENERAL") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOGS");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HOSTS") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOGS") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OPTIMIZER_COSTS") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PRIVILEGES") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "QUERY") == 1)
      { /* Todo: actually it seems MySQL no longer accepts query cache */
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CACHE");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RELAY") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOGS");
        if (hparse_errno > 0) return;
        hparse_f_for_channel(); /* actually in MariaDB this only exists for version 10.7+ */
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SLOW") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOGS");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USER_RESOURCES") == 1) {;}
      /* 2024-07-26: Added a bunch of MariaDB optins. Todo: merge in alphabetical order. */
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CHANGED_PAGE_BITMAPS") == 1) {;} /* assume plugin exists */
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CLIENT_STATISTICS") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_11_5, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GLOBAL") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MARIADB_11_5, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX_STATISTICS") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER") == 1) {;} /* deprecated */
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "QUERY_RESPONSE_TIME") == 1) {;} /* assume plugin exists */
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLICA") == 1) {;} /* undocumented */
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_11_5, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SESSION") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MARIADB_11_5, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept_slave_or_replica(FLAG_VERSION_MARIADB_10_0) == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_7, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SSL") == 1) {;} /* actually it wsa 10.4 */
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE_STATISTICS") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USER_STATISTICS") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USER_VARIABLES") == 1) {;}
      else hparse_f_error();
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_GET, "GET"))
  {
    hparse_statement_type= TOKEN_KEYWORD_GET;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURRENT");
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DIAGNOSTICS");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONDITION") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      do
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
        if (hparse_errno > 0) return;
        hparse_f_condition_information_item_name();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
    else do
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NUMBER") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROW_COUNT");
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_GRANT, "GRANT"))
  {
    hparse_statement_type= TOKEN_KEYWORD_GRANT;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    bool proxy_seen= false;
    bool role_name_seen= false;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROXY") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON");
      if (hparse_errno > 0) return;
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
      if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      proxy_seen= true;
    }
    else
    {
      hparse_f_grant_or_revoke(TOKEN_KEYWORD_GRANT, &role_name_seen);
      if (hparse_errno > 0) return;
    }
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TO");
    if (hparse_errno > 0) return;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    hparse_f_user_specification_list(TOKEN_KEYWORD_GRANT);
    if (hparse_errno > 0) return;
    hparse_f_require(TOKEN_KEYWORD_GRANT, proxy_seen, role_name_seen);
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_HANDLER, "HANDLER"))
  {
    hparse_statement_type= TOKEN_KEYWORD_HANDLER;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OPEN") == 1)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_HANDLER_ALIAS,TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "READ") == 1)
    {
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIRST") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NEXT") == 1)) {;}
      else if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_INDEX) == 1)
      {
        if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "<=") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ">=") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ">") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "<") == 1))
        {
          hparse_f_expression_list(TOKEN_KEYWORD_HANDLER);
          if (hparse_errno > 0) return;
        }
        else if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FIRST") == 1)
              || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NEXT") == 1)
              || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PREV") == 1)
              || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LAST") == 1))
           {;}
        else hparse_f_error();
        if (hparse_errno > 0) return;
      }
      hparse_f_where();
      hparse_f_limit(TOKEN_KEYWORD_HANDLER);
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CLOSE") == 1) {;}
    else hparse_f_error();
    if (hparse_errno > 0) return;
  }
  /* "HELP" is checked in hparse_f_query subroutines */
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INSERT, "INSERT"))
  {
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_INSERT;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    hparse_subquery_is_allowed= true;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOW_PRIORITY") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DELAYED") == 1) {;}
    else hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HIGH_PRIORITY");
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORE");
    hparse_f_insert_or_replace();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DUPLICATE, "DUPLICATE");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UPDATE");
      if (hparse_errno > 0) return;
      hparse_f_assignment(TOKEN_KEYWORD_INSERT, TOKEN_KEYWORD_DUPLICATE, false, false);
      if (hparse_errno > 0) return;
    }
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INSTALL, "INSTALL") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_INSTALL;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_f_install();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_KILL, "KILL"))
  {
    hparse_statement_type= TOKEN_KEYWORD_KILL;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HARD") == 0) hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SOFT");
    }
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONNECTION") == 0)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "QUERY") == 1)
      {
        if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ID");
      }
    }
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LOAD, "LOAD"))
  {
    hparse_statement_type= TOKEN_KEYWORD_LOAD;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DATA") == 1)
    {
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOW_PRIORITY") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONCURRENT") == 1))
        {;}
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCAL") == 1)
        {;}
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INFILE");
      if (hparse_errno > 0) return;
      if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLACE") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORE") == 1))
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      }
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INTO");
      if (hparse_errno > 0) return;
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE");
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      /* MariaDB manual doesn't mention partition clause but it's supported */
      hparse_f_partition_list(true, false);
      if (hparse_errno > 0) return;
      hparse_f_infile_or_outfile();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORE") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LINES") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROWS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1) /* [(col_name_or_user_var...)] */
      {
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
        do
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_COLUMN_OR_USER_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
          if (hparse_errno > 0) return;
        } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SET") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
        hparse_f_assignment(TOKEN_KEYWORD_LOAD, 0, false, false);
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INTO");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CACHE");
      if (hparse_errno > 0) return;
      do
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_partition_list(true, true);
        if (hparse_errno > 0) return;
        if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INDEX") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "KEY") == 1))
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
          if (hparse_errno > 0) return;
          do
          {
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_INDEX,TOKEN_TYPE_IDENTIFIER, "[identifier]");
            if (hparse_errno > 0) return;
          } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
          if (hparse_errno > 0) return;
        }
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORE") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LEAVES");
          if (hparse_errno > 0) return;
        }
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "XML") == 1)
    {
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOW_PRIORITY") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONCURRENT") == 1)) {;}
      hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCAL");
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INFILE");
      if (hparse_errno > 0) return;
      if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPLACE") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORE") == 1))
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      }
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INTO");
      if (hparse_errno > 0) return;
      main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE");
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_character_set() == 1)
      {
        if (hparse_f_character_set_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROWS") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IDENTIFIED");
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BY");
        if (hparse_errno > 0) return;
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IGNORE") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LINES") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROWS") == 1)) {;}
        else hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        do
        {
          if (hparse_f_qualified_name_of_operand(0, false,false,false) == 0)
          {
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER,  "[identifier]");
          }
          if (hparse_errno > 0) return;
        } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SET") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
        hparse_f_assignment(TOKEN_KEYWORD_LOAD, 0, false, false);
        if (hparse_errno > 0) return;
      }
    }
    else hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LOCK, "LOCK"))
  {
    hparse_statement_type= TOKEN_KEYWORD_LOCK;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLES") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE") == 1))/* TABLE is undocumented */
    {
      do
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ALIAS_OF_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ALIAS_OF_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) {;}
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "READ") == 1)
        {
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOCAL") == 1) {;}
        }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOW_PRIORITY") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WRITE");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WRITE") == 1)
        {
          if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONCURRENT");
        }
        else hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
    else
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INSTANCE") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR");
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BACKUP");
        if (hparse_errno > 0) return;
      }
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LUA, "LUA"))
  {
    hparse_statement_type= TOKEN_KEYWORD_LUA;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_OPTIMIZE, "OPTIMIZE") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_OPTIMIZE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    int table_or_view;
    if (hparse_f_analyze_or_optimize(TOKEN_KEYWORD_OPTIMIZE, &table_or_view) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PRAGMA, "PRAGMA") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_PRAGMA;
    bool is_valid= false;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if ((hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_COLLATION_LIST, "COLLATION_LIST") == 1)
     || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FOREIGN_KEY_LIST, "FOREIGN_KEY_LIST") == 1)
     || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INDEX_INFO, "INDEX_INFO") == 1)
     || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_STATS, "STATS") == 1)
     || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TABLE_INFO, "TABLE_INFO") == 1))
      is_valid= true;
    /* Following possibilities haven't existed since Tarantool 2.4 or earlier. Someday throw the code away, eh? */
    if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL_2_4) == 0)
    {
      if ((hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CASE_SENSITIVE_LIKE") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COUNT_CHANGES") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFER_FOREIGN_KEYS") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FULL_COLUMN_NAMES") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INDEX_LIST, "INDEX_LIST") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PARSER_TRACE, "PARSER_TRACE") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RECURSIVE_TRIGGERS") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REVERSE_UNORDERED_SELECTS") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SELECT_TRACE") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SHORT_COLUMN_NAMES") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SQL_COMPOUND_SELECT_LIMIT, "SQL_COMPOUND_SELECT_LIMIT") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SQL_DEFAULT_ENGINE, "SQL_DEFAULT_ENGINE") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL_TRACE") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VDBE_ADDOPTRACE") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VDBE_DEBUG") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VDBE_EQP") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VDBE_LISTING") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VDBE_TRACE") == 1)
       || (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WHERE_TRACE") == 1))
         is_valid= true;
    }
    if (is_valid == true)
    {
      int pragma_type= main_token_types[hparse_i_of_last_accepted];

      if ((pragma_type == TOKEN_KEYWORD_COLLATION_LIST)
       || (pragma_type == TOKEN_KEYWORD_FOREIGN_KEY_LIST)
       || (pragma_type == TOKEN_KEYWORD_PARSER_TRACE)
       || (pragma_type == TOKEN_KEYWORD_STATS))
      {
        ;
      }
      else
      {
        bool parenthesis_seen= false;
        bool equal_seen= false;
        if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
          parenthesis_seen= true;
        else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=") == 1)
          equal_seen= true;
        if ((parenthesis_seen) || (equal_seen))
        {
          if ((pragma_type == TOKEN_KEYWORD_BUSY_TIMEOUT)
           || (pragma_type == TOKEN_KEYWORD_SQL_COMPOUND_SELECT_LIMIT))
          {
            if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
            if (hparse_errno > 0) return;
          }
          else if (pragma_type == TOKEN_KEYWORD_INDEX_INFO)
          {
            hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
            if (hparse_errno > 0) return;
            hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ".");
            if (hparse_errno > 0) return;
            hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_INDEX,TOKEN_TYPE_IDENTIFIER, "[identifier]");
            if (hparse_errno > 0) return;
           }
          else if ((pragma_type == TOKEN_KEYWORD_TABLE_INFO)
                || (pragma_type == TOKEN_KEYWORD_INDEX_LIST))
          {
            if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) {;}
            else if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_TARANTOOL, TOKEN_LITERAL_FLAG_STRING) == 1) {;}
            else hparse_f_error();
            if (hparse_errno > 0) return;
          }
          else if (pragma_type == TOKEN_KEYWORD_SQL_DEFAULT_ENGINE)
          {
            if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ENGINE,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) {;}
            else if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_TARANTOOL, TOKEN_LITERAL_FLAG_STRING) == 1) {;}
            else hparse_f_error();
            if (hparse_errno > 0) return;
          }
          else
          {
            /* TODO: in fact the only valid thing now is (boolean) */
            if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) {;}
            else if (hparse_f_literal(TOKEN_REFTYPE_VARIABLE, FLAG_VERSION_TARANTOOL, TOKEN_LITERAL_FLAG_ANY) == 1) {;}
            else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON") == 1) {;}
            else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NO") == 1) {;}
            else hparse_f_error();
            if (hparse_errno > 0) return;
          }
        }
        if (parenthesis_seen)
        {
          hparse_f_expect(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
          if (hparse_errno > 0) return;
        }
      }
    }
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PREPARE, "PREPARE") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_PREPARE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_STATEMENT,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM");
    if (hparse_errno > 0) return;
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PURGE, "PURGE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_PURGE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BINARY") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOGS");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TO") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BEFORE");
      if (hparse_errno > 0) return;
      hparse_f_opr_1(0, 0); /* actually, should be "datetime expression" */
      if (hparse_errno > 0) return;
    }
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_RELEASE, "RELEASE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_RELEASE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SAVEPOINT");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SAVEPOINT");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_SAVEPOINT,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    return;
  }
  //else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REINDEX, "REINDEX") == 1)
  //{
  //  hparse_statement_type= TOKEN_KEYWORD_REINDEX;
  //  main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
  //  hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE);
  //  if (hparse_errno > 0) return;
  //}
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_RENAME, "RENAME") == 1)
  {
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_RENAME;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USER"))
    {
      do
      {
        if (hparse_errno > 0) return;
        if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TO");
        if (hparse_errno > 0) return;
        if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
    else {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE");
      if (hparse_errno > 0) return;
      do
      {
        if (hparse_errno > 0) return;
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TO");
        if (hparse_errno > 0) return;
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REPAIR, "REPAIR") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_REPAIR;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    int table_or_view;
    if (hparse_f_analyze_or_optimize(TOKEN_KEYWORD_REPAIR, &table_or_view) == 1)
    {
      if (table_or_view == TOKEN_KEYWORD_TABLE)
      {
        hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "QUICK");
        hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXTENDED");
        hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USE_FRM");
        hparse_f_accept(FLAG_VERSION_MARIADB_11_5, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FORCE");
      }
      if (table_or_view == TOKEN_KEYWORD_VIEW)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MYSQL");
          if (hparse_errno > 0) return;
        }
      }
    }
    else hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REPLACE, "REPLACE"))
  {
    if (hparse_errno > 0) return;
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    hparse_statement_type= TOKEN_KEYWORD_REPLACE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    //hparse_statement_type= TOKEN_KEYWORD_INSERT;
    hparse_subquery_is_allowed= true;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOW_PRIORITY") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DELAYED") == 1) {;}
    hparse_f_insert_or_replace();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_RESET, "RESET"))
  {
    hparse_statement_type= TOKEN_KEYWORD_RESET;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PERSIST") == 1) {;}
    else
    {
      /* Bools to stop duplicates like "replica,replica" although actually MySQL and MariaDB allow them. */
      bool is_master_seen= false; /* becomes true if either binary or master seen */
      bool is_query_seen= false;
      bool is_slave_seen= false; /* becomes true if either slave or replica seen */
      if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0) != 0) is_query_seen= true; /* i.e. MySQL 8.0+ disallows query cache */
      do
      {
        if (is_master_seen == false) /* MySQL prefers "binary" but allows "master" with warning, MariaDB only has "master" */
        {
          if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BINARY, "BINARY") == 1)
          {
            hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LOGS");
            if (hparse_errno > 0) return;
            hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_AND, "AND");
            if (hparse_errno > 0) return;
            hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GTIDS");
            if (hparse_errno > 0) return;
            is_master_seen= true;
          }
          else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER") == 1)
          {
            is_master_seen= true;
          }
          if (is_master_seen == true)
          {
            if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TO") == 1)
            {
              if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();
              if (hparse_errno > 0) return;
            }
            continue;
          }
        }
        if (is_slave_seen == false)  /* MySQL pre-8.4 prefers "replica" but allows "slave" with warning, MariaDB seems neutral */
        {
          if (hparse_f_accept_slave_or_replica(FLAG_VERSION_MYSQL_OR_MARIADB_ALL) == 1)
          {
            hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING); /* "connection_name" */
            hparse_f_accept(FLAG_VERSION_MYSQL_8_0 | FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ALL, "ALL");
            if ((hparse_dbms_mask & (FLAG_VERSION_MYSQL_8_0 | FLAG_VERSION_MARIADB_10_7)) != 0)
            {
              hparse_f_for_channel();
              if (hparse_errno > 0) return;
            }
            is_slave_seen= true;
            continue;
          }
        }
        if (is_query_seen == false)
        {
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_QUERY, "QUERY") == 1)
          {
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CACHE");
            if (hparse_errno > 0) return;
            is_query_seen= true;
            continue;
          }
        }
        hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_RESIGNAL, "RESIGNAL"))
  {
    hparse_statement_type= TOKEN_KEYWORD_RESIGNAL;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    /* We accept RESIGNAL even if we're not in a condition handler; we're just a recognizer. */
    hparse_f_signal_or_resignal(TOKEN_KEYWORD_RESIGNAL, block_top);
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REVOKE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_REVOKE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    bool role_name_seen= false;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PROXY") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ON");
      if (hparse_errno > 0) return;
      if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else
    {
      hparse_f_grant_or_revoke(TOKEN_KEYWORD_REVOKE, &role_name_seen);
      if (hparse_errno > 0) return;
    }
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM");
    if (hparse_errno > 0) return;
    do
    {
      if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ROLLBACK, "ROLLBACK") == 1)
  {
    int saved_hparse_i_of_last_accepted= hparse_i_of_last_accepted;
    hparse_statement_type= TOKEN_KEYWORD_ROLLBACK;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WORK");
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TO") == 1)
    {
      main_token_types[saved_hparse_i_of_last_accepted]= TOKEN_KEYWORD_ROLLBACK_IN_ROLLBACK_TO;
      /* it's not documented, but the word SAVEPOINT is optional */
      hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SAVEPOINT");
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_SAVEPOINT,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      return;
    }
    else hparse_f_commit_or_rollback();
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SAVEPOINT, "SAVEPOINT") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_SAVEPOINT;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_SAVEPOINT,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    return;
  }
  /* this checks for "SELECT" and "VALUES" and "WITH" and "TABLE" */
  else if (hparse_f_query(block_top, false, true, true) == 1)
  {
    if (hparse_errno > 0) return;
  }
  else if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_TARANTOOL_2_4, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SET, "SET"))
        || (hparse_f_is_in_compound() && hparse_f_accept(FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_SET, "SET")))
  {
    hparse_statement_type= TOKEN_KEYWORD_SET;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_subquery_is_allowed= true;
#if (OCELOT_MYSQL_INCLUDE == 1)
    int hparse_i_of_set_statement= hparse_i_of_last_accepted;
#endif //#if (OCELOT_MYSQL_INCLUDE == 1)
    if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL_2_4) != 0)
    {
      hparse_f_expect(FLAG_VERSION_TARANTOOL_2_4, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SESSION");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_TARANTOOL_2_4, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_TARANTOOL_2_4, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_TARANTOOL_2_4, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
      return;
    }
    bool global_seen= false;
    bool persist_seen= false;
    bool equal_seen= false;
    hparse_f_next_nexttoken();
    /* role, statement, transaction are unreserved, can be variables */
    if ((hparse_next_token == "=")
     || (hparse_next_token == ":="))
    {
      if ((hparse_token_type == TOKEN_KEYWORD_ROLE)
       || (hparse_token_type == TOKEN_KEYWORD_STATEMENT)
       || (hparse_token_type == TOKEN_KEYWORD_TRANSACTION))
      {
        equal_seen= true;
      }
    }
    hparse_f_is_global_or_persist(&global_seen, &persist_seen);
    if ((persist_seen == false) && (equal_seen == false))
    {if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TRANSACTION") == 1)
    {
      bool isolation_seen= false, read_seen= false;
      do
      {
        if ((isolation_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ISOLATION") == 1))
        {
          isolation_seen= true;
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LEVEL");
          if (hparse_errno > 0) return;
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REPEATABLE") == 1)
          {
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "READ");
            if (hparse_errno > 0) return;
          }
          else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "READ") == 1)
          {
            if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COMMITTED") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNCOMMITTED");
            if (hparse_errno > 0) return;
          }
          else hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SERIALIZABLE");
          if (hparse_errno > 0) return;
        }
        else if ((read_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "READ") == 1))
        {
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WRITE") == 0) hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ONLY");
          if (hparse_errno > 0) return;
        }
        else hparse_f_error();
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      return;
    }
    }
    if ((global_seen == false) && (persist_seen == false) && (equal_seen == false))
    {if (hparse_f_character_set() == 1)
    {
      if (hparse_f_character_set_name() == 0)
      {
        if (hparse_f_literal(TOKEN_REFTYPE_CHARACTER_SET, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0)
        {
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "DEFAULT"))
          {
            main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
            return;
          }
        }
        if (hparse_errno > 0) return;
      }
      return;
    }
    }
    if (hparse_errno > 0) return;
    if ((global_seen == false) && (persist_seen == false) && (equal_seen == false))
    {
    if (((hparse_dbms_mask & (FLAG_VERSION_MARIADB_ALL | FLAG_VERSION_MYSQL_8_0)) != 0) && (hparse_f_accept(FLAG_VERSION_MARIADB_ALL | FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 1))
    {
      main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
      hparse_f_expect(FLAG_VERSION_MARIADB_ALL | FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROLE");
      if (hparse_errno > 0) return;
      if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NONE") == 0)
        {
          if (hparse_f_user_or_role_name(TOKEN_REFTYPE_ROLE) == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        }
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR") == 1)
        {
          if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        }
      }
      else /* presumably MySQL 8.0 */
      {
        if ((hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NONE") == 1)
         || (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1))
        {
          ;
        }
        else
        {
          do
          {
            if (hparse_f_user_or_role_name(TOKEN_REFTYPE_ROLE) == 0) hparse_f_error();
            if (hparse_errno > 0) return;
          } while (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
        }
        hparse_f_expect(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TO");
        if (hparse_errno > 0) return;
        do
        {
          if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        } while (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      }
      return;
    }
    }
    if ((global_seen == false) && (persist_seen == false) && (equal_seen == false))
    {if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NAMES") == 1)
    {
      if (hparse_f_character_set_name() == 0)
      {
        if (hparse_f_literal(TOKEN_REFTYPE_CHARACTER_SET, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0)
        {
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_KEYWORD, "DEFAULT"))
          {
            main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
            return;
          }
        }
      }
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COLLATE"))
      {
        if (hparse_f_collation_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      return;
    }
    }
    if ((global_seen == false) && (persist_seen == false) && (equal_seen == false))
    {
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PASSWORD") == 1)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR") == 1)
      {
        if (hparse_f_user_or_role_name(TOKEN_REFTYPE_USER) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PASSWORD") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
        if (hparse_errno > 0) return;
        if (hparse_f_literal(TOKEN_REFTYPE_PASSWORD, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
      else
      {
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      }
      if (hparse_errno > 0) return;
      return;
    }
    }
    if ((global_seen == false) && (persist_seen == false) && (equal_seen == false))
    {
    if (((hparse_dbms_mask & (FLAG_VERSION_MARIADB_ALL | FLAG_VERSION_MYSQL_8_0)) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROLE") == 1))
    {
      if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NONE") == 0)
        {
          if (hparse_f_user_or_role_name(TOKEN_REFTYPE_ROLE) == 0) hparse_f_error();
        }
        if (hparse_errno > 0) return;
      }
      else /* presumably MySQL 8.0 */
      {
        bool is_needing_list= false;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NONE") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1)
        {
          if (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXCEPT") == 1)
            is_needing_list= true;
        }
        else is_needing_list= true;
        if (is_needing_list == true)
        {
          do
          {
            if (hparse_f_user_or_role_name(TOKEN_REFTYPE_ROLE) == 0) hparse_f_error();
            if (hparse_errno > 0) return;
          } while (hparse_f_accept(FLAG_VERSION_MYSQL_8_0, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
        }
      }
      return;
    }
    }
    if ((global_seen == false) && (persist_seen == false) && (equal_seen == false))
    {
    if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATEMENT") == 1))
    {
      hparse_f_assignment(TOKEN_KEYWORD_SET, 0, false, false);
      if (hparse_errno > 0) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR");
      if (hparse_errno > 0) return;
      hparse_f_pseudo_statement(block_top);
      if (hparse_errno > 0) return;
      return;
    }
    }
    /* TODO: This fails to take "set autocommit = {0 | 1}" into account as special. */
    /* TODO: This fails to take "set sql_log_bin = {0 | 1}" into account as special. */
    hparse_f_assignment(TOKEN_KEYWORD_SET, 0, global_seen, persist_seen);
    if (hparse_errno > 0) return;
    /* SET sql_mode=... can change how we parse further statements,
       if SET is top level (within a stored procedure, we don't care).
       Re-tokenize as we do for delimiters is not necessary,
       but tokens_to_keywords() will be redone so we can say whether
       "x" is a literal or an identifier.
       Not always reliable, see get_sql_mode() for details.
    */
#if (OCELOT_MYSQL_INCLUDE == 1)
    if (hparse_i_of_set_statement == block_top)
    {
      if (get_sql_mode(TOKEN_KEYWORD_SET, hparse_text_copy, true, hparse_i_of_set_statement) == true)
      {
        tokens_to_keywords(hparse_text_copy, hparse_i, hparse_sql_mode_ansi_quotes);
      }
    }
#endif //#if (OCELOT_MYSQL_INCLUDE == 1)
  }
  /* "SHOW" is checked in hparse_f_query subroutines */
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SHUTDOWN, "SHUTDOWN"))
  {
    hparse_statement_type= TOKEN_KEYWORD_SHUTDOWN;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SIGNAL, "SIGNAL"))
  {
    hparse_statement_type= TOKEN_KEYWORD_SIGNAL;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_signal_or_resignal(TOKEN_KEYWORD_SIGNAL, block_top) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SONAME, "SONAME") == 1))
  {
    hparse_statement_type= TOKEN_KEYWORD_SONAME;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_START, "START"))
  {
    hparse_statement_type= TOKEN_KEYWORD_START;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TRANSACTION") == 1)
    {
      if (hparse_errno > 0) return;
      bool with_seen= false, read_seen= false;
      do
      {
        if ((with_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WITH")))
        {
          with_seen= true;
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONSISTENT");
          if (hparse_errno > 0) return;
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SNAPSHOT");
          if (hparse_errno > 0) return;
        }
        if ((read_seen == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "READ")))
        {
          read_seen= true;
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ONLY") == 1) ;
          else hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WRITE");
          if (hparse_errno > 0) return;
        }
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GROUP_REPLICATION") == 1) {;}
    else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1))
    {
      if (hparse_f_accept_slaves_or_replicas(FLAG_VERSION_MYSQL_OR_MARIADB_ALL) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      do
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IO_THREAD") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL_THREAD") == 1) {;}
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
    else if (hparse_f_accept_slave_or_replica(FLAG_VERSION_MYSQL_OR_MARIADB_ALL) == 1)
    {
      if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING);
      do
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IO_THREAD") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL_THREAD") == 1) {;}
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNTIL") == 1)
      {
        if (((hparse_dbms_mask & FLAG_VERSION_MYSQL_ALL) != 0)
         && ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL_BEFORE_GTIDS") == 1)
          || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL_AFTER_GTIDS") == 1)
          || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL_AFTER_MTS_GAPS") == 1)))
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT) == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_LOG_FILE") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
          if (hparse_errno > 0) return;
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",");
          if (hparse_errno > 0) return;
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_LOG_POS");
          if (hparse_errno > 0) return;
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RELAY_LOG_FILE") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();

          if (hparse_errno > 0) return;
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",");
          if (hparse_errno > 0) return;
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RELAY_LOG_POS");
          if (hparse_errno > 0) return;
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error();

          if (hparse_errno > 0) return;
        }
        else if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MASTER_GTID_POS") == 1))
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT) == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        }
        else hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_ALL) != 0)
      {
        for (;;)
        {
          bool expect_something= false;
          if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USER") == 1) expect_something= true;
          else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PASSWORD") == 1) expect_something= true;
          else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DEFAULT_AUTH") == 1) expect_something= true;
          else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PLUGIN_DIR") == 1) expect_something= true;
          else break;
          if (hparse_errno > 0) return;
          if (expect_something)
          {
            hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
            if (hparse_errno > 0) return;
            if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT) == 0) hparse_f_error();
            if (hparse_errno > 0) return;
          }
        }
        hparse_f_for_channel();
        if (hparse_errno > 0) return;
      }
    }
    else hparse_f_error();
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_STOP, "STOP") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_STOP;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "GROUP_REPLICATION") == 1) {;}
    else if (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ALL") == 1)
    {
      if (hparse_f_accept_slaves_or_replicas(FLAG_VERSION_MARIADB_ALL) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept_slave_or_replica(FLAG_VERSION_MYSQL_OR_MARIADB_ALL) == 1)
    {
      if (((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) && (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 1)) {;}
      do
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IO_THREAD") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQL_THREAD") == 1) {;}
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_ALL) != 0)
      {
        hparse_f_for_channel();
        if (hparse_errno > 0) return;
      }
    }
    else hparse_f_error();
  }
  /* "TABLE" is covered by the earlier call to hparse_f_query(0, ) */
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TRUNCATE, "TRUNCATE"))
  {
    if (hparse_errno > 0) return; /* todo: check: why do I need this here and not for other statements? */
    hparse_statement_type= TOKEN_KEYWORD_TRUNCATE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE") != 1)
    {
      if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0) hparse_f_error();
    }
    if (hparse_errno > 0) return;
    if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if ((hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DROP") == 1)
     || (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REUSE") == 1))
    {
      hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STORAGE");
      if (hparse_errno > 0) return;
    }
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_UNINSTALL, "UNINSTALL") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_UNINSTALL;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PLUGIN") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_PLUGIN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    else
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SONAME");
      if (hparse_errno > 0) return;
      if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_UNLOCK, "UNLOCK"))
  {
    hparse_statement_type= TOKEN_KEYWORD_UNLOCK;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLES") == 1)
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "TABLE") == 1)  /* TABLE is undocumented */
     || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INSTANCE") == 1))
    {
      ;
    }
    else hparse_f_error();
    return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_UPDATE, "UPDATE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_UPDATE;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
#ifdef ALLOW_CONFLICT_CLAUSES
    if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "OR") == 1)
    {
      hparse_f_conflict_algorithm();
      if (hparse_errno > 0) return;
    }
#endif
    hparse_subquery_is_allowed= true;
    if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
    {
      if (hparse_f_qualified_name_with_star() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_table_index_hint_list();
      if (hparse_errno > 0) return;
    }
    else
    {
      /* todo: this sees some choices that are only valid with SELECT */
      if (hparse_f_table_reference(0) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    bool multi_seen= false;
    while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1)
    {
      multi_seen= true;
      if (hparse_f_table_reference(0) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SET");
    if (hparse_errno > 0) return;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    hparse_f_assignment(TOKEN_KEYWORD_UPDATE, 0, false, false);
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WHERE") == 1)
    {
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return;
    }
    if (multi_seen == false)
    {
      if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0)
      {
        hparse_f_order_by(0);
        if (hparse_errno > 0) return;
        hparse_f_limit(TOKEN_KEYWORD_UPDATE);
        if (hparse_errno > 0) return;
      }
    }
  }
  else if (hparse_f_accept(FLAG_VERSION_TARANTOOL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_VACUUM, "VACUUM"))
  {
    hparse_statement_type= TOKEN_KEYWORD_VACUUM;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    return;
  }
  /* "VALUES" is covered by the earlier call to hparse_f_query(0, ) */
  /* "WITH" is covered by the earlier call to hparse_f_query(0, ) */
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_XA, "XA"))
  {
    hparse_statement_type= TOKEN_KEYWORD_XA;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "START") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BEGIN_XA, "BEGIN") == 1))
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "JOIN") == 1) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RESUME") == 1)) {;}
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "END") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SUSPEND") == 1)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR") == 1)
        {
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "MIGRATE");
          if (hparse_errno > 0) return;
        }
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PREPARE") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "COMMIT") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ONE") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PHASE");
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ROLLBACK") == 1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "RECOVER") == 1)
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONVERT") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "XID");
        if (hparse_errno > 0) return;
      }
    }
    else hparse_f_error();
  }
  else
  {
    /* Check for Lua unless it's first word (hmm, won't it always be?) and there are partial matches. */
    if (((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
     && ((hparse_dbms_mask & FLAG_VERSION_LUA_OUTPUT) == 0)
     && (hparse_f_is_in_compound() == false))
    {
      if (hparse_errno > 0) return;
#ifdef DBMS_TARANTOOL
      if ((hparse_i == 0) && (completer_widget->count_wrapper() > 0)) hparse_f_error();
      else hparse_f_lua_blocklist(0, hparse_i);
#endif
    }
    else hparse_f_error();
  }
  log("hparse_f_statement end", 80);
}

/*
  ANALYZE, EXPLAIN, SET STATEMENT, and WITH can contain sub-statements
  that syntactically are complete, and that's okay, but we don't want to
  turn on the is_start_statement flag for sub-statements. We would
  confuse the debugger.
*/
void MainWindow::hparse_f_pseudo_statement(int block_top)
{
  int hparse_j= hparse_i;
  hparse_f_statement(block_top);
  if ((main_token_flags[hparse_j] & TOKEN_FLAG_IS_START_STATEMENT) != 0)
  {
    main_token_flags[hparse_j] &= (~TOKEN_FLAG_IS_START_STATEMENT);
    main_token_flags[hparse_j] &= (~TOKEN_FLAG_IS_DEBUGGABLE);
    main_token_flags[hparse_j] |= TOKEN_FLAG_IS_START_CLAUSE;
  }
}


/*
  compound statement, or statement
  Pass: calling_statement_type = 0 (top level) | TOKEN_KEYWORD_FUNCTION/PROCEDURE/EVENT/TRIGGER
  Todo: for plsql, we still don't accept "begin procname; ...". I'm
        uncertain whether "if ... then ... procname; ..." is okay too.
        Luckily "v := funcname;" is something MariaDB can't handle.
  TODO: every statement in the block should treat failure with
        "delay check if (hparse_errno > 0) return;" (?)
  TODO: every statement in the block should get start-statement flag (?)
*/
void MainWindow::hparse_f_block(int calling_statement_type,
                                int block_top)
{
  log("hparse_f_block start", 80);
  if (hparse_errno > 0) return;
  hparse_subquery_is_allowed= false;
  int hparse_i_of_start= hparse_i;
  if ((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0)
  {
    if (hparse_i == block_top)
    {
      if ((calling_statement_type == TOKEN_KEYWORD_PROCEDURE)
       || (calling_statement_type == TOKEN_KEYWORD_FUNCTION))
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IS, "IS") == 0)
          hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_AS, "AS");
        if (hparse_errno > 0) return;
        hparse_as_seen= true;
        hparse_f_declare_plsql(TOKEN_KEYWORD_AS);
        if (hparse_f_recover_if_error(true, "") == 2) return;
      }
    }
  }

  /* PLSQL DECLARE can be late. Todo: can it be preceded by label? */
  if (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DECLARE, "DECLARE") == 1)
  {
    hparse_f_declare_plsql(TOKEN_KEYWORD_DECLARE);
    if (hparse_f_recover_if_error(true, "") == 2) return;
  }

  /*
    TODO:
      For labels + conditions + local variables, you could:
      push on stack when they come into scope
      pop from stack when they go out of scope
      check they're valid when you see reference
      show what they are when you see hover (requires showing where they're declared too)
      ... but currently we're saying any identifier will be okay
    But we're working on it. As a first step, for keeping track of
    scope, we have hparse_i_of_block = offset of last
    BEGIN|LOOP|WHILE|REPEAT|IF (or label that precedes that, if any).
    So when we reach END we can set main_token_pointers[] to point
    "back" to where a block started.
    We also use main_token_pointers[] for pointing from variable_refer
    to variable_define, but it's not compulsory, it could be unknown.
    Todo: Consider using the same technique for ()s and statements.
    Todo: Consider pointing forward as well as backward.
  */

  int hparse_i_of_block= -1;
  QString label= "";
  /* Label check. */
  /* Todo: most checks are illegal if preceded by a label. Check for that. */
  if (hparse_count_of_accepts != 0)
  {
    label= hparse_f_label(&hparse_i_of_block);
    if (hparse_errno > 0) return;
  }
  /*
    BEGIN could be the start of a BEGIN END block, but
    "BEGIN;" or "BEGIN WORK" are start-transaction statements.
    Ugly.
    Todo: See what happens if next is \G or delimiter.
  */
  bool next_is_semicolon_or_work= false;
  hparse_f_next_nexttoken();
  if ((hparse_next_token == ";")
   || (hparse_next_token == hparse_delimiter_str)
   || (QString::compare(hparse_next_token, "WORK", Qt::CaseInsensitive) == 0))
  {
    next_is_semicolon_or_work= true;
  }
  //int hparse_i_of_start= hparse_i;
  if ((next_is_semicolon_or_work == false) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BEGIN, "BEGIN") == 1))
  {
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    hparse_statement_type= TOKEN_KEYWORD_BEGIN;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_FLOW_CONTROL;
    ++hparse_begin_count; ++hparse_flow_count;
    if (hparse_f_accept(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOT") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ATOMIC, "ATOMIC");
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms_mask & FLAG_VERSION_PLSQL) == 0)
    {
      /* The MariaDB parser won't handle top-level BEGIN without NOT, unless sql_mode=oracle. */
      if (hparse_count_of_accepts < 2)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WORK"); /* impossible but enhances expected_list */
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";");   /* impossible but enhances expected_list */
        hparse_f_error();
        return;
      }
    }
    for (;;)                                                           /* DECLARE statements */
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DECLARE") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DECLARE;
        hparse_f_declare(calling_statement_type, block_top);
        if (hparse_f_recover_if_error(true, "") == 2) return;
      }
      else break;
    }

    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "END") == 0)
    {
      for (;;)
      {
        hparse_f_block(calling_statement_type, block_top);
        if (hparse_errno > 0) return;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "END") == 1)
        {
          --hparse_begin_count; --hparse_flow_count;
          if (hparse_begin_count == 0) hparse_as_seen= false;
          break;
        }
      }
    }
    else
    {
      --hparse_begin_count; --hparse_flow_count;
      if (hparse_begin_count == 0) hparse_as_seen= false;
    }
    if (((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0)
     && (hparse_i_of_start == block_top))
    {
      if ((calling_statement_type == TOKEN_KEYWORD_PROCEDURE)
       || (calling_statement_type == TOKEN_KEYWORD_FUNCTION))
      {
        int j;
        bool is_routine_name_seen= false;
        for (j= hparse_i_of_start; j != 0; --j)
        {
          if ((main_token_reftypes[j] == TOKEN_REFTYPE_PROCEDURE)
           || (main_token_reftypes[j] == TOKEN_REFTYPE_FUNCTION))
          {
            is_routine_name_seen= true;
            break;
          }
          if ((main_token_flags[j] & TOKEN_FLAG_IS_START_STATEMENT) != 0) break;
        }
        QString routine_name= "[identifier]";
        if (is_routine_name_seen)
          routine_name= hparse_text_copy.mid(main_token_offsets[j], main_token_lengths[j]);
        /* Todo: find out why "Expecting:" doesn't show for this accept */
        hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_FUNCTION_OR_PROCEDURE,TOKEN_TYPE_IDENTIFIER, routine_name);
      }
    }

    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    main_token_pointers[hparse_i_of_last_accepted]= hparse_i_of_block;
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_LABEL_REFER,TOKEN_TYPE_IDENTIFIER, label);
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CASE, "CASE") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_FLOW_CONTROL;
    ++hparse_flow_count;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    int when_count= 0;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WHEN") == 0)
     {
      hparse_f_opr_1(0, 0); /* not compulsory */
      if (hparse_errno > 0) return;
    }
    else when_count= 1;
    if (when_count == 0)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WHEN");
      if (hparse_errno > 0) return;
    }
    for (;;)
    {
      hparse_subquery_is_allowed= true;
      hparse_f_opr_1(0, 0);
      hparse_subquery_is_allowed= false;
      if (hparse_f_recover_if_error(false, "THEN") == 2) return;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "THEN");
      if (hparse_errno > 0) return;
      int break_word= 0;
      for (;;)
      {
        hparse_f_block(calling_statement_type, block_top);
        if (hparse_errno > 0) return;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "END") == 1)
        {
          break_word= TOKEN_KEYWORD_END; break;
        }
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WHEN") == 1)
        {
          break_word= TOKEN_KEYWORD_WHEN; break;
        }
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ELSE") == 1)
        {
          break_word= TOKEN_KEYWORD_ELSE; break;
        }
      }
      if (break_word == TOKEN_KEYWORD_END) break;
      if (break_word == TOKEN_KEYWORD_WHEN)
      {
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_FLOW_CONTROL;
        continue;
      }
      assert(break_word == TOKEN_KEYWORD_ELSE);
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_FLOW_CONTROL;
      for (;;)
      {
        hparse_f_block(calling_statement_type, block_top);
        if (hparse_errno > 0) return;
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "END") == 1) break;
      }
      break;
    }
    --hparse_flow_count;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    main_token_pointers[hparse_i_of_last_accepted]= hparse_i_of_block;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CASE, "CASE");
    if (hparse_errno > 0) return;
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_LABEL_REFER,TOKEN_TYPE_IDENTIFIER, label);
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FOR_IN_FOR_STATEMENT, "FOR") == 1)
  {
    QString do_or_loop= "DO";
    if ((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0) do_or_loop= "LOOP";
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_FLOW_CONTROL;
    ++hparse_flow_count;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    /* Todo: Find out later if integer-variable or record-variable" */
    hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_VARIABLE_DEFINE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    //int hparse_i_of_identifier= hparse_i_of_last_accepted;
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IN, "IN");
    if (hparse_errno > 0) return;
    {
      hparse_f_next_nexttoken();
      if ((hparse_token == "(")
       && (hparse_next_token.toUpper() == "SELECT"))
      {
        if (hparse_f_query(0, false, false, false) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if (hparse_next_token.toUpper() == do_or_loop)
      {
        hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_CURSOR_REFER,TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
      }
      else
      {
        hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "REVERSE");
        hparse_f_opr_1(0, 0);
        if (hparse_errno > 0) return;
        hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "..");
        if (hparse_errno > 0) return;
        hparse_f_opr_1(0, 0);
        if (hparse_errno > 0) return;
      }
    }
    if (hparse_f_recover_if_error(false, do_or_loop) == 2) return;
    hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DO, do_or_loop);
    if (hparse_errno > 0) return;
    for (;;)
    {
      hparse_f_block(calling_statement_type, block_top);
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "END") == 1) break;
    }
    --hparse_flow_count;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    main_token_pointers[hparse_i_of_last_accepted]= hparse_i_of_block;
    if ((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0)
      hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FOR_IN_FOR_STATEMENT, "LOOP");
    else
      hparse_f_expect(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FOR_IN_FOR_STATEMENT, "FOR");
    if (hparse_errno > 0) return;
    hparse_f_accept(FLAG_VERSION_MARIADB_10_3, TOKEN_REFTYPE_LABEL_REFER,TOKEN_TYPE_IDENTIFIER, label);
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF, "IF") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_FLOW_CONTROL;
    ++hparse_flow_count;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    for (;;)
    {
      hparse_subquery_is_allowed= true;
      hparse_f_opr_1(0, 0);
      hparse_subquery_is_allowed= false;
      if (hparse_f_recover_if_error(false, "THEN") == 2) return;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "THEN");
      if (hparse_errno > 0) return;
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
      int break_word= 0;
      for (;;)
      {
        hparse_f_block(calling_statement_type, block_top);
        if (hparse_errno > 0) return;
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "END") == 1)
        {
          break_word= TOKEN_KEYWORD_END; break;
        }
        QString q_elseif= "ELSEIF";
        if ((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0) q_elseif= "ELSIF";
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ELSEIF, q_elseif) == 1)
        {
          break_word= TOKEN_KEYWORD_ELSEIF; break;
        }
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "ELSE") == 1)
        {
          break_word= TOKEN_KEYWORD_ELSE; break;
        }
      }
      if (break_word == TOKEN_KEYWORD_END)
      {
        break;
      }
      if (break_word == TOKEN_KEYWORD_ELSEIF)
      {
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_FLOW_CONTROL;
        continue;
      }
      assert(break_word == TOKEN_KEYWORD_ELSE);
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_FLOW_CONTROL;
      for (;;)
      {
        hparse_f_block(calling_statement_type, block_top);
        if (hparse_errno > 0) return;
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "END") == 1) break;
      }
      break;
    }
    --hparse_flow_count;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    main_token_pointers[hparse_i_of_last_accepted]= hparse_i_of_block;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF, "IF");
    if (hparse_errno > 0) return;
    hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_LABEL_REFER,TOKEN_TYPE_IDENTIFIER, label);
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LOOP, "LOOP") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_FLOW_CONTROL;
    ++hparse_flow_count;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    for (;;)
    {
      hparse_f_block(calling_statement_type, block_top);
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "END") == 1) break;
    }
    --hparse_flow_count;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    main_token_pointers[hparse_i_of_last_accepted]= hparse_i_of_block;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LOOP, "LOOP");
    if (hparse_errno > 0) return;
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_LABEL_REFER,TOKEN_TYPE_IDENTIFIER, label);
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REPEAT, "REPEAT") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_FLOW_CONTROL;
    ++hparse_flow_count;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    for (;;)
    {
      hparse_f_block(calling_statement_type, block_top);
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNTIL") == 1) break;
    }
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    hparse_subquery_is_allowed= true;
    hparse_f_opr_1(0, 0);
    if (hparse_errno > 0) return;
    hparse_subquery_is_allowed= false;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "END");
    if (hparse_errno > 0) return;
    --hparse_flow_count;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    main_token_pointers[hparse_i_of_last_accepted]= hparse_i_of_block;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REPEAT, "REPEAT");
    if (hparse_errno > 0) return;
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_LABEL_REFER,TOKEN_TYPE_IDENTIFIER, label);
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if ((hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CONTINUE, "CONTINUE") == 1)
       || (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_EXIT, "EXIT") == 1)
       || (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_GOTO, "GOTO") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ITERATE, "ITERATE") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "LEAVE") == 1))
  {
    hparse_statement_type= main_token_types[hparse_i_of_last_accepted];
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if ((hparse_statement_type == TOKEN_KEYWORD_CONTINUE)
     || (hparse_statement_type == TOKEN_KEYWORD_EXIT))
    {
      hparse_f_find_define(block_top, TOKEN_REFTYPE_LABEL_DEFINE, TOKEN_REFTYPE_LABEL_REFER, false);
      if (hparse_f_recover_if_error(false, "") == 2) return;
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "WHEN") == 1)
      {
        hparse_f_opr_1(hparse_statement_type, 0);
        if (hparse_f_recover_if_error(false, "") == 2) return;
      }
    }
    else hparse_f_find_define(block_top, TOKEN_REFTYPE_LABEL_DEFINE, TOKEN_REFTYPE_LABEL_REFER, true);
    if (hparse_f_recover_if_error(false, "") == 2) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";");
    if (hparse_f_recover_if_error(true, "") == 2) return;
  }
  else if ((hparse_f_is_in_compound()) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CLOSE, "CLOSE") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_DEBUGGABLE;
    main_token_pointers[hparse_i]= hparse_f_find_define(block_top, TOKEN_REFTYPE_CURSOR_DEFINE, TOKEN_REFTYPE_CURSOR_REFER, true);
    if (hparse_f_recover_if_error(false, "") == 2) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";");
    if (hparse_f_recover_if_error(true, "") == 2) return;
  }
  else if ((hparse_f_is_in_compound()) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FETCH, "FETCH") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NEXT") == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM");
      if (hparse_f_recover_if_error(false, "") == 2) return;
    }
    else hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FROM");
    main_token_pointers[hparse_i]= hparse_f_find_define(block_top, TOKEN_REFTYPE_CURSOR_DEFINE, TOKEN_REFTYPE_CURSOR_REFER, true);
    if (hparse_f_recover_if_error(false, "") == 2) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "INTO");
    if (hparse_f_recover_if_error(false, "") == 2) return;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_CLAUSE;
    do
    {
      int i_of_define;
      int reftype;
      reftype= hparse_f_variables(&i_of_define);
      if (reftype == 0) hparse_f_error();
      if (hparse_errno > 0) break;
      /* guaranteed to succeed */
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, reftype,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) break;
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    if (hparse_f_recover_if_error(false, "") == 2) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";");
    if (hparse_f_recover_if_error(true, "") == 2) return;
  }
  else if ((hparse_f_is_in_compound()) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_OPEN, "OPEN") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    main_token_pointers[hparse_i]= hparse_f_find_define(block_top, TOKEN_REFTYPE_CURSOR_DEFINE, TOKEN_REFTYPE_CURSOR_REFER, true);
    if (hparse_f_recover_if_error(false, "") == 2) return;
    if ((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0)
    {
      hparse_f_call_arguments();
    }
    if (hparse_f_recover_if_error(false, "") == 2) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";");
    if (hparse_f_recover_if_error(true, "") == 2) return;
  }
  else if ((hparse_f_is_in_compound()) && (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_RAISE, "RAISE") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_f_plsql_condition(block_top);
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";");
    if (hparse_f_recover_if_error(true, "") == 2) return;
  }
  else if (((calling_statement_type == TOKEN_KEYWORD_FUNCTION)
           || ((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0))
        && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_RETURN, "RETURN") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
    hparse_subquery_is_allowed= true;
    if (calling_statement_type == TOKEN_KEYWORD_FUNCTION)
    {
      hparse_f_opr_1(0, 0);
      if (hparse_f_recover_if_error(false, "") == 2) return;
    }
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_f_recover_if_error(true, "") == 2) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WHILE, "WHILE") == 1)
  {
    QString do_or_loop= "DO";
    if ((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0) do_or_loop= "LOOP";
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_FLOW_CONTROL;
    ++hparse_flow_count;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    hparse_subquery_is_allowed= true;
    hparse_f_opr_1(0, 0);
    hparse_subquery_is_allowed= false;
    if (hparse_f_recover_if_error(false, do_or_loop)) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DO, do_or_loop);
    if (hparse_errno > 0) return;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    for (;;)
    {
      hparse_f_block(calling_statement_type, block_top);
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "END") == 1) break;
    }
    --hparse_flow_count;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    main_token_pointers[hparse_i_of_last_accepted]= hparse_i_of_block;
    if ((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0)
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WHILE, "LOOP");
    else
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WHILE, "WHILE");
    if (hparse_errno > 0) return;
    hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_LABEL_REFER,TOKEN_TYPE_IDENTIFIER, label);
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_EXCEPTION, "EXCEPTION") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_FLOW_CONTROL;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    int when_count= 0;
    /* TODO: watch https://jira.mariadb.org/browse/MDEV-10586 for more conditions. */
    for (;;)
    {
      if (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WHEN, "WHEN") == 0)
      {
        if (when_count == 0) hparse_f_error();
        break;
      }
      ++when_count;
      if (hparse_f_plsql_condition(block_top) == 0) hparse_f_error();
      if (hparse_f_recover_if_error(false, "THEN") == 2) return;
      hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_THEN, "THEN");
      if (hparse_errno > 0) return;
      hparse_f_block(calling_statement_type, block_top);
      if (hparse_errno > 0) return;
    }
  }
  else
  {
    /* Some oracle_mode statements that can only be in begin ... end */
    bool is_statement_done= false;
    if (((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0)
     && (hparse_f_is_in_compound()))
    {
      hparse_f_label(&hparse_i_of_block);
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_NULL, "NULL") == 1)
      {
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
        is_statement_done= true;
      }
      else
      {
        /* if token is identifier: "x :=" means it's a variable,
           "x;" or "x(" or "x." means it's a procedure,
           "x eof" means it's either variable or procedure,
           "x other" means error */
        int r= hparse_f_qualified_name_of_operand(0, true, true, false);
        if (hparse_errno > 0) is_statement_done= true;
        if (r == 1)
        {
          int reftype= main_token_reftypes[hparse_i_of_last_accepted];
          if (reftype == TOKEN_REFTYPE_VARIABLE_REFER)
          {
            main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
            /* todo: check if hparse_f_assignment is better? */
            hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ":=");
            /* delay check if (hparse_errno > 0) return; */
            if (hparse_errno == 0) hparse_f_opr_1(0, 0);
            /* delay check if (hparse_errno > 0) return; */
            is_statement_done= true;
          }
          else if (reftype == TOKEN_REFTYPE_FUNCTION)
          {
            hparse_statement_type= TOKEN_KEYWORD_CALL;
            main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
            hparse_f_call_arguments();
            /* delay check if (hparse_errno > 0) return; */
            is_statement_done= true;
          }
          else hparse_f_error();
        }
      }
      if (hparse_errno > 0) is_statement_done= true;
    }
    if (is_statement_done == false) hparse_f_statement(block_top);

    if (hparse_f_recover_if_error(false, "") == 2) return;

    /* This kludge occurs more than once. */
    if ((hparse_prev_token != ";") && (hparse_prev_token != hparse_delimiter_str))
    {
      {
        if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0)
        {
          hparse_f_error();
        }
      }
    }
    log("hparse_f_block end", 80);
    if (hparse_errno > 0) return;
    return;
  }
}

/*
  When we see BEGIN at start of a block, we ++begin_count.
  When we see END that matches a BEGIN, we --begin_count.
  So "begin_count > 00" means we're within BEGIN ... END.
  Warning: CREATE PROCEDURE p() WHILE 0 > 1 ... doesn't
           get counted, this only is for BEGIN ... END.
           Use hparse_f_flow_count for knowing that.
  Knowing this is useful because if we're not within
  BEGIN ... END there can't be any relevant SQL/PSM DECLAREs.
  For plsql, we also flag IS|AS (but it goes off after the
  last END is seen), we can use it to know if SQLCODE is legal.
*/
bool MainWindow::hparse_f_is_in_compound()
{
  if (hparse_as_seen) return true;
  if (hparse_begin_count == 0) return false;
  return true;
}

/* Todo: it's a shame we must say "identifier" when we know it's keyword */
int MainWindow::hparse_f_plsql_condition(int block_top)
{
  if ((hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_CONDITION_REFER,TOKEN_TYPE_IDENTIFIER, "DUP_VAL_ON_INDEX") == 1)
   || (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_CONDITION_REFER,TOKEN_TYPE_IDENTIFIER, "NO_DATA_FOUND") == 1)
   || (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_CONDITION_REFER,TOKEN_TYPE_IDENTIFIER, "INVALID_CURSOR") == 1)
   || (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_CONDITION_REFER,TOKEN_TYPE_IDENTIFIER, "OTHERS") == 1)
   || (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_CONDITION_REFER,TOKEN_TYPE_IDENTIFIER, "TOO_MANY_ROWS") == 1))
  {
    return main_token_types[hparse_i_of_last_accepted];
  }
  if (hparse_f_find_define(block_top,TOKEN_REFTYPE_CONDITION_DEFINE, TOKEN_REFTYPE_CONDITION_REFER, false) > 0)
    return TOKEN_TYPE_IDENTIFIER;
  return 0;
}

void MainWindow::hparse_f_declare(int calling_statement_type, int block_top)
{
  if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONTINUE") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "EXIT") == 1)
   || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "UNDO") == 1))
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "HANDLER");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR");
    if (hparse_errno > 0) return;
    do
    {
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQLSTATE") == 1)
      {
        hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALUE");
        if (hparse_f_literal(TOKEN_REFTYPE_SQLSTATE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQLWARNING") == 1) {;}
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "NOT") == 1)
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOUND");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQLEXCEPTION") == 1) {;}
      else if (hparse_f_find_define(block_top, TOKEN_REFTYPE_CONDITION_DEFINE, TOKEN_REFTYPE_CONDITION_REFER, false) > 0) {;}
      else
      {
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
      }
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    hparse_f_block(calling_statement_type, block_top);
    return;
  }
  int identifier_count= 0;
  bool condition_seen= false;
  bool cursor_seen= false;
  do
  {
    hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_CONDITION_OR_CURSOR,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    int hparse_i_of_identifier= hparse_i_of_last_accepted;
    if (hparse_errno > 0) return;
    ++identifier_count;
    if ((identifier_count == 1) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CONDITION") == 1))
    {
      main_token_reftypes[hparse_i_of_identifier]= TOKEN_REFTYPE_CONDITION_DEFINE;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SQLSTATE") == 1)
      {
        hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VALUE");
        if (hparse_f_literal(TOKEN_REFTYPE_SQLSTATE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
      }
      else
      {
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
      }
      if (hparse_errno > 0) return;
      condition_seen= true;
      break;
    }
    if ((identifier_count == 1) && (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURSOR") == 1))
    {
      main_token_reftypes[hparse_i_of_identifier]= TOKEN_REFTYPE_CURSOR_DEFINE;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "FOR");
      if (hparse_errno > 0) return;
      if (hparse_f_query(0, false, false, false) == 0)
      {
        hparse_f_error();
        return;
      }
      cursor_seen= true;
    }
    else
    {
      main_token_reftypes[hparse_i_of_identifier]= TOKEN_REFTYPE_VARIABLE_DEFINE;
    }
  } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  if (condition_seen == true) {;}
  else if (cursor_seen == true) {;}
  else
  {
    if (hparse_f_data_type(TOKEN_KEYWORD_DECLARE) == -1) hparse_f_error();
    if (hparse_errno > 0) return;
    if (hparse_f_default_clause(TOKEN_KEYWORD_DECLARE) == 1)
    {
      if (hparse_errno > 0) return;
    }
  }
  hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";");
  if (hparse_errno > 0) return;
}


/*
  Oracle mode, variable declares. E.g.  DECLARE a INT; b VARCHAR(10);
  In compounds it's DECLARE variable-declares BEGIN
  In routines it's IS|AS variable-declares AS BEGIN ...
  In packages it's variable-declares PROCEDURE|FUNCTION ...
  Regrettably, FUNCTION and END are not reserved words.
  Todo: I'm not sure that the check for xxx%type and xxx%rowtype
        should be in here rather than in hparse_f_data_type.
        It depends whether we use such things outside variable-declares.
        !! No. It's bad. We call hparse_f_data_type twice from here!
*/
int MainWindow::hparse_f_declare_plsql(int token_type)
{
  if (token_type == TOKEN_KEYWORD_PACKAGE)
  {
    for (;;)
    {
      if (hparse_f_accept(FLAG_VERSION_PLSQL | FLAG_VERSION_MARIADB_11_4, TOKEN_REFTYPE_VARIABLE_DEFINE,TOKEN_KEYWORD_FUNCTION, "FUNCTION") == 1)
        return TOKEN_KEYWORD_FUNCTION;
      else if (hparse_f_accept(FLAG_VERSION_PLSQL | FLAG_VERSION_MARIADB_11_4, TOKEN_REFTYPE_VARIABLE_DEFINE,TOKEN_KEYWORD_PROCEDURE, "PROCEDURE") == 1)
        return TOKEN_KEYWORD_PROCEDURE;
      else if (((hparse_dbms_mask & FLAG_VERSION_PLSQL) == 0) &&  (hparse_f_accept(FLAG_VERSION_MARIADB_11_4, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "DECLARE") == 1))
      {
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_CONDITION_OR_CURSOR,TOKEN_TYPE_IDENTIFIER, "[identifier]");
        int hparse_i_of_identifier= hparse_i_of_last_accepted;
        if (hparse_errno > 0) return -1;
        main_token_reftypes[hparse_i_of_identifier]= TOKEN_REFTYPE_VARIABLE_DEFINE;
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DECLARE;
        if (hparse_f_data_type(TOKEN_KEYWORD_DECLARE) == -1) hparse_f_error();
        if (hparse_errno > 0) return -1;
        if (hparse_f_default_clause(TOKEN_KEYWORD_DECLARE) == 1)
        {
          if (hparse_errno > 0) return -1;
        }
        hparse_f_expect(FLAG_VERSION_MARIADB_11_4, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";");
        if (hparse_errno > 0) return -1;
      }
      else if (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_VARIABLE_DEFINE,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
      {
        if (hparse_f_data_type(TOKEN_KEYWORD_DECLARE) == -1) hparse_f_error();
        if (hparse_errno > 0) return -1;
        hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";");
        if (hparse_errno > 0) return -1;
      }
      else hparse_f_error();
      if (hparse_errno > 0) return -1;
    }
  }
  /* We won't reach here if token_type == TOKEN_KEYWORD_PACKAGE. */
  int i;
  for (i= 0;; ++i)
  {
    if ((i != 0) || (token_type == TOKEN_KEYWORD_AS))
    {
      if (QString::compare(hparse_token, "BEGIN", Qt::CaseInsensitive) == 0) return 0;
      /* guaranteed to fail */
      hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BEGIN");
    }
    if (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CURSOR") == 1)
    {
      hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_CURSOR_DEFINE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) goto e;
      hparse_f_parameter_list(TOKEN_KEYWORD_CURSOR);
      if (hparse_errno > 0) goto e;
      if (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "IS") == 0)
        hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "AS");
      if (hparse_errno > 0) goto e;
      if (hparse_f_query(0, false, false, false) == 0)
      {
        hparse_f_error();
        goto e;
      }
    }
    else
    {
      hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_VARIABLE_DEFINE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) goto e;
      int prev_i_of_last_accepted= hparse_i_of_last_accepted;
      if (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_EXCEPTION, "EXCEPTION") == 1)
      {
        main_token_reftypes[prev_i_of_last_accepted]= TOKEN_REFTYPE_CONDITION_DEFINE;
      }
      else
      {
        if (hparse_f_data_type(TOKEN_KEYWORD_DECLARE) == -1)
        {
          if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE, TOKEN_REFTYPE_TABLE) == 0)
            hparse_f_error();
          if (hparse_errno > 0) goto e;
          /* What Oracle calls an "attribute" indicator */
          hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "%");
          if (hparse_errno > 0) goto e;
          if (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ATTRIBUTE,TOKEN_TYPE_KEYWORD, "TYPE") == 0)
            hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ATTRIBUTE,TOKEN_TYPE_KEYWORD, "ROWTYPE");
          if (hparse_errno > 0) goto e;
          main_token_types[hparse_i_of_last_accepted]= TOKEN_TYPE_IDENTIFIER;
        }
        if (hparse_errno > 0) goto e;
        if (hparse_f_default_clause(TOKEN_KEYWORD_DECLARE) == 1)
        {
          if (hparse_errno > 0) goto e;
        }
      }
    }
e:  if (hparse_errno > 0)
      if (hparse_f_recover_if_error(false, "AS") == 2) return 0;
    hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";");
    if (hparse_errno > 0) return 0;
    /* This flag is checked in is_statement_complete() */
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_PLSQL_DECLARE_SEMICOLON;
  }
  return 0;
}

/*
  Recovery
  Sometimes parse errors happen because we missed some new|unknown
  MySQL/MariaDB syntax. Since the debugger depends on hparse now, we'll
  try to recover (and may as well do so even if it's not debugger), thus:
  In hparse_f_block(), if hparse_f_statement() fails, or if
  hparse_f_declare() fails, instead of return with error, call
  hparse_f_recover_if_error() and proceed. In hparse_f_recover_if_error()
  we skip ahead to the next ";" (or next delimiter) and carry on.
  (We don't carry on if hparse_f_recover_if_error() returns 2, though.)
  (Semicolon is always a stopper, we might pass an extra one e.g. THEN.)
  Todo: There are other spots inside hparse_f_block() where we could try
        to recover: e.g. in ITERATE or RETURN statements. But failure to
        see "END" is not recoverable.
  Todo: maybe hparse_f_recover_if_error() should handle all hparse_f_opr_1() fails
        especially failure of hparse_f_opr_1() in a loop condition so
        skip to end of the operand not to the next ";"
  Todo: The error message must refer to the first error, right now we've
        got it pointing to the last error.
  Todo: there might be references to functions that we could try to
        parse anyway -- missing token_reftype is bad for debugging.
        So the skipping should be: "if identifier followed by ( ...
        else if ; ... else anything else" but you have to be sure
        it's not "create table x (" and maybe other false markers exist
        e.g.
          hparse_f_accept("[identifier]")
          else hparse_f_accept("[literal]")
          else hparse_f_accept(';")
          else
            main_token_reftypes[hparse_i]= reftype;
            hparse_expected= "";
            hparse_i_of_last_accepted= hparse_i;
            hparse_f_nexttoken();
            ++hparse_count_of_accepts;
  Todo: save the error count, it is needed later for "Warning: some statements skipped".
        at the end, you still have to return an error, or errors  Todo: document this behaviour
  Todo: this doesn't mean you can get away with failing to recognize good syntax forever
  Todo: ideally we could have recoveries inside individual statements
  Warning: We can get an error at ";" because it comes too early.
           To avoid an ugly loop where we never move past the ";",
           call with eat_the_stopper == true if you were expecting
           to eat a semicolon anyway.
           We should try to think of a real solution.
  Todo: If you are calling from inside a loop, you will never get
        out of the loop. That's why we should have a comparison whether
        we've called twice with the same hparse_i value.
        We should return 2 if it happens.
  Todo: The extra stopper might not really end the phrase:
        THEN might be in a CASE expression, DO might be a variable,
        IS might be part of IS NULL within a subquery.
        I'm hoping that's rare, but we could try to catch the rare stuff.
*/
int MainWindow::hparse_f_recover_if_error(bool eat_the_stopper, QString extra_stopper)
{
  if (hparse_errno == 0) return 0;
  QString token;
  int i;
  for (i= hparse_i;; ++i)
  {
    if (main_token_lengths[i] == 0) return 2;
    token= hparse_text_copy.mid(main_token_offsets[i], main_token_lengths[i]);
    {
      if ((token == ";")
       || ((extra_stopper != "") && (token.toUpper() == extra_stopper))
       || (token.toUpper() == "\\G")
       || (token == hparse_delimiter_str))
        break;
    }
  }
  if (main_token_lengths[i + 1] == 0) return 2;
  hparse_errno= 0;
  ++hparse_errno_count;
  hparse_i= i - 1;
  hparse_token= hparse_prev_token= "";
  hparse_f_nexttoken(); /* so we're pointing at ";" again, I hope */
  if (eat_the_stopper)
  {
    if ((extra_stopper != "") && (token.toUpper() == extra_stopper))
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, extra_stopper);
    else
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";");
    /* a failure would mean we hit a delimiter which is not ; */
    if (hparse_errno > 0) return 2;
  }
  return 0;
}


#ifdef DBMS_TARANTOOL
/*
  From the Lua bnf https://www.lua.org/manual/5.1/manual.html
*/
/*
  stat ::=  varlist `=´ explist |
  functioncall |
  do block end |
  while exp do block end |
  repeat block until exp |
  if exp then block {elseif exp then block} [else block] end |
  for Name `=´ exp `,´ exp [`,´ exp] do block end |
  for namelist in explist do block end |
  function funcname funcbody |
  local function Name funcbody |
  local namelist [`=´ explist]
*/
/* TODO: We haven't paid much attention to TOKEN_REFTYPE */
static int lua_calling_statement_type, lua_block_top;
static int lua_depth;

void MainWindow::hparse_f_lua_blocklist(int calling_statement_type, int block_top)
{
  log("hparse_f_lua_blocklist start", 80);
  bool saved_hparse_sql_mode_ansi_quotes= hparse_sql_mode_ansi_quotes;
  hparse_sql_mode_ansi_quotes= false;
  int saved_hparse_i= hparse_i;
  unsigned int saved_hparse_dbms_mask= hparse_dbms_mask;
  lua_depth= 0;
  hparse_dbms_mask= FLAG_VERSION_LUA;
  hparse_f_lua_blockseries(calling_statement_type, block_top, false);
  hparse_dbms_mask= saved_hparse_dbms_mask;
  if (hparse_errno > 0) goto e;
  main_token_flags[saved_hparse_i]|= TOKEN_FLAG_IS_LUA;
e:hparse_sql_mode_ansi_quotes= saved_hparse_sql_mode_ansi_quotes;
  log("hparse_f_lua_blocklist end", 80);
}
/* 0 or more statements or blocks of statements, optional semicolons */
void MainWindow::hparse_f_lua_blockseries(int calling_statement_type, int block_top, bool is_in_loop)
{
  int statement_type;
  ++lua_depth;
  for (;;)
  {
    statement_type= hparse_f_lua_block(calling_statement_type, block_top, is_in_loop);
    if (statement_type == 0) break;
    assert(lua_depth >= 0);
    /* todo: if "break" or "return", can anything follow? */
    /* This kludge occurs more than once. */
    if ((hparse_prev_token != ";") && (hparse_prev_token != hparse_delimiter_str))
    {
      hparse_f_semicolon_and_or_delimiter(calling_statement_type);
      if (hparse_errno > 0) return;
    }
    if (hparse_prev_token == hparse_delimiter_str)
    {
      if (hparse_delimiter_str != ";") return;
      if (lua_depth == 1) return;
    }
  }
  --lua_depth;
}
int MainWindow::hparse_f_lua_block(int calling_statement_type, int block_top, bool is_in_loop)
{
  lua_calling_statement_type= calling_statement_type;
  lua_block_top= block_top;
  if (hparse_errno > 0) return 0;
  hparse_subquery_is_allowed= false;
  int hparse_i_of_block= -1;
  if ((is_in_loop == true)
   && (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BREAK, "break") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    return TOKEN_KEYWORD_BREAK;
  }
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DO_LUA, "do") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    hparse_f_lua_blockseries(calling_statement_type, block_top, is_in_loop);
    if (hparse_errno > 0) return 0;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "end");
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_DO;
  }
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FOR, "for") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno != 0) return 0;
    if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1)
    {
      if (hparse_f_lua_namelist() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IN, "in");
      if (hparse_errno != 0) return 0;
    }
    else
    {
      hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_errno != 0) return 0;
    }
    if (hparse_f_lua_explist() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_FUNCTION,TOKEN_KEYWORD_DO, "do");
    if (hparse_errno > 0) return 0;
    hparse_f_lua_blockseries(calling_statement_type, block_top, true);
    if (hparse_errno > 0) return 0;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "end");
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_FOR;
  }
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FUNCTION, "function") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    if (hparse_f_lua_funcname() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    if (hparse_f_lua_funcbody() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_FUNCTION;
  }
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_GOTO, "goto") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_LABEL_REFER, TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_GOTO;
  }
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF, "if") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    main_token_flags[hparse_i_of_last_accepted] &= (~TOKEN_FLAG_IS_FUNCTION);
    for (;;)
    {
      hparse_subquery_is_allowed= true;
      hparse_f_lua_exp();
      if (hparse_errno > 0) return 0;
      hparse_subquery_is_allowed= false;
      /* if (hparse_errno > 0) return 0; */ /* identicalConditionAfterEarlyExit */
      hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "then");
      if (hparse_errno > 0) return 0;
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
      int break_word= 0;
      hparse_f_lua_blockseries(calling_statement_type, block_top, is_in_loop);
      if (hparse_errno > 0) return 0;
      if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "end") == 1)
      {
        break_word= TOKEN_KEYWORD_END;
      }
      else if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "elseif") == 1)
      {
        break_word= TOKEN_KEYWORD_ELSEIF;
      }
      else if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "else") == 1)
      {
        break_word= TOKEN_KEYWORD_ELSE;
      }
      else
      {
        hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      if (break_word == TOKEN_KEYWORD_END)
      {
        break;
      }
      if (break_word == TOKEN_KEYWORD_ELSEIF)
      {
        main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
        continue;
      }
      assert(break_word == TOKEN_KEYWORD_ELSE);
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
      hparse_f_lua_blockseries(calling_statement_type, block_top, is_in_loop);
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "end");
      if (hparse_errno > 0) return 0;
      break;
    }
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    main_token_pointers[hparse_i_of_last_accepted]= hparse_i_of_block;
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_IF;
  }
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LOCAL, "local") == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FUNCTION, "function") == 1)
    {
      if (hparse_f_lua_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      if (hparse_f_lua_funcbody() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else
    {
      if (hparse_f_lua_namelist() == 0) hparse_f_error();
      if (hparse_errno != 0) return 0;
      if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=") == 1)
      {
        if (hparse_errno > 0) return 0;
        if (hparse_f_lua_explist() == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
    }
    return TOKEN_KEYWORD_LOCAL;
  }
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REPEAT, "repeat") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    hparse_f_lua_blockseries(calling_statement_type, block_top, true);
    if (hparse_errno > 0) return 0;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "until");
    if (hparse_errno > 0) return 0;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    hparse_subquery_is_allowed= true;
    hparse_f_lua_exp();
    if (hparse_errno > 0) return 0;
    hparse_subquery_is_allowed= false;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    main_token_pointers[hparse_i_of_last_accepted]= hparse_i_of_block;
    /* if (hparse_errno > 0) return 0; */ /* identicalConditionAfterEarlyExit */
    return TOKEN_KEYWORD_REPEAT;
  }
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_RETURN, "return") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    hparse_subquery_is_allowed= true;
    hparse_f_lua_explist();
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_RETURN;
  }
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WHILE, "while") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    if (hparse_i_of_block == -1) hparse_i_of_block= hparse_i_of_last_accepted;
    hparse_subquery_is_allowed= true;
    hparse_f_lua_exp();
    if (hparse_errno > 0) return 0;
    hparse_subquery_is_allowed= false;
    /* if (hparse_errno > 0) return 0; */ /* identicalConditionAfterEarlyExit */
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DO, "do");
    if (hparse_errno > 0) return 0;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    hparse_f_lua_blockseries(calling_statement_type, block_top, true);
    if (hparse_errno > 0) return 0;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_END, "end");
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    main_token_pointers[hparse_i_of_last_accepted]= hparse_i_of_block;
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_WHILE;
  }
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "::") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_LABEL_DEFINE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return 0;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "::");
    if (hparse_errno > 0) return 0;
    return TOKEN_TYPE_OPERATOR;
  }
  int result_of_functioncall= hparse_f_lua_functioncall();
  if (hparse_errno > 0) return 0;
  if (result_of_functioncall == 1)
  {
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
    if (hparse_errno > 0) return 0;
    if (hparse_f_lua_explist() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_DECLARE;
  }
  if (result_of_functioncall == 2)
  {
    return TOKEN_KEYWORD_CALL;
  }
  /* todo: hparse_f_statement will fail because of hparse_dbms_mask */
  //hparse_f_statement(block_top);
  //if (hparse_errno > 0) return 0;
  return 0;
}
/* funcname ::= Name {`.´ Name} [`:´ Name] */
int MainWindow::hparse_f_lua_funcname()
{
  do
  {
    if (hparse_f_lua_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
  } while (hparse_f_lua_accept_dotted(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ".") == 1);
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ":") == 1)
  {
    if (hparse_f_lua_name() == 0) hparse_f_error();
  }
  return 1;
}
/* varlist ::= var {`,´ var} */
int MainWindow::hparse_f_lua_varlist()
{
  do
  {
    if (hparse_f_lua_var() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
  }
  while (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ",") == 1);
  return 1;
}
/* var ::=  Name | prefixexp `[´ exp `]´ | prefixexp `.´ Name */
int MainWindow::hparse_f_lua_var()
{
  if ((hparse_f_lua_name() == 1)
   || (hparse_f_lua_name_equivalent() == 1))
  {
    if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "[") == 1)
    {
      hparse_f_lua_exp();
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "]");
      if (hparse_errno > 0) return 0;
    }
    if (hparse_f_lua_accept_dotted(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ".") == 1)
    {
      if (hparse_f_lua_var() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      return 1;
    }
    return 1;
  }
  return 0;
}
/* namelist ::= Name {`,´ Name} */
int MainWindow::hparse_f_lua_namelist()
{
  do
  {
    if (hparse_f_lua_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
  } while (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
  return 1;
}
/* explist ::= {exp `,´} exp */
int MainWindow::hparse_f_lua_explist()
{
  do
  {
    if (hparse_f_lua_exp() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
  } while (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
   return 1;
}
/*
  exp ::=  nil | false | true | Number | String | `...´ | function |
           prefixexp | tableconstructor | exp binop exp | unop exp
*/
int MainWindow::hparse_f_lua_exp()
{
  //if (hparse_f_lua_prefixexp() == 1) return 1;
  //if (hparse_errno > 0) return 0;
  hparse_f_lua_opr_1(0);
  if (hparse_errno > 0) return 0;
  return 1;
}
/* prefixexp ::= var | functioncall | `(´ exp `)´ */
/* todo: this is never called. remove? */
int MainWindow::hparse_f_lua_prefixexp()
{
  if (hparse_f_lua_var() == 1) return 1;
  if (hparse_errno > 0) return 0;
  if (hparse_f_lua_functioncall() == 1) return 1;
  if (hparse_errno > 0) return 0;
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "(") == 0)
  {
    if (hparse_f_lua_exp() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return 0;
  }
  return 0;
}
/* functioncall ::=  prefixexp args | prefixexp `:´ Name args */
/*
  The return differs from other hparse_f_lua functions.
  Return: 0 neither, 1 var, not function, 2 function
*/
int MainWindow::hparse_f_lua_functioncall()
{
  bool is_var;
  if (hparse_f_lua_var() == 0)
  {
    return 0;
  }
  if (hparse_errno > 0) return 0;
so_far_it_is_a_var:
  if (hparse_f_lua_args() == 1) goto so_far_it_is_a_functioncall;
  if (hparse_errno > 0) return 0;
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ":") == 1)
  {
    if (hparse_f_lua_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    if (hparse_f_lua_args() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    goto so_far_it_is_a_functioncall;
  }
  return 1;
so_far_it_is_a_functioncall:
  is_var= false;
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "[") == 1)
  {
    hparse_f_lua_exp();
    if (hparse_errno > 0) return 0;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "]");
    if (hparse_errno > 0) return 0;
    is_var= true;
  }
  if (hparse_f_lua_accept_dotted(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ".") == 1)
  {
    if (hparse_f_lua_var() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    is_var= true;
  }
  if (is_var) goto so_far_it_is_a_var;
  return 2;
}
/* args ::=  `(´ [explist] `)´ | tableconstructor | String */
int MainWindow::hparse_f_lua_args()
{
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "(") == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ")") == 0)
    {
      hparse_f_lua_explist();
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return 0;
    }
    return 1;
  }
  if (hparse_f_lua_tableconstructor() == 1) return 1;
  if (hparse_errno > 0) return 0;
  if (hparse_f_lua_string() == 1) return 1;
  return 0;
}
/* function ::= function funcbody */
int MainWindow::hparse_f_lua_function()
{
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_FUNCTION, "function") == 1)
  {
    if (hparse_f_lua_funcbody() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    return 1;
  }
  return 0;
}
/* funcbody ::= `(´ [parlist] `)´ block end */
int MainWindow::hparse_f_lua_funcbody()
{
  hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return 0;
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ")") == 0)
  {
    hparse_f_lua_parlist();
    if (hparse_errno > 0) return 0;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return 0;
  }
  hparse_f_lua_blockseries(lua_calling_statement_type, lua_block_top, false);
  if (hparse_errno > 0) return 0;
  hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_END, "end");
  if (hparse_errno > 0) return 0;
  return 1;
}
/* parlist ::= namelist [`,´ `...´] | `...´ */
int MainWindow::hparse_f_lua_parlist()
{
  return hparse_f_lua_namelist();
}
/* tableconstructor ::= `{´ [fieldlist] `}´ */
int MainWindow::hparse_f_lua_tableconstructor()
{
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "{") == 0)
    return 0;
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "}") == 0)
  {
    if (hparse_f_lua_fieldlist() == 0) hparse_f_error();
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "}");
    if (hparse_errno > 0) return 0;
  }
  return 1;
}
/* fieldlist ::= field {fieldsep field} [fieldsep] */
int MainWindow::hparse_f_lua_fieldlist()
{
  do
  {
    hparse_f_lua_field();
    if (hparse_errno > 0) return 0;
  }
  while (hparse_f_lua_fieldsep() == 1);
  if (hparse_errno > 0) return 0;
  return 1;
}
/* field ::= `[´ exp `]´ `=´ exp | Name `=´ exp | exp */
int MainWindow::hparse_f_lua_field()
{
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "[") == 1)
  {
    if (hparse_f_lua_exp() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "]");
    if (hparse_errno > 0) return 0;
    return 1;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "=");
    if (hparse_errno > 0) return 0;
    if (hparse_f_lua_exp() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    return 1;
  }
  hparse_f_next_nexttoken();
  if (hparse_next_token == "=")
  {
    if (hparse_f_lua_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "=");
    if (hparse_errno > 0) return 0;
    hparse_f_lua_exp();
    if (hparse_errno > 0) return 0;
    return 1;
  }
  if (hparse_f_lua_exp() == 1) return 1;
  return 0;
}
/* fieldsep ::= `,´ | `;´ */
int MainWindow::hparse_f_lua_fieldsep()
{
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ",") == 1) return 1;
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ";") == 1) return 1;
  return 0;
}
/* Name = "any string of letters, digits, and underscores, not beginning with a digit. */
int MainWindow::hparse_f_lua_name()
{
  return hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_IDENTIFIER, "[identifier]");
}
/* (exp).name and (exp)[x] are variables, but (exp) is not a name */
int MainWindow::hparse_f_lua_name_equivalent()
{
  int i= hparse_i;
  QString token;
  token= hparse_text_copy.mid(main_token_offsets[i], main_token_lengths[i]);
  if (token != "(")
  {
    hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "(");
    return 0;
  }
  int parentheses_count= 0;
  for (;;)
  {
    token= hparse_text_copy.mid(main_token_offsets[i], main_token_lengths[i]);
    if (token == "(") ++parentheses_count;
    if (token == ")")
    {
      --parentheses_count;
      if (parentheses_count == 0) break;
    }
    if (token == "") break;
    ++i;
  }
  token= hparse_text_copy.mid(main_token_offsets[i + 1], main_token_lengths[i + 1]);
  if ((token != "[") && (token != ".") && (token != "")) return 0;
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "(") == 1)
  {
    hparse_f_lua_exp();
    if (hparse_errno > 0) return 0;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return 0;
    return 1;
  }
  return 0;
}
/* Number ::= decimals and approximates ok. todo: 0xff */
int MainWindow::hparse_f_lua_number()
{
  if (main_token_types[hparse_i] == TOKEN_TYPE_LITERAL_WITH_DIGIT)
  {
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_LUA, TOKEN_LITERAL_FLAG_NUMBER) == 0) hparse_f_error(); /* guaranteed to succeed */
    if (hparse_errno > 0) return 0;
    return 1;
  }
  hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "[number]"); /* guaranteed to fail */
  return 0;
}
/* String :: = in 's or "s or (todo:) [[...]]s or [==...]==]s */
/* todo: merge with hparse_f_literal() */
int MainWindow::hparse_f_lua_string()
{
  if ((main_token_types[hparse_i] == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
   || (main_token_types[hparse_i] == TOKEN_TYPE_LITERAL_WITH_BRACKET)
   || (main_token_types[hparse_i] == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE))
  {
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "[literal]"); /* guaranteed to succeed */
    if (hparse_errno > 0) return 0;
    return 1;
  }
  hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "[string]"); /* guaranteed to fail */
  return 0;
}
/* todo: merge with hparse_f_literal() */
int MainWindow::hparse_f_lua_literal()
{
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_NIL, "nil") == 1) return 1;
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_FALSE, "false") == 1) return 1;
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_NIL, "true") == 1) return 1;
  if (hparse_f_lua_number() == 1) return 1;
  if (hparse_errno > 0) return 0;
  if (hparse_f_lua_string() == 1) return 1;
  if (hparse_errno > 0) return 0;
  return 0;
}

void MainWindow::hparse_f_lua_opr_1(int who_is_calling) /* Precedence = 1 (bottom) */
{
  hparse_f_lua_opr_2(who_is_calling);
}

void MainWindow::hparse_f_lua_opr_2(int who_is_calling) /* Precedence = 2 */
{
  hparse_f_lua_opr_3(who_is_calling);
  if (hparse_errno > 0) return;
  while (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "or") == 1)
  {
    hparse_f_lua_opr_3(who_is_calling);
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_lua_opr_3(int who_is_calling) /* Precedence = 3 */
{
  hparse_f_lua_opr_4(who_is_calling);
}

void MainWindow::hparse_f_lua_opr_4(int who_is_calling) /* Precedence = 4 */
{
  hparse_f_lua_opr_5(who_is_calling);
  if (hparse_errno > 0) return;
  while (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "and") == 1)
  {
    hparse_f_lua_opr_5(who_is_calling);
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_lua_opr_5(int who_is_calling) /* Precedence = 5 */
{
  hparse_f_lua_opr_6(who_is_calling);
}

void MainWindow::hparse_f_lua_opr_6(int who_is_calling) /* Precedence = 6 */
{
  hparse_f_lua_opr_7(who_is_calling);
}

/* Most comp-ops can be chained e.g. "a <> b <> c", but not LIKE or IN. */
void MainWindow::hparse_f_lua_opr_7(int who_is_calling) /* Precedence = 7 */
{
  if (hparse_f_is_equal(hparse_token, "(")) hparse_f_lua_opr_8(who_is_calling, ALLOW_FLAG_IS_MULTI);
  else hparse_f_lua_opr_8(who_is_calling, 0);
  if (hparse_errno > 0) return;
  for (;;)
  {
    if ((hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "<") == 1)
     || (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ">") == 1)
     || (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "<=") == 1)
     || (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ">=") == 1)
     || (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "~=") == 1)
     || (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "==") == 1))
    {
      if (hparse_f_is_equal(hparse_token, "(")) hparse_f_lua_opr_8(who_is_calling, ALLOW_FLAG_IS_MULTI);
      else hparse_f_lua_opr_8(who_is_calling, 0);
      if (hparse_errno > 0) return;
      continue;
    }
    break;
  }
}

void MainWindow::hparse_f_lua_opr_8(int who_is_calling, int allow_flags) /* Precedence = 8 */
{
  if (hparse_errno > 0) return;
  hparse_f_lua_opr_9(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while (hparse_f_lua_accept_dotted(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "..") == 1)
  {
    hparse_f_lua_opr_9(who_is_calling, allow_flags);
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_lua_opr_9(int who_is_calling, int allow_flags) /* Precedence = 9 */
{
  hparse_f_lua_opr_10(who_is_calling, allow_flags);
}

void MainWindow::hparse_f_lua_opr_10(int who_is_calling, int allow_flags) /* Precedence = 10 */
{
  hparse_f_lua_opr_11(who_is_calling, allow_flags);
}

void MainWindow::hparse_f_lua_opr_11(int who_is_calling, int allow_flags) /* Precedence = 11 */
{
  hparse_f_lua_opr_12(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "-") == 1) || (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "+") == 1))
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_BINARY_PLUS_OR_MINUS;
    hparse_f_lua_opr_12(who_is_calling, 0);
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_lua_opr_12(int who_is_calling, int allow_flags) /* Precedence = 12 */
{
  if (hparse_errno > 0) return;
  hparse_f_lua_opr_13(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "*") == 1)
   || (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "/") == 1)
   || (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "%") == 1))
  {
    hparse_f_lua_opr_13(who_is_calling, 0);
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_lua_opr_13(int who_is_calling, int allow_flags) /* Precedence = 13 */
{
  hparse_f_lua_opr_14(who_is_calling, allow_flags);
}

void MainWindow::hparse_f_lua_opr_14(int who_is_calling, int allow_flags) /* Precedence = 14 */
{
  if ((hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "-") == 1)
   || (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "not") == 1)
   || (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "#") == 1))
  {
    hparse_f_lua_opr_15(who_is_calling, 0);
  }
  else hparse_f_lua_opr_15(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_lua_opr_15(int who_is_calling, int allow_flags) /* Precedence = 15 */
{
  hparse_f_lua_opr_16(who_is_calling, allow_flags);
  if (hparse_errno > 0) return;
  while (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "^") == 1)
  {
    hparse_f_lua_opr_16(who_is_calling, 0);
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_lua_opr_16(int who_is_calling, int allow_flags) /* Precedence = 16 */
{
  hparse_f_lua_opr_17(who_is_calling, allow_flags);
}

void MainWindow::hparse_f_lua_opr_17(int who_is_calling, int allow_flags) /* Precedence = 17 */
{
  hparse_f_lua_opr_18(who_is_calling, allow_flags);
}

/*
  Final level is operand.
  factor = identifier | number | "(" expression ")" .
*/
void MainWindow::hparse_f_lua_opr_18(int who_is_calling, int allow_flags) /* Precedence = 18, top */
{
  if (hparse_errno > 0) return;
  /* if we get 1 it's var, ok. if we get 2 it's functioncall, ok. */
  int nn= hparse_f_lua_functioncall();
  if (nn > 0) return;
  if (hparse_errno > 0) return;
  QString opd= hparse_token;

  int saved_hparse_i= hparse_i;
  hparse_f_next_nexttoken();
  if (hparse_next_token == "(")
  {
    if ((main_token_flags[hparse_i] & TOKEN_FLAG_IS_FUNCTION) != 0)
    {
      int saved_token= main_token_types[hparse_i];
      if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
      {
        hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "[reserved function]");
        if (hparse_errno > 0) return;
      }
      main_token_types[saved_hparse_i]= saved_token;
    }
  }
  if (hparse_f_lua_literal() == 1) return;
  if (hparse_errno > 0) return;
  if (hparse_f_lua_tableconstructor() == 1) return;
  if (hparse_errno > 0) return;
  if (hparse_f_lua_accept_dotted(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "..."))
    return;
  if (hparse_f_accept(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "("))
  {
    if (hparse_errno > 0) return;
    /* if subquery is allowed, check for "(SELECT ...") */
    if ((allow_flags & ALLOW_FLAG_IS_MULTI) != 0)
    {
      int expression_count= 0;
      hparse_f_parenthesized_multi_expression(&expression_count);
    }
    else hparse_f_lua_opr_1(who_is_calling);
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_LUA, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
    return;
  }
  /* functions as operands e.g. "f = function () x = x + 1 end" */
  if (hparse_f_lua_function() == 1) return;
  if (hparse_errno > 0) return;
  hparse_f_error();
  return;
}
/* tokenize() treats every "." as a separate token, work around that */
/* Todo: We could change tokenize() instead. */
int MainWindow::hparse_f_lua_accept_dotted(unsigned int flag_version, unsigned char reftype, int proposed_type, QString token)
{
  if (hparse_errno > 0) return 0;
  int i= hparse_i;
  int dots;
  if (hparse_text_copy.mid(main_token_offsets[i], 3) == "...") dots= 3;
  else if (hparse_text_copy.mid(main_token_offsets[i], 2) == "..") dots= 2;
  else if (hparse_text_copy.mid(main_token_offsets[i], 1) == ".") dots= 1;
  else dots= 0;
  if (dots == token.length())
  {
    if (dots >= 3) hparse_f_accept(flag_version, reftype,proposed_type, ".");
    if (hparse_errno > 0) return 0;
    if (dots >= 2) hparse_f_accept(flag_version, reftype,proposed_type, ".");
    if (hparse_errno > 0) return 0;
    return (hparse_f_accept(flag_version, reftype,proposed_type, "."));
  }

  /* these 2 lines are duplicated in hparse_f_accept() */
  if (hparse_expected > "") hparse_expected.append(" or ");
  hparse_expected.append(hparse_f_token_to_appendee(token, hparse_i, TOKEN_REFTYPE_ANY));
  return 0;
}
#endif

/*
  SQL/PSM label looks like "label:".
  PL/SQL label looks like "<<label>>".
  A label cannot be a reserved word.
  It is legal to have a space e.g. "label1 :".
  Todo: Check: where should statement start be if Oracle?
*/
QString MainWindow::hparse_f_label(int *hparse_i_of_block)
{
  QString label= "";
  if ((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0)
  {
    if (hparse_f_accept(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "<<") == 1)
    {
      label= hparse_token;
      main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_FUNCTION);
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_LABEL_DEFINE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      hparse_f_expect(FLAG_VERSION_PLSQL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ">>");
      if (hparse_errno > 0) return "";
    }
  }
  else
  {
    hparse_f_next_nexttoken();
    if (hparse_next_token == ":")
    {
      label= hparse_token;
      main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_FUNCTION);
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_LABEL_DEFINE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT | TOKEN_FLAG_IS_DEBUGGABLE;
      *hparse_i_of_block= hparse_i_of_last_accepted;
      if (hparse_errno > 0) return "";
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ":");
      if (hparse_errno > 0) return "";
    }
  }
  return label;
}

/*
  Some compound-statement items have definitions and references. E.g.
  DECLARE abc CURSOR ... OPEN abc ... We're looking at the "refer",
  we want to find the "define". We search backwards (going up in
  main_token_list until we hit the top) because later defines can hide
  earlier ones. Skip out-of-scope blocks (ENDs will have pointers to
  BEGINs etc.) During the search we might find candidates, i.e. it's
  a "define", and by passing them to hparse_f_accept we ensure
  hparse_expected.append() happens even if it's not a match
  so they're seen during hovering. If you go all the way up (which
  means there was no exact match) and there were no candidates to
  pass to hparse_f_accept, then call hparse_f_accept to ensure it's
  an identifier but don't force an error unless is_compulsory is true.
  (E.g. maybe it's not an error if cursor isn't found because it's in
  a calling routine? I forget.)
  Called for LEAVE|ITERATE|GOTO|CONTINUE|EXIT label.
  Called for OPEN|FETCH|CLOSE cursor.
  Called for SIGNAL | HANDLER FOR condition.
  Todo: Make sure elsewhere that TOKEN_KEYWORD_END is always legitimate.
  Todo: Try this for aliases (block_top would be statement|subquery start).
  Todo: This works poorly for `...``...` and "...""...". I suppose we'll
        succeed with hparse_f_accept(..."[identifier]") eventually but
        we won't match defines with refers (I guess).
*/
int MainWindow::hparse_f_find_define(int block_top,
                                      int reftype_define,
                                      int reftype_refer,
                                      bool is_compulsory)
{
  int i;
  for (i= hparse_i - 1; ((i >= 0) && (i >= block_top)); --i)
  {
    if (main_token_types[i] == TOKEN_KEYWORD_END)
    {
      int j= main_token_pointers[i];
      if ((j >= i) || (j < block_top)) break; /* should be an assert */
      i= main_token_pointers[i];
      continue;
    }
    if ((hparse_f_is_identifier(main_token_types[i]) == true)
     && (main_token_reftypes[i] == reftype_define))
    {
      QString s= hparse_text_copy.mid(main_token_offsets[i], main_token_lengths[i]);
      s= connect_stripper(s, true);
      if (hparse_token.left(1) == "`")
      {
        s= "`" + s;
        if (hparse_token.right(1) == "`") s= s + "`";
      }
      else if ((hparse_token.left(1) == "\"") && (hparse_sql_mode_ansi_quotes == true))
      {
        s= "\"" + s;
        if (hparse_token.right(1) == "\"") s= s + "\"";
      }
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, reftype_refer, TOKEN_TYPE_IDENTIFIER, s) == 1) return i;
    }
  }
  if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA_OUTPUT, reftype_refer, TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) return i;
  if (is_compulsory) hparse_f_error();
  return 0;
}


/*
  Called from hparse_f_block() for FETCH x cursor INTO variable.
  Also called just to see whether there's a parameter or variable
  definition for what's at hparse_i, in which case if mandatory==false.
  Search method is similar to the one in hparse_f_find_define(),
  but we go as far as statement start rather than block_top,
  because parameter declarations precede block top.
  But maybe it's not error if you can't find cursor definition?
  I forget whether that's somehow possible, so allow it.
  Todo: For a parameter, make sure it's an OUT parameter.
  TODO: Finding variables could be useful in lots more places.
  TODO: Check: what if there are 1000 variables, does anything overflow?
        (I guess I was thinking about how hparse_f_accept will append
        to hparse_expected for hovering purposes.)
  TODO: There is a flaw with PLSQL. We see END, we skip the prior
        BEGIN. But we don't skp a DECLARE that might precede the BEGIN.
        So we think variables are in scope, when they are not.
  TODO: The compare is case insensitive even if variable-name is quoted.
*/
int MainWindow::hparse_f_variables(int *i_of_define)
{
  *i_of_define= 0;
  for (int i= hparse_i - 1; ((i >= 0) && (i >= hparse_i_of_statement)); --i)
  {
    if (main_token_types[i] == TOKEN_KEYWORD_END)
    {
      int j= main_token_pointers[i];
      if ((j >= i) || (j < hparse_i_of_statement)) break; /* should be an assert */
      i= main_token_pointers[i];
      continue;
    }
    if (hparse_f_is_identifier(main_token_types[i]) == true)
    {
      if ((main_token_reftypes[i] == TOKEN_REFTYPE_VARIABLE_DEFINE)
       || (main_token_reftypes[i] == TOKEN_REFTYPE_PARAMETER_DEFINE))
      {
        QString s= hparse_text_copy.mid(main_token_offsets[i], main_token_lengths[i]);
        QString t= hparse_text_copy.mid(main_token_offsets[hparse_i], main_token_lengths[hparse_i]);
        s= connect_stripper(s, true);
        t= connect_stripper(t, true);
        if (QString::compare(s, t, Qt::CaseInsensitive) == 0)
        {
          *i_of_define= i;
          main_token_pointers[hparse_i]= i;
          if (main_token_reftypes[i] == TOKEN_REFTYPE_VARIABLE_DEFINE) return TOKEN_REFTYPE_VARIABLE_REFER;
          return TOKEN_REFTYPE_PARAMETER_REFER;
        }
      }
    }
  }
  return 0;
}


/*
  This is the top. This should be the main entry for parsing.
  A user might put more than one statement, or block of statements,
  on the statement widget before asking for execution.
  Re hparse_dbms_mask:
    We do check (though not always and not reliably) whether
    (hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0) before accepting | expecting,
    for example "role" will only be recognized if (hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0
    or FLAG_VERSION_MYSQL_8_0.
    We check version number too, but only major|minor, and carelessly.
    If we are connected, then the SELECT VERSION() result, which we stored in
    statement_edit_widget->dbms_version, will include the string "MariaDB".
    If we are not connected, the default is "mysql" but the user can start with
    ocelotgui --ocelot_dbms=mariadb, and we store that in ocelot_dbms.
    I think that set_dbms_version() handles all this nowadays.
  Todo: Typing "The rain in Spain falls mainly in the plain", then fiddling with edits at the end, is noticeably
        slow. It's almost certainly because the first word is an error and the number of choices for the first
        word of a statement is so great. Proof: typing "Select the rain in Spain falls mainly in the plain",
        then fiddling with edits at the end, is not noticeably slower. So perhaps come up with a special check
        for when what you're changing is long after the error and the word with the error hasn't changed.
*/
void MainWindow::hparse_f_multi_block(QString text)
{
  log("hparse_f_multi_block start", 90);

  /* Todo: decide whether this is a good time to move below the cursor, and decide whether clear() is necessary here */
  completer_widget->clear_wrapper();

  completer_widget->hide_wrapper();
  hparse_dbms_mask= dbms_version_mask;
  hparse_dbms_mask &= (~FLAG_VERSION_LUA_OUTPUT);
  hparse_sql_mode_ansi_quotes= sql_mode_ansi_quotes;
  hparse_i= -1;
  hparse_delimiter_str= ocelot_delimiter_str;
  hparse_errno_count= 0;
  hparse_specified_schema= ""; /* so far only "set ocelot_query = show erdiagram of schema_name ..." sets this */
  hparse_specified_list.clear(); /* this too is associated with erdiagram */
  for (;;)
  {
    hparse_statement_type= -1;
    hparse_errno= 0;
    hparse_text_copy= text;
    hparse_begin_count= 0; hparse_flow_count= 0;
    hparse_as_seen= false;
    hparse_like_seen= false;
    hparse_create_trigger_seen= false;
    // 2017-08-07: "hparse_token_type= 0;" caused a Lua statement
    //             followed by an SQL statement to fail. Star it out.
    //hparse_token_type= 0;
    hparse_next_token= "";
    hparse_next_next_token= "";
    hparse_next_next_next_token= "";
    hparse_next_next_next_next_token= "";
    hparse_next_token_type= 0;
    hparse_next_next_token_type= 0;
    hparse_next_next_next_token_type= 0;
    hparse_next_next_next_next_token_type= 0;
    hparse_prev_token= "";
    hparse_subquery_is_allowed= false;
    hparse_variable_is_allowed= false;
    hparse_variable_must_be_int= false;
    hparse_count_of_accepts= 0;
    hparse_i_of_last_accepted= 0;
    if (hparse_i == -1) hparse_f_nexttoken();
    hparse_i_of_statement= hparse_i;
    int hparse_f_client_statement_result= hparse_f_client_statement();
    if (hparse_f_client_statement_result > 0)
    {
      if ((main_token_lengths[hparse_i] == 0) && (hparse_f_client_statement_result != TOKEN_KEYWORD_SET))
      {
        log("hparse_f_multi_block end", 90);
        return; /* empty token marks end of input */
      }
      if ((hparse_prev_token != ";") && (hparse_prev_token != hparse_delimiter_str))
      {
        if ((hparse_f_semicolon_and_or_delimiter(0) == 0) && (hparse_f_client_statement_result == TOKEN_KEYWORD_SET)) hparse_f_error();
        if (hparse_errno > 0) goto error;
        if (main_token_lengths[hparse_i] == 0)
        {
          log("hparse_f_multi_block end", 90);
          return;
        }
      }
      continue; /* ?? rather than "return"? */
    }
    if (hparse_errno > 0) goto error;
#ifdef DBMS_MARIADB
    if ((hparse_dbms_mask & FLAG_VERSION_MARIADB_ALL) != 0)
    {
      hparse_f_block(0, hparse_i);
    }
    else
#endif
    {
#ifdef DBMS_TARANTOOL
      if (((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
       && (hparse_f_is_nosql(text) == true))
         tparse_f_block(0);
      else
#endif
      hparse_f_statement(hparse_i);
      if (hparse_errno > 0) goto error;
      /*
        Todo: we had trouble because some functions eat the final semicolon.
        The best thing would be to eat properly. Till then, we'll kludge:
        if we've just seen ";", don't ask for it again.
      */
      if ((hparse_prev_token != ";") && (hparse_prev_token != hparse_delimiter_str))
      {
        if (hparse_f_semicolon_and_or_delimiter(0) != 1) hparse_f_error();
      }
      if (hparse_errno > 0) goto error;
    }
    //hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "[eof]");
    if (hparse_errno > 0) goto error;
    if (hparse_i > 0) main_token_flags[hparse_i - 1]= (main_token_flags[hparse_i - 1] | TOKEN_FLAG_IS_BLOCK_END);
    log("hparse_f_multi_block end", 90);
    if (main_token_lengths[hparse_i] == 0) return; /* empty token marks end of input */
  }
  log("hparse_f_multi_block end", 90);
  return;
error:
  log("hparse_f_multi_block error", 90);
  QString expected_list;
  bool unfinished_comment_seen= false;
  bool unfinished_identifier_seen= false;
  if ((hparse_i == 0) && (main_token_lengths[0] == 0))
  {
    log("hparse_f_multi_block error return #1", 90);
    return;
  }
  /* Do not add to expecteds if we're still inside a comment */
  if ((hparse_i > 0) && (main_token_lengths[hparse_i] == 0))
  {
    int j= hparse_i - 1;
    if (main_token_types[j] == TOKEN_TYPE_COMMENT_WITH_SLASH)
    {
      QString token= hparse_text_copy.mid(main_token_offsets[j], main_token_lengths[j]);
      if (token.right(2) != "*/")
      {
        unfinished_comment_seen= true;
        if ((token.right(1) == "*") && (token != "/*")) expected_list= "Expecting: /";
        else expected_list= "Expecting: */";
      }
    }
    if ((main_token_types[j] == TOKEN_TYPE_COMMENT_WITH_MINUS)
     || (main_token_types[j] == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE))
    {
      QString rest= hparse_text_copy.mid(main_token_offsets[j]);
      if (rest.contains("\n") == false)
      {
        log("hparse_f_multi_block error return #2", 90);
        return;
      }
    }
  }
  /* Add different set of expecteds if we're still inside a quoted identifier */
  if ((main_token_types[hparse_i] == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK)
   || (main_token_types[hparse_i] == TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE))
  {
    QString token= hparse_text_copy.mid(main_token_offsets[hparse_i], main_token_lengths[hparse_i]);
    bool missing_termination= false;
    if (token.size() == 1) missing_termination= true;
    else if ((token.left(1) == "\"") && (token.right(1) != "\"")) missing_termination= true;
    else if ((token.left(1) == "`") && (token.right(1) != "`")) missing_termination= true;
    if (missing_termination == true)
    {
      ; /* This is the place where we used to have rehash_search(), which is now moved to hparse_f_expected_append */
    }
  }

  if ((unfinished_comment_seen == false) && (unfinished_identifier_seen == false))
  {
    // completer_widget->show_wrapper();
    expected_list= "Expecting: ";
    QString s_token;
    QString errmsg= hparse_errmsg;
    int word_start= errmsg.indexOf("tokens is: ") + 11;
    int word_end;
    for (;;)
    {
      word_end= errmsg.indexOf(" ", word_start);
      if (word_end == -1) word_end= errmsg.size();
      s_token= errmsg.mid(word_start, word_end - word_start);
      s_token= connect_stripper(s_token, false);
      if (s_token != "or")
      {
        if ((s_token.left(1) == "[")
         || (QString::compare(hparse_token, s_token.left(hparse_token.size()), Qt::CaseInsensitive) == 0))
        {
          /* This bit is to prevent saying the same token twice */
          QString s_token_2= s_token;
          s_token_2.append(" ");
          QString expected_list_2= expected_list.right(expected_list.size() - 11);
          if (expected_list_2.contains(s_token_2, Qt::CaseInsensitive) == false)
          {
            expected_list.append(s_token);
            expected_list.append(" ");
          }
        }
      }
      word_start= word_end + 1;
      if (word_start >= errmsg.size()) break;
    }
  }
  log("hparse_f_multi_block error end", 90);
}

#ifdef DBMS_TARANTOOL
/* Warning: We can call this without going via hparse_f_multi_block() */
bool MainWindow::hparse_f_is_nosql(QString text)
{
  QString s= text.mid(main_token_offsets[hparse_i], main_token_lengths[hparse_i]);
  if ((QString::compare(s, "SELECT", Qt::CaseInsensitive) == 0)
   || (QString::compare(s, "INSERT", Qt::CaseInsensitive) == 0)
   || (QString::compare(s, "DELETE", Qt::CaseInsensitive) == 0))
  {
    QString s= text.mid(main_token_offsets[hparse_i + 1], main_token_lengths[hparse_i + 1]);
    if ((s.left(2) == "/*") && (s.right(2) == "*/"))
    {
      s= s.mid(2, s.length() - 4).trimmed();
      if (QString::compare(s, "NOSQL", Qt::CaseInsensitive) == 0)
      {
        return true;
      }
    }
  }
  return false;
}
#endif

/*
  Pass: hparse_i and text are assumed good
  Return: i of start of statement-or-subquery that hparse_i is inside
  This is not the same as hparse_i_of_statement, if there are compound statements.
  We expect that statements are always flagged with TOKEN_FLAG_IS_START_STATEMENT, subqueries are not.
  Todo: is there a better way to check for a subquery start?
*/
int MainWindow::i_of_elementary_statement()
{
  int i;
  int parentheses_count= 0;
  for (i= hparse_i; i >= 0; --i)
  {
    if ((main_token_flags[i] & TOKEN_FLAG_IS_START_STATEMENT) != 0) break;
    if ((main_token_types[i] == TOKEN_KEYWORD_SELECT)
     || (main_token_types[i] == TOKEN_KEYWORD_WITH)
     || (main_token_types[i] == TOKEN_KEYWORD_VALUES))
    {
      if (parentheses_count <= 0)
      {
        int j= next_i(i, -1);
        if (hparse_text_copy.mid(main_token_offsets[j], main_token_lengths[j]) == "(") break;
      }
    }
    if (main_token_types[i] == TOKEN_TYPE_OPERATOR)
    {
      QString operator_token= hparse_text_copy.mid(main_token_offsets[i], main_token_lengths[i]);
      if (operator_token == ")")
      {
        ++parentheses_count;
      }
      if (operator_token == "(")
      {
        --parentheses_count;
      }
    }
  }
  return i;
}

/*
  hparse_f_pre_rehash_search() -- looking for a column's table.
  In hparse_f_multi_block() when we see errmsg has column identifier and we know rehash happened,
  we skip back to the start of the statement or subquery that contains hparse_i,
  then skip forward till we reach hparse_i, looking for a table name (if hparse_i might be at a column name).
  Ignore anything inside ()s.
  Return a table name. When we get to rehash_search, column names will be restricted to columns of the table.
  Restrictions:
    We only return one table, the last one seen, or the one that's a qualifier. QStringList would be better.
    We only do the searching for columns, but other objects could be in a hierarchy too.
    We don't notice SELECT x1 AS x2 FROM t ORDER BY x2;
    We skip anything inside ()s but FROM clauses can have table names inside ()s.
    We stop at hparse_i though if we're in a select-list then FROM table-list is after hparse_i.
  Example for 'I': DELETE FROM T INDEXED BY `
  Todo: See the comments preceding QString MainWindow::rehash_search(char *search_string, int reftype)
        "Todo: We only look at column[1] column_name. We should look at column[0] table_name."
        We haven't done this yet.
  Todo: we perhaps should hint by saying X (table-qualifier) ... x (select-list name) ... x (column in table) etc.
  Todo: expand, it might be more than TOKEN_REFTYPE_COLUMN
        but there is no equivalent search for table because our rehash only does one database, the current one.
*/
QString MainWindow::hparse_f_pre_rehash_search(int reftype)
{
  if (hparse_f_is_rehash_searchable(reftype) == false) return ""; /* If there has been no rehash return "" */
  //int reftype= main_token_reftypes[hparse_i];
  /* Similar checks are in rehash_search */
  if ((reftype != TOKEN_REFTYPE_COLUMN)
   && (reftype != TOKEN_REFTYPE_TABLE_OR_COLUMN)
   && (reftype != TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN)
   && (reftype != TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN_OR_FUNCTION)
   && (reftype != TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN_OR_FUNCTION_OR_VARIABLE)
   && (reftype != TOKEN_REFTYPE_COLUMN_OR_USER_VARIABLE)
   && (reftype != TOKEN_REFTYPE_COLUMN_OR_VARIABLE)
   && (reftype != TOKEN_REFTYPE_TABLE_OR_COLUMN_OR_FUNCTION)
   && (reftype != TOKEN_REFTYPE_INDEX))
    return "";
  int i_start= i_of_elementary_statement();
  QString table_name= "";
  int parentheses_count= 0;
  for (int i= i_start; i <= hparse_i; ++i)
  {
    if (main_token_types[i] == TOKEN_TYPE_OPERATOR)
    {
      QString operator_token= hparse_text_copy.mid(main_token_offsets[i], main_token_lengths[i]);
      if (operator_token == ")") ++parentheses_count;
      if (operator_token == "(") --parentheses_count;
    }
    if (parentheses_count != 0) continue;
    if (main_token_reftypes[i] == TOKEN_REFTYPE_TABLE)
    {
      int j= next_token(i);
      if (hparse_text_copy.mid(main_token_offsets[j], main_token_lengths[j]) == ".")
      {
        if (next_token(j) != hparse_i) continue;
        table_name= hparse_text_copy.mid(main_token_offsets[i], main_token_lengths[i]);
        break;
      }
      table_name= hparse_text_copy.mid(main_token_offsets[i], main_token_lengths[i]);
    }
  }
  return table_name;
}

/*
  Return true if token should be searched in a rehash list, else return false.
  Certainly it is false if rehash_result_row_count == 0, presumably there has been no rehash.
  It might be false if we are creating the object, which can happen if:
    (not done for all!) CREATE ... object_type object_name
    ALTER TABLE ... ADD COLUMN|CONSTRAINT object_name
    (not done!) DECLARE variable_name ... hmm, it would be pointless, variables aren't database objects.
  Todo: maybe a qualifier will be followed by "." in which case return true.
*/
bool MainWindow::hparse_f_is_rehash_searchable(int reftype)
{
  if ((main_token_flags[hparse_i] & TOKEN_FLAG_IS_NEW) != 0) return false;
  if (ocelot_explorer_visible == "yes")
  {
    if (reftype == TOKEN_REFTYPE_DATABASE) return true;
    if ((reftype == TOKEN_REFTYPE_TABLE) && (hparse_specified_schema > "")) return true;
  }
  return true;
}

/*
  A client statement can be "\" followed by a character, for example \C.
  Todo: The tokenizer must find these combinations and produce two tokens: \ and C.
        It does not matter what follows the C, it is considered to be a breaker.
        Even statements like "drop table e\Gdrop table y;" are supposed to be legal.
  Case sensitive.
  Checking for the first word of a client statement.
  \? etc. are problems because (a) they're two tokens, (b) they're case sensitive (c) they delimit.
  \ is a statement-end, and C is a statement.
  For highlighting, the \ is an operator and the C is a keyword.
  So this could actually be called from hparse_f_semicolon_or_delimiter.
  This routine does not accept and go ahead, it just tells us that's what we're looking at.
  These tokens will not show up in a list of predictions.
*/
int MainWindow::hparse_f_backslash_command(bool eat_it)
{
  int slash_token= -1;
  if (hparse_token != "\\") return 0;
  if (main_token_lengths[hparse_i + 1] != 1) return 0;
  QString s= hparse_text_copy.mid(main_token_offsets[hparse_i + 1], 1);
  if (s == QString("?")) slash_token= TOKEN_KEYWORD_QUESTIONMARK;
  else if (s == QString("C")) slash_token= TOKEN_KEYWORD_CHARSET;
  else if (s == QString("c")) slash_token= TOKEN_KEYWORD_CLEAR;
  else if (s == QString("r")) slash_token= TOKEN_KEYWORD_CONNECT;
  else if (s == QString("d")) slash_token= TOKEN_KEYWORD_DELIMITER;
  else if (s == QString("e")) slash_token= TOKEN_KEYWORD_EDIT;
  else if (s == QString("G")) slash_token= TOKEN_KEYWORD_EGO;
  else if (s == QString("g")) slash_token= TOKEN_KEYWORD_GO;
  else if (s == QString("h")) slash_token= TOKEN_KEYWORD_HELP_IN_CLIENT;
  else if (s == QString("n")) slash_token= TOKEN_KEYWORD_NOPAGER;
  else if (s == QString("t")) slash_token= TOKEN_KEYWORD_NOTEE;
  else if (s == QString("w")) slash_token= TOKEN_KEYWORD_NOWARNING;
  else if (s == QString("P")) slash_token= TOKEN_KEYWORD_PAGER;
  else if (s == QString("p")) slash_token= TOKEN_KEYWORD_PRINT;
  else if (s == QString("R")) slash_token= TOKEN_KEYWORD_PROMPT;
  else if (s == QString("q")) slash_token= TOKEN_KEYWORD_QUIT;
  else if (s == QString("#")) slash_token= TOKEN_KEYWORD_REHASH;
  else if (s == QString(".")) slash_token= TOKEN_KEYWORD_SOURCE;
  else if (s == QString("s")) slash_token= TOKEN_KEYWORD_STATUS;
  else if (s == QString("!")) slash_token= TOKEN_KEYWORD_SYSTEM;
  else if (s == QString("T")) slash_token= TOKEN_KEYWORD_TEE;
  else if (s == QString("u")) slash_token= TOKEN_KEYWORD_USE;
  else if (s == QString("W")) slash_token= TOKEN_KEYWORD_WARNINGS;
  //else if (s == QString("x")) slash_token= TOKEN_KEYWORD_TOKEN_KEYWORD_RESETCONNECTION;
  else return 0;
  if (eat_it == true)
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,slash_token, "\\"); /* Todo: mark as TOKEN_FLAG_END */
    if (hparse_errno > 0) return 0;
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,slash_token, s);
    if (hparse_errno > 0) return 0;
  }
  return slash_token;
}

/*
  Certain client statements -- delimiter, prompt, source -- take pretty well anything
  as the end of the line. So we want the highlight to always be the same
  (we picked "literal" but anything would do). Some deviations from mysql client:
  * we allow SOURCE 'literal' etc., anything within the quote marks is the argument
  * we allow comments, we do not consider them to be part of the argument
  * we haven't figured out what to do with delimiters or ;
  * it's uncertain what we'll do when it comes time to execute
  * delimiter can end with space, but source and prompt cannot, that's not handled
  * todo: this isn't being called for prompt
  Delimiters
  ----------
  If DELIMITER is the only or the first statement, rules are documented and comprehensible:
    Whatever follows \d or "delimiter" is a single token as far as " ",
    or a quoted string quoted by ' or " or `. So we change tokenizer() to say:
    * first check for quoted string (if it is, then token #1 is quoted string)
    * if it's start of token#1, and token#0 is \d or "delimiter", skip till " " or <eof>
    The result is in effect for the next tokenize, not for subsequent statements on the line.
    (Hmm, with 2023-05-07 change, there won't be subsequent statements on the line, I guess.)
  Todo: after delimiter // then delimiter ;; the delimiter is ;; but statement widget has a ;
  If DELIMITER is not the first statement, rules are not documented and bizarre:
    The string that follows is the new delimiter, but the rest of the line is ignored.
  DELIMITER causes new rules! Everything following as far as " " is delimiter-string.
*/
/* flag values: 1 means "; marks end" */
void MainWindow::hparse_f_other(int flags)
{
  if ((main_token_types[hparse_i] == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
   || (main_token_types[hparse_i] == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE))
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_LITERAL, "[literal]"); /* guaranteed to succeed */
    if (hparse_errno > 0) return;
  }
  else if (main_token_types[hparse_i] == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK)
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "[identifier]"); /* guaranteed to succeed */
    if (hparse_errno > 0) return;
  }
  else
  {
    if (main_token_lengths[hparse_i] == 0)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_LITERAL, "[literal]"); /* guaranteed to fail */
      if (hparse_errno > 0) return;
    }
  }
  for (;;)
  {
    if (main_token_lengths[hparse_i] == 0) break;
    if (((flags&1) == 1) && (main_token_lengths[hparse_i] == 1))
    {
      if (hparse_text_copy.mid(main_token_offsets[hparse_i], 1) == ";")
      {
        hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ";");
        break;
      }
    }
    main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_RESERVED);
    main_token_types[hparse_i]= hparse_token_type= TOKEN_TYPE_LITERAL;
    //if (main_token_lengths[hparse_i + 1] == 0)
    //{
    //  break;
    //}
    bool line_break_seen= false;
    /* Warning: main_token_offsets[hparse_i+1] can be a crazy number so check size or check if q=="" */
    for (int i_off= main_token_offsets[hparse_i] + main_token_lengths[hparse_i];; ++i_off)
    {
      if (i_off >= main_token_offsets[hparse_i + 1]) break;
      if (i_off >= hparse_text_copy.size()) break;
      QString q= hparse_text_copy.mid(i_off, 1);
      if ((q == "\n") || (q == "\r"))
      {
        line_break_seen= true;
        break;
      }
    }
    if (line_break_seen == true)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_LITERAL, "[literal]"); /* guaranteed to succeed */
      if (hparse_errno > 0) return;
      break;
    }

    //if (main_token_offsets[hparse_i] + main_token_lengths[hparse_i]
    //   < main_token_offsets[hparse_i + 1])
    //{
    //  break;
    //}
    hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_LITERAL, "[literal]"); /* guaranteed to succeed */
    if (hparse_errno > 0) return;
  }
}

#if (OCELOT_IMPORT_EXPORT == 1)
/*
  Called from hparse_f_client_statement() for special handling of SET ocelot_export.
  See comments in ocelotgui.cpp before import_export_rule_set()
  Return 1 = ocelot_ but no conditional possible, or 0 if error.
*/
int MainWindow::hparse_f_client_set_export()
{
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FORMAT, "FORMAT") == 0)
  {
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0)
    {
      hparse_f_error();
      return 0;
    }
    return 1;
  }
  int type= 0;

  {
    QStringList q= (QStringList() << "text" << "table" << "html" << "none");
    int picked= hparse_pick_from_list(q);
    if (picked == -1)
    {
      hparse_f_error();
      return 1;
    }
    if (picked == 0) type= TOKEN_KEYWORD_TEXT;
    if (picked == 1) type= TOKEN_KEYWORD_TABLE;
    if (picked == 2) type= TOKEN_KEYWORD_HTML;
    if (picked == 3) type= TOKEN_KEYWORD_NONE;
  }
  if (type == TOKEN_KEYWORD_NONE) return 1;

  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INTO, "INTO") == 1)
  {
    if (hparse_f_literal(TOKEN_REFTYPE_FILE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 1) {;}
    else hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_STDOUT, "STDOUT");
    if (hparse_errno > 0) return 0;
  }
  int k= 0; /* When k == 0 we don't accept, e.g. when it's HTML we don't have a MARGIN option */
  if ((type == TOKEN_KEYWORD_TEXT) || (type == TOKEN_KEYWORD_TABLE)) k= 1;
  if (k == 1) hparse_f_infile_or_outfile();
  if (hparse_errno > 0) return 0;
  while ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_MAX_ROW_COUNT, "MAX_ROW_COUNT") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_COLUMN_NAMES, "COLUMN_NAMES") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL*k, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_QUERY, "QUERY") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL*k, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ROW_COUNT, "ROW_COUNT") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL*k, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_MARGIN, "MARGIN") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL*k, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PAD, "PAD") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL*k, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LAST, "LAST") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL*k, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DIVIDER, "DIVIDER") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL*k, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REPLACE, "REPLACE") == 1)
   || (hparse_f_accept(FLAG_VERSION_ALL*k, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IF, "IF") == 1))
  {
    if ((main_token_types[hparse_i_of_last_accepted] == TOKEN_KEYWORD_MAX_ROW_COUNT)
     || (main_token_types[hparse_i_of_last_accepted] == TOKEN_KEYWORD_MARGIN))
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_NUMBER) != 1)
      {
        hparse_f_error();
        break;
      }
    }
    else if (main_token_types[hparse_i_of_last_accepted] == TOKEN_KEYWORD_REPLACE)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) != 1)
      {
        hparse_f_error();
        break;
      }
      hparse_f_expect(FLAG_VERSION_ALL*k, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WITH, "WITH");
      if (hparse_errno > 0) break;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) != 1)
      {
        hparse_f_error();
        break;
      }
    }
    else if (main_token_types[hparse_i_of_last_accepted] == TOKEN_KEYWORD_IF)
    {
      if (hparse_f_accept(FLAG_VERSION_ALL*k, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_NULL, "NULL") == 1)
      {
        if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DEFAULT, "DEFAULT") != 1)
         && (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) != 1))
        {
          hparse_f_error();
          break;
        }
      }
      else
      {
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FILE, "FILE");
        if (hparse_errno > 0) break;
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FILE, "EXISTS");
        if (hparse_errno > 0) break;
        QStringList q= (QStringList() << "append" << "error" << "replace");
        if (hparse_pick_from_list(q) == -1)
        {
          hparse_f_error();
          break;
        }
      }
    }
    else
    {
      QStringList q= (QStringList() << "yes" << "no");
      if (hparse_pick_from_list(q) == -1)
      {
        hparse_f_error();
        break;
      }
    }
  }
  return 1;
}
#endif

/*
  Called from hparse_f_client_statement() for special handling of SET ocelot_statement_rule.
  See comments in ocelotgui.cpp before statement_format_rule()
  Return 1 = ocelot_ but no conditional possible
*/
int MainWindow::hparse_f_client_set_rule()
{
  QString replacee;
  for (;;)
  {
    replacee= hparse_token.toUpper();
    if (hparse_f_acceptf(1, "") == 0)
    {
      hparse_f_error();
      return 1;
    }
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BECOMES, "BECOMES");
    if (hparse_errno > 0) return 1;
    while (hparse_f_acceptf(2, replacee) == 1) {;}
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1) continue;
    break;
  }
  return 1;
}

/* Called from hparse_f_client_statement() for special handling of SET ocelot_query. */
int MainWindow::hparse_f_client_set_query()
{
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
  if (hparse_errno > 0) return 1;

  /* INSERT INTO plugins|menus VALUES (string[,string...]) -- SQLish but so restricted we won't use the main INSERT routine */
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INSERT, "INSERT") == 1)
  {
    int table_type= 0; /* 0 if menu, TOKEN_KEYWORD_PLUGINS if plugin */
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_INTO, "INTO");
    if (hparse_errno > 0) return 1;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "PLUGINS") == 1)
      table_type= TOKEN_KEYWORD_PLUGINS;
    else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "MENUS") == 1)
      table_type= 0;
    else hparse_f_error();
    if (hparse_errno > 0) return 1;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_VALUES, "VALUES");
    if (hparse_errno > 0) return 1;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return 1;
    if (table_type == TOKEN_KEYWORD_PLUGINS)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_LIBRARY_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return 1;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",");
      if (hparse_errno > 0) return 1;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ACTION_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return 1;
    }
    else /* MENUS */
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ID_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return 1;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",");
      if (hparse_errno > 0) return 1;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_MENU_TITLE_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return 1;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",");
      if (hparse_errno > 0) return 1;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_MENU_ITEM_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return 1;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",");
      if (hparse_errno > 0) return 1;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ACTION_STRING) == 0) hparse_f_error();
      if (hparse_errno > 0) return 1;
    }
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return 1;
    return 1;
  }

  /* SELECT * FROM plugins|menus; or SELECT lteral AS identifier; */
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SELECT, "SELECT") == 1)
  {
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "*") == 1)
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FROM, "FROM");
      if (hparse_errno > 0) return 1;
#if (OCELOT_PLUGIN == 1)
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "PLUGINS") != 1)
#endif
      {
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "CONDITIONAL_SETTINGS") != 1)
          hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "MENUS");
      }
      if (hparse_errno > 0) return 1;
    }
    else
    {
      for (;;)
      {
        do
        {
          if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT) == 0) hparse_f_error();
          if (hparse_errno > 0) return 1;
          if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_AS, "AS") == 1)
          {
            hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_COLUMN,TOKEN_TYPE_IDENTIFIER, "[identifier]");
            if (hparse_errno > 0) return 1;
          }
        } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_UNION, "UNION") == 0) break;
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ALL, "ALL");
        if (hparse_errno > 0) return 1;
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SELECT, "SELECT");
        if (hparse_errno > 0) return 1;
        /* continue */
      }
    }
    return 1;
  }

  /* DELETE FROM menus WHERE id = '' -- SQLish but so restrictive we won't use the main DELETE routine */
  /* or DELETE FROM plugins WHERE action = '' */
  /* Todo: the table list and WHERE clause are the same as in UPDATE, and WHERE for conditional_settings looks hard */
  /* Todo: need pick_from_list as in UPDATE, and conditional_settings should have an id */
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DELETE, "DELETE") == 1)
  {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FROM, "FROM");
      if (hparse_errno > 0) return 1;
      int table_type= -1;
#if (OCELOT_PLUGIN == 1)
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "PLUGINS") == 1)
        table_type= TOKEN_KEYWORD_PLUGINS;
      else
#endif
      {
        table_type= 0;
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "CONDITIONAL_SETTINGS") != 1)
          hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "MENUS");
      }
    if (hparse_errno > 0) return 1;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WHERE, "WHERE");
    if (hparse_errno > 0) return 1;
    if (table_type == TOKEN_KEYWORD_PLUGINS) hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "action");
    else hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "id");
    if (hparse_errno > 0) return 1;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
    if (hparse_errno > 0) return 1;
    /* todo: distinguish action literal | id literal */
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
    if (hparse_errno > 0) return 1;
     return TOKEN_KEYWORD_SET;
  }

  /* UPDATE menus SET shortcut = 'literal' WHERE id = '' -- SQLish but so restricted we won't use the main UPDATE routine */
  /* todo: after this it's expecting another WHERE which isn't right */
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_UPDATE, "UPDATE") == 1)
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "MENUS");
    if (hparse_errno > 0) return 1;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SET, "SET");
    if (hparse_errno > 0) return 1;
    int token_literal_flag= TOKEN_LITERAL_FLAG_STRING;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "MENU_ITEM") == 1)
      token_literal_flag= TOKEN_LITERAL_FLAG_MENU_ITEM_STRING;
    else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "SHORTCUT") == 1)
      token_literal_flag= TOKEN_LITERAL_FLAG_KEYSEQUENCE_STRING;
    else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "ACTION") == 1)
      token_literal_flag= TOKEN_LITERAL_FLAG_ACTION_STRING;
    else hparse_f_error();
    if (hparse_errno > 0) return 1;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
    if (hparse_errno > 0) return 1;
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, token_literal_flag) == 0) hparse_f_error();
    if (hparse_errno > 0) return 1;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WHERE, "WHERE");
    if (hparse_errno > 0) return 1;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_IDENTIFIER, "id");
    if (hparse_errno > 0) return 1;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
    if (hparse_errno > 0) return 1;
    QStringList q;
    for (int i= 0; i < menu_spec_struct_list.size(); ++i)
    {
      if (menu_spec_struct_list[i].menu_type == MENU_SPEC_TYPE_MENUITEM)
       q.append(menu_spec_struct_list[i].id);
    }
    if (hparse_pick_from_list(q) == -1) hparse_f_error();
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_SET;
    return 1;
  }

  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SHOW, "SHOW");
  if (hparse_errno > 0) return 1;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_ERDIAGRAM, "ERDIAGRAM") == 1)
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_OF, "OF");
    if (hparse_errno > 0) return 1;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_DATABASE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return 0;
    hparse_specified_schema= hparse_text_copy.mid(main_token_offsets[hparse_i_of_last_accepted], main_token_lengths[hparse_i_of_last_accepted]);
    hparse_specified_schema= connect_stripper(hparse_specified_schema, false);
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_COLUMNS, "COLUMNS") == 1)  /* option */
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PRIMARY, "PRIMARY");
      if (hparse_errno > 0) return 0;
    }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LINES, "LINES") == 1)  /* option */
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_IN, "IN");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_BACKGROUND, "BACKGROUND");
      if (hparse_errno > 0) return 0;
    }

    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LINES, "TABLES") == 1)  /* option */
    {
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return 0;
      hparse_specified_list.clear(); /* unnecessary */
      for (unsigned int i_of_oei= 0; i_of_oei < oei_count; ++i_of_oei)
      {
        if (oei[i_of_oei].object_type == "T")
        {
          if (QString::compare(oei[i_of_oei].schema_name, hparse_specified_schema, Qt::CaseInsensitive) == 0)
          {
            hparse_specified_list << oei[i_of_oei].object_name;
          }
        }
      }
      do
      {
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) { hparse_specified_schema= ""; hparse_specified_list.clear(); return 0; }
        QString s_match= hparse_text_copy.mid(main_token_offsets[hparse_i_of_last_accepted], main_token_lengths[hparse_i_of_last_accepted]);
        int i_of_match= hparse_specified_list.indexOf(s_match, Qt::CaseInsensitive);
        hparse_specified_list.removeAt(i_of_match);
        if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_INTEGER) != 0) /* [x y] */
        {
          if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_INTEGER) == 0) hparse_f_error();
        }
        if (hparse_errno > 0) { hparse_specified_schema= ""; hparse_specified_list.clear(); return 0; }
      } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
      hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) { hparse_specified_schema= ""; hparse_specified_list.clear(); return 0; }
    }
    hparse_specified_schema= "";
    hparse_specified_list.clear();
  }
  else
  {
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_FOREIGN, "FOREIGN");
    if (hparse_errno > 0) return 1;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_KEYS, "KEYS");
    if (hparse_errno > 0) return 1;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_OF, "OF");
    if (hparse_errno > 0) return 1;
    if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_TABLE,TOKEN_REFTYPE_TABLE) == 0)
      hparse_f_error();
  }
  return 1;
}

/*
  Called from hparse_f_client_statement() for special handling of SET ocelot_... = literal
  Return: 0 = not ocelot_ (with hparse_errno > 0), 1 = ocelot_ but no conditional possible,
          3 = ocelot_ and conditional possible and "," seen, 4 = ocelot_and_conditional possible but no ","
  Todo: if it ends with _COLOR then be specific about what color literals are ok
  Todo: if it can be conditional then , is ok but later WHERE is mandatory
  If you add to this, hparse_errmsg might not be big enough.
  Todo: strvalues items are in order so you could bsearch().
  Todo: TOKEN_REFTYPE_ANY is vague, you'd do well with a reftype for ocelot_ items
  Todo: it's really TOKEN_TYPE_IDENTIFIER so see what hovering does
  Todo: we have autocomplete for color and font_style|weight|family but we culd have more
  Todo: why is it an error if starts with ocelot_ but isn't specifically one of the reserved ocelot_ items?
*/
int MainWindow::hparse_f_client_set()
{
  bool is_conditional_settings_possible= false;
  QString last_accepted_item= hparse_text_copy.mid(main_token_offsets[hparse_i_of_last_accepted], main_token_lengths[hparse_i_of_last_accepted]);

  int count_of_previous_items= 0;
  QStringList list_of_previous_items;
  int count_of_conditionals_in_group= 0;
  if (last_accepted_item == ",")
  {
    QString prev_token_string;
    for (int i_of_prev= hparse_i_of_last_accepted;;)
    {
      if (i_of_prev == 0) break;
      int prev_token_type= main_token_types[i_of_prev];
      if (prev_token_type == TOKEN_KEYWORD_SET) break;
      if ((prev_token_type >= TOKEN_KEYWORD_OCELOT_BATCH) && (prev_token_type <= TOKEN_KEYWORD_OCELOT_XML))
      {
        ++count_of_previous_items;
        prev_token_string= strvalues[prev_token_type].chars;
        list_of_previous_items.append(prev_token_string);
        if (count_of_previous_items == 1)
        {
          int i_of_strvalues;
          for (i_of_strvalues= prev_token_type; i_of_strvalues > TOKEN_KEYWORD_OCELOT_BATCH; --i_of_strvalues)
          {
            QString strvalue= strvalues[i_of_strvalues].chars;
            if (strvalue.left(8) != prev_token_string.left(8)) break;
          }
          ++i_of_strvalues;
          for (; i_of_strvalues > TOKEN_KEYWORD_OCELOT_BATCH; ++i_of_strvalues)
          {
            QString strvalue= strvalues[i_of_strvalues].chars;
            if (strvalue.left(8) != prev_token_string.left(8)) break;
            if ((strvalues[i_of_strvalues].reserved_flags & FLAG_VERSION_CONDITIONAL) != 0)
              ++count_of_conditionals_in_group;
          }
        }
      }
      i_of_prev= next_i(i_of_prev, -1);
    }
  }
  int i_in_strvalues;
  for (i_in_strvalues=TOKEN_KEYWORD_OCELOT_BATCH; i_in_strvalues <= TOKEN_KEYWORD_OCELOT_XML; ++i_in_strvalues)
  {
    if (last_accepted_item == ",")
    {
      if ((strvalues[i_in_strvalues].reserved_flags & FLAG_VERSION_CONDITIONAL) == 0) continue;
      QString ocelot_keyword_string= strvalues[i_in_strvalues].chars;
      bool is_prev_not_same_group= false;
      bool is_prev_same_keyword= false;
      for (int i_of_list= 0; i_of_list < list_of_previous_items.size(); ++i_of_list)
      {
        QString text_of_a_previous_item= list_of_previous_items.at(i_of_list);
        if (text_of_a_previous_item.left(8) != ocelot_keyword_string.left(8)) is_prev_not_same_group= true;
        if (text_of_a_previous_item == ocelot_keyword_string) is_prev_same_keyword= true;
      }
      if (is_prev_not_same_group == true) continue;
      if (is_prev_same_keyword == true) continue;
    }
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,i_in_strvalues, strvalues[i_in_strvalues].chars) == 1)
      break;
  }
  /* Todo: figure out: is it an error if starts with ocelot_ but not one of our keywords etc.? or is it okay? */
  if (i_in_strvalues > TOKEN_KEYWORD_OCELOT_XML) hparse_f_error(); /* i.e. if all hparse_f_accepts failed */
  if (hparse_errno > 0) return 0; /* starts with ocelot_ but not one of our keywords, or we've already seen it */
  if ((strvalues[i_in_strvalues].reserved_flags & FLAG_VERSION_CONDITIONAL) != 0)
  {
    is_conditional_settings_possible= true;
  }

  if (main_token_types[hparse_i_of_last_accepted] == TOKEN_KEYWORD_OCELOT_STATEMENT_FORMAT_RULE)
  {
    return hparse_f_client_set_rule();
  }

  //if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0)
  {
    if (main_token_types[hparse_i_of_last_accepted] == TOKEN_KEYWORD_OCELOT_QUERY)
    {
      return hparse_f_client_set_query();
    }
  }
  int last_accepted= main_token_types[hparse_i_of_last_accepted];

  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
  if (hparse_errno > 0) return 0;

#if (OCELOT_IMPORT_EXPORT == 1)
  if (last_accepted == TOKEN_KEYWORD_OCELOT_EXPORT)
  {
    return hparse_f_client_set_export();
  }
#endif

  int i_of_keyword= i_in_strvalues;

  main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_RESERVED);
  main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_FUNCTION);
  QStringList q;
  q.clear();
  if (xsettings_widget->ocelot_variable_is_color(i_of_keyword) == true)
    for (int q_i= color_off; strcmp(s_color_list[q_i]," ") > 0; q_i+= 2) q.append(s_color_list[q_i]);
  if (xsettings_widget->ocelot_variable_is_font_weight(i_of_keyword) == true)
  {
    for (int q_i= 0; q_i < FONTWEIGHTSVALUES_SIZE; ++q_i) q.append(fontweightsvalues[q_i].chars);
    for (int q_i= 0; q_i < FONTWEIGHTSVALUES_SIZE; ++q_i) q.append(QString::number(fontweightsvalues[q_i].css_number));
  }
  if (xsettings_widget->ocelot_variable_is_font_style(i_of_keyword) == true)
  {
    q.append("normal"); q.append("italic"); q.append("oblique");
  }
  if (xsettings_widget->ocelot_variable_is_font_family(i_of_keyword) == true)
  {
    QFontDatabase font_database;
    q= font_database.families();
  }
  if (i_of_keyword == TOKEN_KEYWORD_OCELOT_GRID_CHART)
  {
    q.append("BAR"); q.append("LINE"); q.append("PIE");
    q.append("BAR VERTICAL"); q.append("BAR STACKED"); q.append("BAR VERTICAL STACKED");
    q.append("BAR SUBGROUP BY VALUE % 3"); q.append("LINE SUBGROUP BY VALUE % 3"); q.append("PIE SUBGROUP BY VALUE % 3");
    q.append("BAR SUBGROUP BY LEFT(COLUMN_NAME, 2)"); q.append("LINE SUBGROUP BY LEFT(COLUMN_NAME, 2)"); q.append("PIE SUBGROUP BY LEFT(COLUMN_NAME, 2)");
  }
  if (q.count() > 0)
  {
    if (hparse_pick_from_list(q) == -1)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING) == 0) hparse_f_error();
    }
  }
  else if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_STRING | TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER) == 0) hparse_f_error();
  if (hparse_errno > 0) return 0;
  if (is_conditional_settings_possible == true)
  {
    if (((count_of_previous_items + 1) < count_of_conditionals_in_group) || (count_of_conditionals_in_group == 0))
    {
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ",") == 1)
        return 3; /* so caller will call hparse_f_client_set() again */
    }
    return 4; /* so caller will not call hparse_f_client_set() again but sees there was a conditional */
  }
  return 1;
}

/*
  For picking a string literal when there are fixed choices.
  Used for colors + fonts.
  Todo: use for more. Pass a flag if it's to be enclosed in ''s. Pass a flag if it's to be sorted.
*/
int MainWindow::hparse_pick_from_list(QStringList q)
{
  for (int q_i= 0; q_i < q.size(); ++q_i)
  {
    QString s= "'" + q.at(q_i) + "'";
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, s) == 1)
      return q_i;
  }
  return -1;
}

/*
  Statements handled locally (by ocelotgui), which won't go to the server.
  Todo: we're ignoring --binary-mode.
  Todo: we're only parsing the first word to see if it's client-side, we could do more.
  SET is a special problem because it can be either client or server (flaw in our design?).
  Within client statements, reserved words don't count so we turn the reserved flag off.
  HELP is a special problem because it can be either client or server (flaw in mysql design).
  Return 0 = not client statement, 1 = client statement but not SET, TOKEN_KEYWORD_SET = client statement specifically SET
*/
int MainWindow::hparse_f_client_statement()
{
  hparse_next_token= hparse_next_next_token= "";
  hparse_next_next_next_token= hparse_next_next_next_next_token= "";
  int saved_hparse_i= hparse_i;
  int saved_hparse_token_type= hparse_token_type;
  QString saved_hparse_token= hparse_token;
  int slash_token= hparse_f_backslash_command(true);
  if (hparse_errno > 0) return 0;
  if ((slash_token == TOKEN_KEYWORD_QUESTIONMARK) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_QUESTIONMARK, "?") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_CHARSET) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CHARSET, "CHARSET") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_RESERVED);
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_CHARACTER_SET,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_CHARACTER_SET, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();

      if (hparse_errno > 0) return 0;
    }
  }
  else if ((slash_token == TOKEN_KEYWORD_CLEAR) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CLEAR, "CLEAR") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_CONNECT) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CONNECT, "CONNECT") == 1))
  {
     if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_DELIMITER) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_DELIMITER, "DELIMITER") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    QString tmp_delimiter= get_delimiter(hparse_token, hparse_text_copy, main_token_offsets[hparse_i]);
    if (tmp_delimiter > " ")
    {
      /* 2023-05-07: We no longer stop on ; because DELIMITER ;; is legal */
      hparse_delimiter_str= "";
      hparse_f_other(0);
      hparse_delimiter_str= tmp_delimiter;
      /* Redo tokenize because if delimiter changes then token ends change. */
      if ((main_token_lengths[hparse_i] != 0) && (main_token_offsets[hparse_i] != 0))
      {
        int offset_of_rest= main_token_offsets[hparse_i];
        tokenize(hparse_text_copy.data() + offset_of_rest,
                 hparse_text_copy.size() - offset_of_rest,
                 main_token_lengths + hparse_i,
                 main_token_offsets + hparse_i,
                 main_token_max_count - (hparse_i + 1),
                 (QChar*)"33333",
                 1,
                 hparse_delimiter_str,
                 1);
        for (int ix= hparse_i; main_token_lengths[ix] != 0; ++ix)
        {
          main_token_offsets[ix]+= offset_of_rest;
        }
        tokens_to_keywords(hparse_text_copy, hparse_i, hparse_sql_mode_ansi_quotes);
      }
    }
    else hparse_f_other(1);
    if (hparse_errno > 0) return 0;
  }
  else if ((slash_token == TOKEN_KEYWORD_EDIT) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_EDIT, "EDIT") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_EGO) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_EGO, "EGO") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_EXIT) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_EXIT, "EXIT") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_GO) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_GO, "GO") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if (slash_token == TOKEN_KEYWORD_HELP_IN_CLIENT)
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_HELP, "HELP") == 1)
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0)
    {
      if (main_token_lengths[hparse_i] != 0)
      {
        QString d= hparse_text_copy.mid(main_token_offsets[hparse_i], main_token_lengths[hparse_i]);
        if (d != hparse_delimiter_str)
        {
          hparse_i= saved_hparse_i;
          hparse_token_type= saved_hparse_token_type;
          hparse_token= saved_hparse_token;
          return 0;
        }
      }
    }
    main_token_types[hparse_i_of_last_accepted]= TOKEN_KEYWORD_HELP_IN_CLIENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_NOPAGER) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_NOPAGER, "NOPAGER") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_NOTEE) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_NOTEE, "NOTEE") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_NOWARNING) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_NOWARNING, "NOWARNING") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_PAGER) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PAGER, "PAGER") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_PRINT) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PRINT, "PRINT") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_PROMPT) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_PROMPT, "PROMPT")== 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    /* PROMPT can be followed by any bunch of junk as far as ; or delimiter or eof or \n*/
    QString d;
    int j;
    for (;;)
    {
      j= main_token_offsets[hparse_i - 1] + main_token_lengths[hparse_i - 1];
      d= hparse_text_copy.mid(j, main_token_offsets[hparse_i]- j);
      if (d.contains("\n")) break;
      if ((main_token_lengths[hparse_i] == 0)
       //|| (hparse_token == ";")
       || (hparse_token == hparse_delimiter_str)) break;
      main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_RESERVED);
      main_token_flags[hparse_i] &= (~TOKEN_FLAG_IS_FUNCTION);
      main_token_types[hparse_i]= TOKEN_TYPE_OTHER;
      hparse_f_nexttoken();
    }
  }
  else if ((slash_token == TOKEN_KEYWORD_QUIT) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_QUIT, "QUIT") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_REHASH) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REHASH, "REHASH") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  //else if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_KEYWORD_RESETCONNECTION, "RESETCONNECTION") == 1))
  //{
  // if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  //}
#if (OCELOT_EXPLORER == 1)
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_REFRESH, "REFRESH") == 1)
  {
    main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
#endif
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SET, "SET") == 1)
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    if (main_token_lengths[hparse_i] != 0)
    {
      QString s= hparse_token.mid(0, 7);
      if (QString::compare(s, "OCELOT_", Qt::CaseInsensitive) != 0)
      {
        hparse_i= saved_hparse_i;
        hparse_token_type= saved_hparse_token_type;
        hparse_token= saved_hparse_token;
        return 0;
      }
    }
    int hparse_client_set_result= hparse_f_client_set();
    if (hparse_errno > 0) return 0;
    if (hparse_client_set_result >= 3) /* i.e. is conditional_settings possible? */
    {
      if (hparse_client_set_result == 3)
        while (hparse_f_client_set() == 3) {;} /* possible series of "," + ocelot_grid color settings */
      if (hparse_errno > 0) return 0;
      int assignee_keyword;
      for (int i_of_assignee= hparse_i_of_last_accepted;;)
      {
        assignee_keyword= main_token_types[i_of_assignee];
        if (assignee_keyword >= TOKEN_KEYWORD_ABS) break;
        i_of_assignee= next_i(i_of_assignee, -1);
      }
      if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_WHERE, "WHERE"))
      {
        for (;;)
        {
          int tlf= -1;
          if ((assignee_keyword < TOKEN_KEYWORD_OCELOT_EXPLORER_ACTION)
           || (assignee_keyword > TOKEN_KEYWORD_OCELOT_EXPLORER_WIDTH))
          { /* for comparisons that are recommended for grid */
            if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_COLUMN_NAME, "COLUMN_NAME") == 1) tlf= TOKEN_LITERAL_FLAG_STRING;
            else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_COLUMN_NUMBER, "COLUMN_NUMBER") == 1) tlf= TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER;
            else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_COLUMN_TYPE, "COLUMN_TYPE") == 1) tlf= TOKEN_LITERAL_FLAG_STRING;
            else if ((assignee_keyword != TOKEN_KEYWORD_OCELOT_GRID_CHART) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_ROW_NUMBER, "ROW_NUMBER") == 1)) tlf= TOKEN_LITERAL_FLAG_UNSIGNED_INTEGER;
          }
          if (tlf == -1) /* for a comparison that is recommended for either grid or explorer */
          {
            //The following lines would be good if we could be sure we were looking at context menu
            //QStringList q= explorer_text_list();
            //if (hparse_pick_from_list(q) == -1) hparse_f_error();
            if ((assignee_keyword != TOKEN_KEYWORD_OCELOT_GRID_CHART) && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_VALUE, "VALUE") == 1)) tlf= TOKEN_LITERAL_FLAG_STRING_OR_NUMBER_OR_CONSTANT;
          }
          if (tlf == -1) hparse_f_error(); /* for none of the recommended comparisons */
          if (hparse_errno > 0) return 0;
          bool is_is_seen= false;
          if (hparse_f_comp_op() == 0)
          {
            if ((tlf & TOKEN_LITERAL_FLAG_STRING) != 0)
            {
              if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_IS, "IS") == 1)
              {
                is_is_seen= true;
                hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_NULL, "NULL");
              }
              else hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_LIKE, "REGEXP");
            }
            else hparse_f_error();
          }
          if (hparse_errno > 0) return 0;
          if (is_is_seen == false)
            if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, tlf) == 0) hparse_f_error();
          if (hparse_errno > 0) return 0;
          if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_AND, "AND") != 1)
           && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_KEYWORD_OR, "OR") != 1)) break;
        }
      }
      else /* For some assignees WHERE is compulsory. Todo: this only checks last assignee, should check all.  */
      {
        if ((assignee_keyword == TOKEN_KEYWORD_OCELOT_EXPLORER_ACTION)
         || (assignee_keyword == TOKEN_KEYWORD_OCELOT_EXPLORER_ENABLED)
         || (assignee_keyword == TOKEN_KEYWORD_OCELOT_EXPLORER_SHORTCUT)
         || (assignee_keyword == TOKEN_KEYWORD_OCELOT_EXPLORER_TEXT))
        {
          hparse_f_error();
          return 0;
        }
      }
    }
    return TOKEN_KEYWORD_SET;
  }
  else if ((slash_token == TOKEN_KEYWORD_SOURCE) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SOURCE, "SOURCE") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    hparse_f_other(0);
    if (hparse_errno > 0) return 0;
  }
  else if ((slash_token == TOKEN_KEYWORD_STATUS) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_STATUS, "STATUS") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
  else if ((slash_token == TOKEN_KEYWORD_SYSTEM) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_SYSTEM, "SYSTEM") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    hparse_f_other(1);
    if (hparse_errno > 0) return 0;
  }
  else if ((slash_token == TOKEN_KEYWORD_TEE) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_TEE, "TEE") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
    hparse_f_other(1);
    if (hparse_errno > 0) return 0;
  }
  else if ((slash_token == TOKEN_KEYWORD_USE) || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_USE, "USE") == 1))
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
#if (FLAG_VERSION_MARIADB_12_0 != 0)
    hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_CATALOG, "CATALOG");
#endif
    if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_DATABASE,TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
    {
      if (hparse_f_literal(TOKEN_REFTYPE_DATABASE, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();
    }
    /* In mysql client, garbage can follow. It's not documented so don't call hparse_f_other(). */
    if (hparse_errno > 0) return 0;
  }
  else if ((slash_token == TOKEN_KEYWORD_WARNINGS) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY,TOKEN_KEYWORD_WARNINGS, "WARNINGS")) == 1)
  {
    if (slash_token <= 0) main_token_flags[hparse_i_of_last_accepted] |= TOKEN_FLAG_IS_START_STATEMENT;
  }
#if (OCELOT_MYSQL_DEBUGGER == 1)
  else if ((hparse_token.mid(0, 1) == "$")
    && ((hparse_dbms_mask & FLAG_VERSION_MYSQL_OR_MARIADB_ALL) != 0))
  {
    main_token_flags[hparse_i] |= TOKEN_FLAG_IS_START_STATEMENT;
    /* TODO: We aren't parsing $debug statements well */
    if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_BREAKPOINT, "$BREAKPOINT", 2) == 1)
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION_OR_PROCEDURE, TOKEN_REFTYPE_FUNCTION_OR_PROCEDURE) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();

      if (hparse_errno > 0) return 0;
      if ((hparse_token.length() == 0) || (hparse_token == ";")) return 1;
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_CLEAR, "$CLEAR", 3) == 1)
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION_OR_PROCEDURE, TOKEN_REFTYPE_FUNCTION_OR_PROCEDURE) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();

      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_CONTINUE, "$CONTINUE", 3) == 1)
    {
      ;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_DEBUG, "$DEBUG", 4) == 1)
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION_OR_PROCEDURE, TOKEN_REFTYPE_FUNCTION_OR_PROCEDURE) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        if (hparse_token != ")")
        {
          do
          {
            if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0)
            {
              hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
            }
            if (hparse_errno > 0) return 0;
          } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
        }
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ")");
      }
    }
     //|| (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_DELETE, "$DELETE") == 1)
     //|| (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_EXECUTE, "$EXECUTE") == 1)
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_EXIT, "$EXIT", 4) == 1)
    {
      ;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_INFORMATION, "$INFORMATION", 4) == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "STATUS");
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_INSTALL, "$INSTALL", 4) == 1)
    {
      ;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_LEAVE, "$LEAVE", 2) == 1)
    {
      ;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_NEXT, "$NEXT", 2) == 1)
    {
      ;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_REFRESH, "$REFRESH", 8) == 1)
    {
      if ((hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "BREAKPOINTS") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "CALL_STACK") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "PREPARED_STATEMENTS") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "SERVER_VARIABLES") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "USER_VARIABLES") == 1)
       || (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_KEYWORD, "VARIABLES") == 1))
        {;}
      else hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_SET, "$SET", 4) == 1)
    {
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_VARIABLE,TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
      if (hparse_errno > 0) return 0;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();

      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_SETUP, "$SETUP", 5) == 1)
    {
      while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "-") == 1)
      {
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_SWITCH_NAME,TOKEN_TYPE_IDENTIFIER, "TRACK_STATEMENTS") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_SWITCH_NAME,TOKEN_TYPE_IDENTIFIER, "TRACK_USER_VARIABLES") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_SWITCH_NAME,TOKEN_TYPE_IDENTIFIER, "TRACK_DECLARED_VARIABLES") == 1) {;}
        else hparse_f_error();
        if (hparse_errno > 0) return 0;
        hparse_f_expect(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, "=");
        if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "0") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "1") == 1) {;}
        else if (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_LITERAL, "2") == 1) {;}
        else hparse_f_error();
        if (hparse_errno > 0) return 0;
      }
      do
      {
        if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION_OR_PROCEDURE, TOKEN_REFTYPE_FUNCTION_OR_PROCEDURE) == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY,TOKEN_TYPE_OPERATOR, ","));
    }
     //|| (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_SKIP, "$SKIP") == 1)
     //|| (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_SOURCE, "$SOURCE") == 1)
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_STEP, "$STEP", 3) == 1)
    {
      ;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_TBREAKPOINT, "$TBREAKPOINT", 2) == 1)
    {
      if (hparse_f_qualified_name_of_object(0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION_OR_PROCEDURE, TOKEN_REFTYPE_FUNCTION_OR_PROCEDURE) == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();

      if (hparse_errno > 0) return 0;
      if ((hparse_token.length() == 0) || (hparse_token == ";")) return 1;
      hparse_f_opr_1(0, 0);
      if (hparse_errno > 0) return 0;
    }
    else return 0;
    return 1;
  }
#endif //if (OCELOT_MYSQL_DEBUGGER == 1)
  else
  {
    return 0; /* not client statement */
  }
  return 1; /* client statement but not SET */
}

#ifdef DBMS_TARANTOOL
/*
  See ocelotgui.h comments under heading "Tarantool comments".

  Syntax check (!! THIS COMMENT MAY BE OBSOLETE!)
  Use the recognizer for a small subset of MySQL/MariaDB syntax ...
  DELETE FROM identifier WHERE identifier = literal [AND identifier = literal ...];
  INSERT INTO identifier VALUES (literal [, literal...]);
  REPLACE INTO identifier VALUES (literal [, literal...]);
  SELECT * FROM identifier [WHERE identifier <comparison-operator> literal [AND identifier <comparison-operator> literal ...]];
  UPDATE identifier SET identifier=value [, identifier=value...]
                    WHERE identifier = literal [AND identifier = literal ...];
  SET identifier = expression [, identifier = expression ...]
  Legal comparison-operators within SELECT are = > < >= <=
  Comments are legal anywhere.
  Todo: Keywords should not be reserved, for example DELETE FROM INTO WHERE SELECT=5; is legal.
  We call tparse_f_block(0) for "SELECT|INSERT|DELETE / * NOSQL * / ..."
  Todo: with the current arrangement we never could reach "TRUNCATE".
  Todo: with the current arrangement we never could reach "SET".
  Todo: UPDATE fails.
*/

/* These items are permanent and are initialized in parse_f_program */
static int tparse_iterator_type= TARANTOOL_BOX_INDEX_EQ;
static int tparse_indexed_condition_count= 0;

/*
 factor = identifier | literal | "(" expression ")" .
*/
void MainWindow::tparse_f_factor()
{
  if (hparse_errno > 0) return;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_COLUMN, TOKEN_TYPE_IDENTIFIER, "[identifier]"))
  {
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_LITERAL, "[literal]"))
  {
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "("))
  {
    if (hparse_errno > 0) return;
    tparse_f_expression();
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
  }
  else
  {
    hparse_f_error();
    return;
  }
}

/*
  term = factor {("*"|"/") factor}
*/
void MainWindow::tparse_f_term()
{
  if (hparse_errno > 0) return;
  tparse_f_factor();
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "*") == 1) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "/") == 1))
  {
    tparse_f_factor();
    if (hparse_errno > 0) return;
  }
}

/*
   expression = ["+"|"-"] term {("+"|"-") term}
*/
void MainWindow::tparse_f_expression()
{
  if (hparse_errno > 0) return;
  if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "+") == 1) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "-") == 1)) {;}
  tparse_f_term();
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "+") == 1) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "-") == 1))
  {
    tparse_f_term();
    if (hparse_errno > 0) return;
  }
}

/*
  restricted expression = ["+"|"-"] literal
*/
void MainWindow::tparse_f_restricted_expression()
{
  if (hparse_errno > 0) return;
  if ((hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "+") == 1) || (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "-") == 1)) {;}
  if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();
}

/*
 condition =
     identifier ("="|"<"|"<="|"="|">"|">=") literal
     [AND condition ...]
*/
void MainWindow::tparse_f_indexed_condition(int keyword)
{
  if (hparse_errno > 0) return;
  do
  {
    if (tparse_indexed_condition_count >= 255)
    {
      hparse_expected= "no more conditions. The maximum is 255 (box.schema.INDEX_PART_MAX).";
      hparse_f_error();
      return;
    }
    int comp_op= -1;
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_COLUMN, TOKEN_TYPE_IDENTIFIER, "[field identifier]") == 0)
    {
      hparse_expected= "field identifier with the format: ";
      hparse_expected.append(TARANTOOL_FIELD_NAME_BASE);
      hparse_expected.append("_ followed by an integer greater than zero. ");
      hparse_expected.append("Maximum length = 64. ");
      hparse_expected.append(QString::number(TARANTOOL_MAX_FIELD_NAME_LENGTH));
      hparse_expected.append(". For example: ");
      hparse_expected.append(TARANTOOL_FIELD_NAME_BASE);
      hparse_expected.append("_1");
      hparse_f_error();
      return;
    }

    if (tparse_indexed_condition_count > 0)
    {
      int ok= 0;
      if ((hparse_token == "<")
       && (tparse_iterator_type == TARANTOOL_BOX_INDEX_LE)
       && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "<") == 1)) {;}
      else if ((hparse_token == ">")
            && (tparse_iterator_type == TARANTOOL_BOX_INDEX_GE)
            && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ">") == 1)) ++ok;
      else if ((hparse_token == "=")
            && (tparse_iterator_type == TARANTOOL_BOX_INDEX_EQ)
            && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "=") == 1)) ++ok;
      else if ((hparse_token == "<=")
            && (tparse_iterator_type == TARANTOOL_BOX_INDEX_LE)
            && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "<=") == 1)) ++ok;
      else if ((hparse_token == ">=")
            && (tparse_iterator_type == TARANTOOL_BOX_INDEX_GE)
            && (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ">=") == 1)) ++ok;
      if (ok == 0)
      {
        hparse_expected= "A conditional operator compatible with previous ones. ";
        hparse_expected.append("When there is more than one ANDed condition, ");
        hparse_expected.append("allowed combinations are: > after a series of >=s, ");
        hparse_expected.append("or < after a series of <=s, or all =s, or all >=s, or all <=s.");
        hparse_f_error();
        return;
      }
    }
    else
    {
      if (keyword == TOKEN_KEYWORD_SELECT)
      {
        if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "=") == 1) comp_op= TARANTOOL_BOX_INDEX_EQ;
        else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "<") == 1) comp_op= TARANTOOL_BOX_INDEX_LT;
        else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "<=") == 1) comp_op= TARANTOOL_BOX_INDEX_LE;
        else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "=") == 1) comp_op= TARANTOOL_BOX_INDEX_EQ;
        else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ">") == 1) comp_op= TARANTOOL_BOX_INDEX_GT;
        else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ">=") == 1) comp_op= TARANTOOL_BOX_INDEX_GE;
        else hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else
      {
        hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "=");
        if (hparse_errno > 0) return;
        comp_op= TARANTOOL_BOX_INDEX_EQ;
      }
    }
    tparse_iterator_type= comp_op;
    ++tparse_indexed_condition_count;
    if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "AND"));
}


/*
 unindexed condition =
     expression ("="|"<"|"<="|"="|">"|">=") expression
     [AND|OR condition ...]
  For a sequential search, i.e. a full-table scan or a filter of the
  rows selected by indexed conditions, we can have OR as well as AND,
  expressions as well as identifiers, expressions as well as literals,
  and <> as well as other comp-ops.
  May implement for a HAVING clause somday.
*/
//void MainWindow::tparse_f_unindexed_condition()
//{
//  if (hparse_errno >0) return;
//  do
//  {
//    if (hparse_errno > 0) return;
//    tparse_f_expression();
//    if (hparse_errno > 0) return;
//    /* TODO: THIS IS NOWHERE NEAR CORRECT! THERE MIGHT BE MORE THAN ONE OPERAND! */
//    {
//      if ((hparse_token == "=")
//      || (hparse_token == "<>")
//      || (hparse_token == "<")
//      || (hparse_token == "<=")
//      || (hparse_token == ">")
//      || (hparse_token == ">="))
//      {
//        if (hparse_token == "=") tparse_iterator_type= TARANTOOL_BOX_INDEX_EQ;
//        if (hparse_token == "<>") tparse_iterator_type= TARANTOOL_BOX_INDEX_ALL; /* TODO: NO SUPPORT */
//        if (hparse_token == "<") tparse_iterator_type= TARANTOOL_BOX_INDEX_LT;
//        if (hparse_token == "<=") tparse_iterator_type= TARANTOOL_BOX_INDEX_LE;
//        if (hparse_token == ">") tparse_iterator_type= TARANTOOL_BOX_INDEX_GT;
//        if (hparse_token == ">=") tparse_iterator_type= TARANTOOL_BOX_INDEX_GE;
//        hparse_f_nexttoken();
//        parse_f_expression();
//      }
//      else hparse_f_error();
//    }
//  } while (hparse_f_accept(FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_IDENTIFIER, "AND"));
//}


void MainWindow::tparse_f_assignment()
{
  do
  {
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_VARIABLE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "=");
    if (hparse_errno > 0) return;
    tparse_f_expression();
    if (hparse_errno > 0) return;
    } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ","));
}

/*
statement =
    "insert" [into] ident "values" (literal {"," literal} )
    | "replace" [into] ident "values" (number {"," literal} )
    | "delete" "from" ident "where" condition [AND condition ...]
    | "select" * "from" ident ["where" condition [AND condition ...]]
    | "set" ident = number [, ident = expression ...]
    | "truncate" "table" ident
    | "update" ident "set" ident=literal {"," ident=literal} WHERE condition [AND condition ...]
*/
void MainWindow::tparse_f_statement()
{
  if (hparse_errno > 0) return;
  if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "INSERT"))
  {
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_INSERT;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "INTO");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "VALUES");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "(");
    do
    {
      if (hparse_errno > 0) return;
      tparse_f_restricted_expression();
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ","));
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "REPLACE"))
  {
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_REPLACE;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "INTO");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "VALUES");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "(");
    do
    {
      if (hparse_errno > 0) return;
      if (hparse_f_literal(TOKEN_REFTYPE_ANY, FLAG_VERSION_ALL, TOKEN_LITERAL_FLAG_ANY) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ","));
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "DELETE"))
  {
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_DELETE;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "FROM");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "WHERE");
    if (hparse_errno > 0) return;
    tparse_f_indexed_condition(TOKEN_KEYWORD_DELETE);
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "TRUNCATE"))
  {
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_TRUNCATE;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "TABLE");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "SELECT"))
  {
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_SELECT;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "*");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "FROM");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "WHERE"))
    {
      tparse_f_indexed_condition(TOKEN_KEYWORD_SELECT);
      if (hparse_errno > 0) return;
    }
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "UPDATE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_UPDATE;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_TABLE, TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "SET");
    if (hparse_errno > 0) return;
    tparse_f_assignment();
    if (hparse_errno > 0) return;
    hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "WHERE");
    if (hparse_errno > 0) return;
    tparse_f_indexed_condition(TOKEN_KEYWORD_UPDATE);
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_KEYWORD, "SET"))
  {
    hparse_statement_type= TOKEN_KEYWORD_SET;
    tparse_f_assignment();
    if (hparse_errno > 0) return;
  }
  else
  {
    hparse_f_error();
  }
}

/*
  statement
*/
void MainWindow::tparse_f_block(int calling_type)
{
  (void) calling_type; /* to avoid "unused parameter" warning */
  if (hparse_errno > 0) return;
  tparse_iterator_type= TARANTOOL_BOX_INDEX_EQ;
  tparse_indexed_condition_count= 0;
  tparse_f_statement();
}

void MainWindow::tparse_f_program(QString text)
{
  //tarantool_errno[connection_number]= 0; /* unnecessary, I think */

  hparse_text_copy= text;
  hparse_token= "";
  hparse_i= -1;
  hparse_expected= "";
  hparse_errno= 0;
  hparse_token_type= 0;
  hparse_statement_type= -1;
  hparse_f_nexttoken();
  tparse_f_block(0);
  if (hparse_errno > 0) return;
  /* If you've got a bloody semicolon that's okay too */
  if (hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, ";")) return;
  hparse_f_expect(FLAG_VERSION_ALL, TOKEN_REFTYPE_ANY, TOKEN_TYPE_OPERATOR, "[eof]"); /* was: parse_expect(TOKEN_KEYWORD_PERIOD); */
  if (hparse_errno > 0) return;
}
#endif
