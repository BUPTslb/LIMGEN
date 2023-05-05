#include "logic_func.h"

//写入逻辑
//这一部分和write_back是有很大重叠的,修正
//write_back中读取、写入都有
//目前得到：操作数个数、操作数来源的类型、操作数来源的阵列位置
void input_logic(int operand_num, int input1_type, int input1_id, int input2_type, int input2_id,
                 int decide_array_type, int decide_array_id, Node *now,
                 vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    cout << "input_logic中的decide_array_type= " << decide_array_type << endl;
    int op_type= op2int(now->operator_name);
    switch (decide_array_type) {
        case 1://LUT
        {
            if (array_list1.empty()) //以防万一，如果lut阵列是空的
                decide_array_id=build(1,op_type, array_list1,array_list2, array_list3);
            return;//LUT输入无需写入
        }

        case 2: //执行阵列的类型是SA
        {
            if (operand_num == 1) //假设只有一个操作数
            {
                //在阵列中：跳过
                //不在阵列中：直接输到sa_direct(依赖的节点id)
                //如果不在一个阵列中，更新
                if (input1_type != 6 || input1_id != decide_array_id) {
                    if (now->depend1 != nullptr) //不是立即数
                        array_list2[decide_array_id].sa_direct = now->depend1->node_id;
                    else
                        array_list2[decide_array_id].sa_direct = -1;
                }

            } else    //有两个操作数，一个输入到sa_direct,一个写入sa
            {
                //考虑所有情况
                // 1. 两个数都在阵列中 跳过
                if (input1_type == 6 && input1_id == decide_array_id &&
                    input2_type == 6 && input2_id == decide_array_id)
                    return;

                // 2. 1在 2不在   2输送到direct
                if (input1_type == 6 && input1_id == decide_array_id &&
                    (input2_type != 6 || input2_id != decide_array_id)) {
                    //对立即数进行判断
                    if (now->depend2 != nullptr) {
                        array_list2[decide_array_id].sa_direct = now->depend2->node_id;
                    } else {
                        array_list2[decide_array_id].sa_direct = -1;
                    }
                }

                // 3. 2在 1不在    1输送到direct
                if (input2_type == 6 && input2_id == decide_array_id &&
                    (input1_type != 6 || input1_id != decide_array_id)) {
                    //对立即数进行判断
                    if (now->depend1 != nullptr) {
                        array_list2[decide_array_id].sa_direct = now->depend1->node_id;
                    } else {
                        array_list2[decide_array_id].sa_direct = -1;
                    }
                }

                // 4. 1不在 2不在
                if ((input1_type != 6 || input1_id != decide_array_id) &&
                    (input2_type != 6 || input2_id != decide_array_id)) {
                    //随机选择一种
                    //4.1 1写入 2direct
                    //4.2 2写入 1direct
                    vector<int> idea_ready = {1, 2};
                    int idea_dse = rand() % 2;
                    array_list2[decide_array_id].write_number++;
                    //获取现在的时间
                    cout << "执行阵列的类型为sa,其decide_array_type = " << decide_array_type << endl;
                    double time_n = time_now(array_list1, array_list2, array_list3, now, decide_array_type,decide_array_id);
                    cout<<"input_logic中的time_now正常运行"<<endl;
                    //1.depend1写入,注意时间的节点
                    //将时间更新到depend1
                    if (idea_dse == 0) {
                        //更新阵列的写次数
                        array_list2[decide_array_id].write_number++;
                        //对立即数进行讨论
                        if (now->depend1!= nullptr) //depend1不是立即数,则是一个=节点, 写入sa阵列中
                        {
                            if (cap_array_lost(2, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                                write_cover(0, now->depend1, 2, decide_array_id,1, array_list1, array_list2, array_list3);
                            }
                            else
                            {
                                //更新阵列的存储节点表
                                array_add_node(2,decide_array_id,now->depend1, array_list1, array_list2,array_list3);
                                //更新节点的写回表
                                now->depend1->wb_pos[2].push_back(decide_array_id);
                                //更新节点的do_type sa存储 6
                                now->depend1->do_type = 6;
                                //更新节点的finish_id
                                now->depend1->finish_id = decide_array_id;
                                //更新1的节点的时间、阵列的时间
                                time_update(0, decide_array_type, decide_array_id, time_n, now->depend1, array_list1,
                                            array_list2, array_list3);
                            }
                        }
                        else //depend1是立即数,只需要更新写入阵列这段时间
                        {
                            //判断是否需要写覆盖,这里op_type只要不是0，就不会用到now,只进行写覆盖
                            if (cap_array_lost(2, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                                write_cover(100, now, 2, decide_array_id,1, array_list1, array_list2, array_list3);
                            }
                            array_list2[decide_array_id].start_time = time_n;
                            double time_up = sa_latency(0, array_list2[decide_array_id].sa_type);
                            array_list2[decide_array_id].over_time = time_n + time_up;
                            //set is_using
                            array_list2[decide_array_id].is_using = true;
                        }
                        //更新能量，对type-id阵列执行了写入（0）操作
                        energy_update(0, 6, decide_array_id, array_list1, array_list2, array_list3);
                        //2直接到direct
                        array_list2[decide_array_id].sa_direct =(now->depend2== nullptr)? -1: now->depend2->node_id;
                    }
                    else//2.depend2写入
                    {
                        //更新阵列的写次数
                        array_list2[decide_array_id].write_number++;
                        //对立即数进行讨论
                        if (now->depend2!= nullptr) //depend1不是立即数,则是一个=节点, 写入sa阵列中
                        {
                            if (cap_array_lost(2, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                                write_cover(0, now->depend2, 2, decide_array_id,1, array_list1, array_list2, array_list3);
                            }
                            else
                            {
                                //更新阵列的存储节点表
                                array_add_node(2,decide_array_id,now->depend2, array_list1, array_list2,array_list3);
                                //更新节点的写回表
                                now->depend2->wb_pos[2].push_back(decide_array_id);
                                //更新节点的do_type sa存储 6
                                now->depend2->do_type = 6;
                                //更新节点的finish_id
                                now->depend2->finish_id = decide_array_id;
                                //更新1的节点的时间、阵列的时间
                                time_update(0, decide_array_type, decide_array_id, time_n, now->depend2, array_list1,
                                            array_list2, array_list3);
                            }
                        }
                        else //depend2是立即数,只需要更新写入阵列这段时间
                        {

                            //判断是否需要写覆盖,这里op_type只要不是0，就不会用到now,只进行写覆盖
                            if (cap_array_lost(2, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                                write_cover(100, now, 2, decide_array_id,1, array_list1, array_list2, array_list3);
                            }
                            array_list2[decide_array_id].start_time = time_n;
                            double time_up = sa_latency(0, array_list2[decide_array_id].sa_type);
                            array_list2[decide_array_id].over_time = time_n + time_up;
                            //set is_using
                            array_list2[decide_array_id].is_using = true;

                        }
                        //更新能量，对type-id阵列执行了写入（0）操作
                        //写入sa阵列，= do_type类型为6
                        energy_update(0, 6, decide_array_id, array_list1, array_list2, array_list3);
                        //2直接到direct
                        array_list2[decide_array_id].sa_direct =(now->depend2== nullptr)? -1: now->depend2->node_id;
                    }
                }
            }
        }
            break;
        case 3: //MAGIC
        {
            //不管有几个操作数，都写入
            if (operand_num == 1) {
                //不在一个阵列中，写入
                if (input1_type != 3 || input1_id != decide_array_id) {
                    //获取现在的时间
                    double time_n = time_now(array_list1, array_list2, array_list3, now, decide_array_type, decide_array_id);
                    cout<<"input_logic中的time_now正常运行"<<endl;
                    //更新阵列的写次数
                    array_list3[decide_array_id].write_number++;
                    //判断立即数
                    if (now->depend1!= nullptr)
                    {
                        //判断是否需要写覆盖
                        if (cap_array_lost(3, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, 3,decide_array_id, 1,array_list1, array_list2, array_list3);
                        }
                        else
                        {
                            //更新阵列的存储节点表
                            array_list3[decide_array_id].store_node.push_back(now->depend1->node_id);
                            //更新节点的写回表
                            now->depend1->wb_pos[3].push_back(decide_array_id);
                            //更新节点的do_type magic 3
                            now->depend1->do_type = 3;
                            //更新节点的finish_id
                            now->depend1->finish_id = decide_array_id;
                            //更新节点的时间、能量
                            time_update(0, decide_array_type, decide_array_id, time_n, now->depend1, array_list1, array_list2,
                                        array_list3);
                            energy_update(0, decide_array_type, decide_array_id, array_list1, array_list2, array_list3);
                        }

                    }
                    else //是立即数
                    {
                        if (cap_array_lost(3, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(100, now, 3,decide_array_id, 1,array_list1, array_list2, array_list3);
                        }
                        else
                        {
                            array_list3[decide_array_id].start_time = time_n;
                            double time_up = ma_latency(0);
                            array_list3[decide_array_id].over_time = time_n + time_up;
                            //set is_using
                            array_list3[decide_array_id].is_using = true;
                            energy_update(0, decide_array_type, decide_array_id, array_list1, array_list2, array_list3);
                        }

                    }


                }

            } else {
                //1不在一个阵列中，写入
                if (input1_type != 3 || input1_id != decide_array_id) {
                    //获取现在的时间
                    double time_n = time_now(array_list1, array_list2, array_list3, now, decide_array_type, decide_array_id);
                    cout<<"input_logic中的time_now正常运行"<<endl;
                    //更新阵列的写次数
                    array_list3[decide_array_id].write_number++;
                    //判断立即数
                    if (now->depend1!= nullptr)
                    {
                        //判断是否需要写覆盖
                        if (cap_array_lost(3, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, 3,decide_array_id, 1,array_list1, array_list2, array_list3);
                        }
                        else
                        {
                            //更新阵列的存储节点表
                            array_list3[decide_array_id].store_node.push_back(now->depend1->node_id);
                            //更新节点的写回表
                            now->depend1->wb_pos[3].push_back(decide_array_id);
                            //更新节点的do_type magic 3
                            now->depend1->do_type = 3;
                            //更新节点的finish_id
                            now->depend1->finish_id = decide_array_id;
                            //更新节点的时间、能量
                            time_update(0, decide_array_type, decide_array_id, time_n, now->depend1, array_list1, array_list2,
                                        array_list3);
                            energy_update(0, decide_array_type, decide_array_id, array_list1, array_list2, array_list3);
                        }

                    }
                    else //是立即数
                    {
                        if (cap_array_lost(3, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(100, now, 3,decide_array_id, 1,array_list1, array_list2, array_list3);
                        }
                        else
                        {
                            array_list3[decide_array_id].start_time = time_n;
                            double time_up = ma_latency(0);
                            array_list3[decide_array_id].over_time = time_n + time_up;
                            //set is_using
                            array_list3[decide_array_id].is_using = true;
                            energy_update(0, decide_array_type, decide_array_id, array_list1, array_list2, array_list3);
                        }

                    }


                }
                //2不在一个阵列中，写入
                if (input2_type != 3 || input2_id != decide_array_id) {
                    //获取现在的时间
                    double time_n = time_now(array_list1, array_list2, array_list3, now, decide_array_type, decide_array_id);
                    cout<<"input_logic中的time_now正常运行"<<endl;
                    //更新阵列的写次数
                    array_list3[decide_array_id].write_number++;
                    //判断立即数
                    if (now->depend2!= nullptr)
                    {
                        //判断是否需要写覆盖
                        if (cap_array_lost(3, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, 3,decide_array_id, 1,array_list1, array_list2, array_list3);
                        }
                        else
                        {
                            //更新阵列的存储节点表
                            array_list3[decide_array_id].store_node.push_back(now->depend2->node_id);
                            //更新节点的写回表
                            now->depend2->wb_pos[3].push_back(decide_array_id);
                            //更新节点的do_type magic 3
                            now->depend2->do_type = 3;
                            //更新节点的finish_id
                            now->depend2->finish_id = decide_array_id;
                            //更新节点的时间、能量
                            time_update(0, decide_array_type, decide_array_id, time_n, now->depend2, array_list1, array_list2,
                                        array_list3);
                            energy_update(0, decide_array_type, decide_array_id, array_list1, array_list2, array_list3);
                        }

                    }
                    else //是立即数
                    {
                        if (cap_array_lost(3, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(100, now, 3,decide_array_id, 1,array_list1, array_list2, array_list3);
                        }
                        else
                        {
                            array_list3[decide_array_id].start_time = time_n;
                            double time_up = ma_latency(0);
                            array_list3[decide_array_id].over_time = time_n + time_up;
                            //set is_using
                            array_list3[decide_array_id].is_using = true;
                            energy_update(0, decide_array_type, decide_array_id, array_list1, array_list2, array_list3);
                        }

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
//数据都已经准备完毕
void output_logic(int decide_array_type, int decide_array_id, int op_type, Node *now, \
                vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    //执行期间对阵列的影响
    //对节点：需要调用模块时，更新finish_id
    //时间参数的影响
    //1.是否需要调用模块 LUT SA MA 都有设计好的模块供调用
    //获取时间
    double time_n = time_now(array_list1, array_list2, array_list3, now, decide_array_type, decide_array_id);
    cout<<"output_logic中的time_now正常运行"<<endl;
    switch (decide_array_type) {
        case 1: //LUT
        {
            cout<<"outlogic使用的执行阵列类型为 lut,现在lut阵列的个数为 "<<array_list1.size()<<endl;
            op_lut(op_type, decide_array_id, now, time_n, array_list1, array_list2, array_list3);
            //需要一个函数判断是否需要调用模块
            //根据op_type，进行每一个阵列下的操作：lut_op\sa_op\ma_op
            return;
        }

        case 2: {
            op_sa(op_type, decide_array_id, now, time_n, array_list1, array_list2, array_list3);
            return;
        }

        case 3: {
            op_magic(op_type, decide_array_id, now, time_n, array_list1, array_list2, array_list3);
            return;
        }

        default:
            return;
    }

}

//lut执行逻辑
void op_lut(int op_type, int decide_array_id, Node *now, double time_now, vector<lut_arr> &array_list1,
            vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    //重新确认节点的执行类型
    now->do_type = 1;
    //节点的结束位置
    now->finish_id = decide_array_id;
    // 判断当前的lut-out看是否需要写回
    if (array_list1[decide_array_id].lut_out > 0)  //节点id>=1
    {
        int lut_out_now = array_list1[decide_array_id].lut_out;
        //当前lut-out的节点的操作类型
        int on_op = op2int(find_node_by_number(lut_out_now)->operator_name);
        if (on_op > 0) //不是等号，直接更新lut-out
            array_list1[decide_array_id].lut_out = now->node_id;
        else //否则判断一下其写回表和出度
        {
            if (find_node_by_number(lut_out_now)->out_degree > 0 &&
                wb_empty(find_node_by_number(lut_out_now))) {
                //TODO:设置优先级，buffer只能写回本阵列
                write_back(1, find_node_by_number(lut_out_now)->finish_id, find_node_by_number(lut_out_now),
                           array_list1, array_list2, array_list3);
            }
        }

    }
    //将当前节点设置为lut-out
    array_list1[decide_array_id].lut_out=now->node_id;
    // 更新时间
    time_update(op_type, 1, decide_array_id, time_now, now, array_list1, array_list2, array_list3);
//更新能量
    energy_update(op_type, 1, decide_array_id, array_list1, array_list2, array_list3);

}

//sa执行逻辑
void op_sa(int op_type, int decide_array_id, Node *now, double time_now, vector<lut_arr> &array_list1,
           vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    now->do_type = 2;
    now->finish_id = decide_array_id;//模块调用怎么写？

    if (array_list2[decide_array_id].sa_out > 0)  //节点id>=1
    {
        int sa_out_now = array_list2[decide_array_id].sa_out;
        //当前lut-out的节点的操作类型
        int on_op = op2int(find_node_by_number(sa_out_now)->operator_name);
        if (on_op > 0) //不是等号，直接更新lut-out
            array_list2[decide_array_id].sa_out = now->node_id;
        else //否则判断一下其写回表和出度
        {
            if (find_node_by_number(sa_out_now)->out_degree > 0 &&
                wb_empty(find_node_by_number(sa_out_now))) {
                //TODO:设置优先级，buffer只能写回本阵列
                write_back(2, find_node_by_number(sa_out_now)->finish_id,find_node_by_number(sa_out_now),
                           array_list1, array_list2, array_list3);
            }
        }

    }
    //处理本节点
    array_list2[decide_array_id].sa_out = now->node_id;
    //更新时间
    time_update(op_type, 2, decide_array_id, time_now, now, array_list1, array_list2, array_list3);
    //更新能量
    if (op_type==0)
        energy_update(op_type, 6, decide_array_id, array_list1, array_list2, array_list3);
    else
        energy_update(op_type, 2, decide_array_id, array_list1, array_list2, array_list3);
}

//magic执行逻辑
//涉及到中间值的时候，需要进行写入
void op_magic(int op_type, int decide_array_id, Node *now, double time_now, vector<lut_arr> &array_list1,
              vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    now->do_type = 3;
    now->finish_id = decide_array_id;//模块调用怎么写？对操作进行拆解
    int row_need=op_row_need(op_type, 3);
    //判断中间值和结果是否需要写覆盖
    if (cap_array_lost(3, decide_array_id, array_list1, array_list2, array_list3) == 0) {
        write_cover(100, now, 3, decide_array_id,row_need, array_list1, array_list2, array_list3);
    }
    //更新时间
    time_update(op_type, 3, decide_array_id, time_now, now, array_list1, array_list2, array_list3);
    //更新能量
    energy_update(op_type, 3, decide_array_id, array_list1, array_list2, array_list3);
}
//
// Created by shenlibo on 23-4-19.
//
