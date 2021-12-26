set(This lang)
set(Sources 
    utils.cpp
    lox.cpp
    Interpreter.cpp
    AstPrinter.cpp
    Resolver.cpp
    LoxInstance.cpp
)

add_library(${This} ${Sources})
find_package(folly CONFIG REQUIRED)
include_directories(${FOLLY_INCLUDE_DIR})

target_link_libraries(cpploxlib ${FOLLY_LIBRARIES})
