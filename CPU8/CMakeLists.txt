cmake_minimum_required (VERSION 3.8)

add_executable (CPU8 "CPU8.cpp"
                     "core/characterdisplay.cpp"
                     "core/cpu.cpp"
                     "core/memoryunit.cpp"
                     "core/registerbasedcpu.cpp"
                     "compiler/codegenerator.cpp"
                     "compiler/lexer.cpp")

target_compile_features(CPU8 PRIVATE cxx_std_17)