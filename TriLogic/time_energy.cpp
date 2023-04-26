#include "logic_func.h"
#include "parameter.h"

//现在已经决定了从哪里找操作数，在哪里执行，可以直接进行时间更新
//TODO:时间更新策略有问题
//注意，现在只是获取开始的时间
double
time_now(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3, Node *node_now,
         int decide_array_type, int decide_array_id) {
    //函数执行的操作：
    //执行阵列：更新开始时间和结束时间，将使用的阵列置为is_using=true
    //其他阵列，如果当前执行阵列的开始时间比其结束时间要大,将is_using置为false
    double time_n = 0;
    double time1 = 0;
    double time2 = 0;
    double time3 = 0;
    double time_do_array = 0;
    //假设确定了使用的阵列，那时间就是使用阵列的结束时间
    //虽然操作并行可能会让op类型比=先执行，但时间和能量均不影响
    switch (decide_array_type) {
        case 1:
            time_do_array = array_list1[decide_array_id].over_time;
            break;
        case 2:
            time_do_array = array_list2[decide_array_id].over_time;
            break;
        case 3:
            time_do_array = array_list3[decide_array_id].over_time;
            break;
        default:
            break;
    }
    int op_type = op2int(node_now->operator_name);//操作类型
    //先看控制依赖
    if (node_now->control) {
        time3 = node_now->control->end_time;
        time_n = max(time_n, time3);
    }
    if (op_type == 0) //写操作,只有一个数据依赖，可能有控制依赖
    {
        if (node_now->depend1) //有依赖
        {
            if (node_now->depend1->do_type == 1 ||
                node_now->depend1->do_type == 2) //依赖的数据的来源是SA/lut的当前输出
                time1 = node_now->depend1->end_time;
            if (node_now->depend1->do_type == 3) //依赖的数据的来源是magic,把写放在了执行中
                time1 = node_now->depend1->end_time;
            //寄存器中的先不管
            //TODO：BUFFER,后续再改
            if (node_now->depend1->do_type == 4 ||
                node_now->depend1->do_type == 5)
                time1 = node_now->depend1->end_time;

            if ((time_do_array>time1||time3 > time1) && wb_empty(node_now->depend1)) //阵列结束时间、控制依赖更晚，对于sa_out和lut-out需要在buffer中保存
            {
//写回 from_type表示写回的类型 0:Reg 1:lut-out 2:sa-out 3:magic存储 4:sa-buffer 5：lut-buffer 6:sa存储
                write_back(node_now->depend1->do_type,node_now->depend1->finish_id, node_now, array_list1, array_list2, array_list3);
                time_n = time_now(array_list1, array_list2, array_list3, node_now, decide_array_type,
                                  decide_array_id);//更新时间
            }
            time_n = max(time_n, time1);
        }

    } else //op操作
    {
        if (node_now->depend1) //有依赖
        {
            if (node_now->depend1->do_type == 1 ||
                node_now->depend1->do_type == 2) //依赖的数据的来源是SA/lut的当前输出
                time1 = node_now->depend1->end_time;
            if (node_now->depend1->do_type == 3) //依赖的数据的来源是magic,把写放在了执行中
                time1 = node_now->depend1->end_time;
            //暂时不管buffer
            //TODO：BUFFER,后续再改
            if (node_now->depend1->do_type == 4 ||
                node_now->depend1->do_type == 5)
                time1 = node_now->depend1->end_time;
            if ((time_do_array>time1||time3 > time1) && wb_empty(node_now->depend1)) //控制依赖更晚，对于sa_out和lut-out需要在buffer中保存
            {
//写回 from_type表示写回的类型 0:Reg 1:lut-out 2:sa-out 3:magic存储 4:sa-buffer 5：lut-buffer 6:sa存储
                write_back(node_now->depend1->do_type,node_now->depend1->finish_id, node_now, array_list1, array_list2, array_list3);
                time_n = time_now(array_list1, array_list2, array_list3, node_now, decide_array_type,decide_array_id);//更新时间
            }

            time_n = max(time1, time_n);
        }
        if (node_now->depend2) //有依赖
        {
            if (node_now->depend2->do_type == 1 ||
                node_now->depend2->do_type == 2) //依赖的数据的来源是SA/lut的当前输出
                time2 = node_now->depend2->end_time;
            if (node_now->depend2->do_type == 3) //依赖的数据的来源是magic,把写放在了执行中
                time2 = node_now->depend2->end_time;
            //TODO：BUFFER,后续再改
            if (node_now->depend2->do_type == 4 ||
                node_now->depend2->do_type == 5)
                time2 = node_now->depend2->end_time;
            if ((time_do_array>time2||time3 > time2) && wb_empty(node_now->depend2)) //阵列结束时间、控制依赖更晚，对于sa_out和lut-out需要在buffer中保存
            {
//写回 from_type表示写回的类型 0:Reg 1:lut-out 2:sa-out 3:magic存储 4:sa-buffer 5：lut-buffer 6:sa存储
                write_back(node_now->depend2->do_type,node_now->depend2->finish_id, node_now, array_list1, array_list2, array_list3);
                time_n = time_now(array_list1, array_list2, array_list3, node_now, decide_array_type,
                                  decide_array_id);//更新时间
            }
            time_n = max(time_n, time2);
        }
        time_n = max(time_n, time_do_array);//数据依赖，执行阵列可用的最大时间为当前时间
        //对控制和等待进行讨论
        //1和2如果有时间差，需要将其中一个存储起来
        //存储后时间会被更新
        auto ft = ([=] {
            if (time1 > time2 && wb_empty(node_now->depend2)) return 1;//存储2
            if (time2 > time1 && wb_empty(node_now->depend1)) return 2;//存储1
            if (time1 == time2) return 0;
        });
        if (ft() == 1) //2
        {
            //存储其中一个节点，write_in
            write_back(node_now->depend2->do_type,node_now->depend2->finish_id, node_now, array_list1, array_list2, array_list3);//不确定写到哪了
            //更新时间
            time_n = time_now(array_list1, array_list2, array_list3, node_now, decide_array_type, decide_array_id);
        }
        if (ft() == 2) //2
        {
            //存储其中一个节点，write_in
            write_back(node_now->depend1->do_type, node_now->depend2->finish_id,node_now, array_list1, array_list2, array_list3);//不确定写到哪了
            //更新时间
            time_n = time_now(array_list1, array_list2, array_list3, node_now, decide_array_type, decide_array_id);
        }
//需要写回的全部写回完毕

    }

    //update the is_using of array
    for (auto i: array_list1) {
        if (i.over_time < time_n)
            i.is_using = false;
    }
    for (auto i: array_list2) {
        if (i.over_time < time_n)
            i.is_using = false;
    }
    for (auto i: array_list3) {
        if (i.over_time < time_n)
            i.is_using = false;
    }

    return time_n;

}

//TODO:设置时间库，直接引用
void time_update(int op_type, int decide_array_type, int decide_array_id, double time_now, Node *node_now,
                 vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    //set start time of node
    node_now->start_time = time_now;
    if (decide_array_id == -1 && decide_array_type == -1) //写到寄存器中
    {
        //TODO:设置为写到寄存器中的时间
        node_now->end_time = time_now + reg.reg_write_time;//寄存器写
        return;
    }
    switch (decide_array_type) {
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
            double time_up = sa_latency(op_type, 1);//TODO:dse change sa type
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
            array_list3[decide_array_id].over_time = node_now->end_time;
            //set is_using
            array_list3[decide_array_id].is_using = true;
        }
            break;
        default:
            break;

    }
}

//TODO:，每次读都得调用这个函数来更新其能量
void read_energy_update(int array_type, int array_id, Node *node_now,
                        vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {

    switch (array_type) {
        case -1://REG
        {
            Reg_sum.read_energy_sum += reg.reg_read_energy;
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
        case 1: {
            array_list1[array_id].energy += bit_num_operand * rram.read_energy;
        }
            break;
        case 2: {
            //TODO:sa_type
            array_list2[array_id].energy +=
                    bit_num_operand * rram.read_energy + bit_num_operand * CSA.read_energy;//RRAM的读能耗+CSA的
            //array_list2[array_id].energy += bit_num_operand*rram.read_energy+ bit_num_operand*DSA.read_energy;//RRAM的读能耗+DSA的
        }
            break;
        case 3: {
            array_list3[array_id].energy += bit_num_operand * rram.read_energy;
        }
            break;
        default:
            break;
    }

}


//TODO:，每次写、执行都得调用这个函数来更新其能量
void energy_update(int op_type, int array_type, int array_id, Node *node_now,
                   vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {

    switch (array_type) {
        case -1://REG
        {
            Reg_sum.write_energy_sum += reg.reg_write_energy;
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
        case 1: //lut执行
        {
            array_list1[array_id].energy += lut_energy(op_type);
        }
            break;
        case 2: //sa
        {
            //TODO:sa_type
            array_list2[array_id].energy += sa_energy(op_type, 1);
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



//
// Created by shenlibo on 23-4-19.
//
