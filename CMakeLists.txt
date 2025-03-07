cmake_minimum_required(VERSION 3.10)
project(Pong VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set source files (not includng the main file)
set(SOURCES 
    src/entities.cpp
    src/2dvector.cpp
    src/properties.cpp
)

# Set header files
set(HEADERS
    src/entities.h
    src/2dvector.h
    src/properties.h
)

# Find SDL2 libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)

# primary source file lkibrary
add_library(mainLib STATIC ${SOURCES})
target_include_directories(mainLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Create the executable and include main
add_executable(${PROJECT_NAME} src/main.cpp)

# Link the executable to the library made previously
target_link_libraries(${PROJECT_NAME} PRIVATE mainLib)

# Use target-specific include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_mixer_INCLUDE_DIRS}
    ${SDL2_ttf_INCLUDE_DIRS}
    ${SDL2_image_INCLUDE_DIRS}
)

# Link SDL2 libraries to the exe
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
    SDL2_image::SDL2_image
)

# Handle MinGW-specific linking if necessary
if(MINGW)
    target_link_libraries(${PROJECT_NAME} PRIVATE
    -lmingw32
    mingw32 
    SDL2::SDL2main 
    SDL2::SDL2 
    -lSDL2_image
    -lSDL2_ttf
    -lSDL2_mixer
    )
endif()
