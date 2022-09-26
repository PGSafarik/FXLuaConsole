CMAKE_MINIMUM_REQUIRED( VERSION 3.10 )
PROJECT( FXLuaConsole VERSION 0.1 DESCRIPTION "The GUI for games and an launchers" )

# The project settings
SET( CMAKE_INSTALL_PREFIX "/opt/${PROJECT_NAME}" )  # Install target directory
SET( LIB_FOX   "/opt/foxlib/1.7/64" )       # Fox toolkit path
SET( LIB_FXGHI "/opt/foxghi/0.2/1"  )       # Fox GHI path
SET( CMAKE_CXX_STANDARD 98 )                # C++ Standard
SET( CMAKE_CXX_STANDARD_REQUIRED True )     # C++ Standard
#add_compile_options(-Wall -Wextra -pedantic -Werror)
#add_compile_options(-Wall -pedantic)

# Configure project
CONFIGURE_FILE( MakeConfig.h.in MakeConfig.h )
FILE( GLOB SOURCES src/*.cpp )
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/include" )

# Add directories for project dependecies
INCLUDE_DIRECTORIES( "${LIB_FOX}/include/" )
LINK_DIRECTORIES(    "${LIB_FOX}/lib/" )
INCLUDE_DIRECTORIES( "${LIB_FXGHI}/include/" )
LINK_DIRECTORIES(    "${LIB_FXGHI}/lib/" )

# Add to the executable
ADD_EXECUTABLE( ${PROJECT_NAME} ${SOURCES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} X11 FOX-1.7 FXGHI lua5.3-c++ )
TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}" )

# Install build project
INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION . )
INSTALL( DIRECTORY data DESTINATION ${CMAKE_INSTALL_PREFIX} )		# Globalni data
#INSTALL( FILES integration/medusa-head.png DESTINATION /usr/share/pixmaps/ )
#INSTALL( FILES integration/Gorgona.desktop DESTINATION /usr/share/applications/ )

### END ###
