#include "logic_func.h"
void array_add_node(vector<Node> &nodes2,int array_type, int array_id, Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                    vector<magic_arr> &array_list3) {
    if (is_in_wb(array_type,array_id, now)) return;
    else{
        if (array_type == 2) {
            array_list2[array_id].store_node.push_back(now->node_id);
        }
        if (array_type == 3) {
            array_list3[array_id].store_node.push_back(now->node_id);
        }
    }

}

//将阵列存储表中的节点删除
void array_erase_node(vector<Node> &nodes2,int erase_node,int array_type,int array_id,
                      vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,vector<magic_arr> &array_list3)
{
    switch (array_type) {
        case 2: //sa
        {
            for (int i=0;i<array_list2[array_id].store_node.size();i++) {
                if (array_list2[array_id].store_node[i]==erase_node)
                    array_list2[array_id].store_node.erase(array_list2[array_id].store_node.begin()+i);
            }

        }
            break;
        case 3:
        {
            for (int i=0;i<array_list3[array_id].store_node.size();i++) {
                if (array_list3[array_id].store_node[i]==erase_node)
                    array_list3[array_id].store_node.erase(array_list3[array_id].store_node.begin()+i);
            }
        }
            break;
        default:
            break;
    }
}


//
// Created by shenlibo on 23-4-30.
//
