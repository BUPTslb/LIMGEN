#include "logic_func.h"
//计算当前还剩下多少能用,返回其数字：计算立即数
//decide_array_type的类型：-1 1 2 3 4 5 6
int cap_array_lost(int decide_array_type, int decide_array_id, vector<lut_arr> &array_list1, \
                            vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    int cap_array = 0;
    switch (decide_array_type) {
        case 1://LUT, 计算还能够放下几个运算
            break;
        case 2://SA
        {

            //需要一个函数，计算当前存储节点占用的空间
            //检验越界
//            cout<<"选中sa阵列的行数："<<array_list2[decide_array_id].row_num<<endl;
            cap_array = array_list2[decide_array_id].row_num
                        - capped_now(decide_array_type, decide_array_id, array_list1,array_list2, array_list3);
//            cout<<"capped_now函数运行正常"<<endl;
            cap_array = max(0, cap_array);
        }
            break;
        case 3://MAGIC
        {
            //节点存入magic有两种情况:1.要更新写回= 2.操作中有立即数，类型为op
            //magic中，立即数也会被写入，因此要全部统计
//            cout<<"选中magic阵列的行数："<<array_list3[decide_array_id].row_num<<endl;
            cap_array = array_list3[decide_array_id].row_num - capped_now(decide_array_type, decide_array_id, array_list1,array_list2, array_list3);
//            cout<<"capped_now函数运行正常"<<endl;
            cap_array = max(0, cap_array);
        }
            break;
        default:
            break;
    }
//    cout<<"cap_array_lost函数正常运行"<<endl;
    return cap_array;

}

//只计算=节点
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
            //对于立即数 和中间结果，可以认为其不存在，只计算=节点
            for (int i : array_list2[decide_array_id].store_node) {
                if (find_node_by_number(i))  //可以找到这个节点
                {
                    if (find_node_by_number( i)->out_degree == 0) //节点出度为0，不再被需要
                    {
                        cap_array++;
                        //等被覆盖的时候再擦除，无意义的擦除浪费资源
                    }
                    else  //出度不为零
                    {
                        //但是在多个地方存储的都有
                        num_node_position(find_node_by_number( i))>1;
                        cap_array++;
                    }
                }


            }
        }
            break;
        case 3://MAGIC
        {
            //只有等号会被存入
            for (int i : array_list3[decide_array_id].store_node) {
                if (find_node_by_number(i)) //能找到该节点
                {
                    if (find_node_by_number( i)->out_degree ==0) //节点出度为0，不再被需要
                    {
                        cap_array++;
                    }
                    else //出度>0
                    {
                        if (num_node_position(find_node_by_number(i))>1) //在多个地方存储
                        {
                            cap_array++;
                        }

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
//不管是否出度为0，只要存在阵列中，没有被消除，就计算面积占用，只计算=
int capped_now(int decide_array_type, int decide_array_id, vector<lut_arr> &array_list1,
               vector<sa_arr> &array_list2, vector<magic_arr> &array_list3)
{
    int capped_now=0;
    switch (decide_array_type) {
        case 2://sa-out
            //问题，store_node中的数字出现了乱码
            for (int i:array_list2[decide_array_id].store_node) {
                if (find_node_by_number(i)) //能找到该节点
                    capped_now++;
            }
            break;
        case 5://sa-buffer
            for (int i:array_list2[decide_array_id].store_node) {
                if (find_node_by_number(i)) //能找到该节点
                    capped_now++;
            }
            break;
        case 6://sa-store
            for (int i:array_list2[decide_array_id].store_node) {
                if (find_node_by_number(i)) //能找到该节点
                    capped_now++;
            }
            break;
        case 3:
            for (int i:array_list3[decide_array_id].store_node) {
                if (find_node_by_number(i)) //能找到该节点
                    capped_now++;
            }
            break;

        default:
            break;

    }
    return capped_now;
}

// Created by shenlibo on 23-4-19.

