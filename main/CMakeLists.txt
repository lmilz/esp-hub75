# Main component - links the selected application

if(APP STREQUAL "blank")
  set(APP_SRC "../apps/blank/main.cpp")
elseif(APP STREQUAL "matrix")
  set(APP_SRC "../apps/matrix/main.cpp")
elseif(APP STREQUAL "gol")
  set(APP_SRC "../aps/game_of_life/main.cpp")
endif()

idf_component_register(SRCS ${APP_SRC}
	INCLUDE_DIRS "."
  REQUIRES hub75)
