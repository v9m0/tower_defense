cmake_minimum_required(VERSION 3.15)
project(TowerDefenseKursovaya LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
