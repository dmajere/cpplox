set(This lang)
set(Sources 
    utils.cpp
    LoxClass.cpp
    LoxInstance.cpp
    AstPrinter.cpp
    Interpreter.cpp
    Resolver.cpp
    lox.cpp
)

add_library(${This} ${Sources})
find_package(folly CONFIG REQUIRED)
include_directories(${FOLLY_INCLUDE_DIR})

target_link_libraries(cpploxlib ${FOLLY_LIBRARIES})
