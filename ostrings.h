/* Copyright (c) 2014-2016 by Ocelot Computer Services Inc. All rights reserved.

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
  This file has strings that include English text,
  such as error messages, menu headings, and color names.
  Ocelot's policy is to keep strings containing English text
  separate from ocelotgui.cpp, so translations to other
  languages will be easier.
  Of course it does not translate messages from the server,
  for that you need to read
  https://dev.mysql.com/doc/refman/5.7/en/error-message-language.html
  Todo: many string literals haven't been moved yet.
  Todo: help and documentation are still pure English.
  Languages: There are two languages: english (the default) and upper.
             To set language to upper, say ocelotgui --ocelot_language='upper'.
             Upper has the same strings as english, except all upper case.
             Upper is not a serious language, so we don't document it.
             It's just to illustrate how easy it is to add a language:
             (1) in string_languages[], add new language's name,
                 after "upper", before ""
             (2) add the new language's strings in er_strings,
                 er_colors, and er_menu. Important: the number
                 of strings must be the same as for english and upper,
                 and the %d and %s markers in strings must be preserved,
                 or ocelotgui will crash.
             (3) rebuild from source.
*/

#ifndef OSTRINGS_H
#define OSTRINGS_H

static const char *string_languages[]=
{
  "english",
  "upper",
  ""
};

/*
  Error / Warning messages
  Usually we make messages with:
    make_and_put_message_in_result(int, int, char*)
  Example:
    make_and_put_message_in_result(ER_OK, 0, "");
  But if there is more than one %d or %s we call sprintf() instead.
*/
#define ER_OK                             0
#define ER_NOT_CONNECTED                  1
#define ER_ONE_DATABASE                   2
#define ER_UNKNOWN_COLOR                  3
#define ER_UNKNOWN_FONT_SIZE              4
#define ER_UNKNOWN_FONT_WEIGHT            5
#define ER_UNKNOWN_FONT_STYLE             6
#define ER_DELETE_STATEMENT               7
#define ER_SKIP_STATEMENT                 8
#define ER_SOURCE_STATEMENT               9
#define ER_EXECUTE_STATEMENT              10
#define ER_DEBUG_NOT_DONE                 11
#define ER_REHASH_IS_NOT_SUPPORTED        12
#define ER_SELECT_FAILED                  13
#define ER_MYSQL_STORE_RESULT_FAILED      14
#define ER_0_ROWS_RETURNED                15
#define ER_NO_DATABASE_SELECTED           16
#define ER_OK_REHASH                      17
#define ER_USE                            18
#define ER_DATABASE_CHANGED               19
#define ER_SOURCE                         20
#define ER_FILE_OPEN                      21
#define ER_DELIMITER                      22
#define ER_HELP                           23
#define ER_CHARSET                        24
#define ER_EDIT                           25
#define ER_EGO                            26
#define ER_GO                             27
#define ER_NOPAGER                        28
#define ER_PAGER                          29
#define ER_PRINT                          30
#define ER_FOPEN_FAILED                   31
#define ER_SYNTAX                         32
#define ER_FORMAT_STATEMENT               33
#define ER_FORMAT_CLAUSE                  34
#define ER_FORMAT_KEY_CASE                35
#define ER_UNKNOWN_BORDER_SIZE            36
#define ER_UNKNOWN_CELL_BORDER_SIZE       37
#define ER_UNKNOWN_CELL_DRAG_LINE_SIZE    38
#define ER_ILLEGAL_VALUE                  39
#define ER_FAILED_TO_CONNECT              40
#define ER_POPEN_FAILED                   41
#define ER_PCLOSE_FAILED                  42
#define ER_FAILED_TO_CONNECT_TO_TARANTOOL 43
#define ER_SETUP                          44
#define ER_COULD_NOT_GET                  45
#define ER_STATUS                         46
#define ER_ROUTINE_HAS_STOPPED            47
#define ER_DEBUGGER_REQUIRES              48
#define ER_MISSING_ROUTINE_NAMES          49
#define ER_MISSING_ROUTINE_NAME           50
#define ER_DEBUG_IS_ALREADY_RUNNING       51
#define ER_SURROGATE                      52
#define ER_SURROGATE_NOT_FOUND            53
#define ER_COULD_NOT_FIND_A_ROUTINE       54
#define ER_ROUTINE_IS_MISSING             55
#define ER_DEBUGGEE_NOT_RESPONDING        56
#define ER_NO_DEBUG_SESSION               57
#define ER_BREAKPOINT_SYNTAX              58
#define ER_TBREAKPOINT_SYNTAX             59
#define ER_CLEAR_SYNTAX                   60
#define ER_OVERFLOW                       61
#define ER_DEBUGGEE_WAIT_LOOP             62
#define ER_DEBUGGEE_WAIT_LOOP_IS_NOT      63
#define ER_I_STATUS_FAILED                64
#define ER_I_STATUS_FAILED_NOT_SEVERE     65
#define ER_MYSQL_FETCH_ROW_FAILED         66
#define ER_MYSQL_NUM_FIELDS               67
#define ER_MYSQL_LIBRARY_INIT_FAILED      68
#define ER_LIBMYSQLCLIENT_DOES_NOT_HAVE   69
#define ER_LIBMYSQLCLIENT_WAS_NOT_FOUND   70
#define ER_MYSQL_QUERY_FAILED             71
#define ER_ROWS_AFFECTED                  72
#define ER_WARNING                        73
#define ER_ERROR                          74
#define ER_THE_SYNTAX_CHECKER_THINKS      75
#define ER_DO_YOU_WANT_TO_CONTINUE        76
#define ER_YES                            77
#define ER_NO                             78
#define ER_END                            79
#define ER_MAX_LENGTH       1024
static const char *er_strings[]=
{
  /* ENGLISH */
  "OK",                           /* ER_OK */
  "ERROR not connected",          /* ER_NOT_CONNECTED */
  "ERROR due to --one-database",  /* ER_ONE_DATABASE */
  "Unknown color",                /* ER_UNKNOWN_COLOR */
  "Unknown font size",            /* ER_UNKNOWN_FONT_SIZE */
  "Unknown font weight",          /* ER_UNKNOWN_FONT_WEIGHT */
  "Unknown font style",           /* ER_UNKNOWN_FONT_STYLE */
  "The $DELETE statement is not supported at this time", /* ER_DELETE_STATEMENT */
  "The $SKIP statement is not supported at this time", /* ER_SKIP_STATEMENT */
  "The $SOURCE statement is not supported at this time", /* ER_SOURCE_STATEMENT */
  "The $EXECUTE statement is not supported at this time", /* ER_EXECUTE_STATEMENT */
  "$DEBUG not done",               /* ER_DEBUG_NOT_DONE */
  "Error: rehash is not supported for Tarantool", /* ER_REHASH_IS_NOT_SUPPORTED */
  "Error: select failed", /* ER_SELECT_FAILED */
  "Error: mysql_store_result failed", /* ER_MYSQL_STORE_RESULT_FAILED */
  "Error: 0 rows returned", /* ER_0_ROWS_RETURNED */
  "Error: no database selected", /* ER_NO_DATABASE_SELECTED */
  "OK. database=%s. tables=%d. columns=%d. functions=%d. procedures=%d. triggers=%d. events=%d. indexes=%d.", /* ER_OK_REHASH */
  "Error. USE statement has no argument.", /* ER_USE */
  "Database changed", /* ER_DATABASE_CHANGED */
  "Error, SOURCE statement has no argument", /* ER_SOURCE */
  "Error, file open() failed", /* ER_FILE_OPEN */
  "Error, delimiter should not be blank", /* ER_DELIMITER */
  "For HELP, use the Help menu items. For example click: Help | The Manual.", /* ER_HELP */
  "CHARSET not implemented",
  "EDIT not implemented",
  "EGO does nothing unless it's on its own line after an executable statement, and --named-commands is true.", /* ER_EGO */
  "GO does nothing unless it's on its own line after an executable statement, and --named-commands is true.", /* ER_GO */
  "NOPAGER not implemented", /* ER_NOPAGER */
  "PAGER not implemented", /* ER_PAGER */
  "PRINT not implemented", /* ER_PRINT */
  "Error, fopen failed", /* ER_FOPEN */
  "Syntax checker value must be between 0 and 3", /* ER_SYNTAX */
  "Format statement indent value must be between 0 and 8", /* ER_FORMAT_STATEMENT */
  "Format clause indent value must be between 0 and 8", /* ER_FORMAT_CLAUSE */
  "Format key case value must be be 'upper' or 'lower' or 'unchanged'", /* FORMAT_KEY_CASE */
  "Unknown border size", /* ER_UNKNOWN_BORDER_SIZE */
  "Unknown cell border size", /* ER_UNKNOWN_CELL_BORDER_SIZE */
  "Unknown cell drag line size", /* ER_UNKNOWN_CELL_DRAG_LINE_SIZE */
  "Illegal value", /* ER_ILLEGAL_VALUE */
  " Failed to connect. Use menu item File|Connect to try again", /* ER_FAILED_TO_CONNECT */
  " popen() failed", /* ER_POPEN_FAILED */
  " pclose() failed", /* ER_PCLOSE_FAILED */
  " Failed to connect to Tarantool server. Use menu item File|Connect to try again", /* ER_FAILED_TO_CONNECT_TO_TARANTOOL */
  "$setup generated %d surrogates but the current maximum is %d'", /* ER_SETUP */
  "Could not get a routine definition for %s.%s. Are you the routine creator and/or do you have SELECT privilege for mysql.proc?", /* ER_COULD_NOT_GET */
  "DBMS version = %s Host = %s Port = %s", /* ER_STATUS */
  "Routine has stopped. Suggested next step is: $EXIT", /* ER_ROUTINE_HAS_STOPPED */
  "Debugger requires MySQL version 5.5 or later", /* ER_DEBUGGER_REQUIRES */
  "Missing routine name(s). Expected syntax is: $setup routine-name [, routine-name ...]", /* ER_MISSING_ROUTINE_NAMES */
  "Missing routine name", /* ER_MISSING_ROUTINE_NAME */
  "Debug is already running. Use Debug|exit to stop it. This could be temporary.", /* ER_DEBUG_IS_ALREADY_RUNNING */
  "Surrogate not found. Probably $setup wasn't done for a group including this routine.", /* ER_SURROGATE */
  "Surrogate not found. Perhaps $setup was not done?", /* ER_SURROGATE_NOT_FOUND */
  "%s Could not find a routine in the $setup group: %s.%s", /* ER_COULD_NOT_FIND_A_ROUTINE */
  "Routine is missing", /* ER_ROUTINE_IS_MISSING */
  "Debuggee not responding. Code = %d. Thread has not been stopped.\n", /* ER_DEBUGGEE_NOT_RESPONDING */
  "No debug session in progress", /* ER_NO_DEBUG_SESSION */
  "Error, correct statement format is $breakpoint [schema_identifier.].routine_identifier] line_number_minimum [-line_number_maximum]", /* ER_BREAKPOINT_SYNTAX */
  "Error, correct statement format is $tbreakpoint [schema_identifier.].routine_identifier] line_number_minimum [-line_number_maximum]", /* ER_TBREAKPOINT_SYNTAX */
  "Error, correct statement format is $clear [schema_identifier.].routine_identifier] line_number_minimum [-line_number_maximum]", /* ER_CLEAR_SYNTAX */
  "Overflow", /* ER_OVERFLOW */
  "%s. maybe a new $SETUP needed? cannot continue. Suggested next step is: $EXIT", /* ER_DEBUGGEE_WAIT_LOOP */
  "debuggee_wait_loop() is not happening", /* ER_DEBUGGEE_WAIT_LOOP_IS_NOT */
  "i status command failed", /* ER_I_STATUS_FAILED */
  "i status command failed (this is not always a severe error)", /* ER_I_STATUS_FAILED_NOT_SEVERE */
  "mysql_fetch row failed", /* ER_MYSQL_FETCH_ROW_FAILED */
  "mysql_num_fields < 14", /* ER_MYSQL_NUM_FIELDS */
  "Error, mysql_library_init() failed", /* ER_MYSQL_LIBRARY_INIT_FAILED */
  "Severe error: libmysqlclient does not have these names: %s. Close ocelotgui, restart with a better libmysqlclient. See Help|libmysqlclient for tips. For tips about making sure ocelotgui finds the right libmysqlclient, click Help|libmysqlclient", /* ER_LIBMYSQLCLIENT_DOES_NOT_HAVE */
  "Error, libmysqlclient was not found or a loading error occurred. Message was: %s. For tips about making sure ocelotgui finds libmysqlclient, click Help|libmysqlclient", /* ER_LIBMYSQLCLIENT_WAS_NOT_FOUND */
  "(mysql_query failed)", /* ER_MYSQL_QUERY_FAILED */
  " %llu rows affected", /* ER_ROWS_AFFECTED */
  ", %d warning", /* ER_WARNING */
  "Error ", /* ER_ERROR */
  "The Syntax Checker thinks there might be a syntax error. ", /* ER_THE_SYNTAX_CHECKER_THINKS */
  "Do you want to continue?", /* ER_DO_YOU_WANT_TO_CONTINUE */
  "Yes", /* ER_YES */
  "No", /* ER_NO */
  /* UPPER */
  "OK",                           /* ER_OK */
  "ERROR NOT CONNECTED",          /* ER_NOT_CONNECTED */
  "ERROR DUE TO --ONE-DATABASE",  /* ER_ONE_DATABASE */
  "UNKNOWN COLOR",                /* ER_UNKNOWN_COLOR */
  "UNKNOWN FONT SIZE",            /* ER_UNKNOWN_FONT_SIZE */
  "UNKNOWN FONT WEIGHT",          /* ER_UNKNOWN_FONT_WEIGHT */
  "UNKNOWN FONT STYLE",           /* ER_UNKNOWN_FONT_STYLE */
  "THE $DELETE STATEMENT IS NOT SUPPORTED AT THIS TIME", /* ER_DELETE_STATEMENT */
  "THE $SKIP STATEMENT IS NOT SUPPORTED AT THIS TIME", /* ER_SKIP_STATEMENT */
  "THE $SOURCE STATEMENT IS NOT SUPPORTED AT THIS TIME", /* ER_SOURCE_STATEMENT */
  "THE $EXECUTE STATEMENT IS NOT SUPPORTED AT THIS TIME", /* ER_EXECUTE_STATEMENT */
  "$DEBUG NOT DONE",               /* ER_DEBUG_NOT_DONE */
  "ERROR: REHASH IS NOT SUPPORTED FOR TARANTOOL", /* ER_REHASH_IS_NOT_SUPPORTED */
  "ERROR: SELECT FAILED", /* ER_SELECT_FAILED */
  "ERROR: MYSQL_STORE_RESULT FAILED", /* ER_MYSQL_STORE_RESULT_FAILED */
  "ERROR: 0 ROWS RETURNED", /* ER_0_ROWS_RETURNED */
  "ERROR: NO DATABASE SELECTED", /* ER_NO_DATABASE_SELECTED */
  "OK. DATABASE=%S. TABLES=%D. COLUMNS=%D. FUNCTIONS=%D. PROCEDURES=%D. TRIGGERS=%D. EVENTS=%D. INDEXES=%D.", /* ER_OK_REHASH */
  "ERROR. USE STATEMENT HAS NO ARGUMENT.", /* ER_USE */
  "DATABASE CHANGED", /* ER_DATABASE_CHANGED */
  "ERROR, SOURCE STATEMENT HAS NO ARGUMENT", /* ER_SOURCE */
  "ERROR, FILE OPEN() FAILED", /* ER_FILE_OPEN */
  "ERROR, DELIMITER SHOULD NOT BE BLANK", /* ER_DELIMITER */
  "FOR HELP, USE THE HELP MENU ITEMS. FOR EXAMPLE CLICK: HELP | THE MANUAL.", /* ER_HELP */
  "CHARSET NOT IMPLEMENTED",
  "EDIT NOT IMPLEMENTED",
  "EGO DOES NOTHING UNLESS IT'S ON ITS OWN LINE AFTER AN EXECUTABLE STATEMENT, AND --NAMED-COMMANDS IS TRUE.", /* ER_EGO */
  "GO DOES NOTHING UNLESS IT'S ON ITS OWN LINE AFTER AN EXECUTABLE STATEMENT, AND --NAMED-COMMANDS IS TRUE.", /* ER_GO */
  "NOPAGER NOT IMPLEMENTED", /* ER_NOPAGER */
  "PAGER NOT IMPLEMENTED", /* ER_PAGER */
  "PRINT NOT IMPLEMENTED", /* ER_PRINT */
  "ERROR, FOPEN FAILED", /* ER_FOPEN */
  "SYNTAX CHECKER VALUE MUST BE BETWEEN 0 AND 3", /* ER_SYNTAX */
  "FORMAT STATEMENT INDENT VALUE MUST BE BETWEEN 0 AND 8", /* ER_FORMAT_STATEMENT */
  "FORMAT CLAUSE INDENT VALUE MUST BE BETWEEN 0 AND 8", /* ER_FORMAT_CLAUSE */
  "FORMAT KEY CASE VALUE MUST BE BE 'UPPER' OR 'LOWER' OR 'UNCHANGED'", /* FORMAT_KEY_CASE */
  "UNKNOWN BORDER SIZE", /* ER_UNKNOWN_BORDER_SIZE */
  "UNKNOWN CELL BORDER SIZE", /* ER_UNKNOWN_CELL_BORDER_SIZE */
  "UNKNOWN CELL DRAG LINE SIZE", /* ER_UNKNOWN_CELL_DRAG_LINE_SIZE */
  "ILLEGAL VALUE", /* ER_ILLEGAL_VALUE */
  " FAILED TO CONNECT. USE MENU ITEM FILE|CONNECT TO TRY AGAIN", /* ER_FAILED_TO_CONNECT */
  " POPEN() FAILED", /* ER_POPEN_FAILED */
  " PCLOSE() FAILED", /* ER_PCLOSE_FAILED */
  " FAILED TO CONNECT TO TARANTOOL SERVER. USE MENU ITEM FILE|CONNECT TO TRY AGAIN", /* ER_FAILED_TO_CONNECT_TO_TARANTOOL */
  "$SETUP GENERATED %D SURROGATES BUT THE CURRENT MAXIMUM IS %D'", /* ER_SETUP */
  "COULD NOT GET A ROUTINE DEFINITION FOR %S.%S. ARE YOU THE ROUTINE CREATOR AND/OR DO YOU HAVE SELECT PRIVILEGE FOR MYSQL.PROC?", /* ER_COULD_NOT_GET */
  "DBMS VERSION = %S HOST = %S PORT = %S", /* ER_STATUS */
  "ROUTINE HAS STOPPED. SUGGESTED NEXT STEP IS: $EXIT", /* ER_ROUTINE_HAS_STOPPED */
  "DEBUGGER REQUIRES MYSQL VERSION 5.5 OR LATER", /* ER_DEBUGGER_REQUIRES */
  "MISSING ROUTINE NAME(S). EXPECTED SYNTAX IS: $SETUP ROUTINE-NAME [, ROUTINE-NAME ...]", /* ER_MISSING_ROUTINE_NAMES */
  "MISSING ROUTINE NAME", /* ER_MISSING_ROUTINE_NAME */
  "DEBUG IS ALREADY RUNNING. USE DEBUG|EXIT TO STOP IT. THIS COULD BE TEMPORARY.", /* ER_DEBUG_IS_ALREADY_RUNNING */
  "SURROGATE NOT FOUND. PROBABLY $SETUP WASN'T DONE FOR A GROUP INCLUDING THIS ROUTINE.", /* ER_SURROGATE */
  "SURROGATE NOT FOUND. PERHAPS $SETUP WAS NOT DONE?", /* ER_SURROGATE_NOT_FOUND */
  "%S COULD NOT FIND A ROUTINE IN THE $SETUP GROUP: %S.%S", /* ER_COULD_NOT_FIND_A_ROUTINE */
  "ROUTINE IS MISSING", /* ER_ROUTINE_IS_MISSING */
  "DEBUGGEE NOT RESPONDING. CODE = %D. THREAD HAS NOT BEEN STOPPED.\n", /* ER_DEBUGGEE_NOT_RESPONDING */
  "NO DEBUG SESSION IN PROGRESS", /* ER_NO_DEBUG_SESSION */
  "ERROR, CORRECT STATEMENT FORMAT IS $BREAKPOINT [SCHEMA_IDENTIFIER.].ROUTINE_IDENTIFIER] LINE_NUMBER_MINIMUM [-LINE_NUMBER_MAXIMUM]", /* ER_BREAKPOINT_SYNTAX */
  "ERROR, CORRECT STATEMENT FORMAT IS $TBREAKPOINT [SCHEMA_IDENTIFIER.].ROUTINE_IDENTIFIER] LINE_NUMBER_MINIMUM [-LINE_NUMBER_MAXIMUM]", /* ER_TBREAKPOINT_SYNTAX */
  "ERROR, CORRECT STATEMENT FORMAT IS $CLEAR [SCHEMA_IDENTIFIER.].ROUTINE_IDENTIFIER] LINE_NUMBER_MINIMUM [-LINE_NUMBER_MAXIMUM]", /* ER_CLEAR_SYNTAX */
  "OVERFLOW", /* ER_OVERFLOW */
  "%S. MAYBE A NEW $SETUP NEEDED? CANNOT CONTINUE. SUGGESTED NEXT STEP IS: $EXIT", /* ER_DEBUGGEE_WAIT_LOOP */
  "DEBUGGEE_WAIT_LOOP() IS NOT HAPPENING", /* ER_DEBUGGEE_WAIT_LOOP_IS_NOT */
  "I STATUS COMMAND FAILED", /* ER_I_STATUS_FAILED */
  "I STATUS COMMAND FAILED (THIS IS NOT ALWAYS A SEVERE ERROR)", /* ER_I_STATUS_FAILED_NOT_SEVERE */
  "MYSQL_FETCH ROW FAILED", /* ER_MYSQL_FETCH_ROW_FAILED */
  "MYSQL_NUM_FIELDS < 14", /* ER_MYSQL_NUM_FIELDS */
  "ERROR, MYSQL_LIBRARY_INIT() FAILED", /* ER_MYSQL_LIBRARY_INIT_FAILED */
  "SEVERE ERROR: LIBMYSQLCLIENT DOES NOT HAVE THESE NAMES: %S. CLOSE OCELOTGUI, RESTART WITH A BETTER LIBMYSQLCLIENT. SEE HELP|LIBMYSQLCLIENT FOR TIPS. FOR TIPS ABOUT MAKING SURE OCELOTGUI FINDS THE RIGHT LIBMYSQLCLIENT, CLICK HELP|LIBMYSQLCLIENT", /* ER_LIBMYSQLCLIENT_DOES_NOT_HAVE */
  "ERROR, LIBMYSQLCLIENT WAS NOT FOUND OR A LOADING ERROR OCCURRED. MESSAGE WAS: %S. FOR TIPS ABOUT MAKING SURE OCELOTGUI FINDS LIBMYSQLCLIENT, CLICK HELP|LIBMYSQLCLIENT", /* ER_LIBMYSQLCLIENT_WAS_NOT_FOUND */
  "(MYSQL_QUERY FAILED)", /* ER_MYSQL_QUERY_FAILED */
  " %llu ROWS AFFECTED", /* ER_ROWS_AFFECTED */
  ", %d WARNING", /* ER_WARNING */
  "ERROR ", /* ER_ERROR */
  "THE SYNTAX CHECKER THINKS THERE MIGHT BE A SYNTAX ERROR. ", /* ER_THE_SYNTAX_CHECKER_THINKS */
  "DO YOU WANT TO CONTINUE?", /* ER_DO_YOU_WANT_TO_CONTINUE */
  "YES", /* ER_YES */
  "NO" /* ER_NO */
};

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
#define COLOR_END 300
static const char *s_color_list[]=
{
/* ENGLISH */
"AliceBlue","#F0F8FF",
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
"","",
/* UPPER */
"ALICEBLUE","#F0F8FF",
"ANTIQUEWHITE","#FAEBD7",
"AQUA","#00FFFF",
"AQUAMARINE","#7FFFD4",
"AZURE","#F0FFFF",
"BEIGE","#F5F5DC",
"BISQUE","#FFE4C4",
"BLACK","#000000",
"BLANCHEDALMOND","#FFEBCD",
"BLUE","#0000FF",
"BLUEVIOLET","#8A2BE2",
"BROWN","#A52A2A",
"BURLYWOOD","#DEB887",
"CADETBLUE","#5F9EA0",
"CHARTREUSE","#7FFF00",
"CHOCOLATE","#D2691E",
"CORAL","#FF7F50",
"CORNFLOWERBLUE","#6495ED",
"CORNSILK","#FFF8DC",
"CRIMSON","#DC143C",
"CYAN","#00FFFF",
"DARKBLUE","#00008B",
"DARKCYAN","#008B8B",
"DARKGOLDENROD","#B8860B",
"DARKGRAY","#A9A9A9",
"DARKGREEN","#006400",
"DARKKHAKI","#BDB76B",
"DARKMAGENTA","#8B008B",
"DARKOLIVEGREEN","#556B2F",
"DARKORANGE","#FF8C00",
"DARKORCHID","#9932CC",
"DARKRED","#8B0000",
"DARKSALMON","#E9967A",
"DARKSEAGREEN","#8FBC8F",
"DARKSLATEBLUE","#483D8B",
"DARKSLATEGRAY","#2F4F4F",
"DARKTURQUOISE","#00CED1",
"DARKVIOLET","#9400D3",
"DEEPPINK","#FF1493",
"DEEPSKYBLUE","#00BFFF",
"DIMGRAY","#696969",
"DODGERBLUE","#1E90FF",
"FIREBRICK","#B22222",
"FLORALWHITE","#FFFAF0",
"FORESTGREEN","#228B22",
"FUCHSIA","#FF00FF",
"GAINSBORO","#DCDCDC",
"GHOSTWHITE","#F8F8FF",
"GOLD","#FFD700",
"GOLDENROD","#DAA520",
"GRAY","#808080",
"GRAYX11","#BEBEBE",
"GREEN","#008000",
"GREENX11","#00FF00",
"GREENYELLOW","#ADFF2F",
"HONEYDEW","#F0FFF0",
"HOTPINK","#FF69B4",
"INDIANRED","#CD5C5C",
"INDIGO","#4B0082",
"IVORY","#FFFFF0",
"KHAKI","#F0E68C",
"LAVENDER","#E6E6FA",
"LAVENDERBLUSH","#FFF0F5",
"LAWNGREEN","#7CFC00",
"LEMONCHIFFON","#FFFACD",
"LIGHTBLUE","#ADD8E6",
"LIGHTCORAL","#F08080",
"LIGHTCYAN","#E0FFFF",
"LIGHTGOLDENRODYELLOW","#FAFAD2",
"LIGHTGRAY","#D3D3D3",
"LIGHTGREEN","#90EE90",
"LIGHTPINK","#FFB6C1",
"LIGHTSALMON","#FFA07A",
"LIGHTSEAGREEN","#20B2AA",
"LIGHTSKYBLUE","#87CEFA",
"LIGHTSLATEGRAY","#778899",
"LIGHTSTEELBLUE","#B0C4DE",
"LIGHTYELLOW","#FFFFE0",
"LIME","#00FF00",
"LIMEGREEN","#32CD32",
"LINEN","#FAF0E6",
"MAGENTA","#FF00FF",
"MAROON","#800000",
"MAROONX11","#B03060",
"MEDIUMAQUAMARINE","#66CDAA",
"MEDIUMBLUE","#0000CD",
"MEDIUMORCHID","#BA55D3",
"MEDIUMPURPLE","#9370DB",
"MEDIUMSEAGREEN","#3CB371",
"MEDIUMSLATEBLUE","#7B68EE",
"MEDIUMSPRINGGREEN","#00FA9A",
"MEDIUMTURQUOISE","#48D1CC",
"MEDIUMVIOLETRED","#C71585",
"MIDNIGHTBLUE","#191970",
"MINTCREAM","#F5FFFA",
"MISTYROSE","#FFE4E1",
"MOCCASIN","#FFE4B5",
"NAVAJOWHITE","#FFDEAD",
"NAVY","#000080",
"OLDLACE","#FDF5E6",
"OLIVE","#808000",
"OLIVEDRAB","#6B8E23",
"ORANGE","#FFA500",
"ORANGERED","#FF4500",
"ORCHID","#DA70D6",
"PALEGOLDENROD","#EEE8AA",
"PALEGREEN","#98FB98",
"PALETURQUOISE","#AFEEEE",
"PALEVIOLETRED","#DB7093",
"PAPAYAWHIP","#FFEFD5",
"PEACHPUFF","#FFDAB9",
"PERU","#CD853F",
"PINK","#FFC0CB",
"PLUM","#DDA0DD",
"POWDERBLUE","#B0E0E6",
"PURPLE","#800080",
"PURPLEX11","#A020F0",
"REBECCAPURPLE","#663399",
"RED","#FF0000",
"ROSYBROWN","#BC8F8F",
"ROYALBLUE","#4169E1",
"SADDLEBROWN","#8B4513",
"SALMON","#FA8072",
"SANDYBROWN","#F4A460",
"SEAGREEN","#2E8B57",
"SEASHELL","#FFF5EE",
"SIENNA","#A0522D",
"SILVER","#C0C0C0",
"SKYBLUE","#87CEEB",
"SLATEBLUE","#6A5ACD",
"SLATEGRAY","#708090",
"SNOW","#FFFAFA",
"SPRINGGREEN","#00FF7F",
"STEELBLUE","#4682B4",
"TAN","#D2B48C",
"TEAL","#008080",
"THISTLE","#D8BFD8",
"TOMATO","#FF6347",
"TURQUOISE","#40E0D0",
"VIOLET","#EE82EE",
"WEBGRAY","#808080",
"WEBGREEN","#008000",
"WEBMAROON","#7F0000",
"WEBPURPLE","#7F007F",
"WHEAT","#F5DEB3",
"WHITE","#FFFFFF",
"WHITESMOKE","#F5F5F5",
"YELLOW","#FFFF00",
"YELLOWGREEN","#9ACD32",
"",""
};

#define MENU_FILE       0
#define MENU_FILE_CONNECT       1
#define MENU_FILE_EXIT       2
#define MENU_EDIT       3
#define MENU_EDIT_UNDO       4
#define MENU_EDIT_REDO       5
#define MENU_EDIT_CUT       6
#define MENU_EDIT_COPY       7
#define MENU_EDIT_PASTE       8
#define MENU_EDIT_SELECT_ALL       9
#define MENU_EDIT_PREVIOUS_STATEMENT       10
#define MENU_EDIT_NEXT_STATEMENT 11
#define MENU_EDIT_FORMAT       12
#define MENU_RUN       13
#define MENU_RUN_EXECUTE       14
#define MENU_RUN_KILL       15
#define MENU_SETTINGS       16
#define MENU_SETTINGS_MENU       17
#define MENU_SETTINGS_HISTORY_WIDGET       18
#define MENU_SETTINGS_GRID_WIDGET       19
#define MENU_SETTINGS_STATEMENT_WIDGET         20
#define MENU_SETTINGS_EXTRA_RULE_1       21
#define MENU_OPTIONS       22
#define MENU_OPTIONS_DETACH_HISTORY_WIDGET       23
#define MENU_OPTIONS_DETACH_RESULT_GRID_WIDGET       24
#define MENU_OPTIONS_DETACH_DEBUG_WIDGET       25
#define MENU_DEBUG       26
#define MENU_DEBUG_INSTALL       27
#define MENU_DEBUG_SETUP       28
#define MENU_DEBUG_DEBUG       29
#define MENU_DEBUG_BREAKPOINT       30
#define MENU_DEBUG_CONTINUE       31
#define MENU_DEBUG_LEAVE       32
#define MENU_DEBUG_NEXT       33
#define MENU_DEBUG_SKIP       34
#define MENU_DEBUG_STEP       35
#define MENU_DEBUG_CLEAR       36
#define MENU_DEBUG_DELETE       37
#define MENU_DEBUG_EXIT       38
#define MENU_DEBUG_INFORMATION       39
#define MENU_DEBUG_REFRESH_SERVER_VARIABLES       40
#define MENU_DEBUG_REFRESH_USER_VARIABLES       41
#define MENU_DEBUG_REFRESH_VARIABLES       42
#define MENU_DEBUG_REFRESH_CALL_STACK       43
#define MENU_HELP       44
#define MENU_HELP_ABOUT       45
#define MENU_HELP_THE_MANUAL       46
#define MENU_HELP_LIBMYSQLCLIENT       47
#define MENU_HELP_SETTINGS       48
static int MENU_STATEMENT_TEXT_COLOR= 49;
static int MENU_STATEMENT_BACKGROUND_COLOR= 50;
static int MENU_STATEMENT_HIGHLIGHT_LITERAL_COLOR= 51;
static int MENU_STATEMENT_HIGHLIGHT_IDENTIFIER_COLOR= 52;
static int MENU_STATEMENT_HIGHLIGHT_COMMENT_COLOR= 53;
static int MENU_STATEMENT_HIGHLIGHT_OPERATOR_COLOR= 54;
static int MENU_STATEMENT_HIGHLIGHT_KEYWORD_COLOR= 55;
static int MENU_STATEMENT_PROMPT_BACKGROUND_COLOR= 56;
static int MENU_STATEMENT_BORDER_COLOR= 57;
static int MENU_STATEMENT_HIGHLIGHT_CURRENT_LINE_COLOR= 58;
static int MENU_STATEMENT_HIGHLIGHT_FUNCTION_COLOR= 59;
static int MENU_GRID_TEXT_COLOR= 60;
static int MENU_GRID_BACKGROUND_COLOR= 61;
static int MENU_GRID_CELL_BORDER_COLOR= 62;
static int MENU_GRID_CELL_DRAG_LINE_COLOR= 63;
static int MENU_GRID_HEADER_BACKGROUND_COLOR= 64;
static int MENU_GRID_BORDER_COLOR= 65;
static int MENU_GRID_BORDER_SIZE= 66;
static int MENU_GRID_CELL_BORDER_SIZE= 67;
static int MENU_GRID_CELL_DRAG_LINE_SIZE= 68;
static int MENU_HISTORY_TEXT_COLOR= 69;
static int MENU_HISTORY_BACKGROUND_COLOR= 70;
static int MENU_HISTORY_BORDER_COLOR= 71;
static int MENU_MENU_TEXT_COLOR= 72;
static int MENU_MENU_BACKGROUND_COLOR= 73;
static int MENU_MENU_BORDER_COLOR= 74;
static int MENU_FONT= 75;
static int MENU_MAX_ROW_COUNT= 76;
static int MENU_SYNTAX_CHECKER= 77;
static int MENU_CONDITION= 78;
static int MENU_DISPLAY_AS= 79;
static int MENU_CANCEL= 80;
static int MENU_OK= 81;
static int MENU_SETTINGS_FOR_MENU= 82;
static int MENU_SETTINGS_FOR_HISTORY= 83;
static int MENU_SETTINGS_FOR_GRID= 84;
static int MENU_SETTINGS_FOR_STATEMENT= 85;
static int MENU_SETTINGS_FOR_EXTRA_RULE_1= 86;
static int MENU_PICK_NEW_FONT= 87;
#define MENU_CONNECTION_DIALOG_BOX 88
#define MENU_FILE_CONNECT_HEADING 89
#define MENU_END 90

static const char *menu_strings[]=
{
/* ENGLISH */
"File", /* MENU_FILE */
"Connect", /* MENU_FILE_CONNECT */
"Exit", /* MENU_FILE_EXIT */
"Edit", /* MENU_EDIT */
"Undo", /* MENU_EDIT_UNDO */
"Redo", /* MENU_EDIT_REDO */
"Cut", /* MENU_EDIT_CUT */
"Copy", /* MENU_EDIT_COPY */
"Paste", /* MENU_EDIT_PASTE */
"Select All", /* MENU_EDIT_SELECT_ALL */
"Previous statement", /* MENU_EDIT_PREVIOUS_STATEMENT */
"Next statement", /* MENU_EDIT_NEXT_STATEMENT */
"Format", /* MENU_EDIT_FORMAT */
"Run", /* MENU_RUN */
"Execute", /* MENU_RUN_EXECUTE */
"Kill", /* MENU_RUN_KILL */
"Settings", /* MENU_SETTINGS */
"Menu", /* MENU_SETTINGS_MENU */
"History Widget", /* MENU_SETTINGS_HISTORY_WIDGET */
"Grid Widget", /* MENU_SETTINGS_GRID_WIDGET */
"Statement Widget", /* MENU_SETTINGS_STATEMENT_WIDGET */
"Extra Rule 1", /* MENU_SETTINGS_EXTRA_RULE_1 */
"Options", /* MENU_OPTIONS */
"detach history widget", /* MENU_OPTIONS_DETACH_HISTORY_WIDGET */
"detach result grid widget", /* MENU_OPTIONS_DETACH_RESULT_GRID_WIDGET */
"detach debug widget", /* MENU_OPTIONS_DETACH_DEBUG_WIDGET */
"Debug", /* MENU_DEBUG */
"Install", /* MENU_DEBUG_INSTALL */
"Setup", /* MENU_DEBUG_SETUP */
"Debug", /* MENU_DEBUG_DEBUG */
"Breakpoint", /* MENU_DEBUG_BREAKPOINT */
"Continue", /* MENU_DEBUG_CONTINUE */
"Leave", /* MENU_DEBUG_LEAVE */
"Next", /* MENU_DEBUG_NEXT */
"Skip", /* MENU_DEBUG_SKIP */
"Step", /* MENU_DEBUG_STEP */
"Clear", /* MENU_DEBUG_CLEAR */
"Delete", /* MENU_DEBUG_DELETE */
"Exit", /* MENU_DEBUG_EXIT */
"Information", /* MENU_DEBUG_INFORMATION */
"Refresh server variable", /* MENU_DEBUG_REFRESH_SERVER_VARIABLES */
"Refresh user variables", /* MENU_DEBUG_REFRESH_USER_VARIABLES */
"Refresh variables", /* MENU_DEBUG_REFRESH_VARIABLES */
"Refresh call stack", /* MENU_DEBUG_REFRESH_CALL_STACK */
"Help", /* MENU_HELP */
"About", /* MENU_HELP_ABOUT */
"The Manual", /* MENU_HELP_THE_MANUAL */
"libmysqlclient", /* MENU_HELP_LIBMYSQLCLIENT */
"settings", /* MENU_HELP_SETTINGS */
    "Statement text color", /* MENU_STATEMENT_TEXT_COLOR */
    "Statement Background Color",
    "Statement Highlight Literal Color",
    "Statement Highlight Identifier Color",
    "Statement Highlight Comment Color",
    "Statement Highlight Operator Color",
    "Statement Highlight Keyword Color",
    "Statement Prompt Background Color",
    "Statement Border Color",
    "Statement Highlight Current Line Color",
    "Statement Highlight Function Color",
    "Grid Text Color",
    "Grid Background Color",
    "Grid Cell Border Color",
    "Grid Cell Drag Line Color",
    "Grid Header Background Color",
    "Grid Border Color",
    "Grid Border Size",
    "Grid Cell Border Size",
    "Grid Cell Drag Line Size",
    "History Text Color",
    "History Background Color",
    "History Border Color",
    "Menu Text Color",
    "Menu Background Color",
    "Menu Border Color",
    "Font       ",
    "Max Row Count",
    "Syntax Checker (1=highlight,3=highlight+error dialog)",
    "Condition",
    "Display as",
    "Cancel",
    "OK",
    "Settings -- -- for Menu",
    "Settings -- -- for History",
    "Settings -- -- for Grid",
    "Settings -- -- for Statement",
    "Settings -- -- for Extra Rule 1",
    "Pick new font",
    "Connection Dialog Box",
    "File|Connect. Usually only the first 8 fields are important.",
/* UPPER */
"FILE", /* MENU_FILE */
"CONNECT", /* MENU_FILE_CONNECT */
"EXIT", /* MENU_FILE_EXIT */
"EDIT", /* MENU_EDIT */
"UNDO", /* MENU_EDIT_UNDO */
"REDO", /* MENU_EDIT_REDO */
"CUT", /* MENU_EDIT_CUT */
"COPY", /* MENU_EDIT_COPY */
"PASTE", /* MENU_EDIT_PASTE */
"SELECT ALL", /* MENU_EDIT_SELECT_ALL */
"PREVIOUS STATEMENT", /* MENU_EDIT_PREVIOUS_STATEMENT */
"NEXT STATEMENT", /* MENU_EDIT_NEXT_STATEMENT */
"FORMAT", /* MENU_EDIT_FORMAT */
"RUN", /* MENU_RUN */
"EXECUTE", /* MENU_RUN_EXECUTE */
"KILL", /* MENU_RUN_KILL */
"SETTINGS", /* MENU_SETTINGS */
"MENU", /* MENU_SETTINGS_MENU */
"HISTORY WIDGET", /* MENU_SETTINGS_HISTORY_WIDGET */
"GRID WIDGET", /* MENU_SETTINGS_GRID_WIDGET */
"STATEMENT WIDGET", /* MENU_SETTINGS_STATEMENT_WIDGET */
"EXTRA RULE 1", /* MENU_SETTINGS_EXTRA_RULE_1 */
"OPTIONS", /* MENU_OPTIONS */
"DETACH HISTORY WIDGET", /* MENU_OPTIONS_DETACH_HISTORY_WIDGET */
"DETACH RESULT GRID WIDGET", /* MENU_OPTIONS_DETACH_RESULT_GRID_WIDGET */
"DETACH DEBUG WIDGET", /* MENU_OPTIONS_DETACH_DEBUG_WIDGET */
"DEBUG", /* MENU_DEBUG */
"INSTALL", /* MENU_DEBUG_INSTALL */
"SETUP", /* MENU_DEBUG_SETUP */
"DEBUG", /* MENU_DEBUG_DEBUG */
"BREAKPOINT", /* MENU_DEBUG_BREAKPOINT */
"CONTINUE", /* MENU_DEBUG_CONTINUE */
"LEAVE", /* MENU_DEBUG_LEAVE */
"NEXT", /* MENU_DEBUG_NEXT */
"SKIP", /* MENU_DEBUG_SKIP */
"STEP", /* MENU_DEBUG_STEP */
"CLEAR", /* MENU_DEBUG_CLEAR */
"DELETE", /* MENU_DEBUG_DELETE */
"EXIT", /* MENU_DEBUG_EXIT */
"INFORMATION", /* MENU_DEBUG_INFORMATION */
"REFRESH SERVER VARIABLE", /* MENU_DEBUG_REFRESH_SERVER_VARIABLES */
"REFRESH USER VARIABLES", /* MENU_DEBUG_REFRESH_USER_VARIABLES */
"REFRESH VARIABLES", /* MENU_DEBUG_REFRESH_VARIABLES */
"REFRESH CALL STACK", /* MENU_DEBUG_REFRESH_CALL_STACK */
"HELP", /* MENU_HELP */
"ABOUT", /* MENU_HELP_ABOUT */
"THE MANUAL", /* MENU_HELP_THE_MANUAL */
"LIBMYSQLCLIENT", /* MENU_HELP_LIBMYSQLCLIENT */
"SETTINGS", /* MENU_HELP_SETTINGS */
    "STATEMENT TEXT COLOR", /* MENU_STATEMENT_TEXT_COLOR */
    "STATEMENT BACKGROUND COLOR",
    "STATEMENT HIGHLIGHT LITERAL COLOR",
    "STATEMENT HIGHLIGHT IDENTIFIER COLOR",
    "STATEMENT HIGHLIGHT COMMENT COLOR",
    "STATEMENT HIGHLIGHT OPERATOR COLOR",
    "STATEMENT HIGHLIGHT KEYWORD COLOR",
    "STATEMENT PROMPT BACKGROUND COLOR",
    "STATEMENT BORDER COLOR",
    "STATEMENT HIGHLIGHT CURRENT LINE COLOR",
    "STATEMENT HIGHLIGHT FUNCTION COLOR",
    "GRID TEXT COLOR",
    "GRID BACKGROUND COLOR",
    "GRID CELL BORDER COLOR",
    "GRID CELL DRAG LINE COLOR",
    "GRID HEADER BACKGROUND COLOR",
    "GRID BORDER COLOR",
    "GRID BORDER SIZE",
    "GRID CELL BORDER SIZE",
    "GRID CELL DRAG LINE SIZE",
    "HISTORY TEXT COLOR",
    "HISTORY BACKGROUND COLOR",
    "HISTORY BORDER COLOR",
    "MENU TEXT COLOR",
    "MENU BACKGROUND COLOR",
    "MENU BORDER COLOR",
    "FONT       ",
    "MAX ROW COUNT",
    "SYNTAX CHECKER (1=HIGHLIGHT,3=HIGHLIGHT+ERROR DIALOG)",
    "CONDITION",
    "DISPLAY AS",
    "CANCEL",
    "OK",
    "SETTINGS -- -- FOR MENU",
    "SETTINGS -- -- FOR HISTORY",
    "SETTINGS -- -- FOR GRID",
    "SETTINGS -- -- FOR STATEMENT",
    "SETTINGS -- -- FOR EXTRA RULE 1",
    "PICK NEW FONT",
    "CONNECTION DIALOG BOX",
    "FILE|CONNECT. USUALLY ONLY THE FIRST 8 FIELDS ARE IMPORTANT."
};

static unsigned int er_off= 0;    /* to offset er_strings language */
static unsigned int color_off= 0; /* to offset color_strings language */
static unsigned int menu_off= 0;  /* to offset menu_strings language */

#endif // OSTRINGS_H

