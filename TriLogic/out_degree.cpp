#include"logic_func.h"
//更新节点的出度，一个节点入度最多是3，出度可以很大
//这个节点执行完毕了，其依赖的节点的出度就都会-1
void out_degree(Nodes *now) {
    if (now->depend1)
        now->depend1->out_degree--;
    if (now->depend2)
        now->depend2->out_degree--;
    if (now->control)
        now->control->out_degree--;
}
//
// Created by Lenovo on 2023/6/28.
//
