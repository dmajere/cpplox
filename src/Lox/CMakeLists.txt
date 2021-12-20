set(This lang)
set(Sources
    interpreter.cpp
    scanner.cpp
    parser.cpp
    AstPrinter.cpp
)

add_library(${This} ${Sources})
find_package(folly CONFIG REQUIRED)
include_directories(${FOLLY_INCLUDE_DIR})

target_link_libraries(cpploxlib ${FOLLY_LIBRARIES})