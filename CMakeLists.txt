cmake_minimum_required(VERSION 3.19.2)
set(CMAKE_CXX_STANDARD 17)
enable_testing()

set(This cpploxlib)
project(cpplox)

set(Sources 
${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)


add_library(${This} STATIC ${Sources})
add_subdirectory(src/Lox)
find_package(gflags REQUIRED)
target_link_libraries(${This} ${GFLAGS_LIBRARIES} lang)

add_subdirectory(test)

add_executable(cpplox ${Sources})
target_link_libraries(cpplox cpploxlib)
