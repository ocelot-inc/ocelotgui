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
  Languages: There are two languages: english (the default) and french.
             To set language to french, say ocelotgui --ocelot_language='french'.
             French has translations (thanks google).
             French is not tested, so we don't document it.
             It's just to illustrate how easy it is to add a language:
             (1) in string_languages[], add new language's English name,
                 after "french", before "", and add new language's
                 name for itself, after "français", before "".
             (2) add the new language's strings in er_strings,
                 er_colors, and er_menu. Important: the number
                 of strings must be the same as for english and french,
                 and the %d and %s markers in strings must be preserved,
                 or ocelotgui will crash.
             (3) translating color names: color names must not begin
                 with "#" or be blank. We accept SET ..._color = 'bleu';
                 etc. even if language='english' because
                 canonical_color_name() searches all s_color_list.
                 Color names should be short, watch MAX_COLOR_NAME_WIDTH.
                 Probably French names don't have to be in the same order
                 as English names, but I haven't tested that carefully.
             (4) rebuild from source.
  Of course ocelotgui does not translate messages from the server,
  for that you need to read
  https://dev.mysql.com/doc/refman/5.7/en/error-message-language.html
*/
/*
Todo: many string literals haven't been moved yet.
Todo: help and documentation are still pure English.
Todo: add english/canadian, with correct spelling "colour", eh?
Todo: fontMetrics().boundingRect("LightGoldenrodYellow").width());
Todo: During initialization, check validity of everything in ostrings.
*/

#ifndef OSTRINGS_H
#define OSTRINGS_H

static const char *string_languages[]=
{
  "english",
  "french",
  "",
  "anglais",
  "français",
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
  /* FRENCH */
    "OK",                           /* ER_OK */
    "Erreur non connecté",          /* ER_NOT_CONNECTED */
    "Erreur due à --one-database",  /* ER_ONE_DATABASE */
    "Couleur inconnue",                /* ER_UNKNOWN_COLOR */
    "Inconnu taille de la police",            /* ER_UNKNOWN_FONT_SIZE */
    "Inconnu poids de la police",          /* ER_UNKNOWN_FONT_WEIGHT */
    "Inconnue style de police",           /* ER_UNKNOWN_FONT_STYLE */
    "L'instruction $DELETE est pas pris en charge à ce moment", /* ER_DELETE_STATEMENT */
    "L'instruction $SKIP est pas pris en charge à ce moment", /* ER_SKIP_STATEMENT */
    "L'instruction $SOURCE est pas pris en charge à ce moment", /* ER_SOURCE_STATEMENT */
    "L'instruction $EXECUTE est pas pris en charge à ce moment", /* ER_EXECUTE_STATEMENT */
    "$DEBUG n'a pas été fait",               /* ER_DEBUG_NOT_DONE */
    "Erreur: rehash est pas pris en charge pour Tarantool", /* ER_REHASH_IS_NOT_SUPPORTED */
    "Erreur: select échoué", /* ER_SELECT_FAILED */
    "Erreur: mysql_store_result échoué", /* ER_MYSQL_STORE_RESULT_FAILED */
    "Erreur: 0 lignes retournées", /* ER_0_ROWS_RETURNED */
    "Erreur: aucune base de données sélectionnée", /* ER_NO_DATABASE_SELECTED */
    "OK. base de données =% s. tables =% d. colonnes =% d. fonctions =% et. procédures =% et. = déclenche% et. events =% d. index =% d", /* ER_OK_REHASH */
    "Error. USE statement has no argument.", /* ER_USE */
    "Erreur. instruction USE n'a pas d'argument.", /* ER_DATABASE_CHANGED */
    "Erreur. instruction SOURCE n'a pas d'argument.", /* ER_SOURCE */
    "Erreur: file_open() échoué", /* ER_FILE_OPEN */
    "Erreur, delimiter ne doit pas être vide", /* ER_DELIMITER */
    "Pour HELP, utiliser les éléments du menu Aide. Par exemple cliquez sur: Aide | Le manuel.", /* ER_HELP */
    "CHARSET pas mis en oeuvre",
    "EDIT not implemented",
    "EGO ne fait rien à moins que son sur sa propre ligne après une instruction exécutable, et --named-commandes est vrai.", /* ER_EGO */
    "GO ne fait rien à moins que son sur sa propre ligne après une instruction exécutable, et --named-commandes est vrai..", /* ER_GO */
    "NOPAGER pas mis en œuvre", /* ER_NOPAGER */
    "PAGER pas mis en oeuvre", /* ER_PAGER */
    "PRINT pas mis en oeuvre", /* ER_PRINT */
    "Erreur, fopen échoué", /* ER_FOPEN */
    "Syntaxe valeur (pour vérificateur de syntaxe) doit être comprise entre 0 et 3", /* ER_SYNTAX */
    "Format déclaration  valeur de retrait doit être comprise entre 0 et 8", /* ER_FORMAT_STATEMENT */
    "Format clause valeur de retrait doit être comprise entre 0 et 8", /* ER_FORMAT_CLAUSE */
    "Format valeur de cas clé doit être soit «supérieure» ou «inférieur» ou «inchangé»", /* FORMAT_KEY_CASE */
    "Taille de la bordure inconnu", /* ER_UNKNOWN_BORDER_SIZE */
    "Taille de la bordure de la cellule inconnu ", /* ER_UNKNOWN_CELL_BORDER_SIZE */
    "Taille de dragline cellulaire inconnu", /* ER_UNKNOWN_CELL_DRAG_LINE_SIZE */
    "Valeur illégale", /* ER_ILLEGAL_VALUE */
    " Échec de connexion. Utilisez le menu Fichier|Connexion pour essayer de nouveau", /* ER_FAILED_TO_CONNECT */
    " popen() échoué", /* ER_POPEN_FAILED */
    " pclose() échoué", /* ER_PCLOSE_FAILED */
    " Échec de connexion au serveur Tarantool. Utilisez le menu Fichier|Connexion pour essayer de nouveau", /* ER_FAILED_TO_CONNECT_TO_TARANTOOL */
    "$setup a généré %d substituts, mais le courant maximum est %d'", /* ER_SETUP */
    "Impossible d'obtenir une définition de routine pour %s.%s. Êtes-vous le créateur de routine et / ou avez-vous le privilège SELECT pour mysql.proc?", /* ER_COULD_NOT_GET */
    "SGBD version = %s Hôte = %s Port = %s", /* ER_STATUS */
    "Routine est arrêté. Suggérée étape suivante est: $EXIT", /* ER_ROUTINE_HAS_STOPPED */
    "Debugger nécessite la version MySQL 5.5 ou version ultérieure", /* ER_DEBUGGER_REQUIRES */
    "Nom(s) de routine(s) manquant(s). syntaxe attendue est: $setup nom-routine [, nom-routine ...]", /* ER_MISSING_ROUTINE_NAMES */
    "Nom de routine manquant", /* ER_MISSING_ROUTINE_NAME */
    "Debug est déjà en cours d'exécution. Utilisez Debug | sortie pour l'arrêter. Ceci pourrait être temporaire.", /* ER_DEBUG_IS_ALREADY_RUNNING */
    "Surrogate pas trouvé. Probablement $setup n'a pas été fait pour un groupe, y compris cette routine.", /* ER_SURROGATE */
    "Surrogate pas trouvé. Peut-être $setup n'a pas été fait?", /* ER_SURROGATE_NOT_FOUND */
    "%s n'a pas pu trouver une routine dans le groupe de $setup: %s.%s", /* ER_COULD_NOT_FIND_A_ROUTINE */
    "Routine is missing", /* ER_ROUTINE_IS_MISSING */
    "Debuggee ne répond pas. Code =%d. Sujet n'a pas été arrêté.\n", /* ER_DEBUGGEE_NOT_RESPONDING */
    "Aucune session de débogage en cours", /* ER_NO_DEBUG_SESSION */
    "Erreur, le format de l'instruction correcte est $breakpoint [schema_identifier.].routine_identifier] line_number_minimum [-line_number_maximum]", /* ER_BREAKPOINT_SYNTAX */
    "Erreur, le format de l'instruction correcte est $tbreakpoint [schema_identifier.].routine_identifier] line_number_minimum [-line_number_maximum]", /* ER_TBREAKPOINT_SYNTAX */
    "Erreur, le format de l'instruction correcte est $clear [schema_identifier.].routine_identifier] line_number_minimum [-line_number_maximum]", /* ER_CLEAR_SYNTAX */
    "Débordement", /* ER_OVERFLOW */
    "%s. peut-être une nouvelle $SETUP nécessaire? ne peut pas continuer. Suggérée étape suivante est: $EXIT", /* ER_DEBUGGEE_WAIT_LOOP */
    "debuggee_wait_loop() ne se produit pas", /* ER_DEBUGGEE_WAIT_LOOP_IS_NOT */
    "i status commande a échoué", /* ER_I_STATUS_FAILED */
    "i status commande a échoué (ceci n'est pas toujours une erreur grave)", /* ER_I_STATUS_FAILED_NOT_SEVERE */
    "mysql_fetch row échoué", /* ER_MYSQL_FETCH_ROW_FAILED */
    "mysql_num_fields < 14", /* ER_MYSQL_NUM_FIELDS */
    "Erreur, mysql_library_init() échoué", /* ER_MYSQL_LIBRARY_INIT_FAILED */
    "Severe error: libmysqlclient n'a pas ces noms: %s. Fermer ocelotgui, redémarrez avec une meilleure libmysqlclient. Voir Aide|libmysqlclient pour obtenir des conseils. Pour des conseils sur comment ocelotgui cherche le correct libmysqlclient, cliquez sur Aide|libmysqlclient", /* ER_LIBMYSQLCLIENT_DOES_NOT_HAVE */
    "Erreur, libmysqlclient n'a pas été trouvé ou une erreur de chargement est produite. Message était: %s. Pour des conseils sur comment ocelotgui cherche le correct libmysqlclient, cliquez sur Aide|libmysqlclient", /* ER_LIBMYSQLCLIENT_WAS_NOT_FOUND */
    "(mysql_query échoué)", /* ER_MYSQL_QUERY_FAILED */
    " %llu lignes affectées", /* ER_ROWS_AFFECTED */
    ", %d avertissement", /* ER_WARNING */
    "Erreur ", /* ER_ERROR */
    "Le vérificateur de syntaxe pense qu'il pourrait y avoir une erreur de syntaxe.", /* ER_THE_SYNTAX_CHECKER_THINKS */
    "Voulez-vous continuer?", /* ER_DO_YOU_WANT_TO_CONTINUE */
    "Oui", /* ER_YES */
    "Non" /* ER_NO */
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
#define COLOR_ALICEBLUE 0
#define COLOR_ANTIQUEWHITE 1
#define COLOR_AQUA 2
#define COLOR_AQUAMARINE 3
#define COLOR_AZURE 4
#define COLOR_BEIGE 5
#define COLOR_BISQUE 6
#define COLOR_BLACK 7
#define COLOR_BLANCHEDALMOND 8
#define COLOR_BLUE 9
#define COLOR_BLUEVIOLET 10
#define COLOR_BROWN 11
#define COLOR_BURLYWOOD 12
#define COLOR_CADETBLUE 13
#define COLOR_CHARTREUSE 14
#define COLOR_CHOCOLATE 15
#define COLOR_CORAL 16
#define COLOR_CORNFLOWERBLUE 17
#define COLOR_CORNSILK 18
#define COLOR_CRIMSON 19
#define COLOR_CYAN 20
#define COLOR_DARKBLUE 21
#define COLOR_DARKCYAN 22
#define COLOR_DARKGOLDENROD 23
#define COLOR_DARKGRAY 24
#define COLOR_DARKGREEN 25
#define COLOR_DARKKHAKI 26
#define COLOR_DARKMAGENTA 27
#define COLOR_DARKOLIVEGREEN 28
#define COLOR_DARKORANGE 29
#define COLOR_DARKORCHID 30
#define COLOR_DARKRED 31
#define COLOR_DARKSALMON 32
#define COLOR_DARKSEAGREEN 33
#define COLOR_DARKSLATEBLUE 34
#define COLOR_DARKSLATEGRAY 35
#define COLOR_DARKTURQUOISE 36
#define COLOR_DARKVIOLET 37
#define COLOR_DEEPPINK 38
#define COLOR_DEEPSKYBLUE 39
#define COLOR_DIMGRAY 40
#define COLOR_DODGERBLUE 41
#define COLOR_FIREBRICK 42
#define COLOR_FLORALWHITE 43
#define COLOR_FORESTGREEN 44
#define COLOR_FUCHSIA 45
#define COLOR_GAINSBORO 46
#define COLOR_GHOSTWHITE 47
#define COLOR_GOLD 48
#define COLOR_GOLDENROD 49
#define COLOR_GRAY 50
#define COLOR_GRAYX11 51
#define COLOR_GREEN 52
#define COLOR_GREENX11 53
#define COLOR_GREENYELLOW 54
#define COLOR_HONEYDEW 55
#define COLOR_HOTPINK 56
#define COLOR_INDIANRED 57
#define COLOR_INDIGO 58
#define COLOR_IVORY 59
#define COLOR_KHAKI 60
#define COLOR_LAVENDER 61
#define COLOR_LAVENDERBLUSH 62
#define COLOR_LAWNGREEN 63
#define COLOR_LEMONCHIFFON 64
#define COLOR_LIGHTBLUE 65
#define COLOR_LIGHTCORAL 66
#define COLOR_LIGHTCYAN 67
#define COLOR_LIGHTGOLDENRODYELLOW 68
#define COLOR_LIGHTGRAY 69
#define COLOR_LIGHTGREEN 70
#define COLOR_LIGHTPINK 71
#define COLOR_LIGHTSALMON 72
#define COLOR_LIGHTSEAGREEN 73
#define COLOR_LIGHTSKYBLUE 74
#define COLOR_LIGHTSLATEGRAY 75
#define COLOR_LIGHTSTEELBLUE 76
#define COLOR_LIGHTYELLOW 77
#define COLOR_LIME 78
#define COLOR_LIMEGREEN 79
#define COLOR_LINEN 80
#define COLOR_MAGENTA 81
#define COLOR_MAROON 82
#define COLOR_MAROONX11 83
#define COLOR_MEDIUMAQUAMARINE 84
#define COLOR_MEDIUMBLUE 85
#define COLOR_MEDIUMORCHID 86
#define COLOR_MEDIUMPURPLE 87
#define COLOR_MEDIUMSEAGREEN 88
#define COLOR_MEDIUMSLATEBLUE 89
#define COLOR_MEDIUMSPRINGGREEN 90
#define COLOR_MEDIUMTURQUOISE 91
#define COLOR_MEDIUMVIOLETRED 92
#define COLOR_MIDNIGHTBLUE 93
#define COLOR_MINTCREAM 94
#define COLOR_MISTYROSE 95
#define COLOR_MOCCASIN 96
#define COLOR_NAVAJOWHITE 97
#define COLOR_NAVY 98
#define COLOR_OLDLACE 99
#define COLOR_OLIVE 100
#define COLOR_OLIVEDRAB 101
#define COLOR_ORANGE 102
#define COLOR_ORANGERED 103
#define COLOR_ORCHID 104
#define COLOR_PALEGOLDENROD 105
#define COLOR_PALEGREEN 106
#define COLOR_PALETURQUOISE 107
#define COLOR_PALEVIOLETRED 108
#define COLOR_PAPAYAWHIP 109
#define COLOR_PEACHPUFF 110
#define COLOR_PERU 111
#define COLOR_PINK 112
#define COLOR_PLUM 113
#define COLOR_POWDERBLUE 114
#define COLOR_PURPLE 115
#define COLOR_PURPLEX11 116
#define COLOR_REBECCAPURPLE 117
#define COLOR_RED 118
#define COLOR_ROSYBROWN 119
#define COLOR_ROYALBLUE 120
#define COLOR_SADDLEBROWN 121
#define COLOR_SALMON 122
#define COLOR_SANDYBROWN 123
#define COLOR_SEAGREEN 124
#define COLOR_SEASHELL 125
#define COLOR_SIENNA 126
#define COLOR_SILVER 127
#define COLOR_SKYBLUE 128
#define COLOR_SLATEBLUE 129
#define COLOR_SLATEGRAY 130
#define COLOR_SNOW 131
#define COLOR_SPRINGGREEN 132
#define COLOR_STEELBLUE 133
#define COLOR_TAN 134
#define COLOR_TEAL 135
#define COLOR_THISTLE 136
#define COLOR_TOMATO 137
#define COLOR_TURQUOISE 138
#define COLOR_VIOLET 139
#define COLOR_WEBGRAY 140
#define COLOR_WEBGREEN 141
#define COLOR_WEBMAROON 142
#define COLOR_WEBPURPLE 143
#define COLOR_WHEAT 144
#define COLOR_WHITE 145
#define COLOR_WHITESMOKE 146
#define COLOR_YELLOW 147
#define COLOR_YELLOWGREEN 148
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
"Cyan","#00FFFF",                /* same as aqua */
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
"Lime","#00FF00",                /* same RGB as greenx11 */
"LimeGreen","#32CD32",
"Linen","#FAF0E6",
"Magenta","#FF00FF", /* same RGB as Fuchsia so display will say Fuchsia */
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
/* FRENCH */ /* Some French colors are from an uncopyrighted web page */
"Bleu Gris","#F0F8FF",
"Blanc antique","#FAEBD7",
"Bleu-vert","#00FFFF",
"Aigue-Marine","#7FFFD4",
"Bleu Azur","#F0FFFF",
"Beige","#F5F5DC",
"Beige rosé","#FFE4C4",
"Noir","#000000",
"Coquille d'oeuf","#FFEBCD",
"Bleu","#0000FF",
"Bleu-violet","#8A2BE2",
"Brun","#A52A2A",
"Bois précieux","#DEB887",
"Bleu pétrole","#5F9EA0",
"Vert vif","#7FFF00",
"Chocolat","#D2691E",
"Corail","#FF7F50",
"Bleuet","#6495ED",
"Vanille","#FFF8DC",
"Cramoisi","#DC143C",
"Cyan","#00FFFF",
"Bleu foncé","#00008B",
"Cyan foncé","#008B8B",
"Jaune paille foncé","#B8860B",
"Gris foncé","#A9A9A9",
"Vert foncé","#006400",
"Kaki foncé","#BDB76B",
"Magenta foncé","#8B008B",
"Olive foncé","#556B2F",
"Orange foncé","#FF8C00",
"Orchidée foncé","#9932CC",
"Rouge foncé","#8B0000",
"Saumon foncé","#E9967A",
"Vert d'eau foncé","#8FBC8F",
"Bleu ardoise foncé","#483D8B",
"Gris ardoise foncé","#2F4F4F",
"Turquoise foncé","#00CED1",
"Violet foncé","#9400D3",
"Rose soutenu","#FF1493",
"Bleu ciel soutenu","#00BFFF",
"Gris soutenu","#696969",
"Bleu France","#1E90FF",
"Rouge brique","#B22222",
"Lys","#FFFAF0",
"Vert sapin","#228B22",
"Fuchsia","#FF00FF",
"Gris Souris","#DCDCDC",
"Blanc laiteux","#F8F8FF",
"Or","#FFD700",
"Jaune paille","#DAA520",
"Gris","#808080",
"GrisX11","#BEBEBE",
"Vert","#008000",
"VertX11","#00FF00",
"Vert-jaune","#ADFF2F",
"Opalin","#F0FFF0",
"Rose Intense","#FF69B4",
"Rouge indien","#CD5C5C",
"Indigo","#4B0082",
"Ivoire","#FFFFF0",
"Kaki","#F0E68C",
"Lavande","#E6E6FA",
"Lavandin","#FFF0F5",
"Vert prairie","#7CFC00",
"Mousse de citron","#FFFACD",
"Bleu clair","#ADD8E6",
"Corail clair","#F08080",
"Cyan clair","#E0FFFF",
"Jaune paille clair","#FAFAD2",
"Gris clair","#D3D3D3",
"Vert clair","#90EE90",
"Rose clair","#FFB6C1",
"Saumon clair","#FFA07A",
"Vert d'eau clair","#20B2AA",
"Bleu ciel clair","#87CEFA",
"Gris ardoise clair","#778899",
"Bleu acier clair","#B0C4DE",
"Jaune clair","#FFFFE0",
"Vert citron","#00FF00",
"Citron vert","#32CD32",
"Écru","#FAF0E6",
"Magenta","#FF00FF",
"Marron","#800000",
"Marron Medium X11","#B03060",
"Aigue-marine moyen","#66CDAA",
"Bleu moyen","#0000CD",
"Lilas moyen","#BA55D3",
"Pourpre moyen","#9370DB",
"Vert d'eau moyen","#3CB371",
"Bleu ardoise moyen","#7B68EE",
"Vert printemps moyen","#00FA9A",
"Turquoise moyen","#48D1CC",
"Rouge violacé moyen","#C71585",
"Bleu nuit","#191970",
"Crème de menthe","#F5FFFA",
"Rose pâle","#FFE4E1",
"Chamois","#FFE4B5",
"Chair","#FFDEAD",
"Bleu marine","#000080",
"Blanc cassé","#FDF5E6",
"Olive","#808000",
"Brun verdâtre","#6B8E23",
"Orange","#FFA500",
"Rouge orangé","#FF4500",
"Lilas","#DA70D6",
"Jaune paille pâle","#EEE8AA",
"Vert pâle","#98FB98",
"Turquoise pâle","#AFEEEE",
"Rouge violacé pâle","#DB7093",
"Crème de papaye","#FFEFD5",
"Pêche","#FFDAB9",
"Caramel","#CD853F",
"Rose","#FFC0CB",
"Prune","#DDA0DD",
"Bleu léger","#B0E0E6",
"Pourpre","#800080",
"PourpreX11","#A020F0",
"Rebecca Pourpre","#663399",
"Rouge","#FF0000",
"Vieux rose","#BC8F8F",
"Bleu roi","#4169E1",
"Brun cuir","#8B4513",
"Saumon","#FA8072",
"Sable","#F4A460",
"Vert d'eau","#2E8B57",
"Coquillage","#FFF5EE",
"Terre de Sienne","#A0522D",
"Argent","#C0C0C0",
"Bleu ciel","#87CEEB",
"Bleu ardoise","#6A5ACD",
"Gris ardoise","#708090",
"Neige","#FFFAFA",
"Vert printemps","#00FF7F",
"Bleu acier","#4682B4",
"Grège","#D2B48C",
"Sarcelle","#008080",
"Chardon","#D8BFD8",
"Tomate","#FF6347",
"Turquoise","#40E0D0",
"Violet","#EE82EE",
"WebGris","#808080",
"WebGris","#008000",
"WebMarron","#7F0000",
"WebPourpre","#7F007F",
"Blé","#F5DEB3",
"Blanc","#FFFFFF",
"Blanc cendré","#F5F5F5",
"Jaune","#FFFF00",
"Vert jaunâtre","#9ACD32",
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
    /* FRENCH */
    "Fichier", /* MENU_FILE */
    "Connexion", /* MENU_FILE_CONNECT */
    "Sortie", /* MENU_FILE_EXIT */
    "Éditer", /* MENU_EDIT */
    "Défaire", /* MENU_EDIT_UNDO */
    "Fefaire", /* MENU_EDIT_REDO */
    "Couper", /* MENU_EDIT_CUT */
    "Copier", /* MENU_EDIT_COPY */
    "Coller", /* MENU_EDIT_PASTE */
    "Sélectionné Tout", /* MENU_EDIT_SELECT_ALL */
    "Instruction précédente", /* MENU_EDIT_PREVIOUS_STATEMENT */
    "Instruction suivante", /* MENU_EDIT_NEXT_STATEMENT */
    "Format", /* MENU_EDIT_FORMAT */
    "Courir", /* MENU_RUN */
    "Exécuter", /* MENU_RUN_EXECUTE */
    "Tuer", /* MENU_RUN_KILL */
    "Réglages", /* MENU_SETTINGS */
    "Menu", /* MENU_SETTINGS_MENU */
    "Histoire Widget", /* MENU_SETTINGS_HISTORY_WIDGET */
    "Grille Widget", /* MENU_SETTINGS_GRID_WIDGET */
    "Instruction Widget", /* MENU_SETTINGS_STATEMENT_WIDGET */
    "Règle supplémentaire 1", /* MENU_SETTINGS_EXTRA_RULE_1 */
    "Options", /* MENU_OPTIONS */
    "Détacher history widget", /* MENU_OPTIONS_DETACH_HISTORY_WIDGET */
    "Détacher result grid widget", /* MENU_OPTIONS_DETACH_RESULT_GRID_WIDGET */
    "Détacher debug widget", /* MENU_OPTIONS_DETACH_DEBUG_WIDGET */
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
    "Aide", /* MENU_HELP */
    "Sur", /* MENU_HELP_ABOUT */
    "Le Manuel", /* MENU_HELP_THE_MANUAL */
    "libmysqlclient", /* MENU_HELP_LIBMYSQLCLIENT */
    "réglages", /* MENU_HELP_SETTINGS */
        "Instruction Texte Couleur", /* MENU_STATEMENT_TEXT_COLOR */
        "Instruction Couleur de fond",
        "Instruction Surbrillance Literal Couleur",
        "Instruction Surbrillance Identificateur Couleur",
        "Instruction Surbrillance Commentaire Couleur",
        "Instruction Surbrillance Opérateur Couleur",
        "Instruction Surbrillance Mot-clé Couleur",
        "Instruction Incitation Couleur de fond",
        "Instruction Bordure Couleur",
        "Instruction Surbrillance Ligne Courante Couleur",
        "Instruction Surbrillance Fonction Couleur",
        "Grille Texte Couleur",
        "Grille Couleur de fond",
        "Grille Cellule Couleur de fond",
        "Grille Cellule Drag Line Couleur",
        "Grille Rubrique Couleur de fond",
        "Grille Bordure Couleur",
        "Grille Bordure Taille",
        "Grille Cellule Bordure Taille",
        "Grille Cellule Drag Line Taille",
        "Histoire Texte Couleur",
        "Histoire Texte Couleur de fond",
        "Histoire Bordure Couleur",
        "Menu Texte Couleur",
        "Menu Couleur de fond",
        "Menu Bordure Couleur",
        "Police     ",
        "Comte Max Line",
        "Cérificateur de syntaxe (1=Surbrillance,3=Surbrillance+dialogue d'erreur)",
        "Condition",
        "Afficher comme",
        "Annuler",
        "OK",
        "Réglages -- -- pour Menu",
        "Réglages -- -- pour Histoire",
        "Réglages -- -- pour Grille",
        "Réglages -- -- pour Instruction",
        "Réglages -- -- pour Règle supplémentaire 1",
        "Choisir nouvelle police",
        "Connexion Boîte de dialogue",
        "Fichier|Connect. Habituellement, seuls les 8 premiers champs sont importants."
};

static unsigned int er_off= 0;    /* to offset er_strings language */
static unsigned int color_off= 0; /* to offset color_strings language */
static unsigned int menu_off= 0;  /* to offset menu_strings language */

#endif // OSTRINGS_H
