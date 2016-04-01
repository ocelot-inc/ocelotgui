/* Copyright (c) 2014 by Ocelot Computer Services Inc. All rights reserved.

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

/*
  The possible DBMS values.
  DBMS_MYSQL must always be defined and --dbms='mysql' is default.
  If DBMS_MARIADB is defined and --dbms='mariadb',
    accept existence of MariaDB 10.1 roles and compound statements.
  If DBMS_TARANTOOL is defined and --dbms='tarantool',
    connection to a Tarantool server not a MySQL/MariaDB server.
  #ifdef DBMS_TARANTOOL" was an aborted experiment, ordinarily this #define should be commented out.
*/
#define DBMS_MYSQL 1
#define DBMS_MARIADB 2
//#define DBMS_TARANTOOL 3

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

/* Several possible include paths for mysql.h are hard coded in ocelotgui.pro. */
#include <mysql.h>

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
  struct tnt_stream {
          int alloc; /*!< Allocation mark */
          ssize_t (*write)(struct tnt_stream *s, const char *buf, size_t size); /*!< write to buffer function */
          ssize_t (*writev)(struct tnt_stream *s, struct iovec *iov, int count); /*!< writev function */
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
#endif

/* Flags used for row_form_box. NUM_FLAG is also defined in mysql include, with same value. */
#define READONLY_FLAG 1
#define NUM_FLAG 32768

/*
  Most ocelot_ variables are in ocelotgui.cpp but if one is required by ocelotgui.h, say so here.
  Weirdly, ocelotgui.h is included in two places, ocelotgui.pro + ocelotgui.cpp,
  so say 'static' if you do that.
*/

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
  QString ocelot_statement_syntax_checker, new_ocelot_statement_syntax_checker;
  QString ocelot_statement_style_string;
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
  QString ocelot_menu_text_color, new_ocelot_menu_text_color;
  QString ocelot_menu_background_color, new_ocelot_menu_background_color;
  QString ocelot_menu_border_color, new_ocelot_menu_border_color;
  QString ocelot_menu_font_family, new_ocelot_menu_font_family;
  QString ocelot_menu_font_size, new_ocelot_menu_font_size;
  QString ocelot_menu_font_style, new_ocelot_menu_font_style;
  QString ocelot_menu_font_weight, new_ocelot_menu_font_weight;
  QString ocelot_menu_style_string;

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
  void get_sql_mode(int who_is_calling, QString text);

  void hparse_f_nexttoken();
  void hparse_f_next_nexttoken();
  void hparse_f_error();
  bool hparse_f_is_equal(QString,QString);
  int hparse_f_accept(int,QString);
  int hparse_f_acceptn(int,QString,int);
  int hparse_f_expect(int,QString);
  int hparse_f_literal();
  int hparse_f_default(int);
  int hparse_f_user_name();
  int hparse_f_character_set_name();
  int hparse_f_collation_name();
  int hparse_f_qualified_name();
  int hparse_f_qualified_name_with_star();
  int hparse_f_table_references();
  void hparse_f_table_escaped_table_reference();
  int hparse_f_table_reference(int);
  int hparse_f_table_factor();
  int hparse_f_table_join_table();
  int hparse_f_table_join_condition();
  void hparse_f_table_index_hint_list();
  int hparse_f_table_index_hint();
  int hparse_f_table_index_list();
  void hparse_f_opr_1(),hparse_f_opr_2(),hparse_f_opr_3(),hparse_f_opr_4(),hparse_f_opr_5(),hparse_f_opr_6();
  void hparse_f_opr_7(),hparse_f_opr_8(),hparse_f_opr_9(),hparse_f_opr_10(),hparse_f_opr_11(),hparse_f_opr_12();
  void hparse_f_opr_13(),hparse_f_opr_14(),hparse_f_opr_15(),hparse_f_opr_16(),hparse_f_opr_17(),hparse_f_opr_18();
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
  int hparse_f_explainable_statement();
  void hparse_f_statement();
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
  void hparse_f_column_list(int);
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
  void hparse_f_alter_or_create_server(int);
  void hparse_f_require(int,bool,bool);
  void hparse_f_user_specification_list();
  void hparse_f_create_database();
  void hparse_f_index_columns(int,bool,bool);
  void hparse_f_alter_or_create_view();
  int hparse_f_analyze_or_optimize(int);
  void hparse_f_call();
  void hparse_f_commit_or_rollback();
  void hparse_f_explain_or_describe();
  void hparse_f_grant_or_revoke(int,bool*);
  void hparse_f_insert_or_replace();
  void hparse_f_condition_information_item_name();
  int hparse_f_signal_or_resignal(int);
  int hparse_f_into();
  int hparse_f_select(bool);
  void hparse_f_where();
  void hparse_f_order_by();
  void hparse_f_limit(int);
  void hparse_f_block(int);
  void msgBoxClosed(QAbstractButton*);
  void hparse_f_multi_block(QString text);
  int hparse_f_backslash_command(bool);
  void hparse_f_other(int);
  int hparse_f_client_statement();
  void hparse_f_parse_hint_line_create();

#ifdef DBMS_TARANTOOL
  void parse_f_nextsym();
  void parse_f_error();
  int parse_f_accept(QString);
  int parse_f_expect(QString);
  void parse_f_factor();
  void parse_f_term();
  void parse_f_expression();
  void parse_f_restricted_expression();
  void parse_f_indexed_condition();
  void parse_f_statement();
  void parse_f_assignment();
  void parse_f_block();
  void parse_f_program(QString text);
#endif
#ifdef DBMS_TARANTOOL
  long unsigned int tarantool_num_rows();
  unsigned int tarantool_num_fields();
  int tarantool_num_fields_recursive(const char **tarantool_tnt_reply_data,
                                     char *field_name,
                                     int field_number_within_array,
                                     QString *field_name_list);
  void tarantool_scan_rows(unsigned int p_result_column_count,
                 unsigned int p_result_row_count,
                 MYSQL_RES *p_mysql_res,
                 char **p_result_set_copy,
                 char ***p_result_set_copy_rows,
                 unsigned int **p_result_max_column_widths);
  void tarantool_scan_field_names(
                 const char *which_field,
                 unsigned int p_result_column_count,
                 char **p_result_field_names);
#endif

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

  void create_menu();
  int rehash_scan();
  QString rehash_search(char *search_string);
  void widget_sizer();
  QString get_delimiter(QString,QString,int);
  int execute_client_statement(QString text, int *additional_result);
  void put_diagnostics_in_result();
  void put_message_in_result(QString);
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
  void set_current_colors_and_font();
  void make_style_strings();
  //void create_the_manual_widget();
  int get_next_statement_in_string(int passed_main_token_number, int *returned_begin_count, bool);
  int make_statement_ready_to_send(QString, char *, int);
  void remove_statement(QString);
  void action_execute_one_statement(QString text);

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

  enum {MAX_TOKENS= 10000 };                  /* Todo: shouldn't be fixed */

  /* main_token_flags[] values. so far there are only three but we expect there will be more. */
  #define TOKEN_FLAG_IS_RESERVED 1
  #define TOKEN_FLAG_IS_BLOCK_END 2
  #define TOKEN_FLAG_IS_ERROR 4

  enum {                                      /* possible returns from token_type() */
    TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE= 1, /* starts with ' or N' or X' or B' */
    TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE= 2, /* starts with " */
    TOKEN_TYPE_LITERAL_WITH_DIGIT= 3,        /* starts with 0-9 */
    TOKEN_TYPE_LITERAL_WITH_BRACE= 4,        /* starts with { */ /* obsolete? */
    TOKEN_TYPE_LITERAL= 4,
    TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK= 5,  /* starts with ` */
    TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE= 6, /* starts with " and hparse_ansi_quote=true */
    TOKEN_TYPE_IDENTIFIER_WITH_AT= 7,        /* starts with @ */
    TOKEN_TYPE_IDENTIFIER= 7,
    TOKEN_TYPE_COMMENT_WITH_SLASH = 8,        /* starts with / * or * / */
    TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE= 9,   /* starts with # */
    TOKEN_TYPE_COMMENT_WITH_MINUS= 10,        /* starts with -- */
    TOKEN_TYPE_OPERATOR= 11,                 /* starts with < > = ! etc. */
    TOKEN_TYPE_OTHER= 12,                    /* identifier? keyword? */
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
    TOKEN_KEYWORD_CHARSET= TOKEN_KEYWORD_CHARACTER + 1,
    TOKEN_KEYWORD_CHECK= TOKEN_KEYWORD_CHARSET + 1,
    TOKEN_KEYWORD_CLEAR= TOKEN_KEYWORD_CHECK + 1,
    TOKEN_KEYWORD_COLLATE= TOKEN_KEYWORD_CLEAR + 1,
    TOKEN_KEYWORD_COLUMN= TOKEN_KEYWORD_COLLATE + 1,
    TOKEN_KEYWORD_CONDITION= TOKEN_KEYWORD_COLUMN + 1,
    TOKEN_KEYWORD_CONNECT= TOKEN_KEYWORD_CONDITION + 1,
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
    TOKEN_KEYWORD_DELIMITER= TOKEN_KEYWORD_DELETE + 1,
    TOKEN_KEYWORD_DESC= TOKEN_KEYWORD_DELIMITER + 1,
    TOKEN_KEYWORD_DESCRIBE= TOKEN_KEYWORD_DESC + 1,
    TOKEN_KEYWORD_DETERMINISTIC= TOKEN_KEYWORD_DESCRIBE + 1,
    TOKEN_KEYWORD_DISTINCT= TOKEN_KEYWORD_DETERMINISTIC + 1,
    TOKEN_KEYWORD_DISTINCTROW= TOKEN_KEYWORD_DISTINCT + 1,
    TOKEN_KEYWORD_DIV= TOKEN_KEYWORD_DISTINCTROW + 1,
    TOKEN_KEYWORD_DO= TOKEN_KEYWORD_DIV + 1,
    TOKEN_KEYWORD_DOUBLE= TOKEN_KEYWORD_DO + 1,
    TOKEN_KEYWORD_DROP= TOKEN_KEYWORD_DOUBLE + 1,
    TOKEN_KEYWORD_DUAL= TOKEN_KEYWORD_DROP + 1,
    TOKEN_KEYWORD_EACH= TOKEN_KEYWORD_DUAL + 1,
    TOKEN_KEYWORD_EDIT= TOKEN_KEYWORD_EACH + 1,
    TOKEN_KEYWORD_EGO= TOKEN_KEYWORD_EDIT + 1,
    TOKEN_KEYWORD_ELSE= TOKEN_KEYWORD_EGO + 1,
    TOKEN_KEYWORD_ELSEIF= TOKEN_KEYWORD_ELSE + 1,
    TOKEN_KEYWORD_ENCLOSED= TOKEN_KEYWORD_ELSEIF + 1,
    TOKEN_KEYWORD_END= TOKEN_KEYWORD_ENCLOSED + 1,
    TOKEN_KEYWORD_ENUM= TOKEN_KEYWORD_END + 1,
    TOKEN_KEYWORD_ESCAPED= TOKEN_KEYWORD_ENUM + 1,
    TOKEN_KEYWORD_EVENT= TOKEN_KEYWORD_ESCAPED + 1,
    TOKEN_KEYWORD_EXISTS= TOKEN_KEYWORD_EVENT + 1,
    TOKEN_KEYWORD_EXIT= TOKEN_KEYWORD_EXISTS + 1,
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
TOKEN_KEYWORD_GENERAL= TOKEN_KEYWORD_FUNCTION + 1,
    TOKEN_KEYWORD_GENERATED= TOKEN_KEYWORD_GENERAL + 1,
    TOKEN_KEYWORD_GEOMETRY= TOKEN_KEYWORD_GENERATED + 1,
    TOKEN_KEYWORD_GEOMETRYCOLLECTION= TOKEN_KEYWORD_GEOMETRY + 1,
    TOKEN_KEYWORD_GET= TOKEN_KEYWORD_GEOMETRYCOLLECTION + 1,
    TOKEN_KEYWORD_GO= TOKEN_KEYWORD_GET + 1,
    TOKEN_KEYWORD_GRANT= TOKEN_KEYWORD_GO + 1,
    TOKEN_KEYWORD_GROUP= TOKEN_KEYWORD_GRANT + 1,
    TOKEN_KEYWORD_HAVING= TOKEN_KEYWORD_GROUP + 1,
    TOKEN_KEYWORD_HELP= TOKEN_KEYWORD_HAVING + 1,
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
TOKEN_KEYWORD_MASTER_HEARTBEAT_PERIOD= TOKEN_KEYWORD_MASTER_BIND + 1,
    TOKEN_KEYWORD_MASTER_SSL_VERIFY_SERVER_CERT= TOKEN_KEYWORD_MASTER_HEARTBEAT_PERIOD + 1,
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
    TOKEN_KEYWORD_NOPAGER= TOKEN_KEYWORD_NATURAL + 1,
    TOKEN_KEYWORD_NOT= TOKEN_KEYWORD_NOPAGER + 1,
    TOKEN_KEYWORD_NOTEE= TOKEN_KEYWORD_NOT + 1,
    TOKEN_KEYWORD_NOWARNING= TOKEN_KEYWORD_NOTEE + 1,
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
    TOKEN_KEYWORD_PAGER= TOKEN_KEYWORD_OUTFILE + 1,
    TOKEN_KEYWORD_PARTITION= TOKEN_KEYWORD_PAGER + 1,
    TOKEN_KEYWORD_POINT= TOKEN_KEYWORD_PARTITION + 1,
    TOKEN_KEYWORD_POLYGON= TOKEN_KEYWORD_POINT + 1,
    TOKEN_KEYWORD_PRECISION= TOKEN_KEYWORD_POLYGON + 1,
    TOKEN_KEYWORD_PRIMARY= TOKEN_KEYWORD_PRECISION + 1,
    TOKEN_KEYWORD_PRINT= TOKEN_KEYWORD_PRIMARY + 1,
    TOKEN_KEYWORD_PROCEDURE= TOKEN_KEYWORD_PRINT + 1,
    TOKEN_KEYWORD_PROMPT= TOKEN_KEYWORD_PROCEDURE + 1,
    TOKEN_KEYWORD_PURGE= TOKEN_KEYWORD_PROMPT + 1,
    TOKEN_KEYWORD_QUIT= TOKEN_KEYWORD_PURGE + 1,
    TOKEN_KEYWORD_RANGE= TOKEN_KEYWORD_QUIT + 1,
    TOKEN_KEYWORD_READ= TOKEN_KEYWORD_RANGE + 1,
    TOKEN_KEYWORD_READS= TOKEN_KEYWORD_READ + 1,
    TOKEN_KEYWORD_READ_WRITE= TOKEN_KEYWORD_READS + 1,
    TOKEN_KEYWORD_REAL= TOKEN_KEYWORD_READ_WRITE + 1,
    TOKEN_KEYWORD_REFERENCES= TOKEN_KEYWORD_REAL + 1,
    TOKEN_KEYWORD_REGEXP= TOKEN_KEYWORD_REFERENCES + 1,
    TOKEN_KEYWORD_REHASH= TOKEN_KEYWORD_REGEXP + 1,
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
TOKEN_KEYWORD_SLOW= TOKEN_KEYWORD_SIGNAL + 1,
    TOKEN_KEYWORD_SMALLINT= TOKEN_KEYWORD_SLOW + 1,
    TOKEN_KEYWORD_SOURCE= TOKEN_KEYWORD_SMALLINT + 1,
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
    TOKEN_KEYWORD_STATUS= TOKEN_KEYWORD_STARTING + 1,
    TOKEN_KEYWORD_STORED= TOKEN_KEYWORD_STATUS + 1,
    TOKEN_KEYWORD_STRAIGHT_JOIN= TOKEN_KEYWORD_STORED + 1,
    TOKEN_KEYWORD_SYSTEM= TOKEN_KEYWORD_STRAIGHT_JOIN + 1,
    TOKEN_KEYWORD_TABLE= TOKEN_KEYWORD_SYSTEM + 1,
    TOKEN_KEYWORD_TABLESPACE= TOKEN_KEYWORD_TABLE + 1,
    TOKEN_KEYWORD_TEE= TOKEN_KEYWORD_TABLESPACE + 1,
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
    TOKEN_KEYWORD_TRUNCATE= TOKEN_KEYWORD_TRUE + 1,
    TOKEN_KEYWORD_UNDO= TOKEN_KEYWORD_TRUNCATE + 1,
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
    TOKEN_KEYWORD_VIRTUAL= TOKEN_KEYWORD_VIEW + 1,
    TOKEN_KEYWORD_WARNINGS= TOKEN_KEYWORD_VIRTUAL + 1,
    TOKEN_KEYWORD_WHEN= TOKEN_KEYWORD_WARNINGS + 1,
    TOKEN_KEYWORD_WHERE= TOKEN_KEYWORD_WHEN + 1,
    TOKEN_KEYWORD_WHILE= TOKEN_KEYWORD_WHERE + 1,
    TOKEN_KEYWORD_WITH= TOKEN_KEYWORD_WHILE + 1,
    TOKEN_KEYWORD_WRITE= TOKEN_KEYWORD_WITH + 1,
TOKEN_KEYWORD_XOR= TOKEN_KEYWORD_WRITE + 1,
    TOKEN_KEYWORD_YEAR= TOKEN_KEYWORD_XOR + 1,
    TOKEN_KEYWORD_YEAR_MONTH= TOKEN_KEYWORD_YEAR + 1,
    TOKEN_KEYWORD_ZEROFILL= TOKEN_KEYWORD_YEAR_MONTH + 1,
    TOKEN_KEYWORD__ARMSCII8= TOKEN_KEYWORD_ZEROFILL + 1,
    TOKEN_KEYWORD__ASCII= TOKEN_KEYWORD__ARMSCII8 + 1,
    TOKEN_KEYWORD__BIG5= TOKEN_KEYWORD__ASCII + 1,
    TOKEN_KEYWORD__BINARY= TOKEN_KEYWORD__BIG5 + 1,
    TOKEN_KEYWORD__CP1250= TOKEN_KEYWORD__BINARY + 1,
    TOKEN_KEYWORD__CP1251= TOKEN_KEYWORD__CP1250 + 1,
    TOKEN_KEYWORD__CP1256= TOKEN_KEYWORD__CP1251 + 1,
    TOKEN_KEYWORD__CP1257= TOKEN_KEYWORD__CP1256 + 1,
    TOKEN_KEYWORD__CP850= TOKEN_KEYWORD__CP1257 + 1,
    TOKEN_KEYWORD__CP852= TOKEN_KEYWORD__CP850 + 1,
    TOKEN_KEYWORD__CP866= TOKEN_KEYWORD__CP852 + 1,
    TOKEN_KEYWORD__CP932= TOKEN_KEYWORD__CP866 + 1,
    TOKEN_KEYWORD__DEC8= TOKEN_KEYWORD__CP932 + 1,
    TOKEN_KEYWORD__EUCJPMS= TOKEN_KEYWORD__DEC8 + 1,
    TOKEN_KEYWORD__EUCKR= TOKEN_KEYWORD__EUCJPMS + 1,
    TOKEN_KEYWORD__FILENAME= TOKEN_KEYWORD__EUCKR + 1,
    TOKEN_KEYWORD__GB2312= TOKEN_KEYWORD__FILENAME + 1,
    TOKEN_KEYWORD__GBK= TOKEN_KEYWORD__GB2312 + 1,
    TOKEN_KEYWORD__GEOSTD8= TOKEN_KEYWORD__GBK + 1,
    TOKEN_KEYWORD__GREEK= TOKEN_KEYWORD__GEOSTD8 + 1,
    TOKEN_KEYWORD__HEBREW= TOKEN_KEYWORD__GREEK + 1,
    TOKEN_KEYWORD__HP8= TOKEN_KEYWORD__HEBREW + 1,
    TOKEN_KEYWORD__KEYBCS2= TOKEN_KEYWORD__HP8 + 1,
    TOKEN_KEYWORD__KOI8R= TOKEN_KEYWORD__KEYBCS2 + 1,
    TOKEN_KEYWORD__KOI8U= TOKEN_KEYWORD__KOI8R + 1,
    TOKEN_KEYWORD__LATIN1= TOKEN_KEYWORD__KOI8U + 1,
    TOKEN_KEYWORD__LATIN2= TOKEN_KEYWORD__LATIN1 + 1,
    TOKEN_KEYWORD__LATIN5= TOKEN_KEYWORD__LATIN2 + 1,
    TOKEN_KEYWORD__LATIN7= TOKEN_KEYWORD__LATIN5 + 1,
    TOKEN_KEYWORD__MACCE= TOKEN_KEYWORD__LATIN7 + 1,
    TOKEN_KEYWORD__MACROMAN= TOKEN_KEYWORD__MACCE + 1,
    TOKEN_KEYWORD__SJIS= TOKEN_KEYWORD__MACROMAN + 1,
    TOKEN_KEYWORD__SWE7= TOKEN_KEYWORD__SJIS + 1,
    TOKEN_KEYWORD__TIS620= TOKEN_KEYWORD__SWE7 + 1,
    TOKEN_KEYWORD__UCS2= TOKEN_KEYWORD__TIS620 + 1,
    TOKEN_KEYWORD__UJIS= TOKEN_KEYWORD__UCS2 + 1,
    TOKEN_KEYWORD__UTF16= TOKEN_KEYWORD__UJIS + 1,
    TOKEN_KEYWORD__UTF16LE= TOKEN_KEYWORD__UTF16 + 1,
    TOKEN_KEYWORD__UTF32= TOKEN_KEYWORD__UTF16LE + 1,
    TOKEN_KEYWORD__UTF8= TOKEN_KEYWORD__UTF32 + 1,
    TOKEN_KEYWORD__UTF8MB4= TOKEN_KEYWORD__UTF8 + 1,
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
    TOKEN_TYPE_KEYWORD= TOKEN_KEYWORD_DEBUG_TBREAKPOINT + 1,  /* generic, lots of keywords have this */
    TOKEN_KEYWORD_BEGIN_WORK= TOKEN_TYPE_KEYWORD + 1,         /* some non-reserved keywords */
    TOKEN_KEYWORD_BEGIN_XA= TOKEN_KEYWORD_BEGIN_WORK + 1,
    TOKEN_KEYWORD_CASE_IN_CASE_EXPRESSION= TOKEN_KEYWORD_BEGIN_XA + 1,
    TOKEN_KEYWORD_CLOSE= TOKEN_KEYWORD_CASE_IN_CASE_EXPRESSION + 1,
    TOKEN_KEYWORD_COMMIT= TOKEN_KEYWORD_CLOSE + 1,
    TOKEN_KEYWORD_DEALLOCATE= TOKEN_KEYWORD_COMMIT + 1,
    TOKEN_KEYWORD_END_IN_CASE_EXPRESSION= TOKEN_KEYWORD_DEALLOCATE + 1,
    TOKEN_KEYWORD_EXECUTE= TOKEN_KEYWORD_END_IN_CASE_EXPRESSION + 1,
    TOKEN_KEYWORD_FILE= TOKEN_KEYWORD_EXECUTE + 1,
    TOKEN_KEYWORD_FLUSH= TOKEN_KEYWORD_FILE + 1,
    TOKEN_KEYWORD_HANDLER= TOKEN_KEYWORD_FLUSH + 1,
    TOKEN_KEYWORD_IF_IN_IF_EXPRESSION= TOKEN_KEYWORD_HANDLER + 1,
    TOKEN_KEYWORD_INSTALL= TOKEN_KEYWORD_IF_IN_IF_EXPRESSION + 1,
    TOKEN_KEYWORD_NOW= TOKEN_KEYWORD_INSTALL + 1,
    TOKEN_KEYWORD_OPEN= TOKEN_KEYWORD_NOW + 1,
    TOKEN_KEYWORD_PREPARE= TOKEN_KEYWORD_OPEN + 1,
    TOKEN_KEYWORD_PROCESS= TOKEN_KEYWORD_PREPARE + 1,
    TOKEN_KEYWORD_PROXY= TOKEN_KEYWORD_PROCESS + 1,
    TOKEN_KEYWORD_REPLICATION= TOKEN_KEYWORD_PROXY + 1,
    TOKEN_KEYWORD_RELOAD= TOKEN_KEYWORD_REPLICATION + 1,
    TOKEN_KEYWORD_REPAIR= TOKEN_KEYWORD_RELOAD + 1,
    TOKEN_KEYWORD_REPEAT_IN_REPEAT_EXPRESSION= TOKEN_KEYWORD_REPAIR + 1,
    TOKEN_KEYWORD_RESET= TOKEN_KEYWORD_REPEAT_IN_REPEAT_EXPRESSION + 1,
    TOKEN_KEYWORD_ROLE= TOKEN_KEYWORD_RESET + 1,
    TOKEN_KEYWORD_ROLLBACK= TOKEN_KEYWORD_ROLE + 1,
    TOKEN_KEYWORD_SAVEPOINT= TOKEN_KEYWORD_ROLLBACK + 1,
    TOKEN_KEYWORD_SESSION= TOKEN_KEYWORD_SAVEPOINT + 1,
    TOKEN_KEYWORD_SHUTDOWN= TOKEN_KEYWORD_SESSION + 1,
    TOKEN_KEYWORD_SONAME= TOKEN_KEYWORD_SHUTDOWN + 1,
    TOKEN_KEYWORD_START= TOKEN_KEYWORD_SONAME + 1,
    TOKEN_KEYWORD_STOP= TOKEN_KEYWORD_START + 1,
    TOKEN_KEYWORD_SUPER= TOKEN_KEYWORD_STOP + 1,
    TOKEN_KEYWORD_UNINSTALL= TOKEN_KEYWORD_SUPER + 1,
    TOKEN_KEYWORD_USER= TOKEN_KEYWORD_UNINSTALL + 1,
    TOKEN_KEYWORD_XA= TOKEN_KEYWORD_USER + 1,
    TOKEN_TYPE_DELIMITER= TOKEN_KEYWORD_XA + 1
  };

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
  void tokens_to_keywords_revert(int i_of_body, int i_of_function, int i_of_do, QString text);
  int next_token(int i);
  bool is_client_statement(int);
  int find_start_of_body(QString text, int *i_of_function, int *i_of_do);
  int connect_mysql(unsigned int connection_number);
#ifdef DBMS_TARANTOOL
  int connect_tarantool(unsigned int connection_number);
  void tarantool_flush_and_save_reply();
  int tarantool_real_query(const char *dbms_query, unsigned long dbms_query_len);
  unsigned int tarantool_fetch_row(const char *tarantool_tnt_reply_data, int *bytes);
  const char * tarantool_seek_0();
#endif
  QString select_1_row(const char *select_statement);

  QWidget *main_window;
  QVBoxLayout *main_layout;

  QTextEdit *history_edit_widget;
  QLineEdit *hparse_line_edit;
#ifdef DEBUGGER
#define DEBUG_TAB_WIDGET_MAX 10
  QWidget *debug_top_widget;
  QVBoxLayout *debug_top_widget_layout;
  QLineEdit *debug_line_widget;
  QTabWidget *debug_tab_widget;
  CodeEditor *debug_widget[DEBUG_TAB_WIDGET_MAX]; /* todo: this should be variable-size */
#endif

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

  int  main_token_offsets[MAX_TOKENS];
  int  main_token_lengths[MAX_TOKENS];
  int  main_token_types[MAX_TOKENS];
  unsigned char main_token_flags[MAX_TOKENS]; /* e.g. TOKEN_FLAG_IS_RESERVED */
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
#ifdef DBMS_TARANTOOL
  struct tnt_reply tarantool_tnt_reply;
  char *tarantool_field_names;
#endif

  /* connections_... [] is not a multi-occurrence list but someday it might be */
  int connections_is_connected[1];                    /* == 1 if is connected */
  int connections_dbms[1];                            /* == DBMS_MYSQL or other DBMS_... value */


};

#endif // MAINWINDOW_H

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

protected:
  void paintEvent(QPaintEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void copy();

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
  button_for_cancel= new QPushButton(tr("Cancel"), this);
  button_for_ok= new QPushButton(tr("OK"), this);
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
  typedef ssize_t         (*ttnt_object_format)  (struct tnt_stream *, const char *, int, char *);
  typedef int             (*ttnt_reload_schema)  (struct tnt_stream *);
  typedef ssize_t         (*ttnt_replace)        (struct tnt_stream *, uint32_t, struct tnt_stream *);
  typedef int             (*ttnt_reply)          (struct tnt_reply *, char *, size_t, size_t *);
  typedef tnt_reply*      (*ttnt_reply_init)     (struct tnt_reply *);
  typedef void            (*ttnt_reply_free)     (struct tnt_reply *);
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
  ttnt_reload_schema t__tnt_reload_schema;
  ttnt_replace t__tnt_replace;
  ttnt_reply t__tnt_reply;
  ttnt_reply_init t__tnt_reply_init;
  ttnt_reply_free t__tnt_reply_free;
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
    if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT18) QLibrary lib("libmysqlclient");
    if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT) QLibrary lib("libmysqlclient");
    if (which_library == WHICH_LIBRARY_LIBCRYPTO) QLibrary lib("libcrypto");
#ifdef DBMS_TARANTOOL
    if (which_library == WHICH_LIBRARY_LIBTARANTOOL) QLibrary lib("libtarantool");
    if (which_library == WHICH_LIBRARY_LIBTARANTOOLNET) QLibrary lib("libtarantoolnet");
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
      if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT18) lib.setFileName("libmysqlclient");
      if (which_library == WHICH_LIBRARY_LIBMYSQLCLIENT) lib.setFileName("libmysqlclient");
      if (which_library == WHICH_LIBRARY_LIBCRYPTO) lib.setFileName("libcrypto");
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
        t__tnt_reload_schema= (ttnt_reload_schema) dlsym(dlopen_handle, "tnt_reload_schema"); if (dlerror() != 0) s.append("tnt_reload_schema ");
        t__tnt_replace= (ttnt_replace) dlsym(dlopen_handle, "tnt_replace"); if (dlerror() != 0) s.append("tnt_replace ");
        t__tnt_reply= (ttnt_reply) dlsym(dlopen_handle, "tnt_reply"); if (dlerror() != 0) s.append("tnt_reply ");
        t__tnt_reply_free= (ttnt_reply_free) dlsym(dlopen_handle, "tnt_reply_free"); if (dlerror() != 0) s.append("tnt_reply_free ");
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
        (do not forget to allow for mysql_server_end + mysql_server_init as in Linux, above)
        if ((t__mysql_library_end= (tmysql_library_end) lib.resolve("mysql_library_end")) == 0) s.append("mysql_library_end ");
        if ((t__mysql_library_init= (tmysql_library_init) lib.resolve("mysql_library_init")) == 0) s.append("mysql_library_init ");
        if ((t__mysql_more_results= (tmysql_more_results) lib.resolve("mysql_more_results")) == 0) s.append("mysql_more_results ");
        if ((t__mysql_next_result= (tmysql_next_result) lib.resolve("mysql_next_result")) == 0) s.append("mysql_next_result ");
        if ((t__mysql_num_fields= (tmysql_num_fields) lib.resolve("mysql_num_fields")) == 0) s.append("mysql_num_fields ");
        if ((t__mysql_num_rows= (tmysql_num_rows) lib.resolve("mysql_num_rows")) == 0) s.append("mysql_num_rows ");
        if ((t__mysql_options= (tmysql_options) lib.resolve("mysql_options")) == 0) s.append("mysql_options ");
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
      if (which_library == WHICH_LIBRARY_LIBTARANTOOL)
      {
         /* fill this in when you have Windows */
      }
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
  ssize_t ldbms_tnt_object_format(struct tnt_stream *a, const char *b, int c, char *d)
  {
    return t__tnt_object_format(a,b,c,d);
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
  unsigned int *result_max_column_widths;                     /* dynamic-sized list of actual maximum widths in detail columns */
  unsigned int *grid_column_heights;                         /* dynamic-sized list of heights */
  unsigned char *grid_column_dbms_sources;                   /* dynamic-sized list of sources */
  unsigned short int *result_field_types;          /* dynamic-sized list of types */
  unsigned long result_row_number;                    /* row number in result set */
  MYSQL_ROW row;
  int is_paintable;
  unsigned int max_text_edit_frames_count;                       /* used for a strange error check during paint events */

  unsigned int result_grid_widget_max_height_in_lines;
  unsigned int result_grid_widget_max_height_in_lines_at_fillup_time;

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
  unsigned int *gridx_max_column_widths;                     /* gets a copy of result_max_column_widths */
  unsigned int *gridx_result_indexes;                        /* points to result_ lists */
  unsigned char *gridx_flags;                                /* 0 = normal, 1 = row counter */
  unsigned short int *gridx_field_types;                     /* gets a copy of result_field_types */

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

/* How many rows can fit on the screen? Take a guess. I'm trying to make this obsolete. */
#define RESULT_GRID_WIDGET_MAX_HEIGHT 10

/* Use NULL_STRING when displaying a column value which is null. Length is sizeof(NULL_STRING) - 1. */
#define NULL_STRING "NULL"

ResultGrid(
//        MYSQL_RES *mysql_res,
        ldbms *passed_lmysql,
        MainWindow *parent): QWidget(parent)
{
  is_paintable= 0;

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

  result_grid_widget_max_height_in_lines= RESULT_GRID_WIDGET_MAX_HEIGHT; /* bad if big screen? */

  /* With result_grid_widget_max_height_in_lines=20 and 20 pixels per row this might be safe though it's sure arbitrary. */
  /* TEST! Has this become unnecessary now? */
  //setMaximumHeight(result_grid_widget_max_height_in_lines * 20);

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

/* We call fillup() whenever there is a new result set to put up on the result grid widget. */
void fillup(MYSQL_RES *mysql_res,
            //struct tnt_reply *tarantool_tnt_reply,
            int connections_dbms,
            MainWindow *parent,
            unsigned short ocelot_result_grid_vertical,
            unsigned short ocelot_result_grid_column_names,
            ldbms *passed_lmysql,
            int ocelot_client_side_functions)
{
  long unsigned int xrow;
  unsigned int xcol;

  result_grid_widget_max_height_in_lines_at_fillup_time= result_grid_widget_max_height_in_lines;

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
    result_row_count= copy_of_parent->tarantool_num_rows();
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

#ifdef DBMS_TARANTOOL
  if (connections_dbms == DBMS_TARANTOOL)
    copy_of_parent->tarantool_scan_rows(result_column_count, result_row_count,
              grid_mysql_res,
              &result_set_copy, &result_set_copy_rows,
              &result_max_column_widths);
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
  if (connections_dbms == DBMS_TARANTOOL)
  {
    for (unsigned int i= 0; i < result_column_count; ++i) result_field_types[i]= OCELOT_DATA_TYPE_VAR_STRING;
  }
else
#endif
  for (unsigned int i= 0; i < result_column_count; ++i) result_field_types[i]= mysql_fields[i].type;

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

  /***** BEYOND THIS POINT, IT'S LAYOUT MATTERS *****/

  copy_result_to_gridx(connections_dbms);

  /* Some child widgets e.g. text_edit_frames[n] must not be visible because they'd receive paint events too soon. */
  hide();
  is_paintable= 0;

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

  {
    unsigned int minimum_number_of_cells;
    minimum_number_of_cells= result_grid_widget_max_height_in_lines * gridx_column_count;
    pools_resize(row_pool_size, result_grid_widget_max_height_in_lines, cell_pool_size, minimum_number_of_cells);
    if (row_pool_size < result_grid_widget_max_height_in_lines) row_pool_size= result_grid_widget_max_height_in_lines;
    if (cell_pool_size < minimum_number_of_cells) cell_pool_size= minimum_number_of_cells;
  }

  grid_column_widths= new unsigned int[gridx_column_count];
  grid_column_heights= new unsigned int[gridx_column_count];
  grid_column_dbms_sources= new unsigned char[gridx_column_count];

  dbms_set_grid_column_sources();                 /* Todo: this could return an error? */

  ocelot_grid_text_color= parent->ocelot_grid_text_color;
  ocelot_grid_background_color= parent->ocelot_grid_background_color;
  /* ocelot_grid_cell_drag_line_size_as_int= parent->ocelot_grid_cell_drag_line_size.toInt(); */
  /* ocelot_grid_cell_drag_line_color= parent->ocelot_grid_cell_drag_line_color; */

  //  grid_scroll_area= new QScrollArea(this);                                    /* Todo: see why parent can't be client */

  //  grid_scroll_area->verticalScrollBar()->setMaximum(gridx_row_count);
  //  grid_scroll_area->verticalScrollBar()->setSingleStep(1);
  //  grid_scroll_area->verticalScrollBar()->setPageStep(gridx_row_count / 10);    /* Todo; check if this could become 0 */
  grid_vertical_scroll_bar_value= -1;

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
  ocelot_grid_max_desired_width_in_pixels=(parent->width() - (mm.width("W") * 3));

  {
    /*
      Try to ensure we can fit at least header (if there is a header) plus one row.
      So there's a maximum number of lines per row.
      We assume (border height + horizontal scroll bar height) < 11 (todo: calculate them).
      We assume result grid height = height of main window / 3 (todo: calculate it).
    */
    int result_grid_height= (parent->height() / 3) - 11;
    int line_height= mm.lineSpacing() + extra_height(1);
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
  fill_detail_widgets(0, connections_dbms);                                             /* details */

  /* We'll use the automatic scroll bar for small result sets, we'll use our own scroll bar for large ones. */
  if (grid_result_row_count <= result_grid_widget_max_height_in_lines)
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
          header_height+= extra_height(1);
          text_edit_frames[xrow * gridx_column_count + xcol]->setFixedSize(grid_column_widths[xcol], header_height);
//          text_edit_frames[xrow * gridx_column_count + xcol]->setMaximumHeight(header_height);
//          text_edit_frames[xrow * gridx_column_count + xcol]->setMinimumHeight(header_height);
        }
        else
        {
          text_edit_frames[xrow * gridx_column_count + xcol]->setFixedSize(grid_column_widths[xcol], grid_column_heights[xcol]);
//          text_edit_frames[xrow * gridx_column_count + xcol]->setMaximumHeight(grid_column_heights[col]);
//          text_edit_frames[xrow * gridx_column_count + xcol]->setMinimumHeight(grid_column_heights[col]);
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
 The grid may have columns that are not in the result set.
 So far the only one is "row count" and (for vertical) "header",
 but there will be more.
 So we want to copy the result_ lists to gridx_ lists, possibly adding extra non-result-set columns.
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

  gridx_field_names= new char[total_size];                                   /* allocate */

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
    /* TODO: This shouldn't be here. It's using mysql_fields[] after we should be done with setup */
#ifdef DBMS_TARANTOOL
    if (connections_dbms != DBMS_TARANTOOL)
#endif
    {
      if ((mysql_fields[i].charsetnr == 63) && (gridx_field_types[j] == OCELOT_DATA_TYPE_VAR_STRING)) gridx_field_types[j]= OCELOT_DATA_TYPE_VARBINARY;
      if ((mysql_fields[i].charsetnr == 63) && (gridx_field_types[j] == OCELOT_DATA_TYPE_STRING)) gridx_field_types[j]= OCELOT_DATA_TYPE_BINARY;
      if ((mysql_fields[i].charsetnr != 63) && (gridx_field_types[j] == OCELOT_DATA_TYPE_BLOB)) gridx_field_types[j]= OCELOT_DATA_TYPE_TEXT;
    }
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
  so it's throttled by three ocelot_history variables that are set to small values:
  ocelot_history_max_column_width= 10;
  ocelot_history_max_column_count= 5;
  ocelot_history_max_row_count= 5;
  This is called after preparing a result set in fillup(), but doesn't really depend on most
  of the fillup() preparations except for max column widths.
  Example output:
  +==========+==========+==========+==========+======+
  |TABLE_CATA|TABLE_SCHE|TABLE_NAME|TABLE_TYPE|ENGINE|
  +==========+==========+==========+==========+======+
  |def       |informatio|PARTITIONS|SYSTEM VIE|Aria  |
  |def       |informatio|PARTITIONS|SYSTEM VIE|Aria  |
  |def       |informatio|PARTITIONS|SYSTEM VIE|Aria  |
  |def       |informatio|PARTITIONS|SYSTEM VIE|Aria  |
  |def       |informatio|PARTITIONS|SYSTEM VIE|Aria  |
  +==========+==========+==========+==========+======+
  Todo: this could be adapted for an alternate way to display the result grid.
  Todo: this is repetitious, for example code for "+===+===+' is done three times. Clean up.
  Warning: making the copy bigger would slow down the way the Previous and Next keys work.
*/
QString copy(unsigned int ocelot_history_max_column_width,
          unsigned int ocelot_history_max_column_count,
          unsigned long ocelot_history_max_row_count)
{
  unsigned int col;
  long unsigned int xrow;
  unsigned int length;
  unsigned int history_result_column_count;
  unsigned int *history_max_column_widths;
  unsigned long history_result_row_count;
  char *history_line;
  char *pointer_to_history_line;
  unsigned int history_line_width;
  QString s;

  s= "";
  history_max_column_widths= 0;
  history_line= 0;

  if (gridx_column_count > ocelot_history_max_column_count) history_result_column_count= ocelot_history_max_column_count;
  else history_result_column_count= gridx_column_count;

  history_max_column_widths= new unsigned int[history_result_column_count];
  history_line_width= 2;
  unsigned int column_width;
  for (col= 0; col < history_result_column_count; ++col)
  {
    if (ocelot_result_grid_column_names_copy == 1) column_width= mysql_fields[col].name_length;
    else column_width= 0;
    if (column_width < gridx_max_column_widths[col]) column_width= gridx_max_column_widths[col];
    if (column_width > ocelot_history_max_column_width) column_width= ocelot_history_max_column_width;
    history_max_column_widths[col]= column_width;
    history_line_width+= column_width + 1;
  }

  history_line= new char[history_line_width + 1];

  if (ocelot_result_grid_column_names_copy == 1)
  {
    pointer_to_history_line= history_line;
    *(pointer_to_history_line++)= '+';
    for (col= 0; col < history_result_column_count; ++col)
    {
      memset(pointer_to_history_line, '=', history_max_column_widths[col]);
      pointer_to_history_line+= history_max_column_widths[col];
      *(pointer_to_history_line++)= '+';
    }
    *(pointer_to_history_line)= '\n'; *(pointer_to_history_line + 1)= '\0';
    s.append(history_line);

    pointer_to_history_line= history_line;
    *(pointer_to_history_line++)= '|';
    for (col= 0; col < history_result_column_count; ++col)
    {
      length= mysql_fields[col].name_length;
      if (length > history_max_column_widths[col]) length= history_max_column_widths[col];
      memcpy(pointer_to_history_line, mysql_fields[col].name, length);
      pointer_to_history_line+= length;
      if (length < history_max_column_widths[col])
      {
        length= history_max_column_widths[col] - length;
        memset(pointer_to_history_line, ' ', length);
        pointer_to_history_line+= length;
      }
      *(pointer_to_history_line++)= '|';
    }
    *(pointer_to_history_line)= '\n'; *(pointer_to_history_line + 1)= '\0';
    s.append(history_line);
  }

  pointer_to_history_line= history_line;
  *(pointer_to_history_line++)= '+';
  for (col= 0; col < history_result_column_count; ++col)
  {
    memset(pointer_to_history_line, '=', history_max_column_widths[col]);
    pointer_to_history_line+= history_max_column_widths[col];
    *(pointer_to_history_line++)= '+';
  }
  *(pointer_to_history_line)= '\n'; *(pointer_to_history_line + 1)= '\0';
  s.append(history_line);

  if (grid_result_row_count > ocelot_history_max_row_count) history_result_row_count= ocelot_history_max_row_count;
  else history_result_row_count= grid_result_row_count;
  for (xrow= 0; (xrow < history_result_row_count); ++xrow)
  {
    pointer_to_history_line= history_line;
    *(pointer_to_history_line++)= '|';
    for (col= 0; col < history_result_column_count; ++col)
    {
      length= lengths[col];
      if (length > history_max_column_widths[col]) length= history_max_column_widths[col];
      memcpy(pointer_to_history_line, row[col], length);
      pointer_to_history_line+= length;
      if (length < history_max_column_widths[col])
      {
        length= history_max_column_widths[col] - length;
        memset(pointer_to_history_line, ' ', length);
        pointer_to_history_line+= length;
      }
      *(pointer_to_history_line++)= '|';
    }
    *(pointer_to_history_line)= '\n'; *(pointer_to_history_line + 1)= '\0';
    s.append(history_line);
  }

  pointer_to_history_line= history_line;
  *(pointer_to_history_line++)= '+';
  for (col= 0; col < history_result_column_count; ++col)
  {
    memset(pointer_to_history_line, '=', history_max_column_widths[col]);
    pointer_to_history_line+= history_max_column_widths[col];
    *(pointer_to_history_line++)= '+';
  }
  *(pointer_to_history_line)= '\n'; *(pointer_to_history_line + 1)= '\0';
  s.append(history_line);

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
  We know the defined width (update: rather than defined width we now use actual max width)
  and the number of columns
  (result_column_count which became gridx_column_count).
  We don't want to exceed the maximum grid width if we can help it.
  So let's give each column exactly what it needs, and perform a
  "squeeze" (reducing big columns) until the rows will fit, or until
  there's nothing more that can be squeezed.
  Re <cr>: I'm not very worried because it merely causes elider
      This assumes that every header or cell in the table has the same font.
      Todo: Consider using font.setStyleHint(QFont::TypeWriter); i.e. default fixed-width font
      I don't look at mysql_fields[i].max_lengths, perhaps that's a mistake.
      Todo: this might have to be re-done after a font change
      Todo: take into account that a number won't contain anything wider than '9'.
*/
/* header height calculation should differ from ordinary-row height calculation */
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

  /* Translate from char width+height to pixel width+height */
  QFontMetrics mm= QFontMetrics(*pointer_to_font);
  /* This was a bug: "this" might not have been updated by setStyleSheet() yet ... QFontMetrics mm(this->fontMetrics()); */
  unsigned int max_width_of_a_char= mm.width("W");    /* not really a maximum unless fixed-width font */
  max_height_of_a_char= mm.lineSpacing();             /* Actually this is mm.height() + mm.leading(). */

  /* KLUDGE. Small fonts don't work, we have to pretend they're wider. No, I don't know why. */
  if (max_width_of_a_char <= 5) max_width_of_a_char+= 2;
  if (max_height_of_a_char <= 11) max_height_of_a_char+= 3;

  /* KlUDGE. Usually we don't need "+= 5" but before removing test with Courier New */
  max_height_of_a_char+= 4;

  sum_tmp_column_lengths= 0;

  /*
    The first approximation
    Take it that grid_column_widths[i] = defined column width or max actual column width.
    If this is good enough, then grid_row_heights[i] = 1 char and column width = grid_column_widths[i] chars.
    Todo: the lengths are in bytes; take into account that they might arrive in a multi-byte character set.
  */
  for (i= 0; i < gridx_column_count; ++i)
  {
    if (is_using_column_names != 0)
    {
      grid_column_widths[i]= dbms_get_field_name_length(i, connections_dbms); /* this->mysql_fields[i].name_length; */
    }
    else grid_column_widths[i]= 1;
    if (grid_column_widths[i] < gridx_max_column_widths[i]) grid_column_widths[i]= gridx_max_column_widths[i]; /* fields[i].length */
    /* For explanation of next line, see comment "Extra size". Removed temporarily. */
    if ((grid_column_widths[i] < 2) && (ocelot_grid_cell_drag_line_size_as_int > 0)) grid_column_widths[i]= 2;
    grid_column_widths[i]= (grid_column_widths[i] * max_width_of_a_char)
                            + ocelot_grid_cell_border_size_as_int * 2
                            + ocelot_grid_cell_drag_line_size_as_int;
    grid_column_widths[i]+= max_width_of_a_char; /* ?? something to do with border width, I suppose */
    sum_tmp_column_lengths+= grid_column_widths[i];
  }

  /*
    The Squeeze
    This cuts the widths of the really long columns, it might loop several times.
    This is a strong attempt to reduce to the user-settable maximum, but if we have to override it, we do.
    Cannot squeeze to less than header length
    Todo: there should be a minimum for the sake of elide, and null, and border
          (actually I think now we're taking null into account well enough)
    Todo: maybe there should be a user-settable minimum column width, not just the header-related minimum
  */

  sum_amount_reduced= 1;

  while ((sum_tmp_column_lengths > ocelot_grid_max_desired_width_in_pixels) && (sum_amount_reduced > 0))
  {
    necessary_reduction= sum_tmp_column_lengths - ocelot_grid_max_desired_width_in_pixels;
    sum_amount_reduced= 0;
    for (i= 0; i < gridx_column_count; ++i)
    {
      unsigned int min_width;
      //min_width= (dbms_get_field_name_length(i) + 1) * max_width_of_a_char /* mysql_fields[i].name_length */
      //       + ocelot_grid_cell_border_size_as_int * 2
      //        + ocelot_grid_cell_drag_line_size_as_int;
//              + border_size * 2;

      /* KlUDGE. Usually we don't need "+= 3" but before removing test with Courier New */
      min_width= mm.width(dbms_get_field_name(i, connections_dbms))
              + max_width_of_a_char
              + ocelot_grid_cell_border_size_as_int * 2
              + ocelot_grid_cell_drag_line_size_as_int
              + max_width_of_a_char
              + 3;

      if (grid_column_widths[i] <= min_width) continue;
      max_reduction= grid_column_widths[i] - min_width;
      if (grid_column_widths[i] >= (sum_tmp_column_lengths / gridx_column_count))
      {
        amount_being_reduced= grid_column_widths[i] / 2;
        if (amount_being_reduced > necessary_reduction) amount_being_reduced= necessary_reduction;
        if (amount_being_reduced > max_reduction) amount_being_reduced= max_reduction;
        grid_column_widths[i]= grid_column_widths[i] - amount_being_reduced;
        sum_amount_reduced+= amount_being_reduced;
        necessary_reduction-= amount_being_reduced;
        sum_tmp_column_lengths-= amount_being_reduced;
      }
      if (necessary_reduction == 0) break; /* todo: consider making this "< 10" */
    }
  }

  grid_actual_row_height_in_lines= 1;

  /*
    Each column's height = (gridx_max_column_widths[i] i.e. actual max) / grid_column_widths[i] rounded up.
    If that's greater than the user-defined maximum, reduce to user-defined maximum
    (the QTextEdit will get a vertical scroll bar if there's an overflow).
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

  grid_height_of_highest_column_in_pixels= 0;

  /* Warning: header-height calculation is also "*(max_height_of_a_char+(border_size*2))", in a different place. */
  /* This calculation of height is horrendously difficult, and still does not seem to be exactly right. */
  /* todo: it looks as if grid_height_of_highest_column_in_pixels is no longer used */
  for (i= 0; i < gridx_column_count; ++i)
  {
//    grid_column_heights[i]= (grid_column_heights[i] * (max_height_of_a_char+(border_size * 2))) + 9;
    int old_grid_column_height= grid_column_heights[i];
    grid_column_heights[i]= (grid_column_heights[i] * max_height_of_a_char)
                            + ocelot_grid_cell_border_size_as_int * 2
                            + ocelot_grid_cell_drag_line_size_as_int;
    grid_column_heights[i]+= extra_height(old_grid_column_height);
    if (grid_column_heights[i] > grid_height_of_highest_column_in_pixels)
    {
      grid_height_of_highest_column_in_pixels= grid_column_heights[i];
    }
  }
}


/*
  Increase the allowed height in pixels for a detail column or a header.
  The numbers in this routine are the result of trial and error.
  I have no idea why we need to allow for anything extra.
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
   ... todo: check: Maybe setting minimum height / width of text_edit_frames[...] is a problem?
   Update: June 7 2015: the problem seems to have disappeared so I temporarily removed the line.
   Todo: another way to calculate a size involves layout->activate().
*/
int extra_height(int line_count)
{
  int extra= 6;
  if (ocelot_grid_cell_drag_line_size_as_int > 0)
  {
    extra+= 3;
    if (ocelot_grid_cell_drag_line_size_as_int > 8) ++extra;
  }
  else
  {
    extra+= 2;
  }
  if (line_count > 1) extra+= line_count;
  return extra;
}


/*
  Make a copy of mysql_res.
    It's insane that I have to make a copy of what was in mysql_res, = result_set_copy.
    But things get complicated if there are multiple result sets i.e. if mysql_more_results is true.
    Also, after the copy, we're less (or not at all?) dependent on calls to MySQL functions.
  For each column, we have: (unsigned int) length, (char) unused or null flag, (char[n]) contents.
  We want max actual length too.
  Todo: reconsider: maybe result_max_column_widths should have come from max_length in MYSQL_FIELD.
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
         && (v_lengths[i] == sizeof("ocelot_row_number()") - 1)
         && (strncasecmp(v_row[i], "ocelot_row_number()", v_lengths[i]) == 0))
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
        *(result_set_copy_pointer + sizeof(unsigned int))= 1;
        result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
      }
      else
      {

        if ((ocelot_client_side_functions_copy == 1)
         && (v_lengths[i] == sizeof("ocelot_row_number()") - 1)
         && (strncasecmp(v_row[i], "ocelot_row_number()", v_lengths[i]) == 0))
        {
          char tmp[16];
          sprintf(tmp, "%ld", v_r + 1);
          unsigned int v_length= strlen(tmp);
          if (v_length > (*p_result_max_column_widths)[i]) (*p_result_max_column_widths)[i]= v_length;
          memcpy(result_set_copy_pointer, &v_length, sizeof(unsigned int));
          *(result_set_copy_pointer + sizeof(unsigned int))= 0;
          result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
          memcpy(result_set_copy_pointer, tmp, v_length);
          result_set_copy_pointer+= v_length;
        }
        else
        {
          if (v_lengths[i] > (*p_result_max_column_widths)[i]) (*p_result_max_column_widths)[i]= v_lengths[i];
          memcpy(result_set_copy_pointer, &v_lengths[i], sizeof(unsigned int));
          *(result_set_copy_pointer + sizeof(unsigned int))= 0;
          result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
          memcpy(result_set_copy_pointer, v_row[i], v_lengths[i]);
          result_set_copy_pointer+= v_lengths[i];
        }
      }
    }
  }
}


/*
  Using the same technique as in scan_rows, make a copy of field names.

  Todo: Wherever there is a reference to mysql_fields[...].name or mysql_fields[...].name_length,
  replace with a reference to the appropriate spot in result_field_names.

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
*/
void set_alignment_and_height(int ki, int grid_col, int field_type)
{
  TextEditWidget *cell_text_edit_widget= text_edit_widgets[ki];
  if (field_type <= MYSQL_TYPE_DOUBLE) text_align(cell_text_edit_widget, Qt::AlignRight);
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
    text_edit_frames[ki]->setFixedSize(this_width, grid_column_heights[grid_col]);
    /* Todo: test if following 2 lines are redundant since setFixedSize does the job. */
    text_edit_frames[ki]->setMaximumHeight(grid_column_heights[grid_col]);
    text_edit_frames[ki]->setMinimumHeight(grid_column_heights[grid_col]);
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
      if (*(row_pointer - 1) == 1)
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

      if (*(row_pointer - 1) == 1)
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
      if (grid_row >= result_grid_widget_max_height_in_lines_at_fillup_time) break;
      row_pointer+= new_content_length;
      memcpy(&new_content_length, row_pointer, sizeof(unsigned int));
      row_pointer+= sizeof(unsigned int) + sizeof(unsigned char);
    }
    //for (text_edit_frames_index= text_edit_frames_index + 1; text_edit_frames_index < max_text_edit_frames_count; ++text_edit_frames_index) text_edit_frames[text_edit_frames_index]->hide();
  }

  else /* if ocelot_result_grid_vertical_copy == 0 */
  {
    for (result_row_number= first_row, grid_row= 1;
         (result_row_number < result_row_count) && (grid_row < result_grid_widget_max_height_in_lines_at_fillup_time);
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
          if (*(row_pointer - 1) == 1)
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
  for (grid_row= grid_row; grid_row < result_grid_widget_max_height_in_lines_at_fillup_time; ++grid_row) /* so if scroll bar goes past end we won't see these */
  {
    for (i= 0; i < gridx_column_count; ++i)
    {
      text_edit_frames_index= grid_row * gridx_column_count + i;
      text_edit_frames[text_edit_frames_index]->hide();
    }
  }
}


/*
  If result_grid_table_widget[i] height changes, that should affect max displayable lines.
  This does not affect other occurrences of result_grid_table_widget[i].
  This could be called from eventfilter instead.
  Height changes at start, or due to squeezing by statement + history widgets,
  or due to user action if the widget is detached.
  We're making the over-cautious assumption that it will be necessary to assign
  1 texteditframe for 1 line. In fact a texteditframe is always bigger than a line.
  Todo: Font size change could have the same effects so do the calculation there too.
  Todo: Right now the only effect is on row_pool_size next time fillup() happens,
  that is, resize's effect becomes visible when the next SELECT happens, not when the
  resize event happens, and someday we must make the effect more immediate, but without
  repainting everything every time the user shifts one pixel.
*/
void resizeEvent(QResizeEvent *event)
{
  unsigned int h_of_widget= event->size().height();

  if (h_of_widget != (unsigned int) event->oldSize().height())
  {
    QFont tmp_font= this->font();
    QFontMetrics mm= QFontMetrics(this->font());
    unsigned int h_of_line= mm.lineSpacing();
    unsigned int max_height_in_lines= h_of_widget / h_of_line;
    if ((max_height_in_lines * h_of_line) < h_of_widget) ++ max_height_in_lines;
    if (max_height_in_lines > result_grid_widget_max_height_in_lines)
    {
      result_grid_widget_max_height_in_lines= max_height_in_lines;
    }
  }
}


/*
  Called from eventfilter
  "    if (event->type() == QEvent::FontChange) return (result_grid_table_widget->fontchange_event());"
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

  /* It's ridiculous to do these settings every time. But when is the best time to to them? Which event matters? */
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
*/
void garbage_collect()
{
  remove_layouts();
  if (grid_column_widths != 0) { delete [] grid_column_widths; grid_column_widths= 0; }
  if (result_max_column_widths != 0) { delete [] result_max_column_widths; result_max_column_widths= 0; }
  if (grid_column_heights != 0) { delete [] grid_column_heights; grid_column_heights= 0; }
  if (grid_column_dbms_sources != 0) { delete [] grid_column_dbms_sources; grid_column_dbms_sources= 0; }
  if (result_field_types != 0) { delete [] result_field_types; result_field_types= 0; }
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
                          QString new_ocelot_grid_cell_drag_line_size)
{
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
#ifdef DBMS_TARANTOOL
  if (connections_dbms == DBMS_TARANTOOL) return 0;
#endif
  return mysql_fields[column_number].flags;
}


QString dbms_get_field_name(unsigned int column_number, int connections_dbms)
{
  (void) connections_dbms; /* suppress "unused parameter" warning */
#ifdef DBMS_TARANTOOL
  if (connections_dbms == DBMS_TARANTOOL)
  {
    char *result_field_names_pointer;
    char tmp[64];
    unsigned int v_lengths;
    result_field_names_pointer= &result_field_names[0];
    for (unsigned int i= 0; i < column_number; ++i)
    {
      memcpy(&v_lengths, result_field_names_pointer, sizeof(unsigned int));
      result_field_names_pointer+= v_lengths + sizeof(unsigned int);
    }
    memcpy(&v_lengths, result_field_names_pointer, sizeof(unsigned int));
    result_field_names_pointer+= sizeof(unsigned int);
    strcpy(tmp, result_field_names_pointer);
    printf("column_number=%d. tmp=%s.\n", column_number, tmp);
    QString s;
    s= tmp;
    return s;
  }
#endif
  return mysql_fields[column_number].name;
}


unsigned int dbms_get_field_name_length(unsigned int column_number, int connections_dbms)
{
  (void) connections_dbms; /* suppress "unused parameter" warning */
#ifdef DBMS_TARANTOOL
  if (connections_dbms == DBMS_TARANTOOL)
  {
    char *result_field_names_pointer;
    unsigned int v_lengths;
    result_field_names_pointer= &result_field_names[0];
    for (unsigned int i= 0; i < column_number; ++i)
    {
      memcpy(&v_lengths, result_field_names_pointer, sizeof(unsigned int));
      result_field_names_pointer+= v_lengths + sizeof(unsigned int);
    }
    memcpy(&v_lengths, result_field_names_pointer, sizeof(unsigned int));
    printf("v_lengths=%d.\n", v_lengths);
    return v_lengths;
  }
#endif
  return mysql_fields[column_number].name_length;
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
  //printf("deleting ResultGrid()\n");
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
*/

public:

  QVBoxLayout *main_layout;
  QWidget *widget_3, *widget_for_font_dialog;
  QHBoxLayout *hbox_layout_3, *hbox_layout_for_font_dialog;
  QPushButton *button_for_cancel, *button_for_ok, *button_for_font_dialog;
  QLabel *widget_colors_label, *widget_font_label;

  QWidget *widget_for_color[10];
  QHBoxLayout *hbox_layout_for_color[10];
  QLabel *label_for_color[10];
  QLabel *label_for_color_rgb[10];
  QLabel *label_for_font_dialog;

  QWidget *widget_for_syntax_checker;
  QLabel *label_for_syntax_checker;
  QComboBox *combo_box_for_syntax_checker;
  QHBoxLayout *hbox_layout_for_syntax_checker;

  QWidget *widget_for_size[3];
  QHBoxLayout *hbox_layout_for_size[3];
  QLabel *label_for_size[3];
  QComboBox *combo_box_for_size[3];

  QComboBox *combo_box_for_color_pick[10];
  QLabel *label_for_color_show[10];
  MainWindow *copy_of_parent;

  /* current_widget = MAIN_WIDGET | HISTORY_WIDGET | GRID_WIDGET | STATEMENT_WIDGET | etc. */
  int current_widget;


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
  copy_of_parent->new_ocelot_statement_syntax_checker= copy_of_parent->ocelot_statement_syntax_checker;


  {
    QString s= tr("Settings -- ");
    if (current_widget == MAIN_WIDGET) s.append(" -- for Menu");
    if (current_widget == HISTORY_WIDGET) s.append(" -- for History");
    if (current_widget == GRID_WIDGET) s.append(" -- for Grid");
    if (current_widget == STATEMENT_WIDGET) s.append(" -- for Statement");
    if (current_widget == EXTRA_RULE_1) s.append(" -- for Extra Rule 1");
    setWindowTitle(s);                                                /* affects "this"] */
  }
  widget_colors_label= new QLabel(tr("Colors"));

  /* Hboxes for foreground, background, and highlights */
  /* Todo: following calculation should actually be width of largest tr(label) + approximately 5. */
  int label_for_color_width= this->fontMetrics().boundingRect("W").width();
  for (int ci= 0; ci < 10; ++ci)
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
    label_for_color[ci]->setFixedWidth(label_for_color_width * 20);
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

  if (current_widget == STATEMENT_WIDGET)
  {
    widget_for_syntax_checker= new QWidget(this);
    label_for_syntax_checker= new QLabel("Syntax Checker (1=highlight,3=highlight+error dialog)");
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

  if (current_widget == GRID_WIDGET)
  {
    /* int label_for_size_width= this->fontMetrics().boundingRect("W").width(); */
    for (int ci= 0; ci < 3; ++ci)
    {
      widget_for_size[ci]= new QWidget(this);
      if (ci == 0) label_for_size[ci]= new QLabel(tr("Grid Border size"));
      if (ci == 1) label_for_size[ci]= new QLabel(tr("Grid Cell Border Size"));
      if (ci == 2) label_for_size[ci]= new QLabel(tr("Grid Cell Drag Line Size"));
      combo_box_for_size[ci]= new QComboBox();
      combo_box_for_size[ci]->setFixedWidth(label_for_color_width * 3);
      for (int cj= 0; cj < 10; ++cj) combo_box_for_size[ci]->addItem(QString::number(cj));
      label_for_size[ci]->setFixedWidth(label_for_color_width * 20);
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
    label_for_size[0]= new QLabel(tr("Condition"));
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
    label_for_size[1]= new QLabel(tr("Display as"));
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
  for (int ci= 0; ci < 10; ++ci) main_layout->addWidget(widget_for_color[ci]);
  main_layout->addWidget(widget_font_label);
  main_layout->addWidget(widget_for_font_dialog);
  if (current_widget == STATEMENT_WIDGET) main_layout->addWidget(widget_for_syntax_checker);
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
    case 0: { color_type= tr("Statement Text Color"); color_name= copy_of_parent->new_ocelot_statement_text_color; break; }
    case 1: { color_type= tr("Statement Background Color"); color_name= copy_of_parent->new_ocelot_statement_background_color; break; }
    case 2: { color_type= tr("Statement Highlight Literal Color"); color_name= copy_of_parent->new_ocelot_statement_highlight_literal_color; break; }
    case 3: { color_type= tr("Statement Highlight Identifier Color"); color_name= copy_of_parent->new_ocelot_statement_highlight_identifier_color; break; }
    case 4: { color_type= tr("Statement Highlight Comment Color"); color_name= copy_of_parent->new_ocelot_statement_highlight_comment_color; break; }
    case 5: { color_type= tr("Statement Highlight Operator Color"); color_name= copy_of_parent->new_ocelot_statement_highlight_operator_color; break; }
    case 6: { color_type= tr("Statement Highlight Keyword Color"); color_name= copy_of_parent->new_ocelot_statement_highlight_keyword_color; break; }
    case 7: { color_type= tr("Statement Prompt Background Color"); color_name= copy_of_parent->new_ocelot_statement_prompt_background_color; break; }
    case 8: { color_type= tr("Statement Border Color"); color_name= copy_of_parent->new_ocelot_statement_border_color; break; }
    case 9: { color_type= tr("Statement Highlight Current Line Color"); color_name= copy_of_parent->new_ocelot_statement_highlight_current_line_color; break; }
    }
  }
  if (current_widget == GRID_WIDGET)
  {
    switch (ci)
    {
    case 0: { color_type= tr("Grid Text Color"); color_name= copy_of_parent->new_ocelot_grid_text_color; break; }
    case 1: { color_type= tr("Grid Background Color"); color_name= copy_of_parent->new_ocelot_grid_background_color; break; }
    case 2: { color_type= tr("Grid Cell Border Color"); color_name= copy_of_parent->new_ocelot_grid_cell_border_color; break; }
    case 3: { color_type= tr("Grid Cell Drag Line Color"); color_name= copy_of_parent->new_ocelot_grid_cell_drag_line_color; break; }
    case 7: { color_type= tr("Grid Header Background Color"); color_name= copy_of_parent->new_ocelot_grid_header_background_color; break; }
    case 8: { color_type= tr("Grid Border Color"); color_name= copy_of_parent->new_ocelot_grid_border_color; break; }
    }
  }
  if (current_widget == EXTRA_RULE_1)
  {
    switch (ci)
    {
    case 0: { color_type= tr("Grid Text Color"); color_name= copy_of_parent->new_ocelot_extra_rule_1_text_color; break; }
    case 1: { color_type= tr("Grid Background Color"); color_name= copy_of_parent->new_ocelot_extra_rule_1_background_color; break; }
    }
  }
  if (current_widget == HISTORY_WIDGET)
  {
    switch (ci)
    {
    case 0: { color_type= tr("History Text Color"); color_name= copy_of_parent->new_ocelot_history_text_color; break; }
    case 1: { color_type= tr("History Background Color"); color_name= copy_of_parent->new_ocelot_history_background_color; break; }
    case 8: { color_type= tr("History Border Color"); color_name= copy_of_parent->new_ocelot_history_border_color; break; }
    }
  }
  if (current_widget == MAIN_WIDGET)
  {
    switch (ci)
    {
    case 0: { color_type= tr("Menu Text Color"); color_name= copy_of_parent->new_ocelot_menu_text_color; break; }
    case 1: { color_type= tr("Menu Background Color"); color_name= copy_of_parent->new_ocelot_menu_background_color; break; }
    case 8: { color_type= tr("Menu Border Color"); color_name= copy_of_parent->new_ocelot_menu_border_color; break; }
    }
  }
  label_for_color[ci]->setText(color_type);
  //label_for_color_rgb[ci]->setText(color_name);

  int cli;
  cli= q_color_list_index(color_name);
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

  for (ci= 0; ci < 10; ++ci) set_widget_values(ci);
  if (i == STATEMENT_WIDGET)
  {
    color_type= "Prompt background";               /* ?? unnecessary now that set_widget_values() does it, eh? */
    label_for_color[7]->setText(color_type);
    for (ci= 2; ci < 10 ; ++ci)
    {
      label_for_color[ci]->show();
      label_for_color_rgb[ci]->show();
      label_for_color_show[ci]->show();
      combo_box_for_color_pick[ci]->show();
    }
  }

  if (i == GRID_WIDGET)
  {
    color_type= "Grid Header Background Color";     /* ?? unnecessary now that set_widget_values() does it, eh? */
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
  }
  if (i == EXTRA_RULE_1)
  {
    for (ci= 2; ci < 10; ++ci)
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
    s_for_label_for_font_dialog= copy_of_parent->new_ocelot_menu_font_family;
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
    label_for_color_rgb[0]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[0]->setStyleSheet(s);
  }
  if (current_widget == GRID_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[0]->itemText(item_number);
    copy_of_parent->new_ocelot_grid_text_color= new_color;
    label_for_color_rgb[0]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[0]->setStyleSheet(s);
  }
  if (current_widget == EXTRA_RULE_1)
  {
    QString new_color= combo_box_for_color_pick[0]->itemText(item_number);
    copy_of_parent->new_ocelot_extra_rule_1_text_color= new_color;
    label_for_color_rgb[0]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[0]->setStyleSheet(s);
  }
  if (current_widget == HISTORY_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[0]->itemText(item_number);
    copy_of_parent->new_ocelot_history_text_color= new_color;
    label_for_color_rgb[0]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[0]->setStyleSheet(s);
  }
  if (current_widget == MAIN_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[0]->itemText(item_number);
    copy_of_parent->new_ocelot_menu_text_color= new_color;
    label_for_color_rgb[0]->setText(new_color);
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
    copy_of_parent->new_ocelot_statement_background_color= new_color;
    label_for_color_rgb[1]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[1]->setStyleSheet(s);
  }
  if (current_widget == GRID_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[1]->itemText(item_number);
    copy_of_parent->new_ocelot_grid_background_color= new_color;
    label_for_color_rgb[1]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[1]->setStyleSheet(s);
  }
  if (current_widget == EXTRA_RULE_1)
  {
    QString new_color= combo_box_for_color_pick[1]->itemText(item_number);
    copy_of_parent->new_ocelot_extra_rule_1_background_color= new_color;
    label_for_color_rgb[1]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[1]->setStyleSheet(s);
  }
  if (current_widget == HISTORY_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[1]->itemText(item_number);
    copy_of_parent->new_ocelot_history_background_color= new_color;
    label_for_color_rgb[1]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[1]->setStyleSheet(s);
  }
  if (current_widget == MAIN_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[1]->itemText(item_number);
    copy_of_parent->new_ocelot_menu_background_color= new_color;
    label_for_color_rgb[1]->setText(new_color);
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
    copy_of_parent->new_ocelot_statement_highlight_literal_color= new_color;
    label_for_color_rgb[2]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[2]->setStyleSheet(s);
  }

  if (current_widget == GRID_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[2]->itemText(item_number);
    copy_of_parent->new_ocelot_grid_cell_border_color= new_color;
    label_for_color_rgb[2]->setText(new_color);
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
    copy_of_parent->new_ocelot_statement_highlight_identifier_color= new_color;
    label_for_color_rgb[3]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[3]->setStyleSheet(s);
  }

  if (current_widget == GRID_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[3]->itemText(item_number);
    copy_of_parent->new_ocelot_grid_cell_drag_line_color= new_color;
    label_for_color_rgb[3]->setText(new_color);
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
    copy_of_parent->new_ocelot_statement_highlight_comment_color= new_color;
    label_for_color_rgb[4]->setText(new_color);
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
    copy_of_parent->new_ocelot_statement_highlight_operator_color= new_color;
    label_for_color_rgb[5]->setText(new_color);
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
    copy_of_parent->new_ocelot_statement_highlight_keyword_color= new_color;
    label_for_color_rgb[6]->setText(new_color);
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
   copy_of_parent->new_ocelot_statement_prompt_background_color= new_color;
   label_for_color_rgb[7]->setText(new_color);
   QString s= "border: 1px solid black; background-color: ";
   s.append(copy_of_parent->qt_color(new_color));
   label_for_color_show[7]->setStyleSheet(s);
  }
  if (current_widget == GRID_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[7]->itemText(item_number);
    copy_of_parent->new_ocelot_grid_header_background_color= new_color;
    label_for_color_rgb[7]->setText(new_color);
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
    copy_of_parent->new_ocelot_statement_border_color= new_color;
    label_for_color_rgb[8]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[8]->setStyleSheet(s);
  }
  if (current_widget == GRID_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[8]->itemText(item_number);
    copy_of_parent->new_ocelot_grid_border_color= new_color;
    label_for_color_rgb[8]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[8]->setStyleSheet(s);
  }
  if (current_widget == HISTORY_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[8]->itemText(item_number);
    copy_of_parent->new_ocelot_history_border_color= new_color;
    label_for_color_rgb[8]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[8]->setStyleSheet(s);
  }
  if (current_widget == MAIN_WIDGET)
  {
    QString new_color= combo_box_for_color_pick[8]->itemText(item_number);
    copy_of_parent->new_ocelot_menu_border_color= new_color;
    label_for_color_rgb[8]->setText(new_color);
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
    copy_of_parent->new_ocelot_statement_highlight_current_line_color= new_color;
    label_for_color_rgb[9]->setText(new_color);
    QString s= "border: 1px solid black; background-color: ";
    s.append(copy_of_parent->qt_color(new_color));
    label_for_color_show[9]->setStyleSheet(s);
  }
}

void handle_combo_box_for_syntax_check(int i)
{
  if (current_widget == STATEMENT_WIDGET)
    copy_of_parent->new_ocelot_statement_syntax_checker= QString::number(i);
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


