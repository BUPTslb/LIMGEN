#include "logic_func.h"

void array_add_node(int array_type, int array_id, Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                    vector<magic_arr> &array_list3) {
    if (is_in_wb(array_type, array_id, now)) return;
    else {
        if (array_type == 2) {
            array_list2[array_id].store_node.push_back(now->node_id);
        }
        if (array_type == 3) {
            array_list3[array_id].store_node.push_back(now->node_id);
        }
    }

}

//将阵列存储表中的节点删除
void array_erase_node(int erase_node, int array_type, int array_id,
                      vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    switch (array_type) {
        case 2: //sa
        {
            for (int i = 0; i < array_list2[array_id].store_node.size(); i++) {
                if (array_list2[array_id].store_node[i] == erase_node)
                    array_list2[array_id].store_node.erase(array_list2[array_id].store_node.begin() + i);
            }

        }
            break;
        case 3: {
            for (int i = 0; i < array_list3[array_id].store_node.size(); i++) {
                if (array_list3[array_id].store_node[i] == erase_node)
                    array_list3[array_id].store_node.erase(array_list3[array_id].store_node.begin() + i);
            }
        }
            break;
        default:
            break;
    }
}

//to_id从from_id取数据
void array_data_transfer(int from_type, int from_id, int to_type, int to_id,
                         vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    int size1 = array_list1.size();
    int size2 = array_list2.size();
    int size3 = array_list3.size();
    if (from_type == -1) //reg
    {
        switch (to_type) {
            case -1:

                break;
            case 1:

                break;
            case 2:

                break;

            case 3:

                break;

        }
    } else if (from_type == 1 || from_type == 4) //lut
    {
        switch (to_type) {
            case -1:
                break;
            case 1:
                if (from_id == to_id) break;
                else if (to_id < array_list1[to_id].data_exchange[1].size()) {
                    array_list1[to_id].data_exchange[1][to_id]++;
                } else {
                    int now_size = array_list1[to_id].data_exchange[1].size();


                }
                break;
            case 2:

                break;

            case 3:

                break;

        }

    } else if (from_type == 2 || from_type == 5 || from_type == 6)//sa
    {

    } else //magic
    {

    }

}
//
// Created by shenlibo on 23-4-30.
//
