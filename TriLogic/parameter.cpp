#include "parameter.h"
#include "mainfunc.h"

RRAM rram={1.0,1.0,0.02,0.13,1e12};//RRAM参数 全局 ns pj
REG reg={1,1,0.48,0.48,0.002};//单个reg的索引,数据来源：PUMA 1KB Reg
Register Reg_sum={0,0,0,0};
BUFFER buffer={1,1,0.29,0.29,0.0001375};//TODO:这里仅限1bit,数据来源：PUMA
Buffer buffer_sum={0,0,0,0};

Sa_op CSA_and = {6, 0.03, 0.029};//ns pj
Sa_op CSA_or = {7, 0.04, 0.022};
Sa_op CSA_not = {8, 0.04, 0.016};
Sa_op CSA_nor = {9, 0.08, 0.038}; //or + not
Sa_op CSA_xor = {10, 0.32, 0.04};
Sa_op CSA_add = {11, 0.38, 0.069};//add 1bit
std::vector<Sa_op> csa = {CSA_and,CSA_or,CSA_not,CSA_nor,CSA_xor,CSA_add};
SA CSA={0.04,16.79,csa};

Sa_op DSA_and = {6, 0.078, 0.015};//ns pj
Sa_op DSA_or = {7, 0.09, 0.01};
Sa_op DSA_not = {8, 0.09, 0.01};
Sa_op DSA_nor = {9, 0.18, 0.02}; //or + not
Sa_op DSA_xor = {10, 0.10, 0.025};
Sa_op DSA_add = {11, 0.12, 0.026};
std::vector<Sa_op> dsa = {DSA_and,DSA_or,DSA_not,DSA_nor,DSA_xor,DSA_add};
SA DSA={0.09,10.01,dsa};

//需要写spice仿真一下
Ma_op ma_seq={0,rram.write_time,rram.write_energy};//用set reset平均（0.219+0.034）/2
Ma_op ma_nor={9,2.3,0.18};//1.3ns 50fJ+0.13pJ
//FELIX
Ma_op ma_and={6,3.6,0.203};//data from: FELIX AND3
Ma_op ma_or={7,2.3,0.139};
Ma_op ma_not={8,2.3,0.154};
Ma_op ma_xor={10,3.6,0.16};//nand 0.049+0.009
//一共写4.5次
Ma_op ma_add={11,7.2,0.6};//1bit N+4 4是两个xor=7.2ns N每一个都是7（直接全部设置成7.2）
std::vector<Ma_op> magic_op={ma_seq,ma_and,ma_or,ma_not,ma_nor,ma_xor,ma_add};
Ma_Record magic_record={rram.read_energy,rram.write_energy,magic_op};



double lut_latency(int op_type)
{
    if (op_type==0) return reg.reg_write_time;//写到了reg中
    double one_latency=0.104;//一个lut的传输延时，上升沿+下降沿/2（0.048+0.16）/2 单位：ns
    double lut_latency;
//    int lut_num= lut_num_op(op_type,6);
    int lut_level= lut_level_op(op_type,6);
    lut_latency=lut_level*one_latency;
    return lut_latency;

}

double lut_energy(int op_type)
{
    //静态功耗：1.121uW
    //反转能耗：4.1852fJ
    //lut mux:功耗 2.143u
    if (op_type==0) return bit_num_operand*reg.reg_write_energy;//写到了reg中
    double do_time= lut_latency(op_type);
    double static_energy=do_time*3.264;
    double lut_energy;
//    int lut_num= lut_num_op(op_type,6);
    int lut_level= lut_level_op(op_type,6);
    int lut_num= lut_num_op(op_type,6);
    double dynamic_energy=lut_num*0.5*4.2;//动态功耗*翻转率*次数
    lut_energy=(static_energy+dynamic_energy)/1000;//输出pJ

    return lut_energy;
}

double sa_latency(int op_type,int sa_type)
{
    double sa_latency=0;
    if (op_type==0) return rram.read_time;//write_back
    if (sa_type==1) //CSA
    {
        //在CSA的参数表中找到延迟数据，并返回
        for (auto i:CSA.sa_op) {
            if (i.op_type==op_type)
            {
                if (op_type==11)//add
                    sa_latency=i.op_latency*bit_num_operand;
                else
                    sa_latency=i.op_latency;
            }
        }
    }
    else //DSA
    {
        for (auto i:DSA.sa_op) {
            if (i.op_type==op_type)
            {
                if (op_type==11)//add
                    sa_latency=i.op_latency*bit_num_operand;
                else
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


double ma_latency(int op_type)
{
    double ma_latency=0;
    for (auto i:magic_op) {
        if (i.op_type==op_type)
        {
            if (op_type==11)
                ma_latency=i.op_time*(bit_num_operand+4);//N+4
            else
                ma_latency=i.op_time;
        }
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
