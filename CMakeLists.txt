cmake_minimum_required(VERSION 3.21)
option(VCPKG_MANIFEST_MODE "Use to auto install dependencies within build tree. Off by default.")
project("Chip8Emu" LANGUAGES "CXX")

add_subdirectory(src)
