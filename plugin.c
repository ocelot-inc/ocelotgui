/*
plugin.c An example program which demonstrates the ocelotgui plugin feature.

Copyright (c) 2024 by Peter Gulutzan.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
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
  The ocelotgui plugin feature makes it easy to write routines that intercept "plugin points" in the ocelotgui code
  and thus monitor or modify the GUI's behaviour.
  This file contains examples. It is not yet part of an official release. It assumes the operating system is Linux.
  Making a C library:
    Copy plugin.c to your default directory.
    Copy ocelotgui.h and other .h files from the ocelotgui source to your default directory.
    gcc -shared -o libplugin.so -fPIC plugin.c
  Persuading ocelotgui to install a plugin from a library:
    SET ocelot_query = INSTALL PLUGIN [name of plugin function] SONAME '[name of library]';
    Or start oocelotgui with --ocelot_query = INSTALL PLUGIN plugin_name SONAME '/home/pgulutzan/plugin/libplugin.so';
    When ocelotgui encounters (for example)
    set ocelot_query = install plugin real_query soname '/home/pgulutzan/plugin/libplugin.so';
    it will seek and open libplugin.so which must be in the specified directory.
    (If it is not found and opened, there will be error messages.)
    Subsequently, whenever ocelotgui is in real_query (which is where it calls the DBMS server with a statement text),
    it will call the plugin_function real_query (which is one of the functions in this example file) with these conventions:
    struct plugin_pass has some essential components which help the plugin know about the call
    the plugin is expected to return an integer
  Note: The example printf()s may be obscured if ocelotgui has the whole screen.
  Making a C++ library
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
    have code between #ifdef __cplusplus ... #endif.
*/

#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "ocelotgui.h"
#include "ostrings.h" /* This #include may be necessary for the "menu" plugin */

int before_install(struct plugin_pass *plugin_pass);
int before_connect(struct plugin_pass *plugin_pass);
int real_query(struct plugin_pass *plugin_pass);
int all(struct plugin_pass *plugin_pass);
int execute_one_statement(struct plugin_pass *plugin_pass);
int text_changed(struct plugin_pass *plugin_pass);
int fillup(struct plugin_pass *plugin_pass);
int display_html(struct plugin_pass *plugin_pass);
int menu(struct plugin_pass *plugin_pass);

/*static struct connect_arguments *ca; */ /* copy of pointer to an important structure, which should be made by plugin_init( */
/* obsolete? */
int plugin_init(struct plugin_pass *plugin_pass)
{
//  struct connect_arguments *ca= (struct connect_arguments *)arg2; /* passed argument is not really void, so cast */
//  printf("(plugin) arg1=PLUGIN_INIT. arg2.host=%s.\n", ca->host_as_utf8);
//  return 0;
  printf("**** (plugin) init)\n");
  return PLUGIN_RETURN_OK;
}

/*
  PLUGIN_BEFORE_INSTALL is always called during ocelot_query= INSTALL ...
  This is automatically one of the items in the program_widgets list (implicitly) because of its importance.
  The argument arg is a pointer to struct plugin_pass ocelot_plugin_pass,
  which has a pointer to struct connect_arguments ocelot_ca.
  It never moves. Therefore the plugin can safely take a copy of the address for later use.
  In this example, the plugin decides whether it is okay to install.
  The arbitrary criteria will be: if the plugin name is "real_query" and the port (in ocelot_ca) is 3307,
  then return PLUGIN_RETURN_SKIP which means "do not install", and INSTALL will be cancelled.
  Otherwise return PLUGIN_RETURN_OK.
*/
int before_install(struct plugin_pass *plugin_pass)
{
  printf("**** (plugin_= install)\n");
  struct connect_arguments ca= *plugin_pass->ca;
  printf("**** host=%d.\n", ca.port);
  printf("**** name=%s.\n", plugin_pass->name);
  if ((ca.port == 3307) && (strcmp(plugin_pass->name, "plugin_real_query") == 0)) return PLUGIN_RETURN_SKIP;
  return PLUGIN_RETURN_OK;
//  struct connect_arguments *ca= (struct connect_arguments *)arg2; /* passed argument is not really void, so cast */
//  printf("(plugin) arg1=PLUGIN_BEFORE_INSTALL. arg2.host=%s.\n", ca->host_as_utf8);
}

/*
  PLUGIN_BEFORE_CONNECT is called after ocelotgui has what's on the command line or in .cnf files,
  before it tries to connect. Since "SET" statements aren't possible at this early stage of execution,
  the only way that this will happen is if ocelotgui was started with a command line option e.g.
  ocelotgui --ocelot_query="install plugin x soname '/home/pgulutzan/plugin/libplugin.so'"
  or if ocelotgui found a line like
  ocelot_query=ocelot_query="install plugin x soname 'plugin.so'"
  in a .cnf file.
  Warning: if there is a syntax error during INSTALL nothing will happen, you will see no error message on the command line.
*/
int before_connect(struct plugin_pass *plugin_pass)
{
  printf("**** (plugin_= before_connect)l\n");
  return PLUGIN_RETURN_OK;
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
  PLUGIN_REAL_QUERY is what ocelotgui calls when about ot send a user-initiated query to the server.
  The call includes char *query, it's local within the executing function.
  At this point if the objective is to monitor, you could simply print the query.
  In this example, the plugin will instead replace the query with something illegal so that all server statements fail.
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
  PLUGIN_ALL is what ocelotgui calls for all plugin types, that is, if "set ocelot_query = INSTALL PLUGIN all ..." happened,
  then ocelotgui will call this for every plugin point.
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
  The statement is probably going to be executed by the server, in which PLUGIN_REAL_QUERY will
  also be called.
  In this example, if the statement is SELECT 5; then the plugin does its own action and then tells the server to skip execution.
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
  PLUGIN_DISPLAY_HTML is what ocelotgui calls after it has converted result set to HTML and is ready to display one screenful
  (or to display the whole result if it's exporting).
  a result set from any sort of query (including internal queries that usres don't see).
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
  PLUGIN_MENU is what ocelotgui calls for every menu or menu item. The items that are worth looking at are
    plugin_pass->query which has the name that will be put on the menu
    plugin_pass->subtype which has numbers defined in ostrings.h (which is supplied with ocelotgui source)
  Re automatic call:
    If you start ocelotgui with
    ocelotgui --ocelot_query="install plugin menu soname '/home/pgulutzan/plugin/libplugin.so'"
    or put "install plugin menu soname '/home/pgulutzan/plugin/libplugin.so'"
  Re what this plugin does:
    MENU_EDIT is defined in ostrings.h, and by default, if English is on, it corresponds to "Edit".
    We will translate this to the Danish word redigere.
    (Warning: if translating to words that are not Latin1, make sure that the editor environment is UTF-8.)
*/
int menu(struct plugin_pass *plugin_pass)
{
  static char new_query[1024];
  printf("**** (plugin = menu\n");
  if (plugin_pass->subtype == MENU_EDIT)
  {
 printf("**** MENU_EDIT!\n");
    strcpy(new_query, "redigere");
    plugin_pass->replacer_buffer= new_query;
    plugin_pass->replacer_buffer_length= strlen(new_query);
    return PLUGIN_RETURN_OK_AND_REPLACED;
  }
  return PLUGIN_RETURN_OK;
}
