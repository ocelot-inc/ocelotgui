/*
  ocelotgui -- Ocelot GUI Front End for MySQL or MariaDB

   Version: 1.0.6
   Last modified: July 30 2018
*/

/*
  Copyright (c) 2014-2017 by Ocelot Computer Services Inc. All rights reserved.

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
   The class named CodeEditor inside the ocelotgui program is taken and modified from
   http://qt-project.org/doc/qt-4.8/widgets-codeeditor.html
   and therefore CodeEditor's maker's copyright and BSD license provisions
   are reproduced below. These provisions apply only for the
   part of the CodeEditor class which is included by #include "codeeditor.h".
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

#ifdef DEBUGGER
/*
  The routine named debug_mdbug_install inside this program is taken and modified from
  https://launchpad.net/mdbug, specifically http://bazaar.launchpad.net/~hp-mdbug-team/mdbug/trunk/view/head:/install.sql,
  and therefore MDBug's maker's copyright and GPL version 2 license provisions
  are reproduced below. These provisions apply only for the
  part of the debug_mdbug_install routine which is marked within the program.
  The program as a whole is copyrighted by Ocelot and
  licensed under GPL version 2 as stated above.

  This file is part of MDBug.

  (c) Copyright 2012 Hewlett-Packard Development Company, L.P.

  This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with this program; if not, see
  <http://www.gnu.org/licenses>.

  Linking MDBug statically or dynamically with other modules is making a combined work based on MDBug. Thus, the terms and
  conditions of the GNU General Public License cover the whole combination.

  In addition, as a special exception, the copyright holders of MDBug give you permission to combine MDBug with free
  software programs or libraries that are released under the GNU LGPL and with code included in the standard release
  of Eclipse under the Eclipse Public License version 1.0 (or modified versions of such code, with unchanged license).
  You may copy and distribute such a system following the terms of the GNU GPL for MDBug and the licenses of the other
  code concerned, provided that you include the source code of that other code when and as the GNU GPL requires
  distribution of source code.

  Note that people who make modified versions of MDBug are not obligated to grant this special exception for their
  modified versions; it is their choice whether to do so. The GNU General Public License gives permission to release a
  modified version without this exception; this exception also makes it possible to release a modified version which
  carries forward this exception.
*/
#endif

/*
  On Windows, ocelotgui is statically linked with libraries
  MariaDB Connector C and Qt. See COPYING.thirdparty.
*/

/*
  General comments

  These are comments about the code.
  The user manual and blog comments are elsewhere; look at README.md or manual.htm
  or README.txt or ocelot.ca or ocelot.ca/blog or the repository that
  this program was downloaded from, probably github.com/ocelot-inc/ocelotgui.

  The code files are:
    ocelotgui.pro       small. used by Qt qmake
    ocelotgui.h         headers but also quite a lot of executable
    ocelotgui.cpp       executable
    install_sql.cpp     for creating debugger routines
    ocelotgui.ui        small. used to make ui_ocelotgui.h
    codeeditor.h        small. separated so license applicability is clear
    hparse.h            the recognizer
    ostrings.h          strings that contain English translatable text

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
  The code includes a tokenizer and recognizer for MySQL syntax.
  The coding style is generally according to MySQL coding guidelines
  http://dev.mysql.com/doc/internals/en/coding-guidelines.html
  but lines may be long, and sometimes spaces occur at ends of lines,
  and "if (x) y" may be preferred to "if (x) <newline> y".

  The code was written by Ocelot Computer Services Inc. employees, except
  for about 50 lines in the codeeditor.h CodeEditor section (Digia copyright / BSD license),
  and except for most of the lines in install_sql.cpp (HP copyright / GPL license).
  Other contributors will be acknowledged here and/or in a "Help" display.

  The code #includes header files from MySQL/Oracle and from Qt/Digia,
  and relies on the MySQL client library and the Qt core, gui,
  and widgets libraries. Builds have been successful with several
  Linux distros and gcc 4.6/4.7. Build instructions are in the user manual or
  in a readme file

  There are many comments. Searching for the word "Todo:" in the comments
  will give some idea of what's especially weak and what's planned.

  UTF8 Conversion
  Everything which will be passed to MySQL will first be converted to UTF8.
  Todo: Ensure that sending/receiving character set is UTF8, regardless of data language.
        Currently we do say ocelot_set_charset_name= "utf8"; which is passed to mysql_options()
        but the user can explicitly override that.
  Todo: When receiving, only convert characters, not binary or numeric.
  Todo: Handle Qt codecs for other character sets.
  Todo: Figure out how to handle a literal preceded by an _introducer.
  A typical set of instructions will look like this and have the comment
  "See comment "UTF8 Conversion".
     int tmp_len= QStr.toUtf8().size());                  ... See how big UTF8 will be
     char *tmp= new char[tmp_len + 1];                      ... Allocate that amount
     memcpy(tmp, QStr.toUtf8().constData()), tmp_len + 1);  ... Copy including trail '\0' byte
     ... And later, say "delete []tmp" or hope for garbage collection

  Todo: when I started, I wasn't aware that I could say
        QString::fromUtf8 to copy a C string to a QString.
        So there are several places where I copy to another temporary
        C string and then assign. Such code should be replaced.

  Todo: Allow right-click for changing individual widgets.

  See end of program for comments re valgrind.

  The usual ways to build are described in README.txt (actually README.md).
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
        That ruins what Qt Creator expects when I get back to Qt Creator, so I get back to 5.1 with:
        cd ~/ocelotgui
        /home/pgulutzan/Qt/5.1.1/gcc_64/bin/qmake ./ocelotgui.pro
        mv ui_ocelotgui.h ui_ocelotgui.h.bak
        rm ui_ocelotgui.h
        make (ignore failure)
        re-enter Qt Creator. Build | Clean all. Build all. Run.
*/

#define MYSQL_MAIN_CONNECTION 0
#define MYSQL_DEBUGGER_CONNECTION 1
#define MYSQL_KILL_CONNECTION 2
#define MYSQL_REMOTE_CONNECTION 3
#define MYSQL_MAX_CONNECTIONS 4

#include "ostrings.h"
#include "ocelotgui.h"
#include "ui_ocelotgui.h"

/* Whenever you see STRING_LENGTH_512, think: here's a fixed arbitrary allocation, which should be be fixed up. */
#define STRING_LENGTH_512 512

/* Connect arguments and options */
  static char* ocelot_host_as_utf8= 0;                  /* --host=s */
  static char* ocelot_database_as_utf8= 0;              /* --database=s */
  static char* ocelot_user_as_utf8= 0;                  /* --user=s */
  static char* ocelot_password_as_utf8= 0;              /* --password[=s] */
  static unsigned short ocelot_port= MYSQL_PORT;        /* --port=n */
  static char* ocelot_unix_socket_as_utf8= 0;           /* --socket=s */
  static unsigned int ocelot_protocol_as_int= 0;        /* --protocol=s for MYSQL_OPT_PROTOCOL */
  static char* ocelot_init_command_as_utf8= 0;          /* --init_command=s for MYSQL_INIT_COMMAND */
  /* Connect arguments below this point are minor and many are unsupported. */
  static unsigned short ocelot_abort_source_on_error= 0;   /* --abort_source_on_error (MariaDB) */
  static unsigned short ocelot_auto_rehash= 1;             /* --auto_rehash */
  static unsigned short ocelot_auto_vertical_output= 0;    /* --auto_vertical_output */
  static unsigned short ocelot_batch= 0;                   /* --batch */
  static unsigned short ocelot_binary_mode= 0;             /* --binary_mode */
  /* QString ocelot_bind_address */                        /* --bind_address=s */
  static char* ocelot_set_charset_dir_as_utf8= 0;         /* --character_sets_dir=s for MYSQL_SET_CHARSET_DIR */
  static unsigned short ocelot_result_grid_column_names= 1;/* --column_names */
  static unsigned short ocelot_column_type_info= 0;        /* --column_type_info */
  static unsigned short ocelot_comments= 0;               /* --comments */
  static unsigned short ocelot_opt_compress= 0;           /* --compress for MYSQL_OPT_COMPRESS */
  static unsigned long int ocelot_opt_connect_timeout= 0;  /* --connect_timeout = n for MYSQL_OPT_CONNECT_TIMEOUT */
  /* QString ocelot_debug */                               /* --debug[=s] */
  static unsigned short ocelot_debug_check= 0;             /* --debug_check */
  static unsigned short ocelot_debug_info= 0;              /* --debug_info */
  static char* ocelot_default_auth_as_utf8= 0;      /* --default_auth=s for MYSQL_DEFAULT_AUTH */
  static char* ocelot_set_charset_name_as_utf8= 0;  /* --default_character_set=s for MYSQL_SET_CHARSET_NAME */
  /* exists as QString */                    /* --defaults_extra_file=s */
  /* exists as QString */                    /* --defaults_file=s */
  /* exists as QString */                    /* --defaults_group_suffix=s */
  /* exists as QString */                    /* --delimiter=s */
  static unsigned short ocelot_enable_cleartext_plugin= 0;  /* --enable_cleartext_plugin for MYSQL_ENABLE_CLEARTEXT_PLUGIN */
  /* QString ocelot_execute */               /* --execute=s */
  static unsigned short ocelot_force= 0;     /* --force */
  static unsigned short ocelot_help= 0;      /* --help */
  static bool ocelot_history_hist_file_is_open= false;
  static bool ocelot_history_hist_file_is_copied= false;
  /* exists as QString */                    /* --histfile=s */
  /* exists as QString */                    /* --histignore=s */
  static unsigned short ocelot_html= 0;      /* --html */
  static unsigned short ocelot_ignore_spaces= 0;  /* --ignore_spaces */
  /* QString ocelot_ld_run_path */                /* --ld_run_path=s */
  static unsigned short ocelot_line_numbers= 0;   /* --line_numbers */
  static unsigned short ocelot_opt_local_infile= 0;    /* --local_infile[=n]  for MYSQL_OPT_LOCAL_INFILE */
  /* QString ocelot_login_path */            /* --login_path=s */
  static int ocelot_log_level= 100;          /* --ocelot_log_level */
  static unsigned long int ocelot_max_allowed_packet= 16777216; /* --max_allowed_packet=n */
  static unsigned long int ocelot_max_join_size= 1000000; /* --max_join_size = n */
  static unsigned short ocelot_named_commands= 0;         /* --named_commands */
  static unsigned long int ocelot_net_buffer_length= 16384; /* --net_buffer_length=n */
  static unsigned short ocelot_no_beep= 0;                /* --no_beep */
  static unsigned short ocelot_no_defaults= 0;            /* --no_defaults */
  static unsigned short ocelot_one_database= 0;           /* --one-database */
  /* QString ocelot_pager */                              /* --pager[=s] */
  static unsigned short ocelot_pipe= 0;                   /* --pipe */
  static char* ocelot_plugin_dir_as_utf8= 0;        /* --plugin_dir=s for MYSQL_PLUGIN_DIR */
  static unsigned short ocelot_print_defaults= 0;   /* --print_defaults */
  /* QString ocelot_prompt */                       /* --prompt=s */
  static bool ocelot_prompt_is_default= true;
  static unsigned short ocelot_quick= 0;            /* --quick */
  static unsigned short ocelot_raw= 0;              /* --raw */
  static unsigned int ocelot_opt_reconnect= 0;      /* --reconnect for MYSQL_OPT_RECONNECT */                                           /* --reconnect */
  static unsigned short ocelot_safe_updates= 0;           /* --safe-updates or --i-am-a-dummy */
  static unsigned short ocelot_secure_auth= 1;             /* --secure_auth for MYSQL_SECURE_AUTH (default=true if version >= 5.6.5) */
  static unsigned long int ocelot_select_limit= 0;  /* --select_limit = n */
  static char* ocelot_server_public_key_as_utf8= 0; /* --server_public_key=s for MYSQL_SERVER_PUBLIC_KEY */
  static char* ocelot_shared_memory_base_name_as_utf8= 0; /* --shared_memory_base_name=s for MYSQL_SHARED_MEMORY_BASE_NAME */
  static unsigned short ocelot_history_includes_warnings= 0; /* --show_warnings include warning(s) returned from statement? default = no. */
  static unsigned short ocelot_sigint_ignore= 0;          /* --sigint_ignore */
  static unsigned short ocelot_silent= 0;                 /* --silent */
  static char* ocelot_opt_ssl_as_utf8= 0;           /* --ssl for CONNECT */
  static char* ocelot_opt_ssl_ca_as_utf8= 0;        /* --ssl-ca for MYSQL_OPT_SSL_CA */
  static char* ocelot_opt_ssl_capath_as_utf8= 0;    /* --ssl-capath for MYSQL_OPT_SSL_CAPATH. */
  static char* ocelot_opt_ssl_cert_as_utf8= 0;      /* --ssl-cert for MYSQL_OPT_SSL_CERT */
  static char* ocelot_opt_ssl_cipher_as_utf8= 0;    /* --ssl-cipher for MYSQL_OPT_SSL_CIPHER */
  static char* ocelot_opt_ssl_crl_as_utf8= 0;       /*  --ssl-crl for MYSQL_OPT_SSL_CRL */
  static char* ocelot_opt_ssl_crlpath_as_utf8= 0;   /* --ssl-crlpath for MYSQL_OPT_SSL_CRLPATH */
  static char* ocelot_opt_ssl_key_as_utf8= 0;       /* --ssl-key for MYSQL_OPT_SSL_KEY */
  static char* ocelot_opt_ssl_mode_as_utf8= 0;      /* --ssl-mode for MYSQL_OPT_SSL_MODE */
  static unsigned short int ocelot_opt_ssl_verify_server_cert= 0;  /* --ssl-verify-server-cert for MYSQL_OPT_SSL_VERIFY_SERVER_CERT. --ssl-verify-server-cert (5.7) */
  static unsigned short ocelot_syslog= 0;           /* --syslog (5.7) */
  static unsigned short ocelot_table= 0;            /* --table */
  static bool ocelot_history_tee_file_is_open= false;            /* --tee for tee  ... arg=history_tee_file_name*/
  static unsigned short ocelot_unbuffered= 0;       /* --unbuffered */
  static unsigned short ocelot_verbose= 0;          /* --verbose */
  static unsigned short ocelot_version= 0;          /* --version */
  static unsigned short ocelot_result_grid_vertical= 0;   /* --vertical for vertical */
  static unsigned short ocelot_wait= 0;                   /* --wait ... actually this does nothing */
  static unsigned short ocelot_xml= 0;                    /* --xml */

  /*
    For MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS + --connect-expired-password.
    mysql client has this off by default, but ocelotgui has it on by default
    so to turn it off say ocelotgui --skip_connect_expired_password
  */
  static unsigned short ocelot_opt_can_handle_expired_passwords= 1;

  /* Some items we allow, which are not available in mysql client */
  static char* ocelot_opt_bind_as_utf8= 0;              /* for MYSQL_OPT_BIND */
  static char* ocelot_opt_connect_attr_delete_as_utf8= 0;  /* for MYSQL_OPT_CONNECT_ATTR_DELETE */
  static unsigned short int ocelot_opt_connect_attr_reset= 0; /* for MYSQL_OPT_CONNECT_ATTR_RESET */
  static char* ocelot_read_default_file_as_utf8= 0; /* for MYSQL_READ_DEFAULT_FILE */
  static char* ocelot_read_default_group_as_utf8= 0;/* for MYSQL_READ_DEFAULT_GROUP */
  static unsigned int ocelot_opt_read_timeout= 0;          /* for MYSQL_OPT_READ_TIMEOUT */
  static unsigned short int ocelot_report_data_truncation= 0; /* for MYSQL_REPORT_DATA_TRUNCATION */
  static unsigned short int ocelot_opt_use_result= 0; /* for MYSQL_OPT_USE_RESULT */
  /* It's easy to increase ocelot_grid_tabs so more multi results are seen but don't make it ridiculous. */
  static unsigned short int ocelot_grid_tabs= 16;
  static unsigned short int ocelot_grid_actual_tabs= 0; /* Todo: move this, it's not an option. */
  static unsigned short int ocelot_client_side_functions= 1;
  static char ocelot_shortcut_connect[80]= "default";
  static char ocelot_shortcut_exit[80]= "default";
  static char ocelot_shortcut_undo[80]= "default";
  static char ocelot_shortcut_redo[80]= "default";
  static char ocelot_shortcut_cut[80]= "default";
  static char ocelot_shortcut_copy[80]= "default";
  static char ocelot_shortcut_paste[80]= "default";
  static char ocelot_shortcut_select_all[80]= "default";
  static char ocelot_shortcut_history_markup_previous[80]= "default";
  static char ocelot_shortcut_history_markup_next[80]= "default";
  static char ocelot_shortcut_format[80]= "default";
  static char ocelot_shortcut_zoomin[80]= "default";
  static char ocelot_shortcut_zoomout[80]= "default";
  static char ocelot_shortcut_autocomplete[80]= "default";
  static char ocelot_shortcut_execute[80]= "default";
  static char ocelot_shortcut_kill[80]= "default";
  static char ocelot_shortcut_next_window[80]= "default";
  static char ocelot_shortcut_previous_window[80]= "default";
  static char ocelot_shortcut_breakpoint[80]= "default";
  static char ocelot_shortcut_continue[80]= "default";
  static char ocelot_shortcut_next[80]= "default";
  static char ocelot_shortcut_step[80]= "default";
  static char ocelot_shortcut_clear[80]= "default";
  static char ocelot_shortcut_debug_exit[80]= "default";
  static char ocelot_shortcut_information[80]= "default";
  static char ocelot_shortcut_refresh_server_variables[80]= "default";
  static char ocelot_shortcut_refresh_user_variables[80]= "default";
  static char ocelot_shortcut_refresh_variables[80]= "default";
  static char ocelot_shortcut_refresh_call_stack[80]= "default";

  /* Some items we allow, but the reasons we allow them are lost in the mists of time */
  /* I gather that one is supposed to read the charset file. I don't think we do. */
  static unsigned short ocelot_opt_named_pipe;          /* for MYSQL_OPT_NAMED_PIPE */
  static unsigned int ocelot_opt_write_timeout= 0; /* for MYSQL_OPT_WRITE_TIMEOUT */

  static bool ocelot_detach_history_widget= false;
  static bool ocelot_detach_result_grid_widget= false;
  static bool ocelot_detach_debug_widget= false;

  static int is_libmysqlclient_loaded= 0;
  static int is_libcrypto_loaded= 0;
  static void *libmysqlclient_handle= NULL;
  static void *libcrypto_handle= NULL;
#ifdef DBMS_TARANTOOL
  static int is_libtarantool_loaded= 0;
  //static int is_libtarantoolnet_loaded= 0;
  static void *libtarantool_handle= 0;
  //static void *libtarantoolnet_handle= 0;
  /* Todo: these shouldn't be global */
  tnt_reply *tarantool_tnt_for_new_result_set;
  bool tarantool_begin_seen= false;
#endif

  static  unsigned int rehash_result_column_count= 0;
  static unsigned int rehash_result_row_count= 0;
  static char *rehash_result_set_copy= 0;       /* gets a copy of mysql_res contents, if necessary */
  static char **rehash_result_set_copy_rows= 0; /* dynamic-sized list of result_set_copy row offsets, if necessary */

  int options_and_connect(unsigned int connection_number, char *database_as_utf8);

  /* This should correspond to the version number in the comment at the start of this program. */
  static const char ocelotgui_version[]="1.0.6"; /* For --version. Make sure it's in manual too. */
  /* Todo: initialize this as we do for hparse_dbms_mask */
  static unsigned short int dbms_version_mask;

/* Global mysql definitions */
  static MYSQL mysql[MYSQL_MAX_CONNECTIONS];
  static int connected[MYSQL_MAX_CONNECTIONS]= {0, 0, 0, 0};
  pthread_t debug_thread_id;
  bool debug_thread_exists= false;
  static int connections_is_connected[MYSQL_MAX_CONNECTIONS];  /* == 1 if is connected */
  static int connections_dbms[MYSQL_MAX_CONNECTIONS];          /* == DBMS_MYSQL or other DBMS_... value */
#ifdef DBMS_TARANTOOL
/* Global Tarantool definitions */
  static struct tnt_stream *tnt[MYSQL_MAX_CONNECTIONS];
  static int tarantool_errno[MYSQL_MAX_CONNECTIONS];
  static char tarantool_errmsg[3072]; /* same size as hparse_errmsg? */
  QString tarantool_server_name= "";
  static long unsigned int tarantool_row_count[MYSQL_MAX_CONNECTIONS];
#endif

  static ldbms *lmysql= 0;

  static bool is_mysql_library_init_done= false;

  unsigned int mysql_errno_result= 0;


  static QString hparse_text_copy;
  static QString hparse_token;
  static int hparse_i;
  static QString hparse_expected;
  static int hparse_errno; /* 0=ok, 1=error, 2=error-unrecoverable */
  static int hparse_errno_count;
  static int hparse_token_type;
  static int hparse_statement_type= -1;
  static bool sql_mode_ansi_quotes= false;
  /* hparse_f_accept can dump many expected tokens in hparse_errmsg */
  static char hparse_errmsg[3072]; /* same size as tarantool_errmsg? */
  static QString hparse_next_token, hparse_next_next_token;
  static int hparse_next_token_type, hparse_next_next_token_type;
  static QString hparse_next_next_next_token, hparse_next_next_next_next_token;
  static int hparse_next_next_next_token_type, hparse_next_next_next_next_token_type;
  static int hparse_begin_count;
  static bool hparse_as_seen;
  static bool hparse_create_trigger_seen;
  static int hparse_count_of_accepts;
  static int hparse_i_of_statement;
  static int hparse_i_of_last_accepted;
  static QString hparse_prev_token;
  static bool hparse_like_seen;
  static bool hparse_subquery_is_allowed;
  static QString hparse_delimiter_str;
  static bool hparse_sql_mode_ansi_quotes= false;
  static unsigned short int hparse_dbms_mask= FLAG_VERSION_MYSQL_OR_MARIADB_ALL;
  static bool hparse_is_in_subquery= false;

/* Suppress useless messages
   https://bugreports.qt.io/browse/QTBUG-57180  (Windows startup)
   https://github.com/MartinBriza/QGnomePlatform/issues/23 (Fedora)
   Todo: consider also suppressing "OpenType support missing for script" */
#if (QT_VERSION >= 0x50000)
void dump_qtmessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  if (type == QtWarningMsg)
  {
    if (msg.contains("CreateFontFaceFromHDC")) return;
    if (msg.contains("GtkDialog mapped without a transient parent")) return;
  }
  QByteArray localMsg = msg.toLocal8Bit();
  printf("Qt message: %u: %s.\n", context.line, localMsg.constData());
  if (type == QtFatalMsg) abort();
}
#endif

int main(int argc, char *argv[])
{
#if (defined(_WIN32) && (QT_VERSION >= 0x50000))
  qInstallMessageHandler(dump_qtmessage);
#endif
  QApplication main_application(argc, argv);
  MainWindow w(argc, argv);
  /* We depend on w being maximized in resizeEvent() */
  w.showMaximized();
  return main_application.exec();
}


MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  /* Maximum QString length = sizeof(int)/4. Maximum LONGBLOB length = 2**32. So 32 bit int is ok. */
  assert(sizeof(int) >= 4);

#ifdef OCELOT_OS_LINUX
  assert(MENU_FONT == 80); /* See kludge alert in ocelotgui.h Settings() */
#endif

  assert(TOKEN_REFTYPE_MAX == 91); /* See comment after ocelotgui.h TOKEN_REFTYPE_MAX */

  /* Initialization */

  main_window_maximum_width= 0;
  main_window_maximum_height= 0;
  main_token_max_count= 0;

  /*
    setupUi() is in ui_ocelotgui.h, which is generated by a tool named "uic",
    which is called by qmake because there is a line in ocelotgui.pro:
    FORMS    += ocelotgui.ui
    I would love to get rid of the file and just put setupUi in ocelotgui.h,
    but different versions of qmake produce different contents for ui_ocelotgui.h.
    This is the routine that attaches the menubar to MainWindow, among other things.
  */
  ui->setupUi(this);

/*
  The Menu Bar did not appear with Ubuntu 14.04.
  This might be the horrific bug
  https://bugs.launchpad.net/ubuntu/+source/appmenu-qt5/+bug/1307619
  But QT_QPA_PLATFORMTHEME= did not solve.
  So insist that the menu goes in MainWindow.
*/
#ifdef OCELOT_OS_LINUX
  ui->menuBar->setNativeMenuBar(false);
#endif

  setWindowTitle("ocelotgui");

  connections_is_connected[0]= 0;
  mysql_res= 0;

  /* client variable defaults */
  /* Most settings done here might be overridden when connect_mysql_options_2 reads options. */
  ocelot_dbms= "mysql";
  connections_dbms[0]= DBMS_MYSQL;
  ocelot_grid_max_row_lines= 5; /* obsolete? */               /* maximum number of lines in 1 row. warn if this is exceeded? */
  ocelot_statement_prompt_background_color= s_color_list[COLOR_LIGHTGRAY*2 + 1]; /* set early because initialize_widget_statement() depends on this */
  ocelot_grid_border_color= s_color_list[COLOR_BLACK*2 + 1];;
  ocelot_grid_header_background_color= s_color_list[COLOR_LIGHTGRAY*2 + 1];
  ocelot_grid_cell_drag_line_color= s_color_list[COLOR_LIGHTBLUE*2 + 1];;
  ocelot_grid_cell_border_color= s_color_list[COLOR_BLACK*2 + 1];
  ocelot_grid_cell_border_size= "1";
  ocelot_grid_cell_drag_line_size= "5";
  /* Probably result_grid_table_widget_saved_font only matters if the connection dialog box has to go up. */
  QFont tmp_font;
//  QFont *saved_font;
  tmp_font= this->font();
//  saved_font=&tmp_font;
//  result_grid_table_widget= new ResultGrid(0, saved_font, this);

  result_grid_tab_widget= new QTabWidget48(this); /* 2015-08-25 added "this" */
  result_grid_tab_widget->hide();

  main_layout= new QVBoxLayout();

  history_edit_widget= new TextEditHistory(this);         /* 2015-08-25 added "this" */
  statement_edit_widget= new CodeEditor(this);
  statement_edit_widget->is_debug_widget= false;

  QFont fixed_font= get_fixed_font();
  history_edit_widget->setFont(fixed_font);
  statement_edit_widget->setFont(fixed_font);


#ifdef DEBUGGER
  create_widget_debug();
#endif

  hparse_f_parse_hint_line_create();

  main_window= new QWidget(this);                  /* 2015-08-25 added "this" */

  /*
    Defaults for items that can be changed with Settings menu item.
    First we get current values, after processing of any Qt options on the command line.
    Then we make arbitrary colors for details that current values probably didn't supply.
    The settings Red=string DarkGreen=Comment Green=object Blue=keyword DarkGray=operator
    have been seen elsewhere, and also pink|magenta=built-in-function so keep that in reserve.
    Then we read option files.
  */
  set_current_colors_and_font(fixed_font); /* set ocelot_statement_text_color, ocelot_grid_text_color, etc. */
  ocelot_statement_border_color= s_color_list[COLOR_BLACK*2 + 1];

  ocelot_statement_highlight_literal_color= s_color_list[COLOR_RED*2 + 1];
  ocelot_statement_highlight_identifier_color= s_color_list[COLOR_GREEN*2 + 1];
  ocelot_statement_highlight_comment_color= s_color_list[COLOR_LIMEGREEN*2 + 1];
  ocelot_statement_highlight_operator_color= s_color_list[COLOR_DARKGRAY*2 + 1];;
  ocelot_statement_highlight_keyword_color= s_color_list[COLOR_BLUE*2 + 1];
  ocelot_statement_highlight_current_line_color= s_color_list[COLOR_YELLOW*2 + 1];
  ocelot_statement_highlight_function_color= s_color_list[COLOR_FUCHSIA*2 + 1];
  ocelot_statement_syntax_checker= "1";
  ocelot_statement_format_statement_indent= "2";
  ocelot_statement_format_clause_indent= "4";
  ocelot_statement_format_keyword_case= "upper";

  ocelot_history_border_color= s_color_list[COLOR_BLACK*2 + 1];
  ocelot_menu_border_color= s_color_list[COLOR_BLACK*2 + 1];

  lmysql= new ldbms();                               /* lmysql will be deleted in action_exit(). */

  /* picking up possible settings options from argc+argv after setting initial defaults, so late */
  /* as a result, ocelotgui --version and ocelotgui --help will look slow */
  connect_mysql_options_2(argc, argv);               /* pick up my.cnf and command-line MySQL-related options, if any */

  if (ocelot_version != 0)                           /* e.g. if user said "ocelotgui --version" */
  {
    print_version();
    exit(0);
  }

  if (ocelot_help != 0)                              /* e.g. if user said "ocelotgui --help" */
  {
    print_help();
    exit(0);
  }

  for (int q_i= 0; strcmp(string_languages[q_i]," ") > 0; ++q_i)
  {
    QString s= string_languages[q_i];
    if (QString::compare(ocelot_language, s, Qt::CaseInsensitive) == 0)
    {
      er_off= ER_END * q_i;
      color_off= COLOR_END * q_i;
      menu_off= MENU_END * q_i;
    }
  }

  set_dbms_version_mask(ocelot_dbms);

  for (int q_i= color_off; strcmp(s_color_list[q_i]," ") > 0; ++q_i) q_color_list.append(s_color_list[q_i]);
  assign_names_for_colors();

  make_style_strings();
  //main_window->setStyleSheet(ocelot_menu_style_string);
  ui->menuBar->setStyleSheet(ocelot_menu_style_string);
  initialize_widget_history();

  initialize_widget_statement();
  result_grid_add_tab();

  main_layout->addWidget(history_edit_widget);
  main_layout->addWidget(result_grid_tab_widget);
  main_layout->addWidget(statement_edit_widget);
#ifdef DEBUGGER
  main_layout->addWidget(debug_top_widget);
#endif
  main_layout->addWidget(hparse_line_edit);

  main_window->setLayout(main_layout);

  setCentralWidget(main_window);

  create_menu();    /* Do this at a late stage because widgets must exist before we call connect() */

  /*
    If the command-line option was -p but not a password, then password input is necessary
    so put up the connection dialog box. Otherwise try immediately to connect.
    Todo: better messages so the user gets the hint: connection is necessary / password is necessary.
  */
  if (ocelot_password_was_specified == 0)
  {
    statement_edit_widget->insertPlainText("CONNECT");
    action_execute(1);
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
  We want to know the maximum widget size for dialog boxes.
  We can get this from the second time resizeEvent happens
  on MainWindow, which is asserted to be maximized.
*/
void MainWindow::resizeEvent(QResizeEvent *ev)
{
  (void) ev; /* suppress "unused parameter" warning */
  QSize main_window_size= size();
  if (main_window_size.width() > main_window_maximum_width)
    main_window_maximum_width= main_window_size.width();
  if (main_window_size.height() > main_window_maximum_height)
    main_window_maximum_height= main_window_size.height();
}

/*
  Normally there is only one result set, and therefore only one tab,
  and we hide the tabbing if there is only one tab. Abnormally there
  is more than one because a stored procedure does more than one SELECT.
  We call this at start, and we call this when we need a new tab.
  We never delete. That might be bad because Settings changes will be slower.
  We have:
  ocelot_grid_tabs. The maximum. Default 16. Settable with --ocelot_grid_tabs=n.
  result_grid_tab_widget. What we add the tabs to.
  ocelot_grid_actual_tabs. When we successfully add a tab, this goes up.
  Return 0 for success, 1 for failure (failure is probably because we hit maximum).
*/
int MainWindow::result_grid_add_tab()
{
  ResultGrid *r;
  int i_r= ocelot_grid_actual_tabs;
  if ((ocelot_grid_actual_tabs >= ocelot_grid_tabs)
  && (ocelot_grid_tabs != 0))
    return 1;
  {
    r= new ResultGrid(lmysql, this, true);
    int new_tab_index= result_grid_tab_widget->addTab(r, QString::number(i_r + 1));
    assert(new_tab_index == i_r);
    r->hide(); /* Maybe this isn't necessary */
  }
  {
    /* Todo: is this pointless? I don't catch fontchange anyway! */
    r= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(i_r));
    assert(r != 0);
    r->installEventFilter(this); /* must catch fontChange, show, etc. */
    r->grid_vertical_scroll_bar->installEventFilter(this);
  }
  {
    r= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(i_r));
    r->set_all_style_sheets(ocelot_grid_style_string, ocelot_grid_cell_drag_line_size, 0, false);
  }
  ++ocelot_grid_actual_tabs;
  return 0;
}

/*
  Initialize statement_edit_widget. Assume "statement_edit_widget= new CodeEditor();" already done.
  All user SQL input goes into statement_edit_widget.
  This will be a CodeEditor, which is a class derived from QTextEdit, with a prompt on the left.
  Todo: Check: after font change or main window resize, are line numbers and text in sync?
*/
void MainWindow::initialize_widget_statement()
{

  statement_edit_widget_setstylesheet();

  statement_edit_widget->setLineWrapMode(QPlainTextEdit::NoWrap);
  /* statement_edit_widget->setAcceptRichText(false); */ /* Todo: test whether this works */
  connect(statement_edit_widget->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(action_statement_edit_widget_text_changed(int,int,int)));
  statement_edit_widget_text_changed_flag= 0;

  /*
    Defaults.
    The prompt can be changed with a "prompt" statement or by my.cnf contents.
    The widget-left i.e. prompt bgcolor can be changed with a menu item.
  */
  statement_edit_widget->statement_edit_widget_left_bgcolor= QColor(ocelot_statement_prompt_background_color);
  statement_edit_widget->statement_edit_widget_left_treatment1_textcolor= QColor(ocelot_statement_text_color);

  statement_edit_widget->prompt_default= ocelot_prompt;     /* Todo: change to "\N [\d]>"? */
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
  statement_edit_widget->setMouseTracking(true);
}


/* Statement widget and debug widgets both use ocelot_statement_style_string. */
void MainWindow::statement_edit_widget_setstylesheet()
{
  statement_edit_widget->setStyleSheet(ocelot_statement_style_string);

  for (int debug_widget_index= 0; debug_widget_index < DEBUG_TAB_WIDGET_MAX; ++debug_widget_index)
  {
    if (debug_widget[debug_widget_index] != 0)
    {
      debug_widget[debug_widget_index]->setStyleSheet(ocelot_statement_style_string);
    }
  }
}

/*
  Formatter -- minimal, but this format might be okay for debugger users.
  It only works if the statement has been recognized.
  All we're trying to accomplish is
    "different statements different lines"
    "block start causes indent"
    "keywords upper case"
    "lists of columns lined up in select/update/insert/set".
  Cursor will go back to start.
  Indentation is meaningless unless there is a fixed font.
  Todo: Formatter is on the Edit menu but there is no shortcut key.
      It would be easy to add a shortcut key with setShortcut()
      or add a line in eventfilter() e.g.
      if (key->key() == Qt::Key_1) { statement_edit_widget_formatter(); return true; }
      But I haven't decided what the shortcut key should be.
  Todo: stop giving up when you see DELIMITER, it can be figured out.
*/
void MainWindow::statement_edit_widget_formatter()
{
  if (((ocelot_statement_syntax_checker.toInt()) & FLAG_FOR_HIGHLIGHTS) == 0) return;

  int *output_offsets;
  int i;
  for (i= 0; main_token_lengths[i] != 0; ++i) ;
  output_offsets= new int[i + 1];

  QString text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */
  QString output= "";
  int indent_base= 0;
  int statement_indent= ocelot_statement_format_statement_indent.toInt();
  int clause_indent= ocelot_statement_format_clause_indent.toInt();
  QString keyword_case= ocelot_statement_format_keyword_case.toLower();
  int token;
  QString s;
  int token_type;
  for (i= 0; main_token_lengths[i] != 0; ++i)
  {
    if ((main_token_flags[i] & TOKEN_FLAG_IS_ERROR) != 0)
    {
      int st= 0;
      if (i > 0) st= main_token_offsets[i - 1] + main_token_lengths[i - 1];
      s= text.right(text.length() - st);
      output.append(s);
      break; /* Give up */
    }
    token_type= 0;
    token= main_token_types[i];
    if (token == TOKEN_TYPE_DELIMITER)
    {
      QString d= text.mid(main_token_offsets[i], main_token_lengths[i]);
      output.append(d);
      continue;
    }
    if (text.mid(main_token_offsets[i], 2).toUpper() == "\\G")
    {
      output.append(text.mid(main_token_offsets[i], 2));
      ++i;
      continue;
    }
    if ((main_token_flags[i] & TOKEN_FLAG_IS_START_IN_COLUMN_LIST) != 0)
      output_offsets[i]= output.length();
    if ((main_token_flags[i] & TOKEN_FLAG_IS_END_IN_COLUMN_LIST) != 0)
    {
      output.append(",");
      /* indent to where the column list began */
      int indent_of_column= 0;
      for (int k= i - 1; k >= 0; --k)
      {
        if ((main_token_flags[k] & TOKEN_FLAG_IS_START_IN_COLUMN_LIST) != 0)
        {
          int l= output_offsets[k];
          for (; (l < output.length()) && (output.mid(l, 1) <= " "); ++l) ;
          for (int m= l; ; --m)
          {
            if ((output.mid(m, 1) == "\n") || (m == 0))
            {
              if (m != 0) ++m;
              indent_of_column= l - m;
              break;
            }
          }
          break;
        }
      }
      output.append("\n");
      for (int n= 0; n < indent_of_column; ++n) output.append(" ");
    }
    else
    {
      if (token == TOKEN_KEYWORD_DELIMITER)
      {
        if (output != "") output.append("\n");
        QString d;
        d= text.mid(main_token_offsets[i], main_token_lengths[i]);
        QString e= d.toUpper();
        if (e == "DELIMITER") output.append(e);
        else output.append(d);
        if (main_token_lengths[i + 1] != 0)
        {
          int j= main_token_offsets[i] + main_token_lengths[i];
          d= text.mid(j, main_token_offsets[i + 1] - j);
          if (d.contains("\n"))
          {
            continue; /* delimiter \n = an error, but possible */
          }
          output.append(" ");
          ++i;
          d= text.mid(main_token_offsets[i], main_token_lengths[i]);
          output.append(d);
          while ((main_token_lengths[i + 1] != 0)
           && (main_token_offsets[i + 1] == main_token_offsets[i] + main_token_lengths[i]))
          {
            d= text.mid(main_token_offsets[i + 1], main_token_lengths[i + 1]);
            output.append(d);
            ++i;
          }
          continue;
        }
      }
      if (token == TOKEN_KEYWORD_PROMPT)
      {
        if (output != "") output.append("\n");
        QString d;
        d= text.mid(main_token_offsets[i], main_token_lengths[i]);
        QString e= d.toUpper();
        if (e == "PROMPT") output.append(e);
        else output.append(d);
        int k= i + 1;
        while ((main_token_lengths[k] != 0)
            && ((main_token_flags[k] & TOKEN_FLAG_IS_START_STATEMENT) == 0)
            && ((main_token_flags[k] & TOKEN_FLAG_IS_ERROR) == 0))
          ++k;
        --k;
        if (k != i)
        {
          int j;
          j= main_token_offsets[k] - (main_token_offsets[i] + main_token_lengths[i]) + main_token_lengths[k];
          d= text.mid(main_token_offsets[i] + main_token_lengths[i], j);
          output.append(d);
        }
        i= k;
        continue;
      }
      if (((main_token_flags[i] & TOKEN_FLAG_IS_START_STATEMENT) != 0)
       || ((main_token_flags[i] & TOKEN_FLAG_IS_START_CLAUSE) != 0))
      {
        token_type= 1;
        if ((token == TOKEN_KEYWORD_BEGIN)
        || (token == TOKEN_KEYWORD_CASE)
        || (token == TOKEN_KEYWORD_FOR_IN_FOR_STATEMENT) /* will only be true if MariaDB 10.3 */
        || (token == TOKEN_KEYWORD_IF)
        || (token == TOKEN_KEYWORD_LOOP)
        || (token == TOKEN_KEYWORD_REPEAT)
        || (token == TOKEN_KEYWORD_WHILE))
          token_type= 3;
      }
      if (token_type > 0)
      {
        /* todo: check if (not already at indent-base) */
        if (output != "") output.append("\n");
        for (int k= 0; k < indent_base; ++k) output.append(" ");
        if ((main_token_flags[i] & TOKEN_FLAG_IS_START_CLAUSE) != 0)
        {
          for (int k= 0; k < clause_indent; ++k) output.append(" ");
        }
        if (token == TOKEN_KEYWORD_END)
        {
          indent_base-= statement_indent;
        }
        if (token_type == 3)
        {
          indent_base+= statement_indent;
        }
      }
      s= text.mid(main_token_offsets[i], main_token_lengths[i]);
      QString p= "";
      int p_flags= 0;
      int p_type= 0;
      if (i > 0)
      {
        p= text.mid(main_token_offsets[i - 1], main_token_lengths[i - 1]);
        p_flags= main_token_flags[i - 1];
        p_type= main_token_types[i - 1];
      }

      if (output.right(1) == "\\")
      {
        if (main_token_offsets[i] == main_token_offsets[i - 1] + main_token_lengths[i - 1])
        {
          output.append(s);
          continue;
        }
      }

      if (output.right(1) > " ")
      {
        if (main_token_types[i] == TOKEN_TYPE_OPERATOR)
        {
          if ((s == ",") || (s == ";") || (s == ")") || (s == "}") || (s == ".") || (s == ":")) {;}
          else if (s == "(")
          {
            /* no space between function name and '('. mandatory. */
            if ((p_flags & TOKEN_FLAG_IS_FUNCTION) != 0)
            {
              ;
            }
            else if (p_type == TOKEN_TYPE_IDENTIFIER)
            {
              ;
            }
            else if ((p_flags & TOKEN_FLAG_IS_DATA_TYPE) != 0)
            {
              ;
            }
            else if ((p != "(")
             && ((main_token_flags[i] & TOKEN_FLAG_IS_NOT_AFTER_SPACE) == 0))
              output.append(" ");
          }
          else output.append(" ");
        }
        else
        {
          if ((p == "(") || (p == "{") || (p == ".") || (p == "@") || (p == "~")) {;}
          else if ((p == "-") || (p == "+"))
          {
            if ((main_token_flags[i - 1] & TOKEN_FLAG_IS_BINARY_PLUS_OR_MINUS) != 0)
            {
              output.append(" ");
            }
          }
          else if (s == "@") {;}
          else output.append(" ");
        }
      }
      if (main_token_types[i] >= TOKEN_KEYWORDS_START)
      {
        if (keyword_case == "upper") s= s.toUpper();
        if (keyword_case == "lower") s= s.toLower();
      }
      output.append(s);
    }
  }
  /* Change statement widget contents with possibility of "undo" later */
  QTextCursor cur(statement_edit_widget->textCursor());
  cur.beginEditBlock();
  cur.setPosition(0);
  cur.select(QTextCursor::Document);
  cur.removeSelectedText();
  cur.insertText(output);
  cur.endEditBlock();
  delete [] output_offsets;
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
  Note: statement_edit_widget mouseMoveEvent handling is in ocelotgui.h
  Todo: Consider: Perhaps this should not be in MainWindow:: but in CodeEditor::.
  Todo: Consider: Perhaps this should be a menu item, not a filter event.
                  (There's already a menu item, but it's not for Enter|Return.)
  There are a few "Ocelot keyword" items that do not require ";" or delimiter
  provided they're the first word, for example "QUIT".
*/
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
  return eventfilter_function(obj, event);
}

bool MainWindow::eventfilter_function(QObject *obj, QEvent *event)
{
//  if (obj == result_grid_table_widget[0]->grid_vertical_scroll_bar)
//  {
//    /*
//      Probably some of these events don't cause scroll bar value to change,
//      I've only seen that happen for MouseMove and MouseButtonRelease.
//      But that's harmless, it only means we call scroll_event for nothing.
//    */
//    if ((event->type() == QEvent::KeyPress)
//    ||  (event->type() == QEvent::KeyRelease)
//    ||  (event->type() == QEvent::MouseButtonDblClick)
//    ||  (event->type() == QEvent::MouseButtonPress)
//    ||  (event->type() == QEvent::MouseButtonRelease)
//    ||  (event->type() == QEvent::MouseMove)
//    ||  (event->type() == QEvent::MouseTrackingChange)) return (result_grid_table_widget->scroll_event());
//  }
  QString text;

  if (event->type() == QEvent::FocusIn)
  {
    menu_activations(obj, QEvent::FocusIn);
    return false;
  }

  if (event->type() == QEvent::FocusOut)
  {
    menu_activations(obj, QEvent::FocusOut);
    return false;
  }

  {
    ResultGrid* r;
    for (int i_r= 0; i_r < ocelot_grid_actual_tabs; ++i_r)
    {
      r= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(i_r));
      if (obj == r)
      {
        if (event->type() == QEvent::FontChange) return (r->fontchange_event());
        if (event->type() == QEvent::Show) return (r->show_event());
      }
      if (obj == r->grid_vertical_scroll_bar)
      {
        return (r->vertical_scroll_bar_event(connections_dbms[0]));
      }
    }
  }
#ifdef DEBUGGER
  for (int debug_widget_index= 0; debug_widget_index < DEBUG_TAB_WIDGET_MAX; ++debug_widget_index)
  {
    if (obj == debug_widget[debug_widget_index])
    {
      if (event->type() == QEvent::MouseButtonPress)
      {
        action_debug_mousebuttonpress(event, debug_widget_index);
        return false;
      }
    }
  }
#endif
  if (event->type() != QEvent::KeyPress) return false;
  QKeyEvent *key= static_cast<QKeyEvent *>(event);
  /* See comment with label "Shortcut Duplication" */
  if (keypress_shortcut_handler(key, false) == true) return true;
  if (obj != statement_edit_widget) return false;
  if ((key->key() != Qt::Key_Enter) && (key->key() != Qt::Key_Return)) return false;
  /* No delimiter needed if Ctrl+Enter, which we'll regard as a synonym for Ctrl+E */
  if (key->modifiers() & Qt::ControlModifier)
  {
    action_execute(1);
    return true;
  }

  /*
    It's just Enter.
    If it's not at the end, not counting white space, don't try to execute.
    Anyway, don't force.
  */
  if (statement_edit_widget->textCursor().atEnd() == false)
  {
    int cursor_position= statement_edit_widget->textCursor().position();
    QString plain_text= statement_edit_widget->toPlainText();
    for (int i= cursor_position; i < plain_text.size(); ++i)
    {
      if (plain_text.mid(i, 1) > " ") return false;
    }
  }
  if (action_execute(0) == 1) return false;
  return true;
}

/*
  There are two reasons for keypress_shortcut_handler() to exist:
  1: see comment = "Shortcut duplication"
  2: we call not only from MainWindow::eventfilter_function()
     but also from TextEditWidget::keyPressEvent().
  If the keypress is a shortcut, we handle it and return true.
  If it's not, then it's probably text input, we return false.
  There is one situation that we do not handle:
    TextEditWidget handles ocelot_shortcut_copy_keysequence separately.
  Todo: Maybe event->matches() is a more standard way to compare.
  Todo: Now there's useless code -- we have setShortcut() for most of
        these combinations, but keypress_shortcut_handler() happens
        first (?? I think), so they're useless. But a few things are
        still handled by shortcuts, e.g. format, which is harmless
        because there's only one widget that format works with.
  Todo: can we get here for a disabled menu item?
  Todo: if e.g. menu_edit_undo() does nothing because it can't find the
        classname, it should return false so keypress_shortcut_handler()
        can return false. Currently these things are all void.
  Todo: the system hasn't been tested with detached debugger widgets
        for which we've said SET ocelot_shortcut_...='something odd'.
*/
bool MainWindow::keypress_shortcut_handler(QKeyEvent *key, bool return_true_if_copy)
{
  Qt::KeyboardModifiers modifiers= key->modifiers();
  int qki= 0;
  if ((modifiers & Qt::ControlModifier) != 0) qki= (qki | Qt::CTRL);
  if ((modifiers & Qt::ShiftModifier) != 0) qki= (qki | Qt::SHIFT);
  if ((modifiers & Qt::AltModifier) != 0) qki= (qki | Qt::ALT);
  if ((modifiers & Qt::MetaModifier) != 0) qki= (qki | Qt::META);
  QKeySequence qk= QKeySequence(key->key() | qki);
  if ((qk == ocelot_shortcut_copy_keysequence)
   && (return_true_if_copy)) return true;
  if (qk == ocelot_shortcut_connect_keysequence) { action_connect(); return true; }
  if (qk == ocelot_shortcut_exit_keysequence) { action_exit(); return true; }
  if (qk == ocelot_shortcut_undo_keysequence) { menu_edit_undo(); return true; }
  if (qk == ocelot_shortcut_redo_keysequence) { menu_edit_redo(); return true; }
  if (qk == ocelot_shortcut_cut_keysequence) { menu_edit_cut(); return true; }
  if (qk == ocelot_shortcut_copy_keysequence) { menu_edit_copy(); return true; }
  if (qk == ocelot_shortcut_paste_keysequence) { menu_edit_paste(); return true; }
  if (qk == ocelot_shortcut_select_all_keysequence) { menu_edit_select_all(); return true; }
  if (qk == ocelot_shortcut_history_markup_previous_keysequence) { history_markup_previous(); return true; }
  if (qk == ocelot_shortcut_history_markup_next_keysequence) { history_markup_next(); return true; }
  if (qk == ocelot_shortcut_execute_keysequence){ action_execute(1); return true; }
  if (qk == ocelot_shortcut_zoomin_keysequence){menu_edit_zoomin(); return true; }
  if (qk == ocelot_shortcut_zoomout_keysequence){menu_edit_zoomout(); return true; }
  if (menu_edit_action_autocomplete->isEnabled() == true)
  {
    if (qk == ocelot_shortcut_autocomplete_keysequence) {
      return menu_edit_autocomplete(); }
  }
  if (menu_run_action_kill->isEnabled() == true)
  {
    if (qk == ocelot_shortcut_kill_keysequence) { action_kill(); return true; }
  }
  if (qk == ocelot_shortcut_next_window_keysequence){action_option_next_window(); return true; }
  if (qk == ocelot_shortcut_previous_window_keysequence){action_option_previous_window(); return true; }
  if (menu_debug_action_breakpoint->isEnabled())
    if (qk == ocelot_shortcut_breakpoint_keysequence) { action_debug_breakpoint(); return true; }
  if (menu_debug_action_continue->isEnabled())
    if (qk == ocelot_shortcut_continue_keysequence) { action_debug_continue(); return true; }
  if (menu_debug_action_next->isEnabled())
    if (qk == ocelot_shortcut_next_keysequence) { action_debug_next(); return true; }
  if (menu_debug_action_step->isEnabled())
    if (qk == ocelot_shortcut_step_keysequence) { action_debug_step(); return true; }
  if (menu_debug_action_clear->isEnabled())
    if (qk == ocelot_shortcut_clear_keysequence) { action_debug_clear(); return true; }
  if (menu_debug_action_exit->isEnabled())
    if (qk == ocelot_shortcut_debug_exit_keysequence) { action_debug_exit(); return true; }
  if (menu_debug_action_information->isEnabled())
    if (qk == ocelot_shortcut_information_keysequence) { action_debug_information(); return true; }
  if (menu_debug_action_refresh_server_variables->isEnabled())
    if (qk == ocelot_shortcut_refresh_server_variables_keysequence) { action_debug_refresh_server_variables(); return true; }
  if (menu_debug_action_refresh_user_variables->isEnabled())
    if (qk == ocelot_shortcut_refresh_user_variables_keysequence) { action_debug_refresh_user_variables(); return true; }
  if (menu_debug_action_refresh_variables->isEnabled())
    if (qk == ocelot_shortcut_refresh_variables_keysequence) { action_debug_refresh_variables(); return true; }
  if (menu_debug_action_refresh_call_stack->isEnabled())
    if (qk == ocelot_shortcut_refresh_call_stack_keysequence) { action_debug_refresh_call_stack(); return true; }
  return false;
}


/*
  We want to know: do we have a complete statement at the string start.
  So we find out how many tokens are in the (first) statement.
  We go forward to find the first non-comment token, the statement type.
  We go backward to find the final non-comment token, the ; or delimiter -- or not.
  Using these, and knowing what is ocelot_delimiter_str, we can decide if it's complete.
  We also check for \G or \g.
*/

bool MainWindow::is_statement_complete(QString text)
{
  int number_of_tokens_in_statement= 0;
  int returned_begin_count= 0;
  int i= 0;
  int first_token_type= -1;
  int first_token_i= 0;
  //int last_token_type= -1;
  QString first_token= "";
  QString last_token= "";
  QString second_last_token= "";
  number_of_tokens_in_statement= get_next_statement_in_string(0, &returned_begin_count, false);
  for (i= 0; i < number_of_tokens_in_statement; ++i)
  {
    int t= main_token_types[i];
    if (t == TOKEN_TYPE_COMMENT_WITH_SLASH) continue;
    if (t == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE) continue;
    if (t == TOKEN_TYPE_COMMENT_WITH_MINUS) continue;
    first_token_type= t;
    first_token= text.mid(main_token_offsets[i], main_token_lengths[i]);
    first_token_i= i;
    break;
  }
  for (i= number_of_tokens_in_statement - 1; i >= 0; --i)
  {
    int t= main_token_types[i];
    if (t == TOKEN_TYPE_COMMENT_WITH_SLASH) continue;
    if (t == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE) continue;
    if (t == TOKEN_TYPE_COMMENT_WITH_MINUS) continue;
    //last_token_type= t;
    last_token= text.mid(main_token_offsets[i], main_token_lengths[i]);
    if (i > 0) second_last_token= text.mid(main_token_offsets[i - 1], main_token_lengths[i - 1]);
    break;
  }
  /* No delimiter needed if first word in first statement of the input is an Ocelot keyword e.g. QUIT */
  /* Todo: Check: does this mean that a client statement cannot be spread over two lines? */
  if (is_client_statement(first_token_type, first_token_i, text) == true)
  {
    return true;
  }

  /* "go" or "ego", alone on the line, if --named-commands, is statement end */
  /* Todo: but these are client statements so we won't ever get here */
  /* Todo: I forget why we care. Is it in fact because the token is \G or \g? */
  if ((ocelot_named_commands > 0)
  && ((first_token_type == TOKEN_KEYWORD_GO) || (first_token_type == TOKEN_KEYWORD_EGO)))
  {
    QString q;
    for (int i_off= main_token_offsets[i] - 1;; --i_off)
    {
      if (i_off < 0) q= "\n";
      else q= text.mid(i_off, 1);
      if ((q == "\n") || (q == "\r"))
      {
        return true;
      }
      if (q != " ") break;
    }
  }

  /* if create-routine && count-of-ENDs == count-of-BEGINS then ; is the end else ; is not the end */
  if (ocelot_delimiter_str != ";") returned_begin_count= 0;
  else
  {
    int token_count= get_next_statement_in_string(0, &returned_begin_count, false);
    if (returned_begin_count == 0)
    {
      if (QString::compare(last_token, ";", Qt::CaseInsensitive) == 0)
      {
        /* Warning: this only works if we've gone through hparse */
        /* plsql routines might have var|cursor|condition before begin */
        if ((main_token_flags[token_count - 1] & TOKEN_FLAG_IS_PLSQL_DECLARE_SEMICOLON) != 0)
        {
          for (int k= token_count - 1;;--k)
          {
            if (main_token_types[k] == TOKEN_KEYWORD_BEGIN) break;
            if (k == 0)
            {
              /* saw no begin but flag counts as a begin */
              ++returned_begin_count;
              break;
            }
          }
        }
        if (returned_begin_count == 0) return true;
      }
      if (QString::compare(last_token, "G", Qt::CaseInsensitive) == 0)
      {
        if (QString::compare(second_last_token, "\\", Qt::CaseInsensitive) == 0)
        {
          return true;
        }
      }
    }
  }
  if (last_token != ocelot_delimiter_str)
  {
    return false;
  }

  if (returned_begin_count > 0)
  {
    return false;
  }
  /* All conditions have been met. Tell caller to Execute, and eat the return key. */
  return true;
}


/*
  History
  =======

  The history widget history_edit_widget is an editable subclass of QTextEdit
  which contains retired statements + errors/warnings, scrolling
  so it looks not much different from the mysql retired-statement
  scrolling.
  However, our history does not include result sets (unless
  result_set_for_history, described later).
  These user-settable variables affect history_edit_widget:
  ocelot_history_text|background|border_color      default = system
  ocelot_history_font_family|size|style|weight     default = system
  ocelot_history_includes_warnings                 default = 0 (no)
  ocelot_history_max_row_count                     default = 0 (suppressed)

  The statement is always followed by an error message,
  but ocelot_history_includes_warnings is affected by ...
  warnings  (\W) Show warnings after every statement.
  nowarning (\w) Don't show warnings after every statement.
  If the prompt is included, then we should be saving time-of-day
  for the first statement-line prompt and doing prompt_translate()
  for each line of the statement when we copy it out.
  The history_edit_widget is TextEditHistory which is derived from
  QTextEdit, differing from statement_edit_widget which is CodeEditor
  which is derived from QPlainTextEdit.

  History menu items / commands:
  * The usual edit menu items = cut, copy, paste, etc.
    Therefore we don't really need to limit history size, users can clear.
  * Previous Statement ^P and Next Statement ^N
    ? Possible alternatives: alt+up-arrow|alt+down-arrow, PgUp|PgDn
    ? Possible alternative: up-arrow if we're at top of statement widget
      (which is more like what mysql client would do)
    * disable if there's no previous / next
    * if done on statement widget, brings in from history -- dunno if history should scroll when that happens
    ? when bringing in from history to statement, don't re-execute
    * if user executes a restated statement, there's a behaviour choice: now the restated statement is last statement, or now the last-picked statement is still last statement
  * Open | Save | Close | Delete (not implemented)
    * file items, with the intent of keeping history
    * you can go back in history by going back in a file
    * format should be the same as MySQL log history
      ? but in that case, should display look like MySQL log too?
    * Save means "save what's being shown, so it's affected by Hide
  * Find | Goto
    * you need these menu items for history soon, so might as well have them for statements too
  * Hide prompt + Hide result + Hide result set
  + Settings: History Widget has different colors for prompt/statement/result/result set

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
  Unlike statement_edit_widget, history_edit_widget is TextEditHistory
  derived from QTextEdit and allows rich text, editable, containing HTML.
  Each history entry has: prompt, statement, result, and possibly result set.
  (Result set depends on ocelot_history_max_row_count, 0 by default, and
  see copy_to_history, it's supposed to appear like mysql client result set.)
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

void MainWindow::initialize_widget_history()
{
  history_edit_widget->setStyleSheet(ocelot_history_style_string);
  history_edit_widget->setReadOnly(false);       /* if history shouldn't be editable, set to "true" here */
  history_edit_widget->hide();                   /* hidden until a statement is executed */
  history_markup_make_strings();
  history_edit_widget->installEventFilter(this); /* must catch focusIn */
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
/* is_interactive == false if we're reading from mysql_histfile during start */
/* Todo: right justify. Make it optional to show the prompt, unless prompt can be hidden. */
void MainWindow::history_markup_append(QString result_set_for_history, bool is_interactive)
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
  history_statement= "<i></i>";                                           /* hint that what's coming is HTML */
  history_statement.append(history_markup_statement_start);

  for (statement_line_index= 0; statement_line_index < statement_lines.count(); ++statement_line_index)
  {
    history_statement.append("<span style=\" background-color:");
    history_statement.append(ocelot_statement_prompt_background_color);
    history_statement.append(";\">");
    history_statement.append(history_markup_prompt_start);
    if (is_interactive == true) history_statement.append(history_markup_copy_for_history(statement_edit_widget->prompt_translate(statement_line_index + 1)));
    else history_statement.append("-");
    history_statement.append(history_markup_prompt_end);
    history_statement.append("</span>");
    history_statement.append(history_markup_copy_for_history(statement_lines[statement_line_index]));
    history_statement.append("<br>");
  }

  history_statement.append(history_markup_result);
  if (is_interactive == true)
    history_statement.append(history_markup_copy_for_history(statement_edit_widget->result)); /* the main "OK" or error message */
  else history_statement.append("--");
  if (result_set_for_history > "")
  {
    history_statement.append("<pre>");
    history_statement.append(history_markup_copy_for_history(result_set_for_history));
    history_statement.append("</pre>");
  }

  history_statement.append(history_markup_statement_end);

  history_edit_widget->append(history_statement);

  history_markup_counter= 0;

  if (is_interactive == false) return;

  /*  not related to markup, just a convenient place to call */
  history_file_write("TEE", query_utf16);
  history_file_write("HIST", query_utf16);
  history_file_write("TEE", statement_edit_widget->result);
  if (result_set_for_history > "")
    history_file_write("TEE", result_set_for_history);
}


/* When copying to history, change < and > and & and " to entities. */
/* Change on 2015-03-16: change newline to <br>. */
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
    if (c == "\n") c= "<br>";
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
  tee+hist
  --------
  TEE
  * Code related to tee should have the comment somewhere = "for tee"
  * bool ocelot_history_tee_file_is_open initially is false
  * the options --tee=filename and --no-tee exist, and they are checked (I think)
  * the client statements tee filename and notee will be seen
  * there are no menu options (todo: decide whether this is a flaw)
  * apparently the mysql client would flush, therefore we call flush() after writing
  * the mysql client would include results from queries, but we do so only for TEE (todo: decide whether this is a flaw)
  * there might be html in the output (todo: decide whether this is a flaw)
  HIST
  * read http://ocelot.ca/blog/blog/2015/08/04/mysql_histfile-and-mysql_history/
  * bool ocelot_history_hist_file_is_open initially is false but it's opened if successful connect
  * --batch or --silent or setting name to /deev/null turns history off
  EITHER
  * Ignore if filename is "", is "/dev/null", or is a link to "/dev/null"
*/
void MainWindow::history_file_write(QString history_type, QString text_line)  /* see comment=tee+hist */
{
  if (history_type == "TEE")
  {
    if (ocelot_history_tee_file_is_open == false) return;
  }
  else
  {
    if (ocelot_history_hist_file_is_open == false) return;
    /* see Wildcard Matching section in http://doc.qt.io/qt-4.8/qregexp.html */
    int qfrom= 0;
    int qindex;
    QString qs;
    for (;;)
    {
      QRegExp rx;
      if (qfrom >= ocelot_histignore.length()) break;
      qindex= ocelot_histignore.indexOf(":", qfrom, Qt::CaseInsensitive);
      if (qindex == -1) qindex= ocelot_histignore.length();
      qs= ocelot_histignore.mid(qfrom, qindex - qfrom);
      rx= QRegExp(qs);
      rx.setPatternSyntax(QRegExp::Wildcard);
      rx.setCaseSensitivity(Qt::CaseInsensitive);
      if (rx.exactMatch(text_line) == true) return;
      qfrom= qindex + 1;
    }
  }

  QString s= text_line;
  int query_len= s.toUtf8().size();                  /* See comment "UTF8 Conversion" */
  char *query= new char[query_len + 1];
  memcpy(query, s.toUtf8().constData(), query_len + 1);
  query[query_len]= '\0';                            /* todo: think: is this necessary? */
  if (history_type == "TEE")
  {
    ocelot_history_tee_file.write(query, strlen(query));
    ocelot_history_tee_file.write("\n", strlen("\n"));
    ocelot_history_tee_file.flush();
  }
  else
  {
    ocelot_history_hist_file.write(query, strlen(query));
    ocelot_history_hist_file.write("\n", strlen("\n"));
    ocelot_history_hist_file.flush();
  }
  delete []query;
}


int MainWindow::history_file_start(QString history_type, QString file_name) /* see comment=tee+hist */
{
  QString file_name_to_open;

  if (history_type == "TEE")
  {
    file_name_to_open= ocelot_history_tee_file_name;
    if (ocelot_history_tee_file_is_open == true)
    {
      ocelot_history_tee_file.close();
      ocelot_history_tee_file_is_open= false;
    }
  }
  else
  {
    file_name_to_open= ocelot_history_hist_file_name;
    if (ocelot_history_hist_file_is_open == true)
    {
      ocelot_history_hist_file.close();
      ocelot_history_hist_file_is_open= false;
    }
    if (ocelot_batch != 0) return 1;                     /* if --batch happened, no history */
    if (ocelot_silent != 0) return 1;                    /* if --silent happened, no history */
  }

  if (file_name != "") file_name_to_open= file_name;
  int query_len= file_name_to_open.toUtf8().size();  /* See comment "UTF8 Conversion" */
  char *query= new char[query_len + 1];
  memcpy(query, file_name_to_open.toUtf8().constData(), query_len + 1);
  query[query_len]= '\0';                            /* todo: think: is this necessary? */

  /* If file name == "/dev/null" or something that links to "/dev/null", don't try to open. */
  if (file_name_to_open == "/dev/null") { delete []query; return 0; }
#ifdef OCELOT_OS_LINUX
  char tmp_link_file[9 + 1];
  if (readlink(query, tmp_link_file, 9 + 1) == 9)
  {
    if (memcmp(tmp_link_file, "/dev/null", 9) == 0) {delete []query; return 0; }
  }
#endif


  bool open_result;
  if (history_type == "TEE")
  {
    ocelot_history_tee_file.setFileName(query);
    open_result= ocelot_history_tee_file.open(QIODevice::Append | QIODevice::Text);
    delete []query;
    if (open_result == false) return 0;
    ocelot_history_tee_file_is_open= true;
    ocelot_history_tee_file_name= file_name_to_open;
    return 1;
  }
  else
  {
    ocelot_history_hist_file.setFileName(query);
    open_result= ocelot_history_hist_file.open(QIODevice::Append | QIODevice::Text);
    delete []query;
    if (open_result == false) return 0;
    ocelot_history_hist_file_is_open= true;
    ocelot_history_hist_file_name= file_name_to_open;
    return 1;
  }
  return 0;
}


void MainWindow::history_file_stop(QString history_type)   /* see comment=tee+hist */
{
  if (history_type == "TEE")
  {
    ocelot_history_tee_file.close();
    ocelot_history_tee_file_is_open= false;
  }
  else
  {
    ocelot_history_hist_file.close();
    ocelot_history_hist_file_is_open= false;
  }
}


/*
  This is putting in the history widget, indeed, BUT ...
  2. The history widget seems to grow when I type something for the first time
  4. Make sure there's no disaster if file is /dev/null or blank.
  5. ^P doesn't work, and that's probably because we depend on markup to see statement start.
  TODO: Fix for ^P
  Nothing happens if --batch or --silent
  We try to open the history file during each connect.
  If we successfully open, but only the first time, we copy its lines to the history widget.
  3. We have to limit the number of lines, since the file might be big.
     So we do an fseek to the last 10000 bytes of the file, then skip the first line from there.
*/
#define HISTFILESIZE 10000
void MainWindow::history_file_to_history_widget()         /* see comment=tee+hist */
{
  FILE *history_file;
  if (ocelot_batch != 0) return;                          /* if --batch happened, no history */
  if (ocelot_silent != 0) return;                         /* if --silent happened, no history */
  if (ocelot_history_hist_file_is_copied == true) return;        /* we've alredy done this */
  //if (ocelot_history_hist_file_is_open == false) return;
  {
    int query_len= ocelot_history_hist_file_name.toUtf8().size();  /* See comment "UTF8 Conversion" */
    char *query= new char[query_len + 1];
    memcpy(query, ocelot_history_hist_file_name.toUtf8().constData(), query_len + 1);
    query[query_len]= '\0';                            /* todo: think: is this necessary? */
    history_file= fopen(query, "r");
    delete []query;
  }
  if (history_file == NULL) return;
  if (fseek(history_file, 0 , SEEK_END) == 0)
  {
    int file_size = ftell(history_file);
    if (file_size > HISTFILESIZE) fseek(history_file, file_size - HISTFILESIZE, SEEK_SET);
    else fseek(history_file, 0, SEEK_SET);
  }
  char line[4096];
  while(fgets(line, sizeof line, history_file) != NULL) /* put all non-"" in history  widget */
  {
    query_utf16= line;
    query_utf16= query_utf16.trimmed();
    if (query_utf16 > "") history_markup_append("", false);
  }
  fclose(history_file);
  ocelot_history_hist_file_is_copied= true;
  query_utf16= er_strings[er_off + ER_START_OF_SESSION];
}


/*
  Shortcut duplication
  Example:
    In create_menu():
      connect(menu_file_action_exit, SIGNAL(triggered()), this, SLOT(action_exit()));
      menu_file_action_exit->setShortcut(QKeySequence::Quit);
    In eventFilter():
      if (key->matches(QKeySequence::Quit))
      {
        action_exit();
        return true;
      }
  With Ubuntu 12.04 the menu shortcut is executed.
  With Ubuntu 15.04 the event filter is executed.
  That is: the duplication in the event filter is a workaround for the fact that,
  on some distro versions, some menu shortcuts are missed. Maybe it's a Qt bug.
  Maybe it's an Ubuntu bug: https://bugs.launchpad.net/ubuntu/+source/texmaker/+bug/1386111
*/

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
*/
void MainWindow::create_menu()
{
  menu_file= ui->menuBar->addMenu(menu_strings[menu_off + MENU_FILE]);
  /* Todo: consider adding fileMenu = new QMenu(tr("&File"), this); -*/
  menu_file_action_connect= menu_file->addAction(menu_strings[menu_off + MENU_FILE_CONNECT]);

  connect(menu_file_action_connect, SIGNAL(triggered()), this, SLOT(action_connect()));
  shortcut("ocelot_shortcut_connect", "", false, true);
  menu_file_action_exit= menu_file->addAction(menu_strings[menu_off + MENU_FILE_EXIT]);
  connect(menu_file_action_exit, SIGNAL(triggered()), this, SLOT(action_exit()));
  shortcut("ocelot_shortcut_exit", "", false, true);
  menu_edit= ui->menuBar->addMenu(menu_strings[menu_off + MENU_EDIT]);
  menu_edit_action_undo= menu_edit->addAction(menu_strings[menu_off + MENU_EDIT_UNDO]);
  connect(menu_edit_action_undo, SIGNAL(triggered()), this, SLOT(menu_edit_undo()));
  shortcut("ocelot_shortcut_undo", "", false, true);
  menu_edit_action_redo= menu_edit->addAction(menu_strings[menu_off + MENU_EDIT_REDO]);
  connect(menu_edit_action_redo, SIGNAL(triggered()), this, SLOT(menu_edit_redo()));
  shortcut("ocelot_shortcut_redo", "", false, true);
  menu_edit->addSeparator();
  menu_edit_action_cut= menu_edit->addAction(menu_strings[menu_off + MENU_EDIT_CUT]);
  connect(menu_edit_action_cut, SIGNAL(triggered()), this, SLOT(menu_edit_cut()));
  shortcut("ocelot_shortcut_cut", "", false, true);
  menu_edit_action_copy= menu_edit->addAction(menu_strings[menu_off + MENU_EDIT_COPY]);
  connect(menu_edit_action_copy, SIGNAL(triggered()), this, SLOT(menu_edit_copy()));
  shortcut("ocelot_shortcut_copy", "", false, true);
  menu_edit_action_paste= menu_edit->addAction(menu_strings[menu_off + MENU_EDIT_PASTE]);
  connect(menu_edit_action_paste, SIGNAL(triggered()), this, SLOT(menu_edit_paste()));
  shortcut("ocelot_shortcut_paste", "", false, true);
  menu_edit->addSeparator();
  menu_edit_action_select_all= menu_edit->addAction(menu_strings[menu_off + MENU_EDIT_SELECT_ALL]);
  connect(menu_edit_action_select_all, SIGNAL(triggered()), this, SLOT(menu_edit_select_all()));
  shortcut("ocelot_shortcut_select_all", "", false, true);
  menu_edit_action_history_markup_previous= menu_edit->addAction(menu_strings[menu_off + MENU_EDIT_PREVIOUS_STATEMENT]);
  connect(menu_edit_action_history_markup_previous, SIGNAL(triggered()), this, SLOT(history_markup_previous()));
  shortcut("ocelot_shortcut_history_markup_previous", "", false, true);
  menu_edit_action_history_markup_next= menu_edit->addAction(menu_strings[menu_off + MENU_EDIT_NEXT_STATEMENT]);
  connect(menu_edit_action_history_markup_next, SIGNAL(triggered()), this, SLOT(history_markup_next()));
  shortcut("ocelot_shortcut_history_markup_next", "", false, true);
  menu_edit_action_formatter= menu_edit->addAction(menu_strings[menu_off + MENU_EDIT_FORMAT]);
  connect(menu_edit_action_formatter, SIGNAL(triggered()), this, SLOT(statement_edit_widget_formatter()));
  shortcut("ocelot_shortcut_format", "", false, true);
  menu_edit_action_zoomin= menu_edit->addAction(menu_strings[menu_off + MENU_EDIT_ZOOMIN]);
  connect(menu_edit_action_zoomin, SIGNAL(triggered()), this, SLOT(menu_edit_zoomin()));
  shortcut("ocelot_shortcut_zoomin", "", false, true);
  menu_edit_action_zoomout= menu_edit->addAction(menu_strings[menu_off + MENU_EDIT_ZOOMOUT]);
  connect(menu_edit_action_zoomout, SIGNAL(triggered()), this, SLOT(menu_edit_zoomout()));
  shortcut("ocelot_shortcut_zoomout", "", false, true);
  menu_edit_action_autocomplete= menu_edit->addAction(menu_strings[menu_off + MENU_EDIT_AUTOCOMPLETE]);
  connect(menu_edit_action_autocomplete, SIGNAL(triggered()), this, SLOT(menu_edit_autocomplete_via_menu()));
  shortcut("ocelot_shortcut_autocomplete", "", false, true);
  menu_run= ui->menuBar->addMenu(menu_strings[menu_off + MENU_RUN]);
  menu_run_action_execute= menu_run->addAction(menu_strings[menu_off + MENU_RUN_EXECUTE]);
  connect(menu_run_action_execute, SIGNAL(triggered()), this, SLOT(action_execute_force()));
  shortcut("ocelot_shortcut_execute", "", false, true);
  menu_run_action_kill= menu_run->addAction(menu_strings[menu_off + MENU_RUN_KILL]);
  connect(menu_run_action_kill, SIGNAL(triggered()), this, SLOT(action_kill()));
  shortcut("ocelot_shortcut_kill", "", false, true);
  menu_run_action_kill->setEnabled(false);
  menu_settings= ui->menuBar->addMenu(menu_strings[menu_off + MENU_SETTINGS]);
  menu_settings_action_menu= menu_settings->addAction(menu_strings[menu_off + MENU_SETTINGS_MENU]);
  menu_settings_action_history= menu_settings->addAction(menu_strings[menu_off + MENU_SETTINGS_HISTORY_WIDGET]);
  menu_settings_action_grid= menu_settings->addAction(menu_strings[menu_off + MENU_SETTINGS_GRID_WIDGET]);
  menu_settings_action_statement= menu_settings->addAction(menu_strings[menu_off + MENU_SETTINGS_STATEMENT_WIDGET]);
  menu_settings_action_extra_rule_1= menu_settings->addAction(menu_strings[menu_off + MENU_SETTINGS_EXTRA_RULE_1]);
  connect(menu_settings_action_menu, SIGNAL(triggered()), this, SLOT(action_menu()));
  connect(menu_settings_action_history, SIGNAL(triggered()), this, SLOT(action_history()));
  connect(menu_settings_action_grid, SIGNAL(triggered()), this, SLOT(action_grid()));
  connect(menu_settings_action_statement, SIGNAL(triggered()), this, SLOT(action_statement()));
  connect(menu_settings_action_extra_rule_1, SIGNAL(triggered()), this, SLOT(action_extra_rule_1()));
  menu_options= ui->menuBar->addMenu(menu_strings[menu_off + MENU_OPTIONS]);
  menu_options_action_option_detach_history_widget= menu_options->addAction(menu_strings[menu_off + MENU_OPTIONS_DETACH_HISTORY_WIDGET]);
  menu_options_action_option_detach_history_widget->setCheckable(true);
  menu_options_action_option_detach_history_widget->setChecked(ocelot_detach_history_widget);
  connect(menu_options_action_option_detach_history_widget, SIGNAL(triggered(bool)), this, SLOT(action_option_detach_history_widget(bool)));
  menu_options_action_option_detach_result_grid_widget= menu_options->addAction(menu_strings[menu_off + MENU_OPTIONS_DETACH_RESULT_GRID_WIDGET]);
  menu_options_action_option_detach_result_grid_widget->setCheckable(true);
  menu_options_action_option_detach_result_grid_widget->setChecked(ocelot_detach_result_grid_widget);
  connect(menu_options_action_option_detach_result_grid_widget, SIGNAL(triggered(bool)), this, SLOT(action_option_detach_result_grid_widget(bool)));
  menu_options_action_option_detach_debug_widget= menu_options->addAction(menu_strings[menu_off + MENU_OPTIONS_DETACH_DEBUG_WIDGET]);
  menu_options_action_option_detach_debug_widget->setCheckable(true);
  menu_options_action_option_detach_debug_widget->setChecked(ocelot_detach_debug_widget);
  connect(menu_options_action_option_detach_debug_widget, SIGNAL(triggered(bool)), this, SLOT(action_option_detach_debug_widget(bool)));
  menu_options_action_next_window= menu_options->addAction(menu_strings[menu_off + MENU_OPTIONS_NEXT_WINDOW]);
  connect(menu_options_action_next_window, SIGNAL(triggered(bool)), this, SLOT(action_option_next_window()));
  shortcut("ocelot_shortcut_next_window", "", false, true);
  menu_options_action_previous_window= menu_options->addAction(menu_strings[menu_off + MENU_OPTIONS_PREVIOUS_WINDOW]);
  connect(menu_options_action_previous_window, SIGNAL(triggered(bool)), this, SLOT(action_option_previous_window()));
  shortcut("ocelot_shortcut_previous_window", "", false, true);
#ifdef DEBUGGER
  menu_debug= ui->menuBar->addMenu(menu_strings[menu_off + MENU_DEBUG]);
//  menu_debug_action_install= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_INSTALL]);
//  connect(menu_debug_action_install, SIGNAL(triggered()), this, SLOT(action_debug_install()));
//=shortcut(?)  menu_debug_action_install->setShortcut(QKeySequence(tr("Alt+A")));
//  menu_debug_action_setup= menu_debug->addAction(tr("Setup"));
//  connect(menu_debug_action_setup, SIGNAL(triggered()), this, SLOT(action_debug_setup()));
//=shortcut(?)  menu_debug_action_setup->setShortcut(QKeySequence(tr("Alt+5")));
//  menu_debug_action_debug= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG]);
//  connect(menu_debug_action_debug, SIGNAL(triggered()), this, SLOT(action_debug_debug()));
//=shortcut(?)  menu_debug_action_debug->setShortcut(QKeySequence(tr("Alt+3")));
  menu_debug_action_breakpoint= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_BREAKPOINT]);
  connect(menu_debug_action_breakpoint, SIGNAL(triggered()), this, SLOT(action_debug_breakpoint()));
  shortcut("ocelot_shortcut_breakpoint", "", false, true);
  menu_debug_action_continue= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_CONTINUE]);
  connect(menu_debug_action_continue, SIGNAL(triggered()), this, SLOT(action_debug_continue()));
  shortcut("ocelot_shortcut_continue", "", false, true);
//  menu_debug_action_leave= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_LEAVE);
//  connect(menu_debug_action_leave, SIGNAL(triggered()), this, SLOT(action_debug_leave()));
//=shortcut(?)  menu_debug_action_leave->setShortcut(QKeySequence(tr("Alt+B")));
  menu_debug_action_next= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_NEXT]);
  connect(menu_debug_action_next, SIGNAL(triggered()), this, SLOT(action_debug_next()));
  shortcut("ocelot_shortcut_next", "", false, true);
//  menu_debug_action_skip= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_SKIP);
//  connect(menu_debug_action_skip, SIGNAL(triggered()), this, SLOT(action_debug_skip()));
//=shortcut(?)  menu_debug_action_skip->setShortcut(QKeySequence(tr("Alt+4")));
  menu_debug_action_step= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_STEP]);
  connect(menu_debug_action_step, SIGNAL(triggered()), this, SLOT(action_debug_step()));
  shortcut("ocelot_shortcut_step", "", false, true);
  menu_debug_action_clear= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_CLEAR]);
  connect(menu_debug_action_clear, SIGNAL(triggered()), this, SLOT(action_debug_clear()));
  shortcut("ocelot_shortcut_clear", "", false, true);
//  menu_debug_action_delete= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_DELETE]);
//  connect(menu_debug_action_delete, SIGNAL(triggered()), this, SLOT(action_debug_delete()));
//=shortcut(?)  menu_debug_action_delete->setShortcut(QKeySequence(tr("Alt+G")));
  menu_debug_action_exit= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_EXIT]);
  connect(menu_debug_action_exit, SIGNAL(triggered()), this, SLOT(action_debug_exit()));
  shortcut("ocelot_shortcut_debug_exit", "", false, true);
  menu_debug_action_information= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_INFORMATION]);
  connect(menu_debug_action_information, SIGNAL(triggered()), this, SLOT(action_debug_information()));
  shortcut("ocelot_shortcut_information", "", false, true);
  menu_debug_action_refresh_server_variables= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_REFRESH_SERVER_VARIABLES]);
  connect(menu_debug_action_refresh_server_variables, SIGNAL(triggered()), this, SLOT(action_debug_refresh_server_variables()));
  shortcut("ocelot_shortcut_refresh_server_variables", "", false, true);
  menu_debug_action_refresh_user_variables= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_REFRESH_USER_VARIABLES]);
  connect(menu_debug_action_refresh_user_variables, SIGNAL(triggered()), this, SLOT(action_debug_refresh_user_variables()));
  shortcut("ocelot_shortcut_refresh_user_variables", "", false, true);
  menu_debug_action_refresh_variables= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_REFRESH_VARIABLES]);
  connect(menu_debug_action_refresh_variables, SIGNAL(triggered()), this, SLOT(action_debug_refresh_variables()));
  shortcut("ocelot_shortcut_refresh_variables", "", false, true);
  menu_debug_action_refresh_call_stack= menu_debug->addAction(menu_strings[menu_off + MENU_DEBUG_REFRESH_CALL_STACK]);
  connect(menu_debug_action_refresh_call_stack, SIGNAL(triggered()), this, SLOT(action_debug_refresh_call_stack()));
  shortcut("ocelot_shortcut_refresh_call_stack", "", false, true);
  debug_menu_enable_or_disable(TOKEN_KEYWORD_BEGIN); /* Disable most of debug menu */
#endif
  menu_help= ui->menuBar->addMenu(menu_strings[menu_off + MENU_HELP]);
  menu_help_action_about= menu_help->addAction(menu_strings[menu_off + MENU_HELP_ABOUT]);
  connect(menu_help_action_about, SIGNAL(triggered()), this, SLOT(action_about()));
  menu_help_action_the_manual= menu_help->addAction(menu_strings[menu_off + MENU_HELP_THE_MANUAL]);
  connect(menu_help_action_the_manual, SIGNAL(triggered()), this, SLOT(action_the_manual()));
  /* Qt says I should also do "addSeparator" if Motif style. Harmless. */
  ui->menuBar->addSeparator();
  /* exitAction->setPriority(QAction::LowPriority); */
  menu_help_action_libmysqlclient= menu_help->addAction(menu_strings[menu_off + MENU_HELP_LIBMYSQLCLIENT]);
  connect(menu_help_action_libmysqlclient, SIGNAL(triggered()), this, SLOT(action_libmysqlclient()));
  menu_help_action_settings= menu_help->addAction(menu_strings[menu_off + MENU_HELP_SETTINGS]);
  connect(menu_help_action_settings, SIGNAL(triggered()), this, SLOT(action_settings()));
}

/*
  Shortcuts -- set variable value, or do setShortcut().
  Example:
    At the beginning of this program we have a declaration
    static char ocelot_shortcut_exit[80]= "default";
    The figure 80 is big enough for "Ctrl+Shift+Alt+F12\0" (19) 4 times
    but not for multiple keys.
    We can change it with a setting in a .cnf file e.g. .my.cnf:
    ocelot_shortcut_key = 'Alt+L'
    We can change it with a SET statement:
    SET ocelot_shortcut_exit = 'Ctrl+q';
    or
    SET ocelot_shortcut_exit = 'default';
    or
    SET ocelot_shortcut_exit = 'ALT+A,Alt+B';
  See also http://doc.qt.io/qt-4.8/qkeysequence.html#fromString
  I have tested
  SET ocelot_shortcut_exit = 'Alt+L';
  and it works (File|Exit menu shows Alt+L, and Alt+L causes quit).
  Thus this changes what's in create_menu(), above.
  Called from create_menu(),execute_client_statement(),connect_set_variable().
  Return 1 if it's ocelot_shortcut_exit etc., even if it fails.
  You might want to "set" e.g. ocelot_shortcut_exit= value;
  You might want to "do" e.g. menu_file_action_exit->setShortcut(value);
  You might want to both "set" and "do".
  Todo: ensure two keys don't have the same action (error check).
  Todo: ensure string is valid -- error checks and warnings|errors
        (now we check for displayable ASCII or F1-F12, is that okay?)
  Todo: syntax checker should see this.
  Todo: all the other shortcut keys.
  Todo: also, something for "what to execute" e.g. SQL statement.
  Todo: return 0 if token1 first characters are not "ocelot_shortcut_"
  Todo: The format shortcut should be Ctrl+Shift+F, but isn't.
        See the comment about this in ocelotgui.cpp.
  Todo: There is no GUI "shortcut editor" here, but such things exist:
        see doc.qt.io/qt-5/qkeysequenceedit.html#details.
        We could show a table with current settings, and let
        users pick or type in a key sequence.
  Warn: Although we can handle multiple-key shortcuts,
        the workaround for the Qt/Ubuntu bug in eventfilter_function
        only looks at one key.
  Warn: We don't say default = QKeySequence::Zoomin for zoomin, because
        it is Ctrl++ (Qt::CTRL + Qt::Key_Plus). But + requires shift.
        It seems more common to use without shift, which is Ctrl+=.
*/
int MainWindow::shortcut(QString token1, QString token3, bool is_set, bool is_do)
{
  QString target= token1.toLower();
  QString source;
  char source_as_utf8[80*4];
  if (target.mid(0, 16) != "ocelot_shortcut_") return 0;
  if (is_set)
  {
    source= connect_stripper(token3, false);
    if (source.length() >= 80) return -1;
    source= source.toLower();
    strcpy(source_as_utf8, source.toUtf8());
    if (strlen(source_as_utf8) >= 80) return -1;
    if (strcmp(source_as_utf8, "default") != 0)
    {
      QKeySequence k= QKeySequence(source_as_utf8);
      if ((k.count() < 1) || (k.count() > 4)) return -1;
      if (k.isEmpty()) return -1;
      if (k.toString() < " ") return -1;
      for (unsigned int i= 0; i < (unsigned) k.count(); ++i)
      {
        int qi= k.operator[](i);
        qi= (qi & ~(Qt::CTRL | Qt::SHIFT | Qt::ALT | Qt::META));
        bool is_ok= false;
        if ((qi >= Qt::Key_Space) && (qi <= Qt::Key_ydiaeresis)) is_ok= true;
        if ((qi >= Qt::Key_F1) && (qi <= Qt::Key_F12)) is_ok= true;
        if (is_ok == false) return -1;
      }
    }
  }
  if (target == "ocelot_shortcut_connect")
  {
    if (is_set) strcpy(ocelot_shortcut_connect, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_connect, "default") == 0)
        ocelot_shortcut_connect_keysequence= QKeySequence::Open;
      else
        ocelot_shortcut_connect_keysequence= QKeySequence(ocelot_shortcut_connect);
      menu_file_action_connect->setShortcut(ocelot_shortcut_connect_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_exit")
  {
    if (is_set) strcpy(ocelot_shortcut_exit, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_exit, "default") == 0)
        /* With Puppy Linux --non-KDE non-Gnome -- QKeySequence::Quit fails. */
        ocelot_shortcut_exit_keysequence= QKeySequence("Ctrl+Q");
      else
        ocelot_shortcut_exit_keysequence= QKeySequence(ocelot_shortcut_exit);
      menu_file_action_exit->setShortcut(ocelot_shortcut_exit_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_undo")
  {
    if (is_set) strcpy(ocelot_shortcut_undo, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_undo, "default") == 0)
        ocelot_shortcut_undo_keysequence= QKeySequence::Undo;
      else
        ocelot_shortcut_undo_keysequence= QKeySequence(ocelot_shortcut_undo);
      menu_edit_action_undo->setShortcut(ocelot_shortcut_undo_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_redo")
  {
    if (is_set) strcpy(ocelot_shortcut_redo, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_redo, "default") == 0)
        ocelot_shortcut_redo_keysequence= QKeySequence::Redo;
      else
        ocelot_shortcut_redo_keysequence= QKeySequence(ocelot_shortcut_redo);
      menu_edit_action_redo->setShortcut(ocelot_shortcut_redo_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_cut")
  {
    if (is_set) strcpy(ocelot_shortcut_cut, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_cut, "default") == 0)
        ocelot_shortcut_cut_keysequence= QKeySequence::Cut;
      else
        ocelot_shortcut_cut_keysequence= QKeySequence(ocelot_shortcut_cut);
      menu_edit_action_cut->setShortcut(ocelot_shortcut_cut_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_copy")
  {
    if (is_set) strcpy(ocelot_shortcut_copy, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_copy, "default") == 0)
        ocelot_shortcut_copy_keysequence= QKeySequence::Copy;
      else
        ocelot_shortcut_copy_keysequence= QKeySequence(ocelot_shortcut_copy);
      menu_edit_action_copy->setShortcut(ocelot_shortcut_copy_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_paste")
  {
    if (is_set) strcpy(ocelot_shortcut_paste, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_paste, "default") == 0)
        ocelot_shortcut_paste_keysequence= QKeySequence::Paste;
      else
        ocelot_shortcut_paste_keysequence= QKeySequence(ocelot_shortcut_paste);
      menu_edit_action_paste->setShortcut(ocelot_shortcut_paste_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_select_all")
  {
    if (is_set) strcpy(ocelot_shortcut_select_all, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_select_all, "default") == 0)
        ocelot_shortcut_select_all_keysequence= QKeySequence::SelectAll;
      else
        ocelot_shortcut_select_all_keysequence= QKeySequence(ocelot_shortcut_select_all);
      menu_edit_action_select_all->setShortcut(ocelot_shortcut_select_all_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_history_markup_previous")
  {
    if (is_set) strcpy(ocelot_shortcut_history_markup_previous, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_history_markup_previous, "default") == 0)
        ocelot_shortcut_history_markup_previous_keysequence= QKeySequence("Ctrl+P");
      else
        ocelot_shortcut_history_markup_previous_keysequence= QKeySequence(ocelot_shortcut_history_markup_previous);
      menu_edit_action_history_markup_previous->setShortcut(ocelot_shortcut_history_markup_previous_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_history_markup_next")
  {
    if (is_set) strcpy(ocelot_shortcut_history_markup_next, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_history_markup_next, "default") == 0)
        ocelot_shortcut_history_markup_next_keysequence= QKeySequence("Ctrl+N");
      else
        ocelot_shortcut_history_markup_next_keysequence= QKeySequence(ocelot_shortcut_history_markup_next);
      menu_edit_action_history_markup_next->setShortcut(ocelot_shortcut_history_markup_next_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_format")
  {
    if (is_set) strcpy(ocelot_shortcut_format, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_format, "default") == 0)
        ocelot_shortcut_format_keysequence= QKeySequence("Alt+Shift+F");
      else
        ocelot_shortcut_format_keysequence= QKeySequence(ocelot_shortcut_format);
      menu_edit_action_formatter->setShortcut(ocelot_shortcut_format_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_zoomin")
  {
    if (is_set) strcpy(ocelot_shortcut_zoomin, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_zoomin, "default") == 0)
        ocelot_shortcut_zoomin_keysequence= QKeySequence("Ctrl+=");
      else
        ocelot_shortcut_zoomin_keysequence= QKeySequence(ocelot_shortcut_zoomin);
      menu_edit_action_zoomin->setShortcut(ocelot_shortcut_zoomin_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_zoomout")
  {
    if (is_set) strcpy(ocelot_shortcut_zoomout, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_zoomout, "default") == 0)
        ocelot_shortcut_zoomout_keysequence= QKeySequence::ZoomOut;
      else
        ocelot_shortcut_zoomout_keysequence= QKeySequence(ocelot_shortcut_zoomout);
      menu_edit_action_zoomout->setShortcut(ocelot_shortcut_zoomout_keysequence);
    }
    return 1;
  }
  /* Todo: Tab is like mysql but a poor default choice. Try Ctrl+Space? */
  if (target == "ocelot_shortcut_autocomplete")
  {
    if (is_set) strcpy(ocelot_shortcut_autocomplete, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_autocomplete, "default") == 0)
        ocelot_shortcut_autocomplete_keysequence= QKeySequence("Tab");
      else
        ocelot_shortcut_autocomplete_keysequence= QKeySequence(ocelot_shortcut_autocomplete);
      menu_edit_action_autocomplete->setShortcut(ocelot_shortcut_autocomplete_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_execute")
  {
    if (is_set) strcpy(ocelot_shortcut_execute, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_execute, "default") == 0)
        ocelot_shortcut_execute_keysequence= QKeySequence("Ctrl+E");
      else
        ocelot_shortcut_execute_keysequence= QKeySequence(ocelot_shortcut_execute);
      menu_run_action_execute->setShortcut(ocelot_shortcut_execute_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_kill")
  {
    if (is_set) strcpy(ocelot_shortcut_kill, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_kill, "default") == 0)
        ocelot_shortcut_kill_keysequence= QKeySequence("Ctrl+C");
      else
        ocelot_shortcut_kill_keysequence= QKeySequence(ocelot_shortcut_kill);
      menu_run_action_kill->setShortcut(ocelot_shortcut_kill_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_next_window")
  {
    if (is_set) strcpy(ocelot_shortcut_next_window, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_next_window, "default") == 0)
        ocelot_shortcut_next_window_keysequence= QKeySequence::NextChild;
      else
        ocelot_shortcut_next_window_keysequence= QKeySequence(ocelot_shortcut_next_window);
      menu_options_action_next_window->setShortcut(ocelot_shortcut_next_window_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_previous_window")
  {
    if (is_set) strcpy(ocelot_shortcut_previous_window, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_previous_window, "default") == 0)
        ocelot_shortcut_previous_window_keysequence= QKeySequence::PreviousChild;
      else
        ocelot_shortcut_previous_window_keysequence= QKeySequence(ocelot_shortcut_previous_window);
      menu_options_action_previous_window->setShortcut(ocelot_shortcut_previous_window_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_breakpoint")
  {
    if (is_set) strcpy(ocelot_shortcut_breakpoint, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_breakpoint, "default") == 0)
        ocelot_shortcut_breakpoint_keysequence= QKeySequence("Alt+1");
      else
        ocelot_shortcut_breakpoint_keysequence= QKeySequence(ocelot_shortcut_breakpoint);
      menu_debug_action_breakpoint->setShortcut(ocelot_shortcut_breakpoint_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_continue")
  {
    if (is_set) strcpy(ocelot_shortcut_continue, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_continue, "default") == 0)
        ocelot_shortcut_continue_keysequence= QKeySequence("Alt+2");
      else
        ocelot_shortcut_continue_keysequence= QKeySequence(ocelot_shortcut_continue);
      menu_debug_action_continue->setShortcut(ocelot_shortcut_continue_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_next")
  {
    if (is_set) strcpy(ocelot_shortcut_next, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_next, "default") == 0)
        ocelot_shortcut_next_keysequence= QKeySequence("Alt+3");
      else
        ocelot_shortcut_next_keysequence= QKeySequence(ocelot_shortcut_next);
      menu_debug_action_next->setShortcut(ocelot_shortcut_next_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_step")
  {
    if (is_set) strcpy(ocelot_shortcut_step, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_step, "default") == 0)
        ocelot_shortcut_step_keysequence= QKeySequence("Alt+5");
      else
        ocelot_shortcut_step_keysequence= QKeySequence(ocelot_shortcut_step);
      menu_debug_action_step->setShortcut(ocelot_shortcut_step_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_clear")
  {
    if (is_set) strcpy(ocelot_shortcut_clear, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_clear, "default") == 0)
        ocelot_shortcut_clear_keysequence= QKeySequence("Alt+6");
      else
        ocelot_shortcut_clear_keysequence= QKeySequence(ocelot_shortcut_clear);
      menu_debug_action_clear->setShortcut(ocelot_shortcut_clear_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_debug_exit")
  {
    if (is_set) strcpy(ocelot_shortcut_debug_exit, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_debug_exit, "default") == 0)
        ocelot_shortcut_debug_exit_keysequence= QKeySequence("Alt+7");
      else
        ocelot_shortcut_debug_exit_keysequence= QKeySequence(ocelot_shortcut_debug_exit);
      menu_debug_action_exit->setShortcut(ocelot_shortcut_debug_exit_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_information")
  {
    if (is_set) strcpy(ocelot_shortcut_information, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_information, "default") == 0)
        ocelot_shortcut_information_keysequence= QKeySequence("Alt+8");
      else
        ocelot_shortcut_information_keysequence= QKeySequence(ocelot_shortcut_information);
      menu_debug_action_information->setShortcut(ocelot_shortcut_information_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_refresh_server_variables")
  {
    if (is_set) strcpy(ocelot_shortcut_refresh_server_variables, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_refresh_server_variables, "default") == 0)
        ocelot_shortcut_refresh_server_variables_keysequence= QKeySequence("Alt+9");
      else
        ocelot_shortcut_refresh_server_variables_keysequence= QKeySequence(ocelot_shortcut_refresh_server_variables);
      menu_debug_action_refresh_server_variables->setShortcut(ocelot_shortcut_refresh_server_variables_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_refresh_user_variables")
  {
    if (is_set) strcpy(ocelot_shortcut_refresh_user_variables, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_refresh_user_variables, "default") == 0)
        ocelot_shortcut_refresh_user_variables_keysequence= QKeySequence("Alt+0");
      else
        ocelot_shortcut_refresh_user_variables_keysequence= QKeySequence(ocelot_shortcut_refresh_user_variables);
      menu_debug_action_refresh_user_variables->setShortcut(ocelot_shortcut_refresh_user_variables_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_refresh_variables")
  {
    if (is_set) strcpy(ocelot_shortcut_refresh_variables, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_refresh_variables, "default") == 0)
        ocelot_shortcut_refresh_variables_keysequence= QKeySequence("Alt+A");
      else
        ocelot_shortcut_refresh_variables_keysequence= QKeySequence(ocelot_shortcut_refresh_variables);
      menu_debug_action_refresh_variables->setShortcut(ocelot_shortcut_refresh_variables_keysequence);
    }
    return 1;
  }
  if (target == "ocelot_shortcut_refresh_call_stack")
  {
    if (is_set) strcpy(ocelot_shortcut_refresh_call_stack, source_as_utf8);
    if (is_do)
    {
      if (strcmp(ocelot_shortcut_refresh_call_stack, "default") == 0)
        ocelot_shortcut_refresh_call_stack_keysequence= QKeySequence("Alt+B");
      else
        ocelot_shortcut_refresh_call_stack_keysequence= QKeySequence(ocelot_shortcut_refresh_call_stack);
      menu_debug_action_refresh_call_stack->setShortcut(ocelot_shortcut_refresh_call_stack_keysequence);
    }
  }
  return 0;
}

/*
  Edit Menu Dispatcher

  We have only one edit menu, but we have multiple edit widgets.
  So the connect-signal-slot code in create_menu() jumps to menu_edit slots.
  The alternatives were: Multiple Document Interface, or multiple windows. I thought this was easier.
  For example, for cut: the slot is menu_edit_cut, it gets the focused widget and calls its cut().
  But Undo and Redo are complicated: for some widgets they're enabled, for some widgets they're not.
  TODO: We are not enabling|disabling edit menu items properly. We should be saying (examples):
        Initially, menu_edit_action_cut->setEnabled(false);
        When creating any editor widget,
          connect(widget, SIGNAL(copyAvailable(bool)),menu_edit_action_cut, SLOT(setEnabled(bool)));
        When we want to know if something can be pasted,
          connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(processClipboardChange()));
          qMimeData* x= clipboard->mimeData();
          qStringList* y= x->formats();
          int z= y->size();
          if (z > 0) menu_edit_action_paste->setEnabled();                 << wrong, you only know if paste is possible
        If something is selected and focus changes: de-select      
  TODO: I have no idea what "else if (strcmp(class_name, "+") == 0)"
        is for, maybe it's a bug. Compare menu_edit_redo().
*/
void MainWindow::menu_edit_undo()
{
  QWidget* focus_widget= QApplication::focusWidget();
  const char *class_name= focus_widget->metaObject()->className();
  if (strcmp(class_name, "CodeEditor") == 0)
    action_undo();
  else if (strcmp(class_name, "TextEditWidget") == 0)
    qobject_cast<TextEditWidget*>(focus_widget)->undo();
  else if (strcmp(class_name, "TextEditHistory") == 0)
    qobject_cast<TextEditHistory*>(focus_widget)->undo();
  else if (strcmp(class_name, "QTextEdit") == 0)
    qobject_cast<QTextEdit*>(focus_widget)->undo();
  else if (strcmp(class_name, "+") == 0)
    qobject_cast<QTextEdit*>(focus_widget)->undo();
}
void MainWindow::menu_edit_redo()
{
  QWidget* focus_widget= QApplication::focusWidget();
  if (focus_widget == 0) return; /* This would be unexpected */
  const char *class_name= focus_widget->metaObject()->className();
  if (strcmp(class_name, "CodeEditor") == 0)
    action_redo();
  else if (strcmp(class_name, "TextEditWidget") == 0)
    qobject_cast<TextEditWidget*>(focus_widget)->redo();
  else if (strcmp(class_name, "TextEditHistory") == 0)
    qobject_cast<TextEditHistory*>(focus_widget)->redo();
  else if (strcmp(class_name, "QTextEdit") == 0)
    qobject_cast<QTextEdit*>(focus_widget)->redo();
}
void MainWindow::menu_edit_cut()
{
  QWidget* focus_widget= QApplication::focusWidget();
  if (focus_widget == 0) return; /* This would be unexpected */
  const char *class_name= focus_widget->metaObject()->className();
  if (strcmp(class_name, "CodeEditor") == 0)
    qobject_cast<CodeEditor*>(focus_widget)->cut();
  else if (strcmp(class_name, "TextEditWidget") == 0)
    qobject_cast<TextEditWidget*>(focus_widget)->cut();
  else if (strcmp(class_name, "TextEditHistory") == 0)
    qobject_cast<TextEditHistory*>(focus_widget)->cut();
  else if (strcmp(class_name, "QTextEdit") == 0)
    qobject_cast<QTextEdit*>(focus_widget)->cut();
}
void MainWindow::menu_edit_copy()
{
  QWidget* focus_widget= QApplication::focusWidget();
  if (focus_widget == 0) return; /* This would be unexpected */
  const char *class_name= focus_widget->metaObject()->className();
  if (strcmp(class_name, "CodeEditor") == 0)
    qobject_cast<CodeEditor*>(focus_widget)->copy();
  else if (strcmp(class_name, "TextEditWidget") == 0)
    /* see TextEditWidget::copy() later in this file */
    qobject_cast<TextEditWidget*>(focus_widget)->copy();
  else if (strcmp(class_name, "TextEditHistory") == 0)
    qobject_cast<TextEditHistory*>(focus_widget)->copy();
  else if (strcmp(class_name, "QTextEdit") == 0)
    qobject_cast<QTextEdit*>(focus_widget)->copy();
}
void MainWindow::menu_edit_paste()
{
  QWidget* focus_widget= QApplication::focusWidget();
  if (focus_widget == 0) return; /* This would be unexpected */
  const char *class_name= focus_widget->metaObject()->className();
  if (strcmp(class_name, "CodeEditor") == 0)
    qobject_cast<CodeEditor*>(focus_widget)->paste();
  else if (strcmp(class_name, "TextEditWidget") == 0)
    qobject_cast<TextEditWidget*>(focus_widget)->paste();
  if (strcmp(class_name, "TextEditHistory") == 0)
    qobject_cast<TextEditHistory*>(focus_widget)->paste();
  else if (strcmp(class_name, "QTextEdit") == 0)
    qobject_cast<QTextEdit*>(focus_widget)->paste();
}
void MainWindow::menu_edit_select_all()
{
  QWidget* focus_widget= QApplication::focusWidget();
  if (focus_widget == 0) return; /* This would be unexpected */
  const char *class_name= focus_widget->metaObject()->className();
  if (strcmp(class_name, "CodeEditor") == 0)
    qobject_cast<CodeEditor*>(focus_widget)->selectAll();
  else if (strcmp(class_name, "TextEditWidget") == 0)
    qobject_cast<TextEditWidget*>(focus_widget)->selectAll();
  else if (strcmp(class_name, "TextEditHistory") == 0)
    qobject_cast<TextEditHistory*>(focus_widget)->selectAll();
  else if (strcmp(class_name, "QTextEdit") == 0)
    qobject_cast<QTextEdit*>(focus_widget)->selectAll();
}
/*
  zoom in/out won't work by calling default zoomIn()/zoomOut(), probably
  because we use style sheets. So we'll get the current style
  sheet, add an increment (currently always +1 or -1), change
  the style sheet. This does not affect what's set with Settings.
  That's deliberate. Zooming is temporary, setting is permanent.
  Re extracting: Compare what we do with get_font_from_style_sheet().
  Todo: Changing font size won't affect images, we need special handling.
  Todo: Maybe we should qobject_cast for setStyleSheet() too.
  Todo: BUG: If you just click on a ResultGrid cell, thinking you have
        given it the focus, and then you click Ctrl++ or Ctrl-- i.e.
        the shortcut not the menu item, it doesn't recognize it.
        Click away, then come back, and it does recognize it.
*/
void MainWindow::menu_edit_zoominorout(int increment)
{
  QWidget* focus_widget= QApplication::focusWidget();
  if (focus_widget == 0) return; /* This would be unexpected */
  QString old_stylesheet;
  const char *class_name= focus_widget->metaObject()->className();
  if (strcmp(class_name, "CodeEditor") == 0)
    old_stylesheet= qobject_cast<CodeEditor*>(focus_widget)->styleSheet();
  else if (strcmp(class_name, "TextEditWidget") == 0)
    old_stylesheet= qobject_cast<TextEditWidget*>(focus_widget)->styleSheet();
  else if (strcmp(class_name, "TextEditHistory") == 0)
    old_stylesheet= qobject_cast<TextEditHistory*>(focus_widget)->styleSheet();
  else if (strcmp(class_name, "QTextEdit") == 0)
    old_stylesheet= qobject_cast<QTextEdit*>(focus_widget)->styleSheet();
  else return; /* This would be unexpected. */
  int i= old_stylesheet.indexOf("font-size:");
  if (i == -1) return;
  i+= sizeof("font-size:");
  int j= old_stylesheet.indexOf("pt", i);
  if (j == -1) return;
  QString old_font_size= old_stylesheet.mid(i-1, j-(i-1));
  int font_size= old_font_size.toInt();
  font_size+= increment;
  if (font_size < FONT_SIZE_MIN) font_size= FONT_SIZE_MIN;
  if (font_size > FONT_SIZE_MAX) font_size= FONT_SIZE_MAX;
  QString new_stylesheet= old_stylesheet.mid(0, i-1);
  new_stylesheet.append(QString::number(font_size));
  new_stylesheet.append(old_stylesheet.mid(j,old_stylesheet.size()-j));
  focus_widget->setStyleSheet(new_stylesheet);
  return;
}
void MainWindow::menu_edit_zoomin()
{
  menu_edit_zoominorout(FONT_SIZE_ZOOM_INCREMENT);
}
void MainWindow::menu_edit_zoomout()
{
  menu_edit_zoominorout(-FONT_SIZE_ZOOM_INCREMENT);
}

/*
  The autocomplete shortcut is handled by event filter which
  calls keypress_shortcut_handler() which calls menu_edit_autocomplete()
  directly. Therefore we can only get here if the user chooses
  the autocomplete menu item rather than press a shortcut key.
  And in that case we don't care if the return is false.
*/
void MainWindow::menu_edit_autocomplete_via_menu()
{
  menu_edit_autocomplete();
}

/*
  Called from edit menu choice = autocomplete or keypress_shortcut_handler().
  Depends on rehash, ocelot_auto_rehash etc. e.g. was REHASH called.
  The extra checking here applies because default key is Tab Qt::Key_Tab,
  we want to return false if we don't handle it as a shortcut,
  so that it will simply be added to the widget contents.
*/
bool MainWindow::menu_edit_autocomplete()
{
  QString text;
  if (ocelot_auto_rehash > 0)
  {
    if (hparse_line_edit->isHidden() == false)
    {
      if (statement_edit_widget->hasFocus() == true)
      {
        QString s= hparse_line_edit->text();
        int word_start= s.indexOf(": ", 0);
        int word_end= s.indexOf(" ", word_start + 2);
        if (word_end == -1) word_end= s.size();
        QString word= s.mid(word_start + 2, (word_end - word_start) - 1);
        int i;
        for (i= 0; main_token_lengths[i] != 0; ++i)
        {
          if ((main_token_flags[i] & TOKEN_FLAG_IS_ERROR) != 0) break;
        }
        if ((main_token_flags[i] & TOKEN_FLAG_IS_ERROR) == 0) return true;
        text= statement_edit_widget->toPlainText();
        if (word.left(1) != "[")
        {
          int offset;
          if (main_token_lengths[i] == 0)
          {
            offset= text.size();
            word= " " + word;
          }
          else offset= main_token_offsets[i];
          QString new_text= text.left(offset);
          new_text.append(word);
          int rest_start= offset + main_token_lengths[i];
          new_text.append(text.right(text.size() - rest_start));
          statement_edit_widget->setPlainText(new_text);
          QTextCursor c= statement_edit_widget->textCursor();
          c.movePosition(QTextCursor::End);
          statement_edit_widget->setTextCursor(c);
          return true;
        }
      }
    }
  }
  return false;
}


/*
  The required size of main_token_offsets|lengths|types|flags is
  #-of-tokens + 1. We allocate lots more than enough after calculating
  x = (size-in-bytes + 1) rounded up to nearest 1000.
  We never reduce and perhaps that will appear to be a memory leak.
  todo: check if 'new' fails.
*/
void MainWindow::main_token_new(int text_size)
{
  unsigned int desired_count;
  desired_count= text_size + 1;
  if (desired_count >= main_token_max_count)
  {
    if (main_token_max_count != 0)
    {
      delete [] main_token_offsets;
      delete [] main_token_lengths;
      delete [] main_token_types;
      delete [] main_token_flags;
      delete [] main_token_pointers;
      delete [] main_token_reftypes;
    }
    desired_count= (desired_count - (desired_count % 1000)) + 1000;
    main_token_offsets= new int[desired_count];
    main_token_lengths= new int[desired_count];
    main_token_lengths[0]= 0;
    main_token_types= new int[desired_count];
    main_token_flags= new unsigned int[desired_count];
    main_token_pointers= new int[desired_count];
    main_token_reftypes= new unsigned char[desired_count];
    main_token_max_count= desired_count;
  }
}

/* I'm now defining HAVE_PUSH_AND_POP for the debugger stub */
#define HAVE_PUSH_AND_POP 1
#ifdef HAVE_PUSH_AND_POP
/*
  Saving and restoring the main_token variables.
  This was conceived for a trick with subqueries that I didn't do,
  but $setup uses it now. The problem is that main_token_*
  variables are global (okay, bad early laziness, but refactoring
  now would be hard). So save them when you're about to parse
  something else e.g. a local subquery, and restore when the
  subquery is done. Optimistically I call this push + pop, but
  in fact can only save one level. Expect text to be saved elsewhere.
*/
void MainWindow::main_token_push()
{
  assert(sizeof(main_token_offsets[0] == sizeof(int)));
  assert(sizeof(main_token_lengths[0] == sizeof(int)));
  assert(sizeof(main_token_types[0] == sizeof(int)));
  assert(sizeof(main_token_flags[0] == sizeof(unsigned int)));
  assert(sizeof(main_token_pointers[0] == sizeof(int)));
  assert(sizeof(main_token_reftypes[0] == sizeof(unsigned char)));
  saved_main_token_count_in_all= main_token_count_in_all;
  saved_main_token_count_in_statement= main_token_count_in_statement;
  saved_main_token_number= main_token_number;
  saved_main_token_offsets= new int[saved_main_token_count_in_all];
  saved_main_token_lengths= new int[saved_main_token_count_in_all];
  saved_main_token_types= new int[saved_main_token_count_in_all];
  saved_main_token_flags= new unsigned int[saved_main_token_count_in_all];
  saved_main_token_pointers= new int[saved_main_token_count_in_all];
  saved_main_token_reftypes= new unsigned char[saved_main_token_count_in_all];
  memcpy(saved_main_token_offsets, main_token_offsets, saved_main_token_count_in_all * sizeof(int));
  memcpy(saved_main_token_lengths, main_token_lengths, saved_main_token_count_in_all * sizeof(int));
  memcpy(saved_main_token_types, main_token_types, saved_main_token_count_in_all * sizeof(int));
  memcpy(saved_main_token_flags, main_token_flags, saved_main_token_count_in_all * sizeof(unsigned int));
  memcpy(saved_main_token_pointers, main_token_pointers, saved_main_token_count_in_all * sizeof(int));
  memcpy(saved_main_token_reftypes, main_token_reftypes, saved_main_token_count_in_all * sizeof(unsigned char));
}
void MainWindow::main_token_pop()
{
  memcpy(main_token_offsets, saved_main_token_offsets, saved_main_token_count_in_all * sizeof(int));
  memcpy(main_token_lengths, saved_main_token_lengths, saved_main_token_count_in_all * sizeof(int));
  memcpy(main_token_types, saved_main_token_types, saved_main_token_count_in_all * sizeof(int));
  memcpy(main_token_flags, saved_main_token_flags, saved_main_token_count_in_all * sizeof(unsigned int));
  memcpy(main_token_pointers, saved_main_token_pointers, saved_main_token_count_in_all * sizeof(int));
  memcpy(main_token_reftypes, saved_main_token_reftypes, saved_main_token_count_in_all * sizeof(unsigned char));
  delete [] saved_main_token_reftypes;
  delete [] saved_main_token_pointers;
  delete [] saved_main_token_flags;
  delete [] saved_main_token_types;
  delete [] saved_main_token_lengths;
  delete [] saved_main_token_offsets;
  main_token_count_in_all= saved_main_token_count_in_all;
  main_token_count_in_statement= saved_main_token_count_in_statement;
  main_token_number= saved_main_token_number;
}
#endif

/*
  ACTIONS
  ! All action_ functions must be in the "public slots" area of ocelotgui.h
*/

/*
  action_statement_edit_widget_text_changed() is a slot.
  Actually the connect() was for statement_edit_widget->document()'s
  contentsChanged(), rather than statement_edit_widget's textChanged(),
  but I don't see any difference.
  This routine can cause the document to change, causing a signal
  and we get action_statement_edit_widget_text_changed() again.
  It would be infinite. We have two ways to prevent the loop, and we
  use them both (yes it's redundant but this loop really worries me):
    exit if a global flag is on
    disconnect to ensure this slot won't be activated by signals that it itself generates.
  Todo: A hang occurs sometimes, and log() suggests it's in this routine.
        That's why there are so many log() and assert() invocations.
  Until May 2017 I used blockSignals(true)+blockSignals(false) instead
  of disconnect+connect; also I set statement_edit_widget_text_changed_flag= 1;
  within the routine; I hope these changes are safe.
*/
void MainWindow::action_statement_edit_widget_text_changed(int position,int chars_removed,int chars_added)
{
  log("action_statement_edit_widget_text_changed start", 90);
  if (statement_edit_widget_text_changed_flag != 0)
  {
    position_for_redo= position;
    chars_removed_for_redo= chars_removed;
    chars_added_for_redo= chars_added;
    return;
  }
  log("action_statement_edit_widget_text_changed after flag check", 90);
  //statement_edit_widget_text_changed_flag= 1;
  //statement_edit_widget->document()->blockSignals(true);
  disconnect(statement_edit_widget->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(action_statement_edit_widget_text_changed(int,int,int)));

  QString text;
  int i;
  int pos;

  /* Syntax highlighting */
  text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */

  /* Todo: avoid total tokenize() + tokens_to_keywords() if user is just adding at end */
  main_token_new(text.size());
  tokenize(text.data(),
           text.size(),
           main_token_lengths, main_token_offsets, main_token_max_count,
           (QChar*)"33333", 1, ocelot_delimiter_str, 1);

  tokens_to_keywords(text, 0, sql_mode_ansi_quotes);
  if (((ocelot_statement_syntax_checker.toInt()) & FLAG_FOR_HIGHLIGHTS) != 0)
  {
    hparse_f_multi_block(text); /* recognizer */
  }
  log("action_statement_edit_widget_text_changed after hparse_f_multi_block", 90);
  /* This "sets" the colour, it does not "merge" it. */
  /* Do not try to set underlines, they won't go away. */
  QTextDocument *pDoc= statement_edit_widget->document();
  QTextCursor cur(pDoc);

  /* cur.select (QTextCursor::Document); */ /* desperate attempt to fix so undo/redo is not destroyed ... does not work */

  QTextCharFormat format_of_literal;
  format_of_literal.setForeground(QColor(qt_color(ocelot_statement_highlight_literal_color)));
  QTextCharFormat format_of_identifier;
  format_of_identifier.setForeground(QColor(qt_color(ocelot_statement_highlight_identifier_color)));
  QTextCharFormat format_of_comment;
  format_of_comment.setForeground(QColor(qt_color(ocelot_statement_highlight_comment_color)));
  QTextCharFormat format_of_operator;
  format_of_operator.setForeground(QColor(qt_color(ocelot_statement_highlight_operator_color)));
  QTextCharFormat format_of_reserved_word;
  format_of_reserved_word.setForeground(QColor(qt_color(ocelot_statement_highlight_keyword_color)));
  QTextCharFormat format_of_function;
  format_of_function.setForeground(QColor(qt_color(ocelot_statement_highlight_function_color)));
  QTextCharFormat format_of_other;
  format_of_other.setForeground(QColor(qt_color(ocelot_statement_text_color)));

  pos= 0;
  /* cur.setPosition(pos, QTextCursor::KeepAnchor); */

  cur.joinPreviousEditBlock(); /* was cur.beginEditBlock() till 2017-07-23 */
  /* ought to affect undo/redo stack? */

  {
    /* This sets everything to normal format / no underline. Gets overridden by token formats. */
    QTextCharFormat format_of_white_space;
    cur.setPosition(0, QTextCursor::MoveAnchor);
    cur.setPosition(text.size(), QTextCursor::KeepAnchor);
    format_of_white_space= format_of_other;
    format_of_white_space.setUnderlineStyle(QTextCharFormat::NoUnderline);
    cur.setCharFormat(format_of_white_space);
  }

  log("action_statement_edit_widget_text_changed loop start", 90);

  for (i= 0; main_token_lengths[i] != 0; ++i)
  {
    assert(main_token_lengths[i] > 0);
    assert(main_token_lengths[i] <= text.size());
    assert(main_token_offsets[i] >= 0);
    assert(main_token_offsets[i] <= text.size());
    assert(pos >= 0);
    assert(pos <= text.size());
    assert(i >= 0);
    assert(i <= (int) main_token_max_count);
    QTextCharFormat format_of_current_token;
    /* Todo: find out why this is necessary. It looks redundant but without it there's trouble. */
    for (; pos < main_token_offsets[i]; ++pos)
    {
      cur.setPosition(pos, QTextCursor::MoveAnchor);
      format_of_current_token= format_of_other;
      format_of_current_token.setUnderlineStyle(QTextCharFormat::NoUnderline);
      cur.setCharFormat(format_of_current_token);
      /* cur.clearSelection(); */
    }
    int t= main_token_types[i];
    if (t == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE) format_of_current_token= format_of_literal;
    if (t == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE) format_of_current_token= format_of_literal;
    if (t == TOKEN_TYPE_LITERAL_WITH_DIGIT) format_of_current_token= format_of_literal;
    /* literal_with_brace == literal */
    if (t == TOKEN_TYPE_LITERAL_WITH_BRACE) format_of_current_token= format_of_literal; /* obsolete? */
    if (t == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK) format_of_current_token= format_of_identifier;
    if (t == TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE) format_of_current_token= format_of_identifier;
    if (t == TOKEN_TYPE_IDENTIFIER) format_of_current_token= format_of_identifier;
    if (t == TOKEN_TYPE_IDENTIFIER_WITH_AT) format_of_current_token= format_of_identifier;
    if (t == TOKEN_TYPE_COMMENT_WITH_SLASH) format_of_current_token= format_of_comment;
    if (t == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE) format_of_current_token= format_of_comment;
    if (t == TOKEN_TYPE_COMMENT_WITH_MINUS) format_of_current_token= format_of_comment;
    if (t == TOKEN_TYPE_OPERATOR) format_of_current_token= format_of_operator;
    if (t >= TOKEN_KEYWORDS_START)
    {
      if (((main_token_flags[i] & TOKEN_FLAG_IS_FUNCTION) != 0)
       && (main_token_lengths[i + 1] == 1)
       && (text.mid(main_token_offsets[i + 1], 1) == "("))
      {
        format_of_current_token= format_of_function;
      }
      else format_of_current_token= format_of_reserved_word;
    }
    if (t == TOKEN_TYPE_OTHER) format_of_current_token= format_of_other;

    /* Todo: consider using SpellCheckUnderline instead of WaveUnderline. */
    if ((main_token_flags[i] & TOKEN_FLAG_IS_ERROR) != 0)
    {
      format_of_current_token.setUnderlineStyle(QTextCharFormat::WaveUnderline);
      format_of_current_token.setUnderlineColor(Qt::red);
    }
    else
    {
      //format_of_current_token.setUnderlineStyle(QTextCharFormat::NoUnderline);
    }

    cur.setPosition(pos, QTextCursor::MoveAnchor);
    cur.setPosition(pos + main_token_lengths[i], QTextCursor::KeepAnchor);
    cur.setCharFormat(format_of_current_token);
    pos+= main_token_lengths[i];
  }

  log("action_statement_edit_widget_text_changed loop end", 90);
  cur.endEditBlock();

  /* Todo: consider what to do about trailing whitespace. */

  widget_sizer(); /* Perhaps adjust relative sizes of the main widgets. */
  log("action_statement_edit_widget_text_changed after widget_sizer", 90);
  //statement_edit_widget->document()->blockSignals(false);
  connect(statement_edit_widget->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(action_statement_edit_widget_text_changed(int,int,int)));
  //statement_edit_widget_text_changed_flag= 0;
  log("action_statement_edit_widget_text_changed end", 90);
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
  If we're connecting, action_connect() calls action_connect_once with arg = "File|Connect".
  If we're printing for --help, print_help calls action_connect_once with arg = "Print".
  Todo:
  If user types OK and there's an error, repeat the dialog box with a new message e.g. "Connect failed ...".
  This is called from program-start!
  This should put "CONNECT" in the statement widget and cause its execution, so it shows up on the history widget.
*/

void MainWindow::action_connect_once(QString message)
{
  int column_count;
  QString *row_form_label;
  int *row_form_type;
  int *row_form_is_password;
  QString *row_form_data;
  QString *row_form_width;
  QString row_form_title;
  QString row_form_message;
  int i;
  Row_form_box *co;
  row_form_label= 0;
  row_form_type= 0;
  row_form_is_password= 0;
  row_form_data= 0;
  row_form_width= 0;
  column_count= 84; /* If you add or remove items, you have to change this */
  row_form_label= new QString[column_count];
  row_form_type= new int[column_count];
  row_form_is_password= new int[column_count];
  row_form_data= new QString[column_count];
  row_form_width= new QString[column_count];
  row_form_label[i=0]= "host"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_host; row_form_width[i]= 80;
  row_form_label[++i]= "port"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_port); row_form_width[i]= 4;
  row_form_label[++i]= "user"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_user; row_form_width[i]= 80;
  row_form_label[++i]= "database"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_database; row_form_width[i]= 80;
  row_form_label[++i]= "socket"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_unix_socket; row_form_width[i]= 80;
  row_form_label[++i]= "password"; row_form_type[i]= 0; row_form_is_password[i]= 1; row_form_data[i]= ocelot_password; row_form_width[i]= 80;
  row_form_label[++i]= "protocol"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_protocol; row_form_width[i]= 80;
  row_form_label[++i]= "init_command"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_init_command; row_form_width[i]= 80;
  row_form_label[++i]= "abort_source_on_error"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_abort_source_on_error); row_form_width[i]= 5;
  row_form_label[++i]= "auto_rehash"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_auto_rehash); row_form_width[i]= 5;
  row_form_label[++i]= "auto_vertical_output"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_auto_vertical_output); row_form_width[i]= 5;
  row_form_label[++i]= "batch"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_batch); row_form_width[i]= 5;
  row_form_label[++i]= "binary_mode"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_binary_mode); row_form_width[i]= 5;
  row_form_label[++i]= "bind_address"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_bind_address; row_form_width[i]= 5;
  row_form_label[++i]= "character_sets_dir"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_set_charset_dir; row_form_width[i]= 5;
  row_form_label[++i]= "column_names"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_result_grid_column_names); row_form_width[i]= 5;
  row_form_label[++i]= "column_type_info"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_column_type_info); row_form_width[i]= 5;
  row_form_label[++i]= "comments"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_comments); row_form_width[i]= 5;
  row_form_label[++i]= "compress"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_opt_compress); row_form_width[i]= 5;
  row_form_label[++i]= "connect_expired_password"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_opt_can_handle_expired_passwords); row_form_width[i]= 5;
  row_form_label[++i]= "connect_timeout"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_opt_connect_timeout); row_form_width[i]= 5;
  row_form_label[++i]= "debug"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_debug; row_form_width[i]= 5;
  row_form_label[++i]= "debug_check"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_debug_check); row_form_width[i]= 5;
  row_form_label[++i]= "debug_info"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_debug_info); row_form_width[i]= 5;
  row_form_label[++i]= "default_auth"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_default_auth; row_form_width[i]= 5;
  row_form_label[++i]= "default_character_set"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_set_charset_name; row_form_width[i]= 5;
  row_form_label[++i]= "defaults_extra_file"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_defaults_extra_file; row_form_width[i]= 5;
  row_form_label[++i]= "defaults_file"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_defaults_file; row_form_width[i]= 5;
  row_form_label[++i]= "defaults_group_suffix"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_defaults_group_suffix; row_form_width[i]= 5;
  row_form_label[++i]= "delimiter"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_delimiter_str; row_form_width[i]= 5;
  row_form_label[++i]= "enable_cleartext_plugin"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_enable_cleartext_plugin); row_form_width[i]= 5;
  row_form_label[++i]= "execute"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_execute; row_form_width[i]= 5;
  row_form_label[++i]= "force"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_force); row_form_width[i]= 5;
  row_form_label[++i]= "help"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0;  row_form_data[i]= QString::number(ocelot_help); row_form_width[i]= 5;
  row_form_label[++i]= "histfile"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_history_hist_file_name; row_form_width[i]= 5;
  row_form_label[++i]= "histignore"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_histignore; row_form_width[i]= 5;
  row_form_label[++i]= "html"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_html); row_form_width[i]= 5;
  row_form_label[++i]= "ignore_spaces"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_ignore_spaces); row_form_width[i]= 5;
  row_form_label[++i]= "ld_run_path"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_ld_run_path; row_form_width[i]= 5;
  if (is_libmysqlclient_loaded != 0) row_form_type[i]= (row_form_type[i] | READONLY_FLAG);
  row_form_label[++i]= "line_numbers"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_line_numbers); row_form_width[i]= 5;
  row_form_label[++i]= "local_infile"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_opt_local_infile); row_form_width[i]= 5;
  row_form_label[++i]= "login_path"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_login_path; row_form_width[i]= 5;
  row_form_label[++i]= "max_allowed_packet"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_max_allowed_packet); row_form_width[i]= 5;
  row_form_label[++i]= "max_join_size"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_max_join_size); row_form_width[i]= 5;
  row_form_label[++i]= "named_commands"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_named_commands); row_form_width[i]= 5;
  row_form_label[++i]= "net_buffer_length"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_net_buffer_length); row_form_width[i]= 5;
  row_form_label[++i]= "no_beep"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_no_beep); row_form_width[i]= 5;
  row_form_label[++i]= "no_defaults"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_no_defaults); row_form_width[i]= 5;
  row_form_label[++i]= "one_database"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_one_database); row_form_width[i]= 5;
  row_form_label[++i]= "pager"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_pager; row_form_width[i]= 5;
  row_form_label[++i]= "pipe"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_pipe); row_form_width[i]= 5;
  row_form_label[++i]= "plugin_dir"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_plugin_dir; row_form_width[i]= 5;
  row_form_label[++i]= "print_defaults"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_print_defaults); row_form_width[i]= 5;
  row_form_label[++i]= "prompt"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_prompt; row_form_width[i]= 5;
  row_form_label[++i]= "quick"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_quick); row_form_width[i]= 5;
  row_form_label[++i]= "raw"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_raw); row_form_width[i]= 5;
  row_form_label[++i]= "reconnect"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_opt_reconnect); row_form_width[i]= 5;
  row_form_label[++i]= "safe_updates"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_safe_updates); row_form_width[i]= 5;
  row_form_label[++i]= "secure_auth"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_secure_auth); row_form_width[i]= 5;
  row_form_label[++i]= "select_limit"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_select_limit); row_form_width[i]= 5;
  row_form_label[++i]= "server_public_key"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_server_public_key; row_form_width[i]= 5;
  row_form_label[++i]= "shared_memory_base_name"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_shared_memory_base_name; row_form_width[i]= 5;
  /* It used to crash if I said number(ocelot_history_includes_warnings). Problem has disappeared. */
  row_form_label[++i]= "show_warnings"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_history_includes_warnings); row_form_width[i]= 5;
  row_form_label[++i]= "sigint_ignore"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_sigint_ignore); row_form_width[i]= 5;
  row_form_label[++i]= "silent"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_silent); row_form_width[i]= 5;
  row_form_label[++i]= "ssl"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_ca"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_ca; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_capath"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_capath; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_cert"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_cert; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_cipher"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_cipher; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_crl"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_crl; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_crlpath"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_crlpath; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_key"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_key; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_mode"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_mode; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_verify_server_cert"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_opt_ssl_verify_server_cert); row_form_width[i]= 5;
  row_form_label[++i]= "syslog"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_syslog); row_form_width[i]= 5;
  row_form_label[++i]= "table"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_table); row_form_width[i]= 5;
  row_form_label[++i]= "tee"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_history_tee_file_name; row_form_width[i]= 5;
  row_form_label[++i]= "unbuffered"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_unbuffered); row_form_width[i]= 5;
  row_form_label[++i]= "verbose"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_verbose); row_form_width[i]= 5;
  row_form_label[++i]= "version"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_version); row_form_width[i]= 5;
  row_form_label[++i]= "vertical"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_result_grid_vertical); row_form_width[i]= 5;
  row_form_label[++i]= "wait"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_wait); row_form_width[i]= 5;
  row_form_label[++i]= "xml"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_xml); row_form_width[i]= 5;
  assert(i == column_count - 1);
  if (message == "Print")
  {
    char output_string[5120];
    for (int j= 0; j < i; ++j)
    {
      strcpy(output_string, row_form_label[j].toUtf8());
      printf("%-34s", output_string);
      strcpy(output_string, row_form_data[j].toUtf8());
      printf("%s\n", output_string);
    }
  }
  else /* if (message == "File|Connect") */
  {

    row_form_title= menu_strings[menu_off + MENU_CONNECTION_DIALOG_BOX];
    row_form_message= message;

    co= new Row_form_box(column_count, row_form_label,
                                       row_form_type,
                                       row_form_is_password, row_form_data,
  //                                     row_form_width,
                                       row_form_title,
                                       menu_strings[menu_off + MENU_FILE_CONNECT_HEADING],
                                       this);
    co->exec();

    if (co->is_ok == 1)
    {
      ocelot_host= row_form_data[0].trimmed();
      ocelot_port= to_long(row_form_data[1].trimmed());
      ocelot_user= row_form_data[2].trimmed();
      ocelot_database= row_form_data[3].trimmed();
      ocelot_unix_socket= row_form_data[4].trimmed();
      ocelot_password= row_form_data[5].trimmed();
      ocelot_protocol= row_form_data[6].trimmed(); ocelot_protocol_as_int= get_ocelot_protocol_as_int(ocelot_protocol);
      ocelot_init_command= row_form_data[7].trimmed();
      ocelot_abort_source_on_error= to_long(row_form_data[8].trimmed());
      ocelot_auto_rehash= to_long(row_form_data[9].trimmed());

      i= 10;
      ocelot_auto_vertical_output= to_long(row_form_data[i++].trimmed());
      ocelot_batch= to_long(row_form_data[i++].trimmed());
      ocelot_binary_mode= to_long(row_form_data[i++].trimmed());
      ocelot_bind_address= row_form_data[i++].trimmed();
      ocelot_set_charset_dir= row_form_data[i++].trimmed(); /* "character_sets_dir" */
      ocelot_result_grid_column_names= to_long(row_form_data[i++].trimmed());
      ocelot_column_type_info= to_long(row_form_data[i++].trimmed());
      ocelot_comments= to_long(row_form_data[i++].trimmed());
      ocelot_opt_compress= to_long(row_form_data[i++].trimmed());
      ocelot_opt_can_handle_expired_passwords= to_long(row_form_data[i++].trimmed());
      ocelot_opt_connect_timeout= to_long(row_form_data[i++].trimmed());
      ocelot_debug= row_form_data[i++].trimmed();
      ocelot_debug_check= to_long(row_form_data[i++].trimmed());
      ocelot_debug_info= to_long(row_form_data[i++].trimmed());
      ocelot_default_auth= row_form_data[i++].trimmed();
      ocelot_set_charset_name= row_form_data[i++].trimmed(); /* "default_character_set" */
      ocelot_defaults_extra_file= row_form_data[i++].trimmed();
      ocelot_defaults_file= row_form_data[i++].trimmed();
      ocelot_defaults_group_suffix= row_form_data[i++].trimmed();
      ocelot_delimiter_str= row_form_data[i++].trimmed();
      ocelot_enable_cleartext_plugin= to_long(row_form_data[i++].trimmed());
      ocelot_execute= row_form_data[i++].trimmed();
      ocelot_force= to_long(row_form_data[i++].trimmed());
      ocelot_help= to_long(row_form_data[i++].trimmed());
      ocelot_history_hist_file_name= row_form_data[i++].trimmed();
      ocelot_histignore= row_form_data[i++].trimmed();
      ocelot_html= to_long(row_form_data[i++].trimmed());
      ocelot_ignore_spaces= to_long(row_form_data[i++].trimmed());
      ocelot_ld_run_path= row_form_data[i++].trimmed();
      ocelot_line_numbers= to_long(row_form_data[i++].trimmed());
      ocelot_opt_local_infile= to_long(row_form_data[i++].trimmed());
      ocelot_login_path= row_form_data[i++].trimmed();
      ocelot_max_allowed_packet= to_long(row_form_data[i++].trimmed());
      ocelot_max_join_size= to_long(row_form_data[i++].trimmed());
      ocelot_named_commands= to_long(row_form_data[i++].trimmed());
      ocelot_net_buffer_length= to_long(row_form_data[i++].trimmed());
      ocelot_no_beep= to_long(row_form_data[i++].trimmed());
      ocelot_no_defaults= to_long(row_form_data[i++].trimmed());
      ocelot_one_database= to_long(row_form_data[i++].trimmed());
      ocelot_pager= row_form_data[i++].trimmed();
      ocelot_pipe= to_long(row_form_data[i++].trimmed());
      ocelot_plugin_dir= row_form_data[i++].trimmed();
      ocelot_print_defaults= to_long(row_form_data[i++].trimmed());
      ocelot_prompt= row_form_data[i++].trimmed();
      ocelot_quick= to_long(row_form_data[i++].trimmed());
      ocelot_raw= to_long(row_form_data[i++].trimmed());
      ocelot_opt_reconnect= to_long(row_form_data[i++].trimmed());
      ocelot_safe_updates= to_long(row_form_data[i++].trimmed());
      ocelot_secure_auth= to_long(row_form_data[i++].trimmed());
      ocelot_select_limit= to_long(row_form_data[i++].trimmed());
      ocelot_server_public_key= row_form_data[i++].trimmed();
      ocelot_shared_memory_base_name= row_form_data[i++].trimmed();
      ocelot_history_includes_warnings= to_long(row_form_data[i++].trimmed());
      ocelot_sigint_ignore= to_long(row_form_data[i++].trimmed());
      ocelot_silent= to_long(row_form_data[i++].trimmed());
      ocelot_opt_ssl= row_form_data[i++].trimmed();
      ocelot_opt_ssl_ca= row_form_data[i++].trimmed();
      ocelot_opt_ssl_capath= row_form_data[i++].trimmed();
      ocelot_opt_ssl_cert= row_form_data[i++].trimmed();
      ocelot_opt_ssl_cipher= row_form_data[i++].trimmed();
      ocelot_opt_ssl_crl= row_form_data[i++].trimmed();
      ocelot_opt_ssl_crlpath= row_form_data[i++].trimmed();
      ocelot_opt_ssl_key= row_form_data[i++].trimmed();
      ocelot_opt_ssl_mode= row_form_data[i++].trimmed();
      ocelot_opt_ssl_verify_server_cert= to_long(row_form_data[i++].trimmed());
      ocelot_syslog= to_long(row_form_data[i++].trimmed());
      ocelot_table= to_long(row_form_data[i++].trimmed());
      ocelot_history_tee_file_name= row_form_data[i++].trimmed();
      ocelot_unbuffered= to_long(row_form_data[i++].trimmed());
      ocelot_verbose= to_long(row_form_data[i++].trimmed());
      ocelot_version= row_form_data[i++].trimmed().toInt();
      ocelot_result_grid_vertical= to_long(row_form_data[i++].trimmed());
      ocelot_wait= to_long(row_form_data[i++].trimmed());
      ocelot_xml= to_long(row_form_data[i++].trimmed());
      assert(i == column_count);
      /* This should ensure that a record goes to the history widget */
      /* Todo: clear statement_edit_widget first */
      statement_edit_widget->insertPlainText("CONNECT");
      action_execute(1);
      if (ocelot_init_command > "")
      {
        statement_edit_widget->insertPlainText(ocelot_init_command);
        action_execute(1);
      }
    }
    delete(co);
  }

  if (row_form_width != 0) delete [] row_form_width;
  if (row_form_data != 0) delete [] row_form_data;
  if (row_form_is_password != 0)  delete [] row_form_is_password;
  if (row_form_type != 0) delete [] row_form_type;
  if (row_form_label != 0) delete [] row_form_label;
}


/*
  For menu item "exit" we said connect(...SLOT(action_exit())));
  By default this is on and associated with File|Exit menu item.
  Stop the program.
*/
void MainWindow::action_exit()
{
  log("action_exit start", 90);
  if (ocelot_dbms.contains("tarantool", Qt::CaseInsensitive))
  {
    /* Todo: if there was a successful connection, close it */
#ifdef OCELOT_OS_LINUX
#ifdef DBMS_TARANTOOL
#if (OCELOT_THIRD_PARTY != 1)
    if (is_libtarantool_loaded == 1) { dlclose(libtarantool_handle); is_libtarantool_loaded= 0; }
    //if (is_libtarantoolnet_loaded == 1) { dlclose(libtarantoolnet_handle); is_libtarantoolnet_loaded= 0; }
#endif
#endif
#endif
  }
  else
  {
#ifdef DEBUGGER
    /* Get rid of debuggee if it's still around. */
    /* Todo: this might not be enough. Maybe you should be intercepting the "close window" event. */
    if (menu_debug_action_exit->isEnabled() == true) action_debug_exit();
#endif
    /* Usually this closes main connection, kill connection or debug connection probably are = 0 */
    for (int i= 0; i < MYSQL_MAX_CONNECTIONS; ++i)
    {
      if (connected[i] != 0)
      {
        lmysql->ldbms_mysql_close(&mysql[i]);
        connected[i]= 0;
      }
    }
    if (is_mysql_library_init_done == true)
    {
      /*
        This assumes mysql_thread_end() was done for any debugger or kill threads,
        but we don't call mysql_thread_end() for the main thread (is that okay?).
        If we don't call mysql_library_end(), we'll get a few extra valgrind complaints.
      */
      lmysql->ldbms_mysql_library_end();
      is_mysql_library_init_done= false;
    }
    /* Some code added 2015-08-25 due to valgrind */
    if (lmysql != 0) { delete lmysql; lmysql= 0; }
#ifdef OCELOT_OS_LINUX
    if (is_libmysqlclient_loaded == 1) { dlclose(libmysqlclient_handle); is_libmysqlclient_loaded= 0; }
    if (is_libcrypto_loaded == 1) { dlclose(libcrypto_handle); is_libcrypto_loaded= 0; }
#endif
  }
  delete_utf8_copies();
  log("action_exit mid", 90);
  close();
  log("action_exit end", 90);
}


/*
  detach
  Would "floating" be a better word than "detached"? or "undock"? or "detachable"?
  Todo: We need a client statement for detached, e.g. SET result_grid_detached = 1; or "detach".
  Todo: Change border, size, title, frame width, position. Perhaps in Settings dialog.
  Todo: Menu keys should be slotted to the new window as well as the main window. E.g. control-Q.
  Todo: How to bring to front / bring to back? Currently it's always in front of main widget.
  Todo: title bar of result grid widget could show part of query and number of rows.
  Todo: Maybe these should be detached by default, or always detached.
  Todo: Check: Do you lose the parent? If that happens, there will be memory leaks.
  Todo: Detached widgets optionally could have their own menus. Lots of work.
        * Similar to create_menu() with all addActions() work, but mostly
          we can (after separating actions and adding #defines) copy with
          QAction *actionb = menu_edit->actions().at(EDIT_MENU_UNDO);
        * In every routine that uses action_edit_menu_undo etc., pass
          QMenu object so you can use actions().at.
        Otherwise click on menu changes focus to undetached widget.
*/

/*
  Flags for setWindowFlags().
  Doesn't include Qt::WindowCloseButtonHint so there will be no close button.
  Doesn't include Qt::WindowStaysOnTopHint but in fact it will be on top of other widgets of app.
*/
#define DETACHED_WINDOW_FLAGS Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint | Qt::WindowSystemMenuHint

/* menu item = Options|detach history widget */
void MainWindow::action_option_detach_history_widget(bool checked)
{
  bool is_visible= history_edit_widget->isVisible();
  ocelot_detach_history_widget= checked;
  if (checked)
  {
    menu_options_action_option_detach_history_widget->setText("attach history widget");
    history_edit_widget->setWindowFlags(Qt::Window | DETACHED_WINDOW_FLAGS);
    history_edit_widget->setWindowTitle("history widget");
    history_edit_widget->detach_start();
  }
  else
  {
    menu_options_action_option_detach_history_widget->setText("detach history widget");
    history_edit_widget->setWindowFlags(Qt::Widget);
    history_edit_widget->detach_stop();
  }
  if (is_visible) history_edit_widget->show();
}


/* menu item = Options|detach result grid widget */
void MainWindow::action_option_detach_result_grid_widget(bool checked)
{
  bool is_visible= result_grid_tab_widget->isVisible();
  ocelot_detach_result_grid_widget= checked;
  if (checked)
  {
    menu_options_action_option_detach_result_grid_widget->setText("attach result grid widget");
    result_grid_tab_widget->setWindowFlags(Qt::Window | DETACHED_WINDOW_FLAGS);
    result_grid_tab_widget->setWindowTitle("result grid widget");
  }
  else
  {
    menu_options_action_option_detach_result_grid_widget->setText("detach result grid widget");
    result_grid_tab_widget->setWindowFlags(Qt::Widget);
  }
  if (is_visible) result_grid_tab_widget->show();
}


/* menu item = Options|detach debug widget */
void MainWindow::action_option_detach_debug_widget(bool checked)
{
  bool is_visible= debug_tab_widget->isVisible();
  ocelot_detach_debug_widget= checked;
  if (checked)
  {
    menu_options_action_option_detach_debug_widget->setText("attach debug widget");
    debug_tab_widget->setWindowFlags(Qt::Window| DETACHED_WINDOW_FLAGS);
    debug_tab_widget->setWindowTitle("debug widget");
  }
  else
  {
    menu_options_action_option_detach_debug_widget->setText("detach debug widget");
    debug_tab_widget->setWindowFlags(Qt::Widget);
  }
  if (is_visible) debug_tab_widget->show();
}

/*
  Next/Prev focus widget. Qt hardcodes tab | Shift+Tab (backtab) see
  http://www.vikingsoft.eu/keyboard-navigation-and-the-event-system/
  but we want to allow putting tab in any editable window, that is,
  we hardcode autocomplete shortcut = tab but allow it to go in.
  I'm confused about whether the window manager e.g. KDE always will
  override (as it does for Alt+Tab), but I know that if I try
  SET ocelot_shortcut_autocomplete = 'Alt+Tab';
  I get an error message = Illegal value.
  Row_form_box uses setTabChangesFocus() but nothing else does.
  In result grid, maybe we could have Tab for "next cell" and
  shift+tab for "next window". and something else for "Next Tab".
  But, mainly: next/prev is done with a customizable shortcut
  associated with QKeySequence::NextChild / QKeySequence::PreviousChild
  focusNextPrevChild(true) focusNextPrevChild(false).
  See also "Navigating Between Window Panes" according to Microsoft
  https://msdn.microsoft.com/en-us/library/ms971323.aspx
*/

void MainWindow::action_option_next_window()
{
  focusNextPrevChild(true);
}

void MainWindow::action_option_previous_window()
{
  focusNextPrevChild(false);
}

/*
  Called from action_about("ocelotgui_logo.png") and action_the_manual("README.htm").
  We might assume that the docs including .png or .jpg files are on application_dir_path
  -- applicationDirPath() ""Returns the directory that contains the application executable" --
  but at build time one can pass -DOCELOTGUI_DOCDIR=x, indeed this is what happens with
  an install via cmake+cpack because they want the docs on /usr/share/doc not /usr/bin.
  But maybe cmake said it would go to /usr/local/share/doc but in fact it went to /usr/share/doc.
  So if we fail once, we try again after stripping /local from the path.
  Todo: have more choice where to look for README.md
        we could try: according to an option = "documentation" directory
                      ld_run_path, ocelot_login_path, ocelot_plugin_dir
                      some other path used by Qt or MySQL or Linux
                      (prefer the path that has everything)
*/
QString MainWindow::get_doc_path(QString file_name)
{
#ifdef OCELOTGUI_DOCDIR
  QString application_dir_path= OCELOTGUI_DOCDIR;
#else
  QString application_dir_path= QCoreApplication::applicationDirPath();
#endif
  QString readme_path= application_dir_path;
  readme_path.append("/");
  readme_path.append(file_name);
  QFile file(readme_path);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    file.close();
    return application_dir_path;
  }
 #ifdef OCELOTGUI_DOCDIR
  int index_of_local= application_dir_path.indexOf("/local/");
  if (index_of_local != -1)
  {
    QString left_path= application_dir_path.left(index_of_local);
    QString right_path= application_dir_path.right(application_dir_path.length() - (index_of_local + strlen("/local")));
    application_dir_path= left_path + right_path;
    readme_path= application_dir_path;
    readme_path.append("/");
    readme_path.append(file_name);
    file.setFileName(readme_path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      file.close();
      return application_dir_path;
    }
  }
#endif
  return "";
}


void MainWindow::action_about()
{
  QString the_text= "\
<img src=\"./ocelotgui_logo.png\" alt=\"ocelotgui_logo.png\">\
<b>ocelotgui -- Ocelot Graphical User Interface</b><br>Copyright (c) 2014-2017 by Ocelot Computer Services Inc.<br>\
This program is free software: you can redistribute it and/or modify \
it under the terms of the GNU General Public License as published by \
the Free Software Foundation, version 2 of the License,<br>\
<br>\
This program is distributed in the hope that it will be useful, \
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
GNU General Public License for more details.<br>\
<br>\
You should have received a copy of the GNU General Public License \
along with this program.  If not, see &lt;http://www.gnu.org/licenses/&gt;.";

  the_text.append("<br><br>");
  the_text.append(get_version());
  if (is_mysql_library_init_done == true)
  {
    the_text.append("<br>using DBMS client library version ");
    the_text.append(lmysql->ldbms_mysql_get_client_info());
  }
  if (statement_edit_widget->dbms_version > "")
  {
    the_text.append("<br>using DBMS server version ");
    the_text.append(statement_edit_widget->dbms_version);
  }
  the_text.append("<br>Some source code related to the debugger feature is ");
  the_text.append("<br>(c) Copyright 2012 Hewlett-Packard Development Company, L.P.");
#if (OCELOT_STATIC_LIBRARY == 1)
  the_text.append("<br>Static-linked to MariaDB Connector C library");
  the_text.append("<br>Copyright (C) 2000 MySQL AB & MySQL Finland AB & TCX DataKonsult AB");
  the_text.append("<br>Static-linked to Qt's GUI library");
  the_text.append("<br>Copyright 2008-2014 Digia Plc. All rights reserved.");
#endif
  QString application_dir_path= get_doc_path("ocelotgui_logo.png");
  if (application_dir_path != "")
  {
    QString img_path= "img src=\"";
    img_path.append(application_dir_path);
    img_path.append("/");
    the_text.replace("img src=\"", img_path);
  }
  Message_box *message_box;
  message_box= new Message_box("Help|About", the_text, 500, this);
  message_box->exec();
  delete message_box;
}


/*
  the_manual_widget will be shown when user clicks Help | The Manual.
  It does not inherit the fonts+colors settings of the main window (that's a message_box todo).
  It reads the manual from README.md.
  It uses HTML.
  README.md refers to img src="...png|jpg" files, we expect them on the same directory as README.md.
   Todo: Help | X should find X in the manual and display only X.
*/
void MainWindow::action_the_manual()
{
  QString the_text="\
  <BR><h1>ocelotgui</h1>  \
  <BR>  \
  <BR>Version 1.0.6, December 12 2017  \
  <BR>  \
  <BR>  \
  <BR>Copyright (c) 2014-2017 by Ocelot Computer Services Inc. All rights reserved.  \
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
  <BR>For the most recent version of the manual, see \
  <BR>https://github.com/ocelot-inc/ocelotgui#user-manual \
    ";
  QString application_dir_path= get_doc_path("README.md");
  if (application_dir_path != "")
  {
    QString readme_path= application_dir_path;
    readme_path.append("/");
    readme_path.append("README.md");
    QFile file(readme_path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QString line;
      QTextStream in(&file);
      the_text= "";
      while(!in.atEnd())
      {
        line= in.readLine();
        the_text.append(line);
        the_text.append(" ");
      }
      file.close();
      QString img_path= "img src=\"";
      img_path.append(application_dir_path);
      img_path.append("/");
      the_text.replace("img src=\"", img_path);
    }
  }
  Message_box *message_box;
  /* Don't use width=960 if screen width is smaller, e.g. on a VGA screen. */
  QDesktopWidget desktop;
  int desktop_width= desktop.availableGeometry().width();
  if (desktop_width > (960 + 50)) message_box= new Message_box("Help|The Manual", the_text, 960, this);
  else message_box= new Message_box("Help|The Manual", the_text, desktop_width - 50, this);
  message_box->exec();
  delete message_box;
}


void MainWindow::action_libmysqlclient()
{
  QString the_text= "<b>libmysqlclient</b><br>\
  Before ocelotgui can try to connect to a MySQL server, \
  it needs a shared library named libmysqlclient \
  (file name on Linux is 'libmysqlclient.so' and/or 'libmysqlclient.so.18'). \
  If a mysql client was installed, possibly due to install \
  of a package named 'libmysqlclient-dev' or something similar, \
  then this file exists ... somewhere. \
  ocelotgui searches for libmysqlclient.so in these directories:<br> \
  (1) as specified by environment variable LD_RUN_PATH<br> \
  (2) as specified by environment variable LD_LIBRARY_PATH<br> \
  (3) as specified during build in file ocelotgui.pro, \
  which are by default hard-coded as: /usr/local/lib \
  /usr/mysql/lib /usr/local/mysql/lib /usr/lib /usr/local/lib/mysql \
  /usr/lib/mysql /usr/local /usr/local/mysql /usr/local /usr, etc.<br> \
  If a message appears saying libmysqlclient cannot be found, \
  or if there is a suspicion that an obsolete copy of libmysqlclient \
  was found, a possible solution is:<br> \
  1. Find the right libmysqlclient.so with Linux 'find' or 'locate'. \
  Suppose it is /home/jeanmartin/libmysqlclient.so.<br> \
  2. Specify the library when starting ocelotgui, thus:<br> \
  LD_RUN_PATH=/home/jeanmartin ocelotgui<br> \
  ... ocelotgui will also look for libmariadbclient.so or \
  libmariadb.so in the same fashion but will look first for \
  libmysqlclient.so unless one starts with ocelot_dbms='mariadb'.";
  Message_box *message_box;
  message_box= new Message_box("Help|libmysqlclient", the_text, 500, this);
  message_box->exec();
  delete message_box;
}


void MainWindow::action_settings()
{
  QString the_text= "<b>settings</b><br>\
  For all widgets, the color and font are changeable.<br>\
          <br>\
  CHANGING COLORS<br>\
  For example, to change the statement background color to dark blue:<br>\
  * Click the menu item Settings<br>\
    then click Statement Widget: Colors and Fonts<br>\
    then click the box on the right of the Statement Background Color line<br>\
    then click on the word 'DarkBlue' in the list of choices<br>\
    then click OK to confirm<br>\
  * Or, Start ocelotgui with a parameter:<br>\
    ocelotgui --ocelot_statement_background-color='DarkBlue'<br>\
  * Or, put a line permanently in an option file such as ~/.my.cnf<br>\
    [ocelot]<br>\
    ocelot_statement_background_color='DarkBlue'<br>\
  * Or, execute a statement<br>\
    SET ocelot_statement_background_color='DarkBlue';<br>\
  Color names may be any of the many names listed on this W3C chart: \
  http://www.w3.org/wiki/CSS/Properties/color/keywords \
  as well as additions from this unofficial X11 chart: \
  https://en.wikipedia.org/wiki/X11_color_names#Color_name_chart. \
  Hexadecimal-RGB values for example '#0000FE' are allowed in parameters or statements. \
  <br>\
  <br>\
  CHANGING FONTS<br>\
  For example, to change the menu font to bold italic:<br>\
  * Click the menu item Settings<br>\
    then click Menu: Colors and Fonts<br>\
    then click the Pick New Font button<br>\
    then under the label Style: scroll to Italic<br>\
    then click OK<br>\
    then click OK to confirm<br>\
  * Or, Start ocelotgui with a parameter:<br>\
    ocelotgui --ocelot_menu_font_style='italic'<br>\
  * Or, put a line permanently in an option file such as ~/.my.cnf<br>\
    [ocelot]<br>\
    ocelot_menu_font_style='italic'<br>\
  * Or, execute a statement<br>\
    SET ocelot_menu_font_style = 'italic';<br>\
  Some font changes do not take effect until after the next statement \
  is executed. \
  Some extremely large font sizes will be accepted but \
  the results will be ugly.";
  Message_box *message_box;
  message_box= new Message_box("Help|libmysqlclient", the_text, 500, this);
  message_box->exec();
  delete message_box;
}


/*
  Problem: to do syntax highlighting, one must have a slot
  action_statement_edit_widget_text_changed() to intercept
  every change in action_statement_edit_widget. But, when
  user chooses Undo, that interception happens again, which
  negates the effects of undo. Therefore, instead of getting
  slot() invoked directly, I get to action_undo() which temporarily
  disables use of action_statement_edit_widget_changed().
  And of course the same considerations apply for redo().
  KLUDGE #1: In action_statement_edit_widget_changed(), change
             cur.beginEditBlock() to cur.joinPreviousEditBlock()
             so the syntax-highlight will be merged with the
             content change.
  KLUDGE #2: If a change is done via key stroke, then the stack
             gets 2 items, one of which is junk (just moving the
             cursor to after the first word), and wow, I wish I
             knew why. If a change is done via cut-and-paste,
             there's only one item. So I have to look at what
             the change was, and discard the junk item if I find
             it with undo, or put it back on the stack if I find
             it with redo.
  KLUDGE #3: Usually redo() moves the cursor the end, and wow, I
             wish I knew why. So I calculate where it should be =
             position of the first character that differs, plus
             (chars added - chars removed) (if positive).
  Todo: consider: will there be a bug if syntax highlighting is disabled?
        (currently it seems that disabling doesn't work so I can't test)
  Todo: see what happens for other situations besides
        key stroke | cut | paste. E.g. we get rid of a
        statement at a time in multi-statement sendings.
  Todo: menu item enable|disable -- not simple, it will
        depend what has focus
*/
void MainWindow::action_undo()
{
  log("action_undo start", 90);
  statement_edit_widget_text_changed_flag= 1;
  QString text_before= statement_edit_widget->toPlainText();
  statement_edit_widget->undo();
  QString text_after= statement_edit_widget->toPlainText();
  if (text_before == text_after)
    statement_edit_widget->undo();
  statement_edit_widget_text_changed_flag= 0;
  log("action_undo end", 90);
}

void MainWindow::action_redo()
{
  log("action_redo start", 90);
  QTextCursor cur= statement_edit_widget->textCursor();
  QString text_before= statement_edit_widget->toPlainText();
  statement_edit_widget_text_changed_flag= 1;
  statement_edit_widget->redo();
  int final_pos_for_redo;
  QString text_after_1= statement_edit_widget->toPlainText();
  statement_edit_widget->redo();
  QString text_after_2= statement_edit_widget->toPlainText();
  if (text_after_2 != text_after_1)
    statement_edit_widget->undo();
  statement_edit_widget_text_changed_flag= 0;
  for (final_pos_for_redo= 0; final_pos_for_redo < text_before.length(); ++final_pos_for_redo)
  {
    if (text_before.mid(final_pos_for_redo,1) != text_after_1.mid(final_pos_for_redo,1))
      break;
  }
  if (chars_added_for_redo > chars_removed_for_redo)
    final_pos_for_redo+= (chars_added_for_redo - chars_removed_for_redo);
  if (final_pos_for_redo > text_after_1.length())
  {
    log("FINAL_POS_FOR_REDO WAS ADJUSTED", 90);
    final_pos_for_redo= text_after_1.length();
  }
  cur.setPosition(final_pos_for_redo);
  statement_edit_widget->setTextCursor(cur);
  log("action_redo end", 90);
}

/*
  When focusOut happens for one of the main widgets (the ones that have
  an event filter on them), we might be switching to the menu bar.
  (I wish I knew for sure, but if it's to something else, no harm done.)
  On the edit menu, some items should be enabled or disabled (gray).
  Inefficient, but these actions don't happen dozens of times per second.
  Incredibly, installeventfilter for menuBar + look for focusIn or
  focusOut on menuBar didn't work, so this will have to do.
  Objects with event filters are: ResultGrid, CodeEditor,
  QScrollBar, TextEditHistory, so they all cause a call to here,
  but we only care about CodeEditor and TextEditHistory. Also we
  call from TextEditWidget::focusOutEvent(), an override rather
  than an editfilter event.
  Warn: TextEditWidget::focusInEvent() is also an override rather than
        an editfilter event, because (I don't know why) otherwise we
        don't see when TextEditWidget get focus or loses focus.
        This is the only time when QEvent::focusIn matters. So usually
        menu_activations() for menu_activations(QEvent::focusIn)
        is a waste of time. Remember, though, that shortcuts often won't
        work if the menu item is disabled.
  Todo: Incredibly, availableRedoSteps() only works for CodeEditor i.e.
        statement_edit_widget. I wish I knew why, but rather than spend
        more hours, will just assume redo is otherwise always okay.
        Maybe someday look harder.
  Todo: Saving the object so you'd know to setFocus to it after being on
        the menu would be good, at the moment it isn't automatic if the
        widget is detached.
  Todo: Other edit menu items can be enabled|disabled:
        previous statement, next statement.
  Todo: if font_size already <= FONT_SIZE_MIN, disable zoomout.
        if font_size already >= FONT_SIZE_MAX, disable zoomout.
        (Get the widget's ->styleSheet() as you do elsewhere.)
*/
void MainWindow::menu_activations(QObject *focus_widget, QEvent::Type qe)
{
  bool is_can_undo= true, is_can_redo= true;
  bool is_can_copy= false, is_can_cut= false, is_can_paste= false;
  bool is_can_format= false, is_can_zoomin= false, is_can_zoomout= false;
  bool is_can_autocomplete= false;
  const char *class_name= focus_widget->metaObject()->className();
  if (qe == QEvent::FocusIn)
  {
    if (strcmp(class_name, "TextEditWidget") != 0) return;
  }
  if (strcmp(class_name, "CodeEditor") == 0)
  {
    CodeEditor *t= qobject_cast<CodeEditor*>(focus_widget);
    QTextDocument *doc= t->document();
    if (doc->availableUndoSteps() <= 0) is_can_undo= false;
    if (doc->availableRedoSteps() <= 0) is_can_redo= false;
    is_can_copy= is_can_cut= t->textCursor().hasSelection();
    is_can_paste= t->canPaste();
    is_can_format= is_can_zoomin= is_can_zoomout= !doc->isEmpty();
    if (ocelot_auto_rehash > 0)
    {
      if (hparse_line_edit->isHidden() == false)
      {
        if (statement_edit_widget->hasFocus() == true)
        {
          is_can_autocomplete= true;
        }
      }
    }
  }
  else if (strcmp(class_name, "TextEditWidget") == 0)
  {
    TextEditWidget *t= qobject_cast<TextEditWidget*>(focus_widget);
    QTextDocument *doc= t->document();
    if (doc->availableUndoSteps() <= 0) is_can_undo= false;
    if (t->text_edit_frame_of_cell->is_image_flag)
    {
      /* zoomin + zoomout are already false */
      is_can_cut= false;
      is_can_paste= false;
    }
    else
    {
      is_can_copy= is_can_cut= t->textCursor().hasSelection();
      is_can_paste= t->canPaste();
      is_can_zoomin= is_can_zoomout= !doc->isEmpty();
    }
    is_can_format= false;
  }
  else if (strcmp(class_name, "TextEditHistory") == 0)
  {
    TextEditHistory *t= qobject_cast<TextEditHistory*>(focus_widget);
    QTextDocument *doc= t->document();
    if (doc->availableUndoSteps() <= 0) is_can_undo= false;
    is_can_copy= is_can_cut= t->textCursor().hasSelection();
    is_can_paste= t->canPaste();
    is_can_format= false;
    is_can_zoomin= is_can_zoomout= !doc->isEmpty();
  }
  else
  {
    return;
  }
  menu_edit_action_undo->setEnabled(is_can_undo);
  menu_edit_action_redo->setEnabled(is_can_redo);
  menu_edit_action_cut->setEnabled(is_can_cut);
  menu_edit_action_copy->setEnabled(is_can_copy);
  menu_edit_action_paste->setEnabled(is_can_paste);
  menu_edit_action_formatter->setEnabled(is_can_format);
  menu_edit_action_zoomin->setEnabled(is_can_zoomin);
  menu_edit_action_zoomout->setEnabled(is_can_zoomout);
  menu_edit_action_autocomplete->setEnabled(is_can_autocomplete);
}


/*
  Comments relevant to action_statement(), action_grid(), action_history(), action_menu() ...
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
/* Todo: make sure there's no need to call set_current_colors_and_font(); */
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

void MainWindow::action_statement()
{
  Settings *se= new Settings(STATEMENT_WIDGET, this);
  int result= se->exec();
  if (result == QDialog::Accepted)
  {
    //make_style_strings();
    //statement_edit_widget_setstylesheet();
    /* For each changed Settings item, produce and execute a settings-change statement. */
    action_change_one_setting(ocelot_statement_text_color, new_ocelot_statement_text_color,"ocelot_statement_text_color");
    action_change_one_setting(ocelot_statement_background_color, new_ocelot_statement_background_color,"ocelot_statement_background_color");
    action_change_one_setting(ocelot_statement_border_color, new_ocelot_statement_border_color,"ocelot_statement_border_color");
    action_change_one_setting(ocelot_statement_font_family, new_ocelot_statement_font_family,"ocelot_statement_font_family");
    action_change_one_setting(ocelot_statement_font_size, new_ocelot_statement_font_size,"ocelot_statement_font_size");
    action_change_one_setting(ocelot_statement_font_style, new_ocelot_statement_font_style,"ocelot_statement_font_style");
    action_change_one_setting(ocelot_statement_font_weight, new_ocelot_statement_font_weight,"ocelot_statement_font_weight");
    action_change_one_setting(ocelot_statement_highlight_literal_color, new_ocelot_statement_highlight_literal_color,"ocelot_statement_highlight_literal_color");
    action_change_one_setting(ocelot_statement_highlight_identifier_color, new_ocelot_statement_highlight_identifier_color,"ocelot_statement_highlight_identifier_color");
    action_change_one_setting(ocelot_statement_highlight_comment_color, new_ocelot_statement_highlight_comment_color,"ocelot_statement_highlight_comment_color");
    action_change_one_setting(ocelot_statement_highlight_operator_color, new_ocelot_statement_highlight_operator_color,"ocelot_statement_highlight_operator_color");
    action_change_one_setting(ocelot_statement_highlight_keyword_color, new_ocelot_statement_highlight_keyword_color,"ocelot_statement_highlight_keyword_color");
    action_change_one_setting(ocelot_statement_prompt_background_color, new_ocelot_statement_prompt_background_color,"ocelot_statement_prompt_background_color");
    action_change_one_setting(ocelot_statement_highlight_current_line_color, new_ocelot_statement_highlight_current_line_color,"ocelot_statement_highlight_current_line_color");
    action_change_one_setting(ocelot_statement_highlight_function_color, new_ocelot_statement_highlight_function_color,"ocelot_statement_highlight_function_color");
    action_change_one_setting(ocelot_statement_syntax_checker, new_ocelot_statement_syntax_checker,"ocelot_statement_syntax_checker");

    /* Todo: consider: maybe you have to do a restore like this */
    //text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */
    //tokenize(text.data(),
    //         text.size(),
    //         main_token_lengths, main_token_offsets, MAX_TOKENS, (QChar*)"33333", 1, ocelot_delimiter_str, 1);
    //tokens_to_keywords(text);
    /* statement_edit_widget->statement_edit_widget_left_bgcolor= QColor(ocelot_statement_prompt_background_color); */
    statement_edit_widget->statement_edit_widget_left_treatment1_textcolor= QColor(ocelot_statement_text_color);
    action_statement_edit_widget_text_changed(0, 0, 0);            /* only for highlight? repaint so new highlighting will appear */
  }
  delete(se);
}

/*
  The setstylesheet here could take a long time if there are many child widgets being displayed.
*/
void MainWindow::action_grid()
{
  Settings *se= new Settings(GRID_WIDGET, this);
  int result= se->exec();
  if (result == QDialog::Accepted)
  {
    //make_style_strings();                                                      /* I think this should be commented out */
    //result_grid_tab_widget[0]->set_all_style_sheets();
    /* For each changed Settings item, produce and execute a settings-change statement. */
    action_change_one_setting(ocelot_grid_text_color, new_ocelot_grid_text_color, "ocelot_grid_text_color");
    action_change_one_setting(ocelot_grid_border_color, new_ocelot_grid_border_color, "ocelot_grid_border_color");
    action_change_one_setting(ocelot_grid_background_color, new_ocelot_grid_background_color, "ocelot_grid_background_color");
    action_change_one_setting(ocelot_grid_header_background_color, new_ocelot_grid_header_background_color, "ocelot_grid_header_background_color");
    action_change_one_setting(ocelot_grid_font_family, new_ocelot_grid_font_family, "ocelot_grid_font_family");
    action_change_one_setting(ocelot_grid_font_size, new_ocelot_grid_font_size, "ocelot_grid_font_size");
    action_change_one_setting(ocelot_grid_font_style, new_ocelot_grid_font_style, "ocelot_grid_font_style");
    action_change_one_setting(ocelot_grid_font_weight, new_ocelot_grid_font_weight, "ocelot_grid_font_weight");
    action_change_one_setting(ocelot_grid_cell_border_color, new_ocelot_grid_cell_border_color, "ocelot_grid_cell_border_color");
    action_change_one_setting(ocelot_grid_cell_drag_line_color, new_ocelot_grid_cell_drag_line_color, "ocelot_grid_cell_drag_line_color");
    action_change_one_setting(ocelot_grid_border_size, new_ocelot_grid_border_size, "ocelot_grid_border_size");
    action_change_one_setting(ocelot_grid_cell_border_size, new_ocelot_grid_cell_border_size, "ocelot_grid_cell_border_size");
    action_change_one_setting(ocelot_grid_cell_drag_line_size, new_ocelot_grid_cell_drag_line_size, "ocelot_grid_cell_drag_line_size");
  }
  delete(se);
}


/*
  TODO: extra_rule_1 could affect statement | prompt | word, not just cell
  TODO: Action could be to invoke a Lua procedure, not just char/hex/image
  TODO: There are lots of things to add to Condition and to Display As
*/
void MainWindow::action_extra_rule_1()
{
  Settings *se= new Settings(EXTRA_RULE_1, this);
  int result= se->exec();
  if (result == QDialog::Accepted)
  {
    action_change_one_setting(ocelot_extra_rule_1_condition, new_ocelot_extra_rule_1_condition, "ocelot_extra_rule_1_condition");
    action_change_one_setting(ocelot_extra_rule_1_text_color, new_ocelot_extra_rule_1_text_color, "ocelot_extra_rule_1_text_color");
    action_change_one_setting(ocelot_extra_rule_1_background_color, new_ocelot_extra_rule_1_background_color, "ocelot_extra_rule_1_background_color");
    action_change_one_setting(ocelot_extra_rule_1_display_as, new_ocelot_extra_rule_1_display_as, "ocelot_extra_rule_1_display_as");
 }
  delete(se);
}


void MainWindow::action_history()
{
  Settings *se= new Settings(HISTORY_WIDGET, this);
  int result= se->exec();
  if (result == QDialog::Accepted)
  {
    //make_style_strings();
    //history_edit_widget->setStyleSheet(ocelot_history_style_string);
    action_change_one_setting(ocelot_history_text_color, new_ocelot_history_text_color, "ocelot_history_text_color");
    action_change_one_setting(ocelot_history_background_color, new_ocelot_history_background_color, "ocelot_history_background_color");
    action_change_one_setting(ocelot_history_border_color, new_ocelot_history_border_color, "ocelot_history_border_color");
    action_change_one_setting(ocelot_history_font_family, new_ocelot_history_font_family, "ocelot_history_font_family");
    action_change_one_setting(ocelot_history_font_size, new_ocelot_history_font_size, "ocelot_history_font_size");
    action_change_one_setting(ocelot_history_font_style, new_ocelot_history_font_style, "ocelot_history_font_style");
    action_change_one_setting(ocelot_history_font_weight, new_ocelot_history_font_weight, "ocelot_history_font_weight");
    action_change_one_setting(ocelot_history_max_row_count, new_ocelot_history_max_row_count, "ocelot_history_max_row_count");
  }
  delete(se);
}

/* We used to change main_window but there's too much inheriting. Really, menu is what matters. */
void MainWindow::action_menu()
{
  Settings *se= new Settings(MAIN_WIDGET, this);
  int result= se->exec();
  if (result == QDialog::Accepted)
  {
    //make_style_strings();
    //main_window->setStyleSheet(ocelot_menu_style_string);
    //ui->menuBar->setStyleSheet(ocelot_menu_style_string);
      action_change_one_setting(ocelot_menu_text_color, new_ocelot_menu_text_color, "ocelot_menu_text_color");
      action_change_one_setting(ocelot_menu_background_color, new_ocelot_menu_background_color, "ocelot_menu_background_color");
      action_change_one_setting(ocelot_menu_border_color, new_ocelot_menu_border_color, "ocelot_menu_border_color");
      action_change_one_setting(ocelot_menu_font_family, new_ocelot_menu_font_family, "ocelot_menu_font_family");
      action_change_one_setting(ocelot_menu_font_size, new_ocelot_menu_font_size, "ocelot_menu_font_size");
      action_change_one_setting(ocelot_menu_font_style, new_ocelot_menu_font_style, "ocelot_menu_font_style");
      action_change_one_setting(ocelot_menu_font_weight, new_ocelot_menu_font_weight, "ocelot_menu_font_weight");
  }
  delete(se);
}

/*
  If a Settings-menu use has caused a change:
  Produce a settings-change SQL statement, e.g.
  SET ocelot_statement_background_color = 'red'
  and execute it.
  If it's a color, it might be an RGB, in that case change to name first.
*/
void MainWindow::action_change_one_setting(QString old_setting,
                                           QString new_setting,
                                           const char *name_of_setting)
{
  if (old_setting != new_setting)
  {
    QString text;
    old_setting= new_setting;
    main_token_number= 0;
    text= "SET ";
    text.append(name_of_setting);
    text.append(" = ");
    text.append("'");
    QString source= new_setting;
    if (strstr(name_of_setting, "_color") != NULL)
      source= rgb_to_color(new_setting);
    for (int i= 0; i < source.length(); ++i)
    {
      QString c= source.mid(i, 1);
      text.append(c);
      if (c == "'") text.append(c);
    }
    text.append("';");
    main_token_count_in_statement= 5;
    main_token_new(text.size());
    tokenize(text.data(),
             text.size(),
             main_token_lengths, main_token_offsets,
             main_token_max_count, (QChar*)"33333", 1,
             ocelot_delimiter_str, 1);
   tokens_to_keywords(text, 0, sql_mode_ansi_quotes);
   action_execute_one_statement(text);
   history_edit_widget->verticalScrollBar()->setValue(history_edit_widget->verticalScrollBar()->maximum());
  }
}


/*
  Pass QString with rgb name '#RRGGBB'.
  Find that string in q_color_list.
  If it's not there: add it.
*/
QString MainWindow::q_color_list_name(QString rgb_name)
{
  for (int i= 1; i < q_color_list.size(); i+= 2)
  {
    if (QString::compare(rgb_name, q_color_list[i], Qt::CaseInsensitive) == 0) return rgb_name;
  }
  q_color_list.append(rgb_name);
  q_color_list.append(rgb_name);
  return rgb_name;
}

/*
  It's possible to say SET [ocelot color name] = rgb_name e.g. "#FFFFFF"
  or have rgb_name in an options file or get an RGB name with set_current_colors_and_fonts().
  We'd prefer to show as W3C names where possible, X11 names as second choice, if English.
  But where the color is not in the W3C or X11 lists, we'll add.
*/
void MainWindow::assign_names_for_colors()
{
  if (ocelot_statement_text_color.left(1) == "#") ocelot_statement_text_color= q_color_list_name(ocelot_statement_text_color);
  if (ocelot_statement_background_color.left(1) == "#") ocelot_statement_background_color= q_color_list_name(ocelot_statement_background_color);
  if (ocelot_statement_border_color.left(1) == "#") ocelot_statement_border_color= q_color_list_name(ocelot_statement_border_color);
  if (ocelot_statement_highlight_literal_color.left(1) == "#") ocelot_statement_highlight_literal_color= q_color_list_name(ocelot_statement_highlight_literal_color);
  if (ocelot_statement_highlight_identifier_color.left(1) == "#") ocelot_statement_highlight_identifier_color= q_color_list_name(ocelot_statement_highlight_identifier_color);
  if (ocelot_statement_highlight_comment_color.left(1) == "#") ocelot_statement_highlight_comment_color= q_color_list_name(ocelot_statement_highlight_comment_color);
  if (ocelot_statement_highlight_operator_color.left(1) == "#") ocelot_statement_highlight_operator_color= q_color_list_name(ocelot_statement_highlight_operator_color);
  if (ocelot_statement_highlight_keyword_color.left(1) == "#") ocelot_statement_highlight_keyword_color= q_color_list_name(ocelot_statement_highlight_keyword_color);
  if (ocelot_statement_prompt_background_color.left(1) == "#") ocelot_statement_prompt_background_color= q_color_list_name(ocelot_statement_prompt_background_color);
  if (ocelot_statement_highlight_current_line_color.left(1) == "#") ocelot_statement_highlight_current_line_color= q_color_list_name(ocelot_statement_highlight_current_line_color);
  if (ocelot_statement_highlight_function_color.left(1) == "#") ocelot_statement_highlight_function_color= q_color_list_name(ocelot_statement_highlight_function_color);
  if (ocelot_grid_text_color.left(1) == "#") ocelot_grid_text_color= q_color_list_name(ocelot_grid_text_color);
  if (ocelot_grid_border_color.left(1) == "#") ocelot_grid_border_color= q_color_list_name(ocelot_grid_border_color);
  if (ocelot_grid_background_color.left(1) == "#") ocelot_grid_background_color= q_color_list_name(ocelot_grid_background_color);
  if (ocelot_grid_header_background_color.left(1) == "#") ocelot_grid_header_background_color= q_color_list_name(ocelot_grid_header_background_color);
  if (ocelot_grid_font_family.left(1) == "#") ocelot_grid_font_family= q_color_list_name(ocelot_grid_font_family);
  if (ocelot_grid_font_size.left(1) == "#") ocelot_grid_font_size= q_color_list_name(ocelot_grid_font_size);
  if (ocelot_grid_font_style.left(1) == "#") ocelot_grid_font_style= q_color_list_name(ocelot_grid_font_style);
  if (ocelot_grid_font_weight.left(1) == "#") ocelot_grid_font_weight= q_color_list_name(ocelot_grid_font_weight);
  if (ocelot_grid_cell_border_color.left(1) == "#") ocelot_grid_cell_border_color= q_color_list_name(ocelot_grid_cell_border_color);
  if (ocelot_grid_cell_drag_line_color.left(1) == "#") ocelot_grid_cell_drag_line_color= q_color_list_name(ocelot_grid_cell_drag_line_color);
  if (ocelot_grid_border_size.left(1) == "#") ocelot_grid_border_size= q_color_list_name(ocelot_grid_border_size);
  if (ocelot_grid_cell_border_size.left(1) == "#") ocelot_grid_cell_border_size= q_color_list_name(ocelot_grid_cell_border_size);
  if (ocelot_grid_cell_drag_line_size.left(1) == "#") ocelot_grid_cell_drag_line_size= q_color_list_name(ocelot_grid_cell_drag_line_size);
  if (ocelot_history_text_color.left(1) == "#") ocelot_history_text_color= q_color_list_name(ocelot_history_text_color);
  if (ocelot_history_background_color.left(1) == "#") ocelot_history_background_color= q_color_list_name(ocelot_history_background_color);
  if (ocelot_history_border_color.left(1) == "#") ocelot_history_border_color= q_color_list_name(ocelot_history_border_color);
  if (ocelot_menu_text_color.left(1) == "#") ocelot_menu_text_color= q_color_list_name(ocelot_menu_text_color);
  if (ocelot_menu_background_color.left(1) == "#") ocelot_menu_background_color= q_color_list_name(ocelot_menu_background_color);
  if (ocelot_menu_border_color.left(1) == "#") ocelot_menu_border_color= q_color_list_name(ocelot_menu_border_color);
  if (ocelot_extra_rule_1_text_color.left(1) == "#") ocelot_extra_rule_1_text_color= q_color_list_name(ocelot_extra_rule_1_text_color);
  if (ocelot_extra_rule_1_background_color.left(1) == "#") ocelot_extra_rule_1_background_color= q_color_list_name(ocelot_extra_rule_1_background_color);
}


/*
  Todo: something has to be figured out about border width and border colors.
  I think, uncertainly, that the default border width is 0 and the color is Palette:shadow i.e. black.
  Possibly I could get the original value from QWidget::styleSheet() is some startup parameter changed it?
  Possibly I'd need to get a QFrame with a lineWidth() and frameWidth()?
  Possibly I should just assume that border width = 0px except for main window, and
  take out any mention of border in the Settings dialogs.
*/
void MainWindow::set_current_colors_and_font(QFont fixed_font)
{
  QFont font;

  QWidget *widget= new QWidget(); /* A dummy to which Qt will assign default settings for palette() etc. */
  ocelot_statement_text_color= statement_edit_widget->palette().color(QPalette::WindowText).name(); /* = QPalette::Foreground */
  ocelot_statement_background_color= statement_edit_widget->palette().color(QPalette::Window).name(); /* = QPalette::Background */
  font= statement_edit_widget->font();
  ocelot_statement_font_family= font.family();
  if (font.italic()) ocelot_statement_font_style= "italic"; else ocelot_statement_font_style= "normal";
  ocelot_statement_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
  ocelot_statement_font_weight= canonical_font_weight(QString::number(font.weight()));

  ocelot_grid_text_color= widget->palette().color(QPalette::WindowText).name(); /* = QPalette::Foreground */
  ocelot_grid_background_color= widget->palette().color(QPalette::Window).name(); /* = QPalette::Background */
  font= fixed_font;
  ocelot_grid_font_family= font.family();
  if (font.italic()) ocelot_grid_font_style= "italic"; else ocelot_grid_font_style= "normal";
  ocelot_grid_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
  ocelot_grid_font_weight= canonical_font_weight(QString::number(font.weight()));

  ocelot_history_text_color= history_edit_widget->palette().color(QPalette::WindowText).name(); /* = QPalette::Foreground */
  ocelot_history_background_color=history_edit_widget->palette().color(QPalette::Window).name(); /* = QPalette::Background */
  font= history_edit_widget->font();
  ocelot_history_font_family= font.family();
  if (font.italic()) ocelot_history_font_style= "italic"; else ocelot_history_font_style= "normal";
  ocelot_history_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
  ocelot_history_font_weight= canonical_font_weight(QString::number(font.weight()));
  ocelot_history_max_row_count= "0";

  ocelot_menu_text_color= ui->menuBar->palette().color(QPalette::WindowText).name(); /* = QPalette::Foreground */
  ocelot_menu_background_color= ui->menuBar->palette().color(QPalette::Window).name(); /* = QPalette::Background */
  font= ui->menuBar->font();
  ocelot_menu_font_family= font.family();
  if (font.italic()) ocelot_menu_font_style= "italic"; else ocelot_menu_font_style= "normal";
  ocelot_menu_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
  ocelot_menu_font_weight= canonical_font_weight(QString::number(font.weight()));

  ocelot_extra_rule_1_text_color= widget->palette().color(QPalette::WindowText).name(); /* = QPalette::Foreground */
  ocelot_extra_rule_1_background_color= widget->palette().color(QPalette::Window).name(); /* = QPalette::Background */
  ocelot_extra_rule_1_condition= "data_type LIKE '%BLOB'";
  ocelot_extra_rule_1_display_as= "char";

  delete widget;
}

/*
  Call this for the initial default font, which we want as fixed pitch.
  It can be overridden by command-line options, SET statements, etc.
  Usually by default Qt will make a widget with a non-fixed font,
  this might be controlled by qtconfig-qt4 if Qt4,
  this might be controlled by (say) Gnome|KDE settings if Qt5.
  Do not use
    #if QT_VERSION >= 0x50200
    QFontDatabase::systemFont(QFontDatabase::FixedFont)
    #endif
    because Qt5 distribution is for "Qt5", not "Qt5.2+",
    but we can find the default by making a dummy with "NoSuchFont".
  If default is fixed: good, return it.
  Else search QFontDatabase for the font with the most points
  Else return the default.
  Points:
    If (first word match) +3 e.g. if default="Ubuntu" "Ubuntu Mono" wins
    If (dummy default match) +2 e.g. "DejaVu Sans Mono" often wins
    If (exact size match) +1
    If (close size match) +1
    If (weight match) +1
    If (italic match) +1
    If not ("*Webdings*" or "*Wingdings*" or "*Dingbats*") +2
  After you get it, it should determine the initial style sheets.
  Windows generates warnings if I don't exclude some fonts,
  but I suppress some of the warnings.
  Todo: consider changing QApplication font rather than individual fonts.
  Todo: there's a memory leak, though unimportant (we only call once)
*/
QFont MainWindow::get_fixed_font()
{
  QWidget *w1= new QWidget();
  QFont f1= w1->font();
  QFontInfo fi(f1);
  if (fi.fixedPitch()) return f1;
  int point_size= fi.pointSize();
  int weight= fi.weight();
  bool italic= fi.italic();
  QString first_word= fi.family();
  if (first_word.indexOf(" ") != -1)
    first_word= first_word.left(first_word.indexOf(" "));
  first_word= first_word.toUpper();
  int first_word_length= first_word.length();
  QFontDatabase database;
  QPlainTextEdit plain;
  QFont fo3("NoSuchFont", point_size, weight, italic);
  fo3.setStyleHint(QFont::TypeWriter);
  plain.setFont(fo3);
  QString recommended_family= plain.fontInfo().family();
  QString winner_font_family= "";
  int winner_font_point_size= 0;
  int winner_font_weight= 0;
  bool winner_font_italic= 0;
  int winner_font_points= 0;
  foreach (const QString &family, database.families())
  {
#ifdef OCELOT_OS_NONLINUX
    {
      QString f= family.toUpper();
      if ((family == "FIXEDSYS") || (family == "MODERN")
       || (family == "SANS SERIF") || (family == "MS SERIF")
       || (family == "ROMAN") || (family == "SCRIPT")
       || (family == "SMALL FONTS") || (family == "SYSTEM")
       || (family == "TERMINAL"))
         continue;
    }
#endif
    int this_points= 0;
    QFont fo1= QFont(family, point_size, weight, italic);
    fo1.setStyleHint(QFont::TypeWriter);
    plain.setFont(fo1);
    if ((plain.fontInfo().fixedPitch())
     && (plain.fontInfo().pointSize() > 5))
    {
      if ((plain.fontInfo().family().toUpper().contains("WINGDING"))
       || (plain.fontInfo().family().toUpper().contains("WEBDING"))
       || (plain.fontInfo().family().toUpper().contains("DINGBAT")))
       ;
      else this_points+= 2;
      if (plain.fontInfo().family() == recommended_family)
        this_points+= 2;
      if (plain.fontInfo().family().left(first_word_length).toUpper() == first_word)
        this_points+= 3;
      if (plain.fontInfo().pointSize() == point_size)
        this_points+= 1;
      if ((plain.fontInfo().pointSize() >= (point_size * 0.9))
       && (plain.fontInfo().pointSize() <= (point_size * 1.2)))
        this_points+= 1;
      if (plain.fontInfo().weight() == weight)
        this_points+= 1;
      if (plain.fontInfo().italic() == italic)
        this_points+= 1;
      if (this_points > winner_font_points)
      {
        winner_font_family= plain.fontInfo().family();
        winner_font_point_size= plain.fontInfo().pointSize();
        winner_font_weight= plain.fontInfo().weight();
        winner_font_italic= plain.fontInfo().italic();
        winner_font_points= this_points;
      }
    }
  }
  if (winner_font_points == 0) return f1;
  QFont winner_font= QFont(winner_font_family,
                           winner_font_point_size,
                           winner_font_weight,
                           winner_font_italic);
  return winner_font;
}

/*
  Given RGB, return color name for current language
  If current language is not English, this may not be the
  canonical color name.
  It might be a bug if we don't find the rgb in s_color_list,
  but there a few extra RGBs in q_color_list.
  Todo: I think there's redundancy somewhere, that is,
        some other procedure does the same thing.
*/
/*
  Test: change q_i = 0 to q_i= color_off
        change q_i + color_off to q_i
*/
QString MainWindow::rgb_to_color(QString rgb)
{
  char rgb_as_utf8[128]; /* actually max rgb len = 7, allow for error */
  strcpy(rgb_as_utf8, rgb.toUpper().toUtf8());
  for (int q_i= color_off; strcmp(s_color_list[q_i]," ") > 0; q_i+= 2)
  {
    if (strcmp(s_color_list[q_i + 1], rgb_as_utf8) == 0)
    {
      QString color_name= s_color_list[q_i];
      return color_name;
    }
  }
  return rgb;
}


/*
  Pass: a string which is supposed to have a color name.
  Return: a canonical color name, i.e. an RGB value.
  1. Check color_list (names). If match, return RGB. Might be a case change e.g. Gray not gray.
  2. Accept some color name variants, e.g. Gray not grey, return RGB.
     They appear in http://www.w3.org/TR/SVG/types.html#ColorKeywords.
  3. Put the color in a QColor. If result is invalid, return "" which means invalid.
  4. Get the color back as #RRGGBB
  5. Return the #RRGGBB color.
  This does not mean that absolutely no synonyms are allowed
  -- two names may have the same #RRGGBB, as with Fuchsia|Magenta.
*/
QString MainWindow::canonical_color_name(QString color_name_string)
{
  QString s;
  QString co;

  co= color_name_string;

  if (co.left(1) != "#")
  {
    /*
      Todo: This appears to work for setting colors that contain ''
      but what if I SET ... = "...''..." elsewhere?
      Also: I think co.replace() does the job, I don't need to assign.
    */
    if (co.contains("''")) co= co.replace("''", "'");

    /* Search #1: in the color list for the current language offset. */
    for (int i= color_off; strcmp(s_color_list[i], "") > 0; i+= 2)
    {
      s= s_color_list[i];
      if (QString::compare(co, s, Qt::CaseInsensitive) == 0)
      {
        s= s_color_list[i + 1];                /* Return the RGB */
        return s;
      }
    }

    /* Search #2: Fixed list of variants equivalent to some W3C words */
    if (QString::compare(color_name_string, "Cornflower", Qt::CaseInsensitive) == 0) return s_color_list[COLOR_CORNFLOWERBLUE*2 + 1];
    if (QString::compare(color_name_string, "Darkgrey", Qt::CaseInsensitive) == 0) return s_color_list[COLOR_DARKGRAY*2 + 1];
    if (QString::compare(color_name_string, "DarkSlateGrey", Qt::CaseInsensitive) == 0) return s_color_list[COLOR_DARKSLATEGRAY*2 + 1];
    if (QString::compare(color_name_string, "DimGrey", Qt::CaseInsensitive) == 0) return s_color_list[COLOR_DIMGRAY*2 + 1];
    if (QString::compare(color_name_string, "Grey", Qt::CaseInsensitive) == 0) return s_color_list[COLOR_GRAY*2 + 1];
    if (QString::compare(color_name_string, "LightGoldenrod", Qt::CaseInsensitive) == 0) return s_color_list[COLOR_LIGHTGOLDENRODYELLOW*2 + 1];
    if (QString::compare(color_name_string, "LightGrey", Qt::CaseInsensitive) == 0) return s_color_list[COLOR_LIGHTGRAY*2 + 1];
    if (QString::compare(color_name_string, "LightSlateGrey", Qt::CaseInsensitive) == 0) return s_color_list[COLOR_LIGHTSLATEGRAY*2 + 1];
    if (QString::compare(color_name_string, "NavyBlue", Qt::CaseInsensitive) == 0) return s_color_list[COLOR_NAVY*2 + 1];
    if (QString::compare(color_name_string, "SlateGrey", Qt::CaseInsensitive) == 0) return s_color_list[COLOR_SLATEGRAY*2 + 1];

    /* Search #3: in the color list for all languages */
    {
      int lc;
      for (lc= 0; strcmp(string_languages[lc], "") != 0; ++lc) ;
      for (int j= 0; j < lc; ++j)
      {
        for (int i= j * COLOR_END; strcmp(s_color_list[i], "") > 0; i+= 2)
        {
          s= s_color_list[i];
          if (QString::compare(co, s, Qt::CaseInsensitive) == 0)
          {
            s= s_color_list[i + 1];                /* Return the RGB */
            return s;
          }
        }
      }
    }
  }

  /*
    It's not in any of our color lists, but it might be a name that Qt
    will accept anyway (e.g. in some version that I'm unaware of),
    or it was passed with an initial # so it's some variant of #RGB.
    But make sure it's valid, and convert to the canonical: #RRGGBB.
    NB: returning "" means error but not every caller checks for that.
  */

  QColor qq_color;
  qq_color.setNamedColor(co);
  if (qq_color.isValid() == false) return "";                 /* bad color, maybe bad format */
  QString qq_color_name= qq_color.name();                      /* returns name as "#RRGGBB" */

  //for (int i= color_off + 1; strcmp(s_color_list[i], "") > 0; i+= 2)
  //{
  //  s= s_color_list[i];
  //  if (QString::compare(qq_color_name, s, Qt::CaseInsensitive) == 0)
  //  {
  //    s= s_color_list[i - 1];
  //    return s;
  //  }
  //}
  return qq_color_name;
}


/* Called from: make_style_strings(). We allow sixteen color names that Qt doesn't like. */
QString MainWindow::qt_color(QString color_name)
{
  if (QString::compare(color_name, "Aqua", Qt::CaseInsensitive) == 0) return "#00FFFF"; /* = Cyan */
  if (QString::compare(color_name, "Fuchsia", Qt::CaseInsensitive) == 0) return "#FF00FF"; /* = Magenta */
  if (QString::compare(color_name, "GrayX11", Qt::CaseInsensitive) == 0) return "#BEBEBE";
  if (QString::compare(color_name, "GreenX11", Qt::CaseInsensitive) == 0) return "#00FF00";
  if (QString::compare(color_name, "Indigo", Qt::CaseInsensitive) == 0) return "#4B0082";
  if (QString::compare(color_name, "Lime", Qt::CaseInsensitive) == 0) return "#00FF00"; /* = Green */
  if (QString::compare(color_name, "MaroonX11", Qt::CaseInsensitive) == 0) return "#B03060";
  if (QString::compare(color_name, "Olive", Qt::CaseInsensitive) == 0) return "#808000";
  if (QString::compare(color_name, "PurpleX11", Qt::CaseInsensitive) == 0) return "#A020F0";
  if (QString::compare(color_name, "RebeccaPurple", Qt::CaseInsensitive) == 0) return "#663399";
  if (QString::compare(color_name, "Silver", Qt::CaseInsensitive) == 0) return "#C0C0C0";
  if (QString::compare(color_name, "Teal", Qt::CaseInsensitive) == 0) return "#008080";
  if (QString::compare(color_name, "WebGray", Qt::CaseInsensitive) == 0) return "#808080";
  if (QString::compare(color_name, "WebGreen", Qt::CaseInsensitive) == 0) return "#008000";
  if (QString::compare(color_name, "WebMaroon", Qt::CaseInsensitive) == 0) return "#7F0000";
  if (QString::compare(color_name, "WebPurple", Qt::CaseInsensitive) == 0) return "#7F007F";
  return color_name;
}


/*
  Pass: a string which is supposed to have a font weight. Return: a canonical font weight.
  If it's not an expected value, return "" which means invalid.
  The string can be light|normal|demibold|bold|black, or a number which we'll try to interpret.
  Todo: check whether setStyleSheet accepts numbers too.
  I think the font dialog box returns lower case, so that's what we regard as canonical.
*/
QString MainWindow::canonical_font_weight(QString font_weight_string)
{
  if (QString::compare(font_weight_string, "Light", Qt::CaseInsensitive) == 0) return "light";
  if (QString::compare(font_weight_string, "Normal", Qt::CaseInsensitive) == 0) return "normal";
  if (QString::compare(font_weight_string, "DemiBold", Qt::CaseInsensitive) == 0) return "demibold";
  if (QString::compare(font_weight_string, "Bold", Qt::CaseInsensitive) == 0) return "bold";
  if (QString::compare(font_weight_string, "Black", Qt::CaseInsensitive) == 0) return "black";
  /* Todo: Allow these new values if Qt version = 5.6 */
  //if (QString::compare(font_weight_string, "Thin", Qt::CaseInsensitive) == 0) return "thin";
  //if (QString::compare(font_weight_string, "ExtraLight", Qt::CaseInsensitive) == 0) return "extralight";
  //if (QString::compare(font_weight_string, "Medium", Qt::CaseInsensitive) == 0) return "medium";
  //if (QString::compare(font_weight_string, "ExtraBold", Qt::CaseInsensitive) == 0) return "extrabold";
  bool ok;
  int font_weight_as_int= font_weight_string.toInt(&ok);
  if ((ok == false) || (font_weight_as_int < 0) || (font_weight_as_int > 100)) return "";
  if (font_weight_as_int <= QFont::Light) return "light";
  if (font_weight_as_int <= QFont::Normal) return "normal";
  if (font_weight_as_int <= QFont::DemiBold) return "demibold";
  if (font_weight_as_int <= QFont::Bold) return "bold";
  return "black";
}


/*
  Pass: a string which is supposed to have a font style. Return: a canonical font style.
  If it's not an expected value, return "" which means invalid.
*/
QString MainWindow::canonical_font_style(QString font_style_string)
{
  if (QString::compare(font_style_string, "normal", Qt::CaseInsensitive) == 0) return "normal";
  if (QString::compare(font_style_string, "italic", Qt::CaseInsensitive) == 0) return "italic";
  return "";
}


/* Called from: action_statement() etc. Make a string that setStyleSheet() can use. */
/*
  Todo: I wasn't able to figure out a simple way to emphasize widget title, for example make it bold.
  Todo: Notice hardcoded "gray" for QMenu::item:disabled? We could have
        more selectable options for that, and for other QMenu::item
        pseudo- stuff, and QMenu::separator.
        E.g. QMenu:item:selected if you can decide on a colour.
  TODO: Also for QTextedit we should have ...:selected because right now
        when you select and then change focus, you lose the colour.
*/
void MainWindow::make_style_strings()
{
  ocelot_statement_style_string= "color:"; ocelot_statement_style_string.append(qt_color(ocelot_statement_text_color));
  ocelot_statement_style_string.append(";background-color:"); ocelot_statement_style_string.append(qt_color(ocelot_statement_background_color));
  ocelot_statement_style_string.append(";border:1px solid "); ocelot_statement_style_string.append(qt_color(ocelot_statement_border_color));
  ocelot_statement_style_string.append(";font-family:"); ocelot_statement_style_string.append(ocelot_statement_font_family);
  ocelot_statement_style_string.append(";font-size:"); ocelot_statement_style_string.append(ocelot_statement_font_size);
  ocelot_statement_style_string.append("pt;font-style:"); ocelot_statement_style_string.append(ocelot_statement_font_style);
  ocelot_statement_style_string.append(";font-weight:"); ocelot_statement_style_string.append(ocelot_statement_font_weight);

  ocelot_grid_style_string= "color:"; ocelot_grid_style_string.append(qt_color(ocelot_grid_text_color));
  ocelot_grid_style_string.append(";background-color:"); ocelot_grid_style_string.append(qt_color(ocelot_grid_background_color));
  ocelot_grid_style_string.append(";border:");
  ocelot_grid_style_string.append(ocelot_grid_cell_border_size);
  ocelot_grid_style_string.append("px solid ");
  ocelot_grid_style_string.append(qt_color(ocelot_grid_border_color));
  ocelot_grid_style_string.append(";font-family:"); ocelot_grid_style_string.append(ocelot_grid_font_family);
  ocelot_grid_style_string.append(";font-size:"); ocelot_grid_style_string.append(ocelot_grid_font_size);
  ocelot_grid_style_string.append("pt;font-style:"); ocelot_grid_style_string.append(ocelot_grid_font_style);
  ocelot_grid_style_string.append(";font-weight:"); ocelot_grid_style_string.append(ocelot_grid_font_weight);

  ocelot_grid_header_style_string= "color:"; ocelot_grid_header_style_string.append(qt_color(ocelot_grid_text_color));
  ocelot_grid_header_style_string.append(";background-color:"); ocelot_grid_header_style_string.append(qt_color(ocelot_grid_header_background_color));
  ocelot_grid_header_style_string.append(";border:");
  ocelot_grid_header_style_string.append(ocelot_grid_cell_border_size);
  ocelot_grid_header_style_string.append("px solid ");
  ocelot_grid_header_style_string.append(qt_color(ocelot_grid_border_color));
  ocelot_grid_header_style_string.append(";font-family:"); ocelot_grid_header_style_string.append(ocelot_grid_font_family);
  ocelot_grid_header_style_string.append(";font-size:"); ocelot_grid_header_style_string.append(ocelot_grid_font_size);
  ocelot_grid_header_style_string.append("pt;font-style:"); ocelot_grid_header_style_string.append(ocelot_grid_font_style);
  ocelot_grid_header_style_string.append(";font-weight:"); ocelot_grid_header_style_string.append(ocelot_grid_font_weight);

  ocelot_history_style_string= "color:"; ocelot_history_style_string.append(qt_color(ocelot_history_text_color));
  ocelot_history_style_string.append(";background-color:"); ocelot_history_style_string.append(qt_color(ocelot_history_background_color));
  ocelot_history_style_string.append(";border:1px solid "); ocelot_history_style_string.append(qt_color(ocelot_history_border_color));
  ocelot_history_style_string.append(";font-family:"); ocelot_history_style_string.append(ocelot_history_font_family);
  ocelot_history_style_string.append(";font-size:"); ocelot_history_style_string.append(ocelot_history_font_size);
  ocelot_history_style_string.append("pt;font-style:"); ocelot_history_style_string.append(ocelot_history_font_style);
  ocelot_history_style_string.append(";font-weight:"); ocelot_history_style_string.append(ocelot_history_font_weight);

  ocelot_menu_style_string="* {";
  ocelot_menu_style_string.append("color:");
  ocelot_menu_style_string.append(qt_color(ocelot_menu_text_color));
  ocelot_menu_style_string.append(";background-color:"); ocelot_menu_style_string.append(qt_color(ocelot_menu_background_color));
  ocelot_menu_style_string.append(";border:1px solid "); ocelot_menu_style_string.append(qt_color(ocelot_menu_border_color));
  ocelot_menu_style_string.append(";font-family:"); ocelot_menu_style_string.append(ocelot_menu_font_family);
  ocelot_menu_style_string.append(";font-size:"); ocelot_menu_style_string.append(ocelot_menu_font_size);
  ocelot_menu_style_string.append("pt;font-style:"); ocelot_menu_style_string.append(ocelot_menu_font_style);
  ocelot_menu_style_string.append(";font-weight:"); ocelot_menu_style_string.append(ocelot_menu_font_weight);
  ocelot_menu_style_string.append("} ");
  ocelot_menu_style_string.append("QMenu::item:disabled {");
  ocelot_menu_style_string.append("background-color:gray");
  ocelot_menu_style_string.append("}");

  ocelot_extra_rule_1_style_string= "color:"; ocelot_extra_rule_1_style_string.append(qt_color(ocelot_extra_rule_1_text_color));
  ocelot_extra_rule_1_style_string.append(";background-color:"); ocelot_extra_rule_1_style_string.append(qt_color(ocelot_extra_rule_1_background_color));
  ocelot_extra_rule_1_style_string.append(";border:");
  ocelot_extra_rule_1_style_string.append(ocelot_grid_cell_border_size);
  ocelot_extra_rule_1_style_string.append("px solid ");
  ocelot_extra_rule_1_style_string.append(qt_color(ocelot_grid_border_color));
  ocelot_extra_rule_1_style_string.append(";font-family:"); ocelot_extra_rule_1_style_string.append(ocelot_grid_font_family);
  ocelot_extra_rule_1_style_string.append(";font-size:"); ocelot_extra_rule_1_style_string.append(ocelot_grid_font_size);
  ocelot_extra_rule_1_style_string.append("pt;font-style:"); ocelot_extra_rule_1_style_string.append(ocelot_grid_font_style);
  ocelot_extra_rule_1_style_string.append(";font-weight:"); ocelot_extra_rule_1_style_string.append(ocelot_grid_font_weight);
}


/*
  Use: ocelot_grid_style string. Return: max height of one char, and combined height of borders.
  Assume that ocelot_grid_style_string also has "...border:...px...".

  This is always a problem.
  Simply doing QFontMetrics() for the widget you're working on won't work till show() happens.
  Todo: probably the spacing could look a little tidier.
  Todo: right now we do nothing with width, we could add that later
        (for one Latin character, or for a passed string).
  Todo: Find out why one must magic numbers to prevent vertical scroll bars from appearing.
        It seems to depend on font size -- with bigger fonts I could add smaller numbers.
  Todo: This is working for Row_form_box, now get it working for the result grid.
  Beware: height() is internal height but setMinimumSize() may depend on external height.
*/
void MainWindow::component_size_calc(int *character_height, int *borders_height)
{
  {
    QFont font= get_font_from_style_sheet(ocelot_grid_style_string);
    QFontMetrics fm(font);
    *character_height= fm.lineSpacing();
    /* See also the  kludge in grid_column_size_calc */
  }
  {
    int border_size_start= ocelot_grid_style_string.indexOf("border:");
    int border_size_end= ocelot_grid_style_string.indexOf("px", border_size_start);
    QString border_size= ocelot_grid_style_string.mid(border_size_start + 7, border_size_end - (border_size_start + 7));
    border_size= border_size.trimmed(); /* probably unnecessary */
    bool ok;
    int border_size_as_int= border_size.toInt(&ok);
    assert(ok == true);
    assert(border_size_as_int >= 0);
    *borders_height= border_size_as_int * 2 + 9;
  }
}


/*
  Pass: style_string. Return: QFont.
  In practice we always pass ocelot_grid_style_string and all we care about is height.
  Assume that style string always has "font-family... font-size:...pt;...font-weight:...".
  style_string has points not pixels, because the dialog box for getting a font always uses points.
  Here we make a new QFont based on font-family, font-size, font-weight (NB: all are necessary);
  from this font we can calculate a widget's internal height in pixels.
  Alternative: we could create a widget, apply style_string to it, then ask what its font is,
  but that's probably a bit slower because we have to show() the widget first.
  Some font weights are Qt 5.6: https://doc-snapshots.qt.io/qt5-5.6/qfont.html#Weight-enum
*/
QFont MainWindow::get_font_from_style_sheet(QString style_string)
{
  QString font_family= "";
  int font_size_as_int= -1;
  bool font_style_as_bool= false;
  int font_weight_as_int= -1;
  {
    int font_family_start= style_string.indexOf("font-family:");
    int font_family_end= style_string.indexOf(";", font_family_start);
    font_family= style_string.mid(font_family_start + 12, font_family_end - (font_family_start + 12));
    font_family= font_family.trimmed();
    assert(font_family > "");
  }
  {
    int font_size_start= style_string.indexOf("font-size:");
    int font_size_end= style_string.indexOf("pt", font_size_start);
    QString font_size= style_string.mid(font_size_start + 10, font_size_end - (font_size_start + 10));
    font_size= font_size.trimmed();
    bool ok;
    font_size_as_int= font_size.toInt(&ok);
    assert(ok == true);
    assert(font_size_as_int >= 1);
  }
  {
    int font_style_start= style_string.indexOf("font-style:");
    int font_style_end= style_string.indexOf(";", font_style_start);
    QString font_style= style_string.mid(font_style_start + 11, font_style_end - (font_style_start + 11));
    font_style= font_style.trimmed();
    assert(font_style > "");
    if (QString::compare(font_style,"normal",Qt::CaseInsensitive) == 0) font_style_as_bool= false;
    if (QString::compare(font_style,"italic",Qt::CaseInsensitive) == 0) font_style_as_bool= true;
    if (QString::compare(font_style,"oblique",Qt::CaseInsensitive) == 0) font_style_as_bool= true;
  }
  {
    int font_weight_start= style_string.indexOf("font-weight:");
    int font_weight_end= style_string.indexOf(";", font_weight_start);
    if (font_weight_end < 0) font_weight_end= style_string.length();
    QString font_weight= style_string.mid(font_weight_start + 12, font_weight_end - (font_weight_start + 12));
    font_weight= font_weight.trimmed();
    if (QString::compare(font_weight,"Light",Qt::CaseInsensitive) == 0) font_weight_as_int= QFont::Light;
    if (QString::compare(font_weight,"Normal",Qt::CaseInsensitive) == 0) font_weight_as_int= QFont::Normal;
    if (QString::compare(font_weight,"DemiBold",Qt::CaseInsensitive) == 0) font_weight_as_int= QFont::DemiBold;
    if (QString::compare(font_weight,"Bold",Qt::CaseInsensitive) == 0) font_weight_as_int= QFont::Bold;
    if (QString::compare(font_weight,"Black",Qt::CaseInsensitive) == 0) font_weight_as_int= QFont::Black;
    //if (QString::compare(font_weight,"Thin",Qt::CaseInsensitive) == 0) font_weight_as_int= 0;
    //if (QString::compare(font_weight,"ExtraLight",Qt::CaseInsensitive) == 0) font_weight_as_int= 12;
    //if (QString::compare(font_weight,"Medium",Qt::CaseInsensitive) == 0) font_weight_as_int= 57;
    //if (QString::compare(font_weight,"ExtraBold",Qt::CaseInsensitive) == 0) font_weight_as_int= 81;
    assert(font_weight_as_int >= 0);
  }
  QFont font(font_family, font_size_as_int, font_weight_as_int, font_style_as_bool);
  return font;
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
  Re: Don't bother with begin_count if PROCEDURE or FUNCTION or TRIGGER
      or EVENT hasn't been seen. But sometimes BEGIN or DECLARE can be
      starts of compound statements.
  Re: package_executable_section_begin_seen. CREATE PACKAGE BODY can end
      with END, or with BEGIN END. In the latter case we have too many
      BEGINs because we count CREATE PACKAGE as equivalent to BEGIN.
      So anything associated with this variable is an attempt to avoid
      the miscount.
  Beware: insert into t8 values (5); prompt w (unlike mysql client we wait for ';' here)
  Beware: create procedure p () begin end// select 5//
  Beware: input might be a file dump, and statements might be long.
  Todo: think about delimiter. Maybe delimiters don't count if you're in a delimiter statement?
  Todo: this doesn't allow for the possibility of "END comment WHILE|LOOP|REPEAT;"
  Todo: although label:begin declare v1 int; end; is illegal, it's confusing to miss the begin.
        we end up splitting the "end;" into a separate statement.
*/
int MainWindow::get_next_statement_in_string(int passed_main_token_number,
                                             int *returned_begin_count,
                                             bool check_if_client)
{
  int i;
  int begin_count= 0;
  QString last_token, second_last_token;
  QString text;
  int i_of_first_non_comment_seen= -1;
  bool begin_seen= false;
  text= statement_edit_widget->toPlainText(); /* Todo: decide whether I'm doing this too often */
  /*
    First, check for client statement, because the rules for client statement end are:
    ; OR delimiter OR \n OR \n\r
    but a DELIMITER statement ends with first whitespace after its argument
    or next client statement? or next statement of any kind?
  */
  bool client_statement_seen= false;
  bool create_package_seen= false;
  bool package_executable_section_begin_seen= false;
  for (i= passed_main_token_number; main_token_lengths[i] != 0; ++i)
  {
    int token= main_token_types[i];
    if ((token == TOKEN_TYPE_COMMENT_WITH_SLASH)
     || (token == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
     || (token == TOKEN_TYPE_COMMENT_WITH_MINUS))
      continue;
    if (is_client_statement(token, i, text) == true) client_statement_seen= true;
    break;
  }
  if ((client_statement_seen == true) && (check_if_client == true))
  {
    for (i= i; main_token_lengths[i] != 0; ++i)
    {
      if (main_token_lengths[i + 1] != 0)
      {
        int j= main_token_offsets[i] + main_token_lengths[i];
        bool line_break_seen= false;
        while (j < main_token_offsets[i + 1])
        {
          if (text.mid(j, 1) == "\n")
          {
            line_break_seen= true;
            break;
          }
          ++j;
        }
        if (line_break_seen == true)
        {
          ++i;
          break;
        }
      }
      int token= main_token_types[i];
      if ((token == TOKEN_TYPE_COMMENT_WITH_SLASH)
       || (token == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
       || (token == TOKEN_TYPE_COMMENT_WITH_MINUS))
      {
        continue;
      }
      last_token= text.mid(main_token_offsets[i], main_token_lengths[i]);
      if (QString::compare(ocelot_delimiter_str, ";") != 0)
      {
        if (QString::compare(last_token, ocelot_delimiter_str) == 0)
        {
          ++i; break;
        }
      }
      if (QString::compare(last_token, ";") == 0)
      {
        ++i; break;
      }
    }
  }
  else
  {
    bool is_maybe_in_compound_statement= false;
    bool is_create_trigger= false;
    int statement_type= -1;
    if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
    {
      QString word0= "", word1= "", word2= "";
      int word_number= 0;
      for (i= passed_main_token_number; main_token_lengths[i] != 0; ++i)
      {
        if ((main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_SLASH)
         || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
         || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_MINUS))
          continue;
        if (word_number == 0) word0= text.mid(main_token_offsets[i], main_token_lengths[i]);
        if (word_number == 1) word1= text.mid(main_token_offsets[i], main_token_lengths[i]);
        if (word_number == 2)
        {
          word2= text.mid(main_token_offsets[i], main_token_lengths[i]);
          break;
        }
        ++word_number;
      }
      statement_type= get_statement_type_low(word0, word1, word2);
    }
    last_token= "";
    for (i= passed_main_token_number; main_token_lengths[i] != 0; ++i)
    {
      if ((hparse_dbms_mask & FLAG_VERSION_PLSQL) != 0)
      {
        if (main_token_types[i] == TOKEN_KEYWORD_DECLARE)
        {
          is_maybe_in_compound_statement= true;
        }
        if (main_token_types[i] == TOKEN_KEYWORD_BEGIN)
        {
          begin_seen= true;
        }
      }

      second_last_token= last_token;
      last_token= text.mid(main_token_offsets[i], main_token_lengths[i]);
      if (QString::compare(ocelot_delimiter_str, ";") != 0)
      {
        if (QString::compare(last_token, ocelot_delimiter_str) == 0)
        {
          ++i; break;
        }
      }
      if ((QString::compare(last_token, "G", Qt::CaseInsensitive) == 0)
       && (QString::compare(second_last_token, "\\", Qt::CaseInsensitive) == 0))
      {
        ++i; break;
      }
      if ((QString::compare(last_token, ";") == 0) && (begin_count == 0))
      {
        if (((dbms_version_mask & FLAG_VERSION_PLSQL) != 0)
         && (is_maybe_in_compound_statement == true)
         && (begin_seen == false))
        {
          ; /* pl/sql: e.g. skip ; if it's between declare and begin */
        }
        else
        {
          ++i; break;
        }
      }
      if (i_of_first_non_comment_seen != -1)
      {
        if (main_token_types[i_of_first_non_comment_seen] == TOKEN_KEYWORD_CREATE)
        {
          if ((main_token_types[i] == TOKEN_KEYWORD_PROCEDURE)
          ||  (main_token_types[i] == TOKEN_KEYWORD_FUNCTION)
          ||  (main_token_types[i] == TOKEN_KEYWORD_TRIGGER)
          ||  (main_token_types[i] == TOKEN_KEYWORD_EVENT))
          {
            is_maybe_in_compound_statement= true;
            is_create_trigger= true;
          }
          if (main_token_types[i] == TOKEN_KEYWORD_PACKAGE) /* new */
          { /* new */
            is_maybe_in_compound_statement= true; /* new */
            ++begin_count;       /* new */
            begin_seen= true; /* new */
            create_package_seen= true;
          } /* new */
        }
      }
      if (i_of_first_non_comment_seen == -1)
      {
        if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
        {
          if (statement_type == TOKEN_KEYWORD_DO_LUA)
          {
            is_maybe_in_compound_statement= true;
          }
        }
        else
        {
          /* Following should only occur if we're sure it's MariaDB */
          if ((main_token_types[i] == TOKEN_KEYWORD_BEGIN)
          ||  (main_token_types[i] == TOKEN_KEYWORD_CASE)
          ||  (main_token_types[i] == TOKEN_KEYWORD_FOR_IN_FOR_STATEMENT)
          ||  (main_token_types[i] == TOKEN_KEYWORD_IF)
          ||  (main_token_types[i] == TOKEN_KEYWORD_LOOP)
          ||  (main_token_types[i] == TOKEN_KEYWORD_REPEAT)
          ||  (main_token_types[i] == TOKEN_KEYWORD_WHILE))
          {
            is_maybe_in_compound_statement= true;
          }
        }
        if ((main_token_types[i] != TOKEN_TYPE_COMMENT_WITH_SLASH)
         && (main_token_types[i] != TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
         && (main_token_types[i] != TOKEN_TYPE_COMMENT_WITH_MINUS))
        {
          i_of_first_non_comment_seen= i;
        }
      }
      /* For some reason the following was checking TOKEN_KEYWORD_ELSEIF too. Removed. */
      if (is_maybe_in_compound_statement == true)
      {
        if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
        {
          if (statement_type == TOKEN_KEYWORD_DO_LUA)
          {
            if ((main_token_types[i] == TOKEN_KEYWORD_DO_LUA)
            ||  (main_token_types[i] == TOKEN_KEYWORD_IF)
            ||  (main_token_types[i] == TOKEN_KEYWORD_FUNCTION)
            ||  (main_token_types[i] == TOKEN_KEYWORD_REPEAT))
            {
              ++begin_count;
            }
            if ((main_token_types[i] == TOKEN_KEYWORD_END)
            ||  (main_token_types[i] == TOKEN_KEYWORD_UNTIL))
            {
              --begin_count;
            }
          }
          else if (is_create_trigger == true)
          {
            if (main_token_types[i] == TOKEN_KEYWORD_BEGIN)
            {
              ++begin_count;
            }
            if (main_token_types[i] == TOKEN_KEYWORD_END)
            {
              --begin_count;
            }
          }
        }
        else
        {
          if ((main_token_types[i] == TOKEN_KEYWORD_BEGIN)
          ||  ((main_token_types[i] == TOKEN_KEYWORD_CASE)   && ((i == i_of_first_non_comment_seen) || (main_token_types[i - 1] != TOKEN_KEYWORD_END)))
          ||  ((main_token_types[i] == TOKEN_KEYWORD_FOR_IN_FOR_STATEMENT) && ((i == i_of_first_non_comment_seen) || (main_token_types[i - 1] != TOKEN_KEYWORD_END)))
          ||  ((main_token_types[i] == TOKEN_KEYWORD_IF)     && ((i == i_of_first_non_comment_seen) || (main_token_types[i - 1] != TOKEN_KEYWORD_END)))
          ||  ((main_token_types[i] == TOKEN_KEYWORD_LOOP)   && ((i == i_of_first_non_comment_seen) || (main_token_types[i - 1] != TOKEN_KEYWORD_END)))
          ||  ((main_token_types[i] == TOKEN_KEYWORD_REPEAT) && ((i == i_of_first_non_comment_seen) || (main_token_types[i - 1] != TOKEN_KEYWORD_END)))
          ||  ((main_token_types[i] == TOKEN_KEYWORD_WHILE)  && ((i == i_of_first_non_comment_seen) || (main_token_types[i - 1] != TOKEN_KEYWORD_END))))
          {
            if ((create_package_seen)
             && (main_token_types[i] == TOKEN_KEYWORD_BEGIN)
             && (begin_count == 1))
            {
              int j= next_i(i, -1);
              if ((main_token_types[j] != TOKEN_KEYWORD_AS)
               && (main_token_types[j] != TOKEN_KEYWORD_IS))
                package_executable_section_begin_seen= true;
            }
            ++begin_count;
          }
          if ((main_token_types[i] == TOKEN_KEYWORD_END)
           || (main_token_types[i] == TOKEN_KEYWORD_END_IN_CREATE_STATEMENT)) /* new */
          {
            --begin_count;
            if ((package_executable_section_begin_seen)
             && (begin_count == 1))
              --begin_count;
          }
        }
      }
    }
  }
  *returned_begin_count= begin_count;

  /* If delimiter follows ; then it's part of the statement, to be stripped later. */
  bool delimiter_seen= false;
  if (text.mid(main_token_offsets[i], main_token_lengths[i]) == ocelot_delimiter_str)
  {
    delimiter_seen= true;
  }

  /*
    If comment follows ; on the same line, then we'll take it as part of the statement.
    But if it comes after delimiter other than ; then we won't take it as part of the
    statement, because we have to strip delimiters before sending to the server,
    and for that we expect the delimiter token to be the last token in the statement.
    Todo: it might be better to not expect token to be the last token in the statement.
    Todo: there might be more than one comment.
  */
  bool comment_seen= false;
  if ((i > 0)
   && (main_token_lengths[i] != 0)
   && ((ocelot_delimiter_str == ";")
    || (text.mid(main_token_offsets[i - 1], main_token_lengths[i - 1]) != ocelot_delimiter_str))
   //&& ((i < 2)
   // || (text.mid(main_token_offsets[i - 1], main_token_lengths[i - 1]).toUpper() != "G")
   // || (text.mid(main_token_offsets[i - 2], main_token_lengths[i - 2]) != "\\"))
          )

  {
    if ((main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_SLASH)
     || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
     || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_MINUS))
    {
      comment_seen= true;
      int j= main_token_offsets[i - 1] + main_token_lengths[i - 1];
      while (j < main_token_offsets[i])
      {
        if (text.mid(j,1) == "\n")
        {
          comment_seen= false;
        }
        ++j;
      }
    }
  }
  if ((delimiter_seen == true) || (comment_seen == true)) ++i;
  return i - passed_main_token_number;
}


/*
  Make statement ready to send to server: strip comments and delimiter, output UTF8.
  We have an SQL statement that's ready to go to the server.
  We have a guarantee that the result i.e. "char *query" is big enough.
  Rremove comments if that's what the user requested with --skip-comments which is default.
  Remove final token if it's delimiter but not ';'.
  Ignore last token(s) if delimiter, \G, \g, or (sometimes) go, ego.
  Todo: there's a conversion to UTF8 but it should be to what server expects.
  Todo: um, in that case, make server expect UTF8.
  Re comment stripping: currently this is default because in mysql client it's default
    but there should be no assumption that this will always be the case,
    so some sort of warning might be good. The history display has to include
    the comments even if they're not sent to the server, so the caller of this
    routine will use both (original string,offset,length) and (returned string).
    Comments should be replaced with a single space.
    Do not strip comments that start with / * ! or / * M !
  Re Tarantool comment stripping: "#" does not begin a comment,
    and inline comments will be trouble if ocelot_comments <> 0
    because they're not legal in Lua (we don't try to solve that).
  Todo: Tarantool comments are different, eh?
  Todo: It's silly to call this every time you call real_query.
        Do it once, and you don't even need to call tokenize() again.
*/
int MainWindow::make_statement_ready_to_send(QString text, char *dbms_query, int dbms_query_len, int connection_number)
{
  log("make_statement_ready_to_send start", 80);
  int  *token_offsets;
  int  *token_lengths;
  int desired_count;
  unsigned int i;
  QString q;
  char *tmp;
  int tmp_len;
  //unsigned int token_count;
  desired_count= dbms_query_len + 1;
  token_offsets= new int[desired_count];
  token_lengths= new int[desired_count];
  token_lengths[0]= 0;
  tokenize(text.data(),
           text.size(),
           &token_lengths[0], &token_offsets[0], desired_count - 1,
          (QChar*)"33333", 1, ocelot_delimiter_str, 1);
  dbms_query[0]= '\0';
  for (i= 0; token_lengths[i] != 0; ++i)
  {
    if (ocelot_comments == 0)
    {
      /* TOKEN_TYPE_COMMENT_WITH_SLASH */
      /* or TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE */
      /* or TOKEN_TYPE_COMMENT_WITH_MINUS */
      if ((text.mid(token_offsets[i], 2) == "/*")
       || (text.mid(token_offsets[i], 2) == "--")
       || ((text.mid(token_offsets[i], 1) == "#")
           && (connections_dbms[connection_number] != DBMS_TARANTOOL)))
      {
        if ((text.mid(token_offsets[i], 3) != "/*!")
         && (text.mid(token_offsets[i], 4) != "/*M!"))
        {
          strcat(dbms_query," ");
          continue;
        }
      }
    }
    /* Don't send \G or \g */
    if ((token_lengths[i + 1] == 1)
     && (text.mid(token_offsets[i], 2).toUpper() == "\\G"))
    {
      ++i;
      continue;
    }
    /* Don't send delimiter unless it is ; */
    if ((ocelot_delimiter_str != ";")
     && (text.mid(token_offsets[i], ocelot_delimiter_str.length()) == ocelot_delimiter_str))
    {
      break;
    }
    /* Preserve whitespace after a token, unless this is the last token */
    int token_length;
    if (token_lengths[i + 1] == 0) token_length= token_lengths[i];
    else token_length= token_offsets[i + 1] - token_offsets[i];
    q= text.mid(token_offsets[i], token_length);
    tmp_len= q.toUtf8().size();           /* See comment "UTF8 Conversion" */
    tmp= new char[tmp_len + 1];
    memcpy(tmp, q.toUtf8().constData(), tmp_len);
    tmp[tmp_len]= 0;
    assert(strlen(dbms_query) + strlen(tmp) <= (unsigned int) dbms_query_len);
    strcat(dbms_query, tmp);
    delete [] tmp;
  }
  delete [] token_offsets;
  delete [] token_lengths;
  log("make_statement_ready_to_send end", 80);
  return (strlen(dbms_query));
}

/*
  select_1_row() is a convenience routine, made because frequently there are single-row selects.
  And if they are multiple-row selects, we can always do them one-at-a-time with limit and offset clauses.
  Pass: select statement. Return: unexpected_error, expected to be ""
  And up to 5 string variables from the first row that gets selected.
  This should be used only if you know that no fields are null.
*/
QString select_1_row_result_1, select_1_row_result_2, select_1_row_result_3, select_1_row_result_4, select_1_row_result_5;
QString MainWindow::select_1_row(const char *select_statement)
{
  MYSQL_RES *res= NULL;
  MYSQL_ROW row= NULL;
  const char *unexpected_error= NULL;
  QString s;
  unsigned int num_fields;

  if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], select_statement, strlen(select_statement)))
  {
    unexpected_error= "select failed";
  }

  if (unexpected_error == NULL)
  {
    res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
    if (res == NULL)
    {
      unexpected_error= "mysql_store_result failed";
    }
  }

  if (unexpected_error == NULL)
  {
    num_fields= lmysql->ldbms_mysql_num_fields(res);
  }

  if (unexpected_error == NULL)
  {
    row= lmysql->ldbms_mysql_fetch_row(res);
    if (row == NULL)
    {
      unexpected_error= er_strings[er_off + ER_MYSQL_FETCH_ROW_FAILED]; /* Beware! Look for a proc that compares routine with this string value! */
    }
    else
    {
      if (num_fields > 0) select_1_row_result_1= QString::fromUtf8(row[0]);
      if (num_fields > 1) select_1_row_result_2= QString::fromUtf8(row[1]);
      if (num_fields > 2) select_1_row_result_3= QString::fromUtf8(row[2]);
      if (num_fields > 3) select_1_row_result_4= QString::fromUtf8(row[3]);
      if (num_fields > 4) select_1_row_result_5= QString::fromUtf8(row[4]);
    }
  }

  if (res != NULL) lmysql->ldbms_mysql_free_result(res);

  if (unexpected_error != NULL) s= unexpected_error;
  else s= "";

  return s;
}

/* Variables used by kill thread, but which might be checked by debugger */
#define KILL_STATE_CONNECT_THREAD_STARTED 0
#define KILL_STATE_CONNECT_FAILED 1
#define KILL_STATE_IS_CONNECTED 2
#define KILL_STATE_MYSQL_REAL_QUERY_ERROR 3
#define KILL_STATE_ENDED 4
int volatile kill_state;
int volatile kill_connection_id;

#define LONG_QUERY_STATE_STARTED 0
#define LONG_QUERY_STATE_ENDED 1
char *dbms_query;
int dbms_query_len;
int dbms_query_connection_number;
volatile int dbms_long_query_result;
volatile int dbms_long_query_state= LONG_QUERY_STATE_ENDED;


#ifdef DEBUGGER

/*
  Debug

  Re debug_top_widget: It's a QWidget which contains a status line and debug_tab_widget.

  Re debug_tab_widget: It's a QTabWidget which may contain more than one debug_widget.

  Re debug_widget: it's derived from CodeEditor just as statement_edit_widget is, and it uses the
  same stylesheet as statement_edit_widget. It starts hidden, but debug|initialize shows it.
  It will contain the text of a procedure which is being debugged.
  Actually debug_widget[] occurs 10 times -- todo: there's no check for overflow.

  Re statements that come via statement_edit_widget: suppose the statement is "$DEB p8;".
  During tokens_to_keywords() we recognize that this is TOKEN_KEYWORD_DEBUG_DEBUG and highlight appropriately.
  During execute_client_statement(), after we check if statement_type == TOKEN_DEBUG_DEBUG_DEBUG and find that
  it's true, there's a call to action_debug_debug() followed by "return 1". When we're in action_debug_debug(),
  we have access to main_tokens so we can say things like "QString s= text.mid(main_token_offsets[i], main_token_lengths[i]);".
*/

/* This means "while debugger is going check status 20 times per second" (50 = 50 milliseconds). */
#define DEBUG_TIMER_INTERVAL 50

#define DEBUGGEE_STATE_0 0
#define DEBUGGEE_STATE_CONNECT_THREAD_STARTED 1
#define DEBUGGEE_STATE_BECOME_DEBUGEE_CONNECTION 2
#define DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP 3
#define DEBUGGEE_STATE_CONNECT_FAILED -1
#define DEBUGGEE_STATE_MYSQL_REAL_QUERY_ERROR -2
#define DEBUGGEE_STATE_MYSQL_STORE_RESULT_ERROR -3
#define DEBUGGEE_STATE_MYSQL_FETCH_ROW_ERROR -4
#define DEBUGGEE_STATE_BECOME_DEBUGGEE_CONNECTION_ERROR -5
#define DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR -6
#define DEBUGGEE_STATE_END -7

char debuggee_channel_name[128]= "";
int volatile debuggee_state= DEBUGGEE_STATE_0;
char debuggee_state_error[STRING_LENGTH_512]= "";
char debuggee_information_status_debugger_name[32];
char debuggee_information_status_debugger_version[8];
char debuggee_information_status_timestamp_of_status_message[32];
char debuggee_information_status_number_of_status_message[8];
char debuggee_information_status_icc_count[8];
char debuggee_information_status_schema_identifier[256];
char debuggee_information_status_routine_identifier[256];
char debuggee_information_status_line_number[8];
char debuggee_information_status_is_at_breakpoint[8];
char debuggee_information_status_is_at_tbreakpoint[8];
char debuggee_information_status_is_at_routine_exit[8];
char debuggee_information_status_stack_depth[8];
char debuggee_information_status_last_command[256];
char debuggee_information_status_last_command_result[256];
char debuggee_information_status_commands_count[8];
char debuggee_database[256];

QString debug_routine_schema_name[DEBUG_TAB_WIDGET_MAX + 1];
QString debug_routine_name[DEBUG_TAB_WIDGET_MAX + 1];

QString debug_q_schema_name_in_statement;
QString debug_q_routine_name_in_statement;
QString debug_statement;

QTimer *debug_timer;                                    /* For calling action_debug_timer_status() every 1/10 seconds */
int debug_timer_old_line_number;
char debug_timer_old_schema_identifier[256];
char debug_timer_old_routine_identifier[256];
int volatile debuggee_connection_id;                             /* Save this if you need to kill the debuggee (an unusual need) */
int debug_timer_old_commands_count;
int debug_timer_old_number_of_status_message;
int debug_timer_old_icc_count;
int debug_timer_old_debug_widget_index;



/*
  Enable or disable debug menu items.
  If debug succeeds: most items after debug are enabled. (Todo: eventually all should be enabled.)
  If exit succeeds: most items after debug are disabled.
*/
void MainWindow::debug_menu_enable_or_disable(int statement_type)
{
  bool e;

  if (statement_type == TOKEN_KEYWORD_BEGIN) e= false;
  if (statement_type == TOKEN_KEYWORD_DEBUG_DEBUG) e= true;
  if (statement_type == TOKEN_KEYWORD_DEBUG_EXIT) e= false;

//  menu_debug_action_install->setEnabled(e);
//  menu_debug_action_setup->setEnabled(e);
//  menu_debug_action_debug->setEnabled(e);
  menu_debug_action_breakpoint->setEnabled(e);
  menu_debug_action_continue->setEnabled(e);
//  menu_debug_action_leave->setEnabled(e);
  menu_debug_action_next->setEnabled(e);
//  menu_debug_action_skip->setEnabled(e);
  menu_debug_action_step->setEnabled(e);
  menu_debug_action_clear->setEnabled(e);
//  menu_debug_action_delete->setEnabled(e);
  menu_debug_action_exit->setEnabled(e);                                      /* everything after debug goes gray after this succeeds */
  menu_debug_action_information->setEnabled(e);
  menu_debug_action_refresh_server_variables->setEnabled(e);
  menu_debug_action_refresh_user_variables->setEnabled(e);
  menu_debug_action_refresh_variables->setEnabled(e);
  menu_debug_action_refresh_call_stack->setEnabled(e);
}


void MainWindow::create_widget_debug()
{
  /* We make debug_tab_widget now but we don't expect anyone to see it until debug|setup happens. */
  /* Todo: should you create with an initially-hidden flag? */
  debug_top_widget= new QWidget(this);              /* 2015-08-25 added "this" */
  debug_top_widget->hide();                         /* hidden until debug|initialize is executed */

  /*
    Todo: check: am I doing it right by saying parent = debug_top_widget
    i.e. will automatic delete happen when debug_top_widget is deleted?
    or will there be a memory leak?
    Hmm, why do I care? I don't delete debug_top_widget anyway, do I?
    Hmm Hmm, yes I do but it must be an error.
  */

  debug_line_widget= new QLineEdit(debug_top_widget);
  debug_line_widget->setText("Debugger Status = Inactive");
  debug_tab_widget= new QTabWidget(debug_top_widget);

  debug_top_widget_layout= new QVBoxLayout(debug_top_widget);
  debug_top_widget_layout->addWidget(debug_line_widget);
  debug_top_widget_layout->addWidget(debug_tab_widget);
  debug_top_widget->setLayout(debug_top_widget_layout);

  /* We make debug_timer now but we don't start it until debug status can be shown. */
  debug_timer= new QTimer(this);
  connect(debug_timer, SIGNAL(timeout()), this, SLOT(action_debug_timer_status()));

  /* This must be done before we set statement stylesheet. */
  for (int i= 0; i < DEBUG_TAB_WIDGET_MAX; ++i) debug_widget[i]= 0;

}


//#include <pthread.h>

/*
  The debuggee is a separate thread and makes its own connection using ocelot_ parameters just like the main connection.
  Problem: debuggee_thread() cannot be in MainWindow (at least, I didn't figure out how to make it in MainWindow).
  That explains why I made a lot of things global.
  Todo: Check: Why do you end statements with ";"?
  Todo: If an error occurs, copy the whole thing into a fixed-size char so debugger can see it.
        And notice that the result is that the thread ends, so beware of racing and worry about cleanup.
*/

void* debuggee_thread(void* unused)
{
  (void) unused; /* suppress "unused parameter" warning */
  char call_statement[512];
  int is_connected= 0;
  MYSQL_RES *debug_res= NULL;
  MYSQL_ROW debug_row= NULL;
  const char *unexpected_error= NULL;

  debuggee_connection_id= 0;

  memset(debuggee_state_error, 0, STRING_LENGTH_512);
  debuggee_state= DEBUGGEE_STATE_CONNECT_THREAD_STARTED;

  for (;;)
  {
    /*
      The debuggee connection.
      Todo: the_connect() could easily fail: parameters are changed, # of connections = max, password no longer works, etc.
            ... so you should try to pass back a better explanation if the_connect() fails here.
    */
    if (options_and_connect(MYSQL_DEBUGGER_CONNECTION, debuggee_database))
    {
       debuggee_state= DEBUGGEE_STATE_CONNECT_FAILED;
       strncpy(debuggee_state_error, lmysql->ldbms_mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
       break;
    }
    is_connected= 1;

    /* Get connection_id(). This is only in case regular "exit" fails and we have to issue a kill statement. */
    strcpy(call_statement, "select connection_id();");
    if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_DEBUGGER_CONNECTION], call_statement, strlen(call_statement)))
    {
      debuggee_state= DEBUGGEE_STATE_MYSQL_REAL_QUERY_ERROR;
      strncpy(debuggee_state_error, lmysql->ldbms_mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
      break;
    }
    debug_res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_DEBUGGER_CONNECTION]);
    if (debug_res == NULL)
    {
      debuggee_state= DEBUGGEE_STATE_MYSQL_STORE_RESULT_ERROR;
      strncpy(debuggee_state_error, lmysql->ldbms_mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
      break;
    }
    debug_row= lmysql->ldbms_mysql_fetch_row(debug_res);
    if (debug_row == NULL)
    {
      debuggee_state= DEBUGGEE_STATE_MYSQL_FETCH_ROW_ERROR;
      strncpy(debuggee_state_error, lmysql->ldbms_mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
      lmysql->ldbms_mysql_free_result(debug_res);
      break;
    }
    debuggee_connection_id= atoi(debug_row[0]);
    lmysql->ldbms_mysql_free_result(debug_res);

    strcpy(call_statement, "call xxxmdbug.become_debuggee_connection('");
    strcat(call_statement, debuggee_channel_name);
    strcat(call_statement, "');");
    debuggee_state= DEBUGGEE_STATE_BECOME_DEBUGEE_CONNECTION;
    if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_DEBUGGER_CONNECTION], call_statement, strlen(call_statement)))
    {
      debuggee_state= DEBUGGEE_STATE_BECOME_DEBUGGEE_CONNECTION_ERROR;
      strncpy(debuggee_state_error, lmysql->ldbms_mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
      break;
    }
    /*
      Originally we just had debuggee_wait_loop here.
      Then the PREPARE+EXECUTE failed because of a procedure that contained a SELECT.
      So I replaced debuggee_wait_loop()'s PREPARE with "LEAVE".
      The actual execution will be done here in C.
      Todo: Replace all debuggee_wait_loop() with a C routine.
      Todo: Catch errors -- debuggee_wait_loop() didn't necessarily end via LEAVE x.
    */
    strcpy(call_statement, "call xxxmdbug.debuggee_wait_loop();");
    debuggee_state= DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP;
    if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_DEBUGGER_CONNECTION], call_statement, strlen(call_statement)))
    {
      debuggee_state= DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR;
      strncpy(debuggee_state_error, lmysql->ldbms_mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
      break;
    }
    {
      char xxxmdbug_status_last_command_result[512];
      if (lmysql->ldbms_mysql_query(&mysql[MYSQL_DEBUGGER_CONNECTION], "select @xxxmdbug_status_last_command_result"))
      {
        /* This can happen, for example if debug command failed. */
        debuggee_state= DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR;
        unexpected_error=" select @xxxmdbug_status_last_command_result failed";
        strcpy(debuggee_state_error, "no result from last command");
        break;
      }
      debug_res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_DEBUGGER_CONNECTION]);
      if (debug_res == NULL)
      {
        unexpected_error= "mysql_store_result failed";
      }
      if (lmysql->ldbms_mysql_num_fields(debug_res) != 1)
      {
        unexpected_error= "wrong field count";
        lmysql->ldbms_mysql_free_result(debug_res);
        break;
      }
      debug_row= lmysql->ldbms_mysql_fetch_row(debug_res);
      if (debug_row == NULL)
      {
        unexpected_error= "mysql_fetch_row failed";
        lmysql->ldbms_mysql_free_result(debug_res);
       break;
      }
      strcpy(xxxmdbug_status_last_command_result, debug_row[0]);
      lmysql->ldbms_mysql_free_result(debug_res);
      /* There are various errors that xxxmdbug.check_surrogate_routine could have returned. */
      if ((strstr(xxxmdbug_status_last_command_result,"Error reading setup log") != 0)
       || (strstr(xxxmdbug_status_last_command_result,"is incorrect format for a surrogate identifier") != 0)
       || (strstr(xxxmdbug_status_last_command_result,"is not found once in xxxmdbug.setup_log") != 0)
       || (strstr(xxxmdbug_status_last_command_result,"was set up with an older debugger version") != 0)
       || (strstr(xxxmdbug_status_last_command_result,"original was not found") != 0)
       || (strstr(xxxmdbug_status_last_command_result,"original was altered") != 0)
       || (strstr(xxxmdbug_status_last_command_result,"surrogate was not found") != 0))
      {
        debuggee_state= DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR;
        strncpy(debuggee_state_error, xxxmdbug_status_last_command_result, STRING_LENGTH_512 - 1);
        break;
      }
    }
    if (lmysql->ldbms_mysql_query(&mysql[MYSQL_DEBUGGER_CONNECTION], "select @xxxmdbug_what_to_call"))
    {
      unexpected_error=" select @xxxmdbug_what_to_call failed";
      break;
    }
    debug_res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_DEBUGGER_CONNECTION]);
    if (debug_res == NULL)
    {
      unexpected_error= "mysql_store_result failed";
    }
    if (lmysql->ldbms_mysql_num_fields(debug_res) != 1)
    {
      unexpected_error= "wrong field count";
      lmysql->ldbms_mysql_free_result(debug_res);
      break;
    }
    debug_row= lmysql->ldbms_mysql_fetch_row(debug_res);
    if (debug_row == NULL)
    {
      unexpected_error= "mysql_fetch_row failed";
      lmysql->ldbms_mysql_free_result(debug_res);
      break;
    }
    strcpy(call_statement, debug_row[0]);
    lmysql->ldbms_mysql_free_result(debug_res);
    if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_DEBUGGER_CONNECTION], call_statement, strlen(call_statement)))
    {
      debuggee_state= DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR;
      strncpy(debuggee_state_error, lmysql->ldbms_mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
    }
    else
    {
        /*
          Following is pretty much the same as for MYSQL_MAIN_CONNECTION.
          We're just throwing result sets away. NB: the statement has not
          really ended -- when there's a result set MySQL temporarily drops
          out of the procedure, and resumes during each iteration of this loop.
          Todo: we can put each result into the result grid widget as it comes;
          we'll have to decide whether the debugger should do it, or the main.
        */
        if (lmysql->ldbms_mysql_more_results(&mysql[MYSQL_DEBUGGER_CONNECTION]))
        {
          debug_res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_DEBUGGER_CONNECTION]);
          if (debug_res != 0)
          {
            lmysql->ldbms_mysql_free_result(debug_res);

            if (lmysql->ldbms_mysql_more_results(&mysql[MYSQL_DEBUGGER_CONNECTION]))
            {
              while (lmysql->ldbms_mysql_next_result(&mysql[MYSQL_DEBUGGER_CONNECTION]) == 0)
              {
                debug_res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_DEBUGGER_CONNECTION]);
                if (debug_res != 0) lmysql->ldbms_mysql_free_result(debug_res);
              }
              debug_res= 0;
            }
          }
        }
      debuggee_state= DEBUGGEE_STATE_END;
    }
    break;
  }

  /* Todo: Even after failure, get rid of result sets. But, oddly, lmysql->ldbms_mysql_next_result() never returns anything. */
  /* This just would throw any results away, which is what happens also with MYSQL_MAIN_CONNECTION. */
  /* What I'd have liked to do is: have the main connection put the result set up on the main screen. */
  //while (lmysql->ldbms_mysql_next_result(&mysql[MYSQL_DEBUGGER_CONNECTION]) == 0) ;
  //if (lmysql->ldbms_mysql_more_results(&mysql[MYSQL_DEBUGGER_CONNECTION]))
  //{
  //  while (lmysql->ldbms_mysql_next_result(&mysql[MYSQL_DEBUGGER_CONNECTION]) == 0)
  //  {
  //    debug_res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_DEBUGGER_CONNECTION]);
  //    if (debug_res != 0) lmysql->ldbms_mysql_free_result(debug_res);
  //  }
  //  debug_res= 0;
  //}
  if (unexpected_error != NULL)
  {
    strncpy(debuggee_state_error, lmysql->ldbms_mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
  }
  /* Cleanup */
  /* Shut the connection, which seems to cause disconnect automatically. */
  if (is_connected == 1) lmysql->ldbms_mysql_close(&mysql[MYSQL_DEBUGGER_CONNECTION]);

  /* Typically we'll reach this point with last_command_result = "debuggee_wait_loop() termination" */

  /* Here I am overwriting DEBUGGEE_WAIT_LOOP_ERROR / DEBUGGEE_STATE_END. Maybe they're informative but they caused trouble. */
  //debuggee_state= DEBUGGEE_STATE_0;
  /* options_and_connect called mysql_init which called mysql_thread_init, so cancel it */
  lmysql->ldbms_mysql_thread_end();
  /* The thread will end. */
  return ((void*) NULL);
}


/*
  Debug|Install -- install the debugger routines and tables in xxxmdbug.
  The actual CREATE statements are all xxxmdbug_install_sql in
  a separate file, install_sql.cpp, so that it's clear what the HP-licence part is.
  debug|install menu item is commented out, one must say $INSTALL
  Todo: decide what to do if it's already installed.
  Todo: privilege check
  Todo: what we really should be doing: passing the original command, in all cases.
*/
//void MainWindow::action_debug_install()
//{
//  statement_edit_widget->setPlainText("$INSTALL");
//  action_execute(1);
//}

void MainWindow::debug_install_go()
{
  char x[32768];
  char command_string[2048];
  QString qstring_error_message;

  if (debug_error((char*)"") != 0) return;

  if ((statement_edit_widget->dbms_version.mid(1,1) == ".")
  &&  (statement_edit_widget->dbms_version < "5.5"))
  {
    strcpy(command_string, er_strings[er_off + ER_DEBUGGER_REQUIRES]);
    if (debug_error(command_string) != 0) return;
  }

  qstring_error_message= debug_privilege_check(TOKEN_KEYWORD_DEBUG_INSTALL);
  if (qstring_error_message != "")
  {
    strcpy(command_string, qstring_error_message.toUtf8());
    if (debug_error(command_string) != 0) return; /* setup wouldn't be able to operate so fail */
  }

  if (debug_mdbug_install_sql(&mysql[MYSQL_MAIN_CONNECTION], x) < 0)
  {
      put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
      return;
    //if (debug_error((char*)"Install failed") != 0) return;
  }
  put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
}

/* For copyright and license notice of debug_mdbug_install function contents, see beginning of this program. */
void debug_mdbug_install()
{
  ;
}
/* End of debug_mdbug_install. End of effect of licence of debug_mdbug_install_function contents. */


/*
  Debug|Setup
  Format =  $setup [options] object-name-list;
  debug|setup menu item is removed, one must say $SETUP ...

  Todo: This should put up a list box with the routines which the user can execute. No hard coding of test.p8 and test.p9.
        Or, it should look in the log for the last setup by this user, and re-do it.
  Todo: Shortcut = Same as last time.
  Todo: Search for schema name not just routine name.
  Todo: $setup does a GET_LOCK() so that two users won't do $setup at
        the same time. Figure out a better way.
  Todo: Check that 'install' has happened -- but for now it's OK, at least we return some sort of error.

  May 2018: $setup had to be totally rewritten, because it wouldn't work
  with MySQL 8.0. For explanation read
  https://ocelot.ca/blog/blog/2017/08/22/no-more-mysql-proc-in-mysql-8-0
  So the old unused code in install_sql.cpp still exists, and if we say
  #define NEW_SETUP 0 then it will still be invoked for test purposes,
  but now $setup code is entirely in ocelotgui.cpp (and incidentally
  is entirely written by Ocelot Computer Services, only the old
  code in install_sql.cpp was written for HP).
*/
#define NEW_SETUP 1

#if (NEW_SETUP == 0)
void MainWindow::debug_setup_go(QString text)
{
  if (hparse_dbms_mask & FLAG_VERSION_MYSQL_8_0)
  {
    QMessageBox msgbox;
     QString s= "Sorry, the ocelotgui debugger won't work with MySQL 8.0. ";
     s.append("For explanation read https://ocelot.ca/blog/blog/2017/08/22/no-more-mysql-proc-in-mysql-8-0/");
     s.append("We have replacement code that we believe will work -- ");
     s.append("try rebuilding ocelotgui after changing the line saying ");
     s.append("#define NEW_SETUP 0 to #define NEW_SETUP 1.");
     msgbox.setText(s);
     msgbox.exec();
     return;
  }
  char command_string[512];
  int index_of_number_1, index_of_number_2;
  QString qstring_error_message;
  QString q_routine_schema, q_routine_name;

  /* Todo: Check that 'install' has happened -- but for now it's OK, at least we return some sort of error. */
  /* Todo: use debug_error instead, provided debug_error gets rid of any problematic ''s */
  qstring_error_message= debug_privilege_check(TOKEN_KEYWORD_DEBUG_SETUP);
  if (qstring_error_message != "")
  {
    strcpy(command_string, qstring_error_message.toUtf8());
    if (debug_error(command_string) != 0) return; /* setup wouldn't be able to operate so fail */
  }
  if (debug_parse_statement(text, command_string, &index_of_number_1, &index_of_number_2) < 0)
  {
    if (debug_error(command_string) != 0) return;
  }

  q_routine_schema= debug_q_schema_name_in_statement;
  q_routine_name= debug_q_routine_name_in_statement;
  if ((q_routine_schema == "") || (q_routine_name == ""))
  {
    if (debug_error((char*)er_strings[er_off + ER_MISSING_ROUTINE_NAMES]) != 0) return;
  }
  /* Todo: since yes there is a bug, but we want to get this out, just release the lock instead of doing the right thing. */
  {
    QString result_string= select_1_row("SELECT is_free_lock('xxxmdbug_lock')");
    if (select_1_row_result_1.toInt() != 0)
    {
      //if (debug_error((char*)"Another user has done $setup and caused a lock. This might be an ocelotgui bug.") != 0) return;
      result_string= select_1_row("SELECT release_lock('xxxmdbug_lock')");
    }
  }

  /* Throw away the first word i.e. "$setup" and execute "call xxxmdbug.setup('...')". */
  char call_statement[512 + 128];
  strcpy(call_statement, "call xxxmdbug.setup('");
  strcat(call_statement, strstr(command_string, " ") + 1);
  strcat(call_statement, "')");

  if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
  {
    put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
    return;
    //if (debug_error((char*)"call xxxmdbug.setup() failed.") != 0) return;
  }
  debug_setup_mysql_proc_insert();
}
#endif


/*
  The following routine replaces the procedures xxxmdbug.mysql_proc_insert()
  and xxxmdbug.mysql_proc_insert_caller() in install_sql.cpp, which are no
  longer called from xxxmdbug.setup(). The effect is the same,
  but there is no INSERT into a mysql-database table.
  Also: definition_of_surrogate_routine will have three statements: DROP IF EXISTS, SET, CREATE.
        I could execute them all at once, but prefer to separate. Assume original name didn't contain ;.
        ... For icc_core routines the SET statement might be missing, I don't know why.
*/
/* TODO: THIS IS NO GOOD! YOU DO NOT WANT TO INSERT INTO MYSQL.PROC! */
void MainWindow::debug_setup_mysql_proc_insert()
{
  log("debug_setup_mysql_proc_insert start", 90);
  char command[512];
  MYSQL_RES *res= NULL;
  MYSQL_ROW row= NULL;
  const char *unexpected_error= NULL;
  unsigned int num_fields;
  unsigned int num_rows;
  QString definition_of_surrogate_routine;
  int it_is_ok_if_proc_is_already_there;
  QString list_of_drop_statements= "";

  lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "select * from xxxmdbug.routines");
  res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
  if (res != NULL)
  {
    num_rows= lmysql->ldbms_mysql_num_rows(res);
    lmysql->ldbms_mysql_free_result(res);
    res= NULL;
    if (num_rows - 3 >= DEBUG_TAB_WIDGET_MAX)
    {
      sprintf(command, er_strings[er_off + ER_SETUP], num_rows - 3, DEBUG_TAB_WIDGET_MAX - 1);
      put_message_in_result(command);
      return;
    }
  }
  lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "SET @xxxmdbug_saved_sql_mode=@@sql_mode");
  /*
    Go through all the rows of mysql.proc, doing DROP IF EXISTS and CREATE.
    If a create fails, we will later have a cleanup which should drop what we created.
  */
  int index_of_semicolon, index_of_set, index_of_create;
  QString first_query, second_query, third_query;
  //int second_iteration_is_necessary= 0;
  res= NULL;
  //for (int iteration=0; iteration <= 1; ++iteration)
  {
    //if ((iteration == 1) && (second_iteration_is_necessary == 0))
    //{
    //  break;
    //}
    if (res != NULL)
    {
      lmysql->ldbms_mysql_free_result(res);
      res= NULL;
    }
    row= NULL;

    strcpy(command,
    "SELECT TRIM(BOTH '`' FROM schema_identifier_of_original),"
    "       TRIM(BOTH '`' FROM routine_identifier_of_original),"
    "       TRIM(BOTH '`' FROM routine_identifier_of_surrogate),"
    "       definition_of_surrogate_routine,"
    "       offset_of_begin "
    "FROM xxxmdbug.routines");

    if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], command, strlen(command)))
    {
      unexpected_error= "select failed";
    }
    if (unexpected_error == NULL)
    {
      res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
      if (res == NULL) unexpected_error= "mysql_store_result failed";
    }
    if (unexpected_error == NULL)
    {
      num_fields= lmysql->ldbms_mysql_num_fields(res);
      if (num_fields != 5) unexpected_error= "wrong field count";
    }

    if (unexpected_error == NULL)
    {
      MYSQL_RES *res_2= NULL;
      for (;;)
      {
        row= lmysql->ldbms_mysql_fetch_row(res);
        if (row == NULL) break;
        it_is_ok_if_proc_is_already_there= 0;
        /* todo: make sure row[2] i.e. routine_identifier_of_surrogate is not null */
        if ((strcmp(row[2], "icc_process_user_command_r_server_variables") == 0)
         || (strcmp(row[2], "icc_process_user_command_set_server_variables") == 0))
        {
          /* We will not insert icc_process_user_command_r_server_variables
             or icc_process_user_command_set_server_variables into mysql.proc
             if it already exists. This is okay as long as there is no change of
             DBMS server version or engine or plugin. But recommend always:
             DROP PROCEDURE xxxmdbug.icc_process_user_command_r_server_variables; */
          it_is_ok_if_proc_is_already_there= 1;
        }
        else
        {
          /* MDBug might not care if the routine definition was blank, but we do. */
          char tmp[512];
          MYSQL_ROW row_2= NULL;
          unsigned int num_rows_2= 0;
          strcpy(tmp, "select routine_definition from information_schema.routines where routine_schema='");
          strcat(tmp, row[0]);
          strcat(tmp, "' and routine_name='");
          strcat(tmp, row[1]);
          strcat(tmp, "'");
          if (lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], tmp)) num_rows_2= 0;
          else
          {
            res_2= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
            if (res_2 != NULL)
            {
              num_rows_2= lmysql->ldbms_mysql_num_rows(res_2);
            }
          }
          if (num_rows_2 == 1)
          {
            row_2= lmysql->ldbms_mysql_fetch_row(res_2);
            if ((row_2 == NULL) || (row_2[0] == NULL) || (strcmp(row_2[0],"") == 0))
            {
              lmysql->ldbms_mysql_free_result(res_2);
              res_2= NULL;
              /* ""Could not get a routine definition for %s.%s" ... */
              sprintf(tmp, er_strings[er_off + ER_COULD_NOT_GET], row[0], row[1]);
              /* Todo: merge this sort of stuff into debug_error() */
              unexpected_error= "create failed";
              put_message_in_result(tmp);
              //second_iteration_is_necessary= 1;
              break;
            }
          }
          if (res_2 != NULL)
          {
            lmysql->ldbms_mysql_free_result(res_2);
            res_2= NULL;
          }
        }
        definition_of_surrogate_routine= QString::fromUtf8(row[3]);
        index_of_semicolon= definition_of_surrogate_routine.indexOf(";");
        index_of_set= definition_of_surrogate_routine.indexOf("SET session sql_mode = '", index_of_semicolon);
        if (index_of_set == -1) index_of_create= definition_of_surrogate_routine.indexOf("CREATE ", index_of_semicolon);
        else index_of_create= definition_of_surrogate_routine.indexOf("CREATE ", index_of_set);
        if (index_of_create == -1) unexpected_error= "bad definition_of_surrogate_routine";
        if (unexpected_error == NULL)
        {
          if (index_of_set == -1) first_query= definition_of_surrogate_routine.mid(0, index_of_create - 1);
          else first_query= definition_of_surrogate_routine.mid(0, index_of_set - 1);
          if (index_of_set != -1) second_query= definition_of_surrogate_routine.mid(index_of_set, index_of_create - index_of_set);
          third_query= definition_of_surrogate_routine.mid(index_of_create, -1);
          if (it_is_ok_if_proc_is_already_there == 0)
          {
            if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], first_query.toUtf8(), strlen(first_query.toUtf8())))
            {
              /* Actually, a failed drop will have to be considered to be okay. */
            }
          }
          if ((unexpected_error == NULL) && (index_of_set != -1))
          {
            if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], second_query.toUtf8(), strlen(second_query.toUtf8())))
            {
              unexpected_error= "set failed";
              break;
            }
          }
          if (unexpected_error == NULL)
          {
            //if (iteration == 0)
            {
              /* Kludge. If we have DEFINER=@ we have to change to DEFINER=''@''. */
              {
                int i;
                i= third_query.indexOf("DEFINER=@");
                if (i > 0)
                {
                  third_query.insert(i + 8, "''");
                }
                i=third_query.indexOf("@ PROCEDURE `");
                if (i < 0) i=third_query.indexOf("@ FUNCTION `");
                if (i < 0) i=third_query.indexOf("@ TRIGGER `");
                if (i > 0)
                {
                  third_query.insert(i + 1, "''");
                }
              }
              if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], third_query.toUtf8(), strlen(third_query.toUtf8())))
              {
                /* If there's an error, the diagnostics here should go all the way back up to the user. */
                if (it_is_ok_if_proc_is_already_there == 0)
                {
                  unexpected_error= "create failed";
                  put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
                  //second_iteration_is_necessary= 1;
                  if (res != NULL)
                  {
                    lmysql->ldbms_mysql_free_result(res);
                    res= NULL;
                  }
                  break;
                }
              }
              list_of_drop_statements.append(first_query);
            }
          }
        }
      }
      if (res != NULL)
      {
        lmysql->ldbms_mysql_free_result(res);
        res= NULL;
      }
      if (res_2 != NULL)
      {
        lmysql->ldbms_mysql_free_result(res_2);
        res_2= NULL;
      }
    }
    if (res != NULL)
    {
      lmysql->ldbms_mysql_free_result(res);
      res= NULL;
    }
  }
  if (res != NULL)
  {
    lmysql->ldbms_mysql_free_result(res);
    res= NULL;
  }

  log("debug_setup_mysql_proc_insert end", 90);
  lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "set session sql_mode=@xxxmdbug_saved_sql_mode");

  if (unexpected_error != NULL)
  {
    while (list_of_drop_statements > " ")
    {
      int i= list_of_drop_statements.indexOf(";") + 1;
      if (i <= 0) break;
      QString first_query= list_of_drop_statements.left(i);
      if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], first_query.toUtf8(), strlen(first_query.toUtf8())))
      {
        ; /* create failed, cleanup failed, but what can I do? */
      }
      list_of_drop_statements= list_of_drop_statements.right(list_of_drop_statements.size() - i);
    }
    if (strcmp(unexpected_error, "create failed") ==0) return; /* we already have the diagnostics */
    put_message_in_result(unexpected_error);
    return;
  }
  put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
}


/*
  debug_privilege_check() -- check in advance whether some debuggee-related privileges exist
  It will be more convenient if the user is told in advance, and is told multiple problems at once.
  Of course I could search for the grants, but it's easier to try it and see what happens.
  However, this doesn't consider all possibilities and is only called before initializing.
  To see if you have execute privilege, see if you can select from information_schema.routines, rather than execute.
  A variant of that would be selecting where routine_definition > '' if you want to look for all privileges.
  As well as the checks here, $setup also needs routine_definition > '' i.e. you must be routine creator
  or you must have select privilege on mysql.proc.
  Todo: check other possible things that the debuggee could do, and check all routines not just xxxmdbug.command.
        one way would be to say 'command' OR 'other_routine' ... but putting into a loop would be better, I think.
  Todo: xxxmdbug.command calls xxxmdbug.privilege_checks which will generate a signal,
        but I'm wondering whether it's good because it sends messages using init_connect every time.
  statement_type = TOKEN_KEYWORD_DEBUG_DEBUG or TOKEN_KEYWORD_DEBUG_SETUP or TOKEN_KEYWORD_DEBUG_INSTALL.
*/
QString MainWindow::debug_privilege_check(int statement_type)
{
  QString s= "";
  char call_statement[512];
  unsigned int num_rows;
  MYSQL_RES *res;
  QString result_string;

  if (connections_is_connected[0] != 1)
  {
    s.append("Not connected");
    return s;
  }

  if (statement_type == TOKEN_KEYWORD_DEBUG_INSTALL)
  {
    /* Somebody has to have said: grant create, drop, create routine, alter routine, select, insert, update, delete, select on xxxmdbug.* ... */
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "create database xxxmdbug");
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need create privilege on xxxmdbug.*. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "drop table if exists xxxmdbug.xxxmdbug");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "create table xxxmdbug.xxxmdbug (s1 int)");
    if ((s == "") && (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142)) s.append("Need create privilege on xxxmdbug.*. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'select * from xxxmdbug.xxxmdbug'");
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need select privilege on xxxmdbug.*. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'insert into xxxmdbug.xxxmdbug values (1)'");
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need insert privilege on xxxmdbug.*. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'delete from xxxmdbug.xxxmdbug'");
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need delete privilege on xxxmdbug.*. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'update xxxmdbug.xxxmdbug set s1 = 1'");
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need update privilege on xxxmdbug.*. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "drop table xxxmdbug.xxxmdbug");
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need drop privilege on xxxmdbug.*. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "drop procedure if exists xxxmdbug.xxxmdbug");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "create procedure xxxmdbug.xxxmdbug () set @a = 1");
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need create routine privilege on xxxmdbug.*. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "drop procedure xxxmdbug.xxxmdbug");
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1370) s.append("Need alter routine privilege on xxxmdbug.*. ");
    return s;
  }

  /* First make sure xxxmdbug exists */
  num_rows= 0;
  if (lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "select * from information_schema.schemata where schema_name = 'xxxmdbug'"))
  {
    s.append("Cannot select from information_schema");
    return s;
  }
  res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
  if (res != NULL)
  {
    num_rows= lmysql->ldbms_mysql_num_rows(res);
    lmysql->ldbms_mysql_free_result(res);
  }
  if (num_rows != 1)
  {
    s.append("Cannot access database xxxmdbug. Check that $install was done and privileges were granted.");
    return s;
  }

  if ((statement_type == TOKEN_KEYWORD_DEBUG_DEBUG) || (statement_type == TOKEN_KEYWORD_DEBUG_SETUP))
  {
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'select * from xxxmdbug.readme union all select * from xxxmdbug.copyright'");/* Returns 1142 if SELECT denied */
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need select privilege on xxxmdbug.*. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'create temporary table xxxmdbug.xxxmdbug_tmp (s1 int)'");/* Returns 1044 if CREATE TEMPORARY denied */
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need create temporary privilege on xxxmdbug.*. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
  }

  if (statement_type == TOKEN_KEYWORD_DEBUG_DEBUG)
  {
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'set global init_connect = @@init_connect'");/* Returns 1227 if you need SUPER */
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1227) s.append("Need super privilege. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'call xxxmdbug.command()'");
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need execute privilege on xxxmdbug.command. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
  }

  if (statement_type == TOKEN_KEYWORD_DEBUG_SETUP)
  {
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare stmt1 from 'select * from mysql.proc'");
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need select privilege on mysql.proc. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");

    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'create table xxxmdbug.xxxmdbug_nontmp (s1 int)'");/* Returns 1044 if CREATE TEMPORARY denied */
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need create privilege on xxxmdbug.*. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");

    /* Can't prepare create-routine statements so test by actually creating and dropping a trivial for-test-only routine */
    sprintf(call_statement, "create procedure xxxmdbug.xxxmdbugp () set @=@a");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement);
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need create routine privilege on xxxmdbug.*. ");
    else
    {
      sprintf(call_statement, "drop procedure xxxmdbug.xxxmdbugp");
      lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement);
      if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need drop routine privilege on xxxmdbug.*. ");
    }
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'call xxxmdbug.setup()'");
    if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need execute privilege on xxxmdbug.setup. ");
    lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");

    result_string= select_1_row("select count(*) from information_schema.tables where table_schema = 'xxxmdbug' and table_name = 'setup_log'" );
    if (result_string != "") s.append(result_string);
    else if (select_1_row_result_1.toInt() != 0)
    {
      lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'select * from xxxmdbug.setup_log'");
      if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need select privilege on xxxmdbug.setup_log. ");
      lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
      lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'insert into setup_log select * from xxxmdbug.setup_log'");
      if (lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need insert privilege on xxxmdbug.setup_log. ");
      lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
      /* Todo: Find out why repair table is sometimes necessary. I know it can get corrupted but don't yet know why. */
      if (lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "repair table xxxmdbug.setup_log"))
      {
        s.append("Repair Table xxxmdbug.setup_log failed");
      }
      else
      {
        res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
        if (res != NULL) lmysql->ldbms_mysql_free_result(res);
      }
    }
  }

  /* TODO: Following isn't good. */

  result_string= select_1_row("select count(*) from information_schema.routines where routine_schema = 'xxxmdbug' and routine_name = 'command'" );
  if (result_string != "") s.append(result_string);
  else if (select_1_row_result_1.toInt() == 0) s.append("Need execute privilege for xxxmdbug.*. ");
  return s;
}


/*
  Called from: debug_debug_go(), debug_breakpoint_or_clear_go(), or other debug_ routines that might contain arguments.
  An SQL statement may contain comments, plus semicolon and/or delimiter, which should be stripped before passing to xxxmdbug.
  Also these procedures need to know: which tokens (if any) are for schema, routine, start line number, end line number.
  For example from "$DEBUG x.y -- comment;", we want to produce "debug x.y" and the offsets of x and y.
  Possible are: breakpoint/tbreakpoint location information [condition information]
                clear [schema_identifier[.routine_identifier]] line_number_minimum [-line_number_maximum]
                debug [schema_identifier[.routine_identifier]
                delete breakpoint number
  This does not detect syntax errors, we assume xxxmdbug.command() will return an error immediately for a syntax error.
  So it's not really a parse, but should find the tokens that matter.
  (Of course, hparse code is called by default, so parsing will happen.)
  Todo: see if confusion happens if there's condition information, or parameters enclosed inside ''s.
  If there is clearly an error, debug_parse_statement() returns -1 and command_string has an error message.
  We pass text, but we tokenized, so we have main_token_* tables and
  main_token_number and main_token_count_in_statement.
*/
int MainWindow::debug_parse_statement(QString text,
                           char *command_string,
                           int *index_of_number_1,
                           int *index_of_number_2)
{
  char token[512];
  int i, s_len;
  int token_type;
  QString s;
//  int index_of_dot= -1;
//  int index_of_minus= -1;
  int statement_type= 0;
  //bool left_parenthesis_seen= false;
  bool name_is_expected= false; /* true if syntax demands [schema_name.]routine_name at this point */
  char default_schema_name[512];

  strcpy(default_schema_name, "");
  debug_q_schema_name_in_statement="";
  debug_q_routine_name_in_statement="";
  *index_of_number_1= -1;
  *index_of_number_2= -1;
  strcpy(command_string, "");
  int last_token= main_token_number + main_token_count_in_statement;
  for (i= main_token_number;
       ((main_token_lengths[i] != 0) && (i < last_token));
       ++i)
  {
    token_type= main_token_types[i];
    if ((token_type == TOKEN_TYPE_COMMENT_WITH_SLASH)
     || (token_type == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
     || (token_type == TOKEN_TYPE_COMMENT_WITH_MINUS)) continue;
    s= text.mid(main_token_offsets[i], main_token_lengths[i]);

    if ((token_type >= TOKEN_KEYWORD_DEBUG_BREAKPOINT)
     && (token_type <= TOKEN_KEYWORD_DEBUG_TBREAKPOINT)
     && (strcmp(command_string, "") == 0))
    {
      /* We're at the first word, which is the statement type. Write it in full. */
      statement_type= token_type;
      if (token_type == TOKEN_KEYWORD_DEBUG_BREAKPOINT) { strcpy(command_string, "breakpoint"); name_is_expected= true; }
      if (token_type == TOKEN_KEYWORD_DEBUG_CLEAR) { strcpy(command_string, "clear"); name_is_expected= true; }
      if (token_type == TOKEN_KEYWORD_DEBUG_CONTINUE) strcpy(command_string, "continue");
      if (token_type == TOKEN_KEYWORD_DEBUG_DEBUG) { strcpy(command_string, "debug"); name_is_expected= true; }
      if (token_type == TOKEN_KEYWORD_DEBUG_DELETE) strcpy(command_string, "delete");
      if (token_type == TOKEN_KEYWORD_DEBUG_EXECUTE) strcpy(command_string, "execute");
      if (token_type == TOKEN_KEYWORD_DEBUG_EXIT) strcpy(command_string, "exit");
      if (token_type == TOKEN_KEYWORD_DEBUG_INFORMATION) strcpy(command_string, "information");
      if (token_type == TOKEN_KEYWORD_DEBUG_INSTALL) strcpy(command_string, "install");
      if (token_type == TOKEN_KEYWORD_DEBUG_LEAVE) strcpy(command_string, "leave");
      if (token_type == TOKEN_KEYWORD_DEBUG_NEXT) strcpy(command_string, "next");
      if (token_type == TOKEN_KEYWORD_DEBUG_REFRESH) strcpy(command_string, "refresh");
      if (token_type == TOKEN_KEYWORD_DEBUG_SET) strcpy(command_string, "set");
      if (token_type == TOKEN_KEYWORD_DEBUG_SETUP) { strcpy(command_string, "setup"); name_is_expected= true; }
      if (token_type == TOKEN_KEYWORD_DEBUG_SKIP) strcpy(command_string, "skip");
      if (token_type == TOKEN_KEYWORD_DEBUG_SOURCE) strcpy(command_string, "source");
      if (token_type == TOKEN_KEYWORD_DEBUG_STEP) strcpy(command_string, "step");
      if (token_type == TOKEN_KEYWORD_DEBUG_TBREAKPOINT) { strcpy(command_string, "tbreakpoint"); name_is_expected= true; }
      continue;
    }

    s= text.mid(main_token_offsets[i], main_token_lengths[i]);
    s_len= s.toUtf8().size(); /* See comment "UTF8 Conversion" */
    if (s_len + 1 >= 512) { strcpy(command_string, "overflow"); return -1; }
    memcpy(token, s.toUtf8().constData(), s_len);
    token[s_len]= 0;
    if (token[0] == ';') continue;
    if (s == ocelot_delimiter_str) continue;
//    if (token[0] == '.') index_of_dot= i;
//    if (token[0] == '-') index_of_minus= i;
//    if (token[0] == '(') left_parenthesis_seen= true;
    if (name_is_expected)
    {
      if ((token_type == TOKEN_TYPE_OTHER)
       || (token_type == TOKEN_TYPE_IDENTIFIER)
       || (token_type == TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE)
       || (token_type == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK))
      {
        /* It's possibly a routine name, and we're expecting a routine name. */
        char tmp_schema_name[512];
        char tmp_routine_name[512];
        int s2_len;
        QString s2;
        if (text.mid(main_token_offsets[i + 1], 1) == ".")
        {
          /* schema-name . routine_name */
          if ((main_token_types[i + 2] == TOKEN_TYPE_OTHER)
           || (main_token_types[i + 2] == TOKEN_TYPE_IDENTIFIER)
           || (token_type == TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE)
           || (main_token_types[i + 2] == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK))
          {
            strcpy(tmp_schema_name, token);
            s2= text.mid(main_token_offsets[i + 2], main_token_lengths[i + 2]);
            s2_len= s2.toUtf8().size(); /* See comment "UTF8 Conversion" */
            if (s2_len + 1 + s_len + 1 >= 512) { strcpy(command_string, "overflow"); return -1; }
            memcpy(tmp_routine_name, s2.toUtf8().constData(), s2_len);
            tmp_routine_name[s2_len]= 0;
            i+= 2;
          }
          else
          {
            strcpy(command_string, "Expected routine-name after .");
            return -1;
          }
        }
        else
        {
          /* Just routine name. Fill in [default schema name]. Todo: shouldn't require a server request so frequently. */
          if (strcmp(default_schema_name, "") == 0)
          {
            QString result_string;
            select_1_row_result_1= "";
            result_string= select_1_row("select database()");
            if (result_string != "") { strcpy(command_string, "Need to know default database but select database() failed"); return -1; }
            if (select_1_row_result_1== "") { strcpy(command_string, "Need to know default database but select database() returned nothing"); return -1; }
            s2= select_1_row_result_1;
            s2_len= s2.toUtf8().size();
            if (s2_len + 1 + s_len + 1 >= 512) { strcpy(command_string, "overflow"); return -1; }
            memcpy(default_schema_name, s2.toUtf8().constData(), s2_len);
            default_schema_name[s2_len]= 0;
          }
          strcpy(tmp_routine_name, token);
          strcpy(tmp_schema_name, default_schema_name);
        }
        name_is_expected= false;

        QString aa= tmp_schema_name;
        aa= connect_stripper(aa, true);
        strcpy(tmp_schema_name, aa.toUtf8());
        aa= tmp_routine_name;
        aa= connect_stripper(aa, true);
        strcpy(tmp_routine_name, aa.toUtf8());

        strcpy(token, tmp_schema_name);
        strcat(token, ".");
        strcat(token, tmp_routine_name);

        {
          QString result_string;
          char query_for_select_check[512];
          sprintf(query_for_select_check, "select count(*) from information_schema.routines where routine_schema='%s' and routine_name='%s'", tmp_schema_name, tmp_routine_name);
          result_string= select_1_row(query_for_select_check);
          if (result_string != "") { strcpy(command_string, "Tried to check routine name but select from information_schema.routines failed"); return -1; }
          if (select_1_row_result_1.toInt() == 0)
          {
            sprintf(command_string, "Could not find routine %s.%s", tmp_schema_name, tmp_routine_name);
            return -1;
          }

        }

        debug_q_schema_name_in_statement= tmp_schema_name;
        debug_q_routine_name_in_statement= tmp_routine_name;
      }
      else
      {
        /* It's not possibly a routine identifier, and we're expecting a routine identifier. */
        strcpy(command_string, "Expected a routine name"); return -1;
      }
    }
    if ((token[0] == ',') && (statement_type == TOKEN_KEYWORD_DEBUG_SETUP)) name_is_expected= true;

    if ((token_type == TOKEN_TYPE_LITERAL_WITH_DIGIT)
     || (token_type == TOKEN_TYPE_LITERAL))
    {
      if (*index_of_number_1 == -1) *index_of_number_1= i;
      else if (*index_of_number_2 == -1) *index_of_number_2= i;
    }
    if (strlen(command_string) + strlen(token) + 1 >= 512) { strcpy(command_string, "overflow"); return -1; }
    if (strcmp(command_string, "") != 0) strcat(command_string, " ");
    strcat(command_string, token);
  }
  return 0;
}


/*
  First: find the surrogate routine that debuggee will call (if it's not there, error = you should call setup).
  Todo: schema_name could be '.' i.e. default
  Then: put up debug_top_widget
  Then: execute xxxmdbug.command(...,'debug ...')
  Todo: check that surrogate routine is not obsolete, i.e. timestamp is after original routine's timestamp
  Todo: check that everything in the group is present and executable
  Todo: check that user is the same ... but how can you do that? Is it only in setup_log?
  Todo: get rid of initialize() from menu
  Todo: when starting, make sure debuggee is not already running -- even a finished routine is not over
  Todo: if clicking causes you to get to action_debug_debug() you must generate a $DEBUG statement -- but how click a list??
*/
//void MainWindow::action_debug_debug()
//{
//  /* Figure out what the parameter should be -- it's not text, eh? */
//  QString parameter;
//  debug_debug_go(parameter);
//}


/*
  Call debug_error((char*)"error-text") whenever, in one of the debug routines, you encounter an error.
  Call debug_error((char*)"") regardless, at the start of a debug routine, so basic prerequisite checks can happen.
  For example, if the user says "$debug test.x" and there is no surrogate for x, we want the user to see:
  Error 1644 (05678) Surrogate not found.
  TODO: make sure all the debug-routine failures go through here!
  Todo: if the error is due to a failure of the last call, add what that last call's error message was.
*/
int MainWindow::debug_error(char *text)
{
//  unsigned int statement_type;

  if (connections_is_connected[0] == 0)
  {
    /* Unfortunately this might not get through. */
    make_and_put_message_in_result(ER_NOT_CONNECTED, 0, (char*)"");
    return 1;
  }

  if (debuggee_state < 0)
  {
    char tmp_string[STRING_LENGTH_512]="";
    char tmp_string_2[STRING_LENGTH_512 + 64];
    if (debuggee_state != DEBUGGEE_STATE_END)
    {
      /* debuggee_state_error should tell us something, but remove all ' first */
      strcpy(tmp_string, debuggee_state_error);
      for (int i= 0; tmp_string[i] != '\0'; ++i) if (tmp_string[i] == '\047') tmp_string[i]= ' ';
    }
    if (debuggee_state == DEBUGGEE_STATE_END)
    {
      make_and_put_message_in_result(ER_ROUTINE_HAS_STOPPED, 0, (char*)"");
    }
    else
    {
      sprintf(tmp_string_2, "'%s %d. %s'", "debuggee_state:", debuggee_state, tmp_string);
      put_message_in_result(tmp_string_2);
    }
    return 1;
  }


  if (strcmp(text,"") != 0)
  {
    put_message_in_result(text);
    return 1;
  }

  return 0;
}


/* $DEBUG [schema.]routine [(parameters)] */
void MainWindow::debug_debug_go(QString text) /* called from execute_client_statement() or action_debug_debug() */
{
  log("debug_debug_go", 90);
  char routine_schema[512];
  char routine_name[512];
  char call_statement[512];
  QString result_string;
  QString qstring_error_message;
  int current_widget_index;
  QString q_routine_schema, q_routine_name;
  char command_string[2048];
  int index_of_number_1, index_of_number_2;
  /* Todo: Check that 'debug' has not happened */
  if (debug_parse_statement(text, command_string, &index_of_number_1, &index_of_number_2) < 0)
  {
    if (debug_error(command_string) != 0) return;
  }

  q_routine_schema= debug_q_schema_name_in_statement;
  q_routine_name= debug_q_routine_name_in_statement;

  if ((q_routine_schema == "") || (q_routine_name == ""))
  {
    if (debug_error((char*)er_strings[er_off + ER_MISSING_ROUTINE_NAME]) != 0) return;
  }

  /*
    If a $debug was started before, and didn't finish, we demand that the user do $exit.
    We could instead call debug_exit_go(1).
    Perhaps $exit will fail somehow, and users will be falsely told they can't debug.
    That's a bug but better than hanging, which is a risk if we make two debug threads.
  */
  if (debug_thread_exists == true)
  {
    if (debug_error((char*)er_strings[er_off + ER_DEBUG_IS_ALREADY_RUNNING]) != 0) return;
  }

  if (debuggee_state < 0) debuggee_state= DEBUGGEE_STATE_0;
  if (debuggee_state == DEBUGGEE_STATE_END) debuggee_state= DEBUGGEE_STATE_0;
  if (debug_error((char*)"") != 0) return;

  strcpy(routine_schema, q_routine_schema.toUtf8());
  strcpy(routine_name, q_routine_name.toUtf8());

  if (debuggee_state > 0)
  {
    if (debug_error((char*)er_strings[er_off + ER_DEBUG_IS_ALREADY_RUNNING]) != 0) return;
  }

  qstring_error_message= debug_privilege_check(TOKEN_KEYWORD_DEBUG_DEBUG);
  if (qstring_error_message != "")
  {
    strcpy(command_string, qstring_error_message.toUtf8());
    if (debug_error(command_string) != 0) return; /* debuggee wouldn't be able to operate so fail */
  }
  /* Call xxxmdbug.debuggee_get_surrogate_name */
  strcpy(call_statement, "CALL xxxmdbug.debuggee_get_surrogate_name('");
  strcat(call_statement, routine_name);
  strcat(call_statement, "','");
  strcat(call_statement, routine_schema);
  strcat(call_statement, "',@schema_identifier,@surrogate_routine_identifier,@routine_type,@remainder_of_original_name)");
if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
  {
    if (debug_error((char*)er_strings[er_off + ER_SURROGATE]) != 0) return;
  }
  result_string= select_1_row("select @schema_identifier, concat(left(@surrogate_routine_identifier,11), '%'), @routine_type, @remainder_of_original_name, isnull(@schema_identifier)");
  if (result_string != "")
  {
    char char_result_string[512];
    strcpy(char_result_string, result_string.toUtf8());
    if (debug_error(char_result_string) != 0) return;
  }
  /* select_1_row_result_1 = @schema_identifier. select_1_row_result_2 = left(@surrogate_routine_identifier,11) || '%'. */

  /* Todo: this checks whether select returned null, good. But it should make even more certain that $debug will succeed. */
  if (select_1_row_result_5.toInt() == 1)
  {
    if (debug_error((char*)er_strings[er_off + ER_SURROGATE_NOT_FOUND]) != 0) return;
  }

  //QString routine_schema_name_for_search= select_1_row_result_1;
  QString routine_name_for_search= select_1_row_result_2;
  /*
    This will find all surrogate routines which have the same prefix, xxxmdbug___
    That means they're in the same group. Ignore icc_core.
    Get only the part that's not part of the prefix.
    Don't ask for schema name, group's routines can be in different schema.
    Interesting idea: you could have a way to show both the surrogate and the original.
  */
  int i, j;
  char i_as_string[10];
  /* Todo: n should not be hard-coded here, it limits us to only 10 routines can be searched */
  for (i= 0, j= 0; j < DEBUG_TAB_WIDGET_MAX; ++i)
  {
    sprintf(i_as_string, "%d", i);
    strcpy(call_statement, "select routine_schema, right(routine_name,length(routine_name)-12) from information_schema.routines ");
    strcat(call_statement, "where routine_name like '");
    strcat(call_statement, routine_name_for_search.toUtf8());
    strcat(call_statement, "' order by routine_name limit 1 offset ");
    strcat(call_statement, i_as_string);
    result_string= select_1_row(call_statement);
    if (result_string != "")
    {
      char char_result_string[512];
      if (result_string == er_strings[er_off + ER_MYSQL_FETCH_ROW_FAILED]) break;
      strcpy(char_result_string, result_string.toUtf8());
      if (debug_error(char_result_string) != 0) return;
    }
    if (select_1_row_result_2 != "icc_core")
    {
      /* Todo: debug_routine_schema_name and debug_routine_name are global so setup() will ruin them. Fix that! */
      debug_routine_schema_name[j]= select_1_row_result_1;
      debug_routine_name[j]= select_1_row_result_2;
      ++j;
    }
  }

  /* Todo: check: can this crash if there are DEBUG_TAB_WIDGET_MAX routines? Is this unnecessary if there are DEBUG_TAB_WIDGET_MAX routines? */
  debug_routine_name[j]= "";

  int debug_widget_index;

  /* Todo: check if this is useless. You're supposed to have called debug_delete_tab_widgets[] during $exit. */
  for (debug_widget_index= 0; debug_widget_index < DEBUG_TAB_WIDGET_MAX; ++debug_widget_index)
  {
    debug_widget[debug_widget_index]= 0;
  }
  /* After this point, some items that get created are persistent, so be sure to clear them if there's an error. */
  //MYSQL_RES *debug_res;
  //MYSQL_ROW row;
  QString routine_definition;
  current_widget_index= -1;
  for (debug_widget_index= 0; debug_widget_index < DEBUG_TAB_WIDGET_MAX; ++debug_widget_index)
  {
    if (strcmp(debug_routine_name[debug_widget_index].toUtf8(), "") == 0) break;
    strcpy(call_statement, "select routine_definition from information_schema.routines where routine_schema = '");
    strcat(call_statement, debug_routine_schema_name[debug_widget_index].toUtf8());
    strcat(call_statement, "' and routine_name = '");
    strcat(call_statement, debug_routine_name[debug_widget_index].toUtf8());
    strcat(call_statement, "'");
    QString error_return= select_1_row(call_statement);
    if (error_return != "")
    {
      debug_delete_tab_widgets();                          /* delete already-made widgets. Rest of 'exit' shouldn't happen. */
      {
        /* Could not find a routine in the $setup group: ... */
        char t_error_return[256];
        char t_schema_name[256];
        char t_routine_name[256];
        strcpy(t_error_return, error_return.toUtf8());
        strcpy(t_schema_name, debug_routine_schema_name[debug_widget_index].toUtf8());
        strcpy(t_routine_name, debug_routine_name[debug_widget_index].toUtf8());
        sprintf(call_statement,
                er_strings[er_off + ER_COULD_NOT_FIND_A_ROUTINE],
                t_error_return,
                t_schema_name,
                t_routine_name);
      }
      debug_error(call_statement);                         /* Todo: you forgot to look for icc_core */
      return;
    }
    routine_definition= select_1_row_result_1;             /* = information_schema.routines.routine_definition */

    if (routine_definition == "")
    {
      debug_delete_tab_widgets();
      {
        /* Could not get a routine definition for ... */
        char t_routine_name[256];
        strcpy(t_routine_name, debug_routine_name[debug_widget_index].toUtf8());
        sprintf(call_statement, er_strings[er_off + ER_COULD_NOT_GET], t_routine_name);
      }
      if (debug_error(call_statement)) return;
    }
    debug_widget[debug_widget_index]= new CodeEditor(this);
    debug_widget[debug_widget_index]->is_debug_widget= true;
    debug_widget[debug_widget_index]->statement_edit_widget_left_bgcolor= QColor(ocelot_statement_prompt_background_color);
    debug_widget[debug_widget_index]->statement_edit_widget_left_treatment1_textcolor= QColor(ocelot_statement_text_color);

    debug_maintain_prompt(0, debug_widget_index, 0); /* clear prompt_as_input_by_user */

    debug_widget[debug_widget_index]->setStyleSheet(ocelot_statement_style_string);

    /* todo: call 'refresh breakpoints' and debug_maintain_prompt() for breakpoints that already have been set up. */
    debug_widget[debug_widget_index]->prompt_default= (QString)"\\2 \\L";
    debug_widget[debug_widget_index]->result= (QString)"ABCDEFG";
    debug_widget[debug_widget_index]->setPlainText(routine_definition);

    debug_widget[debug_widget_index]->setReadOnly(false);                 /* if debug shouldn't be editable, set to "true" here */
    debug_widget[debug_widget_index]->installEventFilter(this);           /* is this necessary? */
    debug_tab_widget->addTab(debug_widget[debug_widget_index], debug_routine_name[debug_widget_index]);

    if ((QString::compare(debug_routine_name[debug_widget_index], q_routine_name, Qt::CaseInsensitive) == 0)
    &&  (QString::compare(debug_routine_schema_name[debug_widget_index], q_routine_schema, Qt::CaseInsensitive) == 0))
    {
      current_widget_index= debug_widget_index;
    }
  }
  if (current_widget_index == -1)
  {
    debug_delete_tab_widgets();
    if (debug_error((char*)er_strings[er_off + ER_ROUTINE_IS_MISSING])) return;
  }
  debug_tab_widget->setCurrentIndex(current_widget_index);

  /* Getting ready to create a separate thread for the debugger and 'attach' to it */

  char error_message[512];

  debuggee_state= DEBUGGEE_STATE_0;

  /*
    We want the debuggee default database to be the main default
    database, but that's not necessarily ocelot_database_as_utf8
    because maybe we had a USE statement.
    Todo: I'm a bit unclear what to do if this fails.
  */
  if (select_1_row("select database()") != 0) debuggee_database[0]= '\0';
  else strcpy(debuggee_database, select_1_row_result_1.toUtf8());

  /*
    We want a channel name that will differ from what others might choose.
    Arbitrary decision = debug_channel_name will be "ch#" || connection-number-of-main-connection.
    It could be uniquified beyond that if necessary, e.g. add user name.
    Todo: this could be moved so it doesn't happen every time $debug happens. */

  sprintf(debuggee_channel_name, "ch#%d", statement_edit_widget->dbms_connection_id);

  /* Create a debuggee thread. */
  /* Todo: consider whether it would have been better to use the Qt QThread function */
  int pthread_create_result= pthread_create(&debug_thread_id, NULL, &debuggee_thread, NULL);
  assert(pthread_create_result == 0);
  debug_thread_exists= true;
  /*
    Wait till debuggee has indicated that it is about to call debuggee_wait_loop().
    Todo: Give a better diagnostic if this doesn't happen.
    Todo: Is it possible to fail anyway? If so, sleep + repeat?
    Todo: This gives up after 1 second. Maybe on a heavily loaded machine that's too early?
  */
  for (int k= 0; k < 100; ++k)
  {
    QThread48::msleep(10);
    if (debuggee_state == DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP) break;
  }
  QThread48::msleep(10); /* in case debuggee_wait_loop() fails immediately */
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    /* Todo: if somehow (bizarrely) debuggee_state >= 0, then the thread did not end and needs to be stopped. */
    debug_delete_tab_widgets();
    sprintf(error_message, er_strings[er_off + ER_DEBUGGEE_NOT_RESPONDING], debuggee_state);
    if (debug_error(error_message) != 0) return;
  }
  /*
    Attach to the debuggee.
    Todo: Check: is it possible for this to fail because thread has not connected yet? If so, sleep + repeat?
    Todo: Check: why use insertPlainText not setPlainText?
  */
  strcpy(call_statement, "call xxxmdbug.command('");
  strcat(call_statement, debuggee_channel_name);
  strcat(call_statement, "', 'attach');\n");
  if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
  {
    /* Attach failed. Doubtless there's some sort of error message. Put it out now, debug_exit_go() won't override it. */
    put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
    debug_exit_go(1); /* This tries to tell the debuggee to stop, because we're giving up. */
    return;
  }
  debug_timer_old_line_number= -1;
  debug_timer_old_schema_identifier[0]= '\0';
  debug_timer_old_routine_identifier[0]= '\0';
  debug_timer_old_commands_count= -1;
  debug_timer_old_number_of_status_message= -1;
  debug_timer_old_icc_count= -1;
  debug_timer_old_debug_widget_index= -1;
  debug_timer->start(DEBUG_TIMER_INTERVAL);

  /* By the way, we fill in [schema.] if it's missing, because otherwise default would be 'test'. */

  //  strcpy(call_statement, "debug ");
  //if (strcmp(routine_schema, "") != 0)
  //{
  //  strcat(call_statement, routine_schema);
  //  strcat(call_statement, ".");
  //}
  //strcat(call_statement, routine_name);

  if (debug_call_xxxmdbug_command(command_string) != 0)
  {
    /* Debug failed. Doubtless there's some sort of error message. Put it out now, debug_exit_go() won't override it. */
    put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
    debug_exit_go(1); /* This tries to tell the debuggee to stop, because we're giving up. */
    return;
  }
  put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
  /* TODO: next line was removed TEMPORARILY. But maybe highlighting will occur due to temporary breakpoint? */
  //debug_highlight_line(); /* highlight the first line. todo: should be the first executable line, no? */
  debug_top_widget->show(); /* Is this necessary? I think so because initially debug_window should be hidden. */
  debug_menu_enable_or_disable(TOKEN_KEYWORD_DEBUG_DEBUG);             /* Until now most debug menu items were gray. */
}


/*
  Change the contents of debug_widget[debug_widget_index]->prompt_as_input_by_user.
  action: 0 = initialize/clear, 1 = add breakpoint, 2 = delete breakpoint, 3 = add tbreakpoint
  line_number: irrelevant if action == 0, else line number for add/clear
  We have to set the prompt -- the separated-from-the-text stuff on the left -- so that it shows
  the line numbers with \2 \L, and shows the breakpoints with K(line-number).
  We avoid 'refresh breakpoints' because it waits for debuggee to be free; hope it doesn't get out of synch.
  We should end up with something like "\2 \L \K(5,red,Ⓑ) \K(4,red,Ⓑ)" if there are breakpoints on line 5 and line 4.
  Todo: Even if we're doing adds, do all the deletes first so that if this is called twice there won't be a duplicate.
        (Todo: I think the debuggee would not make duplicates, but that needs checking.)
  Todo: breakpoint and tbreakpoint look the same in this scheme.
*/
void MainWindow::debug_maintain_prompt(int action, int debug_widget_index, int line_number)
{
  if (action == 0)                                                         /* clear */
  {
    debug_widget[debug_widget_index]->prompt_as_input_by_user= (QString)"\\2 \\L";
    return;
  }
  QString prompt_including_symbol;
  QString breakpoint_symbol;

  prompt_including_symbol= (QString)" \\K(";
  prompt_including_symbol.append(QString::number(line_number));
  breakpoint_symbol= QChar(0x24b7);                                        /* CIRCLED LATIN CAPITAL LETTER B */
  prompt_including_symbol.append(",red,");
  prompt_including_symbol.append(breakpoint_symbol);
  prompt_including_symbol.append(")");

  if (action == 2)                                                        /* delete (well, actually: clear */
  {
    /* I don't expect that indexOf() could return -1, and I ignore it if it does happen. */
    int index_of_prompt;
    index_of_prompt= debug_widget[debug_widget_index]->prompt_default.indexOf(prompt_including_symbol);
    if (index_of_prompt >= 0) debug_widget[debug_widget_index]->prompt_default.remove(index_of_prompt, prompt_including_symbol.size());
    index_of_prompt= debug_widget[debug_widget_index]->prompt_as_input_by_user.indexOf(prompt_including_symbol);
    if (index_of_prompt >= 0) debug_widget[debug_widget_index]->prompt_as_input_by_user.remove(index_of_prompt, prompt_including_symbol.size());
  }
  if ((action == 1) || (action == 3))                                    /* add */
  {
    debug_widget[debug_widget_index]->prompt_default.append(prompt_including_symbol);
    debug_widget[debug_widget_index]->prompt_as_input_by_user.append(prompt_including_symbol);
  }
}


/*
  Find out what is current routine and current line, put a symbol beside it.
  Execute a command e.g. "debug test.p8 5".
  So far: we get block_number = current line number - 1.
  Todo: we don't show the B in a circle!
  Do not confuse with action_mousebuttonpress which also generates "$breakpoint ...".
*/
void MainWindow::action_debug_breakpoint()
{
  log("action_debug_breakpoint", 90);
  char command[512];
  int line_number;
  char line_number_as_string[10];

  int debug_widget_index= debug_tab_widget->currentIndex();
  /* I can't imagine how currentIndex() could be == -1, but if it is, do nothing. */
  if (debug_widget_index < 0) return;
  line_number= debug_widget[debug_widget_index]->block_number + 1;
  sprintf(line_number_as_string, "%d", line_number);
  strcpy(command, "$BREAKPOINT ");
  strcat(command, debug_routine_schema_name[debug_widget_index].toUtf8());
  strcat(command,".");
  strcat(command, debug_routine_name[debug_widget_index].toUtf8());
  strcat(command, " ");
  strcat(command, line_number_as_string);

  statement_edit_widget->setPlainText(command);
  action_execute(1);
}


/*
  "breakpoint" and "clear" and "tbreakpoint" have the same argument syntax so we use the same routine for all,
  passing statement_type == TOKEN_KEYWORD_DEBUG_BREAKPOINT or TOKEN_KEYWORD_DEBUG_CLEAR.
  Todo: "$breakpoint ... hit_count = N" is not working.
  Todo: "$breakpoint ... variable_name <> old" is not working.
*/
void MainWindow::debug_breakpoint_or_clear_go(int statement_type, QString text)
{
  char command_string[512];
  int index_of_number_1, index_of_number_2;
  QString routine_name;
  QString schema_name;

  /* Todo: Check that 'debug' has happened */
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)er_strings[er_off + ER_NO_DEBUG_SESSION]) != 0) return;
  }

  if (debug_parse_statement(text, command_string, &index_of_number_1, &index_of_number_2) < 0)
  {
    char error_message[512];
    if (statement_type == TOKEN_KEYWORD_DEBUG_BREAKPOINT)
      strcpy(error_message, er_strings[er_off + ER_BREAKPOINT_SYNTAX]);
    else if (statement_type == TOKEN_KEYWORD_DEBUG_TBREAKPOINT)
      strcpy(error_message, er_strings[er_off + ER_TBREAKPOINT_SYNTAX]);
    else
      strcpy(error_message, er_strings[er_off + ER_CLEAR_SYNTAX]);
    if (debug_error(error_message) != 0) return;
  }
  schema_name= debug_q_schema_name_in_statement;
  routine_name= debug_q_routine_name_in_statement;

  if ((schema_name == "") || (routine_name == ""))
  {
    if (debug_error((char*)er_strings[er_off + ER_MISSING_ROUTINE_NAME]) != 0) return;
  }

  if (debug_error((char*)"") != 0) return;

  if (debug_call_xxxmdbug_command(command_string) != 0)
  {
    put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
    return;
  }

  /* Figure out line number and debug_widget_index from the index variables. */
  /* Todo: This is failing to compare schema name. */

  int debug_widget_index;

  for (debug_widget_index= 0; debug_widget_index < DEBUG_TAB_WIDGET_MAX; ++debug_widget_index)
  {
    if (debug_widget[debug_widget_index] != 0)
    {
      if ((QString::compare(routine_name, debug_routine_name[debug_widget_index]) == 0)
       && (QString::compare(schema_name, debug_routine_schema_name[debug_widget_index]) == 0)) break;
    }
  }

  if (debug_widget_index == DEBUG_TAB_WIDGET_MAX)
  {
    if (debug_error((char*)"No such routine") != 0) return;
  }

  int line_number_1, line_number_2;
  QString q_line_number;
  q_line_number= text.mid(main_token_offsets[index_of_number_1], main_token_lengths[index_of_number_1]);
  line_number_1= q_line_number.toInt();
  if (index_of_number_2 == -1) line_number_2= line_number_1;
  else
  {
    q_line_number= text.mid(main_token_offsets[index_of_number_2], main_token_lengths[index_of_number_2]);
    line_number_2= q_line_number.toInt();
  }

  for (int i= line_number_1; i <= line_number_2; ++i)
  {
    if (i > debug_widget[debug_widget_index]->blockCount())
    {
      break;
    }
    if (statement_type == TOKEN_KEYWORD_DEBUG_BREAKPOINT) debug_maintain_prompt(1, debug_widget_index, i);
    if (statement_type == TOKEN_KEYWORD_DEBUG_CLEAR) debug_maintain_prompt(2, debug_widget_index, i);
    if (statement_type == TOKEN_KEYWORD_DEBUG_TBREAKPOINT) debug_maintain_prompt(3, debug_widget_index, i);
  }
  /*
    Todo: I have had trouble, sometimes the breakpoint symbols don't show up unless I click on the line.
          I think that one of these kludges is helping, but I don't know if it always helps,
          and I don't know which of the two lines is actually fixing the problem,
          and I don't know whether there's a better way. So find out, eh?
  */
  debug_widget[debug_widget_index]->repaint();
  debug_widget[debug_widget_index]->viewport()->update();
  put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
}


/*
  Todo: The problem with "delete n" is we don't know which breakpoint is n.
  Of course it's in xxxmdbug.breakpoints, but "r breakpoints" only works if we're at a breakpoint.
*/
void MainWindow::debug_delete_go()
{
  make_and_put_message_in_result(ER_DELETE_STATEMENT, 0, (char*)"");
}


/*
  We will get here if mouse button pressed on debug_tab_widget (we don't know which debug_widget yet, possibly none of them).
  What's it good for? Probably this will be the main way to set a breakpoint.
  I think we'll need to find a right mousebutton press and put up a menu.
  But maybe it would be better if debug_window had a menu bar.
  In ddd, one of the many options is: double-click. But I didn't succeed with doubleclick.
  Todo: figure out what to do if there's already a breakpoint here. Clear it? Right now we're just duplicating it.
*/
void MainWindow::action_debug_mousebuttonpress(QEvent *event, int which_debug_widget_index)
{
  log("action_debug_mousebutonpress", 90);
  int line_number;
  char command[512];
  char schema_name[512];
  char routine_name[512];
  int debug_widget_index;

  QMouseEvent *mouse_event= static_cast<QMouseEvent*> (event);            /* Get line number where mousepress happened */
  QPoint point= mouse_event->pos();
  debug_widget_index= debug_tab_widget->currentIndex();
  /* I saw a crash once so maybe these checks aren't to paranoid. But I don't know whether they avoid the crash. */
  if (debug_widget_index == -1) return;                                   /* Check for an impossible condition */
  if (debug_widget_index > DEBUG_TAB_WIDGET_MAX) return;                  /* "" */
  if (which_debug_widget_index >= debug_tab_widget->count()) return;      /* "" */
  if (which_debug_widget_index != debug_widget_index) return;             /* "" */
  QTextCursor text_cursor= debug_widget[debug_widget_index]->cursorForPosition(point);
  line_number= text_cursor.blockNumber() + 1;

  strcpy(schema_name, debug_routine_schema_name[debug_widget_index].toUtf8());
  strcpy(routine_name, debug_routine_name[debug_widget_index].toUtf8());

  sprintf(command, "$breakpoint %s.%s %d", schema_name, routine_name, line_number);
  statement_edit_widget->setPlainText(command);                           /* Execute "$breakpoint ..." */
  action_execute(1);
}


/*
  Debug|Clear
  Similar code is in action_debug_breakpoint().
  Do not confuse with Debug|Delete: "$delete 5" clears a breakpoint #5.
  Todo: Check that somewhere something says debug_maintain_prompt(2, debug_widget_index, line_number);
*/
void MainWindow::action_debug_clear()
{
  log("action_debug_clear", 90);
  char command[512];
  int line_number;
  char line_number_as_string[10];

  int debug_widget_index= debug_tab_widget->currentIndex();
  /* I can't imagine how currentIndex() could be == -1, but if it is, do nothing. */
  if (debug_widget_index < 0) return;
  line_number= debug_widget[debug_widget_index]->block_number + 1;
  sprintf(line_number_as_string, "%d", line_number);
  strcpy(command, "$CLEAR ");
  strcat(command, debug_routine_schema_name[debug_widget_index].toUtf8());
  strcat(command,".");
  strcat(command, debug_routine_name[debug_widget_index].toUtf8());
  strcat(command, " ");
  strcat(command, line_number_as_string);
  statement_edit_widget->setPlainText(command);
  action_execute(1);
}


/*
  Debug|Continue -- "CALL xxxmdbug.command([channel-name],'continue');" as a client statement
*/
void MainWindow::action_debug_continue()
{
  log("action_debug_continue", 90);
  statement_edit_widget->setPlainText("$CONTINUE");
  action_execute(1);
}

/*
  Debug|Step
  This is much like Debug|Continue, but we don't care if we end up on a permanent breakpoint.
*/
void MainWindow::action_debug_step()
{
  log("action_debug_step", 90);
  statement_edit_widget->setPlainText("$STEP");
  action_execute(1);
}

/*
  Debug|Leave
  This isn't working quite right. It gets out of loops, but if it's
  in a subroutine then it will try to execute all the way to the end
  of the main routine. (A breakpoint can stop that from happening.)
  So we will allow the $LEAVE statement but not advertise it on the menu.
*/
//void MainWindow::action_debug_leave()
//{
//    statement_edit_widget->setPlainText("$LEAVE");
//    action_execute(1);
//}


/* $SKIP seems to act like $CONT which isn't terribly useful */
void MainWindow::debug_skip_go()
{
  make_and_put_message_in_result(ER_SKIP_STATEMENT, 0, (char*)"");
}


void MainWindow::debug_source_go()
{
  make_and_put_message_in_result(ER_SOURCE_STATEMENT, 0, (char*)"");
}


/*
  $SET declared_variable_name = value;
  Todo: As an additional error check: look up declared_variable_name in xxxmdbug.variables.
  TODO: BUG: $set x = 'literal'; results in a syntax error so say only numbers are allowed.
*/
void MainWindow::debug_set_go(QString text)
{
  char command_string[5120];
  int index_of_number_1, index_of_number_2;

  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)er_strings[er_off + ER_NO_DEBUG_SESSION]) != 0) return;
  }
  if (debug_parse_statement(text, command_string, &index_of_number_1, &index_of_number_2) < 0)
  {
    if (debug_error((char*)er_strings[er_off + ER_OVERFLOW]) != 0) return;
    return;
  }
  debug_call_xxxmdbug_command(command_string);
  put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
}


/*
  $execute sql-statement
  todo: this will fail if first token is a comment
  todo: get rid of this, it fails
*/
void MainWindow::debug_execute_go()
{
  make_and_put_message_in_result(ER_EXECUTE_STATEMENT, 0, (char*)"");

//  QString s;
//  char command_string[5120];
//  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
//  {
//    if (debug_error((char*)"No debug session in progress") != 0) return;
//  }
//  if (debug_error((char*)"") != 0) return;
//
//  s= "execute ";
//  s.append(text.right(text.length() - (main_token_offsets[main_token_number] + main_token_lengths[main_token_number])));
//  QMessageBox msgbox;
//  msgbox.setText(s);
//  msgbox.exec();
//  strcpy(command_string, s.toUtf8());
//  printf("command_string=%s.\n", command_string);
//  debug_call_xxxmdbug_command(s.toUtf8());
//  put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
////  put_message_in_result("This statement is not supported at this time");
}


/* For: $next, $continue, $refresh */
/* The following could be used for all the $... statements where one merely passes the command on to the debuggee */
/* We strip the comments and the ; but if there's junk after the first word it will cause an error, as it should. */
/* Todo: Should you add a semicolon? */
/* Todo: For some reason "$refresh variables;" returns a message with
   ok, 40 rows affected", I don't know where the 40 came from. */
void MainWindow::debug_other_go(QString text)
{
  char command_string[512];
  int index_of_number_1, index_of_number_2;
  QString q_schema_name, q_routine_name;

  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)er_strings[er_off + ER_NO_DEBUG_SESSION]) != 0) return;
  }
  if (debug_parse_statement(text, command_string, &index_of_number_1, &index_of_number_2) < 0)
  {
    if (debug_error((char*)er_strings[er_off + ER_OVERFLOW]) != 0) return;
    return;
  }
  q_schema_name= debug_q_schema_name_in_statement;
  q_routine_name= debug_q_routine_name_in_statement;
  if (debug_error((char*)"") != 0) return;
  if (debug_call_xxxmdbug_command(command_string) != 0) return;
  put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
}


/*
  Debug|Next
*/
void MainWindow::action_debug_next()
{
 log("action_debug_next", 90);
 statement_edit_widget->setPlainText("$NEXT");
 action_execute(1);
}


/*
  Debug|Skip
  Commented out -- not working at this time
*/
//void MainWindow::action_debug_skip()
//{
//  statement_edit_widget->setPlainText("$SKIP");
//  action_execute(1);
//}


/*
  Given 'information status' schema + routine name, find out what the tab number is and make it current.
  Then change the cursor to point to a particular line.
  After the cursor is set to the line, the CodeEditor's highlightCurrentLine wil highlight it.
  Remember, blockNumber() is base 0
  Todo: If the call was completed, you shouldn't be here -- the line number will be too big!
*/
void MainWindow::debug_highlight_line()
{
  QTextCursor cursor;
  int debug_widget_index;
  int new_line_number;
  QString debuggee_schema_identifier;
  QString debuggee_routine_identifier;

  for (debug_widget_index= 0; debug_widget_index < DEBUG_TAB_WIDGET_MAX; ++debug_widget_index)
  {
    debuggee_schema_identifier= debuggee_information_status_schema_identifier;
    debuggee_schema_identifier= connect_stripper(debuggee_schema_identifier, false);
    debuggee_routine_identifier= debuggee_information_status_routine_identifier;
    debuggee_routine_identifier= connect_stripper(debuggee_routine_identifier, false);
    if (QString::compare(debuggee_schema_identifier, debug_routine_schema_name[debug_widget_index]) == 0)
    {
      if (QString::compare(debuggee_routine_identifier, debug_routine_name[debug_widget_index]) == 0)
      {
        break; /* now we know that debug_widget_index is for the tab of the routine the debuggee's at */
      }
    }
  }

  if (debug_widget_index >= DEBUG_TAB_WIDGET_MAX)
  {
    /* I think it's impossible that routine won't be there, but if it isn't, do nothing. */
    /* Well, not quite impossible -- schema or routine might be "unknown" or "". That's probably harmless. */
    /* Todo: Change status line, on supposition that the routine has not started or has ended. */
      return;
  }

  debug_tab_widget->setCurrentWidget(debug_widget[debug_widget_index]);

  new_line_number= atoi(debuggee_information_status_line_number);

  --new_line_number;

  /*
    Something like QTextCursor::HighlightCurrentLine. Make the background light red.
    This cancels the "current line" background (which is light yellow), and can be cancelled by it -- I think that's okay.
  */

  /* If this is a different debug_widget than the last one, this should turn highlight off for the last one. */
  if (debug_widget_index != debug_timer_old_debug_widget_index)
  {
    if (debug_timer_old_debug_widget_index != -1)
    {
      if (debug_timer_old_debug_widget_index != -1)
      {
        QList<QTextEdit::ExtraSelection> old_extraSelections;
        debug_widget[debug_timer_old_debug_widget_index]->setExtraSelections(old_extraSelections);
      }
    }
    debug_timer_old_debug_widget_index= debug_widget_index;
  }

  QList<QTextEdit::ExtraSelection> extraSelections;

  QTextEdit::ExtraSelection selection;

  /* debug highlight color = current line color e.g. yellow, but faded */
  QColor lineColor= QColor(ocelot_statement_highlight_current_line_color).lighter(160);

  QTextDocument* doc= debug_widget[debug_widget_index]->document();
  QTextBlock block;
  /* new_line_number will be -2 if "debuggee_wait_loop ended". probably -1 is impossible */
  if (new_line_number == -2) block= doc->findBlockByNumber(doc->blockCount() - 1);
  else if (new_line_number < 0) block= doc->findBlockByNumber(0);
  else block= doc->findBlockByNumber(new_line_number);
  int pos= block.position();

  selection.format.setBackground(lineColor);
  selection.format.setProperty(QTextFormat::FullWidthSelection, true);
  selection.cursor= QTextCursor(doc);
  selection.cursor.setPosition(pos, QTextCursor::MoveAnchor);
  selection.cursor.setPosition(pos + 1, QTextCursor::KeepAnchor);
  selection.cursor.clearSelection();
  extraSelections.append(selection);
  debug_widget[debug_widget_index]->setExtraSelections(extraSelections);

  /*
    I don't understand why selection.cursor.setPosition(pos) does not
    really move the cursor, I think it's got do with whether the cursor
    is a copy. Anyway, without the following code, if the highlighted
    line moves off the screen, it won't be visible unless the user
    scrolls up or down.
    The user can move the cursor too, this only overrides temporarily.
  */
  QTextCursor c =  debug_widget[debug_widget_index]->textCursor();
  c.setPosition(pos);
  debug_widget[debug_widget_index]->setTextCursor(c);
  //debug_widget[debug_widget_index]->ensureCursorVisible(); /* I think we don't need this */
}


/*
  Debug|Delete
*/
//void MainWindow::action_debug_delete()
//{
//  printf("STUB: delete\n");
//}


/*
  Debug|Exit
  = "CALL xxxmdbug.command([channel-name],'exit');" as a client statement
  Check debuggee_state. If it's not DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP, debuggee_wait_loop() is not going on. THIS APPLIES FOR MANY COMMANDS!
  Send "exit" command, which should cause debuggee to exit debuggee_wait_loop() and return from the thread function.
  Wait up to 1 second for "exit" to work.
  If it didn't work, "kill debuggee-connection-id".
  TODO: If debuggee fails to respond, it's up to you to kill it.
  TODO: Call this when main window exits, otherwise the thread's connection is immortal.
  TODO: I wish there was some way to clear up init_connect now too.
  Todo: menu item to shut down debugger, and hide debug_tab_widget, close the connection, and kill the thread.
  Todo: when would you like to get rid of debug_widgets[]?
  We call debug_menu_enable_or_disable(TOKEN_KEYWORD_DEBUG_EXIT) after calling debug_exit_go().
*/
void MainWindow::action_debug_exit()
{
  log("action_debug_exit", 90);
  statement_edit_widget->setPlainText("$EXIT");
  action_execute(1);
}


/* flagger == 0 means this is a regular $exit; flagger == 1 means we're getting rid of stuff due to a severe severe error */
void MainWindow::debug_exit_go(int flagger)
{
  log("debug_exit_go", 90);
  char call_statement[512];

  if (flagger == 0)
  {
    /* Todo: merge this with debug_error somehow, and make sure nothing's enabled/disabled unless debug/exit succeeded! */
    if (menu_debug_action_exit->isEnabled() == false)
    {
      make_and_put_message_in_result(ER_DEBUG_NOT_DONE, 0, (char*)"");
      return;
    }
  }

  debug_top_widget->hide();
  debug_timer->stop();
  /* Todo: more cleanup e.g. kill the debuggee thread, close the connection, even if it's not responding */
  /* Todo: yet more cleanup if you want to get rid of the setup result */

  debug_delete_tab_widgets();

  if (debuggee_state == DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_call_xxxmdbug_command("exit") != 0)
    {
      put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
      return;
    }
    for (int kk= 0; kk < 10; ++kk) {QThread48::msleep(100); if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP) break; }
    if ((debuggee_state == DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP) && (debuggee_connection_id != 0))
    {
      sprintf(call_statement, "kill %d", debuggee_connection_id);

      if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
      {
        sprintf(call_statement, "SIGNAL SQLSTATE '05678' SET message_text='exit failed and kill failed -- try executing manually: kill %d'", debuggee_connection_id);
        lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement));
      }
    }
  }

  /* This should erase left-over messages in the pipe, such as the 'i status' message. Probably there's only one. */
  QString error_return;
  sprintf(call_statement, "%s%s%s","call xxxmdbug.dbms_pipe_receive('xxxmdbug_",debuggee_channel_name,
          "', 1, @xxxmdbug_tmp_1, @xxxmdbug_tmp_2)");
  for (int i= 0; i < 50; ++i)                                              /* 50 is an arbitrary big number */
  {
    if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement))) break;
    error_return= select_1_row("select @xxxmdbug_tmp_1");
    if (error_return != "") break;
    if (select_1_row_result_1 == "") break;
  }

  /* $install was saying "Suggested next step is: $EXIT" even though $EXIT was already done. */
  debuggee_state= DEBUGGEE_STATE_0;

  /*
     We call pthread_join so that memory is freed.
     But it might wait forever if debug thread is loopy.
     Perhaps we should kill the thread if we suspect that.
     Anyway: $exit is slow, and pthread_join is probably the cause.
  */
  if (debug_thread_exists == true)
  {
    int pthread_join_result= pthread_join(debug_thread_id, NULL);
    assert(pthread_join_result == 0);
    debug_thread_exists= false;
  }

  if (flagger == 0)
  {
    put_diagnostics_in_result(MYSQL_MAIN_CONNECTION); /* This should show the result of the final call or select, so it should be "ok" */
  }
}


/* This is called from $exit, but also might be called from $debug if $debug fails */
/* We're reversing the effects of "new CodeEditor() and "addTab()" in debug_debug_go(). */
void MainWindow::debug_delete_tab_widgets()
{
  debug_tab_widget->clear(); /* equivalent to removeTab() for all tab widgets */
  for (int debug_widget_index= 0; debug_widget_index < DEBUG_TAB_WIDGET_MAX; ++debug_widget_index)
  {
    if (debug_widget[debug_widget_index] != 0)
    {
      debug_widget[debug_widget_index]->removeEventFilter(this);
      delete debug_widget[debug_widget_index];
      debug_widget[debug_widget_index]= 0;
    }
  }
}


/*
  Debug|Information
*/
void MainWindow::action_debug_information()
{
  log("action_debug_information", 90);
  if (debug_call_xxxmdbug_command("information status") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.information_status;");
  action_execute(1);
}


/*
  Debug|Refresh server variables
*/
void MainWindow::action_debug_refresh_server_variables()
{
  log("action_debug_refresh_server_variables", 90);
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)er_strings[er_off + ER_NO_DEBUG_SESSION]) != 0) return;
  }
  if (debug_call_xxxmdbug_command("refresh server_variables") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.server_variables;");
  action_execute(1);
}


/*
  Debug|Refresh user variables
*/
void MainWindow::action_debug_refresh_user_variables()
{
  log("action_debug_refresh_user_variables", 90);
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)er_strings[er_off + ER_NO_DEBUG_SESSION]) != 0) return;
  }
  if (debug_call_xxxmdbug_command("refresh user_variables") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.user_variables;");
  action_execute(1);
}


/*
  Debug|Refresh variables
*/
void MainWindow::action_debug_refresh_variables()
{
  log("action_debug_refresh_variables", 90);
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)er_strings[er_off + ER_NO_DEBUG_SESSION]) != 0) return;
  }
  if (debug_call_xxxmdbug_command("refresh variables") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.variables;");
  action_execute(1);
}


/*
  Debug|Refresh call_stack
*/
void MainWindow::action_debug_refresh_call_stack()
{
  log("action_debug_refresh_call_stack", 90);
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)er_strings[er_off + ER_NO_DEBUG_SESSION]) != 0) return;
  }
  if (debug_call_xxxmdbug_command("refresh call_stack") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.call_stack;");
  action_execute(1);
}


/*
  Called from: action_debug_exit(), action_debug_debug(), action_debug_information(),
  and anything else which requires xxxmdbug.command -- except 'attach', except
  'information status' from from action_debug_timer_status().
  Make the SQL statement that is actually executed, but don't trouble the user with
  the details by showing it. For example, if the user said "$DEBUG test.p8", we
  execute "call xxxmdbug.command('channel#...','debug test.p8')" and return the
  result from that.
  Todo: unstripper might be okay but if the command has ''s within ''s
        then converting to 0x... and hex bytes would maybe be better
        (I decided against connect_unstripper since it's not a QString.)
*/
int MainWindow::debug_call_xxxmdbug_command(const char *command)
{
  char call_statement[512];

  char command_unstripped[512];
  QString s= command;
  s= connect_unstripper(s);
  strcpy(command_unstripped, s.toUtf8());

  strcpy(call_statement, "call xxxmdbug.command('");
  strcat(call_statement, debuggee_channel_name);
  strcat(call_statement, "', ");
  strcat(call_statement, command_unstripped);
  strcat(call_statement, ");\n");

  if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
  {
    /* Initially this can happen because we start the timer immediately after we call 'attach'. */
    put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
    return 1;
  }
  debug_statement= command;

  //put_diagnostics_in_result(MYSQL_MAIN_CONNECTION)???

  /* We no longer try to get the debuggee response with debug_information_status(). */
  //if (strcmp(command, "information status") != 0)
  //{
  //  QString debuggee_response= "Debuggee response = ";
  //  if (debug_information_status((char *)command) != 0)
  //    {
  //    debuggee_response.append("(none). Possibly debuggee is still executing the last command, which was: ");
  //    debuggee_response.append(debuggee_information_status_last_command);
  //  }
  //  else
  //  {
  //    debuggee_response.append(debuggee_information_status_last_command_result);
  //  }
  //  history_edit_widget->append(debuggee_response);
  //}
  return 0;
}


/*
  INFORMATION STATUS -- internal
  We should call this after 'attach' or after 'debug' to see whether there's been some success.
  Most contacts with the debugger should get a quick response, unless the last was 'continue' or 'step ...' (?)
  ... Actually, check that: maybe 'continue' acknowledges receipt before continuing.
  ... And I think that 'continue' will still cause messages, if it's inside stored-procedure code that's being debugged.
*/
int MainWindow::debug_information_status(char *last_command)
{
  int k;

  /* TODO: JUST GET RID OF THIS PROC. I NO LONGER SEE ANY POINT TO IT. */

  for (k= 0; k < 100; ++k)
  {
    /*
       By sleeping, we give the QTimer a chance to call action_debug_timer_status() and update status.
       But it doesn't work! I guess it's because sleeping doesn't restart the event loop!
    */
    QThread48::msleep(100);
    if (strcmp(debuggee_information_status_last_command, last_command) == 0)
    {
      /* todo: this is not enough! you should be checking also if the command number has gone up! */
      break;
    }
  }

  /* k >= 100 means the 100-iteration loop containing a 10-ms sleep has gone by without a match for last_commnd */
  /* the caller will probably generate a warning */
  if (k >= 100) return 1; else return 0;
}


/*
  QTimer *debug_timer: every 100 milliseconds (every 1/10 second), while the debuggee
  is running, the timer event will invoke action_debug_timer() which updates the status.
  Todo: Start it when debuggee starts, stop it when debuggee stops.
  If a new information status message has appeared, display it.
  Todo: make sure timer is stopped before debug_tab_window is closed.
  Todo: This won't necessarily catch errors, because of the 1/10 second intervals and
        because multiple commands can be sent asynchronously. Think of a way!
        Meanwhile, just say: "If you must know the result, you must wait after each command."
  Todo: It would be faster to interpret the 'information status' results in C code here,
        rather than using xxxmdbug.information_status repeatedly.
        Besides, selecting from xxxmdbug.information_status can change the state of the main mysql connection.
        Maybe you should create a different connection?
  Todo: Worry about race conditions. The main thread could look at a status variable before all
        the status variables are updaated by this function. This might be alleviated by
        updating the counter last, but some sort of mutex might have to be considered.
  Todo: Check whether something closes the connection.
        Maybe it would be safest to say that, once a connection is open, it stays open.
        Hmm, why is this a worry, since we're doing the searching on main connection not debuggee connection?
  Todo: I have seen the xxxmdbug.command() cause "send+receive error" meaning a change to init_connect failed.
        Maybe xxxmdbug.commnd() shouldn't try to change init_connect -- after all, we're not trying to send, only receive.
*/
void MainWindow::action_debug_timer_status()
{
  log("action_debug_timer_status", 20);
  char call_statement[512];
  MYSQL_RES *debug_res= NULL;
  MYSQL_ROW row= NULL;
  char unexpected_error[512];
  int unexpected_new_error= 0;
  /*
    DANGER
    When the main line is running SQL statements, it calls
    QApplication::processEvents(), so there can be horrible
    conflicts -- action_debug_timer_status() runs SQL too.
    Either we must disable the Run|Kill feature, or disable
    the timer. Here, we've chosen to disable the timer.
  */
  if (dbms_long_query_state == LONG_QUERY_STATE_STARTED)
  {
    log("action_debug_timer_status return", 20);
    return;
  }
  unexpected_error[0]= '\0';
  strcpy(call_statement, "call xxxmdbug.command('");
  strcat(call_statement, debuggee_channel_name);
  strcat(call_statement, "', 'information status')");

  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debuggee_state == DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR)
    {
      char tmp[256];
      int l= strlen(debuggee_state_error);
      if (l > 255) l= 255;
      memcpy(tmp, debuggee_state_error, l);
      tmp[l]= '\0';
      sprintf(unexpected_error, er_strings[er_off + ER_DEBUGGEE_WAIT_LOOP], tmp);
    }
    else
    {
      if ((debuggee_state < 0) || (debuggee_state == DEBUGGEE_STATE_END)) strcpy(unexpected_error, "debuggee_wait_loop ended");
      else strcpy(unexpected_error, er_strings[er_off + ER_DEBUGGEE_WAIT_LOOP_IS_NOT]);
    }
  }
  if (unexpected_error[0] == '\0')
  {
    if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
    {
      /* Initially this can happen because we start the timer immediately after we call 'attach'. */
      strcpy(unexpected_error, er_strings[er_off + ER_I_STATUS_FAILED]);
    }
  }
  if (unexpected_error[0]== '\0')
  {
    const char *query= "select * from xxxmdbug.information_status";
    if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], query, strlen(query)))
    {
      strcpy(unexpected_error, er_strings[er_off + ER_I_STATUS_FAILED_NOT_SEVERE]);
    }
  }
  if (unexpected_error[0] == '\0')
  {
    debug_res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
    if (debug_res == NULL)
    {
      strcpy(unexpected_error, er_strings[er_off + ER_MYSQL_STORE_RESULT_FAILED]);
    }
  }
  if (unexpected_error[0] == '\0')
  {
    row= lmysql->ldbms_mysql_fetch_row(debug_res);
    if (row == NULL)
    {
      strcpy(unexpected_error, er_strings[er_off + ER_MYSQL_FETCH_ROW_FAILED]);
    }
    else
    {
      if (lmysql->ldbms_mysql_num_fields(debug_res) < 14)
      {
        strcpy(unexpected_error, er_strings[er_off + ER_MYSQL_NUM_FIELDS]);
      }
      else
      {
        /* Just kludging -- I think something is overflowing. Todo: Find out what's really going on. */
        memset(debuggee_information_status_debugger_name, 0, 32);
        memset(debuggee_information_status_debugger_version, 0, 8);
        memset(debuggee_information_status_timestamp_of_status_message, 0, 32);
        memset(debuggee_information_status_number_of_status_message, 0, 8);
        memset(debuggee_information_status_icc_count, 0, 8);
        memset(debuggee_information_status_schema_identifier, 0, 256);
        memset(debuggee_information_status_routine_identifier, 0, 256);
        memset(debuggee_information_status_line_number, 0, 8);
        memset(debuggee_information_status_is_at_breakpoint, 0, 8);
        memset(debuggee_information_status_is_at_tbreakpoint, 0, 8);
        memset(debuggee_information_status_is_at_routine_exit, 0, 8);
        memset(debuggee_information_status_stack_depth, 0, 8);
        memset(debuggee_information_status_last_command, 0, 256);
        memset(debuggee_information_status_last_command_result, 0, 256);
        memset(debuggee_information_status_commands_count, 0, 8);

        if (row[0] != NULL) strncpy(debuggee_information_status_debugger_name, row[0], 32 - 1);
        if (row[1] != NULL) strncpy(debuggee_information_status_debugger_version, row[1], 8 - 1);
        if (row[2] != NULL) strncpy(debuggee_information_status_timestamp_of_status_message, row[2], 32 - 1);
        if (row[3] != NULL) strncpy(debuggee_information_status_number_of_status_message, row[3], 8 - 1);
        if (row[4] != NULL) strncpy(debuggee_information_status_icc_count, row[4], 8 - 1);
        if (row[5] != NULL) strncpy(debuggee_information_status_schema_identifier, row[5], 256 - 1);
        if (row[6] != NULL) strncpy(debuggee_information_status_routine_identifier, row[6], 256 - 1);
        if (row[7] != NULL) strncpy(debuggee_information_status_line_number, row[7], 8 - 1);
        if (row[8] != NULL) strncpy(debuggee_information_status_is_at_breakpoint, row[8], 8 - 1);
        if (row[9] != NULL) strncpy(debuggee_information_status_is_at_tbreakpoint, row[9], 8 - 1);
        if (row[10] != NULL) strncpy(debuggee_information_status_is_at_routine_exit, row[10], 8 - 1);
        if (row[11] != NULL) strncpy(debuggee_information_status_stack_depth, row[11], 8 - 1);
        if (row[12] != NULL) strncpy(debuggee_information_status_last_command, row[12], 256 - 1);
        if (row[13] != NULL) strncpy(debuggee_information_status_last_command_result, row[13], 256 - 1);
        if (row[14] != NULL) strncpy(debuggee_information_status_commands_count, row[14], 8 - 1);
      }
    }
  }
  if (debug_res != NULL) lmysql->ldbms_mysql_free_result(debug_res);
  if (unexpected_error[0] != '\0')
  {
    if (strcmp(debuggee_information_status_last_command_result, unexpected_error) != 0)
    {
      /* This should be considered as a status change, as if it's a new i status message */
      unexpected_new_error= 1;
      int l= strlen(unexpected_error);
      if (l > 255) l= 255;
      memcpy(debuggee_information_status_last_command_result, unexpected_error, l);
      debuggee_information_status_last_command_result[l]= '\0';
    }
  }
    //printf("debuggee_information_status_debugger_name=%s.\n", debuggee_information_status_debugger_name);
    //printf("debuggee_information_status_debugger_version=%s.\n", debuggee_information_status_debugger_version);
    //printf("debuggee_information_status_timestamp_of_status_message=%s.\n", debuggee_information_status_timestamp_of_status_message);
    //printf("debuggee_information_status_number_of_status_message=%s.\n", debuggee_information_status_number_of_status_message);
    //printf("debuggee_information_status_icc_count=%s.\n", debuggee_information_status_icc_count);
    //printf("debuggee_information_status_schema_identifier=%s.\n", debuggee_information_status_schema_identifier);
    //printf("debuggee_information_status_routine_identifier=%s.\n", debuggee_information_status_routine_identifier);
    //printf("debuggee_information_status_line_number=%s.\n", debuggee_information_status_line_number);
    //printf("debuggee_information_status_is_at_breakpoint=%s.\n", debuggee_information_status_is_at_breakpoint);
    //printf("debuggee_information_status_is_at_tbreakpoint=%s.\n", debuggee_information_status_is_at_tbreakpoint);
    //printf("debuggee_information_status_is_at_routine_exit=%s.\n", debuggee_information_status_is_at_routine_exit);
    //printf("debuggee_information_status_stack_depth=%s.\n", debuggee_information_status_stack_depth);
    //printf("debuggee_information_status_last_command=%s.\n", debuggee_information_status_last_command);
    //printf("debuggee_information_status_last_command_result=%s.\n", debuggee_information_status_last_command_result);
    //printf("debuggee_information_status_commands_count=%s.\n", debuggee_information_status_commands_count);

  /* If a status message would probably confuse a rational user, change it. */
    if (strstr(debuggee_information_status_last_command_result, "completed CALL") != NULL)
      strcpy(debuggee_information_status_last_command_result, "Routine has stopped. Suggested next step is: $EXIT");

    if (strstr(debuggee_information_status_last_command_result, "Failed, the expected command is debug") != NULL)
      strcpy(debuggee_information_status_last_command_result, "Routine has stopped and continuing past the end is not possible.  Suggested next step is: $EXIT");

  /*
    When execution ends, we still have line number = 0 and "stopped at breakpoint".
    That's misleading, but rather than change MDBug, let's override what it returns.
    Also: last command isn't always updated if execution has ended.
  */
  if (strcmp(debuggee_information_status_last_command_result, "debuggee_wait_loop ended") == 0)
  {
    strcpy(debuggee_information_status_line_number, "-1");
    strcpy(debuggee_information_status_is_at_breakpoint, "no");
    strcpy(debuggee_information_status_is_at_routine_exit, "yes");
    strcpy(debuggee_information_status_last_command, debug_statement.toUtf8());
    /* todo: what about debuggee_information_status_last_command_result? */
  }
  /*
    Change line_edit i.e. status widget, and maybe change highlight, if there's a new message.
    It would be extremely wasteful to update every time, i.e. every 1/10 second, if there's no change.
    Apparently the change that you can depend on is icc_count, not commands_count or number_of_status_message.
    I don't think there's any need to check whether line number + routine name have changed, but see below.
  */
  if ((debug_timer_old_commands_count != atoi(debuggee_information_status_commands_count))
  ||  (debug_timer_old_number_of_status_message != atoi(debuggee_information_status_last_command))
  ||  (debug_timer_old_icc_count != atoi(debuggee_information_status_icc_count))
  ||  (debug_timer_old_line_number != atoi(debuggee_information_status_line_number))
  ||  (unexpected_new_error == 1))
  {
    char result[2048];
    strcpy(result, "Debugger status = ");
    if (strcmp(debuggee_information_status_schema_identifier, "unknown") != 0)
    {
      strcat(result, "(Current position: ");
      strcat(result, debuggee_information_status_schema_identifier);
      strcat(result, ".");
      strcat(result, debuggee_information_status_routine_identifier);
      strcat(result, " line ");
      strcat(result, debuggee_information_status_line_number);
      strcat(result, ")");
    }
    strcat(result, "(Last debug command: ");
    strcat(result, debuggee_information_status_last_command);
    strcat(result, ", result: ");
    strcat(result, debuggee_information_status_last_command_result);
    strcat(result, ")");

    if (strcmp(debuggee_information_status_is_at_breakpoint, "yes") == 0)
    {
      strcat(result, "(STOPPED AT BREAKPOINT)");
    }

    if (strcmp(debuggee_information_status_is_at_routine_exit, "yes") == 0)
    {
      strcat(result, "(STOPPED AT ROUTINE EXIT)");
    }
    debug_line_widget->setText(result);
    /*
      If and only if the line number or routine name or schema name has changed,
      highlight i.e. change background color in order to show where we're at now.
      Formerly this checked "if (strcmp(debuggee_information_status_is_at_breakpoint, "yes") == 0)"
      i.e. are we at a breakpoint, but I no longer see why that matters.
      Besides, '$breakpoint' does not move the position.
    */
    if ((debug_timer_old_line_number != atoi(debuggee_information_status_line_number))
     || (strcmp(debug_timer_old_schema_identifier, debuggee_information_status_schema_identifier) == 0)
     || (strcmp(debug_timer_old_routine_identifier, debuggee_information_status_routine_identifier) == 0))
    {
      debug_highlight_line();
      debug_timer_old_line_number= atoi(debuggee_information_status_line_number);
      strcpy(debug_timer_old_schema_identifier, debuggee_information_status_schema_identifier);
      strcpy(debug_timer_old_routine_identifier, debuggee_information_status_routine_identifier);
    }
    debug_timer_old_commands_count= atoi(debuggee_information_status_commands_count);
    debug_timer_old_number_of_status_message= atoi(debuggee_information_status_last_command);
    debug_timer_old_icc_count= atoi(debuggee_information_status_icc_count);
  }
  log("action_debug_timer_status end", 20);
}
#endif


/*
  For menu item "Execute|Kill" we must start another thread,
  which connects and issues a Kill query-id.
  The kill thread has a subset of the actions of the debug thread so see also debug_thread.
  Possible variant: keep it alive and use it for monitoring.
*/
void* kill_thread(void* unused)
{
  (void) unused; /* suppress "unused parameter" warning */
  char call_statement[512];
  int is_connected;

  is_connected= 0;
  kill_state= KILL_STATE_CONNECT_THREAD_STARTED;
  for (;;)
  {
    if (options_and_connect(MYSQL_KILL_CONNECTION, ocelot_database_as_utf8))
    {
      kill_state= KILL_STATE_CONNECT_FAILED;
      break;
    }
    is_connected= 1;
    kill_state= KILL_STATE_IS_CONNECTED;
    sprintf(call_statement, "kill query %d", kill_connection_id);
    if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_KILL_CONNECTION], call_statement, strlen(call_statement)))
    {
      kill_state= KILL_STATE_MYSQL_REAL_QUERY_ERROR;
      break;
    }
    kill_state= KILL_STATE_ENDED;
    break;
  }
  if (is_connected == 1) lmysql->ldbms_mysql_close(&mysql[MYSQL_KILL_CONNECTION]);
  lmysql->ldbms_mysql_thread_end();
  return ((void*) NULL);
}

bool is_kill_requested;

/*
  Todo: This is no good if the running query isn't
  on the main connection.
*/
void MainWindow::action_kill()
{
  pthread_t thread_id;

  if (connections_is_connected[0] != 1) return; /* impossible */
  is_kill_requested= true;
  if (dbms_long_query_state == LONG_QUERY_STATE_STARTED)
  {
    kill_connection_id= statement_edit_widget->dbms_connection_id;
    pthread_create(&thread_id, NULL, &kill_thread, NULL);
    /* no pthread_join here -- what would be the point of waiting? */
  }
}

/*
  Implementing ^C i.e. control-C i.e. menu item Run|Kill ...
  Once an SQL statement or SQL statement series has been accepted, user input
  is restricted to "kill" (a few other events are tolerated but not ones that cause statements),
  so around the loop inside action_execute() there is some enabling and disabling.
  SQL statements are done in a separate thread, dbms_long_query();
  in the main thread there is a wait-loop which stops when the thread ends;
  the wait-loop includes QApplication::ProcessEvents() so a "Kill" will be seen.
  "Kill" causes the SQL statement to end (if there's one running).
  ^C is an appropriate shortcut although it usually means "clip".
  Kill sets a flag so that if there are multiple statements, they are all aborted.
  dbms_long_query is only for potentially-slow SQL statements that might need to be killed.
  Todo: this could be done via a permanently-existing thread or pool of threads.
  Todo: QThread is more portable than pthread, although it looks harder to understand.
  Todo: put this together with the dbms_* routines in ocelotgui.h in a separate class.
  Normally dbms_query_connection_number is for the main connection.
*/
void* dbms_long_query_thread(void* unused)
{
  (void) unused; /* suppress "unused parameter" warning */

  dbms_long_query_result= lmysql->ldbms_mysql_real_query(&mysql[dbms_query_connection_number], dbms_query, dbms_query_len);
  dbms_long_query_state= LONG_QUERY_STATE_ENDED;
  return ((void*) NULL);
}

/* Todo: This assumes connection_number must be MYSQL_MAIN_CONNECTION */
void* dbms_long_next_result_thread(void* unused)
{
  (void) unused; /* suppress "unused parameter" warning */

  dbms_long_query_result= lmysql->ldbms_mysql_next_result(&mysql[MYSQL_MAIN_CONNECTION]);
  dbms_long_query_state= LONG_QUERY_STATE_ENDED;
  return ((void*) NULL);
}

void MainWindow::action_execute_force()
{
  action_execute(1);
}

/*
  For menu item "execute" we said (...SLOT(action_execute(1))));
  By default this is on and associated with File|Execute menu item.
  Execute what's in the statement widget.
  The statement widget might contain multiple statements.
  Return: 0 = ok, 1 = syntax checker failed with "No"; 2 = DBMS error
*/
int MainWindow::action_execute(int force)
{
  QString text;
  int return_value= 0;
  log("action_execute start", 90);
  for (;;)
  {
    int returned_begin_count;
    main_token_number= 0;
    text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */

    main_token_count_in_statement= get_next_statement_in_string(main_token_number,
                                                                &returned_begin_count,
                                                                true);
    if (main_token_count_in_statement == 0) break;
    /* If the next statement is unfinished, we usually want to ignore it. */
    if ((force == 0) && (is_statement_complete(text) == false)) return 1;

    /*
      We probably call hparse_f_multi_block continuously during statement edit,
      but this redundancy is harmless (we're not short of time, eh?).
      This checks for errors in the whole string, not just the first statement.
    */
    if (((ocelot_statement_syntax_checker.toInt()) & FLAG_FOR_ERRORS) != 0)
    {
      hparse_f_multi_block(text);
      /* TODO: QMessageBox jiggle, it displays then moves to centre */
      if ((hparse_errno != 0) || (hparse_errno_count != 0))
      {
        QString s;
        QMessageBox *msgbox= new QMessageBox(this);
        s= er_strings[er_off + ER_THE_SYNTAX_CHECKER_THINKS];
        s.append(hparse_errmsg);
        msgbox->setText(s);
        msgbox->setInformativeText(er_strings[er_off + ER_DO_YOU_WANT_TO_CONTINUE]);
        QPushButton *yes_button= msgbox->addButton(er_strings[er_off + ER_YES], QMessageBox::YesRole);
        QPushButton *no_button= msgbox->addButton(er_strings[er_off + ER_NO], QMessageBox::NoRole);
        msgbox->setDefaultButton(yes_button);
        msgbox->exec();
        if (msgbox->clickedButton() == no_button)
        {
          delete msgbox;
          return 1;
        }
        delete msgbox;
      }
      log("FLAG_FOR_ERRORS seen. end of if", 90);
    }

    /* While executing, we allow no more statements, but a few things are enabled. */
    /* This makes the menu seem to blink. If that's not OK, turn off sub-items not main menu items. */
    menu_file->setEnabled(false);
    menu_edit->setEnabled(false);
    menu_run_action_execute->setEnabled(false);
    if (ocelot_sigint_ignore == 0) menu_run_action_kill->setEnabled(true);
    menu_settings->setEnabled(false);
    menu_debug->setEnabled(false);
    menu_help->setEnabled(false);
    statement_edit_widget->setReadOnly(true);
    is_kill_requested= false;
    return_value= action_execute_one_statement(text);
    menu_file->setEnabled(true);
    menu_edit->setEnabled(true);
    menu_run_action_execute->setEnabled(true);
    menu_run_action_kill->setEnabled(false);
    menu_settings->setEnabled(true);
    menu_debug->setEnabled(true);
    menu_help->setEnabled(true);
    statement_edit_widget->setReadOnly(false);
    /*
      In remove_statement there is a removeSelectedText() call, which
      causes action_statement_edit_widget_text_changed() -- twice.
      We only want it to happen once, so we'll control it ourselves.
      Importantly, it will call tokenize() and hparse_f_multi_block()
      so main_token_... variables will all change.
      Todo: check again, I think the above statement might be false.
    */
    log("before remove_statement", 90);
    statement_edit_widget_text_changed_flag= 1;
    remove_statement(text);
    statement_edit_widget_text_changed_flag= 0;
    log("after remove_statement", 90);
    action_statement_edit_widget_text_changed(0, 0, 0);
    //widget_sizer();
    /* Try to set history cursor at end so last line is visible. Todo: Make sure this is the right time to do it. */
    history_edit_widget->verticalScrollBar()->setValue(history_edit_widget->verticalScrollBar()->maximum());
    history_edit_widget->show(); /* Todo: find out if this is really necessary */
    if (is_kill_requested == true) break;
  }
  log("action_execute end", 90);
  if (return_value != 0)
  {
    return 2;
  }
  return 0;
}

/*
  Remove the first statement in statement_edit_widget,
  including any whitespace after the statement end.
  This is equivalent to statement_edit_widget->clear() if there is only one statement.
  But when a user has cut-and-pasted several statements, we execute one at a time.
  There are other ways to do this e.g. clear() followed by insertText().
  Unanswered question: do we want to clear undo/redo history?
  Unanswered question: do we want to clear selection?
  Unanswered question: do we want to move the visible cursor?
*/
void MainWindow::remove_statement(QString text)
{
  //statement_edit_widget->clear(); /* ?? this is supposed to be a slot. does that matter? */

  QTextCursor q= statement_edit_widget->textCursor();

  int offset_of_statement_end= 0;

  if (main_token_count_in_statement > 0)
  {
    offset_of_statement_end= main_token_offsets[main_token_count_in_statement - 1]
                             + main_token_lengths[main_token_count_in_statement - 1];
  }

  for (;;)
  {
    QString c= text.mid(offset_of_statement_end, 1);
    if (c == "") break;
    if (c > " ") break;\
    ++offset_of_statement_end;
  }

  q.setPosition(0, QTextCursor::MoveAnchor);
  q.setPosition(offset_of_statement_end, QTextCursor::KeepAnchor);
  q.removeSelectedText();


  //q.select(QTextCursor::LineUnderCursor);

  //statement_edit_widget->textCursor().insertText(text);
}

/*
  Todo: There are a few things yet to be considered.
     I'm not 100% confident about the way I'm clearing the old results out, e.g. "delete result_form_text[...]".
     Would it be more efficient to remove only the ones between new column count and old column count?
       No, because data type can change, it might not be a text field the next time around.
     Should I be saying "delete later" somehow?
     Does memory leak?
*/
int MainWindow::action_execute_one_statement(QString text)
{
  log("action_execute_one_statement", 80);
  //QString text;
  MYSQL_RES *mysql_res_for_new_result_set= NULL;
  unsigned short int is_vertical= ocelot_result_grid_vertical; /* true if --vertical or \G or ego */
  unsigned return_value= 0;
  ++statement_edit_widget->statement_count;
  /*
    Todo: There should be an indication that something is being executed.
    Possibly, on the title bar, setPlainText(tr("Executing ...")
    Possibly there should be a status widget or a progress bar.
    Whatever it is, turn it on now and turn it off when statement is done.
    Update: the Run|Execute menu item changes, which is slightly helpful.
  */

  /* Apparently there is no need for a call to tokenize() here, it seems certain that it's already been called. */

  int query_utf16_len= main_token_offsets[main_token_number+main_token_count_in_statement - 1]
                       + main_token_lengths[main_token_number+main_token_count_in_statement - 1]
                       - main_token_offsets[main_token_number];

  query_utf16= text.mid(main_token_offsets[main_token_number], query_utf16_len);

  QString last_token;
  bool strip_last_token= false;
  int length_of_last_token_in_statement= main_token_lengths[main_token_number + main_token_count_in_statement - 1];
  int type_of_last_token_in_statement= main_token_types[main_token_number + main_token_count_in_statement - 1];
  last_token= text.mid(main_token_offsets[main_token_number+main_token_count_in_statement - 1],
                       length_of_last_token_in_statement);
  if ((length_of_last_token_in_statement == 1) && ((type_of_last_token_in_statement == TOKEN_KEYWORD_EGO) || (type_of_last_token_in_statement == TOKEN_KEYWORD_GO)))
  {
    length_of_last_token_in_statement= 2;
    strip_last_token= true;
    if (type_of_last_token_in_statement == TOKEN_KEYWORD_EGO) is_vertical= 1;
  }

  /* Strip last word if it's delimiter or (when --named-commands, not only token) go|ego. */
  /* todo: this is obsolete now */
  if (last_token == ocelot_delimiter_str) strip_last_token= true;
  else if ((ocelot_named_commands > 0) && (main_token_count_in_statement > 1))
  {
    if (type_of_last_token_in_statement == TOKEN_KEYWORD_GO) strip_last_token= true;
    if (type_of_last_token_in_statement == TOKEN_KEYWORD_EGO)
    {
      strip_last_token= true;
      is_vertical= 1;
    }
  }
  if (strip_last_token == true)
  {
    query_utf16_copy= text.mid(main_token_offsets[main_token_number], query_utf16_len-length_of_last_token_in_statement);
  }
  else query_utf16_copy= query_utf16;

  statement_edit_widget->start_time= QDateTime::currentMSecsSinceEpoch(); /* will be used for elapsed-time display */
  int additional_result= 0;
  int ecs= execute_client_statement(text, &additional_result);
  QString result_set_for_history= "";

  if (ecs != 1)
  {
    /* The statement was not handled entirely by the client, it must be passed to the DBMS. */

    bool do_something= true;

    /* If DBMS is not (yet) connected, except for certain SET ocelot_... statements, this is an error. */
    if (connections_is_connected[0] == 0)
    {
      if (ecs == 2) make_and_put_message_in_result(ER_OK, 0, (char*)"");
      else make_and_put_message_in_result(ER_NOT_CONNECTED, 0, (char*)"");
      do_something= false;
    }
    /* If --one-database, and USE caused default database to change, error */
    if ((ocelot_one_database > 0) && (ocelot_database != statement_edit_widget->dbms_database))
    {
      make_and_put_message_in_result(ER_ONE_DATABASE, 0, (char*)"");
      do_something= false;
    }

    if (do_something == true)
    {
      /* Look for a CREATE TABLE statement with a SERVER clause. */
      /* Todo: Make sure you only execute what's in utf16_copy */
      bool is_create_table_server;
#ifdef DBMS_TARANTOOL
      int result= create_table_server(text, &is_create_table_server, main_token_number, main_token_count_in_statement);
      dbms_long_query_result= result;
#else
      dbms_long_query_result= 0;
      is_create_table_server= false;
#endif
      if (is_create_table_server == false)
        execute_real_query(query_utf16, MYSQL_MAIN_CONNECTION);
      if (dbms_long_query_result)
      {
        /* beep() hasn't been tested because getting sound to work on my computer is so hard */
        if (ocelot_no_beep == 0) QApplication::beep();
        return_value= 1;
        if (is_create_table_server == false)
          put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
      }
      else {
        /*
          It was a successful SQL statement, and now look if it returned a result.
          If it did, as determined by looking at the mysql_res that lmysql->ldbms_mysql_store_result() returns,
          then free the previous mysql_res and delete the previous result grid,
          before setting up new ones.
          This means that statements which don't return result sets don't cause clearing
          of old result sets from the screen, e.g. SET has no effect on result grid.
          This means that momentarily there will be an old mysql_res and a new mysql_res.
          That takes up memory. If it were easy to check in advance whether a statement
          caused a result set (e.g. with mysql_next_result or by looking at whether the
          statement is SELECT SHOW etc.), that would be better.
        */
#ifdef DBMS_TARANTOOL
        if (connections_dbms[0] == DBMS_TARANTOOL)
        {
          mysql_res_for_new_result_set= 0;
//          unsigned int main_token_flags_first;
//          int statement_type= get_statement_type(main_token_number, main_token_count_in_statement, &main_token_flags_first);
//          if ((statement_type == TOKEN_KEYWORD_SELECT)
//           || (statement_type == TOKEN_KEYWORD_VALUES)
//           || (statement_type == TOKEN_KEYWORD_LUA)
//           || ((main_token_flags_first & TOKEN_FLAG_IS_LUA) != 0))
          {
            int result_set_type;
            tarantool_result_set_init(0,&tarantool_row_count[MYSQL_MAIN_CONNECTION],&result_set_type);
            if ((result_set_type == 0) || (result_set_type == 1))
              mysql_res_for_new_result_set= 0;
            else
            {
              /* Yes mysql_res_for_new_result_set will be invalid. Fix, eh? */
              {
                MYSQL_RES r;
                mysql_res_for_new_result_set= &r;
              }
            }
          }
        }
        if (connections_dbms[0] != DBMS_TARANTOOL)
#endif
        mysql_res_for_new_result_set= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
        if (mysql_res_for_new_result_set == 0)
        {
          /*
            Last statement did not cause a result set. We could hide the grid and shrink the
            central window with "result_grid_tab_widget[0]->hide()", but we don't.
          */
          get_sql_mode(main_token_types[main_token_number], text, false, main_token_number);
          put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
        }
        else
        {
#ifdef DBMS_TARANTOOL
          if (connections_dbms[0] == DBMS_TARANTOOL)
          {
            /* TODO: Free the last reply, equivalent to mysql_free_result */
          }
          else
#endif
          {
          if (mysql_res != 0)
            {
              lmysql->ldbms_mysql_free_result(mysql_res); /* This is the place we free if myql_more_results wasn't true, see below. */
            }

            mysql_res= mysql_res_for_new_result_set;

            /* We need to think better what to do if we exceed MAX_COLUMNS */
            /* ... or maybe not, it seems we got rid of MAX_COLUMNS */
            /*
              Todo: consider whether it would be appropriate to set grid width with
              result_grid_tab_widget[0]->result_column_count= lmysql->ldbms_mysql_num_fields(mysql_res);
              but it may be unnecessary, and may cause a crash in garbage_collect()
            */

            result_row_count= lmysql->ldbms_mysql_num_rows(mysql_res);                /* this will be the height of the grid */
          }
          ResultGrid *rg;
          {
            for (int i_r= 0; i_r < ocelot_grid_actual_tabs; ++i_r)
            {
              rg= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(i_r));
              rg->is_paintable= 0;
              rg->garbage_collect();
            }
            rg= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(0));
            //QFont tmp_font;
            //tmp_font= rg->font();
            QString fillup_result= rg->fillup(mysql_res,
                      //&tarantool_tnt_reply,
                      connections_dbms[0],
                      //this,
                      is_vertical, ocelot_result_grid_column_names,
                      lmysql, ocelot_client_side_functions,
                      ocelot_batch, ocelot_html, ocelot_raw, ocelot_xml,
                      MYSQL_MAIN_CONNECTION,
                      true);
            if (fillup_result != "OK")
            {
              /* fillup() failure is unexpected so this is crude */
              put_message_in_result(fillup_result);
              return_value= 1;
              goto statement_is_over;
            }
            result_grid_tab_widget->setCurrentWidget(rg);
            result_grid_tab_widget->tabBar()->hide();
            /* next line redundant? display() ends with show() */
            rg->show();
            result_grid_tab_widget->show(); /* Maybe this only has to happen once */
          }

          /*
            Following is no-op by default because ocelot_history_max_row_count=0
          */
          log("copy_to_history (before)", 80);
          if ((ocelot_grid_actual_tabs > 0)
           && (result_set_for_history == ""))
          {
            rg= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(0));
            result_set_for_history= rg->copy_to_history(ocelot_history_max_row_count.toLong(), is_vertical, connections_dbms[0]);
          }
          log("copy_to_history (after)", 80);
          /* Todo: small bug: elapsed_time calculation happens before lmysql->ldbms_mysql_next_result(). */
          /* You must call lmysql->ldbms_mysql_next_result() + lmysql->ldbms_mysql_free_result() if there are multiple sets */
          put_diagnostics_in_result(MYSQL_MAIN_CONNECTION); /* Do this while we still have number of rows */
          //history_markup_append(result_set_for_history, true);

#ifdef DBMS_TARANTOOL
          if ((connections_dbms[0] != DBMS_TARANTOOL)
           && (lmysql->ldbms_mysql_more_results(&mysql[MYSQL_MAIN_CONNECTION])))

#else
          if (lmysql->ldbms_mysql_more_results(&mysql[MYSQL_MAIN_CONNECTION]))
#endif
          {
            lmysql->ldbms_mysql_free_result(mysql_res);
            /*
              We started with CLIENT_MULTI_RESULT flag (not CLIENT_MULTI_STATEMENT).
              We expect that a CALL to a stored procedure might return multiple result sets
              plus a status result at the end. The following lines try to pick up and display
              the extra result sets, up to a fixed maximum, and just throw away everything
              after that, to avoid the dreaded out-of-sync error message.
              If it's an ordinary select, lmysql->ldbms_mysql_free_result(mysql_res) happens later, see above.
            */
            log("sleep loop start", 90);
            int result_grid_table_widget_index= 1;
            for (;;)
            {
              dbms_long_query_state= LONG_QUERY_STATE_STARTED;
              pthread_t thread_id;
              pthread_create(&thread_id, NULL, &dbms_long_next_result_thread, NULL);

              for (;;)
              {
                QThread48::msleep(10);
                if (dbms_long_query_state == LONG_QUERY_STATE_ENDED) break;
                QApplication::processEvents();
              }
              pthread_join(thread_id, NULL);

              if (dbms_long_query_result != 0)
              {
                return_value= 1;
                /* If mysql_next_result) == 1 means error, -1 means no more. */
                /* TODO: don't do this for SOURCE. Check for beep. */
                if (dbms_long_query_result == 1)
                {
                  put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
                  //history_markup_append("", true);
                }
                break;
              }
              /* todo: consider appending row count to result message */
              mysql_res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);

              /* I think the following will help us avoid the "status" return. */
              if (mysql_res == NULL) continue;
              if (result_grid_table_widget_index == ocelot_grid_actual_tabs)
              {
                result_grid_add_tab();
              }

              if (result_grid_table_widget_index < ocelot_grid_actual_tabs)
              {
                ResultGrid* r;
                r= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(result_grid_table_widget_index));
                result_grid_tab_widget->tabBar()->show(); /* is this in the wrong place? */
                result_row_count= lmysql->ldbms_mysql_num_rows(mysql_res);                /* this will be the height of the grid */
                r->fillup(mysql_res,
                          //&tarantool_tnt_reply,
                          connections_dbms[0],
                          //this,
                          is_vertical,
                          ocelot_result_grid_column_names,
                          lmysql,
                          ocelot_client_side_functions,
                          ocelot_batch, ocelot_html, ocelot_raw, ocelot_xml,
                          MYSQL_MAIN_CONNECTION,
                          true);
                /* next line redundant? display() ends with show() */
                r->show();

                //Put in something based on this if you want extra results to go to history:
                //... result_grid_table_widget[result_grid_tab_widget_index]->copy_to_history(); etc.

                ++result_grid_table_widget_index;
              }

              if (mysql_res != 0) lmysql->ldbms_mysql_free_result(mysql_res);
            }
            log("sleep loop end", 90);
            mysql_res= 0;
          }
        }
      }
      //put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
    }
  }
statement_is_over:
  /* statement is over */
  log("action_execute_one_statement end", 80);
  if (additional_result != TOKEN_KEYWORD_SOURCE)
  {
    history_markup_append(result_set_for_history, true); /* add prompt+statement+result to history, with markup */
  }
  return return_value;
}

/*
  Bottom-level call to DBMS.
  We call this when we know that there will be no more change
  to the query (no local-subquery substitution, but maybe
  stripping of comments and delimiters other than ;).
  During this query we allocate or use globals:
  dbms_query, dbms_query_len, dbms_query_connection.
  The query might go to either the main or the local
  connection, the local connection might be either
  mysql or tarantool.
  Todo: Tarantool can't be killed so easily. Make a fiber?
*/
int MainWindow::execute_real_query(QString query, int connection_number)
{
  /*
    If the last error was CR_SERVER_LOST 2013 or CR_SERVER_GONE_ERROR 2006,
    and it might be possible to reconnect, try.
    But if mysql_ping() fails, I don't see much that we can do.
  */
  if ((mysql_errno_result == 2006) || (mysql_errno_result == 2013))
  {
    if (ocelot_opt_reconnect > 0) lmysql->ldbms_mysql_ping(&mysql[MYSQL_MAIN_CONNECTION]);
  }
  /* todo: figure out why you used global dbms_query for this */
  /* TODO: BUG. This statement caused a crash when ocelot_comments == 0:
         create procedure p27 ()
           begin
           declare xxx int;
           declare yyy int;
           set xxx = 0;
           while xxx < 1000 do
             set xxx = xxx;
             set xxx = xxx + 1;
             end while;
           end
      We work around it by allocating double what we need for dbms_query. */
  dbms_query_len= query.toUtf8().size();           /* See comment "UTF8 Conversion" */
  dbms_query= new char[(dbms_query_len + 1) * 2];
  dbms_query_len= make_statement_ready_to_send(query,
                                               dbms_query,
                                               dbms_query_len + 1,
                                               connection_number);
  assert(strlen(dbms_query) < ((unsigned int) dbms_query_len + 1) * 2);
  dbms_query_connection_number= connection_number;
#ifdef DBMS_TARANTOOL
  /* todo: for tarantool as for mysql, call with a separate thread so it's killable */
  if (connections_dbms[connection_number] == DBMS_TARANTOOL)
  {
    dbms_long_query_result= tarantool_real_query(dbms_query, dbms_query_len, MYSQL_MAIN_CONNECTION, main_token_number, main_token_count_in_statement);
    dbms_long_query_state= LONG_QUERY_STATE_ENDED;
  }
  else
#endif
  {
    pthread_t thread_id;
    dbms_long_query_state= LONG_QUERY_STATE_STARTED;
    pthread_create(&thread_id, NULL, &dbms_long_query_thread, NULL);
    for (;;)
    {
      QThread48::msleep(10);
      if (dbms_long_query_state == LONG_QUERY_STATE_ENDED) break;
      QApplication::processEvents();
    }
    pthread_join(thread_id, NULL);
    //     dbms_long_query_result= lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], dbms_query, dbms_query_len);
    //     dbms_long_query_state= LONG_QUERY_STATE_ENDED;
  }
  delete []dbms_query;
  return dbms_long_query_result;
}

/*
  We see "DELIMITER".
  If the thing that follows is 'literal' or "literal" or `identifier`: that's the delimiter.
  Otherwise delimiter is what follows as far as next whitespace or eof or ;
  DELIMITER ; means "go back to the default i.e. ;".
  This is called from hparse_f_client_statement() because it affects parse of later statements.
  This is called from execute_client_statement() because DELIMITER is a client statement.
  One difference from mysql client:
    If mysql client sees "DELIMITER <return>" or "DELIMITER ''" it's an error.
    But we consider that equivalent to "DELIMITER ;"
  We return ";" if nothing follows or quoted blank string follows; mysql client would say error.
*/
QString MainWindow::get_delimiter(QString token, QString text, int offset)
{
  QString token_to_return;
  if ((token.mid(0, 1) == "`") || (token.mid(0, 1) == "'") || (token.mid(0, 1) == "\""))
  {
    token_to_return= connect_stripper(token, false);
  }
  else
  {
    token_to_return= "";
    for (int i= offset;; ++i)
    {
      QString c= text.mid(i, 1);
      if (c <= " ") break;
      if (c == ";")
      {
        if (token_to_return == "") token_to_return= ";";
        break;
      }
      token_to_return.append(c);
    }
  }
  return token_to_return;
}


/*
 Handle "client statements" -- statements that the client itself executes.
  Possible client statements:
  DELIMITER delimiter
  CONNECT ... (Todo: CONNECT depends entirely on settings, it should be possible to have arguments)
*/


/*
  Todo: We want all the mysql commands to work just the way that they work in mysql.
        We will not depend on TOKEN_KEYWORD_... for this because "\" and "q" are separate tokens,
        and "q| is case sensitive.
*/

#define MAX_SUB_TOKENS 22
int MainWindow::execute_client_statement(QString text, int *additional_result)
{
  log("execute_client_statement", 70);
//  int i= 0;
  int i2= 0;
  int  sub_token_offsets[MAX_SUB_TOKENS + 2];
  int  sub_token_lengths[MAX_SUB_TOKENS + 2];
  int  sub_token_types[MAX_SUB_TOKENS + 2];
  int true_text_size, true_token_count; /* size and count ignoring delimiter */
  //QString text;                      /* Todo: see if text should be a global */
  unsigned int statement_type;

  for (int i= main_token_number;; ++i)
  {
    if (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_SLASH
    ||  main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE
    ||  main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_MINUS) continue;
    if (main_token_lengths[i] == 0)
    {
      statement_type= 0;
      break;
    }
    statement_type= main_token_types[i];
    break;
  }

  //text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */

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
  /* And ignore the first token if it is \ because second token is the same */
  for (unsigned int i= main_token_number; /* main_token_lengths[i] != 0 && */ i < main_token_number + true_token_count; ++i)
  {
    /* Todo: find out why you have to figure out type again -- isn't it already known? */
    QString s= text.mid(main_token_offsets[i], main_token_lengths[i]);
    int t= token_type(s.data(), main_token_lengths[i], sql_mode_ansi_quotes);
    if (t == TOKEN_TYPE_COMMENT_WITH_SLASH
    ||  t == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE
    ||  t == TOKEN_TYPE_COMMENT_WITH_MINUS) continue;
    if ((i2 == 0) && (s == "\\"))
    {
      continue;
    }
    sub_token_offsets[i2]= main_token_offsets[i];
    sub_token_lengths[i2]= main_token_lengths[i];
    sub_token_types[i2]= main_token_types[i];
    ++i2;
    if (i2 > MAX_SUB_TOKENS) break;
  }
  sub_token_offsets[i2]= 0;
  sub_token_lengths[i2]= 0;
  sub_token_types[i2]= 0;

  /*
    CONNECT or \r.
    Defaults should have been set earlier. The user had a chance to change the defaults with SET.
    Currently the only possible dbms is "mysql", otherwise nothing happens.
    (or Tarantool, eh?)
    Ignore any other words in the statement.
    Todo: accept the mysql-client syntax, which has a few extras on it for reconnect.
    Todo: connect_mysql() returns an int, find out why you're not looking at it
  */
  /* Todo: We could easily modify so that we don't need sub_token_..., we could just skip comments. */
  if (statement_type == TOKEN_KEYWORD_CONNECT)
  {
    history_file_to_history_widget(); /* TODO: Maybe this is the wrong time to call? */
    history_file_start("HIST", ocelot_history_hist_file_name);
    if (connections_dbms[0] == DBMS_MYSQL) connect_mysql(MYSQL_MAIN_CONNECTION);
#ifdef DBMS_MARIADB
    if (connections_dbms[0] == DBMS_MARIADB) connect_mysql(MYSQL_MAIN_CONNECTION);
#endif
#ifdef DBMS_TARANTOOL
    if (connections_dbms[0] == DBMS_TARANTOOL) connect_tarantool(MYSQL_MAIN_CONNECTION, "DEFAULT", "DEFAULT", "DEFAULT", "DEFAULT");
#endif
    return 1;
  }

#ifdef DBMS_TARANTOOL
  /*
    CREATE SERVER id FOREIGN DATA WRAPPER tarantool
    OPTIONS (PORT ..., HOST ..., PASSWORD, ... USER ...)
    Do Tarantool connect for the sake of create_table_server.
    Todo: check port isn't re-used
  */
  if ((statement_type == TOKEN_KEYWORD_CREATE)
   && (sub_token_types[1] == TOKEN_KEYWORD_SERVER)
   && (sub_token_lengths[2] != 0)
   && (sub_token_types[3] == TOKEN_KEYWORD_FOREIGN)
   && (sub_token_lengths[4] != 0)
   && (sub_token_types[5] != 0)
   && (sub_token_lengths[6] != 0))
  {
    QString wrapper_name= text.mid(sub_token_offsets[6], sub_token_lengths[6]);
    wrapper_name= connect_stripper(wrapper_name, true);
    wrapper_name= wrapper_name.toUpper();
    if (wrapper_name == "OCELOT_TARANTOOL")
    {
      QString create_server_port= "DEFAULT";
      QString create_server_host= "DEFAULT";
      QString create_server_password= "DEFAULT";
      QString create_server_user= "DEFAULT";
      QString curr, prev;
      for (int i= 9; ; ++i)
      {
        if ((i >= MAX_SUB_TOKENS) || (sub_token_lengths[i] == 0))
        {
          make_and_put_message_in_result(ER_CREATE_SERVER_SYNTAX, 0, (char*)"");
          return 1;
        }
        curr= text.mid(sub_token_offsets[i], sub_token_lengths[i]);
        if (curr == ")") break;
        if ((main_token_types[i] >= TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
         && (main_token_types[i] <= TOKEN_TYPE_LITERAL_WITH_BRACE))
        {
          if (prev == "PORT") create_server_port= connect_stripper(curr, true);
          if (prev == "HOST") create_server_host= connect_stripper(curr, true);
          if (prev == "PASSWORD") create_server_password= connect_stripper(curr, true);
          if (prev == "USER") create_server_user= connect_stripper(curr, true);
        }
        prev= curr.toUpper();
      }
      int result= connect_tarantool(MYSQL_REMOTE_CONNECTION,
                                    create_server_port,
                                    create_server_host,
                                    create_server_password,
                                    create_server_user);
      if (result == 0)
      {
        tarantool_server_name= connect_stripper(text.mid(sub_token_offsets[2], sub_token_lengths[2]), true);
        connections_dbms[MYSQL_REMOTE_CONNECTION]= DBMS_TARANTOOL;
      }
      else
      {
        tarantool_server_name= "";
        connections_dbms[MYSQL_REMOTE_CONNECTION]= 0;
      }
      return 1;
    }
  }
#endif

  /* QUIT or \q. mysql equivalent. Todo: add to history box before exiting. */
  if ((statement_type == TOKEN_KEYWORD_QUIT)
  ||  (statement_type == TOKEN_KEYWORD_EXIT))
  {
    action_exit();
    return 1;
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
      make_and_put_message_in_result(ER_USE, 0, (char*)"");
      return 1;
    }
    /* If database name is in quotes or delimited, strip. Todo: stripping might be necessary in lots of cases. */
    s= connect_stripper(s, false);
    int query_len= s.toUtf8().size();                  /* See comment "UTF8 Conversion" */
    char *query= new char[query_len + 1];
    memcpy(query, s.toUtf8().constData(), query_len + 1);

    mysql_select_db_result= lmysql->ldbms_mysql_select_db(&mysql[MYSQL_MAIN_CONNECTION], query);
    delete []query;
    if (mysql_select_db_result != 0) put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
    else
    {
      statement_edit_widget->dbms_database= s;
      make_and_put_message_in_result(ER_DATABASE_CHANGED, 0, (char*)"");
    }
    return 1;
  }

  /* SOURCE or \.: mysql equivalent. */
  if (statement_type == TOKEN_KEYWORD_SOURCE)
  {
    /* Everything as far as statement end is source file name. Todo: so why connect_stripper? */
    /* Todo: allow for comments and ; */
    /* Todo: if we fill up the line, return an overflow message,
       or make line[] bigger and re-read the file. */
    /* Executing the source-file statements is surprisingly easy: just put them in
       the statement widget. This should be activating action_statement_edit_widget_changed
       and that ultimately causes execution.
       Handling multiple statements per line is okay, but SOURCE may not be witin multi-line.
       Difference from mysql client: this puts source-file statements in history, mysql client puts "source" statement. */
    /* Todo: stop recursion i.e. source statement within source statement. That's an error. */
    QString s;
    unsigned statement_length= /* text.size() */ true_text_size;
    if (i2 >= 2) s= text.mid(sub_token_offsets[1], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
    else
    {
      make_and_put_message_in_result(ER_SOURCE, 0, (char*)"");
      return 1;
    }
    s= connect_stripper(s, true);
    QFile file(s);
    bool open_result= file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (open_result == false)
    {
      make_and_put_message_in_result(ER_FILE_OPEN, 0, (char*)"");
      return 1;
    }
    /* Todo: this gets rid of SOURCE statement, but maybe it should be a comment in history. */
    statement_edit_widget->clear();
    QByteArray source_line;
    /* TODO: Would it be good if we could abort source statements with ^C? */
    /* TODO: Check for io error / premature eof */
    /* TODO: This is skipping comment lines and blank lines, that's probably unnecessary. */
    for (;;)
    {
      if (file.atEnd() == true) break;
      source_line= "";
      source_line= file.readLine();
      QString s= source_line;
      s= connect_stripper(s, false);
      if ((s != "") && (s.mid(0,1) != "#") && (s.mid(0,2) != "--"))
      {
        statement_edit_widget->insertPlainText(s);
        if (action_execute(0) == 2)
        {
          /*
            A DBMS-calling statement failed.
            This doesn't operate exactly as --abort-source-on-error
            would for MariaDB client, because we only return 2 for
            statements that call the server and are not multiple.
          */
          if (ocelot_abort_source_on_error > 0) break;
        }
     }
    }
    file.close();
    /* Without the following, the final line of source would go into the history twice. */
    *additional_result= TOKEN_KEYWORD_SOURCE;
    return 1;
  }

  /* PROMPT or \R: mysql equivalent. */
  /* This overrides the default setting which is made from ocelot_prompt during connect. */
  if (statement_type == TOKEN_KEYWORD_PROMPT)
  {
    QString s;
    unsigned statement_length= true_text_size /* text.size() */;
    if ((i2 >= 2) && (sub_token_types[1] != TOKEN_KEYWORD_USE))
      s= text.mid(sub_token_offsets[1], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
    else if (i2 >= 3) s= text.mid(sub_token_offsets[2], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
    else
    {
      statement_edit_widget->prompt_as_input_by_user= statement_edit_widget->prompt_default;
      ocelot_prompt= statement_edit_widget->prompt_default;
    emit statement_edit_widget->update_prompt_width(0); /* not necessary with Qt 5.2 */
      ocelot_prompt_is_default= false; /* todo: check: shouldn't this be true? */
      /* Todo: output a message */
      return 1;
    }
    statement_edit_widget->prompt_as_input_by_user= s;
    ocelot_prompt= s;
    emit statement_edit_widget->update_prompt_width(0); /* not necessary with Qt 5.2 */
    ocelot_prompt_is_default= false;
    make_and_put_message_in_result(ER_OK, 0, (char*)"");
    return 1;
  }

  /* WARNINGS or \W: mysql equivalent. This overrides a command-line option. */
  if (statement_type == TOKEN_KEYWORD_WARNINGS)
  {
    ocelot_history_includes_warnings= 1;
    make_and_put_message_in_result(ER_OK, 0, (char*)"");
    return 1;
  }

  /* NOWARNING or \w: mysql equivalent. This overrides a command-line option. */
  if (statement_type == TOKEN_KEYWORD_NOWARNING)
  {
    ocelot_history_includes_warnings= 0;
    make_and_put_message_in_result(ER_OK, 0, (char*)"");
    return 1;
  }

  /* DELIMITER or \d: mysql equivalent. */
  if (statement_type == TOKEN_KEYWORD_DELIMITER)
  {
    QString s= text.mid(sub_token_offsets[1], sub_token_lengths[1]);
    QString s_result= get_delimiter(s, text, sub_token_offsets[1]);
    if (s_result == "")
    {
      make_and_put_message_in_result(ER_DELIMITER, 0, (char*)"");
      return 1;
    }
    ocelot_delimiter_str= s_result;
    make_and_put_message_in_result(ER_OK, 0, (char*)"");
    return 1;
    }

  /* Todo: the following are placeholders, we want actual actions like what mysql would do. */
  if (statement_type == TOKEN_KEYWORD_QUESTIONMARK)
  {
    make_and_put_message_in_result(ER_HELP, 0, (char*)"");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_CHARSET)
  {
    make_and_put_message_in_result(ER_CHARSET, 0, (char*)"");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_EDIT)
  {
    make_and_put_message_in_result(ER_EDIT, 0, (char*)"");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_EGO)
  {
    make_and_put_message_in_result(ER_EGO, 0, (char*)"");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_GO)
  {
    make_and_put_message_in_result(ER_GO, 0, (char*)"");
  }
  if (statement_type == TOKEN_KEYWORD_HELP_IN_CLIENT)
  {
    make_and_put_message_in_result(ER_HELP, 0, (char*)"");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_NOPAGER)
  {
    make_and_put_message_in_result(ER_NOPAGER, 0, (char*)"");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_NOTEE) /* see comment=tee+hist */
  {
    history_file_stop("TEE");
    make_and_put_message_in_result(ER_OK, 0, (char*)"");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_PAGER)
  {
    make_and_put_message_in_result(ER_PAGER, 0, (char*)"");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_PRINT)
  {
    make_and_put_message_in_result(ER_PRINT, 0, (char*)"");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_REHASH)   /* This overrides a command-line option */
  {
    int i= rehash_scan();
    if (i > 0) ocelot_auto_rehash= 1;
    else ocelot_auto_rehash= 0;
    return 1;
  }
  /* TODO: "STATUS" should output as much information as the mysql client does. */
  if (statement_type == TOKEN_KEYWORD_STATUS)
  {
    if (connections_is_connected[0] != 1) make_and_put_message_in_result(ER_NOT_CONNECTED, 0, (char*)"");
    else
    {
      char dbms_version[256];
      char dbms_host[256];
      char dbms_port[256];
      char buffer[1024];
      strcpy(dbms_version, statement_edit_widget->dbms_version.toUtf8());
      strcpy(dbms_host, statement_edit_widget->dbms_host.toUtf8());
      strcpy(dbms_port, statement_edit_widget->dbms_port.toUtf8());
      sprintf(buffer, er_strings[er_off + ER_STATUS], dbms_version, dbms_host, dbms_port);
      put_message_in_result(buffer);
    }
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_SYSTEM)
  {
    /*
      With mysql client "system ls" would do an ls with system. We use popen not system.
      I don't know whether there is a Windows equivalent; mysql client doesn't support one.
      So the easiest thing for a Windows port is:
      make_and_put_message_in_result(tr("SYSTEM is not implemented."));
      Todo: allow "kill" -- some research required about how to stop a shell command.
      Todo: reconsider: maybe output should go to result grid rather than history.
    */
    QString s;
    unsigned statement_length= true_text_size /* text.size() */;
    s= text.mid(sub_token_offsets[1], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
    char *command_string= new char[s.size() + 1];
    memcpy(command_string, s.toUtf8().constData(), s.size() + 1);
    put_message_in_result(""); /* unnecessary? */
    FILE *fp;
    int status;
    char result_line[STRING_LENGTH_512]; /* arbitrary maximum expected line length */
    fp= popen(command_string, "r");
    if (fp == NULL) make_and_append_message_in_result(ER_POPEN_FAILED, 0, (char*)"");
    else
    {
      while (fgets(result_line, STRING_LENGTH_512, fp) != NULL)
      {
        statement_edit_widget->result.append(result_line);
      }
      status= pclose(fp);
      if (status == -1) make_and_append_message_in_result(ER_PCLOSE_FAILED, 0, (char*)"");
      /* We do not bother to check whether the command failed, display will be blank. */
    }
    delete [] command_string;
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_TEE) /* see comment=tee+hist */
  {
    /* Everything as far as statement end is tee file name. Compare how we do SOURCE file name. */
    QString s;
    unsigned statement_length= /* text.size() */ true_text_size;
    if (i2 >= 2) s= text.mid(sub_token_offsets[1], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
    else s= "";
    if (history_file_start("TEE", s) == 0) make_and_put_message_in_result(ER_FOPEN_FAILED, 0, (char*)"");
    else make_and_put_message_in_result(ER_OK, 0, (char*)"");
    return 1;
  }
#ifdef DEBUGGER
  if (statement_type == TOKEN_KEYWORD_DEBUG_DEBUG)
  {
    debug_debug_go(text);
    return 1;
  }
  if ((statement_type == TOKEN_KEYWORD_DEBUG_BREAKPOINT)
   || (statement_type == TOKEN_KEYWORD_DEBUG_CLEAR)
   || (statement_type == TOKEN_KEYWORD_DEBUG_TBREAKPOINT))
  {
    debug_breakpoint_or_clear_go(statement_type, text);
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_SETUP)
  {
    debug_setup_go(text);
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_INSTALL)
  {
    debug_install_go();
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_EXIT)
  {
    debug_exit_go(0);
    debug_menu_enable_or_disable(TOKEN_KEYWORD_DEBUG_EXIT);
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_DELETE)
  {
    debug_delete_go();
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_EXECUTE)
  {
    debug_execute_go();
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_SKIP)
  {
    debug_skip_go();
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_SET)
  {
    debug_set_go(text);
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_SOURCE)
  {
    debug_source_go();
    return 1;
  }
  if ((statement_type == TOKEN_KEYWORD_DEBUG_CONTINUE)
   || (statement_type == TOKEN_KEYWORD_DEBUG_EXECUTE)
   || (statement_type == TOKEN_KEYWORD_DEBUG_INFORMATION)
   || (statement_type == TOKEN_KEYWORD_DEBUG_LEAVE)
   || (statement_type == TOKEN_KEYWORD_DEBUG_NEXT)
   || (statement_type == TOKEN_KEYWORD_DEBUG_REFRESH)
   || (statement_type == TOKEN_KEYWORD_DEBUG_STEP))
  {
    /* All the other debug commands go to the same place. */
    debug_other_go(text);
    return 1;
  }
#endif
  /* See whether general format is SET ocelot_... = value ;". Read comment with label = "client variables" */
  if (i2 >= 4)
  {
    if (sub_token_types[0] == TOKEN_KEYWORD_SET)
    {
      /* Todo: figure out why sometimes we say connect_stripper() and sometimes we don't */
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_text_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_statement_text_color= ccn;
        make_style_strings();
        statement_edit_widget_setstylesheet();
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)"");return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_statement_background_color= ccn;
        make_style_strings();
        statement_edit_widget_setstylesheet();
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)"");return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_border_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_statement_border_color= ccn;
        make_style_strings();
        statement_edit_widget_setstylesheet();
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)"");return 1;
      }
      /* TODO: setting font_family can fail e.g. say 'Courier' and you could get 'Sans'
               because only 'Courier New' exists. There should be a warning, and
               setting some style hint e.g. "at least it should be monospace" would be good.
               This applies for all font_family settings. */
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_font_family", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_font_family= text.mid(sub_token_offsets[3], sub_token_lengths[3]);
        make_style_strings();
        statement_edit_widget_setstylesheet();
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_font_size", Qt::CaseInsensitive) == 0)
      {
        QString ccn= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        if ((ccn.toInt() < FONT_SIZE_MIN) || (ccn.toInt() > FONT_SIZE_MAX)) { make_and_put_message_in_result(ER_UNKNOWN_FONT_SIZE, 0, (char*)""); return 1; }
        ocelot_statement_font_size= ccn;
        make_style_strings();
        statement_edit_widget_setstylesheet();
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_font_style", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_style(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_FONT_STYLE, 0, (char*)""); return 1; }
        ocelot_statement_font_style= ccn;
        make_style_strings();
        statement_edit_widget_setstylesheet();
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_font_weight", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_weight(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_FONT_WEIGHT, 0, (char*)""); return 1; }
        ocelot_statement_font_weight= ccn;
        make_style_strings();
        statement_edit_widget_setstylesheet();
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_highlight_literal_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_statement_highlight_literal_color= ccn;
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_highlight_identifier_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_statement_highlight_identifier_color= ccn;
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_highlight_comment_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_statement_highlight_comment_color= ccn;
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)"");return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_highlight_operator_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_statement_highlight_operator_color= ccn;
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_highlight_keyword_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_statement_highlight_keyword_color= ccn;
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_prompt_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_statement_prompt_background_color= ccn;
        make_style_strings();
        statement_edit_widget_setstylesheet();
        statement_edit_widget->statement_edit_widget_left_bgcolor= QColor(ocelot_statement_prompt_background_color);
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)"");return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_highlight_current_line_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_statement_highlight_current_line_color= ccn;
        statement_edit_widget->highlightCurrentLine();
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)"");return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_highlight_function_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_statement_highlight_function_color= ccn;
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_syntax_checker", Qt::CaseInsensitive) == 0)
      {
        QString syntax_checker= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        int syntax_checker_as_int= syntax_checker.toInt();
        if ((syntax_checker_as_int < 0) || (syntax_checker_as_int > 3))
        { make_and_put_message_in_result(ER_SYNTAX, 0, (char*)""); return 1; }
        ocelot_statement_syntax_checker= syntax_checker;
        make_and_put_message_in_result(ER_OK, 0, (char*)"");return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_format_statement_indent", Qt::CaseInsensitive) == 0)
      {
        QString format_indent= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        int format_indent_as_int= format_indent.toInt();
        if ((format_indent_as_int < 0) || (format_indent_as_int > 8))
        { make_and_put_message_in_result(ER_FORMAT_STATEMENT, 0, (char*)""); return 1; }
        ocelot_statement_format_statement_indent= format_indent;
        make_and_put_message_in_result(ER_OK, 0, (char*)"");return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_format_clause_indent", Qt::CaseInsensitive) == 0)
      {
        QString format_indent= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        int format_indent_as_int= format_indent.toInt();
        if ((format_indent_as_int < 0) || (format_indent_as_int > 8))
        { make_and_put_message_in_result(ER_FORMAT_CLAUSE, 0, (char*)""); return 1; }
        ocelot_statement_format_clause_indent= format_indent;
        make_and_put_message_in_result(ER_OK, 0, (char*)"");return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_format_keyword_case", Qt::CaseInsensitive) == 0)
      {
        QString format_case= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        format_case= format_case.toLower();
        if ((format_case != "upper") && (format_case != "lower") && (format_case != "unchanged"))
        { make_and_put_message_in_result(ER_FORMAT_KEY_CASE, 0, (char*)""); return 1; }
        ocelot_statement_format_keyword_case= format_case;
        make_and_put_message_in_result(ER_OK, 0, (char*)"");return 1;
      }
      bool is_result_grid_style_changed= false;
      bool is_result_grid_font_size_changed= false;
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_text_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_grid_text_color= ccn;
        assign_names_for_colors(); is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_border_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_grid_border_color= ccn;
        assign_names_for_colors(); is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_grid_background_color= ccn;
        assign_names_for_colors(); is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_header_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_grid_header_background_color= ccn;
        assign_names_for_colors(); is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_font_family", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_font_family= text.mid(sub_token_offsets[3], sub_token_lengths[3]);
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_font_size", Qt::CaseInsensitive) == 0)
      {
        QString ccn= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        if ((ccn.toInt() < FONT_SIZE_MIN) || (ccn.toInt() > FONT_SIZE_MAX)) { make_and_put_message_in_result(ER_UNKNOWN_FONT_SIZE, 0, (char*)""); return 1; }
        if (ccn != ocelot_grid_font_size) is_result_grid_font_size_changed= true;
        ocelot_grid_font_size= ccn;
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_font_style", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_style(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_FONT_STYLE, 0, (char*)""); return 1; }
        ocelot_grid_font_style= ccn;
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_font_weight", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_weight(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_FONT_WEIGHT, 0, (char*)""); return 1; }
        ocelot_grid_font_weight= ccn;
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_cell_border_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_grid_cell_border_color= ccn;
        assign_names_for_colors(); is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_cell_drag_line_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_grid_cell_drag_line_color= ccn;
        assign_names_for_colors(); is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_border_size", Qt::CaseInsensitive) == 0)
      {
        QString ccn= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        if ((ccn.toInt() < 0) || (ccn.toInt() > 9)) { make_and_put_message_in_result(ER_UNKNOWN_BORDER_SIZE, 0, (char*)""); return 1; }
        ocelot_grid_border_size= ccn;
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_cell_border_size", Qt::CaseInsensitive) == 0)
      {
        QString ccn= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        if ((ccn.toInt() < 0) || (ccn.toInt() > 10)) { make_and_put_message_in_result(ER_UNKNOWN_CELL_BORDER_SIZE, 0, (char*)""); return 1; }
        ocelot_grid_cell_border_size= ccn;
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_cell_drag_line_size", Qt::CaseInsensitive) == 0)
      {
        QString ccn= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        if ((ccn.toInt() < 0) || (ccn.toInt() > 10)) { make_and_put_message_in_result(ER_UNKNOWN_CELL_DRAG_LINE_SIZE, 0, (char*)""); return 1; }
        ocelot_grid_cell_drag_line_size= ccn;
        is_result_grid_style_changed= true;
      }
      if (is_result_grid_style_changed == true)
      {
        ResultGrid* r;
        make_style_strings();
        for (int i_r= 0; i_r < ocelot_grid_actual_tabs; ++i_r)
        {
          r= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(i_r));
          r->set_all_style_sheets(ocelot_grid_style_string, ocelot_grid_cell_drag_line_size, 1, is_result_grid_font_size_changed);
        }
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      bool is_extra_rule_1_style_changed= false;
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_extra_rule_1_text_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_extra_rule_1_text_color= ccn;
        assign_names_for_colors(); is_extra_rule_1_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_extra_rule_1_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_extra_rule_1_background_color= ccn;
        assign_names_for_colors(); is_extra_rule_1_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_extra_rule_1_condition", Qt::CaseInsensitive) == 0)
      {
        ocelot_extra_rule_1_condition= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), true);
        is_extra_rule_1_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_extra_rule_1_display_as", Qt::CaseInsensitive) == 0)
      {
        ocelot_extra_rule_1_display_as= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        is_extra_rule_1_style_changed= true;
      }
      if (is_extra_rule_1_style_changed == true)
      {
        make_style_strings();
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }

      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_text_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_history_text_color= ccn;
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_history_background_color= ccn;
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_border_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_history_border_color= ccn;
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_font_family", Qt::CaseInsensitive) == 0)
      {
        ocelot_history_font_family= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_font_size", Qt::CaseInsensitive) == 0)
      {
        QString ccn= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        if ((ccn.toInt() < FONT_SIZE_MIN) || (ccn.toInt() > FONT_SIZE_MAX)) { make_and_put_message_in_result(ER_UNKNOWN_FONT_SIZE, 0, (char*)""); return 1; }
        ocelot_history_font_size= ccn;
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_font_style", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_style(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_FONT_STYLE, 0, (char*)""); return 1; }
        ocelot_history_font_style= ccn;
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_font_weight", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_weight(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_FONT_WEIGHT, 0, (char*)""); return 1; }
        ocelot_history_font_weight= ccn;
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_max_row_count", Qt::CaseInsensitive) == 0)
      {
        QString ccn= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        if (ccn.toInt() < 0) { make_and_put_message_in_result(ER_ILLEGAL_VALUE, 0, (char*)""); return 1; }
        ocelot_history_max_row_count= ccn;
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_text_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_menu_text_color= ccn;
        make_style_strings();
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_menu_background_color= ccn;
        make_style_strings();
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_border_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_COLOR, 0, (char*)""); return 1; }
        ocelot_menu_border_color= ccn;
        make_style_strings();
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        assign_names_for_colors(); make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_font_family", Qt::CaseInsensitive) == 0)
      {
        ocelot_menu_font_family= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        make_style_strings();
        //main_window->setStyleSheet(ocelot_menu_style_string);
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_font_size", Qt::CaseInsensitive) == 0)
      {
        QString ccn= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        if ((ccn.toInt() < FONT_SIZE_MIN) || (ccn.toInt() > FONT_SIZE_MAX)) { make_and_put_message_in_result(ER_UNKNOWN_FONT_SIZE, 0, (char*)""); return 1; }
        ocelot_menu_font_size= ccn;
        make_style_strings();
        //main_window->setStyleSheet(ocelot_menu_style_string);
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_font_style", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_style(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_FONT_STYLE, 0, (char*)""); return 1; }
        ocelot_menu_font_style= ccn;
        make_style_strings();
        //main_window->setStyleSheet(ocelot_menu_style_string);
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_font_weight", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_weight(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { make_and_put_message_in_result(ER_UNKNOWN_FONT_WEIGHT, 0, (char*)""); return 1; }
        ocelot_menu_font_weight= ccn;
        make_style_strings();
        //main_window->setStyleSheet(ocelot_menu_style_string);
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }

      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_batch", Qt::CaseInsensitive) == 0)
      {
        QString ccn= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        int i= ccn.toInt();
        if ((i != 0) && (i != 1)) { make_and_put_message_in_result(ER_ILLEGAL_VALUE, 0, (char*)""); return 1; }
        ocelot_batch= i;
        if (i == 1) ocelot_html= ocelot_xml= 0; /* should we warn? */
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_html", Qt::CaseInsensitive) == 0)
      {
        QString ccn= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        int i= ccn.toInt();
        if ((i != 0) && (i != 1)) { make_and_put_message_in_result(ER_ILLEGAL_VALUE, 0, (char*)""); return 1; }
        ocelot_html= i;
        if (i == 1) ocelot_batch= ocelot_xml= 0; /* should we warn? */
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_xml", Qt::CaseInsensitive) == 0)
      {
        QString ccn= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        int i= ccn.toInt();
        if ((i != 0) && (i != 1)) { make_and_put_message_in_result(ER_ILLEGAL_VALUE, 0, (char*)""); return 1; }
        ocelot_xml= i;
        if (i == 1) ocelot_batch= ocelot_html= 0; /* should we warn? */
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_raw", Qt::CaseInsensitive) == 0)
      {
        QString ccn= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        int i= ccn.toInt();
        if ((i != 0) && (i != 1)) { make_and_put_message_in_result(ER_ILLEGAL_VALUE, 0, (char*)""); return 1; }
        ocelot_raw= i;
        make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
      }
      {
        int ii= shortcut(text.mid(sub_token_offsets[1], sub_token_lengths[1]), text.mid(sub_token_offsets[3], sub_token_lengths[3]), true, true);
        if (ii == 1)
        {
          make_and_put_message_in_result(ER_OK, 0, (char*)""); return 1;
        }
        if (ii == -1)
        {
          make_and_put_message_in_result(ER_ILLEGAL_VALUE, 0, (char*)""); return 1;
        }
      }
    }
  }

  return 0;
}


/*
  REHASH
  Make a cache of all objects in the current database.
  Not a great name, but mysql client requires "rehash" for something
  vaguely similar (they store some column names with a hash) (we don't).
  Currently we're taking in table_name, column_name from information_schema
  for the current database, into result_set_copy.

  Todo: Allow REHASH [TABLES|TRIGGERS|etc.] [database.]name or *
        There might be multiple databases and multiple connections
  Todo: Allow triggers events procedures functions indexes
  Todo: Hover provides more information about the object
  Todo: We know declared variables or other things made within a
        routine (see hparse_f_labels etc.) but didn't merge with
        the results of this routine
  Todo: add rehash to Options menu
  Todo: see correlations | aliases
  Todo: error messages
  Todo: information_schema.engines if MySQL version >= 5.6
  Todo: information_schema.character_sets if MySQL version >= 5.?
  Beware: You don't have read access for everything.
  Beware: Names might be case sensitive.
  Beware: Qualifier might indicate a different database.
  Columns are "table.column"
  When is rehash_scan() called:
    Only the "REHASH" statement does anything.
    During connect | reconnect | "use" statement, we could look at
    --auto-rehash or --no-auto-rehash or --skip-auto-rehash. We don't.
    During "use" statement, we could redo rehashing. We don't.
    So default = TRUE but unlike with mysql client nothing is automatic.
    This affects unsigned short ocelot_auto_rehash.
  When is rehash_search() called:
    When user hits ` i.e. backtick show the choices.
    When user hits ocelot_shortcut_autocomplete_keysequence which by
    default is \t i.e. tab Qt::Key_Tab, if there's an unambiguous name,
    fill it in (don't worry about what happens if user hits tab twice).
  Why we do a single unioned search with unnecessary columns:
    If we did multiple searches, we would affect MAX_QUERIES_PER_HOUR
    more. But this means there could be thousands of entries
    in the cache, and searches are sequential.
    Todo: add an order-by in the select, and do binary searches.
*/

int MainWindow::rehash_scan()
{
  MYSQL_RES *res= NULL;
//  QString s;
  unsigned int *result_max_column_widths;                     /* dynamic-sized list of actual maximum widths in detail columns */

  result_max_column_widths= 0;

  /* garbage_collect from the last rehash, if any */
  if (rehash_result_set_copy != 0) { delete [] rehash_result_set_copy; rehash_result_set_copy= 0; }
  if (rehash_result_set_copy_rows != 0) { delete [] rehash_result_set_copy_rows; rehash_result_set_copy_rows= 0; }
  rehash_result_column_count= 0;
  rehash_result_row_count= 0;

  if (connections_is_connected[0] != 1)
  {
    make_and_put_message_in_result(ER_NOT_CONNECTED, 0, (char*)"");
    return 0;
  }
  char query[1024];

  if (connections_dbms[0] == DBMS_TARANTOOL)
  {
    int result;
    sprintf(query, "select 'T', name,'T' "
                   "from _space "
                   "union all "
                   "select 't', name,'' "
                   "from _trigger "
                   "union all "
                   "select 'I', name,name "
                   "from _index;"

           );

    QString s= query;
    s= tarantool_sql_to_lua(s, TOKEN_KEYWORD_SELECT, 0);

    result=
    tarantool_execute_sql(s, MYSQL_MAIN_CONNECTION, 1);
    if (result != 0)
    {
      make_and_put_message_in_result(ER_SELECT_FAILED, 0, (char*)"");
      return 0;
    }
    /* Todo: Memory leak here */
    ResultGrid *rg= new ResultGrid(lmysql, this, false);
    MYSQL_RES *mysql_res_for_new_result_set= NULL;
    rg->fillup(mysql_res_for_new_result_set,
              //&tarantool_tnt_reply,
              connections_dbms[MYSQL_MAIN_CONNECTION],
              //this,
              false, ocelot_result_grid_column_names,
              lmysql, ocelot_client_side_functions,
              ocelot_batch, ocelot_html, ocelot_raw, ocelot_xml,
              MYSQL_MAIN_CONNECTION,
              false);
    rehash_result_column_count= tarantool_num_fields();
    rehash_result_row_count= tarantool_num_rows(MYSQL_MAIN_CONNECTION);
    if ((rehash_result_column_count == 0) || (rehash_result_row_count == 0))
    {
      make_and_put_message_in_result(ER_0_ROWS_RETURNED, 0, (char*)"");
      return 0;
    }
  }
  else
  {
    sprintf(query, "select 'D',database(),'' "
                   "union all "
                   "select 'C',table_name,column_name "
                     "from information_schema.columns "
                   "where table_schema = database() "
                   "union all "
                   "select 'T',table_name,table_type "
                   "from information_schema.tables "
                   "where table_schema = database() "
                   "union all "
                   "select 'F',routine_name,routine_type "
                   "from information_schema.routines "
                   "where routine_schema = database() and routine_type = 'FUNCTION' "
                   "union all "
                   "select 'P',routine_name,routine_type "
                   "from information_schema.routines "
                   "where routine_schema = database() and routine_type = 'PROCEDURE' "
                   "union all "
                   "select 't',trigger_name,'' "
                   "from information_schema.triggers "
                   "where trigger_schema = database() "
                   "union all "
                   "select 'E',event_name,'' "
                   "from information_schema.events "
                   "where event_schema = database() "
                   "union all "
                   "select 'I',table_name,index_name "
                   "from information_schema.statistics "
                   "where table_schema = database() "
           );
    if (lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], query))
      {
        make_and_put_message_in_result(ER_SELECT_FAILED, 0, (char*)"");
        return 0;
      }

    res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
    if (res == NULL)
    {
      make_and_put_message_in_result(ER_MYSQL_STORE_RESULT_FAILED, 0, (char*)"");
      return 0;
    }
    rehash_result_column_count= lmysql->ldbms_mysql_num_fields(res);
    rehash_result_row_count= lmysql->ldbms_mysql_num_rows(res);
    if ((rehash_result_column_count == 0) || (rehash_result_row_count == 0))
    {
      make_and_put_message_in_result(ER_0_ROWS_RETURNED, 0, (char*)"");
      return 0;
    }
  }
  result_max_column_widths= new unsigned int[rehash_result_column_count];
  ResultGrid* result_grid;
  result_grid= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(0));

  if (connections_dbms[0] == DBMS_TARANTOOL)
  {
    tarantool_scan_rows(
            rehash_result_column_count, /* result_column_count, */
            rehash_result_row_count, /* result_row_count, */
            res, /* grid_mysql_res, */
            &rehash_result_set_copy,
            &rehash_result_set_copy_rows,
            &result_max_column_widths);
    /* Todo: another leak (?) -- we don't free the result set */
  }
  else
  {
    result_grid->scan_rows(
            rehash_result_column_count, /* result_column_count, */
            rehash_result_row_count, /* result_row_count, */
            res, /* grid_mysql_res, */
            &rehash_result_set_copy,
            &rehash_result_set_copy_rows,
            &result_max_column_widths);
    lmysql->ldbms_mysql_free_result(res);
  }
  delete [] result_max_column_widths;
  /* First set of rows is 'D',database(),''. If it's null, error. */
  char database_name[512];
  if (connections_dbms[0] == DBMS_TARANTOOL) strcpy(database_name, "main");
  else
  {
    rehash_get_database_name(database_name);
    if (database_name[0] == '\0')
    {
      make_and_put_message_in_result(ER_NO_DATABASE_SELECTED, 0, (char*)"");
      return 0;
    }
  }
  long unsigned int r;
  int count_of_columns= 0;
  int count_of_tables= 0;
  int count_of_functions= 0;
  int count_of_procedures= 0;
  int count_of_triggers= 0;
  int count_of_events= 0;
  int count_of_indexes= 0;
  for (r= 0; r < rehash_result_row_count; ++r)
  {
    char *row_pointer;
    unsigned int column_length;
    char column_value[512];
    row_pointer= rehash_result_set_copy_rows[r];
    memcpy(&column_length, row_pointer, sizeof(unsigned int));
    row_pointer+= sizeof(unsigned int) + sizeof(char);
    strncpy(column_value, row_pointer, column_length);
    column_value[column_length]= '\0';
    if (strcmp(column_value, "C") == 0) ++count_of_columns;
    if (strcmp(column_value, "T") == 0) ++count_of_tables;
    if (strcmp(column_value, "F") == 0) ++count_of_functions;
    if (strcmp(column_value, "P") == 0) ++count_of_procedures;
    if (strcmp(column_value, "t") == 0) ++count_of_triggers;
    if (strcmp(column_value, "E") == 0) ++count_of_events;
    if (strcmp(column_value, "I") == 0) ++count_of_indexes;
  }
  char buffer[ER_MAX_LENGTH];
  sprintf(buffer, er_strings[er_off + ER_OK_REHASH],
          database_name, count_of_tables, count_of_columns, count_of_functions, count_of_procedures, count_of_triggers, count_of_events, count_of_indexes);
  put_message_in_result(buffer);
  return 1;
}

/*
  Pass: search string. Return: column name matching searching string.
  Todo: We only look at column[1] column_name. We should look at column[0] table_name.
*/
QString MainWindow::rehash_search(char *search_string, int reftype)
{
  long unsigned int r;
  char *row_pointer;
  unsigned int column_length;
  unsigned int i;
  char column_value[512];
  unsigned int search_string_length;
  QString tmp_word= "";
  int count_of_hits= 0;
  char desired_types[TOKEN_REFTYPE_MAX]= "";
  unsigned int column_to_match= 0;
  search_string_length= strlen(search_string);
  column_value[0]= '\0';
  if ((reftype == TOKEN_REFTYPE_COLUMN)
   || (reftype == TOKEN_REFTYPE_TABLE_OR_COLUMN)
   || (reftype == TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN)
   || (reftype == TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN_OR_FUNCTION)
   || (reftype == TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN_OR_FUNCTION_OR_VARIABLE)
   || (reftype == TOKEN_REFTYPE_COLUMN_OR_USER_VARIABLE)
   || (reftype == TOKEN_REFTYPE_COLUMN_OR_VARIABLE)
   || (reftype == TOKEN_REFTYPE_TABLE_OR_COLUMN_OR_FUNCTION))
  {
    strcpy(desired_types, "C");
    column_to_match= 2;
  }
  else if ((reftype == TOKEN_REFTYPE_FUNCTION)
   || (reftype == TOKEN_REFTYPE_DATABASE_OR_FUNCTION))
  {
    strcpy(desired_types, "F");
    column_to_match= 1;
  }
  else if ((reftype == TOKEN_REFTYPE_PROCEDURE)
   || (reftype == TOKEN_REFTYPE_DATABASE_OR_PROCEDURE))
  {
    strcpy(desired_types, "P");
    column_to_match= 1;
  }
  else if ((reftype == TOKEN_REFTYPE_FUNCTION_OR_PROCEDURE)
   || (reftype == TOKEN_REFTYPE_DATABASE_OR_FUNCTION_OR_PROCEDURE))
  {
    strcpy(desired_types, "FP");
    column_to_match= 1;
  }
  else if ((reftype == TOKEN_REFTYPE_TABLE)
   || (reftype == TOKEN_REFTYPE_DATABASE_OR_TABLE))
  {
    strcpy(desired_types, "T");
    column_to_match= 1;
  }
  else if ((reftype == TOKEN_REFTYPE_TRIGGER)
   || (reftype == TOKEN_REFTYPE_DATABASE_OR_TRIGGER))
  {
    strcpy(desired_types, "t");
    column_to_match= 1;
  }
  else if ((reftype == TOKEN_REFTYPE_EVENT)
   || (reftype == TOKEN_REFTYPE_DATABASE_OR_EVENT))
  {
    strcpy(desired_types, "E");
    column_to_match= 1;
  }
  else if (reftype == TOKEN_REFTYPE_INDEX)
  {
    strcpy(desired_types, "I");
    column_to_match= 2;
  }
  else return tmp_word;
  for (r= 0; r < rehash_result_row_count; ++r)
  {
    row_pointer= rehash_result_set_copy_rows[r];
    for (i= 0; i < rehash_result_column_count; ++i)
    {
      memcpy(&column_length, row_pointer, sizeof(unsigned int));
      row_pointer+= sizeof(unsigned int) + sizeof(char);
      /* Now row_pointer points to contents, length has # of bytes */
      if (i == 0)
      {
        assert(column_length == 1);
        if (strchr(desired_types, *row_pointer) == NULL) break;
      }
      if (i == column_to_match)
      {
        if (search_string_length < column_length)
        {
          if (strncmp(row_pointer, search_string, search_string_length) == 0)
          {
            strncpy(column_value, row_pointer, column_length);
            column_value[column_length]= '\0';
            tmp_word.append(column_value);
            tmp_word.append(" ");
            ++count_of_hits;
            if (count_of_hits > 10) break;
          }
        }
      }
      row_pointer+= column_length;
    }
    if (count_of_hits > 10) break;
  }
  return tmp_word;
}

void MainWindow::rehash_get_database_name(char *database_name)
{
  *database_name= '\0';
  if (rehash_result_set_copy == 0) return;
  char *row_pointer;
  unsigned int column_length;
  row_pointer= rehash_result_set_copy_rows[0];
  memcpy(&column_length, row_pointer, sizeof(unsigned int));
  row_pointer+= sizeof(unsigned int) + sizeof(char);
  row_pointer+= column_length;
  memcpy(&column_length, row_pointer, sizeof(unsigned int));
  row_pointer+= sizeof(unsigned int);
  if ((*row_pointer & FIELD_VALUE_FLAG_IS_NULL) != 0) return;
  row_pointer+= sizeof(char);
  strncpy(database_name, row_pointer, column_length);
  *(database_name + column_length)= '\0';
}

/*
  Call widget_sizer() from action_execute() to resize the three
  main_layout widgets if necessary.
  Todo: so far widget_sizer() is just ideas, mostly unimplemented.
  The widgets are in a layout (although grid and history and debug leave the layout if detached).
  But the maximum sizing can be helped by me. Some rules are:
  * If history_edit_widget is higher than needed without scrolling, shrink it.
  * If user has manually expanded a window, leave it.
    ** But a grid should snap back to a minimum size
  * A select() should be visible, up to some maximum line count, even if a result set has many rows.
  * We need a scroll bar for the whole main window. But maybe not for all the main_layout widgets?
  * The widget might be detached -- in that case the sizes of other widgets won't matter.
  * The statement window can shrink quite a bit because after action_execute() there's nothing in it.
  Probably TitleBarHeight won't matter because we set the bar to vertical (update: no, I gave up on vertical)
  Probably PM_DefaultFrameWidth won't matter but I'm not sure what it is.
  So far all I've thought about is a special case for detaching.
    It's a bit silly to allow for height of a horizontal scroll bar, if there is no scroll bar.
  Maybe we should use the WA_DontShowOnScreen trick for this and for size_calc.
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
void MainWindow::put_diagnostics_in_result(unsigned int connection_number)
{
  unsigned int mysql_warning_count;
  char mysql_error_and_state[50]; /* actually we should need less than 50 */
  char elapsed_time_string[50];   /* actually we should need less than 50 */
  QString s1, s2;

  /* Display may include: how long the statement took, to nearest tenth of a second. Todo: fix calculation. */
  {
    qint64 statement_end_time= QDateTime::currentMSecsSinceEpoch();
    qint64 elapsed_time= statement_end_time - statement_edit_widget->start_time;
    long int elapsed_time_as_long_int= (long int) elapsed_time;
    float elapsed_time_as_float= (float) elapsed_time_as_long_int / 1000;
    sprintf(elapsed_time_string, " (%.1f seconds)", elapsed_time_as_float);
  }

#ifdef DBMS_TARANTOOL
  if (connections_dbms[connection_number] == DBMS_TARANTOOL)
  {
    /* todo: show elapsed time */
    if (tarantool_errno[connection_number] == 0)
    {
      s1= er_strings[er_off + ER_OK];
      s1.append(" ");
      s1.append(QString::number(tarantool_row_count[connection_number]));
      s1.append(" rows affected");
    }
    else
    {
      s1= s1= er_strings[er_off + ER_ERROR];
      s1.append(". ");
      s1.append(tarantool_errmsg);
    }
    s1.append(elapsed_time_string);
    statement_edit_widget->result= s1;
    return;
  }
#endif
  if ((connections_dbms[connection_number] != DBMS_MYSQL)
   && (connections_dbms[connection_number] != DBMS_MARIADB))
  {
    statement_edit_widget->result= "Bad connection."; /* shouldn't happen! */
    return;
  }
  mysql_errno_result= lmysql->ldbms_mysql_errno(&mysql[connection_number]);
  mysql_warning_count= lmysql->ldbms_mysql_warning_count(&mysql[connection_number]);
  if (mysql_errno_result == 0)
  {
    s1= er_strings[er_off + ER_OK];
    s1.append(" ");

    /* This should output, e.g. "Records: 3 Duplicates: 0 Warnings: 0" -- but actually nothing happens. */
    if (lmysql->ldbms_mysql_info(&mysql[connection_number])!= NULL)
    {
      /* This only works for certain insert, load, alter or update statements */
      s1.append(tr(lmysql->ldbms_mysql_info(&mysql[connection_number])));
    }
    else
    {
      sprintf(mysql_error_and_state, er_strings[er_off + ER_ROWS_AFFECTED], lmysql->ldbms_mysql_affected_rows(&mysql[connection_number]));
      s1.append(mysql_error_and_state);
      if (mysql_warning_count > 0)
      {
        sprintf(mysql_error_and_state, er_strings[er_off + ER_WARNING], mysql_warning_count);
        s1.append(mysql_error_and_state);
        if (mysql_warning_count > 1) s1.append("s");
      }
    }
    s1.append(elapsed_time_string);
    if (mysql_warning_count > 0)
    {
      if (ocelot_history_includes_warnings > 0)
      {
        lmysql->ldbms_mysql_query(&mysql[connection_number], "show warnings");
        MYSQL_RES *mysql_res_for_warnings;
        MYSQL_ROW warnings_row;
        QString s;
        // unsigned long connect_lengths[0];
        mysql_res_for_warnings= lmysql->ldbms_mysql_store_result(&mysql[connection_number]);
        assert(mysql_res_for_warnings != NULL);
        //for (unsigned int wi= 0; wi <= lmysql->ldbms_mysql_warning_count(&mysql[connection_number]); ++wi)
        int count_of_fields= lmysql->ldbms_mysql_num_fields(mysql_res_for_warnings);
        assert(count_of_fields == 3);
        for (;;)
        {
          warnings_row= lmysql->ldbms_mysql_fetch_row(mysql_res_for_warnings);
          if (warnings_row == NULL) break;
          unsigned long *lengths= lmysql->ldbms_mysql_fetch_lengths(mysql_res_for_warnings);
          unsigned long required_size= lengths[0] + lengths[1] + lengths[2] + 16;
          char *long_warning= new char[required_size];
          sprintf(long_warning, "\n%s (%s) %s.", warnings_row[0], warnings_row[1], warnings_row[2]);
          s1.append(long_warning);
          delete []long_warning;
        }
        lmysql->ldbms_mysql_free_result(mysql_res_for_warnings);
      }
    }
  }
  if (mysql_errno_result > 0)
  {
    s1= er_strings[er_off + ER_ERROR];
    sprintf(mysql_error_and_state, "%d (%s) ", mysql_errno_result, lmysql->ldbms_mysql_sqlstate(&mysql[connection_number]));
    s1.append(mysql_error_and_state);
    s2= lmysql->ldbms_mysql_error(&mysql[connection_number]);
    s1.append(s2);
  }
  statement_edit_widget->result= s1;
}


/*
  Called from execute_client_statement() and from debugger.
  Effect is like put_diagnostics_in_result() when the server returns an error,
  but without elapsed time or sqlstate or error number.
  Todo: consider putting in elapsed time.
*/
void MainWindow::put_message_in_result(QString s1)
{
  statement_edit_widget->result= s1;
}

/*
  Todo: truncate if length would be greater than ER_MAX_LENGTH
*/
void MainWindow::make_and_put_message_in_result(
        unsigned int er_number,
        int er_numeric_parameter,
        char *er_string_parameter)
{
  char buffer[ER_MAX_LENGTH];
  if (strstr(er_strings[er_off + er_number], "%s") != NULL)
    sprintf(buffer, er_strings[er_off + er_number], er_string_parameter);
  else
    sprintf(buffer, er_strings[er_off + er_number], er_numeric_parameter);
  put_message_in_result(buffer);
}

void MainWindow::make_and_append_message_in_result(
        unsigned int er_number,
        int er_numeric_parameter,
        char *er_string_parameter)
{
  char buffer[ER_MAX_LENGTH];
  if (strstr(er_strings[er_off + er_number], "%s") != NULL)
    sprintf(buffer, er_strings[er_off + er_number], er_string_parameter);
  else
    sprintf(buffer, er_strings[er_off + er_number], er_numeric_parameter);
  statement_edit_widget->result.append(buffer);
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

  If ocelot_dbms='tarantool':
    #... is not a comment
    [[...]] is a string
    .. is an operator
    == is an operator
    todo: =[[...]]= and nested strings

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

void MainWindow::tokenize(QChar *text, int text_length, int *token_lengths,
                           int *token_offsets, int max_tokens, QChar *version,
                           int passed_comment_behaviour, QString special_token,
                           int minus_behaviour)
{
  log("tokenize start", 50);
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
  token_lengths[token_number]= 0;
  token_offsets[token_number]= char_offset;
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
#ifdef DBMS_TARANTOOL
    if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
    {
      expected_char= '<';
      if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
      expected_char= '>';
      if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    }
#endif
    goto one_byte_token;
  }
  if (text[char_offset] == '"')    /* " starts a token until next " but watch for ""s and end-of-string and escape */
  {
    expected_char= '"';
    goto skip_till_expected_char;
  }
  if (text[char_offset] == '#')          /* # starts a comment */
  {
    if ((dbms_version_mask&FLAG_VERSION_TARANTOOL) != 0) goto one_byte_token;
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
  if (text[char_offset] == '-') /* MySQL 5.7.9 JSON-colum->path operator */
  {
    expected_char= '>';
    if ((char_offset + 1  < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
  }
  if (minus_behaviour != 2) {
    if ((dbms_version_mask&FLAG_VERSION_TARANTOOL) == 0)
    {
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
    else
    {
      if (text[char_offset] == '-')     /* - one-byte token, unless -- comment */
      {
        if ((char_offset + 1 < text_length) && (text[char_offset + 1]  == '-'))
        {
          if (((char_offset + 2 < text_length) && (text[char_offset + 2] == '['))
           && ((char_offset + 3 < text_length) && (text[char_offset + 3] == '[')))
          {
            goto string_starting_with_bracket_start;
          }
          else
          {
            expected_char= '\n';
            goto comment_start;
          }
        }
        goto one_byte_token;
      }
    }
  }
  if (text[char_offset] == '.')     /* . part of token if previous or following is digit. otherwise one-byte token */
  {
    if (char_offset + 1 < text_length)
    {
      if ((text[char_offset + 1] >= '0') && (text[char_offset + 1] <= '9')) goto part_of_token;
      /* .. is an operator in MariaDB 10.3 for ... end for */
      if ((text[char_offset + 1] == '.')
       && ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_3) != 0))
      {
        n= 2;
        goto n_byte_token;
      }
    }
    if (token_lengths[token_number] > 0)
    {
      if ((text[char_offset - 1] >= '0') && (text[char_offset - 1] <= '9'))
      {
        int j;
        for (j= token_offsets[token_number]; j < char_offset; ++j)
        {
          if (text[j] > '9') goto one_byte_token;
        }
        goto part_of_token;
      }
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
    if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0) expected_char= ':';
    else expected_char= '=';
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
  if (text[char_offset] == '=')
  {
#ifdef DBMS_TARANTOOL
    if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
    {
      expected_char= '=';
      if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    }
#endif
    goto one_byte_token;          /* = one-byte token */
  }
  if (text[char_offset] == '>')   /* > might be start of >= or >>. otherwise one-byte token */
  {
    expected_char= '=';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    expected_char= '>';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == '?') goto one_byte_token; /* ? one-byte token? */
  if (text[char_offset] == '@') /* @ start of @token or start of @@token otherwise one-byte token */
  {
    if (token_lengths[token_number] == 0) goto part_of_token;
    if (token_lengths[token_number] == 1)
    {
      if (text[char_offset - 1] == '@') goto part_of_token;
    }
    goto one_byte_token;
  }
  if (text[char_offset] == '[')
  {
    if ((dbms_version_mask&FLAG_VERSION_TARANTOOL) != 0)
    {
      expected_char= '[';
      if ((char_offset + 1 < text_length) && (text[char_offset + 1] == '[')) goto string_starting_with_bracket_start;
    }
    goto one_byte_token; /* [ one-byte token which is never used */
  }
  if (text[char_offset] == 92)  goto one_byte_token; /* \ one-byte token which is never used */
  if (text[char_offset] == ']') goto one_byte_token; /* ] one-byte token which is never used */
  if (text[char_offset] == '^') goto one_byte_token; /* ^ one-byte token */
  if (text[char_offset] == '`')        /* ` starts a token until next ` but watch for end-of-string and escape */
  {
    expected_char= '`';
    goto skip_till_expected_char;
  }
  /* Changed handling of {...} on February 1 2016 */
  if (text[char_offset] == '{') goto one_byte_token;
  if (text[char_offset] == '}') goto one_byte_token;
  //if (text[char_offset] == '{')      /* { starts a token until next } but watch for end-of-string */
  //{
  //  expected_char= '}';
  //  goto skip_till_expected_char;
  //}
  if (text[char_offset] == '|')      /* | might be start of ||. otherwise one-byte token */
  {
    expected_char= '|';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == '}') goto one_byte_token; /* } one-byte token which is never used */
  if (text[char_offset] == '~') /* ~ one-byte token unless Tarantool+Lua */
  {
#ifdef DBMS_TARANTOOL
    if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0)
    {
      expected_char= '=';
      if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    }
#endif
   goto one_byte_token;
  }
  /* Remaining possibilities are:
     $, 0 to 9, A to Z, a to z
     >127 (top-bit-on is probably a continuation byte of a utf-8 character)
     _ DEL */
part_of_token:
  ++char_offset;
  ++token_lengths[token_number];
  goto next_char;
string_end:
  if (token_lengths[token_number] > 0) token_lengths[token_number + 1]= 0;
  log("tokenize end", 50);
  return;
white_space:
  if (token_lengths[token_number] > 0) ++token_number;
  ++char_offset;
  goto next_token;
string_starting_with_bracket_start:
  if (token_lengths[token_number] > 0)
  {
    ++token_number;
    token_lengths[token_number]= 0;
    token_offsets[token_number]= char_offset;
  }
  for (;;)
  {
    if (char_offset >= text_length) goto string_end;
    if (text[char_offset] == 0) goto string_end;
    if ((char_offset + 1 < text_length)
     && (text[char_offset] == ']')
     && (text[char_offset + 1] == ']'))
     {
       char_offset+= 2;
       token_lengths[token_number]+= 2;
       break;
     }
    ++char_offset;
    ++token_lengths[token_number];
  }
  ++token_number;
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
  if (token_lengths[token_number] == 1)
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
  if (token_lengths[token_number] > 0)
  {
    ++token_number;
    token_lengths[token_number]= 0;
    token_offsets[token_number]= char_offset;
  }
skip_till_expected_char_2:
  ++char_offset;
  ++token_lengths[token_number];
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
      ++token_lengths[token_number];
      goto skip_till_expected_char_2;
    }
  }
  ++char_offset;
  ++token_lengths[token_number];
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
  if (token_lengths[token_number] > 0) ++token_number;
  token_offsets[token_number]= char_offset;
  token_lengths[token_number]= 1;
  ++char_offset;
  ++token_number;
  goto next_token;
n_byte_token:   /* we know that coming token length is n */
  if (token_lengths[token_number] > 0) ++token_number;
  token_offsets[token_number]= char_offset;
  token_lengths[token_number]= n;
  char_offset+= n;
  ++token_number;
  goto next_token;
n_byte_token_skip:
  if (token_lengths[token_number] > 0) ++token_number;
  char_offset+= n;
  goto next_token;
}


/*
  token_type() should be useful for syntax highlighting and for hovering.
  However, it's rudimentary. If parsing routines are called, they'll override.
  Pass: token, token length. we assume it's at least 1.
  Return: type
  I could have figured this out during tokenize(), but didn't.
  See also tokens_to_keywords().
*/
int MainWindow::token_type(QChar *token, int token_length, bool ansi_quotes)
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
    if ((*token == 'N') || (*token == 'X') || (*token == 'B')
     || (*token == 'n') || (*token == 'x') || (*token == 'b'))
    {
     if (*(token + 1) == 39) return TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE;
    }
    if ((*token == '[') && (*(token + 1) == '['))
    {
      return TOKEN_TYPE_LITERAL_WITH_BRACKET;
    }
  }
  if (*token == '"')
  {
    if (ansi_quotes) return TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE;
    return TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE;
  }
  if ((*token >= '0') && (*token <= '9')) return TOKEN_TYPE_LITERAL_WITH_DIGIT;
  if ((token_length > 1) && (*token == '.'))
  {
    if ((*(token + 1)>= '0') && (*(token + 1) <= '9')) return TOKEN_TYPE_LITERAL_WITH_DIGIT;
  }
  //if (*token == '{') return TOKEN_TYPE_LITERAL_WITH_BRACE;
  if ((*token == '{') || (*token == '}')) return TOKEN_TYPE_OPERATOR;
  if (*token == '`') return TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK;
  if (*token == '@') return TOKEN_TYPE_IDENTIFIER_WITH_AT;
  if (token_length > 1)
  {
    if ((*token == '/') && (*(token + 1) == '*')) return TOKEN_TYPE_COMMENT_WITH_SLASH;
    if ((*token == '*') && (*(token + 1) == '/')) return TOKEN_TYPE_COMMENT_WITH_SLASH;
  }
  if (*token == '#')
  {
    if (dbms_version_mask&FLAG_VERSION_TARANTOOL) return TOKEN_TYPE_OPERATOR;
    return TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE;
  }
  if (token_length > 1)
  {
    if ((*token == '-') && (*(token + 1) == '-')) return TOKEN_TYPE_COMMENT_WITH_MINUS;
  }
  if ((*token > ' ') && (*token < 'A') && (*token != '$')) return TOKEN_TYPE_OPERATOR;
  return TOKEN_TYPE_OTHER;
}


/*
  I got tired of repeating
  if (QString::compare(text.mid(main_token_offsets[0], main_token_lengths[0]), "SELECT", Qt::CaseInsensitive) == 0) ...
  so switched to maintaining a permanent list.
  Two compiler-dependent assumptions: bsearch() exists, and char* can be converted to unsigned long.
  Upper case is reserved, lower case is unreserved, we search both ways.
  Warning: We can call this from hparse routines.
*/
/* Todo: use "const" and "static" more often */

void MainWindow::tokens_to_keywords(QString text, int start, bool ansi_quotes)
{
  log("tokens_to_keywords start", 80);
  /*
    Sorted list of keywords.
    If you change this, you must also change bsearch parameters and change TOKEN_KEYWORD list.
    We consider introducers e.g. _UTF8 to be equivalent to reserved words.
  */

#define MAX_KEYWORD_LENGTH 34
struct keywords {
   char  chars[MAX_KEYWORD_LENGTH];
   unsigned short int reserved_flags;
   unsigned short int built_in_function_flags;
   unsigned short int token_keyword;
};
const keywords strvalues[]=
    {
      {"?", 0, 0, TOKEN_KEYWORD_QUESTIONMARK}, /* Ocelot keyword, although tokenize() regards it as an operator */
      {"ABORT", 0, 0, TOKEN_KEYWORD_ABORT},
      {"ABS", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_ABS},
      {"ACCESSIBLE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_ACCESSIBLE},
      {"ACOS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ACOS},
      {"ACTION", 0, 0, TOKEN_KEYWORD_ACTION},
      {"ADD", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_ADD},
      {"ADDDATE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ADDDATE},
      {"ADDTIME", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ADDTIME},
      {"AES_DECRYPT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_AES_DECRYPT},
      {"AES_ENCRYPT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_AES_ENCRYPT},
      {"AFTER", 0, 0, TOKEN_KEYWORD_AFTER},
      {"AGAINST", 0, 0, TOKEN_KEYWORD_AGAINST},
      {"ALGORITHM", 0, 0, TOKEN_KEYWORD_ALGORITHM},
      {"ALL", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_ALL},
      {"ALTER", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_ALTER},
      {"ALWAYS", 0, 0, TOKEN_KEYWORD_ALWAYS},
      {"ANALYZE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_ANALYZE},
      {"AND", FLAG_VERSION_ALL | FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_AND},
      {"ANY", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_ANY},
      {"ANY_VALUE", 0, FLAG_VERSION_MYSQL_ALL, TOKEN_KEYWORD_ANY_VALUE},
      {"AREA", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_AREA}, /* deprecated in MySQL 5.7.6 */
      {"AS", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_AS},
      {"ASBINARY", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ASBINARY}, /* deprecated in MySQL 5.7.6 */
      {"ASC", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_ASC},
      {"ASCII", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ASCII},
      {"ASENSITIVE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_ASENSITIVE},
      {"ASIN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ASIN},
      {"ASTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ASTEXT}, /* deprecated in MySQL 5.7.6 */
      {"ASWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ASWKB}, /* deprecated in MySQL 5.7.6 */
      {"ASWKT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ASWKT}, /* deprecated in MySQL 5.7.6 */
      {"ASYMMETRIC_DECRYPT", 0, FLAG_VERSION_MYSQL_ALL, TOKEN_KEYWORD_ASYMMETRIC_DECRYPT},
      {"ASYMMETRIC_DERIVE", 0, FLAG_VERSION_MYSQL_ALL, TOKEN_KEYWORD_ASYMMETRIC_DERIVE},
      {"ASYMMETRIC_ENCRYPT", 0, FLAG_VERSION_MYSQL_ALL, TOKEN_KEYWORD_ASYMMETRIC_ENCRYPT},
      {"ASYMMETRIC_SIGN", 0, FLAG_VERSION_MYSQL_ALL, TOKEN_KEYWORD_ASYMMETRIC_SIGN},
      {"ASYMMETRIC_VERIFY", 0, FLAG_VERSION_MYSQL_ALL, TOKEN_KEYWORD_ASYMMETRIC_VERIFY},
      {"ATAN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ATAN},
      {"ATAN2", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ATAN2},
      {"ATOMIC", 0, 0, TOKEN_KEYWORD_ATOMIC},
      {"ATTACH", 0, 0, TOKEN_KEYWORD_ATTACH},
      {"AUTOINCREMENT", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_AUTOINCREMENT},
      {"AVG", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_AVG},
          {"BACKUP_ADMIN", 0, 0, TOKEN_KEYWORD_BACKUP_ADMIN},
      {"BEFORE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_BEFORE},
      {"BEGIN", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_BEGIN},
      {"BENCHMARK", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_BENCHMARK},
      {"BETWEEN", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_BETWEEN},
      {"BFILE", 0, 0, TOKEN_KEYWORD_BFILE},
      {"BIGINT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_BIGINT},
      {"BIN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_BIN},
      {"BINARY", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_BINARY},
      {"BINARY_DOUBLE", 0, 0, TOKEN_KEYWORD_BINARY_DOUBLE},
      {"BINARY_FLOAT", 0, 0, TOKEN_KEYWORD_BINARY_FLOAT},
      {"BINLOG", FLAG_VERSION_MARIADB_10_0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_BINLOG},
          {"BINLOG_ADMIN", 0, 0, TOKEN_KEYWORD_BINLOG_ADMIN},
      {"BINLOG_GTID_POS", FLAG_VERSION_MARIADB_10_0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_BINLOG_GTID_POS},
      {"BIT", 0, 0, TOKEN_KEYWORD_BIT},
      {"BIT_AND", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_BIT_AND},
      {"BIT_COUNT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_BIT_COUNT},
      {"BIT_LENGTH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_BIT_LENGTH},
      {"BIT_OR", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_BIT_OR},
      {"BIT_XOR", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_BIT_XOR},
      {"BLOB", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_BLOB},
      {"BODY", FLAG_VERSION_PLSQL, 0, TOKEN_KEYWORD_BODY},
      {"BOOL", 0, 0, TOKEN_KEYWORD_BOOL},
      {"BOOLEAN", 0, 0, TOKEN_KEYWORD_BOOLEAN},
      {"BOTH", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_BOTH},
      {"BREAK", FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_BREAK},
      {"BUFFER", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_BUFFER}, /* deprecated in MySQL 5.7.6 */
      {"BY", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_BY},
      {"CALL", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_CALL},
      {"CASCADE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_CASCADE},
      {"CASE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_CASE},
      {"CAST", FLAG_VERSION_TARANTOOL, FLAG_VERSION_ALL, TOKEN_KEYWORD_CAST},
      {"CEIL", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CEIL},
      {"CEILING", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CEILING},
      {"CENTROID", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CENTROID}, /* deprecated in MySQL 5.7.6 */
      {"CHANGE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_CHANGE},
      {"CHANGES", 0, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_CHANGES},
      {"CHAR", FLAG_VERSION_ALL, FLAG_VERSION_ALL, TOKEN_KEYWORD_CHAR},
      {"CHARACTER", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_CHARACTER},
      {"CHARACTER_LENGTH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CHARACTER_LENGTH},
      {"CHARSET", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CHARSET},  /* + Ocelot keyword */
      {"CHAR_LENGTH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CHAR_LENGTH},
      {"CHECK", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_CHECK},
      {"CLEAR", 0, 0, TOKEN_KEYWORD_CLEAR}, /* Ocelot keyword */
      {"CLOB", 0, 0, TOKEN_KEYWORD_CLOB},
      {"CLOSE", 0, 0, TOKEN_KEYWORD_CLOSE},
      {"COALESCE", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_COALESCE},
      {"COERCIBILITY", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_COERCIBILITY},
      {"COLLATE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_COLLATE},
      {"COLLATION", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_COLLATION},
      {"COLUMN", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_COLUMN},
      {"COLUMN_ADD", FLAG_VERSION_MARIADB_10_0, FLAG_VERSION_MARIADB_10_0, TOKEN_KEYWORD_COLUMN_ADD},
      {"COLUMN_CHECK", FLAG_VERSION_MARIADB_10_0, FLAG_VERSION_MARIADB_10_0, TOKEN_KEYWORD_COLUMN_CHECK},
      {"COLUMN_CREATE", FLAG_VERSION_MARIADB_10_0, FLAG_VERSION_MARIADB_10_0, TOKEN_KEYWORD_COLUMN_CREATE},
      {"COLUMN_DELETE", FLAG_VERSION_MARIADB_10_0, FLAG_VERSION_MARIADB_10_0, TOKEN_KEYWORD_COLUMN_DELETE},
      {"COLUMN_EXISTS", FLAG_VERSION_MARIADB_10_0, FLAG_VERSION_MARIADB_10_0, TOKEN_KEYWORD_COLUMN_EXISTS},
      {"COLUMN_GET", FLAG_VERSION_MARIADB_10_0, FLAG_VERSION_MARIADB_10_0, TOKEN_KEYWORD_COLUMN_GET},
      {"COLUMN_JSON", FLAG_VERSION_MARIADB_10_0, FLAG_VERSION_MARIADB_10_0, TOKEN_KEYWORD_COLUMN_JSON},
      {"COLUMN_LIST", FLAG_VERSION_MARIADB_10_0, FLAG_VERSION_MARIADB_10_0, TOKEN_KEYWORD_COLUMN_LIST},
      {"COMMENT", 0, 0, TOKEN_KEYWORD_COMMENT},
      {"COMMIT", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_COMMIT},
      {"COMPACT", 0, 0, TOKEN_KEYWORD_COMPACT},
      {"COMPRESS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_COMPRESS},
      {"COMPRESSED", 0, 0, TOKEN_KEYWORD_COMPRESSED},
      {"COMPRESSION", 0, 0, TOKEN_KEYWORD_COMPRESSION},
      {"CONCAT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CONCAT},
      {"CONCAT_WS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CONCAT_WS},
      {"CONDITION", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_CONDITION},
      {"CONFLICT", 0, 0, TOKEN_KEYWORD_CONFLICT},
      {"CONNECT", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_CONNECT}, /* Ocelot keyword */
          {"CONNECTION_ADMIN", 0, 0, TOKEN_KEYWORD_CONNECTION_ADMIN},
      {"CONNECTION_ID", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CONNECTION_ID},
      {"CONSTRAINT", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_CONSTRAINT},
      {"CONTAINS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CONTAINS}, /* deprecated in MySQL 5.7.6 */
      {"CONTINUE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_CONTINUE},
      {"CONV", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CONV},
      {"CONVERT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CONVERT},
      {"CONVERT_TZ", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CONVERT_TZ},
      {"CONVEXHULL", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CONVEXHULL}, /* deprecated in MySQL 5.7.6 */
      {"COS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_COS},
      {"COT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_COT},
      {"COUNT", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_COUNT},
      {"CRC32", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CRC32},
      {"CREATE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_CREATE},
      {"CREATE_ASYMMETRIC_PRIV_KEY", 0, FLAG_VERSION_MYSQL_ALL, TOKEN_KEYWORD_CREATE_ASYMMETRIC_PRIV_KEY},
      {"CREATE_ASYMMETRIC_PUB_KEY", 0, FLAG_VERSION_MYSQL_ALL, TOKEN_KEYWORD_CREATE_ASYMMETRIC_PUB_KEY},
      {"CREATE_DH_PARAMETERS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CREATE_DH_PARAMETERS},
      {"CREATE_DIGEST", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CREATE_DIGEST},
      {"CROSS", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_CROSS},
      {"CROSSES", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CROSSES}, /* deprecated in MySQL 5.7.6 */
    {"CUBE", FLAG_VERSION_MYSQL_8_0, 0, TOKEN_KEYWORD_CUBE},
      {"CUME_DIST", FLAG_VERSION_MYSQL_8_0, FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_MARIADB_10_2_2, TOKEN_KEYWORD_CUME_DIST},
      {"CURDATE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CURDATE},
      {"CURRENT", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_CURRENT},
      {"CURRENT_DATE", FLAG_VERSION_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CURRENT_DATE},
      {"CURRENT_ROLE", FLAG_VERSION_MARIADB_10_0, 0, TOKEN_KEYWORD_CURRENT_ROLE},
      {"CURRENT_TIME", FLAG_VERSION_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CURRENT_TIME},
      {"CURRENT_TIMESTAMP", FLAG_VERSION_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CURRENT_TIMESTAMP},
      {"CURRENT_USER", FLAG_VERSION_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CURRENT_USER},
      {"CURSOR", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_CURSOR},
      {"CURTIME", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_CURTIME},
      {"CYCLE", 0, 0, TOKEN_KEYWORD_CYCLE},
      {"DATABASE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DATABASE},
      {"DATABASES", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_DATABASES},
      {"DATE", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_DATE},
      {"DATEDIFF", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DATEDIFF},
      {"DATETIME", 0, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_DATETIME},
      {"DATE_ADD", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DATE_ADD},
      {"DATE_FORMAT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DATE_FORMAT},
      {"DATE_SUB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DATE_SUB},
      {"DAY", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DAY},
      {"DAYNAME", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DAYNAME},
      {"DAYOFMONTH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DAYOFMONTH},
      {"DAYOFWEEK", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DAYOFWEEK},
      {"DAYOFYEAR", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DAYOFYEAR},
      {"DAY_HOUR", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_DAY_HOUR},
      {"DAY_MICROSECOND", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_DAY_MICROSECOND},
      {"DAY_MINUTE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_DAY_MINUTE},
      {"DAY_SECOND", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_DAY_SECOND},
      {"DEALLOCATE", 0, 0, TOKEN_KEYWORD_DEALLOCATE},
      {"DEC", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_DEC},
      {"DECIMAL", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_DECIMAL},
      {"DECLARE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_DECLARE},
      {"DECODE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DECODE},
      {"DECODE_HISTOGRAM", FLAG_VERSION_MARIADB_10_0, FLAG_VERSION_MARIADB_10_0, TOKEN_KEYWORD_DECODE_HISTOGRAM},
      {"DEFAULT", FLAG_VERSION_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DEFAULT},
      {"DEFERRABLE", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_DEFERRABLE},
      {"DEFERRED", 0, 0, TOKEN_KEYWORD_DEFERRED},
      {"DEGREES", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DEGREES},
      {"DELAYED", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_DELAYED},
      {"DELETE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_DELETE},
      {"DELIMITER", 0, 0, TOKEN_KEYWORD_DELIMITER}, /* Ocelot keyword */
      {"DENSE_RANK", FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_TARANTOOL, FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_MARIADB_10_2_2, TOKEN_KEYWORD_DENSE_RANK},
      {"DESC", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_DESC},
      {"DESCRIBE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_DESCRIBE},
      {"DES_DECRYPT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DES_DECRYPT}, /* deprecated in MySQL 5.7.6 */
      {"DES_ENCRYPT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DES_ENCRYPT}, /* deprecated in MySQL 5.7.6 */
      {"DETACH", 0, 0, TOKEN_KEYWORD_DETACH},
      {"DETERMINISTIC", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_DETERMINISTIC},
      {"DIMENSION", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DIMENSION}, /* deprecated in MySQL 5.7.6 */
      {"DIRECTORY", 0, 0, TOKEN_KEYWORD_DIRECTORY},
      {"DISJOINT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DISJOINT}, /* deprecated in MySQL 5.7.6 */
      {"DISTANCE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_DISTANCE}, /* deprecated in MySQL 5.7.6 */
      {"DISTINCT", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_DISTINCT},
      {"DISTINCTROW", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_DISTINCTROW},
      {"DIV", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_DIV},
      {"DO", FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_DO},
      {"DOUBLE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_DOUBLE},
      {"DROP", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_DROP},
      {"DUAL", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_DUAL},
      {"DUPLICATE", 0, 0, TOKEN_KEYWORD_DUPLICATE},
      {"DYNAMIC", 0, 0, TOKEN_KEYWORD_DYNAMIC},
      {"EACH", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_EACH},
      {"EDIT", 0, 0, TOKEN_KEYWORD_EDIT}, /* Ocelot keyword */
      {"EGO", 0, 0, TOKEN_KEYWORD_EGO}, /* Ocelot keyword */
      {"ELSE", FLAG_VERSION_ALL | FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_ELSE},
      {"ELSEIF", FLAG_VERSION_ALL | FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_ELSEIF},
      {"ELSIF", FLAG_VERSION_PLSQL, 0, TOKEN_KEYWORD_ELSIF},
      {"ELT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ELT},
    {"EMPTY", FLAG_VERSION_MYSQL_8_0, 0, TOKEN_KEYWORD_EMPTY},
      {"ENABLE", 0, 0, TOKEN_KEYWORD_ENABLE},
      {"ENCLOSED", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_ENCLOSED},
      {"ENCODE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ENCODE},
      {"ENCRYPT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ENCRYPT}, /* deprecated in MySQL 5.7.6 */
          {"ENCRYPTION_KEY_ADMIN", 0, 0, TOKEN_KEYWORD_ENCRYPTION_KEY_ADMIN},
      {"END", FLAG_VERSION_TARANTOOL | FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_END},
      {"ENDPOINT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ENDPOINT}, /* deprecated in MySQL 5.7.6 */
      {"ENUM", 0, 0, TOKEN_KEYWORD_ENUM},
      {"ENVELOPE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ENVELOPE}, /* deprecated in MySQL 5.7.6 */
      {"EQUALS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_EQUALS}, /* deprecated in MySQL 5.7.6 */
      {"ESCAPE", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_ESCAPE},
      {"ESCAPED", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_ESCAPED},
      {"EVENT", 0, 0, TOKEN_KEYWORD_EVENT},
      {"EXCEPT", FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_3, 0, TOKEN_KEYWORD_EXCEPT},
      {"EXCEPTION", 0, 0, TOKEN_KEYWORD_EXCEPTION},
      {"EXCHANGE", 0, 0, TOKEN_KEYWORD_EXCHANGE},
      {"EXCLUSIVE", 0, 0, TOKEN_KEYWORD_EXCLUSIVE},
      {"EXECUTE", 0, 0, TOKEN_KEYWORD_EXECUTE},
      {"EXISTS", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_EXISTS},
      {"EXIT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_EXIT},
      {"EXP", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_EXP},
      {"EXPANSION", 0, 0, TOKEN_KEYWORD_EXPANSION},
      {"EXPLAIN", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_EXPLAIN},
      {"EXPORT", 0, 0, TOKEN_KEYWORD_EXPORT},
      {"EXPORT_SET", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_EXPORT_SET},
      {"EXTERIORRING", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_EXTERIORRING}, /* deprecated in MySQL 5.7.6 */
      {"EXTRACT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_EXTRACT},
      {"EXTRACTVALUE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_EXTRACTVALUE},
      {"FAIL", 0, 0, TOKEN_KEYWORD_FAIL},
      {"FALSE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_FALSE},
      {"FETCH", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_FETCH},
      {"FIELD", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_FIELD},
      {"FILE", 0, 0, TOKEN_KEYWORD_FILE},
      {"FIND_IN_SET", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_FIND_IN_SET},
          {"FIREWALL_ADMIN", 0, 0, TOKEN_KEYWORD_FIREWALL_ADMIN},
          {"FIREWALL_USER", 0, 0, TOKEN_KEYWORD_FIREWALL_USER},
      {"FIRST", 0, 0, TOKEN_KEYWORD_FIRST}, /* MariaDB 10.2 nonreserved */
      {"FIRST_VALUE", FLAG_VERSION_MYSQL_8_0, 0, TOKEN_KEYWORD_FIRST_VALUE}, /* MariaDB 10.2 nonreserved -- or, maybe not in MariaDB 10.2*/
      {"FIXED", 0, 0, TOKEN_KEYWORD_FIXED},
      {"FLOAT", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_FLOAT},
      {"FLOAT4", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_FLOAT4},
      {"FLOAT8", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_FLOAT8},
      {"FLOOR", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_FLOOR},
      {"FLUSH", 0, 0, TOKEN_KEYWORD_FLUSH},
      {"FOLLOWING", FLAG_VERSION_MARIADB_10_2_2, 0, TOKEN_KEYWORD_FOLLOWING},
      {"FOLLOWS", FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_MARIADB_10_2_3, 0, TOKEN_KEYWORD_FOLLOWS},
      {"FOR", FLAG_VERSION_ALL | FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_FOR},
      {"FORCE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_FORCE},
      {"FOREIGN", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_FOREIGN},
      {"FORMAT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_FORMAT},
      {"FOUND_ROWS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_FOUND_ROWS},
      {"FROM", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_FROM},
      {"FROM_BASE64", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_FROM_BASE64},
      {"FROM_DAYS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_FROM_DAYS},
      {"FROM_UNIXTIME", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_FROM_UNIXTIME},
      {"FULL", 0, 0, TOKEN_KEYWORD_FULL},
      {"FULLTEXT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_FULLTEXT},
      {"FUNCTION", FLAG_VERSION_MYSQL_8_0 | FLAG_VERSION_TARANTOOL | FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_FUNCTION},
      {"GENERAL", 0, 0, TOKEN_KEYWORD_GENERAL},
      {"GENERATED", FLAG_VERSION_MYSQL_5_7, 0, TOKEN_KEYWORD_GENERATED},
      {"GEOMCOLLFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GEOMCOLLFROMTEXT}, /* deprecated in MySQL 5.7.6 */
      {"GEOMCOLLFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GEOMCOLLFROMWKB}, /* deprecated in MySQL 5.7.6 */
      {"GEOMETRY", 0, 0, TOKEN_KEYWORD_GEOMETRY},
      {"GEOMETRYCOLLECTION", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GEOMETRYCOLLECTION},
      {"GEOMETRYCOLLECTIONFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GEOMETRYCOLLECTIONFROMTEXT}, /* deprecated in MySQL 5.7.6 */
      {"GEOMETRYCOLLECTIONFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GEOMETRYCOLLECTIONFROMWKB}, /* deprecated in MySQL 5.7.6 */
      {"GEOMETRYFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GEOMETRYFROMTEXT}, /* deprecated in MySQL 5.7.6 */
      {"GEOMETRYFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GEOMETRYFROMWKB}, /* deprecated in MySQL 5.7.6 */
      {"GEOMETRYN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GEOMETRYN}, /* deprecated in MySQL 5.7.6 */
      {"GEOMETRYTYPE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GEOMETRYTYPE}, /* deprecated in MySQL 5.7.6 */
      {"GEOMFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GEOMFROMTEXT},/* deprecated in MySQL 5.7.6 */
      {"GEOMFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GEOMFROMWKB}, /* deprecated in MySQL 5.7.6 */
      {"GET", FLAG_VERSION_TARANTOOL | FLAG_VERSION_MYSQL_5_6, 0, TOKEN_KEYWORD_GET},
      {"GET_FORMAT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GET_FORMAT},
      {"GET_LOCK", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GET_LOCK},
      {"GLENGTH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GLENGTH}, /* deprecated in MySQL 5.7.6 */
      {"GLOB", FLAG_VERSION_TARANTOOL, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_GLOB},
      {"GLOBAL", 0, 0, TOKEN_KEYWORD_GLOBAL},
      {"GO", 0, 0, TOKEN_KEYWORD_GO}, /* Ocelot keyword */
      {"GOTO", FLAG_VERSION_LUA|FLAG_VERSION_PLSQL, 0, TOKEN_KEYWORD_GOTO},
      {"GRANT", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_GRANT},
      {"GREATEST", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GREATEST},
      {"GROUP", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_GROUP},
    {"GROUPING", FLAG_VERSION_MYSQL_8_0, FLAG_VERSION_MYSQL_8_0, TOKEN_KEYWORD_GROUPING},
    {"GROUPS", FLAG_VERSION_MYSQL_8_0, 0, TOKEN_KEYWORD_GROUPS},
      {"GROUP_CONCAT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GROUP_CONCAT},
          {"GROUP_REPLICATION_ADMIN", 0, 0, TOKEN_KEYWORD_GROUP_REPLICATION_ADMIN},
      {"GTID_SUBSET", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GTID_SUBSET},
      {"GTID_SUBTRACT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_GTID_SUBTRACT},
      {"HANDLER", 0, 0, TOKEN_KEYWORD_HANDLER},
      {"HAVING", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_HAVING},
      {"HELP", 0, 0, TOKEN_KEYWORD_HELP}, /* Ocelot keyword */
      {"HEX", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_HEX},
      {"HIGH_PRIORITY", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_HIGH_PRIORITY},
      {"HOUR", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_HOUR},
      {"HOUR_MICROSECOND", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_HOUR_MICROSECOND},
      {"HOUR_MINUTE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_HOUR_MINUTE},
      {"HOUR_SECOND", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_HOUR_SECOND},
      {"IF", FLAG_VERSION_ALL | FLAG_VERSION_LUA, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_IF},
      {"IFNULL", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_IFNULL},
      {"IGNORE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_IGNORE},
      {"IMMEDIATE", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_IMMEDIATE},
      {"IMPORT", 0, 0, TOKEN_KEYWORD_IMPORT},
      {"IN", FLAG_VERSION_ALL | FLAG_VERSION_LUA, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_IN},
      {"INDEX", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_INDEX},
      {"INDEXED", 0, 0, TOKEN_KEYWORD_INDEXED},
      {"INET6_ATON", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_INET6_ATON},
      {"INET6_NTOA", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_INET6_NTOA},
      {"INET_ATON", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_INET_ATON},
      {"INET_NTOA", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_INET_NTOA},
      {"INFILE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_INFILE},
      {"INITIALLY", 0, 0, TOKEN_KEYWORD_INITIALLY},
      {"INNER", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_INNER},
      {"INOUT", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_INOUT},
      {"INSENSITIVE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_INSENSITIVE},
      {"INSERT", FLAG_VERSION_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_INSERT},
      {"INSTALL", 0, 0, TOKEN_KEYWORD_INSTALL},
      {"INSTEAD", 0, 0, TOKEN_KEYWORD_INSTEAD},
      {"INSTR", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_INSTR},
      {"INT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_INT},
      {"INT1", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_INT1},
      {"INT2", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_INT2},
      {"INT3", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_INT3},
      {"INT4", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_INT4},
      {"INT8", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_INT8},
      {"INTEGER", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_INTEGER},
      {"INTERIORRINGN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_INTERIORRINGN}, /* deprecated in MySQL 5.7.6 */
      {"INTERSECT", FLAG_VERSION_TARANTOOL|FLAG_VERSION_MARIADB_10_3, 0, TOKEN_KEYWORD_INTERSECT},
      {"INTERSECTS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_INTERSECTS}, /* deprecated in MySQL 5.7.6 */
      {"INTERVAL", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_INTERVAL},
      {"INTO", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_INTO},
      {"IO_AFTER_GTIDS", FLAG_VERSION_MYSQL_5_6, 0, TOKEN_KEYWORD_IO_AFTER_GTIDS},
      {"IO_BEFORE_GTIDS", FLAG_VERSION_MYSQL_5_6, 0, TOKEN_KEYWORD_IO_BEFORE_GTIDS},
      {"IS", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_IS},
      {"ISCLOSED", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ISCLOSED}, /* deprecated in MySQL 5.7.6 */
      {"ISEMPTY", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ISEMPTY}, /* deprecated in MySQL 5.7.6 */
      {"ISNULL", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_ISNULL},
      {"ISSIMPLE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ISSIMPLE}, /* deprecated in MySQL 5.7.6 */
      {"IS_FREE_LOCK", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_IS_FREE_LOCK},
      {"IS_IPV4", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_IS_IPV4},
      {"IS_IPV4_COMPAT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_IS_IPV4_COMPAT},
      {"IS_IPV4_MAPPED", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_IS_IPV4_MAPPED},
      {"IS_IPV6", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_IS_IPV6},
      {"IS_USED_LOCK", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_IS_USED_LOCK},
      {"ITERATE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_ITERATE},
      {"JOIN", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_JOIN},
      {"JSON", 0, 0, TOKEN_KEYWORD_JSON},
      {"JSON_APPEND", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_MARIADB_10_2_3, TOKEN_KEYWORD_JSON_APPEND},
      {"JSON_ARRAY", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_MARIADB_10_2_3 | FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_JSON_ARRAY},
      {"JSON_ARRAY_APPEND", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_MARIADB_10_2_3, TOKEN_KEYWORD_JSON_ARRAY_APPEND},
      {"JSON_ARRAY_INSERT", 0, FLAG_VERSION_MYSQL_5_7, TOKEN_KEYWORD_JSON_ARRAY_INSERT},
      {"JSON_CONTAINS", 0, FLAG_VERSION_MYSQL_5_7, TOKEN_KEYWORD_JSON_CONTAINS},
      {"JSON_CONTAINS_PATH", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_MARIADB_10_2_3, TOKEN_KEYWORD_JSON_CONTAINS_PATH},
      {"JSON_DEPTH", 0, FLAG_VERSION_MYSQL_5_7, TOKEN_KEYWORD_JSON_DEPTH},
      {"JSON_EXTRACT", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_MARIADB_10_2_3 | FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_JSON_EXTRACT},
      {"JSON_INSERT", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_JSON_INSERT},
      {"JSON_KEYS", 0, FLAG_VERSION_MYSQL_5_7, TOKEN_KEYWORD_JSON_KEYS},
      {"JSON_LENGTH", 0, FLAG_VERSION_MYSQL_5_7, TOKEN_KEYWORD_JSON_LENGTH},
      {"JSON_MERGE", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_MARIADB_10_2_3, TOKEN_KEYWORD_JSON_MERGE},
      {"JSON_OBJECT", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_MARIADB_10_2_3 | FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_JSON_OBJECT},
      {"JSON_QUERY", 0, FLAG_VERSION_MARIADB_10_2_3, TOKEN_KEYWORD_JSON_QUERY},
      {"JSON_QUOTE", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_MARIADB_10_2_3, TOKEN_KEYWORD_JSON_QUOTE},
      {"JSON_REMOVE", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_JSON_REMOVE},
      {"JSON_REPLACE", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_JSON_REPLACE},
      {"JSON_SEARCH", 0, FLAG_VERSION_MYSQL_5_7, TOKEN_KEYWORD_JSON_SEARCH},
      {"JSON_SET", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_JSON_SET},
    {"JSON_TABLE", FLAG_VERSION_MYSQL_8_0, 0, TOKEN_KEYWORD_JSON_TABLE},
      {"JSON_TYPE", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_JSON_TYPE},
      {"JSON_UNQUOTE", 0, FLAG_VERSION_MYSQL_5_7, TOKEN_KEYWORD_JSON_UNQUOTE},
      {"JSON_VALID", 0, FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_MARIADB_10_2_3 | FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_JSON_VALID},
      {"JSON_VALUE", 0, FLAG_VERSION_MARIADB_10_2_3, TOKEN_KEYWORD_JSON_VALUE},
      {"JULIANDAY", 0, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_JULIANDAY},
      {"KEY", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_KEY},
      {"KEYS", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_KEYS},
      {"KILL", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_KILL},
      {"LAG", FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_MARIADB_10_3, FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_MARIADB_10_3, TOKEN_KEYWORD_LAG}, /* MariaDB 10.2 nonreserved -- or, maybe not in MariaDB 10.2 */
      {"LANGUAGE", 0, 0, TOKEN_KEYWORD_LANGUAGE},
      {"LAST", 0, 0, TOKEN_KEYWORD_LAST}, /* MariaDB 10.2 nonreserved */
      {"LASTVAL", 0, FLAG_VERSION_MARIADB_10_3, TOKEN_KEYWORD_LASTVAL},
      {"LAST_DAY", FLAG_VERSION_MARIADB_10_0, FLAG_VERSION_MARIADB_10_0, TOKEN_KEYWORD_LAST_DAY},
      {"LAST_INSERT_ID", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LAST_INSERT_ID},
      {"LAST_VALUE", FLAG_VERSION_MYSQL_8_0, 0, TOKEN_KEYWORD_LAST_VALUE}, /* MariaDB 10.2 nonreserved */
      {"LCASE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LCASE},
      {"LEAD", FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_MARIADB_10_3, FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_MARIADB_10_3, TOKEN_KEYWORD_LEAD}, /* MariaDB 10.2 nonreserved -- or, maybe not in MariaDB 10.2 */
      {"LEADING", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_LEADING},
      {"LEAST", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LEAST},
      {"LEAVE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_LEAVE},
      {"LEFT", FLAG_VERSION_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LEFT},
      {"LENGTH", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_LENGTH},
      {"LEVEL", 0, 0, TOKEN_KEYWORD_LEVEL},
      {"LIKE", FLAG_VERSION_ALL, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_LIKE},
      {"LIMIT", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_LIMIT},
      {"LINEAR", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_LINEAR},
      {"LINEFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LINEFROMTEXT}, /* deprecated in MySQL 5.7.6 */
      {"LINEFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LINEFROMWKB}, /* deprecated in MySQL 5.7.6 */
      {"LINES", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_LINES},
      {"LINESTRING", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LINESTRING},
      {"LINESTRINGFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LINESTRINGFROMTEXT}, /* deprecated in MySQL 5.7.6 */
      {"LINESTRINGFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LINESTRINGFROMWKB}, /* deprecated in MySQL 5.7.6 */
      {"LN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LN},
      {"LOAD", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_LOAD},
      {"LOAD_FILE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LOAD_FILE},
      {"LOCAL", FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_LOCAL},
      {"LOCALTIME", FLAG_VERSION_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LOCALTIME},
      {"LOCALTIMESTAMP", FLAG_VERSION_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LOCALTIMESTAMP},
      {"LOCATE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LOCATE},
      {"LOCK", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_LOCK},
      {"LOG", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LOG},
      {"LOG10", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LOG10},
      {"LOG2", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LOG2},
      {"LOGFILE", 0, 0, TOKEN_KEYWORD_LOGFILE},
      {"LONG", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_LONG},
      {"LONGBLOB", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_LONGBLOB},
      {"LONGTEXT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_LONGTEXT},
      {"LOOP", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_LOOP},
      {"LOWER", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_LOWER},
      {"LOW_PRIORITY", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_LOW_PRIORITY},
      {"LPAD", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_LPAD},
      {"LTRIM", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_LTRIM},
      {"LUA", 0, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_LUA},
      {"MAKEDATE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MAKEDATE},
      {"MAKETIME", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MAKETIME},
      {"MAKE_SET", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MAKE_SET},
      {"MASTER_BIND", FLAG_VERSION_MYSQL_5_6, 0, TOKEN_KEYWORD_MASTER_BIND},
      {"MASTER_HEARTBEAT_PERIOD", 0, 0, TOKEN_KEYWORD_MASTER_HEARTBEAT_PERIOD},
      {"MASTER_POS_WAIT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MASTER_POS_WAIT},
      {"MASTER_SSL_VERIFY_SERVER_CERT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_MASTER_SSL_VERIFY_SERVER_CERT},
      {"MATCH", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_MATCH},
      {"MAX", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_MAX},
      {"MAXVALUE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_MAXVALUE},
      {"MBRCONTAINS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MBRCONTAINS},
      {"MBRCOVEREDBY", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MBRCOVEREDBY},
      {"MBRCOVERS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MBRCOVERS},
      {"MBRDISJOINT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MBRDISJOINT},
      {"MBREQUAL", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MBREQUAL}, /* deprecated in MySQL 5.7.6 */
      {"MBREQUALS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MBREQUALS},
      {"MBRINTERSECTS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MBRINTERSECTS},
      {"MBROVERLAPS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MBROVERLAPS},
      {"MBRTOUCHES", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MBRTOUCHES},
      {"MBRWITHIN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MBRWITHIN},
      {"MD5", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MD5},
      {"MEDIAN", FLAG_VERSION_MARIADB_10_3, FLAG_VERSION_MARIADB_10_3, TOKEN_KEYWORD_MEDIAN}, /* MariaDB 10.3.3 */
      {"MEDIUMBLOB", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_MEDIUMBLOB},
      {"MEDIUMINT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_MEDIUMINT},
      {"MEDIUMTEXT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_MEDIUMTEXT},
      {"MICROSECOND", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MICROSECOND},
      {"MID", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MID},
      {"MIDDLEINT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_MIDDLEINT},
      {"MIN", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_MIN},
      {"MINUTE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MINUTE},
      {"MINUTE_MICROSECOND", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_MINUTE_MICROSECOND},
      {"MINUTE_SECOND", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_MINUTE_SECOND},
      {"MINVALUE", 0, 0, TOKEN_KEYWORD_MINVALUE},
      {"MLINEFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MLINEFROMTEXT},  /* deprecated in MySQL 5.7.6 */
      {"MLINEFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MLINEFROMWKB},  /* deprecated in MySQL 5.7.6 */
      {"MOD", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MOD},
      {"MODE", 0, 0, TOKEN_KEYWORD_MODE},
      {"MODIFIES", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_MODIFIES},
      {"MONTH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MONTH},
      {"MONTHNAME", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MONTHNAME},
      {"MPOINTFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MPOINTFROMTEXT},  /* deprecated in MySQL 5.7.6 */
      {"MPOINTFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MPOINTFROMWKB}, /* deprecated in MySQL 5.7.6 */
      {"MPOLYFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MPOLYFROMTEXT}, /* deprecated in MySQL 5.7.6 */
      {"MPOLYFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MPOLYFROMWKB}, /* deprecated in MySQL 5.7.6 */
      {"MULTILINESTRING", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MULTILINESTRING},
      {"MULTILINESTRINGFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MULTILINESTRINGFROMTEXT}, /* deprecated in MySQL 5.7.6 */
      {"MULTILINESTRINGFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MULTILINESTRINGFROMWKB}, /* deprecated in MySQL 5.7.6 */
      {"MULTIPOINT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MULTIPOINT},
      {"MULTIPOINTFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MULTIPOINTFROMTEXT},  /* deprecated in MySQL 5.7.6 */
      {"MULTIPOINTFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MULTIPOINTFROMWKB}, /* deprecated in MySQL 5.7.6 */
      {"MULTIPOLYGON", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MULTIPOLYGON},
      {"MULTIPOLYGONFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MULTIPOLYGONFROMTEXT}, /* deprecated in MySQL 5.7.6 */
      {"MULTIPOLYGONFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_MULTIPOLYGONFROMWKB}, /* deprecated in MySQL 5.7.6 */
      {"NAMES", 0, 0, TOKEN_KEYWORD_NAMES},
      {"NAME_CONST", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_NAME_CONST},
      {"NATURAL", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_NATURAL},
      {"NEXTVAL", 0, FLAG_VERSION_MARIADB_10_3, TOKEN_KEYWORD_NEXTVAL},
      {"NIL", FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_NIL},
      {"NO", 0, 0, TOKEN_KEYWORD_NO},
      {"NOPAGER", 0, 0, TOKEN_KEYWORD_NOPAGER}, /* Ocelot keyword */
      {"NOT", FLAG_VERSION_TARANTOOL | FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_NOT},
      {"NOTEE", 0, 0, TOKEN_KEYWORD_NOTEE}, /* Ocelot keyword */
      {"NOTNULL", 0, 0, TOKEN_KEYWORD_NOTNULL},
      {"NOW", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_NOW},
      {"NOWARNING", 0, 0, TOKEN_KEYWORD_NOWARNING}, /* Ocelot keyword */
      {"NO_WRITE_TO_BINLOG", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_NO_WRITE_TO_BINLOG},
      {"NTH_VALUE", FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_MARIADB_10_3, FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_MARIADB_10_3, TOKEN_KEYWORD_NTH_VALUE}, /* MariaDB 10.2 nonreserved -- or, maybe not in MariaDB 10.2 */
      {"NTILE", FLAG_VERSION_MYSQL_8_0, FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_MARIADB_10_2_2, TOKEN_KEYWORD_NTILE},
      {"NULL", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_NULL},
      {"NULLIF", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_NULLIF},
      {"NULLS", FLAG_VERSION_MYSQL_8_0, 0, TOKEN_KEYWORD_NULLS}, /* MariaDB 10.2 nonreserved  -- or, maybe not in MariaDB 10.2 */
      {"NUMBER", 0, 0, TOKEN_KEYWORD_NUMBER},
      {"NUMERIC", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_NUMERIC},
      {"NUMGEOMETRIES", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_NUMGEOMETRIES}, /* deprecated in MySQL 5.7.6 */
      {"NUMINTERIORRINGS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_NUMINTERIORRINGS}, /* deprecated in MySQL 5.7.6 */
      {"NUMPOINTS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_NUMPOINTS}, /* deprecated in MySQL 5.7.6 */
      {"NVARCHAR2", 0, 0, TOKEN_KEYWORD_NVARCHAR2},
      {"OCT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_OCT},
      {"OCTET_LENGTH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_OCTET_LENGTH},
      {"OF", FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_OF},
      {"OFF", 0, 0, TOKEN_KEYWORD_OFF},
      {"OFFSET", 0, 0, TOKEN_KEYWORD_OFFSET},
      {"OJ", 0, 0, TOKEN_KEYWORD_OJ},
      {"OLD_PASSWORD", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_OLD_PASSWORD},
      {"ON", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_ON},
      {"OPEN", 0, 0, TOKEN_KEYWORD_OPEN},
      {"OPTIMIZE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_OPTIMIZE},
      {"OPTIMIZER_COSTS", FLAG_VERSION_MYSQL_5_7, 0, TOKEN_KEYWORD_OPTIMIZER_COSTS},
      {"OPTION", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_OPTION},
      {"OPTIONALLY", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_OPTIONALLY},
      {"OR", FLAG_VERSION_ALL | FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_OR},
      {"ORD", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ORD},
      {"ORDER", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_ORDER},
      {"OUT", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_OUT},
      {"OUTER", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_OUTER},
      {"OUTFILE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_OUTFILE},
      {"OVER", FLAG_VERSION_MYSQL_8_0 | FLAG_VERSION_TARANTOOL | FLAG_VERSION_MARIADB_10_2_2, 0, TOKEN_KEYWORD_OVER},
      {"OVERLAPS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_OVERLAPS}, /* deprecated in MySQL 5.7.6 */
      {"PACKAGE", FLAG_VERSION_PLSQL, 0, TOKEN_KEYWORD_PACKAGE},
      {"PAGER", 0, 0, TOKEN_KEYWORD_PAGER}, /* Ocelot keyword */
      {"PARTIAL", 0, 0, TOKEN_KEYWORD_PARTIAL},
      {"PARTITION", FLAG_VERSION_TARANTOOL | FLAG_VERSION_MYSQL_5_6 | FLAG_VERSION_MARIADB_10_0, 0, TOKEN_KEYWORD_PARTITION},
      {"PASSWORD", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_PASSWORD}, /* deprecated in MySQL 5.7.6 */
      {"PERCENTILE_CONT", 0, FLAG_VERSION_MARIADB_10_3, TOKEN_KEYWORD_PERCENTILE_CONT}, /* MariaDB 10.2 nonreserved -- or, maybe not in MariaDB 10.2 */
      {"PERCENTILE_DISC", 0, FLAG_VERSION_MARIADB_10_3, TOKEN_KEYWORD_PERCENTILE_DISC}, /* MariaDB 10.2 nonreserved -- or, maybe not in MariaDB 10.2 */
      {"PERCENT_RANK", FLAG_VERSION_MYSQL_8_0, FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_MARIADB_10_2_2, TOKEN_KEYWORD_PERCENT_RANK},
      {"PERIOD_ADD", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_PERIOD_ADD},
      {"PERIOD_DIFF", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_PERIOD_DIFF},
    {"PERSIST", FLAG_VERSION_MYSQL_8_0, 0, TOKEN_KEYWORD_PERSIST},
    {"PERSIST_ONLY", FLAG_VERSION_MYSQL_8_0, 0, TOKEN_KEYWORD_PERSIST_ONLY},
          {"PERSIST_RO_VARIABLES_ADMIN", 0, 0, TOKEN_KEYWORD_PERSIST_RO_VARIABLES_ADMIN},
      {"PI", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_PI},
      {"PLAN", 0, 0, TOKEN_KEYWORD_PLAN},
      {"POINT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_POINT},
      {"POINTFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_POINTFROMTEXT}, /* deprecated in MySQL 5.7.6 */
      {"POINTFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_POINTFROMWKB}, /* deprecated in MySQL 5.7.6 */
      {"POINTN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_POINTN}, /* deprecated in MySQL 5.7.6 */
      {"POLYFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_POLYFROMTEXT},
      {"POLYFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_POLYFROMWKB}, /* deprecated in MySQL 5.7 */
      {"POLYGON", 0, 0, TOKEN_KEYWORD_POLYGON},
      {"POLYGONFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_POLYGONFROMTEXT}, /* deprecated in MySQL 5.7.6 */
      {"POLYGONFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_POLYGONFROMWKB}, /* deprecated in MySQL 5.7.6 */
      {"POSITION", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_POSITION},
      {"POW", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_POW},
      {"POWER", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_POWER},
      {"PRAGMA", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_PRAGMA},
      {"PRECEDES", FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_MARIADB_10_2_3, 0, TOKEN_KEYWORD_PRECEDES},
      {"PRECISION", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_PRECISION},
      {"PREPARE", 0, 0, TOKEN_KEYWORD_PREPARE},
      {"PRIMARY", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_PRIMARY},
      {"PRINT", 0, 0, TOKEN_KEYWORD_PRINT}, /* Ocelot keyword */
      {"PRINTF", 0, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_PRINTF},
      {"PROCEDURE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_PROCEDURE},
      {"PROCESS", 0, 0, TOKEN_KEYWORD_PROCESS},
      {"PROMPT", 0, 0, TOKEN_KEYWORD_PROMPT}, /* Ocelot keyword */
      {"PROXY", 0, 0, TOKEN_KEYWORD_PROXY},
      {"PURGE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_PURGE},
      {"QUARTER", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_QUARTER},
      {"QUERY", 0, 0, TOKEN_KEYWORD_QUERY},
      {"QUIT", 0, 0, TOKEN_KEYWORD_QUIT}, /* Ocelot keyword */
      {"QUOTE", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_QUOTE},
      {"RADIANS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_RADIANS},
      {"RAISE", FLAG_VERSION_PLSQL, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_RAISE},
      {"RAND", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_RAND},
      {"RANDOM", 0, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_RANDOM},
      {"RANDOMBLOB", 0, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_RANDOMBLOB},
      {"RANDOM_BYTES", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_RANDOM_BYTES},
      {"RANGE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_RANGE},
      {"RANK", FLAG_VERSION_MYSQL_8_0, FLAG_VERSION_MYSQL_8_0 | FLAG_VERSION_TARANTOOL | FLAG_VERSION_MARIADB_10_2_2, TOKEN_KEYWORD_RANK},
      {"RAW", 0, 0, TOKEN_KEYWORD_RAW},
      {"READ", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_READ},
      {"READS", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_READS},
      {"READ_WRITE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_READ_WRITE},
      {"REAL", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_REAL},
      {"REBUILD", 0, 0, TOKEN_KEYWORD_REBUILD},
      {"RECURSIVE", FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_RECURSIVE},
    {"REDOFILE", FLAG_VERSION_MYSQL_8_0, 0, TOKEN_KEYWORD_REDOFILE},
      {"REDUNDANT", 0, 0, TOKEN_KEYWORD_REDUNDANT},
      {"REFERENCES", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_REFERENCES},
      {"REGEXP", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_REGEXP},
      {"REHASH", 0, 0, TOKEN_KEYWORD_REHASH}, /* Ocelot keyword */
      {"REINDEX", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_REINDEX},
      {"RELEASE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_RELEASE},
      {"RELEASE_ALL_LOCKS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_RELEASE_ALL_LOCKS},
      {"RELEASE_LOCK", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_RELEASE_LOCK},
      {"RELOAD", 0, 0, TOKEN_KEYWORD_RELOAD},
      {"RENAME", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_RENAME},
      {"REORGANIZE", 0, 0, TOKEN_KEYWORD_REORGANIZE},
      {"REPAIR", 0, 0, TOKEN_KEYWORD_REPAIR},
      {"REPEAT", FLAG_VERSION_ALL | FLAG_VERSION_LUA, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_REPEAT},
      {"REPLACE", FLAG_VERSION_ALL, FLAG_VERSION_ALL, TOKEN_KEYWORD_REPLACE},
      {"REPLICATION", 0, 0, TOKEN_KEYWORD_REPLICATION},
          {"REPLICATION_SLAVE_ADMIN", 0, 0, TOKEN_KEYWORD_REPLICATION_SLAVE_ADMIN},
      {"REQUIRE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_REQUIRE},
      {"RESET", 0, 0, TOKEN_KEYWORD_RESET},
      {"RESETCONNECTION", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_RESETCONNECTION},
      {"RESIGNAL", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_RESIGNAL},
          {"RESOURCE_GROUP_ADMIN", 0, 0, TOKEN_KEYWORD_RESOURCE_GROUP_ADMIN},
          {"RESOURCE_GROUP_USER", 0, 0, TOKEN_KEYWORD_RESOURCE_GROUP_USER},
      {"RESTRICT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_RESTRICT},
      {"RETURN", FLAG_VERSION_ALL | FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_RETURN},
      {"RETURNS", 0, 0, TOKEN_KEYWORD_RETURNS},
      {"REVERSE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_REVERSE},
      {"REVOKE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_REVOKE},
      {"RIGHT", FLAG_VERSION_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_RIGHT},
      {"RLIKE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_RLIKE},
      {"ROLE", 0, 0, TOKEN_KEYWORD_ROLE},
          {"ROLE_ADMIN", 0, 0, TOKEN_KEYWORD_ROLE_ADMIN},
      {"ROLLBACK", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_ROLLBACK},
      {"ROUND", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_ROUND},
      {"ROW", FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_ROW},
      {"ROWS", FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_ROWS},
      {"ROW_COUNT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ROW_COUNT},
      {"ROW_NUMBER", FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_TARANTOOL, FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_MARIADB_10_2_2, TOKEN_KEYWORD_ROW_NUMBER},
      {"RPAD", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_RPAD},
      {"RTRIM", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_RTRIM},
      {"SAVEPOINT", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_SAVEPOINT},
      {"SCHEMA", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SCHEMA},
      {"SCHEMAS", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_SCHEMAS},
      {"SECOND", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SECOND},
      {"SECOND_MICROSECOND", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_SECOND_MICROSECOND},
      {"SECURITY", 0, 0, TOKEN_KEYWORD_SECURITY},
      {"SEC_TO_TIME", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SEC_TO_TIME},
      {"SELECT", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_SELECT},
      {"SENSITIVE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_SENSITIVE},
      {"SEPARATOR", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_SEPARATOR},
      {"SEQUENCE", 0, 0, TOKEN_KEYWORD_SEQUENCE},
      {"SERIAL", 0, 0, TOKEN_KEYWORD_SERIAL},
      {"SERVER", 0, 0, TOKEN_KEYWORD_SERVER},
      {"SESSION", 0, 0, TOKEN_KEYWORD_SESSION},
      {"SESSION_USER", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SESSION_USER},
      {"SET", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_SET},
      {"SETVAL", 0, FLAG_VERSION_MARIADB_10_3, TOKEN_KEYWORD_SETVAL},
          {"SET_USER_ID", 0, 0, TOKEN_KEYWORD_SET_USER_ID},
      {"SHA", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SHA},
      {"SHA1", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SHA1},
      {"SHA2", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SHA2},
      {"SHARED", 0, 0, TOKEN_KEYWORD_SHARED},
      {"SHOW", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_SHOW},
      {"SHUTDOWN", 0, 0, TOKEN_KEYWORD_SHUTDOWN},
      {"SIGN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SIGN},
      {"SIGNAL", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_SIGNAL},
      {"SIMPLE", 0, 0, TOKEN_KEYWORD_SIMPLE},
      {"SIN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SIN},
      {"SLEEP", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SLEEP},
      {"SLOW", 0, 0, TOKEN_KEYWORD_SLOW},
      {"SMALLINT", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_SMALLINT},
      {"SONAME", 0, 0, TOKEN_KEYWORD_SONAME},
      {"SOUNDEX", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_SOUNDEX},
      {"SOURCE", 0, 0, TOKEN_KEYWORD_SOURCE}, /* Ocelot keyword */
      {"SPACE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SPACE},
      {"SPATIAL", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_SPATIAL},
      {"SPECIFIC", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_SPECIFIC},
      {"SQL", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_SQL},
      {"SQLEXCEPTION", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_SQLEXCEPTION},
      {"SQLSTATE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_SQLSTATE},
      {"SQLWARNING", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_SQLWARNING},
      {"SQL_BIG_RESULT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_SQL_BIG_RESULT},
      {"SQL_CALC_FOUND_ROWS", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_SQL_CALC_FOUND_ROWS},
      {"SQL_SMALL_RESULT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_SQL_SMALL_RESULT},
      {"SQRT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SQRT},
      {"SRID", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SRID},
      {"SSL", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_SSL},
      {"START", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_START},
      {"STARTING", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_STARTING},
      {"STARTPOINT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_STARTPOINT}, /* deprecated in MySQL 5.7.6 */
      {"STATEMENT", 0, 0, TOKEN_KEYWORD_STATEMENT},
      {"STATUS", 0, 0, TOKEN_KEYWORD_STATUS}, /* Ocelot keyword */
      {"STD", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_STD},
      {"STDDEV", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_STDDEV},
      {"STDDEV_POP", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_STDDEV_POP},
      {"STDDEV_SAMP", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_STDDEV_SAMP},
      {"STOP", 0, 0, TOKEN_KEYWORD_STOP},
      {"STORED", FLAG_VERSION_MYSQL_5_7, 0, TOKEN_KEYWORD_STORED},
      {"STRAIGHT_JOIN", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_STRAIGHT_JOIN},
      {"STRCMP", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_STRCMP},
      {"STRFTIME", 0, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_STRFTIME},
      {"STR_TO_DATE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_STR_TO_DATE},
      {"ST_AREA", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_AREA},
      {"ST_ASBINARY", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_ASBINARY},
      {"ST_ASGEOJSON", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_ASGEOJSON},
      {"ST_ASTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_ASTEXT},
      {"ST_ASWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_ASWKB},
      {"ST_ASWKT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_ASWKT},
      {"ST_BUFFER", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_BUFFER},
      {"ST_BUFFER_STRATEGY", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_BUFFER_STRATEGY},
      {"ST_CENTROID", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_CENTROID},
      {"ST_CONTAINS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_CONTAINS},
      {"ST_CONVEXHULL", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_CONVEXHULL},
      {"ST_CROSSES", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_CROSSES},
      {"ST_DIFFERENCE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_DIFFERENCE},
      {"ST_DIMENSION", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_DIMENSION},
      {"ST_DISJOINT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_DISJOINT},
      {"ST_DISTANCE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_DISTANCE},
      {"ST_DISTANCE_SPHERE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_DISTANCE_SPHERE},
      {"ST_ENDPOINT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_ENDPOINT},
      {"ST_ENVELOPE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_ENVELOPE},
      {"ST_EQUALS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_EQUALS},
      {"ST_EXTERIORRING", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_EXTERIORRING},
      {"ST_GEOHASH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_GEOHASH},
      {"ST_GEOMCOLLFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_GEOMCOLLFROMTEXT},
      {"ST_GEOMCOLLFROMTXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_GEOMCOLLFROMTXT},
      {"ST_GEOMCOLLFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_GEOMCOLLFROMWKB},
      {"ST_GEOMETRYCOLLECTIONFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_GEOMETRYCOLLECTIONFROMTEXT},
      {"ST_GEOMETRYCOLLECTIONFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_GEOMETRYCOLLECTIONFROMWKB},
      {"ST_GEOMETRYFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_GEOMETRYFROMTEXT},
      {"ST_GEOMETRYFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_GEOMETRYFROMWKB},
      {"ST_GEOMETRYN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_GEOMETRYN},
      {"ST_GEOMETRYTYPE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_GEOMETRYTYPE},
      {"ST_GEOMFROMGEOJSON", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_GEOMFROMGEOJSON},
      {"ST_GEOMFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_GEOMFROMTEXT},
      {"ST_GEOMFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_GEOMFROMWKB},
      {"ST_INTERIORRINGN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_INTERIORRINGN},
      {"ST_INTERSECTION", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_INTERSECTION},
      {"ST_INTERSECTS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_INTERSECTS},
      {"ST_ISCLOSED", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_ISCLOSED},
      {"ST_ISEMPTY", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_ISEMPTY},
      {"ST_ISSIMPLE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_ISSIMPLE},
      {"ST_ISVALID", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_ISVALID},
      {"ST_LATFROMGEOHASH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_LATFROMGEOHASH},
      {"ST_LENGTH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_LENGTH},
      {"ST_LINEFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_LINEFROMTEXT},
      {"ST_LINEFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_LINEFROMWKB},
      {"ST_LINESTRINGFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_LINESTRINGFROMTEXT},
      {"ST_LINESTRINGFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_LINESTRINGFROMWKB},
      {"ST_LONGFROMGEOHASH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_LONGFROMGEOHASH},
      {"ST_MAKEENVELOPE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_MAKEENVELOPE},
      {"ST_MLINEFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_MLINEFROMTEXT},
      {"ST_MLINEFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_MLINEFROMWKB},
      {"ST_MPOINTFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_MPOINTFROMTEXT},
      {"ST_MPOINTFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_MPOINTFROMWKB},
      {"ST_MPOLYFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_MPOLYFROMTEXT},
      {"ST_MPOLYFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_MPOLYFROMWKB},
      {"ST_MULTILINESTRINGFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_MULTILINESTRINGFROMTEXT},
      {"ST_MULTILINESTRINGFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_MULTILINESTRINGFROMWKB},
      {"ST_MULTIPOINTFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_MULTIPOINTFROMTEXT},
      {"ST_MULTIPOINTFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_MULTIPOINTFROMWKB},
      {"ST_MULTIPOLYGONFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_MULTIPOLYGONFROMTEXT},
      {"ST_MULTIPOLYGONFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_MULTIPOLYGONFROMWKB},
      {"ST_NUMGEOMETRIES", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_NUMGEOMETRIES},
      {"ST_NUMINTERIORRING", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_NUMINTERIORRING},
      {"ST_NUMINTERIORRINGS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_NUMINTERIORRINGS},
      {"ST_NUMPOINTS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_NUMPOINTS},
      {"ST_OVERLAPS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_OVERLAPS},
      {"ST_POINTFROMGEOHASH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_POINTFROMGEOHASH},
      {"ST_POINTFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_POINTFROMTEXT},
      {"ST_POINTFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_POINTFROMWKB},
      {"ST_POINTN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_POINTN},
      {"ST_POLYFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_POLYFROMTEXT},
      {"ST_POLYFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_POLYFROMWKB},
      {"ST_POLYGONFROMTEXT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_POLYGONFROMTEXT},
      {"ST_POLYGONFROMWKB", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_POLYGONFROMWKB},
      {"ST_SIMPLIFY", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_SIMPLIFY},
      {"ST_SRID", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_SRID},
      {"ST_STARTPOINT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_STARTPOINT},
      {"ST_SYMDIFFERENCE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_SYMDIFFERENCE},
      {"ST_TOUCHES", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_TOUCHES},
      {"ST_UNION", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_UNION},
      {"ST_VALIDATE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_VALIDATE},
      {"ST_WITHIN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_WITHIN},
      {"ST_X", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_X},
      {"ST_Y", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_ST_Y},
      {"SUBDATE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SUBDATE},
      {"SUBSTR", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_SUBSTR},
      {"SUBSTRING", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SUBSTRING},
      {"SUBSTRING_INDEX", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SUBSTRING_INDEX},
      {"SUBTIME", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SUBTIME},
      {"SUM", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_SUM},
      {"SUPER", 0, 0, TOKEN_KEYWORD_SUPER},
      {"SYSDATE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SYSDATE},
      {"SYSTEM", FLAG_VERSION_MYSQL_8_0|FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_SYSTEM}, /* Ocelot keyword */
      {"SYSTEM_USER", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_SYSTEM_USER},
          {"SYSTEM_VARIABLES_ADMIN", 0, 0, TOKEN_KEYWORD_SYSTEM_VARIABLES_ADMIN},
      {"TABLE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_TABLE},
      {"TABLESPACE", 0, 0, TOKEN_KEYWORD_TABLESPACE},
      {"TAN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_TAN},
      {"TEE", 0, 0, TOKEN_KEYWORD_TEE}, /* Ocelot keyword */
      {"TEMP", 0, 0, TOKEN_KEYWORD_TEMP},
      {"TEMPORARY", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_TEMPORARY},
      {"TERMINATED", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_TERMINATED},
      {"THEN", FLAG_VERSION_ALL | FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_THEN},
      {"TIME", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_TIME},
      {"TIMEDIFF", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_TIMEDIFF},
      {"TIMESTAMP", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_TIMESTAMP},
      {"TIMESTAMPADD", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_TIMESTAMPADD},
      {"TIMESTAMPDIFF", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_TIMESTAMPDIFF},
      {"TIME_FORMAT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_TIME_FORMAT},
      {"TIME_TO_SEC", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_TIME_TO_SEC},
      {"TINYBLOB", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_TINYBLOB},
      {"TINYINT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_TINYINT},
      {"TINYTEXT", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_TINYTEXT},
      {"TO", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_TO},
      {"TOTAL_CHANGES", 0, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_TOTAL_CHANGES},
      {"TOUCHES", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_TOUCHES}, /* deprecated in MySQL 5.7.6 */
      {"TO_BASE64", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_TO_BASE64},
      {"TO_DAYS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_TO_DAYS},
      {"TO_SECONDS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_TO_SECONDS},
      {"TRAILING", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_TRAILING},
      {"TRANSACTION", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_TRANSACTION},
      {"TRIGGER", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_TRIGGER},
      {"TRIM", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_TRIM},
      {"TRUE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL | FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_TRUE},
      {"TRUNCATE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_TRUNCATE},
      {"TYPE", 0, 0, TOKEN_KEYWORD_TYPE},
      {"TYPEOF", 0, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_TYPEOF},
      {"UCASE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_UCASE},
      {"UNBOUNDED", 0, 0, TOKEN_KEYWORD_UNBOUNDED},
      {"UNCOMPRESS", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_UNCOMPRESS},
      {"UNCOMPRESSED_LENGTH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_UNCOMPRESSED_LENGTH},
      {"UNDO", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_UNDO},
      {"UNHEX", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_UNHEX},
      {"UNICODE", 0, FLAG_VERSION_TARANTOOL, TOKEN_KEYWORD_UNICODE},
      {"UNINSTALL", 0, 0, TOKEN_KEYWORD_UNINSTALL},
      {"UNION", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_UNION},
      {"UNIQUE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_UNIQUE},
      {"UNIX_TIMESTAMP", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_UNIX_TIMESTAMP},
      {"UNKNOWN", 0, 0, TOKEN_KEYWORD_UNKNOWN},
      {"UNLOCK", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_UNLOCK},
      {"UNSIGNED", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_UNSIGNED},
      {"UNTIL", FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_UNTIL},
      {"UPDATE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_UPDATE},
      {"UPDATEXML", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_UPDATEXML},
      {"UPPER", 0, FLAG_VERSION_ALL, TOKEN_KEYWORD_UPPER},
      {"USAGE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_USAGE},
      {"USE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_USE}, /* Ocelot keyword, also reserved word */
      {"USER", FLAG_VERSION_TARANTOOL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_USER},
      {"USING", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_USING},
      {"UTC_DATE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_UTC_DATE},
      {"UTC_TIME", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_UTC_TIME},
      {"UTC_TIMESTAMP", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_UTC_TIMESTAMP},
      {"UUID", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_UUID},
      {"UUID_SHORT", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_UUID_SHORT},
      {"VACUUM", 0, 0, TOKEN_KEYWORD_VACUUM},
      {"VALIDATE_PASSWORD_STRENGTH", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_VALIDATE_PASSWORD_STRENGTH},
      {"VALIDATION", 0, 0, TOKEN_KEYWORD_VALIDATION},
      {"VALUES", FLAG_VERSION_ALL, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_VALUES},
      {"VARBINARY", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_VARBINARY},
      {"VARCHAR", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_VARCHAR},
      {"VARCHAR2", 0, 0, TOKEN_KEYWORD_VARCHAR2},
      {"VARCHARACTER", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_VARCHARACTER},
      {"VARIANCE", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_VARIANCE},
      {"VARYING", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_VARYING},
      {"VAR_POP", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_VAR_POP},
      {"VAR_SAMP", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_VAR_SAMP},
      {"VERSION", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_VERSION},
          {"VERSION_TOKEN_ADMIN", 0, 0, TOKEN_KEYWORD_VERSION_TOKEN_ADMIN},
      {"VIEW", FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_VIEW},
      {"VIRTUAL", FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_TARANTOOL, 0, TOKEN_KEYWORD_VIRTUAL},
      {"WAIT_FOR_EXECUTED_GTID_SET", 0, FLAG_VERSION_MYSQL_ALL, TOKEN_KEYWORD_WAIT_FOR_EXECUTED_GTID_SET},
      {"WAIT_UNTIL_SQL_THREAD_AFTER_GTIDS", 0, FLAG_VERSION_MYSQL_ALL, TOKEN_KEYWORD_WAIT_UNTIL_SQL_THREAD_AFTER_GTIDS},
      {"WARNINGS", 0, 0, TOKEN_KEYWORD_WARNINGS}, /* Ocelot keyword */
      {"WEEK", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_WEEK},
      {"WEEKDAY", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_WEEKDAY},
      {"WEEKOFYEAR", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_WEEKOFYEAR},
      {"WEIGHT_STRING", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_WEIGHT_STRING},
      {"WHEN", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_WHEN},
      {"WHENEVER", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_WHENEVER},
      {"WHERE", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_WHERE},
      {"WHILE", FLAG_VERSION_ALL | FLAG_VERSION_LUA, 0, TOKEN_KEYWORD_WHILE},
    {"WINDOW", FLAG_VERSION_MYSQL_8_0, 0, TOKEN_KEYWORD_WINDOW},
      {"WITH", FLAG_VERSION_ALL, 0, TOKEN_KEYWORD_WITH},
      {"WITHIN", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_WITHIN}, /* deprecated in MySQL 5.7.6 */
      {"WITHOUT", 0, 0, TOKEN_KEYWORD_WITHOUT},
      {"WRITE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_WRITE},
      {"X", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_X}, /* deprecated in MySQL 5.7.6 */
      {"XA", 0, 0, TOKEN_KEYWORD_XA},
          {"XA_RECOVER_ADMIN", 0, 0, TOKEN_KEYWORD_XA_RECOVER_ADMIN},
      {"XOR", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_XOR},
      {"Y", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_Y}, /* deprecated in MySQL 5.7.6 */
      {"YEAR", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_YEAR},
      {"YEARWEEK", 0, FLAG_VERSION_MYSQL_OR_MARIADB_ALL, TOKEN_KEYWORD_YEARWEEK},
      {"YEAR_MONTH", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_YEAR_MONTH},
      {"ZEROFILL", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD_ZEROFILL},
      {"_ARMSCII8", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__ARMSCII8},
      {"_ASCII", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__ASCII},
      {"_BIG5", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__BIG5},
      {"_BINARY", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__BINARY},
      {"_CP1250", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__CP1250},
      {"_CP1251", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__CP1251},
      {"_CP1256", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__CP1256},
      {"_CP1257", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__CP1257},
      {"_CP850", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__CP850},
      {"_CP852", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__CP852},
      {"_CP866", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__CP866},
      {"_CP932", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__CP932},
      {"_DEC8", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__DEC8},
      {"_EUCJPMS", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__EUCJPMS},
      {"_EUCKR", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__EUCKR},
      {"_FILENAME", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__FILENAME},
      {"_GB18030", FLAG_VERSION_MYSQL_5_7, 0, TOKEN_KEYWORD__GB18030},
      {"_GB2312", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__GB2312},
      {"_GBK", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__GBK},
      {"_GEOSTD8", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__GEOSTD8},
      {"_GREEK", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__GREEK},
      {"_HEBREW", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__HEBREW},
      {"_HP8", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__HP8},
      {"_KEYBCS2", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__KEYBCS2},
      {"_KOI8R", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__KOI8R},
      {"_KOI8U", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__KOI8U},
      {"_LATIN1", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__LATIN1},
      {"_LATIN2", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__LATIN2},
      {"_LATIN5", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__LATIN5},
      {"_LATIN7", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__LATIN7},
      {"_MACCE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__MACCE},
      {"_MACROMAN", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__MACROMAN},
      {"_SJIS", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__SJIS},
      {"_SWE7", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__SWE7},
      {"_TIS620", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__TIS620},
      {"_UCS2", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__UCS2},
      {"_UJIS", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__UJIS},
      {"_UTF16", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__UTF16},
      {"_UTF16LE", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__UTF16LE},
      {"_UTF32", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__UTF32},
      {"_UTF8", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__UTF8},
      {"_UTF8MB4", FLAG_VERSION_MYSQL_OR_MARIADB_ALL, 0, TOKEN_KEYWORD__UTF8MB4}
    };


  //QString text;
  QString s= "";
  int t;
  char *p_item;
  unsigned long index;
  char key2[MAX_KEYWORD_LENGTH + 1];
  int i, i2;

  //text= statement_edit_widget->toPlainText();
  for (i2= start; main_token_lengths[i2] != 0; ++i2)
  {
    /* Get the next word. */
    s= text.mid(main_token_offsets[i2], main_token_lengths[i2]);
    t= token_type(s.data(), main_token_lengths[i2], ansi_quotes);
    main_token_types[i2]= t;
    main_token_flags[i2]= 0;
    main_token_pointers[i2]= 0;
    main_token_reftypes[i2]= 0;
    if ((t == TOKEN_TYPE_OTHER) && (main_token_lengths[i2] < MAX_KEYWORD_LENGTH))
    {
      /* It's not a literal or operator. Maybe it's a keyword. Convert it to char[]. */
      QByteArray key_as_byte_array= s.toLocal8Bit();
      const char *key= key_as_byte_array.data();
      /* Uppercase it. I don't necessarily have strupr(). */
      for (i= 0; (*(key + i) != '\0') && (i < MAX_KEYWORD_LENGTH); ++i) key2[i]= toupper(*(key + i));
      key2[i]= '\0';
      /* If the following assert happens, you inserted/removed something without changing "917" */

      assert(TOKEN_KEYWORD__UTF8MB4 == TOKEN_KEYWORD_QUESTIONMARK + (917 - 1));

      ///* Test strvalues is ordered by bsearching for every item. */
      //for (int ii= 0; ii < 917; ++ii)
      //{
      //  char *k= (char*) &strvalues[ii].chars;
      //  p_item= (char*) bsearch(k, strvalues, 917, sizeof(struct keywords), (int(*)(const void*, const void*)) strcmp);
      //  assert(p_item != NULL);
      //  index= ((((unsigned long)p_item - (unsigned long)strvalues)) / sizeof(struct keywords));
      //  index+= TOKEN_KEYWORDS_START;
      //  printf("ii=%d, index=%ld, k=%s.\n", ii, index, k);
      //  assert(index == strvalues[ii].token_keyword);
      //}
      /* TODO: you don't need to calculate index, it's strvalues[...].token_keyword. */
      /* Search it with library binary-search. Assume 917 items and everything MAX_KEYWORD_LENGTH bytes long. */
      p_item= (char*) bsearch(key2, strvalues, 917, sizeof(struct keywords), (int(*)(const void*, const void*)) strcmp);
      if (p_item != NULL)
      {
        /* It's in the list, so instead of TOKEN_TYPE_OTHER, make it TOKEN_KEYWORD_something. */
        index= ((((unsigned long)p_item - (unsigned long)strvalues)) / sizeof(struct keywords));
        if ((strvalues[index].reserved_flags & dbms_version_mask) != 0)
          main_token_flags[i2]= (main_token_flags[i2] | TOKEN_FLAG_IS_RESERVED);
        if ((strvalues[index].built_in_function_flags & dbms_version_mask) != 0)
        {
          main_token_flags[i2]= (main_token_flags[i2] | TOKEN_FLAG_IS_FUNCTION);
        }
        if ((strvalues[index].reserved_flags & FLAG_VERSION_LUA) != 0)
          main_token_flags[i2]= (main_token_flags[i2] | TOKEN_FLAG_IS_MAYBE_LUA);
        index+= TOKEN_KEYWORDS_START;
        main_token_types[i2]= index;
      }
#ifdef DEBUGGER
      else
      {
        /* It's not in the list, but if it's unambiguously $DEB[UG] etc. then consider it a debug keyword. */
        if ((key2[0] == '$') && (key2[1] != '\0'))
        {
          if (strncmp(key2, "$BREAKPOINT", strlen(key2)) == 0) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_BREAKPOINT;
          if ((strlen(key2) > 2) && (strncmp(key2, "$CLEAR", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_CLEAR;
          if ((strlen(key2) > 2) && (strncmp(key2, "$CONTINUE", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_CONTINUE;
          if ((strlen(key2) > 3) && (strncmp(key2, "$DEBUG", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_DEBUG;
          if ((strlen(key2) > 3) && (strncmp(key2, "$DELETE", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_DELETE;
          if ((strlen(key2) > 3) && (strncmp(key2, "$EXECUTE", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_EXECUTE;
          if ((strlen(key2) > 3) && (strncmp(key2, "$EXIT", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_EXIT;
          if ((strlen(key2) > 3) && (strncmp(key2, "$INFORMATION", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_INFORMATION;
          if ((strlen(key2) > 3) && (strncmp(key2, "$INSTALL", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_INSTALL;
          if (strncmp(key2, "$LEAVE", strlen(key2)) == 0) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_LEAVE;
          if (strncmp(key2, "$NEXT", strlen(key2)) == 0) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_NEXT;
          if (strncmp(key2, "$REFRESH", strlen(key2)) == 0) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_REFRESH;
          if (strcmp(key2, "$SET") == 0) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_SET;
          if ((strlen(key2) > 4) && (strncmp(key2, "$SETUP", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_SETUP;
          if ((strlen(key2) > 2) && (strncmp(key2, "$SKIP", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_SKIP;
          if ((strlen(key2) > 2) && (strncmp(key2, "$SOURCE", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_SOURCE;
          if ((strlen(key2) > 2) && (strncmp(key2, "$STEP", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_STEP;
          if (strncmp(key2, "$TBREAKPOINT", strlen(key2)) == 0) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_TBREAKPOINT;
        }
      }
#endif
    }
  }

  /* Global. Sometimes we'll want to know how many tokens there are. */
  main_token_count_in_all= i2;

  main_token_types[i2]= 0;

  /* The special cases of BEGIN, DO, END, FUNCTION, ROW. */
  int i_of_function= -1;
  int i_of_do= -1;
  int start_of_body= find_start_of_body(text, start, &i_of_function, &i_of_do);

  tokens_to_keywords_revert(start_of_body, i_of_function, i_of_do, text, start);

  /*
    The special case of "?". Although tokenize() says ? is an operator,
    we might have to treat it as a keyword meaning "help".
  */
  if (main_token_lengths[start] == 1)
  {
    s= text.mid(main_token_offsets[start], 1);
    if (s == QString("?")) main_token_types[start]= TOKEN_KEYWORD_QUESTIONMARK;
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
  /* Todo: There is interference with the statement "prompt \u" and maybe other special prompts */
  /* Todo: This is completely wrecked if start!=0 because of delimiter */
  int xx= -1;
  if (i2 >= 2)
  {
    /* Todo: what about the delimiter? */
    if ((main_token_offsets[1] == 1)
     && (main_token_lengths[1] == 1)
     && (text.mid(main_token_offsets[0], main_token_lengths[0]) == "\\"))
    {
      xx= 0;
    }
    else if ((main_token_lengths[main_token_count_in_all - 2] == 1)
     && (main_token_lengths[main_token_count_in_all - 1] == 1)
     && (text.mid(main_token_offsets[main_token_count_in_all - 2], main_token_lengths[main_token_count_in_all - 2]) == "\\"))
    {
      xx= main_token_count_in_all - 2;
    }
  }
  if (i2 >= 3)
  {
    s= text.mid(main_token_offsets[i2 - 1], main_token_lengths[i2 - 1]);
    /* Todo: compare with delimiter, which isn't always semicolon. */
    if (s == (QString)ocelot_delimiter_str)
    {
      if ((main_token_lengths[main_token_count_in_all - 3] == 1)
       && (main_token_lengths[main_token_count_in_all - 2] == 1)
       && (text.mid(main_token_offsets[main_token_count_in_all - 3], main_token_lengths[main_token_count_in_all - 3]) == "\\"))

      {
        xx= main_token_count_in_all - 3;
      }
    }
  }
  if (xx >= 0)
  {
    s= text.mid(main_token_offsets[xx], 2);
    if (s == QString("\\?")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_QUESTIONMARK;
    if (s == QString("\\C")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_CHARSET;
    if (s == QString("\\c")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_CLEAR;
    if (s == QString("\\r")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_CONNECT;
    if (s == QString("\\d")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_DELIMITER;
    if (s == QString("\\e")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_EDIT;
    if (s == QString("\\G")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_EGO;
    if (s == QString("\\g")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_GO;
    if (s == QString("\\h")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_HELP_IN_CLIENT;
    if (s == QString("\\n")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_NOPAGER;
    if (s == QString("\\t")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_NOTEE;
    if (s == QString("\\w")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_NOWARNING;
    if (s == QString("\\P")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_PAGER;
    if (s == QString("\\p")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_PRINT;
    if (s == QString("\\R")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_PROMPT;
    if (s == QString("\\q")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_QUIT;
    if (s == QString("\\#")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_REHASH;
    if (s == QString("\\.")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_SOURCE;
    if (s == QString("\\s")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_STATUS;
    if (s == QString("\\!")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_SYSTEM;
    if (s == QString("\\T")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_TEE;
    if (s == QString("\\u")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_USE;
    if (s == QString("\\W")) main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_WARNINGS;
  }
  log("tokens_to_keywords end", 80);
}

/*
  Return true if the passed token number is for the first word of a client statement.
*/
bool MainWindow::is_client_statement(int token, int i,QString text)
{
  if ((token == TOKEN_KEYWORD_QUESTIONMARK)
  ||  (token == TOKEN_KEYWORD_CHARSET)
  ||  (token == TOKEN_KEYWORD_CLEAR)
  ||  (token == TOKEN_KEYWORD_CONNECT)
  ||  (token == TOKEN_KEYWORD_DELIMITER)
  ||  (token == TOKEN_KEYWORD_EDIT)
  ||  (token == TOKEN_KEYWORD_EGO)
  ||  (token == TOKEN_KEYWORD_EXIT)
  ||  (token == TOKEN_KEYWORD_GO)
  ||  (token == TOKEN_KEYWORD_HELP_IN_CLIENT)
  ||  (token == TOKEN_KEYWORD_NOPAGER)
  ||  (token == TOKEN_KEYWORD_NOTEE)
  ||  (token == TOKEN_KEYWORD_NOWARNING)
  ||  (token == TOKEN_KEYWORD_PAGER)
  ||  (token == TOKEN_KEYWORD_PRINT)
  ||  (token == TOKEN_KEYWORD_PROMPT)
  ||  (token == TOKEN_KEYWORD_QUIT)
  ||  (token == TOKEN_KEYWORD_REHASH)
  ||  (token == TOKEN_KEYWORD_SOURCE)
  ||  (token == TOKEN_KEYWORD_STATUS)
  ||  (token == TOKEN_KEYWORD_SYSTEM)
  ||  (token == TOKEN_KEYWORD_TEE)
  ||  (token == TOKEN_KEYWORD_USE)
  ||  (token == TOKEN_KEYWORD_WARNINGS))
    return true;
  if (token == TOKEN_KEYWORD_SET)
  {
    if (main_token_lengths[i + 1] > 7)
    {
      QString s= text.mid(main_token_offsets[i + 1], 7);
      if (QString::compare(s, "OCELOT_", Qt::CaseInsensitive) == 0)
      {
        return true;
      }
    }
  }
  if ((token >= TOKEN_KEYWORD_DEBUG_BREAKPOINT)
   && (token <= TOKEN_KEYWORD_DEBUG_TBREAKPOINT))
    return true;
  return false;
}

/*
  Find start of body in CREATE ... FUNCTION|PROCEDURE|TRIGGER statement
  This is tricky and might need revisiting if syntax changes in later MySQL versions.
  First find out whether the statement is CREATE ... FUNCTION|PROCEDURE|TRIGGER|EVENT.
  A definer clause might have to be skipped.
  If it's CREATE TRIGGER: skip till past FOR EACH ROW.
                          but watch for new PRECEDES|FOLLOWS clauses
  If it's CREATE EVENT: skip till past ON and past DO.
  If it's CREATE PROCEDURE|FUNCTION:
    skip past ()s
   (Function) Skip returns clause i.e. skip until anything is seen which is not one of the following:
      anything within ()s
      ASCII BIGINT BINARY BIT BLOB BOOL BOOLEAN CHAR CHARACTER CHARSET COLLATE DATE DATETIME DEC
      DECIMAL DOUBLE ENUM FLOAT
      GEOMETRY GEOMETRYCOLLECTION INT INTEGER LINESTRING LONGBLOB LONGTEXT MEDIUMBLOB MEDIUMINT MEDIUMTEXT
      MULTILINESTRING MULTIPOINT MULTIPOLYGON NUMERIC POINT POLYGON PRECISION RETURNS
      SERIAL SET(...) SMALLINT TIME TIMESTAMP TINYBLOB TINYINT TINYTEXT UNICODE UNSIGNED VARCHAR VARYING YEAR ZEROFILL
      name if previous is CHARSET | CHARACTER SET | COLLATE
   (Function or Procedure) Skip words that are in characteristics i.e.
     COMMENT 'string', LANGUAGE SQL, DETERMINISTIC, NOT DETERMINISTIC,
     CONTAINS SQL, NO SQL, READS SQL DATA, MODIFIES SQL DATA,
     SQL SECURITY DEFINER, SQL SECURITY INVOKER
   (Function or Procedure) Watch for Oracle-style {IS|AS} body
   Skipping comments too
   Return: offset for first word of body, or -1 if not-create-routine | body-not-found
   todo: there might be a problem with "create procedure|function function ...".
   No worries: even if this doesn't get everything right, parsing will come later.
*/
int MainWindow::find_start_of_body(QString text, int start, int *i_of_function, int *i_of_do)
{
  int i;
  int create_seen= 0;
  int procedure_seen= 0;
  int function_seen= 0;
  int trigger_seen= 0;
  int trigger_for_seen= 0;
  int trigger_row_seen= 0;
  int trigger_precedes_seen= 0;
  int trigger_name_seen= 0;
  int parameter_list_seen= 0;
  int parentheses_count= 0;
  int characteristic_seen= 0;
  int data_type_seen= 0;
  int character_set_seen= 0;
  int collate_seen= 0;
  int event_seen= 0;
  int event_do_seen= 0;
  int event_on_seen= 0;

  for (i= start; main_token_lengths[i] != 0; ++i)
  {
    if ((main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
     || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_SLASH)
     || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_MINUS))
    {
      continue;
    }
    if (trigger_precedes_seen == 1)
    {
      if (main_token_types[i] == TOKEN_TYPE_OPERATOR)
      {
        trigger_name_seen= 0;
        continue;
      }
      if (trigger_name_seen == 1)
      {
        return i;
      }
      trigger_name_seen= 1;
      continue;
    }
    if (trigger_row_seen == 1)
    {
      if ((main_token_types[i] == TOKEN_KEYWORD_PRECEDES)
       || (main_token_types[i] == TOKEN_KEYWORD_FOLLOWS))
      {
        trigger_precedes_seen= 1;
        continue;
      }
      return i;
    }
    if (event_do_seen == 1)
    {
      return i;
    }
    if (main_token_types[i] == TOKEN_KEYWORD_CREATE)
    {
      create_seen= 1;
      continue;
    }
    if (create_seen != 1)
    {
      return -1;
    }
    if (main_token_types[i] == TOKEN_KEYWORD_PROCEDURE)
    {
      procedure_seen= 1;
      continue;
    }
    if (main_token_types[i] == TOKEN_KEYWORD_FUNCTION)
    {
      function_seen= 1;
      *i_of_function= i;
      continue;
    }
    if (main_token_types[i] == TOKEN_KEYWORD_TRIGGER)
    {
      trigger_seen= 1;
      continue;
    }
    if (main_token_types[i] == TOKEN_KEYWORD_EVENT)
    {
      event_seen= 1;
      continue;
    }
    if (trigger_seen == 1)
    {
      if (main_token_types[i] == TOKEN_KEYWORD_FOR) trigger_for_seen= 1;
      if (trigger_for_seen == 1)
      {
        if (main_token_types[i] == TOKEN_KEYWORD_ROW) trigger_row_seen= 1;
      }
    }
    if (event_seen == 1)
    {
      if (main_token_types[i] == TOKEN_KEYWORD_ON)
      {
        event_on_seen= 1;
      }
      if (event_on_seen == 1)
      {
        if (main_token_types[i] == TOKEN_KEYWORD_DO)
        {
          *i_of_do= i;
          event_do_seen= 1;
        }
      }
    }
    if ((function_seen == 1) || (procedure_seen == 1))
    {
      if ((parameter_list_seen == 0) && (main_token_types[i] == TOKEN_TYPE_OPERATOR) && (main_token_lengths[i] == 1))
      {
        if (text.mid(main_token_offsets[i], main_token_lengths[i]) == "(")
        {
          ++parentheses_count;
          continue;
        }
        if (text.mid(main_token_offsets[i], main_token_lengths[i]) == ")")
        {
          --parentheses_count;
          if (parentheses_count == 0)
          {
            parameter_list_seen= 1;
            continue;
          }
        }
      }
    }

    if ((function_seen == 1) && (parameter_list_seen == 1) && (characteristic_seen == 0))
    {
      if (text.mid(main_token_offsets[i], main_token_lengths[i]) == "(")
      {
        ++parentheses_count;
        continue;
      }
      if (text.mid(main_token_offsets[i], main_token_lengths[i]) == ")")
      {
        --parentheses_count;
        continue;
      }
      if (parentheses_count != 0)
      {
        continue;
      }
      if (main_token_types[i] == TOKEN_KEYWORD_RETURNS)
      {
        continue;
      }
      if (data_type_seen == 0)
      {
        if ((main_token_types[i] == TOKEN_KEYWORD_ASCII)
         || (main_token_types[i] == TOKEN_KEYWORD_BIGINT)
         || (main_token_types[i] == TOKEN_KEYWORD_BFILE)
         || (main_token_types[i] == TOKEN_KEYWORD_BIGINT)
         || (main_token_types[i] == TOKEN_KEYWORD_BINARY)
         || (main_token_types[i] == TOKEN_KEYWORD_BINARY_DOUBLE)
         || (main_token_types[i] == TOKEN_KEYWORD_BINARY_FLOAT)
         || (main_token_types[i] == TOKEN_KEYWORD_BIT)
         || (main_token_types[i] == TOKEN_KEYWORD_BLOB)
         || (main_token_types[i] == TOKEN_KEYWORD_BOOL)
         || (main_token_types[i] == TOKEN_KEYWORD_BOOLEAN)
         || (main_token_types[i] == TOKEN_KEYWORD_CHAR)
         || (main_token_types[i] == TOKEN_KEYWORD_CHARACTER)
         || (main_token_types[i] == TOKEN_KEYWORD_DATE)
         || (main_token_types[i] == TOKEN_KEYWORD_DATETIME)
         || (main_token_types[i] == TOKEN_KEYWORD_DEC)
         || (main_token_types[i] == TOKEN_KEYWORD_DECIMAL)
         || (main_token_types[i] == TOKEN_KEYWORD_DOUBLE)
         || (main_token_types[i] == TOKEN_KEYWORD_ENUM)
         || (main_token_types[i] == TOKEN_KEYWORD_FLOAT)
         || (main_token_types[i] == TOKEN_KEYWORD_GEOMETRY)
         || (main_token_types[i] == TOKEN_KEYWORD_GEOMETRYCOLLECTION)
         || (main_token_types[i] == TOKEN_KEYWORD_INT)
         || (main_token_types[i] == TOKEN_KEYWORD_INTEGER)
         || (main_token_types[i] == TOKEN_KEYWORD_JSON)
         || (main_token_types[i] == TOKEN_KEYWORD_LINESTRING)
         || (main_token_types[i] == TOKEN_KEYWORD_LONGBLOB)
         || (main_token_types[i] == TOKEN_KEYWORD_LONGTEXT)
         || (main_token_types[i] == TOKEN_KEYWORD_MEDIUMBLOB)
         || (main_token_types[i] == TOKEN_KEYWORD_MEDIUMINT)
         || (main_token_types[i] == TOKEN_KEYWORD_MEDIUMTEXT)
         || (main_token_types[i] == TOKEN_KEYWORD_MULTILINESTRING)
         || (main_token_types[i] == TOKEN_KEYWORD_MULTIPOINT)
         || (main_token_types[i] == TOKEN_KEYWORD_MULTIPOLYGON)
         || (main_token_types[i] == TOKEN_KEYWORD_NUMBER)
         || (main_token_types[i] == TOKEN_KEYWORD_NUMERIC)
         || (main_token_types[i] == TOKEN_KEYWORD_NVARCHAR2)
         || (main_token_types[i] == TOKEN_KEYWORD_POINT)
         || (main_token_types[i] == TOKEN_KEYWORD_POLYGON)
         || (main_token_types[i] == TOKEN_KEYWORD_RAW)
         || (main_token_types[i] == TOKEN_KEYWORD_SERIAL)
         || (main_token_types[i] == TOKEN_KEYWORD_SET)
         || (main_token_types[i] == TOKEN_KEYWORD_SMALLINT)
         || (main_token_types[i] == TOKEN_KEYWORD_TIME)
         || (main_token_types[i] == TOKEN_KEYWORD_TIMESTAMP)
         || (main_token_types[i] == TOKEN_KEYWORD_TINYBLOB)
         || (main_token_types[i] == TOKEN_KEYWORD_TINYINT)
         || (main_token_types[i] == TOKEN_KEYWORD_TINYTEXT)
         || (main_token_types[i] == TOKEN_KEYWORD_UNICODE)
         || (main_token_types[i] == TOKEN_KEYWORD_VARCHAR)
         || (main_token_types[i] == TOKEN_KEYWORD_VARCHAR2)
         || (main_token_types[i] == TOKEN_KEYWORD_VARCHARACTER)
         || (main_token_types[i] == TOKEN_KEYWORD_YEAR))
        {
          data_type_seen= 1;
          continue;
        }
      }
      if (data_type_seen == 1)
      {
        if ((main_token_types[i] == TOKEN_KEYWORD_PRECISION)
         || (main_token_types[i] == TOKEN_KEYWORD_VARYING)
         || (main_token_types[i] == TOKEN_KEYWORD_UNSIGNED)
         || (main_token_types[i] == TOKEN_KEYWORD_ZEROFILL))
        {
          continue;
        }
        if ((main_token_types[i] == TOKEN_KEYWORD_CHARSET) || (main_token_types[i] == TOKEN_KEYWORD_CHARACTER))
        {
          character_set_seen= 1;
          continue;
        }
        if (main_token_types[i] == TOKEN_KEYWORD_COLLATE)
        {
          collate_seen= 1;
          continue;
        }
        if (character_set_seen == 1)
        {
          if (main_token_types[i] == TOKEN_KEYWORD_SET)
          {
            continue;
          }
          character_set_seen= 0;
          continue;
        }
        if (collate_seen == 1)
        {
          collate_seen= 0;
          continue;
        }
      }
    }

    if (((function_seen == 1) || (procedure_seen == 1)) && (parameter_list_seen == 1))
    {
      if ((text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "COMMENT")
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "LANGUAGE")
       || (main_token_types[i] == TOKEN_KEYWORD_SQL)
       || (main_token_types[i] == TOKEN_KEYWORD_DETERMINISTIC)
       || (main_token_types[i] == TOKEN_KEYWORD_NOT)
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "CONTAINS")
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "NO")
       || (main_token_types[i] == TOKEN_KEYWORD_READS)
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "DATA")
       || (main_token_types[i] == TOKEN_KEYWORD_MODIFIES)
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "SECURITY")
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "DEFINER")
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "INVOKER")
       || (main_token_types[i] == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
       || (main_token_types[i] == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE)
       || (main_token_types[i] == TOKEN_TYPE_LITERAL))
      {
        characteristic_seen= 1;
        continue;
      }
    }
    /* Todo: consider checking for ROLE too, but it is not a reserved word. */
    if ((main_token_types[i] == TOKEN_KEYWORD_DATABASE)
     || (main_token_types[i] == TOKEN_KEYWORD_INDEX)
     || (main_token_types[i] == TOKEN_KEYWORD_LOGFILE)
     || (main_token_types[i] == TOKEN_KEYWORD_SERVER)
     || (main_token_types[i] == TOKEN_KEYWORD_TABLE)
     || (main_token_types[i] == TOKEN_KEYWORD_TABLESPACE)
     || (main_token_types[i] == TOKEN_KEYWORD_VIEW))
    {
      if (function_seen + procedure_seen + trigger_seen + event_seen == 0)
      {
        return -1;
      }
    }
    if ((function_seen == 1) || (procedure_seen == 1))
    {
      if (parameter_list_seen == 0) continue;
      if ((main_token_types[i] == TOKEN_KEYWORD_IS)
       || (main_token_types[i] == TOKEN_KEYWORD_AS))
      {
        continue;
      }
      return i;
    }
  }
  return -1;
}


/*
  The special cases of FUNCTION, ROW, DO, END, BEGIN. We don't want to mark them as special unless
  we know they're really serving a non-identifier role in the statement, but they might be,
  because they're not reserved. This has to be right because later we might count BEGINs
  minus ENDs in order to figure out whether a compound statement has ended.
  FUNCTION is a keyword if: previous == DROP or CREATE [definer=...] or DROP (this is known in advance)
  ROW is a keyword if: previous == EACH
  DO is a keyword if: last statement-beginner keyword was WHILE
                      and next is a statement-beginner keyword or a label
                      (but actually this just checks whether we're within WHILE -- Todo: must fix that someday)
  END is a keyword if: previous == ; or BEGIN
  BEGIN is a keyword if: previous ==  ; or : or BEGIN or DO or ELSE or LOOP or ROW or THEN
                         or previous = ) and word before matching ( is PROCEDURE or FUNCTION or TRIGGER
                         and next is not :
                         or it's first word in main body or handler body, which is what causes most of the difficulties.
*/
void MainWindow::tokens_to_keywords_revert(int i_of_body, int i_of_function, int i_of_do, QString text, int start)
{
  if ((hparse_dbms_mask & FLAG_VERSION_TARANTOOL) != 0) return;
  int i;                                                /* index of current token */
  int i_prev= -1;                                       /* index of last non-comment token */
  int i_of_while= -1;                                   /* index of last keyword = while */

  for (i= start; main_token_lengths[i] != 0; ++i)
  {
    /* If this is a comment then don't note its position i.e. i_prev is only for non-comment tokens */
    if ((main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
     || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_SLASH)
     || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_MINUS))
    {
      continue;
    }
    if (i_prev != -1)
    {
      /* If this is ":" then the last item can't have been a keyword */
      if (QString::compare(text.mid(main_token_offsets[i], main_token_lengths[i]), ":") == 0)
      {
        if ((main_token_types[i_prev] >= TOKEN_KEYWORDS_START))
        {
          main_token_types[i_prev]= TOKEN_TYPE_OTHER;
        }
      }
      /* If this is FUNCTION then it's not a keyword unless previous was DROP or CREATE [definer=...] */
      if (main_token_types[i] == TOKEN_KEYWORD_FUNCTION)
      {
        if ((i == i_of_function) || (main_token_types[i_prev] == TOKEN_KEYWORD_DROP)) ;
        else main_token_types[i]= TOKEN_TYPE_OTHER;
      }
      /* If this is ROW then it's not a keyword unless previous was EACH as in FOR EACH ROW */
      if (main_token_types[i] == TOKEN_KEYWORD_ROW)
      {
        if (main_token_types[i_prev] == TOKEN_KEYWORD_EACH) ;
        else main_token_types[i]= TOKEN_TYPE_OTHER;
      }
      /* If this is DO then it's not a keyword unless it's after WHILE (condition) */
      if (main_token_types[i] == TOKEN_KEYWORD_DO)
      {
        if (i_of_while != -1) i_of_while= -1;
        else
        {
          if (i != i_of_do)
          {
            main_token_types[i]= TOKEN_TYPE_OTHER;
          }
        }
      }
      /* If this is END then it's not a keyword unless previous was ; or BEGIN */
      if (main_token_types[i] == TOKEN_KEYWORD_END)
      {
        if ((QString::compare(text.mid(main_token_offsets[i_prev], main_token_lengths[i_prev]), ";") == 0)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_BEGIN)) ;
        else main_token_types[i]= TOKEN_TYPE_OTHER;
      }
      /* If this is BEGIN then it's not a keyword unless it's body start or prev = BEGIN|DO|ELSE|LOOP|ROW|THEN */
      if (main_token_types[i] == TOKEN_KEYWORD_BEGIN)
      {
        if ((i == i_of_body)
        || (QString::compare(text.mid(main_token_offsets[i_prev], main_token_lengths[i_prev]), ":") == 0)
        || (QString::compare(text.mid(main_token_offsets[i_prev], main_token_lengths[i_prev]), ";") == 0)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_BEGIN)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_DO)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_ELSE)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_LOOP)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_ROW)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_THEN)) ;
        else main_token_types[i]= TOKEN_TYPE_OTHER;
      }
    }
    if (main_token_types[i] == TOKEN_KEYWORD_WHILE) i_of_while= i;
    if ((QString::compare(text.mid(main_token_offsets[i], main_token_lengths[i]), "HANDLER", Qt::CaseInsensitive) == 0)
     && (main_token_types[next_token(i)] == TOKEN_KEYWORD_FOR))
    {
      /* DECLARE ... HANDLER FOR might be followed by BEGIN so we have to find where statement starts */
      int i_plus_1, i_plus_2, i_plus_3, i_plus_4, i_next;
      i= next_token(i);
      for (;;)
      {
        i_plus_1= next_token(i);
        i_plus_2= next_token(i_plus_1);
        i_plus_3= next_token(i_plus_2);
        i_plus_4= next_token(i_plus_3);
        if ((main_token_types[i_plus_1] == TOKEN_KEYWORD_SQLSTATE)
         && (QString::compare(text.mid(main_token_offsets[i_plus_2], main_token_lengths[i_plus_2]), "VALUE", Qt::CaseInsensitive) == 0))
        {
          if (main_token_types[i_plus_3] >= TOKEN_KEYWORDS_START) main_token_types[i_plus_3]= TOKEN_TYPE_OTHER;
          i_next= i_plus_4;
        }
        else if (main_token_types[i_plus_1] == TOKEN_KEYWORD_SQLSTATE)
        {
          if (main_token_types[i_plus_2] >= TOKEN_KEYWORDS_START) main_token_types[i_plus_2]= TOKEN_TYPE_OTHER;
          i_next= i_plus_3;
        }
        else if ((main_token_types[i_plus_1] == TOKEN_KEYWORD_NOT)
         && (QString::compare(text.mid(main_token_offsets[i_plus_2], main_token_lengths[i_plus_2]), "FOUND", Qt::CaseInsensitive) == 0))
        {
          i_next= i_plus_3;
        }
        else
        {
          if (main_token_types[i_plus_1] >= TOKEN_KEYWORDS_START) main_token_types[i_plus_1]= TOKEN_TYPE_OTHER;
          i_next= i_plus_2;
        }
        if (QString::compare(text.mid(main_token_offsets[i_next], main_token_lengths[i_next]), ",") == 0)
        {
          i= i_next;
          continue;
        }
        break;
      }
      /* now i_next == first_word_in_statement in DECLARE HANDLER, unless syntax is bad, I hope */
      i= i_next;
    }
    i_prev= i;
  }
}

/* returns next token after i, skipping comments, but do not go past end */
int MainWindow::next_token(int i)
{
  int i2;

  for (i2= i;;)
  {
    if (main_token_lengths[i2] == 0) break;
    ++i2;
    if ((main_token_types[i2] == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
    || (main_token_types[i2] == TOKEN_TYPE_COMMENT_WITH_SLASH)
    || (main_token_types[i2] == TOKEN_TYPE_COMMENT_WITH_MINUS))
    {
      continue;
    }
    break;
  }
  return i2;
}

/*
  Called by mouseMoveEvent for hovering in the statement widget.
  Called from deep within hparse_f_multiblock().
  Pass: offset in main_token lists
  Return: information string
*/
QString MainWindow::token_reftype(int i, bool is_hover, int token_type, char reftype_parameter)
{
#define MAX_REFTYPEWORD_LENGTH 64
struct reftypewords {
   char  chars[MAX_REFTYPEWORD_LENGTH];
   unsigned short int reserved_flags;
   unsigned short int built_in_function_flags;
   unsigned short int token_keyword;
};

    /* reftype_values strings must correspond to the order of TOKEN_REFTYPE_ #defines */
    /* Todo: strings shouldn't be blank */
  const reftypewords reftype_values[]=
    {
    {"", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_ANY},
    {"alias-of-column ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_ALIAS_OF_COLUMN},
    {"alias-of-table ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_ALIAS_OF_TABLE},
    {"attribute ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_ATTRIBUTE},
    {"auto_increment ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_AUTO_INCREMENT},
    {"channel ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_CHANNEL},
    {"character-set ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_CHARACTER_SET},
    {"collation ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_COLLATION},
    {"column ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_COLUMN},
    {"column-or-user-variable ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_COLUMN_OR_USER_VARIABLE},
    {"column-or-variable ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_COLUMN_OR_VARIABLE},
    {"comment ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_COMMENT},
    {"condition-define ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_CONDITION_DEFINE},
    {"condition-refer ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_CONDITION_REFER},
    {"condition-or-cursor ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_CONDITION_OR_CURSOR},
    {"constraint ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_CONSTRAINT},
    {"cursor-define ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_CURSOR_DEFINE},
    {"cursor-refer ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_CURSOR_REFER},
    {"database ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE}, /* or schema */
    {"database-or-constraint ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_CONSTRAINT},
    {"database-or-event ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_EVENT},
    {"database-or-function ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION},
    {"database-or-function-or-procedure ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION_OR_PROCEDURE},
    {"database-or-function-or-variable ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_FUNCTION_OR_VARIABLE},
    {"database-or-package ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_PACKAGE},
    {"database-or-procedure ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_PROCEDURE},
    {"database-or-sequence ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_SEQUENCE},
    {"database-or-table ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_TABLE},
    {"database-or-table-or-row ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_ROW},
    {"database-or-table-or-column ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN},
    {"database-or-table-or-column-or-function ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN_OR_FUNCTION},
    {"database-or-table-or-variable-or-function ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_VARIABLE_OR_FUNCTION},
    {"database-or-table-or-row-or-function-or-column ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_ROW_OR_FUNCTION_OR_COLUMN},
    {"database-or-table-or-row-or-function-or-variable ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_ROW_OR_FUNCTION_OR_VARIABLE},
    {"database-or-table-or-column-or-function-or-variable ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_COLUMN_OR_FUNCTION_OR_VARIABLE},
    {"database-or-table-or-row-or-column-or-function-or-variable ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_TABLE_OR_ROW_OR_COLUMN_OR_FUNCTION_OR_VARIABLE},
    {"database-or-trigger ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_TRIGGER},
    {"database-or-view ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DATABASE_OR_VIEW},
    {"directory ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_DIRECTORY},
    {"engine ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_ENGINE},
    {"event ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_EVENT},
    {"file ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_FILE},
    {"function ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_FUNCTION},
    {"function-or-procedure ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_FUNCTION_OR_PROCEDURE},
    {"function-or-variable ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_FUNCTION_OR_VARIABLE},
    {"handler-alias ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_HANDLER_ALIAS},
    {"host ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_HOST},
    {"index ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_INDEX},
    {"introducer ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_INTRODUCER},
    {"key_cache ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_KEY_CACHE},
    {"label-define ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_LABEL_DEFINE},
    {"label-refer ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_LABEL_REFER},
    {"length ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_LENGTH},
    {"package ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_PACKAGE},
    {"parameter-define ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_PARAMETER_DEFINE},
    {"parameter-refer ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_PARAMETER_REFER},
    {"parser ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_PARSER},
    {"plugin ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_PLUGIN},
    {"procedure ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_PROCEDURE},
    /* plus {"", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_RESERVED_FUNCTION */
    {"partition ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_PARTITION},
    {"partition-number ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_PARTITION_NUMBER},
    {"password ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_PASSWORD},
    {"role ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_ROLE},
    {"row ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_ROW},
    {"row-or-variable ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_ROW_OR_VARIABLE},
    {"savepoint ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_SAVEPOINT},
    {"scale ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_SCALE},
    {"sequence ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_SEQUENCE},
    {"server ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_SERVER},
    {"sqlstate ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_SQLSTATE},
    {"statement ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_STATEMENT},
    {"subpartition ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_SUBPARTITION},
    {"switch ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_SWITCH_NAME},
    {"table ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_TABLE},
    {"table-or-column ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_TABLE_OR_COLUMN},
    {"table-or-column-or-function ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_TABLE_OR_COLUMN_OR_FUNCTION},
    {"table-or-column-or-function-or-variable ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_TABLE_OR_COLUMN_OR_FUNCTION_OR_VARIABLE},
    {"table-or-row ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_TABLE_OR_ROW},
    {"tablespace ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_TABLESPACE},
    {"transaction ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_TRANSACTION},
    {"trigger ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_TRIGGER},
    {"user ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_USER},
    {"user-variable ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_USER_VARIABLE},
    {"variable ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_VARIABLE},         /* i.e. either USER_VARIABLE or DECLARED VARIABLE */
    {"variable-define ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_VARIABLE_DEFINE},
    {"variable-refer ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_VARIABLE_REFER},
    {"view ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_VIEW},
    {"with-table ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_WITH_TABLE},
    {"wrapper ", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_WRAPPER},
    {"", FLAG_VERSION_ALL, 0, TOKEN_REFTYPE_MAX}
  };

  QString s= "";
  int token_flag= main_token_flags[i];

  if (is_hover)
  {
    if ((token_flag & TOKEN_FLAG_IS_ERROR) != 0) s= "(error) ";
    if ((token_flag & TOKEN_FLAG_IS_FUNCTION) != 0) s= "(function) ";
  }
  if ((token_type >= TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
   && (token_type <= TOKEN_TYPE_LITERAL))
  {
    int array_subscript= reftype_parameter;
    s.append((char*)reftype_values[array_subscript].chars);
    s.append("literal");
  }
  else if ((token_type >= TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK)
   && (token_type <= TOKEN_TYPE_IDENTIFIER))
  {
    s.append("[");
    int array_subscript= reftype_parameter;
    s.append((char*)reftype_values[array_subscript].chars);
    s.append("identifier]");
  }
  else if ((token_type >= TOKEN_TYPE_COMMENT_WITH_SLASH)
   && (token_type <= TOKEN_TYPE_COMMENT_WITH_MINUS))
  {
    s.append("comment");
  }
  else if (token_type == TOKEN_TYPE_OPERATOR)
  {
    s.append("operator");
  }
  else if (token_type == TOKEN_TYPE_OTHER)
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
  return s;
}

/*
  Todo: disconnect old if already connected.
  TODO: LOTS OF ERROR CHECKS NEEDED IN THIS!
*/
int MainWindow::connect_mysql(unsigned int connection_number)
{
  QString ldbms_return_string;

  ldbms_return_string= "";

  /* Find libmysqlclient. Prefer ld_run_path, within that prefer libmysqlclient.so.18.
    Generally libmysqlclient.so will have a symlink to libmariadb.so
    or to libmariadbclient.so. But not always. Not on Windows.
    And not on some MariaDB releases -- there were some renames and
    some bugs with missing symlinks.
    Also, if ocelot_dbms=mysql, the libmysqlclient searches come first,
    but if ocelot_dbms=mariadb, the libmariadb searches come first.
    So try multiple combinations. e.g. if libmysqlclient.so.18 didn't
    get loaded, try libmysqlclient without a version number.
  */
  for (int i= 1; i < 12; ++i)
  {
    QString li_path;
    int li_lib;
    if (i == 1) {if (connections_dbms[0] == DBMS_MARIADB) continue; li_path= ocelot_ld_run_path; li_lib= WHICH_LIBRARY_LIBMYSQLCLIENT18; }
    if (i == 2) {if (connections_dbms[0] == DBMS_MARIADB) continue; li_path= ocelot_ld_run_path; li_lib= WHICH_LIBRARY_LIBMYSQLCLIENT; }
    if (i == 3) {                                                   li_path= ocelot_ld_run_path; li_lib= WHICH_LIBRARY_LIBMARIADBCLIENT; }
    if (i == 4) {                                                   li_path= ocelot_ld_run_path; li_lib= WHICH_LIBRARY_LIBMARIADB; }
    if (i == 5) {if (connections_dbms[0] != DBMS_MARIADB) continue; li_path= ocelot_ld_run_path; li_lib= WHICH_LIBRARY_LIBMYSQLCLIENT18; }
    if (i == 6) {if (connections_dbms[0] != DBMS_MARIADB) continue; li_path= ocelot_ld_run_path; li_lib= WHICH_LIBRARY_LIBMYSQLCLIENT; }
    if (i == 7) {if (connections_dbms[0] == DBMS_MARIADB) continue; li_path= ""                ; li_lib= WHICH_LIBRARY_LIBMYSQLCLIENT18; }
    if (i == 8) {if (connections_dbms[0] == DBMS_MARIADB) continue; li_path= ""                ; li_lib= WHICH_LIBRARY_LIBMYSQLCLIENT; }
    if (i == 9) {                                                   li_path= ""                ; li_lib= WHICH_LIBRARY_LIBMARIADBCLIENT; }
    if (i == 10){                                                   li_path= ""                ; li_lib= WHICH_LIBRARY_LIBMARIADB; }
    if (i == 11){if (connections_dbms[0] != DBMS_MARIADB) continue; li_path= ""                ; li_lib= WHICH_LIBRARY_LIBMYSQLCLIENT18; }
    if (i == 12){if (connections_dbms[0] != DBMS_MARIADB) continue; li_path= ""                ; li_lib= WHICH_LIBRARY_LIBMYSQLCLIENT; }
    lmysql->ldbms_get_library(li_path, &is_libmysqlclient_loaded, &libmysqlclient_handle, &ldbms_return_string, li_lib);
    if (is_libmysqlclient_loaded == 1)
    {
      break;
    }
  }


  /* Todo: The following errors would be better if we put them in diagnostics the usual way. */

  if (is_libmysqlclient_loaded == -2)
  {
    /* Todo: QMessageBox should have a parent, use "= new" */
    QMessageBox msgbox;
    char error_message[2048];
    char t_ldbms_return_string[2048];
    strcpy(t_ldbms_return_string, ldbms_return_string.toUtf8());
    sprintf(error_message, er_strings[er_off + ER_LIBMYSQLCLIENT_DOES_NOT_HAVE], t_ldbms_return_string);
    msgbox.setText(error_message);
    msgbox.exec();
    //delete lmysql;
    return 1;
  }

  if (is_libmysqlclient_loaded == 0)
  {
    /* Todo: QMessageBox should have a parent, use "= new" */
    QMessageBox msgbox;
    char error_message[2048];
    char t_ldbms_return_string[2048];
    strcpy(t_ldbms_return_string, ldbms_return_string.toUtf8());
    sprintf(error_message, er_strings[er_off + ER_LIBMYSQLCLIENT_WAS_NOT_FOUND], t_ldbms_return_string);
    msgbox.setText(error_message);
    msgbox.exec();
    //delete lmysql;
    return 1;
  }

  if (is_mysql_library_init_done == false)
  {
    if (lmysql->ldbms_mysql_library_init(0, NULL, NULL))
    {
      /* Todo: QMessageBox should have a parent, use "= new" */
      QMessageBox msgbox;
      msgbox.setText(er_strings[er_off + ER_MYSQL_LIBRARY_INIT_FAILED]);
      msgbox.exec();
      return 1;
    }
    is_mysql_library_init_done= true;
  }
  /* Mysterious crash may happen with one particular MariaDB version. */
#ifdef OCELOT_OS_LINUX
  {
    QString s;
    s= lmysql->ldbms_mysql_get_client_info();
    if (s.contains("10.1.7", Qt::CaseInsensitive) == true)
    {
      QMessageBox msgbox;
      msgbox.setText("Warning: Detected MariaDB 10.1.7 client library. This version has been known to cause problems.");
      msgbox.exec();
    }
  }
#endif
  /* I decided this line is unnecessary, mysql_init is done in options_and_connect() */
  //lmysql->ldbms_mysql_init(&mysql[connection_number]);
  if (the_connect(connection_number))
  {
    put_diagnostics_in_result(connection_number);
    make_and_append_message_in_result(ER_FAILED_TO_CONNECT, 0, (char*)"");
    return 1;
  }
  make_and_put_message_in_result(ER_OK, 0, (char*)"");

  /*
    Initially ocelot_prompt == "mysql>" and ocelot_prompt_is_default == true.
    It might have changed if getenv("MYSQL_PS1") returned something.
    It might have changed if --prompt = prompt-format was specified.
    We can override it with "mariadb>" if the client library comes from MariaDB.
    That is, we depend on mysql_get_client_info() not mysql_get_host_info().
    That is, we say "mariadb>" although mariadb's client would probably say "\N [\d]>".
    For example mysql_get_client_info() might return "10.0.4-MariaDB".
    We'll set ocelot_prompt_is_default= false to ensure this only happens once.
    We are hoping that following statements don't override earlier PROMPT statements by user.
    TODO: This had to be abandoned because it didn't work properly with Qt4.
          Since I didn't know what the true problem was, or exactly which versions
          will work, I cancelled it. The todo is: revive it.
  */
  //{
  //  if (ocelot_prompt_is_default == true)
  //  {
  //    QString s;
  //    s= lmysql->ldbms_mysql_get_client_info();
  //    if (s.contains("MariaDB", Qt::CaseInsensitive) == true)
  //    {
  //      ocelot_prompt= "mariadb>";
  //    }
  //    ocelot_prompt_is_default= false;
  //  }
  //}

  statement_edit_widget->prompt_default= ocelot_prompt;
  statement_edit_widget->prompt_as_input_by_user= statement_edit_widget->prompt_default;

  /*
    Collect some variables in case they're needed for "prompt".
    Todo: handle errors better after mysql_ calls here.
    A possible error is: Error 1226 (42000) User ... has exceeded the 'max_queries_per_hour' resource
    A possible error is: Error 1820 (HY000) You must reset your password using ALTER USER statement before executing this statement.
    Not using the mysql_res global, since this is not for user to see.
  */
  int query_result= lmysql->ldbms_mysql_query(&mysql[connection_number], "select version(), database(), @@port, current_user(), connection_id()");
  if (query_result != 0 )
  {
    connect_mysql_error_box(er_strings[er_off + ER_MYSQL_QUERY_FAILED], connection_number);
    connections_is_connected[0]= 1;
    return 0;
  }
  MYSQL_RES *mysql_res_for_connect;
  MYSQL_ROW connect_row;
  QString s;
  int i;

  // unsigned long connect_lengths[1];
  mysql_res_for_connect= lmysql->ldbms_mysql_store_result(&mysql[connection_number]);
  if (mysql_res_for_connect == NULL)
  {
    connect_mysql_error_box(er_strings[er_off + ER_MYSQL_STORE_RESULT_FAILED], connection_number);
    connections_is_connected[0]= 1;
    return 0;
  }
  connect_row= lmysql->ldbms_mysql_fetch_row(mysql_res_for_connect);
  if (connect_row == NULL)
  {
    connect_mysql_error_box(er_strings[er_off + ER_MYSQL_FETCH_ROW_FAILED], connection_number);
    connections_is_connected[0]= 1;
    return 0;
  }
  /* lengths= lmysql->ldbms_mysql_fetch_lengths(mysql_res_for_connect); */
  statement_edit_widget->dbms_version= connect_row[0];
  statement_edit_widget->dbms_database= connect_row[1];
  statement_edit_widget->dbms_port= connect_row[2];
  s= connect_row[3];
  statement_edit_widget->dbms_current_user= s;
  i= s.indexOf(QRegExp("@"), 0);
  if (i > 0) s= s.left(i);
  else s= "";
  statement_edit_widget->dbms_current_user_without_host= s;
  statement_edit_widget->dbms_connection_id= atoi(connect_row[4]);
  /* Todo: find out why this returns capitalized e.g. "Localhost" rather than "localhost" */
  s= lmysql->ldbms_mysql_get_host_info(&mysql[connection_number]);
  i= s.indexOf(QRegExp(" "), 0);
  if (i > 0) s= s.left(i);
  statement_edit_widget->dbms_host= s;
  lmysql->ldbms_mysql_free_result(mysql_res_for_connect);
  get_sql_mode(TOKEN_KEYWORD_CONNECT, "", false, main_token_number);
  connections_is_connected[0]= 1;
  set_dbms_version_mask(statement_edit_widget->dbms_version);
  return 0;
}

void MainWindow::connect_mysql_error_box(QString s1, unsigned int connection_number)
{
  QString s2;
  char i_mysql_error_and_state[1024];
  int i_mysql_errno_result= lmysql->ldbms_mysql_errno(&mysql[connection_number]);
  s1.append(tr(" Warning: Connection succeeded, but server refused to provide some non-essential information due to Error "));
  sprintf(i_mysql_error_and_state, "%d (%s) ", i_mysql_errno_result, lmysql->ldbms_mysql_sqlstate(&mysql[connection_number]));
  s1.append(i_mysql_error_and_state);
  s2= lmysql->ldbms_mysql_error(&mysql[connection_number]);
  s1.append(s2);
  /* Todo: QMessageBox should have a parent, use "= new" */
  QMessageBox msgbox;
  msgbox.setText(s1);
  msgbox.exec();
}

/*
 We call set_dbms_version_mask(ocelot_dbms) from MainWindow() after
 calling mysql_options_2(),in which case the pass should be the original
 default ("mysql") or something the user specified with --ocelot_dbms
 (probably "mysql" or "mariadb" but could contain version
 before or after and we don't check validity).
 We call set_dbms_version_mask(statement_edit_widget->dbms_version)
 from connect_mysql() after connection succeeds, in which case the
 pass would be what "select version()" returns, such as 10.2.0-MariaDB,
 or 8.0.11 (MySQL might not say 'mysql'). Assume Percona is like MySQL.
 If you must know the vendor, then maybe select @@version_comment
 is good, but I don't know if @@version_comment was in old versions.
 Warn: get_sql_mode() might change dbms_version_mask.
 Todo: a message like "Don't recognize MySQL/MariaDB version" would be nice.
*/
void MainWindow::set_dbms_version_mask(QString version)
{
  if (version.contains("mariadb", Qt::CaseInsensitive) == true)
  {
    if (version.contains("10.0.") == true)
    {
      dbms_version_mask= (FLAG_VERSION_MARIADB_5_5 | FLAG_VERSION_MARIADB_10_0);
    }
    else if (version.contains("10.1.") == true)
    {
      dbms_version_mask= (FLAG_VERSION_MARIADB_5_5 | FLAG_VERSION_MARIADB_10_0 | FLAG_VERSION_MARIADB_10_1);
    }
    else if (version.contains("10.2.") == true)
    {
      QString s= version.mid(version.indexOf("10.2.") + 5, 1);
      if (s >= "3")
        dbms_version_mask= (FLAG_VERSION_MARIADB_5_5 | FLAG_VERSION_MARIADB_10_0 | FLAG_VERSION_MARIADB_10_1 | FLAG_VERSION_MARIADB_10_2_2 | FLAG_VERSION_MARIADB_10_2_3);
      else
        dbms_version_mask= (FLAG_VERSION_MARIADB_5_5 | FLAG_VERSION_MARIADB_10_0 | FLAG_VERSION_MARIADB_10_1 | FLAG_VERSION_MARIADB_10_2_2);
    }
    else
    {
      dbms_version_mask= FLAG_VERSION_MARIADB_ALL;
    }
  }
#ifdef DBMS_TARANTOOL
  else if (version.contains("tarantool", Qt::CaseInsensitive) == true)
  {
    dbms_version_mask= FLAG_VERSION_TARANTOOL;
  }
#endif
  /* MySQL's version string might not contain 'mysql */
  else /* if (version.contains("mysql", Qt::CaseInsensitive) == true) */
  {
    if ((version.contains("5.6") == true)
     && (version.contains("5.5.6") == false))
    {
      dbms_version_mask= (FLAG_VERSION_MYSQL_5_5 | FLAG_VERSION_MYSQL_5_6);
    }
    else if ((version.contains("5.7") == true)
          && (version.contains("5.5.7") == false))
    {
      dbms_version_mask= (FLAG_VERSION_MYSQL_5_5 | FLAG_VERSION_MYSQL_5_6 | FLAG_VERSION_MYSQL_5_7);
    }
    else if (version.contains("8.0") == true)
    {
      dbms_version_mask= (FLAG_VERSION_MYSQL_5_5 | FLAG_VERSION_MYSQL_5_6 | FLAG_VERSION_MYSQL_5_7 | FLAG_VERSION_MYSQL_8_0);
    }
    else if (version.contains("mysql", Qt::CaseInsensitive) == true)
    {
      dbms_version_mask= FLAG_VERSION_MYSQL_ALL;
    }
    else dbms_version_mask= FLAG_VERSION_MYSQL_OR_MARIADB_ALL;
  }
}

/*
  Pass: i_start = where we are now, i_increment = +1 or -1
  Go forward | backward skipping comments
  Stop if end or start of all input (we pay no attention to statement
  start|end).
  Todo: this could be useful in other places where we currently
        are making temporary copies that don't contain comments.
*/
int MainWindow::next_i(int i_start, int i_increment)
{
  int i= i_start;
  for (;;)
  {
    if (i == 0) break;
    if (main_token_lengths[i] == 0) break;
    i= i + i_increment;
    if ((main_token_types[i] != TOKEN_TYPE_COMMENT_WITH_SLASH)
      && (main_token_types[i] != TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
      && (main_token_types[i] != TOKEN_TYPE_COMMENT_WITH_MINUS))
     break;
  }
  return i;
}

/*
  We use sql_mode to decide whether "..." is an identifier or a literal.
  So we try to get its value at connect time or if user says
  SET SESSION SQL_MODE ... or SET @@session.sql_mode
  This might fail if (ocelot_statement_syntax_checker.toInt() < 1
  but I'm not expecting people will set that and then expect recognizing.
  Todo: Handle the undocumented syntax SET @@sql_mode=ANSI.
        If MariaDB, we'd have to ensure that we're not in a block
        that has DECLARE ANSI. But it's clearly used by some people.
  Todo: track global value too so we can handle sql_mode=default.
  Todo: try to do this while parsing, before you can ask the server.
  Todo: check: did the server return a warning for this statement?
  Warning: We might call this from within hparse_f_multi_block().
*/
bool MainWindow::get_sql_mode(int who_is_calling,
                              QString text,
                              bool is_in_hparse,
                              int start_token_number)
{
  QString sql_mode_string;
  bool sql_mode_string_seen= false;
  bool must_ask_server= false;
  QString token;
  bool default_is_session= true;
  bool immediate_is_session= true;
  QString var_name;
  bool old_hparse_sql_mode_ansi_quotes= hparse_sql_mode_ansi_quotes;
  if (who_is_calling == TOKEN_KEYWORD_SET)
  {
    int i;
    int t;
    for (i= start_token_number; main_token_lengths[i] != 0; ++i)
    {
      {
        QString token3= text.mid(main_token_offsets[i], main_token_lengths[i]);
        if ((token3 == ";")
         || (token3 == ocelot_delimiter_str)) break;
      }
      t= main_token_types[i];
      if ((t == TOKEN_KEYWORD_SESSION)
      || (t == TOKEN_KEYWORD_LOCAL))
      {
        default_is_session= true;
        continue;
      }
      if ((t == TOKEN_KEYWORD_GLOBAL)
        || (t == TOKEN_KEYWORD_PERSIST)
        || (t == TOKEN_KEYWORD_PERSIST_ONLY))
      {
        default_is_session= false;
        continue;
      }
      var_name= text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper();
      if ((connect_stripper(var_name, false) == "SQL_MODE")
       || (var_name == "@@SQL_MODE"))
      {
        int i_minus_1= next_i(i, -1);
        int i_minus_2= next_i(i_minus_1, -1);
        int i_plus_1= next_i(i, +1);
        int i_plus_2= next_i(i_plus_1, +1);
        int i_plus_3= next_i(i_plus_2, +1);
        if (var_name == "@@SQL_MODE") immediate_is_session= true;
        else
        {
          immediate_is_session= default_is_session;
          token= text.mid(main_token_offsets[i_minus_1], main_token_lengths[i_minus_1]);
          if (token == ".")
          {
            token= text.mid(main_token_offsets[i_minus_2], main_token_lengths[i_minus_2]).toUpper();
            if ((token == "@@SESSION")
             || (token == "@@LOCAL"))
            {
              immediate_is_session= true;
            }
            if ((token == "@@GLOBAL")
             || (token == "@@PERSIST")
             || (token == "@@PERSIST_ONLY"))
            {
              immediate_is_session= false;
            }
          }
          if (immediate_is_session == false) continue;
        }
        token= text.mid(main_token_offsets[i_plus_1], main_token_lengths[i_plus_1]);
        if ((token == "=") || (token == ":="))
        {
          bool is_simple_literal= false;
          /* is_simple_literal=true if e.g. set sql_mode='ansi' */
          t= main_token_types[i_plus_2];
          if ((t == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
           || (t == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE)
           || (t == TOKEN_TYPE_LITERAL))
          {
            is_simple_literal= true;
          }
          /* is_simple_literal=true if e.g. set sql_mode=ansi */
          if (is_simple_literal == false)
          {
            if ((t == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK)
             || (t == TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE)
             || (t == TOKEN_TYPE_IDENTIFIER)
             || (t >= TOKEN_TYPE_OTHER))
            {
               QString token4= text.mid(main_token_offsets[i_plus_2], main_token_lengths[i_plus_2]);
               if ((token4.left(1) != "@")
                && (main_token_reftypes[i_plus_2] != TOKEN_REFTYPE_VARIABLE_REFER))
                 is_simple_literal= true;
            }
          }
          /* is_simple_literal=false if not simple e.g. 'ansi,ansi' */
          if (is_simple_literal == true)
          {
            QString token3= text.mid(main_token_offsets[i_plus_3], main_token_lengths[i_plus_3]);
            if ((token3 != ",")
             && (token3 != ";")
             && (token3 != ocelot_delimiter_str))
            {
              is_simple_literal= false;
            }
          }
          if (is_simple_literal == false)
          {
            /* User has said SET SESSION sql_mode = not-literal which server can interpret */
            sql_mode_string_seen= false;
            must_ask_server= true;
          }
          else
          {
            /* User has said SET SESSION sql_mode = literal which we can interpret as client */
            sql_mode_string= text.mid(main_token_offsets[i_plus_2], main_token_lengths[i_plus_2]);
            sql_mode_string_seen= true;
            must_ask_server= false;
          }
        }
      }
    }
  }
  if ((who_is_calling == TOKEN_KEYWORD_CONNECT)
   || (must_ask_server == true))
  {
    if (is_in_hparse) return false;
    QString s= select_1_row("select @@session.sql_mode");
    if (s == "")
    {
      sql_mode_string= select_1_row_result_1;
      sql_mode_string_seen= true;
    }
  }
  if (sql_mode_string_seen == true)
  {
    if ((sql_mode_string.contains("ANSI", Qt::CaseInsensitive) == true)
     || (sql_mode_string.contains("DB2", Qt::CaseInsensitive) == true)
     || (sql_mode_string.contains("MAXDB", Qt::CaseInsensitive) == true)
     || (sql_mode_string.contains("MSSQL", Qt::CaseInsensitive) == true)
     || (sql_mode_string.contains("ORACLE", Qt::CaseInsensitive) == true)
     || (sql_mode_string.contains("POSTGRESQL", Qt::CaseInsensitive) == true))
    {
      if (is_in_hparse == false) sql_mode_ansi_quotes= true;
      hparse_sql_mode_ansi_quotes= true;
    }
    else
    {
      if (is_in_hparse)
      {
        /*
           We'll decide it's false if it contains some valid setting.
           We'll decide it's unknown (and return unchanged) otherwise.
           Obviously this is unreliable, but what else can I do?
        */
        sql_mode_string= sql_mode_string.toUpper();
        if ((connect_stripper(sql_mode_string, false) == "")
           || (sql_mode_string.contains("ALLOW_INVALID_DATES"))
           || (sql_mode_string.contains("EMPTY_STRING_IS_NULL"))
           || (sql_mode_string.contains("ERROR_FOR_DIVISION_BY_ZERO"))
           || (sql_mode_string.contains("HIGH_NOT_PRECEDENCE"))
           || (sql_mode_string.contains("IGNORE_BAD_TABLE_OPTIONS"))
           || (sql_mode_string.contains("IGNORE_SPACE"))
           || (sql_mode_string.contains("MYSQL323"))
           || (sql_mode_string.contains("MYSQL40"))
           || (sql_mode_string.contains("NO_AUTO_CREATE_USER"))
           || (sql_mode_string.contains("NO_AUTO_VALUE_ON_ZERO"))
           || (sql_mode_string.contains("NO_BACKSLASH_ESCAPES"))
           || (sql_mode_string.contains("NO_DIR_IN_CREATE"))
           || (sql_mode_string.contains("NO_ENGINE_SUBSTITUTION"))
           || (sql_mode_string.contains("NO_FIELD_OPTIONS"))
           || (sql_mode_string.contains("NO_KEY_OPTIONS"))
           || (sql_mode_string.contains("NO_TABLE_OPTIONS"))
           || (sql_mode_string.contains("NO_UNSIGNED_SUBTRACTION"))
           || (sql_mode_string.contains("NO_ZERO_DATE"))
           || (sql_mode_string.contains("NO_ZERO_IN_DATE"))
           || (sql_mode_string.contains("ONLY_FULL_GROUP_BY"))
           || (sql_mode_string.contains("PAD_CHAR_TO_FULL_LENGTH"))
           || (sql_mode_string.contains("PIPES_AS_CONCAT"))
           || (sql_mode_string.contains("REAL_AS_FLOAT"))
           || (sql_mode_string.contains("SIMULTANEOUS_ASSIGNMENT"))
           || (sql_mode_string.contains("STRICT_ALL_TABLES"))
           || (sql_mode_string.contains("STRICT_TRANS_TABLES"))
           || (sql_mode_string.contains("TIME_TRUNCATE_FRACTIONAL"))
           || (sql_mode_string.contains("TRADITIONAL")))
          {;}
        else return false; /* This has no effect */
      }
      if (is_in_hparse == false)  sql_mode_ansi_quotes= false;
      hparse_sql_mode_ansi_quotes= false;
    }
    if ((sql_mode_string.contains("ORACLE", Qt::CaseInsensitive) == true)
     && ((hparse_dbms_mask & FLAG_VERSION_MARIADB_10_3) != 0))
    {
      if (is_in_hparse == false) dbms_version_mask |= FLAG_VERSION_PLSQL;
      hparse_dbms_mask |= FLAG_VERSION_PLSQL;
    }
    else
    {
      if (is_in_hparse == false) dbms_version_mask &= (~FLAG_VERSION_PLSQL);
      hparse_dbms_mask &= (~FLAG_VERSION_PLSQL);
    }
  }
  if (hparse_sql_mode_ansi_quotes == old_hparse_sql_mode_ansi_quotes) return false;
  else return true;
}

/* 20160915: moved hparse_* and tparse_* routines to a new file. */
#include "hparse.h"

#ifdef DBMS_TARANTOOL

/*
  Connect to tarantool server due to --ocelot_dbms='tarantool'
  or "create server id options (port=x);".
  Todo: disconnect old if already connected.
  TODO: LOTS OF ERROR CHECKS NEEDED IN THIS!
  Usually libtarantool.so and libtarantoolnet.so are in /usr/local/lib or LD_LIBRARY_PATH.
  todo: We no longer use libtarantoolnet.so, remove reference to it.
  After connecting:
    -- get session.id and version
    -- todo: if version is old we shouldn't try to use SQL
  ansi_quotes:
    With Tarantool/SQL "x" is a delimited identifier not a string
    literal. So we set hparse_sql_mode_ansi_quotes= true.
    If we mix Tarantool/SQL and MySQL/MariaDB, this is still required.
    So we guess that users are sane. I hope this becomes policy someday.
    With Tarantool/NoSQL "x" is always a string literal.
*/
int MainWindow::connect_tarantool(unsigned int connection_number,
                                  QString port_maybe,
                                  QString host_maybe,
                                  QString password_maybe,
                                  QString user_maybe)
{
  //(void) connection_number; /* suppress "unused parameter" warning */
  QString ldbms_return_string;

  ldbms_return_string= "";

  /* Find libtarantool. Prefer ld_run_path. */
  if (is_libtarantool_loaded != 1)
  {
    lmysql->ldbms_get_library(ocelot_ld_run_path, &is_libtarantool_loaded, &libtarantool_handle, &ldbms_return_string, WHICH_LIBRARY_LIBTARANTOOL);
  }
  if (is_libtarantool_loaded != 1)
  {
    lmysql->ldbms_get_library("", &is_libtarantool_loaded, &libtarantool_handle, &ldbms_return_string, WHICH_LIBRARY_LIBTARANTOOL);
  }

  /* Find libtarantoolnet. Prefer ld_run_path. */
  /* Now libtarantool.so has everything so this is removed. */
  //if (is_libtarantoolnet_loaded != 1)
  //{
  //  lmysql->ldbms_get_library(ocelot_ld_run_path, &is_libtarantoolnet_loaded, &libtarantoolnet_handle, &ldbms_return_string, WHICH_LIBRARY_LIBTARANTOOLNET);
  //}
  //if (is_libtarantoolnet_loaded != 1)
  //{
  //  lmysql->ldbms_get_library("", &is_libtarantoolnet_loaded, &libtarantoolnet_handle, &ldbms_return_string, WHICH_LIBRARY_LIBTARANTOOLNET);
  //}

  /* Todo: The following errors would be better if we put them in diagnostics the usual way. */

  if (is_libtarantool_loaded == -2)
  {
    /* Todo: QMessageBox should have a parent, use "= new" */
    QMessageBox msgbox;
    QString error_message;
    error_message= "Severe error: libtarantool does not have these names: ";
    error_message.append(ldbms_return_string);
    error_message.append(". Close ocelotgui, restart with a better libtarantool.so.");
    msgbox.setText(error_message);
    msgbox.exec();
    delete lmysql;
    return 1;
  }

  if (is_libtarantool_loaded == 0)
  {
    /* Todo: QMessageBox should have a parent, use "= new" */
    QMessageBox msgbox;
    QString error_message;
    error_message= "Error, libtarantool was not found or a loading error occurred. Message was: ";
    error_message.append(ldbms_return_string);
    msgbox.setText(error_message);
    msgbox.exec();
    delete lmysql;
    return 1;
  }

  /* Todo: this should have been done already; we must be calling from the wrong place. */
  copy_connect_strings_to_utf8();

  /* CONNECT. URI = port, host:port, or username:password@host:port */

  /* tnt[] is static global */
  tnt[connection_number]= lmysql->ldbms_tnt_net(NULL);

  if (ocelot_opt_connect_timeout > 0)
  {
    struct timeval tvp;
    tvp.tv_sec= ocelot_opt_connect_timeout;
    tvp.tv_usec= 0;
    lmysql->ldbms_tnt_set(tnt[connection_number], (int)TNT_OPT_TMOUT_CONNECT, (char*)&tvp);
  }
  if (ocelot_net_buffer_length != 16384)
  {
    /* in ocelotgui.h I said third arg is char*, so casting is goofy */
    /* warning: 16384 is MySQL's default, maybe not Tarantool's */
    /* warning: ocelot_net_buffer_length is long */
    lmysql->ldbms_tnt_set(tnt[connection_number], TNT_OPT_SEND_BUF, (char*)ocelot_net_buffer_length);
    lmysql->ldbms_tnt_set(tnt[connection_number], TNT_OPT_RECV_BUF, (char*)ocelot_net_buffer_length);
  }
  {
    char connection_string[1024];
    char connection_port[128];
    char connection_host[128];
    char connection_password[128];
    char connection_user[128];
    if (port_maybe != "DEFAULT") strcpy(connection_port, port_maybe.toUtf8());
    else sprintf(connection_port, "%d", ocelot_port);
    if (host_maybe != "DEFAULT") strcpy(connection_host, host_maybe.toUtf8());
    else strcpy(connection_host,ocelot_host_as_utf8);
    if (password_maybe != "DEFAULT") strcpy(connection_password, password_maybe.toUtf8());
    else strcpy(connection_password,ocelot_password_as_utf8);
    if (user_maybe != "DEFAULT") strcpy(connection_user, user_maybe.toUtf8());
    else strcpy(connection_user, ocelot_user_as_utf8);
    strcpy(connection_string, "");
    /* guest has no password
       after connect() succeeds user = guest,
       after authenticate() succeeds user = arg
    */
    if (strcmp(connection_user, "guest") != 0)
    {
      strcat(connection_string, connection_user);
      strcat(connection_string, ":");
      strcat(connection_string, connection_password);
      strcat(connection_string, "@");
    }
    strcat(connection_string, connection_host);
    strcat(connection_string, ":");
    strcat(connection_string, connection_port);
    lmysql->ldbms_tnt_set(tnt[connection_number], (int)TNT_OPT_URI, connection_string);
    if (lmysql->ldbms_tnt_connect(tnt[connection_number]) < 0) {
      tarantool_errno[connection_number]= 9999;
      strcpy(tarantool_errmsg, "Connection refused for ");
      strcat(tarantool_errmsg, connection_string);
    }
    else
    {
      tarantool_errno[connection_number]= 0;
      strcpy(tarantool_errmsg, er_strings[er_off + ER_OK]);
    }
  }

  if (tarantool_errno[connection_number] != 0)
  {
    /* Kludge so put_diagnostics_in_result won't crash */
    connections_dbms[connection_number]= DBMS_TARANTOOL;
    put_diagnostics_in_result(connection_number);
    if (connection_number == MYSQL_MAIN_CONNECTION)
    {
      make_and_append_message_in_result(ER_FAILED_TO_CONNECT_TO_TARANTOOL, 0, (char*)"");
    }
    else
    {
      make_and_append_message_in_result(ER_FAILED_TO_CONNECT_TO_TARANTOOL_FOR_SERVER, 0, (char*)"");
    }
    return 1;
  }
  make_and_put_message_in_result(ER_OK, 0, (char*)"");
  connections_is_connected[connection_number]= 1;

  QString session_id, version;
  {
    char query_string[1024];
    sprintf(query_string, "return box.session.id()");
    session_id= tarantool_internal_query(query_string, connection_number);
    sprintf(query_string, "return box.info.version");
    version= tarantool_internal_query(query_string, connection_number);
    int index_of_hyphen= version.indexOf("-");
    if (index_of_hyphen > 0) version= version.left(index_of_hyphen);
  }

  if (connection_number == MYSQL_REMOTE_CONNECTION)
  {
    /* The caller should save the connection for the server id. */
    return 0;
  }

  sql_mode_ansi_quotes= true;        /* see comment = ansi_quotes */
  hparse_sql_mode_ansi_quotes= true; /* probably not necessary */

  /*
    Todo: This overrides any earlier PROMPT statements by user.
    Probably what we want is a flag: "if user did PROMPT, don't override it."
    Or we want PROMPT statement to change ocelot_prompt.
  */
  statement_edit_widget->prompt_default= ocelot_prompt;
  statement_edit_widget->prompt_as_input_by_user= statement_edit_widget->prompt_default;

  statement_edit_widget->dbms_version= version;
  statement_edit_widget->dbms_database= ocelot_database;
  statement_edit_widget->dbms_port= QString::number(ocelot_port);
  statement_edit_widget->dbms_current_user= ocelot_user;
  statement_edit_widget->dbms_current_user_without_host= ocelot_user;
  statement_edit_widget->dbms_connection_id= session_id.toInt();
  statement_edit_widget->dbms_host= ocelot_host;
  //connections_is_connected[connection_number]= 1;
  tarantool_initialize(connection_number);
  return 0;
}
#endif

#ifdef DBMS_TARANTOOL
/*
  For some internal (not user-generated) requests
  returns tarantool_errno[connection_number]
  returns tarantool_errmsg[connection_number]
  if first word is "return" and no error, return a scalar
  the scalar must be MP_STR or MP_UINT or MP_INT, otherwise return = ""
*/
QString MainWindow::tarantool_internal_query(char *query,
                                            int connection_number)
{
  QString returned_string;
  struct tnt_stream *empty= lmysql->ldbms_tnt_object(NULL);
  lmysql->ldbms_tnt_object_add_array(empty, 0);
  lmysql->ldbms_tnt_eval(tnt[connection_number], query, strlen(query), empty);
  tarantool_flush_and_save_reply(connection_number);
  if ((tarantool_errno[connection_number] == 0)
   && (strncmp(query, "return ", 7) == 0))
  {
    uint32_t value_length;
    const char *value;
    char value_as_string[320]; /* must be big enough for any sprintf() result */
    char field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply.data);
    /* if field_type != MP_ARRAY that's an error but we just return "" */
    if (field_type == MP_ARRAY)
    {
      /* The first item will be dd 00 00 01 i.e. "array of length = 1" */
      long unsigned int result;
      result= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply.data);
      if (result == 1)
        field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply.data);
    }
    if (field_type == MP_STR)
    {
      value= lmysql->ldbms_mp_decode_str(&tarantool_tnt_reply.data, &value_length);
      returned_string= QString::fromUtf8(value, value_length);
    }
    else if (field_type == MP_UINT)
    {
      uint64_t uint_value= lmysql->ldbms_mp_decode_uint(&tarantool_tnt_reply.data);
      long long unsigned int llu= uint_value;
      value_length= sprintf(value_as_string, "%llu", llu);
      returned_string= QString::fromUtf8(value_as_string);
    }
    else if (field_type == MP_INT)
    {
      int64_t int_value= lmysql->ldbms_mp_decode_int(&tarantool_tnt_reply.data);
      long long int lli= int_value;
      value_length= sprintf(value_as_string, "%lld", lli);
      returned_string= QString::fromUtf8(value_as_string);
    }
  }
  else returned_string= "";
  lmysql->ldbms_tnt_reply_free(&tarantool_tnt_reply);
  return returned_string;
}
#endif

#ifdef DBMS_TARANTOOL
/*
  Call tarantool_initialize() from connect_tarantool() if successful.
  The main task is to create a function that wraps box.sql.execute()
  so we can get column names -- a temporary thing that will have to
  be changed when Tarantool itself has a good way to do such a return.
  We look for 0xa1 0x08 in the return, as a signal "column list here".
  Todo: check tarantool_errno[connection_number=0 and tarantool_errmsg
  Todo: version check, unless that's somewhere else already.
*/
void MainWindow::tarantool_initialize(int connection_number)
{
const char *my_string= "function ocelot_sqle(p)"
                 "  local n = {}"
                 "    local x, y = pcall(function() n = box.sql.execute(p) end)"
                 "    if (x) then"
                 "      table.insert(n[0], 1, string.char(8))"
                 "      table.insert(n, 1, n[0])"
                 "      return n"
                 "    end"
                 "    box.error{code = 555, reason = y}"
                 "  end";
  QString query_return= tarantool_internal_query((char*)my_string, connection_number);
}
#endif

#ifdef DBMS_TARANTOOL
/*
  tnt_flush() sends a request to the server.
  after every tnt_flush, save reply.
  TODO: KLUDGE: (I wonder many times have I used that word.)
        When I entered [[return box.schema.space.create('T8');]] I got
        error = unsupported Lua type 'function' but the space was
        created. There are github.com/tarantool issues that mention
        this message. I don't understand them. So I'll cancel the error.
*/

void MainWindow::tarantool_flush_and_save_reply(unsigned int connection_number)
{
  lmysql->ldbms_tnt_flush(tnt[connection_number]);

  lmysql->ldbms_tnt_reply_init(&tarantool_tnt_reply);
  int read_result= tnt[connection_number]->read_reply(tnt[connection_number], &tarantool_tnt_reply);
  if (read_result != 0)
  {
    tarantool_errno[connection_number]= 1;
    sprintf(tarantool_errmsg, "read_result=%d", read_result);
    return;
  }
  tarantool_errno[connection_number]= tarantool_tnt_reply.code;

  if (tarantool_tnt_reply.code != 0)
  {
    char *x1= (char*)tarantool_tnt_reply.error;
    char *x2= (char*)tarantool_errmsg;
    while (x1 < tarantool_tnt_reply.error_end) *(x2++)=*(x1++);
    *x2= '\0';
  }
  else strcpy(tarantool_errmsg, er_strings[er_off + ER_OK]);
  if (strcmp(tarantool_errmsg, "unsupported Lua type 'function'") == 0)
  {
    tarantool_errno[connection_number]= 0;
    strcpy(tarantool_errmsg, er_strings[er_off + ER_OK]);
  }
}

/*
  Return statement type = first keyword = TOKEN_KEYWORD_SELECT etc.
  Probably hparse has already found the facts in a better way but
  perhaps you haven't called hparse.
  Currently this is only being used to guess whether it's Lua.
*/
/*
  DESPERATION -- for tarantool_real_query() ...
  We are passing dbms_query + dbms_query_len, which might not be the
  same as a subset of main_token globals. We just need to know what
  are the first two tokens, we're looking for BEGIN, COMMIT, ROLLBACK
  but not ROLLBACK TO, LUA '...'. We have to skip comments, and we
  assume that there aren't more than 98 comments at statement start.
  If first word is not an SQL statement-start word, or LUA.
  turn on the Lua flag.
  ALSO: make_statement_ready_to_send() happened recently, and we could
        have used it to know the first words, would have been faster
  ALSO: hparse_f_multi_block() turned on TOKEN_FLAG_IS_LUA, and we
        could have used it to know if it's Lua, would have been faster
  TODO: return error if too many initial comments, or retry with more
  TODO: if we get WITH, we'd like to know if it's SELECT|INSERT|UPDATE
        (which requires going ahead many more words)
  Warn: only works for Tarantool at the moment, e.g.
        wouldn't recognize MySQL/MariaDB comments starting with #
*/
QString MainWindow::get_statement_type(QString q_dbms_query, int *statement_type)
{
  int token_offsets[100]; /* Surely a single assignable target can't have more */
  int token_lengths[100];
  tokenize(q_dbms_query.data(),
           q_dbms_query.size(),
           &token_lengths[0], &token_offsets[0], 100 - 1,
          (QChar*)"33333", 2, "", 1);
  int word_number= 0;
  QString word0= "", word1= "", word2= "";
  for (int i= 0; i < 100; ++i)
  {
    if (token_lengths[i] == 0) break;
    if ((q_dbms_query.mid(token_offsets[i], 2) == "/*")
     || (q_dbms_query.mid(token_offsets[i], 2) == "--"))
      continue;
    if (word_number == 0)
    {
      word0= q_dbms_query.mid(token_offsets[i], token_lengths[i]);
    }
    if (word_number == 1)
    {
      word1= q_dbms_query.mid(token_offsets[i], token_lengths[i]);
    }
    if (word_number == 2)
    {
      word2= q_dbms_query.mid(token_offsets[i], token_lengths[i]);
      break;
    }
    ++word_number;
  }
  *statement_type= get_statement_type_low(word0, word1, word2);
  return word1;
}

/* TODO: Get fussier about what you'll accept is (Tarantool) SQL. */
int MainWindow::get_statement_type_low(QString word0, QString word1, QString word2)
{
  word0= word0.toUpper();

  int statement_type= TOKEN_KEYWORD_DO_LUA;
  if (word0 == "ALTER")
  {
    if (QString::compare(word1, "TABLE", Qt::CaseInsensitive) == 0) statement_type= TOKEN_KEYWORD_ALTER;
  }
  else if (word0 == "ANALYZE")
  {
    if ((word1 == ";") || (word1 == "")) statement_type= TOKEN_KEYWORD_ANALYZE;
    if (word1 > "")
    {
      QChar w1c1= word1.at(0);
      if ((w1c1.isLetter() == true)
       || (w1c1 == '\"'))
        statement_type= TOKEN_KEYWORD_ANALYZE;
    }
  }
  else if (word0 == "COMMIT")
  {
    if ((word1 == ";") || (word1 == "")) statement_type= TOKEN_KEYWORD_COMMIT;
  }
  else if (word0 == "CREATE")
  {
    if ((QString::compare(word1, "UNIQUE", Qt::CaseInsensitive) == 0)
     || (QString::compare(word1, "INDEX", Qt::CaseInsensitive) == 0)
     || (QString::compare(word1, "TABLE", Qt::CaseInsensitive) == 0)
     || (QString::compare(word1, "TRIGGER", Qt::CaseInsensitive) == 0)
     || (QString::compare(word1, "VIEW", Qt::CaseInsensitive) == 0))
      statement_type= TOKEN_KEYWORD_CREATE;
  }
  else if (word0 == "DELETE")
  {
    if (QString::compare(word1, "FROM", Qt::CaseInsensitive) == 0) statement_type= TOKEN_KEYWORD_DELETE;
  }
  else if (word0 == "DROP")
  {
    if ((QString::compare(word1, "INDEX", Qt::CaseInsensitive) == 0)
     || (QString::compare(word1, "TABLE", Qt::CaseInsensitive) == 0)
     || (QString::compare(word1, "TRIGGER", Qt::CaseInsensitive) == 0)
     || (QString::compare(word1, "VIEW", Qt::CaseInsensitive) == 0))
      statement_type= TOKEN_KEYWORD_DROP;
  }
  else if (word0 == "EXPLAIN") { statement_type= TOKEN_KEYWORD_EXPLAIN; }
  else if (word0 == "INSERT")
  {
    if ((QString::compare(word1, "INTO", Qt::CaseInsensitive) == 0)
     || (QString::compare(word1, "OR", Qt::CaseInsensitive) == 0))
      statement_type= TOKEN_KEYWORD_INSERT;
  }
  else if (word0 == "LUA") { statement_type= TOKEN_KEYWORD_LUA; }
  else if (word0 == "PRAGMA") { statement_type= TOKEN_KEYWORD_PRAGMA; }
  else if (word0 == "RELEASE")
  {
    if (QString::compare(word1, "SAVEPOINT", Qt::CaseInsensitive) == 0) statement_type= TOKEN_KEYWORD_RELEASE;
  }
  else if (word0 == "REPLACE")
  {
    if (QString::compare(word1, "INTO", Qt::CaseInsensitive) == 0) statement_type= TOKEN_KEYWORD_REPLACE;
  }
  else if (word0 == "ROLLBACK")
  {
    if (QString::compare(word1, "TO", Qt::CaseInsensitive) == 0) statement_type= TOKEN_KEYWORD_ROLLBACK_IN_ROLLBACK_TO;
    else
    {
      if ((word1 == ";") || (word1 == "")) statement_type= TOKEN_KEYWORD_ROLLBACK;
    }
  }
  else if (word0 == "SAVEPOINT")
  {
    if (word1 > "")
    {
      QChar w1c1= word1.at(0);
      if ((w1c1.isLetter() == true)
       || (w1c1 == '\"'))
        statement_type= TOKEN_KEYWORD_SAVEPOINT;
    }
  }
  else if (word0 == "SELECT") { statement_type= TOKEN_KEYWORD_SELECT; }
  else if (word0 == "START")
  {
    if (QString::compare(word1, "TRANSACTION", Qt::CaseInsensitive) == 0)
      statement_type= TOKEN_KEYWORD_START;
  }
  else if (word0 == "UPDATE")
  {
    if (word1 > "")
    {
      QChar w1c1= word1.at(0);
      if ((w1c1.isLetter() == true)
       || (w1c1 == '\"'))
        statement_type= TOKEN_KEYWORD_UPDATE;
    }
  }
  else if (word0 == "VALUES")
  {
    if (word1 == "(") statement_type= TOKEN_KEYWORD_VALUES;
  }
  else if (word0 == "WITH")
  {
    if (QString::compare(word2, "AS", Qt::CaseInsensitive) == 0) statement_type= TOKEN_KEYWORD_WITH;
  }
  else { statement_type= TOKEN_KEYWORD_DO_LUA; }
  return statement_type;
}

/* An equivalent to mysql_real_query(). NB: this might be called from a non-main thread */
/*
   Todo: we shouldn't be calling tparse_f_program() yet again!
   Todo: I succeeded in making this work
           lua 'return box.space.t:select()';
         but:
           should use [[...]] or escapes
           should not depend later on looking at TOKEN_KEYWORD_LUA
   Because Tarantool-style transactions require us to send all requests
   as a single send, we have to combine them -- which is sad because we
   spent time earlier in splitting them apart. We use a QStringList.
*/
int MainWindow::tarantool_real_query(const char *dbms_query,
                                     unsigned long dbms_query_len,
                                     unsigned int connection_number,
                                     unsigned int passed_main_token_number,
                                     unsigned int passed_main_token_count_in_statement)
{
  log("tarantool_real_query start", 80);
  tarantool_errno[connection_number]= 10001;
  strcpy(tarantool_errmsg, "Unknown Tarantool Error");

  QString q_dbms_query= QString::fromUtf8(dbms_query, dbms_query_len);
  int statement_type;
  QString word1= get_statement_type(q_dbms_query, &statement_type);
  int token_type= -1;

  //int result_row_count= 0; /* for everything except SELECT we ignore rows that are returned */

  {
    /* If it is LUA 'x'; we only want to pass the 'x' */
    if (statement_type == TOKEN_KEYWORD_LUA) q_dbms_query= word1;

    int statement_number= tarantool_statements_in_begin.size();

    if (statement_number > 0) q_dbms_query= q_dbms_query.append(" ");
    q_dbms_query= tarantool_sql_to_lua(q_dbms_query, statement_type, statement_number);

    tarantool_statements_in_begin.append(q_dbms_query);
  }
  if (statement_type == TOKEN_KEYWORD_BEGIN_WORK)
  {
    tarantool_begin_seen= true;
  }
  if ((statement_type == TOKEN_KEYWORD_COMMIT)
   || (statement_type == TOKEN_KEYWORD_ROLLBACK))
  {
    tarantool_begin_seen= false;
  }
  if (tarantool_begin_seen == true)
  {
    strcpy(tarantool_errmsg, "Deferred till commit|rollback");
    tarantool_errno[connection_number]= 8372;
    return tarantool_errno[connection_number];
  }
  if (hparse_f_is_nosql(q_dbms_query) == false)
  {
    QString s= "";
    QString s2;
    int statement_return_count= 0;

    for (int i= 0; i < tarantool_statements_in_begin.size(); ++i)
    {
      s2= tarantool_statements_in_begin.at(i);
      if (s2.mid(0, 13) == "ocelot_return") ++statement_return_count;
      s.append(s2);
    }

    tarantool_execute_sql(s, connection_number, statement_return_count);
    while (tarantool_statements_in_begin.isEmpty() == false)
      tarantool_statements_in_begin.removeAt(0);
    log("tarantool_real_query end", 80);
    return tarantool_errno[connection_number];
  }

  /* If something is NoSQL, transactions don't work. */
  /* This section is rarely tested, I'm thinking of abandoning it. */
  /* It might be saved if I could push + pop main_tokens. */

  strcpy(tarantool_errmsg, "/* NOSQL */ is temporarily disabled");
  tarantool_errno[connection_number]= 8373;
  return tarantool_errno[connection_number];

  tarantool_select_nosql= true;
  //int hparse_statement_type=-1, clause_type=-1;
  //QString current_token, what_we_expect, what_we_got;

  //tparse_f_program(text); /* syntax check; get offset_of_identifier,statement_type, number_of_literals */
  if (hparse_errno > 0)
  {
    strcpy(tarantool_errmsg, hparse_errmsg);
    tarantool_errno[connection_number]= hparse_errno;
    return tarantool_errno[connection_number];
  }

  /* The number of literals in a statement must be what we'd insert etc. */
  int number_of_literals= 0;
  for (unsigned int i= passed_main_token_number; i < passed_main_token_number + passed_main_token_count_in_statement; ++i)
  {
    if (main_token_types[i] == TOKEN_TYPE_LITERAL)
    {
      ++number_of_literals;
    }
  }

  /* The first identifier in any of the statements must be the space name. */
  /* TODO: UNLESS hparse_statement_type == TOKEN_KEYWORD_SET! */
  /* TODO: This is another reason for having a table of reftypes, eh? */
  int offset_of_space_name= -1;
  for (unsigned int i= passed_main_token_number; i < passed_main_token_number + passed_main_token_count_in_statement; ++i)
  {
    if ((main_token_types[i] == TOKEN_TYPE_IDENTIFIER)
     && (main_token_reftypes[i] == TOKEN_REFTYPE_TABLE))
    {
      offset_of_space_name= (int) i;
      break;
    }
  }

  QString text= q_dbms_query; /* REALLY DUBIOUS */

  int spaceno= -1;
  {
    int i= offset_of_space_name;
    QString space_name= text.mid(main_token_offsets[i], main_token_lengths[i]);
    spaceno= lmysql->ldbms_tnt_get_spaceno(tnt[connection_number], space_name.toUtf8(), main_token_lengths[i]);
    if (spaceno < 0)
    {
      /* Todo: figure out when this really needs to be called */
      lmysql->ldbms_tnt_reload_schema(tnt[connection_number]);
      spaceno= lmysql->ldbms_tnt_get_spaceno(tnt[connection_number], space_name.toUtf8(), main_token_lengths[i]);
      if (spaceno < 0)
      {
        QString s;
        s= "Could not find a space named ";
        s.append(space_name);
        tarantool_errno[connection_number]= 10003;
        strcpy(tarantool_errmsg, s.toUtf8());
        return tarantool_errno[connection_number];
      }
    }
  }

  /*
    The iterator type, if there is one, is the first comp-op
    in the statement. If there are more comp-ops, we've already
    checked that they're valid, during tparse_f_indexed_condition().
  */
  int iterator_type= TARANTOOL_BOX_INDEX_EQ;;
  for (unsigned int i= passed_main_token_number; i < passed_main_token_number + passed_main_token_count_in_statement; ++i)
  {
    if (main_token_types[i] == TOKEN_TYPE_OPERATOR)
    {
      QString token= text.mid(main_token_offsets[i], main_token_lengths[i]);
      if (token == "=") { iterator_type= TARANTOOL_BOX_INDEX_EQ; break; }
      if (token == "<") { iterator_type= TARANTOOL_BOX_INDEX_LT; break; }
      if (token == "<=") {iterator_type= TARANTOOL_BOX_INDEX_LE; break; }
      if (token == ">") { iterator_type= TARANTOOL_BOX_INDEX_GT; break; }
      if (token == ">=") { iterator_type= TARANTOOL_BOX_INDEX_GE; break; }
    }
  }

  /* DELETE + INSERT + REPLACE + (maybe?) SELECT require a tuple of values to insert or search */
  struct tnt_stream *tuple= NULL;
  //if (number_of_literals > 0)
  {
    tuple= lmysql->ldbms_tnt_object(NULL);
    lmysql->ldbms_tnt_object_add_array(tuple, number_of_literals);
    int number_of_adds= 0;
    for (unsigned int i= passed_main_token_number; i < passed_main_token_number + passed_main_token_count_in_statement; ++i)
    {
      token_type= main_token_types[i];
      if (token_type == TOKEN_TYPE_LITERAL)
      {
        /* Todo: figure out literal's exact type, and be sure. */
        assert(main_token_lengths[i] > 0);
        QString first_char= text.mid(main_token_offsets[i], 1);
        if (first_char == "'") token_type= TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE;
        else if (first_char < "#") token_type= TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE;
        else token_type= TOKEN_TYPE_LITERAL_WITH_DIGIT;
      }
      if ((token_type == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
       || (token_type == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE))
      {
        QString s;
        s= text.mid(main_token_offsets[i], main_token_lengths[i]);
        s= s.left(s.length() - 1);
        s= s.right(s.length() - 1);
        lmysql->ldbms_tnt_object_add_str(tuple, s.toUtf8(), s.length());
        ++number_of_adds;
      }
      if (token_type == TOKEN_TYPE_LITERAL_WITH_DIGIT)
      {
        /* todo: binary, float, double */
        QString s;
        s= text.mid(main_token_offsets[i], main_token_lengths[i]);
        lmysql->ldbms_tnt_object_add_int(tuple, s.toInt());
        ++number_of_adds;
      }
      if (token_type == TOKEN_KEYWORD_NULL)
      {
        lmysql->ldbms_tnt_object_add_nil(tuple);
        ++number_of_adds;
      }
    }
    assert(number_of_literals == number_of_adds);
    /* Todo: check whether we really need to say object_container_close. */
    lmysql->ldbms_tnt_object_container_close(tuple);
  }

  result_row_count= 0; /* for everything except SELECT we ignore rows that are returned */

  if (statement_type == TOKEN_KEYWORD_DELETE)
  {
    lmysql->ldbms_tnt_delete(tnt[connection_number], spaceno, 0, tuple);
    tarantool_flush_and_save_reply(connection_number);
    lmysql->ldbms_tnt_reply_free(&tarantool_tnt_reply);
    return tarantool_errno[connection_number];
  }

  if (statement_type == TOKEN_KEYWORD_INSERT)
  {
    if (lmysql->ldbms_tnt_insert(tnt[connection_number], spaceno, tuple) < 0)
    {
      tarantool_errno[connection_number]= 10007;
      strcpy(tarantool_errmsg, "Bug. tnt_insert() returned an error");
      return tarantool_errno[connection_number];
    }
    tarantool_flush_and_save_reply(connection_number);
    lmysql->ldbms_tnt_reply_free(&tarantool_tnt_reply);
    return tarantool_errno[connection_number];
  }

  if (statement_type == TOKEN_KEYWORD_REPLACE)
  {
    if (lmysql->ldbms_tnt_replace(tnt[connection_number], spaceno, tuple) < 0)
    {
      tarantool_errno[connection_number]= 10007;
      strcpy(tarantool_errmsg, "Bug. tnt_replace() returned an error");
      return tarantool_errno[connection_number];
    }
    tarantool_flush_and_save_reply(connection_number);
    lmysql->ldbms_tnt_reply_free(&tarantool_tnt_reply);
    return tarantool_errno[connection_number];
  }

  if (statement_type == TOKEN_KEYWORD_SELECT)
  {
    lmysql->ldbms_tnt_select(tnt[connection_number], spaceno, 0, (2^32) - 1, 0, iterator_type, tuple);
    tarantool_flush_and_save_reply(connection_number);
    if (tarantool_errno[connection_number] != 0) return tarantool_errno[connection_number];
    /* The return should be an array of arrays of scalars. */
    /* If there are no rows, then there are no fields, so we cannot put up a grid. */
    /* Todo: don't forget to free if there are zero rows. */
    {
      const char *tarantool_tnt_reply_data_copy= tarantool_tnt_reply.data;
//      unsigned long r= tarantool_num_rows(connection_number);
      tarantool_tnt_reply.data= tarantool_tnt_reply_data_copy;
//      if (r == 0)
//      {
//        strcpy(tarantool_errmsg, "Zero rows.");
//        tarantool_errno[connection_number]= 10027;
//      }
    }

    return tarantool_errno[connection_number];
  }

  return tarantool_errno[connection_number];
}

/*
  We call this to get basic information about a Tarantool return;
  if it is a result set we get type + row count + return pointer to
  the first row.
  TODO: There might be multiple result sets, for example if there were
        multiple selects inside begin ... commit|rollback. We only look
        at the first. What we should be doing is: pass "selection number"
        and skip till we get to it.
  Todo: The calculation of data_length might be buggy.
  What is in tarantool_tnt_reply.data?
  LUA '"a"'         ... dd 0 0 0 1 a1 61
  LUA '15'          ... dd 0 0 0 1 f
  LUA 'm = 1'       ... dd 0 0 0 0
  LUA '...select()' ... dd 0 0 0 1 93 92  (93 is row count, 92 is field count)
  LUA '1,2,3'       ... dd 0 0 0 3 1  2  3
  SELECT is like LUA '...select()'
  SELECT / * NOSQL * /  dd 0 0 0 4 92 1 a5 68 65 (0 0 0 4 is row count)
  (dd is fixarray-32, 93 is fixarray)
  (the NOSQL option uses tarantool_tnt_select rather than eval)
  So return:
  0 == there is no result set because we don't understand ... error?
  1 == there is no result set because we start with fixarray-32 == 0
       or we get dd 0 0 0 1 c0 (nil)
  2 == array-32 == field count, then fields. so assume row_count == 1
  3 == array-32 == 1, array-x = row count, array-x == field count
  4 == array-32 == row count, then array-x == field count
  5 == same as 3, but first row is 0xa1 0x08 then column names.
*/
const char *MainWindow::tarantool_result_set_init(
        int connection_number,
        long unsigned int *result_row_count,
        int *result_set_type)
{
  log("tarantool_result_set_init start", 80);
  const char *tarantool_tnt_reply_data= tarantool_tnt_reply.data;
  char field_type;
  long unsigned int r= 0;
  int data_length;

  ///* TEST!! start (todo: maybe we should log with low priority) */
  //int mm= tarantool_tnt_reply.data_end - tarantool_tnt_reply.data;
  //printf("  mm=%d\n", mm);
  //for (int i= 0; i < mm; ++i)
  //{
  //  printf("  %x\n", *(tarantool_tnt_reply_data + i));
  //}
  //const char *tarantool_tnt_reply_error= tarantool_tnt_reply.error;
  //mm= tarantool_tnt_reply.error_end - tarantool_tnt_reply.error;
  //printf("    mm=%d\n", mm);
  //for (int i= 0; i < mm; ++i)
  //{
  //  printf("    %x\n", *(tarantool_tnt_reply_error + i));
  //}
  ///* TEST!! end */
  if ((tarantool_tnt_reply.data == NULL)
   || (tarantool_tnt_reply.data_end == NULL))
    goto erret;
  field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data);
  if (field_type != MP_ARRAY) goto erret;
  if (tarantool_select_nosql == true)
  {
    *result_set_type= 4; goto return_point;
  }
  r= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);

  if (r > 0) /* Check if it's nil or a series of nothing but nils */
  {
    long unsigned int q;
    for (q= 0; q < r; ++q)
    {
      if ((unsigned char)*(tarantool_tnt_reply_data + q) != 0xc0) break;
    }
    if (q == r)
    {
      *result_set_type= 1;
      goto return_point;
    }
  }

  /* TEST: ANOTHER EL GRANDE KLUDGE */
  if ((r > 1) && (lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data) == MP_NIL))
  {
    const char *tarantool_tnt_reply_data2= tarantool_tnt_reply_data;
    long r2;
    for (r2= r; r2 != 0; --r2)
    {
      if (lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data2) == MP_NIL)
      {
        ++tarantool_tnt_reply_data2;
        continue;
      }
      break;
    }
    if (r2 == 0)
    {
    }
    else if (lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data2) == MP_ARRAY)
    {
      r= 1;
      tarantool_tnt_reply_data= tarantool_tnt_reply_data2;
    }
  }

  if (r == 0)
  {
    *result_set_type= 1; goto return_point;
  }
  if (r != 1)
  {
    tarantool_tnt_reply_data= tarantool_tnt_reply.data;
    *result_set_type= 2; goto return_point;
  }
  field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data);

  if (field_type != MP_ARRAY)
  {
    tarantool_tnt_reply_data= tarantool_tnt_reply.data;
    *result_set_type= 2; goto return_point;
  }
  r= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);
  field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data);
  if (field_type != MP_ARRAY)
  {
    tarantool_tnt_reply_data= tarantool_tnt_reply.data;
    *result_set_type= 2; goto return_point;
  }
  {
    /* Look for the signature of the ocelot_sqle function */
    data_length= tarantool_tnt_reply.data_end - tarantool_tnt_reply.data;
    if (data_length > 20) data_length= 20;
    for (int ik= 0; ik < data_length - 1; ++ik)
    {
      if (((unsigned char)*(tarantool_tnt_reply_data + ik) == 0xa1)
       && (*(tarantool_tnt_reply_data + ik + 1) == 0x08))
      {
        *result_set_type= 5; goto return_point;
      }
    }
  }
  *result_set_type= 3; goto return_point;

return_point:
  if (*result_set_type == 0) *result_row_count= 0;
  else if (*result_set_type == 2) *result_row_count= 1;
//  if (*result_set_type != 4)
//  {
//    lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);
//  }
//  long unsigned int r;
  else
  {
//  r= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);
    if (*result_set_type == 5) --r;
    *result_row_count= r;
  }
  log("tarantool_result_set_init end 1", 80);
  return tarantool_tnt_reply_data;
erret:
  tarantool_errno[connection_number]= 10008;
  strcpy(tarantool_errmsg, "Error: did not understand received data. ");
  if (tarantool_tnt_reply.data == NULL)
    strcat(tarantool_errmsg, "tarantool_tnt_reply.data == NULL");
  else if (tarantool_tnt_reply.data_end == NULL)
    strcat(tarantool_errmsg, "tarantool_tnt_reply.data_end == NULL");
  else
  {
    int j= tarantool_tnt_reply.data_end - tarantool_tnt_reply.data;
    strcat(tarantool_errmsg, "First few bytes are: ");
    char tmp_hex[16];
    for (int i= 0; i < 10 && i < j; ++i)
    {
      sprintf(tmp_hex, "%x ", *(tarantool_tnt_reply.data + i) & 0xff);
      strcat(tarantool_errmsg, tmp_hex);
    }
    strcat(tarantool_errmsg, ".");
  }
  *result_row_count= 0;
  log("tarantool_result_set_init end 0", 80);
  return 0;
}

/*
  Given tarantool_tnt_reply, return number of rows from a SELECT.
  Used by result grid.
*/
long unsigned int MainWindow::tarantool_num_rows(unsigned int connection_number)
{
  long unsigned int r;
  int result_set_type;
  tarantool_result_set_init(connection_number, &r, &result_set_type);
  return r;
}

/* Given a single SQL or Lua request, make something executable in Lua.
   If LUA 'xxx', strip the quotes.
   If SQL statement, put it in box.sql.execute().
   If Lua statement, return as is.
   Later we hope to "return ocelot_result"
  TODO:
  The [=[ ... ]=] trick will fail if the string contains [=[ or ]=].
  So to be really safe you should look for occurrences of [=[ in
  s_in and change the count of =s until you're different.
  TODO: Since we're inside [=[ ... ]=], do escapes still work?
  TODO: For token_KEYWORD_LUA, make sure we never get extra chars
        after the string literal. That would make execution fail.
*/
QString MainWindow::tarantool_sql_to_lua(
        QString s_in,
        int statement_type,
        int statement_number)
{
  QString s_out;
  if (statement_type == TOKEN_KEYWORD_DO_LUA)
  {
    s_out= s_in;
  }
  else if (statement_type == TOKEN_KEYWORD_LUA)
  {
    s_out= connect_stripper(s_in, false);
  }
  else
  {
    s_out= "ocelot_return";
    s_out.append(QString::number(statement_number));
    s_out.append(" = ");
    if ((statement_type == TOKEN_KEYWORD_SELECT)
     || (statement_type == TOKEN_KEYWORD_VALUES))
      s_out.append("ocelot_sqle");
    else
      s_out.append("box.sql.execute");
    s_out.append("([=[");
    s_out.append(s_in);
    s_out.append("]=])");
  }
  return s_out;
}

/*
  Warning: do not say "delete [] request_string;" early, there
  might be a hidden pointer to it.

  TODO: Remove the giant kludge here. Instead of skipping nulls till
        we can return the first thing that's not null, we should
        "return ocelot_return0,ocelot_return1,..." and if there are
        2 or more result sets, then handle them with tabs the same way
        we do for MySQL/MariaDB.
*/
int MainWindow::tarantool_execute_sql(
            QString dbms_query,
            unsigned int connection_number,
            int statement_return_count)
{
  QString dbms_query_plus_return= dbms_query;

  if (statement_return_count > 0)
  {
    dbms_query_plus_return.append(" return");
    for (int i= 0; i < statement_return_count; ++i)
    {
      if (i > 0) dbms_query_plus_return.append(",");
      dbms_query_plus_return.append(" ");
      dbms_query_plus_return.append("ocelot_return");
      dbms_query_plus_return.append(QString::number(i));
    }
  }

  int dbms_query_len= dbms_query_plus_return.toUtf8().size();

  char *request_string= new char[dbms_query_len + 1];

  strcpy(request_string, dbms_query_plus_return.toUtf8());
  *(request_string + dbms_query_len)= '\0';

  tarantool_select_nosql= false;

  {
    //    Create an empty MsgPack object.
    //    If s is passed as NULL, then the object is allocated. Otherwise, the allocated object is initialized.
    struct tnt_stream *arg;
    arg = lmysql->ldbms_tnt_object(NULL);
    //    Reset a stream object to the basic state.
    lmysql->ldbms_tnt_object_reset(arg);
    //    Append an array header to a stream object.
    //    The header’s size is in bytes.
    //    If TNT_SBO_SPARSE or TNT_SBO_PACKED is set as container type, then size is ignored.
    lmysql->ldbms_tnt_object_add_array(arg, 0);
    struct tnt_request *req2 = lmysql->ldbms_tnt_request_eval(NULL);

    int m= lmysql->ldbms_tnt_request_set_exprz(req2, request_string);
    assert(m >= 0);
    lmysql->ldbms_tnt_request_set_tuple(req2, arg);
    /* uint64_t sync1 = */ lmysql->ldbms_tnt_request_compile(tnt[connection_number], req2);
    tarantool_flush_and_save_reply(connection_number);
    if (tarantool_errno[connection_number] != 0)
    {
      return tarantool_errno[connection_number];
    }
    /* The return should be an array of arrays of scalars. */
    /* If there are no rows, then there are no fields, so we cannot put up a grid. */
    /* Todo: don't forget to free if there are zero rows. */
    /* Todo: Maybe that's not true now? look at result[0] for field names. */
    /* Todo: Really it's not true now, so we get r == 1 with a null. */
    {
      const char *tarantool_tnt_reply_data_copy= tarantool_tnt_reply.data;
      unsigned long r= tarantool_num_rows(connection_number);
      tarantool_tnt_reply.data= tarantool_tnt_reply_data_copy;
//      if ((r == 0)
//       && (tarantool_select_nosql == false)
////       && (statement_type == TOKEN_KEYWORD_SELECT)
//       )
//      {
//        strcpy(tarantool_errmsg, "Zero rows.");
//        tarantool_errno[connection_number]= 10027;
//      }
      result_row_count= r;
    }
  }
  delete [] request_string;
  return tarantool_errno[connection_number];
}

/*
  For LUA 'x x x', how on earth do I know if it returns a result set?
  As far as I can tell, I must know this in advance because for eval()
  I have to say 'return ' somewhere. Somehow the regular tarantool client
  figures this out, but I don't know the official method if there is one.
  My guess is based on the Lua keyword list ...
  If first word = and break do else elseif end for function if in local or repeat return then until while
  (But not: true nil false not)
  then do not add "return "
  else If the second token is: "="
  then do not add "return "
  else add "return "
  ... Eventually we should parse Lua and then we can get rid of this.
      Todo: now we do parse Lua, so consider how to get rid of this.
  TODO: Nothing calls this, we can get rid of it.
*/
QString MainWindow::tarantool_add_return(QString s)
{
  log("tarantool_add_return start", 80);
  int token_offsets[100]; /* Surely a single assignable target can't have more */
  int token_lengths[100];
  tokenize(s.data(),
           s.size(),
           &token_lengths[0], &token_offsets[0], 100 - 1,
          (QChar*)"33333", 2, "", 1);
  QString word_1= s.mid(token_offsets[0], token_lengths[0]);
  if ((word_1 == "and") || (word_1 == "break") || (word_1 == "do")
   || (word_1 == "else") || (word_1 == "elseif") || (word_1 == "end")
   || (word_1 == "for") || (word_1 == "function") || (word_1 == "if")
   || (word_1 == "in") || (word_1 == "local") || (word_1 == "or")
   || (word_1 == "repeat") || (word_1 == "return") || (word_1 == "then")
   || (word_1 == "until") || (word_1 == "while"))
    return s;
  QString word_2;
  for (int i= 1; i < 100 - 1; ++i)
  {
    word_2= s.mid(token_offsets[i], token_lengths[i]);
    if ((word_2 == "") || (word_2 == "(")) break;
    if (word_2 == "=") return s;
  }
  log("tarantool_add_return end", 80);
  return "return " + s;
}

/*
  Given tarantool_tnt_reply, return number of fields from a SELECT. Used by result grid.
  Actually there are two counts: the count of main fields, and the count of sub-fields.
  Todo: what if there are arrays within arrays?
  Todo: shouldn't map fields be arranged so new map value = new field?
  Due to flattening, field_count is count of scalars not count of arrays and maps.
  For example, array[2] X Y array[3] A B C has 5 scalars.
  Todo: Check is it ok to pass connection_number==0 to tarantool_result_set_init
*/
unsigned int MainWindow::tarantool_num_fields()
{
  const char *tarantool_tnt_reply_data_copy= tarantool_tnt_reply.data;
  const char **tarantool_tnt_reply_data= &tarantool_tnt_reply.data;
  char field_name[TARANTOOL_MAX_FIELD_NAME_LENGTH];
  unsigned int max_field_count;

  QString field_name_list= "";

  /* See tarantool_num_rows for pretty well the same code as this */
  /* Todo: this is wrong, connection_number might not be 0 */
  int result_set_type;
  *tarantool_tnt_reply_data= tarantool_result_set_init(0, &result_row_count, &result_set_type);
  if ((result_set_type == 0) || (result_set_type == 1))
  {
//    result_row_count= 0;
    return 0;
  }
//  if (result_set_type == 2) result_row_count= 1;
//  else
//  {
//    if (result_set_type != 4)
//    {
//      lmysql->ldbms_mp_decode_array(tarantool_tnt_reply_data);
//    }
//    result_row_count= lmysql->ldbms_mp_decode_array(tarantool_tnt_reply_data);
//  }
  if (result_set_type == 5)
  {
    int bytes;
    int row_size_1;
    QString fetch_row_result= tarantool_fetch_row(*tarantool_tnt_reply_data, &bytes, &row_size_1);
    //if (fetch_row_result != "OK") return fetch_row_result;
    *(tarantool_tnt_reply_data)+= bytes;
//    --result_row_count;
  }

  strcpy(field_name, TARANTOOL_FIELD_NAME_BASE);
  strcat(field_name, "_");
  for (long unsigned int r= 0; r < result_row_count; ++r)
  {
    int return_value= tarantool_num_fields_recursive(tarantool_tnt_reply_data,
                                                     field_name, 0, &field_name_list);
    if (return_value < 0) assert(0 != 0);
  }

  max_field_count= field_name_list.count(" ");

  /* TODO: THIS IS NEVER FREED! LEAK! */
  tarantool_field_names= new char[max_field_count * TARANTOOL_MAX_FIELD_NAME_LENGTH];

  unsigned int mid_index= 0;
  for (unsigned int i= 0; i < max_field_count; ++i)
  {
    char tmp[TARANTOOL_MAX_FIELD_NAME_LENGTH];
    int j= field_name_list.indexOf(" ", mid_index);
    assert((j != -1) && ((j-mid_index) < TARANTOOL_MAX_FIELD_NAME_LENGTH));
    QString sv;
    sv= field_name_list.mid(mid_index, j-mid_index);
    strcpy(tmp, sv.toUtf8());
    *(tmp+sv.length())='\0';
    strcpy(&tarantool_field_names[i*TARANTOOL_MAX_FIELD_NAME_LENGTH], tmp);
    mid_index= j + 1;
  }

  /* The field name list must be in order, so bubble sort. */
  /* todo: this is unnecessary if row count <= 1 */
  /* todo: use qsort and swap pointers instead of 64-byte strings, this is undignified code */
  {
    unsigned int c, d;
    char swap[TARANTOOL_MAX_FIELD_NAME_LENGTH];
    for (c= 0 ; c < (max_field_count - 1); ++c)
    {
      for (d= 0 ; d < max_field_count - c - 1; ++d)
      {
        if (memcmp(&tarantool_field_names[d*TARANTOOL_MAX_FIELD_NAME_LENGTH],
                   &tarantool_field_names[(d+1)*TARANTOOL_MAX_FIELD_NAME_LENGTH],
                   TARANTOOL_MAX_FIELD_NAME_LENGTH) > 0)
        {
          memcpy(swap, &tarantool_field_names[d*TARANTOOL_MAX_FIELD_NAME_LENGTH], TARANTOOL_MAX_FIELD_NAME_LENGTH);
          memcpy(&tarantool_field_names[d*TARANTOOL_MAX_FIELD_NAME_LENGTH],&tarantool_field_names[(d+1)*TARANTOOL_MAX_FIELD_NAME_LENGTH],TARANTOOL_MAX_FIELD_NAME_LENGTH);
          memcpy(&tarantool_field_names[(d+1)*TARANTOOL_MAX_FIELD_NAME_LENGTH],swap,TARANTOOL_MAX_FIELD_NAME_LENGTH);
        }
      }
    }
  }


  tarantool_tnt_reply.data= tarantool_tnt_reply_data_copy;

  return max_field_count;
}


int MainWindow::tarantool_num_fields_recursive(const char **tarantool_tnt_reply_data,
                                               char *field_name,
                                               int field_number_within_array,
                                               QString *field_name_list)
{
  bool is_scalar;
  char field_type= lmysql->ldbms_mp_typeof(**tarantool_tnt_reply_data);
  if ((field_type == MP_NIL)
   || (field_type == MP_UINT)
   || (field_type == MP_INT)
   || (field_type == MP_STR)
   || (field_type == MP_BIN)
   || (field_type == MP_BOOL)
   || (field_type == MP_FLOAT)
   || (field_type == MP_DOUBLE)
   || (field_type == MP_EXT))
    is_scalar= true;
  else if ((field_type == MP_ARRAY) || (field_type == MP_MAP))
    is_scalar= false;
  else return -1;

  /* p_field_name_end, within field_name, = after last '_' or after TARANTOOL_FIELD_NAME_BASE */
  char *p_field_name_end;
  for (p_field_name_end= field_name + strlen(field_name);; --p_field_name_end)
  {
    if (*p_field_name_end == '_') break;
  }
  ++p_field_name_end;
  if (is_scalar == false)
  {
    uint32_t array_size;
    if (field_type == MP_ARRAY)
      array_size= lmysql->ldbms_mp_decode_array(tarantool_tnt_reply_data);
    else
      array_size= lmysql->ldbms_mp_decode_map(tarantool_tnt_reply_data) * 2;
    if (array_size != 0)
    {
      sprintf(p_field_name_end, "%05d_", field_number_within_array + 1);
      for (uint32_t i= 0; i < array_size; ++i)
      {
        int return_value= tarantool_num_fields_recursive(tarantool_tnt_reply_data,
                                                         field_name, i, field_name_list);
        if (return_value < 0) return return_value;
      }
      p_field_name_end= field_name + strlen(field_name) - 1;
      while (*p_field_name_end != '_') --p_field_name_end;
      *(p_field_name_end - 1)= '\0';
      return 0;
    }
  }
  if (is_scalar == true)
    lmysql->ldbms_mp_next(tarantool_tnt_reply_data);
  /* is_scalar == true or array size == 0 which we treat as null */
  sprintf(p_field_name_end, "%05d ", field_number_within_array + 1);
  if ((*field_name_list).contains(field_name) == false) (*field_name_list).append(field_name);
  return 0;
}

/* To "seek to row zero", start with the initial pointer and skip over the row count. */
/* Also skip field names if result_set_type == 5. */
/* Todo: Check is it ok to pass connection_number==0 to tarantool_result_set_init */
const char * MainWindow::tarantool_seek_0(int *returned_result_set_type)
{
  uint32_t row_count;
  const char *tarantool_tnt_reply_data;
  //tarantool_tnt_reply_data= tarantool_tnt_reply.data;

  long unsigned int tmp_row_count;
  int result_set_type;
  tarantool_tnt_reply_data= tarantool_result_set_init(0, &tmp_row_count, &result_set_type);
  row_count= tmp_row_count;

  if ((result_set_type == 0) || (result_set_type == 1))
  {
    assert(result_set_type > 1);
  }
//  if (result_set_type == 2) row_count= 1;
//  else
//  {
//    if (result_set_type != 4)
//    {
//      lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);
//    }
//    row_count= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);
//  }
  if (result_set_type == 5)
  {
    int bytes;
    int row_size_1;
    QString fetch_row_result= tarantool_fetch_row(tarantool_tnt_reply_data, &bytes, &row_size_1);
    //if (fetch_row_result != "OK") return fetch_row_result;
    tarantool_tnt_reply_data+= bytes;
//    --row_count;
  }
  assert(row_count == result_row_count);
  *returned_result_set_type= result_set_type;
  return tarantool_tnt_reply_data;
}

/*
  Originally this resembled mysql_fetch_row, till we realized we only want the total length.
  The traversal of fields must use the same system as in tarantool_scan_rows.
  The value of tarantool_tnt_reply_data increases, so at the end it points past row end.
  The calculation involves the actual amount that any non-missing field will take
  when sprintf'd to the row copy, but not the per-field overhead for all fields.
  todo: value_length is usually an unnecessary variable, just add to total_length
*/
QString MainWindow::tarantool_fetch_row(const char *tarantool_tnt_reply_data,
                                             int *bytes, int *tsize)
{
  const char *original_tarantool_tnt_reply_data= tarantool_tnt_reply_data;
  unsigned int total_length= 0;
  char field_type;
  field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data);
  if (field_type != MP_ARRAY) return "tarantool_fetch_row: field_type != MP_ARRAY";
  uint32_t field_count= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);
  if (field_count == 0) return "tarantool_fetch_row: field_count == 0";

  for (uint32_t field_number= 0; field_number < field_count; ++field_number)
  {
    field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data);
    assert(field_type <= MP_EXT);
    uint32_t value_length;
    const char *value;
    (void) value; /* suppress 'variable set but not used' warning */
    char value_as_string[320]; /* must be big enough for any sprintf() result */
    if (field_type == MP_NIL)
    {
      lmysql->ldbms_mp_decode_nil(&tarantool_tnt_reply_data);
      value_length= 0;
    }
    if (field_type == MP_UINT)
    {
      uint64_t uint_value= lmysql->ldbms_mp_decode_uint(&tarantool_tnt_reply_data);
      long long unsigned int llu= uint_value;
      value_length= sprintf(value_as_string, "%llu", llu);
    }
    if (field_type == MP_INT)
    {
      int64_t int_value= lmysql->ldbms_mp_decode_int(&tarantool_tnt_reply_data);
      long long int lli= int_value;
      value_length= sprintf(value_as_string, "%lld", lli);
    }
    if (field_type == MP_STR)
    {
      /* todo: allow for the library routine that only gets length */
      value= lmysql->ldbms_mp_decode_str(&tarantool_tnt_reply_data, &value_length);
    }
    if (field_type == MP_BIN)
    {
      /* todo: allow for the library routine that only gets length */
      value= lmysql->ldbms_mp_decode_bin(&tarantool_tnt_reply_data, &value_length);
    }
    if (field_type == MP_ARRAY)
    {
      uint32_t array_size;
      array_size= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);
      field_count+= array_size;
      value_length= 0;
    }
    if (field_type == MP_MAP)
    {
      uint32_t array_size;
      array_size= lmysql->ldbms_mp_decode_map(&tarantool_tnt_reply_data) * 2;
      field_count+= array_size;
      value_length= 0;
    }
    if (field_type == MP_BOOL)
    {
      bool bool_value= lmysql->ldbms_mp_decode_bool(&tarantool_tnt_reply_data);
      value_length= sprintf(value_as_string, "%d", bool_value);
    }
    if (field_type == MP_FLOAT)
    {
      float float_value= lmysql->ldbms_mp_decode_float(&tarantool_tnt_reply_data);
      value_length= sprintf(value_as_string, "%f", float_value);
    }
    if (field_type == MP_DOUBLE)
    {
      double double_value= lmysql->ldbms_mp_decode_double(&tarantool_tnt_reply_data);
      value_length= sprintf(value_as_string, "%E", double_value);
    }
    if (field_type == MP_EXT)
    {
      lmysql->ldbms_mp_next(&tarantool_tnt_reply_data);
      value_length= sizeof("EXT");
    }

    total_length+= value_length;
  }
  *bytes=   tarantool_tnt_reply_data - original_tarantool_tnt_reply_data;
  if (*bytes <= 0) return "tarantool_fetch_row: *bytes <= 0";
  *tsize= total_length;
  return "OK";
}


/*
  The same loop as the one in tarantool_fetch_row(), for row#1.
  This is for result_set_type == 5, where row#1 should actually
  be the header i.e. the field names.
  Skip the dd 00 00 00 01 and the row count.
  Skip the first field which is the a1 08 signature.
  For each header field
   replacing "f_n" section, i.e. as far as the second "_", if any.
   replacing "f_1..." with name#1..., "f_2..." with name#2..., etc.
  Todo: finding "f_n" is inefficient, memcmp etc. should be avoided.
  Todo: Check is it ok to pass connection_number==0 to tarantool_result_set_init
*/
QString MainWindow::tarantool_fetch_header_row(int p_result_column_count)
{
  const char *tarantool_tnt_reply_data;
  tarantool_tnt_reply_data= tarantool_tnt_reply.data;

//  lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);
//  lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);

  char field_type;
//  field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data);
//  if (field_type != MP_ARRAY) return "tarantool_fetch_header_row: field_type != MP_ARRAY";

  long unsigned int tmp_row_count;
  int result_set_type;
  tarantool_tnt_reply_data= tarantool_result_set_init(0, &tmp_row_count, &result_set_type);
  uint32_t field_count= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);
  if (field_count == 0) return "tarantool_fetch_header_row: field_count == 0";

  uint32_t value_length;
  const char *value;
  char *c, *c2;
  value= lmysql->ldbms_mp_decode_str(&tarantool_tnt_reply_data, &value_length);

  for (uint32_t field_number= 0; field_number < field_count - 1; ++field_number)
  {
    field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data);
    if (field_type != MP_STR) return "tarantool_fetch_header_row: non-string field";
    value= lmysql->ldbms_mp_decode_str(&tarantool_tnt_reply_data, &value_length);
    for (int rf= 0; rf < p_result_column_count; ++rf)
    {
      c= &tarantool_field_names[rf*TARANTOOL_MAX_FIELD_NAME_LENGTH];
      char fn[8];
      if (memcmp(c, TARANTOOL_FIELD_NAME_BASE, strlen(TARANTOOL_FIELD_NAME_BASE)) == 0)
      {
        char cm= *(c + strlen(TARANTOOL_FIELD_NAME_BASE));
        if (cm == '_')
        {
          c2= c + strlen(TARANTOOL_FIELD_NAME_BASE) + 1;
          int j1, j2;
          for (j1= 0, j2= 0; *(c2 + j1) != '\0' && *(c2 + j1) != '_'; ++j1, ++j2)
          {
            fn[j2]= *(c2 + j1);
          }
          fn[j2]= '\0';
          unsigned int j3= atoi(fn);
          if ((isdigit(fn[0]) && j3 == field_number + 1))
          {
            char tmp[TARANTOOL_MAX_FIELD_NAME_LENGTH];
            strcpy(tmp, c2 + j1 + strlen(TARANTOOL_FIELD_NAME_BASE) + 1);
            memcpy(c, value, value_length);
            *(c + value_length)= '\0';
            strcat(c, tmp);
          }
        }
      }
    }
  }
  return "OK";
}


/*
  Given tarantool_tnt_reply, fill in field names + types + max widths.
  Return amount that we would need for allocating.
  Called from: scan_rows in result grid.
  Compare: what we do for first loop in scan_rows().
*/
QString MainWindow::tarantool_scan_rows(unsigned int p_result_column_count,
               unsigned int p_result_row_count,
               MYSQL_RES *p_mysql_res,
               char **p_result_set_copy,
               char ***p_result_set_copy_rows,
               unsigned int **p_result_max_column_widths)
{
  (void) p_mysql_res; /* suppress "unused parameter" warning */
  unsigned long int v_r;
  unsigned int i;
  int returned_result_set_type;
  //char **v_row;
  //unsigned long *v_lengths;
//  unsigned int ki;
  const char *tarantool_tnt_reply_data_copy;

  ResultGrid *rg= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(0));

  for (i= 0; i < p_result_column_count; ++i) (*p_result_max_column_widths)[i]= 0;
  /*
    First loop: find how much to allocate. Allocate. Second loop: fill in with pointers within allocated area.
  */
  unsigned int total_size= 0;
  char *result_set_copy_pointer;
  tarantool_tnt_reply_data_copy= tarantool_seek_0(&returned_result_set_type); /* "seek to row 0" */
  for (v_r= 0; v_r < p_result_row_count; ++v_r)                                /* first loop */
  {
    int bytes;
    int row_size_1;
    QString fetch_row_result= tarantool_fetch_row(tarantool_tnt_reply_data_copy, &bytes, &row_size_1);
    if (fetch_row_result != "OK") return fetch_row_result;
    tarantool_tnt_reply_data_copy+= bytes;
    total_size+= row_size_1;
    /* per-field overhead includes overhead for missing fields; they are null */
    int row_size_2= p_result_column_count * (sizeof(unsigned int) + sizeof(char));
    total_size+= row_size_2;
  }
  if (total_size > 2000000000) return "tarantool_scan_rows: total_size too big";
  *p_result_set_copy= new char[total_size];                                         /* allocate */
  *p_result_set_copy_rows= new char*[p_result_row_count];
  result_set_copy_pointer= *p_result_set_copy;
  tarantool_tnt_reply_data_copy= tarantool_seek_0(&returned_result_set_type); /* "seek to row 0" */
  for (v_r= 0; v_r < p_result_row_count; ++v_r)                                 /* second loop */
  {
    (*p_result_set_copy_rows)[v_r]= result_set_copy_pointer;
    //char *tmp_copy_pointer= result_set_copy_pointer;
    /* Form a field name list the same way you did in tarantool_num_fields */
    QString field_name_list= "";
    {
      const char *tarantool_tnt_reply_data_copy_2= tarantool_tnt_reply_data_copy;
      const char **tarantool_tnt_reply_data= &tarantool_tnt_reply_data_copy;
      char field_name[TARANTOOL_MAX_FIELD_NAME_LENGTH];
      strcpy(field_name, TARANTOOL_FIELD_NAME_BASE);
      strcat(field_name, "_");
      int return_value= tarantool_num_fields_recursive(tarantool_tnt_reply_data,
                                                       field_name, 0, &field_name_list);
      if (return_value < 0) return "tarantool_scan_rows: return_value < 0";
      tarantool_tnt_reply_data_copy= tarantool_tnt_reply_data_copy_2;
    }
    char field_type;
    field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data_copy);
    if (field_type != MP_ARRAY) return "tarantool_scan_rows: field_type != MP_ARRAY";
    uint32_t field_count= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data_copy);
    if (field_count > p_result_column_count) return "tarantool_scan_rows: field_count > p_result_column_count";
    const char *value;
    uint32_t value_length;
    char value_as_string[320]; /* must be big enough for any sprintf() result */
    /*
      Number of fields = number of names in field_name_list.
      At this point we can skip over MP_ARRAY and MP_MAP because we only care about scalars.
      Whenever we get a next field, we know its name is next entry in field_name_list
      If that's greater than something in the main list (which can only happen if row count > 1),
      then add NULLs for the missing fields until the main list catches up.
    */
    field_count= field_name_list.count(" ");
    unsigned int mid_index= 0;
    int field_number_in_main_list= 0;
    for (uint32_t field_number= 0; field_number < field_count; ++field_number)
    {
      int i= field_number;
      field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data_copy);
      if (field_type > MP_EXT) return "tarantool_scan_rows: field_type > MP_EXT";
      if (field_type == MP_ARRAY)
      {
        int array_size= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data_copy);
        if (array_size != 0)
        {
          --field_number;
          continue;
        }
      }
      if (field_type == MP_MAP)
      {
        int array_size= lmysql->ldbms_mp_decode_map(&tarantool_tnt_reply_data_copy);
        if (array_size != 0)
        {
          --field_number;
          continue;
        }
      }
      {
        char tmp[TARANTOOL_MAX_FIELD_NAME_LENGTH];
        int j= field_name_list.indexOf(" ", mid_index);
        if ((j != -1) && ((j-mid_index) < TARANTOOL_MAX_FIELD_NAME_LENGTH)) {;}
        else return "tarantool_scan_rows: check TARANTOOL_MAX_FIELD_NAME_LENGTH";
        QString sv;
        sv= field_name_list.mid(mid_index, j-mid_index);
        strcpy(tmp, sv.toUtf8());
        *(tmp+sv.length())='\0';
        for (;;)
        {
          int strcmp_result= strcmp(&tarantool_field_names[field_number_in_main_list*TARANTOOL_MAX_FIELD_NAME_LENGTH], tmp);
          if (strcmp_result > 0) return "tarantool_scan_rows: strcmp_result > 0";
          if (strcmp_result == 0) break;
          /* Dump null. Todo: similar code appears 3 times. */
          if (sizeof(NULL_STRING) - 1 > (*p_result_max_column_widths)[i]) (*p_result_max_column_widths)[i]= sizeof(NULL_STRING) - 1;
          memset(result_set_copy_pointer, 0, sizeof(unsigned int));
          *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_NULL;
          result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
          ++field_number_in_main_list;
        }
        mid_index= j + 1;
        ++field_number_in_main_list;
      }
      if ((field_type == MP_NIL) || (field_type == MP_ARRAY) || (field_type == MP_MAP))
      {
        if (sizeof(NULL_STRING) - 1 > (*p_result_max_column_widths)[field_number_in_main_list - 1]) (*p_result_max_column_widths)[field_number_in_main_list - 1]= sizeof(NULL_STRING) - 1;
        if (field_type == MP_NIL) lmysql->ldbms_mp_decode_nil(&tarantool_tnt_reply_data_copy);
        memset(result_set_copy_pointer, 0, sizeof(unsigned int));
        *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_NULL;
        result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
      }
      else
      {
        if (field_type == MP_UINT)
        {
          uint64_t uint_value= lmysql->ldbms_mp_decode_uint(&tarantool_tnt_reply_data_copy);
          long long unsigned int llu= uint_value;
          value_length= sprintf(value_as_string, "%llu", llu);
          value= value_as_string;
          *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_NUMBER;
        }
        else if (field_type == MP_INT)
        {
          int64_t int_value= lmysql->ldbms_mp_decode_int(&tarantool_tnt_reply_data_copy);
          long long int lli= int_value;
          value_length= sprintf(value_as_string, "%lld", lli);
          value= value_as_string;
          *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_NUMBER;
        }
        else if (field_type == MP_STR)
        {
          value= lmysql->ldbms_mp_decode_str(&tarantool_tnt_reply_data_copy, &value_length);
          *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_STRING;
        }
        else if (field_type == MP_BIN)
        {
          value= lmysql->ldbms_mp_decode_bin(&tarantool_tnt_reply_data_copy, &value_length);
          *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_STRING;
        }
        else if (field_type == MP_BOOL)
        {
          bool bool_value= lmysql->ldbms_mp_decode_bool(&tarantool_tnt_reply_data_copy);
          value_length= sprintf(value_as_string, "%d", bool_value);
          value= value_as_string;
          *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_OTHER;
        }
        else if (field_type == MP_FLOAT)
        {
          float float_value= lmysql->ldbms_mp_decode_float(&tarantool_tnt_reply_data_copy);
          value_length= sprintf(value_as_string, "%f", float_value);
          value= value_as_string;
          *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_NUMBER;
        }
        else if (field_type == MP_DOUBLE)
        {
          double double_value= lmysql->ldbms_mp_decode_double(&tarantool_tnt_reply_data_copy);
          value_length= sprintf(value_as_string, "%E", double_value);
          value= value_as_string;
          *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_NUMBER;
        }
        else if (field_type == MP_EXT)
        {
          lmysql->ldbms_mp_next(&tarantool_tnt_reply_data_copy);
          value_length= sprintf(value_as_string, "%s", "EXT");
          value= value_as_string;
          *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_OTHER;
        }
        else
        {
          /* it probably would be proper to assert() here */
          value_length= 0;
          value= "";
        }
        //if (value_length > (*p_result_max_column_widths)[i]) (*p_result_max_column_widths)[i]= value_length;
        rg->set_max_column_width(value_length, value, (&(*p_result_max_column_widths)[field_number_in_main_list - 1]));
        memcpy(result_set_copy_pointer, &value_length, sizeof(unsigned int));
        result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
        memcpy(result_set_copy_pointer, value, value_length);
        result_set_copy_pointer+= value_length;
      }
    }
    while ((unsigned int) field_number_in_main_list < p_result_column_count)
    {
      /* Dump null. Todo: similar code appears 3 times. */
      if (sizeof(NULL_STRING) - 1 > (*p_result_max_column_widths)[field_number_in_main_list]) (*p_result_max_column_widths)[field_number_in_main_list]= sizeof(NULL_STRING) - 1;
      memset(result_set_copy_pointer, 0, sizeof(unsigned int));
      *(result_set_copy_pointer + sizeof(unsigned int))= FIELD_VALUE_FLAG_IS_NULL;
      result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
      ++field_number_in_main_list;
    }
  }
  /* Now that names are sorted, we can shorten them e.g. f_0001_0007_0001_0001 becomes f_7_1_1. */
  for (unsigned int i= 0; i < p_result_column_count; ++i)
  {
    char *c= &tarantool_field_names[i*TARANTOOL_MAX_FIELD_NAME_LENGTH];
    for (unsigned int j= strlen(TARANTOOL_FIELD_NAME_BASE); *(c + j) != '\0'; ++j)
    {
      if ((*(c + j) == '0') && (*(c + j - 1) == '_'))
      {
        unsigned int k;
        for (k= j + 1; *(c + k) != '\0'; ++k) *(c + k - 1)= *(c + k);
        *(c + k - 1)= '\0';
        --j;
      }
    }
    unsigned int j= strlen(TARANTOOL_FIELD_NAME_BASE);
    if ((*(c + j) == '_') && (*(c + j + 1) == '1'))
    {
      unsigned int k;
      for (k= j + 2; *(c + k) != '\0'; ++k) *(c + k - 2)= *(c + k);
      *(c + k - 2)= '\0';
    }
  }
  if (returned_result_set_type == 5)
  {
    QString fetch_header_row_result= tarantool_fetch_header_row(p_result_column_count);
    if (fetch_header_row_result != "OK") return fetch_header_row_result;
  }

  return "OK";
}


/*
  Todo: check: in tarantool_scan_field_names(), should I use
  sizeof(TARANTOOL_FIELD_NAME_BASE) or strlen(TARANTOOL_FIELD_NAME_BASE)?
*/

void MainWindow::tarantool_scan_field_names(
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
    if (strcmp(which_field, "name") == 0) total_size+= strlen(&tarantool_field_names[i * TARANTOOL_MAX_FIELD_NAME_LENGTH]);
    else if (strcmp(which_field, "org_name") == 0) total_size+= sizeof(TARANTOOL_FIELD_NAME_BASE);
    else if (strcmp(which_field, "org_table") == 0) total_size+= sizeof(TARANTOOL_FIELD_NAME_BASE);
    else /* if (strcmp(which_field, "db") == 0) */ total_size+= sizeof(TARANTOOL_FIELD_NAME_BASE);
  }
  *p_result_field_names= new char[total_size];                               /* allocate */

  result_field_names_pointer= *p_result_field_names;
  for (i= 0; i < p_result_column_count; ++i)                                 /* second loop */
  {
    if (strcmp(which_field, "name") == 0) v_lengths= strlen(&tarantool_field_names[i * TARANTOOL_MAX_FIELD_NAME_LENGTH]);
    else if (strcmp(which_field, "org_name") == 0) v_lengths= sizeof(TARANTOOL_FIELD_NAME_BASE);
    else if (strcmp(which_field, "org_table") == 0) v_lengths= sizeof(TARANTOOL_FIELD_NAME_BASE);
    else /* if (strcmp(which_field, "db") == 0) */ v_lengths= sizeof(TARANTOOL_FIELD_NAME_BASE);
    memcpy(result_field_names_pointer, &v_lengths, sizeof(unsigned int));
    result_field_names_pointer+= sizeof(unsigned int);
    if (strcmp(which_field, "name") == 0) memcpy(result_field_names_pointer, &tarantool_field_names[i * TARANTOOL_MAX_FIELD_NAME_LENGTH], v_lengths);
    else if (strcmp(which_field, "org_name") == 0) memcpy(result_field_names_pointer, TARANTOOL_FIELD_NAME_BASE, v_lengths);
    else if (strcmp(which_field, "org_table") == 0) memcpy(result_field_names_pointer,  TARANTOOL_FIELD_NAME_BASE, v_lengths);
    else /* if (strcmp(which_field, "db") == 0) */ memcpy(result_field_names_pointer, TARANTOOL_FIELD_NAME_BASE, v_lengths);
    result_field_names_pointer+= v_lengths;
  }
}

//void MainWindow::tarantool_close()
//{
//lmysql->ldbms_tnt_close(tnt);
//lmysql->ldbms_tnt_stream_free(tuple);
//lmysql->ldbms_tnt_stream_free(tnt);
//}

#endif

#ifdef DBMS_TARANTOOL
/*
  Handle CREATE [TEMPORARY] TABLE table-name SERVER server-name
   {LUA 'expression'} | {RETURN lua-expression};
  We want to exeute the lua-expression on the remote server created
  with CREATE SERVER. This is going to be complicated.
  The 'expression' must cause return a result set.
  Do a fillup.
  From the result set, we can figure out field names and types.
  So that tells us how to CREATE TABLE table-name (...) on main.
  Then we can INSERT INTO table-name VALUES (result-set values);
  Todo: There are other things we could do, e.g. SELECT from remote.
  Todo: It would be better to check for LUA|RETURN when parsing.
*/
int MainWindow::create_table_server(QString text,
                                         bool *is_create_table_server,
                                         unsigned int passed_main_token_number,
                                         unsigned int passed_main_token_count_in_statement)
{
  *is_create_table_server= false;
  /* Quick search -- if there is no SERVER id clause, get out now. */

  bool is_create= false;
  unsigned int i_of_table= 0;
  unsigned int i_of_server= 0;
  unsigned int i_of_server_id= 0;
  unsigned int i_of_literal= 0;
  unsigned int i_of_lua= 0;
  unsigned int i_of_return= 0;
  for (unsigned int i= passed_main_token_number; i < passed_main_token_number + passed_main_token_count_in_statement; ++i)
  {
    if ((main_token_types[i] >= TOKEN_TYPE_COMMENT_WITH_SLASH)
     && (main_token_types[i] <= TOKEN_TYPE_COMMENT_WITH_MINUS))
      continue;
    if (main_token_types[i] == TOKEN_KEYWORD_CREATE)
    {
      is_create= true;
    }
    if (is_create == false) break;
    if (main_token_types[i] == TOKEN_KEYWORD_TABLE)
    {
      i_of_table= i;
    }
    if ((main_token_types[i] == TOKEN_KEYWORD_SERVER)
     && (i_of_table > 0))
    {
      i_of_server= i;
    }
    if (i_of_server > 0)
    {
      if (main_token_reftypes[i] == TOKEN_REFTYPE_SERVER)
      {
        i_of_server_id= i;
      }
      if (main_token_types[i] == TOKEN_KEYWORD_LUA)
      {
        i_of_lua= i;
      }
      if (main_token_types[i] == TOKEN_KEYWORD_RETURN)
      {
        i_of_return= i;
      }
      if ((main_token_types[i] <= TOKEN_TYPE_LITERAL)
       && (main_token_types[i] >= TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE))
      {
        i_of_literal= i;
      }
    }
  }

  if (i_of_server == 0) return 0;

  *is_create_table_server= true;

  if (tarantool_server_name == "")
  {
    /* "CREATE TABLE ... SERVER fails, CREATE SERVER not done?" */
    make_and_put_message_in_result(ER_CREATE_SERVER, 0, (char*)"");
    return 1;
  }
  {
    QString s;
    if (i_of_server_id == 0) s= "";
    else s= text.mid(main_token_offsets[i_of_server_id], main_token_lengths[i_of_server_id]);
    if (QString::compare(s, tarantool_server_name, Qt::CaseSensitive) != 0)
    {
      /* "CREATE TABLE ... SERVER fails, server id <> CREATE SERVER id\n" */
    make_and_put_message_in_result(ER_CREATE_SERVER, 0, (char*)"");
    return 1;
    }
  }

  if ((i_of_literal == 0) && (i_of_lua != 0))
  {
    /* "CREATE TABLE ... SERVER fails, blank literal\n" */
    make_and_put_message_in_result(ER_EMPTY_LITERAL, 0, (char*)"");
    return 1;
  }

  /* Todo: There should be a separate error message for this ... */
  if (i_of_server_id == 0)
  {
    /* "CREATE TABLE ... SERVER fails, blank literal\n" */
    make_and_put_message_in_result(ER_EMPTY_LITERAL, 0, (char*)"");
    return 1;
  }

  /* Todo: There should be a separate error message for this ... */
  if ((i_of_lua == 0) && (i_of_return == 0))
  {
    /* "CREATE TABLE ... SERVER fails, blank literal\n" */
    make_and_put_message_in_result(ER_EMPTY_LITERAL, 0, (char*)"");
    return 1;
  }

  QString q;
  unsigned int new_passed_main_token_number;
  unsigned int new_passed_main_token_count_in_statement;
  if (i_of_lua == 0)
  {
    unsigned int m= passed_main_token_number + passed_main_token_count_in_statement;
    q= text.mid(main_token_offsets[i_of_return],
                main_token_offsets[m - 1] + main_token_lengths[m - 1] - main_token_offsets[i_of_return]);
    new_passed_main_token_number= i_of_server_id + 1;
    new_passed_main_token_count_in_statement= passed_main_token_count_in_statement - (i_of_return);
  }
  else
  {
    q= text.mid(main_token_offsets[i_of_lua],
                main_token_offsets[i_of_literal] + main_token_lengths[i_of_literal] - main_token_offsets[i_of_lua]);
    new_passed_main_token_number= i_of_lua;
    /* Todo: Check: why i_of_literal? */
    new_passed_main_token_count_in_statement= i_of_literal;
  }
  QString read_format_result= tarantool_read_format(q);
  int result;
  ResultGrid *rg= new ResultGrid(lmysql, this, false);
  for (;;)
  {
    result=
    tarantool_real_query(q.toUtf8(),
                         q.toUtf8().size(),
                         MYSQL_REMOTE_CONNECTION,
                         new_passed_main_token_number,
                         new_passed_main_token_count_in_statement);
    if (result != 0)
    {
      put_diagnostics_in_result(MYSQL_REMOTE_CONNECTION);
      break;
    }
    /* CREATE TABLE y4 SERVER id LUA 'box.space._space:select()'; crashed */
    {
      long unsigned int r;
      int result_set_type;
      tarantool_result_set_init(MYSQL_REMOTE_CONNECTION, &r, &result_set_type);
      if ((result_set_type) == 0 || (result_set_type == 1))
      {
        tarantool_errno[MYSQL_MAIN_CONNECTION]= 9999;
        result= 9999;
        strcpy(tarantool_errmsg, "No result set");
        put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
        break;
      }
      if (r == 0)
      {
        tarantool_errno[MYSQL_MAIN_CONNECTION]= 9999;
        result= 9999;
        strcpy(tarantool_errmsg, "Empty result set");
        put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
        break;
      }
    }

    /* TODO: I'd be much happier if we didn't fool with existing grid */
    /* TODO: Check for an error return from fillup(). */
    //rg= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(0));
    rg->fillup(mysql_res,
              //&tarantool_tnt_reply,
              connections_dbms[MYSQL_REMOTE_CONNECTION],
              //this,
              false, ocelot_result_grid_column_names,
              lmysql, ocelot_client_side_functions,
              ocelot_batch, ocelot_html, ocelot_raw, ocelot_xml,
              MYSQL_REMOTE_CONNECTION,
              false);
    /* TODO: Get field names and data types from fillup!! */
    QString create_table_statement=
    text.mid(main_token_offsets[passed_main_token_number],
             main_token_offsets[i_of_server] - main_token_offsets[passed_main_token_number]);
    /* Pass "CREATE TABLE table-name", fill in (field-names) + execute */
    result= rg->creates(create_table_statement, connections_dbms[MYSQL_MAIN_CONNECTION], read_format_result);
    if (result != 0)
    {
      put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
      break;
    }
    QString temporary_table_name=
            text.mid(main_token_offsets[i_of_table] + main_token_lengths[i_of_table],
                     main_token_offsets[i_of_server] - (main_token_offsets[i_of_table] + main_token_lengths[i_of_table]));
    /* INSERT INTO [temporary_table_name] VALUES (...); */
    result= rg->inserts(temporary_table_name);
    if (result != 0)
    {
      put_diagnostics_in_result(MYSQL_MAIN_CONNECTION);
      break;
    }
    break;
  }
  /* Todo: Now destroy the temporary tables? (What temporary tables?) */
  delete rg;
  return result;
}
#endif

#ifdef DBMS_TARANTOOL
/*
  Pass: lua_request which might contain space_name.
  Read the _space tuple for space_name. Example:
  - [514,                                                                   -- id
     1,                                                                     -- owner
     'y1',                                                                  -- name
     'memtx',                                                               -- engine
     2,                                                                     -- field_count
     {'sql': 'CREATE TABLE y1 (f_1 VARCHAR(5) PRIMARY KEY,f_2 BIGINT )'},   -- flags
     [{'name': 'f_1', 'type': 'scalar'},                                    -- format
      {'name': 'f_2', 'type': 'scalar'}]]
  We care about the format field. It might be blank i.e. [].
  If it is not blank, we want to use the list of names.
  This is for create_table_server i.e. CREATE TABLE ... SERVER ... LUA '...';
  It only works if the Lua request looks like box.space.space-name:select().
  The list might not be complete.
  Compare tarantool_internal_query()
  Returns e.g. [1]id[2]owner[3]name[4]engine[5]field_count[6]flags[7]format
  Todo: the loop calls the server once per field! Fix this real soon, eh?
  Todo: someday use the types too.
  Todo: use for any SELECT from a single table, or box.space.space_name:select()
*/
QString MainWindow::tarantool_read_format(QString lua_request)
{
  QString read_format_result= "";
  QString word= "";
  /* e.g. from LUA 'box.space.t:select()' find 't' */
  /* Todo: this doesn't allow for whitespace */
  int word_start= lua_request.indexOf("box.space.", 0);
  if (word_start == -1) return "";
  word_start+= strlen("box.space.");
  int word_end= lua_request.indexOf(":", word_start);
  if (word_end == -1) return "";
  word= lua_request.mid(word_start, (word_end - word_start));
  for (int i= 1; i < 10; ++i)
  {
    char i_str[10];
    sprintf(i_str, "%d", i);
    QString request= "return box.space._space.index.name:select('"
                     + word
                     + "')[1][7]["
                     + i_str
                     + "].name";
    char tmp[1024];
    strcpy(tmp, request.toUtf8());
    QString result= tarantool_internal_query(tmp, MYSQL_REMOTE_CONNECTION);
    if (result == "") break;
    read_format_result= read_format_result + "[" + i_str + "]" + result;
  }
  return read_format_result;
}
#endif

/*
  log() may be useful for debugging if the program is acting strangely.
  We do not use printf() ordinarily, but if level > ocelot_log_level,
  we printf(message).
  We also fflush(stdout) which is usually unnecessary (messages
  are printed with "\n") unless stdout has been redirected to a file.
  ocelot_log_level default value = 100, can be changed with --ocelot_log_level==N.
  Todo: consider using stderr or a named file.
  Todo: attach a timer or counter so printf occurs if dangers exist.
  Todo: consider using a bit mask instead of a greater-than comparison.
        or, a system with definitions, e.g. 10=trivial, 20=unusual,
        30=surprising, 200=pseudo-assertion
  Todo: consider making this a macro so invocation is easier.
*/
void MainWindow::log(const char *message, int level)
{
  if (level > ocelot_log_level)
  {
    printf("%s\n", message);
#ifdef OCELOT_OS_LINUX
    fflush(stdout);
#endif
  }
}


#include "codeeditor.h"

/*
  TextEditFrame
  This is one of the components of result_grid
*/
TextEditFrame::TextEditFrame(QWidget *parent, ResultGrid *result_grid_widget, unsigned int index) :
    QFrame(parent)
{
  setMouseTracking(true);
  left_mouse_button_was_pressed= 0;
  ancestor_result_grid_widget= result_grid_widget;
  text_edit_frames_index= index;
  is_style_sheet_set_flag= false;
  hide();
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
    /* Actually "> border_size won't happen, if mouse is on frame it's draggable. */
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
      if (ancestor_result_grid_widget->ocelot_result_grid_vertical_copy != 0)
      {
        ancestor_result_grid_widget->frame_resize(text_edit_frames_index, ancestor_grid_column_number, event->x(), height());
        //setFixedSize(event->x(), height());
      }
      else
      {
        /*  Now you must persuade ResultGrid to update all the rows. Beware of multiline rows and header row (row#0). */
        /* Todo: find out why it's "result_row_count + 1" rather than based on gridx_row_count */
        ancestor_result_grid_widget->grid_column_widths[ancestor_grid_column_number]= event->x();
        int xheight;
        for (long unsigned int xrow= 0;
             (xrow < ancestor_result_grid_widget->result_row_count + 1)
             && (xrow < ancestor_result_grid_widget->result_grid_widget_max_height_in_lines);
             ++xrow)
        {
          TextEditFrame *f= ancestor_result_grid_widget->text_edit_frames[xrow * ancestor_result_grid_widget->gridx_column_count + ancestor_grid_column_number];
          if (xrow > 0) xheight= ancestor_result_grid_widget->grid_column_heights[ancestor_grid_column_number];
          if (xrow == 0) xheight= f->height();
          ancestor_result_grid_widget->frame_resize(f->text_edit_frames_index, ancestor_grid_column_number, event->x(), xheight);
          //f->setFixedSize(event->x(), xheight);
        }
      }
    }
  }
  if (widget_side == BOTTOM)
  {
    if (event->y() > minimum_height)
    {
      if (ancestor_result_grid_widget->ocelot_result_grid_vertical_copy > 0)
      {
        ancestor_result_grid_widget->frame_resize(text_edit_frames_index, ancestor_grid_column_number, width(), event->y());
        //setFixedSize(width(), event->y());
      }
      else
      /*
        The following extra 'if' exists because Qt was displaying warnings like
        "QWidget::setMinimumSize: (/TextEditFrame) Negative sizes (-4,45) are not possible"
        when someone grabs the bottom and drags to the left.
        todo: find out why it doesn't seem to happen for ocelot_result_grid_vertical, see above.
      */
      if (event->x() >= minimum_width)
      {
        {
          /* todo: try to remember why you're looking at event->x() when change is to event->y()) */
          /*  Now you must persuade ResultGrid to update all the rows. Beware of multiline rows and header row (row#0). */
          ancestor_result_grid_widget->grid_column_heights[ancestor_grid_column_number]= event->y();
          int xheight;
          for (long unsigned int xrow= 0;
               (xrow < ancestor_result_grid_widget->result_row_count + 1)
               && (xrow < ancestor_result_grid_widget->result_grid_widget_max_height_in_lines);
               ++xrow)
          {
            TextEditFrame *f= ancestor_result_grid_widget->text_edit_frames[xrow * ancestor_result_grid_widget->gridx_column_count + ancestor_grid_column_number];
            if (xrow > 0) xheight= ancestor_result_grid_widget->grid_column_heights[ancestor_grid_column_number];
            if (xrow == 0) xheight= f->height();
            ancestor_result_grid_widget->frame_resize(f->text_edit_frames_index, ancestor_grid_column_number, event->x(), xheight);
            //f->setFixedSize(event->x(), xheight);
          }
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
  This is an event that happens if a result-set grid cell comes into view due to scrolling.
  It can happen multiple times, and it can happen before we're ready to do anything (mysteriously).
  But it's great for handling result sets that have many cells, because some actions are slow.
  For example, setStyleSheet takes 2+ seconds when there are hundreds of child widgets.
  Todo: only apply setStyleSheet if new cell or if style change
*/
void TextEditFrame::paintEvent(QPaintEvent *event)
{
  if (event == 0) return; /* this is just to avoid an "unused parameter" warning */
  if (ancestor_result_grid_widget->is_paintable == 1)
  {
    /*
      Sometimes spurious text_edit_frames show up as if ready to paint.
      Todo: find out if there's a known Qt bug that might explain this.
      It's possible that it no longer happens now that I'm saying "hide()" more often.
    */
    if (text_edit_frames_index >= ancestor_result_grid_widget->max_text_edit_frames_count)
    {
      printf("Trying to paint a texteditframe that isn't in the layout\n");
      printf("  text_edit_frames_index=%d\n", text_edit_frames_index);
      printf("  max_text_edit_frames_count=%d\n", ancestor_result_grid_widget->max_text_edit_frames_count);
    }
    else
    {
      TextEditWidget *text_edit= findChild<TextEditWidget *>();
      if (text_edit != 0)
      {
        if (is_style_sheet_set_flag == false)
        {
          setStyleSheet(ancestor_result_grid_widget->frame_color_setting); /* for drag line color */
          if (cell_type == TEXTEDITFRAME_CELL_TYPE_HEADER) text_edit->setStyleSheet(ancestor_result_grid_widget->copy_of_parent->ocelot_grid_header_style_string);
          else if (cell_type == TEXTEDITFRAME_CELL_TYPE_DETAIL) text_edit->setStyleSheet(ancestor_result_grid_widget->copy_of_parent->ocelot_grid_style_string);
          else text_edit->setStyleSheet(ancestor_result_grid_widget->copy_of_parent->ocelot_extra_rule_1_style_string);
          is_style_sheet_set_flag= true;
        }
        if (is_retrieved_flag == false)
        {
          if (is_image_flag == false)
          {
            if (content_pointer == 0)
            {
              text_edit->setText(QString::fromUtf8(NULL_STRING, sizeof(NULL_STRING) - 1));
            }
            else text_edit->setText(QString::fromUtf8(content_pointer, content_length));
          }
          is_retrieved_flag= true;
        }
      }
    }
  }
}

/*
  TextEditWidget
  This is one of the components of result_grid
*/
TextEditWidget::TextEditWidget(QWidget *parent) :
    QTextEdit(parent)
{
}


TextEditWidget::~TextEditWidget()
{
}

/*
 Finally we're ready to paint a cell inside a frame inside a grid row inside result widget.
 The final decision is: paint as text (default) or paint as image (if blob and if flag).
 If it's a non-blob or if ocelot_blob_flag (?? check this name) is off: default paint.
 If it's a blob and ocelot_blob_flag is on:
   make a pixmap from the contents
   draw the pixmap
   todo: we're allowing resizing (miraculously) but there's no option for scrolling
   todo: consider: is there some better way to decide whether or not to display as image?
*/

void TextEditWidget::paintEvent(QPaintEvent *event)
{
  if (text_edit_frame_of_cell->is_image_flag == false)
  {
    QTextEdit::paintEvent(event);
    return;
  }

  if (text_edit_frame_of_cell->content_pointer == 0)
  {
    setText(QString::fromUtf8(NULL_STRING, sizeof(NULL_STRING) - 1));
    QTextEdit::paintEvent(event);
    return;
  }

  //QPixmap p= QPixmap(QSize(event->rect().width(), event->rect().height()));
  QPixmap p= QPixmap();
  if (p.loadFromData((const uchar*) text_edit_frame_of_cell->content_pointer,
                     text_edit_frame_of_cell->content_length,
                     0,
                     Qt::AutoColor) == false)
  {
    if (text_edit_frame_of_cell->content_pointer != 0)
    {
      setText(QString::fromUtf8(text_edit_frame_of_cell->content_pointer,
                                text_edit_frame_of_cell->content_length));
    }
    QTextEdit::paintEvent(event);
    return;
  }
  QPainter painter(this->viewport());

  /*
    There were choices for QPixmap display. We could have said Qt::IgnoreAspectRatio (the default)
    or Qt::KeepAspectRatioByExpanding. We could have used this->viewport()->size().
    We could have shown a fragment with a scrollbar.
    We could have shown a portion without a scrollbar (one can see the rest by dragging)
    by leaving out the line with the "p.scaled()" function. Maybe users would want such choices?
    Anyway, the line here was wrong:
    p= p.scaled(event->rect().size(), Qt::KeepAspectRatio);
    because event->rect().size() is constantly changing.
    I'm not sure whether this->width(), this->height() might be a few
    pixels too large, but am not seeing noticeable harm.
  */
  p= p.scaled(this->width(), this->height(), Qt::KeepAspectRatio);
  painter.drawPixmap(0, 0, p);
  //painter.drawPixmap(event->rect(), p);
  return;
}


/*
  If the user changes the contents of a cell, that indicates a desire to update the row.
  Make a statement: UPDATE table SET changed-columns WHERE short-columns.
  Re WHERE clause:
    Ideally there is a set of columns in the result set which
    appear in a unique not-null index, such as a primary-key index.
    But we don't check for that! We merely check for values which
    are shorter than 128 bytes (thus avoiding most BLOB searches).
    If we get 50 thousand hits, so be it, the user asked for it.
    Alternative: we could put a LIMIT on.
    Alternative: we could SELECT first, and then UPDATE.
    Alternative: we could UPDATE, then ROLLBACK to savepoint if too many.
    Alternative: we could select from information_schema.statistics.
  TODO: figure out what to do with the string -- is it a hint? does it go direct to statement?
        Perhaps: when the user clicks on a different row, or tries to leave: suggest it.
  TODO: If it's binary or blob, we should do the editing as 0x... rather than '...'.
  TODO: Trail spaces should not matter for char, should matter for binary.
  TODO: I am not distinguishing between NULL and 'NULL'
  TODO: If the SELECT originally had a search-condition X = literal, you could incorporate that
  TODO: If different columns come from different tables, update is impossible, do nothing
*/
#define MAX_WHERE_CLAUSE_LITERAL_SIZE 128   /* arbitrary. maybe should be user-settable. */
void TextEditWidget::keyPressEvent(QKeyEvent *event)
{
  QString content_in_result_set;
  QString content_in_text_edit_widget;
  QString name_in_result_set;
  QString update_statement, where_clause;

  ResultGrid *result_grid= text_edit_frame_of_cell->ancestor_result_grid_widget;
  MainWindow *m= result_grid->copy_of_parent;
  if (m->keypress_shortcut_handler(event, true) == true)
  {
    copy();
    return;
  }
  if (m->keypress_shortcut_handler(event, false) == true) return;
  QString content_in_cell_before_keypress= toPlainText();
  QTextEdit::keyPressEvent(event);
  QString content_in_cell_after_keypress= toPlainText();
  TextEditFrame *text_edit_frame;

  int xrow;
  if ((content_in_cell_before_keypress != content_in_cell_after_keypress)
   && (text_edit_frame_of_cell->cell_type != TEXTEDITFRAME_CELL_TYPE_HEADER)
   && (text_edit_frame_of_cell->is_image_flag == false))
  {
    xrow= text_edit_frame_of_cell->ancestor_grid_result_row_number;

    ++xrow; /* possible bug: should this be done if there's no header row? */
    /* Content has changed since the last keyPressEvent. */
    /* column number = text_edit_frame_of_cell->ancestor_grid_column_number */
    /* length = text_edit_frame_of_cell->content_length */
    /* *content = text_edit_frame_of_cell->content_pointer, which should be 0 for null */

    /* result_grid->text_edit_frames[0] etc. have all the TextEditFrame widgets of the rows */

    /* Go up the line to find first text_edit_frame for the row */
    int text_edit_frame_index_of_first_cell;
    for (text_edit_frame_index_of_first_cell= text_edit_frame_of_cell->text_edit_frames_index;
         text_edit_frame_index_of_first_cell > 0;
         --text_edit_frame_index_of_first_cell)
    {
      int crow= result_grid->text_edit_frames[text_edit_frame_index_of_first_cell]->ancestor_grid_result_row_number + 1;
      if (crow != xrow)
      {
        ++text_edit_frame_index_of_first_cell;
        break;
      }
    }
    update_statement= "";
    where_clause= "";
    char *name_pointer, *table_pointer, *db_pointer;
    unsigned int name_length, table_length, db_length;
    char *field_names_pointer, *org_tables_pointer, *dbs_pointer;
    field_names_pointer= result_grid->result_original_field_names;
    org_tables_pointer= result_grid->result_original_table_names;
    dbs_pointer= result_grid->result_original_database_names;
    unsigned int column_number;
    unsigned int tefi= text_edit_frame_index_of_first_cell;

    for (column_number= 0; column_number < result_grid->result_column_count; )
    {
      text_edit_frame= result_grid->text_edit_frames[tefi];
      if (text_edit_frame->cell_type == TEXTEDITFRAME_CELL_TYPE_HEADER)
      {
        ++tefi;
        continue;
      }

      memcpy(&name_length, field_names_pointer, sizeof(unsigned int));
      field_names_pointer+= sizeof(unsigned int);
      name_pointer= field_names_pointer;
      field_names_pointer+= name_length;

      memcpy(&table_length, org_tables_pointer, sizeof(unsigned int));
      org_tables_pointer+= sizeof(unsigned int);
      table_pointer= org_tables_pointer;
      org_tables_pointer+= table_length;

      memcpy(&db_length, dbs_pointer, sizeof(unsigned int));
      dbs_pointer+= sizeof(unsigned int);
      db_pointer= dbs_pointer;
      dbs_pointer+= db_length;

      /* if in UNION or column-expression, or literal, skip it */
      if ((name_length == 0) || (table_length == 0) || (db_length == 0))
      {
        ++column_number;
        ++tefi;
        continue;
      }

      char *p= text_edit_frame->content_pointer;
      int l;
      if (p == 0) l= 0; /* if content_pointer == 0, that means null */
      else
      {
        l= text_edit_frame->content_length;
        content_in_result_set= QString::fromUtf8(p, l);
      }
      name_in_result_set= QString::fromUtf8(name_pointer, name_length);
      if (l <= MAX_WHERE_CLAUSE_LITERAL_SIZE)
      {
        if (where_clause == "") where_clause= " WHERE ";
        else where_clause.append(" AND ");
        where_clause.append(name_in_result_set);
        if (p == 0) where_clause.append(" IS NULL");
        else
        {
          where_clause.append("=");
          QString s;
          if (result_grid->result_field_types[column_number] <= OCELOT_DATA_TYPE_DOUBLE)
            s= content_in_result_set;
          else s= unstripper(content_in_result_set);
          where_clause.append(s);
        }
      }
      if (text_edit_frame->is_retrieved_flag == true)
      {
        content_in_text_edit_widget= result_grid->text_edit_widgets[tefi]->toPlainText();
        bool contents_changed_flag= true;
        if ((p == 0) && (content_in_text_edit_widget == NULL_STRING)) contents_changed_flag= false;
        else if (content_in_text_edit_widget == content_in_result_set) contents_changed_flag= false;
        if (contents_changed_flag == true)
        {
          if (update_statement == "")
          {
            update_statement= "/* generated */ UPDATE ";
            update_statement.append(QString::fromUtf8(db_pointer, db_length));
            update_statement.append(".");
            update_statement.append(QString::fromUtf8(table_pointer, table_length));
            update_statement.append(" SET ");
          }
          else update_statement.append(",");
          update_statement.append(name_in_result_set);
          update_statement.append('=');
          if ((result_grid->result_field_types[column_number] <= OCELOT_DATA_TYPE_DOUBLE)
          || (content_in_text_edit_widget == NULL_STRING))
            update_statement.append(content_in_text_edit_widget);
          else update_statement.append(unstripper(content_in_text_edit_widget));
        }
      }
      ++column_number;
      ++tefi;
    }

    /* We've got a string. If it's not blank, put it in the statement widget, overwriting. */
    /* It might be blank because user returned to the original values, if so wipe out. */
    CodeEditor *c= m->statement_edit_widget;
    if (update_statement != "")
    {
      update_statement.append(where_clause);
      update_statement.append(";");
      c->setPlainText(update_statement);
    }
    else
    {
      if (c->toPlainText().mid(0, 23) == "/* generated */ UPDATE ") c->setPlainText("");
    }
  }
}


/*
  If the cell has text then we did setText() and QTextEdit::copy() should work okay.
  The reason that we reimplement copy(), and call it from QKeySequence, is:
  for images we do not do setText(). So we (?) have to loadfromData again and
  put in the clipboard as a pixmap.
  This gets the entire unclipped image because we're reloading from the source.
  Todo: check for nulls.
  Todo: decide whether we care about select() before copy().
        If it's an image, Copy is enabled even if there's no selection.
  Todo: although ^C seems okay, right-click and select copy does not seem okay.
  Todo: With Qt4, when program ends, if there was a copy, we might see
        "QClipboard: Unable to receive an event from the clipboard manager in a reasonable time"
        which might be like https://bugreports.qt.io/browse/QTBUG-32853.
*/
void TextEditWidget::copy()
{
  if ((text_edit_frame_of_cell->is_image_flag == true)
  &&  (text_edit_frame_of_cell->content_pointer != 0))
  {
    QPixmap p= QPixmap();
    if (p.loadFromData((const uchar*) text_edit_frame_of_cell->content_pointer,
                       text_edit_frame_of_cell->content_length,
                       0,
                       Qt::AutoColor) == false)
    {
      /* Not readable as an image. Maybe text. Maybe null. */
      QClipboard *p_clipboard= QApplication::clipboard();
      p_clipboard->setText(QString::fromUtf8(text_edit_frame_of_cell->content_pointer,
                           text_edit_frame_of_cell->content_length));
    }
    else
    {
      QClipboard *p_clipboard= QApplication::clipboard();
      p_clipboard->setPixmap(p);
    }
  }
  else QTextEdit::copy();
}


/* Add ' at start and end of a string. Change ' to '' within string. Compare connect_stripper(). */
/* mysql_hex_string() might be useful for some column types here */
QString TextEditWidget::unstripper(QString value_to_unstrip)
{
  QString s;
  QString c;

  s= "'";
  for (int i= 0; i < value_to_unstrip.count(); ++i)
  {
    c= value_to_unstrip.mid(i, 1);
    s.append(c);
    if (c == "'") s.append(c);
  }
  s.append("'");
  return s;
}

void TextEditWidget::focusOutEvent(QFocusEvent *event)
{
  TextEditFrame *t= text_edit_frame_of_cell;
  ResultGrid *r=  t->ancestor_result_grid_widget;
  MainWindow *m= r->copy_of_parent;
  m->menu_activations(this, QEvent::FocusOut);
  /* We probably don't need to say this. */
  QTextEdit::focusOutEvent(event);
}

void TextEditWidget::focusInEvent(QFocusEvent *event)
{
  TextEditFrame *t= text_edit_frame_of_cell;
  ResultGrid *r=  t->ancestor_result_grid_widget;
  MainWindow *m= r->copy_of_parent;
  m->menu_activations(this, QEvent::FocusIn);
  /* We probably don't need to say this. */
  QTextEdit::focusInEvent(event);
}




/*
  CONNECT
*/

/*
   MySQL options
   Any mysql/mariadb client should have a consistent way to see what
   options the user has put in a configuration file such as my.cnf, or added
   on the command line with phrases such as --port=x.
   MySQL's own clients can do it with an include of getopt.h and calls to my_long_options etc.
   but that would introduce some unwanted dependencies, so we do all the option retrievals directly.
   Todo: meld that with whatever a user might say in a CONNECT command line
         or maybe even a dialog box.
   Assume lmysql->ldbms_mysql_init() has not already happened.
   Qt gets to see argc+argv first, and Qt will process options that it recognizes
   such as -style, -session, -graphicssystem. See
   http://qt-project.org/doc/qt-4.8/qapplication.html#details.
   But when Qt is done, it should remove what it parsed.
   Todo: actually you should operate on a copy of argc + argv, rather than change the originals. QT docs say so.
         but that would contradict another todo, which is to blank the password if it's in an argv
   Most options are ignored but the ones which might be essential for connecting are not ignored.
   Example: if ~/.my.cnf has "port=3306" in [clients] group, and start happens with --host=127.0.0.1,
            then port=3306 and current_host=127.0.0.1 and that will get passed to the connect routine.
   Read: http://dev.mysql.com/doc/refman/5.6/en/connecting.html
         See also http://dev.mysql.com/doc/refman/5.6/en/mysql-command-options.html
 */

/*
  Connect -- get options

  Environment variables, then option files, then command-line arguments

  Except that you have to read the command-line arguments to know whether
  you should read the option files, see http://dev.mysql.com/doc/refman/5.7/en/option-file-options.html

  For environment variables:
  Follow http://dev.mysql.com/doc/refman/5.7/en/environment-variables.html
  * Watch: LD_RUN_PATH MYSQL_GROUP_SUFFIX MYSQL_HOST MYSQL_PS1 MYSQL_PWD MYSQL_TCP_PORT TZ USER
           MYSQL_UNIX_PORT

  For option files:
  Follow MySQL manual http://dev.mysql.com/doc/refman/5.7/en/option-files.html
  Don't follow MariaDB documentation https://mariadb.com/kb/en/mariadb/mariadb-documentation/getting-started/getting-installing-and-upgrading-mariadb/configuring-mariadb-with-mycnf/
  Todo: Find out whether MariaDB documentation is merely wrong, or represents a real incompatibility with MySQL.
  * Read these files:
  /etc/my.cnf
  /etc/mysql/my.cnf
  SYSCONFDIR/my.cnf       ?? i.e. [installation-directory]/etc/my.cnf but this should be changeable
  $MYSQL_HOME/my.cnf
  file specified with --defaults-extra-file
  $HOME/.my.cnf             MySQL manual says ~/.my.cnf which isn't necessarily the same thing, but I don't believe that
  $HOME/.mylogin.cnf        MySQL manual says ~/.mylogin.cnf which isn't necessarily the same thing, but I don't believe that
  ... There is no use of DATADIR, which is deprecated, and (I think) would only relate to server anyway.
  On Unix, "ignore configuration files that are world-writable" -- we use stat.h for this.
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

  Todo: whenever you have a token0/token1/token2:
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

//#include <dirent.h>

void connect_set_variable(QString token0, QString token2);
void connect_read_command_line(int argc, char *argv[]);
void connect_read_my_cnf(const char *file_name, int is_mylogin_cnf);

//#include <pwd.h>
//#include <unistd.h>

void MainWindow::connect_mysql_options_2(int argc, char *argv[])
{
  char *mysql_pwd;
  const char *home;
  char *ld_run_path;

  /*
    Todo: check: are there any default values to be set before looking at environment variables?
          I don't see documentation to that effect, so I'm just setting them to "" or 0.
          Except that port initially = MYSQL_PORT (probably 3306), and ocelot_delimiter_str initially = ";"
          and http://dev.mysql.com/doc/refman/5.6/en/connecting.html mentions a few others:
          host = 'localhost' (which means protocol=SOCKET if mysql client, but we ignore that)
          user = Unix login name on Linux, although on Windows it would be 'ODBC'
          and there seem to be some getenv() calls in other clients that I didn't take into account.
  */
  ocelot_no_defaults= 0;
  ocelot_defaults_file= "";
  ocelot_defaults_extra_file= "";

  ocelot_host= "localhost";
  ocelot_database= "";
  ocelot_user= "";
  ocelot_password_was_specified= 0;
  /* ocelot_port= MYSQL_PORT; */ /* already initialized */
  ocelot_unix_socket= "";
  ocelot_default_auth= "";
  ocelot_defaults_group_suffix= "";
  /* ocelot_enable_cleartext_plugin= 0; */ /* already initialized */
  ocelot_history_hist_file_name= "/.mysql_history";
  ocelot_histignore= "";
  ocelot_init_command= "";
  ocelot_opt_bind= "";
  /* ocelot_opt_can_handle_expired_passwords= 0; */ /* already initialized */
  /* ocelot_opt_compress= 0; */ /* already initialized */
  ocelot_opt_connect_attr_delete= "";
  /* ocelot_opt_connect_attr_reset= 0; */ /* already initialized */
  /* ocelot_opt_connect_timeout= 0; */ /* ocelot_opt_connect_timeout is initialized to 0 */
  /* ocelot_opt_local_infile= 0; */ /* ocelot_opt_local_infile is initialized to 0 */
  /* ocelot_opt_named_pipe= 0; */ /* already initialized */
  /* ocelot_opt_read_timeout= 0; */ /* already initialized */
  /* ocelot_opt_reconnect= 0; */ /* ocelot_opt_reconnect is initialized to 0 */
  ocelot_opt_ssl= "";
  ocelot_opt_ssl_ca= "";
  ocelot_opt_ssl_capath= "";
  ocelot_opt_ssl_cert= "";
  ocelot_opt_ssl_cipher= "";
  ocelot_opt_ssl_crl= "";
  ocelot_opt_ssl_crlpath= "";
  ocelot_opt_ssl_key= "";
  ocelot_opt_ssl_mode= "";
  /* ocelot_opt_ssl_verify_server_cert= 0; */ /* already initialized */
  /* ocelot_opt_use_result= 0; */ /* already initialized */
  /* ocelot_opt_write_timeout= 0; */ /* already initialized */
  ocelot_plugin_dir= "";
  ocelot_read_default_file= "";
  ocelot_read_default_group= "";
  /* ocelot_report_data_truncation= 0; */ /* already initialized */
  /* ocelot_secure_auth= 1; secure_auth is 1 by default anyway */ /* =1 if mysql 5.6.7 */
  ocelot_server_public_key= "";
  ocelot_set_charset_dir= "";
  ocelot_set_charset_name= "utf8"; /* ocelot_default_character_set= "";  */
  ocelot_shared_memory_base_name= "";

  ocelot_protocol= ""; ocelot_protocol_as_int= get_ocelot_protocol_as_int(ocelot_protocol);

  ocelot_delimiter_str= ";";
  /* ocelot_history_includes_warnings= 0; is default anyway */
  /* ocelot_sigint_ignore= 0; is default anyway */
  /* ocelot_safe_updates= 0; */ /* ocelot_safe_updates is initialized to 0 */
  /* ocelot_select_limit= 0; */ /* ocelot_select_limit is initialized to 0 */
  /* ocelot_max_join_size= 0; */ /* ocelot_max_join_size is initialized to 0 */
  /* ocelot_silent= 0; */ /* ocelot_silent is initialized to 0 */
  /* ocelot_no_beep= 0; */ /* ocelot_no_beep is initialized to 0 */
  /* ocelot_wait= 0; *//* ocelot_wait is initialized to 0 */

  ocelot_bind_address= "";
  ocelot_debug= "";
  ocelot_execute= "";
  ocelot_ld_run_path= "";
  ocelot_login_path= "";
  ocelot_pager= "";
  //ocelot_prompt= "mysql>";                  /* Todo: change to "\N [\d]>"? */

  options_files_read= "";
#ifdef OCELOT_OS_LINUX
  {
    struct passwd *pw;
    uid_t u;
    u= geteuid();
    pw= getpwuid(u);
    if (pw != NULL) ocelot_user= pw->pw_name;
  }
#endif
  connect_read_command_line(argc, argv);               /* We're doing this twice, the first time won't count. */

  /*
    ocelotgui.pro variables
    Looking at https://dev.mysql.com/doc/refman/5.6/en/source-configuration-options.html
    I gather that some items can be defined at cmake time. The possibly interesting ones
    that we could do in ocelotgui.pro are, for example:
    DEFINES += MYSQL_TCP_PORT=3306
    DEFINES += MYSQL_UNIX_ADDR="/tmp/mysql.sock"
    DEFINES += SYSCONFDIR="..."
    which in cmake terms is -DMYSQL_TCP_PORT=3306 -DMYSQL_UNIX_ADDR="/tmp/mysql.sock" etc.
    MySQL_UNIX_ADDR must go through stringification.
    Following shows how we'd do stringification, but then we don't do anything with it.
    Warning: The stringification might be adding extra "s.
    Warning: MYSQL_UNIX_ADDR has a definition in a mysql.h-related include file.
    Todo: pay attention to these settings.
  */
//#define xstr(a) str(a)
//#define str(a) #a
//#ifdef MYSQL_UNIX_ADDR
//  printf("Ignoring MYSQL_UNIX_ADDR=%s.\n", xstr(MYSQL_UNIX_ADDR));
//#endif

  /* Environment variables */

  if (getenv("HOME") != 0)
  {
    home= getenv("HOME");
  }
  else home= "";

  if (getenv("LD_RUN_PATH") != 0)
  {
    ld_run_path= getenv("LD_RUN_PATH");                  /* maybe used to find libmysqlclient */
    ocelot_ld_run_path= ld_run_path;
  }

  if (getenv("MYSQL_GROUP_SUFFIX") != 0)
  {
    char *tmp_ocelot_defaults_group_suffix;
    tmp_ocelot_defaults_group_suffix= getenv("MYSQL_GROUP_SUFFIX");
    ocelot_defaults_group_suffix= tmp_ocelot_defaults_group_suffix;
  }

  //getenv("MYSQL_HOME");                                /* skip, this is only for server */

  if (getenv("MYSQL_HISTFILE") != 0)
  {
    char *tmp_ocelot_histfile;
    tmp_ocelot_histfile= getenv("MYSQL_HISTFILE");
    ocelot_history_hist_file_name= tmp_ocelot_histfile;
  }
  else ocelot_history_hist_file_name= home + ocelot_history_hist_file_name;

  if (getenv("MYSQL_HISTIGNORE") != 0)
  {
    char *tmp_ocelot_histignore;
    tmp_ocelot_histignore= getenv("MYSQL_HISTIGNORE");
    ocelot_histignore= tmp_ocelot_histignore;
  }

  if (getenv("MYSQL_HOST") != 0)
  {
    char *tmp_ocelot_host;
    tmp_ocelot_host= getenv("MYSQL_HOST");
    ocelot_host= tmp_ocelot_host;
  }

  if (getenv("MYSQL_PS1") != 0)
  {
    char *tmp_ocelot_prompt;
    tmp_ocelot_prompt= getenv("MYSQL_PS1");
    ocelot_prompt= tmp_ocelot_prompt;
    ocelot_prompt_is_default= false;
  }

  if (getenv("MYSQL_PWD") != 0)
  {
    mysql_pwd= getenv("MYSQL_PWD");
    ocelot_password= mysql_pwd;
    ocelot_password_was_specified= 1;
  }
  if (getenv("MYSQL_TCP_PORT") != 0) ocelot_port= atoi(getenv("MYSQL_TCP_PORT"));         /* "" */
  //user= getenv("USER"); no, this is for Windows
  //tz= getenv("TZ");

  if (getenv("MYSQL_UNIX_PORT") != 0)
  {
    char *tmp_ocelot_unix_socket;
    tmp_ocelot_unix_socket= getenv("MYSQL_UNIX_PORT");
    ocelot_unix_socket= tmp_ocelot_unix_socket;
  }

  /*
    Options files i.e. Configuration files i.e. my_cnf files
    Don't read option files if ocelot_no_defaults==1 (which is true if --no-defaults was specified on command line).
    For Tarantool, we look in the same places as for MariaDB.
    MariaDB's mysql client wouldn't read .mylogin.cnf, but we do.
    Todo: check: does MariaDB read mylogin.cnf even if ocelot_no_defaults==1?
    Todo: put mycnf_file list somewhere where ocelotgui --help can see
  */
  if (QString::compare(ocelot_defaults_file, " ") > 0)
  {
    char tmp_my_cnf[1024];                         /* file name = $HOME/.my.cnf or $HOME/.mylogin.cnf or defaults_extra_file */
    strcpy(tmp_my_cnf, ocelot_defaults_file.toUtf8());
    connect_read_my_cnf(tmp_my_cnf, 0);
  }
  else
  {
    if (ocelot_no_defaults == 0)
    {
      char my_cnf_file[10][1024];
      int i= 0;
#ifdef OCELOT_OS_LINUX
      strcpy(my_cnf_file[i++], "/etc/my.cnf");
      strcpy(my_cnf_file[i++], "/etc/mysql/my.cnf");
      /* todo: think: is argv[0] what you want for SYSCONFDIR? not exact, but it's where the program is now. no, it might be a copy. */
      // connect_read_my_cnf("SYSCONFDIR/etc/my.cnf", 0) /* ?? i.e. [installation-directory]/etc/my.cnf but this should be changeable */
      /* skip $MYSQL_HOME/my.cnf, only server stuff should be in it */
      // connect_read_my_cnf("file specified with --defaults-extra-file", 0);
      if (QString::compare(ocelot_defaults_extra_file, " ") > 0)
      {
        strcpy(my_cnf_file[i++], ocelot_defaults_extra_file.toUtf8());
      }
      strcpy(my_cnf_file[i], home);
      strcat(my_cnf_file[i++], "/.my.cnf");
      if (getenv("MYSQL_TEST_LOGIN_FILE") != NULL)
      {
        strcpy(my_cnf_file[i++], getenv("MYSQL_TEST_LOGIN_FILE"));
      }
      else
      {
        strcpy(my_cnf_file[i], home);          /* $HOME/.mylogin.cnf */
        strcat(my_cnf_file[i++], "/.mylogin.cnf");
      }
#else
      /* Todo: %PROGRAMDATA%... (MySQL) */
      if ((hparse_dbms_mask & (FLAG_VERSION_MARIADB_ALL|FLAG_VERSION_TARANTOOL)) != 0)
      {
        if (getenv("SYSTEM") != 0)
        {
          strcpy(my_cnf_file[i], getenv("SYSTEM"));
          strcat(my_cnf_file[i++], "\\my.ini");
          strcpy(my_cnf_file[i], getenv("SYSTEM"));
          strcat(my_cnf_file[i++], "\\my.cnf");
        }
      }
      if (getenv("WINDIR") != 0)
      {
        strcpy(my_cnf_file[i], getenv("WINDIR")); /* e.g. c:\windows */
        strcat(my_cnf_file[i++], "\\my.ini");
        strcpy(my_cnf_file[i], getenv("WINDIR"));
        strcat(my_cnf_file[i++], "\\my.cnf");
      }
      strcpy(my_cnf_file[i++], "c:\\my.ini");
      strcpy(my_cnf_file[i++], "c:\\my.cnf");
      if ((hparse_dbms_mask & FLAG_VERSION_MYSQL_ALL) != 0)
      {
        /* Todo: This should be BASEDIR | INSTALLDIR. Close enough? */
        strcpy(my_cnf_file[i], QCoreApplication::applicationDirPath().toUtf8());
        strcat(my_cnf_file[i++], "\\my.ini");
        strcpy(my_cnf_file[i], QCoreApplication::applicationDirPath().toUtf8());
        strcat(my_cnf_file[i++], "\\my.cnf");
      }
      if ((hparse_dbms_mask & (FLAG_VERSION_MARIADB_ALL|FLAG_VERSION_TARANTOOL)) != 0)
      {
        if (getenv("MYSQL_HOME") != 0)
        {
          strcpy(my_cnf_file[i], getenv("MYSQL_HOME"));
          strcat(my_cnf_file[i++], "\\my.ini");
          strcpy(my_cnf_file[i], getenv("MYSQL_HOME"));
          strcat(my_cnf_file[i++], "\\my.cnf");
        }
      }
      if (QString::compare(ocelot_defaults_extra_file, " ") > 0)
      {
        strcpy(my_cnf_file[i++], ocelot_defaults_extra_file.toUtf8());
      }
      if (getenv("APPDATA") != 0)
      {
        strcpy(my_cnf_file[i], getenv("APPDATA"));
        strcat(my_cnf_file[i], "\\MYSQL");
        strcat(my_cnf_file[i++], "\\.mylogin.cnf");
      }
#endif
      for (int j= 0; j < i; ++j)
      {
        if (strstr(my_cnf_file[j], ".mylogin.cnf") != 0)
          connect_read_my_cnf(my_cnf_file[j], 1);
        else connect_read_my_cnf(my_cnf_file[j], 0);
      }
    }
  }
  connect_read_command_line(argc, argv);

  if (ocelot_prompt_is_default == true)
  {
    ocelot_prompt= ocelot_dbms;
    ocelot_prompt.append(">");
  }

  //connect_make_statement();
}


/*
  Command line arguments i.e. argc + argv
  Some tokenizing has already been done.
  For example progname --a=b --c=d gives us argv[1]="--a=b" and argv[2]="--c=d".
  For example progname -a b -c d gives us argv[1]="-a" argv[2]="b" argv[3]="-c" argv[4]="d".
  If something is enclosed in single or double quotes, then it has already been stripped of quotes and lead/trail spaces.
  We do command line arguments TWICE! first time is just to find out what my.cnf files should be read, etc.
  second time is to override whatever happened during getenv and option-file processing
  todo: parse so -p x and --port=x etc. are known
  todo: check: is it okay still to abbreviate e.g. us rather than user?
  todo: strip the arguments so Qt doesn't see them, or maybe don't
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
    /* If there is one '-' then token0=argv[i], maybe token1='=', maybe token2=argv[i+1 */
    /* Some items don't need spaces before argument, e.g. -D db_name is same as -Ddb_name */
    else if ((strlen(argv[i]) > 1) && (s_argv.mid(1, 1) != "-"))
    {
      bool is_token2_needed= false;
      token0= argv[i]; token1= ""; token2= "";
      if (token0.size() > 2)
      {
        token1= "=";
        token2= token0.right(token0.size() - 2);
        token0= token0.left(2);
      }
      if (token0 == "-?") token0= "help";
      if (token0 == "-#") token0= "debug";
      if (token0 == "-A") token0= "no_auto_rehash";
      if (token0 == "-B") token0= "batch";
      if (token0 == "-b") token0= "no_beep";
      if (token0 == "-C") token0= "compress";
      if (token0 == "-c") token0= "comments";
      if (token0 == "-D") { token0= "database"; is_token2_needed= true; }
      if (token0 == "-E") token0= "vertical";
      if (token0 == "-e") { token0= "execute"; is_token2_needed= true; }
      if (token0 == "-f") token0= "force";
      if (token0 == "-G") token0= "named_commands";
      if (token0 == "-H") token0= "html";
      if (token0 == "-h") { token0= "host"; is_token2_needed= true; }
      if (token0 == "-i") token0= "ignore_spaces";
      if (token0 == "-j") token0= "syslog";
      if (token0 == "-L") token0= "skip_line_numbers";
      /* Somewhere I think I saw "l login-path" but it doesn't work with mysql client. */
      if (token0 == "-N") token0= "skip_column_names";
      if (token0 == "-n") token0= "unbuffered";
      if (token0 == "-o") token0= "one_database";
      if (token0 == "-P") { token0= "port"; is_token2_needed= true; }
      if (token0 == "-p") token0= "password";
      if (token0 == "-q") token0= "quick";
      if (token0 == "-r") token0= "raw";
      if (token0 == "-S") { token0= "socket"; is_token2_needed= true; }
      if (token0 == "-s") token0= "silent";
      if (token0 == "-T") token0= "debug_info";
      if (token0 == "-t") token0= "table";
      if (token0 == "-U") token0= "safe_updates";
      if (token0 == "-u") { token0= "user"; is_token2_needed= true; }
      if (token0 == "-V") token0= "version";
      if (token0 == "-v") token0= "verbose";
      if (token0 == "-W") token0= "pipe";
      if (token0 == "-w") token0= "wait";
      if (token0 == "-X") token0= "xml";
      if ((i < argc - 1) && (is_token2_needed == true) && (token2 == ""))
      {
        token1= "="; token2= argv[i + 1];
        ++i;
      }
    }
    /* If there are two '-'s then token0=argv[i] left, token1='=', token2=argv[i] right */
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


/* todo: check if we've already looked at the file (this is possible if !include or !includedir happens)
         if so, skip */
/* todo: this might be okay for a Linux file that ends lines with \n, but what about Windows? */
void MainWindow::connect_read_my_cnf(const char *file_name, int is_mylogin_cnf)
{
  //FILE *file;
  char line[2048];
  int token_offsets[100]; /* actually 10 is enough, currently */
  int token_lengths[100];
  int i;
  QString token0, token1, token2, token_for_value;
  QString group;                                         /* what was in the last [...] e.g. mysqld, client, mysql, ocelot */
  int token0_length, token1_length, token2_length;
  unsigned char output_buffer[65536];                    /* todo: should be dynamic size */

  group= "";                                             /* group identifier doesn't carry over from last .cnf file that we read */
#ifdef OCELOT_OS_LINUX
  struct stat st;
  if (stat(file_name, &st) == 0)
  {
    if ((st.st_mode & S_IWOTH) == S_IWOTH)
    {
      printf("Ignoring world-writable configuration file %s.\n", file_name);
      return;
    }
  }
#endif
  QFile file(file_name);
  /* 2016-04-20 removed QIODevice::Text */
  bool open_result= file.open(QIODevice::ReadOnly);
  if (open_result == false)
  {
    return;                                              /* (if file doesn't exist, ok, no error */
  }
  //file= fopen(file_name, "r");                           /* Open specified file, read only */
  //if (file == NULL) return;                              /* (if file doesn't exist, ok, no error */
  if (is_mylogin_cnf == 1)
  {
      /*
        todo: close file when return, including return for error
      */
      /*
        connect_readmylogin() is a variation of readmylogin.c
        AES_KEY and AES_BLOCK_SIZE are defined in ocelotgui.h
        If openSSL is not available, ignore it (mysql would always read because YaSSL is bundled).
        If .mylogin.cnf is not an encrypted file, ignore it (same as what mysql would do).
      */
      //int MainWindow::connect_readmylogin(QFile &file, unsigned char *output_buffer)
      {
        QString ldbms_return_string;

        ldbms_return_string= "";

        /* First find libcrypto.so */
        if (is_libcrypto_loaded != 1)
        {
          lmysql->ldbms_get_library(ocelot_ld_run_path, &is_libcrypto_loaded, &libcrypto_handle, &ldbms_return_string, WHICH_LIBRARY_LIBCRYPTO);
        }
        if (is_libcrypto_loaded != 1)
        {
          lmysql->ldbms_get_library("", &is_libcrypto_loaded, &libcrypto_handle, &ldbms_return_string, WHICH_LIBRARY_LIBCRYPTO);
        }
        if (is_libcrypto_loaded != 1)
        {
          file.close(); return; /* if decryption fails, ignore */
        }
        unsigned char cipher_chunk[4096];
        unsigned int cipher_chunk_length, output_length= 0, i;
        unsigned char key_in_file[20];
        unsigned char key_after_xor[AES_BLOCK_SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        AES_KEY key_for_aes;

        if (file.seek(4) == false)
        //if (fseek(file, 4, SEEK_SET) != 0)
        {
          file.close(); return; /* if decryption fails, ignore */
        }
        if (file.read((char*)key_in_file, 20) != 20)
        //if (fread(key_in_file, 1, 20, file) != 20)
        {
          file.close(); return; /* if decryption fails, ignore */
        }

        for (i= 0; i < 20; ++i) *(key_after_xor + (i%16))^= *(key_in_file + i);
        lmysql->ldbms_AES_set_decrypt_key(key_after_xor, 128, &key_for_aes);

        while (file.read((char*)&cipher_chunk_length, 4) == 4)
        //while (fread(&cipher_chunk_length, 1, 4, file) == 4)
        {
          if (cipher_chunk_length > sizeof(cipher_chunk))
          {
            file.close(); return; /* if decryption fails, ignore */
          }
          if (file.read((char*)cipher_chunk, cipher_chunk_length) != cipher_chunk_length)
          //if (fread(cipher_chunk, 1, cipher_chunk_length, file) != cipher_chunk_length)
          {
            file.close(); return; /* if decryption fails, ignore */
          }
          for (i= 0; i < cipher_chunk_length; i+= AES_BLOCK_SIZE)
          {
            lmysql->ldbms_AES_decrypt(cipher_chunk+i, output_buffer+output_length, &key_for_aes);
            output_length+= AES_BLOCK_SIZE;
            while ((output_length > 0) && (*(output_buffer+(output_length-1)) < ' ') && (*(output_buffer+(output_length-1)) != '\n')) --output_length;
          }
        }
        *(output_buffer + output_length)= '\0';
        //return 0;
      }
  }
  options_files_read.append(file_name); options_files_read.append(" ");
  char *fgets_result;
  int file_offset= 0;
  int line_offset= 0;
  for (;;)
  {
    if (is_mylogin_cnf == 0)
    {
      if (file.atEnd() == true) {fgets_result= NULL; }
      else
      {
        QByteArray qbline;
        qbline= file.readLine((sizeof line) - 1);
        int ii;
        for (ii= 0; ii < qbline.count(); ++ii) line[ii]= qbline[ii];
        line[ii]= '\0';
        //line= file.readLine(sizeof line);
        fgets_result=&line[0];
      }
      //fgets_result= fgets(line, sizeof line, file);
    }
    else
    {
      for (line_offset= 0; *(output_buffer + file_offset) != '\0'; ++line_offset, ++file_offset)
      {
        *(line + line_offset)= *(output_buffer + file_offset);
        if (*(line + line_offset) == '\n') { ++line_offset; ++file_offset; break; }
      }
      *(line + line_offset)= '\0';
      if (line_offset > 0) fgets_result= (char*) output_buffer;
      else fgets_result= NULL;
    }
    if (fgets_result == NULL) break;
    QString s= line;
    /* tokenize, ignore # comments or / * comments * /, treat '-' as part of token not operator */
    tokenize(s.data(),
             s.size(),
             &token_lengths[0], &token_offsets[0], 100 - 1,
            (QChar*)"33333", 2, "", 2);
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
      connect_read_my_cnf(new_file_name, 0);
    }
    ///* See if it's !includedir */
    //if ((QString::compare(token0, "!") == 0) && (QString::compare(token1, "includedir", Qt::CaseInsensitive) == 0))
    //{
    //  DIR *d;
    //  struct dirent *dir;
    //  char new_directory_name[2048];
    //  strcpy(new_directory_name,token2.toUtf8());
    //  *(new_directory_name + token2_length)= 0;
    //  d= opendir(new_directory_name);
    //  if (d)
    //  {
    //    while ((dir = readdir(d)) != NULL)
    //    {
    //      if ((strlen(dir->d_name)>4) && (strcmp(dir->d_name + strlen(dir->d_name) - 4, ".cnf") == 0))
    //      {
    //        char new_file_name[2048];
    //        strcpy(new_file_name, new_directory_name);
    //        strcat(new_file_name, "/");
    //        strcat(new_file_name, dir->d_name);
    //        connect_read_my_cnf(new_file_name, 0);
    //      }
    //    }
    //    closedir(d);
    //  }
    //}
    /* See if it's !includedir */
    /* Todo: there are no checks for looping; not sure what to do with hidden or symlink */
    if ((QString::compare(token0, "!") == 0) && (QString::compare(token1, "includedir", Qt::CaseInsensitive) == 0))
    {
      QDir dir(token2);
      dir.setFilter(QDir::Files | QDir::Hidden);
      QFileInfoList list= dir.entryInfoList();
      for (int i= 0; i < list.size(); ++i)
      {
        QFileInfo fileInfo= list.at(i);
#ifdef Q_OS_LINUX
        QString file_name= fileInfo.fileName();
        if (file_name.right(4) == ".cnf")
#endif
#ifdef Q_OS_WIN32
        QString file_name= fileInfo.fileName().toUpper();
        if ((file_name.right(4) == ".CNF") || (file_name.right(4) == ".INI"))
#endif
        {
          QString s= token2;
          s.append("/");
          s.append(fileInfo.fileName());
          char new_file_name[2048];
          strcpy(new_file_name, s.toUtf8());
          *(new_file_name + s.length())= '\0';
          connect_read_my_cnf(new_file_name, 0);
        }
      }
    }
    /* See if it's [ group ] */
    if ((QString::compare(token0, "[") == 0) && (QString::compare(token2,"]") == 0))
    {
      group= token1;
      continue;
    }
    if ((is_mylogin_cnf == 1) && (QString::compare(group, ocelot_login_path, Qt::CaseInsensitive) == 0))
    {
      /* it's in .mylogin.cnf and the group matches the specified login path */
    }
    else
    {
      /* Skip if it's not one of the groups that we care about i.e. client or mysql or ocelot */
      /* Todo: Consider: should our group be "ocelot", "ocelotgui", or both? */
      if ((QString::compare(group, "client", Qt::CaseInsensitive) != 0)
      &&  (QString::compare(group, "mysql", Qt::CaseInsensitive) != 0)
      &&  (QString::compare(group, "ocelot", Qt::CaseInsensitive) != 0)
      &&  (QString::compare(group, "client" + ocelot_defaults_group_suffix, Qt::CaseInsensitive) != 0)
      &&  (QString::compare(group, "mysql" + ocelot_defaults_group_suffix, Qt::CaseInsensitive) != 0)
      &&  (QString::compare(group, "ocelot" + ocelot_defaults_group_suffix, Qt::CaseInsensitive) != 0)) continue;
    }

    /* Remove ''s or ""s around the value, then strip lead or trail spaces. */
    token2= connect_stripper(token2, true);
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
  file.close();
  //fclose(file);
}


/*
  Remove ''s or ""s or ``s around a QString, then remove lead or trail spaces.
  Called for connect, and also for things like USE `test`.
  I didn't say remove lead or trail whitespace, so QString "trimmed()" is no good.
  Todo: This will also change 'xxx''yyy' to 'xxx'yyy', I'm not sure that's always good.
  todo: I'm not sure that `` (tildes) should be removed for my.cnf values, check that
        (we are depending on stripping of ``s when we call connect_stripper() with debuggee information status values)
  todo: I am fairly sure that I need to call this from other places too.
  Todo: strip_doublets_flag, i.e. change 'xxx''yyy' to xxx'yyy' etc., is currently only true
        for condition_1 and for command-line options. Maybe it should be more, maybe less.
*/
QString MainWindow::connect_stripper(QString value_to_strip, bool strip_doublets_flag)
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
      QString strippable_char= s.mid(0, 1);                    /* strippable_char = " or ' or ` */
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
      if (strip_doublets_flag == true)
      {
        QString s2= "";
        for (int i= 0; i < s.size(); ++i)
        {
          QString c= s.mid(i, 1);
          s2.append(c);
          if ((c == strippable_char) && (i < (s.size() - 1)) && (c == s.mid(i + 1, 1))) ++i;
        }
        return s2;
      }
    }
  }
  return s;
}

/* Add ' at start and end of a string.
   Change ' to '' within string. Compare connect_stripper().
   with string literals inside single quotes. Need doublets.
   Todo: This is the same code as TextEditWidget::unstripper(),
         see whether you can reduce the duplication.
   Todo: Consider:
         if (co.contains("''")) co= co.replace("''", "'");
         Would surely be faster?
   Todo: We also have something in debug_debug_go for changing ' to '',
         maybe there's code duplication
   Todo: Maybe something in setup_generate_routine_entry_parameter
         has something similar too, maybe there's code duplication.
   Todo: debugger routines should call this when we generate statements
         but so far it's not happening
*/
QString MainWindow::connect_unstripper(QString value_to_unstrip)
{
  QString s;
  QString c;

  s= "'";
  for (int i= 0; i < value_to_unstrip.count(); ++i)
  {
    c= value_to_unstrip.mid(i, 1);
    s.append(c);
    if (c == "'") s.append(c);
  }
  s.append("'");
  return s;
}



/*
  Given token0=option-name [token1=equal-sign token2=value],
  see if option-name corresponds to one of your program-variables,
  and if so set program-variable = true or program-variable = value.
  For example, if token0="user", token1="=", token2="peter",
  then set ocelot_user = "peter".
  But that would be too simple, eh? So here are some complications:
  * unambiguous prefixes of option names are allowed until MySQL 5.7
    (this is true for options but not true for values)
    http://dev.mysql.com/doc/refman/5.6/en/program-options.html
  * '-' and '_' are interchangeable
    http://dev.mysql.com/doc/refman/5.6/en/command-line-options.html
  * if the target is numeric and ends with K etc., multiply by 1024 etc.
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
  connect_timeout ocelot_opt_connect_timeout
  compress ocelot_opt_compress
  secure_auth ocelot_secure_auth
  local_infile ocelot_opt_local_infile
  safe_updates or i_am_a_dummy ocelot_safe_updates
  plugin_dir ocelot_plugin_dir
  select_limit ocelot_select_limit
  max_join_size ocelot_max_join_size
  silent ocelot_silent
  no_beep ocelot_no_beep
  wait ocelot_wait
  default-character-set ocelot_default_character_set
*/
/*
  Re undocumented behaviour with option modifiers
  The rules in http://dev.mysql.com/doc/refman/5.7/en/option-modifiers.html
  "Program Option Modifiers" are weird already. To make it weirder, here's
  a partial description of undocumented behaviour of mysql client.
  For booleans:
  ! If first letter of value = '0' it's 0, if first letter of value = '1' it's 1
  ! --enable-disable-show-warnings is OK, the last prefix decides
  ! If enable|disable|skip was stated, and [= value] is stated, then
    If value = 0, then it reverses whatever enable|disable is in place so far
      Else [= value] clause is ignored
    Otherwise
      If value is blank or invalid, ignore the whole specification and warn
  For non-booleans:
  ! If we say --skip-port=5, then port = 0 i.e. override previous setting
  ! If we say --skip-delimiter=x then delimiter = ; i.e. return to default
  ! If we say --skip-socket=x then socket='0' i.e. override previous setting
  I try to follow some of this but not all.
*/
void MainWindow::connect_set_variable(QString token0, QString token2)
{
  unsigned int token0_length;
  char token0_as_utf8[80 + 1];
  unsigned short int is_enable;

  token0_length= token0.count();
  if (token0_length >= 64) return; /* no option name is ever longer than 80 bytes */
  strcpy(token0_as_utf8,token0.toUtf8());
  *(token0_as_utf8 + token0_length)= 0;
  for (int i= 0; token0_as_utf8[i] != '\0'; ++i)
  {
    if (token0_as_utf8[i] == '-') token0_as_utf8[i]= '_';
  }

  /* option modifiers: ignore loose, set enable=true if enable or false if disable|skip */
  is_enable= 1;
  bool is_enable_disable_skip_specified= false;
  for (;;)
  {
    if (strncmp(token0_as_utf8, "loose_", sizeof("loose_") - 1) == 0)
    {
      strcpy(token0_as_utf8, token0_as_utf8 + sizeof("loose_") - 1);
      continue;
    }
    if (strncmp(token0_as_utf8, "enable_", sizeof("enable_") - 1) == 0)
    {
      is_enable_disable_skip_specified= true;
      is_enable= 1;
      strcpy(token0_as_utf8, token0_as_utf8 + sizeof("enable_") - 1);
      continue;
    }
    if (strncmp(token0_as_utf8, "disable_", sizeof("disable_") - 1) == 0)
    {
      is_enable_disable_skip_specified= true;
      is_enable= 0;
      strcpy(token0_as_utf8, token0_as_utf8 + sizeof("disable_") - 1);
      continue;
    }
    if (strncmp(token0_as_utf8, "skip_", sizeof("skip_") - 1) == 0)
    {
      is_enable_disable_skip_specified= true;
      is_enable= 0;
      strcpy(token0_as_utf8, token0_as_utf8 + sizeof("skip_") - 1);
      continue;
    }
    break;
  }
  if ((is_enable_disable_skip_specified == false) && (token2 > ""))
  {
    QString token2_upper= token2.toUpper();
    if ((token2 == "ON") || (token2 == "TRUE") || (token2.left(1) == "1")) ;
    else if ((token2 == "OFF") || (token2 == "FALSE") || (token2.left(1) == "0")) is_enable= 0;
    else /* error */ is_enable= 0;
  }

  if (strcmp(token0_as_utf8, "abort_source_on_error") == 0) { ocelot_abort_source_on_error= is_enable; return; }
  if (strcmp(token0_as_utf8, "auto_rehash") == 0) { ocelot_auto_rehash= is_enable; return; }
  if (strcmp(token0_as_utf8, "auto_vertical_output") == 0) { ocelot_auto_vertical_output= is_enable; return; }
  if (strcmp(token0_as_utf8, "batch") == 0)
  {
    ocelot_batch= is_enable;
    ocelot_silent= is_enable;
    return;
  }
  if (strcmp(token0_as_utf8, "binary_mode") == 0) { ocelot_binary_mode= is_enable; return; }
  if (strcmp(token0_as_utf8, "bind") == 0) /* not available in mysql client */
  {
    ocelot_opt_bind= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "bind_address") == 0) { ocelot_bind_address= is_enable; return; }

  if (strcmp(token0_as_utf8, "connect_expired_password") == 0) /* not available in mysql client before version 5.7 */
  {
    ocelot_opt_can_handle_expired_passwords= is_enable;
    return;
  }

  if (strcmp(token0_as_utf8, "character_sets_dir") == 0)
  {
    ocelot_set_charset_dir= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "column_names") == 0) { ocelot_result_grid_column_names= is_enable; return; }
  if (strcmp(token0_as_utf8, "column_type_info") == 0) { ocelot_column_type_info= is_enable; return; }
  if ((token0_length >= sizeof("comm") - 1) && (strncmp(token0_as_utf8, "comments", token0_length) == 0))
  {
    ocelot_comments= is_enable;
    return;
  }
  if ((token0_length >= sizeof("comp") - 1) && (strncmp(token0_as_utf8, "compress", token0_length) == 0))
  {
    ocelot_opt_compress= is_enable;
    return;
  }
  if (strcmp(token0_as_utf8, "connect_attr_delete") == 0) /* not available in mysql client */
  {
    ocelot_opt_connect_attr_delete= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "connect_attr_reset") == 0) /* not available in mysql client */
  {
    ocelot_opt_connect_attr_reset= is_enable;
    return;
  }
  if ((token0_length >= sizeof("con") - 1) && (strncmp(token0_as_utf8, "connect_timeout", token0_length) == 0))
  {
    ocelot_opt_connect_timeout= to_long(token2);
    return;
  }
  if (strcmp(token0_as_utf8, "database") == 0)
  {
    ocelot_database= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "debug") == 0) { ocelot_debug= token2; return; }
  if (strcmp(token0_as_utf8, "debug_info") == 0) { ocelot_debug_info= is_enable; return; }
  if ((token0_length >= sizeof("default_a") - 1) && (strncmp(token0_as_utf8, "default_auth", token0_length) == 0))
  {
    ocelot_default_auth= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "default_character_set") == 0)
  {
    ocelot_set_charset_name= token2;
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
  if (strcmp(token0_as_utf8, "defaults_group_suffix") == 0)
  {
    ocelot_defaults_group_suffix= token2;
    return;
  }
  if ((token0_length >= sizeof("del") - 1) && (strncmp(token0_as_utf8, "delimiter", token0_length) == 0))
  {
    ocelot_delimiter_str= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "cleartext_plugin") == 0)
  {
    ocelot_enable_cleartext_plugin= is_enable;
    return;
  }
  if (strcmp(token0_as_utf8, "execute") == 0) { ocelot_execute= token2; return; }
  if (strcmp(token0_as_utf8, "force") == 0) { ocelot_force= is_enable; return; }
  if (strcmp(token0_as_utf8, "help") == 0) { ocelot_help= is_enable; return; }
  if (strcmp(token0_as_utf8, "histfile") == 0) { ocelot_history_hist_file_name= token2; return; }
  if (strcmp(token0_as_utf8, "histignore") == 0) { ocelot_histignore= token2; return; }
  if ((token0_length >= sizeof("ho") - 1) && (strncmp(token0_as_utf8, "host", token0_length) == 0))
  {
    ocelot_host= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "html") == 0) { ocelot_html= is_enable; return; }
  if ((token0_length >= sizeof("i_") - 1) && (strncmp(token0_as_utf8, "i_am_a_dummy", token0_length) == 0))
  {
    ocelot_safe_updates= is_enable;
    return;
  }
  if (strcmp(token0_as_utf8, "ignore_spaces") == 0) { ocelot_ignore_spaces= is_enable; return; }
  if (strcmp(token0_as_utf8, "ld_run_path") == 0) { ocelot_ld_run_path= token2; return; }
  if (strcmp(token0_as_utf8, "init_command") == 0)
  {
    ocelot_init_command= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "line_numbers") == 0) { ocelot_line_numbers= is_enable; return; }
  if ((token0_length >= sizeof("loc") - 1) && (strncmp(token0_as_utf8, "local_infile", token0_length) == 0))
  {
    if (token2 > "") ocelot_opt_local_infile= to_long(token2);
    else ocelot_opt_local_infile= is_enable;
    return;
  }
  if (strcmp(token0_as_utf8, "login_path") == 0) { ocelot_login_path= token2; return; }
  if (strcmp(token0_as_utf8, "max_allowed_packet") == 0) { ocelot_max_allowed_packet= to_long(token2); return; }
  if ((token0_length >= sizeof("max_j") - 1) && (strncmp(token0_as_utf8, "max_join_size", token0_length) == 0))
  {
    ocelot_max_join_size= to_long(token2);
    return;
  }
  if (strcmp(token0_as_utf8, "named_commands") == 0) { ocelot_named_commands= is_enable; return; }
  if (strcmp(token0_as_utf8, "net_buffer_length") == 0) { ocelot_net_buffer_length= to_long(token2); return; }
  if (strcmp(token0_as_utf8, "no_auto_rehash") == 0) { ocelot_auto_rehash= 0; return; }
  if ((token0_length >= sizeof("no_b") - 1) && (strncmp(token0_as_utf8, "no_beep", token0_length) == 0))
  {
    ocelot_no_beep= is_enable;
    return;
  }
  if (strcmp(token0_as_utf8, "no_defaults") == 0) { ocelot_no_defaults= is_enable; return; }
  if (strcmp(token0_as_utf8, "no_named_commands") == 0) { ocelot_named_commands= 0; return; }
  if (strcmp(token0_as_utf8, "no_tee") == 0) { history_file_stop("TEE"); return; }/* see comment=tee+hist */

  QString ccn;
  /* Changes to ocelot_* settings. But we don't check that they're in the [ocelot] group. */
  /* Todo: validity checks */

  if (strcmp(token0_as_utf8, "ocelot_dbms") == 0)
  {
    ocelot_dbms= token2;
    if (ocelot_dbms.contains("mysql", Qt::CaseInsensitive) == true)
    {
      connections_dbms[0]= DBMS_MYSQL;
    }
    else if (ocelot_dbms.contains("mariadb", Qt::CaseInsensitive) == true)
    {
      connections_dbms[0]= DBMS_MARIADB;
    }
#ifdef DBMS_TARANTOOL
    else if (ocelot_dbms.contains("tarantool", Qt::CaseInsensitive) == true)
    {
      connections_dbms[0]= DBMS_TARANTOOL;
    }
#endif
    else connections_dbms[0]= DBMS_MYSQL; /* default */
    return;
  }

  if (strcmp(token0_as_utf8, "ocelot_extra_rule_1_text_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_extra_rule_1_text_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_extra_rule_1_background_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_extra_rule_1_background_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_extra_rule_1_condition") == 0) { ocelot_extra_rule_1_condition= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_extra_rule_1_display_as") == 0) { ocelot_extra_rule_1_display_as= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_text_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_grid_text_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_background_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_grid_background_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_border_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_grid_border_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_header_background_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_grid_header_background_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_font_family") == 0) { ocelot_grid_font_family= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_font_size") == 0) { ocelot_grid_font_size= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_font_style") == 0)
  { ccn= canonical_font_style(token2); if (ccn != "") ocelot_grid_font_style= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_font_weight") == 0)
  { ccn= canonical_font_weight(token2); if (ccn != "") ocelot_grid_font_weight= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_cell_border_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_grid_cell_border_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_cell_drag_line_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_grid_cell_drag_line_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_border_size") == 0) { ocelot_grid_border_size= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_cell_border_size") == 0) { ocelot_grid_cell_border_size= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_cell_drag_line_size") == 0) { ocelot_grid_cell_drag_line_size= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_grid_tabs") == 0) { ocelot_grid_tabs= to_long(token2); return; }
  if (strcmp(token0_as_utf8, "ocelot_history_text_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_history_text_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_history_background_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_history_background_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_history_border_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_history_border_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_history_font_family") == 0) { ocelot_history_font_family= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_history_font_size") == 0) { ocelot_history_font_size= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_history_font_style") == 0)
  { ccn= canonical_font_style(token2); if (ccn != "") ocelot_history_font_style= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_history_font_weight") == 0)
  { ccn= canonical_font_weight(token2); if (ccn != "") ocelot_history_font_weight= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_history_max_row_count") == 0) { ocelot_history_max_row_count= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_language") == 0) { ocelot_language= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_menu_text_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_menu_text_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_menu_background_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_menu_background_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_menu_border_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_menu_border_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_menu_font_family") == 0) { ocelot_menu_font_family= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_menu_font_size") == 0) { ocelot_menu_font_size= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_menu_font_style") == 0)
  { ccn= canonical_font_style(token2); if (ccn != "") ocelot_menu_font_style= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_menu_font_weight") == 0)
  { ccn= canonical_font_weight(token2); if (ccn != "") ocelot_menu_font_weight= ccn; return; }
  /* "ocelot_shortcut_exit" etc. are handled specially */
  if (shortcut(token0_as_utf8, token2, true, false) != 0) return;
  if (strcmp(token0_as_utf8, "ocelot_statement_text_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_statement_text_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_background_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_statement_background_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_border_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_statement_border_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_font_family") == 0) { ocelot_statement_font_family= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_font_size") == 0) { ocelot_statement_font_size= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_font_style") == 0)
  { ccn= canonical_font_style(token2); if (ccn != "") ocelot_statement_font_style= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_font_weight") == 0)
  { ccn= canonical_font_weight(token2); if (ccn != "") ocelot_statement_font_weight= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_highlight_literal_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_statement_highlight_literal_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_highlight_identifier_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_statement_highlight_identifier_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_highlight_comment_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_statement_highlight_comment_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_highlight_operator_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_statement_highlight_operator_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_highlight_keyword_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_statement_highlight_keyword_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_prompt_background_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_statement_prompt_background_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_highlight_current_line_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_statement_highlight_current_line_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_highlight_function_color") == 0)
  { ccn= canonical_color_name(token2); if (ccn != "") ocelot_statement_highlight_function_color= ccn; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_syntax_checker") == 0)
  { ocelot_statement_syntax_checker= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_format_statement_indent") == 0)
  { ocelot_statement_format_statement_indent= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_format_clause_indent") == 0)
  { ocelot_statement_format_clause_indent= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_statement_format_keyword_case") == 0)
  { ocelot_statement_format_keyword_case= token2; return; }
  if (strcmp(token0_as_utf8, "ocelot_client_side_functions") == 0) { ocelot_client_side_functions= is_enable; return; }
  if (strcmp(token0_as_utf8, "ocelot_log_level") == 0)
  {
    ocelot_log_level= to_long(token2);
    return;
  }
  if (strcmp(token0_as_utf8, "one_database") == 0) { ocelot_one_database= is_enable; return; }
  if (strcmp(token0_as_utf8, "pager") == 0) { ocelot_pager= is_enable; return; }
  if ((token0_length >= sizeof("pas") - 1) && (strncmp(token0_as_utf8, "password", token0_length) == 0))
  {
    ocelot_password= token2;
    ocelot_password_was_specified= is_enable;
    return;
  }
  if (strcmp(token0_as_utf8, "pipe") == 0) /* Not sure about this. Windows. Same as protocol? */
  {
    ocelot_opt_named_pipe= is_enable;
    return;
  }
  if ((token0_length >= sizeof("pl") - 1) && (strncmp(token0_as_utf8, "plugin_dir", token0_length) == 0))
  {
    ocelot_plugin_dir= token2;
    return;
  }
  if ((token0_length >= sizeof("po") - 1) && (strncmp(token0_as_utf8, "port", token0_length) == 0))
  {
    ocelot_port= to_long(token2);
    return;
  }
  if (strcmp(token0_as_utf8, "print_defaults") == 0) { ocelot_print_defaults= is_enable; return; }
  if (strcmp(token0_as_utf8, "prompt") == 0) { ocelot_prompt= token2; ocelot_prompt_is_default= false; return; }
  if ((token0_length >= sizeof("prot") - 1) && (strncmp(token0_as_utf8, "protocol", token0_length) == 0))
  {
    ocelot_protocol= token2; /* Todo: perhaps make sure it's tcp/socket/pipe/memory */
    ocelot_protocol_as_int= get_ocelot_protocol_as_int(ocelot_protocol);
    return;
  }
  if (strcmp(token0_as_utf8, "quick") == 0) { ocelot_quick= is_enable; return; }
  if (strcmp(token0_as_utf8, "raw") == 0) { ocelot_raw= is_enable; return; }
  if (strcmp(token0_as_utf8, "read_default_file") == 0) /* not available in mysql client */
  {
    ocelot_read_default_file= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "read_default_group") == 0) /* not available in mysql client */
  {
    ocelot_read_default_group= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "read_timeout") == 0) /* not available in mysql client */
  {
    ocelot_opt_read_timeout= to_long(token2);
    return;
  }
  if (strcmp(token0_as_utf8, "reconnect") == 0)
  {
    ocelot_opt_reconnect= is_enable;
    return;
  }
  if (strcmp(token0_as_utf8, "report_data_truncation") == 0) /* not available in mysql client */
  {
    ocelot_report_data_truncation= is_enable;
    return;
  }
  if ((token0_length >= sizeof("sa") - 1) && (strncmp(token0_as_utf8, "safe_updates", token0_length) == 0))
  {
    ocelot_safe_updates= is_enable;
    return;
  }
  if ((token0_length >= sizeof("sec") -1 ) && (strncmp(token0_as_utf8, "secure_auth", token0_length) == 0))
  {
    ocelot_secure_auth= is_enable;
    return;
  }
  if (strcmp(token0_as_utf8, "server_public_key") == 0) /* not available in mysql client */
  {
    ocelot_server_public_key= token2;
    return;
  }
   if ((token0_length >= sizeof("sel") - 1) && (strncmp(token0_as_utf8, "select_limit", token0_length) == 0))
  {
    ocelot_select_limit= to_long(token2);
    return;
  }

  if (strcmp(token0_as_utf8, "shared_memory_base_name") == 0)
  {
    ocelot_shared_memory_base_name= token2;
    return;
    }
  if ((token0_length >= sizeof("sh") - 1) && (strncmp(token0_as_utf8, "show_warnings", token0_length) == 0))
  {
    ocelot_history_includes_warnings= is_enable;
    return;
    }
  if (strcmp(token0_as_utf8, "sigint_ignore") == 0) { ocelot_sigint_ignore= is_enable; return; }
  if ((token0_length >= sizeof("sil") - 1) && (strncmp(token0_as_utf8, "silent", token0_length) == 0))
  {
    ocelot_silent= is_enable;
    return;
  }
  if ((token0_length >= sizeof("so") - 1) && (strncmp(token0_as_utf8, "socket", token0_length) == 0))
  {
    ocelot_unix_socket= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl") == 0)
  {
    ocelot_opt_ssl= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_ca") == 0)
  {
    ocelot_opt_ssl_ca= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_capath") == 0)
  {
    ocelot_opt_ssl_capath= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_cert") == 0)
  {
    ocelot_opt_ssl_cert= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_cipher") == 0)
  {
    ocelot_opt_ssl_cipher= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_crl") == 0)
  {
    ocelot_opt_ssl_crl= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_crlpath") == 0)
  {
    ocelot_opt_ssl_crlpath= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_key") == 0)
  {
    ocelot_opt_ssl_key= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_mode") == 0)
  {
    ocelot_opt_ssl_mode= token2;
    return;
  }
  if ((token0_length >= sizeof("ssl_verify") - 1) && (strncmp(token0_as_utf8, "ssl_verify_server_cert", token0_length) == 0))
  {
    ocelot_opt_ssl_verify_server_cert= to_long(token2);
    return;
  }
  if (strcmp(token0_as_utf8, "syslog") == 0) { ocelot_syslog= is_enable; return; }
  if (strcmp(token0_as_utf8, "table") == 0) { ocelot_table= is_enable; return; }
  if (strcmp(token0_as_utf8, "tee") == 0) { history_file_start("TEE", token2); /* todo: check whether history_file_start returned NULL which is an error */ return; }/* see comment=tee+hist */
  if (strcmp(token0_as_utf8, "unbuffered") == 0) { ocelot_unbuffered= is_enable; return; }
  if (strcmp(token0_as_utf8, "use_result") == 0) /* not available in mysql client */
  {
    ocelot_opt_use_result= to_long(token2);
    return;
  }
  if ((token0_length >= sizeof("us") - 1) && (strncmp(token0_as_utf8, "user", token0_length) == 0))
  {
    ocelot_user= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "verbose") == 0) { ocelot_verbose= is_enable; return; }
  if (strcmp(token0_as_utf8, "version") == 0) { ocelot_version= is_enable; return; }
  /* todo: check that this finds both --vertical and -E */ /* for vertical */
  if (strcmp(token0_as_utf8, "vertical") == 0) { ocelot_result_grid_vertical= is_enable; return; }
  if ((token0_length >= sizeof("wa") - 1) && (strncmp(token0_as_utf8, "wait", token0_length) == 0))
  {
    ocelot_wait= is_enable;
    return;
  }
  if (strcmp(token0_as_utf8, "write_timeout") == 0)
  {
    ocelot_opt_write_timeout= to_long(token2);
    return;
  }
  if (strcmp(token0_as_utf8, "xml") == 0) { ocelot_xml= is_enable; return; }
}


/*
  Given the options that have actually been set to non-default values,
  form a CONNECT statement.
  todo: something should call this for the sake of history, at least.
  todo: instead o x > "", use if (QString::compare(x, " ") > 0)
*/
void MainWindow::connect_make_statement()
{
  /* Todo: QMessageBox should have a parent, use "= new" */
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
  if (ocelot_opt_connect_timeout > 0) statement_text= statement_text + " connect_timeout=" + QString::number(ocelot_opt_connect_timeout);
  if (ocelot_opt_compress > 0) statement_text= statement_text + " compress";
  if (ocelot_secure_auth > 0) statement_text= statement_text + " secure_auth";
  if (ocelot_opt_local_infile > 0) statement_text= statement_text + " local_infile";
  if (ocelot_safe_updates > 0) statement_text= statement_text + " safe_updates";
  if (ocelot_plugin_dir > "") statement_text= statement_text + " plugin_dir=" + ocelot_plugin_dir;
  if (ocelot_select_limit > 0) statement_text= statement_text + " select_limit=" + QString::number(ocelot_select_limit);
  if (ocelot_max_join_size > 0) statement_text= statement_text + " max_join_size=" + QString::number(ocelot_max_join_size);
  if (ocelot_silent > 0) statement_text= statement_text + " silent";
  if (ocelot_no_beep > 0) statement_text= statement_text + "no_beep";
  if (ocelot_wait > 0) statement_text= statement_text + "wait";
  if (ocelot_set_charset_name > "") statement_text= statement_text + "default_character_set=" + ocelot_set_charset_name;
  if (ocelot_opt_ssl > "") statement_text= statement_text + "ssl=" + ocelot_opt_ssl;
  if (ocelot_opt_ssl_ca > "") statement_text= statement_text + "ssl_ca=" + ocelot_opt_ssl_ca;
  if (ocelot_opt_ssl_capath > "") statement_text= statement_text + "ssl_capath=" + ocelot_opt_ssl_capath;
  if (ocelot_opt_ssl_cert > "") statement_text= statement_text + "ssl_cert=" + ocelot_opt_ssl_cert;
  if (ocelot_opt_ssl_cipher > "") statement_text= statement_text + "ssl_cipher=" + ocelot_opt_ssl_cipher;
  if (ocelot_opt_ssl_crl > "") statement_text= statement_text + "ssl_crl=" + ocelot_opt_ssl_crl;
  if (ocelot_opt_ssl_crlpath > "") statement_text= statement_text + "ssl_crlpath=" + ocelot_opt_ssl_crlpath;
  if (ocelot_opt_ssl_key > "") statement_text= statement_text + "ssl_key=" + ocelot_opt_ssl_key;
  if (ocelot_opt_ssl_mode > "") statement_text= statement_text + "ssl_mode=" + ocelot_opt_ssl_mode;
  if (ocelot_opt_ssl_verify_server_cert > 0) statement_text= statement_text + "ssl_verify_server_cert=" + ocelot_opt_ssl_verify_server_cert;
  msgBox.setText(statement_text);
  msgBox.exec();
}

#define PROTOCOL_TCP 1
#define PROTOCOL_SOCKET 2
#define PROTOCOL_PIPE 3
#define PROTOCOL_MEMORY 4
unsigned int MainWindow::get_ocelot_protocol_as_int(QString ocelot_protocol)
{
  if (QString::compare(ocelot_protocol, "TCP", Qt::CaseInsensitive) == 0) return PROTOCOL_TCP;
  if (QString::compare(ocelot_protocol, "SOCKET", Qt::CaseInsensitive) == 0) return PROTOCOL_SOCKET;
  if (QString::compare(ocelot_protocol, "PIPE", Qt::CaseInsensitive) == 0) return PROTOCOL_PIPE;
  if (QString::compare(ocelot_protocol, "MEMORY", Qt::CaseInsensitive) == 0) return PROTOCOL_MEMORY;
  return 0;
}


/*
  Todo: this routine calls mysql_options() iff option value != 0,
  forgetting that 0 might be non-default, or a change from non-0.
  Also it doesn't check whether mysql_options() failed, but okay.
  Note: we don't pass ocelot_opt_can_handle_expired_passwords because
  it does nothing (maybe due to an old libmysqlclient?), instead we
  pass client_can_handle_expired_passwords to mysql_real_connect().

  Re MySQL 5.7.11 and ssl_mode: mysql allows truncation e.g.
  --ssl_mode='PREF' but we don't it must be in full.
  We don't know in advance whether we'll be connecting with MySQL 5.7.11
  so we'll call only if we (not MariaDB|Tarantool) and (not default).
  Our default is '' but we'll also do nothing if 'preferred'.
  Check if mysql_options() returns != 0 indicating libmysqlclient
  doesn't recognize MYSQL_OPT_SSL_MODE, if so assume it didn't work.
  If it's VERIFY_IDENTITY, we don't try to use --ssl-verify-server-cert.
  It should mean we don't try to use --ssl, but we don't anyway.
*/
int options_and_connect(
    unsigned int connection_number,
    char *database_as_utf8)
{
  if (connected[connection_number] != 0)
  {
    connected[connection_number]= 0;
    lmysql->ldbms_mysql_close(&mysql[connection_number]);
  }
#ifdef _WIN32
  /* Actually this has no effect in my tests. Maybe just superstition. */
  /* But it's harmless. See https://bugs.mysql.com/bug.php?id=8059 */
  int padding= 42;
  mysql_init(&mysql[connection_number]);
#else
  lmysql->ldbms_mysql_init(&mysql[connection_number]);
#endif
  int opt= 0;
  if (ocelot_default_auth_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_23, ocelot_default_auth_as_utf8);
  if (ocelot_enable_cleartext_plugin == true) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_36, (char *) &ocelot_enable_cleartext_plugin);
  if (ocelot_init_command_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_3, ocelot_init_command_as_utf8);
  if (ocelot_opt_bind_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_24, ocelot_opt_bind_as_utf8);
  if (ocelot_opt_compress > 0) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_1, NULL);
  if (ocelot_opt_connect_attr_delete_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_34, ocelot_opt_connect_attr_delete_as_utf8);
  if (ocelot_opt_connect_attr_reset != 0) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_32, (char*) &ocelot_opt_connect_attr_reset);
  if (ocelot_opt_connect_timeout != 0)
  {
    unsigned int timeout= ocelot_opt_connect_timeout;
    lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_0, (char*) &timeout);
  }
  if (ocelot_opt_local_infile > 0) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_8, (char*) &ocelot_opt_local_infile);
  if (ocelot_opt_named_pipe > 0) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_2, (char*) &ocelot_opt_named_pipe);
  if (ocelot_protocol_as_int > 0) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_9, (char*)&ocelot_protocol_as_int);
  if (ocelot_opt_read_timeout > 0) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_11, (char*)&ocelot_opt_read_timeout);
  if (ocelot_opt_reconnect > 0) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_20, (char*)&ocelot_opt_reconnect);

  if (ocelot_opt_ssl_mode_as_utf8[0] != '\0')
  {
    for(int i= 0; ocelot_opt_ssl_mode_as_utf8[i] != 0; ++i)
    {
      ocelot_opt_ssl_mode_as_utf8[i] = toupper(ocelot_opt_ssl_mode_as_utf8[i]);
    }
    if (strcmp(ocelot_opt_ssl_mode_as_utf8, "DISABLED") == 0) opt= 1;
    if (strcmp(ocelot_opt_ssl_mode_as_utf8, "PREFERRED") == 0) opt= 2;
    if (strcmp(ocelot_opt_ssl_mode_as_utf8, "REQUIRED") == 0) opt= 3;
    if (strcmp(ocelot_opt_ssl_mode_as_utf8, "VERIFY_CA") == 0) opt= 4;
    if (strcmp(ocelot_opt_ssl_mode_as_utf8, "VERIFY_IDENTITY") == 0) opt= 5;
    if (opt != 0)
    {
      if (lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_42, (char*) &opt) != 0)
      {
        opt= 0;
      }
    }
  }
  /*
    If dlopen() failed for "myql_ssl_set" then ldbms_mysql_ssl_set is a no-op, which is not an error.
    For some options use mysql_ssl_set because it's in MySQL 5.5, for others use mysql_options.
    We treat "" as the same as NULL, and prefer to pass NULL.
    Instead of looking at ocelot_opt_ssl, we check whether anything is non-NULL.
  */
  {
    char *a= 0, *b= 0, *c= 0, *d= 0, *e= 0;
    if (ocelot_opt_ssl_key_as_utf8[0] != '\0') a= ocelot_opt_ssl_key_as_utf8;
    if (ocelot_opt_ssl_cert_as_utf8[0] != '\0') b= ocelot_opt_ssl_cert_as_utf8;
    if (ocelot_opt_ssl_ca_as_utf8[0] != '\0') c= ocelot_opt_ssl_ca_as_utf8;
    if (ocelot_opt_ssl_capath_as_utf8[0] != '\0') d= ocelot_opt_ssl_capath_as_utf8;
    if (ocelot_opt_ssl_cipher_as_utf8[0] != '\0') e= ocelot_opt_ssl_cipher_as_utf8;
    if ((a != 0) || (b != 0) || (c != 0) || (d != 0) || (e != 0))
    {
      lmysql->ldbms_mysql_ssl_set(&mysql[connection_number], a, b, c, d, e);
    }
  }
  if (ocelot_opt_ssl_crl_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_30, ocelot_opt_ssl_crl_as_utf8);
  if (ocelot_opt_ssl_crlpath_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_31, ocelot_opt_ssl_crlpath_as_utf8);
  if (opt != 5)
  {
    if (ocelot_opt_ssl_verify_server_cert > 0) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_21, (char*) &ocelot_opt_ssl_verify_server_cert);
  }
  if (ocelot_opt_write_timeout > 0) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_12, (char*) &ocelot_opt_write_timeout);
  if (ocelot_plugin_dir_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_22, ocelot_plugin_dir_as_utf8);
  if (ocelot_read_default_file_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_4, ocelot_read_default_file_as_utf8);
  if (ocelot_read_default_group_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_5, ocelot_read_default_group_as_utf8);
  if (ocelot_report_data_truncation > 0) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_19, (char*) &ocelot_report_data_truncation);
  if (ocelot_secure_auth > 0) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_18, (char *) &ocelot_secure_auth);
  if (ocelot_server_public_key_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_35, ocelot_server_public_key_as_utf8);
  if (ocelot_set_charset_dir_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_6, ocelot_set_charset_dir_as_utf8);
  if (ocelot_set_charset_name_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_7, ocelot_set_charset_name_as_utf8);
  if (ocelot_shared_memory_base_name_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_10, ocelot_shared_memory_base_name_as_utf8);
  if (ocelot_safe_updates > 0)
  {
    char init_command[100]; /* todo: the size could be more dynamic here */
    sprintf(init_command,
        "SET sql_select_limit = %lu, sql_safe_updates = 1, max_join_size = %lu",
        ocelot_select_limit, ocelot_max_join_size);
    lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_3, init_command);
  }
  /* CLIENT_MULTI_RESULTS but not CLIENT_MULTI_STATEMENTS */
  unsigned long real_connect_flags= CLIENT_MULTI_RESULTS;
  if (ocelot_opt_can_handle_expired_passwords != 0)
    real_connect_flags|= (1UL << 22); /* CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS */

  MYSQL *connect_result;
  char *socket_parameter= ocelot_unix_socket_as_utf8;

  for (int connect_attempt= 0; connect_attempt < 6; ++connect_attempt)
  {
    connect_result= lmysql->ldbms_mysql_real_connect(&mysql[connection_number],
                                                     ocelot_host_as_utf8,
                                                     ocelot_user_as_utf8,
                                                     ocelot_password_as_utf8,
                                                     database_as_utf8,
                                                     ocelot_port,
                                                     socket_parameter,
                                                     real_connect_flags);
     if (connect_result != NULL) break;
     /* See ocelot.ca blog post = Connecting to MySQL or MariaDB with sockets on Linux */
     /* Todo: you should provide info somewhere how the connection was actually done. */
     if ((ocelot_protocol_as_int != 0) && (ocelot_protocol_as_int != PROTOCOL_SOCKET)) break;
#ifdef _WIN32
     break;
#endif
     if ((ocelot_unix_socket_as_utf8 != 0) && (strcmp(ocelot_unix_socket_as_utf8, "") != 0)) break;
     if ((ocelot_host_as_utf8 != 0) && (strcmp(ocelot_host_as_utf8,"") != 0) && (strcmp(ocelot_host_as_utf8, "localhost") != 0)) break;
     if (lmysql->ldbms_mysql_errno(&mysql[connection_number]) != 2002) break; /* 2002 == CR_CONNECTION_ERROR */
     if (connect_attempt == 0) socket_parameter= (char *) "/tmp/mysql.sock";
     if (connect_attempt == 1) socket_parameter= (char *) "/var/lib/mysql/mysql.sock";
     if (connect_attempt == 2) socket_parameter= (char *) "/var/run/mysqld/mysqld.sock";
     if (connect_attempt == 3) socket_parameter= (char *) "/var/run/mysql/mysql.sock";
     if (connect_attempt == 4) socket_parameter= (char *) "/tmp/mysqld.sock";
   }

  if (connect_result == NULL)
  {
    /* connect failed. todo: better diagnostics? anyway, user can retry, a dialog box will come up. */
    return -1;					// Retryable
  }

  /*
    Tell the server: characters from the client are UTF8, characters going to the client are UTF8.
    This partially overrides --default_character_set, except there's no change to character_set_connection.
    Todo: Eventually UTF8MB4 will be preferable but check server version before trying that.
    Todo: We could have an option to receive in UTF16 which is what Qt would prefer.
    Todo: We should warn or disallow if user tries to change these.
    Todo: Think what we're going to do with file I/O, e.g. the user might expect it to be ujis.
    Todo: This makes the server do conversions. Eventually the client could do the conversions but that's lots of work.
    Todo: Worry that there might be some way to start Qt with different character-set assumptions.
    Todo: Worry that we haven't got a plan for _latin2 'string' etc. although we could get the server to translate for us
  */
  if (lmysql->ldbms_mysql_query(&mysql[connection_number], "set character_set_client = utf8")) printf("SET character_set_client failed\n");
  if (lmysql->ldbms_mysql_query(&mysql[connection_number], "set character_set_results = utf8")) printf("SET character_set_results failed\n");

  connected[connection_number]= 1;
  return 0;
}

/*
  Convert string to long.
  For numeric connect-related tokens, K means 1024, M means 1024**2, G means 1024**3.
  Actually the token might be unsigned int or unsigned long, but this will do.
*/
long MainWindow::to_long(QString token)
{
  QString kmg;
  QString token_without_kmg;
  long return_value;

  kmg= token.right(1).toUpper();
  if ((kmg == "K") || (kmg == "M") || (kmg == "G"))
  {
    token_without_kmg= token.left(token.size() - 1);
    return_value= token_without_kmg.toLong();
    if (kmg == "K") return_value *= 1024;
    if (kmg == "M") return_value *= 1024 * 1024;
    if (kmg == "G") return_value *= 1024 * 1024 * 1024;
  }
  else return_value= token.toLong();
  return return_value;

}

/*
  called from: copy_connect_strings_to_utf8(), to initialize before making new copies
  called from: action_exit(), to avoid valgrind complaints
*/
void MainWindow::delete_utf8_copies()
{
  if (ocelot_host_as_utf8 != 0) { delete [] ocelot_host_as_utf8; ocelot_host_as_utf8= 0; }
  if (ocelot_database_as_utf8 != 0) { delete [] ocelot_database_as_utf8; ocelot_database_as_utf8= 0; }
  if (ocelot_user_as_utf8 != 0) { delete [] ocelot_user_as_utf8; ocelot_user_as_utf8= 0; }
  if (ocelot_password_as_utf8 != 0) { delete [] ocelot_password_as_utf8; ocelot_password_as_utf8= 0; }
  if (ocelot_default_auth_as_utf8 != 0) { delete [] ocelot_default_auth_as_utf8; ocelot_default_auth_as_utf8= 0; }
  if (ocelot_default_auth_as_utf8 != 0) { delete [] ocelot_default_auth_as_utf8; ocelot_default_auth_as_utf8= 0; }
  if (ocelot_default_auth_as_utf8 != 0) { delete [] ocelot_default_auth_as_utf8; ocelot_default_auth_as_utf8= 0; }
  if (ocelot_opt_bind_as_utf8 != 0) { delete [] ocelot_opt_bind_as_utf8; ocelot_opt_bind_as_utf8= 0; }
  if (ocelot_opt_connect_attr_delete_as_utf8 != 0) { delete [] ocelot_opt_connect_attr_delete_as_utf8; ocelot_opt_connect_attr_delete_as_utf8= 0; }
  if (ocelot_opt_ssl_as_utf8 != 0) { delete [] ocelot_opt_ssl_as_utf8; ocelot_opt_ssl_as_utf8= 0; }
  if (ocelot_opt_ssl_ca_as_utf8 != 0) { delete [] ocelot_opt_ssl_ca_as_utf8; ocelot_opt_ssl_ca_as_utf8= 0; }
  if (ocelot_opt_ssl_capath_as_utf8 != 0) { delete [] ocelot_opt_ssl_capath_as_utf8; ocelot_opt_ssl_capath_as_utf8= 0; }
  if (ocelot_opt_ssl_cert_as_utf8 != 0) { delete [] ocelot_opt_ssl_cert_as_utf8; ocelot_opt_ssl_cert_as_utf8= 0; }
  if (ocelot_opt_ssl_cipher_as_utf8 != 0) { delete [] ocelot_opt_ssl_cipher_as_utf8; ocelot_opt_ssl_cipher_as_utf8= 0; }
  if (ocelot_opt_ssl_crl_as_utf8 != 0) { delete [] ocelot_opt_ssl_crl_as_utf8; ocelot_opt_ssl_crl_as_utf8= 0; }
  if (ocelot_opt_ssl_crlpath_as_utf8 != 0) { delete [] ocelot_opt_ssl_crlpath_as_utf8; ocelot_opt_ssl_crlpath_as_utf8= 0; }
  if (ocelot_opt_ssl_key_as_utf8 != 0) { delete [] ocelot_opt_ssl_key_as_utf8; ocelot_opt_ssl_key_as_utf8= 0; }
  if (ocelot_opt_ssl_mode_as_utf8 != 0) { delete [] ocelot_opt_ssl_mode_as_utf8; ocelot_opt_ssl_mode_as_utf8= 0; }
  if (ocelot_plugin_dir_as_utf8 != 0) { delete [] ocelot_plugin_dir_as_utf8; ocelot_plugin_dir_as_utf8= 0; }
  if (ocelot_read_default_group_as_utf8 != 0) { delete [] ocelot_read_default_group_as_utf8; ocelot_read_default_group_as_utf8= 0; }
  if (ocelot_read_default_file_as_utf8 != 0) { delete [] ocelot_read_default_file_as_utf8; ocelot_read_default_file_as_utf8= 0; }
  if (ocelot_server_public_key_as_utf8 != 0) { delete [] ocelot_server_public_key_as_utf8; ocelot_server_public_key_as_utf8= 0; }
  if (ocelot_unix_socket_as_utf8 != 0) { delete [] ocelot_unix_socket_as_utf8; ocelot_unix_socket_as_utf8= 0; }
  if (ocelot_set_charset_dir_as_utf8 != 0) { delete [] ocelot_set_charset_dir_as_utf8; ocelot_set_charset_dir_as_utf8= 0; }
  if (ocelot_set_charset_name_as_utf8 != 0) { delete [] ocelot_set_charset_name_as_utf8; ocelot_set_charset_name_as_utf8= 0; }
  if (ocelot_shared_memory_base_name_as_utf8 != 0) { delete [] ocelot_shared_memory_base_name_as_utf8; ocelot_shared_memory_base_name_as_utf8= 0; }
}

/* Todo: check every one of the "new ..." results for failure. */
void MainWindow::copy_connect_strings_to_utf8()
{
  delete_utf8_copies();
  /* See comment "UTF8 Conversion" */

  int tmp_host_len= ocelot_host.toUtf8().size();
  ocelot_host_as_utf8= new char[tmp_host_len + 1];
  memcpy(ocelot_host_as_utf8, ocelot_host.toUtf8().constData(), tmp_host_len + 1);

  int tmp_database_len= ocelot_database.toUtf8().size();
  ocelot_database_as_utf8= new char[tmp_database_len + 1];
  memcpy(ocelot_database_as_utf8, ocelot_database.toUtf8().constData(), tmp_database_len + 1);

  int tmp_user_len= ocelot_user.toUtf8().size();
  ocelot_user_as_utf8= new char[tmp_user_len + 1];
  memcpy(ocelot_user_as_utf8, ocelot_user.toUtf8().constData(), tmp_user_len + 1);

  int tmp_password_len= ocelot_password.toUtf8().size();
  ocelot_password_as_utf8= new char[tmp_password_len + 1];
  memcpy(ocelot_password_as_utf8, ocelot_password.toUtf8().constData(), tmp_password_len + 1);

  int tmp_default_auth_len= ocelot_default_auth.toUtf8().size();
  ocelot_default_auth_as_utf8= new char[tmp_default_auth_len + 1];
  memcpy(ocelot_default_auth_as_utf8, ocelot_default_auth.toUtf8().constData(), tmp_default_auth_len + 1);

  int tmp_init_command_len= ocelot_init_command.toUtf8().size();
  ocelot_init_command_as_utf8= new char[tmp_init_command_len + 1];
  memcpy(ocelot_init_command_as_utf8, ocelot_init_command.toUtf8().constData(), tmp_init_command_len + 1);

  int tmp_opt_bind_len= ocelot_opt_bind.toUtf8().size();
  ocelot_opt_bind_as_utf8= new char[tmp_opt_bind_len + 1];
  memcpy(ocelot_opt_bind_as_utf8, ocelot_opt_bind.toUtf8().constData(), tmp_opt_bind_len + 1);

  int tmp_opt_connect_attr_delete_len= ocelot_opt_connect_attr_delete.toUtf8().size();
  ocelot_opt_connect_attr_delete_as_utf8= new char[tmp_opt_connect_attr_delete_len + 1];
  memcpy(ocelot_opt_connect_attr_delete_as_utf8, ocelot_opt_connect_attr_delete.toUtf8().constData(), tmp_opt_connect_attr_delete_len + 1);

  int tmp_opt_ssl_len= ocelot_opt_ssl.toUtf8().size();
  ocelot_opt_ssl_as_utf8= new char[tmp_opt_ssl_len + 1];
  memcpy(ocelot_opt_ssl_as_utf8, ocelot_opt_ssl.toUtf8().constData(), tmp_opt_ssl_len + 1);

  int tmp_opt_ssl_ca_len= ocelot_opt_ssl_ca.toUtf8().size();
  ocelot_opt_ssl_ca_as_utf8= new char[tmp_opt_ssl_ca_len + 1];
  memcpy(ocelot_opt_ssl_ca_as_utf8, ocelot_opt_ssl_ca.toUtf8().constData(), tmp_opt_ssl_ca_len + 1);

  int tmp_opt_ssl_capath_len= ocelot_opt_ssl_capath.toUtf8().size();
  ocelot_opt_ssl_capath_as_utf8= new char[tmp_opt_ssl_capath_len + 1];
  memcpy(ocelot_opt_ssl_capath_as_utf8, ocelot_opt_ssl_capath.toUtf8().constData(), tmp_opt_ssl_capath_len + 1);

  int tmp_opt_ssl_cert_len= ocelot_opt_ssl_cert.toUtf8().size();
  ocelot_opt_ssl_cert_as_utf8= new char[tmp_opt_ssl_cert_len + 1];
  memcpy(ocelot_opt_ssl_cert_as_utf8, ocelot_opt_ssl_cert.toUtf8().constData(), tmp_opt_ssl_cert_len + 1);

  int tmp_opt_ssl_cipher_len= ocelot_opt_ssl_cipher.toUtf8().size();
  ocelot_opt_ssl_cipher_as_utf8= new char[tmp_opt_ssl_cipher_len + 1];
  memcpy(ocelot_opt_ssl_cipher_as_utf8, ocelot_opt_ssl_cipher.toUtf8().constData(), tmp_opt_ssl_cipher_len + 1);

  int tmp_opt_ssl_crl_len= ocelot_opt_ssl_crl.toUtf8().size();
  ocelot_opt_ssl_crl_as_utf8= new char[tmp_opt_ssl_crl_len + 1];
  memcpy(ocelot_opt_ssl_crl_as_utf8, ocelot_opt_ssl_crl.toUtf8().constData(), tmp_opt_ssl_crl_len + 1);

  int tmp_opt_ssl_crlpath_len= ocelot_opt_ssl_crlpath.toUtf8().size();
  ocelot_opt_ssl_crlpath_as_utf8= new char[tmp_opt_ssl_crlpath_len + 1];
  memcpy(ocelot_opt_ssl_crlpath_as_utf8, ocelot_opt_ssl_crlpath.toUtf8().constData(), tmp_opt_ssl_crlpath_len + 1);

  int tmp_opt_ssl_key_len= ocelot_opt_ssl_key.toUtf8().size();
  ocelot_opt_ssl_key_as_utf8= new char[tmp_opt_ssl_key_len + 1];
  memcpy(ocelot_opt_ssl_key_as_utf8, ocelot_opt_ssl_key.toUtf8().constData(), tmp_opt_ssl_key_len + 1);

  int tmp_opt_ssl_mode_len= ocelot_opt_ssl_mode.toUtf8().size();
  ocelot_opt_ssl_mode_as_utf8= new char[tmp_opt_ssl_mode_len + 1];
  memcpy(ocelot_opt_ssl_mode_as_utf8, ocelot_opt_ssl_mode.toUtf8().constData(), tmp_opt_ssl_mode_len + 1);

  int tmp_plugin_dir_len= ocelot_plugin_dir.toUtf8().size();
  ocelot_plugin_dir_as_utf8= new char[tmp_plugin_dir_len + 1];
  memcpy(ocelot_plugin_dir_as_utf8, ocelot_plugin_dir.toUtf8().constData(), tmp_plugin_dir_len + 1);

  int tmp_read_default_file_len= ocelot_read_default_file.toUtf8().size();
  ocelot_read_default_file_as_utf8= new char[tmp_read_default_file_len + 1];
  memcpy(ocelot_read_default_file_as_utf8, ocelot_read_default_file.toUtf8().constData(), tmp_read_default_file_len + 1);

  int tmp_read_default_group_len= ocelot_read_default_group.toUtf8().size();
  ocelot_read_default_group_as_utf8= new char[tmp_read_default_group_len + 1];
  memcpy(ocelot_read_default_group_as_utf8, ocelot_read_default_group.toUtf8().constData(), tmp_read_default_group_len + 1);

  int tmp_server_public_key_len= ocelot_server_public_key.toUtf8().size();
  ocelot_server_public_key_as_utf8= new char[tmp_server_public_key_len + 1];
  memcpy(ocelot_server_public_key_as_utf8, ocelot_server_public_key.toUtf8().constData(), tmp_server_public_key_len + 1);

  int tmp_unix_socket_len= ocelot_unix_socket.toUtf8().size();
  ocelot_unix_socket_as_utf8= new char[tmp_unix_socket_len + 1];
  memcpy(ocelot_unix_socket_as_utf8, ocelot_unix_socket.toUtf8().constData(), tmp_unix_socket_len + 1);

  int tmp_set_charset_dir_len= ocelot_set_charset_dir.toUtf8().size();
  ocelot_set_charset_dir_as_utf8= new char[tmp_set_charset_dir_len + 1];
  memcpy(ocelot_set_charset_dir_as_utf8, ocelot_set_charset_dir.toUtf8().constData(), tmp_set_charset_dir_len + 1);

  int tmp_set_charset_name_len= ocelot_set_charset_name.toUtf8().size();
  ocelot_set_charset_name_as_utf8= new char[tmp_set_charset_name_len + 1];
  memcpy(ocelot_set_charset_name_as_utf8, ocelot_set_charset_name.toUtf8().constData(), tmp_set_charset_name_len + 1);

  int tmp_shared_memory_base_name_len= ocelot_shared_memory_base_name.toUtf8().size();
  ocelot_shared_memory_base_name_as_utf8= new char[tmp_shared_memory_base_name_len + 1];
  memcpy(ocelot_shared_memory_base_name_as_utf8, ocelot_shared_memory_base_name.toUtf8().constData(), tmp_shared_memory_base_name_len + 1);
}


int MainWindow::the_connect(unsigned int connection_number)
{
  int x;

  /* options_and_connect() cannot use QStrings because it is not in MainWindow */
  copy_connect_strings_to_utf8();

  x= options_and_connect(connection_number, ocelot_database_as_utf8);

  return x;
}


/*
  For telling the user version info of ocelotgui itself,
  and of whatever it's connected to.
  Called by action_about(), print_version().
  qVersion() is Qt runtime, maybe != QT_VERSION_STR
  For action_about() we go further and try to say what we're connected
  to, but for --version, we aren't connecting so such info wouldn't be
  available.
*/
QString MainWindow::get_version()
{
  QString s;
  s = "\n\nocelotgui version ";
  s.append(ocelotgui_version);
#ifdef OCELOT_OS_LINUX
  s.append(", for Linux");
#endif
#ifdef OCELOT_OS_NONLINUX
  s.append(", for Windows");
#endif
#if __x86_64__
  s.append(" (x86_64)");
#endif
  s.append(" using Qt version ");
  s.append(qVersion());
  s.append("\n");
  return s;
}

/* --version, or version in an option file, causes version display and exit */
void MainWindow::print_version()
{
  QString QStr= get_version();
  int tmp_len= QStr.toUtf8().size();
  char *tmp= new char[tmp_len + 1];
  memcpy(tmp, QStr.toUtf8().constData(), tmp_len + 1);
  printf("%s", tmp);
  delete []tmp;
  return;
}

/* --help, or help in an option file, causes help display and exit */
void MainWindow::print_help()
{
  char output_string[5120];

  print_version();
  printf("Copyright (c) 2014-2017 by Ocelot Computer Services Inc. and others\n");
  printf("\n");
  printf("Usage: ocelotgui [OPTIONS] [database]\n");
  printf("Options files that were actually read:\n");
  strcpy(output_string, options_files_read.toUtf8());
  printf("%s\n", output_string);
  printf("Options files groups that ocelotgui looks for: client mysql ocelot\n");
  printf("Possible option values: same as possible option values for mysql client\n");
  printf("Option values after reading options files and command-line arguments:\n");
  printf("Option                            Value\n");
  printf("--------------------------------- ----------------------------------------\n");
  action_connect_once("Print");
}

QStringList debug_routine_list_schemas;
QStringList debug_routine_list_names;
QStringList debug_routine_list_types;
QStringList debug_routine_list_sql_modes;
QStringList debug_routine_list_surrogates;
QStringList debug_routine_list_texts;
int debug_v_statement_number; /* the number for all routines */
QString debug_xxxmdbug_icc_core_surrogate_name;
QStringList c_variable_names;  /* (declared) in reverse order */
QStringList c_variable_tokens; /* (declared) in reverse order */
QStringList debug_tmp_user_variables;
QString debug_v_g;
QString debug_xxxmdbug_debugger_name;
QString debug_xxxmdbug_debugger_version;
int debug_xxxmdbug_signal_errno;
QString debug_xxxmdbug_timestamp;
QString debug_xxxmdbug_setup_group_name;
int i_of_start_of_parameters, i_of_end_of_parameters;
QStringList debug_label_list;
QString debug_xxxmdbug_default_schema;
int debug_track_statements;
int debug_track_user_variables;
int debug_track_declared_variables;
bool debug_ansi_quotes;
QString debug_lf;
QString debug_plugins;
QString debug_session_sql_mode_original;
QString debug_session_sql_mode_after_last_change;
int debug_routine_list_size;
bool is_plsql;

/*
  When NEW_SETUP == 1, which is default since May 2018,
  we handle $setup from new code in ocelotgui.cpp due to the
  changes in MySQL 8.0. Except for debug_setup_go, all the new
  routines begin with "setup_".
  All setup_* functions, on error, should call debug_error() and return <> 0.
*/

#if (NEW_SETUP == 1)
/*
  We expect hparse to catch syntax errors so checks here are minimal.
*/
/* The 'track' switches exist so that one can reduce the amount of instrumenting,
   for space or speed reasons.
   @xxxmdbug_track_statements:            0=none, 1=base set, 2=+iterate/leave/set. default: 2.
   @xxxmdbug_track_user_variables:        0=none, 1=in same routine, 2=all (todo: allow 2). default=1.
   @xxxmdbug_track_declared_variables     0=none, 1=since last declare, 2=all. default: 2.
   Future switch: overwrite
   We are parsing these switches, but they are undocumented.
*/
/*
  Todo: New syntax: $SETUP ... [procedure|function|trigger|event] name.
                    and LIKE | = 'name'
  Todo: Are you sure $setup is starting at 0??
  Todo: Are you okay if there's a delimiter?
  Todo: I do case-insensitive comparisons. That's always okay for
        routine | declared variable | event, but not if
        @@lower_case_table_names == 0 and database | trigger | table.
*/
/*
  Todo: get_lock('xxxmdbug_lock') -- but in the past this has failed.
        Anyway conflict is unlikely unless two users get the same
        group name.
  Todo: I don't think locking is necessary. But check at the end, or
        even at $debug time, whether all routines are current.
        That is: did anyone change or drop a routine that's in the log?
                 or change our access rights to the surrogate routines?
        If so, $setup is probably invalid, return "failed".
  Todo: debug_privilege_check() should include whether we can read the
        information_schema.routines table for MySQL 8.0, but if we can't
        then that will be clear because setting up routine lists fails.
*/
void MainWindow::debug_setup_go(QString text)
{
  if (ocelot_statement_syntax_checker.toInt() < 1)
  {
    debug_error((char*)"Before running $setup you must say 'set ocelot_statement_syntax_checker=1;' or 'set ocelot_statement_syntax_checker=3;'");
    return;
  }
  if (hparse_errno != 0)
  {
    char copy_of_hparse_errmsg[1024];
    strcpy(copy_of_hparse_errmsg, "$setup cannot proceed due to syntax error: ");
    strcat(copy_of_hparse_errmsg, hparse_errmsg);
    debug_error((char*)copy_of_hparse_errmsg);
    return;
  }
  if (setup_initialize_variables()) return;
  /* parse the switches */
  debug_track_statements= 2;                        /* default */
  debug_track_user_variables= 1;                    /* default */
  debug_track_declared_variables= 2;                /* default */
  QString switch_name= "";
  bool is_switch= false;

  int last_token= main_token_number + main_token_count_in_statement;
  for (int i= main_token_number;
       ((main_token_lengths[i] != 0) && (i < last_token));
       ++i)
  {
    QString s= text.mid(main_token_offsets[i], main_token_lengths[i]);
    if (s == "-")
    {
      is_switch= true;
      continue;
    }
    if ((main_token_types[i] == TOKEN_TYPE_IDENTIFIER) && (is_switch))
    {
      switch_name= s;
      continue;
    }
    if (is_switch)
    {
      if ((main_token_types[i] == TOKEN_TYPE_LITERAL_WITH_DIGIT)
       || (main_token_types[i] == TOKEN_TYPE_LITERAL))
      {
        QString p= switch_name.toUpper();
        int j= s.toInt();
        if (p == "TRACK_STATEMENTS") debug_track_statements= j;
        else if (p == "TRACK_USER_VARIABLES") debug_track_user_variables= j;
        else if (p == "TRACK_DECLARED_VARIABLES") debug_track_declared_variables= j;
        is_switch= false;
      }
    }
  }

  QString qstring_error_message;
  //QString command_string;
  /* Todo: use debug_error instead, provided debug_error gets rid of any problematic ''s */
  qstring_error_message= debug_privilege_check(TOKEN_KEYWORD_DEBUG_SETUP);
  if (qstring_error_message != "")
  {
    char command_string[512];
    strcpy(command_string, qstring_error_message.toUtf8());
    if (debug_error(command_string) != 0) return; /* setup wouldn't be able to operate so fail */
  }
  int r= setup_internal(text);
  if (r == 1)
  {
    setup_cleanup();
    /* I assume debug_error() has set up a good error message. */
    return;
  }
  /* Todo: Return how many routines you found. */

  /* Repetition of some code in put_diagnostics_in_result */
  char elapsed_time_string[50];
  {
    qint64 statement_end_time= QDateTime::currentMSecsSinceEpoch();
    qint64 elapsed_time= statement_end_time - statement_edit_widget->start_time;
    long int elapsed_time_as_long_int= (long int) elapsed_time;
    float elapsed_time_as_float= (float) elapsed_time_as_long_int / 1000;
    sprintf(elapsed_time_string, " (%.1f seconds)", elapsed_time_as_float);
  }
  QString s1= QString("OK ");
  s1.append(QString::number(debug_routine_list_size));
  s1.append(" surrogate routines. ");
  s1.append(elapsed_time_string);
  statement_edit_widget->result= s1;
  setup_cleanup();
  return;
}
#endif


/*
  $setup should call this wrapper when it needs mysql_real_query(),
  so that if there is a failure it can find it, prefix it with the
  error_prefix string, put it in the result, and erturn 1.
  We also put it in the log what the query was, with a low number.
  Todo: Error strings should be in ostrings.h.
*/
int MainWindow::setup_mysql_real_query(char *statement,
                                             char *error_prefix)
{
  int statement_len= strlen(statement);
  if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], statement, statement_len))
  {
    QString e= error_prefix;
    char debuggee_state_error[STRING_LENGTH_512];
    strncpy(debuggee_state_error, lmysql->ldbms_mysql_error(&mysql[MYSQL_MAIN_CONNECTION]), STRING_LENGTH_512 - 1);
    e= e + debuggee_state_error;
    put_message_in_result(e);
    log(error_prefix, 60);
    log(statement, 60);
    return 1;
  }
  return 0;
}

/*
  Make the routine_list_* QStringlists from the $SETUP arguments.
  Todo: This won't work for names that contain 's.
  Exclude if:
  * Language <> 'SQL'
    That's mysql.proc.language, an approximation is external_language.
    This might be important if external languages become okay.
    But it won't help with MariaDB, PL/SQL doesn't seem to be flagged.
    In MySQL expect 'SQL', in MariaDB expect NULL.
  * % wildcard for schema, and system database
    That is, mysql, information_schema, performance_schema, xxxmdbug.
  * % wildcard for name, and name like 'xxxmdbug___%'
    Quis custodiet ipsos debuggeros?
*/
/*
  We use COLLATE utf8_general_ci clauses in some comparisons, to avoid
  incompatible-mix-of-collations errors due to a MySQL 8.0 change.
  In MySQL 8.0: routine_catalog and routine_schema are utf8_tolower_ci,
  routine_name is utf8_general_ci. In earlier versions and in
  MariaDB 10.3: they are all utf8_general_ci.
*/
/* Todo:
   Ignore routines whose surrogate names would be too long.
   Todo: put a notice in setup_log.
   IF LENGTH(mysql_proc_name)+LENGTH('xxxmdbugxxxP')>64 THEN ITERATE x1; END IF;
*/
/*
  Todo: DEBUG_TAB_WIDGET_MAX is a maximum caused by our desire not to
  have too many tabs when $debug happens. But you could try to fix that
  some other way, e.g. allow users to change the maximum, or dynamically
  shift tabs in and out from the front on a last-seen basis.
*/

int MainWindow::setup_routine_list(QString text)
{
  debug_routine_list_schemas.clear();
  debug_routine_list_names.clear();
  debug_routine_list_types.clear();
  debug_routine_list_sql_modes.clear();
  debug_routine_list_surrogates.clear();
  debug_routine_list_texts.clear();
  debug_tmp_user_variables.clear();
  debug_routine_list_size= 0;
  QString s;
  QString token;
  QString schema_name= debug_xxxmdbug_default_schema;
  QString routine_name= "";
  char select_statement[2048];
  int i;
  for (i= 0; main_token_lengths[i] != 0; ++i)
  {
    if (main_token_types[i] == TOKEN_TYPE_IDENTIFIER)
    {
      token= text.mid(main_token_offsets[i], main_token_lengths[i]);
      if (main_token_reftypes[i] == TOKEN_REFTYPE_DATABASE)
      {
        schema_name= token.replace("_", "\\_");
      }
      else if (main_token_reftypes[i] == TOKEN_REFTYPE_SWITCH_NAME) {;}
      else
      {
        routine_name= token.replace("_", "\\_"); /* _ is a wildcard so escape it */
        s= "SELECT routine_schema,routine_name,routine_type,sql_mode " ;
        s= s + "FROM information_schema.routines WHERE ";
        s= s + "routine_schema COLLATE utf8_general_ci LIKE '" + connect_stripper(schema_name, false) + "' ";
        s= s + "AND routine_name LIKE '" + connect_stripper(routine_name, false) + "'";
        s= s + " AND (external_language IS NULL OR external_language='SQL')";
        if (schema_name.contains("%"))
        {
          s= s + " AND routine_schema COLLATE utf8_general_ci <> 'information_schema'"
               + " AND routine_schema COLLATE utf8_general_ci <> 'performance_schema'"
               + " AND routine_schema COLLATE utf8_general_ci <> 'mysql'"
               + " AND routine_schema COLLATE utf8_general_ci <> 'xxxmdbug'";
        }
        if (routine_name.contains("%"))
        {
          s= s + " AND routine_name NOT LIKE 'xxxmdbug___%'";
        }
        s= s + ";";
        strcpy(select_statement, s.toUtf8());
        MYSQL_RES *res= NULL;
        MYSQL_ROW row= NULL;
        if (setup_mysql_real_query(select_statement,
                                   (char*)"FAILED. Cannot make a list of routines. ") == 1)
          return 1;
        res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
        if (res == NULL)
        {
          debug_error((char*)"mysql_store_result -- FAILED. Cannot make a list of routines.");
          return 1;
        }
        int counter= 0;
        {
          for (;;)
          {
            row= lmysql->ldbms_mysql_fetch_row(res);
            if (row == NULL) break;
            /* TODO: connect_stripper()? */
            QString t_schema= QString::fromUtf8(row[0]);
            QString t_name= QString::fromUtf8(row[1]);
            QString t_type= QString::fromUtf8(row[2]);
            QString t_sql_mode= QString::fromUtf8(row[3]);
            int name_offset= setup_find(t_schema, t_name);
            if (name_offset >= 0)
            {
              if (debug_routine_list_types.at(name_offset) != t_type)
              {
                debug_error((char*)"Two routines have the same name but different types");
                if (res != NULL) lmysql->ldbms_mysql_free_result(res);
                return 1;
              }
              continue; /* ignore duplicates of the same type */
            }
            /* Todo: Check you don't make the name too long */
            debug_routine_list_schemas << t_schema;
            debug_routine_list_names << t_name;
            debug_routine_list_types << t_type;
            debug_routine_list_sql_modes << t_sql_mode;
            QString v_routine_identifier_of_surrogate;
            v_routine_identifier_of_surrogate= "xxxmdbug"
                    + debug_xxxmdbug_setup_group_name
                    + t_type.mid(0,1)
                    + connect_stripper(t_name, false);
            debug_routine_list_surrogates << v_routine_identifier_of_surrogate;
            ++counter;
          }
        }
        if (res != NULL) lmysql->ldbms_mysql_free_result(res);
        if (counter == 0)
        {
          char c_schema_name[256];
          strcpy(c_schema_name, schema_name.toUtf8());
          char c_routine_name[256];
          strcpy(c_routine_name, routine_name.toUtf8());
          char q[512];
          strcpy(q, "FAILED. Could not find routine ");
          strcat(q, c_schema_name);
          strcat(q, ".");
          strcat(q, c_routine_name);
          debug_error(q);
          return 1;
        }
        schema_name= debug_xxxmdbug_default_schema;
      }
    }
  }
  debug_routine_list_size= debug_routine_list_names.count();
  if (debug_routine_list_size > DEBUG_TAB_WIDGET_MAX)
  {
    char q[256];
    sprintf(q, "Error: Number of routines = %d. The maximum number of routines in a single $setup is %d", debug_routine_list_size, DEBUG_TAB_WIDGET_MAX);
    debug_error((char*)q);
    return 1;
  }
  return 0;
}

/*
  Pass: schema+name.
  Return: offset of schema+name (both must match) in QStringLists.
          or -1 if not found.
  This is an odd case where "return 1" does not mean "error".
  Todo: test with delimited names.
*/
int MainWindow::setup_find(QString t_schema, QString t_name)
{
  int i= 0;
  for (; i < debug_routine_list_schemas.count(); ++i)
  {
    if (QString::compare(debug_routine_list_names.at(i), t_name, Qt::CaseInsensitive) == 0)
    {
      if (QString::compare(debug_routine_list_schemas.at(i), t_schema, Qt::CaseInsensitive) == 0)
        return i;
    }
  }
  return -1;
}

/*
  Generate the surrogate for one routine, specified by routine_number.
  Todo: Make sure that any global checks/setups come before this.
*/
/*
   Todo: see if this is happening ...
   Reset tokens+variables+statements tables for each routine.
   One effect is that @user_variables are only tracked in routines where
   they are mentioned, and we might want to change that if a non-default
   setup switch value is used i.e. track_user_variables=2.
*/

int MainWindow::setup_generate(int routine_number)
{
  debug_label_list.clear();
  if (debug_track_user_variables == 1) debug_tmp_user_variables.clear();
  QString s;
  bool pushed_sql_mode_ansi_quotes;
  bool pushed_hparse_sql_mode_ansi_quotes;
  QString routine_schema= debug_routine_list_schemas.at(routine_number);
  QString routine_name= debug_routine_list_names.at(routine_number);
  QString routine_type= debug_routine_list_types.at(routine_number);
  QString routine_sql_mode= debug_routine_list_sql_modes.at(routine_number);
  QString routine_surrogate= debug_routine_list_surrogates.at(routine_number);

  /* Todo: Put the names inside ``s. */
  /* Todo: check: what if there are nulls, will select_1_row() fail? */
  {
    char select_1_row_arg[1024];
    strcpy(select_1_row_arg, "SHOW CREATE ");
    strcat(select_1_row_arg, routine_type.toUtf8());
    strcat(select_1_row_arg, " `");
    strcat(select_1_row_arg, routine_schema.toUtf8());
    strcat(select_1_row_arg,"`.`");
    strcat(select_1_row_arg, routine_name.toUtf8());
    strcat(select_1_row_arg,"`;");
    s= select_1_row(select_1_row_arg);
    if (s != "")
    {
      strcat(select_1_row_arg, "-- FAILED so cannot generate surrogates");
      debug_error((char*)select_1_row_arg);
      return 1;
    }
  }
  /* Now select_1_row_result_3 has the procedure text. */
  /* Todo: this duplicates stuff in action_statement_edit_widget_text_changed */
  debug_v_g= "";
  QString text;
  int i;
  QString mysql_proc_db= routine_schema;
  QString p_routine_identifier= routine_name;
  bool is_schema_seen= false;

  /* NB: after this push do not "return 1" without popping */
  main_token_push();
  pushed_sql_mode_ansi_quotes= sql_mode_ansi_quotes;
  pushed_hparse_sql_mode_ansi_quotes= hparse_sql_mode_ansi_quotes;

  if (routine_sql_mode.contains("ANSI_QUOTES",Qt::CaseInsensitive))
  {
    debug_ansi_quotes= true;
  }
  else
  {
    debug_ansi_quotes= false;
  }
  hparse_sql_mode_ansi_quotes= debug_ansi_quotes;

  /* Todo: There's no need to copy, we could use select_1_row_result_3 */
  text= select_1_row_result_3;
  /* SHOW CREATE doesn't show ; but maybe I can't depend on that. */
  if (text.right(1) != ";") text= text + ";";

  main_token_new(text.size());
  tokenize(text.data(),
           text.size(),
           main_token_lengths, main_token_offsets, main_token_max_count,
           (QChar*)"33333", 1, ocelot_delimiter_str, 1);
  tokens_to_keywords(text, 0, sql_mode_ansi_quotes);

  if (routine_sql_mode.contains("ORACLE",Qt::CaseInsensitive)) is_plsql= true;
  else is_plsql= false;

  /*
    Workaround for a bug in MariaDB 10.3 / PLSQL.
    If, before IS|AS|RETURN, you see (): remove () and try again.
  */
  if (is_plsql)
  {
    bool bug= false;
    int offset1;
    int offset2;
    int i;
    for (i= 0; main_token_lengths[i] != 0; ++i)
    {
      int token_type= main_token_types[i];
      if ((token_type == TOKEN_KEYWORD_IS)
       || (token_type == TOKEN_KEYWORD_AS)
       || (token_type == TOKEN_KEYWORD_BEGIN)
       || (token_type == TOKEN_KEYWORD_RETURN))
        break;
    }
    for (int j= 0; j < i; ++j)
    {
      offset1= main_token_offsets[j];
      offset2= main_token_offsets[j + 1];
      QString token1= text.mid(offset1, main_token_lengths[j]);
      QString token2= text.mid(offset2, main_token_lengths[j + 1]);
      if ((token1 == "(") && (token2 == ")")) bug= true;
    }
    if (bug)
    {
      QString text_before_parentheses= text.mid(0, offset1 - 1);
      QString text_after_parentheses= text.mid(offset2 - 1);
      text= text_before_parentheses + "  " + text_after_parentheses;
      tokenize(text.data(),
               text.size(),
               main_token_lengths, main_token_offsets, main_token_max_count,
               (QChar*)"33333", 1, ocelot_delimiter_str, 1);
      tokens_to_keywords(text, 0, sql_mode_ansi_quotes);
    }
  }

  hparse_f_multi_block(text); /* recognizer */
  /* Todo: We only check hparse_errno. hparse_errno_count maybe > 0. */
  if (hparse_errno != 0)
  {
    debug_error((char*)"ocelotgui failed to parse the routine body");
    goto pop_and_return_error;
  }
  /* Successful parse. Now we can start the fiddling. */
  i= 0;

  /* Copy CREATE DEFINER ... PROCEDURE|FUNCTION name, maybe add schema */
  for (; main_token_lengths[i] != 0; ++i)
  {
    QString d= text.mid(main_token_offsets[i], main_token_lengths[i]);
    if (main_token_reftypes[i] == TOKEN_REFTYPE_DATABASE) is_schema_seen= true;
    if ((main_token_reftypes[i] == TOKEN_REFTYPE_PROCEDURE)
     || (main_token_reftypes[i] == TOKEN_REFTYPE_FUNCTION))
    {
      if (is_schema_seen == false) debug_v_g= debug_v_g + routine_schema + ".";
      if (setup_append(routine_surrogate, text, i)) goto pop_and_return_error;
      break;
    }
    else if (setup_append(d, text, i)) goto pop_and_return_error;
  }
  ++i;
  /* Todo: error if main_token_lengths[i] == 0 */
  /* Todo: call find_start_of_body() to find where routine really starts? */
  /* TODO: Assumption is wrong,
     CREATE TRIGGER or CREATE EVENT do not have parameter lists. */
  /* Assume always there is (parameter list). Where does it end? */
  {
    int parentheses_count= 0;
    bool is_parenthesis_seen= false;
    QString token;
    int j;
    for (j= i; main_token_lengths[j] != 0; ++j)
    { 
      if (is_plsql)
      {
        if ((main_token_types[j] == TOKEN_KEYWORD_AS)
         || (main_token_types[j] == TOKEN_KEYWORD_IS))
          break;
      }
      if (main_token_types[j] == TOKEN_TYPE_OPERATOR)
      {
        token= text.mid(main_token_offsets[j], main_token_lengths[j]);
        if (token == "(")
        {
          if (is_parenthesis_seen == false) i_of_start_of_parameters= j + 1;
          ++parentheses_count;
          is_parenthesis_seen= true;
        }
        if (token == ")")
        {
          --parentheses_count;
          if (parentheses_count == 0) break;
        }
      }
    }
    i_of_end_of_parameters= j;
  }
  if (setup_insert_into_variables_user_variables(text, i_of_end_of_parameters) == 1) return 1;
  /* Skip past the parameter list. */
  /* In fact skip all the way till the first real statement appears. */
  {
    int i_of_statement;
    for (i_of_statement= i_of_end_of_parameters;
         (main_token_flags[i_of_statement] & TOKEN_FLAG_IS_START_STATEMENT) == 0;
         ++i_of_statement)
    {
      if (main_token_lengths[i_of_statement] == 0)
      {
        debug_error((char*)"found no statements"); /* pseudo-assertion */
        goto pop_and_return_error;
      }
    }

    int j= main_token_offsets[i];
    QString d= text.mid(j, main_token_offsets[i_of_statement] - j);
    if (setup_append(d, text, i)) goto pop_and_return_error;
    i= i_of_statement;
  }

  if (setup_generate_starter(mysql_proc_db,
                         p_routine_identifier,
                         routine_type,
                         text)) goto pop_and_return_error;
  if (setup_generate_statements(i,
                            text,
                            routine_number)) goto pop_and_return_error;
  setup_generate_ender();
  /* Now you have a new CREATE statement in debug_v_g for a surrogate. */
  debug_routine_list_texts << debug_v_g;
  main_token_pop();
  sql_mode_ansi_quotes= pushed_sql_mode_ansi_quotes;
  hparse_sql_mode_ansi_quotes= pushed_hparse_sql_mode_ansi_quotes;
  return 0;
pop_and_return_error:
  main_token_pop();
  sql_mode_ansi_quotes= pushed_sql_mode_ansi_quotes;
  hparse_sql_mode_ansi_quotes= pushed_hparse_sql_mode_ansi_quotes;
  return 1;
}

int MainWindow::setup_append(QString d, QString text, int i)
{
  debug_v_g.append(d);
  int j= main_token_offsets[i] + main_token_lengths[i];
  assert(main_token_offsets[i + 1] >= j);
  debug_v_g.append(text.mid(j, main_token_offsets[i + 1] - j));
  return 0;
}

/* called from generate_starter().
   Job: make a string from the variables table.
   For example: '2,a. int;13,b. int;'
   Todo: don't need to start at 0, could start after routine name.
   Todo: This should be useful when creating the variables table.
   Todo: Actually the variables table can include @variables!
         Data type will not be known.
   Todo: You have to get the WHOLE data type, but replace 's in ENUM
         or SET with ''s.
   Todo: No idea how to handle MariaDB 10's ROW TYPE OF.
*/
int MainWindow::setup_generate_routine_entry_parameter(QString text)
{
  int v_token_number_of_declare= 0;
  QString v_variable_identifier;
  QString v_data_type;
  int i;
  for (i= 0; main_token_lengths[i] != 0; ++i)
  {
    if (main_token_types[i] == TOKEN_KEYWORD_DECLARE)
    {
      v_token_number_of_declare= i - i_of_start_of_parameters;
    }
    if ((main_token_reftypes[i] == TOKEN_REFTYPE_VARIABLE_DEFINE)
     || (main_token_reftypes[i] == TOKEN_REFTYPE_PARAMETER_DEFINE))
    {
      if (setup_row_type(i) == TOKEN_KEYWORD_ROW) continue;
      v_variable_identifier= text.mid(main_token_offsets[i], main_token_lengths[i]);
      v_data_type= "";
      for (int j= i + 1; main_token_lengths[j] != 0; ++j)
      {
        /* Todo: Add error check: no data type, or unknown data type */
        if ((main_token_flags[j] & TOKEN_FLAG_IS_DATA_TYPE) != 0)
        {
          v_data_type= "";
          int parentheses_count= 0;
          for (int k= j; main_token_lengths[k] != 0; ++k)
          {
            QString s= text.mid(main_token_offsets[k], main_token_lengths[k]);
            if (s == "(") ++parentheses_count;
            if (s == ")")
            {
              --parentheses_count;
              if (parentheses_count < 0) break;
            }
            if (parentheses_count == 0)
            {
              if ((s == ";")
               || (s == ",")
               || (QString::compare(s, "DEFAULT", Qt::CaseInsensitive) == 0))
                break;
            }
            if (v_data_type != "") v_data_type= v_data_type + " ";
            if (s.mid(0,1) == "'") s= "'" + s + "'";
            v_data_type= v_data_type + s;
          }
          break;
        }
      }
      debug_v_g= debug_v_g
              + QString::number(v_token_number_of_declare)
              + "*"
              + setup_add_delimiters(v_variable_identifier)
              + "*"
              + v_data_type
              + ";";
    }
  }
  return 0;
}

int MainWindow::setup_generate_starter(QString mysql_proc_db,
                                        QString p_routine_identifier,
                                        QString mysql_proc_type,
                                        QString text)
{
  QString debug_v_generated_time= debug_xxxmdbug_timestamp;
  /* We will use the contents of xxxmdbug_comment to find surrogates, or to check version number. So do not change here! */
  if (is_plsql)
  {
    debug_v_g= debug_v_g + "xxxmdbug_5678 EXCEPTION;" + debug_lf;
  }
  else
  {
    debug_v_g= debug_v_g + debug_lf + "BEGIN ";
    debug_v_g= debug_v_g + debug_lf + "DECLARE ";
  }
  debug_v_g= debug_v_g + "xxxmdbug_comment VARCHAR(1000) DEFAULT";
  debug_v_g= debug_v_g + debug_lf + "'Surrogate routine for `" + mysql_proc_db + "`.`" + p_routine_identifier + "`";
  debug_v_g= debug_v_g + debug_lf + "Generated by " + debug_xxxmdbug_debugger_name + " Version " + debug_xxxmdbug_debugger_version;
  debug_v_g= debug_v_g + debug_lf + "Generated on " + debug_v_generated_time + "';";
  if (is_plsql)
  {
    debug_v_g= debug_v_g + debug_lf + "BEGIN " + debug_lf;
  }
  else
  /* Handling the signal number. If plsql this will be done at the end. */
  {
    debug_v_g= debug_v_g + debug_lf + "DECLARE EXIT HANDLER FOR " + QString::number(debug_xxxmdbug_signal_errno) + " BEGIN CALL xxxmdbug.routine_exit(); RESIGNAL; END;";
    debug_v_g= debug_v_g + debug_lf + "DECLARE EXIT HANDLER FOR SQLEXCEPTION BEGIN CALL xxxmdbug.routine_exit(); RESIGNAL; END;";
  }
  /* "call routine_entry(variable-list,command-list)". includes "Insert Into Call stack". */
  debug_v_g= debug_v_g + debug_lf + "CALL xxxmdbug.routine_entry("
                  + "'`" + mysql_proc_db + "`',"
                  + "'`" + p_routine_identifier + "`',"
                  + "'" + mysql_proc_type + "',"
                  + "'" + debug_xxxmdbug_debugger_version + "',"
                  + "'" + debug_v_generated_time + "','";
  if (setup_generate_routine_entry_parameter(text)) return 1;
  debug_v_g= debug_v_g + "');" + debug_lf;
  /* TODO: routine_entry() should have another parameter for pre-set commands. */

  /* A breakpoint on line 0 is possible. There are no settable variables,
     but the user can see what the routine is, and can set breakpoints."
     breakpoint_check() always stops for line 0 right after 'debug' command. */
  debug_v_g= debug_v_g + debug_lf + "IF xxxmdbug.is_debuggee_and_is_attached()=1 THEN CALL xxxmdbug."
                  + debug_xxxmdbug_icc_core_surrogate_name
                  + "(0);"
                  + "END IF;" + debug_lf;
  return 0;
}

/*
   "Generate: ender": Called from generate(). Was: generate_ender().
   We call this for the last line of a routine.
   We also change call stack (without calling this) for special handling of RETURN.
   routine_exit() should cause "Delete From Call Stack" */
/* in plsql LEAVE x won't work if x is a label before a BEGIN,
   so we put a label at the end for which we can say GOTO */
void MainWindow::setup_generate_ender()
{
  if (is_plsql)
    debug_v_g= debug_v_g + "<<xxxmdbug_routine_exit>>";
  debug_v_g= debug_v_g
           + "\nCALL xxxmdbug.routine_exit();" + debug_lf;
  /* Handling the signal number. If sql/psm this was done at the start. */
  /* Todo: Don't generate "EXCEPTION " if EXCEPTION statement exists. */
  if (is_plsql)
  {
    debug_v_g= debug_v_g
           + "EXCEPTION "
           + "WHEN xxxmdbug_5678 THEN BEGIN RAISE xxxmdbug_5678; END;"
           + debug_lf;
  }
  debug_v_g= debug_v_g
          + "END;" + debug_lf;
}

/*
  Find statements and generate for them.
  Beware, sometimes flow-control statements start with labels.
  Actually I think start_statement flag is on if it's label_define.
  If i_end_of_parameters is at ")" then it won't be shown, but if it
  is at "AS" (as happens with plsql) then it will be shown so ++count.
  Todo: some of the parameters can be calculated from routine_number.
*/
int MainWindow::setup_generate_statements(int i,
                                           QString text,
                                           int routine_number)
{
  int v_statement_number_within_routine= 0;
  int v_line_number_of_start_of_first_token;
  int v_token_number_of_last_token;
  for (; main_token_lengths[i] != 0; ++i)
  {
    QString d= text.mid(main_token_offsets[i], main_token_lengths[i]);
    if ((main_token_flags[i] & TOKEN_FLAG_IS_START_STATEMENT)
     || (main_token_reftypes[i] == TOKEN_REFTYPE_LABEL_DEFINE))
    {
      ++debug_v_statement_number;
      ++v_statement_number_within_routine;
      if (main_token_reftypes[i] == TOKEN_REFTYPE_LABEL_DEFINE)
      {
        int j= main_token_offsets[i];
        ++i;
        for (; ; ++i)
        {
          if (main_token_flags[i] & TOKEN_FLAG_IS_START_STATEMENT) break;
        }
        QString s= text.mid(j, main_token_offsets[i] - j);
        debug_v_g.append(s);
      }
      {
        int j= main_token_offsets[i_of_end_of_parameters];
        int k= main_token_offsets[i];
        QString s= QString::number(i) + "*" + text.mid(j, k - j);
        v_line_number_of_start_of_first_token= s.count(debug_lf);
        int t= main_token_types[i_of_end_of_parameters];
        if ((t == TOKEN_KEYWORD_AS) || (t == TOKEN_KEYWORD_IS))
          ++v_line_number_of_start_of_first_token;
      }
      //v_character_number_of_start_of_first_token= main_token_offsets[i];
      for (int j= i + 1; ; ++j)
      {
        if ((main_token_flags[j] & TOKEN_FLAG_IS_START_STATEMENT)
         || (main_token_reftypes[i] == TOKEN_REFTYPE_LABEL_DEFINE)
         || (main_token_lengths[j] == 0))
        {
          v_token_number_of_last_token= j - 1;
          break;
        }
      }
      if (setup_generate_label(i, text, v_statement_number_within_routine)) return 1;

      if (main_token_flags[i] & TOKEN_FLAG_IS_DEBUGGABLE)
      {
        /* Todo: generate per-statement stuff, this is executable */
        /* We'll set is_leave_possible later. */
        if (setup_generate_statements_debuggable(i,
                                             v_line_number_of_start_of_first_token,
                                             v_statement_number_within_routine,
                                             text,
                                             v_token_number_of_last_token,
                                             routine_number)) return 1;
      }
      else
      {
        if (setup_generate_statement_text(i,
                                      text,
                                      v_token_number_of_last_token,
                                      routine_number)) return 1;
      }
      i= v_token_number_of_last_token;
    }
  }
  return 0;
}

/*
   Initialize some variables. Called from setup(), command(), and become_debuggee_connection().
   Start with some error checks.
   Any names that start with @xxxmdbug are reserved.
   Some of the variables set in this procedure are "constants".
   In debug_parse_statement() there is also some initialization,
   but $setup no longer calls that.
*/
/* For debugger purposes, the pipe prefix forms are:
   'xxxmdbug_' + @xxxmdbug_prefix_endxxx e.g. 'B '
   The 'R ' messages are status sendings which the debugger may or may not choose to receive (the debuggee may overwrite them).
   The @xxxmdbug_prefix_end_for_rxxx messages are responses to specific questions which the debugger is expecting.
*/
/* Todo: why not find out current sql_mode too? */
int MainWindow::setup_initialize_variables()
{
  debug_lf = "\n"; /* constant: Line Feed. add carriage return if desired */
  debug_xxxmdbug_debugger_name= "OCELOTGUI";
  debug_xxxmdbug_debugger_version= ocelotgui_version;
  debug_xxxmdbug_signal_errno= 5678;

  /* Todo: make this call along with other things you need server for. */
  /* Todo: watch out re group_concat_max_len */
  char query[]="SELECT current_timestamp(),database(),group_concat(plugin_name ORDER BY plugin_name), @@session.sql_mode FROM information_schema.plugins;";
  QString s= select_1_row(query);
  if (s != "")
  {
    char error_message[1024];
    strcpy(error_message, "Failed to execute: ");
    strcat(error_message, query);
    debug_error(error_message);
    return 1;
  }

  debug_xxxmdbug_timestamp= select_1_row_result_1;
  debug_xxxmdbug_default_schema= select_1_row_result_2;
  debug_plugins= select_1_row_result_3;
  debug_session_sql_mode_original= select_1_row_result_4;
  debug_session_sql_mode_after_last_change= debug_session_sql_mode_original;
  return 0;
}

/*
   routine_identifier_of_surrogate = xxxmdbug xxx type routine_identifier
   where xxx = set name = a 3-character value with character set { 0-9 A-Z },
   up to 36 * 36 * 36 = 46656 possibilities, and it goes up every time we call setup().
   where type = first letter of mysql.proc.type so 'P' for procedure and 'F' for function.
   This format is expected by debuggee_wait_loop().
   In get_set_setup_group_name() all we do is get the 3-character string, @xxxmdbug_setup_group_name
   Todo: explain what to do if set name reaches 'ZZZ'.
   Todo: Get group name again when you're ready to write.
         If it's not the same, debug_error("Concurrency error").
*/
int MainWindow::setup_get_setup_group_name()
{
  int vxxx_as_number;
  int v_digit1,v_digit2,v_digit3;
  QString v_max_name= "";
  QString v_max_name_in_setup_log= "";
  QString s;
  s= select_1_row("SELECT MAX(routine_name) FROM information_schema.routines WHERE routine_name LIKE 'xxxmdbug____%';");
  if (s != "")
  {
    debug_error((char*)"Failed to read information_schema.routines");
    return 1;
  }
  v_max_name= select_1_row_result_1.toUpper();
  s= select_1_row("SELECT MAX(surrogate_routine_identifier) FROM xxxmdbug.setup_log WHERE surrogate_routine_identifier LIKE 'xxxmdbug____%';");
  if (s != "")
  {
    debug_error((char*)"Failed to read xxxmdbug.setup_log");
    return 1;
  }
  v_max_name_in_setup_log= select_1_row_result_1.toUpper();
  if ((v_max_name_in_setup_log > v_max_name)
   || (v_max_name == ""))
    v_max_name= v_max_name_in_setup_log;
  if (v_max_name == "") debug_xxxmdbug_setup_group_name= "000";
  else
  {
    debug_xxxmdbug_setup_group_name= v_max_name.mid(strlen("xxxmdbug"), 3);
    if (debug_xxxmdbug_setup_group_name == "ZZZ")
    {
      debug_error((char*)"Too many sets of surrogates, cleanup necessary");
      return 1;
    }
    char digits[4];
    strcpy(digits, debug_xxxmdbug_setup_group_name.mid(0, 1).toUtf8());
    v_digit1= (int)digits[0];
    if (v_digit1 <= 57) v_digit1-= 48; else v_digit1 -= (65 - 10);
    strcpy(digits, debug_xxxmdbug_setup_group_name.mid(1, 1).toUtf8());
    v_digit2= (int)digits[0];
    if (v_digit2 <= 57) v_digit2-= 48; else v_digit2 -= (65 - 10);
    strcpy(digits, debug_xxxmdbug_setup_group_name.mid(2, 1).toUtf8());
    v_digit3= (int)digits[0];
    if (v_digit3 <= 57) v_digit3-= 48; else v_digit3 -= (65 - 10);
    vxxx_as_number= ((v_digit1 * 36 * 36) + (v_digit2 * 36) + v_digit3) + 1;
    v_digit3= vxxx_as_number % 36;
    v_digit2= ((vxxx_as_number - v_digit3)/36) %36;
    v_digit1= ((vxxx_as_number-(v_digit3+v_digit2*36))/(36*36)) %36;
    if (v_digit1 <= 9) v_digit1= v_digit1 + 48; else v_digit1= v_digit1 + (65-10);
    if (v_digit2 <= 9) v_digit2= v_digit2 + 48; else v_digit2= v_digit2 + (65-10);
    if (v_digit3 <= 9) v_digit3= v_digit3 + 48; else v_digit3= v_digit3 + (65-10);
    digits[0]= v_digit1;
    digits[1]= v_digit2;
    digits[2]= v_digit3;
    digits[3]= '\0';
    debug_xxxmdbug_setup_group_name= digits;
  }
  return 0;
}


/*
  Identifiers in routines might be "delimited" or `delimited` or bare.
  We usually want `delimited` for consistent comparisons.
  Todo: Convert to upper because, even if variable-name is delimited,
        it is not case sensitive? Well, let's cancel that for a while.
        If we don't send it as lower case, IT DOESN'T WORK.
  Todo: This doesn't handle names that have ` or " inside them.
  Todo: Test with @`var` etc.
*/
QString MainWindow::setup_add_delimiters(QString name)
{
  QString n= name;
  if (n.mid(0,1) == "@") return n;
  if ((debug_ansi_quotes) && (n.mid(0,1) == "\""))
  {
    n= connect_stripper(n, true);
    return "`" + n + "`";
  }
  if (n.mid(0,1) != "`")
  {
    n= "`" + n + "`";
  }
  return n;
}


/* We finished with declared variables.
   Now user variables, that is, any token that starts with '@'.
   Here we dump names that start with '@' into a variables list.
   Later we select from the list and use the result as input for
   generate_icc_core().
   In the end the user sees these variables with command 'refresh user_variables'.
   If the user said call setup('-track_user_variables=0 ...') then we do nothing.
   todo: this is not handling already-delimited user variables correctly.
   todo: check: do we need to check for TOKEN_REFTYPE_COLUMN_OR_USER_VARIABLE?
   todo: I would much prefer to look for TOKEN_REFTYPE_USER_VARIABLE, but
         for some reason hparse isn't generating it. Have a look there.
*/
int MainWindow::setup_insert_into_variables_user_variables(QString text, int i_of_end_of_parameters)
{
  if (debug_track_user_variables > 0)
  {
    for (int i= i_of_end_of_parameters; main_token_lengths[i] != 0; ++i)
    {
      if (main_token_types[i] == TOKEN_TYPE_IDENTIFIER_WITH_AT)
      {
        QString v_value= text.mid(main_token_offsets[i], main_token_lengths[i]);
        if ((v_value.mid(0,1) == "@") && (v_value.mid(1,1) != "@"))
        {
          v_value= setup_add_delimiters(v_value);
          /* don't insert duplicates. */
          if (debug_tmp_user_variables.contains(v_value)) continue;
          debug_tmp_user_variables << v_value;
        }
      }
    }
  }
  debug_tmp_user_variables.sort(); /* I'm not sure this is necessary */
  return 0;
}


int MainWindow::setup_create_setup_log_table()
{
  char statement[]=
  "CREATE TABLE IF NOT EXISTS xxxmdbug.setup_log (\
   group_name VARCHAR(3) CHARACTER SET utf8,\
   user VARCHAR(80) CHARACTER SET utf8,\
   version_number_of_debugger CHAR(5) CHARACTER SET utf8,\
   timestamp_when_setup_procedure_was_run DATETIME,\
   arguments_passed_to_setup VARCHAR(8192) CHARACTER SET utf8,\
   routine_type VARCHAR(10) CHARACTER SET utf8, /* procedure|function. not event|trigger yet. */\
   schema_identifier VARCHAR(66) CHARACTER SET utf8,\
   original_routine_identifier VARCHAR(66) CHARACTER SET utf8,\
   surrogate_routine_identifier VARCHAR(66) CHARACTER SET utf8,\
   outcome VARCHAR(66) CHARACTER SET utf8, /* Made surrogate, Overwrote, or Failed */\
   invocations_of_routines_in_same_group INT,\
   invocations_of_routines_in_any_group INT,\
   comment_based_on_generate VARCHAR(66) CHARACTER SET utf8,\
   comment_based_on_readme VARCHAR(66) CHARACTER SET utf8) engine=myisam;\
  ";

  if (setup_mysql_real_query(statement,
                             (char*)"FAILED. Cannot create setup_log table.") == 1)
    return 1;
  return 0;
}

/* We need to use information_schema.routines and SHOW CREATE.
   todo: decide whether to use body or body_utf8 from mysql.proc.
         it's impossible in MySQL but still possible in MariaDB.
   todo: we could have an additional parameter for extra diagnostics from debug
   Todo: Although I don't trust get_lock any more, I suppose that we
         could lock an xxxmdbug table during this routine. */
int MainWindow::setup_internal(QString command_string)
{
  QString object_name_list= command_string;
  /* Make sure object_name_list is schema_identifier.routine_identifier
    or schema_identifier.% or %.%. Default is %.%.
    (Well, no it is not, the comments here are a bit obsolete.)
    TODO: re-examine whether default schema identifier should be current database or %.
    TODO: re-examine whether wildcarding should be (% and _) or with *."
    Output is a temporary list of schemas and objects. */
  {
    /* CALL xxxmdbug.privilege_checks(); */
    if (setup_create_setup_log_table() == 1) return 1;
    if (setup_get_setup_group_name()) return 1; /* returns @xxxmdbug_setup_group_name = 3-character string for new set name. */
    if (debug_track_user_variables > 0)
      debug_xxxmdbug_icc_core_surrogate_name=
          "xxxmdbug"
          + debug_xxxmdbug_setup_group_name
          + "P"
          + "icc_core";
    else
      debug_xxxmdbug_icc_core_surrogate_name= "icc_core";
  }

  if (setup_routine_list(command_string)) return 1;
  debug_v_statement_number= 0;
  for (int i= 0; i < debug_routine_list_names.count(); ++i)
  {
    if (setup_generate(i)) return 1;
    QString s_insert=
      QString("INSERT INTO xxxmdbug.setup_log VALUES (")
      +  "'" + debug_xxxmdbug_setup_group_name + "'," /* group_name */
      +  "'" + ocelot_user + "'," /* user. ?? maybe statement_edit_widget->dbms_current_user is better? */
      +  "'" + debug_xxxmdbug_debugger_version + "'," /* version_number_of_debugger */
      +  "'" + debug_xxxmdbug_timestamp + "'," /* timestamp_when_setup_procedure_was_run */
      +  "'" + object_name_list + "'," /* arguments_passed_to_setup */
      +  "'" + debug_routine_list_types.at(i) + "'," /* routine_type */
      +  "'" + debug_routine_list_schemas.at(i) + "'," /* schema_identifier */
      +  "'" + debug_routine_list_names.at(i) + "'," /* original_routine_identifier */
      +  "'" + debug_routine_list_surrogates.at(i) + "'," /* surrogate_routine_identifier */
      +  "'done'," /* outcome */ /* todo: something informative, eh? */
      +  "0," /* @xxxmdbug_invocations_of_routines_in_same_group,"*/ /* invocations_of_routines_in_same_group */
      +  "0," /* @xxxmdbug_invocations_of_routines_in_any_group,"*/ /* invocations_of_routines_in_any_group */
      + "NULL," /* comment_based_on_generate */
      + "'thanks. copyright. gpl');"; /* comment_based_on_readme */
    char s_utf8[2048]; /* Todo: this should be dynamically allocated */
    strcpy(s_utf8, s_insert.toUtf8());
    if (setup_mysql_real_query(s_utf8,
                               (char*)"FAILED. Cannot insert into xxxmdbug.setup_log. ") == 1)
      return 1;
  }

  if (debug_track_user_variables > 0)
  {
    if (setup_generate_icc_core() == 1) return 1;
  }

  if (setup_generate_icc_process_user_command_r_server_variables() == 1) return 1;

  /* This is blank: CALL xxxmdbug.view_and_trigger_and_event_check(); */
  /* This is blank: CALL xxxmdbug.checks_and_warnings(mysql_proc_db); */

  /*
    Now routine_lists_* has a bunch of CREATE PROCEDURE|FUNCTION,
    ready to be executed.
    TODO: If there were no @variables you can change calls to the
          generated icc_core back to the regular icc_core, it's a
          simple search-and-replace now.
  */

  if (setup_drop_routines() == 1) return 1;
  /* Create all the routines that are to be created. */
  /* If one of the creates fails, undo all previous creates. */
  /* Todo: change sql_mode before you call. Definer too? */
  for (int i= 0; i < debug_routine_list_surrogates.count(); ++i)
  {
    QString s;
    s= debug_routine_list_texts.at(i);
    {
      setup_set_session_sql_mode(debug_routine_list_sql_modes.at(i));
      int query_len= s.toUtf8().size();                  /* See comment "UTF8 Conversion" */
      char *query= new char[query_len + 1];
      memcpy(query, s.toUtf8().constData(), query_len + 1);
      if (setup_mysql_real_query(query,
                                 (char*)"FAILED. Cannot create a routine. ") == 1)
      {
        setup_drop_routines();
        delete []query;
        return 1;
      }
      delete []query;
    }
  }
  return 0;
}

/*
  Drop all routines in debug_routine_list_surrogates.
  We call this before we try to create those routines.
  Or we call this if any create fails.
  Todo: Think: why return if drop fails, instead of going to next?
*/
int MainWindow::setup_drop_routines()
{
  for (int i= 0; i < debug_routine_list_surrogates.count(); ++i)
  {
    QString s;
    s= "DROP "
            + debug_routine_list_types.at(i)
            + " IF EXISTS "
            + debug_routine_list_schemas.at(i)
            + "."
            + debug_routine_list_surrogates.at(i)
            + ";" + debug_lf;
    {
      int query_len= s.toUtf8().size();                  /* See comment "UTF8 Conversion" */
      char *query= new char[query_len + 1];
      memcpy(query, s.toUtf8().constData(), query_len + 1);

      if (setup_mysql_real_query(query,
                                 (char*)"FAILED. Cannot drop.") == 1)
      {
        delete []query;
        return 1;
      }
    }
  }
  return 0;
}

/*
  At the end of the $setup process, whether or not it succeeded:
  Possibly temporary tables were created, now drop them (no, gone now).
  Possibly some QStringLists were filled, now clear them.
  This is not a complete list, just the most likely to be big.
  Do not call before setup_initialize() where got debug_session_sql_mode.
*/
void MainWindow::setup_cleanup()
{
  debug_routine_list_schemas.clear();
  debug_routine_list_names.clear();
  debug_routine_list_types.clear();
  debug_routine_list_sql_modes.clear();
  debug_routine_list_surrogates.clear();
  debug_routine_list_texts.clear();
  debug_tmp_user_variables.clear();
  setup_set_session_sql_mode(debug_session_sql_mode_original);
}

/*
  Called from setup_internal() when we're about to create a routine.
  Called from setup_cleanup() when we want to restore the original.
  We execute SET SESSION_SQL_MODE=... so that, if sql_mode when the
  original routine was created is different from sql_mode now, we
  won't get in trouble because ""s were used for names, or due to
  pipes_as_concat, or in future something to do with sql_mode='oracle'.
  Actually we've probably changed ""s to ``s but that's not enough.
  Todo: Not much we can do about failure. But return an error if fail.
*/
int MainWindow::setup_set_session_sql_mode(QString s)
{
  if (s != debug_session_sql_mode_after_last_change)
  {
    char query[1024];
    strcpy(query, "SET SESSION SQL_MODE='");
    strcat(query, s.toUtf8());
    strcat(query, "';");
    if (setup_mysql_real_query(query,
                               (char*)"FAILED. Cannot change sql_mode.") == 1)
      return 1;
    debug_session_sql_mode_after_last_change= s;
  }
  return 0;
}

/* This generates the debuggee procedure that will handle the 'refresh server_variables'
   command. It is better to generate rather than have a fixed set of
   statements, because the server variables depend on the version and
   on what engines or plugins are installed. An example first line:
   SET v_ret=CONCAT(v_ret,'"@@ARIA_BLOCK_SIZE",','"',@@ARIA_BLOCK_SIZE,'";');
   todo: watch for v_ret overflow
   Perhaps performance_schema.session_variables won't exist,
   so we don't abort on failure.
   Todo: Nowadays MariaDB information_schema.server_variables has
         data-type and read-only columns, so most of this effort is not
         necessary! But we still need it because MySQL won't have them:
         https://dev.mysql.com/doc/refman/8.0/en/performance-schema-system-variable-tables.html
*/
/* Code from setup_generate_icc_process_user_command_set_server_variables
   has been added, it will use QString v_g_2 instead of QString v_g.
   This generates the debuggee procedure that will handle the 'set @@server_variable=...'
   command. An example first line:
   IF @token_value_2 = '@@ARIA_BLOCK_SIZE' THEN SET @@ARIA_BLOCK_SIZE=@xxxmdbug_token_value_4a; END IF;
   Wrong type or wrong value will cause a severe error, we handle it, but the warning list is cleared.*/
int MainWindow::setup_generate_icc_process_user_command_r_server_variables()
{
  QString mysql_proc_name;
  QString mysql_proc_db;
  QString mysql_proc_type;
  QString v_variable_name;
  QString v_g, v_g_2;
  QStringList c_variable_names; /* not the global with the same name */
  QStringList c_variable_types;
  {
    /* Skip if we did it before and server version + plugins unchanged */
    /* (Maybe 'settable' differs due to privileges, I don't care.) */
    QString s=
      QString("SELECT COUNT(*) ")
      + "FROM information_schema.routines "
      + "WHERE routine_name = 'icc_process_user_command_r_server_variables' "
      + "AND routine_schema COLLATE utf8_general_ci = 'xxxmdbug' "
      + "AND routine_definition LIKE '%" + statement_edit_widget->dbms_version + "%' "
      + "AND routine_definition LIKE '%" + debug_plugins + "%';";
    int query_len= s.toUtf8().size(); /* See comment "UTF8 Conversion" */
    char *query= new char[query_len + 1];
    memcpy(query, s.toUtf8().constData(), query_len + 1);
    QString s2= select_1_row(query);
    delete []query;
    if ((s2 == "") && (select_1_row_result_1.toInt() == 1)) return 0;
  }

  {
    /* Todo: Make sure big enough. Restore to original value. */
    int max_len= 100000;
    char query[256];
    sprintf(query, "SET SESSION GROUP_CONCAT_MAX_LEN=%d;", max_len);
    if (setup_mysql_real_query(query,
                               (char*)"FAILED. Cannot change group_concat_maxlen.") == 1)
      return 1;
  }
  c_variable_names.clear();
  c_variable_types.clear();
  {
    QString s;
    /* todo: maybe for MariaDB I should look first at information_schema */
    s= select_1_row("select group_concat('/*@@',variable_name) from performance_schema.session_variables;");
    if (s != "")
    {
      s= select_1_row("select group_concat('/*@@',variable_name) from information_schema.session_variables;");
      if (s != "")
      {
        debug_error((char*)"select from session_variables failed");
        return 1;
      }
    }
    int j, k;
    j= 0;
    for (int i= 0; ; ++i)
    {
      if (j == select_1_row_result_1.length()) break;
      k= select_1_row_result_1.indexOf("/*@@", j + 1);
      if (k == -1)
      {
        k= select_1_row_result_1.length();
        s= select_1_row_result_1.mid(j + 2, k - (j + 2));
      }
      else s= select_1_row_result_1.mid(j + 2, k - (j + 2 + 1));
      c_variable_names << s;
      j= k;
    }
  }

  if (c_variable_names.count()== 0) /* pseudo-assertion */
  {
    debug_error((char*)"c_variable_names.count() == 0");
    return 1;
  }
  c_variable_names.sort();

  /* Determine the types. Since we're calling from C, we
     have a way that doesn't involve trips to xxxmdbug.uvar.
     More exact too, but we throw away the exactness. */
  {
    MYSQL_RES *res= NULL;
    QString s= "SELECT ";
    for (int i= 0; i < c_variable_names.count(); ++i)
    {
      if (i != 0) s= s + QString(",");
      s= s + c_variable_names.at(i);
    }
    s= s + QString(";");
    int query_len= s.toUtf8().size(); /* See comment "UTF8 Conversion" */
    char *query= new char[query_len + 1];
    memcpy(query, s.toUtf8().constData(), query_len + 1);

    if (setup_mysql_real_query(query,
                               (char*)"FAILED. While selecting variable names.") == 1)
    {
      delete []query;
      return 1;
    }
    delete []query;
    res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
    if (res != NULL)
    {
      int num_fields= lmysql->ldbms_mysql_num_fields(res);
      assert(num_fields == c_variable_names.count());
      MYSQL_FIELD *fields;
      fields= lmysql->ldbms_mysql_fetch_fields(res);
      int t;
      QString v_data_type;
      for (int i= 0; i < num_fields; ++i)
      {
        t= fields[i].type;
        if ((t == MYSQL_TYPE_LONGLONG)
         || (t == MYSQL_TYPE_TINY)
         || (t == MYSQL_TYPE_SHORT)
         || (t == MYSQL_TYPE_LONG)
         || (t == MYSQL_TYPE_INT24))
         v_data_type= "BIGINT";
        else if ((t == MYSQL_TYPE_LONG_BLOB)
              || (t == MYSQL_TYPE_VAR_STRING)
              || (t == MYSQL_TYPE_TIMESTAMP)
              || (t == MYSQL_TYPE_SET)
              || (t == MYSQL_TYPE_ENUM)
              || (t == MYSQL_TYPE_STRING))
        {
          if (fields[i].charsetnr == 63) v_data_type= "LONGBLOB";
          else v_data_type= "LONGTEXT";
        }
        else if (t == MYSQL_TYPE_DECIMAL) v_data_type= "DECIMAL";
        else if ((t == MYSQL_TYPE_FLOAT)
              || (t == MYSQL_TYPE_DOUBLE))
                v_data_type= "DOUBLE";
        else v_data_type= "UNKNOWN";
        c_variable_types << v_data_type;
      }
    }
    if (res != NULL) lmysql->ldbms_mysql_free_result(res);
  }

  mysql_proc_name= "icc_process_user_command_r_user_variables"; /* todo: this is a lie. the original is for server_variables. */
  QString v_g_surrogate_routine_identifier= "icc_process_user_command_r_server_variables";
  QString v_g_2_surrogate_routine_identifier= "icc_process_user_command_set_server_variables";

  QString xxxmdbug_comment;
  {
    QString xxxmdbug_comment_string=
            debug_lf + "Routine for debugger tracking server variables"
            + debug_lf + "Generated by " + debug_xxxmdbug_debugger_name + " Version " + debug_xxxmdbug_debugger_version
            + debug_lf + "Generated on " + debug_xxxmdbug_timestamp
            + debug_lf + "Generated from " + statement_edit_widget->dbms_version
            + debug_lf + "Plugins: " + debug_plugins;
    xxxmdbug_comment=
          debug_lf
          + "DECLARE xxxmdbug_comment VARCHAR("
          + QString::number(xxxmdbug_comment_string.length() + 1)
          + ") DEFAULT '"
          + xxxmdbug_comment_string
          + "';" + debug_lf;
  }
  mysql_proc_db= "xxxmdbug";
  mysql_proc_type= "PROCEDURE";

  /* For ..._r_... Opening matches what is in generate(). */

  v_g = "CREATE "
          + mysql_proc_type
          + " "
          + mysql_proc_db
          + "."
          + v_g_surrogate_routine_identifier
          + "()" + debug_lf;
  //v_g_offset_of_begin= v_g.length() + debug_lf.length();
  v_g= v_g
          + "BEGIN"
          + xxxmdbug_comment
          + "DECLARE v_ret MEDIUMTEXT CHARACTER SET utf8 DEFAULT '';" + debug_lf
          /* todo: try TEXT */
          + "DECLARE v_prefix VARCHAR(128) CHARACTER SET utf8;" + debug_lf;

  /* For ..._set_... Opening matches what is in generate(). */
  v_g_2= v_g_2
          + "CREATE "
          + mysql_proc_type
          + " "
          + mysql_proc_db
          + "."
          + v_g_2_surrogate_routine_identifier
          + "()" + debug_lf;
  //v_g_2_offset_of_begin= v_g_2.length() + debug_lf.length();
  v_g_2= v_g_2
        + "BEGIN"
        + xxxmdbug_comment
        + "DECLARE EXIT HANDLER FOR SQLEXCEPTION CALL xxxmdbug.icc_change_statement_status('Fail');" + debug_lf
        + "SET @xxxmdbug_tmp_for_set = 'Fail';" + debug_lf;

  for (int i= 0; i < c_variable_names.count(); ++i)
  {
    /* Test whether one can assign to the variable. If an exception happens,
       it is almost certainly because the variable is read-only.
       ERROR 1238 (HY000): Variable 'warning_count' is a read only variable.
       In that case 'refresh server_variables' will show that is_settable = 0.
       Do not check @@timestamp because SET TIMESTAMP freezes the clock.
       Todo: actually a new column 'read only' would be smarter, here and for parameters. */
    //QString xxxmdbug_test_setting;
    v_variable_name= c_variable_names.at(i);
    int v_is_settable= 1;
    if (v_variable_name  != "@@TIMESTAMP")
    {
      char xxxmdbug_test_setting[256];
      strcpy(xxxmdbug_test_setting, "SET ");
      strcat(xxxmdbug_test_setting, v_variable_name.toUtf8());
      strcat(xxxmdbug_test_setting, "=");
      strcat(xxxmdbug_test_setting, v_variable_name.toUtf8());
      strcat(xxxmdbug_test_setting, ";");
      /* Not a call to setup_mysql_query because failure is okay. */
      if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], xxxmdbug_test_setting, strlen(xxxmdbug_test_setting)))
      {
        v_is_settable= 0;
      }
    }

    v_g= v_g +
            + "SET v_ret=CONCAT(v_ret,'\""
            + v_variable_name
            + "\",','\"',HEX(IFNULL(LEFT("
            + v_variable_name
            + ",66),'NULL')),'\","
            + QString::number(v_is_settable)
            + ",\""
            + c_variable_types.at(i)
            + "\";');" + debug_lf;

    if (v_is_settable != 0)
    {
      v_g_2= v_g_2
             + "IF @xxxmdbug_token_value_2 = '"
             + v_variable_name
             + "' THEN SET "
             + v_variable_name
             + "= @xxxmdbug_token_value_4a; SET @xxxmdbug_tmp_for_set = 'OK'; END IF;" + debug_lf;
    }
  }

  v_g= v_g
       + "SET v_prefix = CONCAT('xxxmdbug_',@xxxmdbug_channel,@xxxmdbug_prefix_end_for_r_server_variables);\n"
       + "CALL xxxmdbug.dbms_pipe_send(v_prefix,v_ret);" + debug_lf
       + "CALL xxxmdbug.icc_change_statement_status('OK');" + debug_lf
       + "END;" + debug_lf;

  v_g_2= v_g_2
           + "CALL xxxmdbug.icc_change_statement_status(@xxxmdbug_tmp_for_set);" + debug_lf
           + "END;" + debug_lf;

  /*
    When setup_internals() sees what you add here,
    it will actually execute the creates. That is:
    drop + create icc_process_user_command_r_server_variables
    drop + create icc_process_user_command_set_server_variables
    update xxxmdbug.routines
  */
  debug_routine_list_schemas << "xxxmdbug";
  debug_routine_list_names << "";
  debug_routine_list_types << "PROCEDURE";
  debug_routine_list_sql_modes << "";
  debug_routine_list_surrogates << v_g_surrogate_routine_identifier;
  debug_routine_list_texts << v_g;

  debug_routine_list_schemas << "xxxmdbug";
  debug_routine_list_names << "";
  debug_routine_list_types << "PROCEDURE";
  debug_routine_list_sql_modes << "";
  debug_routine_list_surrogates << v_g_2_surrogate_routine_identifier;
  debug_routine_list_texts << v_g_2;
  return 0;
}

/* This generates a variant icc_core that can handle 'refresh user_variables'..
   Any changes to icc_core() should be made here too.
   Comments are stripped so see the original icc_core() above.
   The tmp_user_variables list was populated with @user-variable-names while we were generating.
     Although it is easy to assign to a user variable or server variable by
     preparing and executing a SET statement, it is unreliable because we
     might be in a function rather than in a procedure. So, for 'set', we
     generate fixed SET statements for the user variables that we know of.
   Use the base icc_core if track_user_variables=0.
   Todo: probably this would be faster if it was smaller. */
int MainWindow::setup_generate_icc_core()
{
  QString v_g;
  /* we already know icc_core_surrogate_name */
  v_g= "CREATE PROCEDURE xxxmdbug."
          + debug_xxxmdbug_icc_core_surrogate_name
          + "(line_number INT) "
          + debug_lf;

/* part 1 -- as far as the first 'call xxxmdbug.process_user_command'."
   v_ret and v_prefix are part of the variant. */
/* todo: as far as I can tell, the initial label 'z:' is not needed */
  v_g= v_g
          + "z:BEGIN" + debug_lf
          + "DECLARE v_ret MEDIUMTEXT CHARACTER SET utf8;" + debug_lf
          + "DECLARE v_prefix VARCHAR(128) CHARACTER SET utf8;" + debug_lf
          + "DECLARE sleep_result INT;" + debug_lf
          + "DECLARE v_n INT;" + debug_lf
          + "DECLARE v_flags INT;"
          + "DECLARE v_table_is_temporary_and_to_be_cleared INT;" + debug_lf
          + "SET @xxxmdbug_token_value_1 = '';"
          + "SET @xxxmdbug_icc_count=@xxxmdbug_icc_count+1;" + debug_lf
          + "x1: LOOP" + debug_lf
          + "  CALL xxxmdbug.icc_get_user_command(0);" + debug_lf
          + "  IF @xxxmdbug_message <= 0x20 THEN LEAVE x1; END IF;" + debug_lf
          + "  IF @xxxmdbug_token_value_1 IN ('continue','exit','leave','next','execute','skip','step') THEN LEAVE x1; END IF;"
          + "  IF @xxxmdbug_token_value_2 = 'user_variables' THEN" + debug_lf
          + "    SET v_ret='';" + debug_lf;
/* part 1 end */
  for (int i= 0; i < debug_tmp_user_variables.count(); ++i)
  {
    QString v_variable_name= debug_tmp_user_variables.at(i);
    v_g= v_g
          + "SET @xxxmdbug_uvar="
          + v_variable_name
          + ";"
          + "SET @xxxmdbug_old_uvar="
          + "@xxxmdbug_old_"
          + v_variable_name.mid(1, v_variable_name.length() - 1)
          + ";";
    v_g= v_g
          + "CALL xxxmdbug.uvar(v_ret,'"
          + v_variable_name
          + "');" + debug_lf;
  }

  v_g= v_g + "SET v_prefix = CONCAT('xxxmdbug_',@xxxmdbug_channel,@xxxmdbug_prefix_end_for_r_user_variables);" + debug_lf
                       + "CALL xxxmdbug.dbms_pipe_send(v_prefix,v_ret);" + debug_lf
                       + "CALL xxxmdbug.icc_change_statement_status('OK');" + debug_lf
                       + "ELSE" + debug_lf
                       + "CALL xxxmdbug.icc_process_user_command();" + debug_lf
                       + "END IF;" + debug_lf;

  v_g= v_g + "IF @xxxmdbug_token_value_1 = 'set' AND LEFT(@xxxmdbug_token_value_2,1) = '@' AND LEFT(@xxxmdbug_token_value_2,2) <> '@@' THEN" + debug_lf
                       + "  SET @xxxmdbug_status_last_command = 'set';" + debug_lf
                       + "  SET @xxxmdbug_tmp_for_set='Fail';" + debug_lf;
  for (int i= 0; i < debug_tmp_user_variables.count(); ++i)
  {
    QString v_variable_name= debug_tmp_user_variables.at(i);
    v_g= v_g
                       + "  IF @xxxmdbug_token_value_2 = '"
                       +    v_variable_name
                       +    "' THEN CALL xxxmdbug.retype(); SET "
                       +    v_variable_name
                       +    "= @xxxmdbug_token_value_4a; SET @xxxmdbug_tmp_for_set='OK'; END IF;" + debug_lf;
  }
  v_g= v_g + "CALL xxxmdbug.icc_change_statement_status(@xxxmdbug_tmp_for_set);" + debug_lf;
  v_g= v_g + "END IF;" + debug_lf;
  /* part 2 */
  v_g= v_g
                       + "END LOOP;" + debug_lf
                       + "SET @xxxmdbug_status_line_number = line_number;" + debug_lf
                       + "CALL xxxmdbug.icc_breakpoint_check(line_number,@xxxmdbug_breakpoint_check_result);" + debug_lf
                       + "SET @xxxmdbug_status_breakpoint_check_result = @xxxmdbug_breakpoint_check_result;" + debug_lf
                       + "CALL xxxmdbug.icc_send_statement_status('icc_core');" + debug_lf
                       + "IF @xxxmdbug_breakpoint_check_result > 0 THEN" + debug_lf
                       + "x2: LOOP" + debug_lf
                       + "IF @xxxmdbug_token_value_1 IN ('continue','exit','leave','next','execute','skip','step') THEN" + debug_lf
                       + "LEAVE x2;" + debug_lf
                       + "END IF;" + debug_lf
                       + "CALL xxxmdbug.icc_get_user_command(1);" + debug_lf
                       + "IF @xxxmdbug_token_value_1 IN ('next','step') THEN CALL xxxmdbug.icc_process_user_command_step_or_next(); END IF;" + debug_lf
                       + "IF @xxxmdbug_token_value_1 IN ('continue','exit','leave','next','execute','skip','step') THEN" + debug_lf
                       + "LEAVE x2;" + debug_lf
                       + "END IF;" + debug_lf
                       + "IF @xxxmdbug_token_value_2 = 'user_variables' THEN" + debug_lf
                       + "SET v_ret='';" + debug_lf;
/* part2 end */
  for (int i= 0; i < debug_tmp_user_variables.count(); ++i)
  {
    QString v_variable_name= debug_tmp_user_variables.at(i);
    v_g= v_g
                        + "SET @xxxmdbug_uvar="
                        + v_variable_name
                        + ";"
                        + "SET @xxxmdbug_old_uvar=@xxxmdbug_old_"
                        + v_variable_name.mid(1,v_variable_name.length() - 1)
                        + ";"
                        + "CALL xxxmdbug.uvar(v_ret,'"
                        + v_variable_name
                        + "');" + debug_lf;
  }
  v_g= v_g
                        + "SET v_prefix = CONCAT('xxxmdbug_',@xxxmdbug_channel,@xxxmdbug_prefix_end_for_r_user_variables);" + debug_lf
                        + "CALL xxxmdbug.dbms_pipe_send(v_prefix,v_ret);" + debug_lf
                        + "CALL xxxmdbug.icc_change_statement_status('OK');" + debug_lf
                        + "ELSE "
                        + "CALL xxxmdbug.icc_process_user_command();" + debug_lf
                        + "END IF;" + debug_lf;
  v_g= v_g
                        + "IF @xxxmdbug_token_value_1 = 'set' AND LEFT(@xxxmdbug_token_value_2,1) = '@'  AND LEFT(@xxxmdbug_token_value_2,2) <> '@@' THEN" + debug_lf
                        + "SET @xxxmdbug_status_last_command = 'set';" + debug_lf
                        + "SET @xxxmdbug_tmp_for_set = 'Fail';" + debug_lf;
  for (int i= 0; i < debug_tmp_user_variables.count(); ++i)
  {
    QString v_variable_name= debug_tmp_user_variables.at(i);
    v_g= v_g
                        + "IF @xxxmdbug_token_value_2 = '"
                        + v_variable_name
                        + "' THEN CALL xxxmdbug.retype(); SET "
                        + v_variable_name
                        + "= @xxxmdbug_token_value_4a; SET @xxxmdbug_tmp_for_set = 'OK'; END IF;" + debug_lf;
  }
  v_g= v_g + "CALL xxxmdbug.icc_change_statement_status(@xxxmdbug_tmp_for_set);" + debug_lf;
  v_g= v_g + "END IF;" + debug_lf;
/* part 3 start */
  v_g= v_g + "CALL xxxmdbug.icc_send_statement_status('icc_core');\n"
                       + "END LOOP;" + debug_lf
                       + "CALL xxxmdbug.icc_send_statement_status('icc_core');\n"
                       + "END IF;" + debug_lf;
/* part 3 end */
/* part 4 */
/* Copy all user variables to old user variables. For example, if there is
   a user variable @a, this will generate \"SET @xxxmdbug_old_a=@a;\".
   Results should show up in user_variables.old_value column.
   Todo: check for too-long variable names.
   Todo: there has to be some way to clean up all the @xxxmdbug_old_ variables. */
  for (int i= 0; i < debug_tmp_user_variables.count(); ++i)
  {
    QString v_variable_name= debug_tmp_user_variables.at(i);
    v_g= v_g
            + "SET @xxxmdbug_old_"
            + v_variable_name.mid(1, v_variable_name.length() - 1)
            + "="
            + v_variable_name
            + ";" + debug_lf;
  }
  v_g= v_g + "END;" + debug_lf;

  debug_routine_list_schemas << "xxxmdbug";
  debug_routine_list_names << debug_xxxmdbug_icc_core_surrogate_name;
  debug_routine_list_types << "PROCEDURE";
  debug_routine_list_sql_modes << "";
  debug_routine_list_surrogates << debug_xxxmdbug_icc_core_surrogate_name;
  debug_routine_list_texts << v_g;

  return 0;
}

/* Todo: "    IF v_statement_type = 'debuggable' THEN" ... */
int MainWindow::setup_generate_statements_debuggable(int i_of_statement_start,
                                                      int v_line_number_of_start_of_first_token,
                                                      int v_statement_number_within_routine,
                                                      QString text,
                                                      int v_token_number_of_last_token,
                                                      int routine_number)
{
  /* starts "if statement is being debugged" */
  debug_v_g= debug_v_g + "IF xxxmdbug.is_debuggee_and_is_attached()=1 THEN " + debug_lf;
  /* inner_loop: generate 'xxxmdbug_inner_loop_x: LOOP' */
  QString inner_loop_label= "xxxmdbug_inner_loop_label_" + QString::number(v_statement_number_within_routine);
  if (is_plsql)
    debug_v_g= debug_v_g
            + "<<" + inner_loop_label + ">> "
            + "LOOP" + debug_lf;
  else
    debug_v_g= debug_v_g
            + inner_loop_label
            + ": LOOP" + debug_lf;
//  /* Save @@warning_count. */
//  int v_verb= i; /* == token_number_of_first_token?? */
//  /* TODO: Find out previous_statement_type */
  /* Find out if previous_statement_type was nothing | DECLARE HANDLER */
  QString v_previous_statement_type= "not declare_handler";
  bool handler_seen= false;
  for (int i= i_of_statement_start; i >= 0; --i)
  {
    if (main_token_flags[i] & TOKEN_FLAG_IS_START_STATEMENT)
    {
      if ((main_token_types[i] == TOKEN_KEYWORD_DECLARE)
       && (handler_seen == true))
        v_previous_statement_type= "declare_handler";
      break;
    }
    if (main_token_types[i] == TOKEN_KEYWORD_HANDLER) handler_seen= true;
  }
  int is_first_statement_in_a_declare_handler;
  if (v_previous_statement_type == "declare_handler")
    is_first_statement_in_a_declare_handler= 1;
  else
    is_first_statement_in_a_declare_handler= 0;
  debug_v_g= debug_v_g + debug_lf + "CALL xxxmdbug.icc_start("
           + QString::number(v_line_number_of_start_of_first_token)
           + ","
           + QString::number(is_first_statement_in_a_declare_handler)
           + ");"  + debug_lf;
  if (setup_determine_what_variables_are_in_scope(i_of_statement_start, text)) return 1; /* result=c_variables */

  for (int i= c_variable_names.count() - 1; i >= 0; --i)
  {
    QString v_variable_identifier= c_variable_names.at(i);
    QString v_token_number_of_declare= c_variable_tokens.at(i);
    //"          CALL xxxmdbug.overflow_check(debug_v_g);
    debug_v_g= debug_v_g + debug_lf
         + "CALL xxxmdbug.icc_copy_variable_to_table_row('"
         + v_variable_identifier
         + "',"
         + v_token_number_of_declare
         + ","
         + v_variable_identifier
         + ");" + debug_lf;
  }
  /* generate 'call icc_core' */
  debug_v_g= debug_v_g + "CALL xxxmdbug."
           + debug_xxxmdbug_icc_core_surrogate_name
           + "("
           + QString::number(v_line_number_of_start_of_first_token)
           + ");" + debug_lf;

  /* Generate: "if (exit) signal" */
  /* todo: consider doing the signal in icc_core or icc_process_user_command but maybe then it won't be handled. */
  debug_v_g= debug_v_g + "IF @xxxmdbug_token_value_1 = 'exit' THEN SIGNAL sqlstate '56780' SET mysql_errno = @xxxmdbug_signal_errno; END IF;";
  for (int i= c_variable_names.count() - 1; i >= 0; --i)
  {
    QString v_variable_identifier= c_variable_names.at(i);
    QString v_token_number_of_declare= c_variable_tokens.at(i);
    //"          CALL xxxmdbug.overflow_check(debug_v_g);
    debug_v_g= debug_v_g + debug_lf
         + "CALL xxxmdbug.icc_copy_table_row_to_variable('"
         + v_variable_identifier
         + "',"
         + v_token_number_of_declare
         + ","
         + v_variable_identifier
         + ");" + debug_lf;
  }
  /* inner_loop: 'SET' and 'EXECUTE' might change variables that we're watching.
     So if they happened, go back and copy the variables again.
     Todo: think: should this happen after non-debuggable statements too? */
  QString iterate;
  if (is_plsql) iterate= "CONTINUE ";
  else iterate= "ITERATE ";
  debug_v_g= debug_v_g + "IF @xxxmdbug_breakpoint_check_result>0 AND (@xxxmdbug_token_value_1 = 'set' OR @xxxmdbug_token_value_1 = 'execute') THEN "
             + iterate
             + inner_loop_label
             + "; END IF;" + debug_lf;
  /* Check @@warning_count. */
  debug_v_g= debug_v_g + debug_lf + "CALL xxxmdbug.icc_end();" + debug_lf;
  /* Generate: "if (leave) leave" i.e. leave outer loop */
  /* Sometimes LEAVE is illegal, see insert_into_statements comments.
     If so act as if 'leave' is 'skip'. */
  bool is_leave_possible;
  for (int j= i_of_statement_start - 1;; --j)
  {
    if (j <= i_of_end_of_parameters)
    {
      is_leave_possible= false;
      break;
    }
    if (main_token_types[j] == TOKEN_KEYWORD_END)
    {
      int k= main_token_pointers[j];
      if ((k >= j) || (k < 1))
      {
        is_leave_possible= false;  /* should be an assert */
        break;
      }
      j= main_token_pointers[j];
      continue;
    }
    if (main_token_flags[j] & TOKEN_FLAG_IS_START_STATEMENT)
    {
      bool is_begin_ok= true;
      if ((is_plsql) && (debug_label_list.count() != 1)) is_begin_ok= false;
      if (((main_token_types[j] == TOKEN_KEYWORD_BEGIN) && (is_begin_ok))
       || (main_token_types[j] == TOKEN_KEYWORD_FOR_IN_FOR_STATEMENT)
       || (main_token_types[j] == TOKEN_KEYWORD_LOOP)
       || (main_token_types[j] == TOKEN_KEYWORD_REPEAT)
       || (main_token_types[j] == TOKEN_KEYWORD_WHILE))
      {
        is_leave_possible= true;
        break;
      }
    }
    else
      if (main_token_types[j] == TOKEN_KEYWORD_HANDLER)
    {
      is_leave_possible= false;
      break;
    }
  }
  QString leave;
  if (is_plsql) leave= "EXIT ";
  else leave= "LEAVE ";
  if (is_leave_possible == true)
  {
    if (debug_label_list.count() == 0) /* pseudo-assert */
    {
      debug_error((char*)"debug_label_list.count() == 0");
      return 1;
    }
    debug_v_g= debug_v_g + debug_lf + "IF @xxxmdbug_token_value_1 = 'leave' THEN ";
    if ((is_plsql) && (debug_label_list.count() == 1))
    {
      debug_v_g= debug_v_g +"GOTO xxxmdbug_routine_exit";
    }
    else
    {
      debug_v_g= debug_v_g
             + "LEAVE "
             + debug_label_list.at(debug_label_list.count() - 1);
    }
    debug_v_g= debug_v_g
             + "; END IF;" + debug_lf;
  }
  else
  {
    debug_v_g= debug_v_g + debug_lf + "IF @xxxmdbug_token_value_1 = 'leave' THEN "
             + leave
             + inner_loop_label
             + "; END IF;" + debug_lf;
  }
  /* inner_loop: by leaving inner_loop before doing the instruction, we "skip over" it */
  debug_v_g= debug_v_g + "IF @xxxmdbug_token_value_1 = 'skip' THEN "
           + leave
           + inner_loop_label
           + "; END IF;" + debug_lf;
  /* If the original routine contained only one statement which was not compound and which
     did not end with ';', add ';'.
     Todo: Check: isn't this impossible? Didn't we add ";" earlier?
           I've assumed it's impossible, but statement ends with ";".
  */
  QString v_statement_end_character= ";";
  /* generate: handlers */
  /* removed. see notes before generate_handlers(). CALL xxxmdbug.generate_handlers(debug_v_g,v_line_number_of_start_of_first_token); */
//"  CALL xxxmdbug.overflow_check(debug_v_g);"
  /* todo: we got sql_mode earlier but we don't seem to be using it! */
  if (setup_generate_statement_text(i_of_statement_start, text, v_token_number_of_last_token, routine_number)) return 1;
  /* END the BEGIN that generate_handlers() generated */
  /* removed. see notes before generate_handlers(). SET debug_v_g = CONCAT(debug_v_g,@xxxmdbug_lf,'END;'); */

  /* generate something to insert if statement is 'prepare' or delete if statement is 'deallocate' */
  /* todo: think about the execution path if there is a user-written warning */
  int statement_type= main_token_types[i_of_statement_start];
  if ((statement_type == TOKEN_KEYWORD_PREPARE)
   || (statement_type == TOKEN_KEYWORD_DROP)
   || (statement_type == TOKEN_KEYWORD_DEALLOCATE))
  {
    QString v_value_of_second_token= "";
    QString v_value_of_third_token= "";
    QString v_value_of_fourth_token= "";
    for (int i= i_of_statement_start + 1; main_token_lengths[i] != 0; ++i)
    {
      if ((main_token_types[i] >= TOKEN_TYPE_COMMENT_WITH_SLASH)
       && (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_MINUS))
        continue;
      QString n= text.mid(main_token_offsets[i], main_token_lengths[i]);
      if (v_value_of_second_token == "") v_value_of_second_token= n;
      else if (v_value_of_third_token == "") v_value_of_third_token= n;
      else
      {
        v_value_of_fourth_token= n;
        break;
      }
    }
    if (statement_type == TOKEN_KEYWORD_PREPARE)
    {
      /* I think v_value_of_fourth_token has ''s already, correctly */
      debug_v_g= debug_v_g + debug_lf + "CALL xxxmdbug.insert_into_prepared_statements('"
               + v_value_of_second_token
               + "',"
               + v_value_of_fourth_token
               + ");";
    }
    if (statement_type ==  TOKEN_KEYWORD_DROP)
    {
      /* A little-known alternative syntax for DEALLOCATE PREPARE is DROP PREPARE. */
      if (QString::compare(v_value_of_second_token, "PREPARE", Qt::CaseInsensitive) == 0)
      {
        statement_type= TOKEN_KEYWORD_DEALLOCATE;
      }
    }
    if (statement_type ==  TOKEN_KEYWORD_DEALLOCATE)
    {
      debug_v_g= debug_v_g + debug_lf + "CALL xxxmdbug.delete_from_prepared_statements('"
               + v_value_of_third_token
               + "');";
    }
  }
  /* generate 'update statements_executed' */
  debug_v_g= debug_v_g + debug_lf + "CALL xxxmdbug.update_statements_executed(0);";
  /* inner_loop: leave the loop after doing the original statement once */
  debug_v_g= debug_v_g + debug_lf
          + leave
          + inner_loop_label
          + ";" + debug_lf;
  /* inner loop: ends */
  debug_v_g= debug_v_g + "END LOOP;" + debug_lf;
  debug_v_g= debug_v_g + debug_lf + "ELSE"; /* terminates "if statement is being debugged" */
  /* "generate the statement text" for when we're not debugging */
//"      CALL xxxmdbug.overflow_check(debug_v_g);"
  if (setup_generate_statement_text_as_is(i_of_statement_start, text, v_token_number_of_last_token)) return 1;
  //"      SET debug_v_g = CONCAT(debug_v_g,v_statement_end_character);"
  debug_v_g= debug_v_g + debug_lf + "END IF;";
  return 0;
}

/* "Generate: label": Called from generate()
   Whenever you see WHILE or LOOP or BEGIN or REPEAT, make|copy a label and add to label list
   Whenever you see END (but not END IF or END CASE), remove last item in label list
   LEAVE will use the latest item in label list.
*/
int MainWindow::setup_generate_label(int i_of_start_of_statement, QString text, int v_statement_number)
{
  QString v_label;
  QString v_token_value_of_previous_token;
  int token_type;
  token_type= main_token_types[i_of_start_of_statement];
  if ((token_type == TOKEN_KEYWORD_BEGIN)
   || (token_type == TOKEN_KEYWORD_FOR_IN_FOR_STATEMENT)
   || (token_type == TOKEN_KEYWORD_LOOP)
   || (token_type == TOKEN_KEYWORD_REPEAT)
   || (token_type == TOKEN_KEYWORD_WHILE))
  {
    int j;
    for (j= i_of_start_of_statement - 1; j > i_of_end_of_parameters; --j)
    {
      token_type= main_token_types[j];
      if ((main_token_types[j] >= TOKEN_TYPE_COMMENT_WITH_SLASH)
       && (main_token_types[j] <= TOKEN_TYPE_COMMENT_WITH_MINUS))
        continue;
      v_token_value_of_previous_token= text.mid(main_token_offsets[j], main_token_lengths[j]);
      break;
    }
    if (v_token_value_of_previous_token == ":")
    {
      v_label= text.mid(main_token_offsets[j-1], main_token_lengths[j-1]);
    }
    else
    {
      v_label= " xxxmdbug_label_" + QString::number(v_statement_number);
      /* the ' ' is necessary if the while/loop/begin/repeat is not at start of line */
      if (is_plsql)
        debug_v_g= debug_v_g + " <<" + v_label + ">>";
      else
        debug_v_g= debug_v_g + " " + v_label + ":";
      debug_v_g= debug_v_g + debug_lf;
    }
    debug_label_list << v_label;
  }
  if (token_type == TOKEN_KEYWORD_END)
  {
    int j;
    for (j= i_of_start_of_statement + 1; main_token_lengths[j] != 0; ++j)
    {
      if ((main_token_types[j] >= TOKEN_TYPE_COMMENT_WITH_SLASH)
       && (main_token_types[j] <= TOKEN_TYPE_COMMENT_WITH_MINUS))
        continue;
      break;
    }
    if ((main_token_types[j] != TOKEN_KEYWORD_IF)
     && (main_token_types[j] != TOKEN_KEYWORD_CASE))
    {
      if (debug_label_list.count() == 0) /* pseudo-assert */
      {
        debug_error((char*)"label_list.count() == 0");
        return 1;
      }
      debug_label_list.removeAt(debug_label_list.count() - 1);
    }
  }
  return 0;
}

/* Todo: I'm excluding MariaDB variables defined as row type or
   within FOR var-name IN, since $debug can't handle them.
   It's probably fixable -- we don't absolutely have to have the
   data type (we get along without it for $user-variables); we
   should be able to recognize `row`.`scalar` names even if we
   have to lie to install_sql.cpp.
*/
int MainWindow::setup_row_type(int i_of_variable)
{
  for (int i= i_of_variable; main_token_lengths[i] != 0; ++i)
  {
    if ((main_token_flags[i] & TOKEN_FLAG_IS_DATA_TYPE) != 0)
    {
      if ((main_token_types[i] == TOKEN_KEYWORD_ROW)
       || (main_token_types[i] == TOKEN_KEYWORD_TYPE))
        return TOKEN_KEYWORD_ROW;
      return main_token_types[i];
    }
    if (main_token_types[i] == TOKEN_KEYWORD_IN) return TOKEN_KEYWORD_ROW;
    if ((main_token_flags[i] & TOKEN_FLAG_IS_START_STATEMENT) != 0)
      break; /* reached next statement without seeing data type? impossible */
  }
  return 0;
}

/* Make c_variables = a list of in-scope variables. */
/* Use the same trick that was used for hparse_f_label */
/* token_number_of_declare starts at 1 after end of (parameter list)
   (or maybe it's 2 after start of parameter list?) */
/* Todo: Something can be in scope but shadowed. Look for duplicates! */
/* Todo: You're not handling identifiers that need delimiting */
/* Warning: Since search is from end to start, list is backwards.
   Therefore, when searching or copying, go in reverse order. */
/* Somewhere in some install_sql.cpp routine is an expectation that
   variables are inside ``s. So we add ``s if they're absent. */
/* Todo: Preserve the case of the original, not the reference (?).
   Todo: See what happens if it's in ""s or in ''s. */
/* Todo: We plan to change hparse_f_variables() so variable_refer
         points to variable_define. If that's done, this may be easier. */
int MainWindow::setup_determine_what_variables_are_in_scope(
            int i_of_statement_start,
            QString text)
{
  c_variable_names.clear();
  c_variable_tokens.clear();
  int v_token_number_of_declare= 0;
  bool is_identifier;
  for (int i= i_of_statement_start - 1; i >= 0; --i)
  {
    if (main_token_types[i] == TOKEN_KEYWORD_END)
    {
      int j= main_token_pointers[i];
      if ((j >= i) || (j < 1)) break; /* should be an assert */
      i= main_token_pointers[i];
      continue;
    }
    QString v_variable_identifier= "";
    if ((main_token_types[i] == TOKEN_TYPE_IDENTIFIER)
     || (main_token_types[i] == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK)
     || (main_token_types[i] == TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE))
      is_identifier= true;
    else is_identifier= false;
    if ((is_identifier)
     && (main_token_reftypes[i] == TOKEN_REFTYPE_VARIABLE_DEFINE))
    {
      v_variable_identifier= text.mid(main_token_offsets[i], main_token_lengths[i]);
      /* s = the var! */
      int k;
      for (k= i - 1; k >= 0; --k)
      {
        if ((main_token_flags[k] & TOKEN_FLAG_IS_START_STATEMENT) != 0)
          break;
      }
      v_token_number_of_declare= k - i_of_start_of_parameters;
    }
    if ((is_identifier)
     && (i < i_of_end_of_parameters)
     && (main_token_reftypes[i] == TOKEN_REFTYPE_PARAMETER_DEFINE))
    {
      v_variable_identifier= text.mid(main_token_offsets[i], main_token_lengths[i]);
      v_token_number_of_declare= 0;
    }
    if (v_variable_identifier != "")
    {
      v_variable_identifier= setup_add_delimiters(v_variable_identifier);
      bool is_shadow= false;
      /* todo: variable_names.contains() would do this faster, I think. */
      for (int m= 0; m < c_variable_names.count(); ++m)
      {
        if (QString::compare(c_variable_names.at(m), v_variable_identifier, Qt::CaseInsensitive) == 0)
        {
          is_shadow= true;
          break;
        }
      }
      if (is_shadow == false)
      {
        c_variable_names << v_variable_identifier;
        if (setup_row_type(i) == TOKEN_KEYWORD_ROW)
          c_variable_tokens << QString::number(-1);
        else
          c_variable_tokens << QString::number(v_token_number_of_declare);
      }
    }
  }
  for (int k= c_variable_tokens.count() - 1; k >= 0; --k)
  {
    int k2= c_variable_tokens.at(k).toInt();
    if (k2 == -1)
    {
      c_variable_names.removeAt(k);
      c_variable_tokens.removeAt(k);
    }
  }
  return 0;
}

/*
  Generate the statement text, but replace routine names
  with surrogate routine names if they are in the $setup list,
  and being invoked.
  Beware: spaces after function names matter
  Todo: DO NOT necessarily replace with surrogate routine name. We might have put it in directly."
  Todo: check that argument count = parameter count."
  Todo: handle possible qualifier."
  Todo: handle possible delimiter."
  Todo: check that function name <> built-in name,
        in that case it's ignored (detectable at CREATE time).
  Todo: skip if CREATE? I forget why we substitute for CREATE.
  Todo: call this for flow-control statements, but they must end
        after the expression, e.g. WHILE (expr) DO ends with DO.
        (This also applies for DECLARE HANDLER.)
  Todo: call this for DECLARE because DEFAULT clauses can have (expr).
  Todo: I don't know how to handle PREPARE or EXECUTE IMMEDIATE yet,
        but if they have literal arguments there is hope.
  Todo: watch out for routine characteristic = ansi_quotes.
        (we've used debug_ansi_quotes elsewhere)
  Todo: Flow-control is not debuggable but might contain function calls.
  Warning: We're preserving comments because if they start with / * !
           they are not stripped, but we don't see inside them.
*/
int MainWindow::setup_generate_statement_text(int i_of_statement_start,
                                               QString text,
                                               int v_token_number_of_last_token,
                                               int routine_number
                                              )
{
  //bool statement_might_contain_routine_invocation= false;
  int statement_type= main_token_types[i_of_statement_start];

  /*
     special handling of RETURN -- remove the function name from the call stack before returning.
     we also do delete from call stack at the end of the routine, due to generate_ender.
     todo: more special handling of RETURN: we pointlessly generate
           CALL xxxmdbug.update_statements_executed(0);
           LEAVE xxxmdbug_inner_loop_label_2;
  */
  if (statement_type == TOKEN_KEYWORD_RETURN)
    debug_v_g= debug_v_g + "CALL xxxmdbug.routine_exit();" + debug_lf;
  /*
    TOKEN_KEYWORD_CREATE used to be in this list, but I removed it.
    Todo: Perhaps CREATE TABLE AS SELECT could be okay though.
    Currently it doesn't matter because we check everything.
  */
  //if ((statement_type == TOKEN_KEYWORD_CALL)
  // || (statement_type == TOKEN_KEYWORD_CASE)
  // || (statement_type == TOKEN_KEYWORD_DECLARE)
  // || (statement_type == TOKEN_KEYWORD_DELETE)
  // || (statement_type == TOKEN_KEYWORD_DO)
  // || (statement_type == TOKEN_KEYWORD_ELSEIF)
  // || (statement_type == TOKEN_KEYWORD_FOR_IN_FOR_STATEMENT)
  // || (statement_type == TOKEN_KEYWORD_IF)
  // || (statement_type == TOKEN_KEYWORD_INSERT)
  // || (statement_type == TOKEN_KEYWORD_REPLACE)
  // || (statement_type == TOKEN_KEYWORD_RETURN)
  // || (statement_type == TOKEN_KEYWORD_SELECT)
  // || (statement_type == TOKEN_KEYWORD_SET)
  // || (statement_type == TOKEN_KEYWORD_UNTIL)
  // || (statement_type == TOKEN_KEYWORD_WHILE))
  //  statement_might_contain_routine_invocation= true;
  for (int i= i_of_statement_start; main_token_lengths[i] != 0; ++i)
  {
    QString d= text.mid(main_token_offsets[i], main_token_lengths[i]);

    /*
      special handling of BEGIN NOT ATOMIC -- only output BEGIN
      looks like MariaDB has a bug with "label: BEGIN NOT ATOMIC"
    */
    if (statement_type == TOKEN_KEYWORD_BEGIN)
    {
      int m;
      for (m= i_of_statement_start; m <= v_token_number_of_last_token; ++m)
      {
        if (main_token_lengths[m] == 0) break; /* should be an assert? */
        if (main_token_types[m] == TOKEN_KEYWORD_ATOMIC)
        {

          if (setup_append(d, text, i)) return 1;
          return 0;
        }
      }
    }

    int reftype= main_token_reftypes[i];
    if ((reftype == TOKEN_REFTYPE_FUNCTION)
     || (reftype == TOKEN_REFTYPE_PROCEDURE)
     || (reftype == TOKEN_REFTYPE_FUNCTION_OR_PROCEDURE))
    {
      QString tmp_schema_name= "";
      if (i > 2)
      {
        QString d= text.mid(main_token_offsets[i-1], main_token_lengths[i-1]);
        if (d == ".")
        {
          if (main_token_reftypes[i-2] == TOKEN_REFTYPE_DATABASE)
          {
            tmp_schema_name= text.mid(main_token_offsets[i-2], main_token_lengths[i-2]);
          }
        }
      }
      if (tmp_schema_name == "")
      {
        if (routine_number >= debug_routine_list_schemas.count()) /* pseudo-assert */
        {
          debug_error((char*)"bad routine_number");
          return 1;
        }
        tmp_schema_name= debug_routine_list_schemas.at(routine_number);
      }

      QString c_routine_name= setup_add_delimiters(d);
      QString c_schema_name= setup_add_delimiters(tmp_schema_name);

      /* Todo: What about TOKEN_REFTYPE_FUNCTION_OR_VARIABLE? */
      /* Todo: Are you sure hparse is catching this? */
      /* Todo: you're failing to check whether it's 'P' or 'F' */
      for (int i= 0; i < debug_routine_list_names.count(); ++i)
      {
        QString d_routine_name= debug_routine_list_names.at(i);
        QString d_schema_name= debug_routine_list_schemas.at(i);
        d_routine_name= setup_add_delimiters(d_routine_name);
        d_schema_name= setup_add_delimiters(d_schema_name);
        if ((QString::compare(c_routine_name, d_routine_name, Qt::CaseInsensitive) == 0)
         && (QString::compare(c_schema_name, d_schema_name, Qt::CaseInsensitive) == 0))
        {
          d= debug_routine_list_surrogates.at(i);
        }
      }
    }
    /* Todo: Why are you dumping d but not schema? */
    if (setup_append(d, text, i)) return 1;
    if (i == v_token_number_of_last_token) break;
  }
  return 0;
}

/* Generate: "statement text" but without any changes or special handling.
   This is for "if (statement is being debugged) is false" */
/* Todo: an assert that v_token_number_of_last_token is reasonable. */
/* Todo: copy line feeds and indentation of the original */
int MainWindow::setup_generate_statement_text_as_is(int i_of_statement_start,
                                                     QString text,
                                                     int v_token_number_of_last_token)
{
  int i;
  QString d;
  debug_v_g= debug_v_g + debug_lf;
  i= v_token_number_of_last_token;
  d= text.mid(main_token_offsets[i_of_statement_start],
              (main_token_offsets[i] - main_token_offsets[i_of_statement_start]) + main_token_lengths[i]);
  if (setup_append(d, text, i)) return 1;
  return 0;
}

/*
  SSL test
  --------
  This test was done with MariaDB 10.0.17 source, with files described in
  "Configuring MySQL to use SSL Connections"
  https://dev.mysql.com/doc/refman/5.0/en/using-ssl-connections.html
  1. export MARIADB=10.0.17
  2. Start server with
     mysqld ... --ssl-ca=$HOME/$MARIADB/mysql-test/std_data/cacert.pem --ssl-cert=$HOME/$MARIADB/mysql-test/std_data/server-cert.pem --ssl-key=$HOME/$MARIADB/mysql-test/std_data/server-key.pem
  3. Start client with
     ocelotgui ... --ssl-cert=$HOME/$MARIADB/mysql-test/std_data/client-cert.pem --ssl-key=$HOME/$MARIADB/mysql-test/std_data/client-key.pem
  4. SHOW STATUS LIKE 'Ssl_cipher';
  The result value was DHE-RSA-AES256-SHA. So we declared the test successful.
*/

/*
  Valgrind
  --------

  We get lots of complaints from valgrind about problems that seem to be in
  libraries, including gtk_ and g_ routines, so there is a suppression file.
  Even with the suppressions, some possibly-tolerable leakages are happening:
  * simply closing MainWindow, as opposed to ^Q, will not go via action_exit()
  * --version and --help end with exit(0)
  * dlopen() and mysql_real_connect() are leaving something behind

    This is what is in valgrind suppression file valgrind_suppressions.supp.
    Run valgrind with --suppressions=valgrind_suppressions.supp.
    Put the list in the source files somewhere.
    {
       <Addr4s>
       Memcheck:Addr4
       ...
       fun:FcConfig*
    }
    {
       <Conds>
       Memcheck:Cond
       ...
       fun:g_*
    }
    {
       <Params>
       Memcheck:Param
       ioctl(generic)
       ...
       fun:ioctl*
    }
    {
       <Leaks_g>
       Memcheck:Leak
       ...
       fun:g_*
    }
    {
       <Leaks_gtk>
       Memcheck:Leak
       ...
       fun:gtk_*
    }
    {
       <Leaks_pango>
       Memcheck:Leak
       ...
       fun:pango*
    }
    {
       <Leaks_gl>
       Memcheck:Leak
       ...
       fun:glXGetFBConfigs
    }
    {
       <Leaks_Fc>
       Memcheck:Leak
       ...
       fun:FcConfigParseAndLoad
    }
*/

#include "install_sql.cpp"

#ifdef DBMS_TARANTOOL
#if (OCELOT_THIRD_PARTY==1)
#define OCELOT_THIRD_PARTY_CODE
  #include "third_party.h"
#endif
#endif

