#include "FXIconsTheme.h"
#include<iostream>

FXIconsTheme::FXIconsTheme( FXApp *app )
{
  t_cache = new FXIconCache( app, FXSystem::getHomeDirectory( ) );
}

FXIconsTheme::~FXIconsTheme( )
{

}

/*************************************************************************************************/
FXString FXIconsTheme::operator[ ]( const FXString &input )
{
  /*NOTHING*/
  return FXString::null;
}


FXString FXIconsTheme::at( const FXString &name, int size )
{
  FXString resh = t_pattern;
  FXString tr = t_dict[ name ];

  if( !tr.empty( ) ) {
    FXint pos = tr.find( "/" );
    FXString i_sect = tr.left( pos );
    FXString i_name = tr.mid( pos + 1, tr.length( ) );

    // We have everything we need - We will start replacing the pattern with values
    resh.substitute( "${SIZE}", FXString::value( size ) );
    resh.substitute( "${TYPE}", i_sect );
    resh.substitute( "${NAME}", i_name );
    resh.substitute( "${EXP}",  t_prefix ); 
  }
  
  return resh;
}

FXIcon* FXIconsTheme::get_icon( const FXString &name, const FXString &size_alias )
{
   FXint _size = 16;
   if( !_sizes.empty( ) ) { _size = _sizes.at( size_alias ); }
   return get_icon( name, _size );
}

void FXIconsTheme::load( const FXString &themefile, const FXString &imap )
{
  FXSettings data;
  FXString   head;

  t_dict.clear( );
  t_sizes.clear( );
  t_cache->clear( );
  t_list.clear( );

  if( FXStat::exists( themefile ) && data.parseFile( themefile ) ) {
    std::cout << "Load icons theme \'" << themefile.text( ) << "\'" << std::endl;

    if ( data.existingSection( "Sizes" ) ) {
      FXStringDictionary sm = data.at( "Sizes" );
      for( FXint i = 0; i != sm.no( ); i++ ) {
        FXString k = sm.key( i );
        if( !k.empty( ) ) {
          FXint value =  sm.data( i ).toInt( );
          _sizes.insert( std::pair<FXString, int>( k, value ) );
        }
      }
    } 
  /*
    if ( data.existingSection( "Maps" ) ) {
      FXString _thlist_str = data.readStringEntry( "Maps", "list" ); 
      FXint    _num = ( _thlist_str.contains( ',' ) ) + 1; 
 
      for( FXint i = 0; i != _num; i++ ) {
        FXString _sect = _thlist_str.section( ',', i );
        t_list.append( _sect );
      }
    }  
  */

    // Read base informations about icon theme
    t_name    = imap; 
    if( data.existingSection( t_name ) ) {
      t_prefix  = data.readStringEntry( t_name, "map.type", "png" );
      t_path    = data.readStringEntry( t_name, "map.path" );
      t_pattern = data.readStringEntry( t_name, "map.pattern" );

      // Read a icons dictionary
      t_dict = data.at( t_name ); 
    }
  } 
  else { std::cout << "Load icons theme \'" << themefile.text( ) << "\' is NOT LOADED!" << std::endl;}

  std::cout.flush( );
}
