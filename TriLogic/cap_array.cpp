#include "logic_func.h"
//计算当前还剩下多少能用,返回其数字：计算立即数
int cap_array_lost(int decide_array_type, int decide_array_id, vector<lut_arr> &array_list1, \
                            vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    int cap_array = 0;
    switch (decide_array_type) {
        case 1://LUT, 计算还能够放下几个运算
        {
            auto it = array_list1[decide_array_id].op_type.begin();
            //lut : add mul div l_eq l_bigger l_less
            if (*it == 11 || *it == 12 || *it == 13 || *it == 1 || *it == 2 || *it == 3)
                return 0;
                //TODO: can array co-using lut-mux?
            else
                cap_array = 3 - array_list1[decide_array_id].op_type.size();
            cout << "lut当前的容量为" << cap_array << endl;
        }
            break;
        case 2://SA
        {
            //TODO:根据store_node,计算实际容量,有修改空间
            //需要一个函数，计算当前存储节点占用的空间
            cap_array = array_list2[decide_array_id].row_num - capped_now(decide_array_type, decide_array_id, array_list1,array_list2, array_list3);
            cap_array = max(0, cap_array);
        }
            break;
        case 3://MAGIC
        {
            //节点存入magic有两种情况:1.要更新写回= 2.操作中有立即数，类型为op
            //magic中，立即数也会被写入，因此要全部统计
            cap_array = array_list3[decide_array_id].row_num - capped_now(decide_array_type, decide_array_id, array_list1,array_list2, array_list3);
            cap_array = max(0, cap_array);
        }
            break;
        default:
            break;
    }
    return cap_array;

}

//可覆盖容量,立即数也被算在内，因为OP类型只有立即数会被存入，ASS可以直接=立即数
//对于多步骤的运算，其容量不能单纯的由操作数的个数决定：如add需要使用5行
int cap_array_cover(int decide_array_type, int decide_array_id, vector<lut_arr> &array_list1, \
                            vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    int cap_array = 0;
    switch (decide_array_type) {
        case 1://LUT, 直接返回0
        {
            return 0;
        }
        case 2://SA，只计算出度为0的
        {
            //op类型被存入只有一种可能：存入的是立即数，SA只会存入一个立即数
//根据store_node,计算实际容量,有修改空间
            for (int i : array_list2[decide_array_id].store_node) {
                int op = op2int(find_node_by_number( i)->operator_name);//节点的操作类型
                int operand_num=(find_node_by_number( i)->depend1?1:0)+(find_node_by_number( i)->depend2?1:0);//操作数的个数
                if (find_node_by_number( i)->out_degree == 0) //节点出度为0，不再被需要
                {
                    //节点的写回表中有当前阵列
                    if (is_in_wb(decide_array_type,decide_array_id, find_node_by_number(i)))
                    {
                        if (op==0||op==8) cap_array++;//one stored
                        else if (op==11) {
                            cap_array+=(5-operand_num);//ab a+b a^b
                        }
                        else cap_array+=1;
                    }

//等被覆盖的时候再擦除，无意义的擦除浪费资源
                }
                else //out_degree>0, stored_array num>1,at least 1 array stored
                {
                    //TODO:DSE,擦除还是不擦除，都可能影响整体的性能,至少要剩下一个
                    int stored_array_num= num_node_position(find_node_by_number( i));
                    if (stored_array_num>1) //多个阵列中都存储的有
                    {
                        //容量代表可以覆盖，但还没有执行操作
                        //节点的写回表中有当前阵列
                        if (is_in_wb(decide_array_type,decide_array_id, find_node_by_number(i)))
                        {
                            if (op==0||op==8) cap_array++;//one stored
                            else if (op==11) {
                                cap_array+=(5-operand_num);//ab a+b a^b
                            }
                            else cap_array+=1;
                        }
                    }
                }
            }
        }
            break;
        case 3://MAGIC
        {
//节点存入magic有两种情况:1.要更新写回= 2.操作中有立即数，类型为op 3.op控制节点的结果也要存入
            for (int i : array_list3[decide_array_id].store_node) {
                int op = op2int(find_node_by_number( i)->operator_name);
                if (find_node_by_number( i)->out_degree ==
                    0) //节点出度为0，不再被需要
                {
                    if (op == 0 || op == 8) { cap_array++; } //assign not
                    else if (op == 11) {
                        cap_array += 2;//一定有的：L和C
                        if (find_node_by_number( i)->depend1 ==
                            nullptr)
                            cap_array++;//一个立即数
                        if (find_node_by_number( i)->depend2 ==
                            nullptr)
                            cap_array++;//2个立即数

                    }//加法占用了5row,ABCLS
                    else {
                        if (find_node_by_number( i)->depend1 ==
                            nullptr)
                            cap_array++;//一个立即数
                        if (find_node_by_number( i)->depend2 ==
                            nullptr)
                            cap_array++;//2个立即数
                    }
                }
                if (find_node_by_number( i)->out_degree > 1) //节点出度大于等于2
                {
                    //TODO:DSE,擦除还是不擦除，都可能影响整体的性能,至少要剩下一个
                    if (op == 0 || op == 8) { cap_array++; }
                    else if (op == 11) {
                        cap_array += 2;//一定有的：L和C
                        if (find_node_by_number( i)->depend1 ==
                            nullptr)
                            cap_array++;//一个立即数
                        if (find_node_by_number( i)->depend2 ==
                            nullptr)
                            cap_array++;//2个立即数

                    }//加法占用了5row,ABCLS
                    else {
                        if (find_node_by_number( i)->depend1 ==
                            nullptr)
                            cap_array++;//一个立即数
                        if (find_node_by_number( i)->depend2 ==
                            nullptr)
                            cap_array++;//2个立即数
                    }
                }
            }
        }
            break;
        default:
            break;
    }
    return cap_array;
}
//不管是否出度为0，只要存在阵列中，没有被消除，就计算面积占用
int capped_now(int decide_array_type, int decide_array_id, vector<lut_arr> &array_list1,
               vector<sa_arr> &array_list2, vector<magic_arr> &array_list3)
{
    int capped_now=0;
    switch (decide_array_type) {
        case 1://lut
            return 0;
        case 2://sa
            for (int i:array_list2[decide_array_id].store_node) {
                int op = op2int(find_node_by_number( i)->operator_name);
                int operand_num=(find_node_by_number( i)->depend1?1:0)+(find_node_by_number( i)->depend2?1:0);//操作数的个数
                if (is_in_wb(decide_array_type,decide_array_id, find_node_by_number(i)))
                {
                    if (op==0||op==8) capped_now++;//one stored
                    else if (op==11) {
                        capped_now+=(5-operand_num);//ab a+b a^b
                    }
                    else capped_now+=1;
                }
            }
        case 3: {
            for (int i: array_list3[decide_array_id].store_node) {
                int op = op2int(find_node_by_number( i)->operator_name);
                if (op == 0 || op == 8) { capped_now++; } //assign not
                else if (op == 11) {
                    capped_now += 2;//一定有的：L和C
                    if (find_node_by_number( i)->depend1 ==
                        nullptr)
                        capped_now++;//一个立即数
                    if (find_node_by_number( i)->depend2 ==
                        nullptr)
                        capped_now++;//2个立即数

                }//加法占用了5row,ABCLS
                else {
                    if (find_node_by_number( i)->depend1 ==
                        nullptr)
                        capped_now++;//一个立即数
                    if (find_node_by_number( i)->depend2 ==
                        nullptr)
                        capped_now++;//2个立即数
                }
            }
        }

        default:
            break;


    }


}
//
// Created by shenlibo on 23-4-19.
//
