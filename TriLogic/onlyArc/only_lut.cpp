#include "../mainfunc.h"
#include "../logic_func.h"

//对照实验，只选用lut来实现功能
//需要添加功能：lut阵列做存储，这时就类似ME了
//有 三个地方可以存储数据 -1.reg(0) 3.magic(lut存储)(3-1) 4.lut-buffer(4)
//每一次调用和写回相关的都注意，只能写回这三个地方
std::vector<double>  only_lut(vector<vector<Nodes *>> controlstep2, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                              vector<magic_arr> &array_list3) {
    for (int i = 0; i < controlstep2.size(); i++) {
//        //cout << "step：" << i << endl;
        for (int j = 0; j < controlstep2[i].size(); ++j) {
//            //cout << "node_id：" << controlstep2[i][j]->node_id << endl
//                 << "operator_name：" << controlstep2[i][j]->operator_name
//                 << "depend1：" << (controlstep2[i][j]->depend1 ? controlstep2[i][j]->depend1->node_id : 0) << "  "
//                 << "depend2：" << (controlstep2[i][j]->depend2 ? controlstep2[i][j]->depend2->node_id : 0) << endl;
            //一层层遍历控制步，获取操作数，获取算子，分配阵列，计算速度、功耗、面积
            int type_operation = op2int(controlstep2[i][j]->operator_name);

            //先讨论写回的情况:
            //假设初始输入和立即数被放在外部的寄存器中，因为我们无法索引他（in1 in2 没有id）
            if (type_operation == 0)//等号，其操作数为op或者立即数
                //不一定有依赖，如果常用的立即数，会将其交给寄存器
            {
                //如果没有依赖，交给寄存器
                if (controlstep2[i][j]->depend1 == nullptr) {
                    Reg_sum.write_num_sum++;//寄存器写
                    controlstep2[i][j]->do_type = -1;
                    controlstep2[i][j]->finish_id = -1;//证明写到了寄存器中

                    //更新出度
                    out_degree(controlstep2[i][j]);
                    double time_n = time_only_lut(array_list1, array_list2, array_list3, controlstep2[i][j]);
                    //update the time of do_array
                    time_update(0, -1, -1, time_n, controlstep2[i][j], array_list1, array_list2, array_list3);
                    //update the energy of reg
                    energy_update(0, -1, -1, array_list1, array_list2, array_list3);
                    //更新wb_pos,表示写到了寄存器中
                    controlstep2[i][j]->wb_pos[0].push_back(-1);
                    //输出：
//                    //cout << "do_type: " << controlstep2[i][j]->do_type << "  finish_id: "
//                         << controlstep2[i][j]->finish_id << endl;
                    continue;//进行下一个循环
                }

                //有依赖,则依赖一定来自OP, from array
                //统一，更改节点的执行结束id和do_type
                controlstep2[i][j]->finish_id = controlstep2[i][j]->depend1->finish_id; //op的结束阵列id
                double time_n = time_only_lut(array_list1, array_list2, array_list3, controlstep2[i][j]);//开始执行当前节点的时间
                //如果默认其为直接输出的out,则不会对其他产生任何影响，不需要进行time_update
                //c=a op b || c=a op -1 || c = a
                switch (controlstep2[i][j]->depend1->do_type) {
                    case -1:  //来自寄存器，不知到写回到哪里,直接赋值应该不用写的
                    {
                        int back_type_ready[2] = {0, 3};
                        int back_type = back_type_ready[rand() % 2];
                        int back_id = 0;
                        if (back_type == 0) back_id = -1;
                        if (back_type == 3) {
                            if (array_list3.empty())
                                back_id = build(3, 0, array_list1, array_list2, array_list3);
                            else
                                back_id = array_list3[rand() % array_list3.size()].array_id;
                        }
                        write_back_lut(-1, -1, controlstep2[i][j], array_list1, array_list2, array_list3, back_type,
                                   back_id);
                    }
                        break;
                    case 1: //来自lut的执行结果
                    {
                        controlstep2[i][j]->end_time = time_n;//暂时设置时间
                        //node
                        controlstep2[i][j]->do_type = 1;//LUT-OUT
                        //对现在的lut-out进行处理,其有一个初始值，不能用判断null的方式判断
                        if (array_list1[controlstep2[i][j]->depend1->finish_id].lut_out > 0)  //节点id>=1
                        {
                            int lut_out_now = array_list1[controlstep2[i][j]->depend1->finish_id].lut_out;
                            //当前lut-out的节点的操作类型
                            int on_op = op2int(find_node_by_number(lut_out_now)->operator_name);
                            if (on_op > 0) //不是等号，直接更新lut-out
                                array_list1[controlstep2[i][j]->depend1->finish_id].lut_out = controlstep2[i][j]->node_id;
                            else //否则判断一下其写回表和出度
                            {
                                if (wb_empty(find_node_by_number(lut_out_now))) {
                                    int back_type_ready[3] = {0, 3, 4};
                                    int back_type = back_type_ready[rand() % 3];
                                    int back_id = 0;
                                    if (back_type == 0) back_id = -1;
                                    if (back_type == 3) {
                                        if (array_list3.empty())
                                            back_id = build(3, 0, array_list1, array_list2, array_list3);
                                        else
                                            back_id = array_list3[rand() % array_list3.size()].array_id;
                                    }
                                    if (back_type == 4)
                                        back_id = controlstep2[i][j]->depend1->finish_id;
                                    write_back_lut(1, find_node_by_number(lut_out_now)->finish_id,
                                               find_node_by_number(lut_out_now),
                                               array_list1, array_list2, array_list3, back_type, back_id);
                                }
                            }

                        }

                        //array
                        array_list1[controlstep2[i][j]->depend1->finish_id].lut_out = controlstep2[i][j]->node_id;
                    }
                        break;
                    case 4: //操作数来自lut-buffer
                    {
                        int back_type_ready[2] = {0, 3};
                        int back_type = back_type_ready[rand() % 2];
                        int back_id = 0;
                        if (back_type == 0) back_id = -1;
                        if (back_type == 3) {
                            if (array_list3.empty())
                                back_id = build(3, 0, array_list1, array_list2, array_list3);
                            else
                                back_id = array_list3[rand() % array_list3.size()].array_id;
                        }
                        //也是A=A的类型
                        write_back_lut(4, controlstep2[i][j]->depend1->finish_id, controlstep2[i][j], array_list1,
                                   array_list2, array_list3, back_type, back_id);

                    }
                        break;
                    default:
                        break;

                }

//                //cout << "node_id：" << controlstep2[i][j]->node_id << "  operator_name："
//                     << controlstep2[i][j]->operator_name << endl;
//                //cout << "do_type： " << controlstep2[i][j]->do_type << "  finish_id：" << controlstep2[i][j]->finish_id
//                     << endl;
                //更新出度
                out_degree(controlstep2[i][j]);

                continue;//进行下一个循环
            }//下面的操作没有=了

            //首先确定操作数的个数
            int operand_num = 2;
            if (type_operation == 8) operand_num = 1;//NOT
            //第一步，取操作数,这里的type,id都指的是阵列
            int input1_type = 0, input2_type = 0, input1_id = -1, input2_id = -1;
            //寻找的是节点存储的ID,或者执行完毕的输出位置
            if (operand_num == 1) //1个操作数
            {
                //-1 R 1 lut-out 2 sa-out 3 magic 4 lut-buffer 5 sa-buffer 6 sa
                //判断立即数
                if (controlstep2[i][j]->depend1 != nullptr)
                    find_input(input1_type, input1_id, controlstep2[i][j]->depend1);
                else {
                    //立即数
                    input1_type = -1;
                    input1_id = -1;
                }
            } else    //2个操作数
            {
                //-1 R 1 lut-out 2 sa-out 3 magic 4 lut-buffer 5 sa-buffer 6 sa
                if (controlstep2[i][j]->depend1 != nullptr)
                    find_input(input1_type, input1_id, controlstep2[i][j]->depend1);
                else {
                    //立即数
                    input1_type = -1;
                    input1_id = -1;
                }
                if (controlstep2[i][j]->depend2 != nullptr)
                    find_input(input2_type, input2_id, controlstep2[i][j]->depend2);
                else {
                    //立即数
                    input2_type = -1;
                    input2_id = -1;
                }
            }

            //先决定执行类型
            int do_array_type = 1, do_array_id = -1;//执行阵列的类型,id
//            //cout << "op_type " << op2int(controlstep2[i][j]->operator_name) << endl
//                 << "决定的执行阵列类型：" << do_array_type << endl;
            //更新节点的do_type,执行节点的do_type只有1:lut-out 2:sa-out 3:magic
            controlstep2[i][j]->do_type = do_array_type;
//            //cout << "node " << controlstep2[i][j]->node_id << endl
//                 << "第一次将op的do_type修改后的值为：" << endl
//                 << "do_type: " << controlstep2[i][j]->do_type << endl;
            //如果要执行的类型当前没有阵列，则建立
            if (array_list1.empty())
                do_array_id = build(1, type_operation, array_list1, array_list2, array_list3);
//            //cout << "当前的决定执行阵列的位置为：" << do_array_id << endl;
//            //cout << "op类型的节点选择id之前的三种阵列个数为：" << endl
//                 << array_list1.size() << " " << array_list2.size() << " " << array_list3.size() << endl;
            //决定执行阵列的id
            do_array_id = decide_array_id(type_operation, controlstep2[i][j], do_array_type, array_list1,
                                          array_list2, array_list3, input1_type, input1_id, input2_type, input2_id);

//            //cout << "调用do_array_id后的执行阵列位置为：" << do_array_id << endl;

            if (operand_num == 1) //只有一个操作数，读取
            {
//                //cout << "操作数个数为1" << endl;
                data_read_lut(1, input1_type, input1_id, do_array_type, do_array_id, controlstep2[i][j],
                              array_list1, array_list2, array_list3);
            } else //有两个操作数
            {
//                //cout << "操作数个数为2" << endl;
                data_read_lut(1, input1_type, input1_id, do_array_type, do_array_id, controlstep2[i][j],
                              array_list1, array_list2, array_list3);
                data_read_lut(2, input2_type, input2_id, do_array_type, do_array_id, controlstep2[i][j],
                              array_list1, array_list2, array_list3);
            }
//            //cout << "date_read运行正常" << endl;
            //现在已经知道了操作数的个数operand_num
            //操作数所在的阵列类型：input_type 位置：input_id
            //执行阵列的类型：decide_array_type 位置：decide_array_id
            //将数据输入到执行阵列：input逻辑
            input_logic_lut(operand_num, input1_type, input1_id, input2_type, input2_id, do_array_type, do_array_id,
                            controlstep2[i][j],
                            array_list1, array_list2, array_list3);

            //执行运算,要更新finish_id
            output_logic_lut(do_array_type, do_array_id, type_operation, controlstep2[i][j], array_list1, array_list2,
                         array_list3);

//            //cout << "finish_id of this：" << controlstep2[i][j]->finish_id << endl;
            //写回在最开始，不用在执行
            //只要有写操作，就存在-内部有覆盖的情况，更新出度，清除被覆盖的节点，节点出度为0，找到存储他的阵列，将其擦除
            //补充控制节点，设计比较器件 ，看是否需要加其他运算器，如ALU
            //设计DSE

            //更新出度
            out_degree(controlstep2[i][j]);


        }
    }
    //遍历完控制步，输出延迟、能耗、面积信息


    double all_latency=latency_all(array_list1, array_list2, array_list3);
    double all_energy=energy_all(array_list1, array_list2, array_list3);
    double all_area= area_all_lut(array_list1,array_list2,array_list3);
//    //cout<<"构建的lut阵列个数为: "<<array_list1.size()<<" sa阵列的个数为："<<array_list2.size()<<" magic阵个数为："<<array_list2.size()<<endl;
//    //cout << "整体架构的延迟为： " << all_latency<< "ns" << endl;
//    //cout << "整体架构的能耗为： " << all_energy << "pJ"<<endl;

//    redirect//coutToFile(controlstep2, array_list1, array_list2, array_list3);
    std::vector<double> latency_energy_area={all_latency,all_energy,all_area};
    return latency_energy_area;

}

double time_only_lut(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                     vector<magic_arr> &array_list3, Nodes *node_now, int decide_array_type, int decide_array_id) {
    //函数执行的操作：
    //执行阵列：更新开始时间和结束时间，将使用的阵列置为is_using=true
    //其他阵列，如果当前执行阵列的开始时间比其结束时间要大,将is_using置为false
    double time_n = node_now->end_time;//有初始时间
    double time1 = 0; //depend1的时间
    double time2 = 0; //depend2的时间
    double time3 = 0; //control的时间
    double time_do_array = 0; //阵列的时间
    //假设确定了使用的阵列，那节点开始执行的时间就是使用阵列的结束时间
    //虽然操作并行可能会让op类型比=先执行，但时间和能量均不影响
    switch (decide_array_type) {
        case 1:  //lut,执行
        {
            time_do_array = array_list1[decide_array_id].over_time;
        }
            break;
        case 3: //magic，存储
            time_do_array = array_list3[decide_array_id].over_time;
            break;
        default:
            break;
    }
//    //cout << "time_now_lut" << endl;
    int op_type = op2int(node_now->operator_name);//操作类型
    //先看控制依赖
    if (node_now->control != nullptr) {
        time3 = node_now->control->end_time;
        time_n = max(time_n, time3);
    }
//    //cout << "控制依赖 time3 =" << time3 << endl;
    if (op_type == 0) //写操作,只有一个数据依赖，可能有控制依赖
    {
        if (node_now->depend1 != nullptr) //有依赖 A=b A=B OP C
        {
            //有依赖，这个节点是某一个操作的输出，其开始、结束时间直接等于依赖的结束时间
            if (op2int(node_now->depend1->operator_name) != 0) //依赖的是一个操作
            {
                if (node_now->start_time == 0)
                    node_now->start_time = node_now->depend1->end_time;

                time1 = node_now->depend1->end_time;
            } else //依赖的是一个值节点，还需要看其阵列
            {
                //要将着一个值节点写回，如果值节点来自1,则写回
                if ((time_do_array > time1 || time3 > time1) &&
                    wb_empty(node_now->depend1) &&
                    node_now->depend1->do_type == 1) //阵列结束时间、控制依赖更晚，对于lut-out需要在buffer中保存
                {
//写回 from_type表示写回的类型 0:Reg 1:lut-out 2:sa-out 3:magic存储 4:sa-buffer 5：lut-buffer 6:sa存储
                    //直接写回lut-buffer
                    write_back_lut(node_now->depend1->do_type, node_now->depend1->finish_id, node_now, array_list1,
                               array_list2, array_list3, 4, node_now->depend1->finish_id);
                    time_n = time_only_lut(array_list1, array_list2, array_list3, node_now, decide_array_type,
                                           decide_array_id);//更新时间
                }
            }

            time_n = max(time_n, time1);
        }

    } else //op操作
    {
        if (node_now->depend1 != nullptr) //有依赖
        {
            time1 = node_now->depend1->end_time;
            if ((time_do_array > time1 || time3 > time1) &&
                wb_empty(node_now->depend1) &&
                node_now->depend1->do_type == 1) //控制依赖更晚，对于sa_out和lut-out需要在buffer中保存
            {
//写回 from_type表示写回的类型 0:Reg 1:lut-out 2:sa-out 3:magic存储 4:lut-buffer 5：sa-buffer 6:sa存储
                write_back_lut(node_now->depend1->do_type, node_now->depend1->finish_id, node_now->depend1,
                           array_list1, array_list2, array_list3, 4, node_now->depend1->finish_id);

                time_n = time_only_lut(array_list1, array_list2, array_list3, node_now, decide_array_type,
                                       decide_array_id);//更新时间
            }


            time_n = max(time1, time_n);
        }
        if (node_now->depend2 != nullptr) //2有依赖
        {
            time2 = node_now->depend2->end_time;
            if ((time_do_array > time2 || time3 > time2) &&
                node_now->depend2->do_type == 1 &&
                wb_empty(node_now->depend2)) //阵列结束时间、控制依赖更晚，对于sa_out和lut-out需要在buffer中保存
            {
//写回 from_type表示写回的类型 0:Reg 1:lut-out 2:sa-out 3:magic存储 4:sa-buffer 5：lut-buffer 6:sa存储
                write_back_lut(node_now->depend2->do_type, node_now->depend2->finish_id, node_now->depend2,
                           array_list1, array_list2, array_list3, 4, node_now->depend2->finish_id);
                time_n = time_only_lut(array_list1, array_list2, array_list3, node_now, decide_array_type,
                                       decide_array_id);//更新时间
            }
            time_n = max(time_n, time2);
        }
        time_n = max(time_n, time_do_array);//数据依赖，执行阵列可用的最大时间为当前时间
        //对控制和等待进行讨论
        //1和2如果有时间差，需要将其中一个存储起来
        //存储后时间会被更新
        //对立即数进行讨论
        auto ft = ([=] {
            int ft=0;
            if (node_now->depend1 == nullptr || node_now->depend2 == nullptr)
                return 0;
            else {
                if (time1 > time2 && wb_empty(node_now->depend2)) return 1;//存储2
                if (time2 > time1 && wb_empty(node_now->depend1)) return 2;//存储1
                if (time1 == time2) return 0;
            }
            return ft;
        });
        if (node_now->depend2 != nullptr) //将2存储起来，但是2可能是个立即数，也可能已经存了
        {
            if (ft() == 1 && node_now->depend2->do_type == 1) {
                write_back_lut(node_now->depend2->do_type, node_now->depend2->finish_id, node_now->depend2, array_list1,
                           array_list2, array_list3, 4, node_now->depend2->finish_id);//不确定写到哪了
                //更新时间
                time_n = time_only_lut(array_list1, array_list2, array_list3, node_now, decide_array_type,
                                       decide_array_id);

            }
            //存储其中一个节点，write_in
        }
        if (node_now->depend1 != nullptr) //2
        {
            if (ft() == 2 && node_now->depend1->do_type == 1) { //存储其中一个节点，write_in
                write_back_lut(node_now->depend1->do_type, node_now->depend1->finish_id, node_now->depend1, array_list1,
                           array_list2,
                           array_list3, 4, node_now->depend1->finish_id);//不确定写到哪了
                //更新时间
                time_n = time_only_lut(array_list1, array_list2, array_list3, node_now, decide_array_type,
                                       decide_array_id);
            }
        }
//需要写回的全部写回完毕

    }

    //update the is_using of array
    for (auto &i: array_list1) {
        if (i.over_time < time_n)
            i.is_using = false;
    }
    for (auto &i: array_list3) {
        if (i.over_time < time_n)
            i.is_using = false;
    }

    return time_n;

}

void data_read_lut(int No_depend, int &input_type, int &input_id, int decide_array_type, int decide_array_id, Nodes *now,
                   vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    //如果当前操作数来自寄存器，需要调用寄存器读函数
    //时间和能量加在哪里？
    //根据当前节点，获取当前的时间
    double time_n = time_only_lut(array_list1, array_list2, array_list3, now, decide_array_type, decide_array_id);
//    //cout << "data_read中的time_now运行正常" << endl;
    //从数据读取开始，op节点开始执行
    if (now->start_time == 0)
        now->start_time = time_n;
    if (input_type == -1)//register
    {
        //调用数据读取函数
        //更新读取次数
        Reg_sum.read_num_sum++;
        //更新时间
        read_time_update(-1, -1, time_n, now, array_list1, array_list2, array_list3);
        //更新能量
        read_energy_update(-1, -1, now, array_list1, array_list2, array_list3);
        //读数
        return;
    }
    if (input_type == 1 || input_type == 4) //来自lut阵列
    {
        //两种情况
        //1.是lut的输出
        if (No_depend == 1) //depend1
        {
            //依赖的是lut-out
            if (input_type == 1) //来自lut的输出，写回表肯定是空的
            {
                if (now->start_time != now->depend1->end_time
                    && array_list1[input_id].lut_out != now->depend1->node_id)//对不上
                {
                    //将out写回
                    int back_type_ready[3] = {-1, 3, 4};
                    int back_type = back_type_ready[rand() % 3];
                    int back_id = 0;
                    if (back_type == -1) back_id = -1;
                    if (back_type == 3) {
                        if (array_list3.empty())
                            back_id = build(3, 0, array_list1, array_list2, array_list3);
                        else
                            back_id = array_list3[rand() % array_list3.size()].array_id;
                    }
                    if (back_type == 4) back_id = now->depend1->finish_id;
                    write_back_lut(1, input_id, now->depend1, array_list1, array_list2, array_list3, back_type, back_id);
                    //写回到哪里从哪里接着读取
                    //重新调用find_input
                    if (now->depend1 != nullptr)
                        find_input(input_type, input_id, now->depend1);
                    else {
                        //立即数
                        input_type = -1;
                        input_id = -1;
                    }
                    //修改input_type和input_id继续调用函数
                    data_read_lut(1, input_type, input_id, decide_array_type, decide_array_id, now,
                                  array_list1, array_list2, array_list3);

                }
                //时间刚好,先不写回
                //数据直接传输过去
            } else //来自lut-buffer 4
            {
                //调用数据读取函数
                //更新buffer的读次数
                buffer_sum.buffer_read_sum++;
                //更新时间
                read_time_update(4, input_id, time_n, now, array_list1, array_list2, array_list3);
                //更新能量
                read_energy_update(4, input_id, now, array_list1, array_list2, array_list3);
                //读数
                return;
            }

        }
        if (No_depend == 2) //depend2
        {
            //依赖的是lut-out
            if (input_type == 1) //来自lut的输出,写回表肯定是空的
            {
                if (now->start_time != now->depend2->end_time
                    && array_list1[input_id].lut_out != now->depend2->node_id)//对不上
                {
                    //将out写回
                    //将out写回
                    int back_type_ready[3] = {-1, 3, 4};
                    int back_type = back_type_ready[rand() % 3];
                    int back_id = 0;
                    if (back_type == -1) back_id = -1;
                    if (back_type == 3) {
                        if (array_list3.empty())
                            back_id = build(3, 0, array_list1, array_list2, array_list3);
                        else
                            back_id = array_list3[rand() % array_list3.size()].array_id;
                    }
                    if (back_type == 4) back_id = now->depend2->finish_id;
                    write_back_lut(1, input_id, now->depend2, array_list1, array_list2, array_list3, back_type, back_id);

                    //写回到哪里从哪里接着读取
                    //重新调用find_input
                    if (now->depend2 != nullptr)
                        find_input(input_type, input_id, now->depend2);
                    else {
                        //立即数
                        input_type = -1;
                        input_id = -1;
                    }
                    //修改input_type和input_id继续调用函数
                    data_read_lut(2, input_type, input_id, decide_array_type, decide_array_id, now,
                                  array_list1, array_list2, array_list3);

                }
                //时间刚好,先不写回
                //数据直接传输过去
            } else //来自lut-buffer 4
            {
                //调用数据读取函数
                //更新次数
                buffer_sum.buffer_read_sum++;
                //更新时间
                read_time_update(4, input_id, time_n, now, array_list1, array_list2, array_list3);
                //更新能量
                read_energy_update(4, input_id, now, array_list1, array_list2, array_list3);
                //读数
                return;
            }
        }
    }

    if (input_type == 3)//MA 存储
    {
        //看阵列是否是本阵列，不对应时需要读取！！
        if (decide_array_type != input_type || decide_array_id != input_id) {
            array_list3[input_id].read_number++;
            //更新时间
            read_time_update(input_type, input_id, time_n, now, array_list1, array_list2, array_list3);
            //更新能量
            read_energy_update(input_type, input_id, now, array_list1, array_list2, array_list3);
            return;
        }
    }

//以上都没有讨论直接连线的情况..
}

void input_logic_lut(int operand_num, int input1_type, int input1_id, int input2_type, int input2_id,
                     int decide_array_type, int decide_array_id, Nodes *now,
                     vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    int op_type = op2int(now->operator_name);
    switch (decide_array_type) {
        case 1://LUT
        {
            if (array_list1.empty()) //以防万一，如果lut阵列是空的
                decide_array_id = build(1, op_type, array_list1, array_list2, array_list3);
            return;//LUT输入无需写入
        }

        case 3: //MAGIC
        {
            //不管有几个操作数，都写入
            if (operand_num == 1) {
                //不在一个阵列中，写入
                if (input1_type != 3 || input1_id != decide_array_id) {
                    //获取现在的时间
                    double time_n = time_only_lut(array_list1, array_list2, array_list3, now, decide_array_type,
                                                  decide_array_id);
                    //更新阵列的写次数
                    array_list3[decide_array_id].write_number++;
                    //判断立即数
                    if (now->depend1 != nullptr) {
                        //判断是否需要写覆盖
                        if (cap_array_lost(3, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, 3, decide_array_id, 1, array_list1, array_list2, array_list3);
                        } else {
                            //更新阵列的存储节点表
                            array_list3[decide_array_id].store_node.push_back(now->depend1->node_id);
                            //更新节点的写回表
                            now->depend1->wb_pos[3].push_back(decide_array_id);
                            //更新节点的do_type magic 3
                            now->depend1->do_type = 3;
                            //更新节点的finish_id
                            now->depend1->finish_id = decide_array_id;
                            //更新节点的时间、能量
                            time_update(0, decide_array_type, decide_array_id, time_n, now->depend1, array_list1,
                                        array_list2,
                                        array_list3);
                            energy_update(0, decide_array_type, decide_array_id, array_list1, array_list2, array_list3);
                        }

                    } else //是立即数
                    {
                        if (cap_array_lost(3, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(100, now, 3, decide_array_id, 1, array_list1, array_list2, array_list3);
                        } else {
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
                    double time_n = time_only_lut(array_list1, array_list2, array_list3, now, decide_array_type,
                                                  decide_array_id);
                    //更新阵列的写次数
                    array_list3[decide_array_id].write_number++;
                    //判断立即数
                    if (now->depend1 != nullptr) {
                        //判断是否需要写覆盖
                        if (cap_array_lost(3, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, 3, decide_array_id, 1, array_list1, array_list2, array_list3);
                        } else {
                            //更新阵列的存储节点表
                            array_list3[decide_array_id].store_node.push_back(now->depend1->node_id);
                            //更新节点的写回表
                            now->depend1->wb_pos[3].push_back(decide_array_id);
                            //更新节点的do_type magic 3
                            now->depend1->do_type = 3;
                            //更新节点的finish_id
                            now->depend1->finish_id = decide_array_id;
                            //更新节点的时间、能量
                            time_update(0, decide_array_type, decide_array_id, time_n, now->depend1, array_list1,
                                        array_list2,
                                        array_list3);
                            energy_update(0, decide_array_type, decide_array_id, array_list1, array_list2, array_list3);
                        }

                    } else //是立即数
                    {
                        if (cap_array_lost(3, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(100, now, 3, decide_array_id, 1, array_list1, array_list2, array_list3);
                        } else {
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
                    double time_n = time_only_lut(array_list1, array_list2, array_list3, now, decide_array_type,
                                                  decide_array_id);
                    //更新阵列的写次数
                    array_list3[decide_array_id].write_number++;
                    //判断立即数
                    if (now->depend2 != nullptr) {
                        //判断是否需要写覆盖
                        if (cap_array_lost(3, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, 3, decide_array_id, 1, array_list1, array_list2, array_list3);
                        } else {
                            //更新阵列的存储节点表
                            array_list3[decide_array_id].store_node.push_back(now->depend2->node_id);
                            //更新节点的写回表
                            now->depend2->wb_pos[3].push_back(decide_array_id);
                            //更新节点的do_type magic 3
                            now->depend2->do_type = 3;
                            //更新节点的finish_id
                            now->depend2->finish_id = decide_array_id;
                            //更新节点的时间、能量
                            time_update(0, decide_array_type, decide_array_id, time_n, now->depend2, array_list1,
                                        array_list2,
                                        array_list3);
                            energy_update(0, decide_array_type, decide_array_id, array_list1, array_list2, array_list3);
                        }

                    } else //是立即数
                    {
                        if (cap_array_lost(3, decide_array_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(100, now, 3, decide_array_id, 1, array_list1, array_list2, array_list3);
                        } else {
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

void output_logic_lut(int decide_array_type, int decide_array_id, int op_type, Nodes *now,
                      vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    //执行期间对阵列的影响
    //对节点：需要调用模块时，更新finish_id
    //时间参数的影响
    //1.是否需要调用模块 LUT SA MA 都有设计好的模块供调用
    //获取时间
    double time_n = time_only_lut(array_list1, array_list2, array_list3, now, decide_array_type, decide_array_id);
    op_lut_only(op_type, decide_array_id, now, time_n, array_list1, array_list2, array_list3);
    //需要一个函数判断是否需要调用模块
    //根据op_type，进行每一个阵列下的操作：lut_op\sa_op\ma_op
    return;

}



void write_back_lut(int from_type, int from_id, Nodes *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                    vector<magic_arr> &array_list3, int back_type, int back_id) {
//只能写回lut和reg
    //先列出magic和sa所有能写的阵列，按照优先级进行排序
    auto cap = [&](int array_type, int array_id) {
        return cap_array_lost( array_type, array_id, array_list1, array_list2, array_list3) +
               cap_array_cover( array_type, array_id, array_list1, array_list2, array_list3);
    };

    vector<int> ma_no_using = find_no_using( 0, 3, array_list1, array_list2, array_list3);
    vector<int> ma_waiting = waiting_array_list( 0, 3, array_list1, array_list2, array_list3);
    vector<int> ma_list;
    for (auto i: ma_no_using) {
        if (cap(3, i) > 0)
            ma_list.push_back(i);
    }
    for (auto i: ma_waiting) {
        if (cap(3, i) > 0)
            ma_list.push_back(i);
    }

    if (back_type == -2) { //没有指定写回的位置
        switch (from_type) {
            case -1: //来自寄存器，要写到阵列中
            {

                vector<int> ready_array;
                int write_type = 3;
                //将所有能进行写入的阵列全部列出，进行DSE
                ready_array = ma_list;
                if (ready_array.empty())
                    ready_array.push_back(build( 3, 0, array_list1, array_list2, array_list3));
                int pos = ready_array[rand() % ready_array.size()];
                write_back_lut( -1, -1, now, array_list1, array_list2, array_list3, 3, pos);

            }
                break;
            case 1: //来自lut-out,写回顺位：4 lut-latch、3 ma、0 reg
            {
                vector<int> ready_type = {4, 3, 0}; //写回的顺位
                vector<int> ready_array;
                int write_type = ready_type[rand() % ready_type.size()];
                if (write_type == 4) //buffer,只写回自己阵列
                {
                    write_back_lut( 1, from_id, now, array_list1, array_list2, array_list3, 4, from_id);
                }
                if (write_type == 3) {
                    //将所有能进行写入的阵列全部列出，进行DSE
                    ready_array = ma_list;
                    if (ready_array.empty())
                        ready_array.push_back(build( 3, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back_lut( 1, from_id, now, array_list1, array_list2, array_list3, 3, pos);
                }
                if (write_type == 0) //写回寄存器
                {
                    write_back_lut( 1, from_id, now, array_list1, array_list2, array_list3, 0, -1);
                }

            }
                break;
            case 3: //来自magic存储 ,写回顺位：寄存器0、其他magic 3、sa存储 2
            {
                vector<int> ready_type = {0,3};
                vector<int> ready_array;
                int write_type = ready_type[rand() % ready_type.size()];
                if (write_type == 0) {
                    write_back_lut( 3, from_id, now, array_list1, array_list2, array_list3, 0, -1);
                }
                if (write_type == 3) //写回magic
                {
                    ready_array = ma_list;
                    if (ready_array.empty())
                        ready_array.push_back(build( 3, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back_lut( 3, from_id, now, array_list1, array_list2, array_list3, 3, pos);
                }


            }
                break;
            case 4: //来自lut-buffer。写回顺位：寄存器0、其他magic 3
            {
                vector<int> ready_type = {0, 3};
                vector<int> ready_array;
                int write_type = ready_type[rand() % ready_type.size()];
                if (write_type == 0) {
                    write_back_lut( 4, from_id, now, array_list1, array_list2, array_list3, 0, -1);
                }
                if (write_type == 3) //写回magic
                {
                    ready_array = ma_list;
                    if (ready_array.empty())
                        ready_array.push_back(build( 3, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back_lut( 4, from_id, now, array_list1, array_list2, array_list3, 3, pos);
                }

            }
                break;
            default:
                break;
        }
    } else //指定写回的位置
    {
        switch (from_type) {
            case -1: //寄存器读，阵列写 3
            {
                //从寄存器中读取，更新能量
                Reg_sum.read_num_sum++;
                //将读取时间更新到节点身上
                //使用读时间更新函数
                double time_n = time_only_lut( array_list1, array_list2, array_list3, now, back_type, back_id);
                read_time_update( -1, -1, time_n, now, array_list1, array_list2, array_list3);
//                now->end_time += reg.reg_read_time;
                //更新整体的Reg能量
                read_energy_update( -1, -1, now, array_list1, array_list2, array_list3);
                //判断是否需要写覆盖
                if ((back_type == 2 || back_type == 3) &&
                    cap_array_lost( back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                    write_cover( 0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                    return;
                }
                // 更新阵列写时间
                time_n = time_only_lut( array_list1, array_list2, array_list3, now, back_type, back_id);
                //更新时间,节点，阵列
                time_update( 0, back_type, back_id, time_n, now, array_list1, array_list2, array_list3);
                //阵列写：更新阵列能量
                energy_update( 0, back_type, back_id, array_list1, array_list2, array_list3);
                //更新写回表
                now->wb_pos[back_type].push_back(back_id);
                //更新阵列的结束(执行)类型和位置
                now->do_type = back_type;
                //有两个特殊的处理一下
                if (back_type == 0) now->do_type = -1; //reg
                now->finish_id = back_id;
                //更新阵列存储节点
                array_add_node( back_type, back_id, now, array_list1, array_list2, array_list3);

            }
                break;
            case 1: //来自lut-out
            {
                switch (back_type) {
                    case 0://写回寄存器
                    {
                        //更新寄存器写次数
                        Reg_sum.write_num_sum++;
                        //更新寄存器能量
                        energy_update( 0, -1, -1, array_list1, array_list2, array_list3);
                        //时间
                        double time_n = time_only_lut( array_list1, array_list2, array_list3, now, back_type, back_id);
                        //更新时间,节点，阵列
                        time_update( 0, -1, -1, time_n, now, array_list1, array_list2, array_list3);
                        //更新节点写回表
                        now->wb_pos[0].push_back(-1);

                        //更新阵列的结束(执行)类型和位置
                        now->do_type = -1;//表示写回的是寄存器
                        now->finish_id = back_id;

                    }
                        break;
                    case 3: //写回magic
                    {
                        //阵列写次数++
                        array_list3[back_id].write_number++;
                        //判断是否需要写覆盖
                        if (cap_array_lost( back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover( 0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                            return;
                        }
                        //时间
                        double time_n = time_only_lut( array_list1, array_list2, array_list3, now, back_type, back_id);
                        //更新时间,节点，阵列
                        time_update( 0, back_type, back_id, time_n, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update( 0, back_type, back_id, array_list1, array_list2, array_list3);
                        //更新阵列的结束(执行)类型和位置
                        now->do_type = 3;//代表写回的是magic的存储中
                        now->finish_id = back_id;
                        //更新写回表
                        now->wb_pos[back_type].push_back(back_id);
                        array_add_node( back_type, back_id, now, array_list1, array_list2, array_list3);

                    }
                        break;
                    case 4://lut-out 1 写回lut-buffer 4
                    {
                        //首先要判断buffer中有没有元素，其写回表、出度是否为0
                        //更新buffer伴随着写覆盖问题，写回表中的原来的buffer也要删除掉
                        //buffer：不被覆盖就可以一直存储着
                        if (!array_list1[back_id].lut_latch.empty() &&
                            (array_list1[back_id].lut_latch.back() != now->node_id)) {
                            //先将现在buffer对应的节点[不是now]的写回表中的这个buffer擦除掉
                            wb_erase(find_node_by_number( array_list1[back_id].lut_latch.back()), back_type, back_id);
                            //buffer被擦除了，do_type和finish_id是否也需要更新？
                            //如果其do_type和finish_id都是被擦除的，则将其do_type和finish_id都改为0
                            if (find_node_by_number( array_list1[back_id].lut_latch.back())->do_type == 4
                                && find_node_by_number( array_list1[back_id].lut_latch.back())->finish_id == back_id) {
                                find_node_by_number( array_list1[back_id].lut_latch.back())->do_type = 0;
                                find_node_by_number( array_list1[back_id].lut_latch.back())->finish_id = 0;
                            }

                            //出度>0，写回表为空
                            if (wb_empty(find_node_by_number( array_list1[back_id].lut_latch.back())))
                                //找个地方写回，from_type=4,lut-buffer
                            {
                                //从4.lut-buffer出去的，应该只能写回reg-sa-magic
                                write_back_lut( 4, back_id, find_node_by_number( array_list1[back_id].lut_latch.back()),
                                               array_list1, array_list2, array_list3);
                            }
                        }
                        //清空buffer
                        array_list1[back_id].lut_latch.clear();
                        //更新buffer
                        array_list1[back_id].lut_latch.push_back(now->node_id);//新的buffer输出
                        buffer_sum.buffer_write_sum++;//更新buffer写次数
                        energy_update( 0, 4, back_id, array_list1, array_list2, array_list3); //更新buffer能量
                        //更新节点的end_time
                        double time_n = time_only_lut( array_list1, array_list2, array_list3, now, back_type, back_id);
                        time_update(0, back_type, back_id, time_n, now, array_list1, array_list2, array_list3);

                        //更新节点的结束(执行)类型和位置
                        now->do_type = 4;//代表写回的是lut-buffer
                        now->finish_id = back_id;
                        //更新写回表
                        //更新节点的写回表
                        now->wb_pos[back_type].push_back(back_id);

                    }
                        break;
                    default:
                        break;
                }

            }
                break;
            case 3: //来自magic存储，magic读，寄存器、sa写
            {
                switch (back_type) {
                    case 0://来自magic 3 写回寄存器 0/-1
                    {
                        //从阵列中读数据
                        array_list3[from_id].read_number++;
                        //阵列读时间更新
                        double time_n = time_only_lut(array_list1, array_list2, array_list3, now, from_type, from_id);
                        read_time_update(3, from_id, time_n, now, array_list1, array_list2, array_list3);
                        //读能量更新
                        read_energy_update( 3, from_id, now, array_list1, array_list2, array_list3);
                        //将数据写到寄存器中
                        Reg_sum.write_num_sum++;
                        //更新寄存器写时间
                        time_n = time_only_lut( array_list1, array_list2, array_list3, now, from_type, from_id);
                        time_update( 0, -1, -1, time_n, now, array_list1, array_list2, array_list3);
                        //更新寄存器能量
                        energy_update( 0, -1, -1, array_list1, array_list2, array_list3);
                        //更新节点的结束(执行)类型和位置
                        now->do_type = -1;//代表写回的是寄存器
                        now->finish_id = back_id;
                        now->wb_pos[0].push_back(-1);
                    }
                        break;
                    case 3: //来自magic 3 写回magic
                    {
                        //前提，两个magic不一样
                        if (from_id == back_id) break;
                        //判断是否需要写覆盖
                        if (cap_array_lost( back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover( 0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                            return;
                        }
                        //从阵列中读数据
                        array_list3[from_id].read_number++;
                        //阵列读时间更新
                        double time = time_only_lut( array_list1, array_list2, array_list3, now, from_type, from_id);

                        read_time_update( 3, from_id, time, now, array_list1, array_list2, array_list3);
                        //读能量更新
                        read_energy_update( 3, from_id, now, array_list1, array_list2, array_list3);

                        //阵列写次数++
                        array_list3[back_id].write_number++;
                        //时间
                        time = time_only_lut( array_list1, array_list2, array_list3, now, back_type, back_id);
                        //更新时间,节点，阵列
                        time_update( 0, back_type, back_id, time, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update( 0, back_type, back_id, array_list1, array_list2, array_list3);
                        //更新节点的结束(执行)类型和位置
                        now->do_type = 3;//代表写回的是magic
                        now->finish_id = back_id;
                        //更新节点的写回表
                        now->wb_pos[back_type].push_back(back_id);
                        //更新阵列的存储表
                        array_add_node( back_type, back_id, now, array_list1, array_list2, array_list3);

                    }
                        break;
                    default:
                        break;
                }
            }
                break;
            case 4: //来自lut-buffer，类似于寄存器，buffer读，阵列、寄存器写
            {
                //从buffer中读取，更新能量
                buffer_sum.buffer_read_sum++;
                //将读取时间更新到节点身上
                //使用读时间更新函数
                double time_n = time_only_lut( array_list1, array_list2, array_list3, now);
                read_time_update( 4, from_id, time_n, now, array_list1, array_list2, array_list3);
//                now->end_time += reg.reg_read_time;
                //更新整体的buffer能量
                read_energy_update( 4, from_id, now, array_list1, array_list2, array_list3);
                //判断是否有阵列写覆盖
                if ((back_type == 2 || back_type == 3) &&
                    cap_array_lost( back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                    write_cover( 0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                    return;
                }
                // 更新阵列写时间
                time_n = time_only_lut( array_list1, array_list2, array_list3, now, back_type, back_id);
                //更新时间,节点，阵列
                if (back_type == 0) back_type = -1; //单独处理写回寄存器的类型
                time_update( 0, back_type, back_id, time_n, now, array_list1, array_list2, array_list3);
                //阵列写：更新阵列能量
                energy_update( 0, back_type, back_id, array_list1, array_list2, array_list3);
                //将寄存器类型改回0
                if (back_type == -1) back_type = 0;
                //更新阵列的结束(执行)类型和位置
                now->do_type = back_type;
                //有两个特殊的处理一下
                if (back_type == 0) now->do_type = -1; //reg
                now->finish_id = back_id;
                //更新写回表
                now->wb_pos[back_type].push_back(back_id);
                //更新阵列存储节点
                array_add_node( back_type, back_id, now, array_list1, array_list2, array_list3);

            }
                break;
            default:
                break;
        }

    }

}

void op_lut_only(int op_type, int decide_array_id, Nodes *now, double time_now, vector<lut_arr> &array_list1,
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
        int on_op = op2int(find_node_by_number( lut_out_now)->operator_name);
        if (on_op > 0) //不是等号，直接更新lut-out
            array_list1[decide_array_id].lut_out = now->node_id;
        else //否则判断一下其写回表和出度
        {
            if (wb_empty(find_node_by_number( lut_out_now)))
            {
                //设置优先级，buffer只能写回本阵列
                write_back_lut( 1, find_node_by_number( lut_out_now)->finish_id, find_node_by_number( lut_out_now),
                               array_list1, array_list2, array_list3);
            }
        }

    }
    //将当前节点设置为lut-out
    array_list1[decide_array_id].lut_out=now->node_id;
    // 更新时间
    time_update( op_type, 1, decide_array_id, time_now, now, array_list1, array_list2, array_list3);
//更新能量
    energy_update( op_type, 1, decide_array_id, array_list1, array_list2, array_list3);

}
//
// Created by shenlibo on 23-5-4.
//
