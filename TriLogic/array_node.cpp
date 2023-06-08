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
    //当前的data_exchange<vector<vector>>只有{{}{}{}{}}，需要将to_type , to_id之前的填上0
    if (from_type == -1) //reg
    {
        switch (to_type) {
            case -1:
                break;
            case 1: {
                if (array_list1[to_id].data_exchange[0].size() == 0) array_list1[to_id].data_exchange[0].push_back(0);
                //lut - to_id - reg - [0]交换次数++
                array_list1[to_id].data_exchange[0][0]++;
            }
                break;
            case 2: {
                if (array_list2[to_id].data_exchange[0].size() == 0) array_list2[to_id].data_exchange[0].push_back(0);
                array_list2[to_id].data_exchange[0][0]++;
            }
                break;
            case 3: {
                if (array_list3[to_id].data_exchange[0].size() == 0) array_list3[to_id].data_exchange[0].push_back(0);
                array_list3[to_id].data_exchange[0][0]++;
            }
                break;

        }
    } else if (from_type == 1 || from_type == 4) //lut
    {
        switch (to_type) {
            case -1:
                break;
            case 1: {
                //从lut到lut
                while (array_list1[to_id].data_exchange[1].size() <= to_id) {
                    array_list1[to_id].data_exchange[1].push_back(0);
                }
                if (to_id != from_id)
                    array_list1[to_id].data_exchange[1][to_id]++;
            }
                break;
            case 2: {
                //从lut到sa
                while (array_list2[to_id].data_exchange[1].size() <= to_id) {
                    array_list2[to_id].data_exchange[1].push_back(0);
                }
                //直接添加
                array_list2[to_id].data_exchange[1][to_id]++;
            }
                break;
            case 3: {
                //从lut到magic
                while (array_list3[to_id].data_exchange[1].size() <= to_id) {
                    array_list3[to_id].data_exchange[1].push_back(0);
                }
                //直接添加
                array_list3[to_id].data_exchange[1][to_id]++;
            }
                break;
        }

    } else if (from_type == 2 || from_type == 5 || from_type == 6)//sa
    {
        switch (to_type) {
            case -1:
                break;
            case 1: {
                //从sa到lut
                while (array_list1[to_id].data_exchange[2].size() <= to_id) {
                    array_list1[to_id].data_exchange[2].push_back(0);
                }

                    array_list1[to_id].data_exchange[2][to_id]++;
            }
                break;
            case 2: {
                //从sa到sa
                while (array_list2[to_id].data_exchange[2].size() <= to_id) {
                    array_list2[to_id].data_exchange[2].push_back(0);
                }
                if (to_id != from_id)
                    array_list2[to_id].data_exchange[2][to_id]++;
            }
                break;
            case 3: {
                //从sa到magic
                while (array_list3[to_id].data_exchange[2].size() <= to_id) {
                    array_list3[to_id].data_exchange[2].push_back(0);
                }
                //直接添加
                array_list3[to_id].data_exchange[2][to_id]++;
            }
                break;
        }

    } else //magic
    {
        switch (to_type) {
            case -1:
                break;
            case 1: {
                //从magic到lut

                while (array_list1[to_id].data_exchange[3].size() <= to_id) {
                    array_list1[to_id].data_exchange[3].push_back(0);
                }

                array_list1[to_id].data_exchange[3][to_id]++;
            }
                break;
            case 2: {
                //从magic到sa

                while (array_list2[to_id].data_exchange[3].size() <= to_id) {
                    array_list2[to_id].data_exchange[3].push_back(0);
                }
                if (to_id != from_id)
                    array_list2[to_id].data_exchange[3][to_id]++;
            }
                break;
            case 3: {
                //从magic到magic
                while (array_list3[to_id].data_exchange[3].size() <= to_id) {
                    array_list3[to_id].data_exchange[3].push_back(0);
                }
                if (to_id != from_id)
                    array_list3[to_id].data_exchange[3][to_id]++;
            }
                break;
        }

    }

}
//
// Created by shenlibo on 23-4-30.
//
