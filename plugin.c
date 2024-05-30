/*
plugin.c An example program which demonstrates the ocelotgui plugin feature.

Copyright (c) 2024 by Peter Gulutzan.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    menu  notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the  nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL  BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
  Last updated: 2024-05-30

  The ocelotgui plugin feature
  ----------------------------

  With simple C/C++ code you can monitor or modify ocelotgui.
  You can write "trigger" routines that ocelotgui calls at special points in its code,
  or you can write "menu" routines that users can add and pick.
  This file contains examples.
  It is not yet part of an official release.
  It assumes the operating system is Linux or Windows.
  To try it out, you have to build ocelotgui from source at github.com/ocelot-inc/ocelotgui
  Making a C library:
    Copy plugin.c to your default directory.
    Copy ocelotgui.h and other .h files from the ocelotgui source to your default directory.
    On Linux:    gcc -shared -o libplugin.so -fPIC plugin.c
    On Windows:  No tests have been done but it might work with MinGW 32-bit, thus:
                 gcc -shared -Os -s -o plugin.dll plugin.c
  Persuading ocelotgui to install a plugin from a library:
    SET ocelot_query = INSERT INTO plugins ('name of library', 'name of plugin function');
    Or start with ocelotgui --ocelot_query = "INSERT INTO plugins VALUES ('name_of_library', 'name_of_plugin');"
    When ocelotgui encounters (for example)
    set ocelot_query = insert into plugins values ('/home/pgulutzan/plugin/libplugin.so', 'real_query');
    it will seek and open libplugin.so which must be in the specified directory.
    (If it is not found and opened, there will be error messages.)
    (Path of library must be absolute, for example purposes we say /home/pgulutzan but your directory will differ.)
    Subsequently, whenever ocelotgui is in real_query (which is where it calls the DBMS server with a statement text),
    it will call the plugin_function real_query(struct plugin_pass *p) with these conventions:
      struct plugin_pass has some essential components which help the plugin know about the call
      the plugin may do whatever you want it to do, with whatever computer resources you have access to
      the plugin returns a code e.g. "continue" or "replace the statement with this before sending to the server"
      (int real_query is one of the example functions in this file, below)
  New ocelotgui statements
    These SQL-like statements are new.
    They are "client" statements, that is, they are not passed on to the server and in fact you don't need to be connected.
    SET ocelot_query = INSERT INTO menus VALUES ('id','menu_title','menu_item', 'action');
    SET ocelot_query = UPDATE menus SET menu_item | action | shortcut = 'keysequence' WHERE id = 'id';
    SET ocelot_query = SELECT * FROM menus;
    SET ocelot_query = INSERT INTO plugins VALUES ('library', 'action');
    SET ocelot_query = DELETE FROM plugins WHERE action = 'action';
    SET ocelot_query = SELECT * FROM plugins;
    SET ocelot_query = SELECT * FROM conditional_settings;
    SET ocelot_query = SELECT 'literal' as identifier [,...];
    (If 'action' ends with ; it is interpreted as an SQL statement, else if it is the name of an action defined
    in ocelotgui code it is interpreted as a non-plugin function; else it is interpreted as a plugin function.)
    (Most comparisons are case sensitive.)
  Notes about the "trigger" examples in this file
    All of the examples in this file, except menu_action, are public C routines which ocelotgui will call from
    certain places in the code (by convention the ID of the plugin is the name of the routine in ocelotgui source code).
    The example printf()s may be obscured if ocelotgui has the whole screen.
  Notes about the "menu_action" example (this is PLUGIN_MENU_CHOICE)
    With INSERT INTO plugins you add a plugin to the plugins table.
    With INSERT INTO menus you add a menuitem to the menus table.
    The menuitem may refer to the ID of the plugin.
    With UPDATE menus you may add a shortcut, that's optional.
    Once the menu has been adjusted, you can click on the menu and it will execute the function.
    Possible use: replace what's in the statement widget.
  Making a C++ library (do not try until you have tried out the C library)
    It's a bit harder to make a C++ library but the advantage is that you can see most of the classes in ocelotgui.h,
    and you can use the Qt functions (since ocelotgui is built with Qt it's usually a safe assumption that it's been
    installed already (although not on Windows). So, make a CMakeLists.txt that at a minimum contains these lines:
      set(CMAKE_BUILD_TYPE "RelWithDebInfo")
      cmake_minimum_required(VERSION 2.8.12)
      project(plugin)
      set(CMAKE_AUTOMOC ON)
      #(You can say Qt4Widgets or Qt6Widgets if you have a different Qt version.)
      find_package(Qt5Widgets REQUIRED)
      set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
      #(The MySQL/MariaDB includes won't be included; thsi is just to save space.)
      add_definitions(-DOCELOT_MYSQL_INCLUDE=0)
      add_library(plugin SHARED plugin.cpp)
      target_link_libraries(plugin Qt5::Widgets)
    cp plugin.c plugin.cpp
    cmake .
    make
    ... and now you should have a libplugin.so library. One or more examples here might require C++, they will
    have code between #ifdef __cplusplus ... #endif. They will require extern "C" definition.

  Any plugins that you distribute must be compatible with GPLv2.

*/

#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "ocelotgui.h"
#include "ostrings.h" /* This #include may not be necessary for the examples here */

int before_insert(struct plugin_pass *plugin_pass);
int make_menu(struct plugin_pass *plugin_pass);
int real_query(struct plugin_pass *plugin_pass);
int all(struct plugin_pass *plugin_pass);
int execute_one_statement(struct plugin_pass *plugin_pass);
int text_changed(struct plugin_pass *plugin_pass);
int fillup(struct plugin_pass *plugin_pass);
int display_html(struct plugin_pass *plugin_pass);
int menu_action(struct plugin_pass *plugin_pass);

/*
  PLUGIN_BEFORE_INSERT is always called during
  ocelot_query= INSERT INTO plugins VALUES ('library', 'any-plugin-other-than-before-insert')
  This is automatically one of the items in the program_widgets list (implicitly).
  The argument is a pointer to struct plugin_pass ocelot_plugin_pass,
  which has a pointer to struct connect_arguments ocelot_ca.
  It never moves. Therefore the plugin can safely take a copy of the address for later use.
  In this example, the plugin decides whether it is okay to install, that is, it approves this INSERT.
  The arbitrary criteria will be: if the plugin name is "real_query" and the port (in ocelot_ca) is 3307,
  then return PLUGIN_RETURN_SKIP which means "do not install", and INSERT will be cancelled.
  Otherwise return PLUGIN_RETURN_OK.
*/
int before_insert(struct plugin_pass *plugin_pass)
{
  printf("**** (plugin_= before_insert)\n");
  struct connect_arguments ca= *plugin_pass->ca;
  printf("**** host=%d.\n", ca.port);
  printf("**** name=%s.\n", plugin_pass->id);
  if ((ca.port == 3307) && (strcmp(plugin_pass->id, "plugin_real_query") == 0)) return PLUGIN_RETURN_SKIP;
  return PLUGIN_RETURN_OK;
}

/*
  PLUGIN_MAKE_MENU is called once, very early.
  It's here so you can change the order and appearance of the menu before it appears for the first time,
  although it's easier and safer to change it after it appears, with UPDATE or DELETE or INSERT on ocelotgui's internal MENUS table.
  Because it is called very early, parsing and messaging do not exist, so there will be no feedback if you make a mistake.
  Test as follows, after compiling this (plugin.c) and making plugin.so:
  (This example says /home/pgulutzan/plugin/libplugin.so, but of course you should use your own directory.)
  ocelotgui --ocelot_query="INSERT INTO plugins VALUES ('/home/pgulutzan/plugin/libplugin.so', 'make_menu');"
  The effect will be: The File menu is moved to after the Help menu.
  (Warning: if translating to words that are not Latin1, make sure that the editor environment is UTF-8.)
  Warning: the example works at time of writing but there is no guarantee that all versions of ocelotgui will have this substring.
*/
static char menu_buffer[10000]; /* arbitrary, current size of query is about 7500 for English */
int make_menu(struct plugin_pass *plugin_pass)
{
  printf("**** (plugin_= make_menu)\n");
  printf("**** %d %d\n", plugin_pass->replacer_buffer_length, plugin_pass->type);

  printf("**** query=%s.\n", plugin_pass->query);
  if (plugin_pass->query == NULL) return PLUGIN_RETURN_OK; /* if this is the first call it's probably init so ignore it */
  char *edit= strstr(plugin_pass->query, "INSERT INTO menus VALUES ('menu_edit',");
  if (edit == NULL) return PLUGIN_RETURN_OK; /* If ocelotgui no longer has this substring, nothing will happen */
  strcpy(menu_buffer, edit);
  strncat(menu_buffer, plugin_pass->query, edit - plugin_pass->query);
  plugin_pass->replacer_buffer_length= strlen(menu_buffer);
  plugin_pass->replacer_buffer= menu_buffer;
  
  printf("strlen(menu_buffer=%ld.\n", strlen(menu_buffer));
  printf("menu_buffer=%s.\n", menu_buffer);
  
  return PLUGIN_RETURN_OK_AND_REPLACED;
}


/*
  PLUGIN_ERROR_MESSAGE is what ocelotgui calls just before it puts out an error or warning or okay message.
  The pass will includes char *error_message.
  To replace it: make a new message that will not be automatically destroyed when the function returns,
  and set replacer_buffer to point to it. If return_value == 0 then ocelotgui will immediately copy the return value
  so there is no need to worry about preserving it afterwards.
*/
int error_message(struct plugin_pass *plugin_pass)
{
  static char new_message[1024];
  printf("**** (plugin = error_message)\n");
  printf("**** plugin_error_message %s.\n", plugin_pass->error_message);
  strcpy(new_message, "This is a replacement error message");
  plugin_pass->replacer_buffer= new_message;
  plugin_pass->replacer_buffer_length= strlen(new_message);
  return PLUGIN_RETURN_OK_AND_REPLACED;
//  char *dbms_query= (char *)arg2; /* passed argument is not really void, so cast */
//  printf("(plugin) arg1=PLUGIN_REAL_QUERY. dbms_query=%s.\n", dbms_query);
//  return 0;
}

/*
  PLUGIN_REAL_QUERY is what ocelotgui calls when about to send a user-initiated query to the server.
  The plugin_pass structure includes char *query, it's local within the executing function.
  At this point if the objective is to monitor, you could simply print the query.
  In this example, the plugin will instead replace the query with something illegal so that all server statements fail.
  To activate:
    SET ocelot_query = INSERT INTO plugins VALUES ('/home/pgulutzan/plugin/libplugin.so', 'real_query');
    SELECT 5;
  If it's a MySQL/MariaDB server, you'll see "You have an error in your SQL syntax ..."
  Client statements will not fail, so you can recover by saying
    SET ocelot_query = DELETE FROM plugins WHERE action = 'real_query';
*/
int real_query(struct plugin_pass *plugin_pass)
{
  static char new_query[1024];
  printf("**** (plugin = real_query\n");
  strcpy(new_query, "Illegal query!");
  plugin_pass->replacer_buffer= new_query;
  plugin_pass->replacer_buffer_length= strlen(new_query);
  return PLUGIN_RETURN_OK_AND_REPLACED;
}

/*
  PLUGIN_ALL is what ocelotgui calls for all plugin types. (Well, all "trigger" plugins.)
  That is, if a plugin has been inserted, ocelotgui will call it even if it's not at the trigger point.
  To activate:
    SET ocelot_query = INSERT INTO plugins VALUES ('/home/pgulutzan/plugin/libplugin.so', 'all');
  For this example, we'll just display the type and the name that corresponds to the type.
*/
int all(struct plugin_pass *plugin_pass)
{
  for (int i= 0; i < PLUGIN_MAX; ++i)
  {
    if (plugin_strvalues[i].token_keyword == plugin_pass->type)
    {
      printf("(plugin = all) type=%d, name=%s.\n", plugin_pass->type, plugin_strvalues[i].chars);
      return PLUGIN_RETURN_OK;
    }
  }
  printf("(plugin = all) type=%d is unknown\n", plugin_pass->type);
  return PLUGIN_RETURN_OK;
}

/*
  PLUGIN_EXECUTE_ONE_STATEMENT is what ocelotgui calls when it is about to execute one statement.
  The statement is probably going to be executed by the server, in which case PLUGIN_REAL_QUERY will
  also be called.
  To activate:
    SET ocelot_query = INSERT INTO plugins VALUES ('/home/pgulutzan/plugin/libplugin.so', 'execute_one_statement');
  In this example, if the statement is SELECT 5; then the plugin does its own action and then tells the server to skip execution.
  Notice the comparison is case sensitive. Most comparisons for plugins are case sensitive.
*/
int execute_one_statement(struct plugin_pass *plugin_pass)
{
  printf("(plugin = execute_one_statement)\n");
  if (strcmp(plugin_pass->query, "SELECT 5;") == 0)
  {
    /* Do your own action here ... */
    printf("**** skip\n");
    return PLUGIN_RETURN_SKIP;
  }
  return PLUGIN_RETURN_OK;
}

/*
  PLUGIN_TEXT_CHANGED is what ocelotgui calls when the statement widget has changed, a single keystroke will do it.
  This happens before parsing.
  You can replace but the replaced text will not be displayed in the statement widget, so the next keystroke will cancel your replace.
*/
int text_changed(struct plugin_pass *plugin_pass)
{
  printf("(plugin = text_changed)\n");
  printf("    %s\n", plugin_pass->query);
  return PLUGIN_RETURN_OK;
}

/*
  PLUGIN_FILLUP is what ocelotgui calls after it has a result set from any sort of query (including internal queries that usres don't see).
  ocelot_plugin_pass has result_set_copy, result_row_count, result_column_count.
  The column format is always: (unsigned int) length, (char) flags e.g. whether it's NULL, (length chars) value
  Replace will be ignored.
*/
int fillup(struct plugin_pass *plugin_pass)
{
  printf("(plugin = fillup)\n");
  //printf("    %s\n", plugin_pass->query);
  printf("result_row_count=%ld\n", plugin_pass->result_row_count);
  printf("result_column_count=%d\n", plugin_pass->result_column_count);
  char *row_pointer= plugin_pass->result_set_copy;
  unsigned int column_length;
  for (long unsigned int row_number= 1; row_number <= plugin_pass->result_row_count; ++row_number)
  {
    printf("Row #%ld\n", row_number);
    for (unsigned int column_number= 1; column_number <= plugin_pass->result_column_count; ++column_number)
    {
      printf("  Column #%d. ", column_number);
      memcpy(&column_length, row_pointer, sizeof(unsigned int));
      char flags= *(row_pointer + sizeof(unsigned int));
      row_pointer+= sizeof(unsigned int) + sizeof(char);
      if ((flags & FIELD_VALUE_FLAG_IS_NULL) != 0) printf("[null]\n");
      else if ((flags & FIELD_VALUE_FLAG_IS_IMAGE) != 0) printf("(image)\n");
      else if ((flags & FIELD_VALUE_FLAG_IS_OTHER) != 0) printf("(other) %*.*s\n", column_length, column_length, row_pointer);
      else printf("%.*s\n", column_length, row_pointer);
      row_pointer+= column_length;
    }
  }
  return PLUGIN_RETURN_OK;
}

/*
  PLUGIN_DISPLAY_HTML is what ocelotgui calls after it has converted result set to HTML and is ready to display
  one screenful  (or to display the whole result if it's exporting). of
  a result set from any sort of query (including internal queries that users don't see).
  ocelot_plugin_pass has result_set_copy, result_row_count, result_column_count.
  The column format is always: (unsigned int) length, (char) flags e.g. whether it's NULL, (length chars) value
  Replace will not be ignored, but is very tricky -- it must be null-terminated because result_buffer_length is ignored,
  it must not be bad HTML, it should correspond to what's expected in terms of field count or row count.
  If you want to use ocelotgui's HTML output for a web server, it might be easier to use ocelotgui's export feature instead.
  In this example, we'll display the HTML with printf(), then extract the <TR> and <TD> fields, and replace so the display
  will be much less sophisticated but with the same data. (Also: the algorithm is not sophisticated, if the data contains
  "<TR ..." or "<TD ..." it will get confused.)
  todo: need extern "C" for .cpp
*/
int display_html(struct plugin_pass *plugin_pass)
{
  printf("(plugin = display_html)\n");
  printf("%s\n", plugin_pass->display);
#ifdef __cplusplus
  QMessageBox msg;
  msg.setText(plugin_pass->display);
  msg.exec();
#endif
  return PLUGIN_RETURN_OK;
}

/*
  PLUGIN_MENU_CHOICE
  This will be called when you add a new menu item with your choice of name ('menu_action' is just an example name).
  To activate:
    SET ocelot_query = INSERT INTO plugins VALUES ('/home/pgulutzan/plugin/libplugin.so', 'menu_action');
    SET ocelot_query = INSERT INTO menus VALUES ('menu_action', 'NEW MENU', 'NEW MENU ITEM', 'menu_action');
    Type in a few words, without quote marks or special characters, in the statement widget but do not execute.
    Click the NEW_MENU menu.
    Click the NEW MENU ITEM menuitem.
    The statement widget should now contain SELECT ...  'what-you-typed-in';
*/
static char menu_action_buffer[10000]; /* arbitrary, as long as it's lots more than what you typed in */
int menu_action(struct plugin_pass *plugin_pass)
{
  printf("**** (plugin = menu_action)\n");
  if ((plugin_pass->query != NULL) && (strlen(plugin_pass->query) > 0))
  {
    strcpy(menu_action_buffer, "SELECT ");
    strcat(menu_action_buffer,"'");
    strcat(menu_action_buffer,plugin_pass->query);
    strcat(menu_action_buffer,"';");
    plugin_pass->replacer_buffer_length= strlen(menu_action_buffer);
    plugin_pass->replacer_buffer= menu_action_buffer;
    return PLUGIN_RETURN_OK_AND_REPLACED;
  }
  return ER_OK;
}
