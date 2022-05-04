cmake_minimum_required(VERSION 3.12)
set(CHIP8EMU_SOURCES "main.cpp" "chip8_emu.cpp" "JustFileThings.cpp" "AppLoop.cpp" "util/Timer.cpp")

set(CHIP8EMU_REQUIRED_LIBRARIES "")

if(WIN32 AND (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC"))
 add_compile_options("/ZI")
endif()

add_executable(chip8emu ${CHIP8EMU_SOURCES})
target_compile_features(chip8emu PRIVATE $<IF:$<COMPILE_FEATURES:cxx_std_23>,cxx_std_23,cxx_std_20>)
target_include_directories(chip8emu PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

if(NOT DEFINED CACHE{CHIP8_EMU_FORMAT_LIB})
		try_compile( USE_STD_FORMAT ${CMAKE_BINARY_DIR}/cmake/trycompile
				SOURCES ${CMAKE_SOURCE_DIR}/cmake/trycompile/try_compile_std_format.cpp
				CXX_STANDARD 20)

		if(NOT USE_STD_FORMAT)
				set(CHIP8_EMU_FORMAT_LIB "fmt" CACHE INTERNAL "format library to use")
		else()
				set(CHIP8_EMU_FORMAT_LIB "std" CACHE INTERNAL "format library to use")
		endif()
endif()

if(NOT $CACHE{CHIP8_EMU_FORMAT_LIB} STREQUAL "std")
		set(CHIP8EMU_REQUIRED_LIBRARIES ${CHIP8EMU_REQUIRED_LIBRARIES} "fmt::fmt")
		find_package(fmt CONFIG REQUIRED)
endif()

set(CHIP8EMU_BACKEND_LIBRARY_OVERRIDDEN False)

if(CHIP8EMU_USE_BACKEND_FTXUI)
 set(CHIP8EMU_BACKEND_LIBRARY_OVERRIDDEN True)
 set(CHIP8EMU_REQUIRED_LIBRARIES ${CHIP8EMU_REQUIRED_LIBRARIES} "ftxui::dom" "ftxui::component")
 find_package(ftxui CONFIG REQUIRED)
 target_sources(chip8emu PRIVATE "backend/FtxuiLoop.cpp" "app/chip8_display.cpp" "app/chip8_element.cpp" "app/chip8_component.cpp")
 target_compile_definitions(chip8emu PRIVATE CHIP8EMU_USE_BACKEND_FTXUI)
endif()
if(CHIP8EMU_USE_BACKEND_CURSES)
 if(CHIP8EMU_BACKEND_LIBRARY_OVERRIDDEN)
  message(FATAL_ERROR "Chip8Emu: Two conflicting CMake defines. Use a single -DCHIP8EMU_USE_BACKEND_<CHOICE>"
                      "where <CHOICE> is either 'FTXUI' OR 'CURSES' (no quotes needed)")
 endif()

 set(CHIP8EMU_BACKEND_LIBRARY_OVERRIDDEN True)
 set(CHIP8EMU_REQUIRED_LIBRARIES ${CHIP8EMU_REQUIRED_LIBRARIES} "DefaultCursesLibrary")
 list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
 find_package(DefaultCursesLibrary REQUIRED)

 target_sources(chip8emu PRIVATE "Drawer.cpp")
 target_compile_definitions(chip8emu PRIVATE CHIP8EMU_USE_BACKEND_CURSES)
endif()

if(NOT CHIP8EMU_BACKEND_LIBRARY_OVERRIDDEN)
 set(CHIP8EMU_REQUIRED_LIBRARIES ${CHIP8EMU_REQUIRED_LIBRARIES} "sfml-window" "sfml-graphics")
 find_package(SFML COMPONENTS window graphics CONFIG REQUIRED)
 target_sources(chip8emu PRIVATE "backend/SfmlLoop.cpp")
 target_compile_definitions(chip8emu PRIVATE CHIP8EMU_USE_BACKEND_SFML)
endif()


#add_custom_command(TARGET chip8emu POST_BUILD
#		COMMAND ${CMAKE_COMMAND} -E copy ${THE_CURSES_DLL} ${CMAKE_BINARY_DIR}/src)

target_link_libraries(chip8emu ${CHIP8EMU_REQUIRED_LIBRARIES})

#get_target_property(HELLO chip8emu LOCATION_DEBUG)



