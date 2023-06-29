#include "logic_func.h"
#include "parameter.h"

//现在已经决定了从哪里找操作数，在哪里执行，可以直接进行时间更新
//注意，现在只是获取开始的时间
//这里只根据依赖的阵列、节点来获取时间，对reg,buffer更新不需要调用，直接在节点上更改
double time_now(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
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
        case 1: {
            time_do_array = array_list1[decide_array_id].over_time;
        }
            break;
        case 2:
        {
            time_do_array = array_list2[decide_array_id].over_time;
        }
            break;
        case 3:
        {
            time_do_array = array_list3[decide_array_id].over_time;
        }
            break;
        default:
            break;
    }
    int op_type = op2int(node_now->operator_name);//操作类型
    //cout<<"no error time_now op2int"<<endl;
    //先看控制依赖
    if (node_now->control != nullptr) {
        //cout<<"time_now node_now->control"<<endl;
        time3 = node_now->control->end_time;
        time_n = max(time_n, time3);
        //cout<<"no error time_now node_now->control"<<endl;
    }
    if (op_type == 0) //写操作,只有一个数据依赖，可能有控制依赖
    {
        //cout<<"time_now op_type = 0 "<<endl;
        if (node_now->depend1 != nullptr) //有依赖 A=b A=B OP C
        {
            //cout<<"time_now 1 "<<endl;
            //有依赖，这个节点是某一个操作的输出，其开始、结束时间直接等于依赖的结束时间
            if (op2int(node_now->depend1->operator_name) != 0) //依赖的是一个操作
            {
                //cout<<"time_now 2 "<<endl;
                if (node_now->start_time == 0)
                {
                    node_now->start_time = node_now->depend1->end_time;
                    //cout<<"time_now 3 "<<endl;
                }

                time1 = node_now->depend1->end_time;
                //cout<<"time_now 4 "<<endl;
            } else //依赖的是一个值节点，还需要看其阵列
            {
                if ((time_do_array > time1 || time3 > time1) &&
                    wb_empty(node_now->depend1)) //阵列结束时间、控制依赖更晚，对于sa_out和lut-out需要在buffer中保存
                {
//写回 from_type表示写回的类型 0:Reg 1:lut-out 2:sa-out 3:magic存储 4:sa-buffer 5：lut-buffer 6:sa存储
                    write_back(node_now->depend1->do_type, node_now->depend1->finish_id, node_now, array_list1,
                               array_list2, array_list3);
                    //cout<<"no error time_now write_back"<<endl;
                    time_n = time_now(array_list1, array_list2, array_list3, node_now, decide_array_type,
                                      decide_array_id);//更新时间
                    //cout<<"time_now 5 "<<endl;
                }
            }

            time_n = max(time_n, time1);
        }
        //cout<<"no error time_now op_type=0"<<endl;

    } else //op操作
    {
        //cout<<"time_now 6 "<<endl;
        if (node_now->depend1 != nullptr) //有依赖
        {
            time1 = node_now->depend1->end_time;
            //cout<<"time_now 7 "<<endl;
            if ((time_do_array > time1 || time3 > time1) &&
                wb_empty(node_now->depend1)) //控制依赖更晚，对于sa_out和lut-out需要在buffer中保存
            {
                //cout<<"time_now 8 "<<endl;
//写回 from_type表示写回的类型 0:Reg 1:lut-out 2:sa-out 3:magic存储 4:lut-buffer 5：sa-buffer 6:sa存储
                if (node_now->depend1->do_type == 1 || node_now->depend1->do_type == 2) {
                    write_back(node_now->depend1->do_type, node_now->depend1->finish_id, node_now->depend1,
                               array_list1, array_list2, array_list3);
                }
                time_n = time_now(array_list1, array_list2, array_list3, node_now, decide_array_type,
                                  decide_array_id);//更新时间
                //cout<<"time_now 9 "<<endl;
            }


            time_n = max(time1, time_n);
            //cout<<"time_now 10 "<<endl;
        }
        if (node_now->depend2 != nullptr) //2有依赖
        {
            time2 = node_now->depend2->end_time;
            //cout<<"time_now 11 "<<endl;
            if ((time_do_array > time2 || time3 > time2) &&
                wb_empty(node_now->depend2)) //阵列结束时间、控制依赖更晚，对于sa_out和lut-out需要在buffer中保存
            {
                //cout<<"time_now 12 "<<endl;
//写回 from_type表示写回的类型 0:Reg 1:lut-out 2:sa-out 3:magic存储 4:sa-buffer 5：lut-buffer 6:sa存储
                write_back(node_now->depend2->do_type, node_now->depend2->finish_id, node_now->depend2,
                           array_list1, array_list2, array_list3);
                //cout<<"no error time_now write_back"<<endl;
                time_n = time_now(array_list1, array_list2, array_list3, node_now, decide_array_type,
                                  decide_array_id);//更新时间
                //cout<<"time_now 13 "<<endl;
            }
            //cout<<"time_now 11-1 "<<endl;
            time_n = max(time_n, time2);
            //cout<<"time_now 14 "<<endl;
        }
        //cout<<"time_now 15-1 "<<endl;
        time_n = max(time_n, time_do_array);//数据依赖，执行阵列可用的最大时间为当前时间
        //cout<<"time_now 15 "<<endl;
        //对控制和等待进行讨论
        //1和2如果有时间差，需要将其中一个存储起来
        //存储后时间会被更新
        //对立即数进行讨论
        auto ft = ([=] {
            if (node_now->depend1 == nullptr || node_now->depend2 == nullptr)
                return 0;
            else {
                if (time1 > time2 && wb_empty(node_now->depend2)) return 1;//存储2
                if (time2 > time1 && wb_empty(node_now->depend1)) return 2;//存储1
                if (time1 == time2) return 0;
            }
        });
        if (node_now->depend2 != nullptr) //将2存储起来，但是2可能是个立即数，也可能已经存了
        {
            //cout<<"time_now 16 "<<endl;
            if (ft() == 1) {
                write_back(node_now->depend2->do_type, node_now->depend2->finish_id, node_now->depend2, array_list1,
                           array_list2,array_list3);//不确定写到哪了
                //cout<<"no error time_now write_back"<<endl;
                //更新时间
                time_n = time_now(array_list1, array_list2, array_list3, node_now, decide_array_type, decide_array_id);
                //cout<<"time_now 17 "<<endl;

            }
            //存储其中一个节点，write_in
        }
        if (node_now->depend1 != nullptr) //2
        {
            //cout<<"time_now 18 "<<endl;
            if (ft() == 2) { //存储其中一个节点，write_in
                write_back(node_now->depend1->do_type, node_now->depend1->finish_id, node_now->depend1, array_list1,
                           array_list2,
                           array_list3);//不确定写到哪了
                //cout<<"no error time_now write_back"<<endl;
                //更新时间
                time_n = time_now(array_list1, array_list2, array_list3, node_now, decide_array_type, decide_array_id);
                //cout<<"time_now 19 "<<endl;
            }
        }
//需要写回的全部写回完毕

    }
    //cout<<"time_now 20 "<<endl;
    //update the is_using of array
    //cout<<"arraylist1 size = "<<array_list1.size()<<endl;
    for (int i=0;i< array_list1.size();i++) {
        //cout<<"time_now 21 "<<array_list1[i].array_id<<endl;
        if (array_list1[i].over_time < time_n)
        {
            //cout<<"time_now 21 is using 1"<<endl;
            array_list1[i].is_using = false;
            //cout<<"time_now 21 is using 2"<<endl;
        }
    }
    //cout<<"arraylist2 size = "<<array_list2.size()<<endl;
    for (int i=0;i< array_list2.size();i++) {
        //cout<<"time_now 22 "<<array_list2[i].array_id<<endl;
        if (array_list2[i].over_time < time_n)
        {
            //cout<<"time_now 22 is using 1"<<endl;
            array_list2[i].is_using = false;
            //cout<<"time_now 22 is using 2"<<endl;
        }
    }
    //cout<<"arraylist3 size = "<<array_list3.size()<<endl;
    for (int i=0;i< array_list3.size();i++) {
        //cout<<"time_now 23 "<<array_list3[i].array_id<<endl;
        if (array_list3[i].over_time < time_n)
        {
            //cout<<"time_now 23 is using 1"<<endl;
            array_list3[i].is_using = false;
            //cout<<"time_now 23 is using 2"<<endl;
        }
    }
    //cout<<"time_now 24 "<<endl;


    return time_n;

}



//只有写操作和执行的时候才会调用
void time_update(int op_type, int do_type, int decide_array_id, double time_now, Nodes *node_now,
                 vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {

//    //cout<<node_now->node_id<<"节点 开始"<<node_now->start_time<<" 结束"<<node_now->end_time<<endl;
    //set start time of node
    if (node_now->start_time == 0 || op_type == 0)
        node_now->start_time = time_now;
    if (op_type == 0) //写的操作,目标肯定只有 -1(reg) 3(ma) 4(lut-buffer) 5(sa-buffer) 6(sa)
    {
        //对不合理情况进行判断
        switch (do_type) {
            case -1://REG
            {
                node_now->end_time = time_now + reg.reg_write_time;//寄存器写
            }
                break;
            case 3://MAGIC
            {
                array_list3[decide_array_id].start_time = time_now;
                double time_up = ma_latency(op_type);
                node_now->end_time = time_now + time_up;
                array_list3[decide_array_id].over_time = node_now->end_time;
                //set is_using
                array_list3[decide_array_id].is_using = true;
            }
                break;
            case 4://lut buffer
            {
                node_now->end_time = time_now + buffer.buffer_write_time * 32;
            }
                break;
            case 5://sa buffer
            {
                node_now->end_time = time_now + buffer.buffer_write_time * 32;
            }
            case 6://sa存储
            {
                array_list2[decide_array_id].start_time = time_now;
                double time_up = sa_latency(op_type,array_list2[decide_array_id].sa_type);
                //cout<<"sa memory time up = "<< time_up<<endl;
                node_now->end_time = time_now + time_up;
                array_list2[decide_array_id].over_time = node_now->end_time;
                //set is_using
                array_list2[decide_array_id].is_using = true;
            }
                break;
            default:
                break;
        }

    }
    else
    {
        switch (do_type) {
            //对不合理情况进行判断
            case 1://lut
            {
                double time_up = lut_latency(op_type);//使用lut执行当前操作，所需的时间
                array_list1[decide_array_id].start_time = time_now;
                node_now->end_time = time_now + time_up;
                array_list1[decide_array_id].over_time = node_now->end_time;
                //set is_using
                array_list1[decide_array_id].is_using = true;
            }
                break;
            case 2://SA
            {
                array_list2[decide_array_id].start_time = time_now;
//                //cout<<"time_now= "<<time_now<<endl;
                double time_up = sa_latency(op_type, array_list2[decide_array_id].sa_type);
//                //cout<<"op_type = "<<op_type <<" time_up= "<<time_up<<endl;
                node_now->end_time = time_now + time_up;
                array_list2[decide_array_id].over_time = node_now->end_time;
                //set is_using
                array_list2[decide_array_id].is_using = true;
            }
                break;
            case 3://MAGIC
            {
                array_list3[decide_array_id].start_time = time_now;
                double time_up = ma_latency(op_type);
                node_now->end_time = time_now + time_up;
//                //cout << "magic执行后的时间为：" << node_now->end_time << endl;
                array_list3[decide_array_id].over_time = node_now->end_time;
                //set is_using
                array_list3[decide_array_id].is_using = true;
            }
                break;
            default:
                break;
        }
    }

}

//每次读取都得调用这个函数更新时间
//这里的array_type和写回表一样 -1 1 2 3 4 5
//读，主要更新的是阵列的时间。阵列的时间对节点的时间产生连锁反应
void read_time_update(int array_type, int array_id, double time_now, Nodes *now, vector<lut_arr> &array_list1,
                      vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {

    //对不合理情况进行判断
    switch (array_type) {
        case -1://REG
        {
            //寄存器读时间++,将其加到节点上
            now->end_time = max(now->end_time, time_now) + reg.reg_read_time;
        }
            break;
        case 3://MAGIC
        {
            array_list3[array_id].start_time = max(array_list3[array_id].start_time, time_now);
            double time_up = rram.read_time+controler_latency+magic_decoder_latency;
            now->end_time = time_now + time_up;
            array_list3[array_id].over_time = now->end_time;
//set is_using
            array_list3[array_id].is_using = true;
        }
            break;
        case 4://lut buffer
        {
            now->end_time = max(now->end_time, time_now) + buffer.buffer_read_time;
        }
            break;
        case 5://sa buffer
        {
            now->end_time = max(now->end_time, time_now) + buffer.buffer_read_time;
        }
            break;
        case 6: //sa
        {
            array_list2[array_id].start_time = max(array_list2[array_id].start_time, time_now);
            double time_up = sa_latency(8,array_list2[array_id].sa_type)+controler_latency+sa_decoder_latency;
            now->end_time = time_now + time_up;
            array_list2[array_id].over_time = now->end_time;
//set is_using
            array_list2[array_id].is_using = true;
        }
            break;
        default:
            break;

    }
}

void read_energy_update(int array_type, int array_id, Nodes *node_now,
                        vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    //对不合理情况进行判断
    //读的一定是数，-1.reg 3.magic 4.lut-buffer 5.sa-buffer 6.sa
    switch (array_type) {
        case -1://REG
        {
            Reg_sum.read_energy_sum += reg.reg_read_energy;
        }
            break;
        case 3: //magic
        {
            array_list3[array_id].energy += bit_num_operand * rram.read_energy+controler_latency*controler_power
                                                                        +magic_decoder_power*magic_decoder_latency;
        }
            break;
        case 4://sa buffer
        {
            buffer_sum.buffer_read_energy += buffer.buffer_read_energy * bit_num_operand;
        }
            break;
        case 5://lut buffer
        {
            buffer_sum.buffer_read_energy += buffer.buffer_read_energy * bit_num_operand;
        }
            break;
        case 6:  //sa读取，和not（8）损耗的能量是一样的
        {
            array_list2[array_id].energy += sa_energy(8,array_list2[array_id].sa_type);
        }
            break;

        default:
            break;
    }

}

void energy_update(int op_type, int do_type, int array_id, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                   vector<magic_arr> &array_list3) {
    if (op_type == 0) //写的操作,目标肯定只有 -1 3 4 5 6
    {
        //对不合理情况进行判断
        switch (do_type) {
            case -1://REG
            {
                Reg_sum.write_energy_sum += reg.reg_write_energy;
            }
                break;
            case 3: //magic
            {
                array_list3[array_id].energy += ma_energy(op_type);
            }
                break;
            case 4://sa buffer
            {
                buffer_sum.buffer_write_energy += buffer.buffer_write_energy * bit_num_operand;
            }
                break;
            case 5://lut buffer
            {
                buffer_sum.buffer_write_energy += buffer.buffer_write_energy * bit_num_operand;
            }
                break;
            case 6://sa
            {
                array_list2[array_id].energy+= sa_energy(0,array_list2[array_id].sa_type);
            }
                break;
            default:
                break;
        }
    }
    else //执行操作，目标应该只有 1 2 3
    {
        //对不合理情况进行判断
        switch (do_type) {
            case 1: //lut执行
            {
                array_list1[array_id].energy += lut_energy(op_type);
            }
                break;
            case 2: //sa
            {
                array_list2[array_id].energy += sa_energy(op_type, array_list2[array_id].sa_type);
            }
                break;
            case 3: //magic
            {
                array_list3[array_id].energy += ma_energy(op_type);
            }
                break;
            default:
                break;
        }
    }


}



//
// Created by shenlibo on 23-4-19.
//
