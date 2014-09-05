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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/* All Qt includes go here. Most of them could be handled by just saying "#include <QtWidgets>". */
#include <QMainWindow>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTextEdit>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QColorDialog>
#include <QFontDialog>
#include <QPlainTextEdit>
#include <QWidget>
#include <QMessageBox>
#include <QPainter>
#include <QTextBlock>
#include <QDateTime>
//#include <QDockWidget>
#include <QFrame>
//#include <QLibraryInfo>

/* All mysql includes go here */
/* The include path for mysql.h is hard coded in ocelotgui.pro. */
#include <mysql.h>

namespace Ui
{
class MainWindow;
}

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QMenuBar;
class QComboBox;
class QPushButton;
class QTextEdit;
class QPlainTextEdit;
class QScrollArea;
class QVBoxLayout;
class CodeEditor;
class ResultGrid;
class Settings;
class TextEditFrame;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(int argc, char *argv[], QWidget *parent= 0);
  ~MainWindow();

  /* Client variables that can be changed with the Settings widget */
  QString ocelot_statement_color;
  QString ocelot_statement_background_color;
  QString ocelot_statement_border_color;
  QString ocelot_statement_font_family;
  QString ocelot_statement_font_size;
  QString ocelot_statement_font_style;
  QString ocelot_statement_font_weight;
  QString ocelot_statement_highlight_literal_color;
  QString ocelot_statement_highlight_identifier_color;
  QString ocelot_statement_highlight_comment_color;
  QString ocelot_statement_highlight_operator_color;
  QString ocelot_statement_highlight_reserved_color;
  QString ocelot_statement_prompt_background_color;
  QString ocelot_statement_style_string;
  QString ocelot_grid_color;
  QString ocelot_grid_background_color;
  QString ocelot_grid_border_color;
  QString ocelot_grid_header_background_color;
  QString ocelot_grid_font_family;
  QString ocelot_grid_font_size;
  QString ocelot_grid_font_style;
  QString ocelot_grid_font_weight;
  QString ocelot_grid_cell_border_color;
  QString ocelot_grid_cell_right_drag_line_color;
  QString ocelot_grid_border_size;
  QString ocelot_grid_cell_border_size;
  QString ocelot_grid_cell_right_drag_line_size;
  QString ocelot_grid_style_string;
  QString ocelot_grid_header_style_string;
  QString ocelot_history_color;
  QString ocelot_history_background_color;
  QString ocelot_history_border_color;
  QString ocelot_history_font_family;
  QString ocelot_history_font_size;
  QString ocelot_history_font_style;
  QString ocelot_history_font_weight;
  QString ocelot_history_style_string;
  QString ocelot_main_color;
  QString ocelot_main_background_color;
  QString ocelot_main_border_color;
  QString ocelot_main_font_family;
  QString ocelot_main_font_size;
  QString ocelot_main_font_style;
  QString ocelot_main_font_weight;
  QString ocelot_main_style_string;

  QString ocelot_host;                       /* for CONNECT */
  unsigned short ocelot_port;                /* for CONNECT */
  QString ocelot_user;                       /* for CONNECT */
  QString ocelot_unix_socket;                /* for CONNECT */
  QString ocelot_password;                   /* for CONNECT */
  QString ocelot_protocol;                   /* for CONNECT */
  QString ocelot_init_command;               /* for CONNECT */
  QString ocelot_default_auth;               /* for CONNECT */
  unsigned short ocelot_comments;            /* for CONNECT. not used. */
  unsigned short ocelot_no_defaults;         /* for CONNECT */
  QString ocelot_defaults_file;              /* for CONNECT */
  QString ocelot_defaults_extra_file;        /* for CONNECT */
  unsigned long int ocelot_connect_timeout;  /* for CONNECT */
  unsigned short ocelot_compress;            /* for CONNECT */
  unsigned short ocelot_secure_auth;         /* for CONNECT */
  unsigned short ocelot_local_infile;        /* for CONNECT */
  unsigned short ocelot_safe_updates;        /* for CONNECT */
  QString ocelot_plugin_dir;                 /* for CONNECT */
  unsigned long int ocelot_select_limit;     /* for CONNECT */
  unsigned long int ocelot_max_join_size;    /* for CONNECT */
  unsigned short int ocelot_silent;          /* for CONNECT */
  unsigned short int ocelot_no_beep;         /* for CONNECT */
  unsigned short int ocelot_wait;            /* for CONNECT */

  QString *row_form_label;               /* for row_form */
  int *row_form_type;                    /* for row_form */
  int *row_form_is_password;             /* for row_form */
  QString *row_form_data;                /* for row_form */
  QString *row_form_width;               /* for row_form */
  QString row_form_title;                /* for row_form */
  QString row_form_message;              /* for row_form */

  QString history_markup_statement_start;    /* for markup */
  QString history_markup_statement_end;      /* for markup */
  QString history_markup_prompt_start;       /* for markup */
  QString history_markup_prompt_end;         /* for markup */
  QString history_markup_result;             /* for markup */
  QString history_markup_entity;             /* for markup */

public slots:
  void action_connect();
  void action_connect_once(QString);
  void action_exit();
  void action_execute();
  void action_about();
  void action_the_manual();
  void action_the_manual_close();
  void action_statement_edit_widget_text_changed();
  void action_undo();
  void action_statement();
  void action_grid();
  void action_history();
  void action_main();
  void history_markup_previous();
  void history_markup_next();

protected:
  bool eventFilter(QObject *obj, QEvent *ev);

private:
  Ui::MainWindow *ui;

  int history_markup_previous_or_next();
  void create_widget_history();
  void create_widget_statement();
  void create_menu();
  void widget_sizer();
  int execute_client_statement();
  void put_diagnostics_in_result();
  int options_and_connect(char *host, char *database, char *user, char *password, char *tmp_init_command,
                       char *tmp_plugin_dir, char *tmp_default_auth, char *unix_socket);

  void connect_mysql_options_2(int w_argc, char *argv[]);
  void connect_read_command_line(int argc, char *argv[]);
  void connect_read_my_cnf(const char *file_name);
  QString connect_stripper (QString value_to_strip);
  void connect_set_variable(QString token0, QString token2);
  void connect_make_statement();

  void copy_options_to_main_window();
  int the_connect();
  int the_connect_2(); /* intended replacement for the_connect() */
  //my_bool get_one_option(int optid, const struct my_option *opt __attribute__((unused)),char *argument);
  void connect_init();
  void set_current_colors_and_font();
  void make_style_strings();
  void create_the_manual_widget();
  int get_next_statement_in_string();
  void action_execute_one_statement();

  void history_markup_make_strings();
  void history_markup_append();
  QString history_markup_copy_for_history(QString);

  enum {MAX_TOKENS= 10000 };                  /* Todo: shouldn't be fixed */

  enum {                                      /* possible returns from token_type() */
    TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE= 1, /* starts with ' or N' or X' or B' */
    TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE= 2, /* starts with " */
    TOKEN_TYPE_LITERAL_WITH_DIGIT= 3,        /* starts with 0-9 */
    TOKEN_TYPE_LITERAL_WITH_BRACE= 4,        /* starts with { */
    TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK= 5,  /* starts with ` */
    TOKEN_TYPE_IDENTIFIER_WITH_AT= 6,        /* starts with @ */
    TOKEN_TYPE_COMMENT_WITH_SLASH = 7,        /* starts with / * or * / */
    TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE= 8,   /* starts with # */
    TOKEN_TYPE_COMMENT_WITH_MINUS= 9,        /* starts with -- */
    TOKEN_TYPE_OPERATOR= 10,                 /* starts with < > = ! etc. */
    TOKEN_TYPE_OTHER= 11,                    /* identifier? keyword? */
    /* The TOKEN_KEYWORD_... numbers must match the list in tokens_to_keywords(). */
    TOKEN_KEYWORDS_START= TOKEN_TYPE_OTHER + 1,
    TOKEN_KEYWORD_QUESTIONMARK= TOKEN_KEYWORDS_START, /* Ocelot keyword */
    TOKEN_KEYWORD_ACCESSIBLE= TOKEN_KEYWORD_QUESTIONMARK + 1,
    TOKEN_KEYWORD_ADD= TOKEN_KEYWORD_ACCESSIBLE + 1,
    TOKEN_KEYWORD_ALL= TOKEN_KEYWORD_ADD + 1,
    TOKEN_KEYWORD_ALTER= TOKEN_KEYWORD_ALL + 1,
    TOKEN_KEYWORD_ANALYZE= TOKEN_KEYWORD_ALTER + 1,
    TOKEN_KEYWORD_AND= TOKEN_KEYWORD_ANALYZE + 1,
    TOKEN_KEYWORD_AS= TOKEN_KEYWORD_AND + 1,
    TOKEN_KEYWORD_ASC= TOKEN_KEYWORD_AS + 1,
    TOKEN_KEYWORD_ASENSITIVE= TOKEN_KEYWORD_ASC + 1,
    TOKEN_KEYWORD_BEFORE= TOKEN_KEYWORD_ASENSITIVE + 1,
    TOKEN_KEYWORD_BEGIN= TOKEN_KEYWORD_BEFORE + 1,
    TOKEN_KEYWORD_BETWEEN= TOKEN_KEYWORD_BEGIN + 1,
    TOKEN_KEYWORD_BIGINT= TOKEN_KEYWORD_BETWEEN + 1,
    TOKEN_KEYWORD_BINARY= TOKEN_KEYWORD_BIGINT + 1,
    TOKEN_KEYWORD_BLOB= TOKEN_KEYWORD_BINARY + 1,
    TOKEN_KEYWORD_BOTH= TOKEN_KEYWORD_BLOB + 1,
    TOKEN_KEYWORD_BY= TOKEN_KEYWORD_BOTH + 1,
    TOKEN_KEYWORD_CALL= TOKEN_KEYWORD_BY + 1,
    TOKEN_KEYWORD_CASCADE= TOKEN_KEYWORD_CALL + 1,
    TOKEN_KEYWORD_CASE= TOKEN_KEYWORD_CASCADE + 1,
    TOKEN_KEYWORD_CHANGE= TOKEN_KEYWORD_CASE + 1,
    TOKEN_KEYWORD_CHAR= TOKEN_KEYWORD_CHANGE + 1,
    TOKEN_KEYWORD_CHARACTER= TOKEN_KEYWORD_CHAR + 1,
    TOKEN_KEYWORD_CHARSET= TOKEN_KEYWORD_CHARACTER + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_CHECK= TOKEN_KEYWORD_CHARSET + 1,
    TOKEN_KEYWORD_CLEAR= TOKEN_KEYWORD_CHECK + 1,              /* Ocelot keyword */
    TOKEN_KEYWORD_COLLATE= TOKEN_KEYWORD_CLEAR + 1,
    TOKEN_KEYWORD_COLUMN= TOKEN_KEYWORD_COLLATE + 1,
    TOKEN_KEYWORD_CONDITION= TOKEN_KEYWORD_COLUMN + 1,
    TOKEN_KEYWORD_CONNECT= TOKEN_KEYWORD_CONDITION + 1,        /* Ocelot keyword */
    TOKEN_KEYWORD_CONSTRAINT= TOKEN_KEYWORD_CONNECT + 1,
    TOKEN_KEYWORD_CONTINUE= TOKEN_KEYWORD_CONSTRAINT + 1,
    TOKEN_KEYWORD_CONVERT= TOKEN_KEYWORD_CONTINUE + 1,
    TOKEN_KEYWORD_CREATE= TOKEN_KEYWORD_CONVERT + 1,
    TOKEN_KEYWORD_CROSS= TOKEN_KEYWORD_CREATE + 1,
    TOKEN_KEYWORD_CURRENT_DATE= TOKEN_KEYWORD_CROSS + 1,
    TOKEN_KEYWORD_CURRENT_TIME= TOKEN_KEYWORD_CURRENT_DATE + 1,
    TOKEN_KEYWORD_CURRENT_TIMESTAMP= TOKEN_KEYWORD_CURRENT_TIME + 1,
    TOKEN_KEYWORD_CURRENT_USER= TOKEN_KEYWORD_CURRENT_TIMESTAMP + 1,
    TOKEN_KEYWORD_CURSOR= TOKEN_KEYWORD_CURRENT_USER + 1,
    TOKEN_KEYWORD_DATABASE= TOKEN_KEYWORD_CURSOR + 1,
    TOKEN_KEYWORD_DATABASES= TOKEN_KEYWORD_DATABASE + 1,
    TOKEN_KEYWORD_DAY_HOUR= TOKEN_KEYWORD_DATABASES + 1,
    TOKEN_KEYWORD_DAY_MICROSECOND= TOKEN_KEYWORD_DAY_HOUR + 1,
    TOKEN_KEYWORD_DAY_MINUTE= TOKEN_KEYWORD_DAY_MICROSECOND + 1,
    TOKEN_KEYWORD_DAY_SECOND= TOKEN_KEYWORD_DAY_MINUTE + 1,
    TOKEN_KEYWORD_DEC= TOKEN_KEYWORD_DAY_SECOND + 1,
    TOKEN_KEYWORD_DECIMAL= TOKEN_KEYWORD_DEC + 1,
    TOKEN_KEYWORD_DECLARE= TOKEN_KEYWORD_DECIMAL + 1,
    TOKEN_KEYWORD_DEFAULT= TOKEN_KEYWORD_DECLARE + 1,
    TOKEN_KEYWORD_DELAYED= TOKEN_KEYWORD_DEFAULT + 1,
    TOKEN_KEYWORD_DELETE= TOKEN_KEYWORD_DELAYED + 1,
    TOKEN_KEYWORD_DELIMITER= TOKEN_KEYWORD_DELETE + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_DESC= TOKEN_KEYWORD_DELIMITER + 1,
    TOKEN_KEYWORD_DESCRIBE= TOKEN_KEYWORD_DESC + 1,
    TOKEN_KEYWORD_DETERMINISTIC= TOKEN_KEYWORD_DESCRIBE + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_DISTINCT= TOKEN_KEYWORD_DETERMINISTIC + 1,
    TOKEN_KEYWORD_DISTINCTROW= TOKEN_KEYWORD_DISTINCT + 1,
    TOKEN_KEYWORD_DIV= TOKEN_KEYWORD_DISTINCTROW + 1,
    TOKEN_KEYWORD_DO= TOKEN_KEYWORD_DIV + 1,
    TOKEN_KEYWORD_DOUBLE= TOKEN_KEYWORD_DO + 1,
    TOKEN_KEYWORD_DROP= TOKEN_KEYWORD_DOUBLE + 1,
    TOKEN_KEYWORD_DUAL= TOKEN_KEYWORD_DROP + 1,
    TOKEN_KEYWORD_EACH= TOKEN_KEYWORD_DUAL + 1,
    TOKEN_KEYWORD_EDIT= TOKEN_KEYWORD_EACH + 1,
    TOKEN_KEYWORD_EGO= TOKEN_KEYWORD_EDIT + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_ELSE= TOKEN_KEYWORD_EGO + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_ELSEIF= TOKEN_KEYWORD_ELSE + 1,
    TOKEN_KEYWORD_ENCLOSED= TOKEN_KEYWORD_ELSEIF + 1,
    TOKEN_KEYWORD_END= TOKEN_KEYWORD_ENCLOSED + 1,
    TOKEN_KEYWORD_ESCAPED= TOKEN_KEYWORD_END + 1,
    TOKEN_KEYWORD_EXISTS= TOKEN_KEYWORD_ESCAPED + 1,
    TOKEN_KEYWORD_EXIT= TOKEN_KEYWORD_EXISTS + 1,              /* Ocelot keyword, also regular keyword */
    TOKEN_KEYWORD_EXPLAIN= TOKEN_KEYWORD_EXIT + 1,
    TOKEN_KEYWORD_FALSE= TOKEN_KEYWORD_EXPLAIN + 1,
    TOKEN_KEYWORD_FETCH= TOKEN_KEYWORD_FALSE + 1,
    TOKEN_KEYWORD_FLOAT= TOKEN_KEYWORD_FETCH + 1,
    TOKEN_KEYWORD_FLOAT4= TOKEN_KEYWORD_FLOAT + 1,
    TOKEN_KEYWORD_FLOAT8= TOKEN_KEYWORD_FLOAT4 + 1,
    TOKEN_KEYWORD_FOR= TOKEN_KEYWORD_FLOAT8 + 1,
    TOKEN_KEYWORD_FORCE= TOKEN_KEYWORD_FOR + 1,
    TOKEN_KEYWORD_FOREIGN= TOKEN_KEYWORD_FORCE + 1,
    TOKEN_KEYWORD_FROM= TOKEN_KEYWORD_FOREIGN + 1,
    TOKEN_KEYWORD_FULLTEXT= TOKEN_KEYWORD_FROM + 1,
    TOKEN_KEYWORD_FUNCTION= TOKEN_KEYWORD_FULLTEXT + 1,
    TOKEN_KEYWORD_GET= TOKEN_KEYWORD_FUNCTION + 1,
    TOKEN_KEYWORD_GO= TOKEN_KEYWORD_GET + 1,
    TOKEN_KEYWORD_GRANT= TOKEN_KEYWORD_GO + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_GROUP= TOKEN_KEYWORD_GRANT + 1,
    TOKEN_KEYWORD_HAVING= TOKEN_KEYWORD_GROUP + 1,
    TOKEN_KEYWORD_HELP= TOKEN_KEYWORD_HAVING + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_HIGH_PRIORITY= TOKEN_KEYWORD_HELP + 1,
    TOKEN_KEYWORD_HOUR_MICROSECOND= TOKEN_KEYWORD_HIGH_PRIORITY + 1,
    TOKEN_KEYWORD_HOUR_MINUTE= TOKEN_KEYWORD_HOUR_MICROSECOND + 1,
    TOKEN_KEYWORD_HOUR_SECOND= TOKEN_KEYWORD_HOUR_MINUTE + 1,
    TOKEN_KEYWORD_IF= TOKEN_KEYWORD_HOUR_SECOND + 1,
    TOKEN_KEYWORD_IGNORE= TOKEN_KEYWORD_IF + 1,
    TOKEN_KEYWORD_IN= TOKEN_KEYWORD_IGNORE + 1,
    TOKEN_KEYWORD_INDEX= TOKEN_KEYWORD_IN + 1,
    TOKEN_KEYWORD_INFILE= TOKEN_KEYWORD_INDEX + 1,
    TOKEN_KEYWORD_INNER= TOKEN_KEYWORD_INFILE + 1,
    TOKEN_KEYWORD_INOUT= TOKEN_KEYWORD_INNER + 1,
    TOKEN_KEYWORD_INSENSITIVE= TOKEN_KEYWORD_INOUT + 1,
    TOKEN_KEYWORD_INSERT= TOKEN_KEYWORD_INSENSITIVE + 1,
    TOKEN_KEYWORD_INT= TOKEN_KEYWORD_INSERT + 1,
    TOKEN_KEYWORD_INT1= TOKEN_KEYWORD_INT + 1,
    TOKEN_KEYWORD_INT2= TOKEN_KEYWORD_INT1 + 1,
    TOKEN_KEYWORD_INT3= TOKEN_KEYWORD_INT2 + 1,
    TOKEN_KEYWORD_INT4= TOKEN_KEYWORD_INT3 + 1,
    TOKEN_KEYWORD_INT8= TOKEN_KEYWORD_INT4 + 1,
    TOKEN_KEYWORD_INTEGER= TOKEN_KEYWORD_INT8 + 1,
    TOKEN_KEYWORD_INTERVAL= TOKEN_KEYWORD_INTEGER + 1,
    TOKEN_KEYWORD_INTO= TOKEN_KEYWORD_INTERVAL + 1,
    TOKEN_KEYWORD_IO_AFTER_GTIDS= TOKEN_KEYWORD_INTO + 1,
    TOKEN_KEYWORD_IO_BEFORE_GTIDS= TOKEN_KEYWORD_IO_AFTER_GTIDS + 1,
    TOKEN_KEYWORD_IS= TOKEN_KEYWORD_IO_BEFORE_GTIDS + 1,
    TOKEN_KEYWORD_ITERATE= TOKEN_KEYWORD_IS + 1,
    TOKEN_KEYWORD_JOIN= TOKEN_KEYWORD_ITERATE + 1,
    TOKEN_KEYWORD_KEY= TOKEN_KEYWORD_JOIN + 1,
    TOKEN_KEYWORD_KEYS= TOKEN_KEYWORD_KEY + 1,
    TOKEN_KEYWORD_KILL= TOKEN_KEYWORD_KEYS + 1,
    TOKEN_KEYWORD_LEADING= TOKEN_KEYWORD_KILL + 1,
    TOKEN_KEYWORD_LEAVE= TOKEN_KEYWORD_LEADING + 1,
    TOKEN_KEYWORD_LEFT= TOKEN_KEYWORD_LEAVE + 1,
    TOKEN_KEYWORD_LIKE= TOKEN_KEYWORD_LEFT + 1,
    TOKEN_KEYWORD_LIMIT= TOKEN_KEYWORD_LIKE + 1,
    TOKEN_KEYWORD_LINEAR= TOKEN_KEYWORD_LIMIT + 1,
    TOKEN_KEYWORD_LINES= TOKEN_KEYWORD_LINEAR + 1,
    TOKEN_KEYWORD_LOAD= TOKEN_KEYWORD_LINES + 1,
    TOKEN_KEYWORD_LOCALTIME= TOKEN_KEYWORD_LOAD + 1,
    TOKEN_KEYWORD_LOCALTIMESTAMP= TOKEN_KEYWORD_LOCALTIME + 1,
    TOKEN_KEYWORD_LOCK= TOKEN_KEYWORD_LOCALTIMESTAMP + 1,
    TOKEN_KEYWORD_LONG= TOKEN_KEYWORD_LOCK + 1,
    TOKEN_KEYWORD_LONGBLOB= TOKEN_KEYWORD_LONG + 1,
    TOKEN_KEYWORD_LONGTEXT= TOKEN_KEYWORD_LONGBLOB + 1,
    TOKEN_KEYWORD_LOOP= TOKEN_KEYWORD_LONGTEXT + 1,
    TOKEN_KEYWORD_LOW_PRIORITY= TOKEN_KEYWORD_LOOP + 1,
    TOKEN_KEYWORD_MASTER_BIND= TOKEN_KEYWORD_LOW_PRIORITY + 1,
    TOKEN_KEYWORD_MASTER_SSL_VERIFY_SERVER_CERT= TOKEN_KEYWORD_MASTER_BIND + 1,
    TOKEN_KEYWORD_MATCH= TOKEN_KEYWORD_MASTER_SSL_VERIFY_SERVER_CERT + 1,
    TOKEN_KEYWORD_MAXVALUE= TOKEN_KEYWORD_MATCH + 1,
    TOKEN_KEYWORD_MEDIUMBLOB= TOKEN_KEYWORD_MAXVALUE + 1,
    TOKEN_KEYWORD_MEDIUMINT= TOKEN_KEYWORD_MEDIUMBLOB + 1,
    TOKEN_KEYWORD_MEDIUMTEXT= TOKEN_KEYWORD_MEDIUMINT + 1,
    TOKEN_KEYWORD_MIDDLEINT= TOKEN_KEYWORD_MEDIUMTEXT + 1,
    TOKEN_KEYWORD_MINUTE_MICROSECOND= TOKEN_KEYWORD_MIDDLEINT + 1,
    TOKEN_KEYWORD_MINUTE_SECOND= TOKEN_KEYWORD_MINUTE_MICROSECOND + 1,
    TOKEN_KEYWORD_MOD= TOKEN_KEYWORD_MINUTE_SECOND + 1,
    TOKEN_KEYWORD_MODIFIES= TOKEN_KEYWORD_MOD + 1,
    TOKEN_KEYWORD_NATURAL= TOKEN_KEYWORD_MODIFIES + 1,
    TOKEN_KEYWORD_NONBLOCKING= TOKEN_KEYWORD_NATURAL + 1,
    TOKEN_KEYWORD_NOPAGER= TOKEN_KEYWORD_NONBLOCKING + 1,
    TOKEN_KEYWORD_NOT= TOKEN_KEYWORD_NOPAGER + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_NOTEE= TOKEN_KEYWORD_NOT + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_NOWARNING= TOKEN_KEYWORD_NOTEE + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_NO_WRITE_TO_BINLOG= TOKEN_KEYWORD_NOWARNING + 1,
    TOKEN_KEYWORD_NULL= TOKEN_KEYWORD_NO_WRITE_TO_BINLOG + 1,
    TOKEN_KEYWORD_NUMERIC= TOKEN_KEYWORD_NULL + 1,
    TOKEN_KEYWORD_ON= TOKEN_KEYWORD_NUMERIC + 1,
    TOKEN_KEYWORD_OPTIMIZE= TOKEN_KEYWORD_ON + 1,
    TOKEN_KEYWORD_OPTION= TOKEN_KEYWORD_OPTIMIZE + 1,
    TOKEN_KEYWORD_OPTIONALLY= TOKEN_KEYWORD_OPTION + 1,
    TOKEN_KEYWORD_OR= TOKEN_KEYWORD_OPTIONALLY + 1,
    TOKEN_KEYWORD_ORDER= TOKEN_KEYWORD_OR + 1,
    TOKEN_KEYWORD_OUT= TOKEN_KEYWORD_ORDER + 1,
    TOKEN_KEYWORD_OUTER= TOKEN_KEYWORD_OUT + 1,
    TOKEN_KEYWORD_OUTFILE= TOKEN_KEYWORD_OUTER + 1,
    TOKEN_KEYWORD_PAGER= TOKEN_KEYWORD_OUTFILE + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_PARTITION= TOKEN_KEYWORD_PAGER + 1,
    TOKEN_KEYWORD_PRECISION= TOKEN_KEYWORD_PARTITION + 1,
    TOKEN_KEYWORD_PRIMARY= TOKEN_KEYWORD_PRECISION + 1,
    TOKEN_KEYWORD_PRINT= TOKEN_KEYWORD_PRIMARY + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_PROCEDURE= TOKEN_KEYWORD_PRINT + 1,
    TOKEN_KEYWORD_PROMPT= TOKEN_KEYWORD_PROCEDURE + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_PURGE= TOKEN_KEYWORD_PROMPT + 1,
    TOKEN_KEYWORD_QUIT= TOKEN_KEYWORD_PURGE + 1,  /* Ocelot keyword */
    TOKEN_KEYWORD_RANGE= TOKEN_KEYWORD_QUIT + 1,
    TOKEN_KEYWORD_READ= TOKEN_KEYWORD_RANGE + 1,
    TOKEN_KEYWORD_READS= TOKEN_KEYWORD_READ + 1,
    TOKEN_KEYWORD_READ_WRITE= TOKEN_KEYWORD_READS + 1,
    TOKEN_KEYWORD_REAL= TOKEN_KEYWORD_READ_WRITE + 1,
    TOKEN_KEYWORD_REFERENCES= TOKEN_KEYWORD_REAL + 1,
    TOKEN_KEYWORD_REGEXP= TOKEN_KEYWORD_REFERENCES + 1,
    TOKEN_KEYWORD_REHASH= TOKEN_KEYWORD_REGEXP + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_RELEASE= TOKEN_KEYWORD_REHASH + 1,
    TOKEN_KEYWORD_RENAME= TOKEN_KEYWORD_RELEASE + 1,
    TOKEN_KEYWORD_REPEAT= TOKEN_KEYWORD_RENAME + 1,
    TOKEN_KEYWORD_REPLACE= TOKEN_KEYWORD_REPEAT + 1,
    TOKEN_KEYWORD_REQUIRE= TOKEN_KEYWORD_REPLACE + 1,
    TOKEN_KEYWORD_RESIGNAL= TOKEN_KEYWORD_REQUIRE + 1,
    TOKEN_KEYWORD_RESTRICT= TOKEN_KEYWORD_RESIGNAL + 1,
    TOKEN_KEYWORD_RETURN= TOKEN_KEYWORD_RESTRICT + 1,
    TOKEN_KEYWORD_REVOKE= TOKEN_KEYWORD_RETURN + 1,
    TOKEN_KEYWORD_RIGHT= TOKEN_KEYWORD_REVOKE + 1,
    TOKEN_KEYWORD_RLIKE= TOKEN_KEYWORD_RIGHT + 1,
    TOKEN_KEYWORD_ROW= TOKEN_KEYWORD_RLIKE + 1,
    TOKEN_KEYWORD_SCHEMA= TOKEN_KEYWORD_ROW + 1,
    TOKEN_KEYWORD_SCHEMAS= TOKEN_KEYWORD_SCHEMA + 1,
    TOKEN_KEYWORD_SECOND_MICROSECOND= TOKEN_KEYWORD_SCHEMAS + 1,
    TOKEN_KEYWORD_SELECT= TOKEN_KEYWORD_SECOND_MICROSECOND + 1,
    TOKEN_KEYWORD_SENSITIVE= TOKEN_KEYWORD_SELECT + 1,
    TOKEN_KEYWORD_SEPARATOR= TOKEN_KEYWORD_SENSITIVE + 1,
    TOKEN_KEYWORD_SET= TOKEN_KEYWORD_SEPARATOR + 1,
    TOKEN_KEYWORD_SHOW= TOKEN_KEYWORD_SET + 1,
    TOKEN_KEYWORD_SIGNAL= TOKEN_KEYWORD_SHOW + 1,
    TOKEN_KEYWORD_SMALLINT= TOKEN_KEYWORD_SIGNAL + 1,
    TOKEN_KEYWORD_SOURCE= TOKEN_KEYWORD_SMALLINT + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_SPATIAL= TOKEN_KEYWORD_SOURCE + 1,
    TOKEN_KEYWORD_SPECIFIC= TOKEN_KEYWORD_SPATIAL + 1,
    TOKEN_KEYWORD_SQL= TOKEN_KEYWORD_SPECIFIC + 1,
    TOKEN_KEYWORD_SQLEXCEPTION= TOKEN_KEYWORD_SQL + 1,
    TOKEN_KEYWORD_SQLSTATE= TOKEN_KEYWORD_SQLEXCEPTION + 1,
    TOKEN_KEYWORD_SQLWARNING= TOKEN_KEYWORD_SQLSTATE + 1,
    TOKEN_KEYWORD_SQL_BIG_RESULT= TOKEN_KEYWORD_SQLWARNING + 1,
    TOKEN_KEYWORD_SQL_CALC_FOUND_ROWS= TOKEN_KEYWORD_SQL_BIG_RESULT + 1,
    TOKEN_KEYWORD_SQL_SMALL_RESULT= TOKEN_KEYWORD_SQL_CALC_FOUND_ROWS + 1,
    TOKEN_KEYWORD_SSL= TOKEN_KEYWORD_SQL_SMALL_RESULT + 1,
    TOKEN_KEYWORD_STARTING= TOKEN_KEYWORD_SSL + 1,
    TOKEN_KEYWORD_STATUS= TOKEN_KEYWORD_STARTING + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_STRAIGHT_JOIN= TOKEN_KEYWORD_STATUS + 1,
    TOKEN_KEYWORD_SYSTEM= TOKEN_KEYWORD_STRAIGHT_JOIN + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_TABLE= TOKEN_KEYWORD_SYSTEM + 1,
    TOKEN_KEYWORD_TEE= TOKEN_KEYWORD_TABLE + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_TERMINATED= TOKEN_KEYWORD_TEE + 1,
    TOKEN_KEYWORD_THEN= TOKEN_KEYWORD_TERMINATED + 1,
    TOKEN_KEYWORD_TINYBLOB= TOKEN_KEYWORD_THEN + 1,
    TOKEN_KEYWORD_TINYINT= TOKEN_KEYWORD_TINYBLOB + 1,
    TOKEN_KEYWORD_TINYTEXT= TOKEN_KEYWORD_TINYINT + 1,
    TOKEN_KEYWORD_TO= TOKEN_KEYWORD_TINYTEXT + 1,
    TOKEN_KEYWORD_TRAILING= TOKEN_KEYWORD_TO + 1,
    TOKEN_KEYWORD_TRIGGER= TOKEN_KEYWORD_TRAILING + 1,
    TOKEN_KEYWORD_TRUE= TOKEN_KEYWORD_TRIGGER + 1,
    TOKEN_KEYWORD_UNDO= TOKEN_KEYWORD_TRUE + 1,
    TOKEN_KEYWORD_UNION= TOKEN_KEYWORD_UNDO + 1,
    TOKEN_KEYWORD_UNIQUE= TOKEN_KEYWORD_UNION + 1,
    TOKEN_KEYWORD_UNLOCK= TOKEN_KEYWORD_UNIQUE + 1,
    TOKEN_KEYWORD_UNSIGNED= TOKEN_KEYWORD_UNLOCK + 1,
    TOKEN_KEYWORD_UPDATE= TOKEN_KEYWORD_UNSIGNED + 1,
    TOKEN_KEYWORD_USAGE= TOKEN_KEYWORD_UPDATE + 1,
    TOKEN_KEYWORD_USE= TOKEN_KEYWORD_USAGE + 1, /* Ocelot keyword, also reserved word */
    TOKEN_KEYWORD_USING= TOKEN_KEYWORD_USE + 1,
    TOKEN_KEYWORD_UTC_DATE= TOKEN_KEYWORD_USING + 1,
    TOKEN_KEYWORD_UTC_TIME= TOKEN_KEYWORD_UTC_DATE + 1,
    TOKEN_KEYWORD_UTC_TIMESTAMP= TOKEN_KEYWORD_UTC_TIME + 1,
    TOKEN_KEYWORD_VALUES= TOKEN_KEYWORD_UTC_TIMESTAMP + 1,
    TOKEN_KEYWORD_VARBINARY= TOKEN_KEYWORD_VALUES + 1,
    TOKEN_KEYWORD_VARCHAR= TOKEN_KEYWORD_VARBINARY + 1,
    TOKEN_KEYWORD_VARCHARACTER= TOKEN_KEYWORD_VARCHAR + 1,
    TOKEN_KEYWORD_VARYING= TOKEN_KEYWORD_VARCHARACTER + 1,
    TOKEN_KEYWORD_WARNINGS= TOKEN_KEYWORD_VARYING + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_WHEN= TOKEN_KEYWORD_WARNINGS + 1,
    TOKEN_KEYWORD_WHERE= TOKEN_KEYWORD_WHEN + 1,
    TOKEN_KEYWORD_WHILE= TOKEN_KEYWORD_WHERE + 1,
    TOKEN_KEYWORD_WITH= TOKEN_KEYWORD_WHILE + 1,
    TOKEN_KEYWORD_WRITE= TOKEN_KEYWORD_WITH + 1,
    TOKEN_KEYWORD_XOR= TOKEN_KEYWORD_WRITE + 1,
    TOKEN_KEYWORD_YEAR_MONTH= TOKEN_KEYWORD_XOR + 1,
    TOKEN_KEYWORD_ZEROFILL= TOKEN_KEYWORD_YEAR_MONTH + 1
       };

  void tokenize(QChar *text, int text_length, int (*token_lengths)[MAX_TOKENS], int (*token_offsets)[MAX_TOKENS], int max_tokens, QChar *version, int passed_comment_behaviour, QString special_token, int minus_behaviour);

  int token_type(QChar *token, int token_length);

  void tokens_to_keywords();
  int connect_mysql();

  QWidget *main_window;
  QVBoxLayout *main_layout;

  CodeEditor *statement_edit_widget;
  QTextEdit *history_edit_widget;

//  QDockWidget *test_dock;

  QMenuBar *menuBar;
  QMenu *menu_file;
    QAction *menu_file_action_connect;
    QAction *menu_file_action_exit;
  QMenu *menu_edit;
    QAction *menu_edit_action_cut;
    QAction *menu_edit_action_copy;
    QAction *menu_edit_action_paste;
    QAction *menu_edit_action_undo;
    QAction *menu_edit_action_redo;
    QAction *menu_edit_action_select_all;
    QAction *menu_edit_action_history_markup_previous;
    QAction *menu_edit_action_history_markup_next;
  QMenu *menu_run;
    QAction *menu_run_action_execute;
  QMenu *menu_settings;
    QAction *menu_settings_action_statement;
    QAction *menu_settings_action_grid;
    QAction *menu_settings_action_history;
    QAction *menu_settings_action_main;
  QMenu *menu_help;
    QAction *menu_help_action_about;
    QAction *menu_help_action_the_manual;

  QWidget *the_manual_widget;
    QVBoxLayout *the_manual_layout;
    QTextEdit *the_manual_text_edit;
    QPushButton *the_manual_pushbutton;

  /* QTableWidget *grid_table_widget; */
  ResultGrid *result_grid_table_widget;

  unsigned long result_row_count;

  int history_markup_counter; /* 0 when execute, +1 when "previous statement", -1 for "next statement" */

  int  main_token_offsets[MAX_TOKENS];
  int  main_token_lengths[MAX_TOKENS];
  int  main_token_types[MAX_TOKENS];
  unsigned int main_token_count;
  unsigned int main_token_count_in_statement;
  unsigned int main_token_number;      /* = offset within main_token_offsets, e.g. 0 if currently at first token */
  unsigned int main_statement_type;    /* = 0, or STATEMENT_TYPE_QUESTIONMARK etc. */

  int statement_edit_widget_text_changed_flag;
  QString ocelot_delimiter_str;                                           /* set up in connect section */
  QString query_utf16;
  QString query_utf16_copy;
  int ocelot_password_was_specified;

  /* MYSQL mysql; */
  MYSQL_RES *mysql_res;
  /* MYSQL_FIELD *fields; */
  int is_mysql_connected;

  /*
    Client variables.
    The eventual idea is that client variables can
    sometimes be set with
    SET @client-variable-name = value;
    which is handled by execute_client_statement(),
    and also is passed on to the server (so that a
    user can also "SELECT @client-variable-name;"
    after connecting). Most client variables affect
    the look of the widgets, for example colour.
    That's not happening consistently yet.
    All client variable names start with 'ocelot_'
  */
  /* See also: Client variables that can be changed with the Settings widget */
  /* Todo: Shouldn't client variables be in statement widget? */
  QString ocelot_database;                /* for CONNECT */
  QString ocelot_dbms;                    /* for CONNECT */
  int ocelot_grid_detached;
  unsigned int ocelot_grid_max_row_lines;          /* ?? should be unsigned long? */
  /* unsigned int ocelot_grid_max_desired_width_in_chars; */
  int ocelot_history_includes_warnings;   /* affects history */
};



#endif // MAINWINDOW_H

/*****************************************************************************************************************************/
/* THE TEXTEDITFRAME WIDGET */
/* See comments containing the word TextEditFrame, in ResultGrid code. */

class TextEditFrame : public QFrame
{
  Q_OBJECT

public:
  explicit TextEditFrame(QWidget *parent, ResultGrid *ancestor, int column_number);
  ~TextEditFrame();

  int border_size;
  int minimum_width;
  ResultGrid *ancestor_result_grid_widget;
  int ancestor_column_number;

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent *event);

private:
  int left_mouse_button_was_pressed;
  int widget_side;
  enum {LEFT= 1, RIGHT= 2};

};

/*********************************************************************************************************/

/* THE GRID WIDGET */
/*
  Todo: make sure the QTextEdit widgets are plain text, in case somebody enters HTML.
  Problems:
      How to I set so there's no space between each row/column?
*/
/*
  Re the "cells" of the grid:
  Todo: Originally these were QPlainTextEdit widgets, because QTextEdit
        would insisted on expanding long lines. That doesn't seem to be happening any more,
        but we might have to switch back if there are problems.
  Todo: allow change of setTextDirection() based on language or on client variable.
  Todo: see this re trail spaces: http://qt-project.org/doc/qt-4.8/qtextoption.html#Flag-enum
  Todo: although min(column width) = heading width, don't have to make text editable area that wide
  Todo: have to think what to do with control characters, e.g. for tabs should I have
        setTabChangesFocus(true), for others should I allow the effects or display specially.
  Todo: I don't know if I should do something about setColumnStretch
  Todo: I don't know if I should do something about setContentsMargins() for the layout
  Todo: I used an eventfilter to detect scrolling because QScrollBar::valueChanged()
        failed, but someday I should find out why and retry.
  Todo: "new" operations should have nothrow checks.
  Todo: different rows have different column heights
  Because they're in a grid with (probably) a scroll bar, I want uniform width and height, and
  perhaps have redundant expressions to tell Qt that's what I want -- I flailed around for hours
  until something finally worked, and left it alone after that, although I might have to add even
  more junk about dontshowonscreen or opacity if there is flickering.
  The wrap policy is QTextOption::WrapAnywhere -- there could be a user-settable option for this
  if the preference was to wrap on word boundaries when possible.
*/
/*
  Todo:
  These events can happen to a grid and force recalculation:
  QEvent::ContentsRectChange               The margins of the widget's content rect changed.
  QEvent::FontChange                       Widget's font has changed.
  QEvent::HideToParent                     a child widget has been hidden
  QEvent::Resize
  QEvent::ShowToParent                     a child widget has been shown
  QEvent::StyleChange                      I don't think this matters
  ... Also, user might resize columns or rows if we finally allow resizing, somehow..
*/

/*
   Grid cells are made resizable by putting a frame around the cell, and detecting mouse-click + mouse-movement on the frame.
   grid_main_layout                        QVBoxLayout occurs 1 time
   grid_main_widget                        QWidget occurs 0 times -- currently we use a widget named "client" instead
     grid_row_layouts                           QHBoxLayout occurs #-of-rows times
     grid_row_widgets                           QWidget occurs #-of-rows times, with layout = row_layouts
       text_edit_frames                             TextEditFrame occurs #-of-columns times within each grid_row_widgets
         text_edit_layouts                              QHBoxLayout ""
           text_edit_widgets                                QTextEdit ""

  Making the grid-cell QTextWidgets resizable was difficult.
  I couldn't get satisfactory results with QSplitter, and don't believe that satisfactory results would be possible.
  Finally a suggestion by Aleksey Osipov aliks-os@yandex.ru https://qt-project.org/wiki/Widget-moveable-and-resizeable
  inspired me. I didn't actually use the suggestion (my requirements are much simpler), but I realized that all one has to do is
  subclass a QFrame with a QTextEdit inside it, and put routines in for when the mouse is pressed|moved|released on
  the QFrame. In this implementation only right|left dragging of the subclassed QFrame, called TextEditFrame, is allowed.
  Todo: Mr Osipov had routines for resizing with keyboard control, and I should add that.
  Todo: up|down dragging, although vertical shrinking will not mean that more rows appear on the screen.
  Todo: make border width user-settable, also on the Settings menu.
  Todo: vertical resizing. I've read that "Correct place to do special layout management is overridden resizeEvent."
  Todo: test with a frame that has been scrolled horizontally so half of it is not visible, while there's a scoll bar.
  Todo: There can be a bit of flicker during drag though I doubt that anyone will care.
  Todo: Figure out what effect Tab key should have. Notice that for form_box we say setTabChangesFocus(true).
  Todo: BUG: If the QTextEdit gets a vertical scroll bar, then the horizontal cursor appears over the scroll bar,
        and dragging won't work. The problem is alleviated if border_size > 1. (Check: maybe this is fixed.)
  Todo: Install an event filter for each text_edit_widgets[i]. If user double-clicks: find which widget it is,
        figure out from that which row it is, and put up a dialog box for that row with row_form_box.
  Re cursor shapes, see http://qt-project.org/doc/qt-4.8/qcursor.html#details
*/

/*
  In fact we have a QTextBox inside a modified QFrame. But it appears as if it's a QTextBox with a border.
  So the menu item Settings|Border Color, or dragger color, actually changes the QFrame's background color.
  And the size of the QTextEdit right "border" is actually the amount that we set with setContentsMargins() on the QFrame.
  The changes made by dragging are persistent as long as the result set is up.
*/

/*
  Note#1:
  For TextEditFrame's mouseMoveEvent we say setCursor to change the shape, as we should, to hint it's draggable.
  But http://qt-project.org/doc/qt-4.8/qwidget.html#cursor-prop says:
  "If no cursor has been set, or after a call to unsetCursor(), the parent's cursor is used."
  That affects the child QTextEdit when it touches the border, and it in turn has a child: the vertical scroll bar.
  Therefore there is also an explicit setCursor for the child QTextEdit.
*/

class ResultGrid: public QWidget
{
  Q_OBJECT
public:
  QWidget *client;

  unsigned int result_column_count;
  long unsigned int result_row_count;
  unsigned long *lengths;
  unsigned long *grid_column_widths;                         /* dynamic-sized list of widths */
  unsigned int *grid_column_heights;                        /* dynamic-sized list of heights */
  unsigned long r;
  MYSQL_ROW row;

  MYSQL_FIELD *fields;
  QScrollArea *grid_scroll_area;
  QScrollBar *grid_vertical_scroll_bar;
  int grid_vertical_scroll_bar_value;
  QTextEdit **text_edit_widgets; /* Todo: consider using plaintext */ /* dynamic-sized list of pointers to QPlainTextEdit widgets */
  QHBoxLayout **text_edit_layouts;
  TextEditFrame **text_edit_frames;

  MYSQL_RES *grid_mysql_res;
  unsigned int ocelot_grid_max_grid_height_in_lines;             /* Todo: should be user-settable and passed */
  unsigned int grid_actual_grid_height_in_rows;
  unsigned int grid_actual_row_height_in_lines;
  /* ocelot_grid_height_of_highest_column will be between 1 and ocelot_grid_max_column_height_in_lines, * pixels-per-line */
  unsigned int grid_height_of_highest_column_in_pixels;
  unsigned int max_height_of_a_char;

  QHBoxLayout *hbox_layout;

  QHBoxLayout **grid_row_layouts;                               /* dynamic-sized list of pointers to rows' QHBoxLayout layouts */
  QWidget **grid_row_widgets;                                   /* dynamic-sized list of pointers to rows' QWidget widgets */
  QVBoxLayout *grid_main_layout;                                   /* replaces QGridLayout *grid_layout */
  /* QWidget *grid_main_widget;  */                                /* replaces QGridLayout *grid_layout -- but we say "client" */

  MainWindow *copy_of_parent;

  int border_size;                                             /* used when calculating cell height + width */
  unsigned int ocelot_grid_max_desired_width_in_pixels;        /* used when calculating cell height + width */
  unsigned int ocelot_grid_max_column_height_in_lines;         /* used when calculating cell height + width */

  int ocelot_grid_cell_right_drag_line_size_as_int;
  QString ocelot_grid_color;
  QString ocelot_grid_background_color;
  QString ocelot_grid_cell_right_drag_line_color;

ResultGrid(MYSQL_RES *mysql_res, QFont *saved_font, MainWindow *parent): QWidget(parent)
{
  ocelot_grid_max_grid_height_in_lines= 35;                 /* Todo: should be user-settable and passed */

  client= new QWidget(this);

  copy_of_parent= parent;

  grid_mysql_res= mysql_res;

  text_edit_widgets= 0;                                     /* all dynamic-sized items should be initially zero */
  text_edit_layouts= 0;
  text_edit_frames= 0;
  grid_column_widths= 0;
  grid_column_heights= 0;
  grid_vertical_scroll_bar= 0;
  grid_scroll_area= 0;
  /* grid_layout= 0; */
  hbox_layout= 0;
  grid_row_layouts= 0;
  grid_row_widgets= 0;
  grid_main_layout= 0;
  /* grid_main_widget= 0; */
  border_size= 1;                                          /* Todo: This actually has to depend on stylesheet */

  /* We might say "new ResultGrid(0)" merely so we'd have ResultGrid in the middle spot in the layout-> */
  /* Todo: memory leak? We don't get rid of previous client when we make a new real client. */
  if (mysql_res == 0)
  {
    return;
  }

  ocelot_grid_color= parent->ocelot_grid_color;
  ocelot_grid_background_color= parent->ocelot_grid_background_color;
  /* ocelot_grid_cell_right_drag_line_size_as_int= parent->ocelot_grid_cell_right_drag_line_size.toInt(); */
  /* ocelot_grid_cell_right_drag_line_color= parent->ocelot_grid_cell_right_drag_line_color; */
  result_column_count= mysql_num_fields(grid_mysql_res);           /* this will be the width of the grid */
  result_row_count= mysql_num_rows(grid_mysql_res);                /* this will be the height of the grid */

  /*
    Dynamic-sized arrays for rows and columns.
    Some are two-dimensional e.g. text_edit_widgets; I'll address its elements with text_edit_widgets[row*col+col].
    For every "new " here, there should be a "delete []" in the clear() or garbage_collect() function.
  */
  /* Todo: find out why the size is based on result_row_count -- we don't usually need that many */
  /* ... */
  text_edit_widgets= new QTextEdit*[(result_row_count + 1) * result_column_count]; /* Plain? */
  text_edit_layouts= new QHBoxLayout*[(result_row_count + 1) * result_column_count];
  text_edit_frames= new TextEditFrame*[(result_row_count + 1) * result_column_count];
  grid_column_widths= new long unsigned int[result_column_count];
  grid_column_heights= new unsigned int[result_column_count];
  /* ... */

  fields= mysql_fetch_fields(grid_mysql_res);

  grid_scroll_area= new QScrollArea(this);                                    /* Todo: see why parent can't be client */
  grid_vertical_scroll_bar= new QScrollBar();                                 /* Todo: check if it's bad that this has no parent */

  grid_vertical_scroll_bar->setMaximum(result_row_count);
  grid_vertical_scroll_bar->setPageStep(result_row_count / 10);
  grid_vertical_scroll_bar_value= 0;

  /*
    Calculate desired width and height based on parent width and height.
     Desired max width in chars = width when created - width of scroll bar.
     Max height in lines = height when created - (height of scroll bar + height of header) / 4. minimum = 1.
     Todo: We're dividing height-when-created by 3 because we assume statement+history widgets are there, maybe they're not.
     Todo: max height could be greater if row count < 4, or maybe it should be user-settable.
     Todo: grid_vertical_scroll_bar->width() failed so I just guessed that I should subtract 3 char widths.
  */

  /* Todo: since grid_column_size_calc() recalculates max_height_of_a_char, don't bother with this. */

  QFontMetrics mm= QFontMetrics(*saved_font);

  /* Todo: figure out why this says parent->width() rather than this->width() -- maybe "this" has no width yet? */
  ocelot_grid_max_desired_width_in_pixels=(parent->width() - (mm.width("W") * 3));
  ocelot_grid_max_column_height_in_lines=((parent->height() / 3) - (mm.lineSpacing()) * 3) / mm.lineSpacing();
  ocelot_grid_max_column_height_in_lines=ocelot_grid_max_column_height_in_lines / 3;
  if (ocelot_grid_max_column_height_in_lines < 1) ocelot_grid_max_column_height_in_lines= 1;
  put_widgets_in_layouts(saved_font);
}


void put_widgets_in_layouts(QFont *saved_font)
{
  long unsigned int xrow;
  unsigned int col;

  ocelot_grid_cell_right_drag_line_size_as_int= copy_of_parent->ocelot_grid_cell_right_drag_line_size.toInt();
  ocelot_grid_cell_right_drag_line_color= copy_of_parent->ocelot_grid_cell_right_drag_line_color;

  grid_column_size_calc(saved_font); /* get grid_column_widths[] and grid_column_heights[] */

  /*
    grid_actual_grid_height_in_rows = # of rows that are actually showable at a time,
    = lesser of (grid_max_grid_height_in_lines/grid_max_row_height_in_lines, # of rows in result set + 1)
  */
  grid_actual_grid_height_in_rows= ocelot_grid_max_grid_height_in_lines / grid_actual_row_height_in_lines;
  if (grid_actual_grid_height_in_rows > result_row_count + 1) grid_actual_grid_height_in_rows= result_row_count + 1;

  /*
    Make the cells. Each cell is one QTextEdit within one QHBoxLayout within one TextEditFrame.
    Each TexteditFrame i.e. text_edit_frames[n] will be added to the scroll area.
  */
  for (xrow= 0; xrow < grid_actual_grid_height_in_rows; ++xrow)
  {
    for (unsigned int column_number= 0; column_number < result_column_count; ++column_number)
    {
      QFontMetrics fm= QFontMetrics(*saved_font);
      int ki= xrow * result_column_count + column_number;
      text_edit_widgets[ki]= new QTextEdit();  /* Whoops. Originally this was "new QTextEdit(client)" */

      text_edit_widgets[ki]->setMinimumWidth(fm.width("W") * 3);
      text_edit_widgets[ki]->setMinimumHeight(fm.height() * 2);

      text_edit_widgets[ki]->setCursor(Qt::ArrowCursor); /* See Note#1 above */
      text_edit_layouts[ki]= new QHBoxLayout();

      text_edit_layouts[ki]->setContentsMargins(QMargins(0, 0, ocelot_grid_cell_right_drag_line_size_as_int, 0));
      text_edit_layouts[ki]->addWidget(text_edit_widgets[ki]);
      text_edit_frames[ki]= new TextEditFrame(this, this, column_number);
      /*
        Change the color of the frame. Be specific that it's TextEditFrame, because you don't want the
        children e.g. the QTextEdit to inherit the color. TextEditFrame is a custom widget and therefore
        setStyleSheet is troublesome for most settings, but background-color should be okay, see
        http://stackoverflow.com/questions/7276330/qt-stylesheet-for-custom-widget.
      */
      QString frame_color_setting= "TextEditFrame {background-color: ";
      frame_color_setting.append(ocelot_grid_cell_right_drag_line_color);
      frame_color_setting.append("}");
      text_edit_frames[ki]->setStyleSheet(frame_color_setting);

      /* Todo: I'm not sure exactly where the following three lines should go. Consider moving them. */
      text_edit_frames[ki]->border_size= 10 + border_size;    /* Todo: should just be border_size!! */
      text_edit_frames[ki]->minimum_width= fm.width("W") * 3 + border_size;
      text_edit_frames[ki]->setLayout(text_edit_layouts[ki]);
    }
  }

  if (layout() != 0)
  {
    printf("Pseudo-assertion 2: "" already has a layout\n"); exit(1);
  }

  /* Todo: check if actual_grid_height_in_rows is safe, rather than result_row_count */
  /* text_edit_widgets= new QTextEdit*[actual_grid_height_in_rows*result_column_count]; */
  grid_row_layouts= new QHBoxLayout*[grid_actual_grid_height_in_rows];
  grid_row_widgets= new QWidget*[grid_actual_grid_height_in_rows];
  for (unsigned int xxrow_number= 0; xxrow_number < grid_actual_grid_height_in_rows; ++xxrow_number)
  {
    grid_row_layouts[xxrow_number]= new QHBoxLayout();  /* I had "(client)" here. That caused warnings. */
    grid_row_layouts[xxrow_number]->setSpacing(0);
    grid_row_layouts[xxrow_number]->setContentsMargins(QMargins(0, 0, 0, 0));
    grid_row_widgets[xxrow_number]= new QWidget(client);
  }
  grid_main_layout= new QVBoxLayout(client);
  grid_main_layout->setContentsMargins(QMargins(0, 0, 0, 0));   /* this overrides style settings, I suppose */

  grid_main_layout->setSpacing(0);                          /* ?? premature? */

  if (layout() != 0)
  {
    printf("Pseudo-assertion 2a: "" already has a layout\n"); exit(1);
  }

  unsigned int i;

  /* Put the QTextEdit widgets in a layout. Remember row[0] is for the header. */
  /*
    Re making the row. Each row is [column_count] cells within one QHBoxLayout within one widget.
    grid_row_layout->setSizeConstraint(QLayout::SetMaximumSize) prevents gaps from forming during shrink.
    There's a "border", actually the visible part of TextEditFrame, on the cell's right.
    Drag it left to shrink the cell, drag it right to expand the cell.
    We do not resize cells on the left or right of the to-be-dragged cell, so expanding causes total row width to expand,
    possibly going beyond the original desired maximum width, possibly causing a horizontal scroll bar to appear.
    grid_row_layout->setSpacing(0) means the only thing separating cells is the "border".
  */
  for (long unsigned int xrow= 0; xrow < grid_actual_grid_height_in_rows; ++xrow)
  {
    grid_main_layout->addWidget(grid_row_widgets[xrow]);
    for (col= 0; col < result_column_count; ++col)
    {
      QTextEdit *l= text_edit_widgets[xrow * result_column_count + col];
      TextEditFrame *f= text_edit_frames[xrow * result_column_count + col];
      /*
        I'll right-align if type is number and this isn't the header.
        But I read somewhere that might not be compatible with wrapping,
        so I'll wrap only for non-number.
      */
      /* Todo: some other types e.g. BLOBs might also need special handling. */
      if ((xrow > 0) && (fields[col].flags & NUM_FLAG))
      {
        l->document()->setDefaultTextOption(QTextOption(Qt::AlignRight));
        l->setAlignment(Qt::AlignRight);
      }
      else l->setWordWrapMode(QTextOption::WrapAnywhere);
      /* l->setMaximumWidth(grid_column_widths[col]); */
      /* Height border size = 1 due to setStyleSheet earlier; right border size is passed */
      if (xrow == 0)
      {
        f->setFixedSize(grid_column_widths[col], max_height_of_a_char + (border_size * 2) + 9);
        f->setMaximumHeight(max_height_of_a_char+(border_size * 2) + 10);
        f->setMinimumHeight(max_height_of_a_char+(border_size * 2) + 10);
      }
      else
      {
        /* l->setMinimumHeight(max_height_of_a_char+(border_size*2) + 10); */
        f->setFixedSize(grid_column_widths[col], grid_column_heights[col]);
        f->setMaximumHeight(grid_column_heights[col]);
        f->setMinimumHeight(grid_column_heights[col]);
      }
      f->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);   /* This doesn't seem to do anything */
      /* l->setMinimumWidth(1); */
      /*
        Todo: setStyleSheet() had to be removed because if it's here, setting of header palette won't work.
        I think the solution is: use setStyleSheet always, although then setFont won't work.
        And is there a way to find out what stylesheet settings are in use by QTableWidget?
      */
      /* l->setStyleSheet("border: 1px solid yellow"); */ /* just so I know the size of the border. Todo: user-settable? */

      grid_row_layouts[xrow]->addWidget(f, 0, Qt::AlignTop | Qt::AlignLeft);

      l->updateGeometry();
      }
    }

  if (layout() != 0)
  {
    printf("Pseudo-assertion 2b: "" already has a layout\n"); exit(1);
  }

  grid_main_layout->setSizeConstraint(QLayout::SetFixedSize);  /* This ensures the grid columns have no spaces between them */

  for (long unsigned int xrow= 0; xrow < grid_actual_grid_height_in_rows; ++xrow)
  {
    if (grid_row_widgets[xrow]->layout() != 0)
    {
      printf("Pseudo-assertion: grid_row_widgets[xrow] already has a layout\n"); exit(1);
    }
    grid_row_widgets[xrow]->setLayout(grid_row_layouts[xrow]);
    grid_main_layout->addWidget(grid_row_widgets[xrow], 0, Qt::AlignTop | Qt::AlignLeft);
  }
  if (client->layout() != 0)
  {
    printf("Pseudo-assertion: client already has a layout\n");
  }
  client->setLayout(grid_main_layout);

  /* This doesn't work. Done too early? */
  /* client->setStyleSheet(copy_of_parent->ocelot_grid_style_string); */

  client->show();                                          /* Without this, grid becomes blank after font change */

  grid_scroll_area->setWidget(client);
  grid_scroll_area->setWidgetResizable(true);              /* Without this, the QTextEdit widget heights won't change */

  grid_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  /* We have an external vertical scroll bar. */
  /* area->horizontalScrollBar()->setSingleStep(client->width() / 24); */ /* single-stepping seems pointless */

  /* Horizontal box to hold scroll area and vertical scroll bar. */
  if (layout() != 0)
  {
    printf("Pseudo-assertion 2e: "" already has a layout\n"); exit(1);
  }

  /* setContentsMargins overrides style settings, I suppose. */
  /* Will setSpacing(0) keep scroll bar beside scroll area? Apparently not. Useless. */
  /* We could add a label with a line like the one below. That would be a header. Useless?
     hbox_layout->addWidget(new QLabel("La La La", this)); */
  hbox_layout= new QHBoxLayout(this);
  /*
    An earlier comment about this area said:
    "The following line would make the grid_vertical_scroll_bar appear right beside the
    grid_scroll_area, but the scroll bars on the QTextEdit widgets would disappear. No good.
    hbox_layout->setSizeConstraint(QLayout::SetFixedSize);
       ... but that would be a reasonable thing to do if total width of all columns is less than widget width."
    But now, I'm not seeing that the scroll bars on the QTextEdit
    widgets disappear. If this wasn't here, it would be tough to prevent width expansion when columns
    don't fill the line. Even as it is, the amount of desperate kludging is something to behold, eh?
    Todo: calculate the width more accurately.
    Todo: Now the bloody thing won't get shrink! Need to get the signal for resizing.
  */
  /*
    int xxsize= 0 ;
    for (int xxcol= 0 ; xxcol<result_column_count; ++xxcol) xxsize+=grid_column_widths[xxcol] + 10;
    if (xxsize > (ocelot_grid_max_desired_width_in_pixels-50)) xxsize=ocelot_grid_max_desired_width_in_pixels-50;
    grid_scroll_area->setMinimumWidth(xxsize);
  */
  hbox_layout->setContentsMargins(0, 0, 0, 0);
  hbox_layout->setSpacing(0);
  hbox_layout->addWidget(grid_scroll_area);
  hbox_layout->addWidget(grid_vertical_scroll_bar);
  /* hbox_layout->setSizeConstraint(QLayout::SetFixedSize); */
  /*
    Strange but true: when we said "new QHBoxLayout(this)" that meant this already has a layout,
    so I suppose there is no need to say "setLayout(hbox_layout)" here.
  */
  /* if (layout() != 0) { printf("Pseudo-assertion 3: "" already has a layout\n"); exit(1); } */
  /* setLayout(hbox_layout); */
  /* Can't recall why "grid_vertical_scroll_bar_value= 0;" was here -- it meant that
     after fontchange the rows were back at the start */
  /* grid_vertical_scroll_bar_value= 0; */
  /* grid_vertical_scroll_bar= grid_scroll_area->verticalScrollBar(); */

  vertical_bar_show_as_needed();

  /*
    grid detail rows
    While we're passing through, we also get max column lengths (in characters).
    Todo: Take into account: whether there were any nulls.
    Todo: Don't just assign row[i], use lengths[] in case data contains \0. atoi? strcpy?
  */

  /* Set header text and colour.
     Yes it looks silly that we do this repeatedly, even for scroll. But don't "fix" that. */
  for (i= 0; i < result_column_count; ++i)                      /* row 0 is header */
  {
    /* Todo: we use set>StyleSheet, apparently that's overriding this. */
    text_edit_widgets[0 * result_column_count + i]->setStyleSheet(copy_of_parent->ocelot_grid_header_style_string);
    /*
    QPalette p= text_edit_widgets[0*result_column_count+i]->palette();
     p.setColor(QPalette::Base, QColor(copy_of_parent->ocelot_grid_header_background_color));
     text_edit_widgets[0*result_column_count+i]->setPalette(p);
     */
    QString yy= fields[i].name; /* Todo: use name_length somehow so we don't get fooled by \0 */
    text_edit_widgets[0 * result_column_count + i]->setPlainText(yy);
  }

  fill_detail_widgets();                                             /* details */
}


/*
  More thoughts about grid cell sizing.
  * If it's not plaintext: HTML can cause all sorts of foolishness, but just would cause a scrollbar.
  * There has to be some maximum size, perhaps a user setting.
  * Allow expansion if ctrl-+
  User overrides in statements:
    SQL queries can contain comments in HTML (not just SELECT, also INSERT or UPDATE) so the forms can be changed.
    E.g. SELECT / * <font=times> * / a, b  / * </font> * / c FROM t.
  ocelot.settings:
    Things like default width, maximum height, whether blobs are .png, are all in a table which users can update.
    Maximum height can be # of lines or # of pixels. Maximum width can be # of characters or # of pixels.
  See http://qt-project.org/doc/qt-5.1/qtwidgets/qtextedit.html
  What we've got from (priority order) ocelot.settings, user statement comments, data type + contents.
  result_column_width_in_pixels[]
  result_column_width_in_characters[]
  result_maximum_column_width_in_characters[]           <-- ultimately from data definition
*/

/*
  We know the defined width (fields[i].length)
  and the number of columns
  (result_column_count).
  We don't want to exceed the maximum grid width if we can help it.
  So let's give each column exactly what it needs, and perform a
  "squeeze" (reducing big columns) until the rows will fit, or until
  there's nothing more that can be squeezed.
  Re null: nothing done yet. Todo: do something.
  Re <cr>: I'm not very worried because it merely causes elider
      This assumes that every header or cell in the table has the same font.
      I sometimes wish we could assume fixed-width font.
      I don't look at fields[i].max_lengths, perhaps that's a mistake.
      Todo: this might have to be re-done after a font change
      Todo: take into account that a header is fixed, so you don't need to use 'W'.
      Todo: take into account that a number won't contain anything wider than '9'.
*/
/* header height calculation should differ from ordinary-row height calculation */
/*
  Todo: This did not turn out well ...
        Dunno why I had to say "* 2" towards the end.
        Width is now too much.
*/
/*
  In the calculations, "+(border_size*2)" exists but it's assumed that border_size = 1,
  but I don't know whether that's always true. Eventually we'll try to ensure
  that it is, using setStyleSheet, see above.
*/
void grid_column_size_calc(QFont *saved_font)
{
  unsigned int i;
  /* long unsigned int tmp_column_lengths[MAX_COLUMNS]; */
  long unsigned int sum_tmp_column_lengths;
  long unsigned int sum_amount_reduced;
  long unsigned int necessary_reduction;
  long unsigned int amount_being_reduced;
  long unsigned int max_reduction;

  /* Translate from char width+height to pixel width+height */
  QFontMetrics mm= QFontMetrics(*saved_font);
  /* This was a bug: "this" might not have been updated by setStyleSheet() yet ... QFontMetrics mm(this->fontMetrics()); */
  unsigned int max_width_of_a_char= mm.width("W");    /* not really a maximum unless fixed-width font */
  max_height_of_a_char= mm.lineSpacing();             /* Actually this is mm.height() + mm.leading(). */

  sum_tmp_column_lengths= 0;

  /*
    The first approximation:
    Take it that grid_column_widths[i] = defined column width.
    If this is good enough, then grid_row_heights[i] = 1 char and column width = grid_column_widths[i] chars.
    Todo: the lengths are in bytes; take into account that they might arrive in a multi-byte character set.
  */
  for (i= 0; i < result_column_count; ++i)
  {
    grid_column_widths[i]= this->fields[i].name_length;
    if (grid_column_widths[i] < fields[i].length) grid_column_widths[i]= fields[i].length;
    ++grid_column_widths[i]; /* ?? something do do with border width, I suppose */
    grid_column_widths[i]= grid_column_widths[i] * max_width_of_a_char+(border_size * 2) + 10; /* to pixels */
    sum_tmp_column_lengths+= grid_column_widths[i];
  }

  /*
    The Squeeze
    This cuts the widths of the really long columns, it might loop several times.
    This is a strong attempt to reduce to the user-settable maximum, but if we have to override it, we do.
    Cannot squeeze to less than header length
    Todo: there should be a minimum for the sake of elide, and null, and border
    Todo: maybe there should be a user-settable minimum column width, not just the header-related minimum
  */

  sum_amount_reduced= 1;

  while ((sum_tmp_column_lengths > ocelot_grid_max_desired_width_in_pixels) && (sum_amount_reduced > 0))
  {
    necessary_reduction= sum_tmp_column_lengths - ocelot_grid_max_desired_width_in_pixels;
    sum_amount_reduced= 0;
    for (i= 0; i < result_column_count; ++i)
    {
      unsigned int min_width= (fields[i].name_length + 1) * max_width_of_a_char + border_size * 2;
      if (grid_column_widths[i] <= min_width) continue;
      max_reduction= grid_column_widths[i] - min_width;
      if (grid_column_widths[i] >= (sum_tmp_column_lengths / result_column_count))
      {
        amount_being_reduced= grid_column_widths[i] / 2;

        if (amount_being_reduced > necessary_reduction) amount_being_reduced= necessary_reduction;
        if (amount_being_reduced > max_reduction) amount_being_reduced= max_reduction;
        grid_column_widths[i]= grid_column_widths[i] - amount_being_reduced;
        sum_amount_reduced+= amount_being_reduced;
        necessary_reduction-= amount_being_reduced;
        sum_tmp_column_lengths-= amount_being_reduced;
      }
      if (necessary_reduction == 0) break;
    }
  }

  grid_actual_row_height_in_lines= 1;

  /*
    Each column's height = fields[i].length / grid_column_widths[i] rounded up.
    If that's greater than the user-defined maximum, reduce to user-defined maximum
    (the QTextEdit will get a vertical scroll bar if there's an overflow).
  */
  for (i= 0; i < result_column_count; ++i)
  {
    grid_column_heights[i]= (fields[i].length*max_width_of_a_char) / grid_column_widths[i];
    if ((grid_column_heights[i] * grid_column_widths[i]) < fields[i].length) ++grid_column_heights[i];
    if (grid_column_heights[i] > ocelot_grid_max_column_height_in_lines) grid_column_heights[i]= ocelot_grid_max_column_height_in_lines;
    if (grid_column_heights[i] > grid_actual_row_height_in_lines) grid_actual_row_height_in_lines= grid_column_heights[i];
  }

  grid_height_of_highest_column_in_pixels= 0;

  /* Warning: header-height calculation is also "*(max_height_of_a_char+(border_size*2))", in a different place. */
  for (i= 0; i < result_column_count; ++i)
  {
    grid_column_heights[i]= (grid_column_heights[i] * (max_height_of_a_char+(border_size * 2))) + 9;
    if (grid_column_heights[i] > grid_height_of_highest_column_in_pixels)
    {
      grid_height_of_highest_column_in_pixels= grid_column_heights[i];
    }
  }
}


int scroll_event()
{
  int new_grid_vertical_scroll_bar_value;

  new_grid_vertical_scroll_bar_value= grid_vertical_scroll_bar->value();
  if (new_grid_vertical_scroll_bar_value != grid_vertical_scroll_bar_value)
  {
    grid_vertical_scroll_bar_value= new_grid_vertical_scroll_bar_value;
    fill_detail_widgets();
    return false;                        /* Of course, we don't want the scrolling to go on. */
  }
  return false;
}


/*
  Put values in the detail widgets,
  i.e. all QTextEdit widgets except row[0] which is header.
  Todo: If we've scrolled to near the end, don't fill. Though we shouldn't have to hide!
*/
/* The big problem is that setVerticalSpacing(0) goes awry if I use hide(). */
void fill_detail_widgets()
{
  unsigned int i;
  QString yy;

  mysql_data_seek(grid_mysql_res, grid_vertical_scroll_bar_value);
  for (r= 0; r < grid_actual_grid_height_in_rows - 1; ++r)
  {
    row= mysql_fetch_row(grid_mysql_res);
    if (row == 0)
    {
      printf("Pseudo-assertion: mysql_fetch_row failed!\n");
      /*
        Todo: Originally I was saying hide() here, but that led to problems.
        Now the problem is the blank rows at the end.
        you'll have to show these widgets again if scroll-up happens
      */
      yy= "";
      for (i= 0; i < result_column_count; ++i)
      {
        text_edit_widgets[(r + 1) * result_column_count + i]->setPlainText(yy);
        text_edit_frames[(r + 1) * result_column_count + i]->hide();
      }
    }
    else
    {
      lengths= mysql_fetch_lengths(grid_mysql_res); /* Todo: lengths isn't used */
      for (i= 0; i < result_column_count; ++i)
      {
        /* if (lengths[i]>result_max_column_lengths[i]) result_max_column_lengths[i]= lengths[i]; */
        yy= row[i];
        text_edit_widgets[(r + 1) * result_column_count + i]->setPlainText(yy);
      text_edit_frames[(r + 1) * result_column_count + i]->show();
      }
    }
  }
}


/* Todo: bug: when I switched to Kacst Poster 28pt bold italic, I got scroll bars -- calculations 1 pixel off? */
int fontchange_event()
{
  remove_layouts();
  QFont tmp_font=this->font();
  put_widgets_in_layouts(&tmp_font);
  return 0;
}


/*
  Remove all frame widgets from layouts, then delete layouts.
  We'll make them again by calling put_widgets_in_layouts() immediately after this.
  We are going to delete the widgets, we are going to put them all back.
  And if anybody is wondering "why delete and remake?", it's because I had an
  insoluble problem when I was merely scrolling and changing QTextEdit contents.
*/
void remove_layouts()
{
  long unsigned int xrow;
  unsigned int col;

  client->hide(); /* client->show() will happen again soon */

  if (grid_main_layout != 0)
  {
    for (xrow= 0; xrow < grid_actual_grid_height_in_rows; ++xrow)
    {
      for (col= 0; col < result_column_count; ++col)
      {
        int ki= xrow * result_column_count + col;
        text_edit_layouts[ki]->removeWidget(text_edit_widgets[ki]);
        grid_row_layouts[xrow]->removeWidget(text_edit_frames[ki]);
        delete text_edit_widgets[ki];
        delete text_edit_layouts[ki];
        delete text_edit_frames[ki];
      }
      grid_main_layout->removeWidget(grid_row_widgets[xrow]);
      delete grid_row_layouts[xrow];
      delete grid_row_widgets[xrow];
    }
    /* Todo: fix: grid_main_layout is still the layout of client */
  }
  /* Todo: check whether to delete grid_scroll_area and grid_vertical_scroll_bar -- but I think garbage_collect() does that. */
  if (hbox_layout != 0)
  {
    hbox_layout->removeWidget(grid_scroll_area);
    hbox_layout->removeWidget(grid_vertical_scroll_bar);
  }
  /* Todo: think whether it would be best to set grid_layout=0 after deleting grid_layout, etc. */
  /* if (grid_layout != 0) delete grid_layout; */
  if (hbox_layout != 0) delete hbox_layout;
  if (grid_main_layout != 0) delete grid_main_layout;
}


/*
  We'll do our own garbage collecting for non-Qt items.
  Todo: make sure Qt items have parents where possible so that "delete result_grid_table_widget"
        takes care of them. But we delete text_edit_widgets+layouts+frames in remove_layouts too.
*/
void garbage_collect()
{
  if (text_edit_widgets != 0)
  {
    remove_layouts();
    delete [] text_edit_frames;
    delete [] text_edit_layouts;
    delete [] text_edit_widgets;
  }
  if (grid_row_widgets != 0)
  {
    delete [] grid_row_widgets;
    delete [] grid_row_layouts;
  }
  if (grid_column_widths != 0) delete [] grid_column_widths;
  if (grid_column_heights != 0) delete [] grid_column_heights;

  if (grid_vertical_scroll_bar != 0) delete grid_vertical_scroll_bar;
  if (grid_scroll_area != 0) delete grid_scroll_area;
}


/*
  If (row height * result_row_count) > ResultGrid widget height) we need a vertical scroll bar.
  Todo: I'm not sure whether this is adequate if there's a horizontal scroll bar.
  Todo: I'm not sure whether this is adequate if there's a widget header.
  Todo: I'm not sure whether the calculation should involve result_row_count  + 1 (to include the header).
  Todo: Call this not only when created, but also if font change, column/row size change, resize.
  Todo: Look for a bug! I noticed that vertical scroll bar was missing after a font change. Didn't repeat.
*/
void vertical_bar_show_as_needed()
{
  int h;

  h= 0;
  if (result_row_count > 1) h+= grid_height_of_highest_column_in_pixels * result_row_count;

  if (h > height()) grid_vertical_scroll_bar->show();
  else  grid_vertical_scroll_bar->hide();
}

public slots:
private:
};

/*********************************************************************************************************/

/* The Statement Widget */

/*
  A small part of the code for the statement widget was copied from
  http://qt-project.org/doc/qt-4.8/widgets-codeeditor-codeeditor-h.html
  so the original copyright notice and BSD-license provisions are reproduced
  at the start of ocelotgui.cpp, applicable solely to what was copied.
*/

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class prompt_class;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent= 0);

    void prompt_widget_paintevent(QPaintEvent *event);
    int prompt_width_calculate();
    QString prompt_translate(int line_number);
    unsigned int statement_count;                                            /* used if "prompt \c ..." */
    QString dbms_version; /* Set to "" at start, select version() at connect, maybe display in prompt. */
    QString dbms_database;/* Set to "" at start, select database() at connect, maybe display in prompt. */
    QString dbms_port;/* Set to "" at start, select @@port at connect, maybe display in prompt. */
    QString dbms_current_user;/* Set to "" at start, select current_user() at connect, maybe display in prompt. */
    QString dbms_current_user_without_host;/* Set to "" at start, select current_user() at connect, maybe display in prompt. */
    QString dbms_host; /* Set to "" at start, mysql_get_host_info() at connect, maybe display in prompt. */
    QString delimiter; /* Set to ";" at start, can be changed with "delimiter //" etc. */
    QString result; /* What gets appended to history after statement executeion, e.g. error message */
    qint64 start_time; /* when statement started, in milliseconds since the epoch */

    /*
      The prompt is a read-only widget that
      appears on the left margin of statement_edit_widget.
      Mostly it's controlled by the "PROMPT" command.
      Font is same as main font for statement_edit_widget.
      It looks better if the font is fixed width.
      There is an assumption that prompts are latin1.
      PROMPT client statement decides whethere there are line numbers.
    */

        QColor statement_edit_widget_left_bgcolor;                    /* suggestion = Qt::lightGray. */
        QColor statement_edit_widget_left_treatment1_textcolor;       /* suggestion = Qt::black */
        QString statement_edit_widget_left_treatment1_prompt_text;    /* suggestions = "     >" or "" */

        QString prompt_default;                     /* = "mysql" -- or is it "\N [\d]>"? */
        QString prompt_as_input_by_user;            /* = What the user input with latest PROMPT statement, or prompt_default */
        /* QString prompt_translated;     */             /* = prompt_as_input_by_user, but with some \s converted for ease of parse */
        QString prompt_current;                     /* = latest result of prompt_reform() */

public slots:
    void update_prompt_width(int newBlockCount);

protected:
    void resizeEvent(QResizeEvent *event);

private slots:

    void highlightCurrentLine();
    void update_prompt(const QRect &, int);

private:
    QWidget *prompt_widget;
};

class prompt_class : public QWidget
{
public:
  prompt_class(CodeEditor *editor) : QWidget(editor)
  {
    codeEditor= editor;
  }

  QSize sizeHint() const
  {
    return QSize(codeEditor->prompt_width_calculate(), 0);
  }

protected:
  void paintEvent(QPaintEvent *event)
  {
    codeEditor->prompt_widget_paintevent(event);
  }

private:
  CodeEditor *codeEditor;
};

#endif

/*********************************************************************************************************/
/* THE ROW_FORM_BOX WIDGET */

/*
  Todo: Currently this is only used for connecting.
        But it should be very easy to use with result-table rows,
        either for vertical display or in response to a keystroke on ResultGrid.
*/

/*
  Todo:

  settle minimum width of dialog box
  settle maximum width of each item

  If user hits Enter, that should get Cancel or OK working.

  error text= "password must be entered" | as returned from last attempt | nothing (File|Connect)
  invoke: at start / when user types CONNECT / When File|Connect ... no auto-restart
  Actually call myoptions_and_connect()!
  Re-order the items?
  Would QTextEdit be nicer? You'd have horizontal scroll bars if necessary. However, a password would still be QLineEdit
                            Right ... QTextEdit, and height = 2 lines if otherwise there would be overflow
  Use row_form_width[]!
  Think again about how tabs work: setTabChangesFocus(true) works, but now it's hard to put a tab in except with escaping.
  Make spacing between widgets smaller: see comments about "no effect" below.
*/

class Row_form_box: public QDialog
{
  Q_OBJECT
public:
  QDialog *row_form_box;
  bool is_ok;

private:
  QLabel *label[9];
  QLineEdit *line_edit[9];
  QTextEdit *text_edit[9];
  QHBoxLayout *hbox_layout[9];
  QWidget *widget[9];
  QPushButton *button_for_cancel, *button_for_ok;
  QHBoxLayout *hbox_layout_for_ok_and_cancel;
  QVBoxLayout *main_layout;
  QWidget *widget_for_ok_and_cancel;
  MainWindow *copy_of_parent;
  QLabel *label_for_message;

public:
Row_form_box(QFont *font, MainWindow *parent): QDialog(parent)
{
  int i;

  copy_of_parent= parent;
  is_ok= 0;
  QFontMetrics mm= QFontMetrics(*font);
  //unsigned int max_width_of_a_char= mm.width("W");                 /* not really a maximum unless fixed-width font */
  unsigned int max_height_of_a_char= mm.lineSpacing();             /* Actually this is mm.height() + mm.leading(). */
  main_layout= new QVBoxLayout();
  main_layout->setSpacing(0); /* Todo: check why this doesn't seem to have any effect */
  main_layout->setContentsMargins(0, 0, 0, 0); /* Todo: check why this doesn't seem to have any effect */
  main_layout->setSizeConstraint(QLayout::SetFixedSize);  /* no effect */
  label_for_message= new QLabel(parent->row_form_message);
  main_layout->addWidget(label_for_message);
  /* Todo: watch row_form_type maybe it's NUM_FLAG */
  this->setMinimumWidth(300);
  for (i= 0; i < 8; ++i)
  {
    hbox_layout[i]= new QHBoxLayout();
    label[i]= new QLabel();
    label[i]->setStyleSheet(parent->ocelot_grid_header_style_string);
    label[i]->setText(parent->row_form_label[i]);
    hbox_layout[i]->addWidget(label[i]);
    if (parent->row_form_is_password[i] == 1)
    {
      line_edit[i]= new QLineEdit();
      line_edit[i]->setStyleSheet(parent->ocelot_grid_style_string);
      line_edit[i]->insert(parent->row_form_data[i]);
      line_edit[i]->setEchoMode(QLineEdit::Password); /* maybe PasswordEchoOnEdit would be better */
      line_edit[i]->setMinimumHeight(2 * max_height_of_a_char + 3); /* no effect */
      line_edit[i]->setMaximumHeight(2 * max_height_of_a_char + 3); /* no effect */
      hbox_layout[i]->addWidget(line_edit[i]);
    }
    else
    {
      text_edit[i]= new QTextEdit();
      text_edit[i]->setStyleSheet(parent->ocelot_grid_style_string);
      text_edit[i]->setText(parent->row_form_data[i]);
      //text_edit[i]->setMinimumHeight(max_height_of_a_char + 3); /* no effect */
      text_edit[i]->setMaximumHeight(2 * max_height_of_a_char + 3); /* bizarre effect */
      //text_edit[i]->updateGeometry(); /* no effect */
      text_edit[i]->setTabChangesFocus(true);
      hbox_layout[i]->addWidget(text_edit[i]);
    }
    widget[i]= new QWidget();
    widget[i]->setLayout(hbox_layout[i]);
    main_layout->addWidget(widget[i]);
  }
  button_for_cancel= new QPushButton(tr("Cancel"), this);
  button_for_ok= new QPushButton(tr("OK"), this);
  hbox_layout_for_ok_and_cancel= new QHBoxLayout();
  hbox_layout_for_ok_and_cancel->addWidget(button_for_cancel);
  hbox_layout_for_ok_and_cancel->addWidget(button_for_ok);
  widget_for_ok_and_cancel= new QWidget();
  widget_for_ok_and_cancel->setLayout(hbox_layout_for_ok_and_cancel);
  connect(button_for_ok, SIGNAL(clicked()), this, SLOT(handle_button_for_ok()));
  connect(button_for_cancel, SIGNAL(clicked()), this, SLOT(handle_button_for_cancel()));
  main_layout->addWidget(widget_for_ok_and_cancel);
  this->setLayout(main_layout);
  this->setWindowTitle(parent->row_form_title);
}

private slots:

void handle_button_for_ok()
{
 for (int i= 0; i < 8; ++i)
 {
   if (copy_of_parent->row_form_is_password[i] == 1) copy_of_parent->row_form_data[i]= line_edit[i]->text();
   else copy_of_parent->row_form_data[i]= text_edit[i]->toPlainText();
 }
 is_ok= 1;
 close();
}


void handle_button_for_cancel()
{
  close();
}

};


/*********************************************************************************************************/
/* THE SETTINGS WIDGET */

class Settings: public QDialog
{
  Q_OBJECT
public:
  QDialog *settings;
/*
private slots:
  void handle_combo_box_1(int);
  void handle_button_for_color_pick_0();
  void handle_button_for_color_pick_1();
  void handle_button_for_color_pick_2();
  void handle_button_for_color_pick_3();
  void handle_button_for_color_pick_4();
  void handle_button_for_color_pick_5();
  void handle_button_for_color_pick_6();
  void handle_button_for_color_pick_7();
  void handle_button_for_color_pick_8();
*/

public:

  QVBoxLayout *main_layout;
  QWidget *widget_3, *widget_for_font_dialog;
  QHBoxLayout *hbox_layout_3, *hbox_layout_for_font_dialog;
  QPushButton *button_for_cancel, *button_for_ok, *button_for_font_dialog;
  QLabel *widget_colors_label, *widget_font_label;

  QWidget *widget_for_color[9];
  QHBoxLayout *hbox_layout_for_color[9];
  QLabel *label_for_color[9];
  QLabel *label_for_color_rgb[9];
  QLabel *label_for_font_dialog;

  QWidget *widget_for_size[3];
  QHBoxLayout *hbox_layout_for_size[3];
  QLabel *label_for_size[3];
  QComboBox *combo_box_for_size[3];

  QPushButton *button_for_color_pick[9];
  QPushButton *button_for_color_show[9];
  MainWindow *copy_of_parent;

  QString new_ocelot_statement_color;
  QString new_ocelot_statement_background_color;
  QString new_ocelot_statement_border_color;
  QString new_ocelot_statement_font_family;
  QString new_ocelot_statement_font_size;
  QString new_ocelot_statement_font_style;
  QString new_ocelot_statement_font_weight;
  QString new_ocelot_statement_highlight_literal_color;
  QString new_ocelot_statement_highlight_identifier_color;
  QString new_ocelot_statement_highlight_comment_color;
  QString new_ocelot_statement_highlight_operator_color;
  QString new_ocelot_statement_highlight_reserved_color;
  QString new_ocelot_statement_prompt_background_color;
  QString new_ocelot_grid_color;
  QString new_ocelot_grid_background_color;
  QString new_ocelot_grid_border_color;
  QString new_ocelot_grid_header_background_color;
  QString new_ocelot_grid_font_family;
  QString new_ocelot_grid_font_size;
  QString new_ocelot_grid_font_style;
  QString new_ocelot_grid_font_weight;
  QString new_ocelot_grid_cell_border_color;
  QString new_ocelot_grid_cell_right_drag_line_color;
  QString new_ocelot_grid_border_size;
  QString new_ocelot_grid_cell_border_size;
  QString new_ocelot_grid_cell_right_drag_line_size;
  QString new_ocelot_history_color;
  QString new_ocelot_history_background_color;
  QString new_ocelot_history_border_color;
  QString new_ocelot_history_font_family;
  QString new_ocelot_history_font_size;
  QString new_ocelot_history_font_style;
  QString new_ocelot_history_font_weight;
  QString new_ocelot_main_color;
  QString new_ocelot_main_background_color;
  QString new_ocelot_main_border_color;
  QString new_ocelot_main_font_family;
  QString new_ocelot_main_font_size;
  QString new_ocelot_main_font_style;
  QString new_ocelot_main_font_weight;

  int current_widget; /* 0 = statement, 1 = grid */

public:
Settings(int passed_widget_number, MainWindow *parent): QDialog(parent)
{

  /* settings = new QWidget(this); ... this might come later */

  copy_of_parent= parent;                                /* handle_button_for_ok() might use this */
  current_widget= passed_widget_number;

  /* Copy the parent's settings. They'll be copied back to the parent, possibly changed, if the user presses OK. */
  new_ocelot_statement_color= copy_of_parent->ocelot_statement_color;
  new_ocelot_statement_background_color= copy_of_parent->ocelot_statement_background_color;
  new_ocelot_statement_border_color= copy_of_parent->ocelot_statement_border_color;
  new_ocelot_statement_font_family= copy_of_parent->ocelot_statement_font_family;
  new_ocelot_statement_font_size= copy_of_parent->ocelot_statement_font_size;
  new_ocelot_statement_font_style= copy_of_parent->ocelot_statement_font_style;
  new_ocelot_statement_font_weight= copy_of_parent->ocelot_statement_font_weight;
  new_ocelot_statement_highlight_literal_color= copy_of_parent->ocelot_statement_highlight_literal_color;
  new_ocelot_statement_highlight_identifier_color= copy_of_parent->ocelot_statement_highlight_identifier_color;
  new_ocelot_statement_highlight_comment_color= copy_of_parent->ocelot_statement_highlight_comment_color;
  new_ocelot_statement_highlight_operator_color= copy_of_parent->ocelot_statement_highlight_operator_color;
  new_ocelot_statement_highlight_reserved_color= copy_of_parent->ocelot_statement_highlight_reserved_color;
  new_ocelot_statement_prompt_background_color= copy_of_parent->ocelot_statement_prompt_background_color;

  new_ocelot_grid_color= copy_of_parent->ocelot_grid_color;
  new_ocelot_grid_background_color= copy_of_parent->ocelot_grid_background_color;
  new_ocelot_grid_border_color= copy_of_parent->ocelot_grid_border_color;
  new_ocelot_grid_header_background_color= copy_of_parent->ocelot_grid_header_background_color;
  new_ocelot_grid_font_family= copy_of_parent->ocelot_grid_font_family;
  new_ocelot_grid_font_size= copy_of_parent->ocelot_grid_font_size;
  new_ocelot_grid_font_style= copy_of_parent->ocelot_grid_font_style;
  new_ocelot_grid_font_weight= copy_of_parent->ocelot_grid_font_weight;
  new_ocelot_grid_cell_border_color= copy_of_parent->ocelot_grid_cell_border_color;
  new_ocelot_grid_cell_right_drag_line_color= copy_of_parent->ocelot_grid_cell_right_drag_line_color;
  new_ocelot_grid_border_size= copy_of_parent->ocelot_grid_border_size;
  new_ocelot_grid_cell_border_size= copy_of_parent->ocelot_grid_cell_border_size;
  new_ocelot_grid_cell_right_drag_line_size= copy_of_parent->ocelot_grid_cell_right_drag_line_size;
  new_ocelot_history_color= copy_of_parent->ocelot_history_color;
  new_ocelot_history_background_color= copy_of_parent->ocelot_history_background_color;
  new_ocelot_history_border_color= copy_of_parent->ocelot_history_border_color;
  new_ocelot_history_font_family= copy_of_parent->ocelot_history_font_family;
  new_ocelot_history_font_size= copy_of_parent->ocelot_history_font_size;
  new_ocelot_history_font_style= copy_of_parent->ocelot_history_font_style;
  new_ocelot_history_font_weight= copy_of_parent->ocelot_history_font_weight;

  new_ocelot_main_color= copy_of_parent->ocelot_main_color;
  new_ocelot_main_background_color= copy_of_parent->ocelot_main_background_color;
  new_ocelot_main_border_color= copy_of_parent->ocelot_main_border_color;
  new_ocelot_main_font_family= copy_of_parent->ocelot_main_font_family;
  new_ocelot_main_font_size= copy_of_parent->ocelot_main_font_size;
  new_ocelot_main_font_style= copy_of_parent->ocelot_main_font_style;
  new_ocelot_main_font_weight= copy_of_parent->ocelot_main_font_weight;

  setWindowTitle(tr("Settings -- Colors and Fonts"));                        /* affects "this"] */

  widget_colors_label= new QLabel(tr("Colors"));

  /* Hboxes for foreground, background, and highlights */
  /* Todo: following calculation should actually be width of largest tr(label) + approximately 5. */
  int label_for_color_width= this->fontMetrics().boundingRect("W").width();
  for (int ci= 0; ci < 9; ++ci)
  {
    widget_for_color[ci]= new QWidget(this);
    label_for_color[ci]= new QLabel();
    label_for_color_rgb[ci]= new QLabel();
    button_for_color_pick[ci]= new QPushButton(tr("Pick new color"), this);
    button_for_color_show[ci]= new QPushButton("", this);
    set_widget_values(ci);
    label_for_color[ci]->setFixedWidth(label_for_color_width * 20);
    label_for_color_rgb[ci]->setFixedWidth(label_for_color_width * 8);
    button_for_color_show[ci]->setEnabled(false);
    hbox_layout_for_color[ci]= new QHBoxLayout();
    hbox_layout_for_color[ci]->addWidget(label_for_color[ci]);
    hbox_layout_for_color[ci]->addWidget(label_for_color_rgb[ci]);
    hbox_layout_for_color[ci]->addWidget(button_for_color_show[ci]);
    hbox_layout_for_color[ci]->addWidget(button_for_color_pick[ci]);
    widget_for_color[ci]->setLayout(hbox_layout_for_color[ci]);
  }
  connect(button_for_color_pick[0], SIGNAL(clicked()), this, SLOT(handle_button_for_color_pick_0()));
  connect(button_for_color_pick[1], SIGNAL(clicked()), this, SLOT(handle_button_for_color_pick_1()));
  connect(button_for_color_pick[2], SIGNAL(clicked()), this, SLOT(handle_button_for_color_pick_2()));
  connect(button_for_color_pick[3], SIGNAL(clicked()), this, SLOT(handle_button_for_color_pick_3()));
  connect(button_for_color_pick[4], SIGNAL(clicked()), this, SLOT(handle_button_for_color_pick_4()));
  connect(button_for_color_pick[5], SIGNAL(clicked()), this, SLOT(handle_button_for_color_pick_5()));
  connect(button_for_color_pick[6], SIGNAL(clicked()), this, SLOT(handle_button_for_color_pick_6()));
  connect(button_for_color_pick[7], SIGNAL(clicked()), this, SLOT(handle_button_for_color_pick_7()));
  connect(button_for_color_pick[8], SIGNAL(clicked()), this, SLOT(handle_button_for_color_pick_8()));

  widget_font_label= new QLabel(tr("Font"));

  /* Hbox -- the font picker */

  widget_for_font_dialog= new QWidget(this);

  label_for_font_dialog= new QLabel(this);
  label_for_font_dialog_set_text();

  button_for_font_dialog= new QPushButton(tr("Pick new font"), this);
  hbox_layout_for_font_dialog= new QHBoxLayout();
  hbox_layout_for_font_dialog->addWidget(label_for_font_dialog);
  hbox_layout_for_font_dialog->addWidget(button_for_font_dialog);
  widget_for_font_dialog->setLayout(hbox_layout_for_font_dialog);
  connect(button_for_font_dialog, SIGNAL(clicked()), this, SLOT(handle_button_for_font_dialog()));

  if (current_widget == 1)
  {
    /* int label_for_size_width= this->fontMetrics().boundingRect("W").width(); */
    for (int ci= 0; ci < 3; ++ci)
    {
      widget_for_size[ci]= new QWidget(this);
      if (ci == 0) label_for_size[ci]= new QLabel(tr("Border size"));
      if (ci == 1) label_for_size[ci]= new QLabel(tr("Cell Border size"));
      if (ci == 2) label_for_size[ci]= new QLabel(tr("Cell right drag line size"));
      combo_box_for_size[ci]= new QComboBox();
      combo_box_for_size[ci]->setFixedWidth(label_for_color_width * 3);
      for (int cj= 0; cj < 10; ++cj) combo_box_for_size[ci]->addItem(QString::number(cj));
      label_for_size[ci]->setFixedWidth(label_for_color_width * 20);
      if (ci == 0) combo_box_for_size[0]->setCurrentIndex(new_ocelot_grid_border_size.toInt());
      if (ci == 1) combo_box_for_size[1]->setCurrentIndex(new_ocelot_grid_cell_border_size.toInt());
      if (ci == 2) combo_box_for_size[2]->setCurrentIndex(new_ocelot_grid_cell_right_drag_line_size.toInt());
      hbox_layout_for_size[ci]= new QHBoxLayout();
      hbox_layout_for_size[ci]->addWidget(label_for_size[ci]);
      hbox_layout_for_size[ci]->addWidget(combo_box_for_size[ci]);
    widget_for_size[ci]->setLayout(hbox_layout_for_size[ci]);
    }
    connect(combo_box_for_size[0], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_size_0(int)));
    connect(combo_box_for_size[1], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_size_1(int)));
    connect(combo_box_for_size[2], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_size_2(int)));
  }

  /* The Cancel and OK buttons */
  widget_3= new QWidget(this);
  button_for_cancel= new QPushButton(tr("Cancel"), this);
  button_for_ok= new QPushButton(tr("OK"), this);
  /* I have no idea why SLOT(accept() and SLOT(reject() cause crashes. The crashes can be worked around. */
  connect(button_for_ok, SIGNAL(clicked()), this, SLOT(handle_button_for_ok()));
  connect(button_for_cancel, SIGNAL(clicked()), this, SLOT(handle_button_for_cancel()));
  hbox_layout_3= new QHBoxLayout();
  hbox_layout_3->addWidget(button_for_cancel);
  hbox_layout_3->addWidget(button_for_ok);
  widget_3->setLayout(hbox_layout_3);

  /* Put the HBoxes in a VBox */
  main_layout= new QVBoxLayout();
  main_layout->addWidget(widget_colors_label);
  for (int ci= 0; ci < 9; ++ci) main_layout->addWidget(widget_for_color[ci]);
  main_layout->addWidget(widget_font_label);
  main_layout->addWidget(widget_for_font_dialog);
  if (current_widget == 1) for (int ci= 0; ci < 3; ++ci) main_layout->addWidget(widget_for_size[ci]);
  main_layout->addWidget(widget_3);

  handle_combo_box_1(current_widget);
  this->setLayout(main_layout);
}


private:

/* We call set_widget_values() when we're doing "new Settings" (in which case current_widget == what-was-passed) */
void set_widget_values(int ci)
{
  QString color_type;
  QString color_name;
  if (current_widget == 0)
  {
    switch (ci)
    {
    case 0: { color_type= tr("Foreground"); color_name= new_ocelot_statement_color; break; }
    case 1: { color_type= tr("Background"); color_name= new_ocelot_statement_background_color; break; }
    case 2: { color_type= tr("Highlight literal"); color_name= new_ocelot_statement_highlight_literal_color; break; }
    case 3: { color_type= tr("Highlight identifier"); color_name= new_ocelot_statement_highlight_identifier_color; break; }
    case 4: { color_type= tr("Highlight comment"); color_name= new_ocelot_statement_highlight_comment_color; break; }
    case 5: { color_type= tr("Highlight operator"); color_name= new_ocelot_statement_highlight_operator_color; break; }
    case 6: { color_type= tr("Highlight reserved"); color_name= new_ocelot_statement_highlight_reserved_color; break; }
    case 7: { color_type= tr("Prompt background"); color_name= new_ocelot_statement_prompt_background_color; break; }
    case 8: { color_type= tr("Border"); color_name= new_ocelot_statement_border_color; break; }
    }
  }
  if (current_widget == 1)
  {
    switch (ci)
    {
    case 0: { color_type= tr("Foreground"); color_name= new_ocelot_grid_color; break; }
    case 1: { color_type= tr("Background"); color_name= new_ocelot_grid_background_color; break; }
    case 2: { color_type= tr("Cell border"); color_name= new_ocelot_grid_cell_border_color; break; }
    case 3: { color_type= tr("Cell right drag line"); color_name= new_ocelot_grid_cell_right_drag_line_color; break; }
    case 7: { color_type= tr("Header background"); color_name= new_ocelot_grid_header_background_color; break; }
    case 8: { color_type= tr("Border"); color_name= new_ocelot_grid_border_color; break; }
    }
  }
  if (current_widget == 2)
  {
    switch (ci)
    {
    case 0: { color_type= tr("Foreground"); color_name= new_ocelot_history_color; break; }
    case 1: { color_type= tr("Background"); color_name= new_ocelot_history_background_color; break; }
    case 8: { color_type= tr("Border"); color_name= new_ocelot_history_border_color; break; }
    }
  }
  if (current_widget == 3)
  {
    switch (ci)
    {
    case 0: { color_type= tr("Foreground"); color_name= new_ocelot_main_color; break; }
    case 1: { color_type= tr("Background"); color_name= new_ocelot_main_background_color; break; }
    case 8: { color_type= tr("Border"); color_name= new_ocelot_main_border_color; break; }
    }
  }
  label_for_color[ci]->setText(color_type);
  label_for_color_rgb[ci]->setText(color_name);
  QString sss= "background-color: ";
  sss.append(color_name);
  button_for_color_show[ci]->setStyleSheet(sss);
}


private:
/*
  Todo: Only statement has highlights (I suppose the other controls
  could have highlights too, but regard that as a low priority todo).
  I tried setEnabled(false|true) but think hide|show looks better.
*/
void handle_combo_box_1(int i)
{
  int ci;
  QString color_type;

  current_widget= i;

  for (ci= 0; ci <= 8; ++ci) set_widget_values(ci);
  if (i == 0)
  {                                       /* statement? */
    color_type= "Prompt background";               /* ?? unnecessary now that set_widget_values() does it, eh? */
    label_for_color[7]->setText(color_type);
    for (ci= 2; ci < 8 ; ++ci)
    {
      label_for_color[ci]->show();
      label_for_color_rgb[ci]->show();
      button_for_color_show[ci]->show();
      button_for_color_pick[ci]->show();
    }
  }

  if (i == 1)                                       /* grid? */
  {
    color_type= "Header background";               /* ?? unnecessary now that set_widget_values() does it, eh? */
    label_for_color[7]->setText(color_type);
    for (ci= 4; ci < 7; ++ci)
    {
      label_for_color[ci]->hide();
      label_for_color_rgb[ci]->hide();
      button_for_color_show[ci]->hide();
      button_for_color_pick[ci]->hide();
    }
    for (ci= 7; ci < 8; ++ci)
    {
      label_for_color[ci]->show();
      label_for_color_rgb[ci]->show();
      button_for_color_show[ci]->show();
      button_for_color_pick[ci]->show();
    }
  }
  if (i > 1)
  {
    for (ci= 2; ci < 8; ++ci)
    {
      label_for_color[ci]->hide();
      label_for_color_rgb[ci]->hide();
      button_for_color_show[ci]->hide();
      button_for_color_pick[ci]->hide();
    }
  }
}


void label_for_font_dialog_set_text()
{
  QString s_for_label_for_font_dialog;
  if (current_widget == 0)
  {
    s_for_label_for_font_dialog= new_ocelot_statement_font_family;
    s_for_label_for_font_dialog.append(" ");
    s_for_label_for_font_dialog.append(new_ocelot_statement_font_size);
    s_for_label_for_font_dialog.append("pt");
    if (QString::compare(new_ocelot_statement_font_weight, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(new_ocelot_statement_font_weight);
    }
    if (QString::compare(new_ocelot_statement_font_style, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(new_ocelot_statement_font_style);
    }
  }

  if (current_widget == 1)
  {
    s_for_label_for_font_dialog= new_ocelot_grid_font_family;
    s_for_label_for_font_dialog.append(" ");
    s_for_label_for_font_dialog.append(new_ocelot_grid_font_size);
    s_for_label_for_font_dialog.append("pt");
    if (QString::compare(new_ocelot_grid_font_weight, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(new_ocelot_grid_font_weight);
    }
    if (QString::compare(new_ocelot_grid_font_style, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(new_ocelot_grid_font_style);
    }
  }

  if (current_widget == 2)
  {
    s_for_label_for_font_dialog= new_ocelot_history_font_family;
    s_for_label_for_font_dialog.append(" ");
    s_for_label_for_font_dialog.append(new_ocelot_history_font_size);
    s_for_label_for_font_dialog.append("pt");
    if (QString::compare(new_ocelot_history_font_weight, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(new_ocelot_history_font_weight);
    }
    if (QString::compare(new_ocelot_history_font_style, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(new_ocelot_history_font_style);
    }
  }

  if (current_widget == 3)
  {
    s_for_label_for_font_dialog= new_ocelot_main_font_family;
    s_for_label_for_font_dialog.append(" ");
    s_for_label_for_font_dialog.append(new_ocelot_main_font_size);
    s_for_label_for_font_dialog.append("pt");
    if (QString::compare(new_ocelot_main_font_weight, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(new_ocelot_main_font_weight);
    }
    if (QString::compare(new_ocelot_main_font_style, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(new_ocelot_main_font_style);
    }
  }

  label_for_font_dialog->setText(s_for_label_for_font_dialog);
}


private slots:

/* If user clicks OK, move all "new" settings to parent, and end. */
void handle_button_for_ok()
{
  copy_of_parent->ocelot_statement_color= new_ocelot_statement_color;
  copy_of_parent->ocelot_statement_background_color= new_ocelot_statement_background_color;
  copy_of_parent->ocelot_statement_border_color= new_ocelot_statement_border_color;
  copy_of_parent->ocelot_statement_font_family= new_ocelot_statement_font_family;
  copy_of_parent->ocelot_statement_font_size= new_ocelot_statement_font_size;
  copy_of_parent->ocelot_statement_font_style= new_ocelot_statement_font_style;
  copy_of_parent->ocelot_statement_font_weight= new_ocelot_statement_font_weight;
  copy_of_parent->ocelot_statement_highlight_literal_color= new_ocelot_statement_highlight_literal_color;
  copy_of_parent->ocelot_statement_highlight_identifier_color= new_ocelot_statement_highlight_identifier_color;
  copy_of_parent->ocelot_statement_highlight_comment_color= new_ocelot_statement_highlight_comment_color;
  copy_of_parent->ocelot_statement_highlight_operator_color= new_ocelot_statement_highlight_operator_color;
  copy_of_parent->ocelot_statement_highlight_reserved_color= new_ocelot_statement_highlight_reserved_color;
  copy_of_parent->ocelot_statement_prompt_background_color= new_ocelot_statement_prompt_background_color;

  copy_of_parent->ocelot_grid_color= new_ocelot_grid_color;
  copy_of_parent->ocelot_grid_background_color= new_ocelot_grid_background_color;
  copy_of_parent->ocelot_grid_border_color= new_ocelot_grid_border_color;
  copy_of_parent->ocelot_grid_header_background_color= new_ocelot_grid_header_background_color;
  copy_of_parent->ocelot_grid_font_family= new_ocelot_grid_font_family;
  copy_of_parent->ocelot_grid_font_size= new_ocelot_grid_font_size;
  copy_of_parent->ocelot_grid_font_style= new_ocelot_grid_font_style;
  copy_of_parent->ocelot_grid_font_weight= new_ocelot_grid_font_weight;
  copy_of_parent->ocelot_grid_cell_border_color= new_ocelot_grid_cell_border_color;
  copy_of_parent->ocelot_grid_cell_right_drag_line_color= new_ocelot_grid_cell_right_drag_line_color;
  copy_of_parent->ocelot_grid_border_size= new_ocelot_grid_border_size;
  copy_of_parent->ocelot_grid_cell_border_size= new_ocelot_grid_cell_border_size;
  copy_of_parent->ocelot_grid_cell_right_drag_line_size= new_ocelot_grid_cell_right_drag_line_size;

  copy_of_parent->ocelot_history_color= new_ocelot_history_color;
  copy_of_parent->ocelot_history_background_color= new_ocelot_history_background_color;
  copy_of_parent->ocelot_history_border_color= new_ocelot_history_border_color;
  copy_of_parent->ocelot_history_font_family= new_ocelot_history_font_family;
  copy_of_parent->ocelot_history_font_size= new_ocelot_history_font_size;
  copy_of_parent->ocelot_history_font_style= new_ocelot_history_font_style;
  copy_of_parent->ocelot_history_font_weight= new_ocelot_history_font_weight;

  copy_of_parent->ocelot_main_color= new_ocelot_main_color;
  copy_of_parent->ocelot_main_background_color= new_ocelot_main_background_color;
  copy_of_parent->ocelot_main_border_color= new_ocelot_main_border_color;
  copy_of_parent->ocelot_main_font_family= new_ocelot_main_font_family;
  copy_of_parent->ocelot_main_font_size= new_ocelot_main_font_size;
  copy_of_parent->ocelot_main_font_style= new_ocelot_main_font_style;
  copy_of_parent->ocelot_main_font_weight= new_ocelot_main_font_weight;

  close();
}


/* If user clicks Cancel, don't do anything, and end. */
void handle_button_for_cancel()
{
  close();
}


/*
  For all the handle_button_for_color_pick slots:
  If the user clicks on the button, a dialog box comes up with title hinting what section is being changed.
  If the user then clicks Cancel, isValid() will fail so do nothing.
  If the user then clicks OK, is Valid() will succeed so change the color.
*/
void handle_button_for_color_pick_0()
{
  if (current_widget == 0)
  {
    QColor curr_color= QColor(new_ocelot_statement_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Foreground"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_statement_color= curr_color.name();
      label_for_color_rgb[0]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_statement_color);
      button_for_color_show[0]->setStyleSheet(s);
    }
  }
  if (current_widget == 1)
  {
    QColor curr_color= QColor(new_ocelot_grid_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Foreground"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_grid_color= curr_color.name();
      label_for_color_rgb[0]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_grid_color);
      button_for_color_show[0]->setStyleSheet(s);
    }
  }
  if (current_widget == 2)
  {
    QColor curr_color= QColor(new_ocelot_history_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Foreground"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_history_color= curr_color.name();
      label_for_color_rgb[0]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_history_color);
      button_for_color_show[0]->setStyleSheet(s);
    }
  }
  if (current_widget == 3)
  {
    QColor curr_color= QColor(new_ocelot_main_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Foreground"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_main_color= curr_color.name();
      label_for_color_rgb[0]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_main_color);
      button_for_color_show[0]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_1()
{
  if (current_widget == 0)
  {
    QColor curr_color= QColor(new_ocelot_statement_background_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Background"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_statement_background_color= curr_color.name();
      label_for_color_rgb[1]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_statement_background_color);
      button_for_color_show[1]->setStyleSheet(s);
    }
  }
  if (current_widget == 1)
  {
    QColor curr_color= QColor(new_ocelot_grid_background_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Background"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_grid_background_color= curr_color.name();
      label_for_color_rgb[1]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_grid_background_color);
      button_for_color_show[1]->setStyleSheet(s);
    }
  }
  if (current_widget == 2)
  {
    QColor curr_color= QColor(new_ocelot_grid_background_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Background"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_history_background_color= curr_color.name();
      label_for_color_rgb[1]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_history_background_color);
      button_for_color_show[1]->setStyleSheet(s);
    }
  }
  if (current_widget == 3)
  {
    QColor curr_color= QColor(new_ocelot_grid_background_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Background"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_main_background_color= curr_color.name();
      label_for_color_rgb[1]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_main_background_color);
      button_for_color_show[1]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_2()
{
  if (current_widget == 0)
  {
    QColor curr_color= QColor(new_ocelot_statement_highlight_literal_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Highlight literal"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_statement_highlight_literal_color= curr_color.name();
      label_for_color_rgb[2]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_statement_highlight_literal_color);
      button_for_color_show[2]->setStyleSheet(s);
    }
  }

  if (current_widget == 1)
  {
    QColor curr_color= QColor(new_ocelot_grid_cell_border_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Cell border"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_grid_cell_border_color= curr_color.name();
      label_for_color_rgb[2]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_grid_cell_border_color);
      button_for_color_show[2]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_3()
{
  if (current_widget == 0)
  {
    QColor curr_color= QColor(new_ocelot_statement_highlight_identifier_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Highlight identifier"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_statement_highlight_identifier_color= curr_color.name();
      label_for_color_rgb[3]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_statement_highlight_identifier_color);
      button_for_color_show[3]->setStyleSheet(s);
    }
  }

  if (current_widget == 1)
  {
    QColor curr_color= QColor(new_ocelot_grid_cell_right_drag_line_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Cell drag line"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_grid_cell_right_drag_line_color= curr_color.name();
      label_for_color_rgb[3]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_grid_cell_right_drag_line_color);
       button_for_color_show[3]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_4()
{
  if (current_widget == 0)
  {
    QColor curr_color= QColor(new_ocelot_statement_highlight_comment_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Highlight comment"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_statement_highlight_comment_color= curr_color.name();
      label_for_color_rgb[4]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_statement_highlight_comment_color);
      button_for_color_show[4]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_5()
{
  if (current_widget == 0)
  {
    QColor curr_color= QColor(new_ocelot_statement_highlight_operator_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Highlight operator"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_statement_highlight_operator_color= curr_color.name();
      label_for_color_rgb[5]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_statement_highlight_operator_color);
      button_for_color_show[5]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_6()
{
  if (current_widget == 0)
  {
    QColor curr_color= QColor(new_ocelot_statement_highlight_reserved_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Highlight reserved"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_statement_highlight_reserved_color= curr_color.name();
      label_for_color_rgb[6]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_statement_highlight_reserved_color);
      button_for_color_show[6]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_7()
{
 if (current_widget == 0)
 {
   QColor curr_color= QColor(new_ocelot_statement_prompt_background_color);
   curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Prompt background"), QColorDialog::ShowAlphaChannel);
   if (curr_color.isValid())
   {
     new_ocelot_statement_prompt_background_color= curr_color.name();
     label_for_color_rgb[7]->setText(curr_color.name());
     QString s= "background-color: ";
     s.append(new_ocelot_statement_prompt_background_color);
     button_for_color_show[7]->setStyleSheet(s);
    }
  }
  if (current_widget == 1)
  {
    QColor curr_color= QColor(new_ocelot_grid_header_background_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Header background"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_grid_header_background_color= curr_color.name();
      label_for_color_rgb[7]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_grid_header_background_color);
      button_for_color_show[7]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_8()
{
  if (current_widget == 0)
  {
    QColor curr_color= QColor(new_ocelot_statement_border_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Border"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_statement_border_color= curr_color.name();
      label_for_color_rgb[8]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_statement_border_color);
      button_for_color_show[8]->setStyleSheet(s);
    }
  }
  if (current_widget == 1)
  {
    QColor curr_color= QColor(new_ocelot_grid_border_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Border"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_grid_border_color= curr_color.name();
      label_for_color_rgb[8]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_grid_border_color);
      button_for_color_show[8]->setStyleSheet(s);
    }
  }
  if (current_widget == 2)
  {
    QColor curr_color= QColor(new_ocelot_history_border_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Border"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_history_border_color= curr_color.name();
      label_for_color_rgb[8]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_history_border_color);
      button_for_color_show[8]->setStyleSheet(s);
    }
  }
  if (current_widget == 3)
  {
    QColor curr_color= QColor(new_ocelot_main_border_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Border"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      new_ocelot_main_border_color= curr_color.name();
      label_for_color_rgb[8]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(new_ocelot_main_border_color);
      button_for_color_show[8]->setStyleSheet(s);
    }
  }
}


void handle_combo_box_for_size_0(int i)
{
  new_ocelot_grid_border_size= QString::number(i);
}


void handle_combo_box_for_size_1(int i)
{
  new_ocelot_grid_cell_border_size= QString::number(i);
}


void handle_combo_box_for_size_2(int i)
{
  new_ocelot_grid_cell_right_drag_line_size= QString::number(i);
}

/* Some of the code in handle_button_for_font_dialog() is a near-duplicate of code in set_colors_and_fonts(). */
void handle_button_for_font_dialog()
{
  bool ok;
  QString s;
  QFont font;

  if (current_widget == 0)
  {
    font= QFontDialog::getFont(&ok, QFont(new_ocelot_statement_font_family, new_ocelot_statement_font_size.toInt()), this);
  }
  if (current_widget == 1)
  {
    font= QFontDialog::getFont(&ok, QFont(new_ocelot_grid_font_family, new_ocelot_grid_font_size.toInt()), this);
  }
  if (current_widget == 2)
  {
    font= QFontDialog::getFont(&ok, QFont(new_ocelot_history_font_family, new_ocelot_history_font_size.toInt()), this);
  }
  if (current_widget == 3)
  {
    font= QFontDialog::getFont(&ok, QFont(new_ocelot_main_font_family, new_ocelot_main_font_size.toInt()), this);
  }

  if (ok)
  {
    /* User clicked OK on QFontDialog */
    if (current_widget == 0)
    {
      new_ocelot_statement_font_family= font.family();
      if (font.italic()) new_ocelot_statement_font_style= "italic";
      else new_ocelot_statement_font_style= "normal";
      new_ocelot_statement_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
      if (font.Bold) new_ocelot_statement_font_weight= "bold";
      else new_ocelot_statement_font_weight= "normal";
    }
   if (current_widget == 1)
   {
     new_ocelot_grid_font_family= font.family();
     if (font.italic()) new_ocelot_grid_font_style= "italic";
     else new_ocelot_grid_font_style= "normal";
     new_ocelot_grid_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
     if (font.Bold) new_ocelot_grid_font_weight= "bold";
     else new_ocelot_grid_font_weight= "normal";
   }
   if (current_widget == 2)
   {
     new_ocelot_history_font_family= font.family();
     if (font.italic()) new_ocelot_history_font_style= "italic";
     else new_ocelot_history_font_style= "normal";
     new_ocelot_history_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
     if (font.Bold) new_ocelot_history_font_weight= "bold";
     else new_ocelot_history_font_weight= "normal";
   }
   if (current_widget == 3)
   {
     new_ocelot_main_font_family= font.family();
     if (font.italic()) new_ocelot_main_font_style= "italic";
     else new_ocelot_main_font_style= "normal";
     new_ocelot_main_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
     if (font.Bold) new_ocelot_main_font_weight= "bold";
     else new_ocelot_main_font_weight= "normal";
    }
    label_for_font_dialog_set_text();
  }
}

};
