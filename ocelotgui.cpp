/*
  ocelotgui -- Ocelot GUI Front End for MySQL or MariaDB

   Version: 0.9.0 Beta
   Last modified: March 24 2016
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
  General comments

  These are comments about the code.
  The user manual and blog comments are elsewhere; look at manual.htm
  or README.txt or ocelot.ca or ocelot.ca/blog or the repository that
  this program was downloaded from, probably github.com/ocelot-inc/ocelotgui.

  The code files are:
    ocelotgui.pro       small. used by Qt qmake
    ocelotgui.h         headers but also quite a lot of executable
    ocelotgui.cpp       executable
    install_sql.cpp     for creating debugger routines
    ocelotgui.ui        small. used to make ui_ocelotgui.h

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
  for about 50 lines in the CodeEditor section (Digia copyright / BSD license),
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


/*
  We use s_color_list only twice, when checking command-line parameters
  and then to copy its data to q_color_list,
  which will be what we actually use for handle_combo_box_for_color_pick_*
  in the Settings class. This list of color names prefers W3C names
  http://www.w3.org/wiki/CSS/Properties/color/keywords
  but also includes all X11 color names and hex values, a commonly-available list,
  example = https://en.wikipedia.org/wiki/X11_color_names#Color_name_chart
  (including webGray, webGreen, webMaroon, webPurple, and eight
  others that Qt would reject), and adds GrayX11 GreenX11 MaroonX11 PurpleX11.
  Doubtless this has been done many times before, but I couldn't find examples.
*/
static const char *s_color_list[308]=
{"AliceBlue","#F0F8FF",
"AntiqueWhite","#FAEBD7",
"Aqua","#00FFFF",
"Aquamarine","#7FFFD4",
"Azure","#F0FFFF",
"Beige","#F5F5DC",
"Bisque","#FFE4C4",
"Black","#000000",
"BlanchedAlmond","#FFEBCD",
"Blue","#0000FF",
"BlueViolet","#8A2BE2",
"Brown","#A52A2A",
"Burlywood","#DEB887",
"CadetBlue","#5F9EA0",
"Chartreuse","#7FFF00",
"Chocolate","#D2691E",
"Coral","#FF7F50",
"CornflowerBlue","#6495ED",
"Cornsilk","#FFF8DC",
"Crimson","#DC143C",
"Cyan","#00FFFF",
"DarkBlue","#00008B",
"DarkCyan","#008B8B",
"DarkGoldenrod","#B8860B",
"DarkGray","#A9A9A9",
"DarkGreen","#006400",
"DarkKhaki","#BDB76B",
"DarkMagenta","#8B008B",
"DarkOliveGreen","#556B2F",
"DarkOrange","#FF8C00",
"DarkOrchid","#9932CC",
"DarkRed","#8B0000",
"DarkSalmon","#E9967A",
"DarkSeaGreen","#8FBC8F",
"DarkSlateBlue","#483D8B",
"DarkSlateGray","#2F4F4F",
"DarkTurquoise","#00CED1",
"DarkViolet","#9400D3",
"DeepPink","#FF1493",
"DeepSkyBlue","#00BFFF",
"DimGray","#696969",
"DodgerBlue","#1E90FF",
"Firebrick","#B22222",
"FloralWhite","#FFFAF0",
"ForestGreen","#228B22",
"Fuchsia","#FF00FF",
"Gainsboro","#DCDCDC",
"GhostWhite","#F8F8FF",
"Gold","#FFD700",
"Goldenrod","#DAA520",
"Gray","#808080",
"GrayX11","#BEBEBE",
"Green","#008000",
"GreenX11","#00FF00",
"GreenYellow","#ADFF2F",
"Honeydew","#F0FFF0",
"HotPink","#FF69B4",
"IndianRed","#CD5C5C",
"Indigo","#4B0082",
"Ivory","#FFFFF0",
"Khaki","#F0E68C",
"Lavender","#E6E6FA",
"LavenderBlush","#FFF0F5",
"LawnGreen","#7CFC00",
"LemonChiffon","#FFFACD",
"LightBlue","#ADD8E6",
"LightCoral","#F08080",
"LightCyan","#E0FFFF",
"LightGoldenrodYellow","#FAFAD2",
"LightGray","#D3D3D3",
"LightGreen","#90EE90",
"LightPink","#FFB6C1",
"LightSalmon","#FFA07A",
"LightSeaGreen","#20B2AA",
"LightSkyBlue","#87CEFA",
"LightSlateGray","#778899",
"LightSteelBlue","#B0C4DE",
"LightYellow","#FFFFE0",
"Lime","#00FF00",
"LimeGreen","#32CD32",
"Linen","#FAF0E6",
"Magenta","#FF00FF",
"Maroon","#800000",
"MaroonX11","#B03060",
"MediumAquamarine","#66CDAA",
"MediumBlue","#0000CD",
"MediumOrchid","#BA55D3",
"MediumPurple","#9370DB",
"MediumSeaGreen","#3CB371",
"MediumSlateBlue","#7B68EE",
"MediumSpringGreen","#00FA9A",
"MediumTurquoise","#48D1CC",
"MediumVioletRed","#C71585",
"MidnightBlue","#191970",
"MintCream","#F5FFFA",
"MistyRose","#FFE4E1",
"Moccasin","#FFE4B5",
"NavajoWhite","#FFDEAD",
"Navy","#000080",
"OldLace","#FDF5E6",
"Olive","#808000",
"OliveDrab","#6B8E23",
"Orange","#FFA500",
"OrangeRed","#FF4500",
"Orchid","#DA70D6",
"PaleGoldenrod","#EEE8AA",
"PaleGreen","#98FB98",
"PaleTurquoise","#AFEEEE",
"PaleVioletRed","#DB7093",
"PapayaWhip","#FFEFD5",
"PeachPuff","#FFDAB9",
"Peru","#CD853F",
"Pink","#FFC0CB",
"Plum","#DDA0DD",
"PowderBlue","#B0E0E6",
"Purple","#800080",
"PurpleX11","#A020F0",
"RebeccaPurple","#663399",
"Red","#FF0000",
"RosyBrown","#BC8F8F",
"RoyalBlue","#4169E1",
"SaddleBrown","#8B4513",
"Salmon","#FA8072",
"SandyBrown","#F4A460",
"SeaGreen","#2E8B57",
"Seashell","#FFF5EE",
"Sienna","#A0522D",
"Silver","#C0C0C0",
"SkyBlue","#87CEEB",
"SlateBlue","#6A5ACD",
"SlateGray","#708090",
"Snow","#FFFAFA",
"SpringGreen","#00FF7F",
"SteelBlue","#4682B4",
"Tan","#D2B48C",
"Teal","#008080",
"Thistle","#D8BFD8",
"Tomato","#FF6347",
"Turquoise","#40E0D0",
"Violet","#EE82EE",
"WebGray","#808080",
"WebGreen","#008000",
"WebMaroon","#7F0000",
"WebPurple","#7F007F",
"Wheat","#F5DEB3",
"White","#FFFFFF",
"WhiteSmoke","#F5F5F5",
"Yellow","#FFFF00",
"YellowGreen","#9ACD32",
"",""};

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
  static unsigned short ocelot_connect_expired_password= 0;/* --connect_expired_password */
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

  /* Some items we allow, which are not available in mysql client */
  static char* ocelot_opt_bind_as_utf8= 0;              /* for MYSQL_OPT_BIND */
  static unsigned short ocelot_opt_can_handle_expired_passwords= 0;/* for MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS */
  static char* ocelot_opt_connect_attr_delete_as_utf8= 0;  /* for MYSQL_OPT_CONNECT_ATTR_DELETE */
  static unsigned short int ocelot_opt_connect_attr_reset= 0; /* for MYSQL_OPT_CONNECT_ATTR_RESET */
  static char* ocelot_read_default_file_as_utf8= 0; /* for MYSQL_READ_DEFAULT_FILE */
  static char* ocelot_read_default_group_as_utf8= 0;/* for MYSQL_READ_DEFAULT_GROUP */
  static unsigned int ocelot_opt_read_timeout= 0;          /* for MYSQL_OPT_READ_TIMEOUT */
  static unsigned short int ocelot_report_data_truncation= 0; /* for MYSQL_REPORT_DATA_TRUNCATION */
  static unsigned short int ocelot_opt_use_result= 0; /* for MYSQL_OPT_USE_RESULT */
  /* It's easy to increase ocelot_grid_tabs so more multi results are seen but then start time is longer. */
  static unsigned short int ocelot_grid_tabs= 2;

  /* Items affecting history which cannot be changed except by modifying + rebuilding */
  //static unsigned int ocelot_history_max_column_width= 10;
  //static unsigned int ocelot_history_max_column_count= 5;
  //static unsigned int ocelot_history_max_row_count= 5;

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
  static int is_libtarantoolnet_loaded= 0;
  static void *libtarantool_handle= 0;
  static void *libtarantoolnet_handle= 0;
  /* Todo: these shouldn't be global */
  tnt_reply *tarantool_tnt_for_new_result_set;

#endif

  static  unsigned int rehash_result_column_count= 0;
  static unsigned int rehash_result_row_count= 0;
  static char *rehash_result_set_copy= 0;       /* gets a copy of mysql_res contents, if necessary */
  static char **rehash_result_set_copy_rows= 0; /* dynamic-sized list of result_set_copy row offsets, if necessary */

  int options_and_connect(unsigned int connection_number);

  /* This should correspond to the version number in the comment at the start of this program. */
  static const char ocelotgui_version[]="0.9 Beta"; /* For --version. Make sure it's in manual too. */


/* Global mysql definitions */
#define MYSQL_MAIN_CONNECTION 0
#define MYSQL_DEBUGGER_CONNECTION 1
#define MYSQL_KILL_CONNECTION 2
#ifdef DEBUGGER
  #define MYSQL_MAX_CONNECTIONS 3
  static MYSQL mysql[3];
  static int connected[3]= {0, 0, 0};
  pthread_t debug_thread_id;
  bool debug_thread_exists= false;
#else
  #define MYSQL_MAX_CONNECTIONS 1
  static MYSQL mysql[1];
  static int connected[1]= {0};
#endif

#ifdef DBMS_TARANTOOL
/* Global Tarantool definitions */
  static struct tnt_stream *tnt;
  static int tarantool_errno;
  static char tarantool_errmsg[1024];
#endif

  static ldbms *lmysql= 0;

  static bool is_mysql_library_init_done= false;

  QString hparse_dbms;
  QString hparse_text_copy;
  QString hparse_token;
  int hparse_i;
  QString hparse_expected;
  int hparse_errno;
  int hparse_token_type;
  int hparse_offset_of_space_name= -1;
  int hparse_statement_type= -1;
  int hparse_number_of_literals= 0;
  int hparse_indexed_condition_count= 0;
  char hparse_errmsg[1024];
  QString hparse_next_token, hparse_next_next_token;
  int hparse_next_token_type, hparse_next_next_token_type;
  bool hparse_begin_seen;
  int hparse_count_of_accepts;
  QString hparse_prev_token;
  bool hparse_like_seen;
  bool hparse_subquery_is_allowed;
  QString hparse_delimiter_str;
  bool hparse_sql_mode_ansi_quotes= false;

int main(int argc, char *argv[])
{
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

  /* Initialization */

  main_window_maximum_width= 0;
  main_window_maximum_height= 0;

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
#ifdef __linux
  ui->menuBar->setNativeMenuBar(false);
#endif

  setWindowTitle("ocelotgui");

  connections_is_connected[0]= 0;
  mysql_res= 0;

  /* client variable defaults */
  /* Most settings done here might be overridden when connect_mysql_options_2 reads options. */
  /* TEST! */
#ifdef DBMS_TARANTOOL
  ocelot_dbms= "tarantool";
  connections_dbms[0]= DBMS_TARANTOOL;
#else
  ocelot_dbms= "mysql";
  connections_dbms[0]= DBMS_MYSQL;
#endif
  ocelot_grid_max_row_lines= 5; /* obsolete? */               /* maximum number of lines in 1 row. warn if this is exceeded? */
  ocelot_statement_prompt_background_color= "lightGray"; /* set early because initialize_widget_statement() depends on this */
  ocelot_grid_border_color= "black";
  ocelot_grid_header_background_color= "lightGray";
  ocelot_grid_cell_drag_line_color= "lightBlue";
  ocelot_grid_cell_border_color= "black";
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

  history_edit_widget= new QTextEdit(this);         /* 2015-08-25 added "this" */
  statement_edit_widget= new CodeEditor(this);
  statement_edit_widget->is_debug_widget= false;

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
  set_current_colors_and_font(); /* set ocelot_statement_text_color, ocelot_grid_text_color, etc. */
  ocelot_statement_border_color= "black";

  ocelot_statement_highlight_literal_color= "red";
  ocelot_statement_highlight_identifier_color= "green";
  ocelot_statement_highlight_comment_color= "limeGreen";
  ocelot_statement_highlight_operator_color= "darkGray";
  ocelot_statement_highlight_keyword_color= "blue";
  ocelot_statement_highlight_current_line_color= "yellow";
  ocelot_statement_syntax_checker= "1";

  ocelot_history_border_color= "black";
  ocelot_menu_border_color= "black";

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

  for (int q_i= 0; strcmp(s_color_list[q_i]," ") > 0; ++q_i) q_color_list.append(s_color_list[q_i]);
  assign_names_for_colors();

  make_style_strings();
  //main_window->setStyleSheet(ocelot_menu_style_string);
  ui->menuBar->setStyleSheet(ocelot_menu_style_string);
  initialize_widget_history();

  initialize_widget_statement();
  {
    ResultGrid *r;
    int i_r;
    for (i_r= 0; i_r < ocelot_grid_tabs; ++i_r)
    {
      r= new ResultGrid(lmysql, this);
      result_grid_tab_widget->addTab(r, QString::number(i_r + 1));
      r->hide(); /* Maybe this isn't necessary */
    }
    for (i_r= 0; i_r < ocelot_grid_tabs; ++i_r)
    {
      r= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(i_r));
      assert(r != 0);
      r->installEventFilter(this); /* must catch fontChange, show, etc. */
      r->grid_vertical_scroll_bar->installEventFilter(this);
    }
    for (int i_r= 0; i_r < ocelot_grid_tabs; ++i_r)
    {
      r= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(i_r));
      r->set_all_style_sheets(ocelot_grid_style_string, ocelot_grid_cell_drag_line_size);
    }
  }

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
  connect(statement_edit_widget->document(), SIGNAL(contentsChanged()), this, SLOT(action_statement_edit_widget_text_changed()));
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
  Todo: Consider: Perhaps this should not be in MainWindow:: but in CodeEditor::.
  Todo: Consider: Perhaps this should be a menu item, not a filter event.
                  (There's already a menu item, but it's not for Enter|Return.)
  There are a few "Ocelot keyword" items that do not require ";" or delimiter
  provided they're the first word, for example "QUIT".
*/
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
  QString text;

//  if (obj == result_grid_table_widget[0]->grid_vertical_scroll_bar)
//  {
//    /* BING */
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


  {
    ResultGrid* r;
    for (int i_r= 0; i_r < ocelot_grid_tabs; ++i_r)
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
  if (key->matches(QKeySequence::Open)) { action_connect(); return true; }
  if (key->matches(QKeySequence::Quit)) { action_exit(); return true; }
  if (key->matches(QKeySequence::Undo)) { action_undo(); return true; }
  //if (key->matches(QKeySequence::Redo)) { redo(); return true; } TODO: handle this, eh?
  Qt::KeyboardModifiers modifiers= key->modifiers();
  if ((modifiers & (Qt::ControlModifier | Qt::ShiftModifier | Qt::AltModifier | Qt::MetaModifier)) == Qt::ControlModifier)
  {
    if (key->key() == Qt::Key_P) { history_markup_previous(); return true; }
    if (key->key() == Qt::Key_N) { history_markup_next(); return true; }
    if (key->key() == Qt::Key_E) { action_execute(1); return true; }
    if (menu_run_action_kill->isEnabled() == true)
    {
      if (key->key() == Qt::Key_C) { action_kill(); return true; }
    }
  }
  if ((modifiers & (Qt::ControlModifier | Qt::ShiftModifier | Qt::AltModifier | Qt::MetaModifier)) == Qt::AltModifier)
  {
    if (menu_debug_action_breakpoint->isEnabled())
      if (key->key() == Qt::Key_1) { action_debug_breakpoint(); return true; }
    if (menu_debug_action_continue->isEnabled())
      if (key->key() == Qt::Key_2) { action_debug_continue(); return true; }
    if (menu_debug_action_next->isEnabled())
      if (key->key() == Qt::Key_3) { action_debug_next(); return true; }
    if (menu_debug_action_step->isEnabled())
      if (key->key() == Qt::Key_5) { action_debug_step(); return true; }
    if (menu_debug_action_clear->isEnabled())
      if (key->key() == Qt::Key_6) { action_debug_clear(); return true; }
    if (menu_debug_action_exit->isEnabled())
      if (key->key() == Qt::Key_7) { action_debug_exit(); return true; }
    if (menu_debug_action_information->isEnabled())
      if (key->key() == Qt::Key_8) { action_debug_information(); return true; }
    if (menu_debug_action_refresh_server_variables->isEnabled())
      if (key->key() == Qt::Key_9) { action_debug_refresh_server_variables(); return true; }
    if (menu_debug_action_refresh_user_variables->isEnabled())
      if (key->key() == Qt::Key_0) { action_debug_refresh_user_variables(); return true; }
    if (menu_debug_action_refresh_variables->isEnabled())
      if (key->key() == Qt::Key_A) { action_debug_refresh_variables(); return true; }
    if (menu_debug_action_refresh_call_stack->isEnabled())
      if (key->key() == Qt::Key_B) { action_debug_refresh_call_stack(); return true; }
  }

  if (obj != statement_edit_widget) return false;

  if ((key->key() == Qt::Key_Tab) && (ocelot_auto_rehash > 0))
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
  if (is_client_statement(first_token_type) == true)
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
    get_next_statement_in_string(0, &returned_begin_count, false);
    if (returned_begin_count == 0)
    {
      if (QString::compare(last_token, ";", Qt::CaseInsensitive) == 0)
      {
        return true;
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
  ocelot_history_includes_warnings                 default no

  The statement is always followed by an error message,
  but ocelot_history_includes_warnings is affected by ...
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

void MainWindow::initialize_widget_history()
{
  history_edit_widget->setStyleSheet(ocelot_history_style_string);
  history_edit_widget->setReadOnly(false);       /* if history shouldn't be editable, set to "true" here */
  history_edit_widget->hide();                   /* hidden until a statement is executed */
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
    history_statement.append(result_set_for_history);
    history_statement.append("<//pre>");
  }
  history_statement.append(history_markup_statement_end);

  history_edit_widget->append(history_statement);

  history_markup_counter= 0;

  if (is_interactive == false) return;

  /*  not related to markup, just a convenient place to call */
  history_file_write("TEE", query_utf16);
  history_file_write("HIST", query_utf16);
  history_file_write("TEE", statement_edit_widget->result);
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
  * the options --tee=filename and --no-tee exist and are checked (I think)
  * the client statements tee filename and notee will be seen
  * there are no menu options (todo: decide whether this is a flaw)
  * apparently the mysql client would flush, therefore we call flush() after writing
  * the mysql client would include results from queries, but we don't (todo: this is a flaw)
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
#ifdef __linux
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
  query_utf16= "/* Start Of Session */;";
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
  With Ubuntu 15.04 the event filer is executed.
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
  connect(menu_run_action_execute, SIGNAL(triggered()), this, SLOT(action_execute_force()));
  menu_run_action_execute->setShortcut(QKeySequence(tr("Ctrl+E")));
  menu_run_action_kill= menu_run->addAction(tr("Kill"));
  connect(menu_run_action_kill, SIGNAL(triggered()), this, SLOT(action_kill()));
  menu_run_action_kill->setShortcut(QKeySequence(tr("Ctrl+C")));
  menu_run_action_kill->setEnabled(false);

  menu_settings= ui->menuBar->addMenu(tr("Settings"));
  menu_settings_action_menu= menu_settings->addAction(tr("Menu"));
  menu_settings_action_history= menu_settings->addAction(tr("History Widget"));
  menu_settings_action_grid= menu_settings->addAction(tr("Grid Widget"));
  menu_settings_action_statement= menu_settings->addAction(tr("Statement Widget"));
  menu_settings_action_extra_rule_1= menu_settings->addAction(tr("Extra Rule 1"));
  connect(menu_settings_action_menu, SIGNAL(triggered()), this, SLOT(action_menu()));
  connect(menu_settings_action_history, SIGNAL(triggered()), this, SLOT(action_history()));
  connect(menu_settings_action_grid, SIGNAL(triggered()), this, SLOT(action_grid()));
  connect(menu_settings_action_statement, SIGNAL(triggered()), this, SLOT(action_statement()));
  connect(menu_settings_action_extra_rule_1, SIGNAL(triggered()), this, SLOT(action_extra_rule_1()));

  menu_options= ui->menuBar->addMenu(tr("Options"));
  menu_options_action_option_detach_history_widget= menu_options->addAction(tr("detach history widget"));
  menu_options_action_option_detach_history_widget->setCheckable(true);
  menu_options_action_option_detach_history_widget->setChecked(ocelot_detach_history_widget);
  connect(menu_options_action_option_detach_history_widget, SIGNAL(triggered(bool)), this, SLOT(action_option_detach_history_widget(bool)));
  menu_options_action_option_detach_result_grid_widget= menu_options->addAction(tr("detach result grid widget"));
  menu_options_action_option_detach_result_grid_widget->setCheckable(true);
  menu_options_action_option_detach_result_grid_widget->setChecked(ocelot_detach_result_grid_widget);
  connect(menu_options_action_option_detach_result_grid_widget, SIGNAL(triggered(bool)), this, SLOT(action_option_detach_result_grid_widget(bool)));
  menu_options_action_option_detach_debug_widget= menu_options->addAction(tr("detach debug widget"));
  menu_options_action_option_detach_debug_widget->setCheckable(true);
  menu_options_action_option_detach_debug_widget->setChecked(ocelot_detach_debug_widget);
  connect(menu_options_action_option_detach_debug_widget, SIGNAL(triggered(bool)), this, SLOT(action_option_detach_debug_widget(bool)));

#ifdef DEBUGGER
  menu_debug= ui->menuBar->addMenu(tr("Debug"));

//  menu_debug_action_install= menu_debug->addAction(tr("Install"));
//  connect(menu_debug_action_install, SIGNAL(triggered()), this, SLOT(action_debug_install()));
//  menu_debug_action_install->setShortcut(QKeySequence(tr("Alt+A")));

//  menu_debug_action_setup= menu_debug->addAction(tr("Setup"));
//  connect(menu_debug_action_setup, SIGNAL(triggered()), this, SLOT(action_debug_setup()));
//  menu_debug_action_setup->setShortcut(QKeySequence(tr("Alt+5")));

//  menu_debug_action_debug= menu_debug->addAction(tr("Debug"));
//  connect(menu_debug_action_debug, SIGNAL(triggered()), this, SLOT(action_debug_debug()));
//  menu_debug_action_debug->setShortcut(QKeySequence(tr("Alt+3")));

  menu_debug_action_breakpoint= menu_debug->addAction(tr("Breakpoint"));
  connect(menu_debug_action_breakpoint, SIGNAL(triggered()), this, SLOT(action_debug_breakpoint()));
  menu_debug_action_breakpoint->setShortcut(QKeySequence(tr("Alt+1")));

  menu_debug_action_continue= menu_debug->addAction(tr("Continue"));
  connect(menu_debug_action_continue, SIGNAL(triggered()), this, SLOT(action_debug_continue()));
  menu_debug_action_continue->setShortcut(QKeySequence(tr("Alt+2")));

//  menu_debug_action_leave= menu_debug->addAction(tr("Leave"));
//  connect(menu_debug_action_leave, SIGNAL(triggered()), this, SLOT(action_debug_leave()));
//  menu_debug_action_leave->setShortcut(QKeySequence(tr("Alt+B")));

  menu_debug_action_next= menu_debug->addAction(tr("Next"));
  connect(menu_debug_action_next, SIGNAL(triggered()), this, SLOT(action_debug_next()));
  menu_debug_action_next->setShortcut(QKeySequence(tr("Alt+3")));

//  menu_debug_action_skip= menu_debug->addAction(tr("Skip"));
//  connect(menu_debug_action_skip, SIGNAL(triggered()), this, SLOT(action_debug_skip()));
//  menu_debug_action_skip->setShortcut(QKeySequence(tr("Alt+4")));

  menu_debug_action_step= menu_debug->addAction(tr("Step"));
  connect(menu_debug_action_step, SIGNAL(triggered()), this, SLOT(action_debug_step()));
  menu_debug_action_step->setShortcut(QKeySequence(tr("Alt+5")));

  menu_debug_action_clear= menu_debug->addAction(tr("Clear"));
  connect(menu_debug_action_clear, SIGNAL(triggered()), this, SLOT(action_debug_clear()));
  menu_debug_action_clear->setShortcut(QKeySequence(tr("Alt+6")));

//  menu_debug_action_delete= menu_debug->addAction(tr("Delete"));
//  connect(menu_debug_action_delete, SIGNAL(triggered()), this, SLOT(action_debug_delete()));
//  menu_debug_action_delete->setShortcut(QKeySequence(tr("Alt+G")));

  menu_debug_action_exit= menu_debug->addAction(tr("Exit"));
  connect(menu_debug_action_exit, SIGNAL(triggered()), this, SLOT(action_debug_exit()));
  menu_debug_action_exit->setShortcut(QKeySequence(tr("Alt+7")));

  menu_debug_action_information= menu_debug->addAction(tr("Information"));
  connect(menu_debug_action_information, SIGNAL(triggered()), this, SLOT(action_debug_information()));
  menu_debug_action_information->setShortcut(QKeySequence(tr("Alt+8")));

  menu_debug_action_refresh_server_variables= menu_debug->addAction(tr("Refresh server_variables"));
  connect(menu_debug_action_refresh_server_variables, SIGNAL(triggered()), this, SLOT(action_debug_refresh_server_variables()));
  menu_debug_action_refresh_server_variables->setShortcut(QKeySequence(tr("Alt+9")));

  menu_debug_action_refresh_user_variables= menu_debug->addAction(tr("Refresh user_variables"));
  connect(menu_debug_action_refresh_user_variables, SIGNAL(triggered()), this, SLOT(action_debug_refresh_user_variables()));
  menu_debug_action_refresh_user_variables->setShortcut(QKeySequence(tr("Alt+0")));

  menu_debug_action_refresh_variables= menu_debug->addAction(tr("Refresh variables"));
  connect(menu_debug_action_refresh_variables, SIGNAL(triggered()), this, SLOT(action_debug_refresh_variables()));
  menu_debug_action_refresh_variables->setShortcut(QKeySequence(tr("Alt+A")));

  menu_debug_action_refresh_call_stack= menu_debug->addAction(tr("Refresh call_stack"));
  connect(menu_debug_action_refresh_call_stack, SIGNAL(triggered()), this, SLOT(action_debug_refresh_call_stack()));
  menu_debug_action_refresh_call_stack->setShortcut(QKeySequence(tr("Alt+B")));

  debug_menu_enable_or_disable(TOKEN_KEYWORD_BEGIN); /* Disable most of debug menu */

#endif

  menu_help= ui->menuBar->addMenu(tr("Help"));
  menu_help_action_about= menu_help->addAction(tr("About"));
  connect(menu_help_action_about, SIGNAL(triggered()), this, SLOT(action_about()));
  menu_help_action_the_manual= menu_help->addAction(tr("The Manual"));
  connect(menu_help_action_the_manual, SIGNAL(triggered()), this, SLOT(action_the_manual()));

  /* Qt says I should also do "addSeparator" if Motif style. Harmless. */
  ui->menuBar->addSeparator();
  /* exitAction->setPriority(QAction::LowPriority); */

  menu_help_action_libmysqlclient= menu_help->addAction(tr("libmysqlclient"));
  connect(menu_help_action_libmysqlclient, SIGNAL(triggered()), this, SLOT(action_libmysqlclient()));

  menu_help_action_settings= menu_help->addAction(tr("settings"));
  connect(menu_help_action_settings, SIGNAL(triggered()), this, SLOT(action_settings()));

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
           &main_token_lengths[0], &main_token_offsets[0], MAX_TOKENS, (QChar*)"33333", 1, ocelot_delimiter_str, 1);

  tokens_to_keywords(text, 0);
  if (((ocelot_statement_syntax_checker.toInt()) & FLAG_FOR_HIGHLIGHTS) != 0)
  {
    hparse_f_multi_block(text); /* recognizer */
  }
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
  QTextCharFormat format_of_other;
  format_of_other.setForeground(QColor(qt_color(ocelot_statement_text_color)));

  pos= 0;
  /* cur.setPosition(pos, QTextCursor::KeepAnchor); */
  cur.beginEditBlock();                                     /* ought to affect undo/redo stack? */

  {
    /* This sets everything to normal format / no underline. Gets overridden by token formats. */
    QTextCharFormat format_of_white_space;
    cur.setPosition(0, QTextCursor::MoveAnchor);
    cur.setPosition(text.size(), QTextCursor::KeepAnchor);
    format_of_white_space= format_of_other;
    format_of_white_space.setUnderlineStyle(QTextCharFormat::NoUnderline);
    cur.setCharFormat(format_of_white_space);
  }

  for (i= 0; main_token_lengths[i] != 0; ++i)
  {
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
    if (t >= TOKEN_KEYWORDS_START) format_of_current_token= format_of_reserved_word;
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
  column_count= 82; /* If you add or remove items, you have to change this */
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
  row_form_label[++i]= "connect_expired_password"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_connect_expired_password); row_form_width[i]= 5;
  row_form_label[++i]= "connect_tmeout"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_opt_connect_timeout); row_form_width[i]= 5;
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

    row_form_title= tr("Connection Dialog Box");
    row_form_message= message;

    co= new Row_form_box(column_count, row_form_label,
                                       row_form_type,
                                       row_form_is_password, row_form_data,
  //                                     row_form_width,
                                       row_form_title,
                                       "File|Connect. Usually only the first 8 fields are important.",
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
      ocelot_auto_rehash= to_long(row_form_data[8].trimmed());

      i= 9;
      ocelot_auto_vertical_output= to_long(row_form_data[i++].trimmed());
      ocelot_batch= to_long(row_form_data[i++].trimmed());
      ocelot_binary_mode= to_long(row_form_data[i++].trimmed());
      ocelot_bind_address= row_form_data[i++].trimmed();
      ocelot_set_charset_dir= row_form_data[i++].trimmed(); /* "character_sets_dir" */
      ocelot_result_grid_column_names= to_long(row_form_data[i++].trimmed());
      ocelot_column_type_info= to_long(row_form_data[i++].trimmed());
      ocelot_comments= to_long(row_form_data[i++].trimmed());
      ocelot_opt_compress= to_long(row_form_data[i++].trimmed());
      ocelot_connect_expired_password= to_long(row_form_data[i++].trimmed());
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
  if (is_libmysqlclient_loaded == 1) { dlclose(libmysqlclient_handle); is_libmysqlclient_loaded= 0; }
  if (is_libcrypto_loaded == 1) { dlclose(libcrypto_handle); is_libcrypto_loaded= 0; }
  delete_utf8_copies();
  close();
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
  }
  else
  {
    menu_options_action_option_detach_history_widget->setText("detach history widget");
    history_edit_widget->setWindowFlags(Qt::Widget);
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


/* Todo: consider adding   //printf(qVersion()); */
void MainWindow::action_about()
{
  QString the_text= "\
<img src=\":/ocelotgui_logo.png\" alt=\"ocelotgui_logo.png\">\
<b>ocelotgui -- Ocelot Graphical User Interface</b><br>Copyright (c) 2014 by Ocelot Computer Services Inc.<br>\
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
  Todo: have more choice where to look for README.md
        currently we only look on the directory that the executable (this program) is on, i.e.
        applicationDirPath() ""Returns the directory that contains the application executable."
        we could try: according to an option = "documentation" directory
                      ld_run_path, ocelot_login_path, ocelot_plugin_dir
                      some other path used by Qt or MySQL or Linux
                      (prefer the path that has everything)
   Todo: Help | X should find X in the manual and display only X.
*/
void MainWindow::action_the_manual()
{
  QString the_text="\
  <BR><h1>ocelotgui</h1>  \
  <BR>  \
  <BR>Version 0.9.0, March 18 2016  \
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
  <BR>For the most recent version of the manual, see \
  <BR>https://github.com/ocelot-inc/ocelotgui#user-manual \
    ";

  QString application_dir_path= QCoreApplication::applicationDirPath();
  QString readme_path= application_dir_path;
  readme_path.append("/");
  readme_path.append("README.md");
  QFile file(readme_path);
  if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    /* README.md file not found. Not an error but we'll end up using default the_text. */
  }
  else
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
  Message_box *message_box;
  message_box= new Message_box("Help|The Manual", the_text, 960, this);
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
  LD_RUN_PATH=/home/jeanmartin ocelotgui";
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
    action_change_one_setting(ocelot_statement_syntax_checker, new_ocelot_statement_syntax_checker,"ocelot_statement_syntax_checker");

    /* Todo: consider: maybe you have to do a restore like this */
    //text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */
    //tokenize(text.data(),
    //         text.size(),
    //         &main_token_lengths[0], &main_token_offsets[0], MAX_TOKENS, (QChar*)"33333", 1, ocelot_delimiter_str, 1);
    //tokens_to_keywords(text);
    /* statement_edit_widget->statement_edit_widget_left_bgcolor= QColor(ocelot_statement_prompt_background_color); */
    statement_edit_widget->statement_edit_widget_left_treatment1_textcolor= QColor(ocelot_statement_text_color);
    action_statement_edit_widget_text_changed();            /* only for highlight? repaint so new highlighting will appear */
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
    //result_grid_table_widget[0]->set_all_style_sheets();
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
    for (int i= 0; i < new_setting.length(); ++i)
    {
      QString c= new_setting.mid(i, 1);
      text.append(c);
      if (c == "'") text.append(c);
    }
    text.append("';");
    main_token_count_in_statement= 5;
    tokenize(text.data(),
             text.size(),
             &main_token_lengths[0], &main_token_offsets[0], MAX_TOKENS, (QChar*)"33333", 1, ocelot_delimiter_str, 1);
   tokens_to_keywords(text, 0);
   action_execute_one_statement(text);
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
    if (QString::compare(rgb_name, q_color_list[i], Qt::CaseInsensitive) == 0) return q_color_list[i - 1];
  }
  q_color_list.append(rgb_name);
  q_color_list.append(rgb_name);
  return rgb_name;
}

/*
  It's possible to say SET [ocelot color name] = rgb_name e.g. "#FFFFFF"
  or have rgb_name in an options file or get an RGB name with set_current_colors_and_fonts().
  We'd prefer to show as W3C names where possible, X11 names as second choice.
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
void MainWindow::set_current_colors_and_font()
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
  font= widget->font();
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
  Pass: a string which is supposed to have a color name. Return: a canonical color name.
  1. Check color_list (names). If match, return name. Might be a case change e.g. Gray not gray.
  2. Accept some color name variants, return the name in the list. e.g. Gray not Grey.
     They appear in http://www.w3.org/TR/SVG/types.html#ColorKeywords.
  3. Put the color in a QColor. If result is invalid, return "" which means invalid.
  4. Get the color back as #RRGGBB
  3. Check color_list (RGBs). If match, return name. e.g. Gray not #BEBEBE.
  4. Return the #RRGGBB color.
  This does not mean that absolutely no synonyms are allowed -- two names may have the same #RRGGBB.
*/
QString MainWindow::canonical_color_name(QString color_name_string)
{
  QString s;
  for (int i= 0; strcmp(s_color_list[i], "") > 0; i+= 2)
  {
    s= s_color_list[i];
    if (QString::compare(color_name_string, s, Qt::CaseInsensitive) == 0)
    {
      return s;
    }
  }

  if (QString::compare(color_name_string, "Cornflower", Qt::CaseInsensitive) == 0) return "CornflowerBlue";
  if (QString::compare(color_name_string, "Darkgrey", Qt::CaseInsensitive) == 0) return "Darkgray";
  if (QString::compare(color_name_string, "DarkSlateGrey", Qt::CaseInsensitive) == 0) return "DarkSlateGray";
  if (QString::compare(color_name_string, "DimGrey", Qt::CaseInsensitive) == 0) return "DimGray";
  if (QString::compare(color_name_string, "Grey", Qt::CaseInsensitive) == 0) return "Gray";
  if (QString::compare(color_name_string, "LightGoldenrod", Qt::CaseInsensitive) == 0) return "LightGoldenrodYellow";
  if (QString::compare(color_name_string, "LightGrey", Qt::CaseInsensitive) == 0) return "LightGray";
  if (QString::compare(color_name_string, "LightSlateGrey", Qt::CaseInsensitive) == 0) return "LightSlateGray";
  if (QString::compare(color_name_string, "NavyBlue", Qt::CaseInsensitive) == 0) return "Navy";
  if (QString::compare(color_name_string, "SlateGrey", Qt::CaseInsensitive) == 0) return "SlateGray";
  QColor qq_color;
  qq_color.setNamedColor(color_name_string);
  if (qq_color.isValid() == false) return "";                 /* bad color, maybe bad format */
  QString qq_color_name= qq_color.name();                      /* returns name as "#RRGGBB" */
  for (int i= 1; strcmp(s_color_list[i], "") > 0; i+= 2)
  {
    s= s_color_list[i];
    if (QString::compare(qq_color_name, s, Qt::CaseInsensitive) == 0)
    {
      s= s_color_list[i - 1];
      return s;
    }
  }
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

  ocelot_menu_style_string= "color:"; ocelot_menu_style_string.append(qt_color(ocelot_menu_text_color));
  ocelot_menu_style_string.append(";background-color:"); ocelot_menu_style_string.append(qt_color(ocelot_menu_background_color));
  ocelot_menu_style_string.append(";border:1px solid "); ocelot_menu_style_string.append(qt_color(ocelot_menu_border_color));
  ocelot_menu_style_string.append(";font-family:"); ocelot_menu_style_string.append(ocelot_menu_font_family);
  ocelot_menu_style_string.append(";font-size:"); ocelot_menu_style_string.append(ocelot_menu_font_size);
  ocelot_menu_style_string.append("pt;font-style:"); ocelot_menu_style_string.append(ocelot_menu_font_style);
  ocelot_menu_style_string.append(";font-weight:"); ocelot_menu_style_string.append(ocelot_menu_font_weight);

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
  Don't bother with begin_count if PROCEDURE or FUNCTION or TRIGGER or EVENT hasn't been seen.
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
  int begin_count;
  QString last_token, second_last_token;
  QString text;
  int i_of_first_non_comment_seen= -1;

  text= statement_edit_widget->toPlainText(); /* Todo: decide whether I'm doing this too often */
  begin_count= 0;

  /*
    First, check for client statement, because the rules for client statement end are:
    ; OR delimiter OR \n OR \n\r
    but a DELIMITER statement ends with first whitespace after its argument
    or next client statement? or next statement of any kind?
  */
  bool client_statement_seen= false;
  for (i= passed_main_token_number; main_token_lengths[i] != 0; ++i)
  {
    int token= main_token_types[i];
    if ((token == TOKEN_TYPE_COMMENT_WITH_SLASH)
     || (token == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
     || (token == TOKEN_TYPE_COMMENT_WITH_MINUS))
      continue;
    if (is_client_statement(token) == true) client_statement_seen= true;
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
    bool is_maybe_in_compound_statement= 0;
    last_token= "";
    for (i= passed_main_token_number; main_token_lengths[i] != 0; ++i)
    {
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
        ++i; break;
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
            is_maybe_in_compound_statement= 1;
          }
        }
      }

      if (i_of_first_non_comment_seen == -1)
      {
#ifdef DBMS_MARIADB
        if ((main_token_types[i] == TOKEN_KEYWORD_BEGIN)
        ||  (main_token_types[i] == TOKEN_KEYWORD_CASE)
        ||  (main_token_types[i] == TOKEN_KEYWORD_IF)
        ||  (main_token_types[i] == TOKEN_KEYWORD_LOOP)
        ||  (main_token_types[i] == TOKEN_KEYWORD_REPEAT)
        ||  (main_token_types[i] == TOKEN_KEYWORD_WHILE))
        {
          is_maybe_in_compound_statement= 1;
        }
#endif
        if ((main_token_types[i] != TOKEN_TYPE_COMMENT_WITH_SLASH)
         && (main_token_types[i] != TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
         && (main_token_types[i] != TOKEN_TYPE_COMMENT_WITH_MINUS))
        {
          i_of_first_non_comment_seen= i;
        }
      }

      /* For some reason the following was checking TOKEN_KEYWORD_ELSEIF too. Removed. */
      if (is_maybe_in_compound_statement == 1)
      {
        if ((main_token_types[i] == TOKEN_KEYWORD_BEGIN)
        ||  ((main_token_types[i] == TOKEN_KEYWORD_CASE)   && ((i == i_of_first_non_comment_seen) || (main_token_types[i - 1] != TOKEN_KEYWORD_END)))
        ||  ((main_token_types[i] == TOKEN_KEYWORD_IF)     && ((i == i_of_first_non_comment_seen) || (main_token_types[i - 1] != TOKEN_KEYWORD_END)))
        ||  ((main_token_types[i] == TOKEN_KEYWORD_LOOP)   && ((i == i_of_first_non_comment_seen) || (main_token_types[i - 1] != TOKEN_KEYWORD_END)))
        ||  ((main_token_types[i] == TOKEN_KEYWORD_REPEAT) && ((i == i_of_first_non_comment_seen) || (main_token_types[i - 1] != TOKEN_KEYWORD_END)))
        ||  ((main_token_types[i] == TOKEN_KEYWORD_WHILE)  && ((i == i_of_first_non_comment_seen) || (main_token_types[i - 1] != TOKEN_KEYWORD_END))))
        {
          ++begin_count;
        }
        if (main_token_types[i] == TOKEN_KEYWORD_END)
        {
          --begin_count;
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
  Todo: there's a conversion to UTF8 but it should be to what server expects.
  Todo: um, in that case, make server expect UTF8.
  Re comment stripping: currently this is default because in mysql client it's default
    but there should be no assumption that this will always be the case,
    so some sort of warning might be good. The history display has to include
    the comments even if they're not sent to the server, so the caller of this
    routine will use both (original string,offset,length) and (returned string).
    Comments should be replaced with a single space.
    Do not strip comments that start with / * ! or / * M !
*/
int MainWindow::make_statement_ready_to_send(QString text, char *dbms_query, int dbms_query_len)
{
  int token_type;
  unsigned int i;
  QString q;
  char *tmp;
  int tmp_len;
  //unsigned int token_count;
  dbms_query[0]= '\0';
  ///* Ignore last token(s) if delimiter, \G, \g, or (sometimes) go, ego */
  //if (strip_last_token == true)
  //{
  //  int last_i= main_token_number + main_token_count_in_statement - 1;
  //  QString last_token= text.mid(main_token_offsets[last_i], main_token_lengths[last_i]);
  //  if ((last_token == "G") || (last_token == "g"))
  //  {
  //    token_count= main_token_count_in_statement - 2;
  //  }
  //  else token_count= main_token_count_in_statement - 1;
  //}
  //else token_count= main_token_count_in_statement;
  //token_count= main_token_count_in_statement;

  for (i= main_token_number; i < main_token_number + main_token_count_in_statement; ++i)
  {
    token_type= main_token_types[i];
    /* Don't send comments unless --comments or equivalent, or comment is / * special ... */
    if (ocelot_comments == 0)
    {
      if ((token_type == TOKEN_TYPE_COMMENT_WITH_SLASH)
       || (token_type == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
       || (token_type == TOKEN_TYPE_COMMENT_WITH_MINUS))
      {
        if ((text.mid(main_token_offsets[i], 3) != "/*!")
         && (text.mid(main_token_offsets[i], 4) != "/*M!"))
        {
          strcat(dbms_query," ");
          continue;
        }
      }
    }
    /* Don't send \G or \g */
    if ((i < main_token_number - 1)
     && (main_token_lengths[i + 1] == 1)
     && (text.mid(main_token_offsets[i], 2).toUpper() == "\\G"))
    {
      ++i;
      continue;
    }
    /* Don't send delimiter unless it is ; */
    if ((ocelot_delimiter_str != ";")
     && (text.mid(main_token_offsets[i], main_token_lengths[i]) == ocelot_delimiter_str))
    {
      continue;
    }
    /* Preserve whitespace after a token, unless this is the last token */
    int token_length;
    if (i + 1 >= main_token_number + main_token_count_in_statement) token_length= main_token_lengths[i];
    else token_length= main_token_offsets[i + 1] - main_token_offsets[i];
    q= text.mid(main_token_offsets[i], token_length);
    tmp_len= q.toUtf8().size();           /* See comment "UTF8 Conversion" */
    tmp= new char[tmp_len + 1];
    memcpy(tmp, q.toUtf8().constData(), tmp_len);
    tmp[tmp_len]= 0;
    assert(strlen(dbms_query) + strlen(tmp) <= (unsigned int) dbms_query_len);
    strcat(dbms_query, tmp);
    delete [] tmp;
  }
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
      unexpected_error= "mysql_fetch row failed"; /* Beware! Look for a proc that compares routine with this string value! */
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
    if (options_and_connect(MYSQL_DEBUGGER_CONNECTION))
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
    strcpy(command_string, "Debugger requires MySQL version 5.5 or later");
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
      put_diagnostics_in_result();
      return;
    //if (debug_error((char*)"Install failed") != 0) return;
  }
  put_diagnostics_in_result();
}

/* For copyright and license notice of debug_mdbug_install function contents, see beginning of this program. */
void debug_mdbug_install()
{
  ;
}
/* End of debug_mdbug_install. End of effect of licence of debug_mdbug_install_function contents. */


/*
  Debug|Setup
  Todo: This should put up a list box with the routines which the user can execute. No hard coding of test.p8 and test.p9.
        Or, it should look in the log for the last setup by this user, and re-do it.
  Todo: Shortcut = Same as last time.
  Todo: Search for schema name not just routine name.
  Todo: We require SELECT access on mysql.proc. We could change the install_sql.cpp routines so that $setup accesses
        a view of mysql.proc, and have a WHERE clause that restricts to only some procedures.
  Format =  CALL xxxmdbug.setup('[option] object name list');
  debug|setup menu item is removed, one must say $SETUP ...
*/
//void MainWindow::action_debug_setup()
//{
//  char call_statement[512]; /* Todo: this is dangerously small. */
//  int debug_widget_index;
//  QString routine_schema_name[DEBUG_TAB_WIDGET_MAX + 1];
//  QString routine_name[DEBUG_TAB_WIDGET_MAX + 1];
//
//  /*
//    TODO: Put up a list box with the routines that the user has a right to execute.
//    Allow user to pick which ones will go in routine_names[], which will be what is used for setup().
//    Max = DEBUG_TAB_WIDGET_MAX.
//  */
//
//  routine_schema_name[0]= "test";
//  routine_schema_name[1]= "test";
//  routine_name[0]= "p8";
//  routine_name[1]= "p9";
//  routine_name[2]= "";
//
//  /* Make a setup statement. Example possibility = CALL xxxmdbug.setup('test.p8',test.p9'). */
//  strcpy(call_statement, "CALL xxxmdbug.setup('");
//  for (debug_widget_index= 0; ; ++debug_widget_index)
//  {
//    if (strcmp(routine_name[debug_widget_index].toUtf8(), "") == 0) break;
//    if (debug_widget_index > 0) strcat(call_statement, ",");
//    strcat(call_statement, routine_schema_name[debug_widget_index].toUtf8());
//    strcat(call_statement, ".");
//    strcat(call_statement, routine_name[debug_widget_index].toUtf8());
//  }
//  strcat(call_statement, "');");
//  call_statement=%s\n", call_statement);
//
//  statement_edit_widget->setPlainText(call_statement);
//  action_execute(1);
//}


/* Todo: $setup does a GET_LOCK() so that two users won't do $setup at the same time. Figure out a better way. */
void MainWindow::debug_setup_go(QString text)
{
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
    if (debug_error((char*)"Missing routine name(s). Expected syntax is: $setup routine-name [, routine-name ...]") != 0) return;
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
    put_diagnostics_in_result();
    return;
    //if (debug_error((char*)"call xxxmdbug.setup() failed.") != 0) return;
  }
  debug_setup_mysql_proc_insert();
}


/*
  The following routine replaces the procedures xxxmdbug.mysql_proc_insert()
  and xxxmdbug.mysql_proc_insert_caller() in install_sql.cpp, which are no
  longer called from xxxmdbug.setup(). The effect is the same,
  but there is no INSERT into a mysql-database table.
  Also: definition_of_surrogate_routine will have three statements: DROP IF EXISTS, SET, CREATE.
        I could execute them all at once, but prefer to separate. Assume original name didn't contain ;.
        ... For icc_core routines the SET statement might be missing, I don't know why.
*/
void MainWindow::debug_setup_mysql_proc_insert()
{
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
      sprintf(command, "$setup generated %d surrogates but the current maximum is %d'", num_rows - 3, DEBUG_TAB_WIDGET_MAX - 1);
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
              strcpy(tmp, "Could not get a routine definition for ");
              strcat(tmp, row[0]);
              strcat(tmp, ".");
              strcat(tmp, row[1]);
              strcat(tmp, ". Are you the routine creator and/or do you have SELECT privilege for mysql.proc?");
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
                  put_diagnostics_in_result();
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
  put_diagnostics_in_result();
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
  Todo: see if confusion happens if there's condition information, or parameters enclosed inside ''s.
  If there is clearly an error, debug_parse_statement() returns -1 and command_string has an error message.
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
  for (i= 0; main_token_lengths[i] != 0; ++i)
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
    statement_edit_widget->result= tr("ERROR not connected");
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
    sprintf(tmp_string_2, "'%s %d. %s'", "debuggee_state:", debuggee_state, tmp_string);
    if (debuggee_state == DEBUGGEE_STATE_END)
    {
      sprintf(tmp_string_2, "Routine has stopped. Suggested next step is: $EXIT");
    }
    put_message_in_result(tmp_string_2);
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
    if (debug_error((char*)"Missing routine name") != 0) return;
  }

  /*
    If a $debug was started before, and didn't finish, we demand that the user do $exit.
    We could instead call debug_exit_go(1).
    Perhaps $exit will fail somehow, and users will be falsely told they can't debug.
    That's a bug but better than hanging, which is a risk if we make two debug threads.
  */
  if (debug_thread_exists == true)
  {
    if (debug_error((char*)"Debug is already running. Use Debug|exit to stop it. This could be temporary.") != 0) return;
  }

  if (debuggee_state < 0) debuggee_state= DEBUGGEE_STATE_0;
  if (debuggee_state == DEBUGGEE_STATE_END) debuggee_state= DEBUGGEE_STATE_0;
  if (debug_error((char*)"") != 0) return;

  strcpy(routine_schema, q_routine_schema.toUtf8());
  strcpy(routine_name, q_routine_name.toUtf8());

  if (debuggee_state > 0)
  {
    if (debug_error((char*)"Debug is already running. Use Debug|exit to stop it. This could be temporary.") != 0) return;
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
    if (debug_error((char*)"Surrogate not found. Probably $setup wasn't done for a group including this routine.") != 0) return;
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
    if (debug_error((char*)"Surrogate not found. Perhaps $setup was not done?") != 0) return;
  }

  QString routine_schema_name_for_search= select_1_row_result_1;
  QString routine_name_for_search= select_1_row_result_2;

  /*
    This will find all surrogate routines which have the same prefix, xxxmdbug___
    That means they're in the same group. Ignore icc_core.
    Get only the part that's not part of the prefix.
    Interesting idea: you could have a way to show both the surrogate and the original.
  */
  int i, j;
  char i_as_string[10];
  /* Todo: n should not be hard-coded here, it limits us to only 10 routines can be searched */
  for (i= 0, j= 0; i < DEBUG_TAB_WIDGET_MAX; ++i)
  {
    sprintf(i_as_string, "%d", i);
    strcpy(call_statement, "select routine_schema, right(routine_name,length(routine_name)-12) from information_schema.routines ");
    strcat(call_statement, "where routine_schema = '");
    strcat(call_statement, routine_schema_name_for_search.toUtf8());
    strcat(call_statement, "' and routine_name like '");
    strcat(call_statement, routine_name_for_search.toUtf8());
    strcat(call_statement, "' order by routine_name limit 1 offset ");
    strcat(call_statement, i_as_string);
    result_string= select_1_row(call_statement);
    if (result_string != "")
    {
      char char_result_string[512];
      if (result_string == "mysql_fetch row failed") break;
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
      strcpy(call_statement, error_return.toUtf8());
      strcat(call_statement, " Could not find a routine in the $setup group: ");
      strcat(call_statement, debug_routine_schema_name[debug_widget_index].toUtf8());
      strcat(call_statement,".");
      strcat(call_statement, debug_routine_name[debug_widget_index].toUtf8());
      debug_error(call_statement);                         /* Todo: you forgot to look for icc_core */
      return;
    }
    routine_definition= select_1_row_result_1;             /* = information_schema.routines.routine_definition */

    if (routine_definition == "")
    {
      debug_delete_tab_widgets();
      strcpy(call_statement, "Could not get a routine definition for ");
      strcat(call_statement, debug_routine_name[debug_widget_index].toUtf8());
      strcat(call_statement, ". Are you the routine creator and/or do you have SELECT privilege for mysql.proc?");
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
    if (debug_error((char*)"routine is missing")) return;
  }

  debug_tab_widget->setCurrentIndex(current_widget_index);

  /* Getting ready to create a separate thread for the debugger and 'attach' to it */

  char error_message[512];

  debuggee_state= DEBUGGEE_STATE_0;

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
    sprintf(error_message, "Debuggee not responding. Code = %d. Thread has not been stopped.\n", debuggee_state);
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
    put_diagnostics_in_result();
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

  /*
    To work around a flaw in MDBug, convert all ' to ''.
    Todo: This does not solve for $debug var_proc(''a''bc'');
    so the manual should recommend: use an escape for '.
    Todo: Check if other debug commands containing ', e.g. conditional breakpoints, work.
  */
  if (strchr(command_string, 0x27) != NULL)
  {
    char command_string_2[2048];
    char *iptr, *optr;
    for (iptr= command_string, optr= command_string_2;;)
      {
      if (*iptr == 0x27) { *optr= 0x27; ++optr; }
      *optr= *iptr;
      if (*iptr == '\0') break;
      ++iptr;
      ++optr;
    }
    strcpy(command_string, command_string_2);
  }

  if (debug_call_xxxmdbug_command(command_string) != 0)
  {
    /* Debug failed. Doubtless there's some sort of error message. Put it out now, debug_exit_go() won't override it. */
    put_diagnostics_in_result();
    debug_exit_go(1); /* This tries to tell the debuggee to stop, because we're giving up. */
    return;
  }
  put_diagnostics_in_result();

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
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }

  if (debug_parse_statement(text, command_string, &index_of_number_1, &index_of_number_2) < 0)
  {
    char error_message[512];
    strcpy(error_message, "Error, correct statement format is ");
    if (statement_type == TOKEN_KEYWORD_DEBUG_BREAKPOINT) strcat(error_message," $breakpoint ");
    else if (statement_type == TOKEN_KEYWORD_DEBUG_TBREAKPOINT) strcat(error_message," $tbreakpoint ");
    else strcat(error_message," $clear ");
    strcat(error_message,"[schema_identifier.].routine_identifier] line_number_minimum [-line_number_maximum]");
    if (debug_error(error_message) != 0) return;
  }
  schema_name= debug_q_schema_name_in_statement;
  routine_name= debug_q_routine_name_in_statement;

  if ((schema_name == "") || (routine_name == ""))
  {
    if (debug_error((char*)"Missing routine name") != 0) return;
  }

  if (debug_error((char*)"") != 0) return;

  if (debug_call_xxxmdbug_command(command_string) != 0)
  {
    put_diagnostics_in_result();
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

  printf("in debug_or_clear_go\n");
  printf("  line_number_1=%d\n", line_number_1);
  printf("  line_number_2=%d\n", line_number_2);
  printf("  statement_type=%d\n", statement_type);
  printf("  TOKEN_KEYWORD_DEBUG_BREAKPOINT=%d\n", TOKEN_KEYWORD_DEBUG_BREAKPOINT);

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
  put_diagnostics_in_result();
}


/*
  Todo: The problem with "delete n" is we don't know which breakpoint is n.
  Of course it's in xxxmdbug.breakpoints, but "r breakpoints" only works if we're at a breakpoint.
*/
void MainWindow::debug_delete_go()
{
  put_message_in_result("The $DELETE statement is not supported at this time");
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
  statement_edit_widget->setPlainText("$CONTINUE");
  action_execute(1);
}

/*
  Debug|Step
  This is much like Debug|Continue, but we don't care if we end up on a permanent breakpoint.
*/
void MainWindow::action_debug_step()
{
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
  put_message_in_result("The $SKIP statement is not supported at this time");
}


void MainWindow::debug_source_go()
{
  put_message_in_result("The $SOURCE statement is not supported at this time");
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
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }
  if (debug_parse_statement(text, command_string, &index_of_number_1, &index_of_number_2) < 0)
  {
    if (debug_error((char*)"Overflow") != 0) return;
    return;
  }
  debug_call_xxxmdbug_command(command_string);
  put_diagnostics_in_result();
}


/*
  $execute sql-statement
  todo: this will fail if first token is a comment
  todo: get rid of this, it fails
*/
void MainWindow::debug_execute_go()
{
  put_message_in_result("The $EXECUTE statement is not supported at this time");

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
//  put_diagnostics_in_result();
////  put_message_in_result("This statement is not supported at this time");
}


/* For: $next, $continue, $refresh */
/* The following could be used for all the $... statements where one merely passes the command on to the debuggee */
/* We strip the comments and the ; but if there's junk after the first word it will cause an error, as it should. */
/* Todo: Should you add a semicolon? */
void MainWindow::debug_other_go(QString text)
{
  char command_string[512];
  int index_of_number_1, index_of_number_2;
  QString q_schema_name, q_routine_name;

  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }
  if (debug_parse_statement(text, command_string, &index_of_number_1, &index_of_number_2) < 0)
  {
    if (debug_error((char*)"Overflow") != 0) return;
    return;
  }
  q_schema_name= debug_q_schema_name_in_statement;
  q_routine_name= debug_q_routine_name_in_statement;

  if (debug_error((char*)"") != 0) return;
  if (debug_call_xxxmdbug_command(command_string) != 0) return;
  put_diagnostics_in_result();
}


/*
  Debug|Next
*/
void MainWindow::action_debug_next()
{
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
    statement_edit_widget->setPlainText("$EXIT");
    action_execute(1);
}


/* flagger == 0 means this is a regular $exit; flagger == 1 means we're getting rid of stuff due to a severe severe error */
void MainWindow::debug_exit_go(int flagger)
{
  char call_statement[512];

  if (flagger == 0)
  {
    /* Todo: merge this with debug_error somehow, and make sure nothing's enabled/disabled unless debug/exit succeeded! */
    if (menu_debug_action_exit->isEnabled() == false)
    {
      put_message_in_result("$DEBUG not done");
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
      put_diagnostics_in_result();
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
    put_diagnostics_in_result(); /* This should show the result of the final call or select, so it should be "ok" */
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
  if (debug_call_xxxmdbug_command("information status") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.information_status");
  action_execute(1);
}


/*
  Debug|Refresh server variables
*/
void MainWindow::action_debug_refresh_server_variables()
{
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }
  if (debug_call_xxxmdbug_command("refresh server_variables") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.server_variables");
  action_execute(1);
}


/*
  Debug|Refresh user variables
*/
void MainWindow::action_debug_refresh_user_variables()
{
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }
  if (debug_call_xxxmdbug_command("refresh user_variables") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.user_variables");
  action_execute(1);
}


/*
  Debug|Refresh variables
*/
void MainWindow::action_debug_refresh_variables()
{
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }
  if (debug_call_xxxmdbug_command("refresh variables") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.variables");
  action_execute(1);
}


/*
  Debug|Refresh call_stack
*/
void MainWindow::action_debug_refresh_call_stack()
{
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }
  if (debug_call_xxxmdbug_command("refresh call_stack") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.call_stack");
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
*/
int MainWindow::debug_call_xxxmdbug_command(const char *command)
{
  char call_statement[512];

  strcpy(call_statement, "call xxxmdbug.command('");
  strcat(call_statement, debuggee_channel_name);
  strcat(call_statement, "', '");
  strcat(call_statement, command);
  strcat(call_statement, "');\n");

  if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
  {
    /* Initially this can happen because we start the timer immediately after we call 'attach'. */
    put_diagnostics_in_result();
    return 1;
  }
  debug_statement= command;

  //put_diagnostics_in_result()???

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
  if (dbms_long_query_state == LONG_QUERY_STATE_STARTED) return;

  unexpected_error[0]= '\0';
  strcpy(call_statement, "call xxxmdbug.command('");
  strcat(call_statement, debuggee_channel_name);
  strcat(call_statement, "', 'information status')");

  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debuggee_state == DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR)
    {
      strcpy(unexpected_error, debuggee_state_error);
      strcat(unexpected_error, ". maybe a new $SETUP needed? cannot continue. Suggested next step is: $EXIT");
    }
    else
    {
      if ((debuggee_state < 0) || (debuggee_state == DEBUGGEE_STATE_END)) strcpy(unexpected_error, "debuggee_wait_loop ended");
      else strcpy(unexpected_error, "debuggee_wait_loop() is not happening");
    }
  }

  if (unexpected_error[0] == '\0')
  {
    if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
    {
      /* Initially this can happen because we start the timer immediately after we call 'attach'. */
      strcpy(unexpected_error, "i status command failed");
    }
  }

  if (unexpected_error[0]== '\0')
  {
    const char *query= "select * from xxxmdbug.information_status";
    if (lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], query, strlen(query)))
    {
      strcpy(unexpected_error, "i status command failed (this is not always a severe error)");
    }
  }

  if (unexpected_error[0] == '\0')
  {
    debug_res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
    if (debug_res == NULL)
    {
      strcpy(unexpected_error, "mysql_store_result failed");
    }
  }

  if (unexpected_error[0] == '\0')
  {
    row= lmysql->ldbms_mysql_fetch_row(debug_res);
    if (row == NULL)
    {
      strcpy(unexpected_error, "mysql_fetch row failed");
    }
    else
    {
      if (lmysql->ldbms_mysql_num_fields(debug_res) < 14)
      {
        strcpy(unexpected_error, "mysql_num_fields < 14");
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
      strcpy(debuggee_information_status_last_command_result, unexpected_error);
    }
  }

  //  printf("debuggee_information_status_debugger_name=%s.\n", debuggee_information_status_debugger_name);
  //  printf("debuggee_information_status_debugger_version=%s.\n", debuggee_information_status_debugger_version);
  //  printf("debuggee_information_status_timestamp_of_status_message=%s.\n", debuggee_information_status_timestamp_of_status_message);
  //  printf("debuggee_information_status_number_of_status_message=%s.\n", debuggee_information_status_number_of_status_message);
  //  printf("debuggee_information_status_icc_count=%s.\n", debuggee_information_status_icc_count);
  //  printf("debuggee_information_status_schema_identifier=%s.\n", debuggee_information_status_schema_identifier);
  //  printf("debuggee_information_status_routine_identifier=%s.\n", debuggee_information_status_routine_identifier);
  //  printf("debuggee_information_status_line_number=%s.\n", debuggee_information_status_line_number);
  //  printf("debuggee_information_status_is_at_breakpoint=%s.\n", debuggee_information_status_is_at_breakpoint);
  //  printf("debuggee_information_status_is_at_tbreakpoint=%s.\n", debuggee_information_status_is_at_tbreakpoint);
  //  printf("debuggee_information_status_is_at_routine_exit=%s.\n", debuggee_information_status_is_at_routine_exit);
  //  printf("debuggee_information_status_stack_depth=%s.\n", debuggee_information_status_stack_depth);
  //  printf("debuggee_information_status_last_command=%s.\n", debuggee_information_status_last_command);
  //  printf("debuggee_information_status_last_command_result=%s.\n", debuggee_information_status_last_command_result);
  //  printf("debuggee_information_status_commands_count=%s.\n", debuggee_information_status_commands_count);

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
    if (options_and_connect(MYSQL_KILL_CONNECTION))
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
*/
void* dbms_long_query_thread(void* unused)
{
  (void) unused; /* suppress "unused parameter" warning */

  dbms_long_query_result= lmysql->ldbms_mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], dbms_query, dbms_query_len);
  dbms_long_query_state= LONG_QUERY_STATE_ENDED;
  return ((void*) NULL);
}

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
*/
int MainWindow::action_execute(int force)
{
  QString text;
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
      if (hparse_errno != 0)
      {
        QString s;
        QMessageBox msgbox;
        s= "The Syntax Checker thinks there might be a syntax error. ";
        s.append(hparse_errmsg);
        msgbox.setText(s);
        msgbox.setInformativeText("Do you want to continue?");
        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgbox.setDefaultButton(QMessageBox::Yes);
        if (msgbox.exec() == QMessageBox::No) return 1;
      }
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
    action_execute_one_statement(text);

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
    statement_edit_widget_text_changed_flag= 1;
    remove_statement(text);
    statement_edit_widget_text_changed_flag= 0;
    action_statement_edit_widget_text_changed();
    //widget_sizer();
    /* Try to set history cursor at end so last line is visible. Todo: Make sure this is the right time to do it. */
    history_edit_widget->verticalScrollBar()->setValue(history_edit_widget->verticalScrollBar()->maximum());
    history_edit_widget->show(); /* Todo: find out if this is really necessary */
    if (is_kill_requested == true) break;
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
void MainWindow::action_execute_one_statement(QString text)
{
  //QString text;
  MYSQL_RES *mysql_res_for_new_result_set;
  unsigned short int is_vertical= ocelot_result_grid_vertical; /* true if --vertical or \G or ego */

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

  if (ecs != 1)
  {
    /* The statement was not handled entirely by the client, it must be passed to the DBMS. */

    bool do_something= true;

    /* If DBMS is not (yet) connected, except for certain SET ocelot_... statements, this is an error. */
    if (connections_is_connected[0] == 0)
    {
      if (ecs == 2) statement_edit_widget->result= tr("OK");
      else statement_edit_widget->result= tr("ERROR not connected");
      do_something= false;
    }
    /* If --one-database, and USE caused default database to change, error */
    if ((ocelot_one_database > 0) && (ocelot_database != statement_edit_widget->dbms_database))
    {
      statement_edit_widget->result= tr("ERROR due to --one-database");
      do_something= false;
    }

    if (do_something == true)
    {
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
      dbms_query_len= query_utf16_copy.toUtf8().size();           /* See comment "UTF8 Conversion" */
      dbms_query= new char[(dbms_query_len + 1) * 2];
      dbms_query_len= make_statement_ready_to_send(text,
                                                   dbms_query, dbms_query_len + 1);

      assert(strlen(dbms_query) < ((unsigned int) dbms_query_len + 1) * 2);

      pthread_t thread_id;
#ifdef DBMS_TARANTOOL
      /* todo: for tarantool as for mysql, call with a separate thread so it's killable */
      if (connections_dbms[0] == DBMS_TARANTOOL)
      {
        dbms_long_query_result= tarantool_real_query(dbms_query, dbms_query_len);
        dbms_long_query_state= LONG_QUERY_STATE_ENDED;
      }
      else
#endif
      {
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
      if (dbms_long_query_result)
      {

        /* beep() hasn't been tested because getting sound to work on my computer is so hard */
        if (ocelot_no_beep == 0) QApplication::beep();
        delete []dbms_query;
      }
      else {
        delete []dbms_query;

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
        if (connections_dbms[0] != DBMS_TARANTOOL)
#endif
        mysql_res_for_new_result_set= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
        if (mysql_res_for_new_result_set == 0)
        {
          /*
            Last statement did not cause a result set. We could hide the grid and shrink the
            central window with "result_grid_table_widget[0]->hide()", but we don't.
          */
          get_sql_mode(main_token_types[main_token_number], text);
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

            /*
              Todo: consider whether it would be appropriate to set grid width with
              result_grid_table_widget[0]->result_column_count= lmysql->ldbms_mysql_num_fields(mysql_res);
              but it may be unnecessary, and may cause a crash in garbage_collect()
            */

            result_row_count= lmysql->ldbms_mysql_num_rows(mysql_res);                /* this will be the height of the grid */
          }

          {
            ResultGrid *r;
            for (int i_r= 0; i_r < ocelot_grid_tabs; ++i_r)
            {
              r= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(i_r));
              r->garbage_collect();
            }
            r= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(0));
            //QFont tmp_font;
            //tmp_font= r->font();
            r->fillup(mysql_res,
                      //&tarantool_tnt_reply,
                      connections_dbms[0],
                      this,
                      is_vertical, ocelot_result_grid_column_names,
                      lmysql, ocelot_line_numbers);
            result_grid_tab_widget->setCurrentWidget(r);
            result_grid_tab_widget->tabBar()->hide();
            r->show();
            result_grid_tab_widget->show(); /* Maybe this only has to happen once */
          }

          QString result_set_for_history;
          /*
            Todo: restore this call to copy(), but copy() should be more obviously correct.
          */
          //result_set_for_history= result_grid_table_widget[0]->copy(ocelot_history_max_column_width, ocelot_history_max_column_count, ocelot_history_max_row_count);

          /* Todo: small bug: elapsed_time calculation happens before lmysql->ldbms_mysql_next_result(). */
          /* You must call lmysql->ldbms_mysql_next_result() + lmysql->ldbms_mysql_free_result() if there are multiple sets */
          put_diagnostics_in_result(); /* Do this while we still have number of rows */
          history_markup_append(result_set_for_history, true);

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
            int result_grid_table_widget_index= 1;
            for (;;)
            {

              dbms_long_query_state= LONG_QUERY_STATE_STARTED;
              pthread_create(&thread_id, NULL, &dbms_long_next_result_thread, NULL);

              for (;;)
              {
                QThread48::msleep(10);
                if (dbms_long_query_state == LONG_QUERY_STATE_ENDED) break;
                QApplication::processEvents();
              }
              pthread_join(thread_id, NULL);

              if (dbms_long_query_result != 0) break;

              mysql_res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);

              /* I think the following will help us avoid the "status" return. */
              if (mysql_res == NULL) continue;

              if (result_grid_table_widget_index < ocelot_grid_tabs)
              {
                ResultGrid* r;
                r= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(result_grid_table_widget_index));
                result_grid_tab_widget->tabBar()->show(); /* is this in the wrong place? */
                result_row_count= lmysql->ldbms_mysql_num_rows(mysql_res);                /* this will be the height of the grid */
                r->fillup(mysql_res,
                          //&tarantool_tnt_reply,
                          connections_dbms[0],
                          this,
                          is_vertical,
                          ocelot_result_grid_column_names,
                          lmysql,
                          ocelot_line_numbers);
                r->show();

                //Put in something based on this if you want extra results to go to history:
                //... result_grid_table_widget[result_grid_table_widget_index]->copy(); etc.

                ++result_grid_table_widget_index;
              }

              if (mysql_res != 0) lmysql->ldbms_mysql_free_result(mysql_res);

            }
            mysql_res= 0;
          }

          return;
        }
      }
      put_diagnostics_in_result();
    }
  }

  /* statement is over */
  if (additional_result != TOKEN_KEYWORD_SOURCE)
  {
    history_markup_append("", true); /* add prompt+statement+result to history, with markup */
  }
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

int MainWindow::execute_client_statement(QString text, int *additional_result)
{
//  int i= 0;
  int i2= 0;
  int  sub_token_offsets[10];
  int  sub_token_lengths[10];
  int  sub_token_types[10];
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
    int t= token_type(s.data(), main_token_lengths[i]);
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
    if (connections_dbms[0] == DBMS_TARANTOOL) connect_tarantool(MYSQL_MAIN_CONNECTION);
#endif
    return 1;
  }

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
      put_message_in_result(tr("Error. USE statement has no argument."));
      return 1;
    }
    /* If database name is in quotes or delimited, strip. Todo: stripping might be necessary in lots of cases. */
    s= connect_stripper(s, false);
    int query_len= s.toUtf8().size();                  /* See comment "UTF8 Conversion" */
    char *query= new char[query_len + 1];
    memcpy(query, s.toUtf8().constData(), query_len + 1);

    mysql_select_db_result= lmysql->ldbms_mysql_select_db(&mysql[MYSQL_MAIN_CONNECTION], query);
    delete []query;
    if (mysql_select_db_result != 0) put_diagnostics_in_result();
    else
    {
      statement_edit_widget->dbms_database= s;
      put_message_in_result(tr("Database changed"));
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
      put_message_in_result(tr("Error, SOURCE statement has no argument"));
      return 1;
    }
    s= connect_stripper(s, true);
    QFile file(s);
    bool open_result= file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (open_result == false)
    {
      put_message_in_result(tr("Error, file open() failed"));
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
        action_execute(0);
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
      /* Todo: output a message */
      return 1;
    }
    statement_edit_widget->prompt_as_input_by_user= s;
    put_message_in_result(tr("OK"));
    return 1;
  }

  /* WARNINGS or \W: mysql equivalent. This overrides a command-line option. */
  if (statement_type == TOKEN_KEYWORD_WARNINGS)
  {
    ocelot_history_includes_warnings= 1;
    put_message_in_result(tr("OK"));
    return 1;
  }

  /* NOWARNING or \w: mysql equivalent. This overrides a command-line option. */
  if (statement_type == TOKEN_KEYWORD_NOWARNING)
  {
    ocelot_history_includes_warnings= 0;
    put_message_in_result(tr("OK"));
    return 1;
  }

  /* DELIMITER or \d: mysql equivalent. */
  if (statement_type == TOKEN_KEYWORD_DELIMITER)
  {
    QString s= text.mid(sub_token_offsets[1], sub_token_lengths[1]);
    QString s_result= get_delimiter(s, text, sub_token_offsets[1]);
    if (s_result == "")
    {
      put_message_in_result(tr("Error, delimiter should not be blank"));
      return 1;
    }
    ocelot_delimiter_str= s_result;
    put_message_in_result(tr("OK"));
    return 1;
    }

  /* Todo: the following are placeholders, we want actual actions like what mysql would do. */
  if (statement_type == TOKEN_KEYWORD_QUESTIONMARK)
  {
    put_message_in_result(tr("For HELP, use the Help menu items. For example click: Help | The Manual."));
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_CHARSET)
  {
    put_message_in_result(tr("CHARSET is not implemented."));
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_EDIT)
  {
    put_message_in_result(tr("EDIT is not implemented."));
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_EGO)
  {
    put_message_in_result(tr("EGO does nothing unless it's on its own line after an executable statement, and --named-commands is true."));
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_GO)
  {
    put_message_in_result(tr("GO does nothing unless it's on its own line after an executable statement, and --named-commands is true."));
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_HELP)
  {
    put_message_in_result(tr("For HELP, use the Help menu items. For example click: Help | The Manual."));
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_NOPAGER)
  {
    put_message_in_result(tr("NOPAGER is not implemented."));
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_NOTEE) /* see comment=tee+hist */
  {
    history_file_stop("TEE");
    put_message_in_result(tr("OK"));
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_PAGER)
  {
    put_message_in_result(tr("PAGER is not implemented."));
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_PRINT)
  {
    put_message_in_result(tr("PRINT is not implemented."));
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_REHASH)   /* This overrides a command-line option */
  {
    char result[32];
    sprintf(result, "OK, result set size = %d", rehash_scan());
    ocelot_auto_rehash= 1;
    put_message_in_result(result);
    return 1;
  }
  /* TODO: "STATUS" should output as much information as the mysql client does. */
  if (statement_type == TOKEN_KEYWORD_STATUS)
  {
    if (connections_is_connected[0] != 1) put_message_in_result(tr("not connected."));
    else
    {
      QString s, s2;
      s2= "";
      s= "DBMS version = " + statement_edit_widget->dbms_version;
      s2.append(s);
      s= " Host = " + statement_edit_widget->dbms_host;
      s2.append(s);
      s= " Port = " + statement_edit_widget->dbms_port.toUtf8();
      s2.append(s);
      put_message_in_result(s2);
    }
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_SYSTEM)
  {
    /*
      With mysql client "system ls" would do an ls with system. We use popen not system.
      I don't know whether there is a Windows equivalent; mysql client doesn't support one.
      So the easiest thing for a Windows port is:
      put_message_in_result(tr("SYSTEM is not implemented."));
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
    if (fp == NULL) statement_edit_widget->result.append(tr("popen() failed"));
    else
    {
      while (fgets(result_line, STRING_LENGTH_512, fp) != NULL)
      {
        statement_edit_widget->result.append(result_line);
      }
      status= pclose(fp);
      if (status == -1) statement_edit_widget->result.append(tr("pclose() failed"));
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
    if (history_file_start("TEE", s) == 0) put_message_in_result(tr("Error, fopen failed"));
    else put_message_in_result(tr("OK"));
    return 1;
  }
#ifdef DEBUGGER
  if ((statement_type >= TOKEN_KEYWORD_DEBUG_BREAKPOINT) && (statement_type <= TOKEN_KEYWORD_DEBUG_TBREAKPOINT))
  {
    /* Todo: we have to get off depending on show_compatibility_56. */
    if (statement_edit_widget->dbms_version.contains("5.7") == true)
    {
      QString s= select_1_row("select @@global.show_compatibility_56;");
      if (s == "")
      {
        if (select_1_row_result_1.toInt() == 0)
        {
          QString s= "To use $DEBUG statements with a MySQL 5.7 server, ";
          s.append("you must first execute: ");
          s.append("SET GLOBAL SHOW_COMPATIBILITY_56 = ON;" );
          QMessageBox msgbox;
          msgbox.setText(s);
          msgbox.exec();
          return 1;
        }
      }
    }
  }

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
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_statement_text_color= ccn;
        assign_names_for_colors(); put_message_in_result(tr("OK"));return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_statement_background_color= ccn;
        make_style_strings();
        statement_edit_widget_setstylesheet();
        assign_names_for_colors(); put_message_in_result(tr("OK"));return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_border_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_statement_border_color= ccn;
        make_style_strings();
        statement_edit_widget_setstylesheet();
        assign_names_for_colors(); put_message_in_result(tr("OK"));return 1;
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
        put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_font_size", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_font_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        make_style_strings();
        statement_edit_widget_setstylesheet();
        put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_font_style", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_style(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown font style")); return 1; }
        ocelot_statement_font_style= ccn;
        make_style_strings();
        statement_edit_widget_setstylesheet();
        put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_font_weight", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_weight(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown font weight")); return 1; }
        ocelot_statement_font_weight= ccn;
        make_style_strings();
        statement_edit_widget_setstylesheet();
        put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_highlight_literal_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_statement_highlight_literal_color= ccn;
        assign_names_for_colors(); put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_highlight_identifier_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { statement_edit_widget->result= tr("Unknown color"); return 1; }
        ocelot_statement_highlight_identifier_color= ccn;
        assign_names_for_colors(); put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_highlight_comment_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_statement_highlight_comment_color= ccn;
        assign_names_for_colors(); put_message_in_result(tr("OK"));return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_highlight_operator_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_statement_highlight_operator_color= ccn;
        assign_names_for_colors(); put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_highlight_keyword_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_statement_highlight_keyword_color= ccn;
        assign_names_for_colors(); put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_prompt_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_statement_prompt_background_color= ccn;
        statement_edit_widget->statement_edit_widget_left_bgcolor= QColor(ocelot_statement_prompt_background_color);
        assign_names_for_colors(); put_message_in_result(tr("OK"));return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_highlight_current_line_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_statement_highlight_current_line_color= ccn;
        statement_edit_widget->highlightCurrentLine();
        assign_names_for_colors(); put_message_in_result(tr("OK"));return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_statement_syntax_checker", Qt::CaseInsensitive) == 0)
      {
        QString syntax_checker= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        int syntax_checker_as_int= syntax_checker.toInt();
        if ((syntax_checker_as_int < 0) || (syntax_checker_as_int > 3))
        { put_message_in_result(tr("Syntax checker value must be between 0 and 3")); return 1; }
        ocelot_statement_syntax_checker= syntax_checker;
        put_message_in_result(tr("OK"));return 1;
      }
      bool is_result_grid_style_changed= false;
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_text_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_grid_text_color= ccn;
        assign_names_for_colors(); is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_border_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_grid_border_color= ccn;
        assign_names_for_colors(); is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_grid_background_color= ccn;
        assign_names_for_colors(); is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_header_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
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
        ocelot_grid_font_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_font_style", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_style(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown font style")); return 1; }
        ocelot_grid_font_style= ccn;
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_font_weight", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_weight(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown font weight")); return 1; }
        ocelot_grid_font_weight= ccn;
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_cell_border_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_grid_cell_border_color= ccn;
        assign_names_for_colors(); is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_cell_drag_line_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_grid_cell_drag_line_color= ccn;
        assign_names_for_colors(); is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_border_size", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_border_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_cell_border_size", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_cell_border_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_grid_cell_drag_line_size", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_cell_drag_line_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        is_result_grid_style_changed= true;
      }
      if (is_result_grid_style_changed == true)
      {
        ResultGrid* r;
        make_style_strings();
        for (int i_r= 0; i_r < ocelot_grid_tabs; ++i_r)
        {
          r= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(i_r));
          r->set_all_style_sheets(ocelot_grid_style_string, ocelot_grid_cell_drag_line_size);
        }
        put_message_in_result(tr("OK")); return 1;
      }
      bool is_extra_rule_1_style_changed= false;
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_extra_rule_1_text_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_extra_rule_1_text_color= ccn;
        assign_names_for_colors(); is_extra_rule_1_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_extra_rule_1_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
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
        put_message_in_result(tr("OK")); return 1;
      }

      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_text_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_history_text_color= ccn;
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        assign_names_for_colors(); put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_history_background_color= ccn;
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        assign_names_for_colors(); put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_border_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_history_border_color= ccn;
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        assign_names_for_colors(); put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_font_family", Qt::CaseInsensitive) == 0)
      {
        ocelot_history_font_family= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_font_size", Qt::CaseInsensitive) == 0)
      {
        ocelot_history_font_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_font_style", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_style(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown font style")); return 1; }
        ocelot_history_font_style= ccn;
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_history_font_weight", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_weight(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown font weight")); return 1; }
        ocelot_history_font_weight= ccn;
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_text_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_menu_text_color= ccn;
        make_style_strings();
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        assign_names_for_colors(); put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_background_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_menu_background_color= ccn;
        make_style_strings();
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        assign_names_for_colors(); put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_border_color", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_color_name(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown color")); return 1; }
        ocelot_menu_border_color= ccn;
        make_style_strings();
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        assign_names_for_colors(); put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_font_family", Qt::CaseInsensitive) == 0)
      {
        ocelot_menu_font_family= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        make_style_strings();
        //main_window->setStyleSheet(ocelot_menu_style_string);
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_font_size", Qt::CaseInsensitive) == 0)
      {
        ocelot_menu_font_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false);
        make_style_strings();
        //main_window->setStyleSheet(ocelot_menu_style_string);
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_font_style", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_style(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown font style")); return 1; }
        ocelot_menu_font_style= ccn;
        make_style_strings();
        //main_window->setStyleSheet(ocelot_menu_style_string);
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        put_message_in_result(tr("OK")); return 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "ocelot_menu_font_weight", Qt::CaseInsensitive) == 0)
      {
        QString ccn= canonical_font_weight(connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]), false));
        if (ccn == "") { put_message_in_result(tr("Unknown font weight")); return 1; }
        ocelot_menu_font_weight= ccn;
        make_style_strings();
        //main_window->setStyleSheet(ocelot_menu_style_string);
        ui->menuBar->setStyleSheet(ocelot_menu_style_string);
        put_message_in_result(tr("OK")); return 1;
      }
    }
  }

  return 0;
}


/*
** --auto-rehash or --no-auto-rehash or --skip-auto-rehash. default = TRUE
** unsigned short ocelot_auto_rehash
** Add it to Options menu
** For TOKEN_KEYWORD_REHASH, change ocelot_auto_rehash
** Use scan_rows to make a copy of database/table/column names for current:
   if connect, if reconnect, if "rehash", if "use"
** When user hits \t i.e. tab, if there's an unambiguous name, fill it in
   (don't worry about what happens if user hits tab twice)
** There's also the little matter of whether names are case-sensitive.
They use a hash, we don't.
They are for current database, we're for all (if qualifier). (No, that's doomed due to correlations / aliases.)
In fact we'd like to keep lots of information about columns, for hovering.
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

  if (connections_is_connected[0] != 1) return 0; /* unexpected_error= "not connected"; */

  if (lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION],
                  "select table_name, column_name from information_schema.columns where table_schema = database()"))
    {
      return 0; /* unexpected_error= "select failed"; */
    }

  res= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
  if (res == NULL) return 0; /* unexpected_error= "mysql_store_result failed"; */

  rehash_result_column_count= lmysql->ldbms_mysql_num_fields(res);
  rehash_result_row_count= lmysql->ldbms_mysql_num_rows(res);
  result_max_column_widths= new unsigned int[rehash_result_column_count];
  ResultGrid* r;
  r= qobject_cast<ResultGrid*>(result_grid_tab_widget->widget(0));
#ifdef DBMS_TARANTOOL
  assert(connections_dbms[0] != DBMS_TARANTOOL);
#endif
  r->scan_rows(
          rehash_result_column_count, /* result_column_count, */
          rehash_result_row_count, /* result_row_count, */
          res, /* grid_mysql_res, */
          &rehash_result_set_copy,
          &rehash_result_set_copy_rows,
          &result_max_column_widths);

  lmysql->ldbms_mysql_free_result(res);

  delete [] result_max_column_widths;

  return rehash_result_row_count;
}

/*
  Pass: search string. Return: column name matching searching string.
  Todo: We only look at column[1] column_name. We should look at column[0] table_name.
*/
QString MainWindow::rehash_search(char *search_string)
{
  long unsigned int r;
  char *row_pointer;
  unsigned long column_length;
  unsigned int i;
  char column_value[512];
  unsigned int search_string_length;
  QString tmp_word= "";
  int count_of_hits= 0;

  search_string_length= strlen(search_string);
  column_value[0]= '\0';

  for (r= 0; r < rehash_result_row_count; ++r)
  {
    row_pointer= rehash_result_set_copy_rows[r];
    for (i= 0; i < rehash_result_column_count; ++i)
    {
      memcpy(&column_length, row_pointer, sizeof(unsigned int));
      row_pointer+= sizeof(unsigned int) + sizeof(char);
      /* Now row_pointer points to contents, length has # of bytes */
      if (i == 1)
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
void MainWindow::put_diagnostics_in_result()
{
  unsigned int mysql_errno_result;
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
  if (connections_dbms[0] == DBMS_TARANTOOL)
  {
    /* todo: show elapsed time */
    if (tarantool_errno == 0) s1= "OK";
    else
    {
      s1= "Error. ";
      s1.append(tarantool_errmsg);
    }
    s1.append(elapsed_time_string);
    statement_edit_widget->result= s1;
    return;
  }
#endif

  mysql_errno_result= lmysql->ldbms_mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]);
  mysql_warning_count= lmysql->ldbms_mysql_warning_count(&mysql[MYSQL_MAIN_CONNECTION]);
  if (mysql_errno_result == 0)
  {
    s1= tr("OK ");

    /* This should output, e.g. "Records: 3 Duplicates: 0 Warnings: 0" -- but actually nothing happens. */
    if (lmysql->ldbms_mysql_info(&mysql[MYSQL_MAIN_CONNECTION])!= NULL)
    {
      /* This only works for certain insert, load, alter or update statements */
      s1.append(tr(lmysql->ldbms_mysql_info(&mysql[MYSQL_MAIN_CONNECTION])));
    }
    else
    {
      sprintf(mysql_error_and_state, " %llu rows affected", lmysql->ldbms_mysql_affected_rows(&mysql[MYSQL_MAIN_CONNECTION]));
      s1.append(mysql_error_and_state);
      if (mysql_warning_count > 0)
      {
        sprintf(mysql_error_and_state, ", %d warning", mysql_warning_count);
        s1.append(mysql_error_and_state);
        if (mysql_warning_count > 1) s1.append("s");
      }
    }
    s1.append(elapsed_time_string);
    if (mysql_warning_count > 0)
    {
      if (ocelot_history_includes_warnings > 0)
      {
        lmysql->ldbms_mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "show warnings");
        MYSQL_RES *mysql_res_for_warnings;
        MYSQL_ROW warnings_row;
        QString s;
        // unsigned long connect_lengths[0];
        mysql_res_for_warnings= lmysql->ldbms_mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
        assert(mysql_res_for_warnings != NULL);
        //for (unsigned int wi= 0; wi <= lmysql->ldbms_mysql_warning_count(&mysql[MYSQL_MAIN_CONNECTION]); ++wi)
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
    s1= tr("Error ");
    sprintf(mysql_error_and_state, "%d (%s) ", mysql_errno_result, lmysql->ldbms_mysql_sqlstate(&mysql[MYSQL_MAIN_CONNECTION]));
    s1.append(mysql_error_and_state);
    s2= lmysql->ldbms_mysql_error(&mysql[MYSQL_MAIN_CONNECTION]);
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

void MainWindow::tokenize(QChar *text, int text_length, int *token_lengths,
                           int *token_offsets, int max_tokens, QChar *version,
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
  if (text[char_offset] == '-') /* MySQL 5.7.9 JSON-colum->path operator */
  {
    expected_char= '>';
    if ((char_offset + 1  < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
  }
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
    if (token_lengths[token_number] == 0) goto part_of_token;
    if (token_lengths[token_number] == 1)
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
  if (text[char_offset] == '~') goto one_byte_token; /* ~ one-byte token */
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
  return;
white_space:
  if (token_lengths[token_number] > 0) ++token_number;
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
  //if (*token == '{') return TOKEN_TYPE_LITERAL_WITH_BRACE;
  if ((*token == '{') || (*token == '}')) return TOKEN_TYPE_OPERATOR;
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
  if ((*token > ' ') && (*token < 'A') && (*token != '$')) return TOKEN_TYPE_OPERATOR;
  return TOKEN_TYPE_OTHER;
}


/*
  I got tired of repeating
  if (QString::compare(text.mid(main_token_offsets[0], main_token_lengths[0]), "SELECT", Qt::CaseInsensitive) == 0) ...
  so switched to maintaining a permanent list.
  Two compiler-dependent assumptions: bsearch() exists, and char* can be converted to unsigned long.
  Upper case is reserved, lower case is unreserved, we search both ways.
*/
/* Todo: use "const" and "static" more often */
#define MAX_KEYWORD_LENGTH 32
void MainWindow::tokens_to_keywords(QString text, int start)
{
  /*
    Sorted list of keywords.
    If you change this, you must also change bsearch parameters and change TOKEN_KEYWORD list.
    A * at the end of a word means it's reserved.
    We consider introducers e.g. _UTF8 to be equivalent to reserved words.
  */
  const char strvalues[][MAX_KEYWORD_LENGTH]=
  {
    "?", /* Ocelot keyword, although tokenize() regards it as an operator */
    "ACCESSIBLE*",
    "ADD*",
    "ALL*",
    "ALTER*",
    "ANALYZE*",
    "AND*",
    "AS*",
    "ASC*",
    "ASCII",
    "ASENSITIVE*",
    "BEFORE*",
    "BEGIN",
"BETWEEN*",
    "BIGINT*",
    "BINARY*",
    "BIT",
    "BLOB*",
    "BOOL",
    "BOOLEAN",
    "BOTH*",
    "BY*",
    "CALL*",
    "CASCADE*",
    "CASE*",
    "CHANGE*",
    "CHAR*",
    "CHARACTER*",
    "CHARSET", /* Ocelot keyword */
    "CHECK*",
    "CLEAR", /* Ocelot keyword */
    "COLLATE*",
    "COLUMN*",
    "CONDITION*",
    "CONNECT", /* Ocelot keyword */
    "CONSTRAINT*",
    "CONTINUE*",
    "CONVERT*",
    "CREATE*",
    "CROSS*",
    "CURRENT_DATE*",
    "CURRENT_TIME*",
    "CURRENT_TIMESTAMP*",
    "CURRENT_USER*",
    "CURSOR*",
    "DATABASE*",
    "DATABASES*",
    "DATE",
    "DATETIME",
    "DAY_HOUR*",
    "DAY_MICROSECOND*",
    "DAY_MINUTE*",
    "DAY_SECOND*",
    "DEC*",
    "DECIMAL*",
    "DECLARE*",
    "DEFAULT*",
    "DELAYED*",
    "DELETE*",
    "DELIMITER", /* Ocelot keyword */
    "DESC*",
    "DESCRIBE*",
    "DETERMINISTIC*",
    "DISTINCT*",
    "DISTINCTROW*",
    "DIV*",
    "DO",
    "DOUBLE*",
    "DROP*",
    "DUAL*",
    "EACH*",
    "EDIT", /* Ocelot keyword */
    "EGO", /* Ocelot keyword */
    "ELSE*",
    "ELSEIF*",
    "ENCLOSED*",
    "END",
    "ENUM",
    "ESCAPED*",
    "EVENT",
    "EXISTS*",
    "EXIT*",
    "EXPLAIN*",
    "FALSE*",
    "FETCH*",
    "FLOAT*",
    "FLOAT4*",
    "FLOAT8*",
    "FOR*",
    "FORCE*",
    "FOREIGN*",
    "FROM*",
    "FULLTEXT*",
    "FUNCTION",
"GENERAL",
    "GENERATED*",
    "GEOMETRY",
    "GEOMETRYCOLLECTION",
    "GET*",
    "GO", /* Ocelot keyword */
    "GRANT*",
    "GROUP*",
    "HAVING*",
    "HELP", /* Ocelot keyword */
    "HIGH_PRIORITY*",
    "HOUR_MICROSECOND*",
    "HOUR_MINUTE*",
    "HOUR_SECOND*",
    "IF*",
    "IGNORE*",
    "IN*",
    "INDEX*",
    "INFILE*",
    "INNER*",
    "INOUT*",
    "INSENSITIVE*",
    "INSERT*",
    "INT*",
    "INT1*",
    "INT2*",
    "INT3*",
    "INT4*",
    "INT8*",
    "INTEGER*",
    "INTERVAL",
    "INTO*",
    "IO_AFTER_GTIDS*",
    "IO_BEFORE_GTIDS*",
    "IS*",
    "ITERATE*",
    "JOIN*",
    "KEY*",
    "KEYS*",
    "KILL*",
    "LEADING*",
    "LEAVE*",
    "LEFT*",
    "LIKE*",
    "LIMIT*",
    "LINEAR*",
    "LINES*",
    "LINESTRING",
    "LOAD*",
    "LOCALTIME*",
    "LOCALTIMESTAMP*",
    "LOCK*",
    "LOGFILE",
    "LONG*",
    "LONGBLOB*",
    "LONGTEXT*",
    "LOOP*",
    "LOW_PRIORITY*",
    "MASTER_BIND*",
"MASTER_HEARTBEAT_PERIOD",
    "MASTER_SSL_VERIFY_SERVER_CERT*",
    "MATCH*",
    "MAXVALUE*",
    "MEDIUMBLOB*",
    "MEDIUMINT*",
    "MEDIUMTEXT*",
    "MIDDLEINT*",
    "MINUTE_MICROSECOND*",
    "MINUTE_SECOND*",
    "MOD*",
    "MODIFIES*",
    "MULTILINESTRING",
    "MULTIPOINT",
    "MULTIPOLYGON",
    "NATURAL*",
    "NOPAGER", /* Ocelot keyword */
    "NOT*",
    "NOTEE", /* Ocelot keyword */
    "NOWARNING", /* Ocelot keyword */
    "NO_WRITE_TO_BINLOG*",
    "NULL*",
    "NUMERIC*",
    "ON*",
    "OPTIMIZE*",
    "OPTION*",
    "OPTIONALLY*",
    "OR*",
    "ORDER*",
    "OUT*",
    "OUTER*",
    "OUTFILE*",
    "PAGER", /* Ocelot keyword */
    "PARTITION*",
    "POINT",
    "POLYGON",
    "PRECISION*",
    "PRIMARY*",
    "PRINT", /* Ocelot keyword */
    "PROCEDURE*",
    "PROMPT", /* Ocelot keyword */
    "PURGE*",
    "QUIT", /* Ocelot keyword */
    "RANGE*",
    "READ*",
    "READS*",
    "READ_WRITE*",
    "REAL*",
    "REFERENCES*",
    "REGEXP*",
    "REHASH", /* Ocelot keyword */
    "RELEASE*",
    "RENAME*",
    "REPEAT*",
    "REPLACE*",
    "REQUIRE*",
    "RESIGNAL*",
    "RESTRICT*",
    "RETURN*",
    "RETURNS",
    "REVOKE*",
    "RIGHT*",
    "RLIKE*",
    "ROW",
    "SCHEMA*",
    "SCHEMAS*",
    "SECOND_MICROSECOND*",
    "SELECT*",
    "SENSITIVE*",
    "SEPARATOR*",
    "SERIAL",
    "SERVER",
    "SET*",
    "SHOW*",
    "SIGNAL*",
"SLOW",
    "SMALLINT*",
    "SOURCE", /* Ocelot keyword */
    "SPATIAL*",
    "SPECIFIC*",
    "SQL*",
    "SQLEXCEPTION*",
    "SQLSTATE*",
    "SQLWARNING*",
    "SQL_BIG_RESULT*",
    "SQL_CALC_FOUND_ROWS*",
    "SQL_SMALL_RESULT*",
    "SSL*",
    "STARTING*",
    "STATUS", /* Ocelot keyword */
    "STORED*",
    "STRAIGHT_JOIN*",
    "SYSTEM", /* Ocelot keyword */
    "TABLE*",
    "TABLESPACE",
    "TEE", /* Ocelot keyword */
    "TERMINATED*",
    "THEN*",
    "TIME",
    "TIMESTAMP",
    "TINYBLOB*",
    "TINYINT*",
    "TINYTEXT*",
    "TO*",
    "TRAILING*",
    "TRIGGER*",
    "TRUE*",
    "TRUNCATE",
    "UNDO*",
    "UNICODE",
    "UNION*",
    "UNIQUE*",
    "UNLOCK*",
    "UNSIGNED*",
    "UPDATE*",
    "USAGE*",
    "USE*", /* Ocelot keyword, also reserved word */
    "USING*",
    "UTC_DATE*",
    "UTC_TIME*",
    "UTC_TIMESTAMP*",
    "VALUES*",
    "VARBINARY*",
    "VARCHAR*",
    "VARCHARACTER*",
    "VARYING*",
    "VIEW",
    "VIRTUAL*",
    "WARNINGS", /* Ocelot keyword */
    "WHEN*",
    "WHERE*",
    "WHILE*",
    "WITH*",
    "WRITE*",
"XOR*",
    "YEAR",
    "YEAR_MONTH*",
    "ZEROFILL*",
    "_ARMSCII8*",
    "_ASCII*",
    "_BIG5*",
    "_BINARY*",
    "_CP1250*",
    "_CP1251*",
    "_CP1256*",
    "_CP1257*",
    "_CP850*",
    "_CP852*",
    "_CP866*",
    "_CP932*",
    "_DEC8*",
    "_EUCJPMS*",
    "_EUCKR*",
    "_FILENAME*",
    "_GB2312*",
    "_GBK*",
    "_GEOSTD8*",
    "_GREEK*",
    "_HEBREW*",
    "_HP8*",
    "_KEYBCS2*",
    "_KOI8R*",
    "_KOI8U*",
    "_LATIN1*",
    "_LATIN2*",
    "_LATIN5*",
    "_LATIN7*",
    "_MACCE*",
    "_MACROMAN*",
    "_SJIS*",
    "_SWE7*",
    "_TIS620*",
    "_UCS2*",
    "_UJIS*",
    "_UTF16*",
    "_UTF16LE*",
    "_UTF32*",
    "_UTF8*",
    "_UTF8MB4*",
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
    t= token_type(s.data(), main_token_lengths[i2]);
    main_token_types[i2]= t;
    main_token_flags[i2]= 0;
    if ((t == TOKEN_TYPE_OTHER) && (main_token_lengths[i2] < MAX_KEYWORD_LENGTH))
    {
      /* It's not a literal or operator. Maybe it's a keyword. Convert it to char[]. */
      QByteArray key_as_byte_array= s.toLocal8Bit();
      const char *key= key_as_byte_array.data();
      /* Uppercase it. I don't necessarily have strupr(). */
      for (i= 0; (*(key + i) != '\0') && (i < MAX_KEYWORD_LENGTH); ++i) key2[i]= toupper(*(key + i)); key2[i]= '\0';
      /* If the following assert happens, you inserted/removed something without changing "332" */
      assert(TOKEN_KEYWORD__UTF8MB4 == TOKEN_KEYWORD_QUESTIONMARK + (332 - 1));

      /* Search it with library binary-search. Assume 332 items and everything MAX_KEYWORD_LENGTH bytes long. */
      p_item= (char*) bsearch (key2, strvalues, 332, MAX_KEYWORD_LENGTH, (int(*)(const void*, const void*)) strcmp);
      if (p_item == NULL)
      {
        strcat(key2, "*");
        p_item= (char*) bsearch (key2, strvalues, 332, MAX_KEYWORD_LENGTH, (int(*)(const void*, const void*)) strcmp);
        if (p_item != NULL)
        {
          main_token_flags[i2]= (main_token_flags[i2] | TOKEN_FLAG_IS_RESERVED);
        }
      }
      if (p_item != NULL)
      {
        /* It's in the list, so instead of TOKEN_TYPE_OTHER, make it TOKEN_KEYWORD_something. */
        index= ((((unsigned long)p_item - (unsigned long)strvalues)) / MAX_KEYWORD_LENGTH) + TOKEN_KEYWORDS_START;
        main_token_types[i2]= index;
        if (connections_is_connected[0] == 1)
        {
          if ((index == TOKEN_KEYWORD_GET)
           || (index == TOKEN_KEYWORD_IO_AFTER_GTIDS)
           || (index == TOKEN_KEYWORD_IO_BEFORE_GTIDS)
           || (index == TOKEN_KEYWORD_MASTER_BIND))
          {
            if (statement_edit_widget->dbms_version.contains("mariadb", Qt::CaseInsensitive) == true)
            {
              main_token_flags[i2]= (main_token_flags[i2] & (~TOKEN_FLAG_IS_RESERVED));
            }
          }
          if ((index == TOKEN_KEYWORD_GENERATED)
           || (index == TOKEN_KEYWORD_STORED)
           || (index == TOKEN_KEYWORD_VIRTUAL))
          {
            if (statement_edit_widget->dbms_version.contains("5.7") == false)
            {
              main_token_flags[i2]= (main_token_flags[i2] & (~TOKEN_FLAG_IS_RESERVED));
            }
          }
        }
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

  main_token_count= i2; /* Global. There will be several times when we want to know how many tokens there are. */

  main_token_types[i2]= 0;

  /* The special cases of BEGIN, DO, END, FUNCTION, ROW. */
  int i_of_function= -1;
  int i_of_do= -1;
  int start_of_body= find_start_of_body(text, &i_of_function, &i_of_do);

  tokens_to_keywords_revert(start_of_body, i_of_function, i_of_do, text);

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
  if (is_client_statement(main_token_types[0]) == true)
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
  /* Todo: There is interference with the statement "prompt \u" and maybe other special prompts */
  int xx= -1;
  if (i2  >= 2)
  {
    /* Todo: what about the delimiter? */
    if ((main_token_offsets[1] == 1)
     && (main_token_lengths[1] == 1)
     && (text.mid(main_token_offsets[0], main_token_lengths[0]) == "\\"))
    {
      xx= 0;
    }
    else if ((main_token_lengths[main_token_count - 2] == 1)
     && (main_token_lengths[main_token_count - 1] == 1)
     && (text.mid(main_token_offsets[main_token_count - 2], main_token_lengths[main_token_count - 2]) == "\\"))
    {
      xx= main_token_count - 2;
    }
  }
  if (i2 >= 3)
  {
    s= text.mid(main_token_offsets[i2 - 1], main_token_lengths[i2 - 1]);
    /* Todo: compare with delimiter, which isn't always semicolon. */
    if (s == (QString)ocelot_delimiter_str)
    {
      if ((main_token_lengths[main_token_count - 3] == 1)
       && (main_token_lengths[main_token_count - 2] == 1)
       && (text.mid(main_token_offsets[main_token_count - 3], main_token_lengths[main_token_count - 3]) == "\\"))

      {
        xx= main_token_count - 3;
      }
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
  Return true if the passed token number is for the first word of a client statement.
*/
bool MainWindow::is_client_statement(int token)
{
  if ((token == TOKEN_KEYWORD_QUESTIONMARK)
  ||  (token == TOKEN_KEYWORD_CHARSET)
  ||  (token == TOKEN_KEYWORD_CLEAR)
  ||  (token == TOKEN_KEYWORD_CONNECT)
  ||  (token == TOKEN_KEYWORD_DELIMITER)
  ||  (token == TOKEN_KEYWORD_EDIT)
  ||  (token == TOKEN_KEYWORD_EGO)
  ||  (token == TOKEN_KEYWORD_GO)
  ||  (token == TOKEN_KEYWORD_HELP)
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
   Skipping comments too
   Return: offset for first word of body, or -1 if not-create-routine | body-not-found
   todo: there might be a problem with "create procedure|function function ...".
   No worries: even if this doesn't get everything right, parsing will come later.
*/
int MainWindow::find_start_of_body(QString text, int *i_of_function, int *i_of_do)
{
  int i;
  int create_seen= 0;
  int procedure_seen= 0;
  int function_seen= 0;
  int trigger_seen= 0;
  int trigger_for_seen= 0;
  int trigger_row_seen= 0;
  int parameter_list_seen= 0;
  int parentheses_count= 0;
  int characteristic_seen= 0;
  int data_type_seen= 0;
  int character_set_seen= 0;
  int collate_seen= 0;
  int event_seen= 0;
  int event_do_seen= 0;
  int event_on_seen= 0;

  for (i= 0; main_token_lengths[i] != 0; ++i)
  {
    if ((main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
     || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_SLASH)
     || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_MINUS))
    {
      continue;
    }
    if (trigger_row_seen == 1)
    {
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
         || (main_token_types[i] == TOKEN_KEYWORD_BINARY)
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
         // todo: || (main_token_types[i] == TOKEN_KEYWORD_JSON)
         || (main_token_types[i] == TOKEN_KEYWORD_LINESTRING)
         || (main_token_types[i] == TOKEN_KEYWORD_LONGBLOB)
         || (main_token_types[i] == TOKEN_KEYWORD_LONGTEXT)
         || (main_token_types[i] == TOKEN_KEYWORD_MEDIUMBLOB)
         || (main_token_types[i] == TOKEN_KEYWORD_MEDIUMINT)
         || (main_token_types[i] == TOKEN_KEYWORD_MEDIUMTEXT)
         || (main_token_types[i] == TOKEN_KEYWORD_MULTILINESTRING)
         || (main_token_types[i] == TOKEN_KEYWORD_MULTIPOINT)
         || (main_token_types[i] == TOKEN_KEYWORD_MULTIPOLYGON)
         || (main_token_types[i] == TOKEN_KEYWORD_NUMERIC)
         || (main_token_types[i] == TOKEN_KEYWORD_POINT)
         || (main_token_types[i] == TOKEN_KEYWORD_POLYGON)
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
void MainWindow::tokens_to_keywords_revert(int i_of_body, int i_of_function, int i_of_do, QString text)
{
  int i;                                                /* index of current token */
  int i_prev= -1;                                       /* index of last non-comment token */
  int i_of_while= -1;                                   /* index of last keyword = while */

  for (i= 0; main_token_lengths[i] != 0; ++i)
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
  Todo: disconnect old if already connected.
  TODO: LOTS OF ERROR CHECKS NEEDED IN THIS!
*/
int MainWindow::connect_mysql(unsigned int connection_number)
{
  QString ldbms_return_string;

  ldbms_return_string= "";

  /* Find libmysqlclient. Prefer ld_run_path, within that prefer libmysqlclient.so.18. */
  if (is_libmysqlclient_loaded != 1)
  {
    lmysql->ldbms_get_library(ocelot_ld_run_path, &is_libmysqlclient_loaded, &libmysqlclient_handle, &ldbms_return_string, WHICH_LIBRARY_LIBMYSQLCLIENT18);
  }
  /* if libmysqlclient.so.18 didn't get loaded, try libmysqlclient without a version number */
  if (is_libmysqlclient_loaded != 1)
  {
    lmysql->ldbms_get_library(ocelot_ld_run_path, &is_libmysqlclient_loaded, &libmysqlclient_handle, &ldbms_return_string, WHICH_LIBRARY_LIBMYSQLCLIENT);
  }
  if (is_libmysqlclient_loaded != 1)
  {
    lmysql->ldbms_get_library("", &is_libmysqlclient_loaded, &libmysqlclient_handle, &ldbms_return_string, WHICH_LIBRARY_LIBMYSQLCLIENT18);
  }
  if (is_libmysqlclient_loaded != 1)
  {
    lmysql->ldbms_get_library("", &is_libmysqlclient_loaded, &libmysqlclient_handle, &ldbms_return_string, WHICH_LIBRARY_LIBMYSQLCLIENT);
  }

  /* Todo: The following errors would be better if we put them in diagnostics the usual way. */

  if (is_libmysqlclient_loaded == -2)
  {
    QMessageBox msgbox;
    QString error_message;
    error_message= "Severe error: libmysqlclient does not have these names: ";
    error_message.append(ldbms_return_string);
    error_message.append(". Close ocelotgui, restart with a better libmysqlclient. See Help|libmysqlclient for tips.");
    error_message.append(". For tips about making sure ocelotgui finds the right libmysqlclient, click Help|libmysqlclient");
    msgbox.setText(error_message);
    msgbox.exec();
    delete lmysql;
    return 1;
  }

  if (is_libmysqlclient_loaded == 0)
  {
    QMessageBox msgbox;
    QString error_message;
    error_message= "Error, libmysqlclient was not found or a loading error occurred. Message was: ";
    error_message.append(ldbms_return_string);
    error_message.append(". For tips about making sure ocelotgui finds libmysqlclient, click Help|libmysqlclient");
    msgbox.setText(error_message);
    msgbox.exec();
    delete lmysql;
    return 1;
  }

  if (is_mysql_library_init_done == false)
  {
    if (lmysql->ldbms_mysql_library_init(0, NULL, NULL))
    {
      QMessageBox msgbox;
      msgbox.setText("Error, mysql_library_init() failed");
      msgbox.exec();
      return 1;
    }
    is_mysql_library_init_done= true;
  }

  /* I decided this line is unnecessary, mysql_init is done in options_and_connect() */
  //lmysql->ldbms_mysql_init(&mysql[connection_number]);
  if (the_connect(connection_number))
  {
    put_diagnostics_in_result();
    statement_edit_widget->result.append(tr("Failed to connect. Use menu item File|Connect to try again"));
    return 1;
  }
  statement_edit_widget->result= tr("OK");

  /*
    Todo: This overrides any earlier PROMPT statements by user.
    Probably what we want is a flag: "if user did PROMPT, don't override it."
    Or we want PROMPT statement to change ocelot_prompt.
  */
  statement_edit_widget->prompt_default= ocelot_prompt;
  statement_edit_widget->prompt_as_input_by_user= statement_edit_widget->prompt_default;

  /*
    Collect some variables in case they're needed for "prompt".
    Todo: handle errors better after mysql_ calls here.
    A possible error is: Error 1226 (42000) User ... has exceeded the 'max_queries_per_hour' resource
    Not using the mysql_res global, since this is not for user to see.
  */
  int query_result= lmysql->ldbms_mysql_query(&mysql[connection_number], "select version(), database(), @@port, current_user(), connection_id()");
  if (query_result != 0 ){
    QMessageBox msgbox;
    msgbox.setText("'mysql_query() failed");
    msgbox.exec();
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
    QMessageBox msgbox;
    msgbox.setText("mysql_store_result failed");
    msgbox.exec();
    connections_is_connected[0]= 1;
    return 0;
  }
  connect_row= lmysql->ldbms_mysql_fetch_row(mysql_res_for_connect);
  if (connect_row == NULL)
  {
    QMessageBox msgbox;
    msgbox.setText("mysql_fetch_row failed");
    msgbox.exec();
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
  get_sql_mode(TOKEN_KEYWORD_CONNECT, "");
  connections_is_connected[0]= 1;
  return 0;
}

/*
  We use sql_mode to decide whether "..." is an identifier or a literal.
  So we try to get its value at connect time or if user says
  SET SESSION SQL_MODE ... or SET @@session.sql_mode
*/
void MainWindow::get_sql_mode(int who_is_calling, QString text)
{
  QString sql_mode_string;
  bool sql_mode_string_seen= false;
  bool must_ask_server= false;
  int i2= 0;
  int  sub_token_offsets[10];
  int  sub_token_lengths[10];
  int  sub_token_types[10];

  if (who_is_calling == TOKEN_KEYWORD_SET)
  {
    int i;
    /* Make a copy of the first few tokens, ignoring comments. */
    /* This is a duplication of code in execute_client_statement(). */
    for (i= main_token_number; main_token_lengths[i] != 0; ++i)
    {
      int t= main_token_types[i];
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
    if (text.mid(sub_token_offsets[1], 7).toUpper() == "SESSION")
    {
      i= 2;
    }
    else if (text.mid(sub_token_offsets[1], 9).toUpper() == "@@SESSION")
    {
      if (text.mid(sub_token_offsets[2], 1).toUpper() != ".") return;
      i= 3;
    }
    else return;
    if (text.mid(sub_token_offsets[i], 8).toUpper() != "SQL_MODE") return;
    ++i;
    if (sub_token_lengths[i] == 0) return;
    if ((text.mid(sub_token_offsets[i], 1) != "=")
     && (text.mid(sub_token_offsets[i], 2) != ":=")) return;
    ++i;
    if ((sub_token_types[i] != TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
     && (sub_token_types[i] != TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE)
     && (sub_token_types[i] != TOKEN_TYPE_LITERAL))
    {
      /* User has said SET SESSION sql_mode = not-literal which server can interpret */
      must_ask_server= true;
    }
    else
    {
      /* Uer has said SET SESSION sql_mode = literal which we can interpret as client */
      sql_mode_string= text.mid(sub_token_offsets[i], sub_token_lengths[i]);
      sql_mode_string_seen= true;
    }
  }
  if ((who_is_calling == TOKEN_KEYWORD_CONNECT) || (must_ask_server == true))
  {
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
      hparse_sql_mode_ansi_quotes= true;
    }
    else hparse_sql_mode_ansi_quotes= false;
  }
}

/*
  The routines that start with "hparse_*" are a predictive recursive-descent
  recognizer for MySQL, generally assuming LL(1) grammar but allowing for a few quirks.
  (A recognizer does all the recursive-descent parser stuff except that it generates nothing.)
  Generally recrsive-descent parsers or recognizers are reputed to be good
  because they're simple and can produce good -- often predictive -- error messages,
  but bad because they're huge and slow, and that's certainly the case here.
  The intent is to make highlight and hover look good.
  If any comparison fails, the error message will say:
  what we expected, token number + offset + value for the token where comparison failed.
  Allowed syntaxes are: Any MySQL_5.7 statement, or an Ocelot client statement.
*/

/*
  Todo: Actually identifier length maximum isn't always 64.
  See http://dev.mysql.com/doc/refman/5.7/en/identifiers.html
*/
#define MYSQL_MAX_IDENTIFIER_LENGTH 64

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
*/
void MainWindow::hparse_f_next_nexttoken()
{
  hparse_next_token= hparse_next_next_token= "";
  int saved_hparse_i= hparse_i;
  int saved_hparse_token_type= hparse_token_type;
  QString saved_hparse_token= hparse_token;
  /* todo: check this in h_parse_f_nextsym too? */
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
  }
  hparse_i= saved_hparse_i;
  hparse_token_type= saved_hparse_token_type;
  hparse_token= saved_hparse_token;
}

void MainWindow::hparse_f_error()
{
  if (hparse_errno > 0) return;
  assert(hparse_i >= 0);
  assert(hparse_i < MAX_TOKENS);
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
  while ((unsigned) q_errormsg.toUtf8().length() >= (unsigned int) sizeof(hparse_errmsg) - 1)
    q_errormsg= q_errormsg.left(q_errormsg.length() - 1);
  strcpy(hparse_errmsg, q_errormsg.toUtf8());
  hparse_errno= 10400;
}

/*
  Merely saying "if (hparse_token == 'x') ..." till we saw delimiter usually is not =.
*/
bool MainWindow::hparse_f_is_equal(QString hparse_token_copy, QString token)
{
  if (hparse_token_copy == hparse_delimiter_str) return false;
  if (hparse_token_copy == token) return true;
  return false;
}

/*
  accept means: if current == expected then clear list of what was expected, get next, and return 1,
                else add to list of what was expected, and return 0
*/
int MainWindow::hparse_f_accept(int proposed_type, QString token)
{
  if (hparse_errno > 0) return 0;
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
      hparse_f_nexttoken();
      hparse_f_nexttoken();
      ++hparse_count_of_accepts;
      return 1;
    }
    return 0;
  }
  else if (token == "[identifier]")
  {
    if (hparse_token_type == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK)
    {
      if ((hparse_token.size() == 1) || (hparse_token.right(1) != "`"))
      {
        /* Starts with ` but doesn't end with ` so identifier required but not there yet. */
        hparse_expected= token;
        return 0;
      }
    }
    if (hparse_token_type == TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE)
    {
      if ((hparse_token.size() == 1) || (hparse_token.right(1) != "\""))
      {
        /* Starts with " but doesn't end with " so identifier required but not there yet. */
        hparse_expected= token;
        return 0;
      }
    }
    if ((hparse_token_type == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK)
     || (hparse_token_type == TOKEN_TYPE_IDENTIFIER_WITH_DOUBLE_QUOTE)
     || (hparse_token_type == TOKEN_TYPE_IDENTIFIER_WITH_AT)
     || ((hparse_token_type >= TOKEN_TYPE_OTHER)
      && ((main_token_flags[hparse_i] & TOKEN_FLAG_IS_RESERVED) == 0)))
    {
      equality= true;
      if (hparse_offset_of_space_name == -1) hparse_offset_of_space_name= hparse_i;
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
    if ((hparse_token_type == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
     || (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE)
     || (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_DIGIT)
     /* literal_with_brace == literal */
     || (hparse_token_type == TOKEN_TYPE_LITERAL_WITH_BRACE)) /* obsolete? */
    {
      equality= true;
      ++hparse_number_of_literals;
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
  else if (token == "[column identifier]")
  {
    /* Todo: This should be a simple matter of checking whether it's really an identifier. */
    /* Nah, column identifiers might be qualified. */
    if (hparse_token.length() < MYSQL_MAX_IDENTIFIER_LENGTH)
    {
      equality= true;
    }
  }
  else if (QString::compare(hparse_token, token, Qt::CaseInsensitive) == 0)
  {
    equality= true;
  }

  if (equality == true)
  {
    /*
      Change the token type now that we're sure what it is.
      But for keyword: if it's already more specific, leave it.
      TODO: that exception no longer works because we moved TOKEN_TYPE_KEYWORD to the end
    */
    if (proposed_type == TOKEN_TYPE_KEYWORD)
    {
      if (main_token_types[hparse_i] >= TOKEN_KEYWORDS_START) {;}
      else main_token_types[hparse_i]= proposed_type;
    }
    else main_token_types[hparse_i]= proposed_type;
    hparse_expected= "";
    hparse_f_nexttoken();
    ++hparse_count_of_accepts;
    return 1;
  }
  if (hparse_expected > "") hparse_expected.append(" or ");
  hparse_expected.append(token);
  return 0;
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
    hparse_expected= "";
    hparse_f_nexttoken();
    return 1;
  }
  if (hparse_expected > "") hparse_expected.append(" or ");
  hparse_expected.append(token);
  return 0;
}

/* expect means: if current == expected then get next and return 1; else error */
int MainWindow::hparse_f_expect(int proposed_type, QString token)
{
  if (hparse_errno > 0) return 0;
  if (hparse_f_accept(proposed_type, token)) return 1;
  hparse_f_error();
  return 0;
}

/* [literal] or _introducer [literal], return 1 if true */
/* todo: this is also accepting {ODBC junk} or NULL, sometimes when it shouldn't. */
/* todo: in fact it's far far too lax, you should pass what's acceptable data type */
int MainWindow::hparse_f_literal()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "[introducer]") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
    if (hparse_errno > 0) return 0;
    return 1;
  }
  else if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "{") == 1)
  {
    /* I can't imagine how {oj ...} could be valid if we're looking for a literal */
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "D") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "T") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TS") == 1))
    {
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(TOKEN_TYPE_OPERATOR, "}");
      if (hparse_errno > 0) return 0;
      return 1;
    }
    else hparse_f_error();
    return 0;
  }
  else if ((hparse_f_accept(TOKEN_KEYWORD_NULL, "NULL") == 1)
        || (hparse_f_accept(TOKEN_KEYWORD_TRUE, "TRUE") == 1)
        || (hparse_f_accept(TOKEN_KEYWORD_FALSE, "FALSE") == 1))
  {
    return 1;
  }
  if (hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]") == 1) return 1;
  /* DATE '...' | TIME '...' | TIMESTAMP '...' are literals, but DATE|TIME|TIMESTAMP are not. */
  QString hpu= hparse_token.toUpper();
  if ((hpu == "DATE") || (hpu == "TIME") || (hpu == "TIMESTAMP"))
  {
    hparse_f_next_nexttoken();
    if ((hparse_next_token.mid(0,1) == "\"") || (hparse_next_token.mid(0,1) == "'"))
    {
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATE") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TIME") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TIMESTAMP") == 1))
      {
        hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
        if (hparse_errno > 0) return 0;
        return 1;
      }
    }
  }
  return 0;
}

/*
  DEFAULT is a reserved word which, as an operand, might be
  () the right side of an assignment for INSERT/REPLACE/UPDATE
  () the beginning of DEFAULT(col_name)
*/
int MainWindow::hparse_f_default(int who_is_calling)
{
  if (hparse_f_accept(TOKEN_KEYWORD_DEFAULT, "DEFAULT") == 1)
  {
    bool parenthesis_seen= false;
    if ((who_is_calling == TOKEN_KEYWORD_INSERT)
     || (who_is_calling == TOKEN_KEYWORD_UPDATE)
     || (who_is_calling == TOKEN_KEYWORD_REPLACE))
    {
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1) parenthesis_seen= true;
    }
    else
    {
      hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return 0;
      parenthesis_seen= true;
    }
    if (parenthesis_seen == true)
    {
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return 0;
    }
    return 1;
  }
  return 0;
}

/*
  Beware: we treat @ as a separator so 'a' @ 'b' is a user name.
  MySQL doesn't expect spaces. But I'm thinking it won't cause ambiguity.
*/
int MainWindow::hparse_f_user_name()
{
  if ((hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
   || (hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]") == 1))
  {
    if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "@") == 1)
    {
      if ((hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
       || (hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]") == 1)) {;}
    }
    else if ((hparse_token.mid(0, 1) == "@")
          && (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1))
    {
      ;
    }
    return 1;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CURRENT_USER") == 1)
  {
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return 0;
    }
    return 1;
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
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "ARMSCII8") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ASCII") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BIG5") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BINARY") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CP1250") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CP1251") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CP1256") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CP1257") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CP850") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CP852") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CP866") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CP932") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEC8") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EUCJPMS") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EUCKR") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FILENAME") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GB2312") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GBK") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GEOSTD8") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GREEK") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "HEBREW") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "HP8") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEYBCS2") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KOI8R") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KOI8U") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LATIN1") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LATIN2") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LATIN5") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LATIN7") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MACCE") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MACROMAN") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SJIS") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SWE7") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TIS620") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UCS2") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UJIS") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UTF16") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UTF16LE") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UTF32") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UTF8") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UTF8MB4") == 1))
    return 1;
  if (hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]") == 1) return 1;
  return 0;
}

/* Todo: someday check collation names the way we check character set names. */
int MainWindow::hparse_f_collation_name()
{
  if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) return 1;
  if (hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]") == 1) return 1;
  return 0;
}

/*
  Routines starting with hparse_f_table... are based on
  https://dev.mysql.com/doc/refman/5.5/en/join.html
*/

/*
  Most database objects can be qualifed e.g. "a.b.c.d".
  Todo: be more exact about the maximum number of qualification levels depending on object type.
*/
int MainWindow::hparse_f_qualified_name()
{
  if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
  {
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ".") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ".") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return 0;
        if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ".") == 1)
        {
          hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
          if (hparse_errno > 0) return 0;
        }
      }
    }
    return 1;
  }
  return 0;
}

int MainWindow::hparse_f_qualified_name_with_star() /* like hparse_f_qualified_name but may end with * */
{
  if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
  {
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ".") == 1)
    {
      if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "*") == 1) return 1;
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ".") == 1)
      {
        if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "*") == 1) return 1;
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
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
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  if (hparse_i == saved_hparse_i) return 0;
  return 1;
}

/* table_reference | { OJ table_reference } */
void MainWindow::hparse_f_table_escaped_table_reference()
{
  if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "{") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "OJ");
    if (hparse_errno > 0) return;
    if (hparse_f_table_reference(0) == 0)
    {
      hparse_f_error();
      return;
    }
    hparse_f_expect(TOKEN_TYPE_OPERATOR, "}");
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
      if (who_is_calling == TOKEN_KEYWORD_JOIN) return 1;
      if ((hparse_f_accept(TOKEN_KEYWORD_INNER, "INNER") == 1)
       || (hparse_f_accept(TOKEN_KEYWORD_CROSS, "CROSS") == 1)
       || (hparse_f_accept(TOKEN_KEYWORD_JOIN, "JOIN") == 1)
       || (hparse_f_accept(TOKEN_KEYWORD_STRAIGHT_JOIN, "STRAIGHT_JOIN") == 1)
       || (hparse_f_accept(TOKEN_KEYWORD_LEFT, "LEFT") == 1)
       || (hparse_f_accept(TOKEN_KEYWORD_RIGHT, "RIGHT") == 1)
       || (hparse_f_accept(TOKEN_KEYWORD_OUTER, "OUTER") == 1)
       || (hparse_f_accept(TOKEN_KEYWORD_NATURAL, "NATURAL") == 1))
      {
        hparse_i= saved_hparse_i;
        hparse_token_type= saved_hparse_token_type;
        hparse_token= saved_hparse_token;
        if (hparse_f_table_join_table() == 1)
        {
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
  if (hparse_f_qualified_name() == 1)
  {
    hparse_f_partition_list(false, false);
    if (hparse_errno > 0) return 0;
    if (hparse_f_accept(TOKEN_KEYWORD_AS, "AS") == 1)
    {
      if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
        hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return 0;
    }
    else
    {
      if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
        hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]");
    }
    hparse_f_table_index_hint_list();
    if (hparse_errno > 0) return 0;
    return 1;
  }
  else if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
  {
    if (hparse_f_select(false) == 1)
    {
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return 0;
      hparse_f_accept(TOKEN_KEYWORD_AS, "AS");
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      return 1;
    }
    else
    {
      if (hparse_errno > 0) return 0;
      hparse_f_table_references();
      if (hparse_errno > 0) return 0;
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
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
    bool inner_or_cross_seen= false;
    for (;;)
    {
      if ((hparse_f_accept(TOKEN_KEYWORD_INNER, "INNER") == 1) || (hparse_f_accept(TOKEN_KEYWORD_CROSS, "CROSS") == 1))
      {
        inner_or_cross_seen= true;
        hparse_f_expect(TOKEN_KEYWORD_JOIN, "JOIN");
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
    if (inner_or_cross_seen == true) return 1;
    if (hparse_f_accept(TOKEN_KEYWORD_STRAIGHT_JOIN, "STRAIGHT_JOIN") == 1)
    {
      if (hparse_f_table_factor() == 0)
      {
         hparse_f_error();
         return 0;
      }
      if (hparse_f_accept(TOKEN_KEYWORD_ON, "ON") == 1)
      {
        hparse_f_opr_1();
        if (hparse_errno > 0) return 0;
      }
      return 1;
    }
    if ((hparse_f_accept(TOKEN_KEYWORD_LEFT, "LEFT") == 1) || (hparse_f_accept(TOKEN_KEYWORD_RIGHT, "RIGHT") == 1))
    {
      hparse_f_accept(TOKEN_KEYWORD_OUTER, "OUTER");
      hparse_f_expect(TOKEN_KEYWORD_JOIN, "JOIN");
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
    if (hparse_f_accept(TOKEN_KEYWORD_NATURAL, "NATURAL"))
    {
      if ((hparse_f_accept(TOKEN_KEYWORD_LEFT, "LEFT") == 1) || (hparse_f_accept(TOKEN_KEYWORD_RIGHT, "RIGHT") == 1)) {;}
      else
      {
        hparse_f_error();
        return 0;
      }
      hparse_f_accept(TOKEN_KEYWORD_OUTER, "OUTER");
      hparse_f_expect(TOKEN_KEYWORD_JOIN, "JOIN");
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
  if (hparse_f_accept(TOKEN_KEYWORD_ON, "ON") == 1)
  {
    hparse_f_opr_1();
    if (hparse_errno > 0) return 0;
    return 1;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_USING, "USING") == 1)
  {
    hparse_f_column_list(1);
    if (hparse_errno > 0) return 0;
    return 1;
  }
  return 0;
}

/*  index_hint [, index_hint] ... */
void MainWindow::hparse_f_table_index_hint_list()
{
  do
  {
    if (hparse_f_table_index_hint() == 0) return;
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
}

/* USE    {INDEX|KEY} [FOR {JOIN|ORDER BY|GROUP BY}] ([index_list])
 | IGNORE {INDEX|KEY} [FOR {JOIN|ORDER BY|GROUP BY}] (index_list)
 | FORCE  {INDEX|KEY} [FOR {JOIN|ORDER BY|GROUP BY}] (index_list) */
int MainWindow::hparse_f_table_index_hint()
{
  bool use_seen= false;
  if (hparse_f_accept(TOKEN_KEYWORD_USE, "USE") == 1) use_seen= true;
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IGNORE") == 1)  {;}
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FORCE") == 1)  {;}
  else return 0;
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX") == 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEY") == 0))
  {
    hparse_f_error();
    return 0;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOR") == 1)
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "JOIN") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ORDER") == 1)
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BY") == 0)
      {
        hparse_f_error();
        return 0;
      }
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GROUP") == 1)
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BY") == 0)
      {
        hparse_f_error();
        return 0;
      }
    }
  }
  hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
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
  hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return 0;
  return 1;
}

/* index_name [, index_name] ... */
int MainWindow::hparse_f_table_index_list()
{
  int return_value= 0;
  do
  {
    if (hparse_f_qualified_name() == 1) return_value= 1;
    if (hparse_errno > 0) return 0;
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
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
void MainWindow::hparse_f_opr_1() /* Precedence = 1 (bottom) */
{
  hparse_f_opr_2();
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(TOKEN_TYPE_OPERATOR, ":=") == 1) || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "=") == 1))
  {
    hparse_f_opr_2();
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_2() /* Precedence = 2 */
{
  hparse_f_opr_3();
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "OR") == 1) || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "||") == 1))
  {
    hparse_f_opr_3();
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_3() /* Precedence = 3 */
{
  hparse_f_opr_4();
  if (hparse_errno > 0) return;
  while (hparse_f_accept(TOKEN_TYPE_KEYWORD, "XOR") == 1)
  {
    hparse_f_opr_4();
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_4() /* Precedence = 4 */
{
  hparse_f_opr_5();
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "AND") == 1) || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "&&") == 1))
  {
    hparse_f_opr_5();
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_5() /* Precedence = 5 */
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NOT") == 1) {;}
  hparse_f_opr_6();
  if (hparse_errno > 0) return;
}

/*
  Re MATCH ... AGAINST: unfortunately IN is an operator but also a clause-starter.
  So if we fail because "IN (" was expected, this is the one time when we have to
  override and set hparse_errno back to zero and carry on.
  Re CASE ... END: we change the token types, trying to avoid confusion with CASE statement.
*/
void MainWindow::hparse_f_opr_6() /* Precedence = 6 */
{
  if (hparse_f_accept(TOKEN_KEYWORD_CASE_IN_CASE_EXPRESSION, "CASE") == 1)
  {
    int when_count= 0;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WHEN") == 0)
    {
      hparse_f_opr_1();
      if (hparse_errno > 0) return;
    }
    else when_count= 1;
    for (;;)
    {
      if ((when_count == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WHEN") == 1))
      {
        ++when_count;
        hparse_f_opr_1();
        if (hparse_errno > 0) return;
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "THEN") == 1)
        {
          hparse_f_opr_1();
          if (hparse_errno > 0) return;
        }
        else hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else break;
    }
    if (when_count == 0)
    {
      hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ELSE") == 1)
    {
      hparse_f_opr_1();
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(TOKEN_KEYWORD_END_IN_CASE_EXPRESSION, "END") == 1)
    {
      return;
    }
    else hparse_f_error();
    if (hparse_errno > 0) return;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MATCH") == 1)
  {
    hparse_f_column_list(1);
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "AGAINST");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return;
    hparse_f_opr_1();
    bool in_seen= false;
    if (hparse_errno > 0)
    {
      if (QString::compare(hparse_prev_token, "IN", Qt::CaseInsensitive) != 0) return;
      hparse_errno= 0;
      in_seen= true;
    }
    else
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IN") == 1) in_seen= true;
    }
    if (in_seen == true)
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BOOLEAN") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "MODE");
        return;
      }
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "NATURAL");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "LANGUAGE");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "MODE");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITH") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "QUERY");
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXPANSION");
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITH") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "QUERY");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXPANSION");
      if (hparse_errno > 0) return;
    }
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
    return;
  }

//  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "BETWEEN") == 1)
//   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CASE") == 1)
//   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WHEN") == 1)
//   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "THEN") == 1)
//   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ELSE") == 1)
//          )
//    {;}
  hparse_f_opr_7();
  if (hparse_errno > 0) return;
}

/* Most comp-ops can be chained e.g. "a <> b <> c", but not LIKE or IN. */
void MainWindow::hparse_f_opr_7() /* Precedence = 7 */
{
  if ((hparse_subquery_is_allowed == true) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EXISTS") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return;
    if (hparse_f_select(false) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
    return;
  }
  int expression_count= 0;
  if (hparse_f_is_equal(hparse_token, "(")) hparse_f_parenthesized_multi_expression(&expression_count);
  else hparse_f_opr_8();
  if (hparse_errno > 0) return;
  for (;;)
  {
    /* If we see "NOT", the only comp-ops that can follow are "LIKE" and "IN". */
    bool not_seen= false;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NOT") == 1)
    {
      not_seen= true;
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LIKE") == 1)
    {
      hparse_like_seen= true;
      if (hparse_f_is_equal(hparse_token, "(")) hparse_f_parenthesized_multi_expression(&expression_count);
      else hparse_f_opr_8();
      hparse_like_seen= false;
      break;
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IN") == 1)
    {
      hparse_f_parenthesized_multi_expression(&expression_count);
      if (hparse_errno > 0) return;
      break;
    }
    /* The manual says BETWEEN has a higher priority than this */
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BETWEEN") == 1)
    {
      hparse_f_opr_8();
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "AND");
      if (hparse_errno > 0) return;
      hparse_f_opr_8();
      if (hparse_errno > 0) return;
      return;
    }
    if (not_seen == true)
    {
      hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if ((hparse_f_accept(TOKEN_TYPE_OPERATOR, "->") == 1) /* MySQL 5.7.9 JSON-colum->path operator */
     || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "<=>") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REGEXP") == 1))
    {
      hparse_f_opr_8();
      if (hparse_errno > 0) return;
      continue;
    }
    if ((hparse_f_accept(TOKEN_TYPE_OPERATOR, "=") == 1)
     || (hparse_f_accept(TOKEN_TYPE_OPERATOR, ">=") == 1)
     || (hparse_f_accept(TOKEN_TYPE_OPERATOR, ">") == 1)
     || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "<=") == 1)
     || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "<") == 1)
     || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "<>") == 1)
     || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "!=") == 1))
    {
      if (hparse_subquery_is_allowed == true)
      {
        if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "SOME") == 1)
         || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ANY") == 1)
         || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALL") == 1))
        {
          /* todo: what if some mad person has created a function named any or some? */
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
          if (hparse_errno > 0) return;
          if (hparse_f_select(false) == 0) hparse_f_error();
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
          if (hparse_errno > 0) return;
          continue;
        }
      }
      if (hparse_f_is_equal(hparse_token, "(")) hparse_f_parenthesized_multi_expression(&expression_count);
      else hparse_f_opr_8();
      if (hparse_errno > 0) return;
      continue;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IS") == 1)
    {
      hparse_f_accept(TOKEN_TYPE_KEYWORD, "NOT");
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "NULL") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TRUE") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FALSE") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNKNOWN") == 1))
        {;}
      else hparse_f_error();
      if (hparse_errno > 0) return;
      continue;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SOUNDS") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "LIKE");
      if (hparse_errno > 0) return;
      hparse_f_opr_8();
      if (hparse_errno > 0) return;
      continue;
    }
    break;
  }
}

void MainWindow::hparse_f_opr_8() /* Precedence = 8 */
{
  hparse_f_opr_9();
  if (hparse_errno > 0) return;
  while (hparse_f_accept(TOKEN_TYPE_OPERATOR, "|") == 1)
  {
    hparse_f_opr_9();
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_9() /* Precedence = 9 */
{
  hparse_f_opr_10();
  if (hparse_errno > 0) return;
  while (hparse_f_accept(TOKEN_TYPE_OPERATOR, "&") == 1)
  {
    hparse_f_opr_10();
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_10() /* Precedence = 10 */
{
  hparse_f_opr_11();
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(TOKEN_TYPE_OPERATOR, "<<") == 1) || (hparse_f_accept(TOKEN_TYPE_OPERATOR, ">>") == 1))
  {
    hparse_f_opr_11();
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_11() /* Precedence = 11 */
{
  hparse_f_opr_12();
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(TOKEN_TYPE_OPERATOR, "-") == 1) || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "+") == 1))
  {
    hparse_f_opr_12();
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_12() /* Precedence = 12 */
{
  hparse_f_opr_13();
  if (hparse_errno > 0) return;
  while ((hparse_f_accept(TOKEN_TYPE_OPERATOR, "*") == 1)
   || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "/") == 1)
   || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "DIV") == 1)
   || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "%") == 1)
   || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "MOD") == 1))
  {
    hparse_f_opr_13();
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_13() /* Precedence = 13 */
{
  hparse_f_opr_14();
  if (hparse_errno > 0) return;
  while (hparse_f_accept(TOKEN_TYPE_OPERATOR, "^") == 1)
  {
    hparse_f_opr_14();
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_opr_14() /* Precedence = 14 */
{
  if ((hparse_f_accept(TOKEN_TYPE_OPERATOR, "-") == 1) || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "~") == 1)) {;}
  hparse_f_opr_15();
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_opr_15() /* Precedence = 15 */
{
  if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "!") == 1) {;}
  hparse_f_opr_16();
  if (hparse_errno > 0) return;
}

/* Actually I'm not sure what ESCAPE precedence is, as long as it's higher than LIKE. */
void MainWindow::hparse_f_opr_16() /* Precedence = 16 */
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BINARY") == 1) {;}
  hparse_f_opr_17();
  if (hparse_errno > 0) return;
  if (hparse_like_seen == true)
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ESCAPE") == 1)
    {
      hparse_like_seen= false;
      hparse_f_opr_17();
      if (hparse_errno > 0) return;
      return;
    }
  }
  while (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
  {
    hparse_f_opr_17();
    if (hparse_errno > 0) return;
  }
}

/* todo: disallow INTERVAL unless we've seen + or - */
void MainWindow::hparse_f_opr_17() /* Precedence = 17 */
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INTERVAL") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
    if (hparse_errno > 0) return;
    hparse_f_interval_quantity(TOKEN_KEYWORD_INTERVAL);
    if (hparse_errno > 0) return;
    return;
  }
  hparse_f_opr_18();
  if (hparse_errno > 0) return;
}

/*
  Final level is operand.
  factor = identifier | number | "(" expression ")" .
*/
void MainWindow::hparse_f_opr_18() /* Precedence = 18, top */
{
  if (hparse_errno > 0) return;
  QString opd= hparse_token.toUpper();
  bool identifier_seen= false;
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATE") == 1) /* DATE 'x', else DATE is not reserved so might be an id */
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TIME") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TIMESTAMP") == 1))
  {
    if (hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]") == 1) return;
    identifier_seen= true;
  }
  if ((identifier_seen == true)
   || (hparse_f_qualified_name() == 1))
  {
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1) /* identifier followed by "(" must be a function name */
    {
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ")") == 0)
      {
        hparse_f_function_arguments(opd);
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
    }
    return;
  }

  /* Watching for built-in functions that happen to be reserved words */
  if ((hparse_f_accept(TOKEN_KEYWORD_CHAR, "CHAR") == 1)
   || (hparse_f_accept(TOKEN_KEYWORD_CONVERT, "CONVERT") == 1)
   || (hparse_f_accept(TOKEN_KEYWORD_IF_IN_IF_EXPRESSION, "IF") == 1)
   || (hparse_f_accept(TOKEN_KEYWORD_INSERT, "INSERT") == 1)
   || (hparse_f_accept(TOKEN_KEYWORD_LEFT, "LEFT") == 1)
   || (hparse_f_accept(TOKEN_KEYWORD_LOCALTIME, "LOCALTIME") == 1)
   || (hparse_f_accept(TOKEN_KEYWORD_LOCALTIMESTAMP, "LOCALTIMESTAMP") == 1)
   || (hparse_f_accept(TOKEN_KEYWORD_MOD, "MOD") == 1)
   || (hparse_f_accept(TOKEN_KEYWORD_REPEAT_IN_REPEAT_EXPRESSION, "REPEAT") == 1)
   || (hparse_f_accept(TOKEN_KEYWORD_REPLACE, "REPLACE") == 1)
   || (hparse_f_accept(TOKEN_KEYWORD_RIGHT, "RIGHT") == 1))
  {
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ")") == 0)
    {
      hparse_f_function_arguments(opd);
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    return;
  }

  /* TODO: This should only work for INSERT ... ON DUPLICATE KEY UPDATE */
  if ((hparse_statement_type == TOKEN_KEYWORD_INSERT)
   && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VALUES") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
    return;
  }

  if (hparse_f_literal() == 1)
  {
    if (hparse_errno > 0) return;
    return;
  }
  else if (hparse_errno > 0) return;
  if (hparse_f_default(TOKEN_KEYWORD_SELECT) == 1)
  {
    return;
  }
  else if (hparse_errno > 0) return;
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATABASE") == 1)
        || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SCHEMA") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
    return;
  }
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "CURRENT_DATE") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CURRENT_TIME") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CURRENT_USER") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CURRENT_TIMESTAMP") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UTC_DATE") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UTC_TIME") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UTC_TIMESTAMP") == 1))
  {
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
    {
      hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]");
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    return;
  }
  if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "("))
  {
    if (hparse_errno > 0) return;
    /* if subquery is allowed, check for "(SELECT ...") */
    if ((hparse_subquery_is_allowed == true)
     && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SELECT") == 1))
    {
      hparse_f_select(true);
      if (hparse_errno > 0) return;
    }
    else hparse_f_opr_1();
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
    return;
  }
  hparse_f_error();
  return;
}

/*
  TODO: Recognize all 400+ built-in functions.
  Until then, we'll assume any function has a generalized comma-delimited expression list.
  But we still have to handle the ones that don't have simple lists.
*/
void MainWindow::hparse_f_function_arguments(QString opd)
{
  if ((hparse_f_is_equal(opd,"AVG"))
   || (hparse_f_is_equal(opd, "MIN"))
   || (hparse_f_is_equal(opd, "MAX")))
  {
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "DISTINCT");
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_is_equal(opd, "CAST"))
  {
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "AS");
    if (hparse_errno > 0) return;
    hparse_f_data_type();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_is_equal(opd, "CHAR"))
  {
    do
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USING") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
        break;
      }
      hparse_f_opr_1();
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  }
  else if (hparse_f_is_equal(opd, "CONVERT"))
  {
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "USING");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_is_equal(opd, "IF"))
  {
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ",");
    if (hparse_errno > 0) return;
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ",");
    if (hparse_errno > 0) return;
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_is_equal(opd, "COUNT"))
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DISTINCT") == 1) hparse_f_opr_1();
    else
    {
      if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "*") == 1) {;}
      else hparse_f_opr_1();
    }
    if (hparse_errno > 0) return;
  }
  else if ((hparse_f_is_equal(opd, "SUBSTR")) || (hparse_f_is_equal(opd, "SUBSTRING")))
  {
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
    if ((hparse_f_accept(TOKEN_TYPE_OPERATOR, ",") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FROM") == 1))
    {
      hparse_f_opr_1();
      if (hparse_errno > 0) return;
      if ((hparse_f_accept(TOKEN_TYPE_OPERATOR, ",") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOR") == 1))
      {
        hparse_f_opr_1();
        if (hparse_errno > 0) return;
      }
    }
  }
  else if (hparse_f_is_equal(opd, "TRIM"))
  {
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "BOTH") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LEADING") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TRAILING") == 1)) {;}
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FROM") == 1)
    {
      hparse_f_opr_1();
      if (hparse_errno > 0) return;
    }
  }
  else if (hparse_f_is_equal(opd, "WEIGHT_STRING"))
  {
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AS") == 1)
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHAR") == 0)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "BINARY");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        /* TODO: shouldn't you expect ")" here? */
      }
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LEVEL") == 1)
    {
      do
      {
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "ASC") == 1)
         || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DESC") == 1)
         || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REVERSE") == 1)) {;}
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
  }
  else do
  {
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
}

void MainWindow::hparse_f_expression_list(int who_is_calling)
{
  do
  {
    if (who_is_calling == TOKEN_KEYWORD_SELECT) hparse_f_next_nexttoken();
    if (hparse_errno > 0) return;
    if (hparse_f_default(who_is_calling) == 1) {;}
    else if ((who_is_calling == TOKEN_KEYWORD_SELECT) && (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "*") == 1)) {;}
    else if ((who_is_calling == TOKEN_KEYWORD_SELECT)
          && (hparse_f_is_equal(hparse_next_token, "."))
          && (hparse_f_is_equal(hparse_next_next_token, "*"))
          && (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]")))
    {
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ".");
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "*");
    }
    else
    {
      hparse_f_opr_1();
    }
    if (hparse_errno > 0) return;
    if (who_is_calling == TOKEN_KEYWORD_SELECT)
    {
      bool as_seen= false;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AS") == 1) as_seen= true;
      if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]") == 1) {;}
      else if (as_seen == true) hparse_f_error();
      if (hparse_errno > 0) return;
    }
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
}

/* e.g. (1,2,3) or ( (1,1), (2,2), (3,3) ) i.e. two parenthesization levels are okay */
void MainWindow::hparse_f_parenthesized_value_list()
{
  hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  do
  {
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
    {
      do
      {
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_literal() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_parameter_list(int routine_type)
{
  hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  do
  {
    bool in_seen= false;
    if (routine_type == TOKEN_KEYWORD_PROCEDURE)
    {
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "IN") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "OUT") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INOUT") == 1))
      {
        in_seen= true;
      }
    }
    if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
    {
      hparse_f_data_type();
      if (hparse_errno > 0) return;
    }
    else if (in_seen == true)
    {
      hparse_f_error();
      return;
    }
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_parenthesized_expression()
{
  hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  hparse_f_opr_1();
  if (hparse_errno > 0) return;
  hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
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
  hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  if ((hparse_subquery_is_allowed == true) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SELECT") == 1))
  {
    hparse_f_select(true);
    if (hparse_errno > 0) return;
    (*expression_count)= 2;          /* we didn't really count, but guess it's more than 1 */
  }
  else
  {
    do
    {
      hparse_f_opr_1();
      if (hparse_errno > 0) return;
      ++(*expression_count);
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  }
  hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
}


/* TODO: if statement_type <> TOKEN_KEYWORD_SET, disallow assignment to @@ or @ variables. */
void MainWindow::hparse_f_assignment(int statement_type)
{
  do
  {
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "@@SESSION") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GLOBAL") == 1))
    {
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ".");
      if (hparse_errno > 0) return;
    }
    if (hparse_errno > 0) return;
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "=") == 0) hparse_f_expect(TOKEN_TYPE_OPERATOR, ":=");
    if (hparse_errno > 0) return;
    /* TODO: DEFAULT and ON and OFF shouldn't always be legal. */
    if (hparse_f_default(statement_type) == 1) continue;
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ON") == 1) continue;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "OFF") == 1) continue;
    /* TODO: VALUES should only be legal for INSERT ... ON DUPLICATE KEY */
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
}

void MainWindow::hparse_f_alter_specification()
{
  hparse_f_table_or_partition_options(TOKEN_KEYWORD_TABLE);
  if (hparse_errno > 0) return;
  bool default_seen= false;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 1) default_seen= true;
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ADD") == 1))
  {
    bool column_name_is_expected= false;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLUMN") == 1) column_name_is_expected= true;
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PARTITION") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return;
      /* todo: check that hparse_f_partition_or_subpartition_definition does as expected */
      hparse_f_partition_or_subpartition_definition(TOKEN_KEYWORD_PARTITION);
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_create_definition() == 3) column_name_is_expected= true;
    if (hparse_errno > 0) return;
    if (column_name_is_expected == true)
    {
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        do
        {
          if (hparse_f_qualified_name() == 0) hparse_f_error();
          if (hparse_errno > 0) return;
          hparse_f_column_definition();
          if (hparse_errno > 0) return;
        } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
        hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
      else
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_column_definition();
        if (hparse_errno > 0) return;
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FIRST") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AFTER") == 1)
        {
          if (hparse_f_qualified_name() == 0) hparse_f_error();
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
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALTER") == 1))
  {
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLUMN");
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SET") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "DEFAULT");
      if (hparse_errno > 0) return;
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DROP") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "DEFAULT");
      if (hparse_errno > 0) return;
    }
    else hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ANALYZE") == 1))
  {
    if (hparse_f_partition_list(false, true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHANGE") == 1))
  {
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLUMN");
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    hparse_f_column_definition();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FIRST") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AFTER") == 1)
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    return;
  }
  /* Todo: Following is useless code. CHARACTER SET is a table_option. Error in manual? */
  if ((hparse_f_character_set() == 1))
  {
    hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
    if (hparse_f_character_set_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_collation_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    return;
  }
  if (hparse_errno > 0) return;
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHECK") == 1))
  {
    if (hparse_f_partition_list(false, true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  /* "LOCK" is already handled by hparse_f_algorithm_or_lock() */
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COALESCE") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "PARTITION");
    if (hparse_errno > 0) return;
    if (hparse_f_literal() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONVERT") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "TO");
    if (hparse_errno > 0) return;
    hparse_f_character_set();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 0)
    {
      if (hparse_f_character_set_name() == 0) hparse_f_error();
    }
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
    {
      if (hparse_f_collation_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DISABLE") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "KEYS");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DISCARD") == 1))
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLESPACE") == 1) return;
    if (hparse_f_partition_list(false, true) == 0)
    {
      hparse_f_error();
      if (hparse_errno > 0) return;
    }
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "TABLESPACE");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DROP") == 1))
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PRIMARY") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "KEY");
      if (hparse_errno > 0) return;
    }
    else if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEY") == 1))
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOREIGN") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "KEY");
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PARTITION") == 1)
    {
      /* todo: maybe use if (hparse_f_partition_list(true, false) == 0) hparse_f_error(); */
      do
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
    else
    {
      hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLUMN");
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENABLE") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "KEYS");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EXCHANGE") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "PARTITION");
    if (hparse_errno > 0) return;
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "WITH");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "TABLE");
    if (hparse_errno > 0) return;
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITH") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITHOUT") == 1))
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "VALIDATION");
      if (hparse_errno > 0) return;
    }
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FORCE") == 1))
  {
    return;
  }
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "IMPORT") == 1))
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLESPACE") == 1) return;
    if (hparse_f_partition_list(false, true) == 0)
    {
      hparse_f_error();
      if (hparse_errno > 0) return;
    }
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "TABLESPACE");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MODIFY") == 1))
  {
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLUMN");
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    hparse_f_column_definition();
    if (hparse_errno > 0) return;
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "FIRST") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AFTER") == 1))
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    return;
  }
  /* "LOCK" is already handled by hparse_f_algorithm_or_lock() */
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "OPTIMIZE") == 1))
  {
    if (hparse_f_partition_list(false, true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ORDER") == 1)) /* todo: could use modified hparse_f_order_by */
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "BY");
    if (hparse_errno > 0) return;
    do
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "ASC") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DESC") == 1)) {;}
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REBUILD") == 1))
  {
    if (hparse_f_partition_list(false, true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REMOVE") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "PARTITIONING");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RENAME") == 1))
  {
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "TO") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AS") == 1))
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEY") == 1))
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "TO");
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REORGANIZE") == 1))
  {
    if (hparse_f_partition_list(false, false) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "INTO");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return;
    do
    {
      hparse_f_partition_or_subpartition_definition(TOKEN_KEYWORD_PARTITION);
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPAIR") == 1))
  {
    if (hparse_f_partition_list(false, true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TRUNCATE") == 1))
  {
    if (hparse_f_partition_list(false, true) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UPGRADE") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "PARTITIONING");
    if (hparse_errno > 0) return;
    return;
  }  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITH") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "VALIDATION");
    if (hparse_errno > 0) return;
    return;
  }
  if ((default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITHOUT") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "VALIDATION");
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
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHARACTER") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "SET");
    if (hparse_errno > 0) return 0;
    return 1;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHARSET") == 1) return 1;
  else return 0;
}

void MainWindow::hparse_f_alter_database()
{
  hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
  if (hparse_errno > 0) return;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UPGRADE") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "DATA");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "DIRECTORY");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "NAME");
    if (hparse_errno > 0) return;
  }
  else
  {
    bool character_seen= false, collate_seen= false;
    for (;;)
    {
      if ((character_seen == true) && (collate_seen == true)) break;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT")) {;}
      if ((character_seen == false) && (hparse_f_character_set() == 1))
      {
        character_seen= true;
        if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "=")) {;}
        if (hparse_f_character_set_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        continue;
      }
      if (hparse_errno > 0) return;
      if ((collate_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATE")))
      {
        collate_seen= true;
        if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "=")) {;}
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

void MainWindow::hparse_f_characteristics()
{
  bool comment_seen= false, language_seen= false, contains_seen= false, sql_seen= false;
  bool deterministic_seen= false;
  for (;;)
  {
    if ((comment_seen) && (language_seen) && (contains_seen) && (sql_seen)) break;
    if ((comment_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COMMENT")))
    {
      comment_seen= true;
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      continue;
    }
    else if ((language_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LANGUAGE")))
    {
      language_seen= true;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "SQL");
      if (hparse_errno > 0) return;
      continue;
    }
    else if ((deterministic_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NOT")))
    {
      deterministic_seen= true;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "DETERMINISTIC");
      if (hparse_errno > 0) return;
      continue;
    }
    else if ((deterministic_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DETERMINISTIC")))
    {
      deterministic_seen= true;
      continue;
    }
    else if ((contains_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONTAINS")))
    {
      contains_seen= true;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "SQL");
      if (hparse_errno > 0) return;
      continue;
    }
    else if ((contains_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NO")))
    {
      contains_seen= true;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "SQL");
      if (hparse_errno > 0) return;
      continue;
    }
    else if ((contains_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "READS")))
    {
      contains_seen= true;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "SQL");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "DATA");
      if (hparse_errno > 0) return;
      continue;
     }
    else if ((contains_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MODIFIES")))
    {
      contains_seen= true;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "SQL");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "DATA");
      if (hparse_errno > 0) return;
      continue;
    }
    else if ((sql_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL")))
    {
      sql_seen= true;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "SECURITY");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFINER") == 1) continue;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "INVOKER");
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
    if ((algorithm_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALGORITHM") == 1))
    {
      algorithm_seen= true;
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "=") == 1) {;}
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 1) break;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INPLACE") == 1) break;
      if (hparse_f_expect(TOKEN_TYPE_KEYWORD, "COPY") == 1) break;
      if (hparse_errno > 0) return 0;
    }
    if ((lock_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOCK") == 1))
    {
      lock_seen= true;
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "=") == 1) {;}
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 1) break;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NONE") == 1) break;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SHARED") == 1) break;
      if (hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXCLUSIVE") == 1) break;
      if (hparse_errno > 0) return 0;
    }
    break;
  }
  if ((algorithm_seen == true) || (lock_seen == true)) return 1;
  return 0;
}

void MainWindow::hparse_f_definer()
{
  hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
  if (hparse_errno > 0) return;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CURRENT_USER") == 1) {;}
  else if (hparse_f_user_name() == 1) {;}
  else hparse_f_error();
}

void MainWindow::hparse_f_if_not_exists()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "NOT");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
    if (hparse_errno > 0) return;
  }
}

int MainWindow::hparse_f_analyze_or_optimize(int who_is_calling)
{
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "NO_WRITE_TO_BINLOG") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOCAL") == 1)) {;}
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLE") == 1) {;}
  else if ((who_is_calling == TOKEN_KEYWORD_REPAIR)
        && (hparse_dbms == "mariadb")
        && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VIEW") == 1))
    {;}
  else return 0;
  do
  {
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  return 1;
}

void MainWindow::hparse_f_character_set_or_collate()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ASCII") == 1) {;}
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNICODE") == 1) {;}
  else if (hparse_f_character_set() == 1)
  {
    if (hparse_f_character_set_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_errno > 0) return;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
  {
    if (hparse_f_collation_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
}

/* Used for data type length. Might be useful for any case of "(" integer ")" */
void MainWindow::hparse_f_length(bool is_ok_if_decimal, bool is_ok_if_unsigned, bool is_ok_if_binary)
{
  if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
  {
    if (hparse_f_literal() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if (is_ok_if_decimal)
    {
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ",") == 1)
      {
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
    }
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
  }
  if (is_ok_if_unsigned)
  {
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNSIGNED") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SIGNED") == 1)) {;}
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "ZEROFILL");
  }
  if (is_ok_if_binary)
  {
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "BINARY");
    hparse_f_character_set_or_collate();
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_enum_or_set()
{
  hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  do
  {
    if (hparse_f_literal() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
  hparse_f_character_set_or_collate();
  if (hparse_errno > 0) return;
}

/*
  Todo: we are not distinguishing for the different data-type syntaxes,
  for example in CAST "UNSIGNED INT" is okay but "INT UNSIGNED" is illegal,
  while in CREATE "UNSIGNED INT" is illegal but "UNSIGNED INT" is okay.
  We allow any combination.
  Todo: also, in CAST, only DOUBLE is okay, not DOUBLE PRECISION.
*/
int MainWindow::hparse_f_data_type()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BIT") == 1)
  {
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_BIT;
  }
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "TINYINT") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BOOLEAN") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INT1") == 1))
  {
    hparse_f_length(false, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_TINYINT;
  }
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "SMALLINT") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INT2") == 1))
  {
    hparse_f_length(false, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_SMALLINT;
  }
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "MEDIUMINT") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INT3") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MIDDLEINT") == 1))
  {
    hparse_f_length(false, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_MEDIUMINT;
  }
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "INT") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INT4") == 1))
  {
    hparse_f_length(false, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_INT4;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INTEGER") == 1)
  {
    hparse_f_length(false, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_INTEGER;
  }
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "BIGINT") == 1)|| (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INT8") == 1))
  {
    hparse_f_length(false, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_BIGINT;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REAL") == 1)
  {
    hparse_f_length(true, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_REAL;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DOUBLE") == 1)
  {
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "PRECISION");
    hparse_f_length(true, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_DOUBLE;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FLOAT8") == 1)
  {
    hparse_f_length(true, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_FLOAT8;
  }
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "FLOAT") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FLOAT4") == 1))
  {
    hparse_f_length(true, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_FLOAT4;
  }
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "DECIMAL") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEC") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FIXED") == 1))
  {
    hparse_f_length(true, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_DECIMAL;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NUMERIC") == 1)
  {
    hparse_f_length(true, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_NUMERIC;
  }
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNSIGNED") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SIGNED") == 1))
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INT") == 0) hparse_f_accept(TOKEN_TYPE_KEYWORD, "INTEGER");
    hparse_f_length(false, true, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_UNSIGNED;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SERIAL") == 1) return TOKEN_KEYWORD_SERIAL;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATE") == 1) return TOKEN_KEYWORD_DATE;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TIME") == 1)
  {
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_TIME;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TIMESTAMP") == 1)
  {
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_TIMESTAMP;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATETIME") == 1)
  {
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_DATETIME;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "YEAR") == 1)
  {
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_YEAR;
  }
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHAR") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHARACTER") == 1))
  {
    bool byte_seen= false, varying_seen= false;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BYTE") == 1) byte_seen= true;
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VARYING") == 1) varying_seen= true;
    if (byte_seen == false) hparse_f_length(false, false, true);
    if (hparse_errno > 0) return 0;
    if (varying_seen == true) return TOKEN_KEYWORD_VARCHAR;
    return TOKEN_KEYWORD_CHAR;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VARCHAR") == 1)
  {
    hparse_f_length(false, false, true);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_VARCHAR;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VARCHARACTER") == 1)
  {
    hparse_f_length(false, false, true);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_VARCHARACTER;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NCHAR") == 1)
  {
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "VARYING");
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
    {
      if (hparse_f_collation_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    return TOKEN_KEYWORD_CHAR;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NVARCHAR") == 1)
  {
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
    {
      if (hparse_f_collation_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    return TOKEN_KEYWORD_CHAR;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NATIONAL") == 1)
  {
    bool varchar_seen= false;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHAR") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHARACTER") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VARCHAR") == 1) varchar_seen= true;
    else hparse_f_error();
    if (hparse_errno > 0) return 0;
    if (varchar_seen == false) hparse_f_accept(TOKEN_TYPE_KEYWORD, "VARYING");
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
    {
      if (hparse_f_collation_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    return TOKEN_KEYWORD_CHAR;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LONG") == 1)
  {
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "VARBINARY") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VARCHAR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MEDIUMTEXT") == 1))
    {
      hparse_f_length(false, false, false);
      if (hparse_errno > 0) return 0;
    }
    else hparse_f_error();
    return TOKEN_KEYWORD_LONG;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BINARY") == 1)
  {
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_BINARY;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VARBINARY") == 1)
  {
    hparse_f_length(false, false, false);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_VARBINARY;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TINYBLOB") == 1) return TOKEN_KEYWORD_TINYBLOB;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BLOB") == 1) return TOKEN_KEYWORD_BLOB;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MEDIUMBLOB") == 1) return TOKEN_KEYWORD_MEDIUMBLOB;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LONGBLOB") == 1) return TOKEN_KEYWORD_LONGBLOB;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TINYTEXT") == 1)
  {
    hparse_f_length(false, false, true);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_TINYTEXT;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TEXT") == 1)
  {
    hparse_f_length(false, false, true);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_MEDIUMTEXT;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MEDIUMTEXT") == 1)
  {
    hparse_f_length(false, false, true);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_MEDIUMTEXT;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LONGTEXT") == 1)
  {
    hparse_f_length(false, false, true);
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_LONGTEXT;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENUM") == 1)
  {
    hparse_f_enum_or_set();
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_ENUM;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SET") == 1)
  {
    hparse_f_enum_or_set();
    if (hparse_errno > 0) return 0;
    return TOKEN_KEYWORD_SET;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "JSON") == 1) return 0; /* todo: token_keyword */
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GEOMETRY") == 1) return TOKEN_KEYWORD_GEOMETRY;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "POINT") == 1) return TOKEN_KEYWORD_POINT;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LINESTRING") == 1) return TOKEN_KEYWORD_LINESTRING;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "POLYGON") == 1) return TOKEN_KEYWORD_POLYGON;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MULTIPOINT") == 1) return TOKEN_KEYWORD_MULTIPOINT;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MULTIPOLYGON") == 1) return TOKEN_KEYWORD_MULTIPOLYGON;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GEOMETRYCOLLECTION") == 1) return TOKEN_KEYWORD_GEOMETRYCOLLECTION;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LINESTRING") == 1) return TOKEN_KEYWORD_LINESTRING;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "POLYGON") == 1) return TOKEN_KEYWORD_POLYGON;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BOOL") == 1) return TOKEN_KEYWORD_BOOL;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BOOLEAN") == 1) return TOKEN_KEYWORD_BOOLEAN;
  hparse_f_error();
  return 0;
}

void MainWindow::hparse_f_reference_option()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RESTRICT") == 1) {;}
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CASCADE") == 1) {;}
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SET") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "NULL");
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NO") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "ACTION");
    if (hparse_errno > 0) return;
  }
  else hparse_f_error();
  if (hparse_errno > 0) return;
}


void MainWindow::hparse_f_reference_definition()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REFERENCES") == 1)
  {
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    hparse_f_column_list(0);
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MATCH") == 1)
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FULL") == 1) {;}
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PARTIAL") == 1) {;}
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SIMPLE") == 1) {;}
      else hparse_f_error();
    }
    bool on_delete_seen= false, on_update_seen= false;
    while (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ON") == 1)
    {
      if ((on_delete_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DELETE") == 1))
      {
        hparse_f_reference_option();
        if (hparse_errno > 0) return;
        on_delete_seen= true;
      }
      else if ((on_update_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UPDATE") == 1))
      {
        hparse_f_reference_option();
        if (hparse_errno > 0) return;
        on_update_seen= true;
      }
      else hparse_f_error();
    }
  }
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
int MainWindow::hparse_f_create_definition()
{
  bool constraint_seen= false;
  bool fulltext_seen= false, foreign_seen= false;
  bool unique_seen= false, check_seen= false;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONSTRAINT") == 1)
  {
    hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    constraint_seen= true;
  }
  if ((constraint_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX") == 1)) {;}
  else if ((constraint_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEY") == 1)) {;}
  else if ((constraint_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FULLTEXT") == 1)) fulltext_seen= true;
  else if ((constraint_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SPATIAL") == 1)) fulltext_seen= true;
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PRIMARY") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "KEY");
    if (hparse_errno > 0) return 2;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNIQUE") == 1) unique_seen= true;
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOREIGN") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "KEY");
    if (hparse_errno > 0) return 2;
    foreign_seen= true;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHECK") == 1) check_seen= true;
  else return 3;
  if (check_seen == true)
  {
    hparse_f_parenthesized_expression();
    if (hparse_errno > 0) return 2;
    return 1;
  }
  if ((fulltext_seen == true) || (unique_seen == true))
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX") == 1) {;}
    else hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEY");
  }
  hparse_f_index_columns(TOKEN_KEYWORD_TABLE, fulltext_seen, foreign_seen);
  if (hparse_errno > 0) return 2;
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
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CURRENT_TIMESTAMP") == 1) keyword= TOKEN_KEYWORD_CURRENT_TIMESTAMP;
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOCALTIME") == 1) keyword= TOKEN_KEYWORD_LOCALTIME;
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOCALTIMESTAMP") == 1) keyword= TOKEN_KEYWORD_LOCALTIMESTAMP;
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NOW") == 1) keyword= TOKEN_KEYWORD_NOW;
  else return 0;
  if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
  {
    if (hparse_f_accept(TOKEN_TYPE_LITERAL, "0") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_LITERAL, "1") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_LITERAL, "2") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_LITERAL, "3") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_LITERAL, "4") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_LITERAL, "5") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_LITERAL, "6") == 1) {;}
    hparse_f_expect(TOKEN_TYPE_KEYWORD, ")");
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
void MainWindow::hparse_f_column_definition()
{
  int data_type= hparse_f_data_type();
  if (hparse_errno > 0) return;
  bool generated_seen= false;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GENERATED") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "ALWAYS");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "AS");
    if (hparse_errno > 0) return;
    generated_seen= true;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AS") == 1)
  {
    generated_seen= true;
  }
  if (generated_seen == true)
  {
    hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
    if (hparse_errno > 0) return;
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
    if (hparse_dbms == "mariadb")
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VIRTUAL") == 0)
        hparse_f_accept(TOKEN_TYPE_KEYWORD, "PERSISTENT");
    }
    if (hparse_dbms == "mysql")
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VIRTUAL") == 0)
        hparse_f_accept(TOKEN_TYPE_KEYWORD, "STORED");
    }
  }
  bool null_seen= false, default_seen= false, auto_increment_seen= false;
  bool unique_seen= false, primary_seen= false, comment_seen= false, column_format_seen= false;
  bool on_seen= false;
  for (;;)
  {
    if ((null_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NOT") == 1))
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "NULL");
      if (hparse_errno > 0) return;
      null_seen= true;
    }
    else if ((null_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NULL") == 1))
    {
      null_seen= true;
    }
    else if ((generated_seen == false) && (default_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 1))
    {
      if (((data_type == TOKEN_KEYWORD_DATETIME) || (data_type == TOKEN_KEYWORD_TIMESTAMP))
          && (hparse_f_current_timestamp() == 1)) {;}
      else if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      default_seen= true;
    }
    else if ((generated_seen == false) && (auto_increment_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AUTO_INCREMENT") == 1))
    {
      auto_increment_seen= true;
    }
    else if ((unique_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNIQUE") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEY");
      unique_seen= true;
    }
    else if ((primary_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PRIMARY") == 1))
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "KEY");
      if (hparse_errno > 0) return;
      primary_seen= true;
    }
    else if ((primary_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEY") == 1))
    {
      primary_seen= true;
    }
    else if ((comment_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COMMENT") == 1))
    {
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      comment_seen= true;
    }
    else if ((generated_seen == false) && (column_format_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLUMN_FORMAT") == 1))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FIXED") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DYNAMIC") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 1) {;}
      else hparse_f_error();
      if (hparse_errno > 0) return;
      column_format_seen= true;
    }
    else if ((on_seen == false) && (generated_seen == false)
             && ((data_type == TOKEN_KEYWORD_TIMESTAMP) || (data_type == TOKEN_KEYWORD_DATETIME))
             && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ON") == 1))
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "UPDATE");
      if (hparse_errno > 0) return;
      if (hparse_f_current_timestamp() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      on_seen= true;
    }
    else break;
  }
  if (generated_seen == false)
  {
    hparse_f_reference_definition();
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_comment()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COMMENT") == 1)
  {
    if (hparse_f_literal() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
}

/* Actually this is just "identifier list", and we should be checking for qualifiers. */
void MainWindow::hparse_f_column_list(int is_compulsory)
{
  if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 0)
  {
    if (is_compulsory == 1) hparse_f_error();
    return;
  }
  do
  {
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
}

/*
  engine = engine_name part of either CREATE TABLE or CREATE TABLESPACE.
  Usually it will be a standard engine like MyISAM or InnoDB, but with MariaDB
  there are usually more choices ... in the end, we allow any identifier.
*/
void MainWindow::hparse_f_engine()
{
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "=") == 1) {;}
    if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "ARCHIVE") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "CSV") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "EXAMPLE") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "FEDERATED") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "HEAP") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "INNODB") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "MEMORY") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "MERGE") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "MYISAM") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "NDB") == 1) {;}
    else hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
}

void MainWindow::hparse_f_table_or_partition_options(int keyword)
{
  bool comma_seen= false;
  for (;;)
  {
    if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AUTO_INCREMENT") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AVG_ROW_LENGTH") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_character_set() == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_character_set_name();
      if (hparse_errno > 0) return;
    }
    else if (hparse_errno > 0) return;
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHECKSUM") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "0") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "1");
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATE") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_collation_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "COMMENT") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COMPRESSION")) == 1)
    {
      /* todo: should be: zlib, lz4, or none */
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONNECTION")) == 1)
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATA") == 1))
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "DIRECTORY");
      if (hparse_errno > 0) return;
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 1))
    {
      if (hparse_f_character_set() == 1)
      {
        hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
        hparse_f_character_set_name();
        if (hparse_errno > 0) return;
      }
      else if (hparse_errno > 0) return;
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATE") == 1)
      {
        hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
        if (hparse_f_collation_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DELAY_KEY_WRITE") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "0") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "1");
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms == "mariadb") && (keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENCRYPTED") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "YES") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "NO");
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms == "mysql") && (keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENCRYPTION") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms == "mariadb") && (keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENCRYPTION_KEY_ID") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENGINE") == 1))
    {
      hparse_f_engine();
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms == "mariadb") && (keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IETF_QUOTES") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "YES") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "NO");
      if (hparse_errno > 0) return;
    }
    else if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX") == 1))
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "DIRECTORY");
      if (hparse_errno > 0) return;
      {
        hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
        hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
        if (hparse_errno > 0) return;
      }
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INSERT_METHOD") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "NO") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FIRST") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LAST") == 1)) {;}
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEY_BLOCK_SIZE") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MAX_ROWS") == 1)
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MIN_ROWS") == 1)
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PACK_KEYS") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "0") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "1") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 1)) {;}
      else hparse_f_error();
    }
    else if ((hparse_dbms == "mariadb") && (keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PAGE_CHECKSUM") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "0") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "1");
      else hparse_f_error();
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PASSWORD") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ROW_FORMAT") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DYNAMIC") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FIXED") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COMPRESSED") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REDUNDANT") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COMPACT") == 1)
       || ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PAGE") == 1)))
        {;}
      else hparse_f_error();
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STATS_AUTO_RECALC") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "0") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "1") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 1)) {;}
      else hparse_f_error();
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STATS_PERSISTENT") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "0") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "1") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 1)) {;}
      else hparse_f_error();
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STATS_SAMPLE_PAGES") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_PARTITION) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STORAGE") == 1))
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "ENGINE");
      if (hparse_errno > 0) return;
      hparse_f_engine();
      if (hparse_errno > 0) return;
    }
    else if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLESPACE") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TRANSACTIONAL") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "0") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "1");
      if (hparse_errno > 0) return;
    }
    else if ((keyword == TOKEN_KEYWORD_TABLE) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNION") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return;
      do
      {
        hparse_f_qualified_name();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
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
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ",") == 1) comma_seen= true;
      else comma_seen= false;
    }
  }
}

void MainWindow::hparse_f_partition_options()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PARTITION") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "BY");
    if (hparse_errno > 0) return;
    hparse_f_partition_or_subpartition(TOKEN_KEYWORD_PARTITION);
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PARTITIONS") == 1)
    {
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SUBPARTITION") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return;
      hparse_f_partition_or_subpartition(0);
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SUBPARTITIONS") == 1)
    {
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
    {
      do
      {
        hparse_f_partition_or_subpartition_definition(TOKEN_KEYWORD_PARTITION);
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
  }
}

void MainWindow::hparse_f_partition_or_subpartition(int keyword)
{
  bool linear_seen= false;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LINEAR") == 1) linear_seen= true;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "HASH") == 1)
  {
    hparse_f_parenthesized_expression();
     if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEY") == 1)
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALGORITHM") == 1)
    {
      if (hparse_f_accept(TOKEN_TYPE_LITERAL_WITH_DIGIT, "1") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_LITERAL_WITH_DIGIT, "2") == 1) {;}
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
    hparse_f_column_list(1);
    if (hparse_errno > 0) return;
  }
  else if (((linear_seen == false) && (keyword == TOKEN_KEYWORD_PARTITION))
        && ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "RANGE") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LIST") == 1)))
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLUMNS") == 1)
    {
       hparse_f_column_list(1);
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
  if (keyword == TOKEN_KEYWORD_PARTITION) hparse_f_expect(TOKEN_TYPE_KEYWORD, "PARTITION");
  else hparse_f_expect(TOKEN_TYPE_KEYWORD, "SUBPARTITION");
  if (hparse_errno > 0) return;
  if (hparse_f_qualified_name() == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "VALUES") == 1))
  {
    /* Todo: LESS THAN only for RANGE; IN only for LIST. Right? */
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LESS") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "THAN");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MAXVALUE") == 1) {;}
      else
      {
        /* todo: supposedly this can be either expression or value-list. we take expression-list. */
        hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
        if (hparse_errno > 0) return;
        do
        {
          if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MAXVALUE") == 1) {;}
          else hparse_f_opr_1();
          if (hparse_errno > 0) return;
        } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      }
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IN") == 1)
    {
      hparse_f_parenthesized_value_list();
      if (hparse_errno > 0) return;
    }
    hparse_f_table_or_partition_options(TOKEN_KEYWORD_PARTITION);
    if (hparse_errno > 0) return;
    if ((keyword == TOKEN_KEYWORD_PARTITION) && (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1))
    {
      hparse_f_partition_or_subpartition_definition(0);
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
  }
}

int MainWindow::hparse_f_partition_list(bool is_parenthesized, bool is_maybe_all)
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PARTITION") == 1)
  {
    if (is_parenthesized)
    {
      hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return 0;
    }
    if ((is_maybe_all) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALL") == 1)) {;}
    else
    {
      do
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
    if (is_parenthesized)
    {
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return 0;
    }
    return 1;
  }
  return 0;
}

/* "ALGORITHM" seen, which must mean we're in ALTER VIEW or CREATE VIEW */
void MainWindow::hparse_f_algorithm()
{
  hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
  if (hparse_errno > 0) return;
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNDEFINED") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MERGE") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TEMPTABLE") == 1))
     {;}
  else hparse_f_error();
}

/* "SQL" seen, which must mean we're in ALTER VIEW or CREATE VIEW */
void MainWindow::hparse_f_sql()
{
  hparse_f_expect(TOKEN_TYPE_KEYWORD, "SECURITY");
  if (hparse_errno > 0) return;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFINER") == 1) {;}
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INVOKER") == 1) {;}
  else hparse_f_error();
}

void MainWindow::hparse_f_for_channel()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOR"))
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "CHANNEL");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_interval_quantity(int interval_or_event)
{
  if (((interval_or_event == TOKEN_KEYWORD_INTERVAL) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MICROSECOND") == 1))
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SECOND") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MINUTE") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "HOUR") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DAY") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WEEK") == 1)
   || ((interval_or_event == TOKEN_KEYWORD_INTERVAL) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MONTH") == 1))
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "QUARTER") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "YEAR") == 1)
   || ((interval_or_event == TOKEN_KEYWORD_INTERVAL) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SECOND_MICROSECOND") == 1))
   || ((interval_or_event == TOKEN_KEYWORD_INTERVAL) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MINUTE_MICROSECOND") == 1))
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MINUTE_SECOND") == 1)
   || ((interval_or_event == TOKEN_KEYWORD_INTERVAL) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "HOUR_MICROSECOND") == 1))
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "HOUR_SECOND") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "HOUR_MINUTE") == 1)
   || ((interval_or_event == TOKEN_KEYWORD_INTERVAL) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DAY_MICROSECOND") == 1))
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DAY_SECOND") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DAY_MINUTE") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DAY_HOUR") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "YEAR_MONTH") == 1)) {;}
else hparse_f_error();
}

void MainWindow::hparse_f_alter_or_create_event(int statement_type)
{
  if (hparse_f_qualified_name() == 0) hparse_f_error();
  if (hparse_errno > 0) return;

  bool on_seen= false, on_schedule_seen= false;
  if (statement_type == TOKEN_KEYWORD_CREATE)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "ON");
    if (hparse_errno > 0) return;
    on_seen= true;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "SCHEDULE");
    on_schedule_seen= true;
  }
  else /* if statement_type == TOKEN_KEYWORD_ALTER */
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ON") == 1)
    {
      on_seen= true;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SCHEDULE") == 1)
      {
        on_schedule_seen= true;
      }
    }
  }
  if (on_schedule_seen == true)
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AT") == 1)
    {
      hparse_f_opr_1();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EVERY") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
      hparse_f_interval_quantity(TOKEN_KEYWORD_EVENT);
      if (hparse_errno > 0) return;
    }
    else hparse_f_error();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STARTS") == 1)
    {
      hparse_f_opr_1();
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENDS") == 1)
    {
      hparse_f_opr_1();
      if (hparse_errno > 0) return;
    }
    on_seen= on_schedule_seen= false;
  }
  if (on_seen == false)
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ON") == 1) on_seen= true;
  }
  if (on_seen == true)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "COMPLETION");
    if (hparse_errno > 0) return;
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "NOT");
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "PRESERVE");
    if (hparse_errno > 0) return;
  }
  if (statement_type == TOKEN_KEYWORD_ALTER)
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RENAME") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "TO");
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENABLE") == 1) {;}
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DISABLE") == 1)
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ON") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "SLAVE");
      if (hparse_errno > 0) return;
    }
  }
  hparse_f_comment();
  if (hparse_errno > 0) return;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DO") == 1)
  {
    hparse_f_block(TOKEN_KEYWORD_EVENT);
    if (hparse_errno > 0) return;
  }
  else if (statement_type == TOKEN_KEYWORD_CREATE) hparse_f_error();
}

void MainWindow::hparse_f_create_database()
{
  hparse_f_if_not_exists();
  if (hparse_errno > 0) return;
  if (hparse_f_qualified_name() == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  bool character_seen= false, collate_seen= false;
  for (int i=0; i < 2; ++i)
  {
    bool default_seen= false;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 1) default_seen= true;
    if ((character_seen == false) && (hparse_f_character_set() == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
      if (hparse_f_character_set_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      character_seen= true;
    }
    else if (hparse_errno > 0) return;
    else if ((collate_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATE") == 1))
    {
      hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
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

/* (index_col_name,...) [index_option] for both CREATE INDEX and CREATE TABLE */
void MainWindow::hparse_f_index_columns(int index_or_table, bool fulltext_seen, bool foreign_seen)
{
    hparse_f_qualified_name();                                    /* index_name */
    if ((fulltext_seen == false) && (foreign_seen == false))
    {                                                             /* index_type */
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USING") == 1)
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BTREE") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "HASH") == 1) {;}
        else hparse_f_error();
        if (hparse_errno > 0) return;
      }
    }
  if (index_or_table == TOKEN_KEYWORD_INDEX)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "ON");             /* ON tbl_name */
    if (hparse_errno > 0) return;
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  do                                                             /* index_col_name, ... */
  {
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
    {
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ASC") != 1) hparse_f_accept(TOKEN_TYPE_KEYWORD, "DESC");
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;

  if (foreign_seen == true)
  {
    hparse_f_reference_definition();
    if (hparse_errno > 0) return;
  }
  else /* if (foreign_seen == false) */
  {
    /* MySQL doesn't check whether these clauses are repeated, but we do. */
    bool key_seen= false, using_seen= false, comment_seen= false, with_seen= false;
    for (;;)                                                             /* index_options */
    {
      if ((key_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEY_BLOCK_SIZE") == 1))
      {
        key_seen= true;
        if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "=") == 1) {;}
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        continue;
      }
      if ((using_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USING") == 1))
      {
        using_seen= true;
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BTREE") == 1) {;}
        else hparse_f_expect(TOKEN_TYPE_KEYWORD, "HASH");
        if (hparse_errno > 0) return;
        continue;
      }
      if ((with_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITH") == 1))
      {
        with_seen= true;
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "PARSER");
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
        continue;
      }
      if ((comment_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COMMENT") == 1))
      {
        comment_seen= true;
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        continue;
      }
      break;
    }
  }
}

void MainWindow::hparse_f_alter_or_create_server(int statement_type)
{
  hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
  if (hparse_errno > 0) return;
  if (statement_type == TOKEN_KEYWORD_CREATE)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "FOREIGN");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "DATA");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "WRAPPER");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
  }
  hparse_f_expect(TOKEN_TYPE_KEYWORD, "OPTIONS");
  if (hparse_errno > 0) return;
  hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
  if (hparse_errno > 0) return;
  do
  {
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "HOST") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATABASE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USER") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PASSWORD") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SOCKET") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "OWNER") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PORT") == 1))
    {
      if (hparse_f_literal() == 0) hparse_f_error();
    }
    else hparse_f_error();
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
  if (hparse_errno > 0) return;
}

/* The REQUIRE clause in GRANT and in CREATE USER after MySQL 5.7.6 */
/* + the resource_option clause */
void MainWindow::hparse_f_require(int who_is_calling, bool proxy_seen, bool role_name_seen)
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REQUIRE") == 1)
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NONE") == 1) {;}
    else
    {
      bool and_seen= false;
      for (;;)
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SSL") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "X509") == 1) hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CIPHER") == 1) hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ISSUER") == 1) hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SUBJECT") == 1) hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
        else
        {
          if (and_seen == true) hparse_f_error();
          if (hparse_errno > 0) return;
          break;
        }
        and_seen= false;
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AND") == 1) and_seen= true;
      }
    }
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITH") == 1)
  {
    for (;;)
    {
      if ((who_is_calling == TOKEN_KEYWORD_GRANT)
       && (role_name_seen == false)
       && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GRANT") == 1))
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "OPTION");
        if (hparse_errno > 0) return;
      }
      else if ((who_is_calling == TOKEN_KEYWORD_GRANT)
       && (role_name_seen == true)
       && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ADMIN") == 1))
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "OPTION");
        if (hparse_errno > 0) return;
      }
      else if (proxy_seen == true) {;}
      else if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "MAX_QUERIES_PER_HOUR") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MAX_UPDATES_PER_HOUR") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MAX_CONNECTIONS_PER_HOUR") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MAX_USER_CONNECTIONS") == 1))
      {
        hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
        if (hparse_errno > 0) return;
      }
      else break;
    }
  }
}

void MainWindow::hparse_f_user_specification_list()
{
  do
  {
    if (hparse_f_user_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IDENTIFIED") == 1)
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BY") == 1)
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PASSWORD") == 1)
        {
          if (hparse_f_literal() == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        }
        else
        {
          if (hparse_f_literal() == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        }
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITH") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AS") == 1)
        {
          if (hparse_f_literal() == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        }
      }
    }
    if (hparse_errno > 0) return;
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
}

void MainWindow::hparse_f_alter_or_create_view()
{
  if (hparse_f_qualified_name() == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  hparse_f_column_list(0);
  if (hparse_errno > 0) return;
  hparse_f_expect(TOKEN_TYPE_KEYWORD, "AS");
  if (hparse_errno > 0) return;
  if (hparse_f_select(false) == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITH") == 1)
  {
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "CASCADED") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOCAL") == 1)) {;}
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "CHECK");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "OPTION");
    if (hparse_errno > 0) return;
  }
}

/* For CALL statement or for PROCEDURE clause in SELECT */
void MainWindow::hparse_f_call()
{
  if (hparse_f_qualified_name() == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
  {
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ")") == 1) return;
    do
    {
      hparse_f_opr_1();
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_commit_or_rollback()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AND") == 1)
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NO") == 1) {;}
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "CHAIN");
    if (hparse_errno > 0) return;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NO") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "RELEASE");
    if (hparse_errno > 0) return;
  }
  else
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RELEASE") == 1) {;}
  }
}

void MainWindow::hparse_f_explain_or_describe()
{
  bool explain_type_seen= false;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EXTENDED") == 1)
  {
    explain_type_seen= true;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PARTITIONS") == 1)
  {
    explain_type_seen= true;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FORMAT") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TRADITIONAL") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "JSON");
    if (hparse_errno > 0) return;
    explain_type_seen= true;
  }
  if (explain_type_seen == false)
  {
    if (hparse_f_qualified_name() == 1)
    {
      /* DESC table_name wild ... wild can contain '%' and be unquoted. Ugly. */
      if (hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]") == 1) return;
      for (;;)
      {
        if ((hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
         || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, ".") == 1)
         || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "%") == 1)) continue;
        break;
      }
      return;
    }
  }
  if ((hparse_dbms == "mysql") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOR") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "CONNNECTION");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
    return;
  }
  hparse_f_explainable_statement();
  if (hparse_errno > 0) return;
}

/*
   With GRANT|REVOKE, first we check for identifiers (which could be role names)
   (only MariaDB) and if they're there then everything must be role names,
   if they're not there then everything must not be role names.
   Todo: I'm unsure about GRANT|REVOKE PROXY
   is_maybe_all is for check of REVOKE_ALL_PRIVILEGES,GRANT OPTION

   We need lookahead here to check for GRANT token TO|ON, but if token is
   a role we don't need to worry about GRANT role [, role...] -- see
   https://jira.mariadb.org/browse/MDEV-5772. Affected non-reserved words are:
   event, execute, file, proxy, reload, replication, shutdown, super.
*/
void MainWindow::hparse_f_grant_or_revoke(int who_is_calling, bool *role_name_seen)
{
  *role_name_seen= false;
  bool next_must_be_id= false;
  if (hparse_dbms == "mariadb")
  {
    hparse_f_next_nexttoken();
    if ((hparse_next_token.toUpper() == "TO") && (who_is_calling == TOKEN_KEYWORD_GRANT))
    {
      next_must_be_id= true;
    }
    else if ((hparse_next_token.toUpper() == "FROM") && (who_is_calling == TOKEN_KEYWORD_REVOKE))
    {
      next_must_be_id= true;
    }
  }
  bool is_maybe_all= false;
  int count_of_grants= 0;
  do
  {
    int priv_type= 0;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALL") == 1)
    {
      /* todo: find out why we're not setting priv_type here */
      is_maybe_all= true;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PRIVILEGES") == 1) {;}
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALTER") == 1)
    {
      priv_type= TOKEN_KEYWORD_ALTER;
      is_maybe_all= false;
      hparse_f_accept(TOKEN_TYPE_KEYWORD, "ROUTINE");
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CREATE") == 1)
    {
      priv_type= TOKEN_KEYWORD_CREATE;
      is_maybe_all= false;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ROUTINE") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLESPACE") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TEMPORARY") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "TABLES");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USER") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VIEW") == 1) {;}
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DELETE") == 1)
    {
      priv_type= TOKEN_KEYWORD_DELETE;
      is_maybe_all= false;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DROP") == 1)
    {
      priv_type= TOKEN_KEYWORD_DROP;
      is_maybe_all= false;
    }
    else if ((next_must_be_id == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EVENT") == 1))
    {
      priv_type= TOKEN_KEYWORD_EVENT;
      is_maybe_all= false;
    }
    else if ((next_must_be_id == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EXECUTE") == 1))
    {
      priv_type= TOKEN_KEYWORD_EXECUTE;
      is_maybe_all= false;
    }
    else if ((next_must_be_id == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FILE") == 1))
    {
      priv_type= TOKEN_KEYWORD_FILE;
      is_maybe_all= false;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GRANT") == 1)
    {
      priv_type= TOKEN_KEYWORD_GRANT;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "OPTION");
      if (hparse_errno > 0) return;
      if ((is_maybe_all == true) && (who_is_calling == TOKEN_KEYWORD_REVOKE)) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX") == 1)
    {
      priv_type= TOKEN_KEYWORD_INDEX;
      is_maybe_all= false;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INSERT") == 1)
    {
      priv_type= TOKEN_KEYWORD_INSERT;
      is_maybe_all= false;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOCK") == 1)
    {
      priv_type= TOKEN_KEYWORD_LOCK;
      is_maybe_all= false;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "TABLES");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PROCESS") == 1)
    {
      priv_type= TOKEN_KEYWORD_PROCESS;
      is_maybe_all= false;
    }
    else if ((next_must_be_id == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PROXY") == 1))
    {
      priv_type= TOKEN_KEYWORD_PROXY;
      is_maybe_all= false;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REFERENCES") == 1)
    {
      priv_type= TOKEN_KEYWORD_REFERENCES;
      is_maybe_all= false;
    }
    else if ((next_must_be_id == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RELOAD") == 1))
    {
      priv_type= TOKEN_KEYWORD_RELOAD;
      is_maybe_all= false;
    }
    else if ((next_must_be_id == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPLICATION") == 1))
    {
      priv_type= TOKEN_KEYWORD_REPLICATION;
      is_maybe_all= false;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CLIENT") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SLAVE") == 1) {;}
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SELECT") == 1)
    {
      priv_type= TOKEN_KEYWORD_SELECT;
      is_maybe_all= false;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SHOW") == 1)
    {
      priv_type= TOKEN_KEYWORD_SHOW;
      is_maybe_all= false;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATABASES") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VIEW") == 1) {;}
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((next_must_be_id == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SHUTDOWN") == 1))
    {
      priv_type= TOKEN_KEYWORD_SHUTDOWN;
      is_maybe_all= false;
    }
    else if ((next_must_be_id == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SUPER") == 1))
    {
      priv_type= TOKEN_KEYWORD_SUPER;
      is_maybe_all= false;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TRIGGER") == 1)
    {
      priv_type= TOKEN_KEYWORD_TRIGGER;
      is_maybe_all= false;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UPDATE") == 1)
    {
      priv_type= TOKEN_KEYWORD_UPDATE;
      is_maybe_all= false;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USAGE") == 1)
    {
      priv_type= TOKEN_KEYWORD_USAGE;
      is_maybe_all= false;
    }
    else
    {
      if ((hparse_dbms == "mariadb")
       && (hparse_next_token.toUpper() != "ON")
       && (hparse_next_token != ",")
       && (hparse_next_token != "(")
       && (count_of_grants == 0)
       && (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)) /* possible role name? */
      {
        *role_name_seen= true;
        return;
      }
      hparse_f_error();
      is_maybe_all= false;
    }
    if (hparse_errno > 0) return;
    ++count_of_grants;
    if ((priv_type == TOKEN_KEYWORD_SELECT)
     || (priv_type == TOKEN_KEYWORD_INSERT)
     || (priv_type == TOKEN_KEYWORD_UPDATE))
    {
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "("))
      {
        do
        {
          if (hparse_f_qualified_name() == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
        hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
    }
  } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));

  hparse_f_expect(TOKEN_TYPE_KEYWORD, "ON");
  if (hparse_errno > 0) return;
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLE") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FUNCTION") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PROCEDURE") == 1))
    {;}
  if ((hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "*") == 1) || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)) /* priv_level */
  {
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ".") == 1)
    {
      if ((hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "*") == 1) || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)) {;}
      else hparse_f_error();
    }
  }
  else hparse_f_error();
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_insert_or_replace()
{
  hparse_f_accept(TOKEN_TYPE_KEYWORD, "INTO");
  if (hparse_f_qualified_name() == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  hparse_f_partition_list(true, false);
  if (hparse_errno > 0) return;
  bool col_name_list_seen= false;
  if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
  {
    do
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
    col_name_list_seen= true;
  }
  if ((col_name_list_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SET") == 1))
  {
    hparse_f_assignment(TOKEN_KEYWORD_INSERT);
    if (hparse_errno > 0) return;
  }
  else if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "VALUES") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VALUE")) == 1)
  {
    for (;;)
    {
      hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return;
      hparse_f_expression_list(TOKEN_KEYWORD_INSERT);
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ",") == 0) break;
    }
  }
  else if (hparse_f_select(false) == 1)
  {
    return;
  }
  else hparse_f_error();
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_condition_information_item_name()
{
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "CLASS_ORIGIN") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SUBCLASS_ORIGIN") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RETURNED_SQLSTATE") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MESSAGE_TEXT") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MYSQL_ERRNO") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONSTRAINT_CATALOG") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONSTRAINT_SCHEMA") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONSTRAINT_NAME") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CATALOG_NAME") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SCHEMA_NAME") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLE_NAME") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLUMN_NAME") == 1)
   || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CURSOR_NAME") == 1)) {;}
  else hparse_f_error();
}

int MainWindow::hparse_f_signal_or_resignal(int who_is_calling)
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQLSTATE") == 1)
  {
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "VALUE");
    if (hparse_f_literal() == 0) hparse_f_error();
    if (hparse_errno > 0) return 0;
  }
  else if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) {;}
  else if (who_is_calling == TOKEN_KEYWORD_SIGNAL) return 0;
  if (hparse_errno > 0) return 0;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SET") == 1)
  {
    do
    {
      hparse_f_condition_information_item_name();
      if (hparse_errno > 0) return 0;
      hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
      if (hparse_errno > 0) return 0;
      if (hparse_f_literal() == 0)
      {
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      }
      if (hparse_errno > 0) return 0;
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  }
  return 1;
}

/* An INTO clause may appear in two different places within a SELECT. */
int MainWindow::hparse_f_into()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INTO"))
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "OUTFILE") == 1)
    {
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      /* CHARACTER SET character-set-name and export_options */
      hparse_f_infile_or_outfile();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DUMPFILE") == 1)
    {
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else
    {
      do
      {
        if (hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
        if (hparse_errno > 0) return 0;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
    return 1;
  }
  return 0;
}

/*
  "SELECT ..." or "(SELECT ...)"
*/
int MainWindow::hparse_f_select(bool select_is_already_eaten)
{
  if (hparse_statement_type == 0) hparse_statement_type= TOKEN_KEYWORD_SELECT;
  if (hparse_subquery_is_allowed == false) hparse_subquery_is_allowed= true;
  if (select_is_already_eaten == false)
  {
    /* (SELECT is the only statement that can be in parentheses, eh?) */
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
    {
      if (hparse_f_select(false) == 0)
      {
        hparse_f_error();
        return 0;
      }
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return 0;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNION") == 1)
      {
        if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALL") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DISTINCT") == 1)) {;}
        int return_value= hparse_f_select(false);
        if (hparse_errno > 0) return 0;
        if (return_value == 0)
        {
          hparse_f_error();
          return 0;
        }
      }
      hparse_f_order_by();
      if (hparse_errno > 0) return 0;
      hparse_f_limit(TOKEN_KEYWORD_SELECT);
      if (hparse_errno > 0) return 0;
      return 1;
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SELECT") == 0) return 0;
  }
  for (;;)
  {
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALL") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DISTINCT") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DISTINCTROW") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "HIGH_PRIORITY") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STRAIGHT_JOIN") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL_SMALL_RESULT") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL_BIG_RESULT") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL_BUFFER_RESULT") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL_CACHE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL_NO_CACHE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL_CALC_FOUND_ROWS") == 1))
    {
      ;
    }
    else break;
  }
  hparse_f_expression_list(TOKEN_KEYWORD_SELECT);
  if (hparse_errno > 0) return 0;
  hparse_f_into();
  if (hparse_errno > 0) return 0;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FROM") == 1)         /* FROM + some subsequent clauses are optional */
  {
    /* DUAL is a reserved word, perhaps the only one that could ever be an identifier */
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DUAL") != 1)
    {
      if (hparse_f_table_references() == 0) hparse_f_error();
    }
    if (hparse_errno > 0) return 0;
    hparse_f_where();
    if (hparse_errno > 0) return 0;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GROUP"))
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return 0;
      do
      {
        hparse_f_opr_1();
        if (hparse_errno > 0) return 0;
        if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "ASC") == 1)
         || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DESC") == 1)) {;}
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITH") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "ROLLUP");
        if (hparse_errno > 0) return 0;
      }
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "HAVING"))
    {
      hparse_f_opr_1();
      if (hparse_errno > 0) return 0;
    }
  }
  hparse_f_order_by();
  if (hparse_errno > 0) return 0;
  hparse_f_limit(TOKEN_KEYWORD_SELECT);
  if (hparse_errno > 0) return 0;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PROCEDURE"))
  {
    hparse_f_call();
    if (hparse_errno > 0) return 0;
  }
  hparse_f_into();
  if (hparse_errno > 0) return 0;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOR") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "UPDATE");
    if (hparse_errno > 0) return 0;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOCK") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "IN");
    if (hparse_errno > 0) return 0;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "SHARE");
    if (hparse_errno > 0) return 0;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "MODE");
    if (hparse_errno > 0) return 0;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNION") == 1)
  {
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALL") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DISTINCT") == 1)) {;}
    if (hparse_f_select(false) == 0)
    {
      hparse_f_error();
      return 0;
    }
  }
  return 1;
}

void MainWindow::hparse_f_where()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WHERE")) hparse_f_opr_1();
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_order_by()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ORDER") == 1)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "BY");
    if (hparse_errno > 0) return;
    do
    {
      hparse_f_opr_1();
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ASC") == 0) hparse_f_accept(TOKEN_TYPE_KEYWORD, "DESC");
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  }
}

/* LIMIT 1 or LIMIT 1,0 or LIMIT 1 OFFSET 0 from SELECT, DELETE, UPDATE, or SHOW */
void MainWindow::hparse_f_limit(int who_is_calling)
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LIMIT") == 1)
  {
    if (hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]") == 0)
    {
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    if ((who_is_calling == TOKEN_KEYWORD_DELETE) || (who_is_calling == TOKEN_KEYWORD_UPDATE)) return;
    if ((hparse_f_accept(TOKEN_TYPE_OPERATOR, ",") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "OFFSET") == 1))
    {
      if (hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]") == 0)
      {
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
      }
    }
  }
}

void MainWindow::hparse_f_like_or_where()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LIKE") == 1)
  {
    if (hparse_f_literal() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WHERE") == 1)
  {
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
  }
}

void MainWindow::hparse_f_from_or_like_or_where()
{
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "FROM") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IN") == 1))
  {
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  hparse_f_like_or_where();
}

/* SELECT ... INTO OUTFILE and LOAD DATA INFILE have a similar clause. */
void MainWindow::hparse_f_infile_or_outfile()
{
  if (hparse_f_character_set() == 1)
  {
    if (hparse_f_character_set_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  if (hparse_errno > 0) return;

  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "FIELDS") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLUMNS") == 1))
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TERMINATED") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return;
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    bool optionally_seen= false;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "OPTIONALLY") == 1) optionally_seen= true;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENCLOSED") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return;
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if (optionally_seen == true)
    {
      hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ESCAPED") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return;
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LINES") == 1)
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STARTING") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return;
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TERMINATED") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "BY");
      if (hparse_errno > 0) return;
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
  }
}

void MainWindow::hparse_f_show_columns()
{
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FROM") == 0)
  {
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "IN");
    if (hparse_errno > 0) return;
  }
  if (hparse_f_qualified_name() == 0) hparse_f_error();
  if (hparse_errno > 0) return;
  hparse_f_from_or_like_or_where();
  if (hparse_errno > 0) return;
}

void MainWindow::hparse_f_indexes_or_keys() /* for SHOW {INDEX | INDEXES | KEYS} */
{
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "FROM") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IN") == 1))
  {
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "FROM") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IN") == 1))
  {
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WHERE") == 1)
  {
    hparse_f_opr_1();
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
#define HPARSE_FLAG_USER       512
#define HPARSE_FLAG_VIEW       1024
#define HPARSE_FLAG_INSTANCE   2048
#define HPARSE_FLAG_ANY        65535

void MainWindow::hparse_f_alter_or_create_clause(int who_is_calling, unsigned short int *hparse_flags, bool *fulltext_seen)
{
  bool algorithm_seen= false, definer_seen= false, sql_seen= false, temporary_seen= false;
  bool unique_seen= false, or_seen= false, ignore_seen= false, online_seen= false;
  bool aggregate_seen= false;
  *fulltext_seen= false;
  (*hparse_flags)= HPARSE_FLAG_ANY;

  /* in MySQL OR REPLACE is only for views, in MariaDB OR REPLACE is for all creates */
  int or_replace_flags;
  if (hparse_dbms == "mariadb") or_replace_flags= HPARSE_FLAG_ANY;
  else or_replace_flags= HPARSE_FLAG_VIEW;

  if (who_is_calling == TOKEN_KEYWORD_CREATE)
  {
    ignore_seen= true;
    if (hparse_dbms == "mysql") online_seen= true;
  }
  else
  {
    temporary_seen= true; (*fulltext_seen)= true, unique_seen= true, or_seen= true;
    aggregate_seen= true;
  }
  for (;;)
  {
    if ((((*hparse_flags) & or_replace_flags) != 0) && (or_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "OR") == 1))
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "REPLACE");
      if (hparse_errno > 0) return;
      or_seen= true; (*hparse_flags) &= or_replace_flags;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_VIEW) != 0) && (algorithm_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALGORITHM") == 1))
    {
      hparse_f_algorithm();
      if (hparse_errno > 0) return;
      algorithm_seen= true; (*hparse_flags) &= HPARSE_FLAG_VIEW;
    }
    else if ((((*hparse_flags) & (HPARSE_FLAG_VIEW + HPARSE_FLAG_ROUTINE)) != 0) && (definer_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFINER") == 1))
    {
      hparse_f_definer();
      if (hparse_errno > 0) return;
      definer_seen= true; (*hparse_flags) &= (HPARSE_FLAG_VIEW + HPARSE_FLAG_ROUTINE);
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_VIEW) != 0) && (sql_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL") == 1))
    {
      hparse_f_sql();
      if (hparse_errno > 0) return;
      sql_seen= true;  (*hparse_flags) &= HPARSE_FLAG_VIEW;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_TABLE) != 0) && (ignore_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IGNORE") == 1))
    {
      ignore_seen= true; (*hparse_flags) &= HPARSE_FLAG_TABLE;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_TABLE) != 0) && (temporary_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TEMPORARY") == 1))
    {
      temporary_seen= true; (*hparse_flags) &= HPARSE_FLAG_TABLE;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_TABLE) != 0) && (hparse_dbms == "mariadb") && (online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ONLINE") == 1))
    {
      online_seen= true; (*hparse_flags) &= (HPARSE_FLAG_INDEX | HPARSE_FLAG_TABLE);
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_TABLE) != 0) && (hparse_dbms == "mariadb") && (online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "OFFLINE") == 1))
    {
      online_seen= true; (*hparse_flags) &= HPARSE_FLAG_INDEX;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_INDEX) != 0) && (unique_seen == false) && ((*fulltext_seen) == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FULLTEXT") == 1))
    {
      (*fulltext_seen)= true; (*hparse_flags) &= HPARSE_FLAG_INDEX;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_INDEX) != 0) && ((*fulltext_seen) == false) && (unique_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SPATIAL") == 1))
    {
      (*fulltext_seen)= true; (*hparse_flags) &= HPARSE_FLAG_INDEX;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_INDEX) != 0) && (unique_seen == false) && ((*fulltext_seen) == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNIQUE") == 1))
    {
      unique_seen= true; (*hparse_flags) &= HPARSE_FLAG_INDEX;
    }
    else if ((((*hparse_flags) & HPARSE_FLAG_ROUTINE) != 0) && (aggregate_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AGGREGATE") == 1))
    {
      aggregate_seen= true; (*hparse_flags) &= HPARSE_FLAG_ROUTINE;
    }
    else break;
  }
}

/* ; or (; + delimiter) or delimiter or \G or \g */
int MainWindow::hparse_f_semicolon_and_or_delimiter(int calling_statement_type)
{
  if (hparse_f_accept(TOKEN_TYPE_DELIMITER, "\\G") == 1)
  {
    return 1;
  }
  /* TEST!! removed next line */
  if ((calling_statement_type == 0) || (calling_statement_type != 0))
  {
    if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ";") == 1)
    {
      hparse_f_accept(TOKEN_TYPE_DELIMITER, hparse_delimiter_str);
      return 1;
    }
    else if (hparse_f_accept(TOKEN_TYPE_DELIMITER, hparse_delimiter_str) == 1) return 1;
    return 0;
  }
  else return (hparse_f_accept(TOKEN_TYPE_OPERATOR, ";"));
}

/*
  For EXPLAIN and perhaps for ANALYZE, we want to accept only a
  statement that would legal therein. So check if that's what follows,
  if it is then call hparse_f_statement, if it's not then call
  hparse_f_accept which is guaranteed to fail.
  Return 1 if it was a statement, else return 0 (which might also mean error).
*/
int MainWindow::hparse_f_explainable_statement()
{
  QString hparse_token_upper= hparse_token.toUpper();
  if (hparse_dbms == "mysql")
  {
    if ((hparse_token_upper == "DELETE")
     || (hparse_token_upper == "INSERT")
     || (hparse_token_upper == "REPLACE")
     || (hparse_token_upper == "SELECT")
     || (hparse_token_upper == "UPDATE"))
    {
      hparse_f_statement();
      if (hparse_errno > 0) return 0;
      return 1;
    }
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "DELETE");
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "INSERT");
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPLACE");
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "SELECT");
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "UPDATE");
    return 0;
  }
  else if (hparse_dbms == "mariadb")
  {
    if ((hparse_token_upper == "DELETE")
     || (hparse_token_upper == "SELECT")
     || (hparse_token_upper == "UPDATE"))
    {
      hparse_f_statement();
      if (hparse_errno > 0) return 0;
      return 1;
    }
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "DELETE");
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "SELECT");
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "UPDATE");
    return 0;
  }
  return 0;
}

/* TODO: I THINK I'M FORGETTING TO SAY return FOR A LOT OF MAIN STATEMENTS! */
/*
statement =
    "connect" "create" "drop" etc. etc.
    The idea is to parse everything that's described in the MySQL 5.7 manual.
*/
void MainWindow::hparse_f_statement()
{
  if (hparse_errno > 0) return;
  hparse_statement_type= 0;
  hparse_subquery_is_allowed= 0;
  if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALTER"))
  {
    hparse_statement_type= TOKEN_KEYWORD_ALTER;
    unsigned short int hparse_flags; bool fulltext_seen;
    hparse_f_alter_or_create_clause(TOKEN_KEYWORD_ALTER, &hparse_flags, &fulltext_seen);
    if ((((hparse_flags) & HPARSE_FLAG_DATABASE) != 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATABASE") == 1))
    {
      hparse_f_alter_database();
    }
    else if ((((hparse_flags) & HPARSE_FLAG_ROUTINE) != 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EVENT") == 1))
    {
      hparse_f_alter_or_create_event(TOKEN_KEYWORD_ALTER);
      if (hparse_errno > 0) return;
    }
    else if ((((hparse_flags) & HPARSE_FLAG_ROUTINE) != 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FUNCTION") == 1))
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_characteristics();
    }
    else if ((((hparse_flags) & HPARSE_FLAG_INSTANCE) != 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INSTANCE") == 1))
    {
      /* Todo: This statement appears to have disappeared. */
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "ROTATE");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "INNODB");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "MASTER");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "KEY");
      if (hparse_errno > 0) return;
    }
    /* TODO: ALTER LOGFILE GROUP is not supported */
    else if ((((hparse_flags) & HPARSE_FLAG_ROUTINE) != 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PROCEDURE") == 1))
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_characteristics();
    }
    else if ((((hparse_flags) & HPARSE_FLAG_DATABASE) != 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SCHEMA") == 1))
    {
      hparse_f_alter_database();
    }
    else if ((((hparse_flags) & HPARSE_FLAG_SERVER) != 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SERVER") == 1))
    {
      hparse_f_alter_or_create_server(TOKEN_KEYWORD_ALTER);
      if (hparse_errno > 0) return;
    }
    else if ((((hparse_flags) & HPARSE_FLAG_TABLE) != 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLE") == 1))
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      do
      {
        hparse_f_alter_specification();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      hparse_f_partition_options();
    }
    else if ((((hparse_flags) & HPARSE_FLAG_USER) != 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USER") == 1))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      hparse_f_user_specification_list();
      if (hparse_errno > 0) return;
      hparse_f_require(TOKEN_KEYWORD_ALTER, false, false);
      if (hparse_errno > 0) return;
    }
    else if ((((hparse_flags) & HPARSE_FLAG_VIEW) != 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VIEW") == 1))
    {
      hparse_f_alter_or_create_view();
      if (hparse_errno > 0) return;
    }
    else hparse_f_error();
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ANALYZE") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_ANALYZE;
    if (hparse_f_analyze_or_optimize(TOKEN_KEYWORD_ANALYZE) == 1)
    {
      if (hparse_dbms == "mariadb")
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PERSISTENT") == 1)
        {
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "FOR");
          if (hparse_errno > 0) return;
          if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALL") == 1) return;
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "COLUMNS");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
          if (hparse_errno > 0) return;
          for (;;)
          {
            if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
            {
              if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ",") == 1) continue;
            }
            break;
          }
          hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "INDEXES");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
          if (hparse_errno > 0) return;
          for (;;)
          {
            if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1)
            {
              if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ",") == 1) continue;
            }
            break;
          }
          hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
          if (hparse_errno > 0) return;
        }
      }
      return;
    }
    if (hparse_dbms == "mariadb")
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FORMAT") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
        if (hparse_errno > 0) return;
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TRADITIONAL") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "JSON");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_explainable_statement() == 1) return;
      if (hparse_errno > 0) return;
    }
    hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_BEGIN_WORK, "BEGIN") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_BEGIN_WORK; /* don't confuse with BEGIN for compound */
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WORK") == 1) {;}
    return;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BINLOG") == 1)
  {
    //hparse_statement_type= TOKEN_KEYWORD_BINLOG;
    if (hparse_f_literal() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CACHE") == 1)
  {
    //hparse_statement_type= TOKEN_KEYWORD_CACHE;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "INDEX");
    if (hparse_errno > 0) return;
    do
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEY") == 1)) {;}
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        do
        {
          if (hparse_f_qualified_name() == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
        hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    if (hparse_f_partition_list(true, true) == 0) hparse_f_error(); /* todo: I think ALL is within parentheses? */
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "IN");
    if (hparse_errno > 0) return;
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CALL") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_CALL;
    hparse_f_call();
    if (hparse_errno > 0) return;
    return;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHANGE") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_CHANGE;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER") == 1)
    {
      if (hparse_dbms == "mariadb") hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]");
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "TO");
      if (hparse_errno > 0) return;
      do
      {
        if (((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DO_DOMAIN_IDS") == 1))
         || ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IGNORE_DOMAIN_IDS") == 1))
         || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IGNORE_SERVER_IDS") == 1))
        {
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
          if (hparse_errno > 0) return;
          do
          {
            hparse_f_literal(); /* this allows "()" */
            if (hparse_errno > 0) return;
          } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
          hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_AUTO_POSITION")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_BIND")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_CONNECT_RETRY")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if ((hparse_dbms == "mysql") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_DELAY"))) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_HEARTBEAT_PERIOD")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_HOST")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_LOG_FILE")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_LOG_POS")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_PASSWORD")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_PORT")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if ((hparse_dbms == "mysql") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_RETRY_COUNT"))) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_SSL"))
        {
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "0") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "1");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_SSL_CA")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_SSL_CAPATH")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_SSL_CERT")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_SSL_CIPHER")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_SSL_CRL")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_SSL_CRLPATH")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_SSL_KEY")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_SSL_VERIFY_SERVER_CERT"))
        {
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "0") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "1");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_USER")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_USE_GTID"))) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if ((hparse_dbms == "mysql") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_TLS_VERSION"))) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RELAY_LOG_FILE")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RELAY_LOG_POS")) {hparse_f_expect(TOKEN_TYPE_OPERATOR, "="); hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");}
        else hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      hparse_f_for_channel();
      if (hparse_errno > 0) return;
      return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPLICATION") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "FILTER");
      if (hparse_errno > 0) return;
      do
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPLICATE_DO_DB") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPLICATE_IGNORE_DB") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPLICATE_DO_TABLE") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPLICATE_IGNORE_TABLE") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPLICATE_WILD_DO_TABLE") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPLICATE_WILD_IGNORE_TABLE") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPLICATE_REWRITE_DB") == 1) {;}
        else hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_column_list(1); /* Todo: take into account what kind of list it should be */
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
    else hparse_f_error();
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHECK") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_CHECK;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLE") == 1)
    {
      if (hparse_errno > 0) return;
      do
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      for (;;)
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOR") == 1)
        {
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "UPGRADE");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "QUICK") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FAST") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MEDIUM") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EXTENDED") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHANGED") == 1) {;}
        else break;
      }
    }
    else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VIEW") == 1))
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CHECKSUM") == 1)
  {
    //hparse_statement_type= TOKEN_KEYWORD_CHECKSUM;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "TABLE");
    if (hparse_errno > 0) return;
    do
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "QUICK") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EXTENDED") == 1)) {;}
    else hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COMMIT") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_COMMIT;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WORK") == 1) {;}
    hparse_f_commit_or_rollback();
    return;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONNECT"))
  {
    hparse_statement_type= TOKEN_KEYWORD_CONNECT;
    return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_CREATE, "CREATE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_CREATE;
    unsigned short int hparse_flags;
    bool fulltext_seen;
    hparse_f_alter_or_create_clause(TOKEN_KEYWORD_CREATE, &hparse_flags, &fulltext_seen);
    if (hparse_errno > 0) return;
    if (((hparse_flags & HPARSE_FLAG_DATABASE) != 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATABASE") == 1))
    {
      hparse_f_create_database();
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_ROUTINE) != 0) && (hparse_f_accept(TOKEN_KEYWORD_EVENT, "EVENT") == 1))
    {
      hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      hparse_f_alter_or_create_event(TOKEN_KEYWORD_CREATE);
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_ROUTINE) != 0) && (hparse_f_accept(TOKEN_KEYWORD_FUNCTION, "FUNCTION") == 1))
    {
      if (hparse_dbms == "mariadb") hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      /* If (parameter_list) isn't there, it might be a UDF */
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RETURNS") == 1)
      {
        /* Manual doesn't mention INT or DEC. I wonder what else it doesn't mention. */
        if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "STRING") == 1)
         || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INTEGER") == 1)
         || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INT") == 1)
         || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REAL") == 1)
         || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DECIMAL") == 1)
         || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEC") == 1)) {;}
        else hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "SONAME");
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
        if (hparse_errno > 0) return;
      }
      else
      {
        hparse_f_parameter_list(TOKEN_KEYWORD_FUNCTION);
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "RETURNS");
        if (hparse_errno > 0) return;
        hparse_f_data_type();
        if (hparse_errno > 0) return;
        hparse_f_characteristics();
        if (hparse_errno > 0) return;
        hparse_f_block(TOKEN_KEYWORD_FUNCTION);
        if (hparse_errno > 0) return;
      }
    }
    else if (((hparse_flags & HPARSE_FLAG_INDEX) != 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX") == 1))
    {
      if (hparse_dbms == "mariadb") hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_index_columns(TOKEN_KEYWORD_INDEX, fulltext_seen, false);
      if (hparse_errno > 0) return;
      hparse_f_algorithm_or_lock();
    }
    else if (((hparse_flags & HPARSE_FLAG_ROUTINE) != 0) && (hparse_f_accept(TOKEN_KEYWORD_PROCEDURE, "PROCEDURE") == 1))
    {
      if (hparse_dbms == "mariadb") hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_parameter_list(TOKEN_KEYWORD_PROCEDURE);
      if (hparse_errno > 0) return;
      hparse_f_characteristics();
      if (hparse_errno > 0) return;
      hparse_f_block(TOKEN_KEYWORD_PROCEDURE);
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms == "mariadb") && ((hparse_flags & HPARSE_FLAG_USER) != 0) && (hparse_f_accept(TOKEN_KEYWORD_ROLE, "ROLE") == 1))
    {
      if (hparse_dbms == "mariadb") hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      if  (QString::compare(hparse_token, "NONE", Qt::CaseInsensitive) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITH") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "ADMIN");
        if (hparse_errno > 0) return;
        if (hparse_f_user_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
    }
    else if (((hparse_flags & HPARSE_FLAG_DATABASE) != 0) && (hparse_f_accept(TOKEN_KEYWORD_SCHEMA, "SCHEMA") == 1))
    {
      hparse_f_create_database();
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_SERVER) != 0) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SERVER") == 1))
    {
      hparse_f_alter_or_create_server(TOKEN_KEYWORD_CREATE);
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_TABLE) != 0) && (hparse_f_accept(TOKEN_KEYWORD_TABLE, "TABLE") == 1))
    {
      ; /* TODO: CREATE TABLE -- other possibilities */
      hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LIKE") == 1)
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LIKE") == 1)
        {
          if (hparse_f_qualified_name() == 0) hparse_f_error();
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
          if (hparse_errno > 0) return;
          return;
        }
        do
        {
          if (hparse_f_qualified_name() == 1)
          {
            hparse_f_column_definition();
            if (hparse_errno > 0) return;
          }
          else
          {
            if (hparse_errno > 0) return;
            hparse_f_create_definition();
            if (hparse_errno > 0) return;
          }
        } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
        hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
      hparse_f_table_or_partition_options(TOKEN_KEYWORD_TABLE);
      if (hparse_errno > 0) return;
      hparse_f_partition_options();
      if (hparse_errno > 0) return;
      bool ignore_or_as_seen= false;
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "IGNORE") || hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPLACE") == 1)) ignore_or_as_seen= true;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AS") == 1) ignore_or_as_seen= true;
      if (ignore_or_as_seen == true)
      {
        if (hparse_f_select(false) == 0)
        {
          hparse_f_error();
          return;
        }
      }
      else hparse_f_select(false);
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_TABLESPACE) != 0) && (hparse_f_accept(TOKEN_KEYWORD_TABLESPACE, "TABLESPACE") == 1))
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "ADD");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "DATAFILE");
      if (hparse_errno > 0) return;
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FILE_BLOCK_SIZE") == 1)
      {
        if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "=") == 1) {;}
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENGINE") == 1)
      {
        hparse_f_engine();
        if (hparse_errno > 0) return;
      }
    }
    else if (((hparse_flags & HPARSE_FLAG_ROUTINE) != 0) && (hparse_f_accept(TOKEN_KEYWORD_TRIGGER, "TRIGGER") == 1))
    {
      if (hparse_dbms == "mariadb") hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BEFORE") == 0)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "AFTER");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INSERT") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UPDATE") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DELETE") == 1) {;}
      else hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "ON");
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "FOR");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "EACH");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "ROW");
      if (hparse_errno > 0) return;
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOLLOWS") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PRECEDES")) == 1)
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      hparse_f_block(TOKEN_KEYWORD_TRIGGER);
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_USER) != 0) && (hparse_f_accept(TOKEN_KEYWORD_USER, "USER") == 1))
    {
      hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      hparse_f_user_specification_list();
      if (hparse_errno > 0) return;
      hparse_f_require(TOKEN_KEYWORD_CREATE, false, false);
      if (hparse_errno > 0) return;
    }
    else if (((hparse_flags & HPARSE_FLAG_VIEW) != 0) && (hparse_f_accept(TOKEN_KEYWORD_VIEW, "VIEW") == 1))
    {
      if (hparse_dbms == "mariadb") hparse_f_if_not_exists();
      if (hparse_errno > 0) return;
      hparse_f_alter_or_create_view();
      if (hparse_errno > 0) return;
    }
    else hparse_f_error();
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_DEALLOCATE, "DEALLOCATE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_DEALLOCATE;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "PREPARE");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_DELETE, "DELETE"))
  {
    /* todo: look up how partitions are supposed to be handled */
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_DELETE;
    hparse_subquery_is_allowed= true;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOW_PRIORITY")) {;}
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "QUICK")) {;}
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IGNORE")) {;}
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FROM") == 1)
    {
      bool multi_seen= false;
      if (hparse_f_qualified_name_with_star() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","))
      {
        multi_seen= true;
        do
        {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      }
      if (multi_seen == true) hparse_f_expect(TOKEN_TYPE_KEYWORD, "USING");
      if (hparse_errno > 0) return;
      if ((multi_seen == true) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USING") == 1))
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
      hparse_f_order_by();
      if (hparse_errno > 0) return;
      hparse_f_limit(TOKEN_KEYWORD_DELETE);
      if (hparse_errno > 0) return;
      if (hparse_dbms == "mariadb")
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RETURNING") == 1)
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
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "FROM");
    if (hparse_errno > 0) return;
    if (hparse_f_table_references() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    hparse_f_where();
    if (hparse_errno > 0) return;
    return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_DESC, "DESC") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_DESC;
    hparse_f_explain_or_describe();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_DESCRIBE, "DESCRIBE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_DESCRIBE;
    hparse_f_explain_or_describe();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_DO, "DO"))
  {
    hparse_statement_type= TOKEN_KEYWORD_DO;
    hparse_subquery_is_allowed= true;
    do
    {
      hparse_f_opr_1();
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_DROP, "DROP"))         /* drop database/event/etc. */
  {
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_DROP;
    bool temporary_seen= false, online_seen= false;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TEMPORARY") == 1) temporary_seen= true;
    if ((temporary_seen == false) && (hparse_dbms == "mariadb"))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ONLINE") == 1) online_seen= true;
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "OFFLINE") == 1) online_seen= true;
    }
    if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATABASE")))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EVENT")))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FUNCTION")))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX")))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "ON");
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_algorithm_or_lock();
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PREPARE")))
    {
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PROCEDURE")))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ROLE")== 1))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      do
      {
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SCHEMA")))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SERVER")))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLE")))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      do
      {
        if (hparse_errno > 0) return;
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      if (hparse_errno > 0) return;
      hparse_f_accept(TOKEN_TYPE_KEYWORD, "RESTRICT");
      if (hparse_errno > 0) return;
      hparse_f_accept(TOKEN_TYPE_KEYWORD, "CASCADE");
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLESPACE")))
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENGINE"))
      {
        hparse_f_accept(TOKEN_TYPE_OPERATOR, "=");
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      }
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TRIGGER")))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USER")))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      do
      {
        if (hparse_f_user_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
    else if ((temporary_seen == false) && (online_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VIEW")))
    {
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IF") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXISTS");
        if (hparse_errno > 0) return;
      }
      do
      {
        if (hparse_errno > 0) return;
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      if (hparse_errno > 0) return;
      hparse_f_accept(TOKEN_TYPE_KEYWORD, "RESTRICT");
      if (hparse_errno > 0) return;
      hparse_f_accept(TOKEN_TYPE_KEYWORD, "CASCADE");
      if (hparse_errno > 0) return;
    }
    else hparse_f_error();
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_EXECUTE, "EXECUTE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_EXECUTE;
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USING"))
    {
      do
      {
       hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
       if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_EXPLAIN, "EXPLAIN"))
  {
    hparse_statement_type= TOKEN_KEYWORD_EXPLAIN;
    hparse_f_explain_or_describe();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_FLUSH, "FLUSH") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_FLUSH;
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "NO_WRITE_TO_BINLOG") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOCAL") == 1)) {;}
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLES") == 1)
    {
      bool table_name_seen= false, comma_seen= false;
      for (;;)
      {
        if (hparse_f_qualified_name() == 0)
        {
          if (hparse_errno > 0) return;
          if (comma_seen == true) hparse_f_error();
          if (hparse_errno > 0) return;
          break;
        }
        table_name_seen= true;
        comma_seen= false;
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, ","))
        {
          comma_seen= true;
          continue;
        }
      }
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITH") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "READ");
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "LOCK");
        if (hparse_errno > 0) return;
      }
      else if ((table_name_seen == true) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOR") == 1))
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "EXPORT");
        if (hparse_errno > 0) return;
      }
    }
    else do
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BINARY") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "LOGS");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DES_KEY_FILE") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENGINE") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "LOGS");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ERROR") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "LOGS");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GENERAL") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "LOGS");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "HOSTS") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "OPTIMIZER_COSTS") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PRIVILEGES") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "QUERY") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "CACHE");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RELAY") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "LOGS");
        if (hparse_errno > 0) return;
        hparse_f_for_channel();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SLOW") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "LOGS");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STATUS") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USER_RESOURCES") == 1) {;}
      else hparse_f_error();
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_GET, "GET"))
  {
    hparse_statement_type= TOKEN_KEYWORD_GET;
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "CURRENT");
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "DIAGNOSTICS");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONDITION") == 1)
    {
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      do
      {
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
        if (hparse_errno > 0) return;
        hparse_f_condition_information_item_name();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
    else do
    {
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NUMBER") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "ROW_COUNT");
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_GRANT, "GRANT"))
  {
    hparse_statement_type= TOKEN_KEYWORD_GRANT;
    bool proxy_seen= false;
    bool role_name_seen= false;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PROXY") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "ON");
      if (hparse_errno > 0) return;
      if (hparse_f_user_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      proxy_seen= true;
    }
    else
    {
      hparse_f_grant_or_revoke(TOKEN_KEYWORD_GRANT, &role_name_seen);
      if (hparse_errno > 0) return;
    }
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "TO");
    if (hparse_errno > 0) return;
    hparse_f_user_specification_list();
    if (hparse_errno > 0) return;
    hparse_f_require(TOKEN_KEYWORD_GRANT, proxy_seen, role_name_seen);
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_HANDLER, "HANDLER"))
  {
    hparse_statement_type= TOKEN_KEYWORD_HANDLER;
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "OPEN") == 1)
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AS") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "READ") == 1)
    {
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "FIRST") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NEXT") == 1)) {;}
      else if (hparse_f_qualified_name() == 1)
      {
        if ((hparse_f_accept(TOKEN_TYPE_OPERATOR, "=") == 1)
         || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "<=") == 1)
         || (hparse_f_accept(TOKEN_TYPE_OPERATOR, ">=") == 1)
         || (hparse_f_accept(TOKEN_TYPE_OPERATOR, ">") == 1)
         || (hparse_f_accept(TOKEN_TYPE_OPERATOR, "<") == 1))
        {
          hparse_f_expression_list(TOKEN_KEYWORD_HANDLER);
          if (hparse_errno > 0) return;
        }
        else if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "FIRST") == 1)
              || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NEXT") == 1)
              || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PREV") == 1)
              || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LAST") == 1))
           {;}
        else hparse_f_error();
        if (hparse_errno > 0) return;
      }
      hparse_f_where();
      hparse_f_limit(TOKEN_KEYWORD_HANDLER);
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CLOSE") == 1) {;}
    else hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_HELP, "HELP"))
  {
    hparse_statement_type= TOKEN_KEYWORD_HELP;
    if (hparse_f_literal() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_INSERT, "INSERT"))
  {
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_INSERT;
    hparse_subquery_is_allowed= true;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOW_PRIORITY") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DELAYED") == 1) {;}
    else hparse_f_accept(TOKEN_TYPE_KEYWORD, "HIGH_PRIORITY");
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "IGNORE");
    hparse_f_insert_or_replace();
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ON") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "DUPLICATE");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "KEY");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "UPDATE");
      if (hparse_errno > 0) return;
      hparse_f_assignment(TOKEN_KEYWORD_INSERT);
      if (hparse_errno > 0) return;
    }
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_INSTALL, "INSTALL") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_INSTALL;
    if (hparse_dbms == "mysql")
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "PLUGIN");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    else if (hparse_dbms == "mariadb")
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PLUGIN") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
      }
    }
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "SONAME");
    if (hparse_errno > 0) return;
    if (hparse_f_literal() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_KILL, "KILL"))
  {
    hparse_statement_type= TOKEN_KEYWORD_KILL;
    if (hparse_dbms == "mariadb")
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "HARD") == 0) hparse_f_accept(TOKEN_TYPE_KEYWORD, "SOFT");
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONNECTION") == 0)
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "QUERY") == 1)
      {
        if (hparse_dbms == "mariadb") hparse_f_accept(TOKEN_TYPE_KEYWORD, "ID");
      }
    }
    if (hparse_f_literal() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_LOAD, "LOAD"))
  {
    hparse_statement_type= TOKEN_KEYWORD_LOAD;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATA") == 1)
    {
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOW_PRIORITY") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONCURRENT") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOCAL") == 1))
        {;}
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "INFILE");
      if (hparse_errno > 0) return;
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPLACE") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IGNORE") == 1))
        {;}
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "INTO");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "TABLE");
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      hparse_f_partition_list(true, false);
      if (hparse_errno > 0) return;
      hparse_f_infile_or_outfile();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IGNORE") == 1)
      {
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LINES") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "ROWS");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1) /* [(col_name_or_user_var...)] */
      {
        do
        {
          hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
          if (hparse_errno > 0) return;
        } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
        hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SET") == 1)
      {
        hparse_f_assignment(TOKEN_KEYWORD_LOAD);
        if (hparse_errno > 0) return;
      }

    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "INTO");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "CACHE");
      if (hparse_errno > 0) return;
      do
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_partition_list(true, true);
        if (hparse_errno > 0) return;
        if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEY") == 1))
        {
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
          if (hparse_errno > 0) return;
          do
          {
            if (hparse_f_qualified_name() == 0) hparse_f_error();
            if (hparse_errno > 0) return;
          } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
          hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
          if (hparse_errno > 0) return;
        }
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IGNORE") == 1)
        {
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "LEAVES");
          if (hparse_errno > 0) return;
        }
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "XML") == 1)
    {
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOW_PRIORITY") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONCURRENT") == 1)) {;}
      hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOCAL");
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "INFILE");
      if (hparse_errno > 0) return;
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPLACE") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IGNORE") == 1)) {;}
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "INTO");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "TABLE");
      if (hparse_errno > 0) return;
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_character_set() == 1)
      {
        if (hparse_f_character_set_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ROWS") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "IDENTIFIED");
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "BY");
        if (hparse_errno > 0) return;
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IGNORE") == 1)
      {
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "LINES") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ROWS") == 1)) {;}
        else hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        do
        {
          if (hparse_f_qualified_name() == 0) hparse_f_error();
          if (hparse_errno > 0) return;
        } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
        hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SET") == 1)
      {
        hparse_f_assignment(TOKEN_KEYWORD_LOAD);
        if (hparse_errno > 0) return;
      }
    }
    else hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_LOCK, "LOCK"))
  {
    hparse_statement_type= TOKEN_KEYWORD_LOCK;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLE") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "TABLES"); /* TABLE is undocumented */
    if (hparse_errno > 0) return;
    do
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AS") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) {;}
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "READ") == 1)
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOCAL") == 1) {;}
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOW_PRIORITY") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "WRITE");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WRITE") == 1)
      {
        if (hparse_dbms == "mariadb") hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONCURRENT");
      }
      else hparse_f_error();
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_OPTIMIZE, "OPTIMIZE") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_OPTIMIZE;
    if (hparse_f_analyze_or_optimize(TOKEN_KEYWORD_OPTIMIZE) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_PREPARE, "PREPARE") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_PREPARE;
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "FROM");
    if (hparse_errno > 0) return;
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
    return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_PURGE, "PURGE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_PURGE;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BINARY") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "MASTER");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "LOGS");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TO") == 1)
    {
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "BEFORE");
      if (hparse_errno > 0) return;
      hparse_f_opr_1(); /* actually, should be "datetime expression" */
      if (hparse_errno > 0) return;
    }
    return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_RELEASE, "RELEASE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_RELEASE;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "SAVEPOINT");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_RENAME, "RENAME") == 1)
  {
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_RENAME;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USER"))
    {
      do
      {
        if (hparse_errno > 0) return;
        if (hparse_f_user_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "TO");
        if (hparse_errno > 0) return;
        if (hparse_f_user_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
    else {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "TABLE");
      if (hparse_errno > 0) return;
      do
      {
        if (hparse_errno > 0) return;
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "TO");
        if (hparse_errno > 0) return;
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_REPAIR, "REPAIR") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_REPAIR;
    /* todo: somewhere I got the idea that "FROM MYSQL" is allowed in this vicinity. */
    if (hparse_f_analyze_or_optimize(TOKEN_KEYWORD_REPAIR) == 1)
    {
      hparse_f_accept(TOKEN_TYPE_KEYWORD, "QUICK");
      hparse_f_accept(TOKEN_TYPE_KEYWORD, "EXTENDED");
      hparse_f_accept(TOKEN_TYPE_KEYWORD, "USE_FRM");
    }
    else hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_REPLACE, "REPLACE"))
  {
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_REPLACE;
    //hparse_statement_type= TOKEN_KEYWORD_INSERT;
    hparse_subquery_is_allowed= true;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOW_PRIORITY") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DELAYED") == 1) {;}
    hparse_f_insert_or_replace();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_RESET, "RESET"))
  {
    hparse_statement_type= TOKEN_KEYWORD_RESET;
    do
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER") == 1)
      {
        if (hparse_dbms == "mariadb")
        {
          if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TO") == 1)
          {
            hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
            if (hparse_errno > 0) return;
          }
        }
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "QUERY_CACHE") == 1) {;}
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SLAVE") == 1)
      {
        if (hparse_dbms == "mariadb")
        {
          hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]");
          hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALL");
        }
      }
      else hparse_f_error();
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_RESIGNAL, "RESIGNAL"))
  {
    hparse_statement_type= TOKEN_KEYWORD_RESIGNAL;
    /* We accept RESIGNAL even if we're not in a condition handler; we're just a recognizer. */
    hparse_f_signal_or_resignal(TOKEN_KEYWORD_RESIGNAL);
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REVOKE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_REVOKE;
    bool role_name_seen= false;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PROXY") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "ON");
      if (hparse_errno > 0) return;
      if (hparse_f_user_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else
    {
      hparse_f_grant_or_revoke(TOKEN_KEYWORD_REVOKE, &role_name_seen);
      if (hparse_errno > 0) return;
    }
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "FROM");
    if (hparse_errno > 0) return;
    do
    {
      if (hparse_f_user_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_ROLLBACK, "ROLLBACK") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_ROLLBACK;
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "WORK");
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TO") == 1)
    {
      /* it's not documented, but the word SAVEPOINT is optional */
      hparse_f_accept(TOKEN_TYPE_KEYWORD, "SAVEPOINT");
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      return;
    }
    else hparse_f_commit_or_rollback();
    return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_SAVEPOINT, "SAVEPOINT") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_SAVEPOINT;
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    return;
  }
  else if (hparse_f_select(false) == 1)
  {
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_SET, "SET"))
  {
    hparse_statement_type= TOKEN_KEYWORD_SET;
    hparse_subquery_is_allowed= true;
    bool global_seen= false;
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "GLOBAL") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SESSION") == 1)
     || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOCAL") == 1))
    {
      global_seen= true;
    }
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TRANSACTION") == 1)
    {
      bool isolation_seen= false, read_seen= false;
      do
      {
        if ((isolation_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ISOLATION") == 1))
        {
          isolation_seen= true;
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "LEVEL");
          if (hparse_errno > 0) return;
          if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "REPEATABLE") == 1)
          {
            hparse_f_expect(TOKEN_TYPE_KEYWORD, "READ");
            if (hparse_errno > 0) return;
          }
          else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "READ") == 1)
          {
            if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COMMITTED") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "UNCOMMITTED");
            if (hparse_errno > 0) return;
          }
          else hparse_f_expect(TOKEN_TYPE_KEYWORD, "SERIALIZABLE");
          if (hparse_errno > 0) return;
        }
        else if ((read_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "READ") == 1))
        {
          if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WRITE") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "ONLY");
          if (hparse_errno > 0) return;
        }
        else hparse_f_error();
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      return;
    }
    if ((global_seen == false) && (hparse_f_character_set() == 1))
    {
      if (hparse_f_character_set_name() == 0)
      {
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATE"))
      {
        if (hparse_f_collation_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      return;
    }
    if (hparse_errno > 0) return;
    if ((hparse_dbms == "mariadb") && (global_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 1))
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "ROLE");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NONE") == 0)
      {
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOR") == 1)
      {
        if (hparse_f_user_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      return;
    }
    if ((global_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NAMES") == 1))
    {
      if (hparse_f_character_set_name() == 0)
      {
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATE"))
      {
        if (hparse_f_collation_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      return;
    }
    if ((global_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PASSWORD") == 1))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOR") == 1)
      {
        if (hparse_f_user_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PASSWORD") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
        if (hparse_errno > 0) return;
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
        if (hparse_errno > 0) return;
      }
      else
      {
        if (hparse_f_literal() == 0) hparse_f_error();
      }
      if (hparse_errno > 0) return;
      return;
    }
    if ((hparse_dbms == "mariadb") && (global_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ROLE") == 1))
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NONE") == 0)
      {
        hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      }
      if (hparse_errno > 0) return;
      return;
    }
    if ((hparse_dbms == "mariadb") && (global_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STATEMENT") == 1))
    {
      hparse_f_assignment(TOKEN_KEYWORD_SET);
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "FOR");
      if (hparse_errno > 0) return;
      hparse_f_statement();
      if (hparse_errno > 0) return;
      return;
    }
    /* TODO: This fails to take "set autocommit = {0 | 1}" into account as special. */
    /* TODO: This fails to take "set sql_log_bin = {0 | 1}" into account as special. */
    hparse_f_assignment(TOKEN_KEYWORD_SET);
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_SHOW, "SHOW") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_SHOW;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALL") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "SLAVES");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "STATUS");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "AUTHORS") == 1) {;} /* removed in MySQL 5.6.8 */
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BINARY") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "LOGS");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BINLOG") == 1) /* show binlog */
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "EVENTS");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IN") == 1)
      {
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FROM") == 1)
      {
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      hparse_f_limit(TOKEN_KEYWORD_SHOW);
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_character_set() == 1) /* show character set */
    {
      hparse_f_like_or_where();
      if (hparse_errno > 0) return;
    }
    else if (hparse_errno > 0) return;
    else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CLIENT_STATISTICS") == 1))
    {
      ;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLLATION") == 1) /* show collation */
    {
      hparse_f_like_or_where();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLUMNS") == 1) /* show columns */
    {
      hparse_f_show_columns();
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONTRIBUTORS") == 1))
    {
      ;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COUNT") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_OPERATOR, "(");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "*");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ")");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ERRORS") == 1) ;
      else
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "WARNINGS");
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CREATE") == 1) /* show create ... */
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATABASE") == 1)
      {
        hparse_f_if_not_exists();
        if (hparse_errno > 0) return;
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EVENT") == 1)
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EXPLAIN") == 1))
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "FOR");
        if (hparse_errno > 0) return;
        hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FUNCTION") == 1)
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PROCEDURE") == 1)
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SCHEMA") == 1)
      {
        hparse_f_if_not_exists();
        if (hparse_errno > 0) return;
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLE") == 1)
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TRIGGER") == 1)
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USER") == 1)
      {
        if (hparse_f_user_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VIEW") == 1)
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DATABASES") == 1) /* show databases */
    {
      hparse_f_like_or_where();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENGINE") == 1) /* show engine */
    {
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STATUS") == 0)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "MUTEX");
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ENGINES") == 1) /* show engines */
    {
      ;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ERRORS") == 1) /* show errors */
    {
      hparse_f_limit(TOKEN_KEYWORD_SHOW);
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EVENTS") == 1) /* show events */
    {
      hparse_f_from_or_like_or_where();
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EXPLAIN") == 1))
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "FOR");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FIELDS") == 1) /* show columns */
    {
      hparse_f_show_columns();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FULL") == 1) /* show full [columns|tables|etc.] */
    {
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "COLUMNS") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FIELDS") == 1))
      {
        hparse_f_show_columns();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLES") == 1)
      {
        hparse_f_from_or_like_or_where();
        if (hparse_errno > 0) return;
      }
      else hparse_f_expect(TOKEN_TYPE_KEYWORD, "PROCESSLIST");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FUNCTION") == 1) /* show function [code] */
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CODE") == 1)
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STATUS") == 1)
      {
        hparse_f_like_or_where();
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GLOBAL") == 1) /* show global ... */
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STATUS") == 1)
      {
        hparse_f_like_or_where();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VARIABLES") == 1)
      {
        hparse_f_like_or_where();
        if (hparse_errno > 0) return;
      }
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GRANTS") == 1) /* show grants */
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOR") == 1)
      {
        if (hparse_f_user_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX") == 1) /* show index */
    {
      hparse_f_indexes_or_keys();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEXES") == 1) /* show indexes */
    {
      hparse_f_indexes_or_keys();
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "INDEX_STATISTICS") == 1))
    {
      ;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "KEYS") == 1) /* show keys */
    {
      hparse_f_indexes_or_keys();
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LOCALES") == 1))
    {
      ;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER") == 1) /* show master [status|logs\ */
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STATUS") == 0)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "LOGS");
      }
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "OPEN") == 1) /* show open [tables] */
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "TABLES");
      if (hparse_errno > 0) return;
      hparse_f_from_or_like_or_where();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PLUGINS") == 1) /* show plugins */
    {
      if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SONAME") == 1))
      {
        if (hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]") == 1) {;}
        else hparse_f_from_or_like_or_where();
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PRIVILEGES") == 1) /* show privileges */
    {
      ;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PROCEDURE") == 1) /* show procedure [code|status] */
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CODE") == 1)
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STATUS") == 1)
      {
        hparse_f_like_or_where();
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PROCESSLIST") == 1) /* show processlist */
    {
      ;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PROFILE") == 1) /* show profile */
    {
      for (;;)
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALL") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BLOCK") == 1)
        {
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "IO");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALL") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "BLOCK_IO") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONTEXT") == 1)
        {
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "SWITCHES");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CPU") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IPC") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MEMORY") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PAGE") == 1)
        {
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "FAULTS");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SOURCE") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SWAPS") == 1) {;}
        else break;
        if (hparse_f_accept(TOKEN_TYPE_OPERATOR, ",") == 1) continue;
        break;
      }
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOR") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "QUERY");
        if (hparse_errno > 0) return;
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      hparse_f_limit(TOKEN_KEYWORD_SHOW);
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PROFILES") == 1) /* show profiles */
    {
      ;
    }
    else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "QUERY_RESPONSE_TIME") == 1))
    {
      ;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RELAYLOG") == 1) /* show relaylog */
    {
      if (hparse_dbms == "mariadb") hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]");
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "EVENTS");
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IN") == 1)
      {
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FROM") == 1)
      {
        if (hparse_f_literal() == 0) hparse_f_error();
        if (hparse_errno > 0) return;
      }
      hparse_f_limit(TOKEN_KEYWORD_SHOW);
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SCHEMAS") == 1) /* show schemas */
    {
      hparse_f_like_or_where();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SESSION") == 1) /* show session ... */
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STATUS") == 1)
      {
        hparse_f_like_or_where();
        if (hparse_errno > 0) return;
      }
      else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VARIABLES") == 1)
      {
        hparse_f_like_or_where();
        if (hparse_errno > 0) return;
      }
      else hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SLAVE") == 1) /* show slave */
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "HOSTS") == 1) {;}
      else
      {
        if (hparse_dbms == "mariadb") hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]");
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "STATUS");
        if (hparse_errno > 0) return;
        if (hparse_dbms == "mysql")
        {
          if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NONBLOCKING") == 1) {;}
          hparse_f_for_channel();
          if (hparse_errno > 0) return;
        }
      }
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STATUS") == 1) /* show status */
    {
      hparse_f_like_or_where();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "STORAGE") == 1) /* show storage */
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "ENGINES");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLE") == 1) /* show table */
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "STATUS");
      if (hparse_errno > 0) return;
      hparse_f_from_or_like_or_where();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLES") == 1) /* show tables */
    {
      hparse_f_from_or_like_or_where();
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLE_STATISTICS") == 1))
    {
      ;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TRIGGERS") == 1) /* show triggers */
    {
      hparse_f_from_or_like_or_where();
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USER_STATISTICS") == 1))
    {
      ;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VARIABLES") == 1) /* show variables */
    {
      hparse_f_like_or_where();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WARNINGS") == 1) /* show warnings */
    {
      hparse_f_limit(TOKEN_KEYWORD_SHOW);
      if (hparse_errno > 0) return;
    }
    else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WSREP_MEMBERSHIP") == 1))
    {
      ;
    }
    else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WSREP_STATUS") == 1))
    {
      ;
    }
    else hparse_f_error();
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_SHUTDOWN, "SHUTDOWN"))
  {
    hparse_statement_type= TOKEN_KEYWORD_SHUTDOWN;
    return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_SIGNAL, "SIGNAL"))
  {
    hparse_statement_type= TOKEN_KEYWORD_SIGNAL;
    if (hparse_f_signal_or_resignal(TOKEN_KEYWORD_SIGNAL) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_KEYWORD_SONAME, "SONAME") == 1))
  {
    hparse_statement_type= TOKEN_KEYWORD_SONAME;
    if (hparse_f_literal() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_START, "START"))
  {
    hparse_statement_type= TOKEN_KEYWORD_START;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TRANSACTION") == 1)
    {
      if (hparse_errno > 0) return;
      bool with_seen= false, read_seen= false;
      do
      {
        if ((with_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WITH")))
        {
          with_seen= true;
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "CONSISTENT");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "SNAPSHOT");
          if (hparse_errno > 0) return;
        }
        if ((read_seen == false) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "READ")))
        {
          read_seen= true;
          if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ONLY") == 1) ;
          else hparse_f_expect(TOKEN_TYPE_KEYWORD, "WRITE");
          if (hparse_errno > 0) return;
        }
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GROUP_REPLICATION") == 1) {;}
    else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALL") == 1))
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "SLAVES");
      if (hparse_errno > 0) return;
      do
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IO_THREAD") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL_THREAD") == 1) {;}
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SLAVE") == 1)
    {
      if (hparse_dbms == "mariadb") hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]");
      do
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IO_THREAD") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL_THREAD") == 1) {;}
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNTIL") == 1)
      {
        if ((hparse_dbms == "mysql")
         && ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL_BEFORE_GTIDS") == 1)
          || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL_AFTER_GTIDS") == 1)
          || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL_AFTER_MTS_GAPS") == 1)))
        {
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_LOG_FILE") == 1)
        {
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_OPERATOR, ",");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "MASTER_LOG_POS");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
          if (hparse_errno > 0) return;
        }
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RELAY_LOG_FILE") == 1)
        {
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_OPERATOR, ",");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "RELAY_LOG_POS");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
          if (hparse_errno > 0) return;
        }
        else if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "MASTER_GTID_POS") == 1))
        {
          hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
          if (hparse_errno > 0) return;
        }
        else hparse_f_error();
        if (hparse_errno > 0) return;
      }
      if (hparse_dbms == "mysql")
      {
        for (;;)
        {
          bool expect_something= false;
          if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USER") == 1) expect_something= true;
          else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PASSWORD") == 1) expect_something= true;
          else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT_AUTH") == 1) expect_something= true;
          else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PLUGIN_DIR") == 1) expect_something= true;
          else break;
          if (hparse_errno > 0) return;
          if (expect_something)
          {
            hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
            if (hparse_errno > 0) return;
            hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
            if (hparse_errno > 0) return;
          }
        }
        hparse_f_for_channel();
        if (hparse_errno > 0) return;
      }
    }
    else hparse_f_error();
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_STOP, "STOP") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_STOP;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "GROUP_REPLICATION") == 1) {;}
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ALL") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "SLAVES");
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SLAVE") == 1)
    {
      if ((hparse_dbms == "mariadb") && (hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]") == 1)) {;}
      do
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "IO_THREAD") == 1) {;}
        else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQL_THREAD") == 1) {;}
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
      if (hparse_dbms == "mysql")
      {
        hparse_f_for_channel();
        if (hparse_errno > 0) return;
      }
    }
    else hparse_f_error();
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_TRUNCATE, "TRUNCATE"))
  {
    if (hparse_errno > 0) return;
    hparse_statement_type= TOKEN_KEYWORD_TRUNCATE;
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLE");
    if (hparse_f_qualified_name() == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_UNINSTALL, "UNINSTALL") == 1)
  {
    hparse_statement_type= TOKEN_KEYWORD_UNINSTALL;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PLUGIN") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    }
    else
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "SONAME");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return;
    }
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_UNLOCK, "UNLOCK"))
  {
    hparse_statement_type= TOKEN_KEYWORD_UNLOCK;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "TABLE") == 0) hparse_f_expect(TOKEN_TYPE_KEYWORD, "TABLES"); /* TABLE is undocumented */
    return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_UPDATE, "UPDATE"))
  {
    hparse_statement_type= TOKEN_KEYWORD_UPDATE;
    hparse_subquery_is_allowed= true;
    if (hparse_f_table_reference(0) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
    bool multi_seen= false;
    while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ",") == 1)
    {
      multi_seen= true;
      if (hparse_f_table_reference(0) == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "SET");
    if (hparse_errno > 0) return;
    hparse_f_assignment(TOKEN_KEYWORD_UPDATE);
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WHERE") == 1)
    {
      hparse_f_opr_1();
      if (hparse_errno > 0) return;
    }
    if (multi_seen == false)
    {
      hparse_f_order_by();
      if (hparse_errno > 0) return;
      hparse_f_limit(TOKEN_KEYWORD_UPDATE);
      if (hparse_errno > 0) return;
    }
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_XA, "XA"))
  {
    hparse_statement_type= TOKEN_KEYWORD_XA;
    if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "START") == 1) || (hparse_f_accept(TOKEN_KEYWORD_BEGIN_XA, "BEGIN") == 1))
    {
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "JOIN") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RESUME") == 1)) {;}
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "END") == 1)
    {
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SUSPEND") == 1)
      {
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "FOR") == 1)
        {
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "MIGRATE");
          if (hparse_errno > 0) return;
        }
      }
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "PREPARE") == 1)
    {
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "COMMIT") == 1)
    {
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ONE") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "PHASE");
        if (hparse_errno > 0) return;
      }
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ROLLBACK") == 1)
    {
      if (hparse_f_literal() == 0) hparse_f_error();
      if (hparse_errno > 0) return;
    }
    else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "RECOVER") == 1)
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONVERT") == 1)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "XID");
        if (hparse_errno > 0) return;
      }
    }
    else hparse_f_error();
  }
  else
  {
    hparse_f_error();
  }
}

/*
  compound statement, or statement
  Pass: calling_statement_type = 0 (top level) | TOKEN_KEYWORD_FUNCTION/PROCEDURE/EVENT/TRIGGER
*/
void MainWindow::hparse_f_block(int calling_statement_type)
{
  if (hparse_errno > 0) return;
  hparse_subquery_is_allowed= false;
  /*
    TODO:
      For labels + conditions + local variables, you could:
      push on stack when they come into scope
      pop from stack when they go out of scope
      check they're valid when you see reference
      show what they are when you see hover (requires showing where they're declared too)
      ... but currently we're saying any identifier will be okay
  */

  QString label= "";
  /* Label check. */
  /* Todo: most checks are illegal if preceded by a label. Check for that. */
  if (hparse_count_of_accepts != 0)
  {
    hparse_f_next_nexttoken();
    if (hparse_next_token == ":")
    {
      label= hparse_token;
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_OPERATOR, ":");
      if (hparse_errno > 0) return;
    }
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

  if ((next_is_semicolon_or_work == false) && (hparse_f_accept(TOKEN_KEYWORD_BEGIN, "BEGIN") == 1))
  {
    hparse_statement_type= TOKEN_KEYWORD_BEGIN;
    hparse_begin_seen= true;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NOT") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "ATOMIC");
      if (hparse_errno > 0) return;
    }
    else
    {
      /* The MariaDB parser cannot handle top-level BEGIN without NOT, so we don't either. */
      if (hparse_count_of_accepts < 2)
      {
        hparse_f_expect(TOKEN_TYPE_KEYWORD, "WORK"); /* impossible but enhances expected_list */
        hparse_f_expect(TOKEN_TYPE_OPERATOR, ";");   /* impossible but enhances expected_list */
        hparse_f_error();
        return;
      }
    }
    for (;;)                                                            /* DECLARE statements */
    {
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DECLARE") == 1)
      {
        if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONTINUE") == 1)
         || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "EXIT") == 1)
         || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNDO") == 1))
        {
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "HANDLER");
          if (hparse_errno > 0) return;
          hparse_f_expect(TOKEN_TYPE_KEYWORD, "FOR");
          if (hparse_errno > 0) return;
          do
          {
            if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQLSTATE") == 1)
            {
              hparse_f_accept(TOKEN_TYPE_KEYWORD, "VALUE");
              if (hparse_f_literal() == 0) hparse_f_error();
              if (hparse_errno > 0) return;
            }
            else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQLWARNING") == 1) {;}
            else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NOT") == 1)
            {
              hparse_f_expect(TOKEN_TYPE_KEYWORD, "FOUND");
              if (hparse_errno > 0) return;
            }
            else if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQLEXCEPTION") == 1) {;}
            else if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 1) {;}
            else
            {
              if (hparse_f_literal() == 0) hparse_f_error();
            }
            if (hparse_errno > 0) return;
          } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
          hparse_f_block(calling_statement_type);
          continue;
        }
        int identifier_count= 0;
        bool condition_seen= false;
        bool cursor_seen= false;
        do
        {
          hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
          if (hparse_errno > 0) return;
          ++identifier_count;
          if ((identifier_count == 1) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CONDITION") == 1))
          {
            hparse_f_expect(TOKEN_TYPE_KEYWORD, "FOR");
            if (hparse_errno > 0) return;
            if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SQLSTATE") == 1)
            {
              hparse_f_accept(TOKEN_TYPE_KEYWORD, "VALUE");
            }
            if (hparse_f_literal() == 0) hparse_f_error();
            if (hparse_errno > 0) return;
            condition_seen= true;
            break;
          }
          if ((identifier_count == 1) && (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CURSOR") == 1))
          {
            hparse_f_expect(TOKEN_TYPE_KEYWORD, "FOR");
            if (hparse_errno > 0) return;
            if (hparse_f_select(false) == 0)
            {
              hparse_f_error();
              return;
            }
            cursor_seen= true;
          }
        } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
        if (condition_seen == true) {;}
        else if (cursor_seen == true) {;}
        else
        {
          hparse_f_data_type();
          if (hparse_errno > 0) return;
          if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "DEFAULT") == 1)
          {
            if (hparse_f_literal() == 0) hparse_f_error(); /* todo: must it really be a literal? */
            if (hparse_errno > 0) return;
          }
        }
        hparse_f_expect(TOKEN_TYPE_OPERATOR, ";");
        if (hparse_errno > 0) return;
      }
      else break;
    }
    if (hparse_f_accept(TOKEN_KEYWORD_END, "END") == 0)
    {
      for (;;)
      {
        hparse_f_block(calling_statement_type);
        if (hparse_errno > 0) return;
        if (hparse_f_accept(TOKEN_KEYWORD_END, "END") == 1) break;
      }
    }
    hparse_f_accept(TOKEN_TYPE_IDENTIFIER, label);
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_CASE, "CASE") == 1)
  {
    int when_count= 0;
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WHEN") == 0)
     {
      hparse_f_opr_1(); /* not compulsory */
      if (hparse_errno > 0) return;
    }
    else when_count= 1;
    if (when_count == 0)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "WHEN");
      if (hparse_errno > 0) return;
    }
    for (;;)
    {
      hparse_subquery_is_allowed= true;
      hparse_f_opr_1();
      hparse_subquery_is_allowed= false;
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "THEN");
      if (hparse_errno > 0) return;
      int break_word= 0;
      for (;;)
      {
        hparse_f_block(calling_statement_type);
        if (hparse_errno > 0) return;
        if (hparse_f_accept(TOKEN_KEYWORD_END, "END") == 1) {break_word= TOKEN_KEYWORD_END; break; }
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "WHEN") == 1) {break_word= TOKEN_KEYWORD_WHEN; break; }
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ELSE") == 1) {break_word= TOKEN_KEYWORD_ELSE; break; }
      }
      if (break_word == TOKEN_KEYWORD_END) break;
      if (break_word == TOKEN_KEYWORD_WHEN) continue;
      assert(break_word == TOKEN_KEYWORD_ELSE);
      for (;;)
      {
        hparse_f_block(calling_statement_type);
        if (hparse_errno > 0) return;
        if (hparse_f_accept(TOKEN_KEYWORD_END, "END") == 1) break;
      }
      break;
    }
    hparse_f_expect(TOKEN_KEYWORD_CASE, "CASE");
    if (hparse_errno > 0) return;
    hparse_f_accept(TOKEN_TYPE_IDENTIFIER, label);
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_IF, "IF") == 1)
  {
    for (;;)
    {
      hparse_subquery_is_allowed= true;
      hparse_f_opr_1();
      hparse_subquery_is_allowed= false;
      if (hparse_errno > 0) return;
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "THEN");
      if (hparse_errno > 0) return;
      int break_word= 0;
      for (;;)
      {
        hparse_f_block(calling_statement_type);
        if (hparse_errno > 0) return;
        if (hparse_f_accept(TOKEN_KEYWORD_END, "END") == 1) {break_word= TOKEN_KEYWORD_END; break; }
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ELSEIF") == 1) {break_word= TOKEN_KEYWORD_ELSEIF; break; }
        if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "ELSE") == 1) {break_word= TOKEN_KEYWORD_ELSE; break; }
      }
      if (break_word == TOKEN_KEYWORD_END) break;
      if (break_word == TOKEN_KEYWORD_ELSEIF) continue;
      assert(break_word == TOKEN_KEYWORD_ELSE);
      for (;;)
      {
        hparse_f_block(calling_statement_type);
        if (hparse_errno > 0) return;
        if (hparse_f_accept(TOKEN_KEYWORD_END, "END") == 1) break;
      }
      break;
    }
    hparse_f_expect(TOKEN_KEYWORD_IF, "IF");
    if (hparse_errno > 0) return;
    hparse_f_accept(TOKEN_TYPE_IDENTIFIER, label);
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_LOOP, "LOOP") == 1)
  {
    for (;;)
    {
      hparse_f_block(calling_statement_type);
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_KEYWORD_END, "END") == 1) break;
    }
    hparse_f_expect(TOKEN_KEYWORD_LOOP, "LOOP");
    if (hparse_errno > 0) return;
    hparse_f_accept(TOKEN_TYPE_IDENTIFIER, label);
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_REPEAT, "REPEAT") == 1)
  {
    for (;;)
    {
      hparse_f_block(calling_statement_type);
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "UNTIL") == 1) break;
    }
    hparse_subquery_is_allowed= true;
    hparse_f_opr_1();
    hparse_subquery_is_allowed= false;
    hparse_f_expect(TOKEN_KEYWORD_END, "END");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_KEYWORD_REPEAT, "REPEAT");
    if (hparse_errno > 0) return;
    if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, label)) return;
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if ((hparse_f_accept(TOKEN_KEYWORD_ITERATE, "ITERATE") == 1) || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "LEAVE") == 1))
  {
    /* todo: ITERATE and LEAVE should cause an error if we've never seen BEGIN/LOOP/etc. */
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ";");
    if (hparse_errno > 0) return;
  }
  else if ((hparse_begin_seen == true) && (hparse_f_accept(TOKEN_KEYWORD_CLOSE, "CLOSE") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ";");
    if (hparse_errno > 0) return;
  }
  else if ((hparse_begin_seen == true) && (hparse_f_accept(TOKEN_KEYWORD_FETCH, "FETCH") == 1))
  {
    if (hparse_f_accept(TOKEN_TYPE_KEYWORD, "NEXT") == 1)
    {
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "FROM");
      if (hparse_errno > 0) return;
    }
    else hparse_f_accept(TOKEN_TYPE_KEYWORD, "FROM");
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    hparse_f_accept(TOKEN_TYPE_KEYWORD, "INTO");
    if (hparse_errno > 0) return;
    do
    {
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return;
    } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ";");
    if (hparse_errno > 0) return;
  }
  else if ((hparse_begin_seen == true) && (hparse_f_accept(TOKEN_KEYWORD_OPEN, "OPEN") == 1))
  {
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, ";");
    if (hparse_errno > 0) return;
  }
  else if ((calling_statement_type == TOKEN_KEYWORD_FUNCTION) && (hparse_f_accept(TOKEN_KEYWORD_RETURN, "RETURN") == 1))
  {
    hparse_subquery_is_allowed= true;
    hparse_f_opr_1();
    if (hparse_errno > 0) return;
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else if (hparse_f_accept(TOKEN_KEYWORD_WHILE, "WHILE") == 1)
  {
    hparse_subquery_is_allowed= true;
    hparse_f_opr_1();
    hparse_subquery_is_allowed= false;
    if (hparse_errno > 0) return;
    hparse_f_expect(TOKEN_TYPE_KEYWORD, "DO");
    if (hparse_errno > 0) return;
    for (;;)
    {
      hparse_f_block(calling_statement_type);
      if (hparse_errno > 0) return;
      if (hparse_f_accept(TOKEN_KEYWORD_END, "END") == 1) break;
    }
    hparse_f_expect(TOKEN_KEYWORD_WHILE, "WHILE");
    if (hparse_errno > 0) return;
    hparse_f_accept(TOKEN_TYPE_IDENTIFIER, label);
    if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    if (hparse_errno > 0) return;
  }
  else
  {
    hparse_f_statement();
    if (hparse_errno > 0) return;
    /* This kludge occurs more than once. */
    if ((hparse_prev_token != ";") && (hparse_prev_token != hparse_delimiter_str))
    {
      if (hparse_f_semicolon_and_or_delimiter(calling_statement_type) == 0) hparse_f_error();
    }
    if (hparse_errno > 0) return;
    return;
  }
}

/*
  This is the top. This should be the main entry for parsing.
  A user might put more than one statement, or block of statements,
  on the statement widget before asking for execution.
  Re hparse_dbms:
    We do check (though not always and not reliably) whether
    hparse_dbms == "mysql" | "mariadb" before accepting | expecting,
    for example "role" will only be recognized if hparse_dbms == "mariadb".
    Todo: we should check version number too, someday.
    If we are connected, then the SELECT VERSION() result, which we stored in
    statement_edit_widget->dbms_version, will include the string "MariaDB".
    If we are not connected, the default is "mysql" but the user can start with
    ocelotgui --dbms=mariadb, and we store that in ocelot_dbms.
*/
void MainWindow::hparse_f_multi_block(QString text)
{
  hparse_line_edit->hide();
  if (connections_is_connected[0] == 1)
  {
    if (statement_edit_widget->dbms_version.contains("mariadb", Qt::CaseInsensitive) == true)
    {
      hparse_dbms= "mariadb";
    }
    else hparse_dbms= "mysql";
  }
  else hparse_dbms= ocelot_dbms;
  hparse_i= -1;
  hparse_delimiter_str= ocelot_delimiter_str;
  for (;;)
  {
    hparse_offset_of_space_name= -1;
    hparse_statement_type= -1;
    hparse_number_of_literals= 0;
    hparse_indexed_condition_count= 0;
    hparse_errno= 0;
    hparse_expected= "";
    hparse_text_copy= text;
    hparse_begin_seen= false;
    hparse_like_seen= false;
    hparse_token_type= 0;
    hparse_next_token= "";
    hparse_next_next_token= "";
    hparse_next_token_type= 0;
    hparse_next_next_token_type= 0;
    hparse_prev_token= "";
    hparse_subquery_is_allowed= false;
    hparse_count_of_accepts= 0;
    if (hparse_i == -1) hparse_f_nexttoken();
    if (hparse_f_client_statement() == 1)
    {
      if (main_token_lengths[hparse_i] == 0) return; /* empty token marks end of input */
      if ((hparse_prev_token != ";") && (hparse_prev_token != hparse_delimiter_str))
      {
        hparse_f_semicolon_and_or_delimiter(0);
        if (hparse_errno > 0) goto error;
        if (main_token_lengths[hparse_i] == 0) return;
      }
      continue; /* ?? rather than "return"? */
    }
    if (hparse_errno > 0) goto error;
#ifdef DBMS_MARIADB
    if (hparse_dbms == "mariadb")
    {
      hparse_f_block(0);
    }
    else
#endif
    {
      hparse_f_statement();
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
    //hparse_f_expect(TOKEN_TYPE_OPERATOR, "[eof]");
    if (hparse_errno > 0) goto error;
    if (hparse_i > 0) main_token_flags[hparse_i - 1]= (main_token_flags[hparse_i - 1] | TOKEN_FLAG_IS_BLOCK_END);
    if (main_token_lengths[hparse_i] == 0) return; /* empty token marks end of input */
  }
  return;
error:
  QString expected_list;
  bool unfinished_comment_seen= false;
  bool unfinished_identifier_seen= false;
  if ((hparse_i == 0) && (main_token_lengths[0] == 0)) return;
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
      if (rest.contains("\n") == false) return;
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
      if (ocelot_auto_rehash > 0)
      {
        QString errmsg= hparse_errmsg;
        if (errmsg.contains("[identifier]") == true)
        {
          QString s;
          char search_string[512];
          int search_string_len;

          s= text.mid(main_token_offsets[hparse_i], main_token_lengths[hparse_i]);
          if (s.left(1) == "`") s= s.right(s.size() - 1);
          else if (s.left(1) == "\"") s= s.right(s.size() - 1);

          search_string_len= s.toUtf8().size();
          memcpy(search_string, s.toUtf8().constData(), search_string_len);
          search_string[search_string_len]= '\0';
          QString rehash_search_result= rehash_search(search_string);
          if (rehash_search_result > "")
          {
            expected_list= "Expected: ";
            expected_list.append(rehash_search_result);
            unfinished_identifier_seen= true;
          }
        }
      }
    }
  }
  if ((unfinished_comment_seen == false) && (unfinished_identifier_seen == false))
  {
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
          if (expected_list.contains(s_token, Qt::CaseInsensitive) == false)
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
  hparse_line_edit->setText(expected_list);
  hparse_line_edit->setCursorPosition(0);
  hparse_line_edit->show();
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
  else if (s == QString("h")) slash_token= TOKEN_KEYWORD_HELP;
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
    hparse_f_expect(slash_token, "\\"); /* Todo: mark as TOKEN_FLAG_END */
    if (hparse_errno > 0) return 0;
    hparse_f_expect(slash_token, s);
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
    hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]"); /* guaranteed to succeed */
    if (hparse_errno > 0) return;
  }
  else if (main_token_types[hparse_i] == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK)
  {
    hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]"); /* guaranteed to succeed */
    if (hparse_errno > 0) return;
  }
  else
  {
    if (main_token_lengths[hparse_i] == 0)
    {
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]"); /* guaranteed to fail */
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
        hparse_f_accept(TOKEN_TYPE_OPERATOR, ";");
        break;
      }
    }
    main_token_flags[hparse_i]= (main_token_flags[hparse_i] & (~TOKEN_FLAG_IS_RESERVED));
    main_token_types[hparse_i]= hparse_token_type= TOKEN_TYPE_LITERAL;
    //if (main_token_lengths[hparse_i + 1] == 0)
    //{
    //  break;
    //}
    bool line_break_seen= false;
    for (int i_off= main_token_offsets[hparse_i] + main_token_lengths[hparse_i];; ++i_off)
    {
      if (i_off >= main_token_offsets[hparse_i + 1]) break;
      QString q= hparse_text_copy.mid(i_off, 1);
      if ((q == "\n") || (q == "\r"))
      {
        line_break_seen= true;
        break;
      }
    }
    if (line_break_seen == true)
    {
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]"); /* guaranteed to succeed */
      if (hparse_errno > 0) return;
      break;
    }

    //if (main_token_offsets[hparse_i] + main_token_lengths[hparse_i]
    //   < main_token_offsets[hparse_i + 1])
    //{
    //  break;
    //}
    hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]"); /* guaranteed to succeed */
    if (hparse_errno > 0) return;
  }
}

/*
  Statements handled locally (by ocelotgui), which won't go to the server.
  Todo: we're ignoring --binary-mode.
  Todo: we're only parsing the first word to see if it's client-side, we could do more.
  SET is a special problem because it can be either client or server (flaw in our design?).
  Within client statements, reserved words don't count so we turn the reserved flag off.
  Todo: Figure out how HELP can be both client statement and server statement.
*/
int MainWindow::hparse_f_client_statement()
{
  hparse_next_token= hparse_next_next_token= "";
  int saved_hparse_i= hparse_i;
  int saved_hparse_token_type= hparse_token_type;
  QString saved_hparse_token= hparse_token;
  int slash_token= hparse_f_backslash_command(true);
  if (hparse_errno > 0) return 0;
  if ((slash_token == TOKEN_KEYWORD_QUESTIONMARK) || (hparse_f_accept(TOKEN_KEYWORD_QUESTIONMARK, "?") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_CHARSET) || (hparse_f_accept(TOKEN_KEYWORD_CHARSET, "CHARSET") == 1))
  {
    main_token_flags[hparse_i]= (main_token_flags[hparse_i] & (~TOKEN_FLAG_IS_RESERVED));
    if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
    {
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return 0;
    }
  }
  else if ((slash_token == TOKEN_KEYWORD_CLEAR) || (hparse_f_accept(TOKEN_KEYWORD_CLEAR, "CLEAR") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_CONNECT) || (hparse_f_accept(TOKEN_KEYWORD_CONNECT, "CONNECT") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_DELIMITER) || (hparse_f_accept(TOKEN_KEYWORD_DELIMITER, "DELIMITER") == 1))
  {
    QString tmp_delimiter= get_delimiter(hparse_token, hparse_text_copy, main_token_offsets[hparse_i]);
    if (tmp_delimiter > " ")
    {
      hparse_delimiter_str= ";";
      hparse_f_other(1);
      hparse_delimiter_str= tmp_delimiter;
      /* Redo tokenize because if delimiter changes then token ends change. */
      if ((main_token_lengths[hparse_i] != 0) && (main_token_offsets[hparse_i] != 0))
      {
        int offset_of_rest= main_token_offsets[hparse_i];
        tokenize(hparse_text_copy.data() + offset_of_rest,
                 hparse_text_copy.size() - offset_of_rest,
                 &main_token_lengths[hparse_i],
                 &main_token_offsets[hparse_i],
                 MAX_TOKENS - (hparse_i + 1),
                 (QChar*)"33333",
                 1,
                 hparse_delimiter_str,
                 1);
        for (int ix= hparse_i; main_token_lengths[ix] != 0; ++ix)
        {
          main_token_offsets[ix]+= offset_of_rest;
        }
        tokens_to_keywords(hparse_text_copy, hparse_i);
      }
    }
    else hparse_f_other(1);
    if (hparse_errno > 0) return 0;
  }
  else if ((slash_token == TOKEN_KEYWORD_EDIT) || (hparse_f_accept(TOKEN_KEYWORD_EDIT, "EDIT") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_EGO) || (hparse_f_accept(TOKEN_KEYWORD_EGO, "EGO") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_EXIT) || (hparse_f_accept(TOKEN_KEYWORD_EXIT, "EXIT") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_GO) || (hparse_f_accept(TOKEN_KEYWORD_GO, "GO") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_HELP) || (hparse_f_accept(TOKEN_KEYWORD_HELP, "HELP") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_NOPAGER) || (hparse_f_accept(TOKEN_KEYWORD_NOPAGER, "NOPAGER") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_NOTEE) || (hparse_f_accept(TOKEN_KEYWORD_NOTEE, "NOTEE") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_NOWARNING) || (hparse_f_accept(TOKEN_KEYWORD_NOWARNING, "NOWARNING") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_PAGER) || (hparse_f_accept(TOKEN_KEYWORD_PAGER, "PAGER") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_PRINT) || (hparse_f_accept(TOKEN_KEYWORD_PRINT, "PRINT") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_PROMPT) || (hparse_f_accept(TOKEN_KEYWORD_PROMPT, "PROMPT")== 1))
  {
    /* Apparently PROMPT can be followed by any bunch of junk as far as ; or delimiter or eof */
    for (;;)
    {
      if ((main_token_lengths[hparse_i] == 0)
       || (hparse_token == ";")
       || (hparse_token == hparse_delimiter_str)) break;
      main_token_flags[hparse_i]= (main_token_flags[hparse_i] & (~TOKEN_FLAG_IS_RESERVED));
      main_token_types[hparse_i]= TOKEN_TYPE_OTHER;
      hparse_f_nexttoken();
    }
  }
  else if ((slash_token == TOKEN_KEYWORD_QUIT) || (hparse_f_accept(TOKEN_KEYWORD_QUIT, "QUIT") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_REHASH) || (hparse_f_accept(TOKEN_KEYWORD_REHASH, "REHASH") == 1)) {;}
  //else if ((slash_token == TOKEN_KEYWORD_RESETCONNECTION) || (hparse_f_accept_client(TOKEN_KEYWORD_RESETCONNECTION, "RESETCONNECTION") == 1)) {;}
  else if (hparse_f_accept(TOKEN_KEYWORD_SET, "SET") == 1)
  {
    if (main_token_lengths[hparse_i] != 0)
    {
      QString s= hparse_token.mid(0, 7);
      if  (QString::compare(s, "OCELOT_", Qt::CaseInsensitive) != 0)
      {
        hparse_i= saved_hparse_i;
        hparse_token_type= saved_hparse_token_type;
        hparse_token= saved_hparse_token;
        return 0;
      }
    }
    if ((hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_TEXT_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_BACKGROUND_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_BORDER_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_FONT_FAMILY") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_FONT_SIZE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_FONT_STYLE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_FONT_WEIGHT") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_HIGHLIGHT_LITERAL_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_HIGHLIGHT_IDENTIFIER_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_HIGHLIGHT_COMMENT_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_HIGHLIGHT_OPERATOR_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_highlight_keyword_color") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_HIGHLIGHT_PROMPT_BACKGROUND_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_HIGHLIGHT_CURRENT_LINE_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_STATEMENT_SYNTAX_CHECKER") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_GRID_BACKGROUND_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_GRID_HEADER_BACKGROUND_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_GRID_FONT_FAMILY") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_GRID_FONT_SIZE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_GRID_FONT_STYLE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_GRID_FONT_WEIGHT") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_GRID_CELL_BORDER_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_GRID_CELL_DRAG_LINE_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_GRID_BORDER_SIZE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_GRID_CELL_BORDER_SIZE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_GRID_CELL_DRAG_LINE_SIZE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_EXTRA_RULE_1_TEXT_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_EXTRA_RULE_1_BACKGROUND_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_EXTRA_RULE_1_CONDITION") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_EXTRA_RULE_1_DISPLAY_AS") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_HISTORY_TEXT_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_HISTORY_BACKGROUND_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_HISTORY_BORDER_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_HISTORY_FONT_FAMILY") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_HISTORY_FONT_SIZE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_HISTORY_FONT_STYLE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_HISTORY_FONT_WEIGHT") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_MENU_TEXT_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_MENU_BACKGROUND_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_MENU_BORDER_COLOR") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_FONT_FAMILY") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_MENU_FONT_SIZE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_MENU_FONT_STYLE") == 1)
     || (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "OCELOT_MENU_FONT_WEIGHT") == 1))
    {
      ;
    }
    else hparse_f_error();
    if (hparse_errno > 0) return 0;
    hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
    if (hparse_errno > 0) return 0;
    main_token_flags[hparse_i]= (main_token_flags[hparse_i] & (~TOKEN_FLAG_IS_RESERVED));
    hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
    if (hparse_errno > 0) return 0;
  }
  else if ((slash_token == TOKEN_KEYWORD_SOURCE) || (hparse_f_accept(TOKEN_KEYWORD_SOURCE, "SOURCE") == 1))
  {
    hparse_f_other(0);
    if (hparse_errno > 0) return 0;
  }
  else if ((slash_token == TOKEN_KEYWORD_STATUS) || (hparse_f_accept(TOKEN_KEYWORD_STATUS, "STATUS") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_SYSTEM) || (hparse_f_accept(TOKEN_KEYWORD_SYSTEM, "SYSTEM") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_TEE) || (hparse_f_accept(TOKEN_KEYWORD_TEE, "TEE") == 1)) {;}
  else if ((slash_token == TOKEN_KEYWORD_USE) || (hparse_f_accept(TOKEN_KEYWORD_USE, "USE") == 1))
  {
    if (hparse_f_accept(TOKEN_TYPE_IDENTIFIER, "[identifier]") == 0)
    {
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
    }
    /* In mysql client, garbage can follow. It's not documented so don't call hparse_f_other(). */
    if (hparse_errno > 0) return 0;
  }
  else if ((slash_token == TOKEN_KEYWORD_WARNINGS) || (hparse_f_accept(TOKEN_KEYWORD_WARNINGS, "WARNINGS")) == 1) {;}
  else if (hparse_token.mid(0, 1) == "$")
  {
    /* TODO: We aren't parsing $debug statements well */
    if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_BREAKPOINT, "$BREAKPOINT", 2) == 1)
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return 0;
      if ((hparse_token.length() == 0) || (hparse_token == ";")) return 1;
      hparse_f_opr_1();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_CLEAR, "$CLEAR", 3) == 1)
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_CONTINUE, "$CONTINUE", 3) == 1)
    {
      ;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_DEBUG, "$DEBUG", 4) == 1)
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      if (hparse_f_accept(TOKEN_TYPE_OPERATOR, "(") == 1)
      {
        if (hparse_token != ")")
        {
          do
          {
            if (hparse_f_accept(TOKEN_TYPE_LITERAL, "[literal]") == 0)
            {
              hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
            }
            if (hparse_errno > 0) return 0;
          } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
        }
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
      hparse_f_expect(TOKEN_TYPE_KEYWORD, "STATUS");
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
      if ((hparse_f_accept(TOKEN_TYPE_KEYWORD, "BREAKPOINTS") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "CALL_STACK") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "SERVER_VARIABLES") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "USER_VARIABLES") == 1)
       || (hparse_f_accept(TOKEN_TYPE_KEYWORD, "VARIABLES") == 1))
        {;}
      else hparse_f_error();
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_SET, "$SET", 4) == 1)
    {
      hparse_f_expect(TOKEN_TYPE_IDENTIFIER, "[identifier]");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(TOKEN_TYPE_OPERATOR, "=");
      if (hparse_errno > 0) return 0;
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return 0;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_SETUP, "$SETUP", 5) == 1)
    {
      do
      {
        if (hparse_f_qualified_name() == 0) hparse_f_error();
        if (hparse_errno > 0) return 0;
      } while (hparse_f_accept(TOKEN_TYPE_OPERATOR, ","));
    }
     //|| (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_SKIP, "$SKIP") == 1)
     //|| (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_SOURCE, "$SOURCE") == 1)
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_STEP, "$STEP", 3) == 1)
    {
      ;
    }
    else if (hparse_f_acceptn(TOKEN_KEYWORD_DEBUG_TBREAKPOINT, "$TBREAKPOINT", 2) == 1)
    {
      if (hparse_f_qualified_name() == 0) hparse_f_error();
      if (hparse_errno > 0) return 0;
      hparse_f_expect(TOKEN_TYPE_LITERAL, "[literal]");
      if (hparse_errno > 0) return 0;
      if ((hparse_token.length() == 0) || (hparse_token == ";")) return 1;
      hparse_f_opr_1();
      if (hparse_errno > 0) return 0;
    }
    else return 0;
    return 1;
  }
  else
  {
    return 0;
  }
  return 1;
}

/*
  The hint line that appears underneath the statement widget if syntax error,
  which probably means that the user is typing and hasn't finished a word.
  This is somewhat like a popup but using "Qt::Popup" caused trouble.
*/
void MainWindow::hparse_f_parse_hint_line_create()
{
  hparse_line_edit= new QLineEdit(this);
  hparse_line_edit->setReadOnly(true);
  hparse_line_edit->hide();
}

#ifdef DBMS_TARANTOOL
/*
  Although this is currently within "#ifdef DBMS_TARANTOOL",
  it probably is useful elsewhere too.
*/

/*
  Syntax check (!! THIS COMMENT MAY BE OBSOLETE!)
  Go through all the tokens, comparing what we expect to what we got.
  If any comparison fails, the error message will say:
  what we expected, token number + offset + value for the token where comparison failed.
  Allowed syntaxes are:
  DELETE FROM identifier WHERE identifier = literal [AND identifier = literal ...];
  INSERT INTO identifier VALUES (literal [, literal...]);
  REPLACE INTO identifier VALUES (literal [, literal...]);
  SELECT * FROM identifier [WHERE identifier <comparison-operator> literal [AND identifier <comparison-operator> literal ...]];
  UPDATE identifier SET identifier=value [, identifier=value...]
                    WHERE identifier = literal [AND identifier = literal ...];
  SET identifier = expression [, identifier = expression ...]
  Legal comparison-operators within SELECT are = > < >= <=
  Comments are legal anywhere.
  Keywords are usually not reserved, for example DELETE FROM INTO WHERE SELECT=5; is legal.
*/

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

/* TODO: THESE MUST BE INITIALIZED IN parse_f_program! */
QString text_copy;
QString parse_symbol;
int parse_i;
QString parse_expected;
int parse_errno;
int parse_symbol_type;
int offset_of_space_name= -1;
int statement_type= -1;
int number_of_literals= 0;
int iterator_type= TARANTOOL_BOX_INDEX_EQ;
int parse_indexed_condition_count= 0;

void MainWindow::parse_f_nextsym()
{
  if (parse_errno > 0) return;
  for (;;)
  {
    ++parse_i;
    parse_symbol_type= main_token_types[parse_i];
    if ((parse_symbol_type != TOKEN_TYPE_COMMENT_WITH_SLASH)
     && (parse_symbol_type != TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
     && (parse_symbol_type != TOKEN_TYPE_COMMENT_WITH_MINUS))
      break;
  }
  parse_symbol= text_copy.mid(main_token_offsets[parse_i], main_token_lengths[parse_i]);
}

void MainWindow::parse_f_error()
{
  if (parse_errno > 0) return;
  QString q_errormsg= "Parse error. Got: ";
  if (parse_symbol.length() > 10)
  {
    q_errormsg.append(parse_symbol.left(10));
    q_errormsg.append("...");
  }
  else q_errormsg.append(parse_symbol);
  q_errormsg.append("  (token #");
  q_errormsg.append(QString::number(parse_i + 1));
  q_errormsg.append(", offset ");
  q_errormsg.append(QString::number(main_token_offsets[parse_i] + 1));
  q_errormsg.append(") ");
  q_errormsg.append(". Expected: ");
  q_errormsg.append(parse_expected);
  while ((unsigned) q_errormsg.toUtf8().length() >= (unsigned int) sizeof(tarantool_errmsg))
    q_errormsg= q_errormsg.left(q_errormsg.length() - 1);
  strcpy(tarantool_errmsg, q_errormsg.toUtf8());
  parse_errno= 10400;
  tarantool_errno= 10400;
}

/*
  accept means: if current == expected then clear list of what was expected, get next, and return 1,
                else add to list of what was expected, and return 0
*/
int MainWindow::parse_f_accept(QString token)
{
  if (parse_errno > 0) return 0;
  bool equality= false;
  if (token == "[identifier]")
  {
    if ((parse_symbol_type == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK)
     || (parse_symbol_type == TOKEN_TYPE_IDENTIFIER_WITH_AT)
     || (parse_symbol_type == TOKEN_TYPE_OTHER)
     || (parse_symbol_type >= TOKEN_KEYWORDS_START))
    {
      equality= true;
      if (offset_of_space_name == -1) offset_of_space_name= parse_i;
    }
  }
  else if (token == "[literal]")
  {
    if ((parse_symbol_type == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
     || (parse_symbol_type == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE)
     || (parse_symbol_type == TOKEN_TYPE_LITERAL_WITH_DIGIT)
     || (parse_symbol_type == TOKEN_TYPE_LITERAL_WITH_BRACE))
    {
      equality= true;
      ++number_of_literals;
    }
  }
  else if (token == "[eof]")
  {
    if (parse_symbol.length() == 0)
    {
      equality=true;
    }
  }
  else if (token == "[field identifier]")
  {
    int base_size= strlen(TARANTOOL_FIELD_NAME_BASE);
    bool ok= false;
    int field_integer= 0;
    int field_integer_length= parse_symbol.length() - (base_size + 1);
    if (field_integer_length > 0) field_integer= parse_symbol.right(field_integer_length).toInt(&ok);
    printf("base_size=%d. field_integer_length=%d. field_integer=%d.\n",
           base_size, field_integer_length, field_integer);
    if ((parse_symbol.left(base_size) == TARANTOOL_FIELD_NAME_BASE)
     && (parse_symbol.mid(base_size, 1) == "_")
     && (field_integer > 0)
     && (ok == true)
     && (parse_symbol.length() < TARANTOOL_MAX_FIELD_NAME_LENGTH))
    {
      equality= true;
    }
  }
  else if (QString::compare(parse_symbol, token, Qt::CaseInsensitive) == 0)
  {
    equality= true;
  }

  if (equality == true)
  {
    parse_expected= "";
    parse_f_nextsym();
    return 1;
  }
  if (parse_expected > "") parse_expected.append(" or ");
  parse_expected.append(token);
  return 0;
}

/* expect means: if current == expected then get next and return 1; else error */
int MainWindow::parse_f_expect(QString token)
{
  if (parse_errno > 0) return 0;
  if (parse_f_accept(token)) return 1;
  parse_f_error();
  return 0;
}

/*
 factor = identifier | literal | "(" expression ")" .
*/
void MainWindow::parse_f_factor()
{
  if (parse_errno > 0) return;
  if (parse_f_accept("[identifier]"))
  {
    if (parse_errno > 0) return;
  }
  else if (parse_f_accept("[literal]"))
  {
    if (parse_errno > 0) return;
  }
  else if (parse_f_accept("("))
  {
    if (parse_errno > 0) return;
    parse_f_expression();
    if (parse_errno > 0) return;
    parse_f_expect(")");
    if (parse_errno > 0) return;
  }
  else
  {
    parse_f_error();
    return;
  }
}

/*
  term = factor {("*"|"/") factor}
*/
void MainWindow::parse_f_term()
{
  if (parse_errno > 0) return;
  parse_f_factor();
  if (parse_errno > 0) return;
  while ((parse_f_accept("*") == 1) || (parse_f_accept("/") == 1))
  {
    parse_f_factor();
    if (parse_errno > 0) return;
  }
}

/*
   expression = ["+"|"-"] term {("+"|"-") term}
*/
void MainWindow::parse_f_expression()
{
  if (parse_errno > 0) return;
  if ((parse_f_accept("+") == 1) || (parse_f_accept("-") == 1)) {;}
  parse_f_term();
  if (parse_errno > 0) return;
  while ((parse_f_accept("+") == 1) || (parse_f_accept("-") == 1))
  {
    parse_f_term();
    if (parse_errno > 0) return;
  }
}

/*
  restricted expression = ["+"|"-"] literal
*/
void MainWindow::parse_f_restricted_expression()
{
  if (parse_errno > 0) return;
  if ((parse_f_accept("+") == 1) || (parse_f_accept("-") == 1)) {;}
  parse_f_expect("[literal]");
}

/*
 condition =
     identifier ("="|"<"|"<="|">"|">=") literal
     [AND condition ...]
*/
void MainWindow::parse_f_indexed_condition()
{
  if (parse_errno > 0) return;
  do
  {
    if (parse_indexed_condition_count >= 255)
    {
      parse_expected= "no more conditions. The maximum is 255 (box.schema.INDEX_PART_MAX).";
      parse_f_error();
      return;
    }
    int comp_op= -1;
    if (parse_errno > 0) return;
    if (parse_f_accept("[field identifier]") == 0)
    {
      parse_expected= "field identifier with the format: ";
      parse_expected.append(TARANTOOL_FIELD_NAME_BASE);
      parse_expected.append("_ followed by an integer greater than zero. ");
      parse_expected.append("Maximum length = 64. ");
      parse_expected.append(QString::number(TARANTOOL_MAX_FIELD_NAME_LENGTH));
      parse_expected.append(". For example: ");
      parse_expected.append(TARANTOOL_FIELD_NAME_BASE);
      parse_expected.append("_1");
      parse_f_error();
      return;
    }

    if (parse_indexed_condition_count > 0)
    {
      int ok= 0;
      if ((parse_symbol == "<")
       && (iterator_type == TARANTOOL_BOX_INDEX_LE)
       && (parse_f_accept("<") == 1)) {;}
      else if ((parse_symbol == ">")
            && (iterator_type == TARANTOOL_BOX_INDEX_GE)
            && (parse_f_accept(">") == 1)) ++ok;
      else if ((parse_symbol == "=")
            && (iterator_type == TARANTOOL_BOX_INDEX_EQ)
            && (parse_f_accept("=") == 1)) ++ok;
      else if ((parse_symbol == "<=")
            && (iterator_type == TARANTOOL_BOX_INDEX_LE)
            && (parse_f_accept("<=") == 1)) ++ok;
      else if ((parse_symbol == ">=")
            && (iterator_type == TARANTOOL_BOX_INDEX_GE)
            && (parse_f_accept(">=") == 1)) ++ok;
      printf("iterator_type= %d, ok=%d.\n", iterator_type, ok);
      if (ok == 0)
      {
        parse_expected= "A conditional operator compatible with previous ones. ";
        parse_expected.append("When there is more than one ANDed condition, ");
        parse_expected.append("allowed combinations are: > after a series of >=s, ");
        parse_expected.append("or < after a series of <=s, or all =s, or all >=s, or all <=s.");
        parse_f_error();
        return;
      }
    }
    else
    {
      if (parse_f_accept("=") == 1) comp_op= TARANTOOL_BOX_INDEX_EQ;
      else if (parse_f_accept("<") == 1) comp_op= TARANTOOL_BOX_INDEX_LT;
      else if (parse_f_accept("<=") == 1) comp_op= TARANTOOL_BOX_INDEX_LE;
      else if (parse_f_accept(">") == 1) comp_op= TARANTOOL_BOX_INDEX_GT;
      else if (parse_f_accept(">=") == 1) comp_op= TARANTOOL_BOX_INDEX_GE;
      else parse_f_error();
      if (parse_errno > 0) return;
    }
    iterator_type= comp_op;
    ++parse_indexed_condition_count;
    parse_f_expect("[literal]");
    if (parse_errno > 0) return;
  } while (parse_f_accept("AND"));
}


/*
 unindexed condition =
     expression ("="|"<"|"<="|">"|">=") expression
     [AND|OR condition ...]
  For a sequential search, i.e. a full-table scan or a filter of the
  rows selected by indexed conditions, we can have OR as well as AND,
  expressions as well as identifiers, expressions as well as literals,
  and <> as well as other comp-ops.
  May implement for a HAVING clause somday.
*/
//void MainWindow::parse_f_unindexed_condition()
//{
//  if (parse_errno >0) return;
//  do
//  {
//    if (parse_errno > 0) return;
//    parse_f_expression();
//    if (parse_errno > 0) return;
//    /* TODO: THIS IS NOWHERE NEAR CORRECT! THERE MIGHT BE MORE THAN ONE OPERAND! */
//    {
//      if ((parse_symbol == "=")
//      || (parse_symbol == "<>")
//      || (parse_symbol == "<")
//      || (parse_symbol == "<=")
//      || (parse_symbol == ">")
//      || (parse_symbol == ">="))
//      {
//        if (parse_symbol == "=") iterator_type= TARANTOOL_BOX_INDEX_EQ;
//        if (parse_symbol == "<>") iterator_type= TARANTOOL_BOX_INDEX_ALL; /* TODO: NO SUPPORT */
//        if (parse_symbol == "<") iterator_type= TARANTOOL_BOX_INDEX_LT;
//        if (parse_symbol == "<=") iterator_type= TARANTOOL_BOX_INDEX_LE;
//        if (parse_symbol == ">") iterator_type= TARANTOOL_BOX_INDEX_GT;
//        if (parse_symbol == ">=") iterator_type= TARANTOOL_BOX_INDEX_GE;
//        parse_f_nextsym();
//        parse_f_expression();
//      }
//      else parse_f_error();
//    }
//  } while (parse_f_accept("AND"));
//}


void MainWindow::parse_f_assignment()
{
  do
  {
    if (parse_errno > 0) return;
    parse_f_expect("[identifier]");
    if (parse_errno > 0) return;
    parse_f_expect("=");
    if (parse_errno > 0) return;
    parse_f_expression();
    if (parse_errno > 0) return;
    } while (parse_f_accept(","));
}

/*
statement =
    "insert" [into] ident "values" (literal {"," literal} )
    | "replace" [into] ident "values" (number {"," literal} )
    | "delete" "from" ident "where" condition [AND condition ...]
    | "select" * "from" ident "where" condition [AND condition ...]
    | "set" ident = number [, ident = expression ...]
    | "truncate" "table" ident
    | "update" ident "set" ident=literal {"," ident=literal} WHERE condition [AND condition ...]
*/
void MainWindow::parse_f_statement()
{
  if (parse_errno > 0) return;
  if (parse_f_accept("INSERT"))
  {
    if (parse_errno > 0) return;
    statement_type= TOKEN_KEYWORD_INSERT;
    parse_f_accept("INTO");
    if (parse_errno > 0) return;
    parse_f_expect("[identifier]");
    if (parse_errno > 0) return;
    parse_f_expect("VALUES");
    if (parse_errno > 0) return;
    parse_f_expect("(");
    do
    {
      if (parse_errno > 0) return;
      parse_f_restricted_expression();
      if (parse_errno > 0) return;
    } while (parse_f_accept(","));
    parse_f_expect(")");
    if (parse_errno > 0) return;
  }
  else if (parse_f_accept("REPLACE"))
  {
    if (parse_errno > 0) return;
    statement_type= TOKEN_KEYWORD_REPLACE;
    parse_f_accept("INTO");
    if (parse_errno > 0) return;
    parse_f_expect("INTO");
    if (parse_errno > 0) return;
    parse_f_expect("VALUES");
    if (parse_errno > 0) return;
    parse_f_expect("(");
    do
    {
      if (parse_errno > 0) return;
      parse_f_expect("[literal]");
      if (parse_errno > 0) return;
    } while (parse_f_accept(","));
    parse_f_expect("(");
    if (parse_errno > 0) return;
  }
  else if (parse_f_accept("DELETE"))
  {
    if (parse_errno > 0) return;
    statement_type= TOKEN_KEYWORD_DELETE;
    parse_f_expect("FROM");
    if (parse_errno > 0) return;
    parse_f_expect("[identifier]");
    if (parse_errno > 0) return;
    parse_f_expect("WHERE");
    if (parse_errno > 0) return;
    parse_f_indexed_condition();
    if (parse_errno > 0) return;
  }
  else if (parse_f_accept("TRUNCATE"))
  {
    if (parse_errno > 0) return;
    statement_type= TOKEN_KEYWORD_TRUNCATE;
    parse_f_expect("TABLE");
    if (parse_errno > 0) return;
    parse_f_expect("[identifier]");
    if (parse_errno > 0) return;
  }
  else if (parse_f_accept("SELECT"))
  {
    if (parse_errno > 0) return;
    statement_type= TOKEN_KEYWORD_SELECT;
    parse_f_expect("*");
    if (parse_errno > 0) return;
    parse_f_expect("FROM");
    if (parse_errno > 0) return;
    parse_f_expect("[identifier]");
    if (parse_errno > 0) return;
    parse_f_expect("WHERE");
    if (parse_errno > 0) return;
    parse_f_indexed_condition();
    if (parse_errno > 0) return;
  }
  else if (parse_f_accept("UPDATE"))
  {
    statement_type= TOKEN_KEYWORD_UPDATE;
    parse_f_expect("[identifier]");
    if (parse_errno > 0) return;
    parse_f_expect("SET");
    if (parse_errno > 0) return;
    parse_f_assignment();
    if (parse_errno > 0) return;
    parse_f_expect("WHERE");
    if (parse_errno > 0) return;
    parse_f_indexed_condition();
    if (parse_errno > 0) return;
  }
  else if (parse_f_accept("SET"))
  {
    statement_type= TOKEN_KEYWORD_SET;
    parse_f_assignment();
    if (parse_errno > 0) return;
  }
  else
  {
    parse_f_error();
  }
}

/*
  statement
*/
void MainWindow::parse_f_block()
{
  if (parse_errno > 0) return;
  parse_f_statement();
}

void MainWindow::parse_f_program(QString text)
{
  tarantool_errno= 0;
  parse_errno= 0;
  parse_expected= "";
  text_copy= text;
  parse_i= -1;
  parse_f_nextsym();
  parse_f_block();
  if (parse_errno > 0) return;
  /* If you've got a bloody semicolon that's okay too */
  if (parse_f_expect(";")) return;
  parse_f_expect("[eof]"); /* was: parse_expect(TOKEN_KEYWORD_PERIOD); */
  if (parse_errno > 0) return;
}
#endif

#ifdef DBMS_TARANTOOL

/*
  Todo: disconnect old if already connected.
  TODO: LOTS OF ERROR CHECKS NEEDED IN THIS!
*/
/*
  Usually libtarantool.so and libtarantoolnet.so are in /usr/local/lib or LD_LIBRARY_PATH.
*/
int MainWindow::connect_tarantool(unsigned int connection_number)
{
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
  if (is_libtarantoolnet_loaded != 1)
  {
    lmysql->ldbms_get_library(ocelot_ld_run_path, &is_libtarantoolnet_loaded, &libtarantoolnet_handle, &ldbms_return_string, WHICH_LIBRARY_LIBTARANTOOLNET);
  }
  if (is_libtarantool_loaded != 1)
  {
    lmysql->ldbms_get_library("", &is_libtarantoolnet_loaded, &libtarantoolnet_handle, &ldbms_return_string, WHICH_LIBRARY_LIBTARANTOOLNET);
  }

  /* Todo: The following errors would be better if we put them in diagnostics the usual way. */

  if (is_libtarantool_loaded == -2)
  {
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
    QMessageBox msgbox;
    QString error_message;
    error_message= "Error, libtarantool was not found or a loading error occurred. Message was: ";
    error_message.append(ldbms_return_string);
    msgbox.setText(error_message);
    msgbox.exec();
    delete lmysql;
    return 1;
  }

  /* !! todo: same checking for tarantoolnet.so */

  /* TEST! this is from the sample program in the manual */

  /* Todo: this should have been done already; we must be calling from the wrong place. */
  copy_connect_strings_to_utf8();

  /* CONNECT. URI = port, host:port, or username:password@host:port */
  char connection_string[1024];
  char connection_port_as_utf8[128];
  strcpy(connection_string, "");

  /* TODO
  > I assume that after connect() succeeds I'm user = guest,
  > and after authenticate() succeeds I'm the user specified
  > in the arguments.
  Yes.
  */

  if (strcmp(ocelot_user_as_utf8, "guest") != 0)
  {
    strcat(connection_string, ocelot_user_as_utf8);
    strcat(connection_string, ":");
    strcat(connection_string, ocelot_password_as_utf8);
    strcat(connection_string, "@");
  }
  strcat(connection_string, ocelot_host_as_utf8);
  strcat(connection_string, ":");
  sprintf(connection_port_as_utf8, "%d", ocelot_port);
  strcat(connection_string, connection_port_as_utf8);

  /* tnt is static global */
  tnt = lmysql->ldbms_tnt_net(NULL);
  lmysql->ldbms_tnt_set(tnt, (int)TNT_OPT_URI, connection_string);

  if (lmysql->ldbms_tnt_connect(tnt) < 0) {
      tarantool_errno= 9999;
      strcpy(tarantool_errmsg, "Connection refused for ");
      strcat(tarantool_errmsg, connection_string);
  }
  else
  {
    tarantool_errno= 0;
    strcpy(tarantool_errmsg, "OK");
  }

  if (tarantool_errno != 0)
  {
    put_diagnostics_in_result();
    statement_edit_widget->result.append(tr("Failed to connect to Tarantool server. Use menu item File|Connect to try again"));
    return 1;
  }
  statement_edit_widget->result= tr("OK");
  connections_is_connected[0]= 1;

  /*
    Todo: This overrides any earlier PROMPT statements by user.
    Probably what we want is a flag: "if user did PROMPT, don't override it."
    Or we want PROMPT statement to change ocelot_prompt.
  */
  statement_edit_widget->prompt_default= ocelot_prompt;
  statement_edit_widget->prompt_as_input_by_user= statement_edit_widget->prompt_default;

  statement_edit_widget->dbms_version= "Unknown-Version"; /* todo: find out */
  statement_edit_widget->dbms_database= ocelot_database;
  statement_edit_widget->dbms_port= QString::number(ocelot_port);
  statement_edit_widget->dbms_current_user= ocelot_user;
  statement_edit_widget->dbms_current_user_without_host= ocelot_user;
  statement_edit_widget->dbms_connection_id= 0; /* todo: find out */
  statement_edit_widget->dbms_host= ocelot_host;
  connections_is_connected[0]= 1;
  return 0;
}
#endif

#ifdef DBMS_TARANTOOL
/* tnt_flush() sends a request to the server. after every tnt_flush, save reply */

void MainWindow::tarantool_flush_and_save_reply()
{
  lmysql->ldbms_tnt_flush(tnt);

  lmysql->ldbms_tnt_reply_init(&tarantool_tnt_reply);
  tnt->read_reply(tnt, &tarantool_tnt_reply);
  tarantool_errno= tarantool_tnt_reply.code;

  if (tarantool_tnt_reply.code != 0)
  {
    char *x1= (char*)tarantool_tnt_reply.error;
    char *x2= (char*)tarantool_errmsg;
    while (x1 < tarantool_tnt_reply.error_end) *(x2++)=*(x1++);
    *x2= '\0';
  }
  else strcpy(tarantool_errmsg, "OK");
}

/* An equivalent to mysql_real_query(). NB: this might be called from a non-main thread */
int MainWindow::tarantool_real_query(const char *dbms_query, unsigned long dbms_query_len)
{
  tarantool_errno= 10001;
  strcpy(tarantool_errmsg, "Unknown Tarantool Error");

  QString text= statement_edit_widget->toPlainText();
  int i;
  int token_type=-1;
  //int statement_type=-1, clause_type=-1;
  //QString current_token, what_we_expect, what_we_got;

  parse_f_program(text); /* syntax check; get offset_of_identifier,statement_type, number_of_literals */

  if (parse_errno > 0)
  {
    tarantool_errno= parse_errno;
    return tarantool_errno;
  }

  /* The first identifier in any of the statements must be the space name. */
  /* TODO: UNLESS statement_type == TOKEN_KEYWORD_SET! */
  int spaceno= -1;
  {
    int i= offset_of_space_name;
    QString space_name= text.mid(main_token_offsets[i], main_token_lengths[i]);
    spaceno= lmysql->ldbms_tnt_get_spaceno(tnt, space_name.toUtf8(), main_token_lengths[i]);
    if (spaceno < 0)
    {
      /* Todo: figure out when this really needs to be called */
      lmysql->ldbms_tnt_reload_schema(tnt);
      spaceno= lmysql->ldbms_tnt_get_spaceno(tnt, space_name.toUtf8(), main_token_lengths[i]);
      if (spaceno < 0)
      {
        QString s;
        s= "Could not find a space named ";
        s.append(space_name);
        tarantool_errno= 10003;
        strcpy(tarantool_errmsg, s.toUtf8());
        return tarantool_errno;
      }
    }
  }

  /* DELETE + INSERT + REPLACE + (maybe?) SELECT require a tuple of values to insert or search */
  struct tnt_stream *tuple;
  if (number_of_literals > 0)
  {
    tuple= lmysql->ldbms_tnt_object(NULL);
    lmysql->ldbms_tnt_object_add_array(tuple, number_of_literals);
    for (i= 0; main_token_types[i] != 0; ++i)
    {
      token_type= main_token_types[i];
      if ((token_type == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
       || (token_type == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE))
      {
        QString s;
        s= text.mid(main_token_offsets[i], main_token_lengths[i]);
        s= s.left(s.length() - 1);
        s= s.right(s.length() - 1);
        lmysql->ldbms_tnt_object_add_str(tuple, s.toUtf8(), s.length());
      }
      if (token_type == TOKEN_TYPE_LITERAL_WITH_DIGIT)
      {
        /* todo: binary, float, double */
        QString s;
        s= text.mid(main_token_offsets[i], main_token_lengths[i]);
        lmysql->ldbms_tnt_object_add_int(tuple, s.toInt());
      }
      if (token_type == TOKEN_KEYWORD_NULL)
      {
        lmysql->ldbms_tnt_object_add_nil(tuple);
      }
    }
    /* Todo: check whether we really need to say object_container_close. */
    lmysql->ldbms_tnt_object_container_close(tuple);
  }

  result_row_count= 0; /* for everything except SELECT we ignore rows that are returned */

  if (statement_type == TOKEN_KEYWORD_DELETE)
  {
    lmysql->ldbms_tnt_delete(tnt, spaceno, 0, tuple);
    tarantool_flush_and_save_reply();
    lmysql->ldbms_tnt_reply_free(&tarantool_tnt_reply);
    return tarantool_errno;
  }

  if (statement_type == TOKEN_KEYWORD_INSERT)
  {
    if (lmysql->ldbms_tnt_insert(tnt, spaceno, tuple) < 0)
    {
      tarantool_errno= 10007;
      strcpy(tarantool_errmsg, "Bug. tnt_insert() returned an error");
      return tarantool_errno;
    }
    tarantool_flush_and_save_reply();
    lmysql->ldbms_tnt_reply_free(&tarantool_tnt_reply);
    return tarantool_errno;
  }

  if (statement_type == TOKEN_KEYWORD_REPLACE)
  {
    if (lmysql->ldbms_tnt_replace(tnt, spaceno, tuple) < 0)
    {
      tarantool_errno= 10007;
      strcpy(tarantool_errmsg, "Bug. tnt_replace() returned an error");
      return tarantool_errno;
    }
    tarantool_flush_and_save_reply();
    lmysql->ldbms_tnt_reply_free(&tarantool_tnt_reply);
    return tarantool_errno;
  }

  if (statement_type == TOKEN_KEYWORD_SELECT)
  {
    lmysql->ldbms_tnt_select(tnt, spaceno, 0, (2^32) - 1, 0, iterator_type, tuple);
    tarantool_flush_and_save_reply();
    if (tarantool_errno != 0) return tarantool_errno;
    /* The return should be an array of arrays of scalars. */

    /* If there are no rows, then there are no fields, so we cannot put up a grid. */
    /* Todo: don't forget to free if there are zero rows. */
    {
      const char *tarantool_tnt_reply_data_copy= tarantool_tnt_reply.data;
      unsigned long r= tarantool_num_rows();
      tarantool_tnt_reply.data= tarantool_tnt_reply_data_copy;
      if (r == 0)
      {
        strcpy(tarantool_errmsg, "Zero rows.");
        tarantool_errno= 10027;
      }
    }
    return tarantool_errno;
  }

  return tarantool_errno;
}

/* Given tarantool_tnt_reply, return number of rows from a SELECT. Used by result grid. */
long unsigned int MainWindow::tarantool_num_rows()
{
  const char *tarantool_tnt_reply_data= tarantool_tnt_reply.data;
  char field_type;
  field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data);
  if (field_type != MP_ARRAY)
  {
    tarantool_errno= 10008;
    strcpy(tarantool_errmsg, "Error: result contains non-scalar value\n");
    return tarantool_errno;
  }
  result_row_count= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);
  printf("number_of_rows=%ld\n", result_row_count);
  return result_row_count;
}

/*
  Given tarantool_tnt_reply, return number of fields from a SELECT. Used by result grid.
  Actually there are two counts: the count of main fields, and the count of sub-fields.
  Todo: what if there are arrays within arrays?
  Due to flattening, field_count is count of scalars not count of arrays and maps.
  For example, array[2] X Y array[3] A B C has 5 scalars.
*/
unsigned int MainWindow::tarantool_num_fields()
{
  const char *tarantool_tnt_reply_data_copy= tarantool_tnt_reply.data;
  const char **tarantool_tnt_reply_data= &tarantool_tnt_reply.data;
  char field_type;
  char field_name[TARANTOOL_MAX_FIELD_NAME_LENGTH];
  unsigned int max_field_count;

  QString field_name_list= "";

  field_type= lmysql->ldbms_mp_typeof(**tarantool_tnt_reply_data);
  assert(field_type == MP_ARRAY);
  result_row_count= lmysql->ldbms_mp_decode_array(tarantool_tnt_reply_data);
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

  {
    QMessageBox msgbox;
    msgbox.setText("tarantool_num_fields B");
    msgbox.exec();
  }


  unsigned int mid_index= 0;
  for (unsigned int i= 0; i < max_field_count; ++i)
  {
    char tmp[TARANTOOL_MAX_FIELD_NAME_LENGTH];
    int j= field_name_list.indexOf(" ", mid_index);
    printf("mid_index=%d, j=%d.\n", mid_index, j);
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
const char * MainWindow::tarantool_seek_0()
{
  char field_type;
  uint32_t row_count;
  const char *tarantool_tnt_reply_data;
  tarantool_tnt_reply_data= tarantool_tnt_reply.data;

  field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data);
  printf("    tarantool_seek_0. field_type=%d\n", field_type);
  assert(field_type == MP_ARRAY);
  row_count= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);
  assert(row_count == result_row_count);
  printf("    tarantool_seek_0. row_count=%d\n", row_count);
  assert(tarantool_tnt_reply_data != tarantool_tnt_reply.data);
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
unsigned int MainWindow::tarantool_fetch_row(const char *tarantool_tnt_reply_data,
                                             int *bytes)
{
  const char *original_tarantool_tnt_reply_data= tarantool_tnt_reply_data;
  unsigned int total_length= 0;
  char field_type;
  field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data);
  assert(field_type == MP_ARRAY);
  uint32_t field_count= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data);
  assert(field_count != 0);

  for (uint32_t field_number= 0; field_number < field_count; ++field_number)
  {
    field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data);
    printf("tarantool_fetch_row. field_number=%d, field_type=%d.\n", field_number, field_type);
    assert(field_type <= MP_EXT);
    uint32_t value_length;
    const char *value;
    char value_as_string[64]; /* must be big enough for any sprintf() result */
    if (field_type == MP_NIL)
    {
      lmysql->ldbms_mp_decode_nil(&tarantool_tnt_reply_data);
      value_length= 0;
    }
    if (field_type == MP_UINT)
    {
      uint64_t uint_value= lmysql->ldbms_mp_decode_uint(&tarantool_tnt_reply_data);
      value_length= sprintf(value_as_string, "%lu", uint_value);
      printf("... value_length=%d\n", value_length);
    }
    if (field_type == MP_INT)
    {
      int64_t int_value= lmysql->ldbms_mp_decode_int(&tarantool_tnt_reply_data);
      value_length= sprintf(value_as_string, "%ld", int_value);
    }
    if (field_type == MP_STR)
    {
      /* todo: allow for the library routine that only gets length */
      value= lmysql->ldbms_mp_decode_str(&tarantool_tnt_reply_data, &value_length);
      printf("... value_length=%d\n", value_length);
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
      printf("       mp_array, adding %d to field_count\n", array_size);
      value_length= 0;
    }
    if (field_type == MP_MAP)
    {
      uint32_t array_size;
      array_size= lmysql->ldbms_mp_decode_map(&tarantool_tnt_reply_data) * 2;
      field_count+= array_size;
      printf("       mp_map, adding %d to field_count\n", array_size);
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
      value_length= sprintf(value_as_string, "%f", double_value);
    }
    if (field_type == MP_EXT)
    {
      lmysql->ldbms_mp_next(&tarantool_tnt_reply_data);
      value_length= sizeof("EXT");
    }
    assert(value_length >= 0);
    total_length+= value_length;
  }
  *bytes=   tarantool_tnt_reply_data - original_tarantool_tnt_reply_data;
  assert(bytes != 0);
  printf("  exit tarantool_fetch_row. field_count=%d.\n", field_count);
  return total_length;
}


/*
  Given tarantool_tnt_reply, fill in field names + types + max widths.
  Return amount that we would need for allocating.
  Called from: scan_rows in result grid.
  Compare: what we do for first loop in scan_rows().
*/
void MainWindow::tarantool_scan_rows(unsigned int p_result_column_count,
               unsigned int p_result_row_count,
               MYSQL_RES *p_mysql_res,
               char **p_result_set_copy,
               char ***p_result_set_copy_rows,
               unsigned int **p_result_max_column_widths)
{
  unsigned long int v_r;
  unsigned int i;
  //char **v_row;
  //unsigned long *v_lengths;
//  unsigned int ki;

  {
    QMessageBox msgbox;
    msgbox.setText("scan_rows");
    msgbox.exec();
  }

  const char *tarantool_tnt_reply_data_copy;

  for (i= 0; i < p_result_column_count; ++i) (*p_result_max_column_widths)[i]= 0;

  /*
    First loop: find how much to allocate. Allocate. Second loop: fill in with pointers within allocated area.
  */
  unsigned int total_size= 0;
  char *result_set_copy_pointer;
  printf("seek 1\n");
  tarantool_tnt_reply_data_copy= tarantool_seek_0(); /* "seek to row 0" */
  printf("after seek 1. p_result_row_count=%d\n", p_result_row_count);

  for (v_r= 0; v_r < p_result_row_count; ++v_r)                                /* first loop */
  {
    int bytes;
    int row_size_1= tarantool_fetch_row(tarantool_tnt_reply_data_copy, &bytes);
    tarantool_tnt_reply_data_copy+= bytes;
    total_size+= row_size_1;
    /* per-field overhead includes overhead for missing fields; they are null */
    int row_size_2= p_result_column_count * (sizeof(unsigned int) + sizeof(char));
    total_size+= row_size_2;
    printf("        v_r=%lu. row_size_1=%d, row_size_2=%d\n", v_r, row_size_1, row_size_2);
  }
  *p_result_set_copy= new char[total_size];                                         /* allocate */
  assert(total_size < 1000000000);
  *p_result_set_copy_rows= new char*[p_result_row_count];
  result_set_copy_pointer= *p_result_set_copy;
  printf("seek 2\n");
  tarantool_tnt_reply_data_copy= tarantool_seek_0(); /* "seek to row 0" */
  for (v_r= 0; v_r < p_result_row_count; ++v_r)                                 /* second loop */
  {
    (*p_result_set_copy_rows)[v_r]= result_set_copy_pointer;
    char *tmp_copy_pointer= result_set_copy_pointer;
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
      if (return_value < 0) assert(0 != 0);
      tarantool_tnt_reply_data_copy= tarantool_tnt_reply_data_copy_2;
    }
    char field_type;
    field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data_copy);
    assert(field_type == MP_ARRAY);
    uint32_t field_count= lmysql->ldbms_mp_decode_array(&tarantool_tnt_reply_data_copy);
    assert(field_count <= p_result_column_count);
    const char *value;
    uint32_t value_length;
    char value_as_string[64]; /* must be big enough for any sprintf() result */
    /*
      Number of fields = number of names in field_name_list.
      At this point we can skip over MP_ARRAY and MP_MAP because we only care about scalars.
      Whenever we get a next field, we know its name is next entry in field_name_list
      If that's greater than something in the main list (which can only happen if row count > 1),
      then add NULLs for the missing fields until the main list catches up.
    */
    field_count= field_name_list.count(" ");
    printf("field_count= %d\n", field_count);
    unsigned int mid_index= 0;
    int field_number_in_main_list= 0;
    for (uint32_t field_number= 0; field_number < field_count; ++field_number)
    {
      int i= field_number;
      field_type= lmysql->ldbms_mp_typeof(*tarantool_tnt_reply_data_copy);
      assert(field_type <= MP_EXT);
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
        printf("        before the assert. j = %d, field_number=%d, field_count=%d\n", j, field_number, field_count);
        assert((j != -1) && ((j-mid_index) < TARANTOOL_MAX_FIELD_NAME_LENGTH));
        QString sv;
        sv= field_name_list.mid(mid_index, j-mid_index);
        strcpy(tmp, sv.toUtf8());
        *(tmp+sv.length())='\0';
        for (;;)
        {
          printf("compare: %s, %s.\n", &tarantool_field_names[field_number_in_main_list*TARANTOOL_MAX_FIELD_NAME_LENGTH], tmp);
          int strcmp_result= strcmp(&tarantool_field_names[field_number_in_main_list*TARANTOOL_MAX_FIELD_NAME_LENGTH], tmp);
          assert(strcmp_result <= 0);
          if (strcmp_result == 0) break;
          printf("dump null. field_number_in_main_list=%d\n", field_number_in_main_list);
          /* Dump null. Todo: similar code appears 3 times. */
          if (sizeof(NULL_STRING) - 1 > (*p_result_max_column_widths)[i]) (*p_result_max_column_widths)[i]= sizeof(NULL_STRING) - 1;
          memset(result_set_copy_pointer, 0, sizeof(unsigned int));
          *(result_set_copy_pointer + sizeof(unsigned int))= 1;
          result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
          ++field_number_in_main_list;
        }
        mid_index= j + 1;
        ++field_number_in_main_list;
      }
      printf(" (no more compares)\n");
      if ((field_type == MP_NIL) || (field_type == MP_ARRAY) || (field_type == MP_MAP))
      {
        if (sizeof(NULL_STRING) - 1 > (*p_result_max_column_widths)[i]) (*p_result_max_column_widths)[i]= sizeof(NULL_STRING) - 1;
        if (field_type == MP_NIL) lmysql->ldbms_mp_decode_nil(&tarantool_tnt_reply_data_copy);
        memset(result_set_copy_pointer, 0, sizeof(unsigned int));
        *(result_set_copy_pointer + sizeof(unsigned int))= 1;
        result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
      }
      else
      {
        if (field_type == MP_UINT)
        {
          uint64_t uint_value= lmysql->ldbms_mp_decode_uint(&tarantool_tnt_reply_data_copy);
          value_length= sprintf(value_as_string, "%lu", uint_value);
          value= value_as_string;
        }
        if (field_type == MP_INT)
        {
          int64_t int_value= lmysql->ldbms_mp_decode_int(&tarantool_tnt_reply_data_copy);
          value_length= sprintf(value_as_string, "%ld", int_value);
          value= value_as_string;
        }
        if (field_type == MP_STR)
        {
          value= lmysql->ldbms_mp_decode_str(&tarantool_tnt_reply_data_copy, &value_length);
        }
        if (field_type == MP_BIN)
        {
          value= lmysql->ldbms_mp_decode_bin(&tarantool_tnt_reply_data_copy, &value_length);
        }
        if (field_type == MP_BOOL)
        {
          bool bool_value= lmysql->ldbms_mp_decode_bool(&tarantool_tnt_reply_data_copy);
          value_length= sprintf(value_as_string, "%d", bool_value);
          value= value_as_string;
        }
        if (field_type == MP_FLOAT)
        {
          float float_value= lmysql->ldbms_mp_decode_float(&tarantool_tnt_reply_data_copy);
          value_length= sprintf(value_as_string, "%f", float_value);
          value= value_as_string;
        }
        if (field_type == MP_DOUBLE)
        {
          double double_value= lmysql->ldbms_mp_decode_double(&tarantool_tnt_reply_data_copy);
          value_length= sprintf(value_as_string, "%f", double_value);
          value= value_as_string;
        }
        if (field_type == MP_EXT)
        {
          lmysql->ldbms_mp_next(&tarantool_tnt_reply_data_copy);
          value_length= sprintf(value_as_string, "%s", "EXT");
          value= value_as_string;
        }
        if (value_length > (*p_result_max_column_widths)[i]) (*p_result_max_column_widths)[i]= value_length;
        memcpy(result_set_copy_pointer, &value_length, sizeof(unsigned int));
        *(result_set_copy_pointer + sizeof(unsigned int))= 0;
        result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
        memcpy(result_set_copy_pointer, value, value_length);
        result_set_copy_pointer+= value_length;
      }
    }
    while (field_number_in_main_list < p_result_column_count)
    {
      printf("dump null. field_number_in_main_list=%d\n", field_number_in_main_list);
      /* Dump null. Todo: similar code appears 3 times. */
      if (sizeof(NULL_STRING) - 1 > (*p_result_max_column_widths)[i]) (*p_result_max_column_widths)[i]= sizeof(NULL_STRING) - 1;
      memset(result_set_copy_pointer, 0, sizeof(unsigned int));
      *(result_set_copy_pointer + sizeof(unsigned int))= 1;
      result_set_copy_pointer+= sizeof(unsigned int) + sizeof(char);
      ++field_number_in_main_list;
    }
    printf("    loop 2 row_size = %d\n", result_set_copy_pointer - tmp_copy_pointer);
  }
  {
    QMessageBox msgbox;
    msgbox.setText("  scan_rows end");
    msgbox.exec();
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
}


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

  printf("      scan_field_names. total_size=%d\n", total_size);

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
*/

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
QString CodeEditor::prompt_translate(int line_number_x)
{
  QString s, s_char, s_out;
  int is_2_seen= 0;
  time_t basetime;
  struct tm * timeinfo;
  char formatted_time[80];
  int k_index;

  for (k_index= 0; k_index < K_SIZE; ++k_index) k_line_number[k_index]= -1;
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
      if (s_char == "C")        /* \C is for connection_id, mysql 5.7.6ff. */
      {
        s_char= QString::number(dbms_connection_id);
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
      if (s_char == "K")
      {
        int k_result= prompt_translate_k(s, i);
        if (k_result != 0)
        {
          i+= k_result;
          continue;
        }
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
                                /* todo: try harder. use markup instead of \n? crlf? */
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
      if (s_char == "y")        /* \y is for current year in 2-digit format, e.g. "14" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%y", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "_")        /* \_ is space. Odd, since "\ " is space and more obvious. */
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
  Add the width of "B " if this CodEditor is for a debug widget rather than for the statement widget.
*/
int CodeEditor::prompt_width_calculate()
{
  int line_width_first, line_width_last;
  QString sq;
  int prompt_width;

  sq= prompt_translate(1);
  line_width_first= fontMetrics().width(sq);
  sq= prompt_translate(blockCount() + 1);
  line_width_last= fontMetrics().width(sq);
  if (line_width_first < line_width_last) prompt_width= line_width_last;
  else prompt_width= line_width_first;
#ifdef DEBUGGER
  if (is_debug_widget == true) prompt_width+= fontMetrics().width("B ");
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
#ifdef DEBUGGER
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
#ifdef DEBUGGER
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

/* End of CodeEditor. End of effect of licence of CodeEditor. */

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
        setFixedSize(event->x(), height());
      }
      else
      {
        /*  Now you must persuade ResultGrid to update all the rows. Beware of multiline rows and header row (row#0). */
        /* Todo: find out why it's "result_row_count + 1" rather than based on gridx_row_count */
        ancestor_result_grid_widget->grid_column_widths[ancestor_grid_column_number]= event->x();
        int xheight;
        for (long unsigned int xrow= 0;
             (xrow < ancestor_result_grid_widget->result_row_count + 1)
             && (xrow < ancestor_result_grid_widget->result_grid_widget_max_height_in_lines_at_fillup_time);
             ++xrow)
        {
          TextEditFrame *f= ancestor_result_grid_widget->text_edit_frames[xrow * ancestor_result_grid_widget->gridx_column_count + ancestor_grid_column_number];
          if (xrow > 0) xheight= ancestor_result_grid_widget->grid_column_heights[ancestor_grid_column_number];
          if (xrow == 0) xheight= f->height();
          f->setFixedSize(event->x(), xheight);
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
        setFixedSize(width(), event->y());
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
               (xrow < ancestor_result_grid_widget->result_row_count + 1) && (xrow < RESULT_GRID_WIDGET_MAX_HEIGHT);
               ++xrow)
          {
            TextEditFrame *f= ancestor_result_grid_widget->text_edit_frames[xrow * ancestor_result_grid_widget->gridx_column_count + ancestor_grid_column_number];
            if (xrow > 0) xheight= ancestor_result_grid_widget->grid_column_heights[ancestor_grid_column_number];
            if (xrow == 0) xheight= f->height();
            f->setFixedSize(event->x(), xheight);
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
              if (is_row_number_flag == true) text_edit->setText(QString::number(ancestor_grid_result_row_number + 1));
              else text_edit->setText(QString::fromUtf8(NULL_STRING, sizeof(NULL_STRING) - 1));
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
  */
  p= p.scaled(event->rect().size(), Qt::KeepAspectRatio);
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
  if (event->matches(QKeySequence::Copy)) { copy(); return; }
  QString content_in_cell_before_keypress= toPlainText();
  QTextEdit::keyPressEvent(event);
  QString content_in_cell_after_keypress= toPlainText();
  TextEditFrame *text_edit_frame;

  int xrow;
  if ((content_in_cell_before_keypress != content_in_cell_after_keypress)
   && (text_edit_frame_of_cell->cell_type != TEXTEDITFRAME_CELL_TYPE_HEADER)
   && (text_edit_frame_of_cell->is_image_flag == false)
   && (text_edit_frame_of_cell->is_row_number_flag == false))
  {
    xrow= text_edit_frame_of_cell->ancestor_grid_result_row_number;

    ++xrow; /* possible bug: should this be done if there's no header row? */
    /* Content has changed since the last keyPressEvent. */
    /* column number = text_edit_frame_of_cell->ancestor_grid_column_number */
    /* length = text_edit_frame_of_cell->content_length */
    /* *content = text_edit_frame_of_cell->content_pointer, which should be 0 for null */

    ResultGrid *result_grid= text_edit_frame_of_cell->ancestor_result_grid_widget;
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
      if ((text_edit_frame->cell_type == TEXTEDITFRAME_CELL_TYPE_HEADER)
       || (text_edit_frame->is_row_number_flag == true))
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

      if (table_length == 0)
      {
        ++column_number;
        ++tefi;
        continue;
      }

      memcpy(&db_length, dbs_pointer, sizeof(unsigned int));
      dbs_pointer+= sizeof(unsigned int);
      db_pointer= dbs_pointer;
      dbs_pointer+= db_length;

      /* if in UNION or column-expression, skip it */
      if ((name_length == 0) || (db_length == 0))
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
    MainWindow *m= result_grid->copy_of_parent;
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
      /* ?? dunno what would cause this. maybe it's null. */
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

   At one time there was an include of getopt.h and calls to my_long_options etc.
   That introduced some unwanted dependencies and so now we do all the option retrievals directly.
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
  SYSCONFDIR/my.cnf                          ?? i.e. [installation-directory]/etc/my.cnf but this should be changeable
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
  char *home;
  char tmp_my_cnf[1024];                         /* file name = $HOME/.my.cnf or $HOME/.mylogin.cnf or defaults_extra_file */
  char *ld_run_path;

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
  ocelot_prompt= "mysql>";                  /* Todo: change to "\N [\d]>"? */

  options_files_read= "";

  {
    struct passwd *pw;
    uid_t u;
    u= geteuid();
    pw= getpwuid(u);
    if (pw != NULL) ocelot_user= pw->pw_name;
  }

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

  home= getenv("HOME");

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
    Todo: check: does MariaDB read mylogin.cnf even if ocelot_no_defaults==1?
  */
  if (QString::compare(ocelot_defaults_file, " ") > 0)
  {
    strcpy(tmp_my_cnf, ocelot_defaults_file.toUtf8());
    connect_read_my_cnf(tmp_my_cnf, 0);
  }
  else
  {
    if (ocelot_no_defaults == 0)
    {
      connect_read_my_cnf("/etc/my.cnf", 0);
      connect_read_my_cnf("/etc/mysql/my.cnf", 0);
      /* todo: think: is argv[0] what you want for SYSCONFDIR? not exact, but it's where the program is now. no, it might be a copy. */
      // connect_read_my_cnf("SYSCONFDIR/etc/my.cnf", 0) /* ?? i.e. [installation-directory]/etc/my.cnf but this should be changeable */
      /* skip $MYSQL_HOME/my.cnf, only server stuff should be in it */
      // connect_read_my_cnf("file specified with --defaults-extra-file", 0);
      if (QString::compare(ocelot_defaults_extra_file, " ") > 0)
      {
        strcpy(tmp_my_cnf, ocelot_defaults_extra_file.toUtf8());
        connect_read_my_cnf(tmp_my_cnf, 0);
      }
      strcpy(tmp_my_cnf, home);                              /* $HOME/.my.cnf */
      strcat(tmp_my_cnf, "/.my.cnf");
      connect_read_my_cnf(tmp_my_cnf, 0);
    }

    if (getenv("MYSQL_TEST_LOGIN_FILE") != NULL)
    {
      strcpy(tmp_my_cnf, getenv("MYSQL_TEST_LOGIN_FILE"));
    }
    else
    {
      strcpy(tmp_my_cnf, home);                             /* $HOME/.mylogin.cnf */
      strcat(tmp_my_cnf, "/.mylogin.cnf");
    }
    connect_read_my_cnf(tmp_my_cnf, 1);
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


/* Todo: unfortunately MAX_TOKENS == 10000, when we only need 10, so there's temporary space waste
         maybe we should use main_token_offsets and main_token_lengths? */
/* todo: check if we've already looked at the file (this is possible if !include or !includedir happens)
         if so, skip */
void MainWindow::connect_read_my_cnf(const char *file_name, int is_mylogin_cnf)
{
  //FILE *file;
  char line[2048];
  int token_offsets[MAX_TOKENS];
  int token_lengths[MAX_TOKENS];
  int i;
  QString token0, token1, token2, token_for_value;
  QString group;                                         /* what was in the last [...] e.g. mysqld, client, mysql, ocelot */
  int token0_length, token1_length, token2_length;
  unsigned char output_buffer[65536];                    /* todo: should be dynamic size */

  group= "";                                             /* group identifier doesn't carry over from last .cnf file that we read */
#ifdef __linux
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
  bool open_result= file.open(QIODevice::ReadOnly | QIODevice::Text);
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
             &token_lengths[0], &token_offsets[0], MAX_TOKENS, (QChar*)"33333", 2, "", 2);
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
        QString file_name= fileInfo.fileName().upper();
        if ((file_name.right(4) == ".CNF") || file_name.right(4) == ".INI"))
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

  if (strcmp(token0_as_utf8, "can_handle_expired_passwords") == 0) /* not available in mysql client */
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
  if (strcmp(token0_as_utf8, "ocelot_statement_syntax_checker") == 0)
  { ocelot_statement_syntax_checker= token2; return; }

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
  if (strcmp(token0_as_utf8, "prompt") == 0) { ocelot_prompt= token2; return; }
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
#ifdef DBMS_TARANTOOL
   if (strcmp(token0_as_utf8, "dbms") == 0)
   {
     ocelot_dbms= "mysql"; connections_dbms[0]= DBMS_MYSQL; /* default */
#ifdef DBMS_MARIADB
     if (QString::compare(token2, "mariadb", Qt::CaseInsensitive) == 0)
     { ocelot_dbms= "mariadb"; connections_dbms[0]= DBMS_MARIADB; }
#endif
#ifdef DBMS_TARANTOOL
     if (QString::compare(token2, "tarantool", Qt::CaseInsensitive) == 0)
     { ocelot_dbms= "tarantool"; connections_dbms[0]= DBMS_TARANTOOL; }
#endif
     return;
   }
#endif
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


int options_and_connect(
    unsigned int connection_number)
{
  if (connected[connection_number] != 0)
  {
    connected[connection_number]= 0;
    lmysql->ldbms_mysql_close(&mysql[connection_number]);
  }
  lmysql->ldbms_mysql_init(&mysql[connection_number]);
  if (ocelot_default_auth_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_23, ocelot_default_auth_as_utf8);
  if (ocelot_enable_cleartext_plugin == true) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_36, (char *) &ocelot_enable_cleartext_plugin);
  if (ocelot_init_command_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_3, ocelot_init_command_as_utf8);
  if (ocelot_opt_bind_as_utf8[0] != '\0') lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_24, ocelot_opt_bind_as_utf8);
  if (ocelot_opt_can_handle_expired_passwords != 0) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_37, (char *) &ocelot_opt_can_handle_expired_passwords);
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
  if (ocelot_opt_ssl_verify_server_cert > 0) lmysql->ldbms_mysql_options(&mysql[connection_number], OCELOT_OPTION_21, (char*) &ocelot_opt_ssl_verify_server_cert);
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

  MYSQL *connect_result;
  char *socket_parameter= ocelot_unix_socket_as_utf8;
  for (int connect_attempt= 0; connect_attempt < 6; ++connect_attempt)
  {
    connect_result= lmysql->ldbms_mysql_real_connect(&mysql[connection_number],
                                                     ocelot_host_as_utf8,
                                                     ocelot_user_as_utf8,
                                                     ocelot_password_as_utf8,
                                                     ocelot_database_as_utf8,
                                                     ocelot_port,
                                                     socket_parameter,
                                                     CLIENT_MULTI_RESULTS);
     if (connect_result != 0) break;
     /* See ocelot.ca blog post = Connecting to MySQL or MariaDB with sockets on Linux */
     /* Todo: you should provide info somewhere how the connection was actually done. */
     if ((ocelot_protocol_as_int != 0) && (ocelot_protocol_as_int != PROTOCOL_SOCKET)) break;
     if ((ocelot_unix_socket_as_utf8 != 0) && (strcmp(ocelot_unix_socket_as_utf8, "") != 0)) break;
     if ((ocelot_host_as_utf8 != 0) && (strcmp(ocelot_host_as_utf8,"") != 0) && (strcmp(ocelot_host_as_utf8, "localhost") != 0)) break;
     if (lmysql->ldbms_mysql_errno(&mysql[connection_number]) != 2002) break; /* 2002 == CR_CONNECTION_ERROR */
     if (connect_attempt == 0) socket_parameter= (char *) "/tmp/mysql.sock";
     if (connect_attempt == 1) socket_parameter= (char *) "/var/lib/mysql/mysql.sock";
     if (connect_attempt == 2) socket_parameter= (char *) "/var/run/mysqld/mysqld.sock";
     if (connect_attempt == 3) socket_parameter= (char *) "/var/run/mysql/mysql.sock";
     if (connect_attempt == 4) socket_parameter= (char *) "/tmp/mysqld.sock";
   }
  if (connect_result == 0)
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

  x= options_and_connect(connection_number);

  return x;
}


/* --version, or version in an option file, causes version display and exit */
void MainWindow::print_version()
{
  printf("ocelotgui version %s", ocelotgui_version);
  printf(", for Linux");
  #if __x86_64__
  printf(" (x86_64)");
  #endif
  printf(" using Qt version %s", qVersion()); /* i.e. Qt runtime, maybe != QT_VERSION_STR */
  printf("\n");
}


/* --help, or help in an option file, causes help display and exit */
void MainWindow::print_help()
{
  char output_string[5120];

  print_version();
  printf("Copyright (c) 2014 by Ocelot Computer Services Inc. and others\n");
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
