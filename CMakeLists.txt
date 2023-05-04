cmake_minimum_required(VERSION 3.23)
project(Synthesis)

set(CMAKE_CXX_STANDARD 14)
set(INC_DIR  ./include)
add_executable(Synthesis
        TriLogic/Func_main.cpp
        TriLogic/logic_func.cpp
        TriLogic/logic_func.h
        TriLogic/mainfunc.cpp
        TriLogic/mainfunc.h
        TriLogic/dse.cpp
        TriLogic/rapidjson/document.h
        TriLogic/decide_array.cpp
        TriLogic/time_energy.cpp
        TriLogic/cap_array.cpp
        TriLogic/do_logic.cpp
        TriLogic/pre_array_list.cpp
        TriLogic/parameter.h
        TriLogic/parameter.cpp
        TriLogic/write_in.cpp
        TriLogic/wb_table.cpp
        TriLogic/array_node.cpp
        TriLogic/topoSort.cpp
        TriLogic/control_step.cpp
        TriLogic/only_lut.cpp
        TriLogic/only_sa.cpp
        TriLogic/only_magic.cpp
        )
