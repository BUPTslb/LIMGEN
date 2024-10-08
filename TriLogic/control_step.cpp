#include <cmath>
#include "mainfunc.h"
#include "logic_func.h"
#include "dse.h"
//将所有阵列进行编号
vector<Array_place> place_list;

std::vector<double> control_step(vector<vector<Nodes *>> &controlstep2, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                                 vector<magic_arr> &array_list3) {
    for (int i = 0; i < controlstep2.size(); i++) {
        for (int j = 0; j < controlstep2[i].size(); ++j) {
            //一层层遍历控制步，获取操作数，获取算子，分配阵列，计算速度、功耗、面积
            int type_operation = op2int(controlstep2[i][j]->operator_name);

            //数据位数，bit_num_operand
            //先不管立即数
            //多种方式遍历，然后再剪枝
            //对于当前算子，目标是：找一个阵列/新建一个阵列，然后计算其参数增加，就结束

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
                    double time = time_now(array_list1, array_list2, array_list3, controlstep2[i][j]);
                    //update the time of do_array
                    time_update(0, -1, -1, time, controlstep2[i][j], array_list1, array_list2, array_list3);
                    //update the energy of reg
                    energy_update(0, -1, -1, array_list1, array_list2, array_list3);
                    //更新wb_pos,表示写到了寄存器中
                    controlstep2[i][j]->wb_pos[0].push_back(-1);
                    //输出：
                    continue;//进行下一个循环
                }

                //有依赖,则依赖一定来自OP, from array
                //统一，更改节点的执行结束id和do_type
                controlstep2[i][j]->finish_id = controlstep2[i][j]->depend1->finish_id; //op的结束阵列id
                double time_n = time_now(array_list1, array_list2, array_list3, controlstep2[i][j]);//开始执行当前节点的时间
                //如果默认其为直接输出的out,则不会对其他产生任何影响，不需要进行time_update
                //c=a op b || c=a op -1 || c = a
                switch (controlstep2[i][j]->depend1->do_type) {
                    case -1:  //来自寄存器，不知到写回到哪里,直接赋值应该不用写的
                    {
                        write_back(-1, -1, controlstep2[i][j], array_list1, array_list2, array_list3);
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
                                if (find_node_by_number(lut_out_now)->out_degree > 0 &&
                                    wb_empty(find_node_by_number(lut_out_now))) {
                                    write_back(1, find_node_by_number(lut_out_now)->finish_id,
                                               find_node_by_number(lut_out_now),
                                               array_list1, array_list2, array_list3);
                                }
                            }

                        }

                        //array
                        array_list1[controlstep2[i][j]->depend1->finish_id].lut_out = controlstep2[i][j]->node_id;
                    }
                        break;
                    case 2: //来自sa的执行结果
                    {
                        //目前这个节点的写回表是空的
                        controlstep2[i][j]->do_type = 2;
                        controlstep2[i][j]->end_time = time_n;//暂时设置时间
                        //阵列行为：
                        //对现在的sa-out进行处理,其有一个初始值，不能用判断null的方式判断
                        if (array_list2[controlstep2[i][j]->depend1->finish_id].sa_out > 0)  //节点id>=1
                        {
                            int sa_out_now = array_list2[controlstep2[i][j]->depend1->finish_id].sa_out;
                            //当前lut-out的节点的操作类型
                            int on_op = op2int(find_node_by_number(sa_out_now)->operator_name);
                            if (on_op > 0) //不是等号，直接更新lut-out
                                array_list2[controlstep2[i][j]->depend1->finish_id].sa_out = controlstep2[i][j]->node_id;
                            else //否则判断一下其写回表和出度
                            {
                                if (find_node_by_number(sa_out_now)->out_degree > 0 &&
                                    wb_empty(find_node_by_number(sa_out_now))) {
                                    write_back(2, find_node_by_number(sa_out_now)->finish_id,
                                               find_node_by_number(sa_out_now),
                                               array_list1, array_list2, array_list3);
                                }
                            }

                        }
                        //处理本节点
                        array_list2[controlstep2[i][j]->depend1->finish_id].sa_out = controlstep2[i][j]->node_id;

                    }
                        break;
                    case 3://如果操作数来自MAGIC阵列,写回阵列中
                    {
                        //节点行为
                        controlstep2[i][j]->do_type = 3;
                        controlstep2[i][j]->finish_id = controlstep2[i][j]->depend1->finish_id;
                        controlstep2[i][j]->wb_pos[3].push_back(controlstep2[i][j]->depend1->finish_id);//将阵列加入写回表,magic
                        //阵列行为：写++，添加存储节点
                        array_list3[controlstep2[i][j]->depend1->finish_id].write_number++;
                        //阵列写，但是不用能量更新，因为magic执行时直接写了，这里的写能量被算在执行能量中
                        //                    energy_update(type_operation,3,controlstep2[i][j]->depend1->finish_id, controlstep2[i][j],array_list1, array_list2, array_list3);
                        //这里的写操作是否进行了覆盖？
                        //对depend1执行的阵列容量进行分析
                        if (cap_array_lost(3, controlstep2[i][j]->depend1->finish_id, array_list1, array_list2,
                                           array_list3) == 0) {
                            //剩余容量为0，需要进行写覆盖
                            write_cover(0, controlstep2[i][j], 3, controlstep2[i][j]->depend1->finish_id, 1,
                                        array_list1, array_list2, array_list3);
                        }

                        //将该节点添加到阵列存储表中
                        array_add_node(3, controlstep2[i][j]->depend1->finish_id, controlstep2[i][j], array_list1,
                                       array_list2, array_list3);

                    }
                        break;
                        //下边这几个应该不会出现
                    case 4: //操作数来自lut-buffer
                    {
                        //也是A=A的类型
                        write_back(4, controlstep2[i][j]->depend1->finish_id, controlstep2[i][j], array_list1,
                                   array_list2, array_list3);

                    }
                        break;
                    case 5: //来自sa-buffer
                    {
                        write_back(5, controlstep2[i][j]->depend1->finish_id, controlstep2[i][j], array_list1,
                                   array_list2, array_list3);
                    }
                        break;
                    case 6: //来自sa存储
                    {
                        write_back(6, controlstep2[i][j]->depend1->finish_id, controlstep2[i][j], array_list1,
                                   array_list2, array_list3);
                    }
                        break;
                    default:
                        break;

                }

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
                {
                    find_input(input1_type, input1_id, controlstep2[i][j]->depend1);
                }
                else {
                    //立即数
                    input1_type = -1;
                    input1_id = -1;
                }
            } else    //2个操作数
            {
                //-1 R 1 lut-out 2 sa-out 3 magic 4 lut-buffer 5 sa-buffer 6 sa
                if (controlstep2[i][j]->depend1 != nullptr)
                {
                    find_input(input1_type, input1_id, controlstep2[i][j]->depend1);
                }
                else {
                    //cout<<"control step 7 "<<endl;
                    //立即数
                    input1_type = -1;
                    input1_id = -1;
                    //cout<<"control step 8 "<<endl;
                }
                if (controlstep2[i][j]->depend2 != nullptr)
                {
                    //cout<<"control step 9 "<<endl;
                    find_input(input2_type, input2_id, controlstep2[i][j]->depend2);
                    //cout<<"no error with find_input "<<endl;
                }
                else {
                    //cout<<"control step 10 "<<endl;
                    //立即数
                    input2_type = -1;
                    input2_id = -1;//cout<<"control step 11 "<<endl;
                }
            }

            //先决定执行类型
            int do_array_type = 0, do_array_id = -1;//执行阵列的类型,id
            //cout<<"control step 12 "<<endl;
            do_array_type = decide_array_type(type_operation, design_target);//决定执行阵列类型 1 2 3
            //更新节点的do_type,执行节点的do_type只有1:lut-out 2:sa-out 3:magic
            controlstep2[i][j]->do_type = do_array_type;
            //如果要执行的类型当前没有阵列，则建立
            if (do_array_type == 1 && array_list1.empty() ||
                do_array_type == 2 && array_list2.empty() ||
                do_array_type == 3 && array_list3.empty())
                do_array_id = build(do_array_type, type_operation, array_list1, array_list2, array_list3);
            //决定执行阵列的id
            else
                do_array_id = decide_array_id(type_operation, controlstep2[i][j], do_array_type, array_list1,
                                          array_list2, array_list3, input1_type, input1_id, input2_type, input2_id);

            /*现在已知：
             * 操作数1所在阵列类型，id；
             * 操作数2所在阵列类型，id；
             * 执行阵列类型，执行阵列id；
            * 现在还没有进行读写执行
             *  节点的内容修改：
             *  阵列的内容修改：
            * */
            //从阵列中将数据读出，修改阵列/寄存器的读参数
            //需要用到时间，如果out是=且和时间不对应，那就需要将out存储
            //input_type的类型有：-1(寄存器) 1(lut) 2(sa) 3(magic)
            if (operand_num == 1) //只有一个操作数，读取
            {
                data_read(1, input1_type, input1_id, do_array_type, do_array_id, controlstep2[i][j],
                          array_list1, array_list2, array_list3);
            } else //有两个操作数
            {
                data_read(1, input1_type, input1_id, do_array_type, do_array_id, controlstep2[i][j],
                          array_list1, array_list2, array_list3);
                data_read(2, input2_type, input2_id, do_array_type, do_array_id, controlstep2[i][j],
                          array_list1, array_list2, array_list3);
            }
            //现在已经知道了操作数的个数operand_num
            //操作数所在的阵列类型：input_type 位置：input_id
            //执行阵列的类型：decide_array_type 位置：decide_array_id
            //将数据输入到执行阵列：input逻辑
            input_logic(operand_num, input1_type, input1_id, input2_type, input2_id, do_array_type, do_array_id,
                        controlstep2[i][j],
                        array_list1, array_list2, array_list3);
            //执行运算,要更新finish_id
            output_logic(do_array_type, do_array_id, type_operation, controlstep2[i][j], array_list1, array_list2,
                         array_list3);
            //写回在最开始，不用在执行
            //只要有写操作，就存在-内部有覆盖的情况，更新出度，清除被覆盖的节点，节点出度为0，找到存储他的阵列，将其擦除
            //补充控制节点，设计比较器件 ，看是否需要加其他运算器，如ALU
            //设计DSE

            //更新出度
            out_degree(controlstep2[i][j]);

        }
    }

    //开始执行模拟退火布局布线
    place_list.clear();

    int num_array=array_list1.size()+array_list2.size()+array_list3.size();//阵列个数
    int **data_transfer=new int * [num_array];//数据传输次数
    for (int i = 0; i < num_array; ++i) {
        data_transfer[i]=new int[num_array];
        for (int j = 0; j < num_array; ++j) {
            data_transfer[i][j]=0;
        }
    }
    //由于LUT阵列太大，要进行拆分，因此要设计一个函数，将LUT阵列拆分并添加其连接关系
    //N个阵列为一个LUT模块，其中一个阵列作为输入，另一个作为输出，其他阵列作为中间阵列

    for (int i = 0; i < array_list1.size(); ++i) {
        Array_place place_arr;
        place_arr.array_id = i;
        place_arr.array_type = 1;
        place_arr.array_width = array_list1[i].lut_num;
        place_arr.array_height = array_list1[i].row_num;
        place_arr.pos_x = 0;
        place_arr.pos_y = 0;
        place_list.push_back(place_arr);
        //遍历array_list1的data_exchange
        place_num(array_list1[i].data_exchange, data_transfer, i, array_list1.size(), array_list2.size());//???
    }
    for (int i = array_list1.size(); i <array_list1.size()+array_list2.size(); ++i) {
        Array_place place_arr{};
        place_arr.array_id = i;
        place_arr.array_type = 2;
        place_arr.array_width = array_list2[i-array_list1.size()].col_num;
        place_arr.array_height = array_list2[i-array_list1.size()].row_num;
        place_arr.pos_x = 0;
        place_arr.pos_y = 0;
        place_list.push_back(place_arr);
        place_num(array_list2[i - array_list1.size()].data_exchange, data_transfer, i, array_list1.size(),
                  array_list2.size());//???
    }
    for (int i = array_list1.size()+array_list2.size(); i <num_array; ++i) {
        Array_place place_arr{};
        place_arr.array_id = i;
        place_arr.array_type = 3;
        place_arr.array_width = array_list3[i-array_list1.size()-array_list2.size()].col_num;
        place_arr.array_height = array_list3[i-array_list1.size()-array_list2.size()].row_num;
        place_arr.pos_x = 0;
        place_arr.pos_y = 0;
        place_list.push_back(place_arr);
        place_num(array_list3[i - array_list1.size() - array_list2.size()].data_exchange, data_transfer, i,
                  array_list1.size(), array_list2.size());//???
    }
    //ID从零开始，获取每个阵列的数据传输矩阵
    for (int i = 0; i < num_array; ++i) {
        vector<int> data_transfer_vector_temp;
        for (int j = 0; j < num_array ; ++j) {
            data_transfer_vector_temp.push_back(data_transfer[i][j]);
        }
        place_list[i].connect_line=data_transfer_vector_temp;
    }





    //规定xy的界限
    int x_max=ceil(sqrt(num_array));
    int y_max=x_max;
    int **Place_array=new int *[x_max];
    for (int i = 0; i < x_max; ++i) {
        Place_array[i]=new int[y_max];
        for (int j = 0; j < y_max; ++j) {
            Place_array[i][j]=-1;
        }
    }
    //向布局矩阵中填写阵列的id
    double initialTemperature=1e10;
    double finalTemperature=1e-10;
    double coolingRate=0.95;
    int maxIterations=1e6;
    double threshold=1e-30;
    simulateAnnealing(place_list,Place_array,data_transfer, num_array,x_max,y_max, initialTemperature, finalTemperature,
            coolingRate, maxIterations, threshold);
    //将坐标值更新到 place_list;
    //释放空间
    for (int i = 0; i < num_array; ++i) {
        delete [] data_transfer[i];
    }
    delete [] data_transfer;
    for (int i = 0; i < x_max; ++i) {
        delete [] Place_array[i];
    }
    delete [] Place_array;


    //遍历完控制步，输出延迟、能耗、面积信息
    double all_latency=latency_all(array_list1, array_list2, array_list3);
    double all_energy=energy_all(array_list1, array_list2, array_list3);
    double all_area= area_all(array_list1,array_list2,array_list3);

    std::vector<double> latency_energy_area={all_latency,all_energy,all_area};
    return latency_energy_area;


}
//
// Created by shenlibo on 23-5-1.
//
