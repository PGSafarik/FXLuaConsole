#include<iostream>
#include<fox-1.7/fx.h>

class IconsTheme : public FXDictionaryOf<FXIconCache> {
//FXDECLARE( IconsTheme )
  FXApp    *it_app;      // Ukazatel na instanci aplikace
  FXString  it_folder;   // Defaultni slozka
  FXbool    it_creating; //
  FXString  it_size;     // Velikost ikon

public:
  IconsTheme( FXApp *a, const FXString &folder );
  virtual ~IconsTheme( );

  virtual void create( ) { it_creating = true; }
  FXIconCache* insert( const FXString &key, const FXString &dir );
  FXIconCache* replace( const FXString &key, const FXString &dir );
  FXIconCache* remove( const FXString &key );
  FXIconCache* find( const FXString &key );
  FXIconCache* data( FXint pos );

  inline FXIcon* getIcon( const FXString &type_key, const FXString &icon_key )
  {
    FXIcon      *icon  = NULL;
    FXIconCache *cache = FXDictionaryOf<FXIconCache>::data( FXDictionaryOf<FXIconCache>::find( type_key ) );

    //std::cout << cache->getIconPath( ).text( ) << std::endl;
    //std::cout.flush( );

    if( cache != NULL ) {
      icon = cache->find( icon_key.text( ) );
      if( !icon ) {
        icon = cache->insert( icon_key.text( ) );
        if( it_creating == true ) { icon->create( ); }
      }
    }

    /*
    if( icon == NULL ) {
    std::cout << "Ikona" << ( cache->getIconPath( ) + icon_key ).text( ) << "Nebyla vytvorena!" << std::endl;
    std::cout.flush( );
    }*/

    return icon;
  }

  FXIcon *operator []( const FXString &ident )
  {
     /// theme[ "actions/close.png" ];
     //std::cout << ident.section( "/", 0, 1 ).text( ) << std::endl;
     return getIcon( ident.section( "/", 0, 1 ), ident.section( "/", 1, 1 ) );
  }

protected:
  IconsTheme( ) { }
};
