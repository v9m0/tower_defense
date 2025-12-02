cmake_minimum_required(VERSION 3.15)
project(TowerDefenseKursovaya LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-fsanitize=address,undefined)
  add_link_options(-fsanitize=address,undefined)
endif()


if(APPLE)
  foreach(_sfml_prefix IN ITEMS "/opt/homebrew/opt/sfml" "/opt/homebrew/opt/sfml@2" "/usr/local/opt/sfml" "/usr/local/opt/sfml@2")
    if(EXISTS "${_sfml_prefix}/lib/cmake/SFML")
      list(APPEND CMAKE_PREFIX_PATH "${_sfml_prefix}")
    endif()
  endforeach()
endif()

find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB HEADERS CONFIGURE_DEPENDS include/*.hpp)

add_executable(tower_defense ${SOURCES} ${HEADERS})

target_include_directories(tower_defense PRIVATE include)
target_link_libraries(tower_defense PRIVATE sfml-graphics sfml-window sfml-system)


add_custom_command(TARGET tower_defense POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets
          $<TARGET_FILE_DIR:tower_defense>/assets
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/data
          $<TARGET_FILE_DIR:tower_defense>/data
)
