/*
   The class named CodeEditor inside the ocelotgui program is taken and modified from
   http://doc.qt.io/qt-4.8/qt-widgets-codeeditor-example.html
   and therefore CodeEditor's maker's copyright and BSD license provisions
   are reproduced below. These provisions apply only for the
   part of the CodeEditor class which is included by #include "codeeditor.h".
   The program as a whole is copyrighted by Peter Gulutzan and
   licensed under GPL version 2 as stated at the beginning of ocelotgui.cpp
   and in the COPYING file.
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
  For copyright and license notice of CodeEditor function, see beginning of this program.

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
    \K(left-prompt)     See comment before prompt_translate_k()
    For lines 2ff, the prompt becomes > unless there was something after \L, and is right-justified.
    Bug: for "prompt \w", the \w will be taken as a "nowarnings" keyword, because it's the last thing on the line.
    Todo: There are several prompts that require asking the server. The probable way to handle them is:
          At connect time:
          lmysql->ldbms_mysql_get_host_info() to get the host
          select current_user(), version(), database(), @@port to get other server stuff
    Todo: For prompt 指事> I don't like the height.
*/

#ifndef CODEEDITOR_H_H

/* Following should be multi-occurrence and should be part of CodeEditor */
/* Todo: Sizes should be dynamic! You'll only see stuff for the first 10 lines with what you've got here! */
#define K_SIZE 10
int k_line_number[K_SIZE];
QColor k_color[K_SIZE];
QString k_string[K_SIZE];

CodeEditor::CodeEditor(MainWindow *parent) : QPlainTextEdit(parent)
{
  prompt_widget= new prompt_class(this);
  main_window= parent;

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
/*
  Todo: Getting date and time is slow.
  You should find out in advance (when user changes the prompt) whether you really will need it.
  Or you should calculate it only when you see \D etc., but that might cause apparent inconsistencies.
  Or you could check "Has the time already been calculated?"
  Todo: \C is nonsense if there is no connection
  Todo: \c includes client statements in the count, but mysql client doesn't seem to do that
  Todo: \d is blank if there is no current database, but mysql client says "(none)"
*/
QString CodeEditor::prompt_translate(int line_number_x)
{
  QString s, s_char, s_out;
  int is_2_seen= 0;
  int k_index;

  for (k_index= 0; k_index < K_SIZE; ++k_index) k_line_number[k_index]= -1;
  QDateTime qdt= QDateTime::currentDateTime();
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
      else if (s_char == "C")        /* \C is for connection_id, mysql 5.7.6ff. */
      {
        s_char= QString::number(dbms_connection_id);
      }
      else if (s_char == "c")        /* \c is for statement counter. We keep it in statement_edit_widget, not main window -- maybe that's bad. */
      {
        s_char= QString::number(statement_count);
      }
      else if (s_char == "D")        /* \D is for full current date, e.g. "Fri May 23 14:26:18 2014" */
      {
        s_char= qdt.toString("ddd").left(3) + " " + qdt.toString("MMM").left(3) + qdt.toString(" dd hh:mm:ss yyyy");
      }
      else if (s_char == "d")        /* \d is for database. Discovered at connect time. Changed if user says "use". */
      {
        s_char= dbms_database;
      }
      else if (s_char == "h")        /* \h is for host. */
      {
        s_char= dbms_host;
      }
      else if (s_char == "K")
      {
        int k_result= prompt_translate_k(s, i);
        if (k_result != 0)
        {
          i+= k_result;
          continue;
        }
      }
      else if (s_char == "L")        /* \L is for line number. Ocelot-only. Most effective if \2 seen. */
      {
        s_char= QString::number(line_number_x);
      }
      else if (s_char == "l")        /* \l is for delimiter. */
      {
        s_char= delimiter;
      }
      else if (s_char == "m")        /* \m is for minutes of the current time, e.g. "18" */
      {
        s_char= qdt.toString("mm");
      }
      else if (s_char == "n")        /* \n is for newline. It goes in the stream but there's no apparent effect. */
                                /* todo: try harder. use markup instead of \n? crlf? */
      {
        s_char= "\n";
      }
      else if (s_char == "O")        /* \O is for current month in 3-letter format, e.g. "Feb" */
      {
        s_char= qdt.toString("MMM").left(3);
      }
      else if (s_char == "o")        /* \o is for current month in numeric format, e.g. "12" */
      {
        s_char= qdt.toString("MM");
      }
      else if (s_char == "P")        /* \P is for am/pm, e.g. "PM" */
      {
        s_char= qdt.toString("AP");
        if (s_char == "P.M.") s_char= "PM"; else s_char= "AM";
      }
      else if (s_char == "p") {        /* \p is for port */
        s_char= dbms_port;
      }
      else if (s_char == "R")        /* \R is for current hour on a 24-hour clock, e.g. "19" */
      {
        s_char= qdt.toString("hh");
      }
      else if (s_char == "r")        /* \r is for current hour on a 12-hour clock, e.g. "11" */
      {
        s_char= qdt.toString("hh AP").left(2);
      }
      else if (s_char == "S")        /* \S is for semicolon i.e. ";" */
      {
        s_char= ";";
      }
      else if (s_char == "s")        /* \s is for seconds of the current time, e.g. "58" */
      {
        s_char= qdt.toString("ss");
      }
      else if (s_char == "t")        /* \t is for tab. Appearance can be inconsistent. */
      {
        s_char= "\t";
      }
      else if (s_char == "U")        /* \U is for user@host. */
      {
        s_char= dbms_current_user;
      }
      else if (s_char == "u")        /* \u is for user. */
      {
        s_char= dbms_current_user_without_host;
      }
      else if (s_char == "v")        /* \v is for server version. Discovered at connect time. */
      {
        s_char= dbms_version;
      }
      else if (s_char == "w")        /* \w is for current day of the week in three-letter format, e.g. "Mon" */
      {
        s_char= qdt.toString("ddd").left(3);
      }
      else if (s_char == "Y")        /* \Y is for current year in four-digit format, e.g. "2014" */
      {
        s_char= qdt.toString("yyyy");
      }
      else if (s_char == "y")        /* \y is for current year in 2-digit format, e.g. "14" */
      {
        s_char= qdt.toString("yy");
      }
      else if (s_char == "_")        /* \_ is space. Odd, since "\ " is space and more obvious. */
      {
        s_char= " ";
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
  prompt_translate_k() is called from prompt_translate() when \K is seen.
  The prompt string may contain "\K(line-number,color-name,string)" -- no spaces after K.
  which will put out a string, at the specified line, with the specified color.
  The string will be left-justified so it won't interfere with the rest of the prompt.
  If you want a space between the left and right justification, put ' ' at end of string
  The string cannot contain ) or ' or ", there are no escapes.
  A line number may not be specified twice.
  Erroneous input is ignored.
  Example: '\K(5,red,B)' means: on line 5, left justified, put 'B'.
  Width will increase if and only if the line number exists.
  This is intended for use with debugger breakpoints, but could be used for statements.
  Todo: find out why this is called a large number of times
  Todo: really, you should be using the usual parser, not this quickie hack job
*/
int CodeEditor::prompt_translate_k(QString s, int i)
{
  QString s_char;
  QString string_for_line_number;
  QString string_for_color;
  QString string_for_string;
  int i_left_parenthesis, i_first_comma, i_second_comma, i_right_parenthesis;
  int i2;

  i_left_parenthesis= i_first_comma= i_second_comma= i_right_parenthesis= -1;

  for (i2= i; i2 < s.size(); ++i2)
  {
    s_char= s.mid(i2, 1);
    if ((s_char == "(") && (i_left_parenthesis == -1)) i_left_parenthesis= i2;
    if ((s_char == ",") && (i_first_comma == -1)) i_first_comma= i2;
    else if ((s_char == ",") && (i_first_comma != -1) && (i_second_comma == -1)) i_second_comma= i2;
    if ((s_char == ")") && (i_second_comma != -1) && (i_right_parenthesis == -1)) i_right_parenthesis= i2;
  }

  if (i_left_parenthesis != i + 1) return 0; /* bad left parenthesis */
  if (i_first_comma == -1) return 0; /* bad first comma */
  if (i_second_comma == -1) return 0; /* bad second comma */
  if (i_right_parenthesis == -1) return 0; /* bad right parenthesis */
  string_for_line_number= s.mid(i_left_parenthesis + 1, (i_first_comma - i_left_parenthesis) - 1);
  string_for_color= s.mid(i_first_comma + 1, (i_second_comma - i_first_comma) - 1);
  string_for_string= s.mid(i_second_comma + 1, (i_right_parenthesis - i_second_comma) - 1);

  bool ok;
  int line_number;
  line_number= string_for_line_number.toInt(&ok, 10);
  if ((ok == false) || (line_number <= 0)) return 0; /* bad line number */

  QColor color;
  color.setNamedColor(string_for_color);
  if (color.isValid() == false) return 0; /* bad color */

  /* Find an unused point in k_line_number[], set it to line number. If all points are full, too bad. (Will fix soon!) */
  for (int k_index= 0; k_index < K_SIZE; ++k_index)
  {
    if (k_line_number[k_index] == -1)
    {
      k_line_number[k_index]= line_number;
      k_color[k_index]= color;
      k_string[k_index]= string_for_string;
      break;
    }
  }

  return (i_right_parenthesis - i) + 1;
}


/*
  We'll be setting left margin of viewport to the size we need to draw the prompt.
  Take the width of the first line or the width of the last line, whichever is greater.
  Add the width of "B " if this CodeEditor is for a debug widget rather than for the statement widget.
  Re the isPunct() check: I found that if the last letter is . or [ etc. the width is too small, dunno why.
*/
int CodeEditor::prompt_width_calculate()
{
  int line_width_first, line_width_last;
  QString sq;
  int prompt_width;

  sq= prompt_translate(1);
  if ((sq.size() > 1) && (sq.at(sq.size() - 1).isPunct() == true))
    sq= sq.left(sq.size() - 1) + "W";
  line_width_first= fontMetrics().boundingRect(sq).width();
  sq= prompt_translate(blockCount() + 1);
  line_width_last= fontMetrics().boundingRect(sq).width();
  if (line_width_first < line_width_last) prompt_width= line_width_last;
  else prompt_width= line_width_first;
#if (OCELOT_MYSQL_DEBUGGER == 1)
  if (is_debug_widget == true) prompt_width+= fontMetrics().boundingRect("B ").width();
#endif
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
  I made one change from the original code:
    Originally there was a } brace after "extraSelections.append(selection);".
    I moved it so there is a } brace after "setExtraSelections(extraSelections);".
    I believe this should only affect debug_widget[], which is read-only except when debug_highlight_line() happens.
*/
void CodeEditor::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly())
  {
    QTextEdit::ExtraSelection selection;

    //QColor lineColor= QColor(Qt::yellow).lighter(160);
    QColor lineColor= QColor(main_window->ocelot_statement_highlight_current_line_color).lighter(160);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor= textCursor();
#if (OCELOT_MYSQL_DEBUGGER == 1)
    block_number= selection.cursor.blockNumber();                           /* effectively = line number base 0 */
#endif
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
#if (OCELOT_MYSQL_DEBUGGER == 1)
      /* hmm, actually this loop could stop when k_line_number[k_index] == -1 too */
      for (int k_index= 0; k_index < K_SIZE; ++k_index)
      {
        if (blockNumber == k_line_number[k_index] - 1)
        {
          painter.setPen(QColor(k_color[k_index]));
          painter.drawText(0, top, 10, fontMetrics().height(), Qt::AlignLeft, k_string[k_index]);
          break;
        }
      }
#endif
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

#endif

/* End of CodeEditor. End of effect of licence of CodeEditor. */
