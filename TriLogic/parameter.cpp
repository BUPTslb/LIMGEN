#include "parameter.h"
double lut_latency(int op_type)
{
    if (op_type==0) return reg.reg_write_time;//写到了reg中
    double one_latency=0.69;//TODO:待测
    double lut_latency;
//    int lut_num= lut_num_op(op_type,6);
    int lut_level= lut_level_op(op_type,6);
    lut_latency=lut_level*one_latency;
    return lut_latency;

}

double lut_energy(int op_type)
{
    if (op_type==0) return bit_num_operand*reg.reg_write_energy;//写到了reg中
    double one_energy=0.69;//TODO:待测
    double lut_energy;
//    int lut_num= lut_num_op(op_type,6);
    int lut_level= lut_level_op(op_type,6);
    lut_energy=lut_level*one_energy;
    return lut_energy;
}

double sa_latency(int op_type,int sa_type)
{
    double sa_latency=0;
    if (op_type==0) return rram.read_time;//write_back
    if (sa_type==1) //CSA
    {
        //在CSA的参数表中找到延迟数据，并返回
        //TODO：无法并行的ADD,做特殊处理
        for (auto i:CSA.sa_op) {
            if (i.op_type==op_type)
            {
                sa_latency=i.op_latency;
            }
        }
    }
    else //DSA
    {
        for (auto i:DSA.sa_op) {
            if (i.op_type==op_type)
            {
                sa_latency=i.op_latency;
            }
        }

    }
    return sa_latency;

}

double sa_energy(int op_type,int sa_type)
{
    double sa_energy=0;
//    if (!op_type)
//    {
//        if (sa_type==1) return bit_num_operand*CSA.read_energy;
//        else    return bit_num_operand*DSA.read_energy;
//    }

    if (op_type==0) return bit_num_operand*rram.write_energy;
    if (sa_type==1) //CSA
    {
        //在CSA的参数表中找到延迟数据，并返回
        //TODO：无法并行的ADD,做特殊处理
        for (auto i:CSA.sa_op) {
            if (i.op_type==op_type)
            {
                sa_energy=bit_num_operand*i.op_energy;
            }
        }
    }
    else //DSA
    {
        for (auto i:DSA.sa_op) {
            if (i.op_type==op_type)
            {
                sa_energy=bit_num_operand*i.op_energy;
            }
        }
    }
    return sa_energy;
}

//TODO:ADD
double ma_latency(int op_type)
{
    double ma_latency=0;
    for (auto i:magic_op) {
        if (i.op_type==op_type)
            ma_latency=i.op_time;
    }
    return ma_latency;
}

double ma_energy(int op_type)
{
    double ma_energy=0;
    for (auto i:magic_op) {
        if (i.op_type==op_type)
            ma_energy=bit_num_operand*i.op_energy;
    }
    return ma_energy;

}




//
// Created by shenlibo on 23-4-20.
//
