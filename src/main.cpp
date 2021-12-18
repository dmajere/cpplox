#include <iostream>
#include <gflags/gflags.h>

#include "Lox/interpreter.h"

DEFINE_string(file, "", "Script file path");

int main(int argc, char** argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    auto lox = lox::lang::Interpreter();

    if (!FLAGS_file.empty()) {
        lox.runFromFile(FLAGS_file);
    } else {
        lox.runPrompt();
    }
}