#ifndef SYNTHESIS_PARAMETER_H
#define SYNTHESIS_PARAMETER_H
#include <vector>
#include "mainfunc.h"
// 1 GHz : 1ns /cycle
// 1 ns
// 1 mm^2
// 1 pJ
// 1 uW
struct RRAM{
    double read_time;
    double write_time;
    double read_energy;
    double write_energy;
    double t1r1_area;
//    double set_energy;//0.22pj
//    double reset_energy;//0.03pj
    double write_max;
};
extern RRAM rram;

extern double controler_area;//控制器的面积
extern double controler_power;//控制器的功耗
extern double controler_latency;//控制器的延迟
extern double sa_decoder_area;//sa译码器的面积
extern double sa_decoder_power;//sa译码器的功耗
extern double sa_decoder_latency;//sa译码器的延迟
extern double sa_driver_area;//SA驱动器的面积
extern double sa_driver_power;//sa驱动器的功耗
extern double sa_driver_latency;//sa驱动器的延迟
extern double sa_direct_area;//sa直接输入
extern double sa_wb_area;//sa写回面积
extern double sa_arr_wb_area;//sa写回面积
extern double lut_decoder_area;//lut译码器的面积
extern double lut_decoder_power;//lut译码器的功耗
extern double lut_decoder_latency;//lut译码器的延迟
extern double lut_driver_area;//lut驱动器的面积
extern double lut_driver_power;//lut驱动器的功耗
extern double lut_driver_latency;//lut驱动器的延迟
extern double magic_decoder_area;//magic译码器的面积
extern double magic_decoder_power;//magic译码器的功耗
extern double magic_decoder_latency;//magic译码器的延迟
extern double magic_add;//magic add的面积

extern double mux16_area;
extern double mux16_latency;
extern double mux16_power;
extern double mux64_area;
extern double mux64_power;
extern double mux64_latency;


//单个Reg 1KB
struct REG{
    double reg_read_time;//ns
    double reg_write_time;//ns 0.09
    double reg_read_energy;//pj
    double reg_write_energy;//按照32位的算
    double reg_area;//F^2 32bit
};
extern REG reg;

//计算整体的reg参数
struct Register{
    int write_num_sum;
    int read_num_sum;
    double write_energy_sum;
    double read_energy_sum;
    double Reg_area;
};
extern Register Reg_sum;

//单个buffer的参数
struct BUFFER{
    double buffer_read_time;//ns
    double buffer_write_time;
    double buffer_read_energy;//pj
    double buffer_write_energy;
    double buffer_area;//F^2
};
extern BUFFER buffer;

//计算整体的buffer参数,主要是sa\lut的输出buffer
struct Buffer{
    int buffer_read_sum;
    int buffer_write_sum;
    double buffer_read_energy;
    double buffer_write_energy;
};
extern Buffer buffer_sum;

struct Sa_op {
    int op_type;
    double op_latency;
    double op_energy;
};

struct SA {
    double read_time;
    double read_energy;
    double SA_area;
    double SA_add_area;
    std::vector<Sa_op> sa_op;
};
extern Sa_op CSA_and;
extern Sa_op CSA_or;
extern Sa_op CSA_not;
extern Sa_op CSA_nor;
extern Sa_op CSA_xor;
extern Sa_op CSA_add;
extern std::vector<Sa_op> csa;
extern SA CSA;

extern Sa_op DSA_and;
extern Sa_op DSA_or;
extern Sa_op DSA_not;
extern Sa_op DSA_nor;
extern Sa_op DSA_xor;
extern Sa_op DSA_add;
extern std::vector<Sa_op> dsa;
extern SA DSA;
//VTEAM model
struct Ma_op{
    int op_type;
    double op_time;
    double op_energy;
};


struct Ma_Record{
    double read_energy;
    double write_energy;
    std::vector<Ma_op> magic_op;
};
extern Ma_op ma_seq;
extern Ma_op ma_nor;
//FELIX
extern Ma_op ma_and;
extern Ma_op ma_or;
extern Ma_op ma_not;
extern Ma_op ma_xor;
//一共写4.5次
extern Ma_op ma_add;
extern std::vector<Ma_op> magic_op;
extern Ma_Record magic_record;


struct Lut_Record{
    int op_type;//操作类型
    int data_bits;//操作数位数
    int lut4_num;
    int lut4_level;
    int lut6_num;
    int lut6_level;
};

extern double lut_sa;

std::vector<Lut_Record> lut_records();//获得lut的参数
extern std::vector<Lut_Record> lut_record;//全局


double lut_latency(int op_type);
double lut_energy(int op_type);
double sa_latency(int op_type,int sa_type);
double sa_energy(int op_type,int sa_type);
double ma_latency(int op_type);
double ma_energy(int op_type);

void init_Buffer_Reg();



//
// Created by shenlibo on 23-4-20.
//

#endif //SYNTHESIS_SA_PARAMETER_H
