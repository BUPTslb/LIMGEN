#include "mainfunc.h"
struct RRAM{
    double read_time;
    double write_time;
    double read_energy;
    double write_energy;
    double write_max;
};
RRAM rram={10.0,10.0,0.02,0.10,1e12};//RRAM参数 全局 ns pj

struct REG{
    double reg_read_time;//ns
    double reg_write_time;
    double reg_read_energy;//pj
    double reg_write_energy;
};
REG reg={1,1,1,1};//TODO:

struct BUFFER{
    double buffer_read_time;//ns
    double buffer_write_time;
    double buffer_read_energy;//pj
    double buffer_write_energy;
};
BUFFER buffer={1,1,1,1};//TODO:

struct Sa_op {
    int op_type;
    double op_latency;
    double op_energy;
};

struct SA {
    double read_time;
    double read_energy;
    vector<Sa_op> sa_op;
};

Sa_op CSA_and = {6, 0.03, 28.63};//ns pj
Sa_op CSA_or = {7, 0.04, 22.02};
Sa_op CSA_not = {8, 0.04, 16.79};
Sa_op CSA_nor = {9, 0.08, 38.81}; //or + not
Sa_op CSA_xor = {10, 0.17, 40.37};
Sa_op CSA_add = {11, 0.21, 69.0};//add 1bit
vector<Sa_op> csa = {CSA_and,CSA_or,CSA_not,CSA_nor,CSA_xor,CSA_add};
SA CSA={0.04,16.79,csa};

Sa_op DSA_and = {6, 0.07, 14.71};//ns pj
Sa_op DSA_or = {7, 0.09, 10.08};
Sa_op DSA_not = {8, 0.09, 10.08};
Sa_op DSA_nor = {9, 0.18, 20.16}; //or + not
Sa_op DSA_xor = {10, 0.10, 25.21};
Sa_op DSA_add = {11, 0.12, 25.84};
vector<Sa_op> dsa = {DSA_and,DSA_or,DSA_not,DSA_nor,DSA_xor,DSA_add};
SA DSA={0.09,10.01,dsa};

struct Ma_op{
    int op_type;
    double op_time;
    double op_energy;
};

Ma_op ma_seq={0,rram.write_time,rram.write_energy};
Ma_op ma_and={6,0,0};
Ma_op ma_or={7,0,0};
Ma_op ma_not={8,0,0};
Ma_op ma_nor={9,0,0};
Ma_op ma_xor={10,0,0};
Ma_op ma_add={11,0,0};

struct Ma_Record{
    double read_energy;
    double write_energy;
    vector<Ma_op> magic_op;
};
vector<Ma_op> magic_op={ma_seq,ma_and,ma_or,ma_not,ma_nor,ma_xor,ma_add};
Ma_Record magic_record={rram.read_energy,rram.write_energy,magic_op};

double lut_latency(int op_type);
double lut_energy(int op_type);
double sa_latency(int op_type,int sa_type);
double sa_energy(int op_type,int sa_type);
double ma_latency(int op_type);
double ma_energy(int op_type);


//
// Created by shenlibo on 23-4-20.
//

#ifndef SYNTHESIS_SA_PARAMETER_H
#define SYNTHESIS_SA_PARAMETER_H

#endif //SYNTHESIS_SA_PARAMETER_H
