cmake_minimum_required(VERSION 2.8.11)

project(volumetric_de)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


include_directories("${PROJECT_BINARY_DIR}")

find_package(OpenGL REQUIRED)
set(LIBRARIES ${LIBRARIES} ${OPENGL_gl_LIBRARY})

find_package(glm CONFIG REQUIRED)
include_directories(${GLM_INCLUDE_DIR})
set(LIBRARIES ${LIBRARIES} glm)

find_package(RapidJSON REQUIRED)
SET(INCLUDES ${INCLUDES} ${RAPIDJSON_INCLUDE_DIRS})

find_package(SDL2 CONFIG REQUIRED)
set(LIBRARIES ${LIBRARIES} SDL2::SDL2main SDL2::SDL2-static)

find_package(sqlite3 CONFIG REQUIRED)
set(LIBRARIES ${LIBRARIES} sqlite3)

find_package(GLEW REQUIRED)
set(LIBRARIES ${LIBRARIES} GLEW::GLEW)

# Turn on colour output.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
endif()

include_directories(skeleton)

file(GLOB SOURCES skeleton/*)

file(GLOB_RECURSE shaders RELATIVE ${CMAKE_SOURCE_DIR} skeleton/shaders/*)
file(GLOB_RECURSE data RELATIVE ${CMAKE_SOURCE_DIR} skeleton/data/*)
foreach(file ${shaders} ${data})
    configure_file(${file} ${file} COPYONLY)
endforeach(file)

add_executable(volumetric_de ${SOURCES})
target_compile_definitions(volumetric_de PRIVATE GLM_FORCE_CXX14=1 _USE_MATH_DEFINES=1 GLM_ENABLE_EXPERIMENTAL=1)
target_link_libraries(volumetric_de PRIVATE ${LIBRARIES} )
target_include_directories(volumetric_de PRIVATE ${INCLUDES})

