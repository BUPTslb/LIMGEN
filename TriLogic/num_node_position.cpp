#include"logic_func.h"
//数据存储的位置有几个
int num_node_position(Nodes *now) {
    int num = 0;
    for (const auto &i: now->wb_pos) {
        if (!i.empty())
            num += i.size();
    }
    return num;
}
//
// Created by Lenovo on 2023/6/28.
//
