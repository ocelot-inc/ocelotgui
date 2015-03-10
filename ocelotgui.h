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

/* The debugger is integrated now, but "ifdef DEBUGGER" directives help to delineate code that is debugger-specific. */
#define DEBUGGER

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
#ifdef DEBUGGER
#include <QThread>
#include <QTimer>
#endif

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
class QThread48;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(int argc, char *argv[], QWidget *parent= 0);
  ~MainWindow();

  /*
    Client variables
    Client variables can be changed via Settings menu or via SET statements.
    Client variables have the prefix 'ocelot_'.
    Every client variable has an item on the Settings menu,
    and may be changed with the SET statement. For example,
    ocelot_grid_cell_right_drag_line_color is on menu item Settings | Grid Widget,
    SET @ocelot_grid_cell_right_drag_line_color = value; will change.
    The important thing is: if it's changed on the Settings menu, then
    a SET statement is generated, so that can be saved and replayed.
    Naming convention: ocelot_ + settings-menu-item + object + color|size|font.
    Todo: pass it on to the server, without interfering with ordinary SET statements.
    Todo: use keywords.
    Todo: allow SET @ocelot_... = DEFAULT
    Todo: pass it on to the server iff there's a server in existence
    Todo: REFRESH-FROM-SERVER in case server value changed during a function
    Todo: need a more flexible parser, eventually.
    Todo: should it be optional whether such statements go to history?
    Todo: do not consider it an error if you're not connected but it's a SET statement
    Todo: make sure it's a valid setting
    Todo: pay attention to the delimiter
    Todo: what should happen if the SET fails?
    Todo: isValid() check
    Todo: rename the Settings menu items and let the prompts match the names
    Todo: there is also  Qt way to save current settings, maybe it's another option
    Todo: menu item = "save settings" which would put a file in ~/ocelot.ini
    Todo: a single big setting
    Todo: a comment e.g. / *! OCELOT CLIENT * / meaning do not pass to server
    Problem: you cannot muck statement itself because menu might be changed while statement is up
    See also: Client variables that can be changed with the Settings widget
    Todo: Shouldn't client variables be in statement widget?
  */
  QString ocelot_statement_color, new_ocelot_statement_color;
  QString ocelot_statement_background_color, new_ocelot_statement_background_color;
  QString ocelot_statement_border_color, new_ocelot_statement_border_color;
  QString ocelot_statement_font_family, new_ocelot_statement_font_family;
  QString ocelot_statement_font_size, new_ocelot_statement_font_size;
  QString ocelot_statement_font_style, new_ocelot_statement_font_style;
  QString ocelot_statement_font_weight, new_ocelot_statement_font_weight;
  QString ocelot_statement_highlight_literal_color, new_ocelot_statement_highlight_literal_color;
  QString ocelot_statement_highlight_identifier_color, new_ocelot_statement_highlight_identifier_color;
  QString ocelot_statement_highlight_comment_color, new_ocelot_statement_highlight_comment_color;
  QString ocelot_statement_highlight_operator_color, new_ocelot_statement_highlight_operator_color;
  QString ocelot_statement_highlight_reserved_color, new_ocelot_statement_highlight_reserved_color;
  QString ocelot_statement_prompt_background_color, new_ocelot_statement_prompt_background_color;
  QString ocelot_statement_style_string;
  QString ocelot_grid_color, new_ocelot_grid_color;
  QString ocelot_grid_background_color, new_ocelot_grid_background_color;
  QString ocelot_grid_border_color, new_ocelot_grid_border_color;
  QString ocelot_grid_header_background_color, new_ocelot_grid_header_background_color;
  QString ocelot_grid_font_family, new_ocelot_grid_font_family;
  QString ocelot_grid_font_size, new_ocelot_grid_font_size;
  QString ocelot_grid_font_style, new_ocelot_grid_font_style;
  QString ocelot_grid_font_weight, new_ocelot_grid_font_weight;
  QString ocelot_grid_cell_border_color, new_ocelot_grid_cell_border_color;
  QString ocelot_grid_cell_right_drag_line_color, new_ocelot_grid_cell_right_drag_line_color;
  QString ocelot_grid_border_size, new_ocelot_grid_border_size;
  QString ocelot_grid_cell_border_size, new_ocelot_grid_cell_border_size;
  QString ocelot_grid_cell_right_drag_line_size, new_ocelot_grid_cell_right_drag_line_size;
  QString ocelot_grid_style_string;
  QString ocelot_grid_header_style_string;
  QString ocelot_history_color, new_ocelot_history_color;
  QString ocelot_history_background_color, new_ocelot_history_background_color;
  QString ocelot_history_border_color, new_ocelot_history_border_color;
  QString ocelot_history_font_family, new_ocelot_history_font_family;
  QString ocelot_history_font_size, new_ocelot_history_font_size;
  QString ocelot_history_font_style, new_ocelot_history_font_style;
  QString ocelot_history_font_weight, new_ocelot_history_font_weight;
  QString ocelot_history_style_string, new_ocelot_history_style_string;
  QString ocelot_main_color, new_ocelot_main_color;
  QString ocelot_main_background_color, new_ocelot_main_background_color;
  QString ocelot_main_border_color, new_ocelot_main_border_color;
  QString ocelot_main_font_family, new_ocelot_main_font_family;
  QString ocelot_main_font_size, new_ocelot_main_font_size;
  QString ocelot_main_font_style, new_ocelot_main_font_style;
  QString ocelot_main_font_weight, new_ocelot_main_font_weight;
  QString ocelot_main_style_string;

  /* Strings for CONNECT. These will be converted e.g. ocelot_host to ocelot_host_as_utf8 */
  QString ocelot_host;
  QString ocelot_database;
  QString ocelot_user;
  QString ocelot_password;
  QString ocelot_unix_socket;
  QString ocelot_default_auth;
  QString ocelot_init_command;
  QString ocelot_opt_bind;
  QString ocelot_opt_connect_attr_delete;
  QString ocelot_opt_ssl;
  QString ocelot_opt_ssl_ca;
  QString ocelot_opt_ssl_capath;
  QString ocelot_opt_ssl_cert;
  QString ocelot_opt_ssl_cipher;
  QString ocelot_opt_ssl_crl;
  QString ocelot_opt_ssl_crlpath;
  QString ocelot_opt_ssl_key;
  QString ocelot_plugin_dir;
  QString ocelot_read_default_file;
  QString ocelot_read_default_group;
  QString ocelot_server_public_key;
  QString ocelot_set_charset_dir;
  QString ocelot_set_charset_name;           /* was: ocelot_default_character_set */
  QString ocelot_shared_memory_base_name;
  QString ocelot_protocol;
  unsigned short ocelot_comments;            /* for CONNECT. not used. */
  unsigned short ocelot_no_defaults;         /* for CONNECT */
  QString ocelot_defaults_file;              /* for CONNECT */
  QString ocelot_defaults_extra_file;        /* for CONNECT */


  /* Following were moved from 'private:', merely so all client variables could be together. Cannot be used with SET. */

  QString ocelot_dbms;                    /* for CONNECT */
  int ocelot_grid_detached;
  unsigned int ocelot_grid_max_row_lines;          /* ?? should be unsigned long? */
  /* unsigned int ocelot_grid_max_desired_width_in_chars; */
  int ocelot_history_includes_warnings;   /* affects history */

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
  void action_kill();
  void action_about();
  void action_the_manual();
  void action_the_manual_close();
  void action_statement_edit_widget_text_changed();
  void action_undo();
  void action_statement();
  void action_change_one_setting(QString old_setting, QString new_setting, const char *name_of_setting);
  void action_grid();
  void action_history();
  void action_main();
  void history_markup_previous();
  void history_markup_next();
#ifdef DEBUGGER
  int debug_mdbug_install_sql(MYSQL *mysql, char *x); /* the only routine in install_sql.cpp */
  int debug_parse_statement(QString text,
                             char *returned_command_string,
                             int *index_of_number_1,
                             int *index_of_number_2);
  int debug_error(char *text);
//  void action_debug_install();
  void debug_install_go();
//  void action_debug_setup();
  void debug_setup_go(QString text);
  void debug_setup_mysql_proc_insert();
//  void action_debug_debug();
  void debug_debug_go(QString text);
  void action_debug_breakpoint();
  void debug_breakpoint_or_clear_go(int statement_type, QString text);
  void action_debug_mousebuttonpress(QEvent *event, int which_debug_widget_index);
  void action_debug_continue();
//  void action_debug_leave();
  void debug_leave_go();
  void debug_skip_go();
  void debug_source_go();
  void debug_set_go();
  void debug_other_go(QString text);
  void action_debug_next();
//  void action_debug_skip();
  void action_debug_step();
  void action_debug_clear();
//  void action_debug_delete();
  void debug_delete_go();
  void debug_execute_go();
  void action_debug_exit();
  void debug_exit_go(int flagger);
  void debug_delete_tab_widgets();
  void action_debug_information();
  void action_debug_refresh_server_variables();
  void action_debug_refresh_user_variables();
  void action_debug_refresh_variables();
  void action_debug_refresh_call_stack();
  void action_debug_timer_status();
#endif

protected:
  bool eventFilter(QObject *obj, QEvent *ev);

private:
  Ui::MainWindow *ui;

  int history_markup_previous_or_next();
  void create_widget_history();
  void create_widget_statement();
#ifdef DEBUGGER
  void debug_menu_enable_or_disable(int statement_type);
  void create_widget_debug();
  int debug_information_status(char *last_command);
  int debug_call_xxxmdbug_command(const char *command);
  void debug_highlight_line();
  void debug_maintain_prompt(int action, int debug_widget_index, int line_number);
  QString debug_privilege_check(int statement_type);
#endif

  void create_menu();
  void widget_sizer();
  int execute_client_statement(QString text);
  void put_diagnostics_in_result();
  unsigned int get_ocelot_protocol_as_int(QString s);
//  int options_and_connect(char *host, char *database, char *user, char *password, char *tmp_init_command,
//                       char *tmp_plugin_dir, char *tmp_default_auth, char *unix_socket, unsigned int connection_number);

  void connect_mysql_options_2(int w_argc, char *argv[]);
  void connect_read_command_line(int argc, char *argv[]);
  void connect_read_my_cnf(const char *file_name);
  QString connect_stripper(QString value_to_strip);
  void connect_set_variable(QString token0, QString token2);
  void connect_make_statement();

  void copy_options_to_main_window();
  void copy_connect_strings_to_utf8(); /* in effect, copy options from main_window */
  int the_connect(unsigned int connection_number);
  int the_connect_2(); /* intended replacement for the_connect() */
  //my_bool get_one_option(int optid, const struct my_option *opt __attribute__((unused)),char *argument);
  void connect_init();
  void set_current_colors_and_font();
  void make_style_strings();
  void create_the_manual_widget();
  int get_next_statement_in_string();
  void action_execute_one_statement(QString text);

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
    TOKEN_KEYWORD_ASCII= TOKEN_KEYWORD_ASC + 1,
    TOKEN_KEYWORD_ASENSITIVE= TOKEN_KEYWORD_ASCII + 1,
    TOKEN_KEYWORD_BEFORE= TOKEN_KEYWORD_ASENSITIVE + 1,
    TOKEN_KEYWORD_BEGIN= TOKEN_KEYWORD_BEFORE + 1,
    TOKEN_KEYWORD_BETWEEN= TOKEN_KEYWORD_BEGIN + 1,
    TOKEN_KEYWORD_BIGINT= TOKEN_KEYWORD_BETWEEN + 1,
    TOKEN_KEYWORD_BINARY= TOKEN_KEYWORD_BIGINT + 1,
    TOKEN_KEYWORD_BIT= TOKEN_KEYWORD_BINARY + 1,
    TOKEN_KEYWORD_BLOB= TOKEN_KEYWORD_BIT + 1,
    TOKEN_KEYWORD_BOOL= TOKEN_KEYWORD_BLOB + 1,
    TOKEN_KEYWORD_BOOLEAN= TOKEN_KEYWORD_BOOL + 1,
    TOKEN_KEYWORD_BOTH= TOKEN_KEYWORD_BOOLEAN + 1,
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
    TOKEN_KEYWORD_DATE= TOKEN_KEYWORD_DATABASES + 1,
    TOKEN_KEYWORD_DATETIME= TOKEN_KEYWORD_DATE + 1,
    TOKEN_KEYWORD_DAY_HOUR= TOKEN_KEYWORD_DATETIME + 1,
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
    TOKEN_KEYWORD_ENUM= TOKEN_KEYWORD_END + 1,
    TOKEN_KEYWORD_ESCAPED= TOKEN_KEYWORD_ENUM + 1,
    TOKEN_KEYWORD_EVENT= TOKEN_KEYWORD_ESCAPED + 1,
    TOKEN_KEYWORD_EXISTS= TOKEN_KEYWORD_EVENT + 1,
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
    TOKEN_KEYWORD_GEOMETRY= TOKEN_KEYWORD_FUNCTION + 1,
    TOKEN_KEYWORD_GEOMETRYCOLLECTION= TOKEN_KEYWORD_GEOMETRY + 1,
    TOKEN_KEYWORD_GET= TOKEN_KEYWORD_GEOMETRYCOLLECTION + 1,
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
    TOKEN_KEYWORD_LINESTRING= TOKEN_KEYWORD_LINES + 1,
    TOKEN_KEYWORD_LOAD= TOKEN_KEYWORD_LINESTRING + 1,
    TOKEN_KEYWORD_LOCALTIME= TOKEN_KEYWORD_LOAD + 1,
    TOKEN_KEYWORD_LOCALTIMESTAMP= TOKEN_KEYWORD_LOCALTIME + 1,
    TOKEN_KEYWORD_LOCK= TOKEN_KEYWORD_LOCALTIMESTAMP + 1,
    TOKEN_KEYWORD_LOGFILE= TOKEN_KEYWORD_LOCK + 1,
    TOKEN_KEYWORD_LONG= TOKEN_KEYWORD_LOGFILE + 1,
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
    TOKEN_KEYWORD_MULTILINESTRING= TOKEN_KEYWORD_MODIFIES + 1,
    TOKEN_KEYWORD_MULTIPOINT= TOKEN_KEYWORD_MULTILINESTRING + 1,
    TOKEN_KEYWORD_MULTIPOLYGON= TOKEN_KEYWORD_MULTIPOINT + 1,
    TOKEN_KEYWORD_NATURAL= TOKEN_KEYWORD_MULTIPOLYGON + 1,
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
    TOKEN_KEYWORD_POINT= TOKEN_KEYWORD_PARTITION + 1,
    TOKEN_KEYWORD_POLYGON= TOKEN_KEYWORD_POINT + 1,
    TOKEN_KEYWORD_PRECISION= TOKEN_KEYWORD_POLYGON + 1,
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
    TOKEN_KEYWORD_RETURNS= TOKEN_KEYWORD_RETURN + 1,
    TOKEN_KEYWORD_REVOKE= TOKEN_KEYWORD_RETURNS + 1,
    TOKEN_KEYWORD_RIGHT= TOKEN_KEYWORD_REVOKE + 1,
    TOKEN_KEYWORD_RLIKE= TOKEN_KEYWORD_RIGHT + 1,
    TOKEN_KEYWORD_ROW= TOKEN_KEYWORD_RLIKE + 1,
    TOKEN_KEYWORD_SCHEMA= TOKEN_KEYWORD_ROW + 1,
    TOKEN_KEYWORD_SCHEMAS= TOKEN_KEYWORD_SCHEMA + 1,
    TOKEN_KEYWORD_SECOND_MICROSECOND= TOKEN_KEYWORD_SCHEMAS + 1,
    TOKEN_KEYWORD_SELECT= TOKEN_KEYWORD_SECOND_MICROSECOND + 1,
    TOKEN_KEYWORD_SENSITIVE= TOKEN_KEYWORD_SELECT + 1,
    TOKEN_KEYWORD_SEPARATOR= TOKEN_KEYWORD_SENSITIVE + 1,
    TOKEN_KEYWORD_SERIAL= TOKEN_KEYWORD_SEPARATOR + 1,
    TOKEN_KEYWORD_SERVER= TOKEN_KEYWORD_SERIAL + 1,
    TOKEN_KEYWORD_SET= TOKEN_KEYWORD_SERVER + 1,
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
    TOKEN_KEYWORD_TABLESPACE= TOKEN_KEYWORD_TABLE + 1,
    TOKEN_KEYWORD_TEE= TOKEN_KEYWORD_TABLESPACE + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_TERMINATED= TOKEN_KEYWORD_TEE + 1,
    TOKEN_KEYWORD_THEN= TOKEN_KEYWORD_TERMINATED + 1,
    TOKEN_KEYWORD_TIME= TOKEN_KEYWORD_THEN + 1,
    TOKEN_KEYWORD_TIMESTAMP= TOKEN_KEYWORD_TIME + 1,
    TOKEN_KEYWORD_TINYBLOB= TOKEN_KEYWORD_TIMESTAMP + 1,
    TOKEN_KEYWORD_TINYINT= TOKEN_KEYWORD_TINYBLOB + 1,
    TOKEN_KEYWORD_TINYTEXT= TOKEN_KEYWORD_TINYINT + 1,
    TOKEN_KEYWORD_TO= TOKEN_KEYWORD_TINYTEXT + 1,
    TOKEN_KEYWORD_TRAILING= TOKEN_KEYWORD_TO + 1,
    TOKEN_KEYWORD_TRIGGER= TOKEN_KEYWORD_TRAILING + 1,
    TOKEN_KEYWORD_TRUE= TOKEN_KEYWORD_TRIGGER + 1,
    TOKEN_KEYWORD_UNDO= TOKEN_KEYWORD_TRUE + 1,
    TOKEN_KEYWORD_UNICODE= TOKEN_KEYWORD_UNDO + 1,
    TOKEN_KEYWORD_UNION= TOKEN_KEYWORD_UNICODE + 1,
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
    TOKEN_KEYWORD_VIEW= TOKEN_KEYWORD_VARYING + 1,
    TOKEN_KEYWORD_WARNINGS= TOKEN_KEYWORD_VIEW + 1, /* Ocelot keyword */
    TOKEN_KEYWORD_WHEN= TOKEN_KEYWORD_WARNINGS + 1,
    TOKEN_KEYWORD_WHERE= TOKEN_KEYWORD_WHEN + 1,
    TOKEN_KEYWORD_WHILE= TOKEN_KEYWORD_WHERE + 1,
    TOKEN_KEYWORD_WITH= TOKEN_KEYWORD_WHILE + 1,
    TOKEN_KEYWORD_WRITE= TOKEN_KEYWORD_WITH + 1,
    TOKEN_KEYWORD_XOR= TOKEN_KEYWORD_WRITE + 1,
    TOKEN_KEYWORD_YEAR= TOKEN_KEYWORD_XOR + 1,
    TOKEN_KEYWORD_YEAR_MONTH= TOKEN_KEYWORD_YEAR + 1,
    TOKEN_KEYWORD_ZEROFILL= TOKEN_KEYWORD_YEAR_MONTH + 1
#ifdef DEBUGGER
    ,
    TOKEN_KEYWORD_DEBUG_BREAKPOINT = TOKEN_KEYWORD_ZEROFILL + 1,
    TOKEN_KEYWORD_DEBUG_CLEAR= TOKEN_KEYWORD_DEBUG_BREAKPOINT + 1,
    TOKEN_KEYWORD_DEBUG_CONTINUE= TOKEN_KEYWORD_DEBUG_CLEAR + 1,
    TOKEN_KEYWORD_DEBUG_DEBUG= TOKEN_KEYWORD_DEBUG_CONTINUE + 1,
    TOKEN_KEYWORD_DEBUG_DELETE= TOKEN_KEYWORD_DEBUG_DEBUG + 1,
    TOKEN_KEYWORD_DEBUG_EXECUTE= TOKEN_KEYWORD_DEBUG_DELETE + 1,
    TOKEN_KEYWORD_DEBUG_EXIT= TOKEN_KEYWORD_DEBUG_EXECUTE + 1,
    TOKEN_KEYWORD_DEBUG_INFORMATION= TOKEN_KEYWORD_DEBUG_EXIT + 1,
    TOKEN_KEYWORD_DEBUG_INSTALL= TOKEN_KEYWORD_DEBUG_INFORMATION + 1,
    TOKEN_KEYWORD_DEBUG_LEAVE= TOKEN_KEYWORD_DEBUG_INSTALL + 1,
    TOKEN_KEYWORD_DEBUG_NEXT= TOKEN_KEYWORD_DEBUG_LEAVE + 1,
    TOKEN_KEYWORD_DEBUG_REFRESH= TOKEN_KEYWORD_DEBUG_NEXT + 1,
    TOKEN_KEYWORD_DEBUG_SET= TOKEN_KEYWORD_DEBUG_REFRESH + 1,
    TOKEN_KEYWORD_DEBUG_SETUP= TOKEN_KEYWORD_DEBUG_SET + 1,
    TOKEN_KEYWORD_DEBUG_SKIP= TOKEN_KEYWORD_DEBUG_SETUP + 1,
    TOKEN_KEYWORD_DEBUG_SOURCE= TOKEN_KEYWORD_DEBUG_SKIP + 1,
    TOKEN_KEYWORD_DEBUG_STEP= TOKEN_KEYWORD_DEBUG_SOURCE + 1,
    TOKEN_KEYWORD_DEBUG_TBREAKPOINT= TOKEN_KEYWORD_DEBUG_STEP + 1
#endif
       };

  void tokenize(QChar *text, int text_length, int (*token_lengths)[MAX_TOKENS], int (*token_offsets)[MAX_TOKENS], int max_tokens, QChar *version, int passed_comment_behaviour, QString special_token, int minus_behaviour);

  int token_type(QChar *token, int token_length);

  void tokens_to_keywords(QString text);
  void tokens_to_keywords_revert(int i_of_body, int i_of_function, QString text);
  int next_token(int i);
  int find_start_of_body(QString text, int *i_of_function);
  int connect_mysql(unsigned int connection_number);
  QString select_1_row(const char *select_statement);

  QWidget *main_window;
  QVBoxLayout *main_layout;

  CodeEditor *statement_edit_widget;
  QTextEdit *history_edit_widget;
#ifdef DEBUGGER
#define DEBUG_TAB_WIDGET_MAX 10
  QWidget *debug_top_widget;
  QVBoxLayout *debug_top_widget_layout;
  QLineEdit *debug_line_widget;
  QTabWidget *debug_tab_widget;
  CodeEditor *debug_widget[DEBUG_TAB_WIDGET_MAX]; /* todo: this should be variable-size */
#endif

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
    QAction *menu_run_action_kill;
  QMenu *menu_settings;
    QAction *menu_settings_action_statement;
    QAction *menu_settings_action_grid;
    QAction *menu_settings_action_history;
    QAction *menu_settings_action_main;
#ifdef DEBUGGER
  QMenu *menu_debug;
//    QAction *menu_debug_action_install;
//    QAction *menu_debug_action_setup;
//    QAction *menu_debug_action_debug;
    QAction *menu_debug_action_breakpoint;
    QAction *menu_debug_action_continue;
    QAction *menu_debug_action_leave;
    QAction *menu_debug_action_next;
//    QAction *menu_debug_action_skip;
    QAction *menu_debug_action_step;
    QAction *menu_debug_action_clear;
//    QAction *menu_debug_action_delete;
    QAction *menu_debug_action_exit;
    QAction *menu_debug_action_information;
    QAction *menu_debug_action_refresh_server_variables;
    QAction *menu_debug_action_refresh_user_variables;
    QAction *menu_debug_action_refresh_variables;
    QAction *menu_debug_action_refresh_call_stack;
#endif
  QMenu *menu_help;
    QAction *menu_help_action_about;
    QAction *menu_help_action_the_manual;

  QWidget *the_manual_widget;
    QVBoxLayout *the_manual_layout;
    QTextEdit *the_manual_text_edit;
    QPushButton *the_manual_pushbutton;

  /* QTableWidget *grid_table_widget; */
  QTabWidget *result_grid_tab_widget;
/* It's easy to increase this so more multi results are seen but then start time is longer. */
#define RESULT_GRID_TAB_WIDGET_MAX 2
  ResultGrid *result_grid_table_widget[RESULT_GRID_TAB_WIDGET_MAX];

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

};

#endif // MAINWINDOW_H

/*****************************************************************************************************************************/
/* THE TEXTEDITFRAME WIDGET */
/* See comments containing the word TextEditFrame, in ResultGrid code. */

class TextEditFrame : public QFrame
{
  Q_OBJECT

public:
  explicit TextEditFrame(QWidget *parent, ResultGrid *ancestor, unsigned int index);
  ~TextEditFrame();

  int border_size;
  int minimum_width;
  int minimum_height;
  ResultGrid *ancestor_result_grid_widget;
  unsigned int text_edit_frames_index;                 /* e.g. for text_edit_frames[5] this will contain 5 */
  int ancestor_grid_column_number;
  int ancestor_grid_row_number;
  int length;
  char *pointer_to_content;
  bool is_retrieved_flag;
  bool is_style_sheet_set_flag;

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event);

private:
  int left_mouse_button_was_pressed;
  int widget_side;
  enum {LEFT= 1, RIGHT= 2, TOP= 3, BOTTOM= 4};

};

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
  QLabel **label;
  QLineEdit **line_edit;
  QTextEdit **text_edit;
  QHBoxLayout **hbox_layout;
  QWidget **widget;
  QPushButton *button_for_cancel, *button_for_ok;
  QHBoxLayout *hbox_layout_for_ok_and_cancel;
  QVBoxLayout *main_layout;
  QWidget *widget_for_ok_and_cancel;
  MainWindow *copy_of_parent;
  QLabel *label_for_message;
  QScrollArea *scroll_area;
  QWidget *widget_with_main_layout;
  QVBoxLayout *upper_layout;

  int column_count_copy;
  int *row_form_is_password_copy;
  QString *row_form_data_copy;
  Row_form_box *this_row_form_box;

public:
Row_form_box(QFont *font, int column_count, QString *row_form_label,
//             int *row_form_type,
             int *row_form_is_password, QString *row_form_data,
//             QString *row_form_width,
             QString row_form_title, QString row_form_message, MainWindow *parent): QDialog(parent)
{
  int i;

  this_row_form_box= this;
  copy_of_parent= parent;

  column_count_copy= column_count;
  row_form_is_password_copy= row_form_is_password;
  row_form_data_copy= row_form_data;
  main_layout= 0;
  label_for_message= 0;
  label= 0;
  line_edit= 0;
  text_edit= 0;
  hbox_layout= 0;
  widget= 0;
  button_for_cancel= 0;
  button_for_ok= 0;
  hbox_layout_for_ok_and_cancel= 0;
  widget_for_ok_and_cancel= 0;
  scroll_area= 0;
  widget_with_main_layout= 0;
  upper_layout= 0;

  label= new QLabel*[column_count];
  line_edit= new QLineEdit*[column_count];
  text_edit= new QTextEdit*[column_count];
  hbox_layout= new QHBoxLayout*[column_count];
  widget= new QWidget*[column_count];

  for (i= 0; i < column_count; ++i)
  {
    label[i]= 0;
    line_edit[i]= 0;
    text_edit[i]= 0;
    hbox_layout[i]= 0;
    widget[i]= 0;
  }
//  copy_of_parent= parent; I already did this
  is_ok= 0;
  QFontMetrics mm= QFontMetrics(*font);
  //unsigned int max_width_of_a_char= mm.width("W");                 /* not really a maximum unless fixed-width font */
  unsigned int max_height_of_a_char= mm.lineSpacing();             /* Actually this is mm.height() + mm.leading(). */
  main_layout= new QVBoxLayout();
  main_layout->setSpacing(0); /* Todo: check why this doesn't seem to have any effect */
  main_layout->setContentsMargins(0, 0, 0, 0); /* Todo: check why this doesn't seem to have any effect */
  main_layout->setSizeConstraint(QLayout::SetFixedSize);  /* no effect */
  label_for_message= new QLabel(row_form_message);
  main_layout->addWidget(label_for_message);
  /* Todo: watch row_form_type maybe it's NUM_FLAG */
  for (i= 0; i < column_count; ++i)
  {
    hbox_layout[i]= new QHBoxLayout();
    label[i]= new QLabel();
    label[i]->setStyleSheet(parent->ocelot_grid_header_style_string);
    label[i]->setText(row_form_label[i]);
    hbox_layout[i]->addWidget(label[i]);
    if (row_form_is_password[i] == 1)
    {
      line_edit[i]= new QLineEdit();
      line_edit[i]->setStyleSheet(parent->ocelot_grid_style_string);
      line_edit[i]->insert(row_form_data[i]);
      line_edit[i]->setEchoMode(QLineEdit::Password); /* maybe PasswordEchoOnEdit would be better */
      line_edit[i]->setMinimumHeight(2 * max_height_of_a_char + 3); /* no effect */
      line_edit[i]->setMaximumHeight(2 * max_height_of_a_char + 3); /* no effect */
      hbox_layout[i]->addWidget(line_edit[i]);
    }
    else
    {
      text_edit[i]= new QTextEdit();
      text_edit[i]->setStyleSheet(parent->ocelot_grid_style_string);
      text_edit[i]->setText(row_form_data[i]);
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

//  this->setMinimumWidth(800);                                     /* !! BE SMARTER !! */
//  this->setMinimumHeight(800);                                    /* !! BE SMARTER !! */

  widget_with_main_layout= new QWidget();
  widget_with_main_layout->setLayout(main_layout);

  /* I had trouble making the scroll area size correctly so I've commented out scroll area creation. */
  //scroll_area= new QScrollArea();
  //scroll_area->setWidget(widget_with_main_layout);
  upper_layout= new QVBoxLayout;
  //upper_layout->addWidget(scroll_area);
  upper_layout->addWidget(widget_with_main_layout);
  this->setLayout(upper_layout);
  this->setWindowTitle(row_form_title);

//  set_preferred_size();
}

/*
  Set preferred width and height for sizeHint().
  Initially, I tried creating a dummy widget with a layout for one row, saying setWindowOpacity(0),
  showing, and immediately hiding -- and the result was always garbage, I couldn't figure out a fix.
  So once again I try here to guess what Qt would do.
  width of (the longest label + contents) + (border width * 2)
  height of (the highest label + contents) * # of columns + (border width * 2)
  See also: grid_column_size_calc()
*/
//void set_preferred_size()
//{
//  int width_of_one_row;
//  int height_of_all_rows;
//
////  height_of_all_rows= 2 * max_height_of_a_char + 3;
//  height_of_all_rows*= column_count_copy;
//  printf("height=%d\n", height_of_all_rows);
//}


//QSize sizeHint() const
//{
//  return QSize(200, 200);
//}


private slots:

void handle_button_for_ok()
{
  for (int i= 0; i < column_count_copy; ++i)
  {
    if (this_row_form_box->row_form_is_password_copy[i] == 1) this_row_form_box->row_form_data_copy[i]= line_edit[i]->text();
    else this_row_form_box->row_form_data_copy[i]= text_edit[i]->toPlainText();
  }
  is_ok= 1;
  garbage_collect();
  close();
}


void handle_button_for_cancel()
{
  garbage_collect();
  close();
}


/*
  I'm doing my own garbage collection. Maybe it's a bad idea but it's the way that I know.
  Objective: anything set up with "new", without a "this", must be deleted explicitly.
  Todo: the garbage_collect for result_grid_table isn't as well put together as this.
*/
void garbage_collect ()
{
  int i;
  if (label != 0)
  {
    for (i= 0; i < column_count_copy; ++i) if (label[i] != 0) delete label[i];
    delete [] label;
  }
  if (line_edit != 0)
  {
    for (i= 0; i < column_count_copy; ++i) if (line_edit[i] != 0) delete line_edit[i];
    delete [] line_edit;
  }
  if (text_edit != 0)
  {
    for (i= 0; i < column_count_copy; ++i) if (text_edit[i] != 0) delete text_edit[i];
    delete [] text_edit;
  }
  if (hbox_layout != 0)
  {
    for (i= 0; i < column_count_copy; ++i) if (hbox_layout[i] != 0) delete hbox_layout[i];
    delete [] hbox_layout;
  }
  if (widget != 0)
  {
    for (i= 0; i < column_count_copy; ++i) if (widget[i] != 0) delete widget[i];
    delete [] widget;
  }
  if (button_for_cancel != 0) delete button_for_cancel;
  if (button_for_ok != 0) delete button_for_ok;
  if (hbox_layout_for_ok_and_cancel != 0) delete hbox_layout_for_ok_and_cancel;
  if (widget_for_ok_and_cancel != 0) delete widget_for_ok_and_cancel;
  if (label_for_message != 0) delete label_for_message;
  if (main_layout != 0) delete main_layout;
  if (widget_with_main_layout != 0) delete widget_with_main_layout;
  if (upper_layout != 0) delete upper_layout;
  if (scroll_area != 0) delete scroll_area;
}

};


#define STATEMENT_WIDGET 0
#define GRID_WIDGET 1
#define HISTORY_WIDGET 2
#define MAIN_WIDGET 3


/*********************************************************************************************************/

/* THE GRID WIDGET */
/*
  Re the "cells" of the grid:
  Todo: Originally these were QPlainTextEdit widgets, because QTextEdit
        would insist on expanding long lines. That doesn't seem to be happening any more,
        but we might have to switch back if there are problems.
        Make sure the QTextEdit widgets are plain text, in case somebody enters HTML.
  Todo: allow change of setTextDirection() based on language or on client variable.
  Todo: see this re trail spaces: http://qt-project.org/doc/qt-4.8/qtextoption.html#Flag-enum
  Todo: although min(column width) = heading width, don't have to make text editable area that wide
  Todo: have to think what to do with control characters, e.g. for tabs should I have
        setTabChangesFocus(true), for others should I allow the effects or display specially.
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
  ... Also, user might resize columns or rows via dragging.
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
        Up|down dragging "works" now but is undocumented, and still needs fixes for:
          todo: minimum height (currently doesn't look right), bottom drag line size, bottom drag line color
  Todo: vertical resizing currently looks odd. I've read that "Correct place to do special layout management is overridden resizeEvent."
  Todo: test with a frame that has been scrolled horizontally so half of it is not visible, while there's a scoll bar.
  Todo: There can be a bit of flicker during drag though I doubt that anyone will care.
  Todo: BUG: If the QTextEdit gets a vertical scroll bar, then the horizontal cursor appears over the scroll bar,
        and dragging won't work. The problem is alleviated if border_size > 1. (Check: maybe this is fixed.)
  Todo: Install an event filter for each text_edit_widgets[i]. If user double-clicks: find which widget it is,
        figure out from that which row it is, and put up a dialog box for that row with row_form_box.
  Todo: Find out why cut-and-paste often fails. Maybe it's that selecting doesn't change colour or paintevent returns wrong.
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

/*
  Re: how the grid is displayed
  The display is reasonably close to instant in most cases, even when there are thousands of rows.
  Mostly that's because there are only a few hundred widgets, regardless of result row count.
  We create a pool of cell widgets and a pool of row widgets, which only needs expanding if there are many many columns per row.
  When it comes time to display a cell, if it was previously used  for displaying a different row + column, we change it.
  There's some added complication if sql_more_results is true; in that case we make a copy of the contents of mysql_res.
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
  unsigned long *grid_max_column_widths;                     /* dynamic-sized list of actual maximum widths in detail columns */
  unsigned int *grid_column_heights;                         /* dynamic-sized list of heights */
  unsigned char *grid_column_dbms_sources;                   /* dynamic-sized list of sources */
  unsigned short int *grid_column_dbms_field_numbers;        /* dynamic-sized list of field numbers */
  unsigned long r;
  MYSQL_ROW row;
  int is_paintable;
  unsigned int max_text_edit_frames_count;                       /* used for a strange error check during paint events */

  MYSQL_FIELD *fields;
  QScrollArea *grid_scroll_area;
  QScrollBar *grid_vertical_scroll_bar;                          /* This might take over from the automatic scroll bar. */
  int grid_vertical_scroll_bar_value;                            /* Todo: find out why this isn't defined as long unsigned */
  QTextEdit **text_edit_widgets; /* Todo: consider using plaintext */ /* dynamic-sized list of pointers to QPlainTextEdit widgets */
  QHBoxLayout **text_edit_layouts;
  TextEditFrame **text_edit_frames;

  MYSQL_RES *grid_mysql_res;
  bool mysql_more_results_flag;
  char *mysql_res_copy;                                          /* gets a copy of mysql_res contents, if necessary */
  char **mysql_res_copy_rows;                                      /* dynamic-sized list of mysql_res_copy row offsets, if necessary */
  unsigned int ocelot_grid_max_grid_height_in_lines;             /* Todo: should be user-settable and passed */
//  unsigned int grid_actual_grid_height_in_rows;
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
  int ocelot_grid_cell_border_size_as_int;
  QString ocelot_grid_color;
  QString ocelot_grid_background_color;
  QString ocelot_grid_cell_right_drag_line_color;
  unsigned int row_pool_size;
  unsigned int cell_pool_size;
  QString frame_color_setting;                                 /* based on drag line color */

/* How many rows can fit on the screen? Take a guess. */
#define RESULT_GRID_WIDGET_MAX_HEIGHT 20

/* Use NULL_STRING when displaying a column value which is null. Length is sizeof(NULL_STRING) - 1. */
#define NULL_STRING "NULL"

ResultGrid(
//        MYSQL_RES *mysql_res,
//        QFont *saved_font,
        MainWindow *parent): QWidget(parent)
{
  is_paintable= 0;
  ocelot_grid_max_grid_height_in_lines= 35;                 /* Todo: should be user-settable and passed */

  client= new QWidget(this);

  copy_of_parent= parent;

  text_edit_widgets= 0;                                     /* all dynamic-sized items should be initially zero */
  text_edit_layouts= 0;
  text_edit_frames= 0;
  grid_column_widths= 0;
  grid_max_column_widths= 0;
  grid_column_heights= 0;
  grid_column_dbms_sources= 0;
  grid_column_dbms_field_numbers= 0;
  grid_vertical_scroll_bar= 0;
  grid_scroll_area= 0;
  /* grid_layout= 0; */
  hbox_layout= 0;
  grid_row_layouts= 0;
  grid_row_widgets= 0;
  grid_main_layout= 0;
  /* grid_main_widget= 0; */
  border_size= 1;                                          /* Todo: This actually has to depend on stylesheet */

  result_row_count= 0;
  result_column_count= 0;

  /* With RESULT_GRID_WIDGET_MAX_HEIGHT=20 and 20 pixels per row this might be safe though it's sure arbitrary. */
  setMaximumHeight(RESULT_GRID_WIDGET_MAX_HEIGHT * 20);

  /* We might say "new ResultGrid(0)" merely so we'd have ResultGrid in the middle spot in the layout-> */

  /* Create the cell pool. */
  /*
    Make the cells. Each cell is one QTextEdit within one QHBoxLayout within one TextEditFrame.
    Each TexteditFrame i.e. text_edit_frames[n] will be added to the scroll area.
  */
  /* Todo: say "(this)" a lot so automatic garbage collect will work. */
  grid_scroll_area= new QScrollArea(this);

  grid_scroll_area->setWidget(client);
  grid_scroll_area->setWidgetResizable(true);              /* Without this, the QTextEdit widget heights won't change */

  grid_vertical_scroll_bar= new QScrollBar();                                 /* Todo: check if it's bad that this has no parent */

  /* setContentsMargins overrides style settings, I suppose. */
  /* Will setSpacing(0) keep scroll bar beside scroll area? Apparently not. Useless. */
  /* We could add a label with a line like the one below. That would be a header. Useless? */
  /* hbox_layout->addWidget(new QLabel("La La La", this)); */
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
  /* setLayout(hbox_layout); */
  /* Can't recall why "grid_vertical_scroll_bar_value= 0;" was here -- it meant that
     after fontchange the rows were back at the start */
  /* grid_vertical_scroll_bar_value= 0; */
  /* grid_vertical_scroll_bar= grid_scroll_area->verticalScrollBar(); */

  /* Assume there will never be more than 50 columns per row, but this might be resized during fillup */
  pools_resize(0, RESULT_GRID_WIDGET_MAX_HEIGHT, 0, RESULT_GRID_WIDGET_MAX_HEIGHT * 50);
  row_pool_size= RESULT_GRID_WIDGET_MAX_HEIGHT;
  cell_pool_size= RESULT_GRID_WIDGET_MAX_HEIGHT * 50;

  /*
    Just a note for the archives ...
    Originally I had "grid_main_layout= new QVBoxLayout(client);" here.
    I didn't realize that caused the equivalent of client->setLayout(), apparently.
    So the later pseudo-assertion (checking if client->layout() != 0) happened. Don't need that.
  */
  grid_main_layout= new QVBoxLayout();
  grid_main_layout->setContentsMargins(QMargins(0, 0, 0, 0));   /* this overrides style settings, I suppose */

  grid_main_layout->setSpacing(0);                          /* ?? premature? */

  grid_main_layout->setSizeConstraint(QLayout::SetFixedSize);  /* This ensures the grid columns have no spaces between them */

  client->setLayout(grid_main_layout);

  mysql_res_copy= 0;
  mysql_res_copy_rows= 0;
  set_frame_color_setting();
}


/*
  It takes a long time to make a widget. Therefore we have pools of reusable widgets.
  When the pool is too small, we increase it.
  This can increase the sizes of the new widget pools. It cannot decrease.
*/
void pools_resize(unsigned int old_row_pool_size, unsigned int new_row_pool_size,
                  unsigned int old_cell_pool_size, unsigned int new_cell_pool_size)
{
  QHBoxLayout **tmp_grid_row_layouts;
  QWidget **tmp_grid_row_widgets;
  QTextEdit **tmp_text_edit_widgets;
  QHBoxLayout **tmp_text_edit_layouts;
  TextEditFrame **tmp_text_edit_frames;
  unsigned int i_rp, i_cp;

  if (old_row_pool_size < new_row_pool_size)
  {
    if (old_row_pool_size != 0)
    {
      tmp_grid_row_layouts= new QHBoxLayout*[old_row_pool_size];
      for (i_rp= 0; i_rp < old_row_pool_size; ++i_rp) tmp_grid_row_layouts[i_rp]= grid_row_layouts[i_rp];
      delete [] grid_row_layouts;
      grid_row_layouts= new QHBoxLayout*[new_row_pool_size];
      for (i_rp= 0; i_rp < old_row_pool_size; ++i_rp) grid_row_layouts[i_rp]= tmp_grid_row_layouts[i_rp];
      delete [] tmp_grid_row_layouts;
      tmp_grid_row_widgets= new QWidget*[old_row_pool_size];
      for (i_rp= 0; i_rp < old_row_pool_size; ++i_rp) tmp_grid_row_widgets[i_rp]= grid_row_widgets[i_rp];
      delete [] grid_row_widgets;
      grid_row_widgets= new QWidget*[new_row_pool_size];
      for (i_rp= 0; i_rp < old_row_pool_size; ++i_rp) grid_row_widgets[i_rp]= tmp_grid_row_widgets[i_rp];
      delete [] tmp_grid_row_widgets;
    }
    else
    {
      grid_row_layouts= new QHBoxLayout*[new_row_pool_size];
      grid_row_widgets= new QWidget*[new_row_pool_size];
    }
  }

  if (old_row_pool_size < new_row_pool_size)
  {
    for (i_rp= old_row_pool_size; i_rp < new_row_pool_size; ++i_rp)
    {
      grid_row_layouts[i_rp]= new QHBoxLayout();  /* I had "(client)" here. That caused warnings. */
      grid_row_layouts[i_rp]->setSpacing(0);
      grid_row_layouts[i_rp]->setContentsMargins(QMargins(0, 0, 0, 0));
      grid_row_widgets[i_rp]= new QWidget(this);
      grid_row_widgets[i_rp]->setLayout(grid_row_layouts[i_rp]);
    }
  }

  if (old_cell_pool_size < new_cell_pool_size)
  {
    if (old_cell_pool_size != 0)
    {
      tmp_text_edit_widgets= new QTextEdit*[old_cell_pool_size];
      for (i_cp= 0; i_cp < old_cell_pool_size; ++i_cp) tmp_text_edit_widgets[i_cp]= text_edit_widgets[i_cp];
      delete [] text_edit_widgets;
      text_edit_widgets= new QTextEdit*[new_cell_pool_size];
      for (i_cp= 0; i_cp < old_cell_pool_size; ++i_cp) text_edit_widgets[i_cp]= tmp_text_edit_widgets[i_cp];
      delete [] tmp_text_edit_widgets;
      tmp_text_edit_layouts= new QHBoxLayout*[old_cell_pool_size];
      for (i_cp= 0; i_cp < old_cell_pool_size; ++i_cp) tmp_text_edit_layouts[i_cp]= text_edit_layouts[i_cp];
      delete [] text_edit_layouts;
      text_edit_layouts= new QHBoxLayout*[new_cell_pool_size];
      for (i_cp= 0; i_cp < old_cell_pool_size; ++i_cp) text_edit_layouts[i_cp]= tmp_text_edit_layouts[i_cp];
      delete [] tmp_text_edit_layouts;
      tmp_text_edit_frames= new TextEditFrame*[old_cell_pool_size];
      for (i_cp= 0; i_cp < old_cell_pool_size; ++i_cp) tmp_text_edit_frames[i_cp]= text_edit_frames[i_cp];
      delete [] text_edit_frames;
      text_edit_frames= new TextEditFrame*[new_cell_pool_size];
      for (i_cp= 0; i_cp < old_cell_pool_size; ++i_cp) text_edit_frames[i_cp]= tmp_text_edit_frames[i_cp];
      delete [] tmp_text_edit_frames;
    }
    else
    {
      text_edit_widgets= new QTextEdit*[new_cell_pool_size];
      text_edit_layouts= new QHBoxLayout*[new_cell_pool_size];
      text_edit_frames= new TextEditFrame*[new_cell_pool_size];
    }
  }

  if (old_cell_pool_size < new_cell_pool_size)
  {
    for (i_cp= old_cell_pool_size; i_cp < new_cell_pool_size; ++i_cp)
    {
      text_edit_widgets[i_cp]= new QTextEdit();
      text_edit_widgets[i_cp]->setCursor(Qt::ArrowCursor); /* See Note#1 above */
      text_edit_layouts[i_cp]= new QHBoxLayout();
      text_edit_layouts[i_cp]->addWidget(text_edit_widgets[i_cp]);
      text_edit_frames[i_cp]= new TextEditFrame(this, this, i_cp);
      text_edit_frames[i_cp]->setLayout(text_edit_layouts[i_cp]);
    }
  }
}


/* We call fillup() whenever there is a new result set to put up on the result grid widget. */
void fillup(MYSQL_RES *mysql_res, QFont *saved_font, MainWindow *parent, bool mysql_more_results_parameter)
{
  long unsigned int xrow;
  unsigned int col;

  /* mysql_more_results_flag affects whether we use mysql_res directly, or make a copy */
  mysql_more_results_flag= mysql_more_results_parameter;

  /* Some child widgets e.g. text_edit_frames[n] must not be visible because they'd receive paint events too soon. */
  hide();
  is_paintable= 0;

  grid_mysql_res= mysql_res;

  /* Before changing result_column_count -- revert style sheet changes that were done for header cells */
  /* Currently the check about cell pool size is unnecessary, it's just there in case someday the pool is shrinkable. */
  for (unsigned int i_h= 0; i_h < result_column_count; ++i_h)
  {
    if (i_h >= cell_pool_size) break;
    text_edit_frames[0 * result_column_count + i_h]->is_style_sheet_set_flag= false;
  }

  ocelot_grid_color= parent->ocelot_grid_color;
  ocelot_grid_background_color= parent->ocelot_grid_background_color;
  /* ocelot_grid_cell_right_drag_line_size_as_int= parent->ocelot_grid_cell_right_drag_line_size.toInt(); */
  /* ocelot_grid_cell_right_drag_line_color= parent->ocelot_grid_cell_right_drag_line_color; */
  dbms_set_result_column_count();                                  /* this will be the width of the grid */
  result_row_count= mysql_num_rows(grid_mysql_res);                /* this will be the height of the grid */

  pools_resize(row_pool_size, RESULT_GRID_WIDGET_MAX_HEIGHT, cell_pool_size, RESULT_GRID_WIDGET_MAX_HEIGHT * result_column_count);

  if (row_pool_size < RESULT_GRID_WIDGET_MAX_HEIGHT) row_pool_size= RESULT_GRID_WIDGET_MAX_HEIGHT;
  if (cell_pool_size < RESULT_GRID_WIDGET_MAX_HEIGHT * result_column_count) cell_pool_size= RESULT_GRID_WIDGET_MAX_HEIGHT * result_column_count;

  /*
    Dynamic-sized arrays for rows and columns.
    Some are two-dimensional e.g. text_edit_widgets; I'll address its elements with text_edit_widgets[row*col+col].
    For every "new " here, there should be a "delete []" in the clear() or garbage_collect() function.
  */
  /* ... */

  grid_column_widths= new long unsigned int[result_column_count];
  grid_max_column_widths= new long unsigned int[result_column_count];
  grid_column_heights= new unsigned int[result_column_count];
  grid_column_dbms_sources= new unsigned char[result_column_count];
  grid_column_dbms_field_numbers= new unsigned short int[result_column_count];
  /* ... */

  dbms_set_grid_column_sources();                                             /* Todo: this could return an error? */

//  grid_scroll_area= new QScrollArea(this);                                    /* Todo: see why parent can't be client */

//  grid_scroll_area->verticalScrollBar()->setMaximum(result_row_count);
//  grid_scroll_area->verticalScrollBar()->setSingleStep(1);
//  grid_scroll_area->verticalScrollBar()->setPageStep(result_row_count / 10);    /* Todo; check if this could become 0 */
  grid_vertical_scroll_bar_value= -1;

  scan_rows();

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

  ocelot_grid_cell_right_drag_line_size_as_int= copy_of_parent->ocelot_grid_cell_right_drag_line_size.toInt();
//  ocelot_grid_cell_right_drag_line_color= copy_of_parent->ocelot_grid_cell_right_drag_line_color;
  ocelot_grid_cell_border_size_as_int= copy_of_parent->ocelot_grid_cell_border_size.toInt();

  /*
    Making changes for all in the cell pool.
    Todo: This should only be done for new cells, or if something has changed e.g. font, drag line size.
          That's actually a bug, because the drag line color doesn't change immediately.
    todo: why the whole pool rather than just result-row_count + 1?
  */
  QFontMetrics fm= QFontMetrics(*saved_font);

  for (xrow= 0; (xrow < result_row_count + 1) && (xrow < RESULT_GRID_WIDGET_MAX_HEIGHT); ++xrow)
  {
    for (unsigned int column_number= 0; column_number < result_column_count; ++column_number)
    {
      int ki= xrow * result_column_count + column_number;
      text_edit_widgets[ki]->setMinimumWidth(fm.width("W") * 3);
      text_edit_widgets[ki]->setMinimumHeight(fm.height() * 2);
      text_edit_layouts[ki]->setContentsMargins(QMargins(0, 0, ocelot_grid_cell_right_drag_line_size_as_int, ocelot_grid_cell_right_drag_line_size_as_int));
      /*
        Change the color of the frame. Be specific that it's TextEditFrame, because you don't want the
        children e.g. the QTextEdit to inherit the color. TextEditFrame is a custom widget and therefore
        setStyleSheet is troublesome for most settings, but background-color should be okay, see
        http://stackoverflow.com/questions/7276330/qt-stylesheet-for-custom-widget.
      */

      //text_edit_frames[ki]->setStyleSheet(frame_color_setting);

      text_edit_frames[ki]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);   /* This doesn't seem to do anything */

      /* Todo: I'm not sure exactly where the following three lines should go. Consider moving them. */
      /* border_size and minimum_width and minimum_height are used by mouseMoveEvent */
      text_edit_frames[ki]->border_size= 10 + border_size;    /* Todo: should just be border_size!! */
      text_edit_frames[ki]->minimum_width= fm.width("W") * 3 + border_size;
      text_edit_frames[ki]->minimum_height= fm.height() * 2 + border_size; /* todo: this is probably too much */
    }
  }

  /* Set header text and colour. We will revert some of these changes during garbage_collect. */
  QString yy;
  for (unsigned int i_h= 0; i_h < result_column_count; ++i_h)                      /* row 0 is header */
  {
    text_edit_frames[0 * result_column_count + i_h]->is_style_sheet_set_flag= false;
    /* Todo: we use set>StyleSheet, apparently that's overriding this. */
    //text_edit_widgets[0 * result_column_count + i_h]->setStyleSheet(copy_of_parent->ocelot_grid_header_style_string);
    /*
    QPalette p= text_edit_widgets[0*result_column_count+i]->palette();
     p.setColor(QPalette::Base, QColor(copy_of_parent->ocelot_grid_header_background_color));
     text_edit_widgets[0*result_column_count+i]->setPalette(p);
     */
    //yy= dbms_get_field_name(i_h); /* fields[i].name; */ /* Todo: use name_length somehow so we don't get fooled by \0 */

    /* We don't do the following because header might get lost if there are multiple result sets. */
    //text_edit_widgets[0 * result_column_count + i_h]->pointer= fields[i].name;
    //text_edit_widgets[0 * result_column_count + i_h]->length= fields[i].name_length;
    text_edit_widgets[0 * result_column_count + i_h]->setText(QString::fromUtf8(fields[i_h].name, fields[i_h].name_length));

    //text_edit_widgets[0 * result_column_count + i_h]->setPlainText(yy);
    text_edit_frames[0 * result_column_count + i_h]->is_retrieved_flag= true;
    text_edit_frames[0 * result_column_count + i_h]->ancestor_grid_column_number= i_h;
    text_edit_frames[0 * result_column_count + i_h]->ancestor_grid_row_number= -1;          /* means header row */
  }

  /*
    grid detail rows
    While we're passing through, we also get max column lengths (in characters).
    Todo: Take into account: whether there were any nulls.
  */
  fill_detail_widgets(0);                                             /* details */

  /* We'll use the automatic scroll bar for small result sets, we'll use our own scroll bar for large ones. */
  if (result_row_count + 1 <= RESULT_GRID_WIDGET_MAX_HEIGHT)
  {
    grid_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    grid_vertical_scroll_bar->setVisible(false);
  }
  else
  {
    grid_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    grid_vertical_scroll_bar->setVisible(true);
  }

  is_paintable= 1;

  grid_column_size_calc(saved_font, ocelot_grid_cell_border_size_as_int,
                        ocelot_grid_cell_right_drag_line_size_as_int); /* get grid_column_widths[] and grid_column_heights[] */


  /*
    grid_actual_grid_height_in_rows = # of rows that are actually showable at a time,
    = lesser of (grid_max_grid_height_in_lines/grid_max_row_height_in_lines, # of rows in result set + 1)
  */
  //grid_actual_grid_height_in_rows= result_row_count;
  //if (grid_actual_grid_height_in_rows > result_row_count + 1) grid_actual_grid_height_in_rows= result_row_count + 1;

//  unsigned int i;

  /* Put the QTextEdit widgets in a layout. Remember grid row 0 is for the header. */
  /*
    Re making the row. Each row is [column_count] cells within one QHBoxLayout within one widget.
    grid_row_layout->setSizeConstraint(QLayout::SetMaximumSize) prevents gaps from forming during shrink.
    There's a "border", actually the visible part of TextEditFrame, on the cell's right.
    Drag it left to shrink the cell, drag it right to expand the cell.
    We do not resize cells on the left or right of the to-be-dragged cell, so expanding causes total row width to expand,
    possibly going beyond the original desired maximum width, possibly causing a horizontal scroll bar to appear.
    grid_row_layout->setSpacing(0) means the only thing separating cells is the "border".
  */
  for (long unsigned int xrow= 0; (xrow < result_row_count + 1) && (xrow < RESULT_GRID_WIDGET_MAX_HEIGHT); ++xrow)
  {
//    grid_main_layout->addWidget(grid_row_widgets[xrow]);
    for (col= 0; col < result_column_count; ++col)
    {
      QTextEdit *l= text_edit_widgets[xrow * result_column_count + col];
//    TextEditFrame *f= text_edit_frames[xrow * result_column_count + col];
      /*
        I'll right-align if type is number and this isn't the header.
        But I read somewhere that might not be compatible with wrapping,
        so I'll wrap only for non-number.
        Do not assume it's left-aligned otherwise; there's a pool.
      */
      /* Todo: some other types e.g. BLOBs might also need special handling. */
      if ((xrow > 0) && (dbms_get_field_flag(col) & NUM_FLAG))
      {
        l->document()->setDefaultTextOption(QTextOption(Qt::AlignRight));
        l->setAlignment(Qt::AlignRight);
      }
      else
      {
        l->document()->setDefaultTextOption(QTextOption(Qt::AlignLeft));
        l->setAlignment(Qt::AlignLeft);
        l->setWordWrapMode(QTextOption::WrapAnywhere);
      }
      /* l->setMaximumWidth(grid_column_widths[col]); */
      /* Height border size = 1 due to setStyleSheet earlier; right border size is passed */
      if (xrow == 0)
      {
//        f->setFixedSize(grid_column_widths[col], max_height_of_a_char + (border_size * 2) + 9);
//        f->setMaximumHeight(max_height_of_a_char+(border_size * 2) + 10);
//        f->setMinimumHeight(max_height_of_a_char+(border_size * 2) + 10);
          int header_height= max_height_of_a_char
                           + ocelot_grid_cell_border_size_as_int * 2
                           + ocelot_grid_cell_right_drag_line_size_as_int;
          if (ocelot_grid_cell_right_drag_line_size_as_int > 0) header_height+= max_height_of_a_char;
          text_edit_frames[xrow * result_column_count + col]->setFixedSize(grid_column_widths[col], header_height);
          text_edit_frames[xrow * result_column_count + col]->setMaximumHeight(header_height);
          text_edit_frames[xrow * result_column_count + col]->setMinimumHeight(header_height);
      }
      else
      {
        /* l->setMinimumHeight(max_height_of_a_char+(border_size*2) + 10); */
        text_edit_frames[xrow * result_column_count + col]->setFixedSize(grid_column_widths[col], grid_column_heights[col]);
        text_edit_frames[xrow * result_column_count + col]->setMaximumHeight(grid_column_heights[col]);
        text_edit_frames[xrow * result_column_count + col]->setMinimumHeight(grid_column_heights[col]);
//          f->setFixedSize(grid_column_widths[col], grid_height_of_highest_column_in_pixels);
//          f->setMaximumHeight(grid_height_of_highest_column_in_pixels);
//          f->setMinimumHeight(grid_height_of_highest_column_in_pixels);
      }
//      text_edit_frames[xrow * result_column_count + col]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);   /* This doesn't seem to do anything */

      text_edit_frames[xrow * result_column_count + col]->show();
      /* l->setMinimumWidth(1); */
      /*
        Todo: setStyleSheet() had to be removed because if it's here, setting of header palette won't work.
        I think the solution is: use setStyleSheet always, although then setFont won't work.
        And is there a way to find out what stylesheet settings are in use by QTableWidget?
      */
      /* l->setStyleSheet("border: 1px solid yellow"); */ /* just so I know the size of the border. Todo: user-settable? */
      grid_row_layouts[xrow]->addWidget(text_edit_frames[xrow * result_column_count + col], 0, Qt::AlignTop | Qt::AlignLeft);

 //     l->updateGeometry();    /* removed 2015-03-04. doesn't seem to matter any more */
      }
    }

  /* How many text_edit_frame widgets are we actually using? This assumes number-of-columns-per-row is fixed. */
  max_text_edit_frames_count= (result_row_count + 1) * result_column_count;

//  grid_main_layout->setSizeConstraint(QLayout::SetFixedSize);  /* This ensures the grid columns have no spaces between them */

  /* TEST: ONLY 1! */
  for (long unsigned int xrow= 0; (xrow < result_row_count + 1) && (xrow < RESULT_GRID_WIDGET_MAX_HEIGHT); ++xrow)
  {
//    grid_row_widgets[xrow]->setLayout(grid_row_layouts[xrow]);
    grid_main_layout->addWidget(grid_row_widgets[xrow], 0, Qt::AlignTop | Qt::AlignLeft);
  }

//  client->setLayout(grid_main_layout);

  /* This doesn't work. Done too early? */
  /* client->setStyleSheet(copy_of_parent->ocelot_grid_style_string); */

  /*
    Before client->show(), client->height()=30 and height()=30.
    After client->show(), client->height()=something big e.g. 1098 and height()=30.
    But client = the grid itself rather than the result grid widget?
    Without client->show(), grid becomes blank after a font change.
  */
  client->show();

//  grid_scroll_area->setWidget(client);
//  grid_scroll_area->setWidgetResizable(true);              /* Without this, the QTextEdit widget heights won't change */

  /* area->horizontalScrollBar()->setSingleStep(client->width() / 24); */ /* single-stepping seems pointless */
}

/*
  Headings (not yet implemented, just intended)
  --------

  This is a row with headings-at-top.
  +-------+---------+-----------+
  |  A    |   B     |   C       |
  +-------+---------+-----------+
  | 1111  | 2222    | 3333      |
  +-------+---------+-----------+
  This is the same row with headings-at-left.
  +-------+---------------------+
  | A     |  1111               |
  | B     |  2222               |
  | C     |  3333               |
  +-------+---------------------+
  Headings-at-top is default.
  Headings-at-left is what you get with \G.
  There's also a menu item to "pivot".
  ? Should the right edge be ragged if headings-at-left?

  Other headings settings:
    Headings suppressed
    Heading width maximum (lines)
    Heading height maximum (lines)

  ? Re-use text_edit_frames[].

  But, for this test, all we'll do is
  put up one row.
  Todo: expandable widgets (drag lines again?)
  Todo: stretch factors?
  Todo: scroll bar
  Todo: sometimes you don't want width(), you want frameGeometry().width()
  The following code was part of an experiment for \G style or for clicking to zoom on a row. Come back to it someday.
*/
//void experiment()
//{
//  QHBoxLayout *hbox_layout[100];
//  QWidget *hbox_widget[100];
//  QVBoxLayout *vbox_layout;
//  QWidget *widget;
//  unsigned int i;
//  unsigned int row_number;
//
//  row_number= 3;
//  vbox_layout= new QVBoxLayout();
//
//  vbox_layout->setContentsMargins(QMargins(0, 0, 0, 0));   /* Todo: find out whether this does anything */
//  vbox_layout->setSpacing(0);                              /* Todo: find out whether this does anything */
//  vbox_layout->setSizeConstraint(QLayout::SetFixedSize);   /* Todo: find out whether this does anything */
//  vbox_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft); /* Todo: find out whether this does anything */
//  for (i= 0; i < result_column_count; ++i)
//  {
//    hbox_layout[i]= new QHBoxLayout();
//
//    hbox_layout[i]->setContentsMargins(QMargins(0, 0, 0, 0));   /* Todo: find out whether this does anything */
//    hbox_layout[i]->setSpacing(0);                              /* Todo: find out whether this does anything */
//    hbox_layout[i]->setSizeConstraint(QLayout::SetFixedSize);   /* Todo: find out whether this does anything */
//    hbox_layout[i]->setAlignment(Qt::AlignTop | Qt::AlignLeft); /* Todo: find out whether this does anything */
//    hbox_layout[i]->addWidget(text_edit_frames[i]);
//    hbox_layout[i]->addWidget(text_edit_frames[(result_column_count * row_number) + i]);
//    hbox_widget[i]= new QWidget();
//    hbox_widget[i]->setLayout(hbox_layout[i]);
//    vbox_layout->addWidget(hbox_widget[i]);
//  }
//  widget= new QWidget();
//  widget->setLayout(vbox_layout);
//  widget->show();
//}


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
  We know the defined width (dbms_get_field_length will give us fields[i].length)
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
   Extra size
   I'm saying "if width<2 then width=2" and "if height<2 then height=2".
   If I don't, then the drag line doesn't appear.
   It seems I don't need to do this if drag line size = 0.
   Todo: Maybe the problem is that Qt is allowing for a scroll bar,
   but in that case the addition should be size of scroll bar
   rather than size of char, i.e. QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent)
   and in that case the scroll bar policy could be changed too.
   Todo: Other than that, I can't figure out what else is screwing up the
   calculations, but perhaps lineSpacing() shouldn't be added if there is only one line.
   I know there's a line = text_edit_widgets[ki]->setMinimumHeight(fm.height() * 2);
   but removing it doesn't solve the problem.
*/
void grid_column_size_calc(QFont *saved_font, int ocelot_grid_cell_border_size_as_int, int ocelot_grid_cell_right_drag_line_size_as_int)
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
    Take it that grid_column_widths[i] = defined column width or max actual column width.
    If this is good enough, then grid_row_heights[i] = 1 char and column width = grid_column_widths[i] chars.
    Todo: the lengths are in bytes; take into account that they might arrive in a multi-byte character set.
  */
  for (i= 0; i < result_column_count; ++i)
  {
    grid_column_widths[i]= dbms_get_field_name_length(i); /* this->fields[i].name_length; */
    if (grid_column_widths[i] < dbms_get_field_length(i)) grid_column_widths[i]= dbms_get_field_length(i); /* fields[i].length */

    /* For explanation of next line, see comment "Extra size". */
    if ((grid_column_widths[i] < 2) && (ocelot_grid_cell_right_drag_line_size_as_int > 0)) grid_column_widths[i]= 2;

    ++grid_column_widths[i]; /* ?? something do do with border width, I suppose */

//    grid_column_widths[i]= grid_column_widths[i] * max_width_of_a_char+(border_size * 2) + 10; /* to pixels */


    grid_column_widths[i]= (grid_column_widths[i] * max_width_of_a_char)
                            + ocelot_grid_cell_border_size_as_int * 2
                            + ocelot_grid_cell_right_drag_line_size_as_int;

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
      unsigned int min_width= (dbms_get_field_name_length(i) + 1) * max_width_of_a_char /* fields[i].name_length */
              + ocelot_grid_cell_border_size_as_int * 2
              + ocelot_grid_cell_right_drag_line_size_as_int;
//              + border_size * 2;
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
    Each column's height = (dbms_get_field_length(i) i.e. fields[i].length) / grid_column_widths[i] rounded up.
    If that's greater than the user-defined maximum, reduce to user-defined maximum
    (the QTextEdit will get a vertical scroll bar if there's an overflow).
  */
  for (i= 0; i < result_column_count; ++i)
  {
    grid_column_heights[i]= (dbms_get_field_length(i) * max_width_of_a_char) / grid_column_widths[i]; /* fields[i].length */
    if ((grid_column_heights[i] * grid_column_widths[i]) < dbms_get_field_length(i)) ++grid_column_heights[i];
    if (grid_column_heights[i] > ocelot_grid_max_column_height_in_lines) grid_column_heights[i]= ocelot_grid_max_column_height_in_lines;
    if (grid_column_heights[i] > grid_actual_row_height_in_lines) grid_actual_row_height_in_lines= grid_column_heights[i];
  }

  grid_height_of_highest_column_in_pixels= 0;

  /* Warning: header-height calculation is also "*(max_height_of_a_char+(border_size*2))", in a different place. */
  /* This calculation of height is horrendously difficult, and still does not seem to be exactly right. */
  for (i= 0; i < result_column_count; ++i)
  {
//    grid_column_heights[i]= (grid_column_heights[i] * (max_height_of_a_char+(border_size * 2))) + 9;
    /* For explanation of next line, see comment "Extra size". */
    if ((grid_column_heights[i] < 2) && (ocelot_grid_cell_right_drag_line_size_as_int > 0)) grid_column_heights[i]= 2;
    grid_column_heights[i]= (grid_column_heights[i] * max_height_of_a_char)
                            + ocelot_grid_cell_border_size_as_int * 2
                            + ocelot_grid_cell_right_drag_line_size_as_int;

    if (grid_column_heights[i] > grid_height_of_highest_column_in_pixels)
    {
      grid_height_of_highest_column_in_pixels= grid_column_heights[i];
    }
  }
}


/*
  If (!mysql_more_results_flag) all we want to know is: max actual length
  If (mysql_more_results_flag) we want to max actual length but also want to make a copy of mysql_res.
*/
void scan_rows()
{
  unsigned int i;
//  unsigned int ki;

  for (i= 0; i < result_column_count; ++i) grid_max_column_widths[i]= 0;

  if (!mysql_more_results_flag)
  {
    mysql_data_seek(grid_mysql_res, 0);
    for (r= 0; r < result_row_count; ++r)
    {
      row= mysql_fetch_row(grid_mysql_res);
      lengths= mysql_fetch_lengths(grid_mysql_res);
      for (i= 0; i < result_column_count; ++i)
      {
        if ((row == 0) || (row[i] == 0))
        {
          if (sizeof(NULL_STRING) - 1 > grid_max_column_widths[i]) grid_max_column_widths[i]= sizeof(NULL_STRING) - 1;
        }
        else
        {
          if (lengths[i] > grid_max_column_widths[i]) grid_max_column_widths[i]= lengths[i];
        }
      }
    }
  return;
  }
  /*
    It's insane that I have to make a copy of what was in mysql_res, = mysql_res_copy.
    But things get complicated if there are multiple result sets i.e. if mysql_more_results is true.
    First loop: find how much to allocate. Allocate. Second loop: fill in with pointers within allocated area.
  */
  unsigned int total_size= 0;
  char *mysql_res_copy_pointer;
  mysql_data_seek(grid_mysql_res, 0);
  for (r= 0; r < result_row_count; ++r)                                /* first loop */
  {
    row= mysql_fetch_row(grid_mysql_res);
    lengths= mysql_fetch_lengths(grid_mysql_res);
    for (i= 0; i < result_column_count; ++i)
    {
//      ki= (r + 1) * result_column_count + i;
      if ((row == 0) || (row[i] == 0))
      {
        total_size+= sizeof(lengths[i]);
        total_size+= sizeof(NULL_STRING) - 1;
      }
      else
      {
        total_size+= sizeof(lengths[i]);
        total_size+= lengths[i];
      }
    }
  }
  mysql_res_copy= new char[total_size];                                              /* allocate */
  mysql_res_copy_rows= new char*[result_row_count];
  mysql_res_copy_pointer= mysql_res_copy;
  mysql_data_seek(grid_mysql_res, 0);
  int null_length= sizeof(NULL_STRING) - 1;
  for (r= 0; r < result_row_count; ++r)                                 /* second loop */
  {
    mysql_res_copy_rows[r]= mysql_res_copy_pointer;
    row= mysql_fetch_row(grid_mysql_res);
    lengths= mysql_fetch_lengths(grid_mysql_res);
    for (i= 0; i < result_column_count; ++i)
    {
//      ki= (r + 1) * result_column_count + i;
      if ((row == 0) || (row[i] == 0))
      {
        if (sizeof(NULL_STRING) - 1 > grid_max_column_widths[i]) grid_max_column_widths[i]= sizeof(NULL_STRING) - 1;
        memcpy(mysql_res_copy_pointer, &null_length, sizeof(null_length));
        mysql_res_copy_pointer+= sizeof(lengths[i]);
        memcpy(mysql_res_copy_pointer,NULL_STRING, sizeof(NULL_STRING) - 1);
        mysql_res_copy_pointer+= sizeof(NULL_STRING) - 1;
      }
      else
      {
        if (lengths[i] > grid_max_column_widths[i]) grid_max_column_widths[i]= lengths[i];
        memcpy(mysql_res_copy_pointer, &lengths[i], sizeof(lengths[i]));
        mysql_res_copy_pointer+= sizeof(lengths[i]);
        memcpy(mysql_res_copy_pointer, row[i], lengths[i]);
        mysql_res_copy_pointer+= lengths[i];
      }
    }
  }
}


/*
  Put lengths and pointers in text_edit_frames.
  Set a flag to say "not retrieved yet", that happens at paint time.
  Todo: make a copy if there are multiple result sets.
  Todo: this points directly to a mysql_res row, ignoring the earlier clever ideas in dbms_get_field_value().
*/
/* The big problem is that setVerticalSpacing(0) goes awry if I use hide(). */

void fill_detail_widgets(int first_row)
{
  unsigned int i;
  unsigned int ki;
  unsigned int grid_row;

  if (!mysql_more_results_flag)
  {
    mysql_data_seek(grid_mysql_res, first_row);
    for (r= first_row, grid_row= 1; (r < result_row_count) && (grid_row < RESULT_GRID_WIDGET_MAX_HEIGHT); ++r, ++grid_row)
    {
      row= mysql_fetch_row(grid_mysql_res);
      lengths= mysql_fetch_lengths(grid_mysql_res);
      for (i= 0; i < result_column_count; ++i)
      {
        ki= grid_row * result_column_count + i;
        if ((row == 0) || (row[i] == 0))
        {
          text_edit_frames[ki]->length= sizeof(NULL_STRING) - 1;
          text_edit_frames[ki]->pointer_to_content= 0;
//          if (sizeof(NULL_STRING) - 1 > grid_max_column_widths[i]) grid_max_column_widths[i]= sizeof(NULL_STRING) - 1;
        }
        else
        {
          text_edit_frames[ki]->length= lengths[i];
          text_edit_frames[ki]->pointer_to_content= row[i];
//          if (lengths[i] > grid_max_column_widths[i]) grid_max_column_widths[i]= lengths[i];
        }
        text_edit_frames[ki]->is_retrieved_flag= false;
        text_edit_frames[ki]->ancestor_grid_column_number= i;
        text_edit_frames[ki]->ancestor_grid_row_number= r;
        text_edit_frames[ki]->show();
      }
    }
    for (grid_row= grid_row; grid_row < RESULT_GRID_WIDGET_MAX_HEIGHT; ++grid_row) /* so if scroll bar goes past end we won't see these */
    {
      for (i= 0; i < result_column_count; ++i)
      {
        ki= grid_row * result_column_count + i;
        text_edit_frames[ki]->hide();
      }
    }
  return;
  }

  char *row_pointer;
//  int length;
  for (r= first_row, grid_row= 1; (r < result_row_count) && (grid_row < RESULT_GRID_WIDGET_MAX_HEIGHT); ++r, ++grid_row)
  {
    row_pointer= mysql_res_copy_rows[r];
//    lengths= mysql_fetch_lengths(grid_mysql_res);
    for (i= 0; i < result_column_count; ++i)
    {
      ki= grid_row * result_column_count + i;
      memcpy(&(text_edit_frames[ki]->length), row_pointer, sizeof(lengths[i]));
      row_pointer+= sizeof(lengths[i]);
      text_edit_frames[ki]->pointer_to_content= row_pointer;
      row_pointer+= text_edit_frames[ki]->length;
      text_edit_frames[ki]->is_retrieved_flag= false;
      text_edit_frames[ki]->ancestor_grid_column_number= i;
      text_edit_frames[ki]->ancestor_grid_row_number= r;
      text_edit_frames[ki]->show();
    }
  }
  for (grid_row= grid_row; grid_row < RESULT_GRID_WIDGET_MAX_HEIGHT; ++grid_row) /* so if scroll bar goes past end we won't see these */
  {
    for (i= 0; i < result_column_count; ++i)
    {
      ki= grid_row * result_column_count + i;
      text_edit_frames[ki]->hide();
    }
  }
}


/*
  Called from eventfilter
  "    if (event->type() == QEvent::FontChange) return (result_grid_table_widget->fontchange_event());"
  Todo: bug: when I switched to Kacst Poster 28pt bold italic, I got scroll bars -- calculations 1 pixel off?
        (not repeated recently, maybe it's fixed now)
*/
int fontchange_event()
{
//  remove_layouts();
//  QFont tmp_font=this->font();
//  put_widgets_in_layouts(&tmp_font);
  return 1;
}


/*
  Called from eventfilter
  "    if (event->type() == QEvent::Show) return (result_grid_table_widget->show_event());"
  This is the more appropriate place to decide about the vertical scroll bar because now height is known.
  Todo: catch QEvent::Resize() too!!
*/
bool show_event()
{
//  vertical_bar_show_as_needed();

  return false;           /* We want the show to happen so pass it on */
}


/*
  Called from eventfilter if and only if result_row_count > RESULT_GRID_WIDGET_MAX_HEIGHT
  There is also an automatic show-as-needed scroll bar, which will come on if scrolling is inevitable for a smaller result set.
  But vertical_scroll_bar_event() is only for the non-automatic vertical scroll bar.
  Initially grid_vertical_scroll_bar_value == -1, it's checked so that we don't paint the initial display twice.
*/
bool vertical_scroll_bar_event()
{
  int new_value;

  /* It's impossible to get here if the scroll bar is hidden, but it happens. Well, maybe only for "turning it off" events. */
  if (grid_vertical_scroll_bar->isVisible() == false)
  {
      return false;
  }

  /* It's ridiculous to do these settings every time. But when is the best time to to them? Which event matters? */
  grid_vertical_scroll_bar->setMaximum(result_row_count - 1);
  grid_vertical_scroll_bar->setSingleStep(1);
  grid_vertical_scroll_bar->setPageStep(1);
  grid_vertical_scroll_bar->setMinimum(0);

  new_value= grid_vertical_scroll_bar->value();

  if (new_value != grid_vertical_scroll_bar_value)
  {
    if ((is_paintable == 1) && (grid_vertical_scroll_bar_value != -1))
    {
      fill_detail_widgets(new_value);
      this->update();      /* not sure if we need to update both this and client, but it should be harmless*/
      client->update();
    }
    grid_vertical_scroll_bar_value= new_value;
    return false;
  }
  return false;
}


/*
  To clean up from a previous result set:
    For each row:
      For each column: remove frame widget from row layout
      Remove row widget from main layout
  Todo: Actually we only need to remove widgets if row-count goes down, add when row-count goes up.
  Todo: Consider whether it would be neater to use the takeAt()
*/
void remove_layouts()
{
  long unsigned int xrow;
  unsigned int col;

  client->hide(); /* client->show() will happen again soon */

  if (grid_main_layout != 0)
  {
    for (xrow= 0; (xrow < result_row_count + 1) && (xrow < RESULT_GRID_WIDGET_MAX_HEIGHT); ++xrow)
    {
      for (col= 0; col < result_column_count; ++col)
      {
        int ki= xrow * result_column_count + col;
        grid_row_layouts[xrow]->removeWidget(text_edit_frames[ki]);
        text_edit_frames[ki]->hide();
      }
      grid_main_layout->removeWidget(grid_row_widgets[xrow]);
    }
  }
}


/*
  We'll do our own garbage collecting for non-Qt items.
  Todo: make sure Qt items have parents where possible so that "delete result_grid_table_widget"
        takes care of them.
*/
void garbage_collect()
{
  remove_layouts();
  if (grid_column_widths != 0) { delete [] grid_column_widths; grid_column_widths= 0; }
  if (grid_max_column_widths != 0) { delete [] grid_max_column_widths; grid_max_column_widths= 0; }
  if (grid_column_heights != 0) { delete [] grid_column_heights; grid_column_heights= 0; }
  if (grid_column_dbms_sources != 0) { delete [] grid_column_dbms_sources; grid_column_dbms_sources= 0; }
  if (grid_column_dbms_field_numbers != 0) { delete [] grid_column_dbms_field_numbers; grid_column_dbms_field_numbers= 0; }
  if (mysql_res_copy != 0) { delete [] mysql_res_copy; mysql_res_copy= 0; }
  if (mysql_res_copy_rows != 0) { delete [] mysql_res_copy_rows; mysql_res_copy_rows= 0; }
}


void set_frame_color_setting()
{
  ocelot_grid_cell_right_drag_line_size_as_int= copy_of_parent->ocelot_grid_cell_right_drag_line_size.toInt();
  ocelot_grid_cell_right_drag_line_color= copy_of_parent->ocelot_grid_cell_right_drag_line_color;
  frame_color_setting= "TextEditFrame {background-color: ";
  frame_color_setting.append(ocelot_grid_cell_right_drag_line_color);
  //frame_color_setting.append(";border: 0px");              /* TEST !! */
  frame_color_setting.append("}");
}


/*
  Setting the parent should affect the children.
  But we don't want all text_edit_frames and text_edit_widgets to change because that is slow.
  Let us set a flag which causes change at paint time. with setStyleSheet(copy_of_parent->ocelot_grid_header_style_string);
*/
void set_all_style_sheets()
{

//  this->setStyleSheet(ocelot_grid_style_string);
  unsigned int i_h;

  //QFontMetrics fm= QFontMetrics(*saved_font);
  set_frame_color_setting();
  for (i_h= 0; i_h < cell_pool_size; ++i_h)
  {
    text_edit_frames[i_h]->is_style_sheet_set_flag= false;
    //text_edit_widgets[ki]->setMinimumWidth(fm.width("W") * 3);
    //text_edit_widgets[ki]->setMinimumHeight(fm.height() * 2);
    /* todo: skip following line if ocelot_grid_cell_right_drag_line_size_as_int did not change */
    text_edit_layouts[i_h]->setContentsMargins(QMargins(0, 0, ocelot_grid_cell_right_drag_line_size_as_int, ocelot_grid_cell_right_drag_line_size_as_int));
  }
}


///*
//  If (row height * result_row_count) > ResultGrid widget height) we need a vertical scroll bar.
//  Todo: I'm not sure whether this is adequate if there's a horizontal scroll bar.
//  Todo: I'm not sure whether this is adequate if there's a widget header.
//  Todo: I'm not sure whether the calculation should involve result_row_count  + 1 (to include the header).
//  Todo: Call this not only when show, but also if font change, column/row size change, resize.
//  Todo: Look for a bug! I noticed that vertical scroll bar was missing after a font change. Didn't repeat.
//*/
//void vertical_bar_show_as_needed()
//{
//  int h;
//
//  h= 0;
//  if (result_row_count > 1) h+= grid_height_of_highest_column_in_pixels * result_row_count;
//  if (h > height()) grid_vertical_scroll_bar->show();
//  else  grid_vertical_scroll_bar->hide();
//}

/*
  The functions that start with "dbms_" are supposed to be low level.
  Eventually there might be "#if mysql ... #endif" code inside them.
  Eventually there might be a separate class with all dbms-related calls.
  But we're still a long way from having dbms-independent code here.
  We have these new lists, which are created when we make the grid and deleted when we stop:
    grid_column_dbms_sources[].       = DBMS_SOURCE_IS_MYSQL_FIELD or DBMS_SOURCE_IS_ROW_NUMBER
    grid_column_dbms_field_numbers[]. = mysql field number, or irrelevant
  The idea is that any column can be special, that is, can have a source other than
  the DBMS field. So far the only special column is the row number, experimentally.
*/

/*
  Ordinarily DBMS_ROW_NUMBER_TEST is 0 = off.
  Turn it to 1 = on if testing to get row numbers as leftmost column.
  Todo: If it's 1, then the test will succeed, but is this really the way you want to do things?
  Maybe it's something for "Settings".
  Maybe it should be in the background like a header.
  Maybe you really need "select ocelot.row_number(), * from t;" i.e. you want an application-level function.
*/
#define DBMS_ROW_NUMBER_TEST 0

#define DBMS_SOURCE_IS_MYSQL_FIELD 0
#define DBMS_SOURCE_IS_ROW_NUMBER  1

/*
  dbms_set_result_column_count() gets the number of columns in the grid
  It might be the same as the number of fields returned by mysql_num_fields().
  But if there is an additional field for row-number, ++.
  Todo: check if this will crash if somebody changes the setting for row numbers (if we allow that)?
*/
void dbms_set_result_column_count()
{
  result_column_count= mysql_num_fields(grid_mysql_res);           /* this will be the width of the grid */
  if (DBMS_ROW_NUMBER_TEST == 1) ++result_column_count;
}

void dbms_set_grid_column_sources()
{
  unsigned int column_number;
  unsigned int dbms_field_number;

  fields= mysql_fetch_fields(grid_mysql_res);
  dbms_field_number= 0;
  for (column_number= 0; column_number < result_column_count; ++column_number)
  {
    if (column_number == 0 && DBMS_ROW_NUMBER_TEST == 1) grid_column_dbms_sources[column_number]= DBMS_SOURCE_IS_ROW_NUMBER;
    else
    {
      grid_column_dbms_sources[column_number]= DBMS_SOURCE_IS_MYSQL_FIELD;            /* i.e. use mysql field# */
      grid_column_dbms_field_numbers[column_number]= dbms_field_number;
      ++dbms_field_number;
    }
  }
}

unsigned int dbms_get_field_length(unsigned int column_number)
{
  QString s;
  unsigned int dbms_field_number;

  if (grid_column_dbms_sources[column_number] == DBMS_SOURCE_IS_MYSQL_FIELD)
  {
    dbms_field_number= grid_column_dbms_field_numbers[column_number];
    /* The defined length is fields[dbms_field_number].length. We prefer actual max length which usually is shorter. */
    return grid_max_column_widths[dbms_field_number];
  }
  if (grid_column_dbms_sources[column_number] == DBMS_SOURCE_IS_ROW_NUMBER)
  {
    s= QString::number(result_row_count);
    return s.length();
  }
  return 0; /* to avoid "control reaches end of non-void function" warning */
}

unsigned int dbms_get_field_flag(unsigned int column_number)
{
  unsigned int dbms_field_number;

  if (grid_column_dbms_sources[column_number] == DBMS_SOURCE_IS_MYSQL_FIELD)
  {
    dbms_field_number= grid_column_dbms_field_numbers[column_number];
    return fields[dbms_field_number].flags;
  }
  if (grid_column_dbms_sources[column_number] == DBMS_SOURCE_IS_ROW_NUMBER)
    return NUM_FLAG;
  return 0; /* to avoid "control reaches end of non-void function" warning */
}


QString dbms_get_field_name(unsigned int column_number)
{
  unsigned int dbms_field_number;

  if (grid_column_dbms_sources[column_number] == DBMS_SOURCE_IS_MYSQL_FIELD)
  {
    dbms_field_number= grid_column_dbms_field_numbers[column_number];
    return fields[dbms_field_number].name;
  }
  if (grid_column_dbms_sources[column_number] == DBMS_SOURCE_IS_ROW_NUMBER)
    return "row_number";
  return ""; /* to avoid "control reaches end of non-void function" warning */
}

unsigned int dbms_get_field_name_length(unsigned int column_number)
{
  unsigned int dbms_field_number;

  if (grid_column_dbms_sources[column_number] == DBMS_SOURCE_IS_MYSQL_FIELD)
  {
    dbms_field_number= grid_column_dbms_field_numbers[column_number];
    return fields[dbms_field_number].name_length;
  }
  if (grid_column_dbms_sources[column_number] == DBMS_SOURCE_IS_ROW_NUMBER)
    return 10; /* i.e. length of "row_number" */
  return 0; /* to avoid "control reaches end of non-void function" warning */
}


QString dbms_get_field_value(int row_number, unsigned int column_number)
{
  QString s;
  unsigned int dbms_field_number;

  if (grid_column_dbms_sources[column_number] == DBMS_SOURCE_IS_MYSQL_FIELD)
  {
    dbms_field_number= grid_column_dbms_field_numbers[column_number];
    return row[dbms_field_number];
  }
  if (grid_column_dbms_sources[column_number] == DBMS_SOURCE_IS_ROW_NUMBER)
  {
    s= QString::number(row_number);
    return s;
  }
  return ""; /* to avoid "control reaches end of non-void function" warning */
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
    int prompt_translate_k(QString s, int i);
    unsigned int statement_count;                                            /* used if "prompt \c ..." */
#ifdef DEBUGGER
    unsigned int block_number;                                               /* current line number, base 0 */
#endif
    QString dbms_version; /* Set to "" at start, select version() at connect, maybe display in prompt. */
    QString dbms_database;/* Set to "" at start, select database() at connect, maybe display in prompt. */
    QString dbms_port;/* Set to "" at start, select @@port at connect, maybe display in prompt. */
    QString dbms_current_user;/* Set to "" at start, select current_user() at connect, maybe display in prompt. */
    QString dbms_current_user_without_host;/* Set to "" at start, select current_user() at connect, maybe display in prompt. */
    QString dbms_host; /* Set to "" at start, mysql_get_host_info() at connect, maybe display in prompt. */
    int dbms_connection_id; /* Set to connection_id() at connect */
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

  int current_widget; /* 0 = statement, 1 = grid, 2 = history, 3 = main, as in #define statements earlier */

public:
Settings(int passed_widget_number, MainWindow *parent): QDialog(parent)
{

  /* settings = new QWidget(this); ... this might come later */

  copy_of_parent= parent;                                /* handle_button_for_ok() might use this */
  current_widget= passed_widget_number;

  /* Copy the parent's settings. They'll be copied back to the parent, possibly changed, if the user presses OK. */
  copy_of_parent->new_ocelot_statement_color= copy_of_parent->ocelot_statement_color;
  copy_of_parent->new_ocelot_statement_background_color= copy_of_parent->ocelot_statement_background_color;
  copy_of_parent->new_ocelot_statement_border_color= copy_of_parent->ocelot_statement_border_color;
  copy_of_parent->new_ocelot_statement_font_family= copy_of_parent->ocelot_statement_font_family;
  copy_of_parent->new_ocelot_statement_font_size= copy_of_parent->ocelot_statement_font_size;
  copy_of_parent->new_ocelot_statement_font_style= copy_of_parent->ocelot_statement_font_style;
  copy_of_parent->new_ocelot_statement_font_weight= copy_of_parent->ocelot_statement_font_weight;
  copy_of_parent->new_ocelot_statement_highlight_literal_color= copy_of_parent->ocelot_statement_highlight_literal_color;
  copy_of_parent->new_ocelot_statement_highlight_identifier_color= copy_of_parent->ocelot_statement_highlight_identifier_color;
  copy_of_parent->new_ocelot_statement_highlight_comment_color= copy_of_parent->ocelot_statement_highlight_comment_color;
  copy_of_parent->new_ocelot_statement_highlight_operator_color= copy_of_parent->ocelot_statement_highlight_operator_color;
  copy_of_parent->new_ocelot_statement_highlight_reserved_color= copy_of_parent->ocelot_statement_highlight_reserved_color;
  copy_of_parent->new_ocelot_statement_prompt_background_color= copy_of_parent->ocelot_statement_prompt_background_color;

  copy_of_parent->new_ocelot_grid_color= copy_of_parent->ocelot_grid_color;
  copy_of_parent->new_ocelot_grid_background_color= copy_of_parent->ocelot_grid_background_color;
  copy_of_parent->new_ocelot_grid_border_color= copy_of_parent->ocelot_grid_border_color;
  copy_of_parent->new_ocelot_grid_header_background_color= copy_of_parent->ocelot_grid_header_background_color;
  copy_of_parent->new_ocelot_grid_font_family= copy_of_parent->ocelot_grid_font_family;
  copy_of_parent->new_ocelot_grid_font_size= copy_of_parent->ocelot_grid_font_size;
  copy_of_parent->new_ocelot_grid_font_style= copy_of_parent->ocelot_grid_font_style;
  copy_of_parent->new_ocelot_grid_font_weight= copy_of_parent->ocelot_grid_font_weight;
  copy_of_parent->new_ocelot_grid_cell_border_color= copy_of_parent->ocelot_grid_cell_border_color;
  copy_of_parent->new_ocelot_grid_cell_right_drag_line_color= copy_of_parent->ocelot_grid_cell_right_drag_line_color;
  copy_of_parent->new_ocelot_grid_border_size= copy_of_parent->ocelot_grid_border_size;
  copy_of_parent->new_ocelot_grid_cell_border_size= copy_of_parent->ocelot_grid_cell_border_size;
  copy_of_parent->new_ocelot_grid_cell_right_drag_line_size= copy_of_parent->ocelot_grid_cell_right_drag_line_size;
  copy_of_parent->new_ocelot_history_color= copy_of_parent->ocelot_history_color;
  copy_of_parent->new_ocelot_history_background_color= copy_of_parent->ocelot_history_background_color;
  copy_of_parent->new_ocelot_history_border_color= copy_of_parent->ocelot_history_border_color;
  copy_of_parent->new_ocelot_history_font_family= copy_of_parent->ocelot_history_font_family;
  copy_of_parent->new_ocelot_history_font_size= copy_of_parent->ocelot_history_font_size;
  copy_of_parent->new_ocelot_history_font_style= copy_of_parent->ocelot_history_font_style;
  copy_of_parent->new_ocelot_history_font_weight= copy_of_parent->ocelot_history_font_weight;

  copy_of_parent->new_ocelot_main_color= copy_of_parent->ocelot_main_color;
  copy_of_parent->new_ocelot_main_background_color= copy_of_parent->ocelot_main_background_color;
  copy_of_parent->new_ocelot_main_border_color= copy_of_parent->ocelot_main_border_color;
  copy_of_parent->new_ocelot_main_font_family= copy_of_parent->ocelot_main_font_family;
  copy_of_parent->new_ocelot_main_font_size= copy_of_parent->ocelot_main_font_size;
  copy_of_parent->new_ocelot_main_font_style= copy_of_parent->ocelot_main_font_style;
  copy_of_parent->new_ocelot_main_font_weight= copy_of_parent->ocelot_main_font_weight;

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

  if (current_widget == GRID_WIDGET)
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
      if (ci == 0) combo_box_for_size[0]->setCurrentIndex(copy_of_parent->new_ocelot_grid_border_size.toInt());
      if (ci == 1) combo_box_for_size[1]->setCurrentIndex(copy_of_parent->new_ocelot_grid_cell_border_size.toInt());
      if (ci == 2) combo_box_for_size[2]->setCurrentIndex(copy_of_parent->new_ocelot_grid_cell_right_drag_line_size.toInt());
      hbox_layout_for_size[ci]= new QHBoxLayout();
      hbox_layout_for_size[ci]->addWidget(label_for_size[ci]);
      hbox_layout_for_size[ci]->addWidget(combo_box_for_size[ci]);
    widget_for_size[ci]->setLayout(hbox_layout_for_size[ci]);
    }
    connect(combo_box_for_size[0], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_size_0(int)));
    connect(combo_box_for_size[1], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_size_1(int)));
    connect(combo_box_for_size[2], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_size_2(int)));
  }

  /* I could not get result grid border size to work so it is hidden until someday it is figured out -- maybe never */
  label_for_size[0]->hide();
  combo_box_for_size[0]->hide();

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
  if (current_widget == GRID_WIDGET) for (int ci= 0; ci < 3; ++ci) main_layout->addWidget(widget_for_size[ci]);
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
  if (current_widget == STATEMENT_WIDGET)
  {
    switch (ci)
    {
    case 0: { color_type= tr("Foreground"); color_name= copy_of_parent->new_ocelot_statement_color; break; }
    case 1: { color_type= tr("Background"); color_name= copy_of_parent->new_ocelot_statement_background_color; break; }
    case 2: { color_type= tr("Highlight literal"); color_name= copy_of_parent->new_ocelot_statement_highlight_literal_color; break; }
    case 3: { color_type= tr("Highlight identifier"); color_name= copy_of_parent->new_ocelot_statement_highlight_identifier_color; break; }
    case 4: { color_type= tr("Highlight comment"); color_name= copy_of_parent->new_ocelot_statement_highlight_comment_color; break; }
    case 5: { color_type= tr("Highlight operator"); color_name= copy_of_parent->new_ocelot_statement_highlight_operator_color; break; }
    case 6: { color_type= tr("Highlight reserved"); color_name= copy_of_parent->new_ocelot_statement_highlight_reserved_color; break; }
    case 7: { color_type= tr("Prompt background"); color_name= copy_of_parent->new_ocelot_statement_prompt_background_color; break; }
    case 8: { color_type= tr("Border"); color_name= copy_of_parent->new_ocelot_statement_border_color; break; }
    }
  }
  if (current_widget == GRID_WIDGET)
  {
    switch (ci)
    {
    case 0: { color_type= tr("Foreground"); color_name= copy_of_parent->new_ocelot_grid_color; break; }
    case 1: { color_type= tr("Background"); color_name= copy_of_parent->new_ocelot_grid_background_color; break; }
    case 2: { color_type= tr("Cell border"); color_name= copy_of_parent->new_ocelot_grid_cell_border_color; break; }
    case 3: { color_type= tr("Cell right drag line"); color_name= copy_of_parent->new_ocelot_grid_cell_right_drag_line_color; break; }
    case 7: { color_type= tr("Header background"); color_name= copy_of_parent->new_ocelot_grid_header_background_color; break; }
    case 8: { color_type= tr("Border"); color_name= copy_of_parent->new_ocelot_grid_border_color; break; }
    }
  }
  if (current_widget == HISTORY_WIDGET)
  {
    switch (ci)
    {
    case 0: { color_type= tr("Foreground"); color_name= copy_of_parent->new_ocelot_history_color; break; }
    case 1: { color_type= tr("Background"); color_name= copy_of_parent->new_ocelot_history_background_color; break; }
    case 8: { color_type= tr("Border"); color_name= copy_of_parent->new_ocelot_history_border_color; break; }
    }
  }
  if (current_widget == MAIN_WIDGET)
  {
    switch (ci)
    {
    case 0: { color_type= tr("Foreground"); color_name= copy_of_parent->new_ocelot_main_color; break; }
    case 1: { color_type= tr("Background"); color_name= copy_of_parent->new_ocelot_main_background_color; break; }
    case 8: { color_type= tr("Border"); color_name= copy_of_parent->new_ocelot_main_border_color; break; }
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
  if (current_widget == STATEMENT_WIDGET)
  {
    s_for_label_for_font_dialog= copy_of_parent->new_ocelot_statement_font_family;
    s_for_label_for_font_dialog.append(" ");
    s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_statement_font_size);
    s_for_label_for_font_dialog.append("pt");
    if (QString::compare(copy_of_parent->new_ocelot_statement_font_weight, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_statement_font_weight);
    }
    if (QString::compare(copy_of_parent->new_ocelot_statement_font_style, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_statement_font_style);
    }
  }

  if (current_widget == GRID_WIDGET)
  {
    s_for_label_for_font_dialog= copy_of_parent->new_ocelot_grid_font_family;
    s_for_label_for_font_dialog.append(" ");
    s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_grid_font_size);
    s_for_label_for_font_dialog.append("pt");
    if (QString::compare(copy_of_parent->new_ocelot_grid_font_weight, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_grid_font_weight);
    }
    if (QString::compare(copy_of_parent->new_ocelot_grid_font_style, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_grid_font_style);
    }
  }

  if (current_widget == HISTORY_WIDGET)
  {
    s_for_label_for_font_dialog= copy_of_parent->new_ocelot_history_font_family;
    s_for_label_for_font_dialog.append(" ");
    s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_history_font_size);
    s_for_label_for_font_dialog.append("pt");
    if (QString::compare(copy_of_parent->new_ocelot_history_font_weight, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_history_font_weight);
    }
    if (QString::compare(copy_of_parent->new_ocelot_history_font_style, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_history_font_style);
    }
  }

  if (current_widget == MAIN_WIDGET)
  {
    s_for_label_for_font_dialog= copy_of_parent->new_ocelot_main_font_family;
    s_for_label_for_font_dialog.append(" ");
    s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_main_font_size);
    s_for_label_for_font_dialog.append("pt");
    if (QString::compare(copy_of_parent->new_ocelot_main_font_weight, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_main_font_weight);
    }
    if (QString::compare(copy_of_parent->new_ocelot_main_font_style, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_main_font_style);
    }
  }

  label_for_font_dialog->setText(s_for_label_for_font_dialog);
}


private slots:

/* If user clicks OK, end. The caller will move changed "new" settings to non-new. */
void handle_button_for_ok()
{
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
  if (current_widget == STATEMENT_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_statement_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Foreground"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_statement_color= curr_color.name();
      label_for_color_rgb[0]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_statement_color);
      button_for_color_show[0]->setStyleSheet(s);
    }
  }
  if (current_widget == GRID_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_grid_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Foreground"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_grid_color= curr_color.name();
      label_for_color_rgb[0]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_grid_color);
      button_for_color_show[0]->setStyleSheet(s);
    }
  }
  if (current_widget == HISTORY_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_history_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Foreground"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_history_color= curr_color.name();
      label_for_color_rgb[0]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_history_color);
      button_for_color_show[0]->setStyleSheet(s);
    }
  }
  if (current_widget == MAIN_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_main_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Foreground"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_main_color= curr_color.name();
      label_for_color_rgb[0]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_main_color);
      button_for_color_show[0]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_1()
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_statement_background_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Background"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_statement_background_color= curr_color.name();
      label_for_color_rgb[1]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_statement_background_color);
      button_for_color_show[1]->setStyleSheet(s);
    }
  }
  if (current_widget == GRID_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_grid_background_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Background"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_grid_background_color= curr_color.name();
      label_for_color_rgb[1]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_grid_background_color);
      button_for_color_show[1]->setStyleSheet(s);
    }
  }
  if (current_widget == HISTORY_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_grid_background_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Background"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_history_background_color= curr_color.name();
      label_for_color_rgb[1]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_history_background_color);
      button_for_color_show[1]->setStyleSheet(s);
    }
  }
  if (current_widget == MAIN_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_grid_background_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Background"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_main_background_color= curr_color.name();
      label_for_color_rgb[1]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_main_background_color);
      button_for_color_show[1]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_2()
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_statement_highlight_literal_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Highlight literal"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_statement_highlight_literal_color= curr_color.name();
      label_for_color_rgb[2]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_statement_highlight_literal_color);
      button_for_color_show[2]->setStyleSheet(s);
    }
  }

  if (current_widget == GRID_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_grid_cell_border_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Cell border"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_grid_cell_border_color= curr_color.name();
      label_for_color_rgb[2]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_grid_cell_border_color);
      button_for_color_show[2]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_3()
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_statement_highlight_identifier_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Highlight identifier"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_statement_highlight_identifier_color= curr_color.name();
      label_for_color_rgb[3]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_statement_highlight_identifier_color);
      button_for_color_show[3]->setStyleSheet(s);
    }
  }

  if (current_widget == GRID_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_grid_cell_right_drag_line_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Cell drag line"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_grid_cell_right_drag_line_color= curr_color.name();
      label_for_color_rgb[3]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_grid_cell_right_drag_line_color);
       button_for_color_show[3]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_4()
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_statement_highlight_comment_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Highlight comment"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_statement_highlight_comment_color= curr_color.name();
      label_for_color_rgb[4]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_statement_highlight_comment_color);
      button_for_color_show[4]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_5()
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_statement_highlight_operator_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Highlight operator"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_statement_highlight_operator_color= curr_color.name();
      label_for_color_rgb[5]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_statement_highlight_operator_color);
      button_for_color_show[5]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_6()
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_statement_highlight_reserved_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Highlight reserved"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_statement_highlight_reserved_color= curr_color.name();
      label_for_color_rgb[6]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_statement_highlight_reserved_color);
      button_for_color_show[6]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_7()
{
 if (current_widget == STATEMENT_WIDGET)
 {
   QColor curr_color= QColor(copy_of_parent->new_ocelot_statement_prompt_background_color);
   curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Prompt background"), QColorDialog::ShowAlphaChannel);
   if (curr_color.isValid())
   {
     copy_of_parent->new_ocelot_statement_prompt_background_color= curr_color.name();
     label_for_color_rgb[7]->setText(curr_color.name());
     QString s= "background-color: ";
     s.append(copy_of_parent->new_ocelot_statement_prompt_background_color);
     button_for_color_show[7]->setStyleSheet(s);
    }
  }
  if (current_widget == GRID_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_grid_header_background_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Header background"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_grid_header_background_color= curr_color.name();
      label_for_color_rgb[7]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_grid_header_background_color);
      button_for_color_show[7]->setStyleSheet(s);
    }
  }
}


void handle_button_for_color_pick_8()
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_statement_border_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Border"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_statement_border_color= curr_color.name();
      label_for_color_rgb[8]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_statement_border_color);
      button_for_color_show[8]->setStyleSheet(s);
    }
  }
  if (current_widget == GRID_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_grid_border_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Border"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_grid_border_color= curr_color.name();
      label_for_color_rgb[8]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_grid_border_color);
      button_for_color_show[8]->setStyleSheet(s);
    }
  }
  if (current_widget == HISTORY_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_history_border_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Border"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_history_border_color= curr_color.name();
      label_for_color_rgb[8]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_history_border_color);
      button_for_color_show[8]->setStyleSheet(s);
    }
  }
  if (current_widget == MAIN_WIDGET)
  {
    QColor curr_color= QColor(copy_of_parent->new_ocelot_main_border_color);
    curr_color= QColorDialog::getColor(curr_color, this, tr("Pick Color of Border"), QColorDialog::ShowAlphaChannel);
    if (curr_color.isValid())
    {
      copy_of_parent->new_ocelot_main_border_color= curr_color.name();
      label_for_color_rgb[8]->setText(curr_color.name());
      QString s= "background-color: ";
      s.append(copy_of_parent->new_ocelot_main_border_color);
      button_for_color_show[8]->setStyleSheet(s);
    }
  }
}


void handle_combo_box_for_size_0(int i)
{
  copy_of_parent->new_ocelot_grid_border_size= QString::number(i);
}


void handle_combo_box_for_size_1(int i)
{
  copy_of_parent->new_ocelot_grid_cell_border_size= QString::number(i);
}


void handle_combo_box_for_size_2(int i)
{
  copy_of_parent->new_ocelot_grid_cell_right_drag_line_size= QString::number(i);
}


/* Some of the code in handle_button_for_font_dialog() is a near-duplicate of code in set_colors_and_fonts(). */
void handle_button_for_font_dialog()
{
  bool ok;
  QString s;
  QFont font;
  int boldness= QFont::Normal;
  bool italics= false;

  if (current_widget == STATEMENT_WIDGET)
  {
    if (QString::compare(copy_of_parent->new_ocelot_statement_font_weight, "bold") == 0) boldness= QFont::Bold;
    if (QString::compare(copy_of_parent->new_ocelot_statement_font_style, "italic") == 0) italics= true;
    font= QFontDialog::getFont(&ok, QFont(copy_of_parent->new_ocelot_statement_font_family, copy_of_parent->new_ocelot_statement_font_size.toInt(), boldness, italics), this);
  }
  if (current_widget == GRID_WIDGET)
  {
    if (QString::compare(copy_of_parent->new_ocelot_grid_font_weight, "bold") == 0) boldness= QFont::Bold;
    if (QString::compare(copy_of_parent->new_ocelot_grid_font_style, "italic") == 0) italics= true;
    font= QFontDialog::getFont(&ok, QFont(copy_of_parent->new_ocelot_grid_font_family, copy_of_parent->new_ocelot_grid_font_size.toInt(), boldness, italics), this);
  }
  if (current_widget == HISTORY_WIDGET)
  {
    if (QString::compare(copy_of_parent->new_ocelot_history_font_weight, "bold") == 0) boldness= QFont::Bold;
    if (QString::compare(copy_of_parent->new_ocelot_history_font_style, "italic") == 0) italics= true;
    font= QFontDialog::getFont(&ok, QFont(copy_of_parent->new_ocelot_history_font_family, copy_of_parent->new_ocelot_history_font_size.toInt(), boldness, italics), this);
  }
  if (current_widget == MAIN_WIDGET)
  {
    if (QString::compare(copy_of_parent->new_ocelot_main_font_weight, "bold") == 0) boldness= QFont::Bold;
    if (QString::compare(copy_of_parent->new_ocelot_main_font_style, "italic") == 0) italics= true;
    font= QFontDialog::getFont(&ok, QFont(copy_of_parent->new_ocelot_main_font_family, copy_of_parent->new_ocelot_main_font_size.toInt(), boldness, italics), this);
  }

  if (ok)
  {
    /* User clicked OK on QFontDialog */
    if (current_widget == STATEMENT_WIDGET)
    {
      copy_of_parent->new_ocelot_statement_font_family= font.family();
      if (font.italic()) copy_of_parent->new_ocelot_statement_font_style= "italic";
      else copy_of_parent->new_ocelot_statement_font_style= "normal";
      copy_of_parent->new_ocelot_statement_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
      if (font.weight() >= QFont::Bold) copy_of_parent->new_ocelot_statement_font_weight= "bold";
      else copy_of_parent->new_ocelot_statement_font_weight= "normal";
    }
   if (current_widget == GRID_WIDGET)
   {
     copy_of_parent->new_ocelot_grid_font_family= font.family();
     if (font.italic()) copy_of_parent->new_ocelot_grid_font_style= "italic";
     else copy_of_parent->new_ocelot_grid_font_style= "normal";
     copy_of_parent->new_ocelot_grid_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
     if (font.weight() >= QFont::Bold) copy_of_parent->new_ocelot_grid_font_weight= "bold";
     else copy_of_parent->new_ocelot_grid_font_weight= "normal";
   }
   if (current_widget == HISTORY_WIDGET)
   {
     copy_of_parent->new_ocelot_history_font_family= font.family();
     if (font.italic()) copy_of_parent->new_ocelot_history_font_style= "italic";
     else copy_of_parent->new_ocelot_history_font_style= "normal";
     copy_of_parent->new_ocelot_history_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
     if (font.weight() >= QFont::Bold) copy_of_parent->new_ocelot_history_font_weight= "bold";
     else copy_of_parent->new_ocelot_history_font_weight= "normal";
   }
   if (current_widget == MAIN_WIDGET)
   {
     copy_of_parent->new_ocelot_main_font_family= font.family();
     if (font.italic()) copy_of_parent->new_ocelot_main_font_style= "italic";
     else copy_of_parent->new_ocelot_main_font_style= "normal";
     copy_of_parent->new_ocelot_main_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
     if (font.weight() >= QFont::Bold) copy_of_parent->new_ocelot_main_font_weight= "bold";
     else copy_of_parent->new_ocelot_main_font_weight= "normal";
    }
    label_for_font_dialog_set_text();
  }
}

};

/* QThread::msleep is protected in qt 4.8. so you have to say QThread48::msleep */
class QThread48 : public QThread
{
public:
  static void msleep(int ms)
  {
    QThread::msleep(ms);
  }
};
