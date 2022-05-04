find_package(Curses REQUIRED)

message(WARNING "${CURSES_FOUND} ${CURSES_LIBRARIES} ${CURSES_INCLUDE_DIRS}")

add_library(CursesLibrary SHARED IMPORTED)
find_program(THE_CURSES_DLL PDCurses.dll PATHS ${CURSES_LIBRARIES} PATH_SUFFIXES ..)
target_include_directories(CursesLibrary INTERFACE ${CURSES_INCLUDE_DIRS})
set_target_properties(CursesLibrary PROPERTIES IMPORTED_IMPLIB_DEBUG "${CURSES_LIBRARIES}/pdcurses.lib" IMPORTED_LOCATION_DEBUG "${THE_CURSES_DLL}" )
set_target_properties(CursesLibrary PROPERTIES IMPORTED_IMPLIB "${CURSES_LIBRARIES}/pdcurses.lib" IMPORTED_LOCATION "${THE_CURSES_DLL}" )
