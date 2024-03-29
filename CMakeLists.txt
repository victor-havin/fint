CMAKE_MINIMUM_REQUIRED(VERSION 3.7 FATAL_ERROR)
project(fint)
# vlh
#include(FetchContent)
#
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# compiler must be 17
set(CMAKE_CXX_STANDARD 17)

# required if linking to static library
#add_definitions(-DANTLR4CPP_STATIC)
#add_definitions(-DANTLR4CPP_SHARED)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)

# Specify the version of the antlr4 library needed for this project.
# By default the latest version of antlr4 will be used.  You can specify a
# specific, stable version by setting a repository tag value or a link
# to a zip file containing the libary source.
set(ANTLR4_TAG 4.13.1)
# set(ANTLR4_ZIP_REPOSITORY https://github.com/antlr/antlr4/archive/refs/tags/4.13.1.zip)

# add external build for antlrcpp
include(ExternalAntlr4Cpp)

#vlh
#FetchContent_Declare(antlr
#  INSTALL_DIR "~/test/antlr/antlr4-dev/runtime/Cpp"
#)


# add antlr4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})

# vlh
#include_directories(${INSTALL_DIR}/runtime/src/)
#link_directories(${INSTALL_DIR}/runtime/dist)
#

# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
set(ANTLR_EXECUTABLE /home/victor/test/antlr/antlr-4.13.1-complete.jar)
# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

# Call macro to add lexer and grammar to your build dependencies.
antlr_target(fintLexer fint.g4 LEXER
             PACKAGE fint)
antlr_target(fintParser fint.g4 PARSER 
             PACKAGE fint)

# include generated files in project environment
#include_directories(${ANTLR_SampleGrammarLexer_OUTPUT_DIR})
#include_directories(${ANTLR_SampleGrammarParser_OUTPUT_DIR})
include_directories(${ANTLR_fintLexer_OUTPUT_DIR})
include_directories(${ANTLR_fintParser_OUTPUT_DIR})

# add generated grammar to demo binary target
#add_executable(calc main.cpp
#               ${ANTLR_SampleGrammarLexer_CXX_OUTPUTS}
#               ${ANTLR_SampleGrammarParser_CXX_OUTPUTS})
add_executable(fint fint.cpp
                fintLexer.cpp
                fintParser.cpp)
#               ${ANTLR_fintLexer_CXX_OUTPUTS}
#               ${ANTLR_fintParser_CXX_OUTPUTS})
target_link_libraries(fint antlr4_static)
