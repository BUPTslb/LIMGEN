cmake_minimum_required(VERSION 3.23)
project(Synthesis)

set(CMAKE_CXX_STANDARD 14)
set(INC_DIR  ./include)
add_executable(Synthesis
        TriLogic/Func_main.cpp
        TriLogic/logic_func.cpp
        TriLogic/logic_func.h
        TriLogic/rapidjson/document.h

        )
