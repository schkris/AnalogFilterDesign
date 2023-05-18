cmake_minimum_required(VERSION 3.0)
project(AnalogFilterDesign)

message(STATUS "CMakeLists.txt Directory: ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "Source Directory: ${CMAKE_SOURCE_DIR}")

# Add the source files to the executable
add_executable(AnalogFilterDesign main.cpp GUI.cpp FilterDesign.cpp GUI.h FilterDesign.h)

# Specify the absolute paths to SDL headers and libraries
set(SDL2_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/Libraries used/SDL2-devel-2.26.5-VC/SDL2-2.26.5/include")
set(SDL2_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/Libraries used/SDL2-devel-2.26.5-VC/SDL2-2.26.5/lib/x64/SDL2.lib")
set(SDL2MAIN_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/Libraries used/SDL2-devel-2.26.5-VC/SDL2-2.26.5/lib/x64/SDL2main.lib")
set(SDL2_TTF_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/Libraries used/SDL2_ttf-devel-2.20.2-VC/SDL2_ttf-2.20.2/include")
set(SDL2_TTF_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/Libraries used/SDL2_ttf-devel-2.20.2-VC/SDL2_ttf-2.20.2/lib/x64/SDL2_ttf.lib")

# Include directories
target_include_directories(AnalogFilterDesign PRIVATE ${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})

# Link libraries
target_link_libraries(AnalogFilterDesign PRIVATE ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY} ${SDL2_TTF_LIBRARY})
