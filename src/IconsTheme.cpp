#include<IconsTheme.h>

/*************************************************************************************************/
IconsTheme::IconsTheme( FXApp *a, const FXString &folder )
          : it_app( a ), it_folder( folder ), it_creating( false )
{ }

IconsTheme::~IconsTheme( )
{ }

/*************************************************************************************************/
FXIconCache* IconsTheme::insert( const FXString &key, const FXString &dir )
{
  FXIconCache *cache = NULL;
 
  if( FXDictionaryOf<FXIconCache>::data( FXDictionaryOf<FXIconCache>::find( key ) ) == NULL ) {
    cache = new FXIconCache( it_app, dir );
    cache->setIconPath( it_folder + dir );
    if( FXDictionaryOf<FXIconCache>::insert( key, cache ) != NULL ) { return cache; }
  } 
  else { cache = replace( key, dir ); }

  return cache;
}

FXIconCache* IconsTheme::replace( const FXString &key, const FXString &dir )
{
  FXIconCache *old;
  if( ( old = FXDictionaryOf<FXIconCache>::remove( key ) ) != NULL  ) { delete old; }
  return insert( key, dir );
}

