set(This lang)
set(Sources 
    lox.cpp
    scanner.cpp
    interpreter.cpp
    AstPrinter.cpp
)

add_library(${This} ${Sources})
find_package(folly CONFIG REQUIRED)
include_directories(${FOLLY_INCLUDE_DIR})

target_link_libraries(cpploxlib ${FOLLY_LIBRARIES})