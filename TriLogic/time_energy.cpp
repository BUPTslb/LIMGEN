#include "logic_func.h"
#include "parameter.h"

//现在已经决定了从哪里找操作数，在哪里执行，可以直接进行时间更新
//TODO:时间更新策略有问题
double
time_now(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3, Node *node_now) {
    //函数执行的操作：
    //执行阵列：更新开始时间和结束时间，将使用的阵列置为is_using=true
    //其他阵列，如果当前执行阵列的开始时间比其结束时间要大,将is_using置为false
    double time_now = 0;
    if (node_now->depend1) {
        double time1 = 0;
        if (node_now->depend1->do_type != -1)
            time1 = node_now->depend1->end_time;
        time_now = time1;
    }
    if (node_now->depend2) {
        double time2 = 0;
        if (node_now->depend2->do_type != -1)
            time2 = node_now->depend2->end_time;
        time_now = max(time_now, time2);
    }
    if (node_now->control) {
        double time3 = node_now->control->end_time;
        time_now = max(time_now, time3);
    }
    //update the is_using of array
    for (auto i: array_list1) {
        if (i.over_time < time_now)
            i.is_using = false;
    }
    for (auto i: array_list2) {
        if (i.over_time < time_now)
            i.is_using = false;
    }
    for (auto i: array_list3) {
        if (i.over_time < time_now)
            i.is_using = false;
    }

    return time_now;

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
                   vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3)
{

    switch (array_type) {
        case -1://REG
        {
            Reg_sum.read_energy_sum+=reg.reg_read_energy;
        }
            break;
        case 4://sa buffer
        {
            buffer_sum.buffer_read_energy+=buffer.buffer_read_energy*bit_num_operand;
        }
            break;
        case 5://lut buffer
        {
            buffer_sum.buffer_read_energy+=buffer.buffer_read_energy*bit_num_operand;
        }
            break;
        case 1: {
            array_list1[array_id].energy += bit_num_operand * rram.read_energy;
        }
            break;
        case 2: {
            //TODO:sa_type
            array_list2[array_id].energy += bit_num_operand*rram.read_energy+ bit_num_operand*CSA.read_energy;//RRAM的读能耗+CSA的
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
void energy_update(int op_type,int array_type, int array_id, Node *node_now,
                   vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {

    switch (array_type) {
        case -1://REG
        {
            Reg_sum.write_energy_sum+=reg.reg_write_energy;
        }
            break;
        case 4://sa buffer
        {
            buffer_sum.buffer_write_energy+=buffer.buffer_write_energy*bit_num_operand;
        }
            break;
        case 5://lut buffer
        {
            buffer_sum.buffer_write_energy+=buffer.buffer_write_energy*bit_num_operand;
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
            array_list2[array_id].energy += sa_energy(op_type,1);
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
