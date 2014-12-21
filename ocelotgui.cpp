/*
  ocelotgui -- Ocelot GUI Front End for MySQL or MariaDB

   Version: 0.2.0 Alpha
   Last modified: December 10 2014
*/

/*
  Copyright (c) 2014 by Ocelot Computer Services Inc. All rights reserved.

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
   The class named CodeEditor inside this program is taken and modified from
   http://qt-project.org/doc/qt-4.8/widgets-codeeditor.html
   and therefore CodeEditor's maker's copyright and BSD license provisions
   are reproduced below. These provisions apply only for the
   part of the CodeEditor class which is marked within the program.
   The program as a whole is copyrighted by Ocelot and
   licensed under GPL version 2 as stated above.
 **
 ** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** $QT_BEGIN_LICENSE:BSD$
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
 **     of its contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
*/


/*
  General comments

  These are comments about the code.
  The user manual and blog comments are elsewhere; look at manual.htm
  or README.txt or ocelot.ca or ocelot.ca/blog or the repository that
  this program was downloaded from, probably github.com/ocelot-inc/ocelotgui.

  The code files are:
    ocelotgui.pro       small. used by Qt qmake
    ocelotgui.h         headers but also quite a lot of executable
    ocelotgui.cpp       executable
    ocelotgui.ui        small. ui = user interface

  There are three main widgets, which generally appear top-to-bottom on
  the screen: history_edit_widget = an uncomplicated text edit which gets
  retired statements and diagnostic messages; statement_edit_widget =
  a class derived from text edit ("CodeEditor") which has code for a
  prompt on the left and for syntax highlighting; result_grid_table_widget =
  a class derived from widget ("ResultGrid") which has code for tabular
  display of a result set and manipulation thereof. The result grid is
  not based on Qt's high-level tools like QTableView, it is constructed
  from basic building blocks (scroll bars and repeatedly-occurring
  QTextEdits within QFrames). There is no use of Qt's "MySQL driver".
  The code includes a tokenizer for MySQL syntax, but not a parser.
  The coding style is generally according to MySQL coding guidelines
  http://dev.mysql.com/doc/internals/en/coding-guidelines.html
  but lines may be long, and sometimes spaces occur at ends of lines,
  and "if (x) y" may be preferred to "if (x) <newline> y".

  The code was written by Ocelot Computer Services Inc. employees, except
  for about 50 lines in the CodeEditor section (Digia copyright / BSD license).
  Other contributors will be acknowledged here and/or in a "Help" display.

  The code #includes header files from MySQL/Oracle and from Qt/Digia,
  and relies on the MySQL client library and the Qt core, gui,
  and widgets libraries. Builds have been successful with several
  Linux distros and gcc 4.6. Build instructions are in the user manual or
  in a readme file

  There are many comments. Searching for the word "Todo:" in the comments
  will give some idea of what's especially weak and what's planned.

  UTF8 Conversion
  Everything which will be passed to MySQL will first be converted to UTF8.
  Todo: Ensure that sending/receiving character set is UTF8, regardless of data language.
  Todo: When receiving, only convert characters, not binary or numeric.
  Todo: Handle Qt codecs for other character sets.
  Todo: Figure out how to handle a literal preceded by an _introducer.
  A typical set of instructions will look like this and have the comment
  "See comment "UTF8 Conversion".
     int tmp_len= QStr.toUtf8().size());                  ... See how big UTF8 will be
     char *tmp= new char[tmp_len + 1];                      ... Allocate that amount
     memcpy(tmp, QStr.toUtf8().constData()), tmp_len + 1);  ... Copy including trail '\0' byte
     ... And later, say "delete []tmp" or hope for garbage collection

  Todo: Allow right-click for changing individual widgets.

  Todo: Some more checks for garbage collection, then some equivalent to valgrind.

  The usual ways to build are described in README.txt.
  An unusual way would be with Qt 4.8 source-code libraries supplied by Digia:
      Download 4.8 libraries via http://qt-project.org/downloads
         This is a source download; you'll need to do ./configure and make.
         Follow instructions in http://qt-project.org/doc/qt-4.8/install-x11.html
         After make and make install you'll have /usr/local/Trolltech/Qt-4.8.6
         At this point I got totally stuck with Qt Creator -- it refused to try to build with Qt 4.8.
         And yes, I changed the kit, I changed the paths, but it was all a waste of a lot of time.
         Well, if anyone else succeeds, let me know.
         But I "succeeded" by exiting Qt Creator, creating a new make file, and building from the shell.
         (I copy and then remove ui_ocelotgui.h, which may or may not have some importance that I missed.)
         cd ~/ocelotgui
         mv ui_ocelotgui.h ui_ocelotgui.h.bak
         /usr/local/Trolltech/Qt-4.8.6/bin/qmake ocelotgui.pro
         make
        ./ocelotgui
        That ruins what Qt Creator expects when I get back to Qt Cretaor, so I get back to 5.1 with:
        cd ~/ocelotgui
        /home/pgulutzan/Qt/5.1.1/gcc_64/bin/qmake ./ocelotgui.pro
        mv ui_ocelotgui.h ui_ocelotgui.h.bak
        rm ui_ocelotgui.h
        make (ignore failure)
        re-enter Qt Creator. Build | Clean all. Build all. Run.
*/


#include "ocelotgui.h"
#include "ui_ocelotgui.h"

MYSQL mysql; /* Global */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w (argc, argv);
    w.showMaximized();
    return a.exec();
}


MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  /* Initialization */

  ui->setupUi(this);              /* needed so that the menu will show up */

  setWindowTitle("ocelotgui");

  is_mysql_connected= 0;
  mysql_res= 0;

  /* client variable defaults */
  /* todo: these should be set up earlier in connect_mysql_options_2 because users should be able to pick them as options */
  ocelot_dbms= "mysql";
  ocelot_grid_detached= 0;
  ocelot_grid_max_row_lines= 5; /* obsolete? */               /* maximum number of lines in 1 row. warn if this is exceeded? */
  connect_mysql_options_2(argc, argv);               /* pick up my.cnf and command-line MySQL-related options, if any */

  ocelot_statement_prompt_background_color= "lightGray"; /* set early because create_widget_statement() depends on this */

  /* Probably result_grid_table_widget_saved_font only matters if the connection dialog box has to go up. */
  QFont tmp_font;
  QFont *saved_font;
  tmp_font= this->font();
  saved_font=&tmp_font;
  result_grid_table_widget= new ResultGrid(0, saved_font, this);

  main_layout= new QVBoxLayout();               /* todo: do I need to say "this"? */

  create_widget_history();
  create_widget_statement();

  main_window= new QWidget();

  /*
    Defaults for items that can be changed with Settings menu item.
    Todo: These should come from current values, after processing of any Qt options on the command line.
          We already get "current values" somewhere.
  */
  set_current_colors_and_font(); /* set ocelot_statement_color, ocelot_grid_color, etc. */
  ocelot_statement_border_color= "black";

  ocelot_statement_highlight_literal_color= "green";
  ocelot_statement_highlight_identifier_color= "blue";
  ocelot_statement_highlight_comment_color= "red";
  ocelot_statement_highlight_operator_color= "lightGreen";
  ocelot_statement_highlight_reserved_color= "magenta";

  ocelot_grid_border_color= "black";
  ocelot_grid_header_background_color= "lightGray";
  ocelot_grid_cell_right_drag_line_color= "lightBlue";
  ocelot_grid_cell_border_color= "black";
  ocelot_grid_cell_border_size= "1";
  ocelot_grid_cell_right_drag_line_size= "5";

  ocelot_history_border_color= "black";
  ocelot_main_border_color= "black";

  make_style_strings();
  main_layout->addWidget(history_edit_widget);
  main_layout->addWidget(result_grid_table_widget);
  main_layout->addWidget(statement_edit_widget);

  main_window->setLayout(main_layout);
  setCentralWidget(main_window);

  create_menu();    /* Do this at a late stage because widgets must exist before we call connect() */

  create_the_manual_widget(); /* Make the text for Help | The Manual */

  /*
    If the command-line option was -p but not a password, then password input is necessary
    so put up the connection dialog box. Otherwise try immediately to connect.
    Todo: better messages so the user gets the hint: connection is necessary / password is necessary.
  */
  if (ocelot_password_was_specified == 1)
  {
    statement_edit_widget->insertPlainText("CONNECT");
    action_execute();
  }
  else
  {
    action_connect();
  }
  statement_edit_widget->setFocus(); /* Show user we're ready to accept a statement in the statement edit widget */
}


MainWindow::~MainWindow()
{
  delete ui;
}


/*
  Create statement_edit_widget.
  All user SQL input goes into statement_edit_widget.
  This will be a CodeEditor, which is a class derived from QTextEdit, with a prompt on the left.
  Todo: Check: after font change or main window resize, are line numbers and text in sync?
*/
void MainWindow::create_widget_statement()
{
  statement_edit_widget= new CodeEditor();
  statement_edit_widget->setLineWrapMode(QPlainTextEdit::NoWrap);
  /* statement_edit_widget->setAcceptRichText(false); */ /* Todo: test whether this works */
  connect(statement_edit_widget->document(), SIGNAL(contentsChanged()), this, SLOT(action_statement_edit_widget_text_changed()));
  statement_edit_widget_text_changed_flag= 0;

  /*
    Defaults.
    The prompt can be changed with a "prompt" statement or by my.cnf contents.
    The widget-left i.e. prompt bgcolor can be changed with a menu item.
  */
  statement_edit_widget->statement_edit_widget_left_bgcolor= QColor(ocelot_statement_prompt_background_color);
  statement_edit_widget->statement_edit_widget_left_treatment1_textcolor= QColor(ocelot_statement_color);

  statement_edit_widget->prompt_default= (QString)"mysql>";     /* Todo: change to "\N [\d]>"? */
  statement_edit_widget->prompt_as_input_by_user= statement_edit_widget->prompt_default;

  statement_edit_widget->statement_count= 0;
  statement_edit_widget->dbms_version= (QString)"";
  statement_edit_widget->dbms_database= (QString)"";
  statement_edit_widget->dbms_port= (QString)"";
  statement_edit_widget->dbms_host= (QString)"";
  statement_edit_widget->dbms_current_user= (QString)"";
  statement_edit_widget->dbms_current_user_without_host= (QString)"";
  statement_edit_widget->delimiter= ocelot_delimiter_str;
  statement_edit_widget->result= (QString)"";

  /*
    Problem: the above statements don't cause recalculation of width of prompt on the left.
    (Original calculation happens when widget is created.)
    I can force recalculation by doing an "emit" here.
    But I'm not sure this is the right way to do it.
  */
  emit statement_edit_widget->update_prompt_width(0);

  statement_edit_widget->installEventFilter(this);
}


/*
  The event filter, for detecting:
    Has the user pressed on the vertical scroll bar of result_grid_widget?
    Has the user pressed Enter or return on statement_edit_widget?
      If the user presses Enter or Return on statement_edit_widget,
      and the last non-comment token is the delimiter (normally ";" unless DELIMITER happened),
      and the cursor is at the end i.e. after the delimiter,
      that should cause execution.
      Otherwise return false ...
      I've seen examples where, instead of "return false;", the
      instruction is "return QMainWindow::eventFilter(obj, event);".
      I think the idea there is to go direct to the main processor
      for text editing, bypassing other event filters.
  Todo: Consider: Perhaps this should not be in Main>Window:: but in CodeEditor::.
  Todo: Consider: Perhaps this should be a menu item, not a filter event.
                  (There's already a menu item, but it's not for Enter|Return.)
  There are a few "Ocelot keyword" items that do not require ";" or delimiter
  provided they're the first word, for example "QUIT".
*/
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
  QString text;

  if (obj == result_grid_table_widget->grid_vertical_scroll_bar)
  {
    /*
      Probably some of these events don't cause scroll bar value to change,
      I've only seen that happen for MouseMove and MouseButtonRelease.
      But that's harmless, it only means we call scroll_event for nothing.
    */
    if ((event->type() == QEvent::KeyPress)
    ||  (event->type() == QEvent::KeyRelease)
    ||  (event->type() == QEvent::MouseButtonDblClick)
    ||  (event->type() == QEvent::MouseButtonPress)
    ||  (event->type() == QEvent::MouseButtonRelease)
    ||  (event->type() == QEvent::MouseMove)
    ||  (event->type() == QEvent::MouseTrackingChange)) return (result_grid_table_widget->scroll_event());
  }

  if (obj == result_grid_table_widget)
  {
    if (event->type() == QEvent::FontChange) return (result_grid_table_widget->fontchange_event());
    if (event->type() == QEvent::Show) return (result_grid_table_widget->show_event());
  }

  if (obj != statement_edit_widget) return false;
  if (event->type() != QEvent::KeyPress) return false;
  QKeyEvent *key= static_cast<QKeyEvent *>(event);

  if ((key->key() != Qt::Key_Enter) && (key->key() != Qt::Key_Return)) return false;
  /* No delimiter needed if first word in first statement of the input is an Ocelot keyword e.g. QUIT */
  /* No delimiter needed if Ctrl+Enter, which we'll regard as a synonym for Ctrl+E */
  if ((main_statement_type >= TOKEN_KEYWORD_QUESTIONMARK) || (key->modifiers() & Qt::ControlModifier))
  {
    emit action_execute();
    return true;
  }
  text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */
  int i= 0;
  while (main_token_lengths[i] != 0) ++i;
  QString s= "";
  /*
    Todo: I think the following was written before there was detection of reserved words + Ocelot keywords.
    If so, token_type() doesn't need to be invoked, because main_token_types[] has the result already.
  */
  while (i > 0)
  {
    --i;
    s= text.mid(main_token_offsets[i], main_token_lengths[i]);
    int t= token_type(s.data(), main_token_lengths[i]);
    if (t == TOKEN_TYPE_COMMENT_WITH_SLASH) continue;
    if (t == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE) continue;
    if (t == TOKEN_TYPE_COMMENT_WITH_MINUS) continue;
    break;
  }

  if (s != ocelot_delimiter_str) return false;
  if (statement_edit_widget->textCursor().position() <= main_token_offsets[i]) return false;
  /* All conditions have been met. Execute, and eat the return key. */
  emit action_execute();
  return true;
}


/*
  History
  =======

  The history widget history_edit_widget is an editable QTextEdit
  which contains retired statements + errors/warnings, scrolling
  so it looks not much different from the mysql retired-statement
  scrolling. However, our history does not include result sets.
  These user-settable variables affect history_edit_widget:
  ocelot_history_includes_prompt                   default yes
  ocelot_history_text_select_includes_prompt       default yes
  ocelot_history_max_lines                         default 10000. 0 means it's suppressed.
  ocelot_history_text_font                         default unknown
  ocelot_history_error_font                        default unknown
  ocelot_history_prompt_font                       default unknown
  ocelot_history_detached                          default no
  ocelot_history_includes_warnings                 default no

  The statement is always followed by an error message,
  but @ocelot_history_includes_warnings is affected by ...
  warnings  (\W) Show warnings after every statement.
  nowarning (\w) Don't show warnings after every statement.
  If the prompt is included, then we should be saving time-of-day
  for the first statement-line prompt and doing prompt_translate()
  for each line of the statement when we copy it out.
  In mysql client it's possible to say up-arrow and get a prior statement,
  but we have up-arrow for different purposes, probably PgUp|PgDown is better,
  or alt+up-arrow|alt+down-arrow.
  I didn't see a need to make the widget with a subclass of QTextEdit.
  The history_edit_widget is QTextEdit, differing from statement_edit_widget
  which is CodeEditor which is derived from QPlainTextEdit.

  History menu items / commands:
  * The usual edit menu items = cut, copy, paste, etc.
    Therefore we don't really need to limit history size, users can clear.
  * PgUp + PgDn
    * are menu items for "previous statement" / "next statement"
    * disable if there's no previous / next
    * if done on statement widget, brings in from history -- dunno if history should scroll when that happens
    ? when bringing in from history to statement, don't re-execute
    * if user executes a restated statement, there's a behaviour choice: now the restated statement is last statement, or now the last-picked statement is still last statement
  * Open | Save | Close | Delete
    * file items, with the intent of keeping history
    * you can go back in history by going back in a file
    * format should be the same as MySQL log history
      ? but in that case, should display look like MySQL log too?
    * Save means "save what's being shown, so it's affected by Hide
  * Find | Goto
    * you need these menu items for history soon, so might as well have them for statements too
  * Hide prompt + Hide result + Hide result set
  + Settings: History Widget: Colors and Fonts has different colors for prompt/statement/result/result set

  Comments unrelated to HTML work:
  Possible difficulty: our history is statement-at-a-time, not screen-at-a-time", if there are multi statements
  Certain statements might be suppressed
    * should Quit and Source go in history?
    * should only data-change statements go in history?
    * should client-statements be suppressed?
    * this is a "Filter" matter
    ? to what extent is this useful for audit?
    Wherever there's a menu item, there should also be a client-statement
    * history must include comments even if they're not sent to the server (there's some mysql-option for that)
    * Delimiter might change, so when you copy from history to statement, what should you do?
    * When the debugger comes in, statements done by the debugger are a special history category

  HTML in the history widget
  Unlike statement_edit_widget, history_edit_widget is QTextEdit and allows rich text.
  History is an editable QTextEdit which contains HTML.
  Each history entry has: prompt, statement, result, and possibly result set.
  (Result set is unimplemented but will be mentioned in these comments;
  eventually it will be something that appears like mysql client result set.)
  There are two kinds of markup:
  (1) <span style="...">...</span> for changes to color + font.
      Currently this is only used for history prompt bgcolor = statement prompt bgcolor.
  (2) <a name='[mark]'> for showing what kind of text follows.
      The markups are <a name='STATEMENT START'> ... <a name='STATEMENT END'>
                <a name='PROMPT START'> ... <a name='PROMPT END'>
                <a name='RESULT'> (ends when statement ends)
                <a name='ENTITY'> (always an entity for a single & character)
      Digia does not document or guarantee what it will do with <a>
      and we're not using it for its intended function as anchor.
      Example: "SELECT * FROM t <br> WHERE x < '&';" becomes
      <a name='STATEMENT START'>
      <span ...><a name='PROMPT'>mysql&gt;<a name='PROMPT END'></span>
      SELECT * FROM t <br>
      <a name='PROMPT'>&gt;<a name='PROMPT END'>
      WHERE a &lt; '<a name='ENTITY'>&amp;';
      <span ...><a name='RESULT'>OK</span>
      <a name='STATEMENT END'>
  Handle PgUp | PgDn by picking up next | previous STATEMENT START ... END.
  When copying to history, change < and > and & to entities but mark if '&'.
    Also: change ' and " to entities.
  When copying from history, change entities to < and > and & unless marked.
  Todo: hide prompt + result by putting whole thing within <a name='...'>.
  In order to see the HTML, say:
    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::PlainText);
    msgBox.setText(history_edit_widget->toHtml());
    msgBox.exec();
  History is editable.
    If the user inputs "<" when editing the history,
    change that to an entity immediately -- but maybe Qt will do that.
  Idea: use QTextCursor to change cursor in history if PgUp|PgDn in statement.
  Idea: multiple types of result: warning, error, result set
*/

void MainWindow::create_widget_history()
{
  history_edit_widget= new QTextEdit();
  history_edit_widget->setReadOnly(false);       /* if history shouldn't be editable, set to "true" here */
  history_edit_widget->hide();                   /* hidden until a statement is executed */
  ocelot_history_includes_warnings= 0;           /* include warning(s) returned from statement? default = no. */
  history_markup_make_strings();
  return;
}


/*
  Set the strings that will be used for markup comments in history widget.
  Call history_markup_make_strings() at program start.
  Todo: At the moment these could be constants but I don't think they always will be.
  Note: <br> in history_markup_statement_start is necessary; else prompt_start is ignored.
*/
void MainWindow::history_markup_make_strings()
{
  history_markup_statement_start= "<a name=\"STATEMENT START\"></a><br>";
  history_markup_statement_end=   "<a name=\"STATEMENT END\"></a>";
  history_markup_prompt_start=    "<a name=\"PROMPT START\"></a>";
  history_markup_prompt_end=      "<a name=\"PROMPT END\"></a>";
  history_markup_result=          "<a name=\"RESULT\"></a>";
  history_markup_entity=          "<a name=\"ENTITY\"></a>";
}


/* The following will append the statement to history, line-at-a-time with prompts. */
/* It seems to work except that the prompt is not right-justified. */
/* Todo: right justify. Make it optional to show the prompt, unless prompt can be hidden. */
void MainWindow::history_markup_append()
{
  QString plainTextEditContents;
  QStringList statement_lines;
  int statement_line_index;
  QString history_statement;

  plainTextEditContents= query_utf16; /* Todo: consider: why bother copying rather than using query_uitf16? */
  statement_lines= plainTextEditContents.split("\n");
  //statement_line_index= 0;                                                          /* Todo throw this useless line away */
  /* Todo: There should be a better way to ensure that Qt realizes the whole widget is rich text. */
  /* Todo: Some of this could be at start of history_edit_widget but what would happen if I cleared the whole area? */
  /* Todo: background-color of prompt could be settable for history widget, rather than = statement background color. */
  /* Todo: there's a weird bug that splits prompt into 'm' and 'ysql>', but the output still looks okay. */
  history_statement= "<i></i>";                                           /* hint that what's coming is HTML */
  history_statement.append(history_markup_statement_start);

  for (statement_line_index= 0; statement_line_index < statement_lines.count(); ++statement_line_index)
  {
    history_statement.append("<span style=\" background-color:");
    history_statement.append(ocelot_statement_prompt_background_color);
    history_statement.append(";\">");
    history_statement.append(history_markup_prompt_start);
    history_statement.append(history_markup_copy_for_history(statement_edit_widget->prompt_translate(statement_line_index + 1)));
    history_statement.append(history_markup_prompt_end);
    history_statement.append("</span>");
    history_statement.append(history_markup_copy_for_history(statement_lines[statement_line_index]));
    history_statement.append("<br>");
  }

  history_statement.append(history_markup_result);
  history_statement.append(history_markup_copy_for_history(statement_edit_widget->result)); /* the main "OK" or error message */
  history_statement.append(history_markup_statement_end);

  history_edit_widget->append(history_statement);

  history_markup_counter= 0;
}

/* When copying to history, change < and > and & and " to entities. */
QString MainWindow::history_markup_copy_for_history(QString inputs)
{
  QString outputs;
  QString c;
  int i;

  outputs= "";
  for (i= 0; i < inputs.length(); ++i)
  {
    c= inputs.mid(i, 1);
    if (c == "<") c= "&lt;";
    if (c == ">") c= "&gt;";
    if (c == "&") c= "&amp;";
    if (c == "\"") c= "&quot;";
    outputs.append(c);
  }
  return outputs;
}

/* In response to edit menu "Previous statement" or "Next statement",
   make statement widget contents = a previous statement from history,
   without executing.
   When copying from history, change entities to < and > and & and "
   But the job is much bigger than that.
   Find "STATEMENT START".
   Start copying.
   Skip everything between "PROMPT START" and "PROMPT END".
   Stop copying when you see "RESULT".
*/
void MainWindow::history_markup_previous()
{
  ++history_markup_counter;
  if (history_markup_previous_or_next() == -1) --history_markup_counter;
}


void MainWindow::history_markup_next()
{
  --history_markup_counter;
  if (history_markup_previous_or_next() == -1) ++history_markup_counter;
}


int MainWindow::history_markup_previous_or_next()
{
  QString outputs;
  QString final_outputs;
  int index_of_prompt_end;
  int index_of_a_gt, index_of_lt_a;
  int i;
  QString c;
  int search_start_point;

  QString s;
  int index_of_statement_start;
  int index_of_result;

  s= history_edit_widget->toHtml();
  search_start_point= -1;                                                    /* search starting from end of string */
  for (i= 0;;)
  {
    index_of_statement_start= s.lastIndexOf("\"STATEMENT START\"", search_start_point);
    if (index_of_statement_start == -1) return -1;                           /* (statement start not found) */
    index_of_result= s.indexOf("\"RESULT\"", index_of_statement_start);
    if (index_of_result == -1) return -1;                                    /* (result not found) */
    ++i;
    if (i >= history_markup_counter) break;                                  /* (we're at the right SELECT) */
    search_start_point= index_of_statement_start - 1;                        /* keep going back */
  }
  outputs= "";

  s= s.mid(index_of_statement_start, index_of_result - index_of_statement_start);

  /* At this point s = whatever's between statement start and result */
  /* But there might be a series of repeat(prompt-start prompt-end real-statement) till result */
  for (;;)
  {
    index_of_prompt_end= s.indexOf("\"PROMPT END\"", 0);
    if (index_of_prompt_end == -1) break;
    index_of_a_gt= s.indexOf("a>", index_of_prompt_end) + 2;
    if (index_of_a_gt == -1) break;
    index_of_lt_a= s.indexOf("<a", index_of_a_gt);
    if (index_of_lt_a == -1) break;
    outputs.append(s.mid(index_of_a_gt, index_of_lt_a - index_of_a_gt));
    s= s.mid(index_of_a_gt, -1);
  }

  /* At this point outputs = the statement but it might contain entities
     so reverse whatever happened in history_markup_copy_for_history(),
     and change <br> back to \r.
     Todo: check what happens if original statement contains an entity.
  */
  final_outputs= "";
  for (i= 0; i < outputs.length(); ++i)
  {
    if (outputs.mid(i, 4) == "<br>")
    {
      final_outputs.append("\r");
      i+= 3;
    }
    else if (outputs.mid(i, 6) == "<br />")
    {
      final_outputs.append("\r");
      i+= 5;
    }
    else if (outputs.mid(i, 4) == "&lt;")
    {
      final_outputs.append("<");
      i+= 3;
    }
    else if (outputs.mid(i, 4) == "&gt;")
    {
      final_outputs.append(">");
      i+= 3;
    }
    else if (outputs.mid(i, 5) == "&amp;")
    {
      final_outputs.append("&");
      i+= 4;
    }
    else if (outputs.mid(i, 6) == "&quot;")
    {
      final_outputs.append("\"");
      i+= 5;
    }
    else
    {
      c= outputs.mid(i, 1);
      final_outputs.append(c);
    }
  }

  statement_edit_widget->setPlainText(final_outputs);
  return 0;
}


/*
  Create the menu.
  Apparently the menubar already exists because of the call to UiSetup() earlier. Incomprehensible.
  Todo: Consider making a table with the menu settings. Something like this ...
  Table: ocelot.actions
  Window  Menu_position Menu_item_position Menu_name Menu_item_name Name  Caption/Hint    Menu   Key   Toolbar Action
  ------  ------------- ------------------ --------- -------------- ----  ------------    ----   ---   ------- ------
  Main                1                  1 File      E&xit                                                     exit
  Main    1             1                  File      Exitrun   Execute         Edit   &E    lightning-bolt
  This is creation -- assume there are no existing menus.
  There could also be alter -- wipe existing actions as part of user customization, then call this.
     We do not want users to get rid of "About" box that shows copyrights etc.
  ?? How do I get rid of a QAction?
    Also set up more with setShortcuts(), setStatusTip(), ...
    Also add action to toolbox
     This would addMenu() multiple times for the same menu, so I use a different name each time.
    I should show what the shortcuts are; unfortunately for most actions the shortcut differs according to platform
    Maybe instead of "Execute ctrl+E" I should be using "Run ctrl+R"
    Sure, I tried using & for various menu items e.g. E&xit -- but then Alt+x did not work!
  Todo: With Ubuntu 14.04 the ^O and ^E and ^Q keys didn't work, find out what goes on.
*/
void MainWindow::create_menu()
{
  menu_file= ui->menuBar->addMenu(tr("File"));
  /* Todo: consider adding fileMenu = new QMenu(tr("&File"), this); -*/
  menu_file_action_connect= menu_file->addAction(tr("Connect"));
  connect(menu_file_action_connect, SIGNAL(triggered()), this, SLOT(action_connect()));
  menu_file_action_connect->setShortcut(QKeySequence::Open); /* Todo: think of a better key sequence than ctrl-o */

  menu_file_action_exit= menu_file->addAction(tr("Exit"));
  connect(menu_file_action_exit, SIGNAL(triggered()), this, SLOT(action_exit()));
  menu_file_action_exit->setShortcut(QKeySequence::Quit);

  /*
    Most Edit menu actions don't require slots from me -- cut() etc. already exist for class QTextEdit.
    I just need to ensure they're on the menu.
  */
  menu_edit= ui->menuBar->addMenu(tr("Edit"));
  menu_edit_action_undo= menu_edit->addAction(tr("Undo"));
  connect(menu_edit_action_undo, SIGNAL(triggered()), this, SLOT(action_undo()));
  menu_edit_action_undo->setShortcut(QKeySequence::Undo);
  menu_edit_action_redo= menu_edit->addAction(tr("Redo"));
  connect(menu_edit_action_redo, SIGNAL(triggered()), statement_edit_widget, SLOT(redo()));
  menu_edit_action_redo->setShortcut(QKeySequence::Redo);
  menu_edit->addSeparator();
  menu_edit_action_cut= menu_edit->addAction(tr("Cut"));
  QTextEdit::connect(menu_edit_action_cut, SIGNAL(triggered()), statement_edit_widget, SLOT(cut()));
  menu_edit_action_cut->setShortcut(QKeySequence::Cut);
  menu_edit_action_copy= menu_edit->addAction(tr("Copy"));
  connect(menu_edit_action_copy, SIGNAL(triggered()), statement_edit_widget, SLOT(copy()));
  menu_edit_action_copy->setShortcut(QKeySequence::Copy);
  menu_edit_action_paste= menu_edit->addAction(tr("Paste"));
  connect(menu_edit_action_paste, SIGNAL(triggered()), statement_edit_widget, SLOT(paste()));
  menu_edit_action_paste->setShortcut(QKeySequence::Paste);
  menu_edit->addSeparator();
  menu_edit_action_select_all= menu_edit->addAction(tr("Select All"));
  connect(menu_edit_action_select_all, SIGNAL(triggered()), statement_edit_widget, SLOT(selectAll()));
  menu_edit_action_select_all->setShortcut(QKeySequence::SelectAll);

  menu_edit_action_cut->setEnabled(false);
  menu_edit_action_copy->setEnabled(false);
  menu_edit_action_undo->setEnabled(statement_edit_widget->document()->isUndoAvailable());
  menu_edit_action_redo->setEnabled(statement_edit_widget->document()->isRedoAvailable());

  connect(statement_edit_widget, SIGNAL(copyAvailable(bool)),menu_edit_action_cut, SLOT(setEnabled(bool)));
  connect(statement_edit_widget, SIGNAL(copyAvailable(bool)),menu_edit_action_copy, SLOT(setEnabled(bool)));

  connect(statement_edit_widget->document(), SIGNAL(undoAvailable(bool)),menu_edit_action_undo, SLOT(setEnabled(bool)));
  connect(statement_edit_widget->document(), SIGNAL(redoAvailable(bool)),menu_edit_action_redo, SLOT(setEnabled(bool)));

  menu_edit_action_history_markup_previous= menu_edit->addAction(tr("Previous statement"));
  connect(menu_edit_action_history_markup_previous, SIGNAL(triggered()), this, SLOT(history_markup_previous()));
  menu_edit_action_history_markup_previous->setShortcut(QKeySequence(tr("Ctrl+P")));

  menu_edit_action_history_markup_next= menu_edit->addAction(tr("Next statement"));
  connect(menu_edit_action_history_markup_next, SIGNAL(triggered()), this, SLOT(history_markup_next()));
  menu_edit_action_history_markup_next->setShortcut(QKeySequence(tr("Ctrl+N")));

  menu_run= ui->menuBar->addMenu(tr("Run"));
  menu_run_action_execute= menu_run->addAction(tr("Execute"));
  connect(menu_run_action_execute, SIGNAL(triggered()), this, SLOT(action_execute()));
  menu_run_action_execute->setShortcut(QKeySequence(tr("Ctrl+E")));

  menu_settings= ui->menuBar->addMenu(tr("Settings"));
  menu_settings_action_statement= menu_settings->addAction(tr("Statement Widget: Colors and Fonts"));
  menu_settings_action_grid= menu_settings->addAction(tr("Grid Widget: Colors and Fonts"));
  menu_settings_action_history= menu_settings->addAction(tr("History Widget: Colors and Fonts"));
  menu_settings_action_main= menu_settings->addAction(tr("Main Window: Colors and Fonts"));
  connect(menu_settings_action_statement, SIGNAL(triggered()), this, SLOT(action_statement()));
  connect(menu_settings_action_grid, SIGNAL(triggered()), this, SLOT(action_grid()));
  connect(menu_settings_action_history, SIGNAL(triggered()), this, SLOT(action_history()));
  connect(menu_settings_action_main, SIGNAL(triggered()), this, SLOT(action_main()));

  menu_help= ui->menuBar->addMenu(tr("Help"));
  menu_help_action_about= menu_help->addAction(tr("About"));
  connect(menu_help_action_about, SIGNAL(triggered()), this, SLOT(action_about()));
  menu_help_action_the_manual= menu_help->addAction(tr("The Manual"));
  connect(menu_help_action_the_manual, SIGNAL(triggered()), this, SLOT(action_the_manual()));

  /* Qt says I should also do "addSeparator" if Motif style. Harmless. */
  ui->menuBar->addSeparator();
  /* exitAction->setPriority(QAction::LowPriority); */
}


/*
  ACTIONS
  ! All action_ functions must be in the "public slots" area of ocelotgui.h
*/

/*
  action_statement_edit_widget_text_changed() is a slot.
  Actually the connect() was for statement_edit_widget->document()'s contentsChanged(),
  rather than statement_edit_widget's textChanged(), but I don't see any difference.
*/
void MainWindow::action_statement_edit_widget_text_changed()
{
  QString text;
  int i;
  // int j;
  int pos;
  QTextCharFormat format_of_current_token;

  /*
    When insertText happens later in this routine, it causes a signal
    and we get action_statement_edit_widget_text_changed() again.
    It would be infinite. Doubtless there are good ways to avoid looping.
    I don't know them. So I'll exit if a global flag is on, then set it.
  */
  /* Todo: check whether "wasModified" would have been a better slot. */
  if (statement_edit_widget_text_changed_flag != 0) return;
  statement_edit_widget_text_changed_flag= 1;

  /* Syntax highlighting */
  text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */

  /* Todo: avoid total tokenize() + tokens_to_keywords() if user is just adding at end */

  tokenize(text.data(),
           text.size(),
           &main_token_lengths, &main_token_offsets, MAX_TOKENS,(QChar*)"33333", 1, ocelot_delimiter_str, 1);

  tokens_to_keywords();

  /* This "sets" the colour, it does not "merge" it. */
  /* Do not try to set underlines, they won't go away. */
  QTextDocument *pDoc= statement_edit_widget->document();
  QTextCursor cur(pDoc);

  /* cur.select (QTextCursor::Document); */ /* desperate attempt to fix so undo/redo is not destroyed ... does not work */

  QTextCharFormat format_of_literal;
  format_of_literal.setForeground(QColor(ocelot_statement_highlight_literal_color));
  QTextCharFormat format_of_identifier;
  format_of_identifier.setForeground(QColor(ocelot_statement_highlight_identifier_color));
  QTextCharFormat format_of_comment;
  format_of_comment.setForeground(QColor(ocelot_statement_highlight_comment_color));
  QTextCharFormat format_of_operator;
  format_of_operator.setForeground(QColor(ocelot_statement_highlight_operator_color));
  QTextCharFormat format_of_reserved_word;
  format_of_reserved_word.setForeground(QColor(ocelot_statement_highlight_reserved_color));
  QTextCharFormat format_of_other;
  format_of_other.setForeground(QColor(ocelot_statement_color));

  pos= 0;
  /* cur.setPosition(pos, QTextCursor::KeepAnchor); */
  cur.beginEditBlock();                                     /* ought to affect undo/redo stack? */

  for (i= 0; main_token_lengths[i] != 0; ++i)
  {
    /* Todo: consider: If we've gone over some white space, let it be normal format */
    /* Todo: check if this is working. An earlier test suggested that at least with underline it isn't working. */
    for (; pos < main_token_offsets[i]; ++pos)
    {
      cur.setPosition(pos, QTextCursor::MoveAnchor);
      cur.setPosition(pos, QTextCursor::KeepAnchor);
      cur.setCharFormat(format_of_other);
      /* cur.clearSelection(); */
    }
    int t= main_token_types[i];
    if (t == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE) format_of_current_token= format_of_literal;
    if (t == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE) format_of_current_token= format_of_literal;
    if (t == TOKEN_TYPE_LITERAL_WITH_DIGIT) format_of_current_token= format_of_literal;
    if (t == TOKEN_TYPE_LITERAL_WITH_BRACE) format_of_current_token= format_of_literal;
    if (t == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK) format_of_current_token= format_of_identifier;
    if (t == TOKEN_TYPE_IDENTIFIER_WITH_AT) format_of_current_token= format_of_identifier;
    if (t == TOKEN_TYPE_COMMENT_WITH_SLASH) format_of_current_token= format_of_comment;
    if (t == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE) format_of_current_token= format_of_comment;
    if (t == TOKEN_TYPE_COMMENT_WITH_MINUS) format_of_current_token= format_of_comment;
    if (t == TOKEN_TYPE_OPERATOR) format_of_current_token= format_of_operator;
    if (t >= TOKEN_KEYWORDS_START) format_of_current_token= format_of_reserved_word;
    if (t == TOKEN_TYPE_OTHER) format_of_current_token= format_of_other;

    cur.setPosition(pos, QTextCursor::MoveAnchor);
    cur.setPosition(pos+main_token_lengths[i], QTextCursor::KeepAnchor);
    cur.setCharFormat(format_of_current_token);
    pos+= main_token_lengths[i];
  }
  cur.endEditBlock();

  /* Todo: consider what to do about trailing whitespace. */

  widget_sizer(); /* Perhaps adjust relative sizes of the main widgets. */

  statement_edit_widget_text_changed_flag= 0;
}


/*
  For menu item "connect" we said connect(...SLOT(action_connect())));
  By default this is on and associated with File|Connect menu item.
  Actually connect will be attempted depending on defaults and command-line parameters.
*/
void MainWindow::action_connect()
{
  action_connect_once(tr("File|Connect"));
}


/*
  Todo:
  If user types OK and there's an error, repeat the dialog box with a new message e.g. "Connect failed ...".
  This is called from program-start!
  This should put "CONNECT" in the statement widget and cause its execution, so it shows up on the history widget.
*/

void MainWindow::action_connect_once(QString message)
{
  row_form_label= new QString[10];
  row_form_type= new int[10];
  row_form_is_password= new int[10];
  row_form_data= new QString[10];
  row_form_width= new QString[10];

  row_form_label[0]= "host"; row_form_type[0]= 0; row_form_is_password[0]= 0; row_form_data[0]= ocelot_host; row_form_width[0]= 80;
  row_form_label[1]= "port"; row_form_type[1]= NUM_FLAG; row_form_is_password[1]= 0; row_form_data[1]= QString::number(ocelot_port); row_form_width[1]= 4;
  row_form_label[2]= "user"; row_form_type[2]= 0; row_form_is_password[2]= 0; row_form_data[2]= ocelot_user; row_form_width[2]= 80;
  row_form_label[3]= "database"; row_form_type[3]= 0; row_form_is_password[3]= 0; row_form_data[3]= ocelot_database; row_form_width[3]= 80;
  row_form_label[4]= "socket"; row_form_type[4]= 0; row_form_is_password[4]= 0; row_form_data[4]= ocelot_unix_socket; row_form_width[4]= 80;
  row_form_label[5]= "password"; row_form_type[5]= 0; row_form_is_password[5]= 1; row_form_data[5]= ocelot_password; row_form_width[5]= 80;
  row_form_label[6]= "protocol"; row_form_type[6]= NUM_FLAG; row_form_is_password[6]= 0; row_form_data[6]= ocelot_protocol; row_form_width[6]= 80;
  row_form_label[7]= "init_command"; row_form_type[7]= 0; row_form_is_password[7]= 0; row_form_data[7]= ocelot_init_command; row_form_width[7]= 80;
  //row_form_label[8]= "default_auth"; row_form_type[8]= 0; row_form_is_password[8]= 0; row_form_data[8]= ocelot_default_auth; row_form_width[8]= 80;
  row_form_title= tr("Connection Dialog Box");
  row_form_message= message;

  QFont tmp_font;
  QFont *saved_font;
  tmp_font= result_grid_table_widget->font();
  saved_font=&tmp_font;

  Row_form_box *co= new Row_form_box(saved_font, this);
  co->exec();

  if (co->is_ok == 1)
  {
    ocelot_host= row_form_data[0];
    ocelot_port= row_form_data[1].toInt();
    ocelot_user= row_form_data[2];
    ocelot_database= row_form_data[3];
    ocelot_unix_socket= row_form_data[4];
    ocelot_password= row_form_data[5];
    ocelot_protocol= row_form_data[6];
    ocelot_init_command= row_form_data[7];
    //ocelot_default_auth= row_form_data[8];           /* Todo: this is disabled but should work with MySQL 5.6+ */
    /* This should ensure that a record goes to the history widget */
    /* Todo: clear statement_edit_widget first */
    statement_edit_widget->insertPlainText("CONNECT");
    action_execute();
    if (ocelot_init_command > "")
    {
      statement_edit_widget->insertPlainText(ocelot_init_command);
      action_execute();
    }
  }
  delete(co);

  delete [] row_form_width;
  delete [] row_form_data;
  delete [] row_form_is_password;
  delete [] row_form_type;
  delete [] row_form_label;
}


/*
  For menu item "exit" we said connect(...SLOT(action_exit())));
  By default this is on and associated with File|Exit menu item.
  Stop the program.
*/
void MainWindow::action_exit()
{
  close();
}

/* Todo: consider adding   //printf(qVersion()); */
void MainWindow::action_about()
{
  QMessageBox msgBox;

  msgBox.setWindowTitle(tr("Help | About"));
  msgBox.setText("<b>ocelotgui -- Ocelot Graphical User Interface</b><br>Copyright (c) 2014 by Ocelot Computer Services Inc.<br>\
This program is free software: you can redistribute it and/or modify \
it under the terms of the GNU General Public License as published by \
the Free Software Foundation, either version 3 of the License, or \
(at your option) any later version.<br>\
<br>\
This program is distributed in the hope that it will be useful, \
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
GNU General Public License for more details.<br>\
<br>\
You should have received a copy of the GNU General Public License \
along with this program.  If not, see &lt;http://www.gnu.org/licenses/&gt;.");
    msgBox.exec();
}

/*
  the_manual_widget will be shown when user clicks Help | The Manual.
  It does not inherit the fonts+colors settings of the main window.
  The text is copied from manual.htm; all lines in manual.htm end
  with two spaces so there is a blanket replacement of all "  " to "  \".
*/
void MainWindow::create_the_manual_widget()
{
  the_manual_text_edit= new QTextEdit();
  the_manual_pushbutton= new QPushButton(tr("Close"));
  connect(the_manual_pushbutton, SIGNAL(clicked()), this, SLOT(action_the_manual_close()));
  the_manual_layout= new QVBoxLayout();
  the_manual_layout->addWidget(the_manual_text_edit);
  the_manual_layout->addWidget(the_manual_pushbutton);
  the_manual_widget= new QWidget();
  the_manual_widget->setLayout(the_manual_layout);

  the_manual_widget->setWindowTitle(tr("The Manual"));
  the_manual_widget->setMinimumWidth(250); /* should be equal to width of title, at least */

  the_manual_text_edit->setText("\
<BR><h1>The ocelotgui user manual</h1>  \
<BR>  \
<BR>Version 0.1.3, September 5 2014  \
<BR>  \
<BR>  \
<BR>Copyright (c) 2014 by Ocelot Computer Services Inc. All rights reserved.  \
<BR>  \
<BR>This program is free software; you can redistribute it and/or modify  \
<BR>it under the terms of the GNU General Public License as published by  \
<BR>the Free Software Foundation; version 2 of the License.  \
<BR>  \
<BR>This program is distributed in the hope that it will be useful,  \
<BR>but WITHOUT ANY WARRANTY; without even the implied warranty of  \
<BR>MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the  \
<BR>GNU General Public License for more details.  \
<BR>  \
<BR>You should have received a copy of the GNU General Public License  \
<BR>along with this program; if not, write to the Free Software  \
<BR>Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA  \
<BR>  \
<BR>  \
<BR><h2>Executive Summary</h2>  \
<BR>  \
<BR>The ocelotgui application, more fully  \
The Ocelot Graphical User Interface', allows users to connect to  \
a MySQL or MariaDB DBMS server, enter SQL statements, and receive results.  \
Some of its features are: syntax highlighting, user-settable colors  \
and fonts for each part of the screen, and result-set displays  \
with multi-line rows and resizable columns.  \
<BR>  \
<BR><h2>The company, the product, and the status</h2>  \
<BR>  \
<BR>Ocelot Computer Services is a Canadian company  \
which has specialized in database products for thirty years.  \
Its two employees both worked for MySQL AB and  \
Sun Microsystems and Oracle between 2003 and 2011.  \
<BR>  \
<BR>The ocelotgui program is a front end which connects to MySQL (tm) or MariaDB (tm).  \
In some ways it is like the basic mysql client program,  \
with added GUI features: full-screen editing, syntax  \
highlighting, tabular display, customized fonts and colors.  \
It differs from some other front-end GUI products because  \
it is open source (GPL), is written in C++, and makes use  \
of the Qt multi-platform widget library.  \
<BR>  \
<BR>The product status is: alpha. It has been known to work as described in  \
this manual on several Linux distros. But testing has been short  \
and the 'todo' list is long. We are making an early release for  \
tire kickers who will want to confirm what the manual describes,  \
or want to see the source code and do their own builds.  \
<BR>  \
<BR><h2>Downloading, installing, and building</h2>  \
<BR>  \
<BR>To download the product go to  \
https://github.com/ocelot-inc/ocelotgui.  \
Instructions for installation will be in the README.txt file.  \
This location may change, or alternate locations may appear.  \
If so there will either be an announcement on github or on ocelot.ca.  \
<BR>  \
<BR>The package contains source code, and  \
two executable files, one for use with Qt version 4 and the  \
other for use with Qt version 5.  \
The executables are built dynamically.  \
They look for pre-installed copies of Qt libraries and of the  \
MySQL client library -- see the README.txt file about these.  \
The executables were built with Ubuntu 12.04 and may not work  \
well on other Linux distros -- if that is the case, consult  \
the README.txt and the source code to see how to build from  \
source.  \
<BR><h2>Starting</h2>  \
<BR>  \
<BR>There must be an instance of MySQL or MariaDB running somewhere.  \
<BR>  \
<BR>If connection is possible with the mysql client and does not require  \
unusual options, then connection is possible with ocelotgui. If there is  \
a my.cnf file, ocelotgui will read it, just as the mysql client would.  \
If there are connection-related options on the command line, ocelotgui  \
will accept them just as the mysql client would. Therefore the typical  \
way to start the program is to say  \
ocelotgui [--option [, option...]]  \
<BR>  \
<BR>If a password is required but not supplied, or if the initial  \
attempt to connect fails, a dialog box will appear.  \
Fill in or correct the fields on the dialog box and click OK.  \
If the connection still fails, then ocelotgui will still come up,  \
but only non-DBMS tasks such as screen customizing will be possible.  \
<BR>  \
<BR>In any case, an initial screen will appear. After some activity has  \
taken place, the screen will have four parts, from top to bottom:  \
<BR>menu  \
<BR>history widget, where retired statements and diagnostics end up  \
<BR>results widget, where SELECT result sets appear  \
<BR>statement widget, where users can type in instructions.  \
<BR>Initially, though, only the menu and statement widget will appear.  \
<BR>  \
<BR>Again, this should be reminiscent of the way the mysql client works:  \
statements are typed at the bottom of the screen, and appear to  \
scroll off the top after they are executed, with results in the middle.  \
<BR>  \
<BR><h2>Statement widget</h2>  \
<BR>  \
<BR>The statement widget is an editable multi-line text box.  \
The usual control keys that work on other text editors will work  \
here too; see the later description of Menu Item: Edit.  \
<BR>  \
<BR>The program includes a tokenizer and can recognize the parts of  \
speech in typical MySQL grammar. It will do syntax highlighting  \
by changing the color, for example comments will appear in red,  \
dentifiers in blue, operators in green, and so on.  \
The colors can be  \
customized, see the later description of Menu Item: Settings.  \
<BR>  \
<BR>The left side of the statement widget is reserved for the prompt,  \
and cannot be typed over. Initially the prompt will be 'mysql&gt;',  \
but this can be changed, see the later description of  \
Client Statements: Prompt.  \
<BR>  \
Once a statement has been entered and is ready to be executed,  \
the user can hit control-E, choose menu item Run|Execute, or  \
place the cursor at the end of the text (after the ';' or other  \
delimiter) and type Enter. It is legal to enter multiple  \
statements, separated by semicolons, and then execute them  \
in a single sequence.  \
<BR>  \
<BR><h2>Client statements</h2>  \
<BR>  \
<BR>A client statement is a statement which changes some behavior  \
of the client (that is, of the ocelotgui front end) but does not  \
necessarily go to the MySQL/MariaDB server. Of the statements  \
that the MySQL Reference manual describes in section  \
'mysql commands' http://dev.mysql.com/doc/refman/5.6/en/mysql-commands.html.  \
<BR>  \
<BR>The ocelotgui program has working equivalents for: clear, delimiter, exit,  \
prompt, source, and warnings. For example, entering 'quit;'  \
followed by Enter will cause the program to stop. It is  \
sometimes not mandatory to end a client statement with ';',  \
but is strongly recommended.  \
<BR>  \
<BR>There are some enhancements affecting the PROMPT statement.  \
The special sequence '&#92;2' means 'repeat the prompt on all lines',  \
and the special sequence '&#92;L' means 'show line numbers'. For example,  \
'PROMPT &#92;2&#92;Lmariadb;' will change the prompt so that each line begins  \
with '[line number] mariadb>'.  \
<BR>  \
<BR><h2>History widget</h2>  \
<BR>  \
<BR>Once a statement has been executed, a copy of the statement text  \
and the diagnostic result (for example: 0.04 seconds, OK) will  \
be placed in the history widget. Everything in the history widget  \
is editable including the prompt, and it simply fills up so that  \
after a while the older statements are scrolled off the screen.  \
Thus its main function is to show what recent statements and  \
results were. Statements in the history can be retrieved while  \
the focus is on the statement widget, by selecting 'Previous statement'  \
or 'Next statement' menu items.  \
<BR>  \
<BR><h2>Result widget</h2>  \
<BR>  \
<BR>If a statement is SELECT or SHOW or some other statement that  \
returns a result set, it will appear in the result widget in  \
the middle area of the screen. The result widget is split up  \
into columns. Each column has a header and details taken from  \
what the DBMS returns.  \
<BR>  \
<BR>The width of the column depends on the result set's definition,  \
but extremely wide columns will be split onto multiple lines.  \
That is, one result-set row may take up to five lines.  \
If the data still is too wide or too tall to fit in the cell,  \
then the cell will get a vertical scroll bar. The user can  \
change the width of a column by dragging the column's right  \
border to the right to make the column wider, or to the left  \
to make it narrower.  \
<BR>  \
The result widget as a whole may have a horizontal and a vertical  \
scroll bar. The vertical scroll bar moves a row at a time rather  \
than a pixel at a time -- this makes large result sets more  \
manageable, but makes the vertical scroll bar unresponsive if  \
each row has multiple lines and the number of rows is small.  \
<BR>  \
<BR><h2>Menu</h2>  \
<BR>  \
<BR>The menu at the top of the screen has File, Settings, Run,  \
Exit and Help.  \
<BR>  \
<BR>File|Connect, or Ctrl+O, starts the Connect dialog box.  \
<BR>File|Exit, or Ctrl+Q, stops the program.  \
<BR>  \
<BR>Edit|Undo or Ctrl+Z, , Edit|Redo or Ctrl+Shift+Z, Edit|Cut  \
or Ctrl+X, Edit|Copy , Edit|Cut, Edit|Copy, Edit|Paste,  \
and Edit|Select All work in the conventional manner,  \
except Edit|Redo which can only redo the last change.  \
<BR>  \
<BR>Run|Execute or Ctrl+E causes execution of whatever is in the  \
statement widget.  \
<BR>  \
<BR>Settings|Statement Widget: Colors + Fonts,  \
Settings|Grid Widget: Colors + Fonts,  \
Settings|History Widget: Colors + Fonts, and  \
Settings|Main Widget: Colors + Fonts are the  \
items which affect the behavior of each of each  \
individual widget. The color settings affect foregrounds,  \
backgrounds, borders, and (for the statement widget only)  \
the syntax highlights. The font settings affect font family,  \
boldness, italics, and size. Both color and font settings  \
involve further dialog boxes which are standard with Qt.  \
There may be additional choices affecting appearance,  \
for example the width of the border used to drag columns  \
in the result widget.  \
<BR>  \
<BR>Help|About will show the license and copyright and version.  \
Help|Manual will show the manual.  \
<BR>  \
<BR><h2>Contact</h2>  \
<BR>  \
<BR>Bug reports and feature requests may go on  \
https://github.com/ocelot-inc/ocelotgui/issues.  \
<BR>  \
There may be announcements from time to time on Ocelot's  \
web page (ocelot.ca) or on the employee blog (ocelot.ca/blog).  \
This manual will also be available on ocelot.ca soon.  \
<BR>  \
<BR>Any contributions will be appreciated.  \
  ");
  }


void MainWindow::action_the_manual()
{
  the_manual_widget->setMinimumSize(500,500);
  the_manual_widget->show();
  the_manual_widget->raise();
}


/* For the Close button of Menu | Help | the_manual */
void MainWindow::action_the_manual_close()
{
  the_manual_widget->hide();
}


/*
  Problem: to do syntax highlighting, one must have a slot
  action_statement_edit_widget_text_changed() to intercept
  every change in action_statement_edit_widget. But, when
  user chooses Undo, that interception happens again, which
  negates the effects of undo. Also, undo has to be done
  twice, because everythng between beginEditBlock() and
  endEditBlock() is an undoable change. Therefore, instead
  of getting slot() invoked directly, I get to action_undo()
  which temporarily disables use of action_statement_edit_widget_changed()
  and calls undo() twice.
  Todo: consider: what if the user wants to undo with control-Z instead of menu?
  Todo: consider: will there be a bug if syntax highlighting is disabled?
  Todo: consider: perhaps now redo will be no good.
*/
void MainWindow::action_undo()
{
  statement_edit_widget_text_changed_flag= 1;
  statement_edit_widget->undo();
  statement_edit_widget->undo();
  statement_edit_widget_text_changed_flag= 0;
}


/*
  Comments relevant to action_statement(), action_grid(), action_history(), action_main() ...
  These let the user select the colors and font for some widgets.
  Qt documentation says: "QApplication maintains a system/theme font which serves as a default for all widgets."
  So I'll assume that the Main Window always has that font, but let user change widgets that are within it.
  It's best to use a fixed-width font for most widgets, but I won't force that.
  Todo: consider: Should we pass default = current font (what we changed to last time) rather than default = main window font?
  Todo: Looks like there's a small problem with the history_edit_widget -- it might scroll.
        Probably what we want is: if we're at the bottom before the font change, then move to the bottom after the change.
*/
/* Todo: before se->delete(), check if WA_DeleteOnClose is off, always, for sure. */
/* Todo: edit_widget_text_changed() etc. should only be done if one of the ocelot_statement_highlight variables changed */
/* Todo: edit_widget_text_changed() etc. should be preceded by "emit"? */
/* Todo: put setStyleSheet() in an "if": only redo if ocelot_statement_style_string changed. */
/* Todo: after doing a setStyleSheet() change, put in history as "SET @..." statements. */
/* Todo: make sure there's nno need to call set_current_colors_and_font(); */
/*
   Todo: Get default color too.
   I can get colors too, based on
   color= widget.palette().color(QPalette::Window);            formerly known as QPalette::Background
   color= widget.palette().color(QPalette::WindowText);           formerly known as QPalette::Foreground
   but    color.name() gives the color as "#RRGGBB" -- but I want a name!
   But your problem is: it might be RGB originally. You'd have to accept RGB.
   Luckily setStyleSheet will accept it, e.g. statement_edit_widget->setStyleSheet("background:rgb(200,100,150);");
   See also http://www.w3.org/TR/SVG/types.html#ColorKeywords "recognized color keyword names".
*/
/* Note: the result of se->exec() will be QDialog::Rejected regardless which button was pushed, but that's OK. */
void MainWindow::action_statement()
{
  Settings *se= new Settings(0, this);
  int result= se->exec();
  if (result == QDialog::Rejected || result >= 0)
  {
    make_style_strings();
    statement_edit_widget->setStyleSheet(ocelot_statement_style_string);
    statement_edit_widget->statement_edit_widget_left_bgcolor= QColor(ocelot_statement_prompt_background_color);
    statement_edit_widget->statement_edit_widget_left_treatment1_textcolor= QColor(ocelot_statement_color);
    action_statement_edit_widget_text_changed();            /* only for highlight? repaint so new highlighting will appear */
  }
  delete(se);
}


void MainWindow::action_grid()
{
  Settings *se= new Settings(1, this);
  int result= se->exec();
  if (result == QDialog::Rejected || result >= 0)
  {
    make_style_strings();
    result_grid_table_widget->setStyleSheet(ocelot_grid_style_string);
  }
  delete(se);
}


void MainWindow::action_history()
{
  Settings *se= new Settings(2, this);
  int result= se->exec();
  if (result == QDialog::Rejected || result >= 0)
  {
    make_style_strings();
    history_edit_widget->setStyleSheet(ocelot_history_style_string);
  }
  delete(se);
}


/* Todo: consider: maybe changing should only affect menuBar so there's no inheriting. */
void MainWindow::action_main()
{
  Settings *se= new Settings(3, this);
  int result= se->exec();
  if (result == QDialog::Rejected || result >= 0)
  {
    make_style_strings();
    main_window->setStyleSheet(ocelot_main_style_string);
    ui->menuBar->setStyleSheet(ocelot_main_style_string);
  }
  delete(se);
}


/*
  Todo: something has to be figured out about border width and border colors.
  I think, uncertainly, that the default border width is 0 and the color is Palette:shadow i.e. black.
  Possibly I could get the original value from QWidget::styleSheet() is some startup parameter changed it?
  Possibly I'd need to get a QFrame with a lineWidth() and frameWidth()?
  Possibly I should just assume that border width = 0px except for main window, and
  take out any mention of border in the Settings dialogs.
*/
void MainWindow::set_current_colors_and_font()
{
  QFont font;
  ocelot_statement_color= statement_edit_widget->palette().color(QPalette::WindowText).name(); /* = QPalette::Foreground */
  ocelot_statement_background_color= statement_edit_widget->palette().color(QPalette::Window).name(); /* = QPalette::Background */
  font= statement_edit_widget->font();
  ocelot_statement_font_family= font.family();
  if (font.italic()) ocelot_statement_font_style= "italic"; else ocelot_statement_font_style= "normal";
  ocelot_statement_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
  if (font.Bold) ocelot_statement_font_weight= "bold"; else ocelot_statement_font_weight= "normal";

  ocelot_grid_color= result_grid_table_widget->palette().color(QPalette::WindowText).name(); /* = QPalette::Foreground */
  ocelot_grid_background_color=result_grid_table_widget->palette().color(QPalette::Window).name(); /* = QPalette::Background */
  font= result_grid_table_widget->font();
  ocelot_grid_font_family= font.family();
  if (font.italic()) ocelot_grid_font_style= "italic"; else ocelot_grid_font_style= "normal";
  ocelot_grid_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
  if (font.Bold) ocelot_grid_font_weight= "bold"; else ocelot_grid_font_weight= "normal";

  ocelot_history_color= history_edit_widget->palette().color(QPalette::WindowText).name(); /* = QPalette::Foreground */
  ocelot_history_background_color=history_edit_widget->palette().color(QPalette::Window).name(); /* = QPalette::Background */
  font= history_edit_widget->font();
  ocelot_history_font_family= font.family();
  if (font.italic()) ocelot_history_font_style= "italic"; else ocelot_history_font_style= "normal";
  ocelot_history_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
  if (font.Bold) ocelot_history_font_weight= "bold"; else ocelot_history_font_weight= "normal";

  ocelot_main_color= main_window->palette().color(QPalette::WindowText).name(); /* = QPalette::Foreground */
  ocelot_main_background_color= main_window->palette().color(QPalette::Window).name(); /* = QPalette::Background */
  font= main_window->font();
  ocelot_main_font_family= font.family();
  if (font.italic()) ocelot_main_font_style= "italic"; else ocelot_main_font_style= "normal";
  ocelot_main_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
  if (font.Bold) ocelot_main_font_weight= "bold"; else ocelot_main_font_weight= "normal";
}


/* Called from: action_statement() etc. Make a string that setStyleSheet() can use. */
/*
  Todo: I wasn't able to figure out a simple way to emphasize widget title, for example make it bold.
  Todo: Set a border around a widget if it's detached, when detaching is supported again.
*/
void MainWindow::make_style_strings()
{
  ocelot_statement_style_string= "color:"; ocelot_statement_style_string.append(ocelot_statement_color);
  ocelot_statement_style_string.append(";background-color:"); ocelot_statement_style_string.append(ocelot_statement_background_color);
  ocelot_statement_style_string.append(";border:1px solid "); ocelot_statement_style_string.append(ocelot_statement_border_color);
  ocelot_statement_style_string.append(";font-family:"); ocelot_statement_style_string.append(ocelot_statement_font_family);
  ocelot_statement_style_string.append(";font-size:"); ocelot_statement_style_string.append(ocelot_statement_font_size);
  ocelot_statement_style_string.append("pt;font-style:"); ocelot_statement_style_string.append(ocelot_statement_font_style);
  ocelot_statement_style_string.append(";font-weight:"); ocelot_statement_style_string.append(ocelot_statement_font_weight);

  ocelot_grid_style_string= "color:"; ocelot_grid_style_string.append(ocelot_grid_color);
  ocelot_grid_style_string.append(";background-color:"); ocelot_grid_style_string.append(ocelot_grid_background_color);
  ocelot_grid_style_string.append(";border:");
  ocelot_grid_style_string.append(ocelot_grid_cell_border_size);
  ocelot_grid_style_string.append("px solid ");
  ocelot_grid_style_string.append(ocelot_grid_border_color);
  ocelot_grid_style_string.append(";font-family:"); ocelot_grid_style_string.append(ocelot_grid_font_family);
  ocelot_grid_style_string.append(";font-size:"); ocelot_grid_style_string.append(ocelot_grid_font_size);
  ocelot_grid_style_string.append("pt;font-style:"); ocelot_grid_style_string.append(ocelot_grid_font_style);
  ocelot_grid_style_string.append(";font-weight:"); ocelot_grid_style_string.append(ocelot_grid_font_weight);

  ocelot_grid_header_style_string= "color:"; ocelot_grid_header_style_string.append(ocelot_grid_color);
  ocelot_grid_header_style_string.append(";background-color:"); ocelot_grid_header_style_string.append(ocelot_grid_header_background_color);
  ocelot_grid_header_style_string.append(";border:");
  ocelot_grid_header_style_string.append(ocelot_grid_cell_border_size);
  ocelot_grid_header_style_string.append("px solid ");
  ocelot_grid_header_style_string.append(ocelot_grid_border_color);
  ocelot_grid_header_style_string.append(";font-family:"); ocelot_grid_header_style_string.append(ocelot_grid_font_family);
  ocelot_grid_header_style_string.append(";font-size:"); ocelot_grid_header_style_string.append(ocelot_grid_font_size);
  ocelot_grid_header_style_string.append("pt;font-style:"); ocelot_grid_header_style_string.append(ocelot_grid_font_style);
  ocelot_grid_header_style_string.append(";font-weight:"); ocelot_grid_header_style_string.append(ocelot_grid_font_weight);

  ocelot_history_style_string= "color:"; ocelot_history_style_string.append(ocelot_history_color);
  ocelot_history_style_string.append(";background-color:"); ocelot_history_style_string.append(ocelot_history_background_color);
  ocelot_history_style_string.append(";border:1px solid "); ocelot_history_style_string.append(ocelot_history_border_color);
  ocelot_history_style_string.append(";font-family:"); ocelot_history_style_string.append(ocelot_history_font_family);
  ocelot_history_style_string.append(";font-size:"); ocelot_history_style_string.append(ocelot_history_font_size);
  ocelot_history_style_string.append("pt;font-style:"); ocelot_history_style_string.append(ocelot_history_font_style);
  ocelot_history_style_string.append(";font-weight:"); ocelot_history_style_string.append(ocelot_history_font_weight);

  ocelot_main_style_string= "color:"; ocelot_main_style_string.append(ocelot_main_color);
  ocelot_main_style_string.append(";background-color:"); ocelot_main_style_string.append(ocelot_main_background_color);
  ocelot_main_style_string.append(";border:1px solid "); ocelot_main_style_string.append(ocelot_main_border_color);
  ocelot_main_style_string.append(";font-family:"); ocelot_main_style_string.append(ocelot_main_font_family);
  ocelot_main_style_string.append(";font-size:"); ocelot_main_style_string.append(ocelot_main_font_size);
  ocelot_main_style_string.append("pt;font-style:"); ocelot_main_style_string.append(ocelot_main_font_style);
  ocelot_main_style_string.append(";font-weight:"); ocelot_main_style_string.append(ocelot_main_font_weight);
}


/*
  Get the next statement in a string.
  Pass: string, offset within string. Or: main_token number.
  Return: length i.e. # of tokens including delimiter or ; so offset can be right next time
          length == 0 would mean end-of-string
  Re how to find the end of a statement:
     For each token
       If (token == BEGIN or ELSEIF or IF or LOOP or REPEAT or WHILE) ++begin_count
       If (token == END --begin_count
     If (token == delimiter) this is the end
     If (no more tokens) this is the end
     If (token == ';' and begin_count == 0) this is the end
     Todo: What about \G etc.?
     NB: it's an error if begin_count>0 and this is the end, caller might return client error
  Re CLIENT_MULTI_STATEMENT: this would work too, but in the long term I'll want to
     know for sure: which statement returned result set X? Since a CALL can return
     multiple result sets, I didn't see a way to figure that out.
  Don't bother with begin_count if PROCEDURE or FUNCTION or TRIGGER hasn't been seen.
  Beware: insert into t8 values (5); prompt w (unlike mysql client we wait for ';' here)
  Beware: create procedure p () begin end// select 5//
  Beware: input might be a file dump, and statements might be long.
  Todo: think about delimiter. Maybe delimiters don't count if you're in a delimiter statement?
*/
int MainWindow::get_next_statement_in_string()
{
  int i;
  int begin_count;
  QString last_token;
  QString text;

  text= statement_edit_widget->toPlainText(); /* Todo: decide whether I'm doing this too often */
  bool is_maybe_in_compound_statement= 0;
  begin_count= 0;
  for (i= main_token_number; main_token_lengths[i] != 0; ++i)
  {
    last_token= text.mid(main_token_offsets[i], main_token_lengths[i]);
    if (QString::compare(ocelot_delimiter_str, ";", Qt::CaseInsensitive) != 0)
    {
      if (QString::compare(last_token, ocelot_delimiter_str, Qt::CaseInsensitive) == 0)
      {
        ++i; break;
      }
    }
    if ((QString::compare(last_token, ";", Qt::CaseInsensitive) == 0) && (begin_count == 0))
    {
      ++i; break;
    }
    if ((main_token_types[i] == TOKEN_KEYWORD_PROCEDURE)
    ||  (main_token_types[i] == TOKEN_KEYWORD_FUNCTION)
    ||  (main_token_types[i] == TOKEN_KEYWORD_TRIGGER)) is_maybe_in_compound_statement= 1;
    if (is_maybe_in_compound_statement == 1)
    {
      printf("in_compound_statment\n");
      if ((main_token_types[i] == TOKEN_KEYWORD_BEGIN)
      ||  (main_token_types[i] == TOKEN_KEYWORD_ELSEIF)
      ||  (main_token_types[i] == TOKEN_KEYWORD_IF)
      ||  (main_token_types[i] == TOKEN_KEYWORD_LOOP)
      ||  (main_token_types[i] == TOKEN_KEYWORD_REPEAT)
      ||  (main_token_types[i] == TOKEN_KEYWORD_WHILE))
      {
        printf("  ++begin_count\n");
        ++begin_count;
      }
      if (main_token_types[i] == TOKEN_KEYWORD_END)
      {
        printf("  --begin_count\n");
        --begin_count;
      }
    }
  }
  return i - main_token_number;
}


/*
  Todo: This function should do something!
  Make statement ready to send to server: strip comments and delimiter, output UTF8.
  We have an SQL statement that's ready to go to the server.
  We have a guarantee that the result i.e. "char *query" is big enough.
  Rremove comments if that's what the user requested with --skip-comments.
  Remove final token if it's delimiter but not ';'.
  Todo: there's a conversion to UTF8 but it should be to what server expects.
  Todo: um, in that case, make server expect UTF8.
  Re comment stripping: currently this is default because in mysql client it's default
    but there should be no assumption that this will always be the case,
    so some sort of warning might be good. The history display has to include
    the comments even if they're not sent to the server, so the caller of this
    routine will use both (original string,offset,length) and (returned string).
    Comments should be replaced with a single space.
*/
//void make_statement_ready_to_send(char *query)
//{
//  ;
//}


/*
  For menu item "execute" we said (...SLOT(action_execute())));
  By default this is on and associated with File|Execute menu item.
  Execute what's in the statement widget.
  The statement widget might contain multiple statements.
*/
void MainWindow::action_execute()
{
  main_token_number= 0;

  for (;;)
  {
    main_token_count_in_statement= get_next_statement_in_string();
    if (main_token_count_in_statement == 0) break;
    action_execute_one_statement();
    main_token_number+= main_token_count_in_statement;
  }

  statement_edit_widget->clear(); /* ?? this is supposed to be a slot. does that matter? */
  widget_sizer();

  history_edit_widget->show(); /* Todo: find out if this is really necessary */
}


/*
  Todo: There are a few things yet to be considered.
     I'm not 100% confident about the way I'm clearing the old results out, e.g. "delete result_form_text[...]".
     Would it be more efficient to remove only the ones between new column count and old column count?
       No, because data type can change, it might not be a text field the next time around.
     Should I be saying "delete later" somehow?
     Does memory leak?
*/
void MainWindow::action_execute_one_statement()
{
  QString text;
  int query_len;
  MYSQL_RES *mysql_res_for_new_result_set;

  ++statement_edit_widget->statement_count;

  /* Clear the old result sets. */
  //unsigned int old_result_column_count= result_grid_table_widget->result_column_count;

  /*
    Todo: There should be an indication that something is being executed.
    Possibly, on the title bar, setPlainText(tr("Executing ...")
    Possibly there should be a status widget or a progress bar.
    Whatever it is, turn it on now and turn it off when statement is done.
  */

  /*
    When we get a new result set, we want to remove all the old widgets inside result_grid_form.
    It's probably easy, but I didn't know how to get the original layout as a QFormLayout.
    So I delete the old layout too, and make a new one.
    Also I'm unsure whether the "delete item" will get rid of memory leaking, or merely fail.
    Update: "delete item;" fails so I commented it out. Maybe removeItem is failing too, I didn't check.
    Something about "must be implemented in subclasses" is possibly a hint that I'm doing it wrong.
    ... Perhaps I should create MAX_COLUMNS widgets at the start, and never delete them.
    ... Update: Qt documentation talks about clear() in places, maybe relevantly.
  */

  text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */
  /* Apparently there is no need for a call to tokenize() here, it seems certain that it's already been called. */

  int query_utf16_len= main_token_offsets[main_token_number+main_token_count_in_statement - 1]
                       + main_token_lengths[main_token_number+main_token_count_in_statement - 1]
                       - main_token_offsets[main_token_number];

  query_utf16= text.mid(main_token_offsets[main_token_number], query_utf16_len);

  int length_of_last_token_in_statement= main_token_lengths[main_token_number+main_token_count_in_statement - 1];

  QString last_token= text.mid(main_token_offsets[main_token_number+main_token_count_in_statement - 1],
                                 length_of_last_token_in_statement);

  if (last_token == ocelot_delimiter_str)
  {
    query_utf16_copy= text.mid(main_token_offsets[main_token_number], query_utf16_len-length_of_last_token_in_statement);
  }
  else query_utf16_copy= query_utf16;


  int ecs= execute_client_statement();
  if (ecs != 1)
  {

    /* The statement was not handled entirely by the client, it must be passed to the DBMS. */

    /* If DBMS is not (yet) connected, except for SET, this is an error. */
    if (is_mysql_connected == 0)
    {
      statement_edit_widget->result= tr("ERROR not connected");
    }
    else
    {
      query_len= query_utf16_copy.toUtf8().size();           /* See comment "UTF8 Conversion" */
      statement_edit_widget->start_time= QDateTime::currentMSecsSinceEpoch(); /* will be used for elapsed-time display */
      char *query= new char[query_len + 1];
      memcpy(query, query_utf16_copy.toUtf8().constData(), query_len);
      query[query_len]= 0;
      if (mysql_real_query(&mysql, query, query_len))
      {
        delete []query;
      }
      else {
        delete []query;
        /*
          It was a successful SQL statement, and now look if it returned a result.
          If it did, as determined by looking at the mysql_res that mysql_store_result() returns,
          then free the previous mysql_res and delete the previous result grid,
          before setting up new ones.
          This means that statements which don't return result sets don't cause clearing
          of old result sets from the screen, e.g. SET has no effect on result grid.
          This means that momentarily there will be an old mysql_res and a new mysql_res.
          That takes up memory. If it were easy to check in advance whether a statement
          caused a result set (e.g. with mysql_next_result or by looking at whether the
          statement is SELECT SHOW etc.), that would be better.
          Todo: nothing is happening for multiple result sets.
        */
        mysql_res_for_new_result_set= mysql_store_result(&mysql);
        if (mysql_res_for_new_result_set == 0) {
          /*
            Last statement did not cause a result set. We could hide the grid an shrink the
            central window with "result_grid_table_widget->hide()", but we don't.
          */
          ;
        }
        else
        {
          if (mysql_res != 0) mysql_free_result(mysql_res);
          mysql_res= mysql_res_for_new_result_set;
          /* We need to think better what to do if we exceed MAX_COLUMNS */

          /*
            Todo: consider whether it would be appropriate to set grid width with
            result_grid_table_widget->result_column_count= mysql_num_fields(mysql_res);
            but it may be unnecessary, and may cause a crash in garbage_collect()
          */

          result_row_count= mysql_num_rows(mysql_res);                /* this will be the height of the grid */

          /*
            Destroy the old result_grid_table_widget, and make a new one.
            The assumption is that all the old widget's children are destroyed,
            and anything created by "new" from the old widget is garbage-collected.
            To get the new widget into the old layout, we have to renew the layout.
          */
          main_layout->removeWidget(statement_edit_widget);
          main_layout->removeWidget(result_grid_table_widget);
          main_layout->removeWidget(history_edit_widget);
          delete main_layout;

          result_grid_table_widget->garbage_collect();
          QFont tmp_font;
          QFont *saved_font;
          tmp_font= result_grid_table_widget->font();
          saved_font= &tmp_font;

          delete result_grid_table_widget;
          main_layout= new QVBoxLayout();

          result_grid_table_widget= new ResultGrid(mysql_res, saved_font, this);           /* This does a lot of work. */
          result_grid_table_widget->setStyleSheet(ocelot_grid_style_string);   /* Todo: check: already done in constructor? */
          main_layout->addWidget(history_edit_widget);
          main_layout->addWidget(result_grid_table_widget);
          main_layout->addWidget(statement_edit_widget);

          if (main_window->layout() != 0)
          {
            printf("Pseudo-assertion: main_window already has a layout\n"); exit(1);
          }
          main_window->setLayout(main_layout);
          /* 2014-12-10 Following line was shifted -- it used to come after the show(). */
          result_grid_table_widget->installEventFilter(this);                      /* must catch fontChange, show, etc. */
          result_grid_table_widget->show();    /* Todo: consider: isn't this obsolete? Isn't this being shown too early? */
          /*
            The vertical scroll bar for result_grid_table_widget,
            i.e. grid_vertical_scroll_bar, is created during the initial setup.
            Todo: Consider: is it necessary to remove the event filter later?
          */
          result_grid_table_widget->grid_vertical_scroll_bar->installEventFilter(this);
        }
      }
      put_diagnostics_in_result();
    }
  }

  /* statement is over */

  history_markup_append(); /* add prompt+statemen+result to history, with markup */
}


/*
 Handle "client statements" -- statements that the client itself executes.
  Possible client statements:
  SET @OCELOT_GRID_DETACHED= 1
  SET @OCELOT_GRID_DETACHED = 0 (todo)
  DELIMITER delimiter
  CONNECT ... (Todo: CONNECT depends entirely on settings, it should be possible to have arguments)
*/
/*
  Detach:
  Update: any code related to detaching is old and probably doesn't work.
  Would "floating" be a better word than "detached"? or "undock"? or "detachable"?
  ignore if already detached
  make new window etc. -- earlier the idea was dock widgets, but now I'm dubious
  append to "statements to pass on before execution of next real statement"
  return 1;  / * i.e. we don't pass on immediately, it's a client statement  * /

  Todo: Finishing off the "SET @OCELOT_GRID_DETACHED=1;" tricks ...
  None of the menu keys work. They must be slotted to the new window as well as the old window. E.g. control-Q.
  "SET @OCELOT_GRID_DETACHED= 0;" should reverse the effect.
  Consider the title bar. Should it be possible to close it, and what should the effect be?
  Initial window dimension + position should be = current widget dimention + position (unless it's hidden or obscured).
*/
/*
  Todo: We want all the mysql commands to work just the way that they work in mysql.
        We will not depend on TOKEN_KEYWORD_... for this because "\" and "q" are separate tokens,
        and "q| is case sensitive.
*/

int MainWindow::execute_client_statement()
{
//  int i= 0;
  int i2= 0;
  int  sub_token_offsets[10];
  int  sub_token_lengths[10];
  int  sub_token_types[10];
  int is_client_format= 0;
  int true_text_size, true_token_count; /* size and count ignoring delimiter */
  QString text;                      /* Todo: see if text should be a global */
  unsigned int statement_type;

  statement_type= main_token_types[main_token_number];     /* We used to use main_statement_type */

  text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */

  /* Calculate size. Don't count delimiter unless it's a delimiter statement. */
  true_text_size= query_utf16_copy.size();

  int query_utf16_size= query_utf16.size();
  int query_utf16_copy_size= query_utf16_copy.size();
  if ((query_utf16_size == query_utf16_copy_size) || (statement_type == TOKEN_KEYWORD_DELIMITER))
  {
    /* there was no delimiter, or there was a delimiter but it's a delimiter statement */
    true_text_size= query_utf16_size;
    true_token_count= main_token_count_in_statement;
  }
  else
  {
    /* There was a delimiter, and it's not a delimiter statement */
    true_text_size= query_utf16_copy_size;
    true_token_count= main_token_count_in_statement - 1;
  }

  /* Make a copy of the first few tokens, ignoring comments. */
  for (unsigned int i= main_token_number; /* main_token_lengths[i] != 0 && */ i < main_token_number + true_token_count; ++i)
  {
    /* Todo: find out why you have to figure out type again -- isn't it already known? */
    QString s= text.mid(main_token_offsets[i], main_token_lengths[i]);
    int t= token_type(s.data(), main_token_lengths[i]);
    if (t == TOKEN_TYPE_COMMENT_WITH_SLASH
    ||  t == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE
    ||  t == TOKEN_TYPE_COMMENT_WITH_MINUS) continue;
    sub_token_offsets[i2]= main_token_offsets[i];
    sub_token_lengths[i2]= main_token_lengths[i];
    sub_token_types[i2]= main_token_types[i];
    ++i2;
    if (i2 > 8) break;
  }
  sub_token_offsets[i2]= 0;
  sub_token_lengths[i2]= 0;
  sub_token_types[i2]= 0;

  /*
    CONNECT or \r.
    Defaults should have been set earlier. The user had a chance to change the defaults with SET.
    Currently the only possible dbms is "mysql", otherwise nothing happens.
    Ignore any other words in the statement.
    Todo: accept the mysql-client syntax, which has a few extras on it for reconnect.
  */
  /* Todo: We could easily modify so that we don't need sub_token_..., we could just skip comments. */
  if (statement_type == TOKEN_KEYWORD_CONNECT)
  {
      if (QString::compare(ocelot_dbms, "mysql", Qt::CaseInsensitive) == 0) connect_mysql();
      return 1;
  }

  /* QUIT or \q. mysql equivalent. Todo: add to history box before exiting. */
  if ((statement_type == TOKEN_KEYWORD_QUIT)
  ||  (statement_type == TOKEN_KEYWORD_EXIT))
  {
    action_exit();
  }

  /*
     CLEAR or \c: mysql equivalent.
     Really this just means "ignore the statement".
     It goes into history though.
  */
  if (statement_type == TOKEN_KEYWORD_CLEAR)
  {
    return 1;
  }

  /* USE or \u: mysql equivalent. */
  if (statement_type == TOKEN_KEYWORD_USE)
  {
    QString s;
    int mysql_select_db_result;
    if ((i2 >= 2) && (sub_token_types[0] == TOKEN_KEYWORD_USE)) s= text.mid(sub_token_offsets[1], sub_token_lengths[1]);
    else if (i2 >= 3) s= text.mid(sub_token_offsets[2], sub_token_lengths[2]);
    else
    {
      statement_edit_widget->result= tr("Error. USE statement has no argument.");
      return 1;
    }
    /* If database name is in quotes or delimited, strip. Todo: stripping might be necessary in lots of cases. */
    s= connect_stripper(s);
    int query_len= s.toUtf8().size();                  /* See comment "UTF8 Conversion" */
    char *query= new char[query_len + 1];
    memcpy(query, s.toUtf8().constData(), query_len + 1);

    mysql_select_db_result= mysql_select_db(&mysql, query);
    delete []query;
    if (mysql_select_db_result != 0) put_diagnostics_in_result();
    else
    {
      statement_edit_widget->dbms_database= s;
      statement_edit_widget->result= tr("Database changed");
    }
    return 1;
  }

  /* SOURCE or \.: mysql equivalent. */
  if (statement_type == TOKEN_KEYWORD_SOURCE)
  {
    /* Everything as far as statement end is source file name. */
    /* Todo: if we fill up the line, return an overflow message,
       or make line[] bigger and re-read the file. */
    /* Executing the source-file statements is surprisingly easy: just put them in
       the statement widget. This should be activating action_statement_edit_widget_changed
       and that ultimately causes execution. Handling multiple statements per line is okay.
       Difference from mysql client: this puts source-file statements in history, mysql client puts "source" statement. */
    /* Todo: stop recursion i.e. source statement within source statement. That's an error. */
    QString s;
    unsigned statement_length= /* text.size() */ true_text_size;
    if (i2 >= 2) s= text.mid(sub_token_offsets[1], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
    else
    {
      statement_edit_widget->result= tr("Error, SOURCE statement has no argument");
      return 1;
    }
    int query_len= s.toUtf8().size();                  /* See comment "UTF8 Conversion" */
    char *query= new char[query_len + 1];
    memcpy(query, s.toUtf8().constData(), query_len + 1);
    query[query_len]= '\0';
    FILE *file= fopen(query, "r");                     /* Open specified file, read only */
    delete []query;
    if (file == NULL)
    {
      statement_edit_widget->result= tr("Error, fopen failed");; return 1;
    }
    char line[2048];
    while(fgets(line, sizeof line, file) != NULL)
    {
      QString source_line= line;
      statement_edit_widget->insertPlainText(source_line);
    }
      fclose(file);
      return 1;
    }

  /* PROMPT or \R: mysql equivalent. */
  if (statement_type == TOKEN_KEYWORD_PROMPT)
  {
    QString s;
    unsigned statement_length= true_text_size /* text.size() */;
    if ((i2 >= 2) && (sub_token_types[1] != TOKEN_KEYWORD_USE))
      s= text.mid(sub_token_offsets[1], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
    else if (i2 >= 3) s= text.mid(sub_token_offsets[2], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
    else
    {
      statement_edit_widget->prompt_as_input_by_user=statement_edit_widget->prompt_default;
      /* Todo: output a message */
      return 1;
    }
    statement_edit_widget->prompt_as_input_by_user= s;
    statement_edit_widget->result= tr("OK");
    return 1;
  }

  /* WARNINGS or \W: mysql equivalent. */
  if (statement_type == TOKEN_KEYWORD_WARNINGS)
  {
    ocelot_history_includes_warnings= 1;
    statement_edit_widget->result= tr("OK");
    return 1;
  }

  /* NOWARNING or \w: mysql equivalent. */
  if (statement_type == TOKEN_KEYWORD_NOWARNING)
  {
    ocelot_history_includes_warnings= 0;
    statement_edit_widget->result= tr("OK");
    return 1;
  }

  /* DELIMITER or \d: mysql equivalent. */
  if (statement_type == TOKEN_KEYWORD_DELIMITER)
  {
    if (sub_token_lengths[1] == 0)
    {
      statement_edit_widget->result= tr("Error, delimiter should not be blank");
      return 1;
    }
    /* delimiter = rest of string except lead/trail whitespace */
    QString s;
    unsigned statement_length= true_text_size /* text.size() */;
    s= text.mid(sub_token_offsets[1], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
//        if (s.contains("/")) {
//            statement_edit_widget->result= tr("Error, delimiter should not contain slash");
//            return 1; }
    ocelot_delimiter_str= s.trimmed(); /* Todo: probably trimmed() isn't necessary, but make sure */
    statement_edit_widget->result= tr("OK");
    return 1;
    }

  /* Todo: the following are placeholders, we want actual actions like what mysql would do. */
  if (statement_type == TOKEN_KEYWORD_QUESTIONMARK)
  {
    statement_edit_widget->result= tr("HELP is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_CHARSET)
  {
    statement_edit_widget->result= tr("CHARSET is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_EDIT)
  {
    statement_edit_widget->result= tr("EDIT is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_EGO)
  {
    statement_edit_widget->result= tr("EGO is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_GO)
  {
    statement_edit_widget->result= tr("GO is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_HELP)
  {
    statement_edit_widget->result= tr("HELP is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_NOPAGER)
  {
    statement_edit_widget->result= tr("NOPAGER is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_NOTEE)
  {
    statement_edit_widget->result= tr("NOTEE is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_PAGER)
  {
    statement_edit_widget->result= tr("PAGER is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_PRINT)
  {
    statement_edit_widget->result= tr("PRINT is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_REHASH)
  {
    statement_edit_widget->result= tr("REHASH is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_STATUS)
  {
    statement_edit_widget->result= tr("STATUS is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_SYSTEM)
  {
    statement_edit_widget->result= tr("SYSTEM is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_TEE)
  {
    statement_edit_widget->result= tr("TEE is not implemented.");
    return 1;
  }

  /* See whether general format is SET @ocelot_... = value ;" */
  if (i2 > 4)
  {
    if (sub_token_types[0] == TOKEN_KEYWORD_SET)
    {
      /* Todo: make @ocelot_grid_detached a keyword so you can compare sub_token_types[i] instead. */
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@OCELOT_GRID_DETACHED", Qt::CaseInsensitive) == 0)
      {
        /* Todo: we should compare the rest of the tokens too. */
        is_client_format= 1;
      }
    }
  }

  if (is_client_format == 1)
  {
    /*
      Todo: Here is where we should make a new window. See earlier comments about detaching.
      But the code is old, probably doesn't work any more, and is therefore commented out.
      Todo: consider: should the title bar contain a title?
      Todo: consider alternative non-docking method.
        addWidget                      <- to new window
        setvisible(true)
        new window->show()
        You still have the problem that it won't be at the front, and will be hidden if main window is maximized.
        But there's a way to tell Qt to put something at the front. Look up: raise().
    */
    //test_dock= new QDockWidget(tr(""), this);
    //test_dock->setWidget(statement_edit_widget);
    //addDockWidget(Qt::BottomDockWidgetArea, test_dock);
    //test_dock->show();
    //return 1;
    //statement_edit_widget->hide();                       /* ?? or maybe I should be saying setvisible(false) */
    //main_layout->removeWidget(statement_edit_widget);
    //statement_edit_widget->setParent(0);           /* ?? does removeWidget automatically remove parent anyway? */
    ///* Actually I should be making this a component of a new window, no? */
    //statement_edit_widget->show();
    //return 1;
  }

  return 0;
}


/*
  Call widget_sizer() from action_execute() to resize the three
  main_layout widgets if necessary.
  Todo: so far widget_sizer() is just ideas, mostly unimplemented.
  The widgets are in a layout (I don't think it's possible to have them in a layout if one uses dock windows).
  But the maximum sizing can be helped by me. Some rules are:
  * If history_edit_widget is higher than needed without scrolling, shrink it.
  * If user has manually expanded a window, leave it.
    ** But a grid should snap back to a minimum size
  * A select() should be visible, up to some maximum line count, even if a result set has many rows.
  * We need a scroll bar for the whole main window. But maybe not for all the main_layout widgets?
  * The widget might be undocked -- in that case the sizes of other docked widgets won't matter.
  * The statement window can shrink quite a bit because after action_execute() there's nothing in it.
  Probably TitleBarHeight won't matter because we set the bar to vertical (update: no, I gave up on vertical)
  Probably PM_DefaultFrameWidth won't matter but I'm not sure what it is.
  Probably there is no space between dock windows. ... Update: no, look at it, there is a space.
  So far all I've thought about is a special case for history_dock.
    It's a bit silly to allow for height of a horizontal scroll bar, if there is no scroll bar.
  Maybe we should use the WA_DontShowOnScreen trick.
*/
void MainWindow::widget_sizer()
{
  int history_edit_widget_height;
  /*
    This would work, but only if show() is called before widget_sizer().
    But if history_edit_widget becomes something other than plain text,
    we'd have to do something like this ...
    history_edit_widget_height= history_edit_widget->document()->size().height();
    history_edit_widget->setMaximumHeight(history_edit_widget_height);
  */
  QFontMetrics history_font(history_edit_widget->font());
  history_edit_widget_height= (history_edit_widget->document()->lineCount() + 1) * history_font.lineSpacing();
  history_edit_widget_height+= QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent);
  history_edit_widget->setMaximumHeight(history_edit_widget_height);
}


/*
  If the response to a MySQL query is an error, put error information
  in statement_edit_widget->result and return error number. Else return 0.
  This should only be called for mysql calls, not for client commands.
*/
/*
  Todo: the elapsed-time calculation in the diagnostics is calculated at the wrong time,
  it includes the time to set up the widgets. Move it to just after statement-execution.
*/
void MainWindow::put_diagnostics_in_result()
{
  unsigned int mysql_errno_result;
  char mysql_error_and_state[50]; /* actually we should need less than 50 */
  QString s1, s2;

  mysql_errno_result= mysql_errno(&mysql);
  if (mysql_errno_result == 0)
  {
    s1= tr("OK");

    /* This should output, e.g. "Records: 3 Duplicates: 0 Warnings: 0" -- but actually nothing happens. */
    if (mysql_info(&mysql)!=NULL)
    {
      /* This only works for certain insert, load, alter or update statements */
      s1.append(tr(mysql_info(&mysql)));
    }
    else
    {
      sprintf(mysql_error_and_state, " %llu rows affected", mysql_affected_rows(&mysql));
      s1.append(mysql_error_and_state);
    }
    //printf("info=%s.\n", mysql_info(&mysql));
    /* Add to display: how long the statement took, to nearest tenth of a second. Todo: fix calculation. */
    qint64 statement_end_time= QDateTime::currentMSecsSinceEpoch();
    qint64 elapsed_time= statement_end_time - statement_edit_widget->start_time;
    long int elapsed_time_as_long_int= (long int) elapsed_time;
    float elapsed_time_as_float= (float) elapsed_time_as_long_int / 1000;
    sprintf(mysql_error_and_state, "(%.1f seconds)", elapsed_time_as_float);
    s1.append(mysql_error_and_state);
    if (mysql_warning_count(&mysql) > 0)
    {
      if (ocelot_history_includes_warnings == 1)
      {
        mysql_query(&mysql, "show warnings");
        MYSQL_RES *mysql_res_for_warnings;
        MYSQL_ROW warnings_row;
        QString s;
        // unsigned long connect_lengths[0];
        mysql_res_for_warnings= mysql_store_result(&mysql);
        for (unsigned int wi= 0; wi <= mysql_warning_count(&mysql); ++wi)
        {
          warnings_row= mysql_fetch_row(mysql_res_for_warnings);
          if (warnings_row!=NULL)
          {
            /* lengths= mysql_fetch_lengths(connect_res); */
            sprintf(mysql_error_and_state, "\n%s (%s) %s.", warnings_row[0], warnings_row[1], warnings_row[2]);
            s1.append(mysql_error_and_state);
            printf("row[0]=%s. row[1]=%s. row[2]=%s.\n", warnings_row[0], warnings_row[1], warnings_row[2]);
          }
        }
        mysql_free_result(mysql_res_for_warnings);
      }
    }
  }
  if (mysql_errno_result > 0)
  {
    s1= tr("Error ");
    sprintf(mysql_error_and_state, "%d (%s) ", mysql_errno_result, mysql_sqlstate(&mysql));
    s1.append(mysql_error_and_state);
    s2= mysql_error(&mysql);
    s1.append(s2);
  }
  statement_edit_widget->result= s1;
}


/*
   tokenize(): Produce a list of tokens given an SQL statement using MySQL rules.

   Example: SELECT `a` FROM t; will give:
     token_offsets[0] -> SELECT, token_lengths=0] = 6
     token_offsets[1] -> `a`, token_lengths[1] = 3
     token_offsets[2] -> FROM, token_lengths[2] = 4
     token_offsets[3] -> t, token_lengths[3] = 1
     token_offsets[4] -> ;, token_lengths[4] = 1
     token_offsets[5] -> `'0', token_lengths[5] = 0
   Most character sets are OK because all breaks are ASCII.

  What to pass:
    Token_lengths[] gets the length of the token
    Token_offsets[] gets where the token starts within text
    Max_tokens is the size of token_lengths[] and token_offsets
      This is what I actually depend on but C++ insists I state the size.
    Version is what to compare when you say / * ! nnnnn
      It must be a 5-character string.
      It will be compared with the 5 digits following '!', if any
      If comparison fails, / * ! is treated as / * style comment
    Comment behaviour influences what will happen with comments
      1: save whole comment as one token.
         useful for syntax highlighting.
      2: ignore whole comment.
         useful for debug.
      3: save whole comment as multiple tokens, include markers
         useful for html markup.
      4: save whole comment as multiple tokens, ignore markers
    Special_token is a string, possibly blank, which will be treated as a single n-byte token
    even if it contains special characters. In this program we use it for delimiters.
    Minus behaviour influences what will happen with '-' i.e. 0x2d i.e. hyphen-minus
      1: '-' is an operator, this is the norm when tokenizing SQL
      2: '-' is a token part, this is the norm when tokenizing options

  Adjusting the tokenizer for Qt:
    With tokenize(QString text, char* comment_handling) we find the tokens of a string in statement.
    The results go to token_offsets[] and token_lengths[].
    According to Qt documentation:
      QString contains QChars. It's really UTF-16 except that a surrogate pair counts as two QChars.
      I won't use Qchar's helpers (isHighSurrogate, isLowSurrogate, isDigit, isLetter, isSpace, isNull)
      because I can't be sure that MySQL/MariaDB parser will always act the same way.
      I'm assuming nobody has played with the exec's codec, whatever that means.
      I'm assuming that the compiler will treat each QChar as unsigned.
      So I know that if something is "QString str" then I can compare characters within it to
      "QChar('u')" or "QChar(0x00e7)" etc. and I can get substrings with str.mid(position,length).
  The original program is tokenize.c which is a standalone that uses unsigned char* not QString.
*/

void MainWindow::tokenize(QChar *text, int text_length, int (*token_lengths)[MAX_TOKENS],
                           int (*token_offsets)[MAX_TOKENS], int max_tokens,QChar *version,
                           int passed_comment_behaviour, QString special_token, int minus_behaviour)
{
  int token_number;
  QChar expected_char;
  int char_offset;
  QChar expected_char_at_comment_end;
  int comment_behaviour;
  QChar version_inside_comment[6];
  int n;

  expected_char_at_comment_end=' ';
  comment_behaviour=passed_comment_behaviour;
  token_number= 0;
  char_offset= 0;
next_token:
  (*token_lengths)[token_number]= 0;
  (*token_offsets)[token_number]= char_offset;
next_char:
  if (token_number >= (max_tokens - 1)) goto string_end;
  /* Following IFs happen to be in order by ASCII code */
  if (char_offset >= text_length) goto string_end; /* this is the usual check for end of string */
  if (text[char_offset] == 0) goto string_end;  /* \0 end of string (probably will never happen) */
  /* Check whether special_token occurs at this point. */
  {
    int special_token_length, special_token_offset;
    special_token_length= special_token.length();
    if ((special_token_length > 0)
    &&  (char_offset + special_token_length <= text_length))
    {
      for (special_token_offset= 0; special_token_offset < special_token_length; ++special_token_offset)
      {
        QChar special_token_char= special_token.at(special_token_offset);
        if (special_token_char != text[char_offset+special_token_offset]) break;
      }
      if (special_token_offset == special_token_length)
      {
        n= special_token_length;
        goto n_byte_token;
      }
    }
  }
  if (text[char_offset] <= ' ') goto white_space; /* space tab ff etc. = white space */
  if (text[char_offset] == '!')                 /* ! might be start of != otherwise one-byte token */
  {
    expected_char= '=';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == '"')    /* " starts a token until next " but watch for ""s and end-of-string and escape */
  {
    expected_char= '"';
    goto skip_till_expected_char;
  }
  if (text[char_offset] == '#')          /* # starts a comment */
  {
    expected_char= '\n';
    goto comment_start;
  }
  if (text[char_offset] == '%') goto one_byte_token; /* % one-byte token */
  if (text[char_offset] == '&')     /* & might be start of &&. otherwise one-byte token */
  {
    expected_char= '&';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == 39)     /* ' starts a token until next ' but watch for end-of-string and escape */
  {
    expected_char= 39;
    goto skip_till_expected_char;
  }
  if (text[char_offset] == '(') goto one_byte_token; /* ( one-byte token */
  if (text[char_offset] == ')') goto one_byte_token; /* ) one-byte token */
  if (text[char_offset] == '*') /* * one-byte token unless * / and we're inside a comment */
  {
    if ((expected_char_at_comment_end == '/') && (char_offset + 1 < text_length) && (text[char_offset + 1] == '/'))
    {
      expected_char_at_comment_end= ' ';
      if (comment_behaviour == 3)
      {
        expected_char= '/';
        goto skip_till_expected_char;
      }
      if (comment_behaviour == 4)
      {
        n= 2;
        goto n_byte_token_skip;
      }
    }
    else goto one_byte_token;
  }
  if (text[char_offset] == '+') goto one_byte_token; /* + one-byte token */
  if (text[char_offset] == ',') goto one_byte_token; /* , one-byte token */
  if (minus_behaviour != 2) {
    if (text[char_offset] == '-')     /* - one-byte token, unless -- comment */
    {
      if ((char_offset + 1 < text_length) && (text[char_offset + 1]  == '-'))
      {
        if ((char_offset + 2 < text_length) && (text[char_offset + 2] <= ' '))
        {
          expected_char= '\n';
          goto comment_start;
        }
      }
      goto one_byte_token;
    }
  }
  if (text[char_offset] == '.')     /* . part of token if previous or following is digit. otherwise one-byte token */
  {
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] >= '0') && (text[char_offset + 1] <= '9')) goto part_of_token;
    if ((*token_lengths)[token_number] > 0)
    {
      if ((text[char_offset - 1] >= '0') && (text[char_offset - 1] <= '9')) goto part_of_token;
    }
    goto one_byte_token;
  }
  if (text[char_offset] == '/') /* / one-byte token unless it's '/' followed by '*' */
  {
    expected_char= '/';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == '*')) goto comment_starting_with_slash_start;
    goto one_byte_token;
  }
  if ((text[char_offset] >= '0') && (text[char_offset] <= '9')) goto part_of_token; /* digit part of token */
  if (text[char_offset] == ':')    /* : might be start of := otherwise one-byte token */
  {
    expected_char= '=';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == ';') goto one_byte_token; /* ; one-byte token */
  if (text[char_offset] == '<')       /* < might be start of <=  or << or <=> or <>. otherwise one-byte token */
  {
    expected_char= '=';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char))
    {
      if ((char_offset + 2 < text_length) && (text[char_offset + 2] == '>'))
      {
        expected_char= '>';
        goto skip_till_expected_char;
      }
      goto skip_till_expected_char;
    }
    expected_char= '>';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    expected_char= '<';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == '=') goto one_byte_token; /* = one-byte token */
  if (text[char_offset] == '>')   /* > might be start of >= or >>. otherwise one-byte token */
  {
    expected_char= '=';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    expected_char= '>';
    if ((char_offset + 1  < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == '?') goto one_byte_token; /* ? one-byte token? */
  if (text[char_offset] == '@') /* @ start of @token or start of @@token otherwise one-byte token */
  {
    if ((*token_lengths)[token_number] == 0) goto part_of_token;
    if ((*token_lengths)[token_number] == 1)
    {
      if (text[char_offset - 1] == '@') goto part_of_token;
    }
    goto one_byte_token;
  }
  if (text[char_offset] == '[') goto one_byte_token; /* [ one-byte token which is never used */
  if (text[char_offset] == 92)  goto one_byte_token; /* \ one-byte token which is never used */
  if (text[char_offset] == ']') goto one_byte_token; /* ] one-byte token which is never used */
  if (text[char_offset] == '^') goto one_byte_token; /* ^ one-byte token */
  if (text[char_offset] == '`')        /* ` starts a token until next ` but watch for end-of-string and escape */
  {
    expected_char= '`';
    goto skip_till_expected_char;
  }
  if (text[char_offset] == '{')      /* { starts a token until next } but watch for end-of-string */
  {
    expected_char= '}';
    goto skip_till_expected_char;
  }
  if (text[char_offset] == '|')      /* | might be start of ||. otherwise one-byte token */
  {
    expected_char= '|';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == '}') goto one_byte_token; /* } one-byte token which is never used */
  if (text[char_offset] == '~') goto one_byte_token; /* ~ one-byte token */
  /* Remaining possibilities are:
     $, 0 to 9, A to Z, a to z
     >127 (top-bit-on is probably a continuation byte of a utf-8 character)
     _ DEL */
part_of_token:
  ++char_offset;
  ++(*token_lengths)[token_number];
  goto next_char;
string_end:
  if ((*token_lengths)[token_number] > 0) (*token_lengths)[token_number + 1]= 0;
  return;
white_space:
  if ((*token_lengths)[token_number] > 0) ++token_number;
  ++char_offset;
  goto next_token;
comment_starting_with_slash_start:
comment_start:
  comment_behaviour= passed_comment_behaviour;

  if (text[char_offset] == '/')
  {
    if ((char_offset + 2 < text_length) && (text[char_offset + 2] == '!'))
    {
      if ((char_offset + 8 < text_length) && (text[char_offset + 3] >= '0') && (text[char_offset + 3] <= '9'))
      {
        version_inside_comment[0]= text[char_offset + 3];
        version_inside_comment[1]= text[char_offset + 4];
        version_inside_comment[2]= text[char_offset + 5];
        version_inside_comment[3]= text[char_offset + 6];
        version_inside_comment[4]= text[char_offset + 7];
        version_inside_comment[5]= 0;
      }
      else
      {
        version_inside_comment[0]= 0;
      }
      if (strcmp((char*)version, (char*)version_inside_comment) >= 0)
      {
        if (comment_behaviour == 1) comment_behaviour= 3;
        if (comment_behaviour == 2) comment_behaviour= 4;
        expected_char= ' ';
        expected_char_at_comment_end= '/';
        if (comment_behaviour == 4)
        {
          char_offset+= 3 + strlen((char*)version_inside_comment);
          goto next_token;
        }
        goto skip_till_expected_char;
      }
    }
  }

  if (comment_behaviour == 3)
  {
    expected_char_at_comment_end= expected_char; /* / or \n */
    if (text[char_offset] == '#') goto one_byte_token;
    if (text[char_offset] == '-') expected_char='-';
    if (text[char_offset] == '/') expected_char='*';
  }
  if (comment_behaviour == 4)
  {
    /* comment_behaviour=4 means ignore / * and * / but not comment contents */
    expected_char_at_comment_end= expected_char; /* / or \n */
    if (text[char_offset] == '#') n= 1;
    else n= 2;
    goto n_byte_token_skip;
  }
skip_till_expected_char:
  /* No break if we're facing N'...' or X'...' or B'...' or @`...` or @a"..." */
  /* Todo: check: I might have forgotten N"..." etc. */
  if ((*token_lengths)[token_number] == 1)
  {
    if (expected_char == 39)
    {
      if ((text[char_offset - 1] == 'N') || (text[char_offset - 1] == 'X') || (text[char_offset - 1] == 'B')
              ||(text[char_offset - 1] == 'n') || (text[char_offset - 1] == 'x') || (text[char_offset- 1] == 'b'))
      {
        goto skip_till_expected_char_2;
      }
    }
    if ((expected_char == '`') || (expected_char == '"'))
    {
      if (text[char_offset - 1] == '@')
      {
        goto skip_till_expected_char_2;
      }
    }
  }
  if ((*token_lengths)[token_number] > 0)
  {
    ++token_number;
    (*token_lengths)[token_number]= 0;
    (*token_offsets)[token_number]= char_offset;
  }
skip_till_expected_char_2:
  ++char_offset;
  ++(*token_lengths)[token_number];
  if (char_offset >= text_length) goto string_end;
  if (text[char_offset] == 0) goto string_end;
  if (text[char_offset] != expected_char) goto skip_till_expected_char_2;
  /* No match if / not preceded by *, \' or '' inside ''s, \" or "" inside ""s or `` inside ``s */
  if ((expected_char == '/') && (text[char_offset - 1] != '*')) goto skip_till_expected_char_2;
  if ((expected_char == 39) && (text[char_offset - 1] == 92)) goto skip_till_expected_char_2;
  if ((expected_char == '"') && (text[char_offset - 1] == 92)) goto skip_till_expected_char_2;
  if ((expected_char == 39) || (expected_char == '"') || (expected_char == '`'))
  {
    if ((char_offset + 1 < text_length) && (expected_char == text[char_offset + 1]))
    {
      ++char_offset;
      ++(*token_lengths)[token_number];
      goto skip_till_expected_char_2;
    }
  }
  ++char_offset;
  ++(*token_lengths)[token_number];
  if ((expected_char == '/') || (expected_char == '\n'))
  {
    if (comment_behaviour == 2)
    {
      /*
        comment_behaviour=2 means "ignore comments (treat them as whitespace)",
        so ignore, i.e. go for next token without incrementing token_number.
      */
      goto next_token;
    }
    if (comment_behaviour == 4)
    {
      if (expected_char == expected_char_at_comment_end)
      {
        /*
          comment_behaviour=4 means "don't ignore comments but treat begin/end
          marks as whitespace" so declare there's no more end-marks, then ignore.
        */
        expected_char_at_comment_end= ' ';
        goto next_token;
      }
    }
  }
  ++token_number;
  goto next_token;
one_byte_token: /* we know that coming token length is 1 */
  if ((*token_lengths)[token_number] > 0) ++token_number;
  (*token_offsets)[token_number]= char_offset;
  (*token_lengths)[token_number]= 1;
  ++char_offset;
  ++token_number;
  goto next_token;
n_byte_token:   /* we know that coming token length is n */
  if ((*token_lengths)[token_number] > 0) ++token_number;
  (*token_offsets)[token_number]= char_offset;
  (*token_lengths)[token_number]= n;
  char_offset+= n;
  ++token_number;
  goto next_token;
n_byte_token_skip:
  if ((*token_lengths)[token_number] > 0) ++token_number;
  char_offset+= n;
  goto next_token;
}


/*
  token_type() should be useful for syntax highlighting and for hovering.
  Pass: token, token length. we assume it's at least 1.
  Return: type
  I could have figured this out during tokenize(), but didn't.
  Todo: distinguish between keyword, regular identifier, reserved word.
        There is a check elsewhere for keywords though, see tokens_to_keywords().
*/
int MainWindow::token_type(QChar *token, int token_length)
{
  if (token_length == 0) return TOKEN_TYPE_OTHER;
  /* Check whether token == delimiter_str. similar to a comparison with special_token in tokenize(). */
  {
    int special_token_length, special_token_offset;
    special_token_length= ocelot_delimiter_str.length();
    if ((special_token_length > 0)
    &&  (special_token_length == token_length))
    {
      for (special_token_offset= 0; special_token_offset < special_token_length; ++special_token_offset)
      {
        QChar special_token_char= ocelot_delimiter_str.at(special_token_offset);
        if (special_token_char != *(token+special_token_offset)) break;
      }
      if (special_token_offset == special_token_length)
      {
        return TOKEN_TYPE_OPERATOR;
      }
    }
  }

  if (*token == 39) return TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE;
  if (token_length > 1)
  {
    if ((*token == 'N') && (*(token + 1) == 39)) return TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE;
    if ((*token == 'X') && (*(token + 1) == 39)) return TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE;
    if ((*token == 'B') && (*(token + 1) == 39)) return TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE;
  }
  if (*token == '"') return TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE;
  if ((*token >= '0') && (*token <= '9')) return TOKEN_TYPE_LITERAL_WITH_DIGIT;
  if (*token == '{') return TOKEN_TYPE_LITERAL_WITH_BRACE;
  if (*token == '`') return TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK;
  if (*token == '@') return TOKEN_TYPE_IDENTIFIER_WITH_AT;
  if (token_length > 1)
  {
    if ((*token == '/') && (*(token + 1) == '*')) return TOKEN_TYPE_COMMENT_WITH_SLASH;
    if ((*token == '*') && (*(token + 1) == '/')) return TOKEN_TYPE_COMMENT_WITH_SLASH;
  }
  if (*token == '#') return TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE;
  if (token_length > 1)
  {
    if ((*token == '-') && (*(token + 1) == '-')) return TOKEN_TYPE_COMMENT_WITH_MINUS;
  }
  if ((*token > ' ') && (*token < 'A')) return TOKEN_TYPE_OPERATOR;
  return TOKEN_TYPE_OTHER;
}


/*
  I got tired of repeating
  if (QString::compare(text.mid(main_token_offsets[0], main_token_lengths[0]), "SELECT", Qt::CaseInsensitive) == 0) ...
  so switched to maintaining a permanent list.
  Two compiler-dependent assumptions: bsearch() exists, and char* can be converted to unsigned long.
*/
/* Todo: use "const" and "static" more often */
void MainWindow::tokens_to_keywords()
{
  /*
    Sorted list of keywords.
    If you change this, you must also change bsearch parameters and change TOKEN_KEYWORD list.
  */
  const char strvalues[][30]=
  {
    "?", /* Ocelot keyword, although tokenize() regards it as an operator */
    "ACCESSIBLE",
    "ADD",
    "ALL",
    "ALTER",
    "ANALYZE",
    "AND",
    "AS",
    "ASC",
    "ASENSITIVE",
    "BEFORE",
    "BEGIN",
    "BETWEEN",
    "BIGINT",
    "BINARY",
    "BLOB",
    "BOTH",
    "BY",
    "CALL",
    "CASCADE",
    "CASE",
    "CHANGE",
    "CHAR",
    "CHARACTER",
    "CHARSET", /* Ocelot keyword */
    "CHECK",
    "CLEAR", /* Ocelot keyword */
    "COLLATE",
    "COLUMN",
    "CONDITION",
    "CONNECT", /* Ocelot keyword */
    "CONSTRAINT",
    "CONTINUE",
    "CONVERT",
    "CREATE",
    "CROSS",
    "CURRENT_DATE",
    "CURRENT_TIME",
    "CURRENT_TIMESTAMP",
    "CURRENT_USER",
    "CURSOR",
    "DATABASE",
    "DATABASES",
    "DAY_HOUR",
    "DAY_MICROSECOND",
    "DAY_MINUTE",
    "DAY_SECOND",
    "DEC",
    "DECIMAL",
    "DECLARE",
    "DEFAULT",
    "DELAYED",
    "DELETE",
    "DELIMITER", /* Ocelot keyword */
    "DESC",
    "DESCRIBE",
    "DETERMINISTIC",
    "DISTINCT",
    "DISTINCTROW",
    "DIV",
    "DO",
    "DOUBLE",
    "DROP",
    "DUAL",
    "EACH",
    "EDIT", /* Ocelot keyword */
    "EGO", /* Ocelot keyword */
    "ELSE",
    "ELSEIF",
    "ENCLOSED",
    "END",
    "ESCAPED",
    "EXISTS",
    "EXIT",
    "EXPLAIN",
    "FALSE",
    "FETCH",
    "FLOAT",
    "FLOAT4",
    "FLOAT8",
    "FOR",
    "FORCE",
    "FOREIGN",
    "FROM",
    "FULLTEXT",
    "FUNCTION",
    "GET",
    "GO", /* Ocelot keyword */
    "GRANT",
    "GROUP",
    "HAVING",
    "HELP", /* Ocelot keyword */
    "HIGH_PRIORITY",
    "HOUR_MICROSECOND",
    "HOUR_MINUTE",
    "HOUR_SECOND",
    "IF",
    "IGNORE",
    "IN",
    "INDEX",
    "INFILE",
    "INNER",
    "INOUT",
    "INSENSITIVE",
    "INSERT",
    "INT",
    "INT1",
    "INT2",
    "INT3",
    "INT4",
    "INT8",
    "INTEGER",
    "INTERVAL",
    "INTO",
    "IO_AFTER_GTIDS",
    "IO_BEFORE_GTIDS",
    "IS",
    "ITERATE",
    "JOIN",
    "KEY",
    "KEYS",
    "KILL",
    "LEADING",
    "LEAVE",
    "LEFT",
    "LIKE",
    "LIMIT",
    "LINEAR",
    "LINES",
    "LOAD",
    "LOCALTIME",
    "LOCALTIMESTAMP",
    "LOCK",
    "LONG",
    "LONGBLOB",
    "LONGTEXT",
    "LOOP",
    "LOW_PRIORITY",
    "MASTER_BIND",
    "MASTER_SSL_VERIFY_SERVER_CERT",
    "MATCH",
    "MAXVALUE",
    "MEDIUMBLOB",
    "MEDIUMINT",
    "MEDIUMTEXT",
    "MIDDLEINT",
    "MINUTE_MICROSECOND",
    "MINUTE_SECOND",
    "MOD",
    "MODIFIES",
    "NATURAL",
    "NONBLOCKING",
    "NOPAGER", /* Ocelot keyword */
    "NOT",
    "NOTEE", /* Ocelot keyword */
    "NOWARNING", /* Ocelot keyword */
    "NO_WRITE_TO_BINLOG",
    "NULL",
    "NUMERIC",
    "ON",
    "OPTIMIZE",
    "OPTION",
    "OPTIONALLY",
    "OR",
    "ORDER",
    "OUT",
    "OUTER",
    "OUTFILE",
    "PAGER", /* Ocelot keyword */
    "PARTITION",
    "PRECISION",
    "PRIMARY",
    "PRINT", /* Ocelot keyword */
    "PROCEDURE",
    "PROMPT", /* Ocelot keyword */
    "PURGE",
    "QUIT", /* Ocelot keyword */
    "RANGE",
    "READ",
    "READS",
    "READ_WRITE",
    "REAL",
    "REFERENCES",
    "REGEXP",
    "REHASH", /* Ocelot keyword */
    "RELEASE",
    "RENAME",
    "REPEAT",
    "REPLACE",
    "REQUIRE",
    "RESIGNAL",
    "RESTRICT",
    "RETURN",
    "REVOKE",
    "RIGHT",
    "RLIKE",
    "ROW",
    "SCHEMA",
    "SCHEMAS",
    "SECOND_MICROSECOND",
    "SELECT",
    "SENSITIVE",
    "SEPARATOR",
    "SET",
    "SHOW",
    "SIGNAL",
    "SMALLINT",
    "SOURCE", /* Ocelot keyword */
    "SPATIAL",
    "SPECIFIC",
    "SQL",
    "SQLEXCEPTION",
    "SQLSTATE",
    "SQLWARNING",
    "SQL_BIG_RESULT",
    "SQL_CALC_FOUND_ROWS",
    "SQL_SMALL_RESULT",
    "SSL",
    "STARTING",
    "STATUS", /* Ocelot keyword */
    "STRAIGHT_JOIN",
    "SYSTEM", /* Ocelot keyword */
    "TABLE",
    "TEE", /* Ocelot keyword */
    "TERMINATED",
    "THEN",
    "TINYBLOB",
    "TINYINT",
    "TINYTEXT",
    "TO",
    "TRAILING",
    "TRIGGER",
    "TRUE",
    "UNDO",
    "UNION",
    "UNIQUE",
    "UNLOCK",
    "UNSIGNED",
    "UPDATE",
    "USAGE",
    "USE", /* Ocelot keyword, also reserved word */
    "USING",
    "UTC_DATE",
    "UTC_TIME",
    "UTC_TIMESTAMP",
    "VALUES",
    "VARBINARY",
    "VARCHAR",
    "VARCHARACTER",
    "VARYING",
    "WARNINGS", /* Ocelot keyword */
    "WHEN",
    "WHERE",
    "WHILE",
    "WITH",
    "WRITE",
    "XOR",
    "YEAR_MONTH",
    "ZEROFILL"
  };
  QString text;
  QString s= "";
  int t;
  char *p_item;
  unsigned long index;
  char key2[30 + 1];
  int i, i2;

  text= statement_edit_widget->toPlainText();
  for (i2= 0; main_token_lengths[i2] != 0; ++i2)
  {
    /* Get the next word. */
    s= text.mid(main_token_offsets[i2], main_token_lengths[i2]);
    t= token_type(s.data(), main_token_lengths[i2]);
    main_token_types[i2]= t;
    if (t == TOKEN_TYPE_OTHER)
    {
      /* It's not a literal or operator. Maybe it's a keyword. Convert it to char[]. */
      QByteArray key_as_byte_array= s.toLocal8Bit();
      const char *key= key_as_byte_array.data();
      /* Uppercase it. I don't necessarily have strupr(). */
      for (i= 0; (*(key + i) != '\0') && (i < 30); ++i) key2[i]= toupper(*(key + i)); key2[i]= '\0';
      /* Search it with library binary-search. Assume 259 items and everything 30 bytes long. */
      p_item= (char*) bsearch (key2, strvalues, 259, 30, (int(*)(const void*, const void*)) strcmp);
      if (p_item != NULL)
      {
        /* It's in the list, so instead of TOKEN_TYPE_OTHER, make it TOKEN_KEYWORD_something. */
        index= ((((unsigned long)p_item - (unsigned long)strvalues)) / 30) + TOKEN_KEYWORDS_START;
         main_token_types[i2]= index;
      }
    }
  }

  main_token_count= i2; /* Global. There will be several times when we want to know how many tokens there are. */

  main_token_types[i2]= 0;

  /*
    The special cases of BEGIN, DO, END, FUNCTION, ROW. We don't want to mark them as special unless
    we know they're really serving a non-identifier role in the statment, but they might be,
    because they're not reserved. This has to be right because later we might count BEGINs
    minus ENDs in order to figure out whether a compound statement has ended.
    FUNCTION is a keyword if: previous == CREATE or DROP
    ROW is a keyword if: previous == EACH
    DO is a keyword if: last statement-beginner keyword was WHILE
                        and next is a statement-beginner keyword or a label
                        (but actually this just checks whether we're within WHILE -- Todo: must fix that someday)
    END is a keyword if: previous == ; or BEGIN
    BEGIN is a keyword if: previous ==  ; or : or BEGIN or DO or ELSE or LOOP or ROW or THEN
                           or previous = ) and word before matching ( is PROCEDURE or FUNCTION or TRIGGER
                           and next is not :
  */
  bool is_in_while= 0;
  for (int i22= 0; main_token_types[i22] != 0; ++i22)
  {
    if (main_token_types[i22] == TOKEN_KEYWORD_FUNCTION)
    {
      if ((i22 > 0) && ((main_token_types[i22 - 1] == TOKEN_KEYWORD_CREATE) || (main_token_types[i22 - 1] == TOKEN_KEYWORD_DROP))) ;
      else main_token_types[i22]= TOKEN_TYPE_OTHER;
    }
    if (main_token_types[i22] == TOKEN_KEYWORD_ROW)
    {
      if ((i22 > 0) && (main_token_types[i22 - 1] == TOKEN_KEYWORD_EACH)) ;
      else main_token_types[i22]= TOKEN_TYPE_OTHER;
    }
    if (main_token_types[i22] == TOKEN_KEYWORD_WHILE) is_in_while= 1;
    if (main_token_types[i22] == TOKEN_KEYWORD_DO)
    {
      if (is_in_while == 1) is_in_while= 0;
      else main_token_types[i22]= TOKEN_TYPE_OTHER;
    }
    if (main_token_types[i22] == TOKEN_KEYWORD_END)
    {
      if ((i22 == 0)
         || (QString::compare(text.mid(main_token_offsets[i22 - 1], main_token_lengths[i22 - 1]), ";", Qt::CaseInsensitive) == 0)
         || (main_token_types[i22 - 1] == TOKEN_KEYWORD_BEGIN)) ;
      else main_token_types[i22]= TOKEN_TYPE_OTHER;
    }
    if (main_token_types[i22] == TOKEN_KEYWORD_BEGIN)
    {
      if ((i22 > 0) &&
          ((main_token_types[i22 - 1] == TOKEN_KEYWORD_BEGIN)
          || (main_token_types[i22 - 1] == TOKEN_KEYWORD_DO)
          || (main_token_types[i22 - 1] == TOKEN_KEYWORD_ELSE)
          || (main_token_types[i22 - 1] == TOKEN_KEYWORD_LOOP)
          || (main_token_types[i22 - 1] == TOKEN_KEYWORD_ROW)
          || (main_token_types[i22 - 1] == TOKEN_KEYWORD_THEN)
          || (QString::compare(text.mid(main_token_offsets[i22 - 1], main_token_lengths[i22 - 1]), ";", Qt::CaseInsensitive) == 0)
          || (QString::compare(text.mid(main_token_offsets[i22 - 1], main_token_lengths[i22 - 1]), ":", Qt::CaseInsensitive) == 0)
          || (QString::compare(text.mid(main_token_offsets[i22 - 1], main_token_lengths[i22 - 1]), ")", Qt::CaseInsensitive) == 0))) ;
      else main_token_types[i22]= TOKEN_TYPE_OTHER;
    }
  }

  /*
    The special case of "?". Although tokenize() says ? is an operator,
    we might have to treat it as a keyword meaning "help".
  */
  if (main_token_lengths[0] == 1)
  {
    s= text.mid(main_token_offsets[0], 1);
    if (s == QString("?")) main_token_types[0]= TOKEN_KEYWORD_QUESTIONMARK;
  }

  /* Todo: This has to be moved somewhere because the text might contain multi-statements. */
  if ((main_token_types[0] == TOKEN_KEYWORD_QUESTIONMARK)
  ||  (main_token_types[0] == TOKEN_KEYWORD_CHARSET)
  ||  (main_token_types[0] == TOKEN_KEYWORD_CLEAR)
  ||  (main_token_types[0] == TOKEN_KEYWORD_CONNECT)
  ||  (main_token_types[0] == TOKEN_KEYWORD_DELIMITER)
  ||  (main_token_types[0] == TOKEN_KEYWORD_EDIT)
  ||  (main_token_types[0] == TOKEN_KEYWORD_EGO)
  ||  (main_token_types[0] == TOKEN_KEYWORD_GO)
  ||  (main_token_types[0] == TOKEN_KEYWORD_HELP)
  ||  (main_token_types[0] == TOKEN_KEYWORD_NOPAGER)
  ||  (main_token_types[0] == TOKEN_KEYWORD_NOTEE)
  ||  (main_token_types[0] == TOKEN_KEYWORD_NOWARNING)
  ||  (main_token_types[0] == TOKEN_KEYWORD_PAGER)
  ||  (main_token_types[0] == TOKEN_KEYWORD_PRINT)
  ||  (main_token_types[0] == TOKEN_KEYWORD_PROMPT)
  ||  (main_token_types[0] == TOKEN_KEYWORD_QUIT)
  ||  (main_token_types[0] == TOKEN_KEYWORD_REHASH)
  ||  (main_token_types[0] == TOKEN_KEYWORD_SOURCE)
  ||  (main_token_types[0] == TOKEN_KEYWORD_STATUS)
  ||  (main_token_types[0] == TOKEN_KEYWORD_SYSTEM)
  ||  (main_token_types[0] == TOKEN_KEYWORD_TEE)
  ||  (main_token_types[0] == TOKEN_KEYWORD_USE)
  ||  (main_token_types[0] == TOKEN_KEYWORD_WARNINGS))
  {
    main_statement_type= main_token_types[0];
  }
  else
  {
    main_statement_type= 0;
  }

  /*
    The special cases of "\q" etc.:
    If token[0] is "\" and token[q] == "q" (lower case only), that's equivalent to "quit".
    They're still separate tokens -- changing tokenize() to say "\=" is not a keyword is not
    appropriate because "\" really should be an operator. But treat both tokens as the
    same keyword, TOKEN_KEYWORD_QUIT.
    This is for mysql compatibility.
    I couldn't find a consistent rule, but it seems that the statement pair could be at the
    start, could be at the end, or could be at the end before a delimiter.
    The list is in order by TOKEN_KEYWORD_... value.
  */
  int xx= -1;
  if (i2  >= 2)
  {
    /* Todo: what about the delimiter? */
    if ((main_token_offsets[1] == 1) && (main_token_lengths[1] == 1)) xx= 0;
    if ((main_token_lengths[main_token_count - 2] == 1) && (main_token_lengths[main_token_count - 1] == 1)) xx= main_token_count - 2;
    }
  if (i2 >= 3)
  {
    s= text.mid(main_token_offsets[i2 - 1], main_token_lengths[i2 - 1]);
    /* Todo: compare with delimiter, which isn't always semicolon. */
    if (s == (QString)ocelot_delimiter_str)
    {
      if ((main_token_lengths[main_token_count - 3] == 1) && (main_token_lengths[main_token_count - 2] == 1)) xx= main_token_count - 3;
    }
  }
  if (xx >= 0)
  {
    s= text.mid(main_token_offsets[xx], 2);
    if (s == QString("\\?")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_QUESTIONMARK;
    if (s == QString("\\C")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_CHARSET;
    if (s == QString("\\c")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_CLEAR;
    if (s == QString("\\r")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_CONNECT;
    if (s == QString("\\d")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_DELIMITER;
    if (s == QString("\\e")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_EDIT;
    if (s == QString("\\G")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_EGO;
    if (s == QString("\\g")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_GO;
    if (s == QString("\\h")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_HELP;
    if (s == QString("\\n")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_NOPAGER;
    if (s == QString("\\t")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_NOTEE;
    if (s == QString("\\w")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_NOWARNING;
    if (s == QString("\\P")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_PAGER;
    if (s == QString("\\p")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_PRINT;
    if (s == QString("\\R")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_PROMPT;
    if (s == QString("\\q")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_QUIT;
    if (s == QString("\\#")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_REHASH;
    if (s == QString("\\.")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_SOURCE;
    if (s == QString("\\s")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_STATUS;
    if (s == QString("\\!")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_SYSTEM;
    if (s == QString("\\T")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_TEE;
    if (s == QString("\\u")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_USE;
    if (s == QString("\\W")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_WARNINGS;
  }
}


/*
  Todo: disconnect old if already connected.
*/
int MainWindow::connect_mysql()
{
  mysql_init(&mysql); /* Todo: avoid repetition, this is init'd elsewhere. */

  if (the_connect())
  {
    put_diagnostics_in_result();
    statement_edit_widget->result.append(tr("Failed to connect. Use menu item File|Connect to try again"));
    return 1;
  }
  statement_edit_widget->result= tr("OK");

  /*
    Collect some variables in case they're needed for "prompt".
    Todo: check for errors after these mysql_ calls.
    Not using the mysql_res global, since this is not for user to see.
  */
  mysql_query(&mysql, "select version(), database(), @@port, current_user()");
  MYSQL_RES *mysql_res_for_connect;
  MYSQL_ROW connect_row;
  QString s;
  int i;

  // unsigned long connect_lengths[1];
  mysql_res_for_connect= mysql_store_result(&mysql);
  connect_row= mysql_fetch_row(mysql_res_for_connect);
  /* lengths= mysql_fetch_lengths(mysql_res_for_connect); */
  statement_edit_widget->dbms_version= connect_row[0];
  statement_edit_widget->dbms_database= connect_row[1];
  statement_edit_widget->dbms_port= connect_row[2];
  s= connect_row[3];
  statement_edit_widget->dbms_current_user= s;
  i= s.indexOf(QRegExp("@"), 0);
  if (i > 0) s= s.left(i);
  statement_edit_widget->dbms_current_user_without_host= s;

  s= mysql_get_host_info(&mysql);
  i= s.indexOf(QRegExp(" "), 0);
  if (i > 0) s= s.left(i);
  statement_edit_widget->dbms_host= s;
  mysql_free_result(mysql_res_for_connect);
  is_mysql_connected= 1;
  return 0;
}


/*
  For copyright and license notice of CodeEditor function, see beginning of this progrma.

  CodeEditor is a widget that inherits QPlainTextEdit.
     Therefore it inherits from QAbstractScrollArea, which has viewports.
  prompt_widget is a separate widget within CodeEditor.
      Paint the prompt on prompt_widget.
      Place prompt_widget over CodeEditor's viewport's left margin area.
      Doubtless you'll want to handle mouse events on prompt_widget, for breakpoints.
  Need to adjust when number of lines changes, when viewport is scrolled, or editor's size changes.

  In MainWindow:: we refer to components with "statement_edit_widget->...".
  In CodeEditor:: we refer to components with "this->...". They're the same things.

  Re Prompts ...
    Mainly we follow what mysql client does with prompt, as described in
    http://dev.mysql.com/doc/refman/5.6/en/mysql-commands.html.
    But the following are additional:
    \N                  (todo)                             seems to be "name of server" e.g. mysql or MariaDB
    \2                  Indicates the rest of the prompt gets repeated on all statement lines. Essential if \L is used.
    \L                  Statement line number
    For lines 2ff, the prompt becomes > unless there was something after \L, and is right-justified.
    Bug: for "prompt \w", the \w will be taken as a "nowarnings" keyword, because it's the last thing on the line.
    Todo: There are several prompts that require asking the server. The probable way to handle them is:
          At connect time:
          mysql_get_host_info() to get the host
          select current_user(), version(), database(), @@port to get other server stuff
*/

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
  prompt_widget= new prompt_class(this);

  connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(update_prompt_width(int)));
  connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(update_prompt(QRect,int)));
  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
  /*
    It is necessary to calculate prompt area width and highlight the first line
    when the editor is first created.
  */
  /* I decided "update_prompt_width(0);" here would be unnecessary since I redo prompt width after making a default etc. */
  highlightCurrentLine();
}


/* Todo: translate, i.e. convert \\ to \, convert \linenumber to (binary) 0x00, etc. */
/* On lines 2ff, the prompt is merely ">" unless there was a preceding "\2" */
QString CodeEditor::prompt_translate(int line_number_x)
{
  QString s, s_char, s_out;
  int is_2_seen= 0;
  time_t basetime;
  struct tm * timeinfo;
  char formatted_time[80];

  /*
    Todo: Getting date and time is slow.
    You should find out in advance (when user changes the prompt) whether you really will need it.
    Or you should calculate it only when you see \D etc., but that might cause apparent inconsistencies.
    Or you could check "Has the time already been calculated?"
  */
  time(&basetime);
  timeinfo= localtime (&basetime);

  s= this->prompt_as_input_by_user;
  s_out= (QString)"";
  for (int i= 0; i < s.size(); ++i)
  {
    s_char= s.mid(i, 1);
    if ((s_char == "\\") && (i < s.size() - 1))
    {
      ++i;
      s_char= s.mid(i, 1);
      if (s_char == "2")        /* \2 is to indicate that following items come on lines 2ff, not just line 1. Ocelot-only. */
      {
        s_char= (QString)""; is_2_seen= 1;
      }
      if (s_char == "c")        /* \c is for statement counter. We keep it in statement_edit_widget, not main window -- maybe that's bad. */
      {
        s_char= QString::number(statement_count);
      }
      if (s_char == "D")        /* \D is for full current date, e.g. "Fri May 23 14:26:18 2014" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%a %b %e %H:%M:%S %Y", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "d")        /* \d is for database. Discovered at connect time. Changed if user says "use". */
      {
        s_char= dbms_database;
      }
      if (s_char == "h")        /* \h is for host. */
      {
        s_char= dbms_host;
      }
      if (s_char == "L")        /* \L is for line number. Ocelot-only. Most effective if \2 seen. */
      {
        s_char= QString::number(line_number_x);
      }
      if (s_char == "l")        /* \l is for delimiter. */
      {
        s_char= delimiter;
      }
      if (s_char == "m")        /* \m is for minutes of the current time, e.g. "18" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%M", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "n")        /* \n is for newline. It goes in the stream but there's no apparent effect. */
      {
        s_char= "\n";
      }
      if (s_char == "O")        /* \O is for current month in 3-letter format, e.g. "Feb" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%b", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "o")        /* \o is for current month in numeric format, e.g. "12" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%m", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "P")        /* \P is for am/pm, e.g. "PM" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%p", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "p") {        /* \p is for port */
        s_char= dbms_port;
      }
      if (s_char == "R")        /* \R is for current hour on a 24-hour clock, e.g. "19" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%H", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "r")        /* \r is for current hour on a 12-hour clock, e.g. "11" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%r", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "S")        /* \S is for semicolon i.e. ";" */
      {
        s_char= ";";
      }
      if (s_char == "s")        /* \s is for seconds of the current time, e.g. "58" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%S", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "t")        /* \t is for tab. Appearance can be inconsistent. */
      {
        s_char= "\t";
      }
      if (s_char == "U")        /* \U is for user@host. */
      {
        s_char= dbms_current_user;
      }
      if (s_char == "u")        /* \u is for user. */
      {
        s_char= dbms_current_user_without_host;
      }
      if (s_char == "v")        /* \v is for server version. Discovered at connect time. */
      {
        s_char= dbms_version;
      }
      if (s_char == "w")        /* \w is for current day of the week in three-letter format, e.g. "Mon" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%a", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "Y")        /* \Y is for current year in four-digit format, e.g. "2014" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%Y", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "y")
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%y", timeinfo);
        s_char= formatted_time;
      }
    }
    if ((line_number_x < 2) || (is_2_seen == 1))
    {
      s_out.append(s_char);
    }
  }
  if ((line_number_x > 1) && (is_2_seen == 0)) s_out.append((QString)">");
  return (s_out);
}


/*
  Set left margin of viewport to the size we need to draw the prompt.
   This, presumably, is what prompt_width_calculate returns.
   Take the number of digits in the last line, multiply that by maximum width of a digit.
   We don't know in advance whether which is wider, the first line or the last line.
   (It's calculable in advance, but I didn't bother.)
   So we calculate both, and take the wider one, even if line 1 might not be visible.
   Todo: It's possible that width("11") is less than width("2"), so counting digits would be sensible.
*/
int CodeEditor::prompt_width_calculate()
{
  int line_width_first, line_width_last;
  QString sq;
  /* The new style. Ignoring all the above, give us width. */
  /* Todo: following should be a real interpretation. */
  /* this->prompt_current= this->prompt_translate(blockCount() + 1); */
  int prompt_width= 3;

  sq= prompt_translate(1);
  line_width_first= fontMetrics().width(sq);
  sq= prompt_translate(blockCount() + 1);
  line_width_last= fontMetrics().width(sq);
  if (line_width_first < line_width_last) prompt_width= line_width_last;
  else prompt_width= line_width_first;
  return prompt_width;

  /* Todo: check why this doesn't work -- prompt_width+= fontMetrics().width(this->prompt_current); */
  /* QString ss= prompt_translate(xxx); */
  QString ss;
  for (int i= 0; ss[i] != 0; ++i) prompt_width+= fontMetrics().width(ss[i]);
  return prompt_width;
}


/*
  We get here because of:
  "connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(update_prompt_width(int)));"
*/
void CodeEditor::update_prompt_width(int /* newBlockCount */)
{
  setViewportMargins(prompt_width_calculate(), 0, 0, 0);
}


/*
  We get here because of:
  "connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(update_prompt(QRect,int)));"
  When we update the width of the line number area, we simply call QAbstractScrollArea::setViewportMargins().
*/
void CodeEditor::update_prompt(const QRect &rect, int dy)
{
  if (dy)
    prompt_widget->scroll(0, dy);
  else
    prompt_widget->update(0, rect.y(), prompt_widget->width(), rect.height());
  if (rect.contains(viewport()->rect()))
    update_prompt_width(0);
}


/*
  This slot is invoked when the editor's viewport has been scrolled.
  The QRect given as argument is the part of the editing area that is to be updated (redrawn).
  dy holds the number of pixels the view has been scrolled vertically.
  When the size of the editor changes, we also need to resize the line number area.
*/
void CodeEditor::resizeEvent(QResizeEvent *e)
{
  QPlainTextEdit::resizeEvent(e);

  QRect cr= contentsRect();
  prompt_widget->setGeometry(QRect(cr.left(), cr.top(), prompt_width_calculate(), cr.height()));
}


/*
  We get to highlightCurrentline because of earlier
  "connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));"
  Todo: the hard coding Qt::yellow should instead be something chosen for action_statement().
*/
void CodeEditor::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly())
  {
    QTextEdit::ExtraSelection selection;

    QColor lineColor= QColor(Qt::yellow).lighter(160);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor= textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }
  setExtraSelections(extraSelections);
}


/* The prompt_widget_paintevent() is called from prompt_widget whenever it receives a paint event. */
void CodeEditor::prompt_widget_paintevent(QPaintEvent *event)
{
  QColor bgcolor;
  QColor textcolor;
  QString prompt_text;

  QPainter painter(prompt_widget);
  bgcolor= this->statement_edit_widget_left_bgcolor;
  painter.fillRect(event->rect(), bgcolor);

  QTextBlock block= firstVisibleBlock();
  int blockNumber= block.blockNumber();

  /* At tone time there was an overflow check here: "if (blockNumber > 1000) ... exit(1);". I couldn't remember why. */

  /* We start off by painting the widget's background. */
  int top= (int) blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom= top + (int) blockBoundingRect(block).height();

  /*
    We will now loop through all visible lines and paint the prompt in the extra area for each line.
    Notice that in a plain text edit each line will consist of one QTextBlock;
    though, if line wrapping is enabled, a line may span several rows in the text edit's viewport.
  */

  /*
    We get the top and bottom y-coordinate of the first text block,
    and adjust these values by the height of the current text block in each iteration in the loop.
    Notice that we check if the block is visible in addition to check if it is in the areas viewport -
    a block can, for example, be hidden by a window placed over the text edit.
  */
  while (block.isValid() && top <= event->rect().bottom())
  {
    if (block.isVisible() && bottom >= event->rect().top())
    {
      textcolor= this->statement_edit_widget_left_treatment1_textcolor;
      prompt_text= prompt_translate(blockNumber + 1);
      QString s= prompt_text;
      /* QString number= QString::number(blockNumber + 1); */
      painter.setPen(textcolor);
      painter.drawText(0, top, prompt_widget->width(), fontMetrics().height(),
                       Qt::AlignRight, s);
    }
    block= block.next();
    top= bottom;
    bottom= top + (int) blockBoundingRect(block).height();
    ++blockNumber;
  }
}

/* End of CodeEditor. End of effect of licence of CodeEditor. */

/*
  TextEditFrame
  This is one of the components of result_grid
*/
TextEditFrame::TextEditFrame(QWidget *parent, ResultGrid *result_grid_widget, int column_number) :
    QFrame(parent)
{
  setMouseTracking(true);
  left_mouse_button_was_pressed= 0;
  ancestor_result_grid_widget= result_grid_widget;
  ancestor_column_number= column_number;
}


TextEditFrame::~TextEditFrame()
{
}


void TextEditFrame::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    left_mouse_button_was_pressed= 1;
  }
}


void TextEditFrame::mouseMoveEvent(QMouseEvent *event)
{
  if (!(event->buttons() & Qt::LeftButton))
  {
    /* If cursor is over frame, set it to look like a point-both-ways arrow */
    /* Actuallly "> border_size won't happen, if mouse is on frame it's draggable. */
    /* Actually LEFT is impossible; we set left margin width= 0 earlier */
    /* if (event->x() <= border_size) {
       widget_side= LEFT;
       setCursor(Qt::SizeHorCursor); }
    else */
    if (event->x() >= width() - border_size)
    {       /* perhaps this should be 1 rather than border_size? */
      widget_side= RIGHT;
      setCursor(Qt::SizeHorCursor);
    }
    else if (event->y() >= height() - border_size)
    {       /* perhaps this should be 1 rather than border_size? */
      widget_side= BOTTOM;
      setCursor(Qt::SizeVerCursor);
    }
  }
  if (left_mouse_button_was_pressed == 0) return;      /* if mouse left button was never pressed, or was released, do nothing */
    /* if (widget_side == LEFT) {
        if ((width() - event->x()) > minimum_width) {
          setFixedSize(width() - event->x(),height()); } } */
  if (widget_side == RIGHT)
  {
    if (event->x() > minimum_width)
    {
      /*  Now you must persuade ResultGrid to update all the rows. Beware of multiline rows and header row (row#0). */
      ancestor_result_grid_widget->grid_column_widths[ancestor_column_number]= event->x();
      int xheight;
      for (long unsigned int xrow= 0; xrow < ancestor_result_grid_widget->grid_actual_grid_height_in_rows; ++xrow)
      {
        TextEditFrame *f= ancestor_result_grid_widget->text_edit_frames[xrow * ancestor_result_grid_widget->result_column_count + ancestor_column_number];
        if (xrow > 0) xheight= ancestor_result_grid_widget->grid_column_heights[ancestor_column_number];
        if (xrow == 0) xheight= f->height();
        f->setFixedSize(event->x(), xheight);
      }
    }
  }
  if (widget_side == BOTTOM)
  {
    if (event->y() > minimum_height)
    {
      /*
        The following extra 'if' exists because Qt was displaying warnings like
        "QWidget::setMinimumSize: (/TextEditFrame) Negative sizes (-4,45) are not possible"
        when someone grabs the bottom and drags to the left
      */
      if (event->x() >= minimum_width)
      {
        /*  Now you must persuade ResultGrid to update all the rows. Beware of multiline rows and header row (row#0). */
        ancestor_result_grid_widget->grid_column_heights[ancestor_column_number]= event->y();
        int xheight;
        for (long unsigned int xrow= 0; xrow < ancestor_result_grid_widget->grid_actual_grid_height_in_rows; ++xrow)
        {
          TextEditFrame *f= ancestor_result_grid_widget->text_edit_frames[xrow * ancestor_result_grid_widget->result_column_count + ancestor_column_number];
          if (xrow > 0) xheight= ancestor_result_grid_widget->grid_column_heights[ancestor_column_number];
          if (xrow == 0) xheight= f->height();
          f->setFixedSize(event->x(), xheight);
        }
      }
    }
  }

}


void TextEditFrame::mouseReleaseEvent(QMouseEvent *event)
{
  if (!(event->buttons() != 0)) left_mouse_button_was_pressed= 0;
}

/*
  CONNECT
*/

/*
   MySQL options
   MySQL's client library has routines for a consistent way to see what
   options the user has put in a configuration file such as my.cnf, or added
   on the command line with phrases such as --port=x.
   Todo: meld that with whatever a user might say in a CONNECT command line
         or maybe even a dialog box.
   Assume mysql_init() has already happened. (Currently it's in this function, but it shouldn't be.)
   Qt gets to see argc+argv first, and Qt will process options that it recognizes
   such as -style, -session, -graphicssystem. See
   http://qt-project.org/doc/qt-4.8/qapplication.html#details.
   But when Qt is done, it should remove what it parsed.
   Todo: actually you should operate on a copy of argc + argv, rather than change the originals. QT docs say so.
         but that would contradict another todo, which is to blank the password if it's in an argv
   Most options are ignored but the ones which might be essential for connecting are not ignored.
   Example: if ~/.my.cnf has "port=3306" in [clients] group, and start happens with --host=127.0.0.1,
            then port=3306 and current_host=127.0.0.1 and that will get passed to the connect routine.
   Read: http://dev.mysql.com/doc/refman/5.1/en/connecting.html

   At one time there was an include of getopt.h and calls to my_long_options etc.
   That introduced some unwanted dependencies and so now we do all the option retrievals directly.
 */

/* See also http://dev.mysql.com/doc/refman/5.6/en/mysql-command-options.html */

static int connected= 0;
//extern  MYSQL mysql;

/*
  Connect -- get options

  Environment variables, then option files, then command-line arguments

  Except that you have to read the command-line arguments to know whether
  you should read the option files, see http://dev.mysql.com/doc/refman/5.7/en/option-file-options.html

  For environment variables:
  Follow http://dev.mysql.com/doc/refman/5.7/en/environment-variables.html
  * Watch: LD_RUN_PATH MYSQL_GROUP_SUFFIX MYSQL_HOST MYSQL_PS1 MYSQL_PWD MYSQL_TCP_PORT TZ USER

  For option files:
  Follow MySQL manual http://dev.mysql.com/doc/refman/5.7/en/option-files.html
  Don't follow MariaDB documentation https://mariadb.com/kb/en/mariadb/mariadb-documentation/getting-started/getting-installing-and-upgrading-mariadb/configuring-mariadb-with-mycnf/
  Todo: Find out whether MariaDB documentation is merely wrong, or represents a real incompatibility with MySQL.
  * Read these files:
  /etc/my.cnf
  /etc/mysql/my.cnf
  SYSCONFDIR/my.cnf                          ?? i.e. [installation-directory]/etc/my.cnf but this should be changeable
  $MYSQL_HOME/my.cnf
  file specified with --defaults-extra-file
  $HOME/.my.cnf             MySQL manual says ~/.my.cnf which isn't necessarily the same thing, but I don't believe that
  $HOME/.mylogin.cnf        MySQL manual says ~/.mylogin.cnf which isn't necessarily the same thing, but I don't believe that
  ... There is no use of DATADIR, which is deprecated, and (I think) would only relate to server anyway.
  Todo: On Unix, "ignore configuration files that are world-writable".
  * How to read ...
  for (;;)
    read line
    tokenize for the line
    If blank, or starts with '#', or starts with ';', ignore
    ignore everything following '#' (not necessarily at start of line)
    start only when you see [client] or [mysql] or [ocelot]
    stop when you see [something-else]
    expand escape sequences
    if you see opt_name = value:
      compare to important-opt-name but allow "unambiguous prefix" see http://dev.mysql.com/doc/refman/5.7/en/program-options.html
      if opt_name is important
        get value, strip '' or "", strip lead/trail
        assign: variable-name-corresponding-to-opt-name = value
    if you see !include /home/mydir/myopt.cnf or the like:
        open the file; subsequent read lines will be from there, but you have to return
        [ if you're already in an include, that's probably an error ]
    if you see !includedir /home/mydir or the like:
        each file in the directory might be an options file, ending in ".cnf"
  * Last specification takes precedence, so we're starting with
  (say) a variable named OPT which is blank, setting it to
  something when we see OPT = xxx, and setting it again when
  we see OPT = xxx again. Only PASSWORD is tricky, because it
  can be followed by blank to indicate password is necessary.

  For command-line:
  Follow http://dev.mysql.com/doc/refman/5.7/en/command-line-options.html
  * This is somewhat different from the tokenizing that's done for the
    command line, but it's still a matter of tokenizing
  * The trick is: you have to tokenize twice, before and after command-line stuff
  * Get rid of any arguments that you actually handle, Qt might want the rest
  * todo: recognize --print-defaults and --login-pathconf

  In the end, you have a bunch of variables with values.
  Todo: For each variable with a value, append to "CONNECT" client-statement,
  which eventually will be what you actually use (though password won't be repeated?).
  See connect_make_statement().

  Todo: mysql client handles bad format in my.cnf file thus:
        "error: Found option without preceding group in config file: /home/pgulutzan/.my.cnf at line: 1
         Fatal error in defaults handling. Program aborted"
        Maybe we should do the same?

  The above comments apply to Unix, Windows is a bit different.

  Todo: whenever you have a token1/token2/token3:
        find token1 in case it's one of the options we track
        set the value for that option
  Todo: if argv[i] is a password, you should wipe it out. Compare what mysql client does:
        "while (*argument) *argument++= 'x';"

  Todo: The environment variable LD_LIBRARY_PATH is also important, because it determines
        where libmysqlclient.so and the Qt libraries will be loaded from. Document that,
        and show it somewhere.
        The only complication I can think of is that, if I
        decide someday to support another DBMS, the MySQL
        stuff would still come in or have to be disabled.
*/

#include <dirent.h>

void connect_set_variable(QString token0, QString token2);
void connect_read_command_line(int argc, char *argv[]);
void connect_read_my_cnf(const char *file_name);
QString connect_stripper (QString value_to_strip);

#include <pwd.h>
#include <unistd.h>

void MainWindow::connect_mysql_options_2(int argc, char *argv[])
{
  //char *ld_run_path;
  //char *group_suffix;
  //char *mysql_host;
  //char *mysql_ps1;
  char *mysql_pwd;
  //char *tz_user;
  char *home;
  char tmp_my_cnf[1024];                         /* file name = $HOME/.my.cnf or $HOME/.mylogin.cnf or defaults_extra_file */

  /*
    Todo: check: are there any default values to be set before looking at environment variables?
          I don't see documentation to that effect, so I'm just setting them to "" or 0.
          Except that port initially = MYSQL_PORT (probably 3306), and ocelot_delimiter_str initially = ";"
          and http://dev.mysql.com/doc/refman/5.6/en/connecting.html mentions a few others:
          host = 'localhost' (which means protocol=SOCKET if mysql client, but we ignore that)
          user = Unix login name on Linux, although on Windows it would be 'ODBC'
          and there seem to be some getenv() calls in mysqlcc that I didn't take into account.
  */
  ocelot_no_defaults= 0;
  ocelot_defaults_file= "";
  ocelot_defaults_extra_file= "";
  ocelot_host= "localhost";
  ocelot_user= "";
  ocelot_database= "";
  ocelot_port= MYSQL_PORT;
  ocelot_comments= 0;
  ocelot_init_command= "";
  ocelot_default_auth= "";
  ocelot_protocol= "";
  ocelot_password_was_specified= 0;
  ocelot_unix_socket= "";
  ocelot_delimiter_str= ";";
  ocelot_history_includes_warnings= 0;
  ocelot_connect_timeout= 0;
  ocelot_compress= 0;
  ocelot_secure_auth= 0;
  ocelot_local_infile= 0;
  ocelot_safe_updates= 0;
  ocelot_plugin_dir= "";
  ocelot_select_limit= 0;
  ocelot_max_join_size= 0;
  ocelot_silent= 0;
  ocelot_no_beep= 0;
  ocelot_wait= 0;
  ocelot_default_character_set= "";

  {
    struct passwd *pw;
    uid_t u;
    u= geteuid();
    pw= getpwuid(u);
    if (pw != NULL) ocelot_user= pw->pw_name;
  }

  connect_read_command_line(argc, argv);               /* We're doing this twice, the first time won't count. */

  /* Environment variables */
  //ld_run_path= getenv("LD_RUN_PATH");                /* We're not doing anything with this yet. Maybe we should. */
  //group_suffix= getenv("GROUP_SUFFIX");              /* "" */
  //mysql_host= getenv("MYSQL_HOST");                  /* "" */
  //mysql_ps1= getenv("MYSQL_PS1");                    /* "" */
  if (getenv("MYSQL_PWD") != 0)
  {
    mysql_pwd= getenv("MYSQL_PWD");
    ocelot_password= mysql_pwd;
    ocelot_password_was_specified= 1;
  }
  if (getenv("MYSQL_TCP_PORT") != 0) ocelot_port= atoi(getenv("MYSQL_TCP_PORT"));         /* "" */
  //tz_user= getenv("TZ_USER");
  home= getenv("HOME");

  /*
    Options files i.e. Configuration files i.e. my_cnf files
    Don't read option files if ocelot_no_defaults==1 (which is true if --no-defaults was specified on command line).
    Todo: check: does MariaDB read mylogin.cnf even if ocelot_no_defaults==1?
  */
  if (QString::compare(ocelot_defaults_file, " ") > 0)
  {
    strcpy(tmp_my_cnf, ocelot_defaults_file.toUtf8());
    connect_read_my_cnf(tmp_my_cnf);
  }
  else
  {
    if (ocelot_no_defaults == 0)
    {
      connect_read_my_cnf("/etc/my.cnf");
      connect_read_my_cnf("/etc/mysql/my.cnf");
      /* todo: think: is argv[0] what you want for SYSCONFDIR? not exact, but it's where the program is now. no, it might be a copy. */
      // connect_read_my_cnf("SYSCONFDIR/etc/my.cnf") /* ?? i.e. [installation-directory]/etc/my.cnf but this should be changeable */
      /* skip $MYSQL_HOME/my.cnf, only server stuff should be in it */
      // connect_read_my_cnf("file specified with --defaults-extra-file");
      if (QString::compare(ocelot_defaults_extra_file, " ") > 0)
      {
        strcpy(tmp_my_cnf, ocelot_defaults_extra_file.toUtf8());
        connect_read_my_cnf(tmp_my_cnf);
      }
      strcpy(tmp_my_cnf, home);                              /* $HOME/.my.cnf */
      strcat(tmp_my_cnf, "/.my.cnf");
      connect_read_my_cnf(tmp_my_cnf);
    }
    strcpy(tmp_my_cnf, home);                          /* $HOME/.mylogin.cnf */
    strcat(tmp_my_cnf, "/.mylogin.cnf");
    connect_read_my_cnf(tmp_my_cnf);
  }
  connect_read_command_line(argc, argv);
  //connect_make_statement();
}


/*
  Command line arguments i.e. argc + argv
  Some tokenizing has already been done.
  For example progname --a=b --c=d gives us argv[1]="--a=b" and argv[2]="--c=d".
  For example progname -a b -c d gives us argv[1]="-a" argv[2]="b" argv[3]="-c" argv[4]="d".
  If something is enclosed in single or double quotes, then it has already been stripped of quotes and lead/trail spaces.
  todo: parse so -p x and --port=x etc. are known
  todo: check: is it okay still to abbreviate e.g. us rather than user?
  todo: strip the arguments so Qt doesn't see them, or maybe don't
  todo: do command line arguments TWICE! first time is just to find out what my.cnf files should be read, etc.
        second time is to override whatever happened during getenv and option-file processing
  todo: you seem to be forgetting that Qt can also expect command-line options
*/
void MainWindow::connect_read_command_line(int argc, char *argv[])
{
  QString token0, token1, token2;
  QString s_argv;
  int i;

  for (i= 1; i < argc; ++i)
  {
    s_argv= argv[i];
    /* If there's no '-' then (I guess) this is the database name */
    if (s_argv.mid(0, 1) != "-")
    {
      token0= "database"; token1= "="; token2= s_argv;
    }
    /* If there is one '-' then token1=argv[i], token2='=', token3=argv[i+1 */
    else if ((strlen(argv[i]) > 1) && (s_argv.mid(1, 1) != "-"))
    {
      token0= argv[i]; token1= ""; token2= "";
      if (token0 == "-b") token0= "no_beep";
      if (token0 == "-C") token0= "compress";
      if (token0 == "-c") token0= "comments";
      if (token0 == "-D") token0= "database";
      if (token0 == "-h") token0= "host";
      if (token0 == "-P") token0= "port";
      if (token0 == "-p") token0= "password";
      if (token0 == "-S") token0= "socket";
      if (token0 == "-s") token0= "silent";
      if (token0 == "-U") token0= "safe_updates";
      if (token0 == "-u") token0= "user";
      if (token0 == "-w") token0= "wait";
      if (i < argc - 1)
      {
        token1= "="; token2= argv[i + 1];
        ++i;
      }
    }
    /* If there are two '-'s then token1=argv[i] left, token2='=', token3=argv[i] right */
    else
    {
      QString equaller;
      int argv_length;
      argv_length= strlen(argv[i]);
      equaller= "=";
      int equaller_index;
      equaller_index= s_argv.indexOf(equaller);
      if (equaller_index == -1)
      {
        token0= s_argv.mid(2, argv_length - 2);
        token1= "";
        token2= "";
      }
      else
      {
        token0= s_argv.mid(2, equaller_index - 2);
        token1= "=";
        token2= s_argv.mid(equaller_index + 1, (argv_length - equaller_index) - 1);
      }
    }
    connect_set_variable(token0, token2);
  }
}


/* Todo: unfortunately MAX_TOKENS == 10000, when we only need 10, so there's temporary space waste
         maybe we should use main_token_offsets and main_token_lengths? */
/* todo: check if we've already looked at the file (this is possible if !include or !includedir happens)
         if so, skip */
void MainWindow::connect_read_my_cnf(const char *file_name)
{
  FILE *file;
  char line[2048];
  int token_offsets[MAX_TOKENS];
  int token_lengths[MAX_TOKENS];
  int i;
  QString token0, token1, token2, token_for_value;
  QString group;                                         /* what was in the last [...] e.g. mysqld, client, mysql, ocelot */
  int token0_length, token1_length, token2_length;

  group= "";                                             /* group identifier doesn't carry over from last .cnf file that we read */
  file= fopen(file_name, "r");                           /* Open specified file, read only */
  if (file == NULL)                                      /* (if file doesn't exist, ok, no error */
  {
    return;
  }
  while(fgets(line, sizeof line, file) != NULL)
  {
    QString s= line;
    /* tokenize, ignore # comments or / * comments * /, treat '-' as part of token not operator */
    tokenize(s.data(),
             s.size(),
             &token_lengths, &token_offsets, MAX_TOKENS,(QChar*)"33333", 2, "", 2);
    /* Ignore blank lines and lines that start with ';' */
    if (token_lengths[0] == 0) continue;
    if (QString::compare(s.mid(token_offsets[0], token_lengths[0]), ";", Qt::CaseInsensitive) == 0) continue;
    /* Possible meaningful lines are: [ group ], ! include x, ! includedir x, variable = value */
    token0_length= token_lengths[0];
    token0= s.mid(token_offsets[0], token0_length);
    token1_length= token_lengths[1];
    if (token1_length != 0) token1= s.mid(token_offsets[1], token1_length);
    else token1= "";
    if (token1_length == 0) token_lengths[2]= 0;
    token2_length= token_lengths[2];
    if (token2_length != 0)
    {
      int last_token;
      for (last_token= 3; token_lengths[last_token] != 0; ++last_token) ;
      token2_length= token_offsets[last_token - 1] + token_lengths[last_token - 1] - token_offsets[2];
      token2= s.mid(token_offsets[2], token2_length);
    }
    else token2= "";

    /* See if it's !include + file-name */
    /* todo: check for a sym link, or a directory. I don't know what we should do with such junk. */
    if ((QString::compare(token0, "!") == 0) && (QString::compare(token1, "include", Qt::CaseInsensitive) == 0))
    {
      char new_file_name[2048];
      strcpy(new_file_name,token2.toUtf8());
      *(new_file_name + token2_length)= 0;
      connect_read_my_cnf(new_file_name);
    }
    /* See if it's !includedir */
    if ((QString::compare(token0, "!") == 0) && (QString::compare(token1, "includedir", Qt::CaseInsensitive) == 0))
    {
      DIR *d;
      struct dirent *dir;
      char new_directory_name[2048];
      strcpy(new_directory_name,token2.toUtf8());
      *(new_directory_name + token2_length)= 0;
      d= opendir(new_directory_name);
      if (d)
      {
        while ((dir = readdir(d)) != NULL)
        {
          if ((strlen(dir->d_name)>4) && (strcmp(dir->d_name + strlen(dir->d_name) - 4, ".cnf") == 0))
          {
            char new_file_name[2048];
            strcpy(new_file_name, new_directory_name);
            strcat(new_file_name, "/");
            strcat(new_file_name, dir->d_name);
            connect_read_my_cnf(new_file_name);
          }
        }
        closedir(d);
      }
    }

    /* See if it's [ group ] */
    if ((QString::compare(token0, "[") == 0) && (QString::compare(token2,"]") == 0))
    {
      group= token1;
      continue;
    }
    /* Skip if it's not one of the groups that we care about i.e. client or mysql or ocelot */
    if ((QString::compare(group, "client", Qt::CaseInsensitive) != 0)
    &&  (QString::compare(group, "mysql", Qt::CaseInsensitive) != 0)
    &&  (QString::compare(group, "ocelot", Qt::CaseInsensitive) != 0)) continue;

    /* Remove ''s or ""s around the value, then strip lead or trail spaces. */
    token2= connect_stripper(token2);
    token2_length= token2.count();

    /* Convert escape sequences in the value \b backspace, \t tab, \n newline, \r carriage return, \\ \, \s space */
    token_for_value= "";
    for (i= 0; i < token2_length; ++i)
    {
      QString c, c2;
      c= token2.mid(i, 1);
      if ((c == "\\") && (i < token2_length))
      {
        c2= token2.mid(i + 1, 1);
        if (c2 == "b") { c2= 0x08; token_for_value = token_for_value + c2; ++i; continue; }
        if (c2 == "t"){ c2= 0x09; token_for_value = token_for_value + c2; ++i; continue; }
        if (c2 == "n"){ c2= 0x0a; token_for_value = token_for_value + c2; ++i; continue; }
        if (c2 == "r") { c2= 0x0d; token_for_value = token_for_value + c2; ++i; continue; }
        if (c2 == "\\") { c2= "\\"; token_for_value = token_for_value + c2; ++i; continue; }
        if (c2 == "s") { c2= 0x20; token_for_value = token_for_value + c2; ++i; continue; }
      }
      token_for_value= token_for_value + c;
    }
    token2= token_for_value;
    connect_set_variable(token0, token2);
  }
  fclose(file);
}


/*
  Remove ''s or ""s or ``s around a QString, then remove lead or trail spaces.
  Called for connect, and also for things like USE `test`.
  I didn't say remove lead or trail whitespace, so QString "trimmed()" is no good.
  todo: I'm not sure that `` (tildes) should be removed for my.cnf values, check that
  todo: I am fairly sure that I need to call this from other places too.
  todo: Does not look for 'xxx''yyy'. should it?
*/
QString MainWindow::connect_stripper (QString value_to_strip)
{
  QString s;
  int s_length;
  char c_singlequote[2];
  char c_doublequote[2];
  char c_grave_accent[2]; /* Grave Accent is the Unicode term for `. MySQL calls it backtick. */

  s= value_to_strip;
  s_length= s.count();
  if (s_length > 1)
  {
    c_singlequote[0]= 0x27; c_singlequote[1]= 0;
    c_doublequote[0]= 0x22; c_doublequote[1]= 0;
    c_grave_accent[0]= 0x60; c_grave_accent[1]= 0;
    if (((s.mid(0, 1) == QString(c_singlequote)) && (s.mid(s_length - 1, 1) == QString(c_singlequote)))
    ||  ((s.mid(0, 1) == QString(c_doublequote)) && (s.mid(s_length - 1, 1) == QString(c_doublequote)))
    ||  ((s.mid(0, 1) == QString(c_grave_accent)) && (s.mid(s_length - 1, 1) == QString(c_grave_accent))))
    {
      s= s.mid(1, s_length - 2);
      while ((s.count() > 0) && (s.mid(0, 1) == " "))
      {
        s_length= s.count();
        s= s.mid(1, s_length - 1);
      }
      while ((s.count() > 0) && (s.mid(s.count() - 1, 1) == " "))
      {
        s_length= s.count();
        s= s.mid(0, s_length - 1);
      }
    }
  }
  return s;
}


/*
  Given token1=option-name [token2=equal-sign token3=value],
  see if option-name corresponds to one of your program-variables,
  and if so set program-variable = true or program-variable = value.
  For example, if token1="user", token2="=", token3="peter",
  then set ocelot_user = "peter".
  But that would be too simple, eh? So here are some complications:
  * unambiguous prefixes of option naes are allowed until MySQL 5.7
    (this is true for options but not true for values)
    http://dev.mysql.com/doc/refman/5.6/en/program-options.html
  * '-' and '_' are interchangeable
    http://dev.mysql.com/doc/refman/5.6/en/command-line-options.html
  * todo: if and only if the target is numeric, then before converting
    the string to a number check whether it ends with K or M or G, and
    if it does then multiply by a kilo or a mega or a giga
  option our variable name mysql variable name
  ------ ----------------- -------------------
  ho[st] ocelot_host       current_host
  us[er] ocelot_user       current_user
  database ocelot_database current_db
  so[cket] ocelot_unix_socket                opt_mysql_unix_port
  po[rt]   ocelot_port     mysql opt_mysql_port
  comm[ents] ocelot_comments preserve_comments
  ocelot_init_command opt_init_command
  ocelot_default_auth opt_default_auth
  no-defaults ocelot_no_defaults
  defaults-extra-file ocelot_defaults_extra_file
  defaults-file ocelot_defaults_file
  delimiter ocelot_delimiter_str
  show-warnings ocelot_history_includes_warnings
  connect_timeout ocelot_connect_timeout
  compress ocelot_compress
  secure_auth ocelot_secure_auth
  local_infile ocelot_local_infile
  safe_updates or i_am_a_dummy ocelot_safe_updates
  plugin_dir ocelot_plugin_dir
  select_limit ocelot_select_limit
  max_join_size ocelot_max_join_size
  silent ocelot_silent
  no_beep ocelot_no_beep
  wait ocelot_wait
  default-character-set ocelot_default_character_set
*/
void MainWindow::connect_set_variable(QString token0, QString token2)
{
  unsigned int token0_length;
  char token0_as_utf8[64 + 1];

  token0_length= token0.count();
  if (token0_length >= 64) return; /* no option name is ever longer than 64 bytes */
  strcpy(token0_as_utf8,token0.toUtf8());
  *(token0_as_utf8 + token0_length)= 0;
  for (int i= 0; token0_as_utf8[i] != '\0'; ++i)
  {
    if (token0_as_utf8[i] == '-') token0_as_utf8[i]= '_';
  }
  if ((token0_length >= sizeof("ho")) && (strncmp(token0_as_utf8, "host", token0_length) == 0))
  {
    ocelot_host= token2;
    return;
  }
  if ((token0_length >= sizeof("us")) && (strncmp(token0_as_utf8, "user", token0_length) == 0))
  {
    ocelot_user= token2;
    return;
  }
  if ((token0_length >= sizeof("so")) && (strncmp(token0_as_utf8, "socket", token0_length) == 0))
  {
    ocelot_unix_socket= token2;
    return;
  }
  if ((token0_length >= sizeof("po")) && (strncmp(token0_as_utf8, "port", token0_length) == 0))
  {
    ocelot_port= token2.toInt();
    return;
  }
  if ((token0_length >= sizeof("comm")) && (strncmp(token0_as_utf8, "comments", token0_length) == 0))
  {
    ocelot_comments= 1;
    return;
  }
  if ((token0_length >= sizeof("default_a")) && (strncmp(token0_as_utf8, "default_auth", token0_length) == 0))
  {
    ocelot_default_auth= token2;
    return;
  }
  if ((token0_length >= sizeof("prot")) && (strncmp(token0_as_utf8, "protocol", token0_length) == 0))
  {
    ocelot_protocol= token2; /* Todo: perhaps make sure it's tcp/socket/pipe/memory */
    return;
  }
  if ((token0_length >= sizeof("pas")) && (strncmp(token0_as_utf8, "password", token0_length) == 0))
  {
    ocelot_password= token2;
    ocelot_password_was_specified= 1;
    return;
  }
  if ((token0_length >= sizeof("del")) && (strncmp(token0_as_utf8, "delimiter", token0_length) == 0))
  {
    ocelot_delimiter_str= token2;
    return;
  }
  if ((token0_length >= sizeof("sh")) && (strncmp(token0_as_utf8, "show_warnings", token0_length) == 0))
  {
    ocelot_history_includes_warnings= 1;
    return;
  }
  if ((token0_length >= sizeof("con")) && (strncmp(token0_as_utf8, "connect_timeout", token0_length) == 0))
  {
    ocelot_connect_timeout= token2.toLong();
    return;
  }
  if ((token0_length >= sizeof("comp")) && (strncmp(token0_as_utf8, "compress", token0_length) == 0))
  {
    ocelot_compress= 1;
    return;
  }
  if ((token0_length >= sizeof("sec")) && (strncmp(token0_as_utf8, "secure_auth", token0_length) == 0))
  {
    ocelot_secure_auth= 1;
    return;
  }
  if ((token0_length >= sizeof("lo")) && (strncmp(token0_as_utf8, "local_infile", token0_length) == 0))
  {
    ocelot_local_infile= 1;
    return;
  }
  if ((token0_length >= sizeof("sa")) && (strncmp(token0_as_utf8, "safe_updates", token0_length) == 0))
  {
    ocelot_safe_updates= 1;
    return;
  }
  if ((token0_length >= sizeof("i_")) && (strncmp(token0_as_utf8, "i_am_a_dummy", token0_length) == 0))
  {
    ocelot_safe_updates= 1;
    return;
  }
  if ((token0_length >= sizeof("pl")) && (strncmp(token0_as_utf8, "plugin_dir", token0_length) == 0))
  {
    ocelot_plugin_dir= token2;
    return;
  }
  if ((token0_length >= sizeof("sel")) && (strncmp(token0_as_utf8, "select_limit", token0_length) == 0))
  {
    ocelot_select_limit= token2.toLong();
    return;
  }
  if ((token0_length >= sizeof("sel")) && (strncmp(token0_as_utf8, "select_limit", token0_length) == 0))
  {
    ocelot_select_limit= token2.toLong();
    return;
  }
  if ((token0_length >= sizeof("max_j")) && (strncmp(token0_as_utf8, "max_join_size", token0_length) == 0))
  {
    ocelot_max_join_size= token2.toLong();
    return;
  }
  if ((token0_length >= sizeof("sil")) && (strncmp(token0_as_utf8, "sil", token0_length) == 0))
  {
    ocelot_silent= 1;
    return;
  }
  if ((token0_length >= sizeof("no_b")) && (strncmp(token0_as_utf8, "no_beep", token0_length) == 0))
  {
    ocelot_no_beep= 1;
    return;
  }
  if ((token0_length >= sizeof("w")) && (strncmp(token0_as_utf8, "wait", token0_length) == 0))
  {
    ocelot_wait= 1;
    return;
  }
  if (strcmp(token0_as_utf8, "database") == 0)
  {
    ocelot_database= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "init_command") == 0)
  {
    ocelot_init_command= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "no_defaults") == 0)
  {
    ocelot_no_defaults= 1;
    return;
  }
  if (strcmp(token0_as_utf8, "defaults_extra_file") == 0)
  {
    ocelot_defaults_extra_file= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "defaults_file") == 0)
  {
    ocelot_defaults_file= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "default_character_set") == 0)
  {
    ocelot_default_character_set= token2;
    return;
  }
}


/*
  Given the options that have actually been set to non-default values,
  form a CONNECT statement.
  todo: something should call this for the sake of history, at least.
  todo: instead o x > "", use if (QString::compare(x, " ") > 0)
*/
void MainWindow::connect_make_statement()
{
  QMessageBox msgBox;
  QString statement_text;

  statement_text= "CONNECT";
  if (ocelot_defaults_file > "") statement_text= statement_text + " defaults_file=" + ocelot_defaults_file;
  if (ocelot_no_defaults > 0) statement_text= statement_text + " no_defaults";
  if (ocelot_defaults_extra_file > "") statement_text= statement_text + " defaults_extra_file=" + ocelot_defaults_extra_file;
  if (ocelot_host > "") statement_text= statement_text + " host=" + ocelot_host;
  if (ocelot_user > "") statement_text= statement_text + " user=" + ocelot_user;
  if (ocelot_database > "") statement_text= statement_text + " database=" + ocelot_database;
  if (ocelot_port > 0) statement_text= statement_text + " port=" + QString::number(ocelot_port);
  if (ocelot_comments > 0) statement_text= statement_text + " comments";
  if (ocelot_init_command > "") statement_text= statement_text + " init_command=" + ocelot_init_command;
  if (ocelot_default_auth > "") statement_text= statement_text + " default_auth=" + ocelot_default_auth;
  if (ocelot_protocol > "") statement_text= statement_text + " protocol=" + ocelot_protocol;
  if (ocelot_password_was_specified > 0) statement_text= statement_text + " password=" + ocelot_password;
  if (ocelot_unix_socket > "") statement_text= statement_text + " socket=" + ocelot_unix_socket;
  if (ocelot_delimiter_str > "") statement_text= statement_text + " delimiter=" + ocelot_delimiter_str;
  if (ocelot_history_includes_warnings > 0) statement_text= statement_text + " show_warnings";
  if (ocelot_connect_timeout > 0) statement_text= statement_text + " connect_timeout=" + QString::number(ocelot_connect_timeout);
  if (ocelot_compress > 0) statement_text= statement_text + " compress";
  if (ocelot_secure_auth > 0) statement_text= statement_text + " secure_auth";
  if (ocelot_local_infile > 0) statement_text= statement_text + " local_infile";
  if (ocelot_safe_updates > 0) statement_text= statement_text + " safe_updates";
  if (ocelot_plugin_dir > "") statement_text= statement_text + " plugin_dir=" + ocelot_plugin_dir;
  if (ocelot_select_limit > 0) statement_text= statement_text + " select_limit=" + QString::number(ocelot_select_limit);
  if (ocelot_max_join_size > 0) statement_text= statement_text + " max_join_size=" + QString::number(ocelot_max_join_size);
  if (ocelot_silent > 0) statement_text= statement_text + " silent";
  if (ocelot_no_beep > 0) statement_text= statement_text + "no_beep";
  if (ocelot_wait > 0) statement_text= statement_text + "wait";
  if (ocelot_default_character_set > "") statement_text= statement_text + "default_character_set=" + ocelot_default_character_set;
  msgBox.setText(statement_text);
  msgBox.exec();
}


int MainWindow::options_and_connect(char *host, char *database, char *user, char *password,
    char *tmp_init_command,
    char *plugin_dir,
    char *default_auth,
    char *unix_socket)
{
  if (connected != 0)
  {
    connected= 0;
    mysql_close(&mysql);
  }
  mysql_init(&mysql);

  if (tmp_init_command[0] != '\0') mysql_options(&mysql, MYSQL_INIT_COMMAND, tmp_init_command);

  if (ocelot_compress != 0) mysql_options(&mysql, MYSQL_OPT_COMPRESS, NULL);

  if (ocelot_connect_timeout != 0)
  {
    unsigned int timeout= ocelot_connect_timeout;
    mysql_options(&mysql, MYSQL_OPT_CONNECT_TIMEOUT, (char*) &timeout);
  }

  if (ocelot_secure_auth > 0) mysql_options(&mysql, MYSQL_SECURE_AUTH, (char *) &ocelot_secure_auth);
  if (ocelot_local_infile > 0) mysql_options(&mysql, MYSQL_OPT_LOCAL_INFILE, (char*) &ocelot_local_infile);

  if (QString::compare(ocelot_protocol, " ") > 0)
  {
    #define PROTOCOL_TCP 1
    #define PROTOCOL_SOCKET 2
    #define PROTOCOL_PIPE 3
    #define PROTOCOL_MEMORY 4
    unsigned int tmp_protocol;
    tmp_protocol= 0;
    if (QString::compare(ocelot_protocol, "TCP", Qt::CaseInsensitive) == 0) tmp_protocol= PROTOCOL_TCP;
    if (QString::compare(ocelot_protocol, "SOCKET", Qt::CaseInsensitive) == 0) tmp_protocol= PROTOCOL_SOCKET;
    if (QString::compare(ocelot_protocol, "PIPE", Qt::CaseInsensitive) == 0) tmp_protocol= PROTOCOL_PIPE;
    if (QString::compare(ocelot_protocol, "MEMORY", Qt::CaseInsensitive) == 0) tmp_protocol= PROTOCOL_MEMORY;
    if (tmp_protocol > 0) mysql_options(&mysql, MYSQL_OPT_PROTOCOL, (char*)&tmp_protocol);
  }

  if (ocelot_safe_updates != 0)
  {
    char init_command[100];
    sprintf(init_command,
        "SET sql_select_limit = %lu, sql_safe_updates = 1, max_join_size = %lu",
        ocelot_select_limit, ocelot_max_join_size);
    mysql_options(&mysql, MYSQL_INIT_COMMAND, init_command);
  }

/*
  MYSQL_PLUGIN_DIR and MYSQL_DEFAULT_AUTH are options we should support.
  But there can be problems building on some distros, because they're
  relatively recent additions in mysql.h. Todo: support them real soon.
*/
//  mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, default_charset);

//  if (*plugin_dir != '\0') mysql_options(&mysql, MYSQL_PLUGIN_DIR, plugin_dir);

//  if (*default_auth != '\0') mysql_options(&mysql, MYSQL_DEFAULT_AUTH, default_auth);

  /* CLIENT_MULTI_RESULTS but not CLIENT_MULTI_STATEMENTS */
  if (mysql_real_connect(&mysql, host, user, password,
                          database, ocelot_port, unix_socket,
                          CLIENT_MULTI_RESULTS) == 0)
  {
    /* connect failed. todo: better diagnostics? anyway, user can retry, a dialog box will come up. */
    return -1;					// Retryable
  }
  connected= 1;
  return 0;
}


int MainWindow::the_connect()
{
  int x;

  /* See comment "UTF8 Conversion" */
  int tmp_host_len= ocelot_host.toUtf8().size();
  char *tmp_host= new char[tmp_host_len + 1];
  memcpy(tmp_host, ocelot_host.toUtf8().constData(), tmp_host_len + 1);
  int tmp_database_len= ocelot_database.toUtf8().size();
  char *tmp_database= new char[tmp_database_len + 1];
  memcpy(tmp_database, ocelot_database.toUtf8().constData(), tmp_database_len + 1);
  int tmp_user_len= ocelot_user.toUtf8().size();
  char *tmp_user= new char[tmp_user_len + 1];
  memcpy(tmp_user, ocelot_user.toUtf8().constData(), tmp_user_len + 1);
  int tmp_password_len= ocelot_password.toUtf8().size();
  char *tmp_password= new char[tmp_password_len + 1];
  memcpy(tmp_password, ocelot_password.toUtf8().constData(), tmp_password_len + 1);
  int tmp_init_command_len= ocelot_init_command.toUtf8().size();
  char *tmp_init_command= new char[tmp_init_command_len + 1];
  memcpy(tmp_init_command, ocelot_init_command.toUtf8().constData(), tmp_init_command_len + 1);

  int tmp_plugin_dir_len= ocelot_plugin_dir.toUtf8().size();
  char *tmp_plugin_dir= new char[tmp_plugin_dir_len + 1];
  memcpy(tmp_plugin_dir, ocelot_plugin_dir.toUtf8().constData(), tmp_plugin_dir_len + 1);

  int tmp_default_auth_len= ocelot_default_auth.toUtf8().size();
  char *tmp_default_auth= new char[tmp_default_auth_len + 1];
  memcpy(tmp_default_auth, ocelot_default_auth.toUtf8().constData(), tmp_default_auth_len + 1);
  int tmp_unix_socket_len= ocelot_unix_socket.toUtf8().size();
  char *tmp_unix_socket= new char[tmp_unix_socket_len + 1];
  memcpy(tmp_unix_socket, ocelot_unix_socket.toUtf8().constData(), tmp_unix_socket_len + 1);

  x= options_and_connect(tmp_host, tmp_database, tmp_user, tmp_password,
                      tmp_init_command, tmp_plugin_dir, tmp_default_auth, tmp_unix_socket);

  delete []tmp_unix_socket;
  delete []tmp_default_auth;
  delete []tmp_plugin_dir;
  delete []tmp_init_command;
  delete []tmp_password;
  delete []tmp_user;
  delete []tmp_database;
  delete []tmp_host;
  return x;
}

