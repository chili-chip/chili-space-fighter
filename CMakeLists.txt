# Basic parameters; check that these match your project / environment
cmake_minimum_required(VERSION 3.15...3.31)

# Replace "game" with a name for your project (this is used the name of the output)
project(game LANGUAGES C CXX ASM)

file(GLOB PROJECT_SOURCE src/*)

# ... and any other files you want in the release here
set(PROJECT_DISTRIBS LICENSE README.md)

# Build configuration; approach this with caution!
if(MSVC)
  add_compile_options("/W4" "/wd4244" "/wd4324" "/wd4458" "/wd4100")
else()
  add_compile_options("-Wall" "-Wextra" "-Wdouble-promotion" "-Wno-unused-parameter")
endif()

find_package (32BLIT CONFIG REQUIRED PATHS ../32blit-sdk $ENV{PATH_32BLIT_SDK})

blit_executable (${PROJECT_NAME} ${PROJECT_SOURCE})

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
else()
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")
endif()

blit_assets_yaml (${PROJECT_NAME} assets.yml)

# The story dialogue lives in docs/story/script.md and is turned into C++ on every build
set(STORY_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/docs/story/script.md)
set(STORY_CPP ${CMAKE_CURRENT_BINARY_DIR}/story_script.cpp)
add_custom_command(
  OUTPUT ${STORY_CPP}
  COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_script.py ${STORY_SCRIPT} ${STORY_CPP}
  DEPENDS ${STORY_SCRIPT} ${CMAKE_CURRENT_SOURCE_DIR}/tools/make_script.py
  VERBATIM
)
target_sources (${PROJECT_NAME} PRIVATE ${STORY_CPP})
target_include_directories (${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
blit_metadata (${PROJECT_NAME} metadata.yml)
add_custom_target (flash DEPENDS ${PROJECT_NAME}.flash)

# setup release packages
install (FILES ${PROJECT_DISTRIBS} DESTINATION .)
set (CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set (CPACK_GENERATOR "ZIP" "TGZ")
include (CPack)
