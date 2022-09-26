//#include<lua5.1/lualib.h>
//#include<lua5.1/lauxlib.h>
#include<lua5.3/lua.hpp>
#include<X11/Xlib.h>
#include<fox-1.7/fx.h>
#include<iostream>

/*************************************************************************************************/
/// Externel API (re)implement lua function

// Initioalize
extern void dialogs_init( );
extern void msgbox_init( );
extern void console_init( );

// The console GUI dialogs - dialogs, message
extern int static l_test_print( lua_State *L );  	// print( text )
extern int static l_test_input( lua_State *L );	  // input( info )
extern int static l_test_message( lua_State *L ); // message( )

// The console io - console
extern int static l_test_clear( lua_State *L );	  // clear




