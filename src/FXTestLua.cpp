// Compile : g++ FXTestLua.cpp -o fxtestlua -L /usr/lib/i386-linux-gnu/ -l FOX-1.7 -l CHART-1.7 -l liblua5.1
#include<lua5.3/lua.hpp>
#include<X11/Xlib.h>
#include<fox-1.7/fx.h>
#include<fxghi.h>
#include<iostream>

#include<FXIconsTheme.h>

/*************************************************************************************************/
/// Declaration the API for LUA
int static l_test_print( lua_State *L );
int static l_test_input( lua_State *L );
int static l_test_message( lua_State *L );
int static l_test_clear( lua_State *L );

/*************************************************************************************************/
/// Lua Script class
class FXLuaScript : public FXThread {
  lua_State *l_interpret;
  FXString   l_script;

public :
  FXLuaScript( lua_State *_instance, const FXString &_scriptfile ) : l_interpret( _instance ), l_script( _scriptfile )
  { }

  virtual FXint run( )
  {
    FXint resulth = -1;

    if( l_interpret && ( !l_script.empty( ) ) ) {
      std::cout << "Start script " << l_script.text( ) << std::endl;
      resulth = luaL_dofile( l_interpret, l_script.text( ) );
      std::cout << "End script "  << l_script.text( ) << " width  " << resulth << " status code." << std::endl;
    }
    //this->suspend( );
    this->join( resulth );
    return 0;
  }
};


/*************************************************************************************************/
/// Declaration the main window class
class FXTestLua : public FXGWindow  {
FXDECLARE( FXTestLua )
  /// QUI
  FXText      *m_output;      // Textove pole pro vystup ze skriptu
  //FXLabel   *m_label;       // Lista s cestou a nazvem souboru skriptu
  FXButton    *m_runBtn;      // Indikacni tlacitko spusteni skriptu
  FXMenuBar   *m_menu;        // Lista menu
  FXMenuPane  *m_FileMenu;    // Roleta nabidky soubor
  FXMenuPane  *m_ScriptMenu;  // Roleta scriptu
  FXMenuPane  *m_ConfigMenu;  // Roleta konfigurace
  FXIconsTheme *m_icons_th;   //

  /// STATUS
  lua_State    *m_lua;         // Promena stavu interpreteru
  FXbool        m_init;        // Indikace inicializace interpreteru
  FXString      m_script;      // Soubor scriptu
  FXThreadPool  m_threads;     // Spravce scriptu

  /// CONFIG
  FXbool   m_input;       // Rezim cteni vstupu
  FXString m_workdir;     // Aktualni pracovni adresar
  FXString m_opendir;     // Adresar ze ktereho byl naposledy spusten skript
  FXString m_editor;      // Editor scriptu
  FXbool   m_editwait;    // Blokujici rezim editoru

  /// Output theme
  FXColor  m_out_bg;      // Background color
  FXColor  m_out_fg;      // Foreground color 
  FXString m_out_font;    // Parametry fontu 
  FXString m_icons_name;  // Nazev ikonoveho tematu

public:
  FXTestLua( FXApp *a );
  virtual ~FXTestLua( );

  ///////////////////////////////////////////////
  ///
  ///
  virtual void create( );
  void readConfig( );
  void writeConfig( );

  void ErrorMessage( FXint num = -1, const FXString msg = FXString::null );
  void Error( const FXString &err_string );

  void     output( const FXString &value, FXbool nonl = false );
  FXString input( const FXString &label = FXString::null );
  void     msgBox_init( );

  ///////////////////////////////////////////////
  ///
  ///
  enum {
    SCRIPT_RUN = FXGWindow /*FXMainWindow*/::ID_LAST,
    SCRIPT_NEW,
    SCRIPT_SELECT,
    SCRIPT_EDIT,
    BUFFER_CLEAR,
    BUFFER_SAVE,
    BUFFER_STATUS,
    BUFFER_ENTER,
    BUFFER_INPWAIT,
    CONFIG_WORKDIR,
    ID_LAST
  };

  long onCmd_system( FXObject *tgt, FXSelector sel, void *data );
  long onCmd_script( FXObject *tgt, FXSelector sel, void *data );
  long onCmd_buffer( FXObject *tgt, FXSelector sel, void *data );
  long onCmd_config( FXObject *tgt, FXSelector sel, void *data );

protected :
  FXTestLua( ) { }

  FXint ScriptEdit( const FXString &file = FXString::null );
  FXbool ScriptRun( );
};


/**************************************************************************************************/
/// Implement the Main window class
FXDEFMAP( FXTestLua ) TestLuaMap[ ] = {
  FXMAPFUNC( SEL_COMMAND, FXTestLua::SCRIPT_SELECT, FXTestLua::onCmd_script ),
  FXMAPFUNC( SEL_COMMAND, FXTestLua::SCRIPT_RUN,    FXTestLua::onCmd_script ),
  FXMAPFUNC( SEL_COMMAND, FXTestLua::SCRIPT_NEW,    FXTestLua::onCmd_script ),
  FXMAPFUNC( SEL_COMMAND, FXTestLua::SCRIPT_EDIT,   FXTestLua::onCmd_script ),
  FXMAPFUNC( SEL_COMMAND, FXTestLua::BUFFER_CLEAR,  FXTestLua::onCmd_buffer ),
  FXMAPFUNC( SEL_COMMAND, FXTestLua::BUFFER_SAVE,   FXTestLua::onCmd_buffer ),
  FXMAPFUNC( SEL_COMMAND, FXTestLua::BUFFER_STATUS, FXTestLua::onCmd_buffer ),
  FXMAPFUNC( SEL_COMMAND, FXTestLua::CONFIG_WORKDIR, FXTestLua::onCmd_config ),
};

FXIMPLEMENT( FXTestLua, FXGWindow , TestLuaMap, ARRAYNUMBER( TestLuaMap ) )

FXTestLua::FXTestLua( FXApp *a )
         : FXGWindow( a, "FOX Lua konsole", NULL, NULL, WINDOW_PRIMARY | CONTROLS_NORMAL, 0, 0, 800, 600 )
{
  /// Creating and initialize the program values
  /// m_script = FXString::null;
  m_icons_th = new FXIconsTheme( getApp( ) );

  readConfig( );
  getHeader( )->setText( "Tiger Desktop Utility" );
/*  if( a->getArgc( ) > 1 ) {
    m_script = a->getArgv( )[ 1 ];
  }
*/
  /// Creatinq GUI

  ////////////////////////////////////////////////
  ///  Window composite mask
  ///
  FXVerticalFrame *content = new FXVerticalFrame( this, FRAME_NONE | LAYOUT_FILL/*, 0, 0, 0, 0, DEFAULT_SPACING, DEFAULT_SPACING, DEFAULT_SPACING, DEFAULT_SPACING, 5, 5*/ );

  FXMenuPane *Menu = new FXMenuPane( this );
  m_FileMenu = new FXMenuPane( Menu );
  new FXMenuCommand( m_FileMenu, "Novy Script", m_icons_th->get_icon( "new" ), this, FXSEL( SEL_COMMAND, FXTestLua::SCRIPT_NEW ) );
  new FXMenuCommand( m_FileMenu, "Otevrit Script", m_icons_th->get_icon( "open" ), this, FXSEL( SEL_COMMAND, FXTestLua::SCRIPT_SELECT ) );
  new FXMenuCommand( m_FileMenu, "Editovat Script", m_icons_th->get_icon( "edit" ), this, FXSEL( SEL_COMMAND, FXTestLua::SCRIPT_EDIT ) );
  new FXMenuSeparator( m_FileMenu );
  new FXMenuCommand( m_FileMenu, "Ulozit vysledky", m_icons_th->get_icon( "save" ), this, FXSEL( SEL_COMMAND, FXTestLua::BUFFER_SAVE ) );
  new FXMenuSeparator( m_FileMenu );
  new FXMenuCommand( m_FileMenu, "Zavrit", m_icons_th->get_icon( "close" ), this, FXSEL( SEL_COMMAND, FXApp::ID_QUIT ) );
  new FXMenuCascade( Menu, "Soubor", NULL, m_FileMenu );

  m_ScriptMenu = new FXMenuPane( Menu );
  new FXMenuCommand( m_ScriptMenu, "Spustit", m_icons_th->get_icon( "run" ), this, FXSEL( SEL_COMMAND, FXTestLua::SCRIPT_RUN ) );
  new FXMenuSeparator( m_ScriptMenu );
  new FXMenuCommand( m_ScriptMenu, "Vymazat buffer", m_icons_th->get_icon( "buffer_clean" ), this, FXSEL( SEL_COMMAND, FXTestLua::BUFFER_CLEAR ) );
  new FXMenuCommand( m_ScriptMenu, "Velikost bufferu", m_icons_th->get_icon( "buffer_size" ), this, FXSEL( SEL_COMMAND, FXTestLua::BUFFER_STATUS ) );
  new FXMenuCascade( Menu, "Lua", NULL, m_ScriptMenu );

  m_ConfigMenu = new FXMenuPane( Menu );
  new FXMenuCommand( m_ConfigMenu, "Zmenit pracovni adresar", m_icons_th->get_icon( "directory" ), this, FXSEL( SEL_COMMAND, FXTestLua::CONFIG_WORKDIR ) );
  new FXMenuSeparator( m_ConfigMenu );
  new FXMenuCommand( m_ConfigMenu, "Lua moduly", m_icons_th->get_icon( "modules" ), this, FXSEL( SEL_COMMAND, FXTestLua::SCRIPT_SELECT ) );
  new FXMenuCascade( Menu, "Nastaveni", NULL, m_ConfigMenu );
  
  /*FXMenuButton *MenuButton =*/ new FXMenuButton( getHeader( ), "\t\t Menu Applikace",  getMenuIcon( ), Menu, BUTTON_TOOLBAR|ICON_ABOVE_TEXT|LAYOUT_FILL_Y );
  new FXVerticalSeparator( getHeader( ) );
  // App bar
  new FXButton( getHeader( ), "\t\t Zalozit novy",   m_icons_th->get_icon( "new", "HeaderBar"  ),  this, FXSEL( SEL_COMMAND, FXTestLua::SCRIPT_NEW ),   BUTTON_TOOLBAR | FRAME_RAISED | ICON_ABOVE_TEXT | LAYOUT_FILL_Y );
  new FXButton( getHeader( ), "\t\t Otevrit soubor", m_icons_th->get_icon( "open", "HeaderBar" ), this, FXSEL( SEL_COMMAND, FXTestLua::SCRIPT_SELECT ), BUTTON_TOOLBAR | FRAME_RAISED | ICON_ABOVE_TEXT | LAYOUT_FILL_Y );
  new FXButton( getHeader( ), "\t\t Spustit",        m_icons_th->get_icon( "run", "HeaderBar"  ),  this, FXTestLua::SCRIPT_RUN,   BUTTON_TOOLBAR | FRAME_RAISED | ICON_ABOVE_TEXT | LAYOUT_FILL_Y );
  //new FXButton( _windowfr, "\t\t Zavrit", (*m_icons) [ "Actions/dialog-close.png" ],  getApp( ), FXApp::ID_QUIT, BUTTON_TOOLBAR | FRAME_RAISED | ICON_ABOVE_TEXT | LAYOUT_FILL_Y );
  FXVerticalFrame *_content = new FXVerticalFrame( content, FRAME_SUNKEN | LAYOUT_FILL, 0, 0, 0, 0, DEFAULT_SPACING, DEFAULT_SPACING, DEFAULT_SPACING, DEFAULT_SPACING, 5, 5 );
  /*
  m_menu = new FXMenuBar( _content, LAYOUT_SIDE_TOP | LAYOUT_FILL_X );
  m_FileMenu = new FXMenuPane( m_menu );
  new FXMenuCommand( m_FileMenu, "Novy Script", (*m_icons)[ "Actions/document-new.png" ] , this, FXSEL( SEL_COMMAND, FXTestLua::SCRIPT_NEW ) );
  new FXMenuCommand( m_FileMenu, "Otevrit Script", (*m_icons)[ "Actions/document-open.png" ], this, FXSEL( SEL_COMMAND, FXTestLua::SCRIPT_SELECT ) );
  new FXMenuCommand( m_FileMenu, "Editovat Script", (*m_icons)[ "Actions/document-edit.png" ], this, FXSEL( SEL_COMMAND, FXTestLua::SCRIPT_EDIT ) );
  new FXMenuSeparator( m_FileMenu );
  new FXMenuCommand( m_FileMenu, "Ulozit vysledky", (*m_icons)[ "Actions/document-save.png" ], this, FXSEL( SEL_COMMAND, FXTestLua::BUFFER_SAVE ) );
  new FXMenuSeparator( m_FileMenu );
  new FXMenuCommand( m_FileMenu, "Zavrit", (*m_icons)[ "Actions/application-exit.png" ], this, FXSEL( SEL_COMMAND, FXApp::ID_QUIT ) );
  new FXMenuTitle( m_menu, "Soubor", NULL, m_FileMenu );

  m_ScriptMenu = new FXMenuPane( m_menu );
  new FXMenuCommand( m_ScriptMenu, "Spustit", (*m_icons)[ "Actions/media-playback-start.png" ], this, FXSEL( SEL_COMMAND, FXTestLua::SCRIPT_RUN ) );
  new FXMenuSeparator( m_ScriptMenu );
  new FXMenuCommand( m_ScriptMenu, "Vymazat buffer", (*m_icons)[ "Actions/page-simple.png" ], this, FXSEL( SEL_COMMAND, FXTestLua::BUFFER_CLEAR ) );
  new FXMenuCommand( m_ScriptMenu, "Velikost bufferu", (*m_icons)[ "Actions/office-chart-pie.png" ], this, FXSEL( SEL_COMMAND, FXTestLua::BUFFER_STATUS ) );
  new FXMenuTitle( m_menu, "Lua", NULL, m_ScriptMenu );

  m_ConfigMenu = new FXMenuPane( m_menu );
  new FXMenuCommand( m_ConfigMenu, "Zmenit pracovni adresar", (*m_icons)[ "Actions/document-open.png" ], this, FXSEL( SEL_COMMAND, FXTestLua::CONFIG_WORKDIR ) );
  new FXMenuSeparator( m_ConfigMenu );
  new FXMenuCommand( m_ConfigMenu, "Lua moduly", (*m_icons)[ "Actions/document-open-folder.png" ], this, FXSEL( SEL_COMMAND, FXTestLua::SCRIPT_SELECT ) );
  new FXMenuTitle( m_menu, "Nastaveni", NULL, m_ConfigMenu );
*/
  FXHorizontalFrame *_outbar = new FXHorizontalFrame( _content, FRAME_LINE | LAYOUT_FILL, 0, 0, 0, 0,  0, 0, 0, 0,  0, 0 );
  m_output = new FXText( _outbar, NULL, 0, LAYOUT_FILL );
  m_output->setVisibleRows( 50 );
  m_output->setText( FXString::null );
  m_output->setEditable( m_input );
  m_output->setBackColor( m_out_bg );
  m_output->setTextColor( m_out_fg );
  if( !m_out_font.empty( ) ) {
    FXFont *out_font = new FXFont( getApp( ), m_out_font );
    m_output->setFont( out_font );
  }

  /*FXStatusBar *_statusbar = */ new FXStatusBar( content, FRAME_NONE | LAYOUT_FILL_X );
//  FXHorizontalFrame *_filebar = new FXHorizontalFrame( _statusbar, FRAME_NONE /*| LAYOUT_FILL_X*/ );
//  new FXButton( _filebar, "\t\tOpen script", (*m_icons)[ "Actions/document-open.png" ], this, FXTestLua::SCRIPT_SELECT, LAYOUT_SIDE_LEFT | BUTTON_TOOLBAR );
//  m_runBtn = new FXButton( _filebar, "\t\tRun", (*m_icons)[ "Actions/media-playback-start.png" ], this, FXTestLua::SCRIPT_RUN, LAYOUT_SIDE_LEFT | BUTTON_TOOLBAR );
//  new FXVerticalSeparator( _filebar );
//  new FXLabel( _filebar, "Script file: ", NULL, LABEL_NORMAL );
//  m_label = new FXLabel( _filebar, FXString::null, NULL, JUSTIFY_LEFT | LAYOUT_FILL );



/*
  new FXButton( _statusbar, "\t\tOpen script", (*m_icons)[ "Actions/document-open.png" ], this, FXTestLua::SCRIPT_SELECT,        LAYOUT_SIDE_LEFT | BUTTON_TOOLBAR );
  m_runBtn = new FXButton( _statusbar, "\t\tRun", (*m_icons)[ "Actions/media-playback-start.png" ], this, FXTestLua::SCRIPT_RUN, LAYOUT_SIDE_LEFT | BUTTON_TOOLBAR );
  new FXVerticalSeparator( _statusbar );
  new FXLabel( _statusbar, "Script file: ", NULL, LABEL_NORMAL );
  m_label = new FXLabel( _statusbar, FXString::null, NULL, JUSTIFY_LEFT | LAYOUT_FILL );
*/
  /// Making a Lua state
  if( ( m_lua = luaL_newstate( ) ) != NULL ) {
    luaL_openlibs( m_lua );
    m_init = true;

    msgBox_init( );
    lua_register( m_lua, "print", l_test_print );
    lua_register( m_lua, "input", l_test_input );
    lua_register( m_lua, "clear", l_test_clear );
  }
  else { m_init = false; }

  // Starting threads
  m_threads.start( 5 );
}

FXTestLua::~FXTestLua( )
{
  writeConfig( );
  if( m_init == true ) { lua_close( m_lua ); }

}

void FXTestLua::create( )
{
  FXGWindow::create( );
  show( PLACEMENT_SCREEN );

  if( !m_script.empty( ) ) { this->handle( this, FXSEL( SEL_COMMAND, FXTestLua::SCRIPT_RUN ), NULL ); }
}

void  FXTestLua::ErrorMessage( FXint num, const FXString msg )
{
   FXString err_text;
   FXString err_head = "Script runtime error ( ";
   err_head += FXString::value( num ) + " )";

   if( msg.empty( ) ) {
     err_text = lua_tostring( m_lua, -1 );
     lua_pop( m_lua, 1 );
   }
   else { err_text = msg; }

   output( err_head + "\n" + err_text );
   FXMessageBox::error(this, MBOX_OK, err_head.text( ), err_text.text( ) );
}

void FXTestLua::Error( const FXString &err_string )
{
   lua_pushstring( m_lua, err_string.text( ) );
   lua_error( m_lua );
}

void FXTestLua::output( const FXString &value, FXbool nonl )
{
  FXString endl = FXString::null;
  if( ( nonl == false ) && ( value.at( value.length( ) ) != '\n' ) ) { endl = "\n"; }
  m_output->appendText( value + endl );
  m_output->handle( this, FXSEL( SEL_COMMAND, FXText::ID_SCROLL_BOTTOM ), NULL );
}

FXString FXTestLua::input( const FXString &label )
{
  FXString resh  = FXString::null;
  FXString title = "INPUT COMMAND";
  FXInputDialog::getString( resh, this, title, label, NULL );
  return resh;
}

void FXTestLua::msgBox_init( )
{
  // Creating table for button type MessageBox values
  lua_newtable( m_lua );
  lua_pushnumber( m_lua, 1 /*MBOX_OK*/ );
  lua_setfield( m_lua, -2, "ok" );
  lua_pushnumber( m_lua, 2 /*MBOX_OK_CANCEL*/ );
  lua_setfield( m_lua, -2, "ok_cancel" );
  lua_pushnumber( m_lua, 3 /* MBOX_YES_NO*/ );
  lua_setfield( m_lua, -2, "yes_no" );
  lua_pushnumber( m_lua, 4 /*MBOX_YES_NO_CANCEL*/ );
  lua_setfield( m_lua, -2, "yes_no_cancel" );
  lua_pushnumber( m_lua, 5 /* MBOX_QUIT_CANCEL*/ );
  lua_setfield( m_lua, -2, "quit_cancel" );
  lua_pushnumber( m_lua, 6 /*MBOX_QUIT_SAVE_CANCEL*/ );
  lua_setfield( m_lua, -2, "quit_save_cancel" );
  lua_pushnumber( m_lua, 7 /*MBOX_SKIP_SKIPALL_CANCEL*/ );
  lua_setfield( m_lua, -2, "skip_skipall_cancel" );
  lua_pushnumber( m_lua, 8 /*MBOX_SAVE_CANCEL_DONTSAVE*/ );
  lua_setfield( m_lua, -2, "save_cancel_dontsave" );
  lua_pushnumber( m_lua, 9 /*MBOX_YES_YESALL_NO_NOALL_CANCEL*/ );
  lua_setfield( m_lua, -2, "yes_yesall_no_noall_cancel" );
  lua_setglobal( m_lua, "mbox_buttons" );

  lua_newtable( m_lua );
  lua_pushnumber( m_lua, 1 );
  lua_setfield( m_lua, -2, "information" );
  lua_pushnumber( m_lua, 2 );
  lua_setfield( m_lua, -2, "warnning" );
  lua_pushnumber( m_lua, 3 );
  lua_setfield( m_lua, -2, "error" );
  lua_pushnumber( m_lua, 4 );
  lua_setfield( m_lua, -2, "question" );
  lua_setglobal( m_lua, "mbox_type" );

  lua_register( m_lua, "message", l_test_message );
}

void FXTestLua::readConfig( )
{
  
  FXApp *a = getApp( );
  FXString icons_map;  

  /* Read the registry settings */
  m_opendir  = a->reg( ).readStringEntry( "Core", "workdir",      FXSystem::getHomeDirectory( ).text( ) );
  m_input    = a->reg( ).readBoolEntry(   "Core", "input",        false );

  m_editor   = a->reg( ).readStringEntry( "Tools", "editor",      "/usr/bin/adie" );
  m_editwait = a->reg( ).readBoolEntry(   "Tools", "editor.wait", false );

  m_out_bg     = a->reg( ).readColorEntry(  "Theme", "out.bg",   a->getBackColor( ) );
  m_out_fg     = a->reg( ).readColorEntry(  "Theme", "out.fg",   a->getForeColor( ) );
  m_out_font   = a->reg( ).readStringEntry( "Theme", "out.font", FXString::null );
  m_icons_name = a->reg( ).readStringEntry( "Theme", "icons",    "Oxygen" );
  icons_map   = a->reg( ).readStringEntry( "Theme", "imap",    "/opt/FXLuaConsole/data/icons.map" );

  /* Parsing the cmd arguments */
  FXint num = a->getArgc( );
  if( num > 1 ) {
    for( FXint i = 0; i != num; i++ ) {
      FXString data = a->getArgv( )[ i ];
      if( ( data == "-s" ) or ( data == "--script" ) ) {
        i++;
        m_script = a->getArgv( )[ i ];
      }
    }
  }

  if( !m_icons_name.empty( ) ) { 
    m_icons_th->load( icons_map , m_icons_name );
  } 
}

void FXTestLua::writeConfig( )
{
  FXApp *a = getApp( );

  // Wreite the regostry settings
  a->reg( ).writeStringEntry( "Core", "workdir",      m_opendir.text( ) );
  a->reg( ).writeBoolEntry(   "Core", "input",        m_input );

  a->reg( ).writeStringEntry( "Tools", "editor",      m_editor.text( ) );
  a->reg( ).writeBoolEntry(   "Tools", "editor.wait", m_editwait );

  a->reg( ).writeColorEntry(  "Theme", "out.fg",      m_output->getTextColor( ) );
  a->reg( ).writeColorEntry(  "Theme", "out.bg",      m_output->getBackColor( ) );
  a->reg( ).writeStringEntry( "Theme", "out.font",    m_output->getFont( )->getFont( ).text( ) );
  a->reg( ).writeStringEntry( "Theme", "icons",       m_icons_name.text( ) );

  a->reg( ).write( );
}

long FXTestLua::onCmd_script( FXObject *tgt, FXSelector sel, void *data )
{
  FXString file;

  switch( FXSELID( sel ) ) {
    case FXTestLua::SCRIPT_NEW : {
      FXString filename = FXString::null;
      FXFileDialog dlg( this, "Vytvorit novy soubor..." );
      dlg.setDirectory( m_opendir );
      if( dlg.execute( ) ) {
        filename = dlg.getFilename( );
        if( FXStat::exists( filename ) != true ) { FXFile::create( filename ); }
        this->ScriptEdit( filename );
        m_opendir = dlg.getDirectory( );
      }
      break;
    }
    case FXTestLua::SCRIPT_SELECT : {
      FXFileDialog dlg( this, "Vyberte soubor..." );
      dlg.setDirectory( m_opendir );
      if( dlg.execute( ) ) {
        m_script = dlg.getFilename( );
        m_opendir = dlg.getDirectory( );
        getHeader( )->setText( FXPath::name( m_script ) );
      }
      break;
    }
    case FXTestLua::SCRIPT_RUN : {
      //ScriptRun( );
      //m_threads.start( 2 );
      m_threads.execute( new FXLuaScript( m_lua, m_script ) );
      //m_threads.stop();
      //script.run( );
      break;
    }
    case FXTestLua::SCRIPT_EDIT : {
      this->ScriptEdit( );
      break;
    }
  }
  return 1;
}

long FXTestLua::onCmd_buffer( FXObject *tgt, FXSelector sel, void *data )
{
  long resulth = 1;
  FXString msg;

  switch( FXSELID( sel ) ) {
    case FXTestLua::BUFFER_STATUS : {
      msg = "Aktualni velikost bufferu: ";
      msg += FXString::value( m_output->getLength( ) );
      FXMessageBox::information( this, MBOX_OK, "Buffer info: ", msg.text( ) );
      break;
    }
    case FXTestLua::BUFFER_CLEAR : {
      m_output->setText( FXString::null );
      break;
    }
    case FXTestLua::BUFFER_SAVE : {
      FXString filename = FXString::null;
      FXFileDialog dlg( this, "Ulozit vysledky..." );
      if( dlg.execute( ) ) { filename = dlg.getFilename( ); }
      if( FXStat::exists( filename ) != true ) { FXFile::create( filename ); }
      FXFile fd( filename, FXIO::WriteOnly );
      if( fd.isOpen( ) ) {
        FXString buffer = m_output->getText( );
        fd.writeBlock( (void*) buffer.text( ), buffer.length( ) );
        fd.close( );
      }
      break;
    }
  }

  return resulth;
}

long FXTestLua::onCmd_config( FXObject *tgt, FXSelector sel, void *data )
{
  long resulth = 1;
  FXString msg;

  switch( FXSELID( sel ) ) {
    case FXTestLua::CONFIG_WORKDIR : {
      FXDirDialog dlg( this, "Vyber pracovni adresar" );
      dlg.setDirectory( FXSystem::getCurrentDirectory( ) );
      if( dlg.execute( ) ) { FXSystem::setCurrentDirectory( dlg.getDirectory( ) ); }
    }
  }

  return resulth;
}

FXint FXTestLua::ScriptEdit( const FXString &file )
{
  FXint     code = -1;
  FXProcess editor;
  FXString  f = ( ( file.empty( ) == true ) ? m_script : file );


  if( !f.empty( ) ) {
    const FXchar *const args[ ] = { m_editor.text( ), f.text( ), NULL };
    if( editor.start( m_editor.text( ), args ) ) {
      if( m_editwait == true ) { editor.wait( code ); }
      else{ code = 0; }
    }
  }
  if( code == 0 ) { m_script = f; }

  return code;
}

FXbool FXTestLua::ScriptRun( )
{

  FXbool resh = true;
  /*

  FXLuaScript script( m_lua, m_script );
  if( script.run( ) != 0 ) {
    this->ErrorMessage( resh );
    resh = false;
  }
  */
  return resh;
}
/*************************************************************************************************/
/// LUA API
FXTestLua *win;

int static l_test_print( lua_State *L )
{
  // Kontrola poctu parametru, inicializace datoveho bufferu
  FXString data = "";
  FXint n, num = lua_gettop( L );

  if( num < 1 ) { data = "\n"; }  // neni-li zadan zadny parametr, pouze se provede odradkovani
  else {
  // V opacnem pripade se projdou vsechny parametry funkce a kazdy z nich se ekvivalentnim zpusobem
  // prevede na retezec a ulozi do datoveho bufferu funkce
    n = num + 1;
    for( int i = 1; i != n; i++ ) {
      if( i > 1 ) { data += " \t"; }
      switch( lua_type( L, i ) )  {
        case LUA_TBOOLEAN : {
          if( lua_toboolean( L, i ) == 1 ) { data += "true"; } else { data += "false"; }
          break;
        }
        case LUA_TNIL : {
          data += "nil";
          break;
        }
        case LUA_TNUMBER : {
          data += FXString::value( (double) lua_tonumber( L, i ) );
          break;
        }
        case LUA_TSTRING : {
          data += lua_tostring( L, i );
          break;
        }
        default : {
          data += lua_typename( L, i );
        }
      }
    }
  }

  // Predani datoveho bufferu objektu aplikace, implementujiciho pozadovanou funkcnost
  win->output( data );
  return 0;
}

int static l_test_input( lua_State *L )
{
   FXString caption = "Zadejte, prosim, pozadovanou hodnotu: ";

   if( lua_gettop( L ) == 1 ) {
     switch( lua_type( L, 1 ) ) {
       case LUA_TNUMBER : {
         caption = FXString::value( (double) lua_tonumber( L, 1 ) );
         break;
       }
       case LUA_TSTRING : {
         caption = lua_tostring( L, 1 );
         break;
       }
     }
   }

   FXString data = win->input( caption );

   if( data.empty( ) ) { lua_pushnil( L ); }
   else { lua_pushstring( L, data.text( ) ); }
   return 1;
}

int static l_test_message( lua_State *L )
{
  FXString msg, title, actived;
  FXint num, resh, mtype;
  FXuint buttons;

  if( ( num = lua_gettop( L )  ) < 1 ) {
    win->Error( "MessageBox: Malo argumentu - nutno zadat alespon text zpravy!" );
    return 0;
  } else { msg = lua_tostring( L, 1 );  }
  if( num >= 2 ) { title = lua_tostring( L, 2 ); } else { title = FXString::null; }
  if( num >= 3 ) { //buttons = ( FXuint ) lua_tonumber( L, 3 );
    switch( ( FXint ) lua_tonumber( L, 3 ) ) {
      case 1 : buttons = MBOX_OK;                         break;
      case 2 : buttons = MBOX_OK_CANCEL;                  break;
      case 3 : buttons = MBOX_YES_NO;                     break;
      case 4 : buttons = MBOX_YES_NO_CANCEL;              break;
      case 5 : buttons = MBOX_QUIT_CANCEL;                break;
      case 6 : buttons = MBOX_QUIT_SAVE_CANCEL;           break;
      case 7 : buttons = MBOX_SKIP_SKIPALL_CANCEL;        break;
      case 8 : buttons = MBOX_SAVE_CANCEL_DONTSAVE;       break;
      case 9 : buttons = MBOX_YES_YESALL_NO_NOALL_CANCEL; break;
    }
  } else { buttons = MBOX_OK; }
  if( num == 4 ) { mtype =  ( int ) lua_tonumber( L, 4 ); } else { mtype = 1; }

  switch( mtype ) {
    case 1 : resh = FXMessageBox::information( win, buttons, title.text( ), msg.text( ) ); break;
    case 2 : resh = FXMessageBox::warning( win, buttons, title.text( ), msg.text( ) );     break;
    case 3 : resh = FXMessageBox::error( win, buttons, title.text( ), msg.text( ) );       break;
    case 4 : resh = FXMessageBox::question( win, buttons, title.text( ), msg.text( ) );    break;
  }

  switch( resh ) {
    case 1  : actived = "yes";      break;
    case 2  : actived = "no";       break;
    case 3  : actived = "ok";       break;
    case 4  : actived = "cancel";   break;
    case 5  : actived = "quit";     break;
    case 6  : actived = "save";     break;
    case 7  : actived = "skip";     break;
    case 8  : actived = "skip_all"; break;
    case 9  : actived = "yes_all";  break;
    case 10 : actived = "no_all";   break;
  }

  lua_pushstring( L, actived.text( ) );
  return 1;
}

int l_test_clear( lua_State *L ) { return (int) win->handle( win, FXSEL( SEL_COMMAND, FXTestLua::BUFFER_CLEAR ), NULL ); }




/*************************************************************************************************/
/// main

int main( int argc, char **argv ) {
  FXApp a( "FXLuaApp", "FXLuaKonsole" );
  XInitThreads();				// Suport xlib for multi-threads processing
  a.init( argc, argv );
  win = new FXTestLua( &a );
  a.create( );
  return a.run( );
}





