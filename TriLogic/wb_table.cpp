#include "logic_func.h"

//写回表擦除函数
//作用：在节点now从某一个存储位置中删除，更新写回表
//erase_type:0.reg 1.lut(empty) 2.sa 3.magic 4.lut-buffer 5.sa-buffer
void wb_erase(Node *now, int erase_type, int erase_id) {
    for (int i = 0; i < now->wb_pos[erase_type].size(); i++) {
        if (now->wb_pos[erase_type][i] == erase_id)
            now->wb_pos[erase_type].erase(now->wb_pos[erase_type].begin() + i);
    }
}

bool wb_empty(Node *now) {
//    cout<<now->node_id<<endl;
    if (now->wb_pos[0].empty() &&
        now->wb_pos[1].empty() &&
        now->wb_pos[2].empty() &&
        now->wb_pos[3].empty() &&
        now->wb_pos[4].empty() &&
        now->wb_pos[5].empty()
            )
        return true;
    else
        return false;
}
//判断节点的写回表中有当前阵列
bool is_in_wb(int array_type, int array_id, Node *node_now) {
    //array_type 1 2 3
    switch (array_type) {
        case 1: {
            for (int i : node_now->wb_pos[0]) {
                if (i == array_id)
                    return true;
            }
            return false;
        }
        case 2: {
            for (int i : node_now->wb_pos[1]) {
                if (i == array_id)
                    return true;
            }
            return false;
        }
        case 3: {
            for (int i : node_now->wb_pos[2]) {
                if (i == array_id)
                    return true;
            }
            return false;
        }
        default:
            break;

    }
    return false;
}

//
// Created by shenlibo on 23-4-30.
//
