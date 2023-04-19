#include "logic_func.h"

//写入逻辑
void
input_logic(int input1_type, int input1_id, int input2_type, int input2_id, int decide_array_type, int decide_array_id, \
        Node *now, int *Register, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
            vector<magic_arr> &array_list3) {
    switch (decide_array_type) {
        case 1://LUT
            return;//LUT输入无需写入

        case 2: //SA
        {
            //先讨论操作数1,如果在本SA阵列中（不是单纯的立即数、节点的写回表中有此阵列）
            if (input1_type == 2 && input1_id == decide_array_id &&
                is_in_wb(decide_array_type, decide_array_id, now->depend1)) {
                if (input2_type == 0 || (input2_type == 2 && input2_id == decide_array_id &&
                                         is_in_wb(decide_array_type, decide_array_id, now->depend2)))//输入2不存在或在阵列中
                {
                    return;
                }
                //DSE位置,下方2选1
                //2存在且2不在本阵列中，输入direct
                array_list2[decide_array_id].sa_direct = input2_id;//输入到激活行
                //或者写入
                array_list2[decide_array_id].write_number++;
                if (now->depend2)//不是立即数
                {
                    array_list2[decide_array_id].store_node.push_back(now->depend2->node_id);//阵列的存储节点
                    //更新wb_pos
                    now->depend2->wb_pos[1].push_back(decide_array_id);
                } else //是立即数,只更新store_node,不更新写回表
                {
                    array_list2[decide_array_id].store_node.push_back(now->node_id);
                }
            } else //操作数1不在阵列中
            {
                //先讨论操作数2在不在阵列中
                if (input2_type == 0 || (input2_type == 2 && input2_id == decide_array_id &&
                                         is_in_wb(decide_array_type, decide_array_id, now->depend2))) {
                    //DSE位置,下方2选1
                    //1输入direct
                    array_list2[decide_array_id].sa_direct = input1_id;//输入到激活行
                    //或者写入
                    array_list2[decide_array_id].write_number++;
                    if (now->depend1)//不是立即数
                    {
                        array_list2[decide_array_id].store_node.push_back(now->depend1->node_id);//阵列的存储节点
                        //更新wb_pos
                        now->depend1->wb_pos[1].push_back(decide_array_id);
                    } else //是立即数,只更新store_node,不更新写回表
                    {
                        array_list2[decide_array_id].store_node.push_back(now->node_id);
                    }
                } else //1,2都不在本阵列中
                {
                    //下方存在DSE，1D2W 2D1W 1W2W三种
                    //在写回时候，如果有一个是立即数，则将立即数直连，将变量写回
                    if (now->depend1 && now->depend2) //都不是立即数，DSE: 1D2W 2D1W 1W2W三种
                    {
                        //1D2W
                        array_list2[decide_array_id].sa_direct = input1_id;//1输入到激活行

                        array_list2[decide_array_id].write_number++;
                        array_list2[decide_array_id].store_node.push_back(now->depend2->node_id);//阵列的存储节点
                        //更新wb_pos
                        now->depend2->wb_pos[1].push_back(decide_array_id);

                    } else if (now->depend1) //2是立即数，1不是,将1写入
                    {
                        array_list2[decide_array_id].sa_direct = input2_id;

                        array_list2[decide_array_id].write_number++;
                        array_list2[decide_array_id].store_node.push_back(now->depend1->node_id);//阵列的存储节点
                        //更新wb_pos
                        now->depend1->wb_pos[1].push_back(decide_array_id);

                    } else if (now->depend2) //1是立即数，2不是
                    {
                        array_list2[decide_array_id].sa_direct = input1_id;

                        array_list2[decide_array_id].write_number++;
                        array_list2[decide_array_id].store_node.push_back(now->depend2->node_id);//阵列的存储节点
                        //更新wb_pos
                        now->depend1->wb_pos[1].push_back(decide_array_id);

                    } else //1 2 都是立即数
                    {
                        //随意将一个写入即可
                        array_list2[decide_array_id].sa_direct = input1_id;

                        //是立即数,只更新store_node,不更新写回表
                        //store_node存放当前“op”类型的节点id,表示存入了一个立即数
                        array_list2[decide_array_id].store_node.push_back(now->node_id);

                    }
                }
            }
        }
            break;
        case 3: //MAGIC
        {
            if (input1_type == 3 && input1_id == decide_array_id) //1在阵列中
            {
                if (input2_type == 0 || (input2_type == 3 && input2_id == decide_array_id)) //2也在阵列中 或者 2不存在
                {
                    return;
                } else //2不在阵列中
                {
                    array_list3[decide_array_id].write_number++;
                    //如果不是立即数，将2的节点存到store_node中，更新wb_pos
                    if (now->depend2) {
                        array_list3[decide_array_id].store_node.push_back(now->depend2->node_id);
                        now->depend2->wb_pos[2].push_back(decide_array_id);
                    } else //是立即数,只更新store_node,不更新写回表
                    {
                        array_list3[decide_array_id].store_node.push_back(now->node_id);
                    }
                }
            } else //1不在阵列中
            {
                //2不存在 || 2 在阵列中
                if (input2_type == 0 || input2_type == 3 && input2_id == decide_array_id) {
                    //将1写入阵列
                    array_list3[decide_array_id].write_number++;
                    //如果不是立即数，将1的节点存到store_node中，更新wb_pos
                    if (now->depend1) {
                        array_list3[decide_array_id].store_node.push_back(now->depend1->node_id);
                        now->depend1->wb_pos[2].push_back(decide_array_id);
                    } else //是立即数,只更新store_node,不更新写回表
                    {
                        array_list2[decide_array_id].store_node.push_back(now->node_id);
                    }
                } else  //1 2 都存在 且 都不在阵列中
                {
                    array_list3[decide_array_id].write_number += 2;
                    //不是立即数，找到其所在节点，更新store_node,更新wb_pos
                    if (now->depend1) {
                        array_list3[decide_array_id].store_node.push_back(now->depend1->node_id);
                        now->depend1->wb_pos[2].push_back(decide_array_id);
                    }
                    if (now->depend2) {
                        array_list3[decide_array_id].store_node.push_back(now->depend2->node_id);
                        now->depend2->wb_pos[2].push_back(decide_array_id);
                    }
                    if (!now->depend1 || !now->depend2) //存在立即数
                    {
                        array_list3[decide_array_id].store_node.push_back(now->node_id);
                    }
                }
            }
        }
            break;
        default:
            break;

    }

}

//此时已经确定的条件：阵列可用、阵列类型、阵列id
void output_logic(int decide_array_type, int decide_array_id, int op_type, Node *now, \
                vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    //执行期间对阵列的影响
    //对节点：需要调用模块时，更新finish_id
    //时间参数的影响
    //1.是否需要调用模块 LUT SA MA 都有设计好的模块供调用
    switch (decide_array_type) {
        case 1: //LUT
        {
            op_lut(op_type, decide_array_id, now, array_list1);
            //需要一个函数判断是否需要调用模块
            //根据op_type，进行每一个阵列下的操作：lut_op\sa_op\ma_op

            return;
        }

        case 2: {
            op_sa(op_type, decide_array_id, now, array_list2);
            return;
        }

        case 3: {
            op_magic(op_type, decide_array_id, now, array_list3);
            return;
        }

        default:
            return;
    }

}

//lut执行逻辑
void op_lut(int op_type, int decide_array_id, Node *now, vector<lut_arr> &array_list1) {
    now->do_type = 1;
    now->finish_id = decide_array_id;

}

//sa执行逻辑
void op_sa(int op_type, int decide_array_id, Node *now, vector<sa_arr> &array_list2) {
    now->do_type = 2;
    now->finish_id = decide_array_id;//模块调用怎么写？
}

//magic执行逻辑
void op_magic(int op_type, int decide_array_id, Node *now, vector<magic_arr> &array_list3) {
    now->do_type = 3;
    now->finish_id = decide_array_id;//模块调用怎么写？对操作进行拆解
}
//
// Created by shenlibo on 23-4-19.
//
