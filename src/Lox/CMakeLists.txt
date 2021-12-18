add_library(lang interpreter.cpp scanner.cpp)
find_package(folly CONFIG REQUIRED)
include_directories(${FOLLY_INCLUDE_DIR})

target_link_libraries(cpploxlib ${FOLLY_LIBRARIES})