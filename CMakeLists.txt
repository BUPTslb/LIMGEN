cmake_minimum_required(VERSION 3.23)
project(Synthesis)
set(CMAKE_BUILD_TYPE Debug)

#下面是原来的
set(CMAKE_CXX_STANDARD 14)
set(INC_DIR  ./include)
add_executable(Synthesis
        TriLogic/Func_main.cpp
        TriLogic/arr_size.cpp
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
        TriLogic/onlyArc/only_lut.cpp
        TriLogic/onlyArc/only_sa.cpp
        TriLogic/onlyArc/only_magic.cpp
        TriLogic/dse.h TriLogic/build.cpp TriLogic/find_input.cpp TriLogic/data_read.cpp TriLogic/out_degree.cpp TriLogic/num_node_position.cpp TriLogic/op_row_need.cpp)

