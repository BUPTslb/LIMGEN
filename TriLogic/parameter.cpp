#include "parameter.h"
#include "mainfunc.h"

//控制器的面积，行列各有一个
extern double controler_magic = 1344;
extern double controler_sa = 672;
extern double controler_lut = 672;

//单位 ns pj F^2
RRAM rram = {1, 1, 0.02, 0.13, 6, 1e12};//RRAM参数 全局 ns pj
REG reg = {1, 1, 0.02, 0.02, 1524};//单个reg的索引 只有面积参数是1bit的
Register Reg_sum = {0, 0, 0, 0, 0};
BUFFER buffer = {1, 1, 0.02, 0.02, 96}; //这里的buffer只是锁存而已，原来能量是0.29，现在改成0.02，1bit的大小是96F^2
Buffer buffer_sum = {0, 0, 0, 0};

Sa_op CSA_and = {6, 0.03, 0.029};//ns pj
Sa_op CSA_or = {7, 0.04, 0.022};
Sa_op CSA_not = {8, 0.04, 0.016};
Sa_op CSA_nor = {9, 0.08, 0.038}; //or + not
Sa_op CSA_xor = {10, 0.32, 0.04};
Sa_op CSA_add = {11, 0.38, 0.069};//add 1bit
std::vector<Sa_op> csa = {CSA_and, CSA_or, CSA_not, CSA_nor, CSA_xor, CSA_add};
SA CSA = {0.04, 0.016, 534, 240, csa};

Sa_op DSA_and = {6, 0.078, 0.015};//ns pj
Sa_op DSA_or = {7, 0.09, 0.01};
Sa_op DSA_not = {8, 0.09, 0.01};
Sa_op DSA_nor = {9, 0.18, 0.02}; //or + not
Sa_op DSA_xor = {10, 0.10, 0.025};
Sa_op DSA_add = {11, 0.12, 0.026};
std::vector<Sa_op> dsa = {DSA_and, DSA_or, DSA_not, DSA_nor, DSA_xor, DSA_add};
SA DSA = {0.09, 0.010, 792, 240, dsa}; //读时间 读能量 面积 操作

//TODO:verilog-A仿真一下，magic能耗过高，时间太慢，参数都需要更新
Ma_op ma_seq = {0, rram.write_time, rram.write_energy};//用set reset平均（0.219+0.034）/2
Ma_op ma_nor = {9, 2.3, 0.18};//1.3ns 50fJ+0.13pJ
//FELIX
Ma_op ma_and = {6, 3.6, 0.203};//data from: FELIX AND3
Ma_op ma_or = {7, 2.3, 0.139};
Ma_op ma_not = {8, 2.3, 0.154};
Ma_op ma_xor = {10, 3.6, 0.16};//nand 0.049+0.009
//一共写4.5次
Ma_op ma_add = {11, 7.2, 0.6};//1bit N+4 4是两个xor=7.2ns N每一个都是7（直接全部设置成7.2）
std::vector<Ma_op> magic_op = {ma_seq, ma_and, ma_or, ma_not, ma_nor, ma_xor, ma_add};
Ma_Record magic_record = {rram.read_energy, rram.write_energy, magic_op};


void init_Buffer_Reg() {
    Reg_sum.write_energy_sum = 0;
    Reg_sum.read_energy_sum = 0;
    Reg_sum.read_num_sum = 0;
    Reg_sum.write_num_sum = 0;

    buffer_sum.buffer_write_energy = 0;
    buffer_sum.buffer_read_energy = 0;
    buffer_sum.buffer_read_sum = 0;
    buffer_sum.buffer_write_sum = 0;

}

double lut_latency(int op_type) {
    if (op_type == 0) return reg.reg_write_time;//写到了reg中
    double one_latency = 0.104;//一个lut的传输延时，上升沿+下降沿/2（0.048+0.16）/2 单位：ns
    double lut_latency;
//    int lut_num= lut_num_op(op_type,6);
    int lut_level = lut_level_op(op_type, 6);
    lut_latency = lut_level * one_latency;
    return lut_latency;

}

double lut_energy(int op_type) {
    //静态功耗：1.121uW
    //反转能耗：4.1852fJ
    //lut mux:功耗 2.143u
    if (op_type == 0) return bit_num_operand * reg.reg_write_energy;//写到了reg中
    double do_time = lut_latency(op_type);
    double static_energy = do_time * 3.264;
    double lut_energy;
//    int lut_num= lut_num_op(op_type,6);
    int lut_level = lut_level_op(op_type, 6);
    int lut_num = lut_num_op(op_type, 6);
    double dynamic_energy = lut_num * 0.5 * 4.2;//动态功耗*翻转率*次数
    lut_energy = (static_energy + dynamic_energy) / 1000;//输出pJ

    return lut_energy;
}

double sa_latency(int op_type, int sa_type) {
    double sa_latency = 0;
    if (op_type == 0) return rram.write_time;//write_back
    if (sa_type == 1) //CSA
    {
        //在CSA的参数表中找到延迟数据，并返回
        for (auto i: CSA.sa_op) {
            if (i.op_type == op_type) {
                if (op_type == 11)//add
                    sa_latency = i.op_latency * bit_num_operand;
                else
                    sa_latency = i.op_latency;
            }
        }
    } else //DSA
    {
        for (auto i: DSA.sa_op) {
            if (i.op_type == op_type) {
                if (op_type == 11)//add
                    sa_latency = i.op_latency * bit_num_operand;
                else
                    sa_latency = i.op_latency;
            }
        }

    }
    return sa_latency;

}

double sa_energy(int op_type, int sa_type) {
    double sa_energy = 0;
    if (op_type == 0) return bit_num_operand * rram.write_energy;
    if (sa_type == 1) //CSA
    {
        //在CSA的参数表中找到延迟数据，并返回
        for (auto i: CSA.sa_op) {
            if (i.op_type == op_type) {
                sa_energy = bit_num_operand * i.op_energy;
            }
        }
    } else //DSA
    {
        for (auto i: DSA.sa_op) {
            if (i.op_type == op_type) {
                sa_energy = bit_num_operand * i.op_energy;
            }
        }
    }
    return sa_energy;
}


double ma_latency(int op_type) {
    double ma_latency = 0;
    for (auto i: magic_op) {
        if (i.op_type == op_type) {
            if (op_type == 11) //加法
                ma_latency = bit_num_operand + i.op_time * (bit_num_operand + 4);//N*1 + N+4
            else
                ma_latency = i.op_time;
        }
    }
    return ma_latency;
}

double ma_energy(int op_type) {
    double ma_energy = 0;
    for (auto i: magic_op) {
        if (i.op_type == op_type)
            ma_energy = bit_num_operand * i.op_energy;
    }
    return ma_energy;

}

//计算架构性能参数
//延迟,所有时间中的最大值
double latency_all(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    double latency_all = 0;
    for (auto i: nodes2) {
        latency_all = max(latency_all, i.end_time);
    }
    for (auto i: array_list1) {
        latency_all = max(latency_all, i.over_time);
    }
    for (auto i: array_list2) {
        latency_all = max(latency_all, i.over_time);
    }
    for (auto i: array_list2) {
        latency_all = max(latency_all, i.over_time);
    }
    return latency_all;

}

//面积，阵列+buffer+Reg
//TODO：测量面积
double area_all(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    double area_all = 0;
    //阵列的面积
    for (auto i: array_list1) {
        //注意，这里lut_num才代表真实的列数，col_num只代表输出的列数
        area_all += i.row_num * i.lut_num * rram.t1r1_area;
        if (i.row_num == 16)
            area_all += lut4_out_area * i.lut_num;
        else
            area_all += lut6_out_area * i.lut_num;

    }
    for (auto i: array_list2) {
        if (i.sa_type == 1) {
            //加上csa的面积
            area_all += i.col_num * i.row_num * rram.t1r1_area;
            area_all += CSA.SA_area * i.col_num;
            area_all += CSA.SA_add_area * i.col_num;
        } else {
            //加上dsa的面积
            area_all += i.col_num * i.row_num * rram.t1r1_area;
            area_all += DSA.SA_area * i.col_num;
            area_all += DSA.SA_add_area * i.col_num;
        }
    }

    for (auto i: array_list3) {
        area_all += i.col_num * i.row_num * rram.t1r1_area;
        area_all += i.row_num * controler_magic + i.col_num * controler_magic;
        area_all += i.row_num * 196;
    }
    //寄存器的面积都一样
    //其余的面积都可以认定为一样
    //讲解一下为什么其他面积被忽略了
}

double area_all_lut(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    double area_all = 0;
    //阵列的面积
    for (auto i: array_list1) {
        //注意，这里lut_num才代表真实的列数，col_num只代表输出的列数
        area_all += i.row_num * i.lut_num * rram.t1r1_area;
        if (i.row_num == 16)
            area_all += lut4_out_area * i.lut_num;
        else
            area_all += lut6_out_area * i.lut_num;

    }

    for (auto i: array_list3) {
        area_all += i.col_num * i.row_num * rram.t1r1_area;
        area_all += i.row_num * controler_lut + i.col_num * controler_lut;
    }
    //寄存器的面积都一样
    //其余的面积都可以认定为一样
    //讲解一下为什么其他面积被忽略了
}


//能耗,阵列+buffer+Reg
double energy_all(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    double energy_all = 0;
    for (auto i: array_list1) {
        energy_all += i.energy;
    }
    for (auto i: array_list2) {
        energy_all += i.energy;
    }
    for (auto i: array_list3) {
        energy_all += i.energy;
    }
    energy_all += Reg_sum.read_energy_sum;
    energy_all += Reg_sum.write_energy_sum;
    energy_all += buffer_sum.buffer_read_energy;
    energy_all += buffer_sum.buffer_write_energy;
    return energy_all;
}

void get_best(std::vector<double> &best_latency,
              std::vector<double> &best_energy,
              std::vector<double> &best_area,
              std::vector<double> latency_energy,
              std::vector<int> &array_num_latency,
              std::vector<int> &array_num_energy,
              vector<lut_arr> &array_list1,
              vector<sa_arr> &array_list2,
              vector<magic_arr> &array_list3) {
    //将所有结果都输出，然后在统一绘制图形



    //优化延迟
    if (latency_energy[0] < best_latency[0]) {
        best_latency = latency_energy;
        if (array_num_latency.empty())
            array_num_latency.resize(3);
        array_num_latency[0] = array_list1.size();
        array_num_latency[1] = array_list2.size();
        array_num_latency[2] = array_list3.size();

    }
    cout << "优化延迟" << endl;
    if (latency_energy[1] < best_energy[1]) {
        best_energy = latency_energy;
        if (array_num_energy.empty())
            array_num_energy.resize(3);
        array_num_energy[0] = array_list1.size();
        array_num_energy[1] = array_list2.size();
        array_num_energy[2] = array_list3.size();
    }
    cout << "优化能耗" << endl;


}


//
// Created by shenlibo on 23-4-20.
//
