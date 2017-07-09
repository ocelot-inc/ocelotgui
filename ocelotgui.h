/* Copyright (c) 2014-2017 by Ocelot Computer Services Inc. All rights reserved.

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

#ifndef OCELOTGUI_H
#define OCELOTGUI_H

/* The debugger is integrated now, but "ifdef DEBUGGER" directives help to delineate code that is debugger-specific. */
#define DEBUGGER

/*
  The possible DBMS values. Nowadays these are always defined,
  except Tarantool if Windows. These are related to ocelot_dbms values.
  --ocelot_dbms='mysql' is default.
  --ocelot_dbms='mariadb' is non-default but officially supported.
  --ocelot_dbms='tarantool' is non-default and experimental.
  If we start as MySQL but then connect to MariaDB, or vice versa,
  it's okay because we change to what we connected to.
  If --ocelot_dbms='tarantool',
  connection is to a Tarantool server not a MySQL/MariaDB server, but
  wouldn't work on Windows because Tarantool library is not available.
*/
#define DBMS_MYSQL 1
#define DBMS_MARIADB 2
#ifdef __linux
#define DBMS_TARANTOOL 3
#endif
#define FLAG_VERSION_MYSQL_5_5      1
#define FLAG_VERSION_MYSQL_5_6      2
#define FLAG_VERSION_MYSQL_5_7      4
#define FLAG_VERSION_MYSQL_8_0      8
#define FLAG_VERSION_MYSQL_ALL      (1 | 2 | 4 | 8)
#define FLAG_VERSION_MARIADB_5_5    16
#define FLAG_VERSION_MARIADB_10_0   32
#define FLAG_VERSION_MARIADB_10_1   64
#define FLAG_VERSION_MARIADB_10_2_2 128
#define FLAG_VERSION_MARIADB_10_2_3 256
#define FLAG_VERSION_MARIADB_10_3   512
#define FLAG_VERSION_MARIADB_ALL    (16 | 32 | 64 | 128 | 256 | 512)
#define FLAG_VERSION_MYSQL_OR_MARIADB_ALL (1 | 2 | 4 | 8 | 16 | 32 | 64 | 128 | 256 | 512)
#define FLAG_VERSION_TARANTOOL      1024
#define FLAG_VERSION_ALL (1 | 2 | 4 | 8 | 16 | 32 | 64 | 128 | 256 | 512 | 1024)
#define FLAG_VERSION_LUA            2048
#define FLAG_VERSION_ALL_OR_LUA (FLAG_VERSION_ALL | FLAG_VERSION_LUA)

#include <assert.h>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/* All Qt includes go here. Most of them could be handled by just saying "#include <QtWidgets>". */
#include <QAbstractItemView>
#ifndef __linux
#include <QApplication>
#endif
#include <QClipboard>
#include <QComboBox>
#include <QDateTime>
#include <QDialog>
#include <QDir>
#include <QFontDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#ifndef __linux
#include <QLibrary>
#endif
//#include <QLibraryInfo>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextEdit>
#include <QThread>
#include <QTimer>
//#include <QWidget>
#include <QTextStream>
#include <QDesktopWidget>

/* Several possible include paths for mysql.h are hard coded in ocelotgui.pro. */
#include <mysql.h>

/* Strangely MYSQL_PORT might not be brought in by #include <mysql.h> */
#ifndef MYSQL_PORT
#define MYSQL_PORT 3306
#endif

/*
  Linux-specific:
  We use dlopen() when opening libmysqlclient.so and libcrypto.so, therefore include dlfcn.h.
  We use readlink() when checking if histfile links to dev/null, therefore include unistd.h.
  We use getpwuid() when getting password, therefore include pwd.h.
  We use pthread_create() for debug and kill, therefore include pthread.h.
  We use stat() to see if a configuration file is world-writable, therefore include stat.h.
  Todo: Maybe we should be looking for Qt's Q_OS_LINUX etc. not __linux etc.
*/
#ifdef __linux
#include <dlfcn.h>
#include <unistd.h>
#include <pwd.h>
#include <pthread.h>
#include <sys/stat.h>
#endif

#ifdef DBMS_TARANTOOL
#include <stdint.h>
/*
  Definitions of tnt_opt_type and tnt_reply are taken from tarantool-c.
  Although there is no copyright, there is a request for an AS IS notice (which is done)
*/
  enum tnt_opt_type {
          TNT_OPT_URI,
          TNT_OPT_TMOUT_CONNECT,
          TNT_OPT_TMOUT_RECV,
          TNT_OPT_TMOUT_SEND,
          TNT_OPT_SEND_CB,
          TNT_OPT_SEND_CBV,
          TNT_OPT_SEND_CB_ARG,
          TNT_OPT_SEND_BUF,
          TNT_OPT_RECV_CB,
          TNT_OPT_RECV_CB_ARG,
          TNT_OPT_RECV_BUF
  };
  struct tnt_reply {
          int alloc;
          uint64_t bitmap;
          const char *buf;
          size_t buf_size;
          uint64_t code;
          uint64_t sync;
          uint64_t schema_id;
          const char *error;
          const char *error_end;
          const char *data;
          const char *data_end;
  };
  /* Amazing but true: write_request was silently added after release! */
  struct tnt_stream {
          int alloc; /*!< Allocation mark */
          ssize_t (*write)(struct tnt_stream *s, const char *buf, size_t size); /*!< write to buffer function */
          ssize_t (*writev)(struct tnt_stream *s, struct iovec *iov, int count); /*!< writev function */
          ssize_t (*write_request)(struct tnt_stream *s, struct tnt_request *r, uint64_t *sync); /*!< write request function */
          ssize_t (*read)(struct tnt_stream *s, char *buf, size_t size); /*!< read from buffer function */
          int (*read_reply)(struct tnt_stream *s, struct tnt_reply *r); /*!< read reply from buffer */
          void (*free)(struct tnt_stream *s); /*!< free custom buffer types (destructor) */
          void *data; /*!< subclass data */
          uint32_t wrcnt; /*!< count of write operations */
          uint64_t reqid; /*!< request id of current operation */
  };

  enum  	mp_type {
    MP_NIL = 0, MP_UINT, MP_INT, MP_STR,
    MP_BIN, MP_ARRAY, MP_MAP, MP_BOOL,
    MP_FLOAT, MP_DOUBLE, MP_EXT
  };
  /**
   * \brief Request types
   */
  enum tnt_request_t {
      TNT_OP_SELECT = 1,
      TNT_OP_INSERT = 2,
      TNT_OP_REPLACE = 3,
      TNT_OP_UPDATE = 4,
      TNT_OP_DELETE = 5,
      TNT_OP_CALL = 6,
      TNT_OP_AUTH = 7,
      TNT_OP_EVAL = 8,
      TNT_OP_UPSERT = 9,
      TNT_OP_PING = 64,
      TNT_OP_JOIN = 65,
      TNT_OP_SUBSCRIBE = 66
  };
  /**
   * \brief Iterator types
   */
  enum tnt_iterator_t {
      TNT_ITER_EQ = 0,
      TNT_ITER_REQ,
      TNT_ITER_ALL,
      TNT_ITER_LT,
      TNT_ITER_LE,
      TNT_ITER_GE,
      TNT_ITER_GT,
      TNT_ITER_BITS_ALL_SET,
      TNT_ITER_BITS_ANY_SET,
      TNT_ITER_BITS_ALL_NOT_SET,
      TNT_ITER_OVERLAP,
      TNT_ITER_NEIGHBOR,
  };
  struct tnt_request {
      struct {
          uint32_t sync; /*!< Request sync id. Generated when encoded */
          enum tnt_request_t type; /*!< Request type */
      } hdr; /*!< fields for header */
      uint32_t space_id; /*!< Space number */
      uint32_t index_id; /*!< Index number */
      uint32_t offset; /*!< Offset for select */
      uint32_t limit; /*!< Limit for select */
      enum tnt_iterator_t iterator; /*!< Iterator for select */
      /* Search key, proc name or eval expression */
      const char *key; /*!< Pointer for
                * key for select/update/delete,
                * procedure  for call,
                * expression for eval,
                * operations for upsert
                */
      const char *key_end;
      struct tnt_stream *key_object; /*!< Pointer for key object
                      * if allocated inside requests
                      * functions
                      */
      const char *tuple; /*!< Pointer for
                  * tuple for insert/replace,
                  * ops for update
                  * default tuple for upsert,
                  * args for eval/call
                  */
      const char *tuple_end;
      struct tnt_stream *tuple_object; /*!< Pointer for tuple object
                        * if allocated inside requests
                        * functions
                        */
      int index_base; /*!< field offset for UPDATE */
      int alloc; /*!< allocation mark */
  };

#endif

/* Flags used for row_form_box. NUM_FLAG is also defined in mysql include, with same value. */
#define READONLY_FLAG 1
#define NUM_FLAG 32768

/*
  Most ocelot_ variables are in ocelotgui.cpp but if one is required by ocelotgui.h, say so here.
  Weirdly, ocelotgui.h is included in two places, ocelotgui.pro + ocelotgui.cpp,
  so say 'static' if you do that.
*/

/* References to ostrings.h variables and pseudo-constants */
extern const char *menu_strings[];
extern int MENU_STATEMENT_TEXT_COLOR;
extern int MENU_STATEMENT_BACKGROUND_COLOR;
extern int MENU_STATEMENT_HIGHLIGHT_LITERAL_COLOR;
extern int MENU_STATEMENT_HIGHLIGHT_IDENTIFIER_COLOR;
extern int MENU_STATEMENT_HIGHLIGHT_COMMENT_COLOR;
extern int MENU_STATEMENT_HIGHLIGHT_OPERATOR_COLOR;
extern int MENU_STATEMENT_HIGHLIGHT_KEYWORD_COLOR;
extern int MENU_STATEMENT_PROMPT_BACKGROUND_COLOR;
extern int MENU_STATEMENT_BORDER_COLOR;
extern int MENU_STATEMENT_HIGHLIGHT_CURRENT_LINE_COLOR;
extern int MENU_STATEMENT_HIGHLIGHT_FUNCTION_COLOR;
extern int MENU_GRID_TEXT_COLOR;
extern int MENU_GRID_BACKGROUND_COLOR;
extern int MENU_GRID_CELL_BORDER_COLOR;
extern int MENU_GRID_CELL_DRAG_LINE_COLOR;
extern int MENU_GRID_HEADER_BACKGROUND_COLOR;
extern int MENU_GRID_BORDER_COLOR;
extern int MENU_GRID_BORDER_SIZE;
extern int MENU_GRID_CELL_BORDER_SIZE;
extern int MENU_GRID_CELL_DRAG_LINE_SIZE;
extern int MENU_GRID_TEXT_COLOR;
extern int MENU_GRID_BACKGROUND_COLOR;
extern int MENU_HISTORY_TEXT_COLOR;
extern int MENU_HISTORY_BACKGROUND_COLOR;
extern int MENU_HISTORY_BORDER_COLOR;
extern int MENU_MENU_TEXT_COLOR;
extern int MENU_MENU_BACKGROUND_COLOR;
extern int MENU_MENU_BORDER_COLOR;
extern int MENU_FONT;
extern int MENU_MAX_ROW_COUNT;
extern int MENU_SYNTAX_CHECKER;
extern int MENU_CONDITION;
extern int MENU_DISPLAY_AS;
extern int MENU_CANCEL;
extern int MENU_OK;
extern int MENU_SETTINGS_FOR_MENU;
extern int MENU_SETTINGS_FOR_HISTORY;
extern int MENU_SETTINGS_FOR_GRID;
extern int MENU_SETTINGS_FOR_STATEMENT;
extern int MENU_SETTINGS_FOR_EXTRA_RULE_1;
extern int MENU_PICK_NEW_FONT;

extern unsigned int menu_off;

namespace Ui
{
class MainWindow;
}

QT_BEGIN_NAMESPACE
//class QAction;
//class QLabel;
//class QMenu;
//class QMenuBar;
//class QComboBox;
//class QPushButton;
//class QTextEdit;
//class QPlainTextEdit;
//class QScrollArea;
//class QVBoxLayout;
class CodeEditor;
class ResultGrid;
class Settings;
class TextEditFrame;
class TextEditWidget;
class QScrollAreaWithSize;
class QThread48;
class QTabWidget48;
class TextEditHistory;
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
    and most may be changed with the SET statement. For example,
    ocelot_grid_cell_drag_line_color is on menu item Settings | Grid Widget,
    SET ocelot_grid_cell_drag_line_color = value; will change.
    The important thing is: if it's changed on the Settings menu, then
    a SET statement is generated, so that can be saved and replayed.
    Naming convention: ocelot_ + settings-menu-item + object + color|size|font.
    Todo: (unlikely) pass it on to the server, without interfering with ordinary SET statements.
    Todo: use keywords.
    Todo: allow SET ocelot_... = DEFAULT
    Todo: need a more flexible parser, eventually.
    Todo: should it be optional whether such statements go to history?
    Todo: do not consider it an error if you're not connected but it's a SET statement
    Todo: make sure it's a valid setting
    Todo: pay attention to the delimiter
    Todo: what should happen if the SET fails?
    Todo: isValid() check
    Todo: rename the Settings menu items and let the prompts match the names
    Todo: there is also  Qt way to save current settings, maybe it's another option
    Todo: menu item = "save settings" which would put a file in ~/ocelot.ini or ~/.my.cnf
    Todo: a single big setting
    Todo: a comment e.g. / *! OCELOT CLIENT * / meaning do not pass to server
    Problem: you cannot muck statement itself because menu might be changed while statement is up
    See also: Client variables that can be changed with the Settings widget
    Todo: Shouldn't client variables be in statement widget?
    Todo: (bug) If I change grid color or font weight or font style
          via the settings menu, the effect is immediate. But if I change
          them by executing SET ..., the effect is delayed, until the
          next time I type something or bring up a menu. I believe this
          is happening because going to settings menu will invalidate the
          grid widget area so it gets repainted; however, explicitly
          saying update() or repaint() solves nothing -- only hide()
          and show() force the widget change to be immediate.
          For other widgets I have kludged around similar problems,
          but I fear that I'm repainting multiple times unnecessarily.
  */
  QString ocelot_statement_text_color, new_ocelot_statement_text_color;
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
  QString ocelot_statement_highlight_keyword_color, new_ocelot_statement_highlight_keyword_color;
  QString ocelot_statement_prompt_background_color, new_ocelot_statement_prompt_background_color;
  QString ocelot_statement_highlight_current_line_color, new_ocelot_statement_highlight_current_line_color;
  QString ocelot_statement_highlight_function_color, new_ocelot_statement_highlight_function_color;
  QString ocelot_statement_syntax_checker, new_ocelot_statement_syntax_checker;
  QString ocelot_statement_style_string;
  QString ocelot_statement_format_statement_indent;
  QString ocelot_statement_format_clause_indent;
  QString ocelot_statement_format_keyword_case;
  QString ocelot_grid_text_color, new_ocelot_grid_text_color;
  QString ocelot_grid_background_color, new_ocelot_grid_background_color;
  QString ocelot_grid_border_color, new_ocelot_grid_border_color;
  QString ocelot_grid_header_background_color, new_ocelot_grid_header_background_color;
  QString ocelot_grid_font_family, new_ocelot_grid_font_family;
  QString ocelot_grid_font_size, new_ocelot_grid_font_size;
  QString ocelot_grid_font_style, new_ocelot_grid_font_style;
  QString ocelot_grid_font_weight, new_ocelot_grid_font_weight;
  QString ocelot_grid_cell_border_color, new_ocelot_grid_cell_border_color;
  QString ocelot_grid_cell_drag_line_color, new_ocelot_grid_cell_drag_line_color;
  QString ocelot_grid_border_size, new_ocelot_grid_border_size;
  QString ocelot_grid_cell_border_size, new_ocelot_grid_cell_border_size;
  QString ocelot_grid_cell_drag_line_size, new_ocelot_grid_cell_drag_line_size;
  QString ocelot_grid_style_string;
  QString ocelot_grid_header_style_string;
  QString ocelot_extra_rule_1_background_color, new_ocelot_extra_rule_1_background_color;
  QString ocelot_extra_rule_1_text_color, new_ocelot_extra_rule_1_text_color;
  QString ocelot_extra_rule_1_condition, new_ocelot_extra_rule_1_condition;
  QString ocelot_extra_rule_1_display_as, new_ocelot_extra_rule_1_display_as;
  QString ocelot_extra_rule_1_style_string;
  QString ocelot_history_text_color, new_ocelot_history_text_color;
  QString ocelot_history_background_color, new_ocelot_history_background_color;
  QString ocelot_history_border_color, new_ocelot_history_border_color;
  QString ocelot_history_font_family, new_ocelot_history_font_family;
  QString ocelot_history_font_size, new_ocelot_history_font_size;
  QString ocelot_history_font_style, new_ocelot_history_font_style;
  QString ocelot_history_font_weight, new_ocelot_history_font_weight;
  QString ocelot_history_style_string;
  QString ocelot_history_max_row_count, new_ocelot_history_max_row_count;
  QString ocelot_menu_text_color, new_ocelot_menu_text_color;
  QString ocelot_menu_background_color, new_ocelot_menu_background_color;
  QString ocelot_menu_border_color, new_ocelot_menu_border_color;
  QString ocelot_menu_font_family, new_ocelot_menu_font_family;
  QString ocelot_menu_font_size, new_ocelot_menu_font_size;
  QString ocelot_menu_font_style, new_ocelot_menu_font_style;
  QString ocelot_menu_font_weight, new_ocelot_menu_font_weight;
  QString ocelot_menu_style_string;

  QString ocelot_language;

  /* Strings for CONNECT. Some of these will be converted e.g. ocelot_host to ocelot_host_as_utf8 */
  QString ocelot_histignore;
  QString ocelot_host;
  QString ocelot_database;
  QString ocelot_user;
  QString ocelot_password;
  QString ocelot_unix_socket;
  QString ocelot_default_auth;
  QString ocelot_init_command;
  QString ocelot_opt_bind;
  QString ocelot_bind_address; /* Todo: check: is this the same as ocelot_opt_bind? */
  QString ocelot_opt_connect_attr_delete;
  QString ocelot_debug;
  QString ocelot_execute;
  QString ocelot_ld_run_path;
  QString ocelot_login_path;
  QString ocelot_opt_ssl;
  QString ocelot_opt_ssl_ca;
  QString ocelot_opt_ssl_capath;
  QString ocelot_opt_ssl_cert;
  QString ocelot_opt_ssl_cipher;
  QString ocelot_opt_ssl_crl;
  QString ocelot_opt_ssl_crlpath;
  QString ocelot_opt_ssl_key;
  QString ocelot_opt_ssl_mode;
  QString ocelot_pager;
  QString ocelot_plugin_dir;
  QString ocelot_read_default_file;
  QString ocelot_read_default_group;
  QString ocelot_server_public_key;
  QString ocelot_set_charset_dir;
  QString ocelot_set_charset_name;           /* was: ocelot_default_character_set */
  QString ocelot_shared_memory_base_name;
  QString ocelot_protocol;
  //unsigned short ocelot_no_defaults;         /* for CONNECT */
  QString ocelot_defaults_file;              /* for CONNECT */
  QString ocelot_defaults_extra_file;        /* for CONNECT */
  QString ocelot_defaults_group_suffix;
  QString ocelot_prompt;
  QString ocelot_opt_ssl_verify;
  QString options_files_read;

  QList<QString> q_color_list;
  QString q_color_list_name(QString rgb_name);
  QString qt_color(QString);
  QString rgb_to_color(QString);
  QString canonical_color_name(QString);
  void assign_names_for_colors();
  QString canonical_font_weight(QString);
  QString canonical_font_style(QString);
  QString connect_stripper(QString value_to_strip, bool strip_doublets_flag);
  /* Following were moved from 'private:', merely so all client variables could be together. Cannot be used with SET. */

  QString ocelot_dbms;                    /* for CONNECT */
  unsigned int ocelot_grid_max_row_lines;          /* ?? should be unsigned long? */
  /* unsigned int ocelot_grid_max_desired_width_in_chars; */

  QString history_markup_statement_start;    /* for markup */
  QString history_markup_statement_end;      /* for markup */
  QString history_markup_prompt_start;       /* for markup */
  QString history_markup_prompt_end;         /* for markup */
  QString history_markup_result;             /* for markup */
  QString history_markup_entity;             /* for markup */

  QString ocelot_history_tee_file_name;      /* see comment=tee+hist */
  QFile ocelot_history_tee_file;             /* see comment=tee+hist */
  QString ocelot_history_hist_file_name;     /* see comment=tee+hist */
  QFile ocelot_history_hist_file;            /* see comment=tee+hist */

  CodeEditor *statement_edit_widget;

  int main_window_maximum_width;
  int main_window_maximum_height;
  void component_size_calc(int *character_height, int *borders_height);
  QFont get_font_from_style_sheet(QString style_string);
  void set_dbms_version_mask(QString);
  void get_sql_mode(int who_is_calling, QString text);
  QString get_doc_path(QString);
  void hparse_f_nexttoken();
  void hparse_f_next_nexttoken();
  void hparse_f_error();
  bool hparse_f_is_equal(QString,QString);
  bool hparse_f_is_special_verb(int);
  int hparse_f_accept(unsigned short int,unsigned char,int,QString);
  int hparse_f_acceptn(int,QString,int);
  QString hparse_f_token_to_appendee(QString,int);
  int hparse_f_expect(unsigned short int,unsigned char,int,QString);
  int hparse_f_literal();
  int hparse_f_integer_literal();
  int hparse_f_default(int);
  int hparse_f_user_name();
  int hparse_f_character_set_name();
  int hparse_f_collation_name();
  int hparse_f_qualified_name_with_star();
  int hparse_f_qualified_name_of_object(int,int);
  int hparse_f_qualified_name_of_object_with_star(int,int);
  int hparse_f_qualified_name_of_operand(bool);
  int hparse_f_table_references();
  void hparse_f_table_escaped_table_reference();
  int hparse_f_table_reference(int);
  int hparse_f_table_factor();
  int hparse_f_table_join_table();
  int hparse_f_table_join_condition();
  void hparse_f_table_index_hint_list();
  int hparse_f_table_index_hint();
  int hparse_f_table_index_list();
  void hparse_f_opr_1(int),hparse_f_opr_2(int),hparse_f_opr_3(int),hparse_f_opr_4(int);
  void hparse_f_opr_5(int),hparse_f_opr_6(int),hparse_f_opr_7(int);
  void hparse_f_opr_8(int,int);
  void hparse_f_opr_9(int,int);
  void hparse_f_opr_10(int,int);
  void hparse_f_opr_11(int,int);
  void hparse_f_opr_12(int,int);
  void hparse_f_opr_13(int,int);
  void hparse_f_opr_14(int,int);
  void hparse_f_opr_15(int,int);
  void hparse_f_opr_16(int,int);
  void hparse_f_opr_17(int,int);
  void hparse_f_opr_18(int,int);
  void hparse_f_over(int,int);
  int hparse_f_over_start(int);
  int hparse_f_over_end();
  void hparse_f_function_arguments(QString);
  void hparse_f_expression_list(int);
  void hparse_f_parenthesized_value_list();
  void hparse_f_parameter_list(int);
  void hparse_f_parenthesized_expression();
  void hparse_f_parenthesized_multi_expression(int*);
  void hparse_f_like_or_where();
  void hparse_f_from_or_like_or_where();
  void hparse_f_infile_or_outfile();
  void hparse_f_show_columns();
  void hparse_f_if_not_exists();
  void hparse_f_indexes_or_keys();
  void hparse_f_alter_or_create_clause(int,unsigned short int*,bool*);
  int hparse_f_semicolon_and_or_delimiter(int);
  int hparse_f_explainable_statement(int);
  void hparse_f_statement(int);
  void hparse_f_assignment(int);
  void hparse_f_alter_table();
  int hparse_f_character_set();
  void hparse_f_alter_database();
  void hparse_f_alter_specification();
  void hparse_f_characteristics();
  int hparse_f_algorithm_or_lock();
  void hparse_f_definer();
  void hparse_f_character_set_or_collate();
  void hparse_f_length(bool, bool,bool);
  void hparse_f_enum_or_set();
  int hparse_f_data_type();
  void hparse_f_reference_option();
  void hparse_f_reference_definition();
  int hparse_f_create_definition();
  int hparse_f_current_timestamp();
  void hparse_f_column_definition();
  void hparse_f_comment();
  void hparse_f_column_list(int,int);
  void hparse_f_engine();
  void hparse_f_table_or_partition_options(int);
  void hparse_f_partition_options();
  void hparse_f_partition_or_subpartition(int);
  void hparse_f_partition_or_subpartition_definition(int);
  int hparse_f_partition_list(bool, bool);
  void hparse_f_algorithm();
  void hparse_f_sql();
  void hparse_f_for_channel();
  void hparse_f_interval_quantity(int);
  void hparse_f_alter_or_create_event(int);
  void hparse_f_alter_or_create_sequence(int);
  void hparse_f_alter_or_create_server(int);
  void hparse_f_require(int,bool,bool);
  void hparse_f_user_specification_list();
  void hparse_f_create_database();
  void hparse_f_index_columns(int,bool,bool);
  void hparse_f_alter_or_create_view();
  int hparse_f_analyze_or_optimize(int,int*);
  void hparse_f_call();
  void hparse_f_commit_or_rollback();
  void hparse_f_explain_or_describe(int);
  void hparse_f_grant_or_revoke(int,bool*);
  void hparse_f_insert_or_replace();
  void hparse_f_conflict_clause();
  void hparse_f_conflict_algorithm();
  void hparse_f_condition_information_item_name();
  int hparse_f_signal_or_resignal(int,int);
  int hparse_f_into();
  void hparse_f_with_clause(int);
  int hparse_f_values();
  int hparse_f_unionize();
  int hparse_f_select(bool);
  void hparse_f_where();
  int hparse_f_order_by(int);
  void hparse_f_limit(int);
  void hparse_f_block(int, int);
  void hparse_f_lua_blocklist(int,int);
  void hparse_f_lua_blockseries(int,int,bool);
  int hparse_f_lua_block(int,int,bool);
  int hparse_f_lua_funcname();
  int hparse_f_lua_varlist();
  int hparse_f_lua_var();
  int hparse_f_lua_namelist();
  int hparse_f_lua_explist();
  int hparse_f_lua_exp();
  int hparse_f_lua_prefixexp();
  int hparse_f_lua_functioncall();
  int hparse_f_lua_args();
  int hparse_f_lua_function();
  int hparse_f_lua_funcbody();
  int hparse_f_lua_parlist();
  int hparse_f_lua_tableconstructor();
  int hparse_f_lua_fieldlist();
  int hparse_f_lua_field();
  int hparse_f_lua_fieldsep();
  int hparse_f_lua_name();
  int hparse_f_lua_name_equivalent();
  int hparse_f_lua_number();
  int hparse_f_lua_string();
  int hparse_f_lua_oprlist();
  int hparse_f_lua_literal();
  void hparse_f_lua_opr_1(int);
  void hparse_f_lua_opr_2(int);
  void hparse_f_lua_opr_3(int);
  void hparse_f_lua_opr_4(int);
  void hparse_f_lua_opr_5(int);
  void hparse_f_lua_opr_6(int);
  void hparse_f_lua_opr_7(int);
  void hparse_f_lua_opr_8(int,int);
  void hparse_f_lua_opr_9(int,int);
  void hparse_f_lua_opr_10(int,int);
  void hparse_f_lua_opr_11(int,int);
  void hparse_f_lua_opr_12(int,int);
  void hparse_f_lua_opr_13(int,int);
  void hparse_f_lua_opr_14(int,int);
  void hparse_f_lua_opr_15(int,int);
  void hparse_f_lua_opr_16(int,int);
  void hparse_f_lua_opr_17(int,int);
  void hparse_f_lua_opr_18(int,int);
  int hparse_f_lua_accept_dotted(unsigned short int,unsigned char,int,QString);
  void hparse_f_labels(int);
  void hparse_f_cursors(int);
  int hparse_f_conditions(int);
  int hparse_f_variables(bool);
  void msgBoxClosed(QAbstractButton*);
  void hparse_f_multi_block(QString text);
  int hparse_f_backslash_command(bool);
  void hparse_f_other(int);
  int hparse_f_client_statement();
  void hparse_f_parse_hint_line_create();
  bool hparse_f_is_nosql(QString);
  void log(const char*,int);
  int real_query(QString, int);
#ifdef DBMS_TARANTOOL
  void tparse_f_factor();
  void tparse_f_term();
  void tparse_f_expression();
  void tparse_f_restricted_expression();
  void tparse_f_indexed_condition(int);
  void tparse_f_statement();
  void tparse_f_assignment();
  void tparse_f_block(int);
  void tparse_f_program(QString text);
#endif
#ifdef DBMS_TARANTOOL
  QString tarantool_add_return(QString);
  int tarantool_result_set_type(int);
  long unsigned int tarantool_num_rows(unsigned int connection_number);
  unsigned int tarantool_num_fields();
  int tarantool_num_fields_recursive(const char **tarantool_tnt_reply_data,
                                     char *field_name,
                                     int field_number_within_array,
                                     QString *field_name_list);
  QString tarantool_scan_rows(unsigned int p_result_column_count,
                 unsigned int p_result_row_count,
                 MYSQL_RES *p_mysql_res,
                 char **p_result_set_copy,
                 char ***p_result_set_copy_rows,
                 unsigned int **p_result_max_column_widths);
  void tarantool_scan_field_names(
                 const char *which_field,
                 unsigned int p_result_column_count,
                 char **p_result_field_names);
  int create_table_server(QString, bool *, unsigned int, unsigned int);
  QString tarantool_read_format(QString);
#endif
  QVBoxLayout *main_layout;

public slots:
  void action_connect();
  void action_connect_once(QString);
  void action_exit();
  void action_execute_force();
  int action_execute(int);
  void action_kill();
  void action_about();
  void action_the_manual();
  void action_libmysqlclient();
  void action_settings();
  void action_statement_edit_widget_text_changed();
  void action_undo();
  void statement_edit_widget_formatter();
  void action_change_one_setting(QString old_setting, QString new_setting, const char *name_of_setting);
  void action_menu();
  void action_history();
  void action_grid();
  void action_extra_rule_1();
  void action_statement();
  void history_markup_previous();
  void history_markup_next();
  void action_option_detach_history_widget(bool checked);
  void action_option_detach_result_grid_widget(bool checked);
  void action_option_detach_debug_widget(bool checked);
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
  void debug_skip_go();
  void debug_source_go();
  void debug_set_go(QString text);
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
  void shortcut(QString,QString);
  void menu_edit_undo();
  void menu_edit_redo();
  void menu_edit_cut();
  void menu_edit_copy();
  void menu_edit_paste();
  void menu_edit_select_all();
  bool eventfilter_function(QObject *obj, QEvent *event);

protected:
  bool eventFilter(QObject *obj, QEvent *ev);
  void resizeEvent(QResizeEvent *ev);

private:
  Ui::MainWindow *ui;
  int history_markup_previous_or_next();
  void initialize_widget_history();
  int result_grid_add_tab();
  void initialize_widget_statement();
#ifdef DEBUGGER
  void debug_menu_enable_or_disable(int statement_type);
  void create_widget_debug();
  int debug_information_status(char *last_command);
  int debug_call_xxxmdbug_command(const char *command);
  void debug_highlight_line();
  void debug_maintain_prompt(int action, int debug_widget_index, int line_number);
  QString debug_privilege_check(int statement_type);
#endif
  void main_token_new(int), main_token_push(), main_token_pop();
  void create_menu();
  int rehash_scan();
  QString rehash_search(char *search_string, int reftype);
  void rehash_get_database_name(char *);
  void widget_sizer();
  QString get_delimiter(QString,QString,int);
  int execute_client_statement(QString text, int *additional_result);
  void put_diagnostics_in_result(unsigned int);
  void put_message_in_result(QString);
  void make_and_put_message_in_result(unsigned int, int, char*);
  void make_and_append_message_in_result(unsigned int, int, char*);
  unsigned int get_ocelot_protocol_as_int(QString s);
//  int options_and_connect(char *host, char *database, char *user, char *password, char *tmp_init_command,
//                       char *tmp_plugin_dir, char *tmp_default_auth, char *unix_socket, unsigned int connection_number);

  void connect_mysql_options_2(int w_argc, char *argv[]);
  void connect_read_command_line(int argc, char *argv[]);
  void connect_read_my_cnf(const char *file_name, int is_mylogin_cnf);
  int connect_readmylogin(QFile&, unsigned char *);
  //int connect_readmylogin(FILE *, unsigned char *);
  void connect_set_variable(QString token0, QString token2);
  void connect_make_statement();
  long to_long(QString token);
  void print_version();
  void print_help();

  void copy_options_to_main_window();
  void delete_utf8_copies();
  void copy_connect_strings_to_utf8(); /* in effect, copy options from main_window */
  int the_connect(unsigned int connection_number);
  int the_connect_2(); /* intended replacement for the_connect() */
  //my_bool get_one_option(int optid, const struct my_option *opt __attribute__((unused)),char *argument);
  void connect_init();
  void set_current_colors_and_font(QFont);
  QFont get_fixed_font();
  void make_style_strings();
  //void create_the_manual_widget();
  int get_next_statement_in_string(int passed_main_token_number, int *returned_begin_count, bool);
  int make_statement_ready_to_send(QString, char *, int);
  void remove_statement(QString);
  int action_execute_one_statement(QString text);

  void history_markup_make_strings();
  void history_markup_append(QString result_set_for_history, bool is_interactive);
  QString history_markup_copy_for_history(QString);
  int history_file_start(QString, QString);        /* see comment=tee+hist */
  void history_file_stop(QString);                 /* see comment=tee+hist */
  void history_file_write(QString, QString);       /* see comment=tee+hist */
  void history_file_to_history_widget();           /* see comment=tee+hist */

  void statement_edit_widget_setstylesheet();
  bool is_statement_complete(QString);
  void message_box(QString the_title, QString the_text);

/*
  ocelot_statement_syntax_checker is planned as a bunch of flags, e.g.
    0 = none
    1 = use for highlights
    2 = errors, i.e. pop up a dialog box if user tries to execute a bad-looking statement
    4 = severe e.g. look whether declared variable is known
    8 = severe e.g. look whether table is known (need to ask server)
   16 = tooltip
   32 = word-completion
   ... although so far the only thing being checked is 2 = errors
*/
#define FLAG_FOR_HIGHLIGHTS 1
#define FLAG_FOR_ERRORS     2

  void tokenize(QChar *text, int text_length, int *token_lengths, int *token_offsets, int max_tokens, QChar *version, int passed_comment_behaviour, QString special_token, int minus_behaviour);

  int token_type(QChar *token, int token_length);

  void tokens_to_keywords(QString text, int start);
  void tokens_to_keywords_revert(int i_of_body, int i_of_function, int i_of_do, QString text, int start);
  int next_token(int i);
  bool is_client_statement(int, int, QString);
  int find_start_of_body(QString text, int start, int *i_of_function, int *i_of_do);
  int connect_mysql(unsigned int connection_number);
  void connect_mysql_error_box(QString, unsigned int);
#ifdef DBMS_TARANTOOL
  int connect_tarantool(unsigned int connection_number, QString, QString, QString, QString);
  void tarantool_initialize(int connection_number);
  void tarantool_flush_and_save_reply(unsigned int);
  int tarantool_real_query(const char *dbms_query, unsigned long dbms_query_len, unsigned int, unsigned int, unsigned int);
  int get_statement_type(unsigned int, unsigned int);
  QString tarantool_fetch_row(const char *tarantool_tnt_reply_data, int *bytes, int *tsize);
  QString tarantool_fetch_header_row(int);
  const char * tarantool_seek_0(int*);
  QString tarantool_internal_query(char*, int);
#endif
  QString select_1_row(const char *select_statement);

  QWidget *main_window;

  TextEditHistory *history_edit_widget;
  QLineEdit *hparse_line_edit;
#ifdef DEBUGGER
#define DEBUG_TAB_WIDGET_MAX 10
  QWidget *debug_top_widget;
  QVBoxLayout *debug_top_widget_layout;
  QLineEdit *debug_line_widget;
  QTabWidget *debug_tab_widget;
  CodeEditor *debug_widget[DEBUG_TAB_WIDGET_MAX]; /* todo: this should be variable-size */
#endif

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
    QAction *menu_edit_action_formatter;
  QMenu *menu_run;
    QAction *menu_run_action_execute;
    QAction *menu_run_action_kill;
  QMenu *menu_settings;
    QAction *menu_settings_action_menu;
    QAction *menu_settings_action_history;
    QAction *menu_settings_action_grid;
    QAction *menu_settings_action_statement;
    QAction *menu_settings_action_extra_rule_1;
  QMenu *menu_options;
    QAction *menu_options_action_option_detach_history_widget;
    QAction *menu_options_action_option_detach_result_grid_widget;
    QAction *menu_options_action_option_detach_debug_widget;
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
    QAction *menu_help_action_libmysqlclient;
    QAction *menu_help_action_settings;

  //QWidget *the_manual_widget;
  //  QVBoxLayout *the_manual_layout;
  //  QTextEdit *the_manual_text_edit;
  //  QPushButton *the_manual_pushbutton;

  /* QTableWidget *grid_table_widget; */
  QTabWidget48 *result_grid_tab_widget;

  unsigned long result_row_count;

  int history_markup_counter; /* 0 when execute, +1 when "previous statement", -1 for "next statement" */

  int statement_edit_widget_text_changed_flag;
  QString ocelot_delimiter_str;                                           /* set up in connect section */
  int ocelot_password_was_specified;

  /* MYSQL mysql; */
  MYSQL_RES *mysql_res;
  /* MYSQL_FIELD *fields; */
#ifdef DBMS_TARANTOOL
  struct tnt_reply tarantool_tnt_reply;
  char *tarantool_field_names;
  bool tarantool_select_nosql;
#endif

public:
  int tarantool_execute_sql(const char *, unsigned long, unsigned int, int, QString);
  QString query_utf16;
  QString query_utf16_copy;
  /* main_token_offsets|lengths|types|flags|pointers are alloc'd in main_token_new() */
  int  *main_token_offsets;
  int  *main_token_lengths;
  int  *main_token_types;
  unsigned int *main_token_flags; /* e.g. TOKEN_FLAG_IS_RESERVED */
  int  *main_token_pointers;
  unsigned char *main_token_reftypes;
  unsigned int main_token_max_count;
  unsigned int main_token_count_in_all;
  unsigned int main_token_count_in_statement;
  unsigned int main_token_number;      /* = offset within main_token_offsets, e.g. 0 if currently at first token */

  int  *saved_main_token_offsets;
  int  *saved_main_token_lengths;
  int  *saved_main_token_types;
  unsigned int *saved_main_token_flags;
  int  *saved_main_token_pointers;
  unsigned char *saved_main_token_reftypes;
  /* unsigned int saved_main_token_max_count; doesn't need saving */
  unsigned int saved_main_token_count_in_all;
  unsigned int saved_main_token_count_in_statement;
  unsigned int saved_main_token_number;

  /* main_token_flags[] values. so far there are only twelve but we expect there will be more. */
  #define TOKEN_FLAG_IS_RESERVED 1
  #define TOKEN_FLAG_IS_BLOCK_END 2
  #define TOKEN_FLAG_IS_ERROR 4
  #define TOKEN_FLAG_IS_FUNCTION 8
  #define TOKEN_FLAG_IS_START_STATEMENT 16
  #define TOKEN_FLAG_IS_START_CLAUSE 32
  #define TOKEN_FLAG_IS_START_SUBCLAUSE 64
  #define TOKEN_FLAG_IS_DATA_TYPE 128
  #define TOKEN_FLAG_IS_START_IN_COLUMN_LIST 256
  #define TOKEN_FLAG_IS_END_IN_COLUMN_LIST 512
  #define TOKEN_FLAG_IS_BINARY_PLUS_OR_MINUS 1024
  #define TOKEN_FLAG_IS_NOT_AFTER_SPACE 2048
  #define TOKEN_FLAG_IS_MAYBE_LUA 4096
  #define TOKEN_FLAG_IS_LUA 8192

  enum {                                      /* possible returns from token_type() */
    TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE= 1, /* starts with ' or N' or X' or B' */
    TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE= 2, /* starts with " */
    TOKEN_TYPE_LITERAL_WITH_DIGIT= 3,        /* starts with 0-9 */
    TOKEN_TYPE_LITERAL_WITH_BRACKET= 4,      /* starts with [[, Lua only */
    TOKEN_TYPE_LITERAL_WITH_BRACE= 5,        /* starts with { */ /* obsolete? */
    TOKEN_TYPE_LITERAL= 5,
    TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK= 6,  /* starts with ` */
    TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE= 7, /* starts with " and hparse_ansi_quote=true */
    TOKEN_TYPE_IDENTIFIER_WITH_AT= 8,        /* starts with @ */
    TOKEN_TYPE_IDENTIFIER= 8,
    TOKEN_TYPE_COMMENT_WITH_SLASH = 9,        /* starts with / * or * / */
    TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE= 10,   /* starts with # */
    TOKEN_TYPE_COMMENT_WITH_MINUS= 11,        /* starts with -- */
    TOKEN_TYPE_OPERATOR= 12,                 /* starts with < > = ! etc. */
    TOKEN_TYPE_OTHER= 13,                    /* identifier? keyword? */
    /* The TOKEN_KEYWORD_... numbers must match the list in tokens_to_keywords(). */
    TOKEN_KEYWORDS_START= TOKEN_TYPE_OTHER + 1,
    TOKEN_KEYWORD_QUESTIONMARK= TOKEN_KEYWORDS_START, /* Ocelot keyword */
      TOKEN_KEYWORD_ABORT,
      TOKEN_KEYWORD_ABS,
      TOKEN_KEYWORD_ACCESSIBLE,
      TOKEN_KEYWORD_ACOS,
      TOKEN_KEYWORD_ACTION,
      TOKEN_KEYWORD_ADD,
      TOKEN_KEYWORD_ADDDATE,
      TOKEN_KEYWORD_ADDTIME,
      TOKEN_KEYWORD_AES_DECRYPT,
      TOKEN_KEYWORD_AES_ENCRYPT,
      TOKEN_KEYWORD_AFTER,
      TOKEN_KEYWORD_AGAINST,
      TOKEN_KEYWORD_ALGORITHM,
      TOKEN_KEYWORD_ALL,
      TOKEN_KEYWORD_ALTER,
      TOKEN_KEYWORD_ALWAYS,
      TOKEN_KEYWORD_ANALYZE,
      TOKEN_KEYWORD_AND,
      TOKEN_KEYWORD_ANY_VALUE,
      TOKEN_KEYWORD_AREA,
      TOKEN_KEYWORD_AS,
      TOKEN_KEYWORD_ASBINARY,
      TOKEN_KEYWORD_ASC,
      TOKEN_KEYWORD_ASCII,
      TOKEN_KEYWORD_ASENSITIVE,
      TOKEN_KEYWORD_ASIN,
      TOKEN_KEYWORD_ASTEXT,
      TOKEN_KEYWORD_ASWKB,
      TOKEN_KEYWORD_ASWKT,
      TOKEN_KEYWORD_ASYMMETRIC_DECRYPT,
      TOKEN_KEYWORD_ASYMMETRIC_DERIVE,
      TOKEN_KEYWORD_ASYMMETRIC_ENCRYPT,
      TOKEN_KEYWORD_ASYMMETRIC_SIGN,
      TOKEN_KEYWORD_ASYMMETRIC_VERIFY,
      TOKEN_KEYWORD_ATAN,
      TOKEN_KEYWORD_ATAN2,
      TOKEN_KEYWORD_ATTACH,
      TOKEN_KEYWORD_AUTOINCREMENT,
      TOKEN_KEYWORD_AVG,
      TOKEN_KEYWORD_BEFORE,
      TOKEN_KEYWORD_BEGIN,
      TOKEN_KEYWORD_BENCHMARK,
      TOKEN_KEYWORD_BETWEEN,
      TOKEN_KEYWORD_BFILE,
      TOKEN_KEYWORD_BIGINT,
      TOKEN_KEYWORD_BIN,
      TOKEN_KEYWORD_BINARY,
      TOKEN_KEYWORD_BINARY_DOUBLE,
      TOKEN_KEYWORD_BINARY_FLOAT,
      TOKEN_KEYWORD_BINLOG,
      TOKEN_KEYWORD_BINLOG_GTID_POS,
      TOKEN_KEYWORD_BIT,
      TOKEN_KEYWORD_BIT_AND,
      TOKEN_KEYWORD_BIT_COUNT,
      TOKEN_KEYWORD_BIT_LENGTH,
      TOKEN_KEYWORD_BIT_OR,
      TOKEN_KEYWORD_BIT_XOR,
      TOKEN_KEYWORD_BLOB,
      TOKEN_KEYWORD_BOOL,
      TOKEN_KEYWORD_BOOLEAN,
      TOKEN_KEYWORD_BOTH,
      TOKEN_KEYWORD_BREAK,
      TOKEN_KEYWORD_BUFFER,
      TOKEN_KEYWORD_BY,
      TOKEN_KEYWORD_CALL,
      TOKEN_KEYWORD_CASCADE,
      TOKEN_KEYWORD_CASE,
      TOKEN_KEYWORD_CAST,
      TOKEN_KEYWORD_CEIL,
      TOKEN_KEYWORD_CEILING,
      TOKEN_KEYWORD_CENTROID,
      TOKEN_KEYWORD_CHANGE,
      TOKEN_KEYWORD_CHANGES,
      TOKEN_KEYWORD_CHAR,
      TOKEN_KEYWORD_CHARACTER,
      TOKEN_KEYWORD_CHARACTER_LENGTH,
      TOKEN_KEYWORD_CHARSET,
      TOKEN_KEYWORD_CHAR_LENGTH,
      TOKEN_KEYWORD_CHECK,
      TOKEN_KEYWORD_CLEAR,
      TOKEN_KEYWORD_CLOSE,
      TOKEN_KEYWORD_COALESCE,
      TOKEN_KEYWORD_COERCIBILITY,
      TOKEN_KEYWORD_COLLATE,
      TOKEN_KEYWORD_COLLATION,
      TOKEN_KEYWORD_COLUMN,
      TOKEN_KEYWORD_COLUMN_ADD,
      TOKEN_KEYWORD_COLUMN_CHECK,
      TOKEN_KEYWORD_COLUMN_CREATE,
      TOKEN_KEYWORD_COLUMN_DELETE,
      TOKEN_KEYWORD_COLUMN_EXISTS,
      TOKEN_KEYWORD_COLUMN_GET,
      TOKEN_KEYWORD_COLUMN_JSON,
      TOKEN_KEYWORD_COLUMN_LIST,
      TOKEN_KEYWORD_COMMENT,
      TOKEN_KEYWORD_COMMIT,
      TOKEN_KEYWORD_COMPACT,
      TOKEN_KEYWORD_COMPRESS,
      TOKEN_KEYWORD_COMPRESSED,
      TOKEN_KEYWORD_COMPRESSION,
      TOKEN_KEYWORD_CONCAT,
      TOKEN_KEYWORD_CONCAT_WS,
      TOKEN_KEYWORD_CONDITION,
      TOKEN_KEYWORD_CONFLICT,
      TOKEN_KEYWORD_CONNECT,
      TOKEN_KEYWORD_CONNECTION_ID,
      TOKEN_KEYWORD_CONSTRAINT,
      TOKEN_KEYWORD_CONTAINS,
      TOKEN_KEYWORD_CONTINUE,
      TOKEN_KEYWORD_CONV,
      TOKEN_KEYWORD_CONVERT,
      TOKEN_KEYWORD_CONVERT_TZ,
      TOKEN_KEYWORD_CONVEXHULL,
      TOKEN_KEYWORD_COS,
      TOKEN_KEYWORD_COT,
      TOKEN_KEYWORD_COUNT,
      TOKEN_KEYWORD_CRC32,
      TOKEN_KEYWORD_CREATE,
      TOKEN_KEYWORD_CREATE_ASYMMETRIC_PRIV_KEY,
      TOKEN_KEYWORD_CREATE_ASYMMETRIC_PUB_KEY,
      TOKEN_KEYWORD_CREATE_DH_PARAMETERS,
      TOKEN_KEYWORD_CREATE_DIGEST,
      TOKEN_KEYWORD_CROSS,
      TOKEN_KEYWORD_CROSSES,
      TOKEN_KEYWORD_CUME_DIST,
      TOKEN_KEYWORD_CURDATE,
      TOKEN_KEYWORD_CURRENT,
      TOKEN_KEYWORD_CURRENT_DATE,
      TOKEN_KEYWORD_CURRENT_ROLE,
      TOKEN_KEYWORD_CURRENT_TIME,
      TOKEN_KEYWORD_CURRENT_TIMESTAMP,
      TOKEN_KEYWORD_CURRENT_USER,
      TOKEN_KEYWORD_CURSOR,
      TOKEN_KEYWORD_CURTIME,
      TOKEN_KEYWORD_CYCLE,
      TOKEN_KEYWORD_DATABASE,
      TOKEN_KEYWORD_DATABASES,
      TOKEN_KEYWORD_DATE,
      TOKEN_KEYWORD_DATEDIFF,
      TOKEN_KEYWORD_DATETIME,
      TOKEN_KEYWORD_DATE_ADD,
      TOKEN_KEYWORD_DATE_FORMAT,
      TOKEN_KEYWORD_DATE_SUB,
      TOKEN_KEYWORD_DAY,
      TOKEN_KEYWORD_DAYNAME,
      TOKEN_KEYWORD_DAYOFMONTH,
      TOKEN_KEYWORD_DAYOFWEEK,
      TOKEN_KEYWORD_DAYOFYEAR,
      TOKEN_KEYWORD_DAY_HOUR,
      TOKEN_KEYWORD_DAY_MICROSECOND,
      TOKEN_KEYWORD_DAY_MINUTE,
      TOKEN_KEYWORD_DAY_SECOND,
      TOKEN_KEYWORD_DEALLOCATE,
      TOKEN_KEYWORD_DEC,
      TOKEN_KEYWORD_DECIMAL,
      TOKEN_KEYWORD_DECLARE,
      TOKEN_KEYWORD_DECODE,
      TOKEN_KEYWORD_DECODE_HISTOGRAM,
      TOKEN_KEYWORD_DEFAULT,
      TOKEN_KEYWORD_DEFERRABLE,
      TOKEN_KEYWORD_DEFERRED,
      TOKEN_KEYWORD_DEGREES,
      TOKEN_KEYWORD_DELAYED,
      TOKEN_KEYWORD_DELETE,
      TOKEN_KEYWORD_DELIMITER,
      TOKEN_KEYWORD_DENSE_RANK,
      TOKEN_KEYWORD_DESC,
      TOKEN_KEYWORD_DESCRIBE,
      TOKEN_KEYWORD_DES_DECRYPT,
      TOKEN_KEYWORD_DES_ENCRYPT,
      TOKEN_KEYWORD_DETACH,
      TOKEN_KEYWORD_DETERMINISTIC,
      TOKEN_KEYWORD_DIMENSION,
      TOKEN_KEYWORD_DIRECTORY,
      TOKEN_KEYWORD_DISJOINT,
      TOKEN_KEYWORD_DISTANCE,
      TOKEN_KEYWORD_DISTINCT,
      TOKEN_KEYWORD_DISTINCTROW,
      TOKEN_KEYWORD_DIV,
      TOKEN_KEYWORD_DO,
      TOKEN_KEYWORD_DOUBLE,
      TOKEN_KEYWORD_DROP,
      TOKEN_KEYWORD_DUAL,
      TOKEN_KEYWORD_DUPLICATE,
      TOKEN_KEYWORD_DYNAMIC,
      TOKEN_KEYWORD_EACH,
      TOKEN_KEYWORD_EDIT,
      TOKEN_KEYWORD_EGO,
      TOKEN_KEYWORD_ELSE,
      TOKEN_KEYWORD_ELSEIF,
      TOKEN_KEYWORD_ELT,
      TOKEN_KEYWORD_ENABLE,
      TOKEN_KEYWORD_ENCLOSED,
      TOKEN_KEYWORD_ENCODE,
      TOKEN_KEYWORD_ENCRYPT,
      TOKEN_KEYWORD_END,
      TOKEN_KEYWORD_ENDPOINT,
      TOKEN_KEYWORD_ENUM,
      TOKEN_KEYWORD_ENVELOPE,
      TOKEN_KEYWORD_EQUALS,
      TOKEN_KEYWORD_ESCAPE,
      TOKEN_KEYWORD_ESCAPED,
      TOKEN_KEYWORD_EVENT,
      TOKEN_KEYWORD_EXCEPT,
      TOKEN_KEYWORD_EXCHANGE,
      TOKEN_KEYWORD_EXCLUSIVE,
      TOKEN_KEYWORD_EXECUTE,
      TOKEN_KEYWORD_EXISTS,
      TOKEN_KEYWORD_EXIT,
      TOKEN_KEYWORD_EXP,
      TOKEN_KEYWORD_EXPANSION,
      TOKEN_KEYWORD_EXPLAIN,
      TOKEN_KEYWORD_EXPORT,
      TOKEN_KEYWORD_EXPORT_SET,
      TOKEN_KEYWORD_EXTERIORRING,
      TOKEN_KEYWORD_EXTRACT,
      TOKEN_KEYWORD_EXTRACTVALUE,
      TOKEN_KEYWORD_FAIL,
      TOKEN_KEYWORD_FALSE,
      TOKEN_KEYWORD_FETCH,
      TOKEN_KEYWORD_FIELD,
      TOKEN_KEYWORD_FILE,
      TOKEN_KEYWORD_FIND_IN_SET,
      TOKEN_KEYWORD_FIRST,
      TOKEN_KEYWORD_FIRST_VALUE,
      TOKEN_KEYWORD_FIXED,
      TOKEN_KEYWORD_FLOAT,
      TOKEN_KEYWORD_FLOAT4,
      TOKEN_KEYWORD_FLOAT8,
      TOKEN_KEYWORD_FLOOR,
      TOKEN_KEYWORD_FLUSH,
      TOKEN_KEYWORD_FOLLOWING,
      TOKEN_KEYWORD_FOLLOWS,
      TOKEN_KEYWORD_FOR,
      TOKEN_KEYWORD_FORCE,
      TOKEN_KEYWORD_FOREIGN,
      TOKEN_KEYWORD_FORMAT,
      TOKEN_KEYWORD_FOUND_ROWS,
      TOKEN_KEYWORD_FROM,
      TOKEN_KEYWORD_FROM_BASE64,
      TOKEN_KEYWORD_FROM_DAYS,
      TOKEN_KEYWORD_FROM_UNIXTIME,
      TOKEN_KEYWORD_FULL,
      TOKEN_KEYWORD_FULLTEXT,
      TOKEN_KEYWORD_FUNCTION,
      TOKEN_KEYWORD_GENERAL,
      TOKEN_KEYWORD_GENERATED,
      TOKEN_KEYWORD_GEOMCOLLFROMTEXT,
      TOKEN_KEYWORD_GEOMCOLLFROMWKB,
      TOKEN_KEYWORD_GEOMETRY,
      TOKEN_KEYWORD_GEOMETRYCOLLECTION,
      TOKEN_KEYWORD_GEOMETRYCOLLECTIONFROMTEXT,
      TOKEN_KEYWORD_GEOMETRYCOLLECTIONFROMWKB,
      TOKEN_KEYWORD_GEOMETRYFROMTEXT,
      TOKEN_KEYWORD_GEOMETRYFROMWKB,
      TOKEN_KEYWORD_GEOMETRYN,
      TOKEN_KEYWORD_GEOMETRYTYPE,
      TOKEN_KEYWORD_GEOMFROMTEXT,
      TOKEN_KEYWORD_GEOMFROMWKB,
      TOKEN_KEYWORD_GET,
      TOKEN_KEYWORD_GET_FORMAT,
      TOKEN_KEYWORD_GET_LOCK,
      TOKEN_KEYWORD_GLENGTH,
      TOKEN_KEYWORD_GLOB,
      TOKEN_KEYWORD_GO,
      TOKEN_KEYWORD_GRANT,
      TOKEN_KEYWORD_GREATEST,
      TOKEN_KEYWORD_GROUP,
      TOKEN_KEYWORD_GROUP_CONCAT,
      TOKEN_KEYWORD_GTID_SUBSET,
      TOKEN_KEYWORD_GTID_SUBTRACT,
      TOKEN_KEYWORD_HANDLER,
      TOKEN_KEYWORD_HAVING,
      TOKEN_KEYWORD_HELP,
      TOKEN_KEYWORD_HEX,
      TOKEN_KEYWORD_HIGH_PRIORITY,
      TOKEN_KEYWORD_HOUR,
      TOKEN_KEYWORD_HOUR_MICROSECOND,
      TOKEN_KEYWORD_HOUR_MINUTE,
      TOKEN_KEYWORD_HOUR_SECOND,
      TOKEN_KEYWORD_IF,
      TOKEN_KEYWORD_IFNULL,
      TOKEN_KEYWORD_IGNORE,
      TOKEN_KEYWORD_IMMEDIATE,
      TOKEN_KEYWORD_IMPORT,
      TOKEN_KEYWORD_IN,
      TOKEN_KEYWORD_INDEX,
      TOKEN_KEYWORD_INDEXED,
      TOKEN_KEYWORD_INET6_ATON,
      TOKEN_KEYWORD_INET6_NTOA,
      TOKEN_KEYWORD_INET_ATON,
      TOKEN_KEYWORD_INET_NTOA,
      TOKEN_KEYWORD_INFILE,
      TOKEN_KEYWORD_INITIALLY,
      TOKEN_KEYWORD_INNER,
      TOKEN_KEYWORD_INOUT,
      TOKEN_KEYWORD_INSENSITIVE,
      TOKEN_KEYWORD_INSERT,
      TOKEN_KEYWORD_INSTALL,
      TOKEN_KEYWORD_INSTEAD,
      TOKEN_KEYWORD_INSTR,
      TOKEN_KEYWORD_INT,
      TOKEN_KEYWORD_INT1,
      TOKEN_KEYWORD_INT2,
      TOKEN_KEYWORD_INT3,
      TOKEN_KEYWORD_INT4,
      TOKEN_KEYWORD_INT8,
      TOKEN_KEYWORD_INTEGER,
      TOKEN_KEYWORD_INTERIORRINGN,
      TOKEN_KEYWORD_INTERSECT,
      TOKEN_KEYWORD_INTERSECTS,
      TOKEN_KEYWORD_INTERVAL,
      TOKEN_KEYWORD_INTO,
      TOKEN_KEYWORD_IO_AFTER_GTIDS,
      TOKEN_KEYWORD_IO_BEFORE_GTIDS,
      TOKEN_KEYWORD_IS,
      TOKEN_KEYWORD_ISCLOSED,
      TOKEN_KEYWORD_ISEMPTY,
      TOKEN_KEYWORD_ISNULL,
      TOKEN_KEYWORD_ISSIMPLE,
      TOKEN_KEYWORD_IS_FREE_LOCK,
      TOKEN_KEYWORD_IS_IPV4,
      TOKEN_KEYWORD_IS_IPV4_COMPAT,
      TOKEN_KEYWORD_IS_IPV4_MAPPED,
      TOKEN_KEYWORD_IS_IPV6,
      TOKEN_KEYWORD_IS_USED_LOCK,
      TOKEN_KEYWORD_ITERATE,
      TOKEN_KEYWORD_JOIN,
      TOKEN_KEYWORD_JSON,
      TOKEN_KEYWORD_JSON_APPEND,
      TOKEN_KEYWORD_JSON_ARRAY,
      TOKEN_KEYWORD_JSON_ARRAY_APPEND,
      TOKEN_KEYWORD_JSON_ARRAY_INSERT,
      TOKEN_KEYWORD_JSON_CONTAINS,
      TOKEN_KEYWORD_JSON_CONTAINS_PATH,
      TOKEN_KEYWORD_JSON_DEPTH,
      TOKEN_KEYWORD_JSON_EXTRACT,
      TOKEN_KEYWORD_JSON_INSERT,
      TOKEN_KEYWORD_JSON_KEYS,
      TOKEN_KEYWORD_JSON_LENGTH,
      TOKEN_KEYWORD_JSON_MERGE,
      TOKEN_KEYWORD_JSON_OBJECT,
      TOKEN_KEYWORD_JSON_QUERY,
      TOKEN_KEYWORD_JSON_QUOTE,
      TOKEN_KEYWORD_JSON_REMOVE,
      TOKEN_KEYWORD_JSON_REPLACE,
      TOKEN_KEYWORD_JSON_SEARCH,
      TOKEN_KEYWORD_JSON_SET,
      TOKEN_KEYWORD_JSON_TYPE,
      TOKEN_KEYWORD_JSON_UNQUOTE,
      TOKEN_KEYWORD_JSON_VALID,
      TOKEN_KEYWORD_JSON_VALUE,
      TOKEN_KEYWORD_JULIANDAY,
      TOKEN_KEYWORD_KEY,
      TOKEN_KEYWORD_KEYS,
      TOKEN_KEYWORD_KILL,
      TOKEN_KEYWORD_LAG,
      TOKEN_KEYWORD_LANGUAGE,
      TOKEN_KEYWORD_LAST,
      TOKEN_KEYWORD_LASTVAL,
      TOKEN_KEYWORD_LAST_DAY,
      TOKEN_KEYWORD_LAST_INSERT_ID,
      TOKEN_KEYWORD_LAST_VALUE,
      TOKEN_KEYWORD_LCASE,
      TOKEN_KEYWORD_LEAD,
      TOKEN_KEYWORD_LEADING,
      TOKEN_KEYWORD_LEAST,
      TOKEN_KEYWORD_LEAVE,
      TOKEN_KEYWORD_LEFT,
      TOKEN_KEYWORD_LENGTH,
      TOKEN_KEYWORD_LEVEL,
      TOKEN_KEYWORD_LIKE,
      TOKEN_KEYWORD_LIMIT,
      TOKEN_KEYWORD_LINEAR,
      TOKEN_KEYWORD_LINEFROMTEXT,
      TOKEN_KEYWORD_LINEFROMWKB,
      TOKEN_KEYWORD_LINES,
      TOKEN_KEYWORD_LINESTRING,
      TOKEN_KEYWORD_LINESTRINGFROMTEXT,
      TOKEN_KEYWORD_LINESTRINGFROMWKB,
      TOKEN_KEYWORD_LN,
      TOKEN_KEYWORD_LOAD,
      TOKEN_KEYWORD_LOAD_FILE,
      TOKEN_KEYWORD_LOCAL,
      TOKEN_KEYWORD_LOCALTIME,
      TOKEN_KEYWORD_LOCALTIMESTAMP,
      TOKEN_KEYWORD_LOCATE,
      TOKEN_KEYWORD_LOCK,
      TOKEN_KEYWORD_LOG,
      TOKEN_KEYWORD_LOG10,
      TOKEN_KEYWORD_LOG2,
      TOKEN_KEYWORD_LOGFILE,
      TOKEN_KEYWORD_LONG,
      TOKEN_KEYWORD_LONGBLOB,
      TOKEN_KEYWORD_LONGTEXT,
      TOKEN_KEYWORD_LOOP,
      TOKEN_KEYWORD_LOWER,
      TOKEN_KEYWORD_LOW_PRIORITY,
      TOKEN_KEYWORD_LPAD,
      TOKEN_KEYWORD_LTRIM,
      TOKEN_KEYWORD_LUA,
      TOKEN_KEYWORD_MAKEDATE,
      TOKEN_KEYWORD_MAKETIME,
      TOKEN_KEYWORD_MAKE_SET,
      TOKEN_KEYWORD_MASTER_BIND,
      TOKEN_KEYWORD_MASTER_HEARTBEAT_PERIOD,
      TOKEN_KEYWORD_MASTER_POS_WAIT,
      TOKEN_KEYWORD_MASTER_SSL_VERIFY_SERVER_CERT,
      TOKEN_KEYWORD_MATCH,
      TOKEN_KEYWORD_MAX,
      TOKEN_KEYWORD_MAXVALUE,
      TOKEN_KEYWORD_MBRCONTAINS,
      TOKEN_KEYWORD_MBRCOVEREDBY,
      TOKEN_KEYWORD_MBRCOVERS,
      TOKEN_KEYWORD_MBRDISJOINT,
      TOKEN_KEYWORD_MBREQUAL,
      TOKEN_KEYWORD_MBREQUALS,
      TOKEN_KEYWORD_MBRINTERSECTS,
      TOKEN_KEYWORD_MBROVERLAPS,
      TOKEN_KEYWORD_MBRTOUCHES,
      TOKEN_KEYWORD_MBRWITHIN,
      TOKEN_KEYWORD_MD5,
      TOKEN_KEYWORD_MEDIUMBLOB,
      TOKEN_KEYWORD_MEDIUMINT,
      TOKEN_KEYWORD_MEDIUMTEXT,
      TOKEN_KEYWORD_MICROSECOND,
      TOKEN_KEYWORD_MID,
      TOKEN_KEYWORD_MIDDLEINT,
      TOKEN_KEYWORD_MIN,
      TOKEN_KEYWORD_MINUTE,
      TOKEN_KEYWORD_MINUTE_MICROSECOND,
      TOKEN_KEYWORD_MINUTE_SECOND,
      TOKEN_KEYWORD_MINVALUE,
      TOKEN_KEYWORD_MLINEFROMTEXT,
      TOKEN_KEYWORD_MLINEFROMWKB,
      TOKEN_KEYWORD_MOD,
      TOKEN_KEYWORD_MODE,
      TOKEN_KEYWORD_MODIFIES,
      TOKEN_KEYWORD_MONTH,
      TOKEN_KEYWORD_MONTHNAME,
      TOKEN_KEYWORD_MPOINTFROMTEXT,
      TOKEN_KEYWORD_MPOINTFROMWKB,
      TOKEN_KEYWORD_MPOLYFROMTEXT,
      TOKEN_KEYWORD_MPOLYFROMWKB,
      TOKEN_KEYWORD_MULTILINESTRING,
      TOKEN_KEYWORD_MULTILINESTRINGFROMTEXT,
      TOKEN_KEYWORD_MULTILINESTRINGFROMWKB,
      TOKEN_KEYWORD_MULTIPOINT,
      TOKEN_KEYWORD_MULTIPOINTFROMTEXT,
      TOKEN_KEYWORD_MULTIPOINTFROMWKB,
      TOKEN_KEYWORD_MULTIPOLYGON,
      TOKEN_KEYWORD_MULTIPOLYGONFROMTEXT,
      TOKEN_KEYWORD_MULTIPOLYGONFROMWKB,
      TOKEN_KEYWORD_NAME_CONST,
      TOKEN_KEYWORD_NATURAL,
      TOKEN_KEYWORD_NEXTVAL,
      TOKEN_KEYWORD_NIL,
      TOKEN_KEYWORD_NO,
      TOKEN_KEYWORD_NOPAGER,
      TOKEN_KEYWORD_NOT,
      TOKEN_KEYWORD_NOTEE,
      TOKEN_KEYWORD_NOTNULL,
      TOKEN_KEYWORD_NOW,
      TOKEN_KEYWORD_NOWARNING,
      TOKEN_KEYWORD_NO_WRITE_TO_BINLOG,
      TOKEN_KEYWORD_NTH_VALUE,
      TOKEN_KEYWORD_NTILE,
      TOKEN_KEYWORD_NULL,
      TOKEN_KEYWORD_NULLIF,
      TOKEN_KEYWORD_NULLS,
      TOKEN_KEYWORD_NUMBER,
      TOKEN_KEYWORD_NUMERIC,
      TOKEN_KEYWORD_NUMGEOMETRIES,
      TOKEN_KEYWORD_NUMINTERIORRINGS,
      TOKEN_KEYWORD_NUMPOINTS,
      TOKEN_KEYWORD_NVARCHAR2,
      TOKEN_KEYWORD_OCT,
      TOKEN_KEYWORD_OCTET_LENGTH,
      TOKEN_KEYWORD_OF,
      TOKEN_KEYWORD_OFF,
      TOKEN_KEYWORD_OFFSET,
      TOKEN_KEYWORD_OJ,
      TOKEN_KEYWORD_OLD_PASSWORD,
      TOKEN_KEYWORD_ON,
      TOKEN_KEYWORD_OPEN,
      TOKEN_KEYWORD_OPTIMIZE,
      TOKEN_KEYWORD_OPTIMIZER_COSTS,
      TOKEN_KEYWORD_OPTION,
      TOKEN_KEYWORD_OPTIONALLY,
      TOKEN_KEYWORD_OR,
      TOKEN_KEYWORD_ORD,
      TOKEN_KEYWORD_ORDER,
      TOKEN_KEYWORD_OUT,
      TOKEN_KEYWORD_OUTER,
      TOKEN_KEYWORD_OUTFILE,
      TOKEN_KEYWORD_OVER,
      TOKEN_KEYWORD_OVERLAPS,
      TOKEN_KEYWORD_PAGER,
      TOKEN_KEYWORD_PARTIAL,
      TOKEN_KEYWORD_PARTITION,
      TOKEN_KEYWORD_PASSWORD,
      TOKEN_KEYWORD_PERCENT_RANK,
      TOKEN_KEYWORD_PERIOD_ADD,
      TOKEN_KEYWORD_PERIOD_DIFF,
      TOKEN_KEYWORD_PI,
      TOKEN_KEYWORD_PLAN,
      TOKEN_KEYWORD_POINT,
      TOKEN_KEYWORD_POINTFROMTEXT,
      TOKEN_KEYWORD_POINTFROMWKB,
      TOKEN_KEYWORD_POINTN,
      TOKEN_KEYWORD_POLYFROMTEXT,
      TOKEN_KEYWORD_POLYFROMWKB,
      TOKEN_KEYWORD_POLYGON,
      TOKEN_KEYWORD_POLYGONFROMTEXT,
      TOKEN_KEYWORD_POLYGONFROMWKB,
      TOKEN_KEYWORD_POSITION,
      TOKEN_KEYWORD_POW,
      TOKEN_KEYWORD_POWER,
      TOKEN_KEYWORD_PRAGMA,
      TOKEN_KEYWORD_PRECEDES,
      TOKEN_KEYWORD_PRECISION,
      TOKEN_KEYWORD_PREPARE,
      TOKEN_KEYWORD_PRIMARY,
      TOKEN_KEYWORD_PRINT,
      TOKEN_KEYWORD_PRINTF,
      TOKEN_KEYWORD_PROCEDURE,
      TOKEN_KEYWORD_PROCESS,
      TOKEN_KEYWORD_PROMPT,
      TOKEN_KEYWORD_PROXY,
      TOKEN_KEYWORD_PURGE,
      TOKEN_KEYWORD_QUARTER,
      TOKEN_KEYWORD_QUERY,
      TOKEN_KEYWORD_QUIT,
      TOKEN_KEYWORD_QUOTE,
      TOKEN_KEYWORD_RADIANS,
      TOKEN_KEYWORD_RAISE,
      TOKEN_KEYWORD_RAND,
      TOKEN_KEYWORD_RANDOM,
      TOKEN_KEYWORD_RANDOMBLOB,
      TOKEN_KEYWORD_RANDOM_BYTES,
      TOKEN_KEYWORD_RANGE,
      TOKEN_KEYWORD_RANK,
      TOKEN_KEYWORD_RAW,
      TOKEN_KEYWORD_READ,
      TOKEN_KEYWORD_READS,
      TOKEN_KEYWORD_READ_WRITE,
      TOKEN_KEYWORD_REAL,
      TOKEN_KEYWORD_REBUILD,
      TOKEN_KEYWORD_RECURSIVE,
      TOKEN_KEYWORD_REDUNDANT,
      TOKEN_KEYWORD_REFERENCES,
      TOKEN_KEYWORD_REGEXP,
      TOKEN_KEYWORD_REHASH,
      TOKEN_KEYWORD_REINDEX,
      TOKEN_KEYWORD_RELEASE,
      TOKEN_KEYWORD_RELEASE_ALL_LOCKS,
      TOKEN_KEYWORD_RELEASE_LOCK,
      TOKEN_KEYWORD_RELOAD,
      TOKEN_KEYWORD_RENAME,
      TOKEN_KEYWORD_REORGANIZE,
      TOKEN_KEYWORD_REPAIR,
      TOKEN_KEYWORD_REPEAT,
      TOKEN_KEYWORD_REPLACE,
      TOKEN_KEYWORD_REPLICATION,
      TOKEN_KEYWORD_REQUIRE,
      TOKEN_KEYWORD_RESET,
      TOKEN_KEYWORD_RESETCONNECTION,
      TOKEN_KEYWORD_RESIGNAL,
      TOKEN_KEYWORD_RESTRICT,
      TOKEN_KEYWORD_RETURN,
      TOKEN_KEYWORD_RETURNS,
      TOKEN_KEYWORD_REVERSE,
      TOKEN_KEYWORD_REVOKE,
      TOKEN_KEYWORD_RIGHT,
      TOKEN_KEYWORD_RLIKE,
      TOKEN_KEYWORD_ROLE,
      TOKEN_KEYWORD_ROLLBACK,
      TOKEN_KEYWORD_ROUND,
      TOKEN_KEYWORD_ROW,
      TOKEN_KEYWORD_ROW_COUNT,
      TOKEN_KEYWORD_ROW_NUMBER,
      TOKEN_KEYWORD_RPAD,
      TOKEN_KEYWORD_RTRIM,
      TOKEN_KEYWORD_SAVEPOINT,
      TOKEN_KEYWORD_SCHEMA,
      TOKEN_KEYWORD_SCHEMAS,
      TOKEN_KEYWORD_SECOND,
      TOKEN_KEYWORD_SECOND_MICROSECOND,
      TOKEN_KEYWORD_SECURITY,
      TOKEN_KEYWORD_SEC_TO_TIME,
      TOKEN_KEYWORD_SELECT,
      TOKEN_KEYWORD_SENSITIVE,
      TOKEN_KEYWORD_SEPARATOR,
      TOKEN_KEYWORD_SEQUENCE,
      TOKEN_KEYWORD_SERIAL,
      TOKEN_KEYWORD_SERVER,
      TOKEN_KEYWORD_SESSION,
      TOKEN_KEYWORD_SESSION_USER,
      TOKEN_KEYWORD_SET,
      TOKEN_KEYWORD_SETVAL,
      TOKEN_KEYWORD_SHA,
      TOKEN_KEYWORD_SHA1,
      TOKEN_KEYWORD_SHA2,
      TOKEN_KEYWORD_SHARED,
      TOKEN_KEYWORD_SHOW,
      TOKEN_KEYWORD_SHUTDOWN,
      TOKEN_KEYWORD_SIGN,
      TOKEN_KEYWORD_SIGNAL,
      TOKEN_KEYWORD_SIMPLE,
      TOKEN_KEYWORD_SIN,
      TOKEN_KEYWORD_SLEEP,
      TOKEN_KEYWORD_SLOW,
      TOKEN_KEYWORD_SMALLINT,
      TOKEN_KEYWORD_SONAME,
      TOKEN_KEYWORD_SOUNDEX,
      TOKEN_KEYWORD_SOURCE,
      TOKEN_KEYWORD_SPACE,
      TOKEN_KEYWORD_SPATIAL,
      TOKEN_KEYWORD_SPECIFIC,
      TOKEN_KEYWORD_SQL,
      TOKEN_KEYWORD_SQLEXCEPTION,
      TOKEN_KEYWORD_SQLSTATE,
      TOKEN_KEYWORD_SQLWARNING,
      TOKEN_KEYWORD_SQL_BIG_RESULT,
      TOKEN_KEYWORD_SQL_CALC_FOUND_ROWS,
      TOKEN_KEYWORD_SQL_SMALL_RESULT,
      TOKEN_KEYWORD_SQRT,
      TOKEN_KEYWORD_SRID,
      TOKEN_KEYWORD_SSL,
      TOKEN_KEYWORD_START,
      TOKEN_KEYWORD_STARTING,
      TOKEN_KEYWORD_STARTPOINT,
      TOKEN_KEYWORD_STATUS,
      TOKEN_KEYWORD_STD,
      TOKEN_KEYWORD_STDDEV,
      TOKEN_KEYWORD_STDDEV_POP,
      TOKEN_KEYWORD_STDDEV_SAMP,
      TOKEN_KEYWORD_STOP,
      TOKEN_KEYWORD_STORED,
      TOKEN_KEYWORD_STRAIGHT_JOIN,
      TOKEN_KEYWORD_STRCMP,
      TOKEN_KEYWORD_STRFTIME,
      TOKEN_KEYWORD_STR_TO_DATE,
      TOKEN_KEYWORD_ST_AREA,
      TOKEN_KEYWORD_ST_ASBINARY,
      TOKEN_KEYWORD_ST_ASGEOJSON,
      TOKEN_KEYWORD_ST_ASTEXT,
      TOKEN_KEYWORD_ST_ASWKB,
      TOKEN_KEYWORD_ST_ASWKT,
      TOKEN_KEYWORD_ST_BUFFER,
      TOKEN_KEYWORD_ST_BUFFER_STRATEGY,
      TOKEN_KEYWORD_ST_CENTROID,
      TOKEN_KEYWORD_ST_CONTAINS,
      TOKEN_KEYWORD_ST_CONVEXHULL,
      TOKEN_KEYWORD_ST_CROSSES,
      TOKEN_KEYWORD_ST_DIFFERENCE,
      TOKEN_KEYWORD_ST_DIMENSION,
      TOKEN_KEYWORD_ST_DISJOINT,
      TOKEN_KEYWORD_ST_DISTANCE,
      TOKEN_KEYWORD_ST_DISTANCE_SPHERE,
      TOKEN_KEYWORD_ST_ENDPOINT,
      TOKEN_KEYWORD_ST_ENVELOPE,
      TOKEN_KEYWORD_ST_EQUALS,
      TOKEN_KEYWORD_ST_EXTERIORRING,
      TOKEN_KEYWORD_ST_GEOHASH,
      TOKEN_KEYWORD_ST_GEOMCOLLFROMTEXT,
      TOKEN_KEYWORD_ST_GEOMCOLLFROMTXT,
      TOKEN_KEYWORD_ST_GEOMCOLLFROMWKB,
      TOKEN_KEYWORD_ST_GEOMETRYCOLLECTIONFROMTEXT,
      TOKEN_KEYWORD_ST_GEOMETRYCOLLECTIONFROMWKB,
      TOKEN_KEYWORD_ST_GEOMETRYFROMTEXT,
      TOKEN_KEYWORD_ST_GEOMETRYFROMWKB,
      TOKEN_KEYWORD_ST_GEOMETRYN,
      TOKEN_KEYWORD_ST_GEOMETRYTYPE,
      TOKEN_KEYWORD_ST_GEOMFROMGEOJSON,
      TOKEN_KEYWORD_ST_GEOMFROMTEXT,
      TOKEN_KEYWORD_ST_GEOMFROMWKB,
      TOKEN_KEYWORD_ST_INTERIORRINGN,
      TOKEN_KEYWORD_ST_INTERSECTION,
      TOKEN_KEYWORD_ST_INTERSECTS,
      TOKEN_KEYWORD_ST_ISCLOSED,
      TOKEN_KEYWORD_ST_ISEMPTY,
      TOKEN_KEYWORD_ST_ISSIMPLE,
      TOKEN_KEYWORD_ST_ISVALID,
      TOKEN_KEYWORD_ST_LATFROMGEOHASH,
      TOKEN_KEYWORD_ST_LENGTH,
      TOKEN_KEYWORD_ST_LINEFROMTEXT,
      TOKEN_KEYWORD_ST_LINEFROMWKB,
      TOKEN_KEYWORD_ST_LINESTRINGFROMTEXT,
      TOKEN_KEYWORD_ST_LINESTRINGFROMWKB,
      TOKEN_KEYWORD_ST_LONGFROMGEOHASH,
      TOKEN_KEYWORD_ST_MAKEENVELOPE,
      TOKEN_KEYWORD_ST_MLINEFROMTEXT,
      TOKEN_KEYWORD_ST_MLINEFROMWKB,
      TOKEN_KEYWORD_ST_MPOINTFROMTEXT,
      TOKEN_KEYWORD_ST_MPOINTFROMWKB,
      TOKEN_KEYWORD_ST_MPOLYFROMTEXT,
      TOKEN_KEYWORD_ST_MPOLYFROMWKB,
      TOKEN_KEYWORD_ST_MULTILINESTRINGFROMTEXT,
      TOKEN_KEYWORD_ST_MULTILINESTRINGFROMWKB,
      TOKEN_KEYWORD_ST_MULTIPOINTFROMTEXT,
      TOKEN_KEYWORD_ST_MULTIPOINTFROMWKB,
      TOKEN_KEYWORD_ST_MULTIPOLYGONFROMTEXT,
      TOKEN_KEYWORD_ST_MULTIPOLYGONFROMWKB,
      TOKEN_KEYWORD_ST_NUMGEOMETRIES,
      TOKEN_KEYWORD_ST_NUMINTERIORRING,
      TOKEN_KEYWORD_ST_NUMINTERIORRINGS,
      TOKEN_KEYWORD_ST_NUMPOINTS,
      TOKEN_KEYWORD_ST_OVERLAPS,
      TOKEN_KEYWORD_ST_POINTFROMGEOHASH,
      TOKEN_KEYWORD_ST_POINTFROMTEXT,
      TOKEN_KEYWORD_ST_POINTFROMWKB,
      TOKEN_KEYWORD_ST_POINTN,
      TOKEN_KEYWORD_ST_POLYFROMTEXT,
      TOKEN_KEYWORD_ST_POLYFROMWKB,
      TOKEN_KEYWORD_ST_POLYGONFROMTEXT,
      TOKEN_KEYWORD_ST_POLYGONFROMWKB,
      TOKEN_KEYWORD_ST_SIMPLIFY,
      TOKEN_KEYWORD_ST_SRID,
      TOKEN_KEYWORD_ST_STARTPOINT,
      TOKEN_KEYWORD_ST_SYMDIFFERENCE,
      TOKEN_KEYWORD_ST_TOUCHES,
      TOKEN_KEYWORD_ST_UNION,
      TOKEN_KEYWORD_ST_VALIDATE,
      TOKEN_KEYWORD_ST_WITHIN,
      TOKEN_KEYWORD_ST_X,
      TOKEN_KEYWORD_ST_Y,
      TOKEN_KEYWORD_SUBDATE,
      TOKEN_KEYWORD_SUBSTR,
      TOKEN_KEYWORD_SUBSTRING,
      TOKEN_KEYWORD_SUBSTRING_INDEX,
      TOKEN_KEYWORD_SUBTIME,
      TOKEN_KEYWORD_SUM,
      TOKEN_KEYWORD_SUPER,
      TOKEN_KEYWORD_SYSDATE,
      TOKEN_KEYWORD_SYSTEM,
      TOKEN_KEYWORD_SYSTEM_USER,
      TOKEN_KEYWORD_TABLE,
      TOKEN_KEYWORD_TABLESPACE,
      TOKEN_KEYWORD_TAN,
      TOKEN_KEYWORD_TEE,
      TOKEN_KEYWORD_TEMP,
      TOKEN_KEYWORD_TEMPORARY,
      TOKEN_KEYWORD_TERMINATED,
      TOKEN_KEYWORD_THEN,
      TOKEN_KEYWORD_TIME,
      TOKEN_KEYWORD_TIMEDIFF,
      TOKEN_KEYWORD_TIMESTAMP,
      TOKEN_KEYWORD_TIMESTAMPADD,
      TOKEN_KEYWORD_TIMESTAMPDIFF,
      TOKEN_KEYWORD_TIME_FORMAT,
      TOKEN_KEYWORD_TIME_TO_SEC,
      TOKEN_KEYWORD_TINYBLOB,
      TOKEN_KEYWORD_TINYINT,
      TOKEN_KEYWORD_TINYTEXT,
      TOKEN_KEYWORD_TO,
      TOKEN_KEYWORD_TOTAL_CHANGES,
      TOKEN_KEYWORD_TOUCHES,
      TOKEN_KEYWORD_TO_BASE64,
      TOKEN_KEYWORD_TO_DAYS,
      TOKEN_KEYWORD_TO_SECONDS,
      TOKEN_KEYWORD_TRAILING,
      TOKEN_KEYWORD_TRANSACTION,
      TOKEN_KEYWORD_TRIGGER,
      TOKEN_KEYWORD_TRIM,
      TOKEN_KEYWORD_TRUE,
      TOKEN_KEYWORD_TRUNCATE,
      TOKEN_KEYWORD_TYPEOF,
      TOKEN_KEYWORD_UCASE,
      TOKEN_KEYWORD_UNBOUNDED,
      TOKEN_KEYWORD_UNCOMPRESS,
      TOKEN_KEYWORD_UNCOMPRESSED_LENGTH,
      TOKEN_KEYWORD_UNDO,
      TOKEN_KEYWORD_UNHEX,
      TOKEN_KEYWORD_UNICODE,
      TOKEN_KEYWORD_UNINSTALL,
      TOKEN_KEYWORD_UNION,
      TOKEN_KEYWORD_UNIQUE,
      TOKEN_KEYWORD_UNIX_TIMESTAMP,
      TOKEN_KEYWORD_UNKNOWN,
      TOKEN_KEYWORD_UNLOCK,
      TOKEN_KEYWORD_UNSIGNED,
      TOKEN_KEYWORD_UNTIL,
      TOKEN_KEYWORD_UPDATE,
      TOKEN_KEYWORD_UPDATEXML,
      TOKEN_KEYWORD_UPPER,
      TOKEN_KEYWORD_USAGE,
      TOKEN_KEYWORD_USE,
      TOKEN_KEYWORD_USER,
      TOKEN_KEYWORD_USING,
      TOKEN_KEYWORD_UTC_DATE,
      TOKEN_KEYWORD_UTC_TIME,
      TOKEN_KEYWORD_UTC_TIMESTAMP,
      TOKEN_KEYWORD_UUID,
      TOKEN_KEYWORD_UUID_SHORT,
      TOKEN_KEYWORD_VACUUM,
      TOKEN_KEYWORD_VALIDATE_PASSWORD_STRENGTH,
      TOKEN_KEYWORD_VALIDATION,
      TOKEN_KEYWORD_VALUES,
      TOKEN_KEYWORD_VARBINARY,
      TOKEN_KEYWORD_VARCHAR,
      TOKEN_KEYWORD_VARCHAR2,
      TOKEN_KEYWORD_VARCHARACTER,
      TOKEN_KEYWORD_VARIANCE,
      TOKEN_KEYWORD_VARYING,
      TOKEN_KEYWORD_VAR_POP,
      TOKEN_KEYWORD_VAR_SAMP,
      TOKEN_KEYWORD_VERSION,
      TOKEN_KEYWORD_VIEW,
      TOKEN_KEYWORD_VIRTUAL,
      TOKEN_KEYWORD_WAIT_FOR_EXECUTED_GTID_SET,
      TOKEN_KEYWORD_WAIT_UNTIL_SQL_THREAD_AFTER_GTIDS,
      TOKEN_KEYWORD_WARNINGS,
      TOKEN_KEYWORD_WEEK,
      TOKEN_KEYWORD_WEEKDAY,
      TOKEN_KEYWORD_WEEKOFYEAR,
      TOKEN_KEYWORD_WEIGHT_STRING,
      TOKEN_KEYWORD_WHEN,
      TOKEN_KEYWORD_WHERE,
      TOKEN_KEYWORD_WHILE,
      TOKEN_KEYWORD_WITH,
      TOKEN_KEYWORD_WITHIN,
      TOKEN_KEYWORD_WITHOUT,
      TOKEN_KEYWORD_WRITE,
      TOKEN_KEYWORD_X,
      TOKEN_KEYWORD_XA,
      TOKEN_KEYWORD_XOR,
      TOKEN_KEYWORD_Y,
      TOKEN_KEYWORD_YEAR,
      TOKEN_KEYWORD_YEARWEEK,
      TOKEN_KEYWORD_YEAR_MONTH,
      TOKEN_KEYWORD_ZEROFILL,
      TOKEN_KEYWORD__ARMSCII8,
      TOKEN_KEYWORD__ASCII,
      TOKEN_KEYWORD__BIG5,
      TOKEN_KEYWORD__BINARY,
      TOKEN_KEYWORD__CP1250,
      TOKEN_KEYWORD__CP1251,
      TOKEN_KEYWORD__CP1256,
      TOKEN_KEYWORD__CP1257,
      TOKEN_KEYWORD__CP850,
      TOKEN_KEYWORD__CP852,
      TOKEN_KEYWORD__CP866,
      TOKEN_KEYWORD__CP932,
      TOKEN_KEYWORD__DEC8,
      TOKEN_KEYWORD__EUCJPMS,
      TOKEN_KEYWORD__EUCKR,
      TOKEN_KEYWORD__FILENAME,
      TOKEN_KEYWORD__GB2312,
      TOKEN_KEYWORD__GBK,
      TOKEN_KEYWORD__GEOSTD8,
      TOKEN_KEYWORD__GREEK,
      TOKEN_KEYWORD__HEBREW,
      TOKEN_KEYWORD__HP8,
      TOKEN_KEYWORD__KEYBCS2,
      TOKEN_KEYWORD__KOI8R,
      TOKEN_KEYWORD__KOI8U,
      TOKEN_KEYWORD__LATIN1,
      TOKEN_KEYWORD__LATIN2,
      TOKEN_KEYWORD__LATIN5,
      TOKEN_KEYWORD__LATIN7,
      TOKEN_KEYWORD__MACCE,
      TOKEN_KEYWORD__MACROMAN,
      TOKEN_KEYWORD__SJIS,
      TOKEN_KEYWORD__SWE7,
      TOKEN_KEYWORD__TIS620,
      TOKEN_KEYWORD__UCS2,
      TOKEN_KEYWORD__UJIS,
      TOKEN_KEYWORD__UTF16,
      TOKEN_KEYWORD__UTF16LE,
      TOKEN_KEYWORD__UTF32,
      TOKEN_KEYWORD__UTF8,
      TOKEN_KEYWORD__UTF8MB4,
#ifdef DEBUGGER
    TOKEN_KEYWORD_DEBUG_BREAKPOINT = TOKEN_KEYWORD__UTF8MB4 + 1,
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
    TOKEN_KEYWORD_DEBUG_TBREAKPOINT= TOKEN_KEYWORD_DEBUG_STEP + 1,
#endif
    TOKEN_TYPE_KEYWORD,  /* generic, lots of keywords have this */
    TOKEN_KEYWORD_BEGIN_WORK,         /* some non-reserved keywords */
    TOKEN_KEYWORD_BEGIN_XA,
    TOKEN_KEYWORD_CASE_IN_CASE_EXPRESSION,
    TOKEN_KEYWORD_END_IN_CASE_EXPRESSION,
    TOKEN_KEYWORD_IF_IN_IF_EXPRESSION,
    TOKEN_KEYWORD_IF_IN_IF_EXISTS,
    TOKEN_KEYWORD_REPEAT_IN_REPEAT_EXPRESSION,
    TOKEN_KEYWORD_DO_LUA,
    TOKEN_TYPE_DELIMITER
  };

/*
  TOKEN_TYPE_... shows "what kind of token is it?" e.g. TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK.
  TOKEN_REFTYPE_... shows "what kind of object does the token refer to?"
  e.g. if it's identifier, is it a database identifier?
  We only pass something specific if we are sure that is what must follow.
  Beware: A "user" might be qualified within 'user'@'host' I guess that's a form of qualifier.
  Beware: there are enum values for "x or y" e.g. "user or role", "database or table", etc.
  Todo: store these in main_token_reftypes[] to help hovering.
  Todo: knowing it's "column" doesn't help us yet with knowing: column of what?
        but eventually we can bring in lists of objects, and refer to them by number-within-the-list
  Todo: eventually we can be sure, after qualification is done, for a column (e.g. we've seen FROM)
  Todo: Get rid of enums that aren't actually used.
... And my plan is:
* Always pass reftype for hparse_f_accept and hparse_f_acceptn and hparse_f_expect
* If the pass is "[identifier]" then the expected list gets "[table identifier]", etc.
* Eventually, use this so we can auto-complete any object names
  (We'll have a local list of object names so we can store numbers.)
* Eventually, have reftypes for literals too -- again, meaning "what they refer to", not format
* Check: sometimes TOKEN_TYPE_IDENTIFIER_WITH_AT is not appropriate
* For LIMIT and OFFSET, the only possibilities are @variable and declared variable and parameter
* I'd also like to restrict what FETCH variable can be
* Whenever it is specific, IDENTIFIER_WITH_AT is not appropriate,
  and a maximum length is applicable such as MYSQL_MAX_IDENTIFIER_LENGTH.
* There is one case where we pass "[reserved function]" instead of "[identifier]".
*/
enum {
    TOKEN_REFTYPE_ANY,                 /* any kind, or it's irrelevant, or we don't care */
    TOKEN_REFTYPE_ALIAS, /* or correlation */
    TOKEN_REFTYPE_CHANNEL,
    TOKEN_REFTYPE_CHARACTER_SET,
    TOKEN_REFTYPE_COLLATION,
    TOKEN_REFTYPE_COLUMN,
    TOKEN_REFTYPE_COLUMN_OR_USER_VARIABLE,
    TOKEN_REFTYPE_COLUMN_OR_VARIABLE,
    TOKEN_REFTYPE_CONDITION_DEFINE,
    TOKEN_REFTYPE_CONDITION_REFER,
    TOKEN_REFTYPE_CONDITION_OR_CURSOR,
    TOKEN_REFTYPE_CONSTRAINT,
    TOKEN_REFTYPE_CURSOR_DEFINE,
    TOKEN_REFTYPE_CURSOR_REFER,
    TOKEN_REFTYPE_DATABASE, /* or schema */
    TOKEN_REFTYPE_DATABASE_OR_CONSTRAINT,
    TOKEN_REFTYPE_DATABASE_OR_EVENT,
    TOKEN_REFTYPE_DATABASE_OR_FUNCTION,
    TOKEN_REFTYPE_DATABASE_OR_FUNCTION_OR_PROCEDURE,
    TOKEN_REFTYPE_DATABASE_OR_PROCEDURE,
    TOKEN_REFTYPE_DATABASE_OR_SEQUENCE,
    TOKEN_REFTYPE_DATABASE_OR_TABLE,
    TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN,
    TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN_OR_FUNCTION,
    TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN_OR_FUNCTION_OR_VARIABLE,
    TOKEN_REFTYPE_DATABASE_OR_TRIGGER,
    TOKEN_REFTYPE_DATABASE_OR_VIEW,
    TOKEN_REFTYPE_ENGINE,
    TOKEN_REFTYPE_EVENT,
    TOKEN_REFTYPE_FUNCTION,
    TOKEN_REFTYPE_FUNCTION_OR_PROCEDURE,
    TOKEN_REFTYPE_FUNCTION_OR_VARIABLE,
    TOKEN_REFTYPE_HANDLER_ALIAS,
    TOKEN_REFTYPE_HOST,
    TOKEN_REFTYPE_INDEX,
    TOKEN_REFTYPE_INTRODUCER,
    TOKEN_REFTYPE_KEY_CACHE,
    TOKEN_REFTYPE_LABEL_DEFINE,
    TOKEN_REFTYPE_LABEL_REFER,
    TOKEN_REFTYPE_PARAMETER,
    TOKEN_REFTYPE_PARSER,
    TOKEN_REFTYPE_PLUGIN,
    TOKEN_REFTYPE_PROCEDURE,
    /* plus TOKEN_REFTYPE_RESERVED_FUNCTION */
    TOKEN_REFTYPE_PARTITION,
    TOKEN_REFTYPE_ROLE,
    TOKEN_REFTYPE_SAVEPOINT,
    TOKEN_REFTYPE_SEQUENCE,
    TOKEN_REFTYPE_SERVER,
    TOKEN_REFTYPE_STATEMENT,
    TOKEN_REFTYPE_SUBPARTITION,
    TOKEN_REFTYPE_TABLE,
    TOKEN_REFTYPE_TABLE_OR_COLUMN,
    TOKEN_REFTYPE_TABLE_OR_COLUMN_OR_FUNCTION,
    TOKEN_REFTYPE_TABLESPACE,
    TOKEN_REFTYPE_TRANSACTION,
    TOKEN_REFTYPE_TRIGGER,
    TOKEN_REFTYPE_USER,
    TOKEN_REFTYPE_USER_VARIABLE,
    TOKEN_REFTYPE_VARIABLE,         /* i.e. either USER_VARIABLE or DECLARED VARIABLE */
    TOKEN_REFTYPE_VARIABLE_DEFINE,
    TOKEN_REFTYPE_VARIABLE_REFER,
    TOKEN_REFTYPE_VIEW,
    TOKEN_REFTYPE_WITH_TABLE,
    TOKEN_REFTYPE_WRAPPER,
    TOKEN_REFTYPE_MAX
  };

};

#endif // MAINWINDOW_H

#ifdef DBMS_TARANTOOL
/*
  Tarantool comments
  ------------------

  Automatic field names. Every tuple is an array with possible sub-arrays.
  In standard SQL we'd designate with f[1] f[1][1] f[1][1][1].
  But we will prefer f_1 f_1_1 f_1_1 so it's compatible with MySQL.
  The letter "f" is arbitrary, it's #define TARANTOOL_FIELD_NAME_BASE.

  Todo: probable bug: a map of arrays will probably cause a crash.
*/

/* If you want field names to start with "foo" instead of "f", change TARANTOOL_FIELD_NAME_BASE. */
#define TARANTOOL_FIELD_NAME_BASE "f"

/* Names like f_1_1 are shorter than 64 characters, but we might have user-defined names. */


#define TARANTOOL_MAX_FIELD_NAME_LENGTH 64
#define TARANTOOL_BOX_INDEX_EQ 0
#define TARANTOOL_BOX_INDEX_ALL 2
#define TARANTOOL_BOX_INDEX_LT 3
#define TARANTOOL_BOX_INDEX_LE 4
#define TARANTOOL_BOX_INDEX_GE 5
#define TARANTOOL_BOX_INDEX_GT 6

#endif

/*****************************************************************************************************************************/
/* THE TEXTEDITFRAME WIDGET */
/* See comments containing the word TextEditFrame, in ResultGrid code. */

#ifndef TEXTEDITFRAME_H
#define TEXTEDITFRAME_H

#define TEXTEDITFRAME_CELL_TYPE_DETAIL 0
#define TEXTEDITFRAME_CELL_TYPE_HEADER 1
#define TEXTEDITFRAME_CELL_TYPE_DETAIL_EXTRA_RULE_1 2

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
  int ancestor_grid_result_row_number;
  unsigned int content_length;
  unsigned short int cell_type;                        /* detail or header or detail_extra_rule_1 */
  char *content_pointer;
  bool is_retrieved_flag;
  bool is_style_sheet_set_flag;
  bool is_image_flag;                    /* true if data type = blob and appropriate flag is on */

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
#endif // TEXTEDITFRAME_H

/*********************************************************************************************************/
/* THE TEXTEDITWIDGET WIDGET */
/* subclassed QTextEdit so paintEvent can be caught, for use in result_grid */

#ifndef TEXTEDITWIDGET_H
#define TEXTEDITWIDGET_H
class TextEditWidget : public QTextEdit
{
  Q_OBJECT

public:
  explicit TextEditWidget(QWidget *parent);
  ~TextEditWidget();

  TextEditFrame *text_edit_frame_of_cell;

  void copy();
protected:
  void paintEvent(QPaintEvent *event);
  void keyPressEvent(QKeyEvent *event);

  QString unstripper(QString value_to_unstrip);

};
#endif // TEXTEDITWIDGET_H


/*********************************************************************************************************/
/* THE ROW_FORM_BOX WIDGET */

/*
  Todo: Currently this is only used for connecting.
        But it should be very easy to use with result-table rows,
        either for vertical display or in response to a keystroke on ResultGrid.
*/

/*
  Todo: Settle minimum and maximum width of dialog box.
        The calculation of main_window_maximum_width has been helpful here.
        Settle maximum width of each item.

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

#ifndef ROW_FORM_BOX_H
#define ROW_FORM_BOX_H
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
  QWidget *widget_for_size_hint;

  int column_count_copy;
  int *row_form_is_password_copy;
  QString *row_form_data_copy;
  Row_form_box *this_row_form_box;
  int width_for_size_hint, height_for_size_hint;

public:
Row_form_box(int column_count, QString *row_form_label,
             int *row_form_type,
             int *row_form_is_password, QString *row_form_data,
//             QString *row_form_width,
             QString row_form_title, QString row_form_message,
             MainWindow *parent): QDialog(parent)
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
  widget_for_size_hint= 0;

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
  is_ok= 0;
  int character_height, borders_height, component_height;

  parent->component_size_calc(&character_height, &borders_height);
  component_height= character_height + borders_height;
  /*
    Subsequently, for spacing between lines, I finally realized that it's not enough
    to do setSpacing + setContentsMargins for the QVBoxLayout, I have to do them
    for each QHBoxLayout as well.
  */
  main_layout= new QVBoxLayout();
  main_layout->setSpacing(0);
  main_layout->setContentsMargins(QMargins(0, 0, 0, 0));
  main_layout->setSizeConstraint(QLayout::SetFixedSize);  /* necessary, but I don't know why */
  label_for_message= new QLabel(row_form_message);
  main_layout->addWidget(label_for_message);
  for (i= 0; i < column_count; ++i)
  {
    hbox_layout[i]= new QHBoxLayout();
    //hbox_layout[i]->setSpacing(0);
    hbox_layout[i]->setContentsMargins(QMargins(2, 2, 2, 2));
    hbox_layout[i]->setSizeConstraint(QLayout::SetFixedSize);  /* necessary, but I don't know why */
    label[i]= new QLabel();
    label[i]->setStyleSheet(parent->ocelot_grid_header_style_string);
    label[i]->setMinimumHeight(component_height);
    label[i]->setText(row_form_label[i]);
    hbox_layout[i]->addWidget(label[i]);

    if (row_form_is_password[i] == 1)
    {
      line_edit[i]= new QLineEdit();
      line_edit[i]->setStyleSheet(parent->ocelot_grid_style_string);
      line_edit[i]->insert(row_form_data[i]);
      line_edit[i]->setEchoMode(QLineEdit::Password); /* maybe PasswordEchoOnEdit would be better */
      line_edit[i]->setMaximumHeight(component_height);
      line_edit[i]->setMinimumHeight(component_height);
      hbox_layout[i]->addWidget(line_edit[i]);
    }
    else
    {
      text_edit[i]= new QTextEdit();

      if ((row_form_type[i] & READONLY_FLAG) != 0)
      {
        text_edit[i]->setStyleSheet(parent->ocelot_grid_header_style_string);
        text_edit[i]->setReadOnly(true);
      }
      else
      {
        text_edit[i]->setStyleSheet(parent->ocelot_grid_style_string);
        text_edit[i]->setReadOnly(false);
      }
      text_edit[i]->setText(row_form_data[i]);
      text_edit[i]->setMaximumHeight(component_height);
      text_edit[i]->setMinimumHeight(component_height);
      text_edit[i]->setTabChangesFocus(true);
      /* The following line will work, but I'm undecided whether it's desirable. */
      //if ((row_form_type[i] & NUM_FLAG) != 0) text_edit[i]->setAlignment(Qt::AlignRight);
      hbox_layout[i]->addWidget(text_edit[i]);
    }
    widget[i]= new QWidget();
    widget[i]->setLayout(hbox_layout[i]);
    main_layout->addWidget(widget[i]);
  }
  button_for_cancel= new QPushButton(menu_strings[menu_off + MENU_CANCEL], this);
  button_for_ok= new QPushButton(menu_strings[menu_off + MENU_OK], this);
  hbox_layout_for_ok_and_cancel= new QHBoxLayout();
  hbox_layout_for_ok_and_cancel->addWidget(button_for_cancel);
  hbox_layout_for_ok_and_cancel->addWidget(button_for_ok);
  widget_for_ok_and_cancel= new QWidget();
  widget_for_ok_and_cancel->setLayout(hbox_layout_for_ok_and_cancel);
  connect(button_for_ok, SIGNAL(clicked()), this, SLOT(handle_button_for_ok()));
  connect(button_for_cancel, SIGNAL(clicked()), this, SLOT(handle_button_for_cancel()));

  widget_with_main_layout= new QWidget();
  widget_with_main_layout->setLayout(main_layout);
  widget_with_main_layout->setMaximumHeight(200);
  scroll_area= new QScrollArea();
  scroll_area->setWidget(widget_with_main_layout);
  scroll_area->setWidgetResizable(true);

  upper_layout= new QVBoxLayout;
  upper_layout->addWidget(scroll_area);

  upper_layout->addWidget(widget_for_ok_and_cancel);

  /* Last-minute changes due to failure with Ubuntu 14.04 */
  /* Removing widget_for_size_hint */
  /* Removing QSize size_hint */
  /* Instead we'll use scroll bar always on */
  scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scroll_area->show();
  width_for_size_hint= scroll_area->width() + 25;
  height_for_size_hint= scroll_area->height();


  //{
  //  widget_for_size_hint= new QDialog(this);
  //  widget_for_size_hint->setLayout(upper_layout);
  //  widget_for_size_hint->setWindowOpacity(0);                      /* perhaps unnecessary */
  //  widget_for_size_hint->show();
  //  width_for_size_hint= widget_for_size_hint->width()
  //                     + scroll_area->verticalScrollBar()->width()
  //                     + 5;
  //  height_for_size_hint= widget_for_size_hint->height();
  //  widget_for_size_hint->close();
  //}

  this->setLayout(upper_layout);
  this->setWindowTitle(row_form_title);
}


/*
  Row_form_box will have the wrong width if everything is default:
  about 20 pixels too short, as if it didn't expect a vertical scroll bar.
    I could make Qt do a better calculation by saying
    scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    but someday I'll want to use Row_form_box for purposes besides connect.
  Our solution is to make a not-to-be-really-used widget,
  widget_for_size_hint, with the same layout, and use its width,
  plus the width of the vertical scroll bar, plus 5 (5 is arbitrary).
  We delete widget_for_size_hint during garbage_collect, but hope deletion is automatic.
  Todo: setWindowOpacity(0) for widget_for_size_hint?
*/
QSize sizeHint() const
{
  return QSize(width_for_size_hint, height_for_size_hint);
}

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
  if (widget_for_size_hint != 0) delete widget_for_size_hint;
}

};

#endif // ROW_FORM_BOX_H


/***********************************************************/
/* THE MESSAGE_BOX WIDGET */
/***********************************************************/

/*
  QMessageBox equivalent, but with scroll bars.
  (A simple QMessageBox has no scroll bars.)
  We need scroll bars for some Help displays especially if screen size is small.
  Todo: size calculation as in Row_form_box.
  Todo: Use colors and fonts specified for menu.
*/
#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

class Message_box: public QDialog
{
  Q_OBJECT
public:
  QDialog *message_box;
  bool is_ok;

private:
  int width_for_size_hint, height_for_size_hint;

public:
Message_box(QString the_title, QString the_text, int minimum_width, MainWindow *parent): QDialog(parent)
{

  QScrollArea *scroll_area= new QScrollArea(this);
  scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  QWidget *widget = new QWidget(this);
  scroll_area->setWidget(widget);
  scroll_area->setWidgetResizable(true);

  QVBoxLayout *layout = new QVBoxLayout(widget);
  widget->setLayout(layout);

  QTextEdit *text_edit= new QTextEdit(this);
  text_edit->setText(the_text);
  text_edit->setReadOnly(true);
  layout->addWidget(text_edit);

  QPushButton *push_button= new QPushButton(this);
  push_button->setText("OK");
  layout->addWidget(push_button);

  connect(push_button, SIGNAL(clicked()), this, SLOT(handle_button_for_ok()));
  this->setMinimumHeight(500);
  this->setMinimumWidth(minimum_width);
  this->setWindowTitle(the_title);
  QHBoxLayout *dialog_layout= new QHBoxLayout(this);
  this->setLayout(dialog_layout);
  this->layout()->addWidget(scroll_area);
}

private slots:

void handle_button_for_ok()
{
  /* Skipping garbage collect this time. */
  close();
}

};

#endif // MESSAGE_BOX_H

/***********************************************************/
/* The Low-Level DBMS calls */
/***********************************************************/

/*
  The functions that start with "ldbms_" are supposed to be low level.
  Eventually there might be "#if mysql ... #endif" code inside them.
  Eventually there might be a separate class with all dbms-related calls.
  But we're still a long way from having dbms-independent code here.
  We have these new lists, which are created when we make the grid and deleted when we stop:
    grid_column_dbms_sources[].       = obsolete
    grid_column_dbms_field_numbers[]. = obsolete
  The idea is that any column can be special, that is, can have a source other than
  the DBMS field. So far the only special column is the row number, experimentally.
*/

/* Dynamic load
* Why dynamic-loading at runtime:
  To get rid of the warning "no version information available"
  So that we don't necessarily depend on libmysqlclient -- someday we'll allow other connections.
  The mysql documentation specifically says to use LD_RUN_PATH for mysqlclient; Qt ignores that.
  On the other hand, this is a pretty convoluted wrapper -- probably there's a more standard way
  to do it.
  (I could have checked out Qt's DBMS wrappers, but preferred to learn something at the lower level.)
  Re: mysql_library_end + mysql_library_init versus mysql_server_end + mysql_server_init:
      supposedly mysql_server_* functions are deprecated since MySQL version 5.0.3, but
      I still ran into a recent MariaDB libmysqlclient that used them, therefore if
      initially we can't find mysql_library_* functions we look for mysql_server_* functions.
  Todo: check library version (not sure how I do that, though). Maybe mysql_client_version()?
  Todo: consider allowing library name as parameter e.g. --library=perconaserverclient.so'
  Todo: if error, error message should say what you looked for. if okay, say what you found.
  Todo: consider adding in ocelotgui.pro: /opt/mysql/lib /opt/mysql/lib/mysql /usr/sfw/lib
  Todo: consider executing mysql_config --libs and using what it returns
  Todo: MySQL 5.7 has libmysqlclient.so.20, maybe look for that before libmysqlclient.so.18

  Initiate with: ldbms *lmysql;
                 lmysql= new ldbms();
                 if (lmysql == NULL) ... (Failure is possible if library cannot be loaded.)
  Call with:     lmysql->ldbms_function();
*/

#ifndef LDBMS_H
#define LDBMS_H

enum ocelot_option
{
  OCELOT_OPTION_0=0, /* for ocelot_opt_connect_timeout */
  OCELOT_OPTION_1=1,  /* for ocelot_opt_compress */
  OCELOT_OPTION_2=2,  /* for ocelot_opt_named_pipe */
  OCELOT_OPTION_3=3,  /* for ocelot_init_command */
  OCELOT_OPTION_4=4,  /* for ocelot_read_default_file_as_utf8 */
  OCELOT_OPTION_5=5,  /* for ocelot_read_default_group_as_utf8 */
  OCELOT_OPTION_6=6,  /* for ocelot_set_charset_dir_as_utf8 */
  OCELOT_OPTION_7=7,  /* for ocelot_set_charset_name_as_utf8 */
  OCELOT_OPTION_8=8,  /* for ocelot_opt_local_infile */
  OCELOT_OPTION_9=9,  /* for ocelot_protocol_as_int */
  OCELOT_OPTION_10=10,  /* for ocelot_shared_memory_base_name_as_utf8 */
  OCELOT_OPTION_11=11,  /* for ocelot_opt_read_timeout */
  OCELOT_OPTION_12=12,  /* for ocelot_opt_write_timeout */
  OCELOT_OPTION_13=13,  /* unused. in MySQL, opt_use_result */
  OCELOT_OPTION_14=14,  /* unused. in MySQL, use_remote_connection */
  OCELOT_OPTION_15=15,  /* unused. in MySQL, use_embedded_connection */
  OCELOT_OPTION_16=16,  /* unused. in MySQL, opt_guess_connection */
  OCELOT_OPTION_17=17,  /* unused. in MySQL, set_client_ip */
  OCELOT_OPTION_18=18,  /* for ocelot_secure_auth */
  OCELOT_OPTION_19=19,  /* for ocelot_report_data_truncation */
  OCELOT_OPTION_20=20,  /* for ocelot_opt_reconnect */
  OCELOT_OPTION_21=21,  /* ocelot_opt_ssl_verify_server_cert */
  OCELOT_OPTION_22=22,  /* ocelot_plugin_dir_as_utf8 */
  OCELOT_OPTION_23=23,  /* for ocelot_default_auth_as_utf8 */
  OCELOT_OPTION_24=24,  /* for ocelot_opt_bind_as_utf8 */
  OCELOT_OPTION_25=25,  /* unused. in MySQL, ssl_key */
  OCELOT_OPTION_26=26,  /* unused. in MySQL, opt_ssl_cert */
  OCELOT_OPTION_27=27,  /* unused. in MySQL, opt_ssl_ca */
  OCELOT_OPTION_28=28,  /* unused. in MySQL, opt_ssl_capath */
  OCELOT_OPTION_29=29,  /* unused. in MySQL, opt_ssl_cipher */
  OCELOT_OPTION_30=30,  /* for ocelot_opt_ssl_crl_as_utf8 */
  OCELOT_OPTION_31=31,  /* for ocelot_opt_ssl_crlpath_as_utf8 */
  OCELOT_OPTION_32=32,  /* for ocelot_opt_connect_attr_reset */
  OCELOT_OPTION_33=33,  /* unused. in MySQL, connect_attr_add */
  OCELOT_OPTION_34=34,  /* for ocelot_opt_connect_attr_delete_as_utf8 */
  OCELOT_OPTION_35=35,  /* for ocelot_server_public_key_as_utf8 */
  OCELOT_OPTION_36=36,  /* for ocelot_enable_cleartext_plugin */
  OCELOT_OPTION_37=37,  /* for ocelot_opt_can_handle_expired_passwords */
  OCELOT_OPTION_38=38,  /* for ocelot_opt_ssl_enforce */
  OCELOT_OPTION_39=39,  /* unused. in MySQL, opt_max_allowed_packet */
  OCELOT_OPTION_40=40,  /* unused. in MySQL, opt_net_buffer_length */
  OCELOT_OPTION_41=41,  /* unused. in MySQL, opt_tls_version */
  OCELOT_OPTION_42=42,  /* in MySQL 5.7.11+, opt_ssl_mode */
  OCELOT_OPTION_5999=5999,  /*unused. In MariaDB, progress_callback */
  OCELOT_OPTION_6000=6000,  /* unused. In MariaDB, nonblock */
  OCELOT_OPTION_6001=6001  /* unused. in MariaDB, thread_specific_memory */
};

#ifndef HEADER_AES_H
#define AES_BLOCK_SIZE 16
typedef struct aes_key_st { unsigned char x[244]; } AES_KEY;
#endif

class ldbms : public QWidget
{
public:

  void *dlopen_handle;

  /* For Qt typedef example see http://doc.qt.io/qt-4.8/qlibrary.html#fileName-prop */

  typedef my_ulonglong    (*tmysql_affected_rows)(MYSQL *);
  typedef void            (*tmysql_close)        (MYSQL *);
  typedef void            (*tmysql_data_seek)    (MYSQL_RES *, my_ulonglong);
  typedef unsigned int    (*tmysql_errno)        (MYSQL *);
  typedef const char*     (*tmysql_error)        (MYSQL *);
  typedef MYSQL_FIELD*    (*tmysql_fetch_fields) (MYSQL_RES *);
  typedef unsigned long*  (*tmysql_fetch_lengths)(MYSQL_RES *);
  typedef MYSQL_ROW       (*tmysql_fetch_row)    (MYSQL_RES *);
  typedef void            (*tmysql_free_result)  (MYSQL_RES *);
  typedef const char*     (*tmysql_get_client_info) (void);
  typedef const char*     (*tmysql_get_host_info)(MYSQL *);
  typedef const char*     (*tmysql_info)         (MYSQL *);
  typedef MYSQL*          (*tmysql_init)         (MYSQL *);
  typedef void            (*tmysql_library_end)  (void);
  typedef int             (*tmysql_library_init) (int, char **, char **);
  typedef my_bool         (*tmysql_more_results) (MYSQL *);
  typedef int             (*tmysql_next_result)  (MYSQL *);
  typedef unsigned int    (*tmysql_num_fields)   (MYSQL_RES *);
  typedef my_ulonglong    (*tmysql_num_rows)     (MYSQL_RES *);
  typedef int             (*tmysql_options)      (MYSQL *, enum ocelot_option, const char *);
  typedef int             (*tmysql_ping)         (MYSQL *);
  typedef int             (*tmysql_query)        (MYSQL *, const char *);
  typedef MYSQL*          (*tmysql_real_connect) (MYSQL *, const char *,
                                                  const char *,
                                                  const char *,
                                                  const char *,
                                                  unsigned int,
                                                  const char *,
                                                  unsigned long);
  typedef int             (*tmysql_real_query)   (MYSQL *, const char *, unsigned long);
  typedef int             (*tmysql_select_db)    (MYSQL *, const char *);
  typedef char*           (*tmysql_sqlstate)     (MYSQL *);
  typedef bool            (*tmysql_ssl_set)      (MYSQL *, const char *,
                                                  const char *,
                                                  const char *,
                                                  const char *,
                                                  const char *);
  typedef MYSQL_RES*      (*tmysql_store_result) (MYSQL *);
  typedef void            (*tmysql_thread_end)   (void);
  typedef unsigned int    (*tmysql_warning_count)(MYSQL *);
  typedef int             (*tAES_set_decrypt_key)(unsigned char *, int, AES_KEY *);
  typedef void            (*tAES_decrypt)        (unsigned char *, unsigned char *, AES_KEY *);
#ifdef DBMS_TARANTOOL
  typedef uint32_t        (*tmp_decode_array)    (const char **data);
  typedef const char*     (*tmp_decode_bin)      (const char **data, uint32_t *len);
  //typedef uint32_t        (*tmp_decode_binl)     (const char **data);
  typedef bool            (*tmp_decode_bool)     (const char **data);
  typedef float           (*tmp_decode_float)    (const char **data);
  typedef double          (*tmp_decode_double)   (const char **data);
  typedef int64_t         (*tmp_decode_int)      (const char **data);
  typedef uint32_t        (*tmp_decode_map)      (const char **data);
  typedef void            (*tmp_decode_nil)      (const char **data);
  typedef uint64_t        (*tmp_decode_uint)     (const char **data);
  typedef const char*     (*tmp_decode_str)      (const char **data, uint32_t *len);
  typedef uint32_t        (*tmp_decode_strl)     (const char **data);
  typedef void            (*tmp_next)            (const char **data);
  typedef enum mp_type    (*tmp_typeof)          (const char c);
  typedef ssize_t         (*ttnt_auth)           (struct tnt_stream *, const char *, int, const char *, int);
  typedef ssize_t         (*ttnt_call)           (struct tnt_stream *, const char *, size_t, struct tnt_stream *);
  typedef void            (*ttnt_close)          (struct tnt_stream *);
  typedef int             (*ttnt_connect)        (struct tnt_stream *);
  typedef ssize_t         (*ttnt_eval)           (struct tnt_stream *, const char *, size_t, struct tnt_stream *);
  typedef ssize_t         (*ttnt_delete)         (struct tnt_stream *, uint32_t, uint32_t, tnt_stream *);
  typedef ssize_t         (*ttnt_flush)          (struct tnt_stream *);
  typedef int             (*ttnt_get_indexno)    (struct tnt_stream *, int, const char *, size_t);
  typedef int             (*ttnt_get_spaceno)    (struct tnt_stream *, const char *, size_t);
  typedef void            (*ttnt_stream_free)    (struct tnt_stream *);
  typedef ssize_t         (*ttnt_insert)         (struct tnt_stream *, uint32_t, struct tnt_stream *);
  typedef tnt_stream*     (*ttnt_net)            (struct tnt_stream *);
  typedef tnt_stream*     (*ttnt_object)         (struct tnt_stream *);
  typedef ssize_t         (*ttnt_object_add_array)(struct tnt_stream *, uint32_t);
  typedef ssize_t         (*ttnt_object_add_nil) (struct tnt_stream *);
  typedef ssize_t         (*ttnt_object_add_int) (struct tnt_stream *, int64_t);
  typedef ssize_t         (*ttnt_object_add_str) (struct tnt_stream *, const char *, uint32_t);
  typedef ssize_t         (*ttnt_object_add_bin) (struct tnt_stream *, const void *, uint32_t);
  typedef ssize_t         (*ttnt_object_add_bool)(struct tnt_stream *, char);
  typedef ssize_t         (*ttnt_object_add_float)(struct tnt_stream *s, float);
  typedef ssize_t         (*ttnt_object_add_double)(struct tnt_stream *, double);
  typedef ssize_t         (*ttnt_object_container_close)(struct tnt_stream *);
  typedef ssize_t         (*ttnt_object_format)  (struct tnt_stream *, const char *, int);
  typedef int             (*ttnt_object_reset)   (struct tnt_stream *);
  typedef int             (*ttnt_reload_schema)  (struct tnt_stream *);
  typedef ssize_t         (*ttnt_replace)        (struct tnt_stream *, uint32_t, struct tnt_stream *);
  typedef int             (*ttnt_reply)          (struct tnt_reply *, char *, size_t, size_t *);
  typedef tnt_reply*      (*ttnt_reply_init)     (struct tnt_reply *);
  typedef void            (*ttnt_reply_free)     (struct tnt_reply *);
  typedef int64_t         (*ttnt_request_compile)(struct tnt_stream *, struct tnt_request *);
  typedef tnt_request*    (*ttnt_request_eval)   (struct tnt_request *);
  typedef int             (*ttnt_request_set_exprz) (struct tnt_request *, const char *);
  typedef int             (*ttnt_request_set_tuple) (struct tnt_request *, struct tnt_stream *);
  typedef ssize_t         (*ttnt_select)         (struct tnt_stream *, uint32_t, uint32_t, uint32_t, uint32_t, uint8_t, struct tnt_stream *);
  typedef int             (*ttnt_set)            (struct tnt_stream *, int, char *);
  typedef ssize_t         (*ttnt_update)         (struct tnt_stream *, uint32_t, uint32_t, struct tnt_stream *, struct tnt_stream *);
#endif

  tmysql_affected_rows t__mysql_affected_rows;   /* libmysqlclient */
  tmysql_close t__mysql_close;
  tmysql_data_seek t__mysql_data_seek;
  tmysql_errno t__mysql_errno;
  tmysql_error t__mysql_error;
  tmysql_fetch_fields t__mysql_fetch_fields;
  tmysql_fetch_lengths t__mysql_fetch_lengths;
  tmysql_fetch_row t__mysql_fetch_row;
  tmysql_free_result t__mysql_free_result;
  tmysql_get_client_info t__mysql_get_client_info;
  tmysql_get_host_info t__mysql_get_host_info;
  tmysql_info t__mysql_info;
  tmysql_init t__mysql_init;
  tmysql_library_end t__mysql_library_end;
  tmysql_library_init t__mysql_library_init;
  tmysql_more_results t__mysql_more_results;
  tmysql_next_result t__mysql_next_result;
  tmysql_num_fields t__mysql_num_fields;
  tmysql_num_rows t__mysql_num_rows;
  tmysql_options t__mysql_options;
  tmysql_ping t__mysql_ping;
  tmysql_query t__mysql_query;
  tmysql_real_connect t__mysql_real_connect;
  tmysql_real_query t__mysql_real_query;
  tmysql_select_db t__mysql_select_db;
  tmysql_sqlstate t__mysql_sqlstate;
  tmysql_ssl_set t__mysql_ssl_set;
  tmysql_store_result t__mysql_store_result;
  tmysql_thread_end t__mysql_thread_end;
  tmysql_warning_count t__mysql_warning_count;
  tAES_set_decrypt_key t__AES_set_decrypt_key;
  tAES_decrypt t__AES_decrypt;
#ifdef DBMS_TARANTOOL
  tmp_decode_array t__mp_decode_array;
  tmp_decode_bin t__mp_decode_bin;
  //tmp_decode_binl t__mp_decode_binl;
  tmp_decode_bool t__mp_decode_bool;
  tmp_decode_float t__mp_decode_float;
  tmp_decode_double t__mp_decode_double;
  tmp_decode_int t__mp_decode_int;
  tmp_decode_map t__mp_decode_map;
  tmp_decode_nil t__mp_decode_nil;
  tmp_decode_uint t__mp_decode_uint;
  tmp_decode_str t__mp_decode_str;
  tmp_decode_strl t__mp_decode_strl;
  tmp_next t__mp_next;
  tmp_typeof t__mp_typeof;
  ttnt_auth t__tnt_auth;
  ttnt_call t__tnt_call;
  ttnt_close t__tnt_close;
  ttnt_connect t__tnt_connect;
  ttnt_delete t__tnt_delete;
  ttnt_eval t__tnt_eval;
  ttnt_flush t__tnt_flush;
  ttnt_get_indexno t__tnt_get_indexno;
  ttnt_get_spaceno t__tnt_get_spaceno;
  ttnt_stream_free t__tnt_stream_free;
  ttnt_insert t__tnt_insert;
  ttnt_net t__tnt_net;
  ttnt_object t__tnt_object;
  ttnt_object_add_array t__tnt_object_add_array;
  ttnt_object_add_nil t__tnt_object_add_nil;
  ttnt_object_add_int t__tnt_object_add_int;
  ttnt_object_add_str t__tnt_object_add_str;
  ttnt_object_add_bin t__tnt_object_add_bin;
  ttnt_object_add_bool t__tnt_object_add_bool;
  ttnt_object_add_float t__tnt_object_add_float;
  ttnt_object_add_double t__tnt_object_add_double;
  ttnt_object_container_close t__tnt_object_container_close;
  ttnt_object_format t__tnt_object_format;
  ttnt_object_reset t__tnt_object_reset;
  ttnt_reload_schema t__tnt_reload_schema;
  ttnt_replace t__tnt_replace;
  ttnt_reply t__tnt_reply;
  ttnt_reply_init t__tnt_reply_init;
  ttnt_reply_free t__tnt_reply_free;
  ttnt_request_compile t__tnt_request_compile;
  ttnt_request_eval t__tnt_request_eval;
  ttnt_request_set_exprz t__tnt_request_set_exprz;
  ttnt_request_set_tuple t__tnt_request_set_tuple;
  ttnt_select t__tnt_select;
  ttnt_set t__tnt_set;
  ttnt_update t__tnt_update;
#endif

ldbms() : QWidget()
{
  return;
}

#define WHICH_LIBRARY_LIBMYSQLCLIENT 0
#define WHICH_LIBRARY_LIBCRYPTO 1
#define WHICH_LIBRARY_LIBMYSQLCLIENT18 2
#ifdef DBMS_TARANTOOL
#define WHICH_LIBRARY_LIBTARANTOOL 3
#define WHICH_LIBRARY_LIBTARANTOOLNET 4
#endif


void ldbms_get_library(QString ocelot_ld_run_path,
        int *is_library_loaded,           /* points to is_libXXX_loaded */
        void **library_handle,            /* points to libXXX_handle */
        QString *return_string,
        int which_library)                /* 0 = libmysqlclient. 1 = libcrypto, etc. */
  {
    char *query;
    int query_len;
    QString error_string;

    /*
      What's with all the #ifdef __linux stuff?
      Originally I coded for QLibrary. For reasons I couldn't figure out,
      t__mysql_real_connect crashed. So I switched to dlopen().
      I suspect the ability to say RTLD_DEEPBIND | RTLD_NOW had something to do with it.
      WIthout RTLD_DEEPBIND it crashed though that might be an obsolete observation.
      But perhaps someday we'll want to revive QLibrary for other platforms.

      There is a description re finding libmysqlclient if one types Help | libmysqlclient.
    */

#ifndef __linux
  QLibrary lib;
#endif

    if (*is_library_loaded == 1)
    {
      /*
        Since this is called when we're re-connecting,
        I'd like to dclose() and dlopen() again in case library name has changed.
        But apparently dclose() is not reliable. So don't change library name, eh?
      */
      return;
    }
    if (*is_library_loaded == -2)
    {
      /* The last error was that we got the wrong library. Unrecoverable. */
      return;
    }
#ifndef __linux
    /* I don't know how Windows handles shared-library version numbers */
    if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT18) lib.setFileNameAndVersion("libmysql", 18);
    if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT) lib.setFileName("libmysql");
    if (which_library == WHICH_LIBRARY_LIBCRYPTO) lib.setFileName("libeay32");
#ifdef DBMS_TARANTOOL
    if (which_library == WHICH_LIBRARY_LIBTARANTOOL) lib.setFileName("libtarantool");
    if (which_library == WHICH_LIBRARY_LIBTARANTOOLNET) lib.setFileName("libtarantoolnet");
#endif
#endif
    /*
      Finding libmysqlclient
      I tried "qApp->addLibraryPath(ld_run_path);" but it failed.
      So first I'll try hard-coding lib() with strings from ld_run_path.
    */
    if (ocelot_ld_run_path != "")
    {
      QString ld_run_path_part;
      char *ld_run_path;
      int ld_run_path_len;
      int i= 0;
      int prev_i= 0;

      ld_run_path_len= ocelot_ld_run_path.toUtf8().size();         /* See comment "UTF8 Conversion" */
      ld_run_path= new char[ld_run_path_len + 1];
      memcpy(ld_run_path, ocelot_ld_run_path.toUtf8().constData(), ld_run_path_len + 1);
      ld_run_path[ld_run_path_len]= '\0';

      for (;;)
      {
        for (i= prev_i;; ++i)
        {
          if ((*(ld_run_path + i) == ';') || (*(ld_run_path + i) == '\0')) break;
        }
        ld_run_path_part= ld_run_path + prev_i;
        ld_run_path_part= ld_run_path_part.left(i - prev_i);
        ld_run_path_part= ld_run_path_part.trimmed();
        if (ld_run_path_part > "")
        {
#ifdef __linux
          if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT18) ld_run_path_part.append("/libmysqlclient.so.18");
          if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT) ld_run_path_part.append("/libmysqlclient.so");
          if (which_library == WHICH_LIBRARY_LIBCRYPTO) ld_run_path_part.append("/libcrypto.so");
#ifdef DBMS_TARANTOOL
          if (which_library == WHICH_LIBRARY_LIBTARANTOOL) ld_run_path_part.append("/libtarantool.so");
          if (which_library == WHICH_LIBRARY_LIBTARANTOOLNET) ld_run_path_part.append("/libtarantoolnet.so");
#endif
          query_len= ld_run_path_part.toUtf8().size();         /* See comment "UTF8 Conversion" */
          query= new char[query_len + 1];
          memcpy(query, ld_run_path_part.toUtf8().constData(), query_len + 1);
          query[query_len]= '\0';
          dlopen_handle= dlopen(query,  RTLD_DEEPBIND | RTLD_NOW);
          delete []query;
          if (dlopen_handle == 0) {*is_library_loaded= 0; error_string= dlerror(); }
          else *is_library_loaded= 1;
          *library_handle= dlopen_handle;
#else
          if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT18) ld_run_path_part.append("/libmysqlclient");
          if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT) ld_run_path_part.append("/libmysqlclient");
          if (which_library == WHICH_LIBRARY_LIBCRYPTO) ld_run_path_part.append("/libcrypto");
#ifdef DBMS_TARANTOOL
          if (which_library == WHICH_LIBRARY_LIBTARANTOOL) ld_run_path_part.append("/libtarantool");
          if (which_library == WHICH_LIBRARY_LIBTARANTOOLNET) ld_run_path_part.append("/libtarantoolnet");
#endif
          lib.setFileName(ld_run_path_part);
          *is_library_loaded= lib.load();
          error_string= lib.errorString();
#endif
          if (*is_library_loaded == 1) break;
        }
        if (*(ld_run_path + i) == '\0') break;
        prev_i= i + 1;
      }
      delete []ld_run_path;
    }

    /*
      If ld_run_path was passed, and we found nothing, return now. We want to check for libmysqlclient.so.18
      along run path first (regardless whether it's for libmysqlclient.so.18 or libmysqlclient.so)
    */
    if ((ocelot_ld_run_path != "") && (*is_library_loaded == 0)) return;

    /* If it wasn't found via LD_RUN_PATH, use defaults e.g. LD_LIBRARY_PATH */
    if (*is_library_loaded == 0)
    {
#ifdef __linux
      if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT18) dlopen_handle= dlopen("libmysqlclient.so.18",  RTLD_DEEPBIND | RTLD_NOW);
      if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT) dlopen_handle= dlopen("libmysqlclient.so",  RTLD_DEEPBIND | RTLD_NOW);
      if (which_library == WHICH_LIBRARY_LIBCRYPTO) dlopen_handle= dlopen("libcrypto.so",  RTLD_DEEPBIND | RTLD_NOW);
#ifdef DBMS_TARANTOOL
      if (which_library == WHICH_LIBRARY_LIBTARANTOOL) dlopen_handle= dlopen("libtarantool.so",  RTLD_DEEPBIND | RTLD_NOW);
      if (which_library == WHICH_LIBRARY_LIBTARANTOOLNET) dlopen_handle= dlopen("libtarantoolnet.so",  RTLD_DEEPBIND | RTLD_NOW);
#endif
      if (dlopen_handle == 0) {*is_library_loaded= 0; error_string= dlerror(); }
      else *is_library_loaded= 1;
      *library_handle= dlopen_handle;
#else
      if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT18) lib.setFileName("libmysql");
      if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT) lib.setFileName("libmysql");
      if (which_library == WHICH_LIBRARY_LIBCRYPTO) lib.setFileName("libeay32");
#ifdef DBMS_TARANTOOL
      if (which_library == WHICH_LIBRARY_LIBTARANTOOL) lib.setFileName("libtarantool");
      if (which_library == WHICH_LIBRARY_LIBTARANTOOLNET) lib.setFileName("libtarantoolnet");
#endif
      *is_library_loaded= lib.load();
      error_string= lib.errorString();
#endif
    }
    if (*is_library_loaded == 0)
    {
      *return_string= error_string;
      return;
    }
    if (*is_library_loaded == 1)
    {
      QString s= "";
#ifdef __linux
      if ((which_library == WHICH_LIBRARY_LIBMYSQLCLIENT18) || (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT))
      {
        t__mysql_affected_rows= (tmysql_affected_rows) dlsym(dlopen_handle, "mysql_affected_rows"); if (dlerror() != 0) s.append("mysql_affected_rows ");
        t__mysql_close= (tmysql_close) dlsym(dlopen_handle, "mysql_close"); if (dlerror() != 0) s.append("mysql_close ");
        t__mysql_data_seek= (tmysql_data_seek) dlsym(dlopen_handle, "mysql_data_seek"); if (dlerror() != 0) s.append("mysql_data_seek ");
        t__mysql_errno= (tmysql_errno) dlsym(dlopen_handle, "mysql_errno"); if (dlerror() != 0) s.append("mysql_errno ");
        t__mysql_error= (tmysql_error) dlsym(dlopen_handle, "mysql_error"); if (dlerror() != 0) s.append("mysql_errorinit ");
        t__mysql_fetch_fields= (tmysql_fetch_fields) dlsym(dlopen_handle, "mysql_fetch_fields"); if (dlerror() != 0) s.append("mysql_fetch_fields ");
        t__mysql_fetch_lengths= (tmysql_fetch_lengths) dlsym(dlopen_handle, "mysql_fetch_lengths"); if (dlerror() != 0) s.append("mysql_fetch_lengths ");
        t__mysql_fetch_row= (tmysql_fetch_row) dlsym(dlopen_handle, "mysql_fetch_row"); if (dlerror() != 0) s.append("mysql_fetch_rows ");
        t__mysql_free_result= (tmysql_free_result) dlsym(dlopen_handle, "mysql_free_result"); if (dlerror() != 0) s.append("mysql_free_result ");
        t__mysql_get_client_info= (tmysql_get_client_info) dlsym(dlopen_handle, "mysql_get_client_info"); if (dlerror() != 0) s.append("mysql_get_client_info ");
        t__mysql_get_host_info= (tmysql_get_host_info) dlsym(dlopen_handle, "mysql_get_host_info"); if (dlerror() != 0) s.append("mysql_get_host_info ");
        t__mysql_info= (tmysql_info) dlsym(dlopen_handle, "mysql_info"); if (dlerror() != 0) s.append("mysql_info ");
        t__mysql_init= (tmysql_init) dlsym(dlopen_handle, "mysql_init"); if (dlerror() != 0) s.append("mysql_init ");
        {
          t__mysql_library_end= (tmysql_library_end) dlsym(dlopen_handle, "mysql_library_end");
          if (dlerror() != 0)
          {
            t__mysql_library_end= (tmysql_library_end) dlsym(dlopen_handle, "mysql_server_end");
            if (dlerror() != 0) s.append("mysql_library_end ");
          }
        }
        {
          t__mysql_library_init= (tmysql_library_init) dlsym(dlopen_handle, "mysql_library_init");
          if (dlerror() != 0)
          {
            t__mysql_library_init= (tmysql_library_init) dlsym(dlopen_handle, "mysql_server_init");
            if (dlerror() != 0) s.append("mysql_library_init ");
          }
        }
        t__mysql_more_results= (tmysql_more_results) dlsym(dlopen_handle, "mysql_more_results"); if (dlerror() != 0) s.append("mysql_more_results ");
        t__mysql_next_result= (tmysql_next_result) dlsym(dlopen_handle, "mysql_next_result"); if (dlerror() != 0) s.append("mysql_next_result ");
        t__mysql_num_fields= (tmysql_num_fields) dlsym(dlopen_handle, "mysql_num_fields"); if (dlerror() != 0) s.append("mysql_num_fields ");
        t__mysql_num_rows= (tmysql_num_rows) dlsym(dlopen_handle, "mysql_num_rows"); if (dlerror() != 0) s.append("mysql_num_rows ");
        t__mysql_options= (tmysql_options) dlsym(dlopen_handle, "mysql_options"); if (dlerror() != 0) s.append("mysql_options ");
        t__mysql_ping= (tmysql_ping) dlsym(dlopen_handle, "mysql_ping"); if (dlerror() != 0) s.append("mysql_ping ");
        t__mysql_query= (tmysql_query) dlsym(dlopen_handle, "mysql_query"); if (dlerror() != 0) s.append("mysql_query ");
        t__mysql_real_connect= (tmysql_real_connect) dlsym(dlopen_handle, "mysql_real_connect"); if (dlerror() != 0) s.append("mysql_real_connect ");
        t__mysql_real_query= (tmysql_real_query) dlsym(dlopen_handle, "mysql_real_query"); if (dlerror() != 0) s.append("mysql_real_query ");
        t__mysql_select_db= (tmysql_select_db) dlsym(dlopen_handle, "mysql_select_db"); if (dlerror() != 0) s.append("mysql_select_db ");
        t__mysql_sqlstate= (tmysql_sqlstate) dlsym(dlopen_handle, "mysql_sqlstate"); if (dlerror() != 0) s.append("mysql_sqlstate ");
        t__mysql_ssl_set= (tmysql_ssl_set) dlsym(dlopen_handle, "mysql_ssl_set"); if (dlerror() != 0) t__mysql_ssl_set= NULL;
        t__mysql_store_result= (tmysql_store_result) dlsym(dlopen_handle, "mysql_store_result"); if (dlerror() != 0) s.append("mysql_store_result ");
        t__mysql_thread_end= (tmysql_thread_end) dlsym(dlopen_handle, "mysql_thread_end"); if (dlerror() != 0) s.append("mysql_thread_end ");
        t__mysql_warning_count= (tmysql_warning_count) dlsym(dlopen_handle, "mysql_warning_count"); if (dlerror() != 0) s.append("mysql_warning_count ");
      }
      if (which_library == WHICH_LIBRARY_LIBCRYPTO)
      {
        t__AES_set_decrypt_key= (tAES_set_decrypt_key) dlsym(dlopen_handle, "AES_set_decrypt_key"); if (dlerror() != 0) s.append("AES_set_decrypt_key ");
        t__AES_decrypt= (tAES_decrypt) dlsym(dlopen_handle, "AES_decrypt"); if (dlerror() != 0) s.append("AES_decrypt ");
      }
#ifdef DBMS_TARANTOOL
      if (which_library == WHICH_LIBRARY_LIBTARANTOOL)
      {
        t__mp_decode_array= (tmp_decode_array) dlsym(dlopen_handle, "mp_decode_array"); if (dlerror() != 0) s.append("mp_decode_array ");
        t__mp_decode_bin= (tmp_decode_bin) dlsym(dlopen_handle, "mp_decode_bin"); if (dlerror() != 0) s.append("mp_decode_bin ");
        //t__mp_decode_binl= (tmp_decode_binl) dlsym(dlopen_handle, "mp_decode_binl)"); if (dlerror() != 0) s.append("mp_decode_binl ");
        t__mp_decode_bool= (tmp_decode_bool) dlsym(dlopen_handle, "mp_decode_bool"); if (dlerror() != 0) s.append("mp_decode_bool ");
        t__mp_decode_float= (tmp_decode_float) dlsym(dlopen_handle, "mp_decode_float"); if (dlerror() != 0) s.append("mp_decode_float ");
        t__mp_decode_double= (tmp_decode_double) dlsym(dlopen_handle, "mp_decode_double"); if (dlerror() != 0) s.append("mp_decode_double ");
        t__mp_decode_int= (tmp_decode_int) dlsym(dlopen_handle, "mp_decode_int"); if (dlerror() != 0) s.append("mp_decode_int ");
        t__mp_decode_map= (tmp_decode_map) dlsym(dlopen_handle, "mp_decode_map"); if (dlerror() != 0) s.append("mp_decode_map ");
        t__mp_decode_nil= (tmp_decode_nil) dlsym(dlopen_handle, "mp_decode_nil"); if (dlerror() != 0) s.append("mp_decode_nil ");
        t__mp_decode_uint= (tmp_decode_uint) dlsym(dlopen_handle, "mp_decode_uint"); if (dlerror() != 0) s.append("mp_decode_uint ");
        t__mp_decode_str= (tmp_decode_str) dlsym(dlopen_handle, "mp_decode_str"); if (dlerror() != 0) s.append("mp_decode_str ");
        t__mp_decode_strl= (tmp_decode_strl) dlsym(dlopen_handle, "mp_decode_strl"); if (dlerror() != 0) s.append("mp_decode_strl ");
        t__mp_next= (tmp_next) dlsym(dlopen_handle, "mp_next"); if (dlerror() != 0) s.append("mp_next ");
        t__mp_typeof= (tmp_typeof) dlsym(dlopen_handle, "mp_typeof"); if (dlerror() != 0) s.append("mp_typeof ");
        t__tnt_auth= (ttnt_auth) dlsym(dlopen_handle, "tnt_auth"); if (dlerror() != 0) s.append("tnt_auth ");
        t__tnt_call= (ttnt_call) dlsym(dlopen_handle, "tnt_call"); if (dlerror() != 0) s.append("tnt_call ");
        t__tnt_close= (ttnt_close) dlsym(dlopen_handle, "tnt_close"); if (dlerror() != 0) s.append("tnt_close ");
        t__tnt_connect= (ttnt_connect) dlsym(dlopen_handle, "tnt_connect"); if (dlerror() != 0) s.append("tnt_connect ");
        t__tnt_delete= (ttnt_delete) dlsym(dlopen_handle, "tnt_delete"); if (dlerror() != 0) s.append("tnt_delete");
        t__tnt_eval= (ttnt_eval) dlsym(dlopen_handle, "tnt_eval"); if (dlerror() != 0) s.append("tnt_eval ");
        t__tnt_flush= (ttnt_flush) dlsym(dlopen_handle, "tnt_flush"); if (dlerror() != 0) s.append("tnt_flush ");
        t__tnt_get_indexno= (ttnt_get_indexno) dlsym(dlopen_handle, "tnt_get_indexno"); if (dlerror() != 0) s.append("tnt_get_indexno ");
        t__tnt_get_spaceno= (ttnt_get_spaceno) dlsym(dlopen_handle, "tnt_get_spaceno"); if (dlerror() != 0) s.append("tnt_get_spaceno ");
        t__tnt_stream_free= (ttnt_stream_free) dlsym(dlopen_handle, "tnt_stream_free"); if (dlerror() != 0) s.append("tnt_stream_Free ");
        t__tnt_insert= (ttnt_insert) dlsym(dlopen_handle, "tnt_insert"); if (dlerror() != 0) s.append("tnt_insert ");
        t__tnt_net= (ttnt_net) dlsym(dlopen_handle, "tnt_net"); if (dlerror() != 0) s.append("tnt_net ");
        t__tnt_object= (ttnt_object) dlsym(dlopen_handle, "tnt_object"); if (dlerror() != 0) s.append("tnt_object ");
        t__tnt_object_add_array= (ttnt_object_add_array) dlsym(dlopen_handle, "tnt_object_add_array"); if (dlerror() != 0) s.append("tnt_object_add_array ");
        t__tnt_object_add_nil= (ttnt_object_add_nil) dlsym(dlopen_handle, "tnt_object_add_nil"); if (dlerror() != 0) s.append("tnt_object_add_nil ");
        t__tnt_object_add_int= (ttnt_object_add_int) dlsym(dlopen_handle, "tnt_object_add_int"); if (dlerror() != 0) s.append("tnt_object_add_int ");
        t__tnt_object_add_str= (ttnt_object_add_str) dlsym(dlopen_handle, "tnt_object_add_str"); if (dlerror() != 0) s.append("tnt_object_add_str ");
        t__tnt_object_add_bin= (ttnt_object_add_bin) dlsym(dlopen_handle, "tnt_object_add_bin"); if (dlerror() != 0) s.append("tnt_object_add_bin ");
        t__tnt_object_add_bool= (ttnt_object_add_bool) dlsym(dlopen_handle, "tnt_object_add_bool"); if (dlerror() != 0) s.append("tnt_object_add_bool ");
        t__tnt_object_add_float= (ttnt_object_add_float) dlsym(dlopen_handle, "tnt_object_add_float"); if (dlerror() != 0) s.append("tnt_object_add_float ");
        t__tnt_object_add_double= (ttnt_object_add_double) dlsym(dlopen_handle, "tnt_object_add_double"); if (dlerror() != 0) s.append("tnt_object_add_double ");
        t__tnt_object_container_close= (ttnt_object_container_close) dlsym(dlopen_handle, "tnt_object_container_close"); if (dlerror() != 0) s.append("tnt_object_container_close ");
        t__tnt_object_format= (ttnt_object_format) dlsym(dlopen_handle, "tnt_object_format"); if (dlerror() != 0) s.append("tnt_object_format ");
        t__tnt_object_reset= (ttnt_object_reset) dlsym(dlopen_handle, "tnt_object_reset"); if (dlerror() != 0) s.append("tnt_object_reset ");
        t__tnt_reload_schema= (ttnt_reload_schema) dlsym(dlopen_handle, "tnt_reload_schema"); if (dlerror() != 0) s.append("tnt_reload_schema ");
        t__tnt_replace= (ttnt_replace) dlsym(dlopen_handle, "tnt_replace"); if (dlerror() != 0) s.append("tnt_replace ");
        t__tnt_reply= (ttnt_reply) dlsym(dlopen_handle, "tnt_reply"); if (dlerror() != 0) s.append("tnt_reply ");
        t__tnt_reply_free= (ttnt_reply_free) dlsym(dlopen_handle, "tnt_reply_free"); if (dlerror() != 0) s.append("tnt_reply_free ");
        t__tnt_request_compile= (ttnt_request_compile) dlsym(dlopen_handle, "tnt_request_compile"); if (dlerror() != 0) s.append("tnt_request_compile ");
        t__tnt_request_eval= (ttnt_request_eval) dlsym(dlopen_handle, "tnt_request_eval"); if (dlerror() != 0) s.append("tnt_request_eval ");
        t__tnt_request_set_exprz= (ttnt_request_set_exprz) dlsym(dlopen_handle, "tnt_request_set_exprz"); if (dlerror() != 0) s.append("tnt_request_set_exprz ");
        t__tnt_request_set_tuple= (ttnt_request_set_tuple) dlsym(dlopen_handle, "tnt_request_set_tuple"); if (dlerror() != 0) s.append("tnt_request_set_tuple ");
        t__tnt_reply_init= (ttnt_reply_init) dlsym(dlopen_handle, "tnt_reply_init"); if (dlerror() != 0) s.append("tnt_reply_init ");
        t__tnt_select= (ttnt_select) dlsym(dlopen_handle, "tnt_select"); if (dlerror() != 0) s.append("tnt_select ");
        t__tnt_set= (ttnt_set) dlsym(dlopen_handle, "tnt_set"); if (dlerror() != 0) s.append("tnt_set ");
        t__tnt_update= (ttnt_update) dlsym(dlopen_handle, "tnt_update"); if (dlerror() != 0) s.append("tnt_update ");
      }
#endif
#else
      if ((which_library == WHICH_LIBRARY_LIBMYSQLCLIENT18) || (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT))
      {
        if ((t__mysql_affected_rows= (tmysql_affected_rows) lib.resolve("mysql_affected_rows")) == 0) s.append("mysql_affected_rows ");
        if ((t__mysql_close= (tmysql_close) lib.resolve("mysql_close")) == 0) s.append("mysql_close ");
        if ((t__mysql_data_seek= (tmysql_data_seek) lib.resolve("mysql_data_seek")) == 0) s.append("mysql_data_seek ");
        if ((t__mysql_errno= (tmysql_errno) lib.resolve("mysql_errno")) == 0) s.append("mysql_errno ");
        if ((t__mysql_error= (tmysql_error) lib.resolve("mysql_error")) == 0) s.append("mysql_error ");
        if ((t__mysql_fetch_fields= (tmysql_fetch_fields) lib.resolve("mysql_fetch_fields")) == 0) s.append("mysql_fetch_fields ");
        if ((t__mysql_fetch_lengths= (tmysql_fetch_lengths) lib.resolve("mysql_fetch_lengths")) == 0) s.append("mysql_fetch_lengths ");
        if ((t__mysql_fetch_row= (tmysql_fetch_row) lib.resolve("mysql_fetch_row")) == 0) s.append("mysql_fetch_row ");
        if ((t__mysql_free_result= (tmysql_free_result) lib.resolve("mysql_free_result")) == 0) s.append("mysql_free_result ");
        if ((t__mysql_get_client_info= (tmysql_get_client_info) lib.resolve("mysql_get_client_info")) == 0) s.append("mysql_get_client_info ");
        if ((t__mysql_get_host_info= (tmysql_get_host_info) lib.resolve("mysql_get_host_info")) == 0) s.append("mysql_get_host_info ");
        if ((t__mysql_info= (tmysql_info) lib.resolve("mysql_info")) == 0) s.append("mysql_info ");
        if ((t__mysql_init= (tmysql_init) lib.resolve("mysql_init")) == 0) s.append("mysql_init ");
        {
          t__mysql_library_end= (tmysql_library_end) lib.resolve("mysql_library_end");
          if (t__mysql_library_end == 0)
          {
            t__mysql_library_end= (tmysql_library_end) lib.resolve("mysql_server_end");
            if (t__mysql_library_end == 0) s.append("mysql_library_end ");
          }
        }
        {
          t__mysql_library_init= (tmysql_library_init) lib.resolve("mysql_library_init");
          if (t__mysql_library_init == 0)
          {
            t__mysql_library_init= (tmysql_library_init) lib.resolve("mysql_server_init");
            if (t__mysql_library_init == 0) s.append("mysql_library_init ");
          }
        }
        if ((t__mysql_more_results= (tmysql_more_results) lib.resolve("mysql_more_results")) == 0) s.append("mysql_more_results ");
        if ((t__mysql_next_result= (tmysql_next_result) lib.resolve("mysql_next_result")) == 0) s.append("mysql_next_result ");
        if ((t__mysql_num_fields= (tmysql_num_fields) lib.resolve("mysql_num_fields")) == 0) s.append("mysql_num_fields ");
        if ((t__mysql_num_rows= (tmysql_num_rows) lib.resolve("mysql_num_rows")) == 0) s.append("mysql_num_rows ");
        if ((t__mysql_options= (tmysql_options) lib.resolve("mysql_options")) == 0) s.append("mysql_options ");
        if ((t__mysql_ping= (tmysql_ping) lib.resolve("mysql_ping")) == 0) s.append("mysql_ping ");
        if ((t__mysql_query= (tmysql_query) lib.resolve("mysql_query")) == 0) s.append("mysql_query ");
        if ((t__mysql_real_connect= (tmysql_real_connect) lib.resolve("mysql_real_connect")) == 0) s.append("mysql_real_connect ");
        if ((t__mysql_real_query= (tmysql_real_query) lib.resolve("mysql_real_query")) == 0) s.append("mysql_real_query ");
        if ((t__mysql_select_db= (tmysql_select_db) lib.resolve("mysql_select_db")) == 0) s.append("mysql_select_db ");
        if ((t__mysql_sqlstate= (tmysql_sqlstate) lib.resolve("mysql_sqlstate")) == 0) s.append("mysql_sqlstate ");
        if ((t__mysql_ssl_set= (tmysql_ssl_set) lib.resolve("mysql_ssl_set")) == 0) t__mysql_ssl_set= NULL;
        if ((t__mysql_store_result= (tmysql_store_result) lib.resolve("mysql_store_result")) == 0) s.append("mysql_store_result ");
        if ((t__mysql_thread_end= (tmysql_thread_end) lib.resolve("mysql_thread_end")) == 0) s.append("mysql_thread_end ");
        if ((t__mysql_warning_count= (tmysql_warning_count) lib.resolve("mysql_warning_count")) == 0) s.append("mysql_warning_count ");
      }
      if (which_library == WHICH_LIBRARY_LIBCRYPTO)
      {
        if ((t__AES_set_decrypt_key= (tAES_set_decrypt_key) lib.resolve("AES_set_decrypt_key")) == 0) s.append("AES_set_decrypt_key ");
        if ((t__AES_decrypt= (tAES_decrypt) lib.resolve("AES_decrypt")) == 0) s.append("AES_decrypt ");
      }
#ifdef DBMS_TARANTOOL
      if (which_library == WHICH_LIBRARY_LIBTARANTOOL)
      {
         /* fill this in when you have Windows */
      }
#endif
#endif
      if (s > "")
      {
        {
          /* Unrecoverable error -- one or more names not found. Return the names. */
          *return_string= s;
          *is_library_loaded= -2;
          return;
          }
      }
    }
  }

  my_ulonglong ldbms_mysql_affected_rows(MYSQL *mysql)
  {
    return t__mysql_affected_rows(mysql);
  }

  void ldbms_mysql_close(MYSQL *mysql)
  {
    t__mysql_close(mysql);
  }

  void ldbms_mysql_data_seek(MYSQL_RES *result, my_ulonglong offset)
  {
    t__mysql_data_seek(result, offset);
  }

  unsigned int ldbms_mysql_errno(MYSQL *mysql)
  {
    return t__mysql_errno(mysql);
  }

  const char *ldbms_mysql_error(MYSQL *mysql)
  {
    return t__mysql_error(mysql);
  }

  MYSQL_FIELD *ldbms_mysql_fetch_fields(MYSQL_RES *result)
  {
    return t__mysql_fetch_fields(result);
  }

  unsigned long *ldbms_mysql_fetch_lengths(MYSQL_RES *result)
  {
    return t__mysql_fetch_lengths(result);
  }

  MYSQL_ROW ldbms_mysql_fetch_row(MYSQL_RES *result)
  {
    return t__mysql_fetch_row(result);
  }

  void ldbms_mysql_free_result(MYSQL_RES *result)
  {
    t__mysql_free_result(result);
  }

  const char *ldbms_mysql_get_client_info(void)
  {
    return t__mysql_get_client_info();
  }

  const char *ldbms_mysql_get_host_info(MYSQL *mysql)
  {
    return t__mysql_get_host_info(mysql);
  }

  const char *ldbms_mysql_info(MYSQL *mysql)
  {
    return t__mysql_info(mysql);
  }

  MYSQL *ldbms_mysql_init(MYSQL *mysql)
  {
    return t__mysql_init(mysql);
  }

  void ldbms_mysql_library_end()
  {
    t__mysql_library_end();
  }

  int ldbms_mysql_library_init(int argc, char **argv, char **groups)
  {
    return t__mysql_library_init(argc, argv, groups);
  }

  my_bool ldbms_mysql_more_results(MYSQL *mysql)
  {
    return t__mysql_more_results(mysql);
  }

  int ldbms_mysql_next_result(MYSQL *mysql)
  {
    return t__mysql_next_result(mysql);
  }

  unsigned int ldbms_mysql_num_fields(MYSQL_RES *result)
  {
    return t__mysql_num_fields(result);
  }

  my_ulonglong ldbms_mysql_num_rows(MYSQL_RES *result)
  {
    return t__mysql_num_rows(result);
  }

  int ldbms_mysql_options(MYSQL *mysql, enum ocelot_option option, const char *arg)
  {
    return t__mysql_options(mysql, option, arg);
  }

  int ldbms_mysql_ping(MYSQL *mysql)
  {
    return t__mysql_ping(mysql);
  }

  int ldbms_mysql_query(MYSQL *mysql, const char *stmt_str)
  {
    return t__mysql_query(mysql, stmt_str);
  }

  MYSQL *ldbms_mysql_real_connect(MYSQL *mysql, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, unsigned long client_flag)
  {
    return t__mysql_real_connect(mysql, host, user, passwd, db, port, unix_socket, client_flag);
  }

  /* This is not usually called from the main thread. Do not put a message box in this. */
  int ldbms_mysql_real_query(MYSQL *mysql, const char *stmt_str, unsigned long length)
  {
    return t__mysql_real_query(mysql, stmt_str, length);
  }

  int ldbms_mysql_select_db(MYSQL *mysql, const char *db)
  {
    return t__mysql_select_db(mysql, db);
  }

  const char *ldbms_mysql_sqlstate(MYSQL *mysql)
  {
    return t__mysql_sqlstate(mysql);
  }

  bool ldbms_mysql_ssl_set(MYSQL *mysql, const char *a, const char *b, const char *c, const char *d, const char *e)
  {
    if (t__mysql_ssl_set == NULL) return 0;
    return t__mysql_ssl_set(mysql, a, b, c, d, e);
  }

  MYSQL_RES *ldbms_mysql_store_result(MYSQL *mysql)
  {
    return t__mysql_store_result(mysql);
  }

  void ldbms_mysql_thread_end()
  {
    t__mysql_thread_end();
  }

  unsigned int ldbms_mysql_warning_count(MYSQL *mysql)
  {
    return t__mysql_warning_count(mysql);
  }

  int ldbms_AES_set_decrypt_key(unsigned char *a, int b, AES_KEY *c)
  {
    return t__AES_set_decrypt_key(a, b, c);
  }

  void ldbms_AES_decrypt(unsigned char *a, unsigned char *b, AES_KEY *c)
  {
    t__AES_decrypt(a, b, c);
  }

#ifdef DBMS_TARANTOOL
  u_int32_t ldbms_mp_decode_array(const char **data)
  {
    return t__mp_decode_array(data);
  }
  const char* ldbms_mp_decode_bin(const char **data, uint32_t *len)
  {
    return t__mp_decode_bin(data,len);
  }
  //uint32_t ldbms_mp_decode_binl(const char **data)
  //{
  //  return t__mp_decode_binl(data);
  //}
  bool ldbms_mp_decode_bool(const char **data)
  {
    return t__mp_decode_bool(data);
  }
  float ldbms_mp_decode_float(const char **data)
  {
    return t__mp_decode_float(data);
  }
  double ldbms_mp_decode_double(const char **data)
  {
    return t__mp_decode_double(data);
  }
  int64_t ldbms_mp_decode_int(const char **data)
  {
    return t__mp_decode_int(data);
  }
  u_int32_t ldbms_mp_decode_map(const char **data)
  {
    return t__mp_decode_map(data);
  }
  void ldbms_mp_decode_nil(const char **data)
  {
    return t__mp_decode_nil(data);
  }
  u_int64_t ldbms_mp_decode_uint(const char **data)
  {
    return t__mp_decode_uint(data);
  }
  const char* ldbms_mp_decode_str(const char **data, uint32_t *len)
  {
    return t__mp_decode_str(data,len);
  }
  uint32_t ldbms_mp_decode_strl(const char **data)
  {
    return t__mp_decode_strl(data);
  }
  void ldbms_mp_next(const char **data)
  {
    return t__mp_next(data);
  }
  enum mp_type ldbms_mp_typeof(const char c)
  {
    return t__mp_typeof(c);
  }

  ssize_t ldbms_tnt_auth(struct tnt_stream *a, const char *b, int c, const char *d, int e)
  {
    return t__tnt_auth(a,b,c,d,e);
  }
  ssize_t ldbms_tnt_call(struct tnt_stream *a, const char *b, size_t c, struct tnt_stream *d)
  {
    return t__tnt_call(a,b,c,d);
  }
  void ldbms_tnt_close(struct tnt_stream *a)
  {
    t__tnt_close(a);
  }
  int ldbms_tnt_connect(struct tnt_stream *a)
  {
    return t__tnt_connect(a);
  }
  size_t ldbms_tnt_delete(struct tnt_stream *a, uint32_t b, uint32_t c, tnt_stream *d)
  {
    return t__tnt_delete(a,b,c,d);
  }
  ssize_t ldbms_tnt_eval(struct tnt_stream *a, const char *b, size_t c, struct tnt_stream *d)
  {
    return t__tnt_eval(a,b,c,d);
  }
  ssize_t ldbms_tnt_flush(struct tnt_stream *a)
  {
    return t__tnt_flush(a);
  }
  int ldbms_tnt_get_indexno(struct tnt_stream *a, int b, const char *c, size_t d)
  {
    return t__tnt_get_indexno(a,b,c,d);
  }
  int ldbms_tnt_get_spaceno(struct tnt_stream *a, const char *b, size_t c)
  {
    return t__tnt_get_spaceno(a,b,c);
  }
  void ldbms_tnt_stream_free(struct tnt_stream *a)
  {
    t__tnt_stream_free(a);
  }
  ssize_t ldbms_tnt_insert(struct tnt_stream *a, uint32_t b, struct tnt_stream *c)
  {
    return t__tnt_insert(a,b,c);
  }
  tnt_stream* ldbms_tnt_net(tnt_stream *a)
  {
    return t__tnt_net(a);
  }
  tnt_stream* ldbms_tnt_object(tnt_stream *a)
  {
    return t__tnt_object(a);
  }
  ssize_t ldbms_tnt_object_add_array(tnt_stream *a, uint32_t b)
  {
    return t__tnt_object_add_array(a,b);
  }
  ssize_t ldbms_tnt_object_add_nil(tnt_stream *a)
  {
    return t__tnt_object_add_nil(a);
  }
  ssize_t ldbms_tnt_object_add_int(tnt_stream *a, int64_t b)
  {
    return t__tnt_object_add_int(a,b);
  }
  ssize_t ldbms_tnt_object_add_str(tnt_stream *a, const char *b, uint32_t c)
  {
    return t__tnt_object_add_str(a,b,c);
  }
  ssize_t ldbms_tnt_object_add_bin(tnt_stream *a, const char *b, uint32_t c)
  {
    return t__tnt_object_add_bin(a,b,c);
  }
  ssize_t ldbms_tnt_object_add_bool(tnt_stream *a, char b)
  {
    return t__tnt_object_add_bool(a,b);
  }
  ssize_t ldbms_tnt_object_add_float(tnt_stream *a, float b)
  {
    return t__tnt_object_add_float(a,b);
  }
  ssize_t ldbms_tnt_object_add_double(tnt_stream *a, double b)
  {
    return t__tnt_object_add_double(a,b);
  }
  ssize_t ldbms_tnt_object_container_close(tnt_stream *a)
  {
    return t__tnt_object_container_close(a);
  }
  ssize_t ldbms_tnt_object_format(struct tnt_stream *a, const char *b, int c)
  {
    return t__tnt_object_format(a,b,c);
  }
  int ldbms_tnt_object_reset(struct tnt_stream *a)
  {
    return t__tnt_object_reset(a);
  }
  int ldbms_tnt_reload_schema(struct tnt_stream *a)
  {
    return t__tnt_reload_schema(a);
  }
  ssize_t ldbms_tnt_replace(struct tnt_stream *a, uint32_t b, struct tnt_stream *c)
  {
    return t__tnt_replace(a,b,c);
  }
  int ldbms_ttnt_reply(struct tnt_reply *a, char *b, size_t c, size_t *d)
  {
    return t__tnt_reply(a,b,c,d);
  }
  void ldbms_tnt_reply_free(struct tnt_reply *a)
  {
    t__tnt_reply_free(a);
  }
  int64_t ldbms_tnt_request_compile(struct tnt_stream *a, struct tnt_request *b)
  {
    return t__tnt_request_compile(a,b);
  }
  struct tnt_request* ldbms_tnt_request_eval(struct tnt_request *a)
  {
    return t__tnt_request_eval(a);
  }
  int ldbms_tnt_request_set_exprz(struct tnt_request *a, const char *b)
  {
    return t__tnt_request_set_exprz(a,b);
  }
  int ldbms_tnt_request_set_tuple(struct tnt_request *a, struct tnt_stream *b)
  {
    return t__tnt_request_set_tuple(a,b);
  }
  struct tnt_reply* ldbms_tnt_reply_init(struct tnt_reply *a)
  {
    return t__tnt_reply_init(a);
  }
  ssize_t ldbms_tnt_select(struct tnt_stream *a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint8_t f, struct tnt_stream *g)
  {
    return t__tnt_select(a,b,c,d,e,f,g);
  }
  int ldbms_tnt_set(struct tnt_stream *a, int b, char *c)
  {
    return t__tnt_set(a,b,c);
  }
  ssize_t ldbms_tnt_update(struct tnt_stream *a, uint32_t b, uint32_t c, struct tnt_stream *d, struct tnt_stream *e)
  {
    return t__tnt_update(a,b,c,d,e);
  }
#endif
};

#endif // LDBMS_H

#define MAIN_WIDGET 0
#define HISTORY_WIDGET 1
#define GRID_WIDGET 2
#define STATEMENT_WIDGET 3
#define DEBUG_WIDGET 4
#define EXTRA_RULE_1 5

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
  Actually I think sql_more_results is always true nowadays.
*/

#ifndef RESULTGRID_H
#define RESULTGRID_H

class ResultGrid: public QWidget
{
  Q_OBJECT
public:
  QWidget *client;

  unsigned int result_column_count;
  long unsigned int result_row_count, grid_result_row_count;
  long unsigned int *lengths;
  unsigned int *grid_column_widths;                         /* dynamic-sized list of widths */
  unsigned int *result_max_column_widths; /* chars not bytes */ /* dynamic-sized list of actual maximum widths in detail columns */
  unsigned int *grid_column_heights;                         /* dynamic-sized list of heights */
  unsigned char *grid_column_dbms_sources;                   /* dynamic-sized list of sources */
  unsigned short int *result_field_types;          /* dynamic-sized list of types */
  unsigned int *result_field_charsetnrs;           /* dynamic-sized list of character set numbers */
  unsigned int *result_field_flags;                /* dynamic-sized list of flags */
  unsigned long result_row_number;                    /* row number in result set */
  MYSQL_ROW row;
  int is_paintable;
  unsigned int max_text_edit_frames_count;                       /* used for a strange error check during paint events */

  unsigned int result_grid_widget_max_height_in_lines;

  MYSQL_FIELD *mysql_fields;
  QScrollArea *grid_scroll_area;
  QScrollBar *grid_vertical_scroll_bar;                          /* This might take over from the automatic scroll bar. */
  int grid_vertical_scroll_bar_value;                            /* Todo: find out why this isn't defined as long unsigned */
  TextEditWidget **text_edit_widgets; /* Todo: consider using plaintext */ /* dynamic-sized list of pointers to QPlainTextEdit widgets */
  QHBoxLayout **text_edit_layouts;
  TextEditFrame **text_edit_frames;

  MYSQL_RES *grid_mysql_res;
  unsigned short ocelot_result_grid_vertical_copy;
  unsigned short ocelot_result_grid_column_names_copy;
  unsigned short ocelot_client_side_functions_copy;
  char *result_set_copy;                                     /* gets a copy of mysql_res contents, if necessary */
  char **result_set_copy_rows;                               /* dynamic-sized list of result_set_copy row offsets, if necessary */
  char *result_field_names;                                  /* gets a copy of mysql_fields[].name */
  char *result_original_field_names;                         /* gets a copy of mysql_fields[].org_name */
  char *result_original_table_names;                         /* gets a copy of mysql_fields[].org_table */
  char *result_original_database_names;                      /* gets a copy of mysql_fields[].db */

  unsigned int gridx_column_count;
  long unsigned int gridx_row_count;
  char *gridx_field_names;                                   /* gets a copy of result_field_names */
  unsigned int *gridx_max_column_widths; /* chars not bytes */ /* gets a copy of result_max_column_widths */
  unsigned int *gridx_result_indexes;                        /* points to result_ lists */
  unsigned char *gridx_flags;                                /* 0 = normal, 1 = row counter */
  unsigned short int *gridx_field_types;                     /* gets a copy of result_field_types */

//  unsigned int grid_actual_grid_height_in_rows;
  unsigned int grid_actual_row_height_in_lines;
  /* ocelot_grid_height_of_highest_column will be between 1 and ocelot_grid_max_column_height_in_lines, * pixels-per-line */
  unsigned int max_width_of_a_char, max_height_of_a_char;

  QHBoxLayout *hbox_layout;

  QHBoxLayout **grid_row_layouts;                               /* dynamic-sized list of pointers to rows' QHBoxLayout layouts */
  QWidget **grid_row_widgets;                                   /* dynamic-sized list of pointers to rows' QWidget widgets */
  QVBoxLayout *grid_main_layout;                                   /* replaces QGridLayout *grid_layout */
  /* QWidget *grid_main_widget;  */                                /* replaces QGridLayout *grid_layout -- but we say "client" */

  MainWindow *copy_of_parent;
  int copy_of_connections_dbms;
  unsigned short int copy_of_ocelot_result_grid_vertical;
  unsigned short int copy_of_ocelot_result_grid_column_names;
  unsigned short int copy_of_ocelot_batch;
  unsigned short int copy_of_ocelot_html;
  unsigned short int copy_of_ocelot_raw;
  unsigned short int copy_of_ocelot_xml;

  QTextEdit *batch_text_edit;

  int border_size;                                             /* used when calculating cell height + width */
  unsigned int ocelot_grid_max_desired_width_in_pixels;        /* used when calculating cell height + width */
  unsigned int ocelot_grid_max_column_height_in_lines;         /* used when calculating cell height + width */

  int ocelot_grid_cell_drag_line_size_as_int;
  int ocelot_grid_cell_border_size_as_int;
  QString ocelot_grid_text_color;
  QString ocelot_grid_background_color;
  QString ocelot_grid_cell_drag_line_color;
  unsigned int row_pool_size;
  unsigned int cell_pool_size;
  QString frame_color_setting;                                 /* based on drag line color */
  QFont text_edit_widget_font;
  ldbms *lmysql;
  unsigned int scroll_bar_width;

/* How many rows can fit on the screen? Take a guess for initialization. */
#define RESULT_GRID_WIDGET_INITIAL_HEIGHT 10

/* Use NULL_STRING when displaying a column value which is null. Length is sizeof(NULL_STRING) - 1. */
#define NULL_STRING "NULL"

/* During scan_rows we might set a flag e.g. if a column value is null */
#define FIELD_VALUE_FLAG_IS_ZERO 0
#define FIELD_VALUE_FLAG_IS_NULL 1
#define FIELD_VALUE_FLAG_IS_NUMBER 2
#define FIELD_VALUE_FLAG_IS_STRING 4
#define FIELD_VALUE_FLAG_IS_OTHER 8

ResultGrid(
//        MYSQL_RES *mysql_res,
        ldbms *passed_lmysql,
        MainWindow *parent,
        bool is_displayable): QWidget(parent)
{
  is_paintable= 0;

  /* todo: see if we can get rid of client and go direct to resultgrid */
  client= new QWidget(this);

  lmysql= passed_lmysql;

  copy_of_parent= parent;

  text_edit_widgets= 0;                                     /* all dynamic-sized items should be initially zero */
  text_edit_layouts= 0;
  text_edit_frames= 0;
  grid_column_widths= 0;                                    /* initializing for garbage_collect */
  result_max_column_widths= 0;
  grid_column_heights= 0;
  grid_column_dbms_sources= 0;
  result_field_types= 0;
  result_field_charsetnrs= 0;
  result_field_flags= 0;
  result_set_copy= 0;
  result_set_copy_rows= 0;
  result_field_names= 0;
  result_original_field_names= 0;
  result_original_table_names= 0;
  result_original_database_names= 0;
  gridx_field_names= 0;

  gridx_max_column_widths= 0;
  gridx_result_indexes= 0;
  gridx_flags= 0;
  gridx_field_types= 0;
  grid_vertical_scroll_bar= 0;
  grid_scroll_area= 0;
  /* grid_layout= 0; */
  hbox_layout= 0;
  grid_row_layouts= 0;
  grid_row_widgets= 0;
  grid_main_layout= 0;
  /* grid_main_widget= 0; */
  border_size= 1;                                          /* Todo: This actually has to depend on stylesheet */

  //result_row_count= 0;
  //result_column_count= 0;
  grid_result_row_count= 0;
  max_text_edit_frames_count= 0;

  if (is_displayable == false)
  {
    row_pool_size= cell_pool_size= 0;
    grid_main_layout= 0;
    batch_text_edit= NULL;
    return;
  }

  result_grid_widget_max_height_in_lines= RESULT_GRID_WIDGET_INITIAL_HEIGHT;

  /* We might say "new ResultGrid(0)" merely so we'd have ResultGrid in the middle spot in the layout-> */

  /* Create the cell pool. */
  /*
    Make the cells. Each cell is one QTextEdit (subclassed as TextEditWidget)
    within one QHBoxLayout within one TextEditFrame.
    Each TexteditFrame i.e. text_edit_frames[n] will be added to the scroll area.
  */
  /* Todo: say "(this)" a lot so automatic garbage collect will work. */
  grid_scroll_area= new QScrollArea(this);

  grid_scroll_area->setWidget(client);
  grid_scroll_area->setWidgetResizable(true);              /* Without this, the QTextEdit widget heights won't change */

  grid_vertical_scroll_bar= new QScrollBar(this);

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
  pools_resize(0, result_grid_widget_max_height_in_lines, 0, result_grid_widget_max_height_in_lines * 50);
  row_pool_size= result_grid_widget_max_height_in_lines;
  cell_pool_size= result_grid_widget_max_height_in_lines * 50;

  /* Typically scroll_bar_width = 13.
     If there was an option to change it, we'd have to use
     a different way to find its width.
     Todo: move this, it doesn't need recalculation
  */
  scroll_bar_width= text_edit_widgets[0]->style()->pixelMetric(QStyle::PM_ScrollBarExtent);

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

  text_edit_widget_font= this->font();
  set_frame_color_setting();

  batch_text_edit= new QTextEdit(this);
  batch_text_edit->hide();
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
  TextEditWidget **tmp_text_edit_widgets;
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
      tmp_text_edit_widgets= new TextEditWidget*[old_cell_pool_size];
      for (i_cp= 0; i_cp < old_cell_pool_size; ++i_cp) tmp_text_edit_widgets[i_cp]= text_edit_widgets[i_cp];
      delete [] text_edit_widgets;
      text_edit_widgets= new TextEditWidget*[new_cell_pool_size];
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
      text_edit_widgets= new TextEditWidget*[new_cell_pool_size];
      text_edit_layouts= new QHBoxLayout*[new_cell_pool_size];
      text_edit_frames= new TextEditFrame*[new_cell_pool_size];
    }
  }

  if (old_cell_pool_size < new_cell_pool_size)
  {
    for (i_cp= old_cell_pool_size; i_cp < new_cell_pool_size; ++i_cp)
    {
      text_edit_widgets[i_cp]= new TextEditWidget(this);
      text_edit_widgets[i_cp]->setCursor(Qt::ArrowCursor); /* See Note#1 above */
      /* todo: this should be a constant e.g. MARGIN_AMOUNT_IN_PIXELS */
      text_edit_widgets[i_cp]->document()->setDocumentMargin(0); /* default = 4 */
      text_edit_widgets[i_cp]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      text_edit_layouts[i_cp]= new QHBoxLayout();
      text_edit_layouts[i_cp]->addWidget(text_edit_widgets[i_cp]);
      text_edit_frames[i_cp]= new TextEditFrame(this, this, i_cp);
      text_edit_frames[i_cp]->setLayout(text_edit_layouts[i_cp]);
      text_edit_widgets[i_cp]->text_edit_frame_of_cell= text_edit_frames[i_cp];
    }
  }
}

/*
  Often an OCELOT_DATA_TYPE value is the same as a MYSQL_TYPE value, for example
  MYSQL_TYPE_LONG_BLOB=251 in mysql_com.h and #define OCELOT_DATA_TYPE_LONG_BLOG 251 here.
  But we have additional TEXT and BINARY types because we distinguish when charsetnr=63.
  DECIMAL and NEWDECIMAL are both DECIMAL. LONG is INT. INT24 is MEDIUMINT. LONGLONG is BIGINT.
  STRING is CHAR (only). VAR_STRING is VARCHAR (only). BLOB is BLOB (only).
*/
#define OCELOT_DATA_TYPE_DECIMAL     0
#define OCELOT_DATA_TYPE_TINY        1
#define OCELOT_DATA_TYPE_SHORT       2
#define OCELOT_DATA_TYPE_LONG        3
#define OCELOT_DATA_TYPE_FLOAT       4
#define OCELOT_DATA_TYPE_DOUBLE      5
#define OCELOT_DATA_TYPE_NULL        6
#define OCELOT_DATA_TYPE_TIMESTAMP   7
#define OCELOT_DATA_TYPE_LONGLONG    8
#define OCELOT_DATA_TYPE_INT24       9
#define OCELOT_DATA_TYPE_DATE        10
#define OCELOT_DATA_TYPE_TIME        11
#define OCELOT_DATA_TYPE_DATETIME    12
#define OCELOT_DATA_TYPE_YEAR        13
//#define OCELOT_DATA_TYPE_NEWDATE     14
//#define OCELOT_DATA_TYPE_VARCHAR     15
#define OCELOT_DATA_TYPE_BIT         16
#define OCELOT_DATA_TYPE_JSON        245       /* new in MySQL 5.7. todo: don't ignore it */
#define OCELOT_DATA_TYPE_NEWDECIMAL  246
#define OCELOT_DATA_TYPE_ENUM        247
#define OCELOT_DATA_TYPE_SET         248
//#define OCELOT_DATA_TYPE_TINY_BLOB   249
//#define OCELOT_DATA_TYPE_MEDIUM_BLOB 250
//#define OCELOT_DATA_TYPE_LONG_BLOB   251
#define OCELOT_DATA_TYPE_BLOB        252
#define OCELOT_DATA_TYPE_VAR_STRING  253       /* i.e. VARCHAR or VARBINARY */
#define OCELOT_DATA_TYPE_STRING      254       /* i.e. CHAR or BINARY */
#define OCELOT_DATA_TYPE_GEOMETRY    255
#define OCELOT_DATA_TYPE_BINARY      10001
#define OCELOT_DATA_TYPE_VARBINARY   10002
#define OCELOT_DATA_TYPE_TEXT        10003

/*
  Return true if what's at pointer has an image signature.
  We only do this for Tarantool, we set data type = OCELOT_DATA_TYPE_BLOB
  if the length is > 100 (arbitrary) and if the signature for .jpg or
  .png is at the start (not a reliable check but a false hit won't do a
  lot of harm). See wikipedia article = List of file signatures.
  We don't actually care except for extra_rule_1.
  We could do this check for MySQL/MariaDB too, but for them we simply
  look at the data type.
  Todo: check for other signatures, e.g. gif.
*/
bool is_image_format(int length, char* pointer)
{
  unsigned char *p= (unsigned char*) pointer;
  if (length <= 100) return false;
  if ((*p = 0x89) && (*(p+1) == 0x50) && (*(p+2) == 0x4e))
    return true;
  if ((*p = 0xff) && (*(p+1) == 0xd8) && (*(p+2) == 0xff))
    return true;
  return false;
}

/* We call fillup() whenever there is a new result set to put up on the result grid widget. */
QString fillup(MYSQL_RES *mysql_res,
            //struct tnt_reply *tarantool_tnt_reply,
            int connections_dbms,
            //MainWindow *parent,
            unsigned short ocelot_result_grid_vertical,
            unsigned short ocelot_result_grid_column_names,
            ldbms *passed_lmysql,
            int ocelot_client_side_functions,
            unsigned short int ocelot_batch,
            unsigned short int ocelot_html,
            unsigned short int ocelot_raw,
            unsigned short int ocelot_xml,
            unsigned int connection_number)
{
  /* TODO: put the copy_res_to_result stuff in a subsidiary private procedure. */
  lmysql= passed_lmysql;
  ocelot_result_grid_vertical_copy= ocelot_result_grid_vertical;
  ocelot_result_grid_column_names_copy= ocelot_result_grid_column_names;
  ocelot_client_side_functions_copy= ocelot_client_side_functions;

  grid_mysql_res= mysql_res;
#ifdef DBMS_TARANTOOL
  if (connections_dbms == DBMS_TARANTOOL)
  {
    result_column_count= copy_of_parent->tarantool_num_fields();
    result_row_count= copy_of_parent->tarantool_num_rows(connection_number);
  }
  else
#endif
  {
    result_column_count= lmysql->ldbms_mysql_num_fields(grid_mysql_res);
    result_row_count= lmysql->ldbms_mysql_num_rows(grid_mysql_res);                /* this will be the height of the grid */
    mysql_fields= lmysql->ldbms_mysql_fetch_fields(grid_mysql_res);
  }
  result_max_column_widths= new unsigned int[result_column_count];
  result_field_types= new unsigned short int[result_column_count];
  result_field_charsetnrs= new unsigned int[result_column_count];
  result_field_flags= new unsigned int[result_column_count];

#ifdef DBMS_TARANTOOL
  if (connections_dbms == DBMS_TARANTOOL)
  {
    QString result= copy_of_parent->tarantool_scan_rows(result_column_count, result_row_count,
              grid_mysql_res,
              &result_set_copy, &result_set_copy_rows,
              &result_max_column_widths);
    if (result != "OK")
    {
      garbage_collect();
      return result;
    }
  }
  else
#endif
    scan_rows(result_column_count, result_row_count,
              grid_mysql_res,
              &result_set_copy, &result_set_copy_rows,
              &result_max_column_widths);
#ifdef DBMS_TARANTOOL
  if (connections_dbms == DBMS_TARANTOOL)
  {
    copy_of_parent->tarantool_scan_field_names("name", result_column_count, &result_field_names);
    /* Next three scan_field_names calls are only needed if user will edit the result set */
    copy_of_parent->tarantool_scan_field_names("org_name", result_column_count, &result_original_field_names);
    copy_of_parent->tarantool_scan_field_names("org_table", result_column_count, &result_original_table_names);
    copy_of_parent->tarantool_scan_field_names("db", result_column_count, &result_original_database_names);
  }
  else
#endif
  {
    scan_field_names("name", result_column_count, &result_field_names);
    /* Next three scan_field_names calls are only needed if user will edit the result set */
    scan_field_names("org_name", result_column_count, &result_original_field_names);
    scan_field_names("org_table", result_column_count, &result_original_table_names);
    scan_field_names("db", result_column_count, &result_original_database_names);
  }
#ifdef DBMS_TARANTOOL
  /* Scan entire result set to determine if NUM_FLAG should go on. */
  if (connections_dbms == DBMS_TARANTOOL)
  {
    bool is_image_seen= false;
    for (unsigned int i= 0; i < result_column_count; ++i)
    {
      result_field_types[i]= OCELOT_DATA_TYPE_VAR_STRING;
      result_field_charsetnrs[i]= 83; /* utf8, utf8_bin */
      result_field_flags[i]= 0; /* todo: decide if it's numeric */
    }
    long unsigned int tmp_xrow;
    char *pointer= result_set_copy_rows[0];
    unsigned int v_length;
    for (tmp_xrow= 0; tmp_xrow < result_row_count; ++tmp_xrow)
    {
      for (unsigned int i= 0; i < result_column_count; ++i)
      {
        memcpy(&v_length, pointer, sizeof(unsigned int));
        char tmp_flag= *(pointer + sizeof(unsigned int));
        if ((tmp_flag == FIELD_VALUE_FLAG_IS_NUMBER) || (tmp_flag == FIELD_VALUE_FLAG_IS_STRING))
        {
          if (result_field_flags[i] != FIELD_VALUE_FLAG_IS_STRING)
          {
            result_field_flags[i]= tmp_flag;
          }
          else
          {
            if (is_image_format(v_length, pointer + sizeof(unsigned int) + sizeof(char)))
              is_image_seen= true;
          }
        }
        pointer+= v_length + sizeof(unsigned int) + sizeof(char);
      }
    }
    for (unsigned int i= 0; i < result_column_count; ++i)
    {
      if (result_field_flags[i] == FIELD_VALUE_FLAG_IS_NUMBER)
        result_field_flags[i]= NUM_FLAG;
      else
      {
        result_field_flags[i]= 0;
        if (is_image_seen == true)
        {
          result_field_types[i]= OCELOT_DATA_TYPE_BLOB;
          result_field_charsetnrs[i]= 63;
        }
      }
    }
  }
  else
#endif

  {
    for (unsigned int i= 0; i < result_column_count; ++i)
    {
      result_field_types[i]= mysql_fields[i].type;
      result_field_charsetnrs[i]= mysql_fields[i].charsetnr;
      result_field_flags[i]= mysql_fields[i].flags;
    }
  }
  /*
    At this point, we have:
      result_column_count, result_row_count
      result_set_copy, result_set_copy_rows,
      result_field_names,
      result_original_field_names, result_original_table_names, result_original_database_names,
      result_max_column_widths
      mysql_fields (which we should not use, but we do)
    From now on there should be no need to call mysql_ functions again for this result set.
  */

  /* todo: gotta use MYSQL_REMOTE_CONNECTION rather than 3 someday. */
  if (connection_number == 3) return "OK";
  copy_result_to_gridx(connections_dbms);
  /* Todo: no more grid_result_row_count, and copy_result_to_gridx already
     said what gridx_row_count is. */
  if (ocelot_result_grid_vertical == 0) grid_result_row_count= gridx_row_count + 1;
  else grid_result_row_count= result_row_count * result_column_count;
  if (ocelot_result_grid_vertical == 0)
  {
    gridx_row_count= grid_result_row_count + 1;
  }
  if (ocelot_result_grid_vertical != 0)
  {
    gridx_row_count= result_row_count * result_column_count;
    gridx_column_count= 1;
    if (ocelot_result_grid_column_names != 0) ++gridx_column_count;
  }

  grid_column_widths= new unsigned int[gridx_column_count];
  grid_column_heights= new unsigned int[gridx_column_count];
  grid_column_dbms_sources= new unsigned char[gridx_column_count];

  dbms_set_grid_column_sources();                 /* Todo: this could return an error? */

  /***** BEYOND THIS POINT, IT'S LAYOUT MATTERS *****/
  copy_of_connections_dbms= connections_dbms;
  copy_of_ocelot_result_grid_vertical= ocelot_result_grid_vertical;
  copy_of_ocelot_result_grid_column_names= ocelot_result_grid_column_names;
  copy_of_ocelot_batch= ocelot_batch;
  copy_of_ocelot_html= ocelot_html;
  copy_of_ocelot_raw= ocelot_raw;
  copy_of_ocelot_xml= ocelot_xml;
  display();
  return "OK";
}

/*
  Todo: find out why I get here twice.
        I think it's because fillup() calls display(), but then
        resize_or_font_change() calls display() again.
        This is invisible because resize_or_font_change() calls
        remove_layouts() first, but it's a silly waste of time.
        However, maybe it only happens for the first time I select.
  Todo: grid_main_layout->setSizeConstraint() is only necessary if
        we've recently turned off ocelot_batch + ocelot_html; it
        could be shifted so it's only reset when we reconnect
        and|or change those variables.
  Todo: Bug:
        (start program with ocelot_batch == ocelot_html == 0)
        select * from information_schema.tables limit 10;
        SET ocelot_html = 1;
        select * from information_schema.tables limit 10;
        SET ocelot_html = 0;
        select * from information_schema.tables limit 10;
        the display is obscured, that is why in display() I say
        batch_text_edit->hide();
        but why do I need it, if I've removed it from layout?
*/
void display()
{
  if ((copy_of_ocelot_batch != 0)
   || (copy_of_ocelot_html != 0)
   || (copy_of_ocelot_xml != 0))
  {
    display_batch();
    return;
  }

  grid_main_layout->setSizeConstraint(QLayout::SetFixedSize);  /* This ensures the grid columns have no spaces between them */
  batch_text_edit->hide();

  long unsigned int xrow;
  unsigned int xcol;
  MainWindow *parent= copy_of_parent;
  int connections_dbms= copy_of_connections_dbms;
  unsigned short int ocelot_result_grid_vertical= copy_of_ocelot_result_grid_vertical;
  unsigned short int ocelot_result_grid_column_names= copy_of_ocelot_result_grid_column_names;

  /* Some child widgets e.g. text_edit_frames[n] must not be visible because they'd receive paint events too soon. */
  hide();
  is_paintable= 0;

  ocelot_grid_text_color= parent->ocelot_grid_text_color;
  ocelot_grid_background_color= parent->ocelot_grid_background_color;
  /* ocelot_grid_cell_drag_line_size_as_int= parent->ocelot_grid_cell_drag_line_size.toInt(); */
  /* ocelot_grid_cell_drag_line_color= parent->ocelot_grid_cell_drag_line_color; */

  //  grid_scroll_area= new QScrollArea(this);                                    /* Todo: see why parent can't be client */

  //  grid_scroll_area->verticalScrollBar()->setMaximum(gridx_row_count);
  //  grid_scroll_area->verticalScrollBar()->setSingleStep(1);
  //  grid_scroll_area->verticalScrollBar()->setPageStep(gridx_row_count / 10);    /* Todo; check if this could become 0 */
  grid_vertical_scroll_bar_value= -1;

  {
    unsigned int minimum_number_of_cells;
    minimum_number_of_cells= result_grid_widget_max_height_in_lines * gridx_column_count;
    pools_resize(row_pool_size, result_grid_widget_max_height_in_lines, cell_pool_size, minimum_number_of_cells);
    if (row_pool_size < result_grid_widget_max_height_in_lines) row_pool_size= result_grid_widget_max_height_in_lines;
    if (cell_pool_size < minimum_number_of_cells) cell_pool_size= minimum_number_of_cells;
  }

  /*
    Calculate desired width and height based on parent width and height.
     Desired max width in chars = width when created - width of scroll bar.
     Max height in lines = height when created - (height of scroll bar + height of header) / 4. minimum = 1.
     Todo: We're dividing height-when-created by 3 because we assume statement+history widgets are there, maybe they're not.
     Todo: max height could be greater if row count < 4, or maybe it should be user-settable.
     Todo: grid_vertical_scroll_bar->width() failed so I just guessed that I should subtract 3 char widths.
  */

  /* Todo: since grid_column_size_calc() recalculates max_height_of_a_char, don't bother with this. */

  QFont *pointer_to_font;
  pointer_to_font= &text_edit_widget_font;
  QFontMetrics mm= QFontMetrics(*pointer_to_font);

  /* Todo: figure out why this says parent->width() rather than this->width() -- maybe "this" has no width yet? */
  ocelot_grid_max_desired_width_in_pixels= (parent->width() - (mm.width("W") * 3));

  {
    /*
      Try to ensure we can fit at least header (if there is a header) plus one row.
      So there's a maximum number of lines per row.
      We assume (border height + horizontal scroll bar height) < 11 (todo: calculate them).
      We assume result grid height = height of main window / 3 (todo: calculate it).
    */
    int result_grid_height= (parent->height() / 3) - 11;
    int line_height= mm.lineSpacing();
    if ((ocelot_result_grid_column_names == 1) && (ocelot_result_grid_vertical == 0))
        result_grid_height-= line_height;
    ocelot_grid_max_column_height_in_lines= result_grid_height / line_height;
    if (ocelot_grid_max_column_height_in_lines < 1) ocelot_grid_max_column_height_in_lines= 1;
  }

  ocelot_grid_cell_drag_line_size_as_int= copy_of_parent->ocelot_grid_cell_drag_line_size.toInt();
//  ocelot_grid_cell_drag_line_color= copy_of_parent->ocelot_grid_cell_drag_line_color;
  ocelot_grid_cell_border_size_as_int= copy_of_parent->ocelot_grid_cell_border_size.toInt();

  /*
    Making changes for all in the cell pool.
    Todo: This should only be done for new cells, or if something has changed e.g. font, drag line size.
          That's actually a bug, because the drag line color doesn't change immediately.
    todo: why the whole pool rather than just result-row_count + 1?
  */
  QFont *pointer_to_font_2;
  pointer_to_font_2= &text_edit_widget_font;
  QFontMetrics fm= QFontMetrics(*pointer_to_font_2);
  /* Todo: see whether this loop could be shifted somewhere so it's not repeated for every fillup */
  for (xrow= 0; (xrow < gridx_row_count) && (xrow < result_grid_widget_max_height_in_lines); ++xrow)
  {
    for (unsigned int column_number= 0; column_number < gridx_column_count; ++column_number)
    {
      int ki= xrow * gridx_column_count + column_number;
      text_edit_widgets[ki]->setMinimumWidth(fm.width("W") * 3);
      /* This line was replaced in December 2015 */
      //text_edit_widgets[ki]->setMinimumHeight(fm.height() * 2);
      text_edit_widgets[ki]->setMinimumHeight(fm.lineSpacing());
      text_edit_layouts[ki]->setContentsMargins(QMargins(0, 0, ocelot_grid_cell_drag_line_size_as_int, ocelot_grid_cell_drag_line_size_as_int));
      /*
        Change the color of the frame. Be specific that it's TextEditFrame, because you don't want the
        children e.g. the QTextEdit to inherit the color. TextEditFrame is a custom widget and therefore
        setStyleSheet is troublesome for most settings, but background-color should be okay, see
        http://stackoverflow.com/questions/7276330/qt-stylesheet-for-custom-widget.
      */

      //text_edit_frames[ki]->setStyleSheet(frame_color_setting);

      /* Todo: remove this line and test whether anything goes wrong. */
      text_edit_frames[ki]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);   /* This doesn't seem to do anything */

      /* Todo: I'm not sure exactly where the following three lines should go. Consider moving them. */
      /* border_size and minimum_width and minimum_height are used by mouseMoveEvent */
      text_edit_frames[ki]->border_size= 10 + border_size;    /* Todo: should just be border_size!! */
      text_edit_frames[ki]->minimum_width= fm.width("W") * 3 + border_size;
      text_edit_frames[ki]->minimum_height= fm.height() * 2 + border_size; /* todo: this is probably too much */
    }
  }

  /*
    For each cell:
      Set cell type = detail or header, depending on is_vertical + whether it's first row
      If header: indicate where header text is.
    Todo: this is just assuming top line is header, which is wrong now.
  */
  char *field_names_pointer;
  for (xrow= 0; (xrow < grid_result_row_count) && (xrow < result_grid_widget_max_height_in_lines); ++xrow)
  {
    field_names_pointer= gridx_field_names; /* unnecessary reset if ocelot_result_grid_vertical = 0 */
    for (unsigned int column_number= 0; column_number < gridx_column_count; ++column_number)
    {
      int ki= xrow * gridx_column_count + column_number;
      bool is_header= false;
      if (ocelot_result_grid_vertical != 0)
      {
        if (ocelot_result_grid_column_names != 0)
        {
          if (column_number == 0) is_header= true;
        }
      }
      if ((ocelot_result_grid_vertical == 0) && (xrow == 0)) is_header= true;
      if (is_header == true)
      {
        memcpy(&(text_edit_frames[ki]->content_length), field_names_pointer, sizeof(unsigned int));
        field_names_pointer+= sizeof(unsigned int);
        text_edit_frames[ki]->content_pointer= field_names_pointer;
        field_names_pointer+= text_edit_frames[ki]->content_length;
        text_edit_frames[ki]->is_retrieved_flag= false;
        text_edit_frames[ki]->ancestor_grid_column_number= column_number;
        text_edit_frames[ki]->ancestor_grid_result_row_number= -1;          /* probably unnecessary */
        if (text_edit_frames[ki]->cell_type != TEXTEDITFRAME_CELL_TYPE_HEADER)
        {
          text_edit_frames[ki]->cell_type= TEXTEDITFRAME_CELL_TYPE_HEADER;
          text_edit_frames[ki]->is_style_sheet_set_flag= false;
        }
        text_edit_frames[ki]->is_image_flag= false;
      }
      else
      {
        if (is_extra_rule_1(column_number) == true)
        {
          if (text_edit_frames[ki]->cell_type != TEXTEDITFRAME_CELL_TYPE_DETAIL_EXTRA_RULE_1)
          {
            text_edit_frames[ki]->cell_type= TEXTEDITFRAME_CELL_TYPE_DETAIL_EXTRA_RULE_1;
            text_edit_frames[ki]->is_style_sheet_set_flag= false;
          }
        }
        else
        {
          if (text_edit_frames[ki]->cell_type != TEXTEDITFRAME_CELL_TYPE_DETAIL)
          {
            text_edit_frames[ki]->cell_type= TEXTEDITFRAME_CELL_TYPE_DETAIL;
            text_edit_frames[ki]->is_style_sheet_set_flag= false;
          }
        }
      }
    }
  }
  //if (ocelot_result_grid_vertical != 0)
  //  grid_column_size_calc(ocelot_grid_cell_border_size_as_int,
  //                      ocelot_grid_cell_drag_line_size_as_int,
  //                      0); /* get grid_column_widths[] and grid_column_heights[] */

  if (ocelot_result_grid_vertical != 0)
  {
    /* TODO: Make sure considerations for horizontal are all considered for vertical. */
    /* We'll have to figure out the alignment etc. each time we get ready to display */
    unsigned int grid_row_number, text_edit_frame_index;
    for (grid_row_number= 0, text_edit_frame_index= 0;
         grid_row_number < result_grid_widget_max_height_in_lines;
         ++grid_row_number)
    {
      if (grid_row_number >= gridx_row_count) break;
      for (unsigned int mi= 0; mi < gridx_column_count; ++mi)
      {
        /* todo: test whether we really need to show always */
        text_edit_frames[text_edit_frame_index]->show();
        grid_row_layouts[grid_row_number]->addWidget(text_edit_frames[text_edit_frame_index], 0, Qt::AlignTop | Qt::AlignLeft);
        ++text_edit_frame_index;
      }
    }
    /* How many text_edit_frame widgets are we actually using? This assumes number-of-columns-per-row is fixed. */
    max_text_edit_frames_count= text_edit_frame_index;
  }
  else max_text_edit_frames_count= (grid_result_row_count) * gridx_column_count;

  /*
    grid detail rows
    While we're passing through, we also get max column lengths (in characters).
    Todo: Take into account: whether there were any nulls.
  */
  fill_detail_widgets(0, connections_dbms);                                        /* details */
  grid_vertical_scroll_bar_value= 0;

  /*
    We'll use the automatic scroll bar for small result sets,
    we'll use our own scroll bar for large ones.
    setValue() will cause eventfilter to call vertical_scroll_bar_event().
    But grid_vertical_scroll_bar_value == 0 so nothing will happen except slider movement.
    This section was changed on 2016-04-01.
  */
  if (grid_result_row_count <= result_grid_widget_max_height_in_lines)
  {
    grid_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    grid_vertical_scroll_bar->setVisible(false);
    grid_scroll_area->verticalScrollBar()->setValue(0);
  }
  else
  {
    grid_scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    grid_vertical_scroll_bar->setVisible(true);
    grid_vertical_scroll_bar->setValue(0);
  }

  is_paintable= 1;
  if (ocelot_result_grid_vertical == 0)
  grid_column_size_calc(ocelot_grid_cell_border_size_as_int,
                        ocelot_grid_cell_drag_line_size_as_int,
                        ocelot_result_grid_column_names_copy,
                        connections_dbms); /* get grid_column_widths[] and grid_column_heights[] */

  /*
    grid_actual_grid_height_in_rows = # of rows that are actually showable at a time,
    = lesser of (grid_max_grid_height_in_lines/grid_max_row_height_in_lines, # of rows in result set + 1)
  */
  //grid_actual_grid_height_in_rows= gridx_row_count;
  //if (grid_actual_grid_height_in_rows > gridx_row_count + 1) grid_actual_grid_height_in_rows= gridx_row_count + 1;

  /* Put the QTextEdit widgets in a layout. Remember grid row 0 is for the header.
    Horizontal (default):
      Each row is [column_count] cells within one QHBoxLayout (grid_row_layout) within one widget.
    Vertical (if --vertical or \G):
      Each row is 2 cells within one QHBoxLayout (grid_row_layout) within one widget.
    grid_row_layout->setSizeConstraint(QLayout::SetMaximumSize) prevents gaps from forming during shrink.
    There's a "border", actually the visible part of TextEditFrame, on the cell's right.
    Drag it left to shrink the cell, drag it right to expand the cell.
    We do not resize cells on the left or right of the to-be-dragged cell, so expanding causes total row width to expand,
    possibly going beyond the original desired maximum width, possibly causing a horizontal scroll bar to appear.
    grid_row_layout->setSpacing(0) means the only thing separating cells is the "border".
  */
  if (ocelot_result_grid_vertical == 0)
  {
    for (long unsigned int xrow= 0; (xrow < grid_result_row_count) && (xrow < result_grid_widget_max_height_in_lines); ++xrow)
    {
      for (xcol= 0; xcol < gridx_column_count; ++xcol)
      {
        TextEditWidget *cell_text_edit_widget= text_edit_widgets[xrow * gridx_column_count + xcol];
        if ((xrow > 0) && (dbms_get_field_flag(xcol, connections_dbms) & NUM_FLAG)) text_align(cell_text_edit_widget, Qt::AlignRight);
        else text_align(cell_text_edit_widget, Qt::AlignLeft);
        if (text_edit_frames[xrow * gridx_column_count + xcol]->cell_type != TEXTEDITFRAME_CELL_TYPE_HEADER)
        {
          if (is_image(xcol) == true)
          {
            text_edit_frames[xrow * gridx_column_count + xcol]->is_image_flag= true;
          }
          else text_edit_frames[xrow * gridx_column_count + xcol]->is_image_flag= false;
        }

        /* Height border size = 1 due to setStyleSheet earlier; right border size is passed */
        if (xrow == 0)
        {
          int header_height= max_height_of_a_char
                           + ocelot_grid_cell_border_size_as_int * 2
                           + ocelot_grid_cell_drag_line_size_as_int;
          frame_resize(xrow * gridx_column_count + xcol, xcol, grid_column_widths[xcol], header_height);
        }
        else
        {
          frame_resize(xrow * gridx_column_count + xcol, xcol, grid_column_widths[xcol], grid_column_heights[xcol]);
        }
        /* todo: test whether we really need to show always */
        text_edit_frames[xrow * gridx_column_count + xcol]->show();
        grid_row_layouts[xrow]->addWidget(text_edit_frames[xrow * gridx_column_count + xcol], 0, Qt::AlignTop | Qt::AlignLeft);
      }
    }
  }
//  grid_main_layout->setSizeConstraint(QLayout::SetFixedSize);  /* This ensures the grid columns have no spaces between them */
  for (long unsigned int xrow= 0; (xrow < grid_result_row_count) && (xrow < result_grid_widget_max_height_in_lines); ++xrow)
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
  frame_resize() == Setting text_edit_frames[n] size.
  Called from: display() if vertical == 0 (initial),
               set_alignment_and_height() if vertical != 0 (always),
               TextEditFrame::mouseMoveEvent() if drag line moved.
  Re scroll bar: we make most of the initial size calculations with the
                 assumption that scroll bar will be off, as it usually
                 will be, and since otherwise grid_column_size_calc()
                 becomes tremendously complicated. Here, if you know
                 the contents will fit, turn off the scroll bar.
  I think the width is of the frame; we want the width of the widget.
*/
void frame_resize(int ki, int grid_col, int width, int height)
{
  unsigned int text_edit_width= width -
          (ocelot_grid_cell_drag_line_size_as_int
           + ocelot_grid_cell_border_size_as_int * 2);
  unsigned int text_edit_height= height -
          (ocelot_grid_cell_drag_line_size_as_int
           + ocelot_grid_cell_border_size_as_int * 2);
  unsigned int number_of_characters_per_line= text_edit_width / max_width_of_a_char;
  unsigned int number_of_lines= text_edit_height / max_height_of_a_char;
  unsigned int number_of_characters_in_cell= number_of_characters_per_line
                                        * number_of_lines;
  if (number_of_characters_in_cell < gridx_max_column_widths[grid_col])
    text_edit_widgets[ki]->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  else
    text_edit_widgets[ki]->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  text_edit_frames[ki]->setFixedSize(width, height);
  /* Todo: test if following 2 lines are redundant since setFixedSize does the job. */
  //text_edit_frames[ki]->setMaximumHeight(height);
  //text_edit_frames[ki]->setMinimumHeight(height);
}

/*
  For --batch, avoid normal grid display. Result won't go to history.
  For --html, avoid normal grid display.
              Result will go to history.
              For --html --raw, result is dumped so user sees the markup.
              --html overrides --batch, i.e. we look at html first
              images are done as data URIs if png|jpg|gif
  We say setReadOnly() so edit won't generate update statement.
  We pay attention to --column-names and (sometimes) --raw, as well.
  Todo: BUG: If you reconnect with file|connect, and use a different
        setting for batch or html or raw, display is garbage.
  Todo: SET ocelot_batch = x; should be possible.
        It would not change the current result grid.
  Todo: going directly to a file should be possible too
  Todo: check what happens if row data contains "\n"
  Todo: pay attention to --vertical
  Todo: Do what you do for other displays, e.g. check for null
  Todo: look for --raw, currently we're just assuming it's true
        + we don't need to multiply size by 2 if --raw is off (?)
        Escape ' " \ nul tab newline =  \' \" \\ \0 \t \n
  Todo: we can also support --xml, I don't know whether
        it overrides --batch or whether it complements it somehow.
        also csv and msgpack and json
        also "--like-mysql-client"
        or SET ocelot_grid_format = {'fancy'|'html'|'xml'|'csv'|etc.}
  Todo: Setting should be possible with the syntax that's used for
        INTO OUTFILE
  Todo: I was getting
        QTextOdfWriter: unsupported paragraph alignment;  QFlags(0x20)
        if ^A (select all), ^C (Copy) on a large output.
        Specifying "...Qt::AlignLeft" solved it.
        But it might be nice to write a bug report for the Qt folks.
  Todo: it would take less memory and possibly display quicker if
        I only output when necessary, i.e. when user scrolls
  Todo: check: does it do any good to hide() first?
  Todo: callback to a local Lua function prior to display, for each row
  Todo: Allow switch from one output to another. This does require
        having fillup() completely separate from display().
        setSizeConstraint and setVerticalScrollBarPolicy should be
        reset if we change to non-batch display.
  Todo: allow row update. and, if there's a change in one mode, show
        the changed row when modes are switched
  Todo: xml statement="" and field name="" contents lack escaping.
*/
/*
  Eventually ...
                                                    csv     --html    --batch  --xml
    ocelot_grid_table_start                                 </TABLE>           $statement
    ocelot_grid_header_row_start                            <TR>
    ocelot_grid_header_row_end                      \n      </TR>      \n
    ocelot_grid_header_numeric_column_start                 <TH>
    ocelot_grid_header_numeric_column_end           ,       </TH>      tab
    ocelot_grid_header_char_column_start            "       <TH>
    ocelot_grid_header_char_column_end              ",      </TH>      tab
    ocelot_grid_detail_row_start                                               <row>
    ocelot_grid_detail_row_end                                                 </row>
    ocelot_grid_detail_numeric_column_start                 <TD>
    ocelot_grid_detail_numeric_column_end           ,       </TD>
    ocelot_grid_detail_char_column_start            "       <TD>               <field name="$field">
    ocelot_grid_detail_char_column_end              ",      </TD>              </field>
    ocelot_grid_table_end                                   </TABLE>
    ... up to 9 characters + \0
*/
/*
  Todo: the html style sheet
  For html, instead of batch_text_edit->setStyleSheet(), we want to
  put css specifications in the html header. Example:
  <HTML><head><style type=text/css>table, th, td {border-color: black; border-style: solid; color: red; background-color: blue}</style></head><BODY><TABLE BORDER=5>.
  Although copy_of_parent->ocelot_grid_style_string) has all the
  necessary information, it's the wrong format. Use:
  color = copy_of_parent->ocelot_grid_text_color;
  background-color = copy_of_parent->ocelot_grid_background_color;
  copy_of_parent->ocelot_grid_border_color; (no)
  copy_of_parent->ocelot_grid_header_background_color;

  font-family = copy_of_parent->ocelot_grid_font_family;
  font-size = copy_of_parent->ocelot_grid_font_size;
  font-style = copy_of_parent->ocelot_grid_font_style;
  font-weight = copy_of_parent->ocelot_grid_font_weight;

  border-color = copy_of_parent->ocelot_grid_cell_border_color;
  copy_of_parent->ocelot_grid_cell_drag_line_color; (no)
  copy_of_parent->ocelot_grid_border_size; (no)
  copy_of_parent->ocelot_grid_cell_border_size;
  copy_of_parent->ocelot_grid_cell_drag_line_size;
*/
void display_batch()
{
  char ocelot_grid_table_start[640];
  char ocelot_grid_header_row_start[32];
  char ocelot_grid_header_row_end[32];
  char ocelot_grid_header_numeric_column_start[32];
  char ocelot_grid_header_numeric_column_end[32];
  char ocelot_grid_header_char_column_start[32];
  char ocelot_grid_header_char_column_end[32];
  char ocelot_grid_detail_row_start[32];
  char ocelot_grid_detail_row_end[32];
  char ocelot_grid_detail_numeric_column_start[32];
  char ocelot_grid_detail_numeric_column_end[32];
  char ocelot_grid_detail_char_column_start[320];
  char ocelot_grid_detail_char_column_end[32];
  char ocelot_grid_table_end[320];

  /* Todo: this should be done permanently, on persistent variables. */
  if (copy_of_ocelot_html != 0)
  {
    char html_border_color[32];
    char html_color[32];
    char html_background_color[32];
    char html_header_background_color[32];
    char html_font_family[32];
    char html_font_size[32];
    char html_font_style[32];
    char html_font_weight[32];
    int html_border_size;
    strcpy(html_border_color, copy_of_parent->ocelot_grid_cell_border_color.toUtf8());
    strcpy(html_color, copy_of_parent->ocelot_grid_text_color.toUtf8());
    strcpy(html_background_color, copy_of_parent->ocelot_grid_background_color.toUtf8());
    strcpy(html_header_background_color, copy_of_parent->ocelot_grid_header_background_color.toUtf8());
    strcpy(html_font_family, copy_of_parent->ocelot_grid_font_family.toUtf8());
    strcpy(html_font_size, copy_of_parent->ocelot_grid_font_size.toUtf8());
    strcpy(html_font_style, copy_of_parent->ocelot_grid_font_style.toUtf8());
    strcpy(html_font_weight, copy_of_parent->ocelot_grid_font_weight.toUtf8());
    html_border_size= copy_of_parent->ocelot_grid_cell_border_size.toInt();
    sprintf(ocelot_grid_table_start, "<head><style type=text/css>"
            " th {"
            "border-color: %s; "
            "border-style: solid; "
            "padding-left: 1px; "
            "padding-right: 1px; "
            "color: %s; "
            "background-color: %s; "
            "font-family: %s; "
            "font-size: %spx; "
            "font-style: %s; "
            "font-weight: %s}"
            " td {"
            "border-color: %s; "
            "border-style: solid; "
            "padding-left: 1px; "
            "padding-right: 1px; "
            "color: %s; "
            "background-color: %s; "
            "font-family: %s; "
            "font-size: %spx; "
            "font-style: %s; "
            "font-weight: %s}"
            "</style></head><BODY><TABLE BORDER=%d>",
            html_border_color,
            html_color,
            html_header_background_color,
            html_font_family,
            html_font_size,
            html_font_style,
            html_font_weight,
            html_border_color,
            html_color,
            html_background_color,
            html_font_family,
            html_font_size,
            html_font_style,
            html_font_weight,
            html_border_size);
    strcpy(ocelot_grid_header_row_start, "<TR>");
    strcpy(ocelot_grid_header_row_end, "</TR>");
    strcpy(ocelot_grid_header_numeric_column_start, "<TH>");
    strcpy(ocelot_grid_header_numeric_column_end, "</TH>");
    strcpy(ocelot_grid_header_char_column_start, "<TH>");
    strcpy(ocelot_grid_header_char_column_end, "</TH>");
    strcpy(ocelot_grid_detail_row_start, "<TR>");
    strcpy(ocelot_grid_detail_row_end, "</TR>");
    strcpy(ocelot_grid_detail_numeric_column_start, "<TD align=\"right\">");
    strcpy(ocelot_grid_detail_numeric_column_end,"</TD>");
    strcpy(ocelot_grid_detail_char_column_start, "<TD>");
    strcpy(ocelot_grid_detail_char_column_end , "</TD>");
    strcpy(ocelot_grid_table_end, "</TABLE></BODY></HTML>");
  }
  else if (copy_of_ocelot_batch != 0)
  {
    strcpy(ocelot_grid_table_start, "");
    strcpy(ocelot_grid_header_row_start, "");
    strcpy(ocelot_grid_header_row_end, "\n");
    strcpy(ocelot_grid_header_numeric_column_start, "");
    strcpy(ocelot_grid_header_numeric_column_end, "\t");
    strcpy(ocelot_grid_header_char_column_start, "");
    strcpy(ocelot_grid_header_char_column_end, "\t");
    strcpy(ocelot_grid_detail_row_start, "");
    strcpy(ocelot_grid_detail_row_end, "\n");
    strcpy(ocelot_grid_detail_numeric_column_start, "");
    strcpy(ocelot_grid_detail_numeric_column_end, "\t");
    strcpy(ocelot_grid_detail_char_column_start, "");
    strcpy(ocelot_grid_detail_char_column_end , "\t");
    strcpy(ocelot_grid_table_end, "");
  }
  else /* copy_of_ocelot_xml != 0 */
  {
    strcpy(ocelot_grid_table_start, "<?xml version=\"1.0\"?>"
                                    "<resultset statement=\"");
    strcat(ocelot_grid_table_start, copy_of_parent->query_utf16_copy.toUtf8());
    strcat(ocelot_grid_table_start, "\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">");
    strcpy(ocelot_grid_header_row_start, "<row>");
    strcpy(ocelot_grid_header_row_end, "</row>");
    strcpy(ocelot_grid_header_numeric_column_start, "<field name=\"");
    strcpy(ocelot_grid_header_numeric_column_end, "</field>");
    strcpy(ocelot_grid_header_char_column_start, "<field name=\"");
    strcpy(ocelot_grid_header_char_column_end, "</field>");
    strcpy(ocelot_grid_detail_row_start, "<row>");
    strcpy(ocelot_grid_detail_row_end, "</row>");
    strcpy(ocelot_grid_detail_numeric_column_start, "<field name=\"");
    strcpy(ocelot_grid_detail_numeric_column_end, "</field>");
    strcpy(ocelot_grid_detail_char_column_start, "<field name=\"");
    strcpy(ocelot_grid_detail_char_column_end , "</field>");
    strcpy(ocelot_grid_table_end, "</resultset>");
  }
  hide();
  batch_text_edit->clear();
  grid_main_layout->setSizeConstraint(QLayout::SetMinimumSize);
  grid_vertical_scroll_bar->setVisible(false);
  grid_main_layout->addWidget(batch_text_edit);

  if (copy_of_ocelot_html == 0)
    batch_text_edit->setStyleSheet(copy_of_parent->ocelot_grid_style_string);
  /* Todo: next four lines could be done during the initial setup */
  batch_text_edit->setReadOnly(true);
  batch_text_edit->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  batch_text_edit->setWordWrapMode(QTextOption::NoWrap);
  batch_text_edit->setFrameStyle(QFrame::NoFrame);
  long unsigned int tmp_xrow;
  char *pointer= result_set_copy_rows[0];
  unsigned int v_length, f_length;
  char *result_field_names_pointer;

  /*
    Todo: Adjust calculation for numeric columns, for escapes, for hex.
          It ought to be possible to do a single non-looping calculation.
 */
  unsigned int tmp_size= sizeof(char);

  if ((ocelot_result_grid_column_names_copy == 1)
   && (copy_of_ocelot_xml == 0))
  {
    tmp_size+= strlen(ocelot_grid_header_row_start);
    result_field_names_pointer= &result_field_names[0];
    for (unsigned int i= 0; i < result_column_count; ++i)
    {
      tmp_size+= strlen(ocelot_grid_header_char_column_start);
      memcpy(&v_length, result_field_names_pointer, sizeof(unsigned int));
      tmp_size+= v_length;
      result_field_names_pointer+= v_length + sizeof(unsigned int);
      tmp_size+= strlen(ocelot_grid_header_char_column_end);
    }
    tmp_size+= strlen(ocelot_grid_header_row_end);
  }

  tmp_size+= strlen(ocelot_grid_table_start);
  for (tmp_xrow= 0; tmp_xrow < result_row_count; ++tmp_xrow)
  {
    result_field_names_pointer= &result_field_names[0];
    tmp_size+= strlen(ocelot_grid_detail_row_start);
    for (unsigned int i= 0; i < result_column_count; ++i)
    {  
      if ((result_field_flags[i] & NUM_FLAG) != 0)
        tmp_size+= strlen(ocelot_grid_detail_numeric_column_start);
      else
        tmp_size+= strlen(ocelot_grid_detail_char_column_start);
      if (copy_of_ocelot_xml != 0)
      {
        memcpy(&f_length, result_field_names_pointer, sizeof(unsigned int));
        tmp_size+= f_length + 2;
        result_field_names_pointer+= f_length + sizeof(unsigned int);
      }
      memcpy(&v_length, pointer, sizeof(unsigned int));
      pointer+= sizeof(unsigned int) + sizeof(char);
      /* Todo: we only need v_length*1 unless it's img or hex or escaped */
      tmp_size+= v_length * 2;
      tmp_size+= strlen(ocelot_grid_detail_char_column_end);
      pointer+= v_length;
    }
    tmp_size+= strlen(ocelot_grid_detail_row_end);
  }
  tmp_size+= strlen(ocelot_grid_table_end);
  char *tmp;

  tmp= new char[tmp_size];
  char *tmp_pointer= &tmp[0];

  strcpy(tmp_pointer, ocelot_grid_table_start);
  tmp_pointer+= strlen(ocelot_grid_table_start);

  if ((ocelot_result_grid_column_names_copy == 1)
   && (copy_of_ocelot_xml == 0))
  {
    char *result_field_names_pointer;
    result_field_names_pointer= &result_field_names[0];
    strcpy(tmp_pointer, ocelot_grid_header_row_start);
    tmp_pointer+= strlen(ocelot_grid_header_row_start);
    for (unsigned int i= 0; i < result_column_count; ++i)
    {
      strcpy(tmp_pointer, ocelot_grid_header_char_column_start);
      tmp_pointer+= strlen(ocelot_grid_header_char_column_start);
      memcpy(&v_length, result_field_names_pointer, sizeof(unsigned int));
      result_field_names_pointer+= sizeof(unsigned int);
      memcpy(tmp_pointer, result_field_names_pointer, v_length);
      tmp_pointer+= v_length;
      strcpy(tmp_pointer, ocelot_grid_header_char_column_end);
      tmp_pointer+= strlen(ocelot_grid_header_char_column_end);
      result_field_names_pointer+= v_length;
    }
    strcpy(tmp_pointer, ocelot_grid_header_row_end);
    tmp_pointer+= strlen(ocelot_grid_header_row_end);
  }

  pointer= result_set_copy_rows[0];
  for (tmp_xrow= 0; tmp_xrow < result_row_count; ++tmp_xrow)
  {
    result_field_names_pointer= &result_field_names[0];
    strcpy(tmp_pointer, ocelot_grid_detail_row_start);
    tmp_pointer+= strlen(ocelot_grid_detail_row_start);
    for (unsigned int i= 0; i < result_column_count; ++i)
    {
      if ((result_field_flags[i] & NUM_FLAG) != 0)
      {
        strcpy(tmp_pointer, ocelot_grid_detail_numeric_column_start);
        tmp_pointer+= strlen(ocelot_grid_detail_numeric_column_start);
      }
      else
      {
        strcpy(tmp_pointer, ocelot_grid_detail_char_column_start);
        tmp_pointer+= strlen(ocelot_grid_detail_char_column_start);
      }
      if (copy_of_ocelot_xml != 0)
      {
        memcpy(&f_length, result_field_names_pointer, sizeof(unsigned int));
        result_field_names_pointer+= sizeof(unsigned int);
        memcpy(tmp_pointer, result_field_names_pointer, f_length);
        tmp_pointer+= f_length;
        result_field_names_pointer+= f_length;
        strcpy(tmp_pointer, "\">");
        tmp_pointer+= 2;
      }
      memcpy(&v_length, pointer, sizeof(unsigned int));
      pointer+= sizeof(unsigned int) + sizeof(char);
      bool is_image_written= false;
      if ((copy_of_ocelot_html != 0) && (is_image(i) == true))
      {
        char img_type[4]= "";
        if (v_length > 4)
        {
          if (strncmp(pointer,"\x89PNG",4) == 0) strcpy(img_type, "png");
          else if (strncmp(pointer,"\xFF\xD8",2) == 0) strcpy(img_type, "jpg");
          else if (strncmp(pointer,"GIF",3) == 0) strcpy(img_type, "gif");
          /* to: try BMP? check with loadFromData()? */
        }
        if (strcmp(img_type,"") != 0)
        {
          char *base64_tmp;
          base64_tmp= new char[(v_length * 4) / 3 + 16];
          QByteArray data= QByteArray::fromRawData(pointer, v_length);
          strcpy(base64_tmp, data.toBase64());
          memcpy(tmp_pointer, "<img src=\"data:image/", 21);
          tmp_pointer+= 21;
          memcpy(tmp_pointer, img_type, 3);
          tmp_pointer+= 3;
          memcpy(tmp_pointer, ";base64,", 8);
          tmp_pointer+= 8;
          memcpy(tmp_pointer, base64_tmp, strlen(base64_tmp));
          tmp_pointer+= strlen(base64_tmp);
          memcpy(tmp_pointer, "\"/>", 3);
          tmp_pointer+= 3;
          delete base64_tmp;
          is_image_written= true;
        }
      }
      if (is_image_written == false)
      {
        memcpy(tmp_pointer, pointer, v_length);
        tmp_pointer+= v_length;
      }
      strcpy(tmp_pointer, ocelot_grid_detail_char_column_end);
      tmp_pointer+= strlen(ocelot_grid_detail_char_column_end);
      pointer+= v_length;
    }
    strcpy(tmp_pointer, ocelot_grid_detail_row_end);
    tmp_pointer+= strlen(ocelot_grid_detail_row_end);
  }
  strcpy(tmp_pointer, ocelot_grid_table_end);
  tmp_pointer+= strlen(ocelot_grid_table_end);
  *tmp_pointer= '\0';

  if ((copy_of_ocelot_html != 0) && (copy_of_ocelot_raw == 0))
  {
    batch_text_edit->setHtml(tmp);
  }
  else
  {
    batch_text_edit->insertPlainText(tmp);
  }
  batch_text_edit->moveCursor(QTextCursor::Start);
  batch_text_edit->ensureCursorVisible();
  batch_text_edit->show();
  show();
  client->show();
  delete [] tmp;
  return;
}

#ifdef DBMS_TARANTOOL
/* Given column name e.g. f_15_1 return number e.g. 15 */
int column_number(char *column_name, int *off)
{
  char tmp[10]= "";
  int tmp_offset= 0;
  int i= 0;
  for (;; ++i)
  {
    if (*(column_name + i) == '_') break;
    if (*(column_name + i) == '\0') return 0;
  }
  ++i;
  for (;; ++i)
  {
    if (*(column_name + i) == '_') break;
    if (*(column_name + i) == '\0') break;
    tmp[tmp_offset++]= *(column_name + i);
  }
  tmp[tmp_offset]= '\0';
  *off= i;
  return atoi(tmp);
}
#endif

#ifdef DBMS_TARANTOOL
/*
  Make a create statement for a CREATE TABLE ... SERVER table.
  We've done fillup() so we've done tarantool_scan_field_names()
  so we have result_field_names and result_column_count.
  If Lua statement was 'box.space.X:select()" we may have X's field names,
  in read_format_result.
  If the main connection is also Tarantool, then we make a primary key
  on the first one or two columns -- todo: these might not be the right
  columns, and I'm hoping Tarantool won't always have this requirement.
  Todo: max_column_widths might be unreliable if multibyte character.
*/
int creates(QString create_table_statement, int connections_dbms_0, QString read_format_result)
{
  QString tmp;
  QString first_columns= "";
  char *result_field_names_pointer;
  char column_name[512 + 1];
  unsigned int v_length;

  tmp= create_table_statement;
  tmp.append("(");
  result_field_names_pointer= &result_field_names[0];
  for (unsigned int i= 0; i < result_column_count; ++i)
  {
    memcpy(&v_length, result_field_names_pointer, sizeof(unsigned int));
    result_field_names_pointer+= sizeof(unsigned int);
    memcpy(column_name, result_field_names_pointer, v_length);
    column_name[v_length]= '\0';
    if (i != 0) tmp.append(",");
    char tmp_column_name_number[10];
    int off= 0;
    int c= column_number(column_name, &off);
    sprintf(tmp_column_name_number, "[%d]", c);
    int word_start, word_end;
    QString word= "";
    word_start= read_format_result.indexOf(tmp_column_name_number, 0);
    if (word_start != -1)
    {
      word_start+= strlen(tmp_column_name_number);
      word_end= read_format_result.indexOf("[", word_start);
      if (word_end == -1) word_end= read_format_result.size();
      word= read_format_result.mid(word_start, (word_end - word_start));
    }
    if (word != "")
    {
      char tmp_column_name[512];
      strcpy(tmp_column_name, word.toUtf8());
      strcat(tmp_column_name, column_name + off);
      strcpy(column_name, tmp_column_name);
    }
    tmp.append(column_name);
    if (i == 0) first_columns.append(column_name);
    if (i == 1) {first_columns.append(","); first_columns.append(column_name); }
    if ((result_field_flags[i] & NUM_FLAG) != 0)
    {
      tmp.append(" BIGINT ");
    }
    else
    {
      tmp.append(" VARCHAR(");
      tmp.append(QString::number(result_max_column_widths[i]));
      tmp.append(") ");
    }
    result_field_names_pointer+= v_length;
  }
  if (connections_dbms_0 == DBMS_TARANTOOL)
  {
    tmp.append(", PRIMARY KEY (");
    tmp.append(first_columns);
    tmp.append(")");
  }
  tmp.append(")");
  int result= copy_of_parent->real_query(tmp, 0); /* MYSQL_MAIN_CONNECTION */
  if (result != 0) return result;
  return result;
}
#endif

#ifdef DBMS_TARANTOOL
/* Make some insert statements for a remote subquery temporary table */
/*
  KLUDGE ALERT: A column in the first row might be non-numeric
  (field name) even though the field is numeric.
  This is supposed to be fixed soon. Meanwhile we put '' around it.
  What we're saying is "if it doesn't start with a digit then put
  quotes around it", which is close to absurd.
*/
int inserts(QString temporary_table_name)
{
  long unsigned int tmp_xrow;
  char *pointer= result_set_copy_rows[0];
  unsigned int v_length;
  QString tmp;
  QString s;
  char ctmp[1024];

  pointer= result_set_copy_rows[0];
  for (tmp_xrow= 0; tmp_xrow < result_row_count; ++tmp_xrow)
  {
    tmp= "INSERT INTO ";
    tmp.append(temporary_table_name);
    tmp.append(" VALUES (");
    for (unsigned int i= 0; i < result_column_count; ++i)
    {
      if (i > 0) tmp.append(",");
      memcpy(&v_length, pointer, sizeof(unsigned int));
      char tmp_flag= *(pointer + sizeof(unsigned int));
      pointer+= sizeof(unsigned int) + sizeof(char);
      if (tmp_flag == FIELD_VALUE_FLAG_IS_NULL)
      {
        tmp.append("NULL");
      }
      else if ((v_length > 0) && (*pointer >= '0') && (*pointer <= '9'))
      {
        memcpy(ctmp, pointer, v_length);
        ctmp[v_length]= '\0';
        tmp.append(ctmp);
      }
      else
      {
        tmp.append("'");
        memcpy(ctmp, pointer, v_length);
        ctmp[v_length]= '\0';
        tmp.append(ctmp);
        tmp.append("'");
      }
      pointer+= v_length;
    }
    tmp.append(");");
    int result= copy_of_parent->real_query(tmp, 0); /* MYSQL_MAIN_CONNECTION */
    if (result != 0) return result;
  }
  return 0;
}
#endif

/*
  Copy the result_ lists to gridx_lists.
  Originally the idea behind this was that we'd optionally add columns
  that weren't in the result set, like "row count" and (for vertical)
  "header". That's becoming obsolete now, because we change the result
  set copy if ocelot_client_side_functions_copy <> 0. So, perhaps,
  someday we will get rid of this step.
  The values are for a single detail row or single header row.
   gridx_field_names
   gridx_original_field_names
   gridx_original_table_names
   gridx_original_database_names
   gridx_max_column_widths
   gridx_flags                         header | refer to result _ lists
   gridx_field_types
   gridx_result_indexes                use as index for result_ lists
   gridx_column_count, gridx_row_count
*/
void copy_result_to_gridx(int connections_dbms)
{
  (void) connections_dbms; /* suppress "unused parameter" warning */
  unsigned int i, j;
  unsigned int v_lengths;
  char *result_field_names_pointer;
  char *gridx_field_names_pointer;

  if (gridx_field_names != 0) { delete [] gridx_field_names; gridx_field_names= 0; }
  if (gridx_max_column_widths != 0) { delete [] gridx_max_column_widths; gridx_max_column_widths= 0; }
  if (gridx_result_indexes != 0) { delete [] gridx_result_indexes; gridx_result_indexes= 0; }
  if (gridx_flags != 0) { delete [] gridx_flags; gridx_flags= 0; }
  if (gridx_field_types != 0) { delete [] gridx_field_types; gridx_field_types= 0; }

  gridx_column_count= result_column_count;
  gridx_row_count= result_row_count;
  /* result_set_copy, result_set_copy_rows are left alone */

  /*
    First loop: find how much to allocate. Allocate. Second loop: fill in with pointers within allocated area.
  */

  result_field_names_pointer= &result_field_names[0];
  unsigned int total_size= 0;
  for (i= 0; i < result_column_count; ++i)
  {
    memcpy(&v_lengths, result_field_names_pointer, sizeof(unsigned int));
    total_size+= v_lengths + sizeof(unsigned int);
    result_field_names_pointer+= v_lengths + sizeof(unsigned int);
  }

  gridx_field_names= new char[total_size];                                  /* allocate */

  result_field_names_pointer= &result_field_names[0];
  gridx_field_names_pointer= &gridx_field_names[0];
  for (i= 0; i < result_column_count; ++i)
  {
    memcpy(&v_lengths, result_field_names_pointer, sizeof(unsigned int));
    memcpy(gridx_field_names_pointer, &v_lengths, sizeof(unsigned int));
    result_field_names_pointer+= sizeof(unsigned int);
    gridx_field_names_pointer+= sizeof(unsigned int);
    memcpy(gridx_field_names_pointer, result_field_names_pointer, v_lengths);
    result_field_names_pointer+= v_lengths;
    gridx_field_names_pointer+= v_lengths;
  }

  gridx_max_column_widths= new unsigned int[gridx_column_count];
  gridx_result_indexes= new unsigned int[gridx_column_count];
  gridx_flags= new unsigned char[gridx_column_count];
  gridx_field_types= new short unsigned int[gridx_column_count];
  j= 0;
  for (i= 0; i < result_column_count; ++i)
  {
    gridx_max_column_widths[j]= result_max_column_widths[i];
    gridx_result_indexes[j]= i;
    gridx_flags[j]= 0;
    gridx_field_types[j]= result_field_types[i];
    /* todo: following depends on MySQL quirks, should be done earlier */
    if ((result_field_charsetnrs[i] == 63) && (gridx_field_types[j] == OCELOT_DATA_TYPE_VAR_STRING)) gridx_field_types[j]= OCELOT_DATA_TYPE_VARBINARY;
    if ((result_field_charsetnrs[i] == 63) && (gridx_field_types[j] == OCELOT_DATA_TYPE_STRING)) gridx_field_types[j]= OCELOT_DATA_TYPE_BINARY;
    if ((result_field_charsetnrs[i] != 63) && (gridx_field_types[j] == OCELOT_DATA_TYPE_BLOB)) gridx_field_types[j]= OCELOT_DATA_TYPE_TEXT;
    ++j;
  }

  //result_field_names,
  //result_original_field_names, result_original_table_names, result_original_database_names,
}

/*
  Return true if extra_rule_1 is applicable for this column
*/
bool is_extra_rule_1(int col)
{
  QString condition= copy_of_parent->ocelot_extra_rule_1_condition;
  if (condition == "data_type LIKE '%BLOB'")
  {
    if (gridx_field_types[col] == OCELOT_DATA_TYPE_BLOB)
    {
      return true;
    }
  }
  if (condition == "data_type LIKE '%BINARY'")
  {
    if ((gridx_field_types[col] == OCELOT_DATA_TYPE_BINARY) || (gridx_field_types[col] == OCELOT_DATA_TYPE_VARBINARY))
    {
      return true;
    }
  }
  return false;
}

/*
  Return true if this column should be displayed as an image,
  that is, the image flag should be turned on.
*/
bool is_image(int col)
{
  QString display_as;
  display_as= copy_of_parent->ocelot_extra_rule_1_display_as;
  if (display_as == "image")
  {
    if (is_extra_rule_1(col) == true) return true;
  }
  return false;
}


/*
  Move a limited part of a result set to history.
  We want to show what it looks like, but dumping the whole thing might waste time and space,
  so it's throttled by an ocelot_history variable that can be set to a small values:
  ocelot_history_max_row_count, default "0".
  This is called after preparing a result set in fillup(), and
  depends on fillup() results including max_column widths.
  Example output:
  +------+------+------------+
  | s1   | s2   | The Rain I |
  +------+------+------------+
  |    1 | A    |          0 |
  | NULL | NULL |          0 |
  +------+------+------------+
  The output is the same as what mysql client would display --
    left margin = 1, right margin = 1, left justify if number,
    content length = maximum actual length -- except that mysql
    client has minimum content length = 4 if column is nullable,
    and we don't bother with that, it looks like a flaw.
  Defined limits on column width and on number of columns are arbitrary.
  If changing this, remember to test ^P and ^N which depend on markup.
  Todo: this could be adapted for an alternate way to display the result grid.
  Warning: making the copy bigger would slow down the way the Previous and Next keys work.
  Remaining challenges with copy_to_history:
  * Names and max widths should depend on result_row stuff not gridx_max stuff
  * We should try to keep track of statements so we don't spend too much time going backwards.
  * The "8192" for vertical output is arbitrary. Max should be calculated.
*/
#define HISTORY_COLUMN_MARGIN 1
#define HISTORY_MAX_COLUMN_WIDTH 65535
#define HISTORY_MAX_COLUMN_COUNT 65535
#define HISTORY_MAX_VERTICAL_COLUMN_WIDTH 8192
QString copy_to_history(long int ocelot_history_max_row_count, unsigned short int is_vertical)
{
  if (ocelot_history_max_row_count == 0) return "";
  unsigned int col;
  long unsigned int r;
  unsigned int length;
  unsigned int history_result_column_count;
  unsigned int *history_max_column_widths;
  unsigned long history_result_row_count;
  char *history_line;
  char *divider_line;
  char *pointer_to_history_line;
  unsigned int history_line_width;
  QString s;

  s= "";


  history_max_column_widths= 0;
  history_line= 0;

  /* TODO: See whether gridx_column_count was necessary */
  //if (gridx_column_count > HISTORY_MAX_COLUMN_COUNT) history_result_column_count= HISTORY_MAX_COLUMN_COUNT;
  //else history_result_column_count= gridx_column_count;
  if (result_column_count > HISTORY_MAX_COLUMN_COUNT) history_result_column_count= HISTORY_MAX_COLUMN_COUNT;
  else history_result_column_count= result_column_count;

  history_max_column_widths= new unsigned int[history_result_column_count];
  history_line_width= 2;
  unsigned int column_width;

  {
    char *pointer_to_field_names= result_field_names;
    unsigned int column_length;
    for (col= 0; col < history_result_column_count; ++col)
    {
      if (ocelot_result_grid_column_names_copy == 1)
      {
        memcpy(&column_length, pointer_to_field_names, sizeof(unsigned int));
        pointer_to_field_names+= sizeof(unsigned int);
        pointer_to_field_names+= column_length;
        column_width= column_length;
      }
      else column_width= 0;
      if (column_width < gridx_max_column_widths[col]) column_width= gridx_max_column_widths[col];
      if (column_width > HISTORY_MAX_COLUMN_WIDTH) column_width= HISTORY_MAX_COLUMN_WIDTH;
      history_max_column_widths[col]= column_width;
      history_line_width+= column_width + 1 + HISTORY_COLUMN_MARGIN * 2;
    }
  }

  if (result_row_count > (unsigned long) ocelot_history_max_row_count) history_result_row_count= ocelot_history_max_row_count;
  else history_result_row_count= result_row_count;

  if (is_vertical == 1)
  {
    unsigned int longest_column_name_length= 0;
    unsigned int column_length;
    if (ocelot_result_grid_column_names_copy == 1)
    {
      char *pointer_to_field_names= result_field_names;
      for (col= 0; col < history_result_column_count; ++col)
      {
        memcpy(&column_length, pointer_to_field_names, sizeof(unsigned int));
        if (column_length > longest_column_name_length)
          longest_column_name_length= column_length;
        pointer_to_field_names+= sizeof(unsigned int);
        pointer_to_field_names+= column_length;
      }
    }

    history_line= new char[HISTORY_MAX_VERTICAL_COLUMN_WIDTH + 256];
    for (r= 0; r < history_result_row_count; ++r)
    {
      sprintf(history_line, "*************************** %ld. row ***************************\n", r + 1);
      s.append(history_line);
      char *row_pointer;
      unsigned int column_length;
      char flag;
      char *pointer_to_source;
      row_pointer= result_set_copy_rows[r];
      char *pointer_to_field_names= result_field_names;
      for (col= 0; col < history_result_column_count; ++col)
      {
        pointer_to_history_line= history_line;
        memcpy(&column_length, pointer_to_field_names, sizeof(unsigned int));
        pointer_to_field_names+= sizeof(unsigned int);
        memset(pointer_to_history_line, ' ', longest_column_name_length - column_length);
        pointer_to_history_line+= longest_column_name_length - column_length;
        memcpy(pointer_to_history_line, pointer_to_field_names, column_length);
        pointer_to_field_names+= column_length;
        pointer_to_history_line+= column_length;
        *(pointer_to_history_line++)= ':';
        *(pointer_to_history_line++)= ' ';
        memcpy(&column_length, row_pointer, sizeof(unsigned int));
        flag= *(row_pointer + sizeof(unsigned int));
        row_pointer+= sizeof(unsigned int) + sizeof(char);
        if ((flag & FIELD_VALUE_FLAG_IS_NULL) != 0)
        {
          length= strlen(NULL_STRING);
          pointer_to_source= (char *) NULL_STRING;
        }
        else
        {
          length= column_length;
          pointer_to_source= row_pointer;
        }
        if (length > HISTORY_MAX_VERTICAL_COLUMN_WIDTH) length= HISTORY_MAX_VERTICAL_COLUMN_WIDTH;
        memcpy(pointer_to_history_line, pointer_to_source, length);
        row_pointer+= column_length;
        pointer_to_history_line+= length;
        *(pointer_to_history_line)= '\n'; *(pointer_to_history_line + 1)= '\0';
        s.append(history_line);
      }
    }
    return s;
  }

  history_line= new char[history_line_width + 2];

  divider_line= new char[history_line_width + 2];



  {
    char *pointer_to_divider_line;
    pointer_to_divider_line= divider_line;
    *(pointer_to_divider_line++)= '+';
    for (col= 0; col < history_result_column_count; ++col)
    {
      memset(pointer_to_divider_line, '-',
             history_max_column_widths[col] + HISTORY_COLUMN_MARGIN * 2);
      pointer_to_divider_line+=
             history_max_column_widths[col] + HISTORY_COLUMN_MARGIN * 2;
      *(pointer_to_divider_line++)= '+';
    }
    *(pointer_to_divider_line)= '\n'; *(pointer_to_divider_line + 1)= '\0';
  }

  if (ocelot_result_grid_column_names_copy == 1)
  {
    char *pointer_to_field_names= result_field_names;
    unsigned int column_length;
    s.append(divider_line);
    pointer_to_history_line= history_line;
    *(pointer_to_history_line++)= '|';
    for (col= 0; col < history_result_column_count; ++col)
    {
      memset(pointer_to_history_line, ' ', HISTORY_COLUMN_MARGIN);
      pointer_to_history_line+= HISTORY_COLUMN_MARGIN;
      memcpy(&column_length, pointer_to_field_names, sizeof(unsigned int));
      pointer_to_field_names+= sizeof(unsigned int);
      length= column_length;
      if (length > history_max_column_widths[col]) length= history_max_column_widths[col];
      memcpy(pointer_to_history_line, pointer_to_field_names, length);
      pointer_to_field_names+= column_length;
      pointer_to_history_line+= length;
      if (length < history_max_column_widths[col])
      {
        length= history_max_column_widths[col] - length;
        memset(pointer_to_history_line, ' ', length);
        pointer_to_history_line+= length;
      }
      memset(pointer_to_history_line, ' ', HISTORY_COLUMN_MARGIN);
      pointer_to_history_line+= HISTORY_COLUMN_MARGIN;
      *(pointer_to_history_line++)= '|';
    }
    *(pointer_to_history_line)= '\n'; *(pointer_to_history_line + 1)= '\0';
    s.append(history_line);
  }
  pointer_to_history_line= history_line;
  s.append(divider_line);
  for (r= 0; r < history_result_row_count; ++r)
  {
    char *row_pointer;
    unsigned int column_length;
    char flag;
    char *pointer_to_source;
    unsigned int spaces_before, spaces_after;
    pointer_to_history_line= history_line;
    row_pointer= result_set_copy_rows[r];
    *(pointer_to_history_line++)= '|';
    for (col= 0; col < history_result_column_count; ++col)
    {
      memcpy(&column_length, row_pointer, sizeof(unsigned int));
      flag= *(row_pointer + sizeof(unsigned int));
      row_pointer+= sizeof(unsigned int) + sizeof(char);
      if ((flag & FIELD_VALUE_FLAG_IS_NULL) != 0)
      {
        length= strlen(NULL_STRING);
        pointer_to_source= (char *) NULL_STRING;
      }
      else
      {
        length= column_length;
        pointer_to_source= row_pointer;
      }
      spaces_before= spaces_after= HISTORY_COLUMN_MARGIN;
      if (length > history_max_column_widths[col]) length= history_max_column_widths[col];
      else
      {
        /* Todo: for Tarantool, check (flag & FIELD_VALUE_FLAG_IS_NUMBER) */
        if ((result_field_flags[col] & NUM_FLAG) != 0)
        //if (result_field_types[col] <= MYSQL_TYPE_DOUBLE)
        {
          spaces_before+= history_max_column_widths[col] - length;
        }
        else
        {
          spaces_after+= history_max_column_widths[col] - length;
        }
      }
      memset(pointer_to_history_line, ' ', spaces_before);
      pointer_to_history_line+= spaces_before;
      memcpy(pointer_to_history_line, pointer_to_source, length);
      pointer_to_history_line+= length;
      memset(pointer_to_history_line, ' ', spaces_after);
      pointer_to_history_line+= spaces_after;
      *(pointer_to_history_line++)= '|';
      row_pointer+= column_length;
    }
    *(pointer_to_history_line)= '\n'; *(pointer_to_history_line + 1)= '\0';
    s.append(history_line);
  }
  s.append(divider_line);
  if (history_line != 0) delete [] history_line;
  if (history_max_column_widths != 0) delete [] history_max_column_widths;
  return s;
}

/*
  Thoughts about ocelot_result_grid_vertical
  ------------------------------------------

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
  There could also be a menu item to "pivot".
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
*/


/*
  grid_column_size_calc()
  Given column lengths in characters, calculate widths+heights in pixels.
  We aren't using defined width -- we went through the rows earlier and
  know what the maximum actual width is for each column.
  So let's give each column exactly what it needs, and perform a
  "squeeze" (reducing big columns) until the rows will fit, or until
  there's nothing more that can be squeezed. The text_edit_frame will
  be wider+higher than the text_edit_widget because it also has the
  drag line and the border.
  In order to support drag lines and variable-height columns, we have to
  calculate cell sizes ourselves. There were surprise difficulties:
  * Qt will decide there isn't enough space in a cell even if the
    total character-widths and/or character-heights would fit; the
    only solution that I found was to turn off the horizontal and
    vertical scrolls "Qt::ScrollBarAlwaysOff)", and turn the vertical
    scroll bar back on "Qt::ScrollBarAsNeeded)" only when our own
  * Qt has a left margin setting which I didn't know about, see the line
    text_edit_widgets[i_cp]->document()->setDocumentMargin(0);
  * There are bugs for some Qt versions and platforms.
     Mono fonts seem to be more susceptible. Perhaps outline-versus-bitmap is a hint too.
     Courier New is particularly awful, and we don't seem to be the only ones who've noticed, see
     https://bitbucket.org/equalsraf/vim-qt/issues/59/undercurl-underline-or-even-underscore-not
     Incomprehensibly, sometimes leading and descent can be negative.
     See also https://bugreports.qt.io/browse/QTBUG-15974 (didn't help).
  Todo: For italic|oblique we double the width, but for some fonts
        e.g. Ubuntu mono it's not necessary. Discourage italic|oblique.
  Todo: There has to be some maximum size, perhaps a user setting.
  Todo: If contents are not plaintext, HTML can cause chaos. We allow
        HTML because its effects might be what users want to see, for
        example "select '<i>HELLO</i>';" really shows in italics. But
        our calculator won't detect markup that changes character sizes.
  Todo: The calculator won't detect line feeds or other control
        characters, which change height. But I'm not worried about
        <cr>, it should only cause an elider.
  Todo: Allow expansion if ctrl-+.
  Todo: All calculations are for text. Images work, but that's luck.
  Todo: Maximum line width and maximum lines per row aren't user settable.
  Todo: We calculate column width based on the width of 'W'. For fixed
        fonts this is of course correct since all characters have the
        same width. But for non-fixed fonts, if the column is numeric,
        we know that there is no wide character like 'W' so we can
        calculate a smaller maximum character width, but beware of 'NULL'.
        And for non-fixed fonts, maybe some obscure non-Latin characters
        are wider than 'W'.
  Todo: Depending on font() may be wrong because font() is only what
        was requested not what is actual, so things might be more reliable
        if we used fontInfo() then created a font based on that and then
        used QFontMetrics.
  Todo: Allow different cells to have different fonts.
  Todo: Sometimes we recalculate after font change, but not always.
        Therefore sometimes font change will cause an existing grid
        to look ugly, and users can only fix it by redoing the query.
  Todo: see if some non-fixed-font characters are wider than "W".
  Todo: Our minimum width is the header width, or, when the header width
        is short|unstated, the width of one character + the width of a
        vertical scroll bar (which we assume is always the same).
        Maybe there should be a user-settable minimum column width.
        Or there should be a minimum for the sake of elide.
  Todo: Header height calculation should differ from ordinary-row height
        calculation, but shouldn't be done in a different place.
  Todo: Japanese kanji, size 40, Ubuntu mono, needed another pixel,
        has no top margin i.e. it hugs the top border.
  Todo: if you ever do Windows or Mac OS, you'll have to revisit this.
*/
void grid_column_size_calc(int ocelot_grid_cell_border_size_as_int,
                           int ocelot_grid_cell_drag_line_size_as_int,
                           unsigned short int is_using_column_names,
                           int connections_dbms)
{
  unsigned int i;
  /* unsigned int tmp_column_lengths[MAX_COLUMNS]; */
  unsigned int sum_tmp_column_lengths;
  unsigned int sum_amount_reduced;
  unsigned int necessary_reduction;
  unsigned int amount_being_reduced;
  unsigned int max_reduction;
  QFont *pointer_to_font;

  pointer_to_font= &text_edit_widget_font;

  /* Calculate with rounding up because of inter-character spacing. */
  QFontMetrics mm= QFontMetrics(*pointer_to_font);
  max_width_of_a_char= mm.width("WWWW") / 4;
  if ((max_width_of_a_char * 4) < (unsigned int) mm.width("WWWW")) ++max_width_of_a_char;

  /*
     For italic|oblique I sometimes need zero extra pixels, but I
     sometimes need an incredible number of extra pixels.
     abs(qfm.rightBearing('W')) + abs(qfm.leftBearing('W')) is not enough
     Todo: try again to reduce, meanwhile document: don't use italics.
  */
  if (pointer_to_font->italic() == true)
  {
    max_width_of_a_char*= 2;
  }

  /* (pointer_to_font->fixedPitch() always == false, I don't know why */
  if (mm.width("WWWWWWWWWW") != mm.width("I- 1a!~:wX"))
  {
    ++max_width_of_a_char;
  }

  /* max_height_of_a_char= mm.lineSpacing(); didn't work */
  max_height_of_a_char= abs(mm.leading()) + abs(mm.ascent()) + abs(mm.descent());

  sum_tmp_column_lengths= 0;

  /*
    The first approximation
    Take it that grid_column_widths[i] = max actual column width.
    If this is good enough, then grid_row_heights[i] = 1 char
    and column width = grid_column_widths[i] chars.
  */
  for (i= 0; i < gridx_column_count; ++i)
  {
    if (is_using_column_names != 0)
    {
      /* probably this->mysql_fields[i].name_length */
      grid_column_widths[i]= 0;
      char tmp[1024];
      unsigned int l= dbms_get_field_name_length(i, connections_dbms);
      strncpy(tmp, dbms_get_field_name(i, connections_dbms).toUtf8(), l);
      set_max_column_width(l, tmp, &grid_column_widths[i]);
      //grid_column_widths[i]= dbms_get_field_name_length(i, connections_dbms);
    }
    else grid_column_widths[i]= 1;
    /*
      For some reason -- I never figured it out -- if column width < 3
      and there is a drag line, the drag line disappears.
    */
    if ((grid_column_widths[i] < 3) && (ocelot_grid_cell_drag_line_size_as_int > 0))
      grid_column_widths[i]= 3;
    if (grid_column_widths[i] < gridx_max_column_widths[i]) grid_column_widths[i]= gridx_max_column_widths[i]; /* fields[i].length */
    grid_column_widths[i]= grid_column_widths[i] * max_width_of_a_char
                           + ocelot_grid_cell_border_size_as_int * 2
                           + ocelot_grid_cell_drag_line_size_as_int;
    sum_tmp_column_lengths+= grid_column_widths[i];
  }

  /*
    The Squeeze
    This cuts the widths of the really long columns, it might loop several times.
    This is a strong attempt to reduce to the user-settable maximum, but if we have to override it, we do.
    Cannot squeeze to less than header length
  */

  sum_amount_reduced= 1;

  while ((sum_tmp_column_lengths > ocelot_grid_max_desired_width_in_pixels) && (sum_amount_reduced > 0))
  {
    necessary_reduction= sum_tmp_column_lengths - ocelot_grid_max_desired_width_in_pixels;
    necessary_reduction-= necessary_reduction % max_width_of_a_char;
    sum_amount_reduced= 0;
    for (i= 0; i < gridx_column_count; ++i)
    {
      unsigned int min_width;
      min_width= mm.width(dbms_get_field_name(i, connections_dbms));
      if (min_width < max_width_of_a_char + scroll_bar_width + 1)
        min_width= max_width_of_a_char + scroll_bar_width + 1;
      min_width+= ocelot_grid_cell_border_size_as_int * 2
                  + ocelot_grid_cell_drag_line_size_as_int;
      if (grid_column_widths[i] <= min_width) continue;
      max_reduction= grid_column_widths[i] - min_width;
      max_reduction-= max_reduction % max_width_of_a_char;
      if (grid_column_widths[i] >= (sum_tmp_column_lengths / gridx_column_count))
      {
        amount_being_reduced= grid_column_widths[i] / 2;
        amount_being_reduced-= amount_being_reduced % max_width_of_a_char;
        if (amount_being_reduced > necessary_reduction) amount_being_reduced= necessary_reduction;
        if (amount_being_reduced > max_reduction) amount_being_reduced= max_reduction;
        grid_column_widths[i]= grid_column_widths[i] - amount_being_reduced;
        sum_amount_reduced+= amount_being_reduced;
        necessary_reduction-= amount_being_reduced;
        sum_tmp_column_lengths-= amount_being_reduced;
      }
      if (necessary_reduction <= max_width_of_a_char) break; /* todo: consider making this "< 10" */
    }
  }

  grid_actual_row_height_in_lines= 1;

  /*
    Each column's height = (gridx_max_column_widths[i] i.e. actual max) / grid_column_widths[i] rounded up.
    If that's greater than the user-defined maximum, reduce to user-defined maximum
    The QTextEdit will get a vertical scroll bar if there's an overflow,
  */

  for (i= 0; i < gridx_column_count; ++i)
  {
    grid_column_heights[i]= (gridx_max_column_widths[i] * max_width_of_a_char) / grid_column_widths[i]; /* mysql_fields[i].length */
    if ((grid_column_heights[i] * grid_column_widths[i]) < (gridx_max_column_widths[i] * max_width_of_a_char))
    {
      ++grid_column_heights[i];
    }
    if (grid_column_heights[i] == 0) ++grid_column_heights[i];
    if (grid_column_heights[i] > ocelot_grid_max_column_height_in_lines) grid_column_heights[i]= ocelot_grid_max_column_height_in_lines;
    if (grid_column_heights[i] > grid_actual_row_height_in_lines) grid_actual_row_height_in_lines= grid_column_heights[i];
  }

  /* Warning: header_height is also calculated like this but in a different place. */
  for (i= 0; i < gridx_column_count; ++i)
  {
    grid_column_heights[i]= (grid_column_heights[i] * max_height_of_a_char)
                            + ocelot_grid_cell_border_size_as_int * 2
                            + ocelot_grid_cell_drag_line_size_as_int;
  }
}


/*
  Make a copy of mysql_res.
    It's insane that I have to make a copy of what was in mysql_res, = result_set_copy.
    But things get complicated if there are multiple result sets i.e. if mysql_more_results is true.
    Also, after the copy, we're less (or not at all?) dependent on calls to MySQL functions.
  For each column, we have: (unsigned int) length, (char) unused or null flag, (char[n]) contents.
  We want max actual length too.
*/
void scan_rows(unsigned int p_result_column_count,
               unsigned int p_result_row_count,
               MYSQL_RES *p_mysql_res,
               char **p_result_set_copy,
               char ***p_result_set_copy_rows,
               unsigned int **p_result_max_column_widths)
{
  unsigned long int v_r;
  unsigned int i;
  MYSQL_ROW v_row;
  unsigned long *v_lengths;
//  unsigned int ki;

  for (i= 0; i < p_result_column_count; ++i) (*p_result_max_column_widths)[i]= 0;

  /*
    First loop: find how much to allocate. Allocate. Second loop: fill in with pointers within allocated area.
  */
  unsigned int total_size= 0;
  char *result_set_copy_pointer;
  lmysql->ldbms_mysql_data_seek(p_mysql_res, 0);
  for (v_r= 0; v_r < p_result_row_count; ++v_r)                                /* first loop */
  {
    v_row= lmysql->ldbms_mysql_fetch_row(p_mysql_res);
    v_lengths= lmysql->ldbms_mysql_fetch_lengths(p_mysql_res);
    for (i= 0; i < p_result_column_count; ++i)
    {
//      ki= (v_r + 1) * result_column_count + i;
      if ((v_row == 0) || (v_row[i] == 0))
      {
        total_size+= sizeof(unsigned int) + sizeof(char);
        //total_size+= sizeof(NULL_STRING) - 1;
      }
      else
      {
        if ((ocelot_client_side_functions_copy == 1)
         && (v_lengths[i] == sizeof("row_number() over ()") - 1)
         && (strncasecmp(v_row[i], "row_number() over ()", v_lengths[i]) == 0))
        {
          total_size+= sizeof(unsigned int) + sizeof(char);
          char tmp[16];
          sprintf(tmp, "%ld", v_r + 1);
          total_size+= strlen(tmp);
        }
        else
        {
          total_size+= sizeof(unsigned int) + sizeof(char);
          total_size+= v_lengths[i];
        }
      }
    }
  }
  *p_result_set_copy= new char[total_size];                                              /* allocate */
  *p_result_set_copy_rows= new char*[p_result_row_count];
  result_set_copy_pointer= *p_result_set_copy;
  lmysql->ldbms_mysql_data_seek(p_mysql_res, 0);

  for (v_r= 0; v_r < p_result_row_count; ++v_r)                                 /* second loop */
  {
    (*p_result_set_copy_rows)[v_r]= result_set_copy_pointer;
    v_row= lmysql->ldbms_mysql_fetch_row(p_mysql_res);
    v_lengths= lmysql->ldbms_mysql_fetch_lengths(p_mysql_res);
    for (i= 0; i < p_result_column_count; ++i)
    {
      if ((v_row == 0) || (v_row[i] == 0))
      {
        if (sizeof(NULL_STRING) - 1 > (*p_result_max_column_widths)[i]) (*p_result_max_column_widths)[i]= sizeof(NULL_STRING) - 1;
        memset(result_set_copy_pointer, 0, sizeof(unsigned int));
        *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_NULL;
        result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
      }
      else
      {

        if ((ocelot_client_side_functions_copy == 1)
         && (v_lengths[i] == sizeof("row_number() over ()") - 1)
         && (strncasecmp(v_row[i], "row_number() over ()", v_lengths[i]) == 0))
        {
          char tmp[16];
          sprintf(tmp, "%ld", v_r + 1);
          unsigned int v_length= strlen(tmp);
          //if (v_length > (*p_result_max_column_widths)[i]) (*p_result_max_column_widths)[i]= v_length;
          set_max_column_width(v_length, tmp, (&(*p_result_max_column_widths)[i]));
          memcpy(result_set_copy_pointer, &v_length, sizeof(unsigned int));
          *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_ZERO;
          result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
          memcpy(result_set_copy_pointer, tmp, v_length);
          result_set_copy_pointer+= v_length;
        }
        else
        {
          //if (v_lengths[i] > (*p_result_max_column_widths)[i]) (*p_result_max_column_widths)[i]= v_lengths[i];
          set_max_column_width(v_lengths[i], v_row[i], (&(*p_result_max_column_widths)[i]));
          memcpy(result_set_copy_pointer, &v_lengths[i], sizeof(unsigned int));
          *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_ZERO;
          result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
          memcpy(result_set_copy_pointer, v_row[i], v_lengths[i]);
          result_set_copy_pointer+= v_lengths[i];
        }
      }
    }
  }
}


/*
  How many UTF-8 characters are there, maximum?
  This is more important for a width calculation than length in bytes.
  The following doesn't do a great job -- I wanted to get it right for
  Latin special characters and Greek or Cyrillic or other alphabets --
  so it misses combining characters and it mishandles Chinese|Japanese
  characters which sometimes are wide. Essentially the algorithm is:
  count all the ASCII and continuation bytes, don't count leading bytes.
  Don't bother if the length can't be greater than the current maximum.
  Todo: skip this if it's an image.
  Todo: we're not doing this if vertical!
  Todo: The "++i;" in this code exists so that 3-byte UTF-8 will
        result in double-wide, which seems okay for Japanese kanji.
        But it's ridiculous! Surely many 3-byte UTF-8 characters
        (U+0800 and beyond) are not double-wide, surely some
        2-byte UTF-8 characters are double-wide. For details see
        http://unicode.org/reports/tr11/, http://www.unicode.org/Public/5.2.0/ucd/EastAsianWidth.txt,
        http://stackoverflow.com/questions/3634627/how-to-know-the-preferred-display-width-in-columns-of-unicode-characters
*/
void set_max_column_width(unsigned int v_length,
                         const char *result_set_copy_pointer,
                         unsigned int *p_result_max_column_width)
{
  if (v_length <= *p_result_max_column_width) return;
  unsigned int j= v_length;
  for (unsigned int i= 0; i < v_length; ++i)
  {
    if (( *(result_set_copy_pointer + i)   & 0xc0) == 0x80)
    {
      --j;
      ++i;
    }
  }
  if (j > *p_result_max_column_width) *p_result_max_column_width= j;
}


/*
  Using the same technique as in scan_rows, make a copy of field names.

  Todo: This (length,data,length,data,length,data...) is a bad way to
  store because we have to scan X entries in order to find field X name.
  This would be better: (pointer,pointer,pointer,...data,data,data...).

  MYSQL_FIELD has: name, org_name, org_table, db. We only need name for result set
  display, but we need the others if user edits the result set (see TextEditWidget::keyPressEvent).
  Todo: we could try going through the token list to find this out, but it's tough
  to watch for AS clauses + UNIONs + expressions + which-field-is-which-table, and
  I'm not sure what would occur when the SELECT is inside a stored procedure.
*/
void scan_field_names(
               const char *which_field,
               unsigned int p_result_column_count,
               char **p_result_field_names)
{
  unsigned int i;
  unsigned int v_lengths;

  /*
    First loop: find how much to allocate. Allocate. Second loop: fill in with pointers within allocated area.
  */
  unsigned int total_size= 0;
  char *result_field_names_pointer;

  for (i= 0; i < p_result_column_count; ++i)                                /* first loop */
  {
      total_size+= sizeof(unsigned int);
      if (strcmp(which_field, "name") == 0) total_size+= mysql_fields[i].name_length;
      else if (strcmp(which_field, "org_name") == 0) total_size+= mysql_fields[i].org_name_length;
      else if (strcmp(which_field, "org_table") == 0) total_size+= mysql_fields[i].org_table_length;
      else /* if (strcmp(which_field, "db") == 0) */ total_size+= mysql_fields[i].db_length;
  }
  *p_result_field_names= new char[total_size];                               /* allocate */

  result_field_names_pointer= *p_result_field_names;
  for (i= 0; i < p_result_column_count; ++i)                                 /* second loop */
  {
    if (strcmp(which_field, "name") == 0) v_lengths= mysql_fields[i].name_length;
    else if (strcmp(which_field, "org_name") == 0) v_lengths= mysql_fields[i].org_name_length;
    else if (strcmp(which_field, "org_table") == 0) v_lengths= mysql_fields[i].org_table_length;
    else /* if (strcmp(which_field, "db") == 0) */ v_lengths= mysql_fields[i].db_length;
    memcpy(result_field_names_pointer, &v_lengths, sizeof(unsigned int));
    result_field_names_pointer+= sizeof(unsigned int);
    if (strcmp(which_field, "name") == 0) memcpy(result_field_names_pointer, mysql_fields[i].name, v_lengths);
    else if (strcmp(which_field, "org_name") == 0) memcpy(result_field_names_pointer, mysql_fields[i].org_name, v_lengths);
    else if (strcmp(which_field, "org_table") == 0) memcpy(result_field_names_pointer, mysql_fields[i].org_table, v_lengths);
    else /* if (strcmp(which_field, "db") == 0) */ memcpy(result_field_names_pointer, mysql_fields[i].db, v_lengths);
    result_field_names_pointer+= v_lengths;
  }
}


/*
   Set alignment and height of a cell.
   Todo: There's a terrible amount of duplication:
   If vertical == false, this happens once before we do any displaying (but we don't call this).
   If vertical == true, this happens at start and every time we scroll.
   Todo: pass flags so I don't have to check so many field types
*/
void set_alignment_and_height(int ki, int grid_col, int field_type)
{
  TextEditWidget *cell_text_edit_widget= text_edit_widgets[ki];
  if ((field_type <= MYSQL_TYPE_DOUBLE)
   || (field_type == MYSQL_TYPE_NEWDECIMAL)
   || (field_type == MYSQL_TYPE_LONGLONG)
   || (field_type == MYSQL_TYPE_INT24))
    text_align(cell_text_edit_widget, Qt::AlignRight);
  else text_align(cell_text_edit_widget, Qt::AlignLeft);
  /* Height border size = 1 due to setStyleSheet earlier; right border size is passed */
//  if (xrow == 0)
//  {
//    int header_height= max_height_of_a_char
//                     + ocelot_grid_cell_border_size_as_int * 2
//                     + ocelot_grid_cell_drag_line_size_as_int;
//    if (ocelot_grid_cell_drag_line_size_as_int > 0) header_height+= max_height_of_a_char;
//    text_edit_frames[xrow * gridx_column_count + col]->setFixedSize(grid_column_widths[col], header_height);
//    text_edit_frames[xrow * gridx_column_count + col]->setMaximumHeight(header_height);
//    text_edit_frames[xrow * gridx_column_count + col]->setMinimumHeight(header_height);
//  }
//  else
  {
    int this_width;
    //if (grid_col == 0)
    //{
    //  /* Todo: this should be based on QFontMetrics, 20 is so arbitrary */
    //  this_width= (20) * (text_edit_frames[ki]->content_length + 1);
    //}
    //else
    this_width= grid_column_widths[grid_col];
    frame_resize(ki, grid_col, this_width, grid_column_heights[grid_col]);
  }
}


/*
  Put lengths and pointers in text_edit_frames.
  Set a flag to say "not retrieved yet", that happens at paint time.
  Todo: make a copy if there are multiple result sets.
  Todo: this points directly to a mysql_res row, ignoring the earlier clever ideas in dbms_get_field_value().
*/
/* The big problem is that setVerticalSpacing(0) goes awry if I use hide(). */
/* Todo: Think whether there's a chance that, while somebody scroll to the end,
   the hide() comes too late, i.e. a paint might occur for an invalid row.
   Maybe show() should be delayed until after hide(), or painting should be prevented for a while. */

void fill_detail_widgets(int new_grid_vertical_scroll_bar_value, int connections_dbms)
{
  unsigned int i;
  unsigned int text_edit_frames_index;
  unsigned int grid_row;
  int first_row;
  char *row_pointer;

  first_row= new_grid_vertical_scroll_bar_value;

  if (ocelot_result_grid_vertical_copy != 0)
  {
    unsigned int result_column_number; /* i.e. column number in result set, not column number in grid */
    first_row= new_grid_vertical_scroll_bar_value / result_column_count;
    result_column_number= new_grid_vertical_scroll_bar_value % result_column_count;
    unsigned int new_content_length= 0;
    row_pointer= result_set_copy_rows[first_row];
    for (unsigned int j= 0; ; ++j)
    {
      memcpy(&new_content_length, row_pointer, sizeof(unsigned int));
      row_pointer+= sizeof(unsigned int) + sizeof(char);
      if (j >= result_column_number) break;
      row_pointer+= new_content_length;
    }

    /* now row_pointer -> result column, and each subsequent result column is a grid row */
    //grid_row= first_row;

    grid_row= 0;
    result_row_number= first_row;

    for (;;)
    {
      unsigned int v_lengths= 0;

      text_edit_frames_index= grid_row * gridx_column_count;
      unsigned int o_text_edit_frames_index= text_edit_frames_index;

      /* result_column_number is still known */
      //if (ocelot_client_side_functions_copy != 0)                         /* include row#? */
      //{
      //  text_edit_frames[text_edit_frames_index]->content_length= 0;
      //  text_edit_frames[text_edit_frames_index]->content_pointer= 0;
      //  text_edit_frames[text_edit_frames_index]->is_retrieved_flag= false;
      //  text_edit_frames[text_edit_frames_index]->ancestor_grid_column_number= result_column_number;
      //  text_edit_frames[text_edit_frames_index]->ancestor_grid_result_row_number= result_row_number;
      //  text_edit_frames[text_edit_frames_index]->is_image_flag= false;
      //  ++text_edit_frames_index;
      //}
      if (ocelot_result_grid_column_names_copy != 0)               /* include column header? */
      {
        char *result_field_names_pointer= &result_field_names[0];
        for (unsigned int mi= 0; ; ++mi)
        {
          memcpy(&v_lengths, result_field_names_pointer, sizeof(unsigned int));
          result_field_names_pointer+= sizeof(unsigned int);
          if (mi >= result_column_number) break;
          result_field_names_pointer+= v_lengths;
        }
        text_edit_frames[text_edit_frames_index]->content_length= v_lengths;
        text_edit_frames[text_edit_frames_index]->content_pointer= result_field_names_pointer;
        text_edit_frames[text_edit_frames_index]->is_retrieved_flag= false;
        text_edit_frames[text_edit_frames_index]->ancestor_grid_column_number= result_column_number;
        text_edit_frames[text_edit_frames_index]->ancestor_grid_result_row_number= result_row_number;
        text_edit_frames[text_edit_frames_index]->is_image_flag= false;
        ++text_edit_frames_index;
      }
      text_edit_frames[text_edit_frames_index]->content_length= new_content_length; /* include value. */
      if (*(row_pointer - 1) == FIELD_VALUE_FLAG_IS_NULL)
      {
        text_edit_frames[text_edit_frames_index]->content_pointer= 0;
      }
      else text_edit_frames[text_edit_frames_index]->content_pointer= row_pointer;
      text_edit_frames[text_edit_frames_index]->is_retrieved_flag= false;
      text_edit_frames[text_edit_frames_index]->ancestor_grid_column_number= result_column_number;
      text_edit_frames[text_edit_frames_index]->ancestor_grid_result_row_number= result_row_number;
      if (is_extra_rule_1(result_column_number) == true)
      {
        if (text_edit_frames[text_edit_frames_index]->cell_type == TEXTEDITFRAME_CELL_TYPE_DETAIL)
        {
          text_edit_frames[text_edit_frames_index]->cell_type= TEXTEDITFRAME_CELL_TYPE_DETAIL_EXTRA_RULE_1;
          text_edit_frames[text_edit_frames_index]->is_style_sheet_set_flag= false;
        }
      }
      else
      {
        if (text_edit_frames[text_edit_frames_index]->cell_type == TEXTEDITFRAME_CELL_TYPE_DETAIL_EXTRA_RULE_1)
        {
          text_edit_frames[text_edit_frames_index]->cell_type= TEXTEDITFRAME_CELL_TYPE_DETAIL;
          text_edit_frames[text_edit_frames_index]->is_style_sheet_set_flag= false;
        }
      }
      if (is_image(result_column_number) == true)
      {
        text_edit_frames[text_edit_frames_index]->is_image_flag= true;
      }
      else text_edit_frames[text_edit_frames_index]->is_image_flag= false;

      int column_number_within_gridx= 0;

      if (ocelot_result_grid_column_names_copy != 0) gridx_max_column_widths[column_number_within_gridx++]= v_lengths;

      if (*(row_pointer - 1) == FIELD_VALUE_FLAG_IS_NULL)
      {
        gridx_max_column_widths[column_number_within_gridx]= sizeof(NULL_STRING) - 1;
      }
      else gridx_max_column_widths[column_number_within_gridx]= new_content_length;
      grid_column_size_calc(ocelot_grid_cell_border_size_as_int,
                            ocelot_grid_cell_drag_line_size_as_int,
                            0,
                            connections_dbms); /* get grid_column_widths[] and grid_column_heights[] */
      column_number_within_gridx= 0;
      //if (ocelot_client_side_functions_copy != 0)
      //{
      //  set_alignment_and_height(o_text_edit_frames_index + column_number_within_gridx,
      //                           column_number_within_gridx,
      //                           MYSQL_TYPE_SHORT);
      //  text_edit_frames[o_text_edit_frames_index + column_number_within_gridx]->show();
      //  ++column_number_within_gridx;
      //}
      if (ocelot_result_grid_column_names_copy != 0)
      {
        set_alignment_and_height(o_text_edit_frames_index + column_number_within_gridx,
                                 column_number_within_gridx,
                                 MYSQL_TYPE_STRING);
        text_edit_frames[o_text_edit_frames_index + column_number_within_gridx]->show();
        ++column_number_within_gridx;
      }
      set_alignment_and_height(o_text_edit_frames_index + column_number_within_gridx,
                               column_number_within_gridx,
                               result_field_types[result_column_number]);
      text_edit_frames[o_text_edit_frames_index + column_number_within_gridx]->show();
      ++result_column_number;
      ++grid_row;
      if (result_column_number == result_column_count)
      {
        ++result_row_number;
        result_column_number= 0;
      }
      if (result_row_number >= result_row_count) break;
      if (grid_row >= result_grid_widget_max_height_in_lines) break;
      row_pointer+= new_content_length;
      memcpy(&new_content_length, row_pointer, sizeof(unsigned int));
      row_pointer+= sizeof(unsigned int) + sizeof(unsigned char);
    }
    //for (text_edit_frames_index= text_edit_frames_index + 1; text_edit_frames_index < max_text_edit_frames_count; ++text_edit_frames_index) text_edit_frames[text_edit_frames_index]->hide();
  }

  else /* if ocelot_result_grid_vertical_copy == 0 */
  {
    for (result_row_number= first_row, grid_row= 1;
         (result_row_number < result_row_count) && (grid_row < result_grid_widget_max_height_in_lines);
         ++result_row_number, ++grid_row)
    {
      row_pointer= result_set_copy_rows[result_row_number];
//      lengths= lmysql->ldbms_mysql_fetch_lengths(grid_mysql_res);
      for (i= 0; i < gridx_column_count; ++i)
      {
        text_edit_frames_index= grid_row * gridx_column_count + i;
        if (gridx_flags[i] == 1)        /* row number? */
        {
          text_edit_frames[text_edit_frames_index]->content_length= 0;
          text_edit_frames[text_edit_frames_index]->content_pointer= 0;
        }
        else
        {
          memcpy(&(text_edit_frames[text_edit_frames_index]->content_length), row_pointer, sizeof(unsigned int));
          row_pointer+= sizeof(unsigned int) + sizeof(char);
          if (*(row_pointer - 1) == FIELD_VALUE_FLAG_IS_NULL)
          {
            text_edit_frames[text_edit_frames_index]->content_pointer= 0;
          }
          else text_edit_frames[text_edit_frames_index]->content_pointer= row_pointer;
          row_pointer+= text_edit_frames[text_edit_frames_index]->content_length;
        }
        text_edit_frames[text_edit_frames_index]->is_retrieved_flag= false;
        text_edit_frames[text_edit_frames_index]->ancestor_grid_column_number= i;
        text_edit_frames[text_edit_frames_index]->ancestor_grid_result_row_number= result_row_number;
        text_edit_frames[text_edit_frames_index]->show();
      }
    }
  }
  /* todo: maybe what we really want is to hide as far as # of used rows, which may be < max */
  for (grid_row= grid_row; grid_row < result_grid_widget_max_height_in_lines; ++grid_row) /* so if scroll bar goes past end we won't see these */
  {
    for (i= 0; i < gridx_column_count; ++i)
    {
      text_edit_frames_index= grid_row * gridx_column_count + i;
      text_edit_frames[text_edit_frames_index]->hide();
    }
  }
}

/*
  We don't know exactly the maximum number of lines that will fit
  on a grid widget, but we can guess based on the height of the
  grid widget divided by the height of one line in a text edit widget.
  This affects: the size of the row pool (it must be >= the number
  of lines), the decision what kind of vertical scroll bar to use,
  and the number of detail widgets that we will fill with data.
  Two things can affect the calculation:
  (1) Height of result_grid_table_widget[i] can change, at start,
      or due to squeezing by statement + history widgets, or due
      to user action if the widget is detached. resizeEvent() sees that.
      If the number of lines increases, resize + display again.
  (2) Height of the text edit widget can change, set_all_style_sheets()
      sees that. For either increase or decrease, resize + display again.
  We're making the over-cautious assumption that it will be necessary to assign
  1 texteditframe for 1 line. In fact a texteditframe is always bigger than a line.
  We try to avoid recalculating just because user shifts by a few pixels.
*/
void resize_or_font_change(int height_of_grid_widget, bool is_resize)
{
  if ((copy_of_ocelot_batch != 0)
   || (copy_of_ocelot_html != 0)
   || (copy_of_ocelot_xml != 0))
  {
    if (copy_of_ocelot_html == 0)
      batch_text_edit->setStyleSheet(copy_of_parent->ocelot_grid_style_string);
    return;
  }
  QFont tmp_font= this->font();
  QFontMetrics mm= QFontMetrics(this->font());
  unsigned int height_of_line= mm.lineSpacing();
  unsigned int max_height_in_lines= height_of_grid_widget / height_of_line;
  if ((max_height_in_lines * height_of_line) < (unsigned int) height_of_grid_widget) ++ max_height_in_lines;
  if ((is_resize == false)
   || (max_height_in_lines > result_grid_widget_max_height_in_lines))
  {
    result_grid_widget_max_height_in_lines= max_height_in_lines;
    if (result_set_copy != 0)  /* see garbage_collect() comment */
    {
      remove_layouts();
      display();
      this->show();
    }
  }
}

/*
  Resize of grid widget. This could be called from eventfilter instead.
  Warning: changes in statement widget can cause this to be invoked.
*/
void resizeEvent(QResizeEvent *event)
{
  unsigned int height_of_grid_widget= event->size().height();
  if (height_of_grid_widget != (unsigned int) event->oldSize().height())
  {
    resize_or_font_change(height_of_grid_widget, true);
  }
}


/*
  Called from eventfilter
  "    if (event->type() == QEvent::FontChange) return (result_grid_table_widget->fontchange_event());"
  Todo: This never happens, set_all_style_sheets() gets called for font change.
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
  Todo: catch QEvent::Resize() too!! (maybe this is obsolete, we have void resizeEvent() above.
*/
bool show_event()
{
//  vertical_bar_show_as_needed();

  return false;           /* We want the show to happen so pass it on */
}


/*
  Called from eventfilter if and only if result_row_count > result_grid_widget_max_height_in_lines
  There is also an automatic show-as-needed scroll bar, which will come on if scrolling is inevitable for a smaller result set.
  But vertical_scroll_bar_event() is only for the non-automatic vertical scroll bar.
  Initially grid_vertical_scroll_bar_value == -1, it's checked so that we don't paint the initial display twice.
*/
bool vertical_scroll_bar_event(int connections_dbms)
{
  int new_value;
  /* It's impossible to get here if the scroll bar is hidden, but it happens. Well, maybe only for "turning it off" events. */
  if (grid_vertical_scroll_bar->isVisible() == false)
  {
      return false;
  }
  /* It's ridiculous to do these settings every time. But when is the best time to do them? Which event matters? */
  if (ocelot_result_grid_vertical_copy == 0) grid_vertical_scroll_bar->setMaximum(result_row_count - 1);
  else grid_vertical_scroll_bar->setMaximum(grid_result_row_count - 1);
  grid_vertical_scroll_bar->setSingleStep(1);
  grid_vertical_scroll_bar->setPageStep(1);
  grid_vertical_scroll_bar->setMinimum(0);

  new_value= grid_vertical_scroll_bar->value();

  if (new_value != grid_vertical_scroll_bar_value)
  {
    if ((is_paintable == 1) && (grid_vertical_scroll_bar_value != -1))
    {
      fill_detail_widgets(new_value, connections_dbms);
      this->update();      /* not sure if we need to update both this and client, but it should be harmless*/
      client->update();
    }
    grid_vertical_scroll_bar_value= new_value;
    return false;
  }
  return false;
}


/*
  If a result grid text_edit_widget is a number + not header, call with alignment_flag == Qt::AlighRight.
  Otherwise call with alignment_flag == Qt::AlignLeft.
  Do not assume it's left-aligned otherwise; there's a pool.
  Beware: if you only setAlignment, you lose wrapping, as a side effect of setDefaultTextOption (?)
  Todo: take into account whether it's a right-to-left character set like Arabic or Hebrew.
  Todo: check whether it's actually necessary to setAlignment for text_edit_widget too.
  Todo: check what happens to wrapping if it's a number, I read somewhere that's a problem.
  Todo: some other types e.g. BLOBs might also need special handling.
  Todo: user-settable option rather than WrapAnywhere.
  Todo: check that the effect is immediate, not deferred to the next time resultgrid comes up.
*/
void text_align(QTextEdit *cell_text_edit_widget, enum Qt::AlignmentFlag alignment_flag)
{
  QTextOption to;
  to.setAlignment(alignment_flag);
  to.setWrapMode(QTextOption::WrapAnywhere);
  cell_text_edit_widget->document()->setDefaultTextOption(to);
  cell_text_edit_widget->setAlignment(alignment_flag);
}


/*
  To clean up from a previous result set:
    For each row:
      For each column: remove frame widget from row layout
      Remove row widget from main layout
  Assumption: a blank row i.e. a row with no cells marks the end.
*/
void remove_layouts()
{
  long unsigned int xrow; 
  QLayoutItem *text_edit_frame_item;
  QWidget *text_edit_frame_widget;

  client->hide(); /* client->show() will happen again soon */
  if (grid_main_layout != 0)
  {
    for (xrow= 0; xrow < row_pool_size; ++xrow)
    {
      if (grid_row_layouts[xrow]->count() == 0) break;
      for (;;)
      {
        text_edit_frame_item= grid_row_layouts[xrow]->itemAt(0);
        if (text_edit_frame_item == NULL) break;
        text_edit_frame_widget= text_edit_frame_item->widget();
        if (text_edit_frame_widget == NULL) break;       /* I think this is impossible */
        grid_row_layouts[xrow]->removeWidget(text_edit_frame_widget);
        text_edit_frame_widget->hide(); /* i.e. text_edit_frames[..]->hide() */
      }
      grid_main_layout->removeWidget(grid_row_widgets[xrow]);
    }
    grid_main_layout->removeWidget(batch_text_edit);
  }
}


/*
  We'll do our own garbage collecting for non-Qt items.
  Todo: make sure Qt items have parents where possible so that "delete result_grid_table_widget"
        takes care of them.
  Why we clear() text_edit_widgets:
    If the text is big blobs, and you start with default i.e. ocelot_display_blob_as_image = false,
    then you switch to ocelot_display_blob_as_image = true,
    it is much slower then if you start with ocelot_display_blob_as_image = true.
    Clearing alleviates the problem.
    It would be faster to use max_table_edit_widgets_count not cell_pool_size but that crashes.
    Perhaps it would be better to clear only if current size > (some minimum)?
  Warning: we check if (result_set_copy == 0) to ensure there's a result.
*/
void garbage_collect()
{
  remove_layouts();
  if (grid_column_widths != 0) { delete [] grid_column_widths; grid_column_widths= 0; }
  if (result_max_column_widths != 0) { delete [] result_max_column_widths; result_max_column_widths= 0; }
  if (grid_column_heights != 0) { delete [] grid_column_heights; grid_column_heights= 0; }
  if (grid_column_dbms_sources != 0) { delete [] grid_column_dbms_sources; grid_column_dbms_sources= 0; }
  if (result_field_types != 0) { delete [] result_field_types; result_field_types= 0; }
  if (result_field_charsetnrs != 0) { delete [] result_field_charsetnrs; result_field_charsetnrs= 0; }
  if (result_field_flags != 0) { delete [] result_field_flags; result_field_flags= 0; }
  if (result_set_copy != 0) { delete [] result_set_copy; result_set_copy= 0; }
  if (result_set_copy_rows != 0) { delete [] result_set_copy_rows; result_set_copy_rows= 0; }
  if (result_field_names != 0) { delete [] result_field_names; result_field_names= 0; }
  if (result_original_field_names != 0) { delete [] result_original_field_names; result_original_field_names= 0; }
  if (result_original_table_names != 0) { delete [] result_original_table_names; result_original_table_names= 0; }
  if (result_original_database_names != 0) { delete [] result_original_database_names; result_original_database_names= 0; }
  if (gridx_field_names != 0) { delete [] gridx_field_names; gridx_field_names= 0; }
  if (gridx_max_column_widths != 0) { delete [] gridx_max_column_widths; gridx_max_column_widths= 0; }
  if (gridx_result_indexes != 0) { delete [] gridx_result_indexes; gridx_result_indexes= 0; }
  if (gridx_flags != 0) { delete [] gridx_flags; gridx_flags= 0; }
  if (gridx_field_types != 0) { delete [] gridx_field_types; gridx_field_types= 0; }
  for (unsigned int i= 0; i < cell_pool_size; ++i) text_edit_widgets[i]->clear(); /* unnecessary? */
  if (batch_text_edit != NULL) batch_text_edit->clear(); /* unnecessary? */
}


void set_frame_color_setting()
{
  ocelot_grid_cell_drag_line_size_as_int= copy_of_parent->ocelot_grid_cell_drag_line_size.toInt();
  ocelot_grid_cell_drag_line_color= copy_of_parent->ocelot_grid_cell_drag_line_color;
  frame_color_setting= "TextEditFrame{background-color: ";
  frame_color_setting.append(ocelot_grid_cell_drag_line_color);
  //frame_color_setting.append(";border: 0px");              /* TEST !! */
  frame_color_setting.append("}");
}


/*
  Setting the parent should affect the children.
  But we don't want all text_edit_frames and text_edit_widgets to change because that is slow.
  Let us set a flag which causes change at paint time. with setStyleSheet(copy_of_parent->ocelot_grid_header_style_string);
  This gets called just after we change colors + fonts with the dialog box, so we know
  the new style string, and to get its font we used to create a temporary QTextEdit,
  but nowadays we get font with a function that figures it out from the style sheet syntax.
*/
void set_all_style_sheets(QString new_ocelot_grid_style_string,
                          QString new_ocelot_grid_cell_drag_line_size,
                          int caller,
                          bool is_result_grid_font_size_changed)
{
  if ((copy_of_ocelot_batch != 0)
   || (copy_of_ocelot_html != 0)
   || (copy_of_ocelot_xml != 0))
  {
    resize_or_font_change(this->height(), false);
    return;
  }
  text_edit_widget_font= copy_of_parent->get_font_from_style_sheet(new_ocelot_grid_style_string);

  unsigned int i_h;

  ocelot_grid_cell_drag_line_size_as_int= new_ocelot_grid_cell_drag_line_size.toInt();

  set_frame_color_setting();
  for (i_h= 0; i_h < cell_pool_size; ++i_h)
  {
    text_edit_frames[i_h]->is_style_sheet_set_flag= false;
    //text_edit_widgets[ki]->setMinimumWidth(fm.width("W") * 3);
    //text_edit_widgets[ki]->setMinimumHeight(fm.height() * 2);
    /* todo: skip following line if ocelot_grid_cell_drag_line_size_as_int did not change */
    text_edit_layouts[i_h]->setContentsMargins(QMargins(0, 0, ocelot_grid_cell_drag_line_size_as_int, ocelot_grid_cell_drag_line_size_as_int));
  }
  /* todo: is "caller" redundant? if it's 0, then font change is false? */
  if ((caller == 1) && (is_result_grid_font_size_changed))
  {
    resize_or_font_change(this->height(), false);
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


/* Todo: this doesn't appear to do anything any more. */
void dbms_set_grid_column_sources()
{
  unsigned int column_number;
  unsigned int dbms_field_number;

  dbms_field_number= 0;
  for (column_number= 0; column_number < result_column_count; ++column_number)
  {
    {
      ++dbms_field_number;
    }
  }
}

//unsigned int dbms_get_field_length(unsigned int column_number)
//{
//  QString s;
//  unsigned int dbms_field_number;
//
//  dbms_field_number= grid_column_dbms_field_numbers[column_number];
//  /* The defined length is mysql_fields[dbms_field_number].length. We prefer actual max length which usually is shorter. */
//  return gridx_max_column_widths[dbms_field_number];
//}


unsigned int dbms_get_field_flag(unsigned int column_number, int connections_dbms)
{
  (void) connections_dbms; /* suppress "unused parameter" warning */
  return result_field_flags[column_number];
}


QString dbms_get_field_name(unsigned int column_number, int connections_dbms)
{
  (void) connections_dbms; /* suppress "unused parameter" warning */
  char *result_field_names_pointer;
  unsigned int v_lengths;
  result_field_names_pointer= &result_field_names[0];
  for (unsigned int i= 0; i < column_number; ++i)
  {
    memcpy(&v_lengths, result_field_names_pointer, sizeof(unsigned int));
    result_field_names_pointer+= v_lengths + sizeof(unsigned int);
  }
  memcpy(&v_lengths, result_field_names_pointer, sizeof(unsigned int));
  result_field_names_pointer+= sizeof(unsigned int);
  return result_field_names_pointer;
}


unsigned int dbms_get_field_name_length(unsigned int column_number, int connections_dbms)
{
  (void) connections_dbms; /* suppress "unused parameter" warning */
  char *result_field_names_pointer;
  unsigned int v_lengths;
  result_field_names_pointer= &result_field_names[0];
  for (unsigned int i= 0; i < column_number; ++i)
  {
    memcpy(&v_lengths, result_field_names_pointer, sizeof(unsigned int));
    result_field_names_pointer+= v_lengths + sizeof(unsigned int);
  }
  memcpy(&v_lengths, result_field_names_pointer, sizeof(unsigned int));
  return v_lengths;
}


//QString dbms_get_field_value(int row_number, unsigned int column_number)
//{
//  QString s;
//  unsigned int dbms_field_number;
//
//  if (grid_column_dbms_sources[column_number] == DBMS_SOURCE_IS_MYSQL_FIELD)
//  {
//    dbms_field_number= grid_column_dbms_field_numbers[column_number];
//    return row[dbms_field_number];
//  }
//  if (grid_column_dbms_sources[column_number] == DBMS_SOURCE_IS_ROW_NUMBER)
//  {
//    s= QString::number(row_number);
//    return s;
//  }
//  return ""; /* to avoid "control reaches end of non-void function" warning */
//}

/*
  Deleting ResultGrid
  This probably will never be called explicitly, but if MainWindow parent is deleted when
  the program ends, we'll get here.
*/
~ResultGrid()
{
  //remove_layouts(); /* I think this is unnecessary */
  garbage_collect();
  if (row_pool_size != 0)
  {
    delete [] grid_row_layouts;
    delete [] grid_row_widgets;
    row_pool_size= 0;
  }
  if (cell_pool_size != 0)
  {
    delete [] text_edit_widgets;
    delete [] text_edit_layouts;
    delete [] text_edit_frames;
    cell_pool_size= 0;
  }
}

public slots:
private:
};
#endif // RESULTGRID_H

/*********************************************************************************************************/

/* The Statement Widget */

/*
  A small part of the code for the statement widget was copied from
  http://qt-project.org/doc/qt-4.8/widgets-codeeditor-codeeditor-h.html
  so the original copyright notice and BSD-license provisions are reproduced
  at the start of ocelotgui.cpp, applicable solely to what was copied.
*/

/* CodeEditor is actually used for both the statement widget and the debug widgets. */

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
    CodeEditor(MainWindow *parent= 0);

    void prompt_widget_paintevent(QPaintEvent *event);
    int prompt_width_calculate();
    QString prompt_translate(int line_number);
    int prompt_translate_k(QString s, int i);
    unsigned int statement_count;                                            /* used if "prompt \c ..." */
#ifdef DEBUGGER
    unsigned int block_number;                                               /* current line number, base 0 */
#endif
    bool is_debug_widget;
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
      PROMPT client statement decides whether there are line numbers.
    */

        QColor statement_edit_widget_left_bgcolor;                    /* suggestion = Qt::lightGray. */
        QColor statement_edit_widget_left_treatment1_textcolor;       /* suggestion = Qt::black */
        QString statement_edit_widget_left_treatment1_prompt_text;    /* suggestions = "     >" or "" */

        QString prompt_default;                     /* = "mysql>" -- or is it "\N [\d]>"? */
        QString prompt_as_input_by_user;            /* = What the user input with latest PROMPT statement, or prompt_default */
        /* QString prompt_translated;     */             /* = prompt_as_input_by_user, but with some \s converted for ease of parse */
        QString prompt_current;                     /* = latest result of prompt_reform() */
        MainWindow *main_window;

public slots:
    void update_prompt_width(int newBlockCount);
    void highlightCurrentLine();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:

    void update_prompt(const QRect &, int);

private:
    QWidget *prompt_widget;

/*
   Here is where we show a tooltip help hint e.g. "reserved keyword"
   if the user hovers with the mouse over a statement_edit_widget word.
   Beware: there is more than one CodeEditor.
   We only did setMouseTracking for statement_edit_widget.
   But make sure we're right.
   Beware: This won't work unless we're parsing.
   text= statement_edit_widget->toPlainText(); but maybe I won't care.
   Todo: Consider: QToolTip::showText() instead of setToolTip()
         (it would cause immediate change but might be distracting).
*/
void mouseMoveEvent(QMouseEvent *event)
{
  if (((main_window->ocelot_statement_syntax_checker.toInt()) & FLAG_FOR_HIGHLIGHTS)
   && (main_window->main_token_max_count > 0)
   && (main_window->statement_edit_widget == this))
  {
    QPoint i= event->pos();
    QTextCursor c= this->cursorForPosition(i);
    int p= c.position();
    QString s= "";
    for (int i= 0; main_window->main_token_lengths[i] != 0; ++i)
    {
      int offset= main_window->main_token_offsets[i];
      if (offset > p) break;
      int length= main_window->main_token_lengths[i];
      int token_flag= main_window->main_token_flags[i];
      if ((offset <= p) && ((offset+length) > p))
      {
        int token_type= main_window->main_token_types[i];
        if ((token_flag & TOKEN_FLAG_IS_ERROR) != 0) s= "(error) ";
        if ((token_flag & TOKEN_FLAG_IS_FUNCTION) != 0) s= "(function) ";
        if ((token_type >= MainWindow::TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
         && (token_type <= MainWindow::TOKEN_TYPE_LITERAL))
        {
          s.append("literal");
        }
        else if ((token_type >= MainWindow::TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK)
         && (token_type <= MainWindow::TOKEN_TYPE_IDENTIFIER))
        {
          s.append(main_window->hparse_f_token_to_appendee("[identifier]", main_window->main_token_reftypes[i]));
        }
        else if ((token_type >= MainWindow::TOKEN_TYPE_COMMENT_WITH_SLASH)
         && (token_type <= MainWindow::TOKEN_TYPE_COMMENT_WITH_MINUS))
        {
          s.append("comment");
        }
        else if (token_type == MainWindow::TOKEN_TYPE_OPERATOR)
        {
          s.append("operator");
        }
        else if (token_type == MainWindow::TOKEN_TYPE_OTHER)
        {
          s.append("[identifier or keyword]");
        }
        else
        {
          if ((token_flag & TOKEN_FLAG_IS_RESERVED) != 0)
          {
            s.append("reserved ");
          }
          s.append("keyword");
        }
        break;
      }
    }
    this->setToolTip(s);
  }
  /* We probably don't need to say this. */
  QPlainTextEdit::mouseMoveEvent(event);
  }
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

#endif // CODEEDITOR_H

/*********************************************************************************************************/
/* THE QSCROLLAREAWITHSIZE WIDGET */

/*
  It's really really really hard to make a dialog box have a good size
  if it might scroll. With this subclass I can give a size hint when creating.
  Alas the Height hint tends to be ignored. (Todo: find out why.)
  It also tends to be ignored if I try sizeHint() for the Settings dialog box.
  The workaround is to check main_window_maximum_width later.
*/

#ifndef QSCROLLAREAWITHSIZE_H
#define QSCROLLAREAWITHSIZE_H
class QScrollAreaWithSize : public QScrollArea
{

public:
  int settings_width, settings_height;

QScrollAreaWithSize(int width, int height)
{
  settings_width= width;
  settings_height= height;
}

virtual QSize sizeHint() const
{
  return QSize(settings_width, settings_height);
}

};
#endif // QSCROLLAREAWITHSIZE_H

/*********************************************************************************************************/
/* THE SETTINGS WIDGET */

#ifndef SETTINGS_H
#define SETTINGS_H
class Settings: public QDialog
{
  Q_OBJECT
public:
  QDialog *settings;
/*
private slots:
  void handle_combo_box_1(int);
  void handle_combo_box_for_color_pick_0();
  void handle_combo_box_for_color_pick_1();
  void handle_combo_box_for_color_pick_2();
  void handle_combo_box_for_color_pick_3();
  void handle_combo_box_for_color_pick_4();
  void handle_combo_box_for_color_pick_5();
  void handle_combo_box_for_color_pick_6();
  void handle_combo_box_for_color_pick_7();
  void handle_combo_box_for_color_pick_8();
  void handle_combo_box_for_color_pick_9();
  void handle_combo_box_for_color_pick_10();
*/

public:

  QVBoxLayout *main_layout;
  QWidget *widget_3, *widget_for_font_dialog;
  QHBoxLayout *hbox_layout_3, *hbox_layout_for_font_dialog;
  QPushButton *button_for_cancel, *button_for_ok, *button_for_font_dialog;
  QLabel *widget_font_label;

  QWidget *widget_for_color[11];
  QHBoxLayout *hbox_layout_for_color[11];
  QLabel *label_for_color[11];
  QLabel *label_for_color_rgb[11];
  QLabel *label_for_font_dialog;

  QWidget *widget_for_syntax_checker;
  QLabel *label_for_syntax_checker;
  QComboBox *combo_box_for_syntax_checker;
  QHBoxLayout *hbox_layout_for_syntax_checker;

  QWidget *widget_for_max_row_count;
  QLabel *label_for_max_row_count;
  QSpinBox *spin_box_for_max_row_count;
  QHBoxLayout *hbox_layout_for_max_row_count;

  QWidget *widget_for_size[3];
  QHBoxLayout *hbox_layout_for_size[3];
  QLabel *label_for_size[3];
  QComboBox *combo_box_for_size[3];

  QComboBox *combo_box_for_color_pick[11];
  QLabel *label_for_color_show[11];
  MainWindow *copy_of_parent;

  /* current_widget = MAIN_WIDGET | HISTORY_WIDGET | GRID_WIDGET | STATEMENT_WIDGET | etc. */
  int current_widget;


/* Following might be too short for some new language in ostrings.h */
#define MAX_COLOR_NAME_WIDTH 24

/* TODO: probably some memory is leaking. I don't say "(this)" every time I say "new". */
public:
Settings(int passed_widget_number, MainWindow *parent): QDialog(parent)
{
  int settings_width, settings_height;

  /* settings = new QWidget(this); ... this might come later */

  copy_of_parent= parent;                                /* handle_button_for_ok() might use this */
  current_widget= passed_widget_number;

  /* Copy the parent's settings. They'll be copied back to the parent, possibly changed, if the user presses OK. */
  copy_of_parent->new_ocelot_menu_text_color= copy_of_parent->ocelot_menu_text_color;
  copy_of_parent->new_ocelot_menu_background_color= copy_of_parent->ocelot_menu_background_color;
  copy_of_parent->new_ocelot_menu_border_color= copy_of_parent->ocelot_menu_border_color;
  copy_of_parent->new_ocelot_menu_font_family= copy_of_parent->ocelot_menu_font_family;
  copy_of_parent->new_ocelot_menu_font_size= copy_of_parent->ocelot_menu_font_size;
  copy_of_parent->new_ocelot_menu_font_style= copy_of_parent->ocelot_menu_font_style;
  copy_of_parent->new_ocelot_menu_font_weight= copy_of_parent->ocelot_menu_font_weight;

  copy_of_parent->new_ocelot_history_text_color= copy_of_parent->ocelot_history_text_color;
  copy_of_parent->new_ocelot_history_background_color= copy_of_parent->ocelot_history_background_color;
  copy_of_parent->new_ocelot_history_border_color= copy_of_parent->ocelot_history_border_color;
  copy_of_parent->new_ocelot_history_font_family= copy_of_parent->ocelot_history_font_family;
  copy_of_parent->new_ocelot_history_font_size= copy_of_parent->ocelot_history_font_size;
  copy_of_parent->new_ocelot_history_font_style= copy_of_parent->ocelot_history_font_style;
  copy_of_parent->new_ocelot_history_font_weight= copy_of_parent->ocelot_history_font_weight;
  copy_of_parent->new_ocelot_history_max_row_count= copy_of_parent->ocelot_history_max_row_count;

  copy_of_parent->new_ocelot_grid_text_color= copy_of_parent->ocelot_grid_text_color;
  copy_of_parent->new_ocelot_grid_background_color= copy_of_parent->ocelot_grid_background_color;
  copy_of_parent->new_ocelot_grid_border_color= copy_of_parent->ocelot_grid_border_color;
  copy_of_parent->new_ocelot_grid_header_background_color= copy_of_parent->ocelot_grid_header_background_color;
  copy_of_parent->new_ocelot_grid_font_family= copy_of_parent->ocelot_grid_font_family;
  copy_of_parent->new_ocelot_grid_font_size= copy_of_parent->ocelot_grid_font_size;
  copy_of_parent->new_ocelot_grid_font_style= copy_of_parent->ocelot_grid_font_style;
  copy_of_parent->new_ocelot_grid_font_weight= copy_of_parent->ocelot_grid_font_weight;
  copy_of_parent->new_ocelot_grid_cell_border_color= copy_of_parent->ocelot_grid_cell_border_color;
  copy_of_parent->new_ocelot_grid_cell_drag_line_color= copy_of_parent->ocelot_grid_cell_drag_line_color;
  copy_of_parent->new_ocelot_grid_border_size= copy_of_parent->ocelot_grid_border_size;
  copy_of_parent->new_ocelot_grid_cell_border_size= copy_of_parent->ocelot_grid_cell_border_size;
  copy_of_parent->new_ocelot_grid_cell_drag_line_size= copy_of_parent->ocelot_grid_cell_drag_line_size;

  copy_of_parent->new_ocelot_extra_rule_1_text_color= copy_of_parent->ocelot_extra_rule_1_text_color;
  copy_of_parent->new_ocelot_extra_rule_1_background_color= copy_of_parent->ocelot_extra_rule_1_background_color;
  copy_of_parent->new_ocelot_extra_rule_1_condition= copy_of_parent->ocelot_extra_rule_1_condition;
  copy_of_parent->new_ocelot_extra_rule_1_display_as= copy_of_parent->ocelot_extra_rule_1_display_as;

  copy_of_parent->new_ocelot_statement_text_color= copy_of_parent->ocelot_statement_text_color;
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
  copy_of_parent->new_ocelot_statement_highlight_keyword_color= copy_of_parent->ocelot_statement_highlight_keyword_color;
  copy_of_parent->new_ocelot_statement_prompt_background_color= copy_of_parent->ocelot_statement_prompt_background_color;
  copy_of_parent->new_ocelot_statement_highlight_current_line_color= copy_of_parent->ocelot_statement_highlight_current_line_color;
  copy_of_parent->new_ocelot_statement_highlight_function_color= copy_of_parent->ocelot_statement_highlight_function_color;
  copy_of_parent->new_ocelot_statement_syntax_checker= copy_of_parent->ocelot_statement_syntax_checker;

  {
    QString s;
    if (current_widget == MAIN_WIDGET) s= menu_strings[menu_off + MENU_SETTINGS_FOR_MENU];
    if (current_widget == HISTORY_WIDGET) s= menu_strings[menu_off + MENU_SETTINGS_FOR_HISTORY];
    if (current_widget == GRID_WIDGET) s= menu_strings[menu_off + MENU_SETTINGS_FOR_GRID];
    if (current_widget == STATEMENT_WIDGET) s= menu_strings[menu_off + MENU_SETTINGS_FOR_STATEMENT];
    if (current_widget == EXTRA_RULE_1) s= menu_strings[menu_off + MENU_SETTINGS_FOR_EXTRA_RULE_1];
    setWindowTitle(s);                                                /* affects "this"] */
  }

  /* Hboxes for foreground, background, and highlights */
  /* Todo: following calculation should actually be width of largest tr(label) + approximately 5. */
  int label_for_color_width= this->fontMetrics().boundingRect("W").width();
  for (int ci= 0; ci < 11; ++ci)
  {
    widget_for_color[ci]= new QWidget(this);
    label_for_color[ci]= new QLabel();
    label_for_color_rgb[ci]= new QLabel();
    combo_box_for_color_pick[ci]= new QComboBox(this);

    {
      /*
        When theme = CleanLooks or GTK+, the non-editable combobox's list view has no scroll bar.
        Finding the problem and putting in this one-line solution took 20 man-hours.
      */
      combo_box_for_color_pick[ci]->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      QPixmap pixmap= QPixmap(100, 100);
      QIcon icon;
      int i_44;
      for (i_44= 0; i_44 < copy_of_parent->q_color_list.size() / 2; ++i_44)
      {
        pixmap.fill(QColor(copy_of_parent->q_color_list[(i_44 * 2) + 1]));
        icon= pixmap;
        combo_box_for_color_pick[ci]->addItem(icon, copy_of_parent->q_color_list[(i_44 * 2)]);
      }
    }

    label_for_color_show[ci]= new QLabel(this);
    set_widget_values(ci);
    label_for_color[ci]->setFixedWidth(label_for_color_width * MAX_COLOR_NAME_WIDTH);
    //label_for_color_rgb[ci]->setFixedWidth(label_for_color_width * 12);
    label_for_color_rgb[ci]->setFixedWidth(this->fontMetrics().boundingRect("LightGoldenrodYellow").width());
    //label_for_color_show[ci]->setEnabled(false);
    label_for_color_show[ci]->setMinimumWidth(40);
    hbox_layout_for_color[ci]= new QHBoxLayout();
    hbox_layout_for_color[ci]->addWidget(label_for_color[ci]);
    hbox_layout_for_color[ci]->addWidget(label_for_color_rgb[ci]);
    hbox_layout_for_color[ci]->addWidget(label_for_color_show[ci]);
    hbox_layout_for_color[ci]->addWidget(combo_box_for_color_pick[ci]);
    widget_for_color[ci]->setLayout(hbox_layout_for_color[ci]);
  }
  connect(combo_box_for_color_pick[0], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_color_pick_0(int)));
  connect(combo_box_for_color_pick[1], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_color_pick_1(int)));
  connect(combo_box_for_color_pick[2], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_color_pick_2(int)));
  connect(combo_box_for_color_pick[3], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_color_pick_3(int)));
  connect(combo_box_for_color_pick[4], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_color_pick_4(int)));
  connect(combo_box_for_color_pick[5], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_color_pick_5(int)));
  connect(combo_box_for_color_pick[6], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_color_pick_6(int)));
  connect(combo_box_for_color_pick[7], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_color_pick_7(int)));
  connect(combo_box_for_color_pick[8], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_color_pick_8(int)));
  connect(combo_box_for_color_pick[9], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_color_pick_9(int)));
  connect(combo_box_for_color_pick[10],SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_color_pick_10(int)));

  widget_font_label= new QLabel("");

  /* Hbox -- the font picker */

  widget_for_font_dialog= new QWidget(this);

  label_for_font_dialog= new QLabel(this);
  label_for_font_dialog_set_text();

  button_for_font_dialog= new QPushButton(menu_strings[menu_off + MENU_PICK_NEW_FONT], this);
  hbox_layout_for_font_dialog= new QHBoxLayout();
  hbox_layout_for_font_dialog->addWidget(label_for_font_dialog);
  hbox_layout_for_font_dialog->addWidget(button_for_font_dialog);
  widget_for_font_dialog->setLayout(hbox_layout_for_font_dialog);
  connect(button_for_font_dialog, SIGNAL(clicked()), this, SLOT(handle_button_for_font_dialog()));

  if (current_widget == STATEMENT_WIDGET)
  {
    widget_for_syntax_checker= new QWidget(this);
    label_for_syntax_checker= new QLabel(menu_strings[menu_off + MENU_SYNTAX_CHECKER]);
    combo_box_for_syntax_checker= new QComboBox();
    combo_box_for_syntax_checker->setFixedWidth(label_for_color_width * 3);
    for (int cj= 0; cj <= 3; ++cj) combo_box_for_syntax_checker->addItem(QString::number(cj));
    combo_box_for_syntax_checker->setCurrentIndex(copy_of_parent->new_ocelot_statement_syntax_checker.toInt());
    hbox_layout_for_syntax_checker= new QHBoxLayout();
    hbox_layout_for_syntax_checker->addWidget(label_for_syntax_checker);
    hbox_layout_for_syntax_checker->addWidget(combo_box_for_syntax_checker);
    widget_for_syntax_checker->setLayout(hbox_layout_for_syntax_checker);
    connect(combo_box_for_syntax_checker, SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_syntax_check(int)));
  }

  if (current_widget == HISTORY_WIDGET)
  {
    widget_for_max_row_count= new QWidget(this);
    label_for_max_row_count= new QLabel(menu_strings[menu_off + MENU_MAX_ROW_COUNT]);
    spin_box_for_max_row_count= new QSpinBox();
    spin_box_for_max_row_count->setFixedWidth(label_for_color_width * 5);
    spin_box_for_max_row_count->setMaximum(99999);
    spin_box_for_max_row_count->setMinimum(0);
    spin_box_for_max_row_count->setValue(copy_of_parent->ocelot_history_max_row_count.toInt());
    spin_box_for_max_row_count->setButtonSymbols( QAbstractSpinBox::NoButtons);
    hbox_layout_for_max_row_count= new QHBoxLayout();
    hbox_layout_for_max_row_count->addWidget(label_for_max_row_count);
    hbox_layout_for_max_row_count->addWidget(spin_box_for_max_row_count);
    widget_for_max_row_count->setLayout(hbox_layout_for_max_row_count);
    connect(spin_box_for_max_row_count, SIGNAL(valueChanged(int)), this, SLOT(handle_spin_box_for_max_row_count(int)));
  }

  if (current_widget == GRID_WIDGET)
  {
    /* int label_for_size_width= this->fontMetrics().boundingRect("W").width(); */
    for (int ci= 0; ci < 3; ++ci)
    {
      widget_for_size[ci]= new QWidget(this);
      if (ci == 0) label_for_size[ci]= new QLabel(menu_strings[menu_off + MENU_GRID_BORDER_SIZE]);
      if (ci == 1) label_for_size[ci]= new QLabel(menu_strings[menu_off + MENU_GRID_CELL_BORDER_SIZE]);
      if (ci == 2) label_for_size[ci]= new QLabel(menu_strings[menu_off + MENU_GRID_CELL_DRAG_LINE_SIZE]);
      combo_box_for_size[ci]= new QComboBox();
      combo_box_for_size[ci]->setFixedWidth(label_for_color_width * 3);
      for (int cj= 0; cj < 11; ++cj) combo_box_for_size[ci]->addItem(QString::number(cj));
      label_for_size[ci]->setFixedWidth(label_for_color_width * MAX_COLOR_NAME_WIDTH);
      if (ci == 0) combo_box_for_size[0]->setCurrentIndex(copy_of_parent->new_ocelot_grid_border_size.toInt());
      if (ci == 1) combo_box_for_size[1]->setCurrentIndex(copy_of_parent->new_ocelot_grid_cell_border_size.toInt());
      if (ci == 2) combo_box_for_size[2]->setCurrentIndex(copy_of_parent->new_ocelot_grid_cell_drag_line_size.toInt());
      hbox_layout_for_size[ci]= new QHBoxLayout();
      hbox_layout_for_size[ci]->addWidget(label_for_size[ci]);
      hbox_layout_for_size[ci]->addWidget(combo_box_for_size[ci]);
      widget_for_size[ci]->setLayout(hbox_layout_for_size[ci]);
    }

    connect(combo_box_for_size[0], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_size_0(int)));
    connect(combo_box_for_size[1], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_size_1(int)));
    connect(combo_box_for_size[2], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_size_2(int)));
    /* I could not get result grid border size to work so it is hidden until someday it is figured out -- maybe never */
    label_for_size[0]->hide();
    combo_box_for_size[0]->hide();
  }

  if (current_widget == EXTRA_RULE_1)
  {
    widget_for_size[0]= new QWidget(this);
    label_for_size[0]= new QLabel(menu_strings[menu_off + MENU_CONDITION]);
    combo_box_for_size[0]= new QComboBox();
    //combo_box_for_size[0]->setFixedWidth(label_for_color_width * 30);
/*
    I'd like to be specific, but until there's a lot of time to spare,
    let's go with just two options: LIKE '%BLOB' and LIKE '%BINARY'.
    combo_box_for_size[0]->addItem("");
    combo_box_for_size[0]->addItem("data_type = 'BIT'");
    combo_box_for_size[0]->addItem("data_type = 'TINYINT'");
    combo_box_for_size[0]->addItem("data_type = 'SMALLINT'");
    combo_box_for_size[0]->addItem("data_type = 'MEDIUMINT'");
    combo_box_for_size[0]->addItem("data_type = 'INT'");
    combo_box_for_size[0]->addItem("data_type = 'BIGINT'");
    combo_box_for_size[0]->addItem("data_type = 'DECIMAL'");
    combo_box_for_size[0]->addItem("data_type = 'NUMERIC'");
    combo_box_for_size[0]->addItem("data_type = 'FLOAT'");
    combo_box_for_size[0]->addItem("data_type = 'DOUBLE'");
    combo_box_for_size[0]->addItem("data_type = 'DATE'");
    combo_box_for_size[0]->addItem("data_type = 'DATETIME'");
    combo_box_for_size[0]->addItem("data_type = 'TIMESTAMP'");
    combo_box_for_size[0]->addItem("data_type = 'TIME'");
    combo_box_for_size[0]->addItem("data_type = 'CHAR'");
    combo_box_for_size[0]->addItem("data_type = 'VARCHAR'");
    combo_box_for_size[0]->addItem("data_type = 'BINARY'");
    combo_box_for_size[0]->addItem("data_type = 'VARBINARY'");
    combo_box_for_size[0]->addItem("data_type = 'TINYBLOB'");
    combo_box_for_size[0]->addItem("data_type = 'TINYTEXT'");
    combo_box_for_size[0]->addItem("data_type = 'BLOB'");
    combo_box_for_size[0]->addItem("data_type = 'TEXT'");
    combo_box_for_size[0]->addItem("data_type = 'MEDIUMBLOB'");
    combo_box_for_size[0]->addItem("data_type = 'MEDIUMTEXT'");
    combo_box_for_size[0]->addItem("data_type = 'LONGBLOB'");
    combo_box_for_size[0]->addItem("data_type = 'LONGTEXT'");
    combo_box_for_size[0]->addItem("data_type = 'ENUM'");
    combo_box_for_size[0]->addItem("data_type = 'SET'");
*/
    combo_box_for_size[0]->addItem("");
    combo_box_for_size[0]->addItem("data_type LIKE '%BLOB'");
    combo_box_for_size[0]->addItem("data_type LIKE '%BINARY'");
    //label_for_size[0]->setFixedWidth(label_for_color_width * 30);
    combo_box_for_size[0]->setCurrentIndex(combo_box_for_size[0]->findText(copy_of_parent->new_ocelot_extra_rule_1_condition));

    hbox_layout_for_size[0]= new QHBoxLayout();
    hbox_layout_for_size[0]->addWidget(label_for_size[0]);
    hbox_layout_for_size[0]->addWidget(combo_box_for_size[0]);
    widget_for_size[0]->setLayout(hbox_layout_for_size[0]);
    connect(combo_box_for_size[0], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_size_0(int)));
    widget_for_size[1]= new QWidget(this);
    label_for_size[1]= new QLabel(menu_strings[menu_off + MENU_DISPLAY_AS]);
    combo_box_for_size[1]= new QComboBox();
    //combo_box_for_size[1]->setFixedWidth(label_for_color_width * 30);
    combo_box_for_size[1]->addItem("char");
    combo_box_for_size[1]->addItem("image");
    //combo_box_for_size[1]->addItem("hex");
    //combo_box_for_size[1]->addItem("number");
    //label_for_size[1]->setFixedWidth(label_for_color_width * 30);
    combo_box_for_size[1]->setCurrentIndex(combo_box_for_size[1]->findText(copy_of_parent->new_ocelot_extra_rule_1_display_as));
    hbox_layout_for_size[1]= new QHBoxLayout();
    hbox_layout_for_size[1]->addWidget(label_for_size[1]);
    hbox_layout_for_size[1]->addWidget(combo_box_for_size[1]);
    widget_for_size[1]->setLayout(hbox_layout_for_size[1]);
    connect(combo_box_for_size[1], SIGNAL(currentIndexChanged(int)), this, SLOT(handle_combo_box_for_size_1(int)));
  }

  /* The Cancel and OK buttons */
  widget_3= new QWidget(this);
  button_for_cancel= new QPushButton(menu_strings[menu_off + MENU_CANCEL], this);
  button_for_ok= new QPushButton(menu_strings[menu_off + MENU_OK], this);
  /* I have no idea why SLOT(accept() and SLOT(reject() cause crashes. The crashes can be worked around. */
  connect(button_for_ok, SIGNAL(clicked()), this, SLOT(handle_button_for_ok()));
  connect(button_for_cancel, SIGNAL(clicked()), this, SLOT(handle_button_for_cancel()));
  hbox_layout_3= new QHBoxLayout();
  hbox_layout_3->addWidget(button_for_cancel);
  hbox_layout_3->addWidget(button_for_ok);
  widget_3->setLayout(hbox_layout_3);

  /* Put the HBoxes in a VBox */
  main_layout= new QVBoxLayout();
  for (int ci= 0; ci < 11; ++ci) main_layout->addWidget(widget_for_color[ci]);
  main_layout->addWidget(widget_font_label);
  main_layout->addWidget(widget_for_font_dialog);
  if (current_widget == STATEMENT_WIDGET) main_layout->addWidget(widget_for_syntax_checker);
  if (current_widget == HISTORY_WIDGET) main_layout->addWidget(widget_for_max_row_count);
  if (current_widget == GRID_WIDGET) for (int ci= 0; ci < 3; ++ci) main_layout->addWidget(widget_for_size[ci]);
  if (current_widget == EXTRA_RULE_1) main_layout->addWidget(widget_for_size[0]);
  if (current_widget == EXTRA_RULE_1) main_layout->addWidget(widget_for_size[1]);
  main_layout->addWidget(widget_3);

  handle_combo_box_1(current_widget);
  /*
    If one merely says
    this->setLayout(main_layout);
    that almost always works because the dialog box fits within the
    typical screen area. But if it doesn't, e.g. 640x480 display, we need a scroll bar.
    Additional problem: sizeHint() seems to work for horizontal but not for vertical,
    which is why the setMinimumHeight line exists.
  */
  this->setLayout(main_layout);
  main_layout->activate();
  settings_width= this->width() + 20; /* todo: 20 is arbitrary */
  settings_height= this->height() + 20; /* todo: 20 is arbitrary */
  QWidget *widget_with_main_layout= new QWidget();
  widget_with_main_layout->setLayout(main_layout);
  QScrollAreaWithSize *scroll_area= new QScrollAreaWithSize(settings_width, settings_height);
  scroll_area->setWidget(widget_with_main_layout);
  QHBoxLayout *scroll_area_layout= new QHBoxLayout();
  scroll_area_layout->addWidget(widget_with_main_layout);
  scroll_area->setLayout(scroll_area_layout);
  scroll_area->setWidgetResizable(true);
  QHBoxLayout *upper_layout= new QHBoxLayout();
  upper_layout->addWidget(scroll_area);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  //if (parent->main_window_maximum_width > settings_width) setMinimumWidth(settings_width);
  if (parent->main_window_maximum_height > settings_height) setMinimumHeight(settings_height);
  setLayout(upper_layout);
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
    case 0: { color_type= menu_strings[menu_off + MENU_STATEMENT_TEXT_COLOR];
              color_name= copy_of_parent->new_ocelot_statement_text_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 1: { color_type= menu_strings[menu_off + MENU_STATEMENT_BACKGROUND_COLOR];
              color_name= copy_of_parent->new_ocelot_statement_background_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 2: { color_type= menu_strings[menu_off + MENU_STATEMENT_HIGHLIGHT_LITERAL_COLOR];
              color_name= copy_of_parent->new_ocelot_statement_highlight_literal_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 3: { color_type= menu_strings[menu_off + MENU_STATEMENT_HIGHLIGHT_IDENTIFIER_COLOR];
              color_name= copy_of_parent->new_ocelot_statement_highlight_identifier_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 4: { color_type= menu_strings[menu_off + MENU_STATEMENT_HIGHLIGHT_COMMENT_COLOR];
              color_name= copy_of_parent->new_ocelot_statement_highlight_comment_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 5: { color_type= menu_strings[menu_off + MENU_STATEMENT_HIGHLIGHT_OPERATOR_COLOR];
              color_name= copy_of_parent->new_ocelot_statement_highlight_operator_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 6: { color_type= menu_strings[menu_off + MENU_STATEMENT_HIGHLIGHT_KEYWORD_COLOR];
              color_name= copy_of_parent->new_ocelot_statement_highlight_keyword_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 7: { color_type= menu_strings[menu_off + MENU_STATEMENT_PROMPT_BACKGROUND_COLOR];
              color_name= copy_of_parent->new_ocelot_statement_prompt_background_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 8: { color_type= menu_strings[menu_off + MENU_STATEMENT_BORDER_COLOR];
              color_name= copy_of_parent->new_ocelot_statement_border_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 9: { color_type= menu_strings[menu_off + MENU_STATEMENT_HIGHLIGHT_CURRENT_LINE_COLOR];
              color_name= copy_of_parent->new_ocelot_statement_highlight_current_line_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 10:{ color_type= menu_strings[menu_off + MENU_STATEMENT_HIGHLIGHT_FUNCTION_COLOR];
              color_name= copy_of_parent->new_ocelot_statement_highlight_function_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    }
  }
  if (current_widget == GRID_WIDGET)
  {
    switch (ci)
    {
    case 0: { color_type= menu_strings[menu_off + MENU_GRID_TEXT_COLOR];
              color_name= copy_of_parent->new_ocelot_grid_text_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 1: { color_type= menu_strings[menu_off + MENU_GRID_BACKGROUND_COLOR];
              color_name= copy_of_parent->new_ocelot_grid_background_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 2: { color_type= menu_strings[menu_off + MENU_GRID_CELL_BORDER_COLOR];
              color_name= copy_of_parent->new_ocelot_grid_cell_border_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 3: { color_type= menu_strings[menu_off + MENU_GRID_CELL_DRAG_LINE_COLOR];
              color_name= copy_of_parent->new_ocelot_grid_cell_drag_line_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 7: { color_type= menu_strings[menu_off + MENU_GRID_HEADER_BACKGROUND_COLOR];
              color_name= copy_of_parent->new_ocelot_grid_header_background_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 8: { color_type= menu_strings[menu_off + MENU_GRID_BORDER_COLOR];
              color_name= copy_of_parent->new_ocelot_grid_border_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    }
  }
  if (current_widget == EXTRA_RULE_1)
  {
    switch (ci)
    {
    case 0: { color_type= menu_strings[menu_off + MENU_GRID_TEXT_COLOR];
              color_name= copy_of_parent->new_ocelot_extra_rule_1_text_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 1: { color_type= menu_strings[menu_off + MENU_GRID_BACKGROUND_COLOR];
              color_name= copy_of_parent->new_ocelot_extra_rule_1_background_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    }
  }
  if (current_widget == HISTORY_WIDGET)
  {
    switch (ci)
    {
    case 0: { color_type= menu_strings[menu_off + MENU_HISTORY_TEXT_COLOR];
              color_name= copy_of_parent->new_ocelot_history_text_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 1: { color_type= menu_strings[menu_off + MENU_HISTORY_BACKGROUND_COLOR];
              color_name= copy_of_parent->new_ocelot_history_background_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 8: { color_type= menu_strings[menu_off + MENU_HISTORY_BORDER_COLOR];
              color_name= copy_of_parent->new_ocelot_history_border_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    }
  }
  if (current_widget == MAIN_WIDGET)
  {
    switch (ci)
    {
    case 0: { color_type= menu_strings[menu_off + MENU_MENU_TEXT_COLOR];
              color_name= copy_of_parent->new_ocelot_menu_text_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 1: { color_type= menu_strings[menu_off + MENU_MENU_BACKGROUND_COLOR];
              color_name= copy_of_parent->new_ocelot_menu_background_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    case 8: { color_type= menu_strings[menu_off + MENU_MENU_BORDER_COLOR];
              color_name= copy_of_parent->new_ocelot_menu_border_color;
              color_name= copy_of_parent->canonical_color_name(color_name);
              break; }
    }
  }
  label_for_color[ci]->setText(color_type);
  //label_for_color_rgb[ci]->setText(color_name);
  int cli;
  /* Todo: This is a very roundabout way to get the color index. */
  cli= q_color_list_index(copy_of_parent->rgb_to_color(color_name));
  combo_box_for_color_pick[ci]->setCurrentIndex(cli);
  label_for_color_rgb[ci]->setText(combo_box_for_color_pick[ci]->currentText());
  QString sss= "border: 1px solid black; background-color: ";
  sss.append(copy_of_parent->qt_color(color_name));
  label_for_color_show[ci]->setStyleSheet(sss);
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

  for (ci= 0; ci < 11; ++ci) set_widget_values(ci);
  if (i == STATEMENT_WIDGET)
  {
    color_type= menu_strings[menu_off + MENU_STATEMENT_PROMPT_BACKGROUND_COLOR]; /* necessary even though set_widget_values() does it */
    label_for_color[7]->setText(color_type);
    for (ci= 2; ci < 11 ; ++ci)
    {
      label_for_color[ci]->show();
      label_for_color_rgb[ci]->show();
      label_for_color_show[ci]->show();
      combo_box_for_color_pick[ci]->show();
    }
  }

  if (i == GRID_WIDGET)
  {
    color_type= menu_strings[menu_off + MENU_GRID_HEADER_BACKGROUND_COLOR];  /* necessary even though set_widget_values() does it */
    label_for_color[7]->setText(color_type);
    for (ci= 4; ci < 7; ++ci)
    {
      label_for_color[ci]->hide();
      label_for_color_rgb[ci]->hide();
      label_for_color_show[ci]->hide();
      combo_box_for_color_pick[ci]->hide();
    }
    for (ci= 7; ci < 8; ++ci)
    {
      label_for_color[ci]->show();
      label_for_color_rgb[ci]->show();
      label_for_color_show[ci]->show();
      combo_box_for_color_pick[ci]->show();
    }
    label_for_color[9]->hide();
    label_for_color_rgb[9]->hide();
    label_for_color_show[9]->hide();
    combo_box_for_color_pick[9]->hide();
    label_for_color[10]->hide();
    label_for_color_rgb[10]->hide();
    label_for_color_show[10]->hide();
    combo_box_for_color_pick[10]->hide();
  }
  if (i == EXTRA_RULE_1)
  {
    for (ci= 2; ci < 11; ++ci)
    {
      label_for_color[ci]->hide();
      label_for_color_rgb[ci]->hide();
      label_for_color_show[ci]->hide();
      combo_box_for_color_pick[ci]->hide();
    }
    widget_font_label->hide();
    label_for_font_dialog->hide();
    button_for_font_dialog->hide();
  }
  if ((i == HISTORY_WIDGET) || (i == MAIN_WIDGET))
  {
    for (ci= 2; ci < 8; ++ci)
    {
      label_for_color[ci]->hide();
      label_for_color_rgb[ci]->hide();
      label_for_color_show[ci]->hide();
      combo_box_for_color_pick[ci]->hide();
    }
    label_for_color[9]->hide();
    label_for_color_rgb[9]->hide();
    label_for_color_show[9]->hide();
    combo_box_for_color_pick[9]->hide();
    label_for_color[10]->hide();
    label_for_color_rgb[10]->hide();
    label_for_color_show[10]->hide();
    combo_box_for_color_pick[10]->hide();
  }
}


void label_for_font_dialog_set_text()
{
  QString s_for_label_for_font_dialog= menu_strings[menu_off + MENU_FONT];
  if (current_widget == STATEMENT_WIDGET)
  {
    s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_statement_font_family);
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
    s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_grid_font_family);
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
    s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_history_font_family);
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
    s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_menu_font_family);
    s_for_label_for_font_dialog.append(" ");
    s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_menu_font_size);
    s_for_label_for_font_dialog.append("pt");
    if (QString::compare(copy_of_parent->new_ocelot_menu_font_weight, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_menu_font_weight);
    }
    if (QString::compare(copy_of_parent->new_ocelot_menu_font_style, "normal") != 0)
    {
      s_for_label_for_font_dialog.append(" ");
      s_for_label_for_font_dialog.append(copy_of_parent->new_ocelot_menu_font_style);
    }
  }

  label_for_font_dialog->setText(s_for_label_for_font_dialog);
}


private slots:

/* If user clicks OK, end. The caller will move changed "new" settings to non-new. */
void handle_button_for_ok()
{
  done(QDialog::Accepted); /* i.e. close() but return Accepted */
}


/* If user clicks Cancel, don't do anything, and end. */
void handle_button_for_cancel()
{
  done(QDialog::Rejected); /* i.e. close() but return Rejected */
}


/*
  For all the handle_combo_box_for_color_pick slots:
  We no longer use QColorDialog dialog box, instead there's a QComboBox.
  Why I didn't like QColorDialog ...
    For the "name:" control: if I typed in SkyBlue, it echoed as rrggbb,
    if I typed in Silver, it did nothing, and there was no clear list
    of what color names were available. For the other controls: I thought
    brightness/hue etc., and the color wheel, would put off intimidate users
    since they're not obvious and I expect typical users want names
    rather than numbers.
  The QComboBox for color picking gets around those problems but ...
  Todo: Make sure garbage_collect is working
  Todo: The Settings menu item  itself does not have the font or colors declared by settings menu
  Todo: There surely isn't any need for the blank lines -- so it's not a matter of "hide()", you just don't create
  Todo: You need to look harder at what Tab settings are (it's hard to see what the focus is for the combobox items
*/
void handle_combo_box_for_color_pick_0(int item_number)
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[0]->itemText(item_number);
    copy_of_parent->new_ocelot_statement_text_color= new_color;
    label_for_color_rgb[0]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[0]->setStyleSheet(s);
  }
  if (current_widget == GRID_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[0]->itemText(item_number);
    copy_of_parent->new_ocelot_grid_text_color= new_color;
    label_for_color_rgb[0]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[0]->setStyleSheet(s);
  }
  if (current_widget == EXTRA_RULE_1)
  {
    QString new_color= combo_box_for_color_pick[0]->itemText(item_number);
    copy_of_parent->new_ocelot_extra_rule_1_text_color= new_color;
    label_for_color_rgb[0]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[0]->setStyleSheet(s);
  }
  if (current_widget == HISTORY_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[0]->itemText(item_number);
    copy_of_parent->new_ocelot_history_text_color= new_color;
    label_for_color_rgb[0]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[0]->setStyleSheet(s);
  }
  if (current_widget == MAIN_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[0]->itemText(item_number);
    copy_of_parent->new_ocelot_menu_text_color= new_color;
    label_for_color_rgb[0]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[0]->setStyleSheet(s);
  }
}


void handle_combo_box_for_color_pick_1(int item_number)
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[1]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_statement_background_color= new_color;
    label_for_color_rgb[1]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[1]->setStyleSheet(s);
  }
  if (current_widget == GRID_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[1]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_grid_background_color= new_color;
    label_for_color_rgb[1]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[1]->setStyleSheet(s);
  }
  if (current_widget == EXTRA_RULE_1)
  {
    QString new_color= combo_box_for_color_pick[1]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_extra_rule_1_background_color= new_color;
    label_for_color_rgb[1]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[1]->setStyleSheet(s);
  }
  if (current_widget == HISTORY_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[1]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_history_background_color= new_color;
    label_for_color_rgb[1]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[1]->setStyleSheet(s);
  }
  if (current_widget == MAIN_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[1]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_menu_background_color= new_color;
    label_for_color_rgb[1]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[1]->setStyleSheet(s);
  }
}

void handle_combo_box_for_color_pick_2(int item_number)
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[2]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_statement_highlight_literal_color= new_color;
    label_for_color_rgb[2]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[2]->setStyleSheet(s);
  }

  if (current_widget == GRID_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[2]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_grid_cell_border_color= new_color;
    label_for_color_rgb[2]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[2]->setStyleSheet(s);
  }
}


void handle_combo_box_for_color_pick_3(int item_number)
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[3]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_statement_highlight_identifier_color= new_color;
    label_for_color_rgb[3]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[3]->setStyleSheet(s);
  }

  if (current_widget == GRID_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[3]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_grid_cell_drag_line_color= new_color;
    label_for_color_rgb[3]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[3]->setStyleSheet(s);
  }
}


void handle_combo_box_for_color_pick_4(int item_number)
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[4]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_statement_highlight_comment_color= new_color;
    label_for_color_rgb[4]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[4]->setStyleSheet(s);
  }
}


void handle_combo_box_for_color_pick_5(int item_number)
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[5]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_statement_highlight_operator_color= new_color;
    label_for_color_rgb[5]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[5]->setStyleSheet(s);
  }
}


void handle_combo_box_for_color_pick_6(int item_number)
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[6]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_statement_highlight_keyword_color= new_color;
    label_for_color_rgb[6]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[6]->setStyleSheet(s);
  }
}


void handle_combo_box_for_color_pick_7(int item_number)
{
 if (current_widget == STATEMENT_WIDGET)
 {
   QString new_color= combo_box_for_color_pick[7]->itemText(item_number);
   new_color= copy_of_parent->canonical_color_name(new_color);
   copy_of_parent->new_ocelot_statement_prompt_background_color= new_color;
   label_for_color_rgb[7]->setText(copy_of_parent->rgb_to_color(new_color));
   QString s= "border: 1px solid black; background-color: ";
   s.append(copy_of_parent->qt_color(new_color));
   label_for_color_show[7]->setStyleSheet(s);
  }
  if (current_widget == GRID_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[7]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_grid_header_background_color= new_color;
    label_for_color_rgb[7]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[7]->setStyleSheet(s);
  }
}


void handle_combo_box_for_color_pick_8(int item_number)
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[8]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_statement_border_color= new_color;
    label_for_color_rgb[8]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[8]->setStyleSheet(s);
  }
  if (current_widget == GRID_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[8]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_grid_border_color= new_color;
    label_for_color_rgb[8]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[8]->setStyleSheet(s);
  }
  if (current_widget == HISTORY_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[8]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_history_border_color= new_color;
    label_for_color_rgb[8]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[8]->setStyleSheet(s);
  }
  if (current_widget == MAIN_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[8]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_menu_border_color= new_color;
    label_for_color_rgb[8]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[8]->setStyleSheet(s);
  }
}


void handle_combo_box_for_color_pick_9(int item_number)
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[9]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_statement_highlight_current_line_color= new_color;
    label_for_color_rgb[9]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[9]->setStyleSheet(s);
  }
}


void handle_combo_box_for_color_pick_10(int item_number)
{
  if (current_widget == STATEMENT_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[10]->itemText(item_number);
    new_color= copy_of_parent->canonical_color_name(new_color);
    copy_of_parent->new_ocelot_statement_highlight_function_color= new_color;
    label_for_color_rgb[10]->setText(copy_of_parent->rgb_to_color(new_color));
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[10]->setStyleSheet(s);
  }
}


void handle_combo_box_for_syntax_check(int i)
{
  if (current_widget == STATEMENT_WIDGET)
    copy_of_parent->new_ocelot_statement_syntax_checker= QString::number(i);
}


void handle_spin_box_for_max_row_count(int i)
{
  if (current_widget == HISTORY_WIDGET)
    copy_of_parent->new_ocelot_history_max_row_count= QString::number(i);
}


void handle_combo_box_for_size_0(int i)
{
  if (current_widget == GRID_WIDGET)
    copy_of_parent->new_ocelot_grid_border_size= QString::number(i);
  if (current_widget == EXTRA_RULE_1)
    copy_of_parent->new_ocelot_extra_rule_1_condition= combo_box_for_size[0]->itemText(i);
}


void handle_combo_box_for_size_1(int i)
{
  if (current_widget == GRID_WIDGET)
    copy_of_parent->new_ocelot_grid_cell_border_size= QString::number(i);
  if (current_widget == EXTRA_RULE_1)
    copy_of_parent->new_ocelot_extra_rule_1_display_as= combo_box_for_size[1]->itemText(i);
}


void handle_combo_box_for_size_2(int i)
{
  copy_of_parent->new_ocelot_grid_cell_drag_line_size= QString::number(i);
}


/* Some of the code in handle_button_for_font_dialog() is a near-duplicate of code in set_colors_and_fonts(). */
/* This doesn't look for font_weight=light|demibold|black since QFontDialog ignores them anyway. */
/* This passes italic==true but QFontDialog ignores that if it's expecting an oblique font. */
/* Possible bug: with Qt4, even when font style="normal", dialog box appears showing italic. */
void handle_button_for_font_dialog()
{
  bool ok;
  QString font_name;
  QFont font;
  int boldness= QFont::Normal;
  bool italics= false;

  if (current_widget == STATEMENT_WIDGET)
  {
    font_name= copy_of_parent->connect_stripper(copy_of_parent->new_ocelot_statement_font_family, false);
    if (QString::compare(copy_of_parent->new_ocelot_statement_font_weight, "bold", Qt::CaseInsensitive) == 0) boldness= QFont::Bold;
    if (QString::compare(copy_of_parent->new_ocelot_statement_font_style, "italic", Qt::CaseInsensitive) == 0) italics= true;
    font= QFontDialog::getFont(&ok, QFont(copy_of_parent->new_ocelot_statement_font_family, copy_of_parent->new_ocelot_statement_font_size.toInt(), boldness, italics), this);
  }
  if (current_widget == GRID_WIDGET)
  {
    font_name= copy_of_parent->connect_stripper(copy_of_parent->new_ocelot_grid_font_family, false);
    if (QString::compare(copy_of_parent->new_ocelot_grid_font_weight, "bold", Qt::CaseInsensitive) == 0) boldness= QFont::Bold;
    if (QString::compare(copy_of_parent->new_ocelot_grid_font_style, "italic", Qt::CaseInsensitive) == 0) italics= true;
    font= QFontDialog::getFont(&ok, QFont(font_name, copy_of_parent->new_ocelot_grid_font_size.toInt(), boldness, italics), this);
  }
  if (current_widget == HISTORY_WIDGET)
  {
    font_name= copy_of_parent->connect_stripper(copy_of_parent->new_ocelot_history_font_family, false);
    if (QString::compare(copy_of_parent->new_ocelot_history_font_weight, "bold", Qt::CaseInsensitive) == 0) boldness= QFont::Bold;
    if (QString::compare(copy_of_parent->new_ocelot_history_font_style, "italic", Qt::CaseInsensitive) == 0) italics= true;
    font= QFontDialog::getFont(&ok, QFont(font_name, copy_of_parent->new_ocelot_history_font_size.toInt(), boldness, italics), this);
  }
  if (current_widget == MAIN_WIDGET)
  {
    font_name= copy_of_parent->connect_stripper(copy_of_parent->new_ocelot_menu_font_family, false);
    if (QString::compare(copy_of_parent->new_ocelot_menu_font_weight, "bold", Qt::CaseInsensitive) == 0) boldness= QFont::Bold;
    if (QString::compare(copy_of_parent->new_ocelot_menu_font_style, "italic", Qt::CaseInsensitive) == 0) italics= true;
    font= QFontDialog::getFont(&ok, QFont(font_name, copy_of_parent->new_ocelot_menu_font_size.toInt(), boldness, italics), this);
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
     copy_of_parent->new_ocelot_menu_font_family= font.family();
     if (font.italic()) copy_of_parent->new_ocelot_menu_font_style= "italic";
     else copy_of_parent->new_ocelot_menu_font_style= "normal";
     copy_of_parent->new_ocelot_menu_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
     if (font.weight() >= QFont::Bold) copy_of_parent->new_ocelot_menu_font_weight= "bold";
     else copy_of_parent->new_ocelot_menu_font_weight= "normal";
    }
    label_for_font_dialog_set_text();
  }
}

/*
  Given color name, return index.
  Todo: It can be tricky, if color name isn't in the current language.
*/
int q_color_list_index(QString color_name_string)
{
//  char color_name_string_as_utf8[64];
//  int color_name_string_len;

//  color_name_string_len= color_name_string.size();             /* See comment "UTF8 Conversion" */
//  memcpy(color_name_string_as_utf8, color_name_string.toUtf8().constData(), color_name_string_len);
//  color_name_string_as_utf8[color_name_string_len]= '\0';

  for (int i= 0; i < copy_of_parent->q_color_list.size(); i+= 2)
  {
    if (QString::compare(color_name_string, copy_of_parent->q_color_list[i], Qt::CaseInsensitive) == 0) return i / 2;
    if (QString::compare(color_name_string, copy_of_parent->q_color_list[i + 1], Qt::CaseInsensitive) == 0) return i / 2;
  }
  //return (i - 1) / 2;
  return 0;  /* TEST! */
}

};
#endif // SETTINGS_H


/* QThread::msleep is protected in qt 4.8. so you have to say QThread48::msleep */
#ifndef QTHREAD48_H
#define QTHREAD48_H
class QThread48 : public QThread
{
public:
  static void msleep(int ms)
  {
    QThread::msleep(ms);
  }
};
#endif // QTHREAD48_H

/* QTabWidget:tabBar is protected in qt 4.8. so you have to say QTabWidget48::tabBar */

#ifndef QTABWIDGET48_H
#define QTABWIDGET48_H

class QTabWidget48 : public QTabWidget
{
public:
  QTabWidget48(QWidget *w = 0) : QTabWidget(w){}

public:
  QTabBar *tabBar() const
  {
    return QTabWidget::tabBar();
  }
};

#endif // QTABWIDGET48_H

/* THE PARSEPOP WIDGET */

//#ifndef QPARSEPOP_H
//#define QPARSEPOP_H
//class QPopper : public QTextEdit
//{
//
//public:
//  int settings_width, settings_height;
//
//QParsePop(int width, int height)
//{
//  settings_width= width;
//  settings_height= height;
//}
//
//virtual QSize sizeHint() const
//{
//  return QSize(settings_width, settings_height);
//}
//
//};
//#endif // QPARSEPOP_H

/*****************************************************************************************************************************/
/* THE TEXTEDITHISTORY WIDGET */

/* Subclass of QTextEditWidget used for history_edit_widget */

/*
   Events That Go To Detached Windows
   When we detach a widget by calling setWindowFlags(Qt::Window),
   it becomes independent so relevant events no longer go through
   MainWindow::eventFilter. Therefore each has its own event filter,
   to redirect events to MainWindow::eventFilter.
*/

/* Todo:
   It's unfortunate that a click on the main menu changes the focus.
   Several attempts have been made to fix this, e.g.
   the detached widget gets its own copy of the menu,
   focus-change events are intercepted and ignored.
   But nothing went well.
*/

#ifndef TEXTEDITHISTORY_H
#define TEXTEDITHISTORY_H

class TextEditHistory : public QTextEdit
{
  Q_OBJECT

public:

  MainWindow *main_window;

  TextEditHistory(MainWindow *parent) : QTextEdit(parent)
  {
    main_window= parent;
  }

  /* Following line caused an incomprehensible error. Removed temporarily. */
  //~TextEditHistory();

public:
void detach_start()
{
  installEventFilter(this);
}
void detach_stop()
{
  removeEventFilter(this);
}

bool eventFilter(QObject *obj, QEvent *event)
{
  return main_window->eventfilter_function(obj, event);
}

};
#endif // TEXTEDITHISTORY_H

#endif // OCELOTGUI_H

