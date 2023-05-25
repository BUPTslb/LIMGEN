#include "parameter.h"
#include "mainfunc.h"

//控制器,一个阵列一个
double controler_area = 503598;
double controler_power=2.8755;//控制器的功耗
double controler_latency=0.96;//控制器的延迟
//sa译码器 32/1 64/1 一个阵列一个 选择行 32面积：20802 64：36544
double sa_decoder_area=36544;//sa译码器的面积
double sa_decoder_power=0.02;//sa译码器的功耗
double sa_decoder_latency=0.15;//sa译码器的延迟
//sa驱动器 42译码 一列一个
double sa_driver_area=576;//SA驱动器的面积
double sa_driver_power=0.005;//sa驱动器的功耗
double sa_driver_latency=0.1;//sa驱动器的延迟
double sa_direct_area=72;//sa直接输
double sa_wb_area=192;//sa写回面积
double sa_arr_wb_area=480;//sa写回面积
//lut译码器 一个阵列一个 选择行 64面积：36544
double lut_decoder_area=36544;//lut译码器的面积
double lut_decoder_power=0.024;//lut译码器的功耗
double lut_decoder_latency=0.18;//lut译码器的延迟
//lut驱动器，42译码器，一列一个
double lut_driver_area=576;//lut驱动器的面积
double lut_driver_power=0.005;//lut驱动器的功耗
double lut_driver_latency=0.1;//lut驱动器的延迟
//lut的sa面积 固定部分 latch clk cap
double lut_sa=1056;
//magic译码器 3/8 一线一个
double magic_decoder_area=1248;//magic译码器的面积
double magic_decoder_power=0.01;//magic译码器的功耗
double magic_decoder_latency=0.1;//magic译码器的延迟
double magic_add=148;//magic add的面积,一行一个
//double mux_area=11455.8;
double mux16_area=11455.8;
double mux16_power=0.011;
double mux16_latency=0.17;
//64选1
double mux64_area=48683.8;
double mux64_power=0.044;
double mux64_latency=0.3;


//单位 ns pj F^2
RRAM rram = {1, 1, 0.02, 0.13, 6, 1e12};//RRAM参数 全局 ns pj
REG reg = {0.09, 0.09, 0.003, 0.003, 5214.8};//单个reg的索引 32bit
Register Reg_sum = {0, 0, 0, 0, 0};
BUFFER buffer = {0.09, 0.09, 0.002, 0.002, 96}; //这里的buffer只是锁存而已，原来能量是0.29，现在改成0.02，1bit的大小是96F^2
Buffer buffer_sum = {0, 0, 0, 0};

Sa_op CSA_and = {6, 0.03, 0.029};//ns pj
Sa_op CSA_or = {7, 0.04, 0.022};
Sa_op CSA_not = {8, 0.04, 0.016};
Sa_op CSA_nor = {9, 0.08, 0.038}; //or + not
Sa_op CSA_xor = {10, 0.32, 0.04};
Sa_op CSA_add = {11, 0.38, 0.069};//add 1bit
std::vector<Sa_op> csa = {CSA_and, CSA_or, CSA_not, CSA_nor, CSA_xor, CSA_add};
SA CSA = {0.04, 0.016, 462, 240, csa};

Sa_op DSA_and = {6, 0.078, 0.015};//ns pj
Sa_op DSA_or = {7, 0.09, 0.01};
Sa_op DSA_not = {8, 0.09, 0.01};
Sa_op DSA_nor = {9, 0.18, 0.02}; //or + not
Sa_op DSA_xor = {10, 0.10, 0.025};
Sa_op DSA_add = {11, 0.12, 0.026};
std::vector<Sa_op> dsa = {DSA_and, DSA_or, DSA_not, DSA_nor, DSA_xor, DSA_add};
SA DSA = {0, 0.010, 720, 240, dsa}; //读时间 读能量 面积 操作

//TODO:verilog-A仿真一下，magic能耗过高，时间太慢，参数都需要更新
Ma_op ma_seq = {0, rram.write_time, rram.write_energy};//用set reset平均（0.219+0.034）/2
//Ma_op ma_nor = {9, 2.3, 0.18};//1.3ns 50fJ+0.13pJ
Ma_op ma_nor = {9, 2.3, 0.27};//1.3ns 50fJ+0.13pJ
//FELIX
//Ma_op ma_and = {6, 3.6, 0.203};//data from: FELIX AND3
Ma_op ma_and = {6, 3.9, 0.42};//data from: FELIX AND3
//Ma_op ma_or = {7, 2.3, 0.139};
Ma_op ma_or = {7, 2.3, 0.053};
//Ma_op ma_not = {8, 2.3, 0.154};
Ma_op ma_not = {8, 2.3, 0.154};
//Ma_op ma_xor = {10, 3.6, 0.16};//nand 0.049+0.009
Ma_op ma_xor = {10, 3.9, 0.3};//nand 0.049+0.009
//一共写4.5次
//Ma_op ma_add = {11, 7.2, 0.6};//1bit N+4 4是两个xor=7.2ns N每一个都是7（直接全部设置成7.2）
Ma_op ma_add = {11, 4.9, 1.0};// N*4.9+7.8  能量：（0.6+0.4）* N
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
    one_latency+=mux64_latency;//加上mux64的延迟
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
    //不需要decoder
    if (op_type == 0) return reg.reg_write_energy;//写到了reg中
    double do_time = lut_latency(op_type);
    double static_energy = do_time * 3.264;
    double lut_energy;
//    int lut_num= lut_num_op(op_type,6);
    int lut_level = lut_level_op(op_type, 6);
    int lut_num = lut_num_op(op_type, 6);
    double dynamic_energy = lut_num * 0.5 * 4.2;//动态功耗*翻转率*次数
    lut_energy = (static_energy + dynamic_energy) / 1000;//输出pJ
    lut_energy += lut_num*mux64_latency*mux64_power;
    return lut_energy;
}

double sa_latency(int op_type, int sa_type) {
    double sa_latency = 0;
    //译码延迟
    sa_latency += controler_latency;
//    sa_latency += sa_decoder_latency;
//    sa_latency += sa_driver_latency;
    if (op_type == 0) return sa_latency+rram.write_time;//write_back
    if (sa_type == 1) //CSA
    {
        //在CSA的参数表中找到延迟数据，并返回
        for (auto i: CSA.sa_op) {
            if (i.op_type == op_type) {
                if (op_type == 11)//add
                    sa_latency += i.op_latency * bit_num_operand;
                else
                    sa_latency += i.op_latency;
            }
        }
    } else //DSA
    {
        for (auto i: DSA.sa_op) {
            if (i.op_type == op_type) {
                if (op_type == 11)//add
                    sa_latency += i.op_latency * bit_num_operand;
                else
                    sa_latency += i.op_latency;
            }
        }

    }
    return sa_latency;

}

double sa_energy(int op_type, int sa_type) {
    double sa_energy = 0;
    //先将译码器的开销加上
    sa_energy += controler_latency*controler_power;
    sa_energy += sa_decoder_latency*sa_decoder_power;
    sa_energy += sa_driver_latency*sa_driver_power;
    if (op_type == 0)
        sa_energy += bit_num_operand * rram.write_energy;
    if (sa_type == 1) //CSA
    {
        //在CSA的参数表中找到延迟数据，并返回
        for (auto i: CSA.sa_op) {
            if (i.op_type == op_type) {
                sa_energy += bit_num_operand * i.op_energy;
            }
        }
    } else //DSA
    {
        for (auto i: DSA.sa_op) {
            if (i.op_type == op_type) {
                sa_energy += bit_num_operand * i.op_energy;
            }
        }
    }
    return sa_energy;
}


double ma_latency(int op_type) {
    double ma_latency = 0;
    ma_latency+=controler_latency;
    ma_latency+=2*magic_decoder_latency;//加上译码器的时间
    for (auto i: magic_op) {
        if (i.op_type == op_type) {
            if (op_type == 11) //加法
                ma_latency += (bit_num_operand + i.op_time * bit_num_operand + 7.8);//N*1 + N+4
            else
                ma_latency += i.op_time;
        }
    }
    return ma_latency;
}

double ma_energy(int op_type) {
    double ma_energy = 0;
    ma_energy+=controler_latency*controler_power;
    ma_energy+=2*magic_decoder_latency*magic_decoder_power;
    if (op_type==0)  //写能量
    {
        ma_energy+=bit_num_operand*rram.write_energy;
    }
    for (auto i: magic_op) {
        if (i.op_type == op_type)
            ma_energy += bit_num_operand * i.op_energy;
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
    for (auto i: array_list3) {
        latency_all = max(latency_all, i.over_time);
    }
    return latency_all;

}

//阵列面积+控制器面积+译码器面积+外围电路面积+buffer面积+寄存器面积
//测量面积,假设以寄存器写数量/2来计算面积
double area_all(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    double area_all = 0;
    area_all+=reg.reg_area * Reg_sum.write_num_sum /2;
    //阵列的面积
    for (auto i: array_list1) {
        double array1_area=0;
        //注意，这里lut_num才代表真实的列数，col_num只代表输出的列数
        array1_area+=controler_area;//一个阵列一个控制器
        array1_area+=lut_decoder_area;//一个阵列一个译码器
        array1_area+=i.lut_num * lut_driver_area;//一列一个驱动
        array1_area += i.row_num * i.lut_num * rram.t1r1_area; //RRAM的面积
        if (i.row_num == 16)
            array1_area += mux16_area * i.lut_num;
        else
            array1_area += mux64_area * i.lut_num;
//        cout<<"lut_area:"<<array1_area<<endl;
        area_all+=array1_area;
    }
    for (auto i: array_list2) {
        double array2_area=0;
        array2_area += i.col_num * i.row_num * rram.t1r1_area;
        array2_area+=controler_area;
        array2_area+=sa_decoder_area;
        array2_area+=i.col_num*sa_driver_area;
        array2_area+=sa_direct_area*i.col_num;//sa直接输入
        array2_area+=sa_wb_area*i.col_num;//写回
        if (i.sa_type == 1) {
            //加上csa的面积
            array2_area += CSA.SA_area * i.col_num;
            if (i.add_use== true)
                array2_area+=CSA.SA_add_area*i.col_num;
        } else {
            //加上dsa的面积
            array2_area += DSA.SA_area * i.col_num;
            if (i.add_use== true)
                array2_area += DSA.SA_add_area * i.col_num;
        }
//        cout<<"sa_area: "<<array2_area<<endl;
        area_all+=array2_area;
    }


    for (auto i: array_list3) {
        double array3_area=0;
        array3_area += controler_area;
        array3_area += i.col_num * i.row_num * rram.t1r1_area;
        array3_area += i.row_num * magic_decoder_area + i.col_num * magic_decoder_area;
        if (i.add_use)
            array3_area+=i.row_num*magic_add;
//        cout<<"magic_area: "<<array3_area<<endl;
        area_all+=array3_area;
    }
    //寄存器的面积都一样
    return area_all;
}

double area_all_lut(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    double area_all = 0;
    area_all+=reg.reg_area * Reg_sum.write_num_sum /2;

    //阵列的面积
    for (auto i: array_list1) {
        //注意，这里lut_num才代表真实的列数，col_num只代表输出的列数
        area_all+=controler_area;//一个阵列一个控制器
        area_all+=lut_decoder_area;//一个阵列一个译码器
        area_all+=i.lut_num * lut_driver_area;//一列一个驱动
        area_all += i.row_num * i.lut_num * rram.t1r1_area; //RRAM的面积
        if (i.row_num == 16)
            area_all += mux16_area * i.lut_num;
        else
            area_all += mux64_area * i.lut_num;
    }

    for (auto i: array_list3) {
        area_all += controler_area;
        area_all += i.col_num * i.row_num * rram.t1r1_area;
        area_all += lut_decoder_area;
        area_all += i.row_num * lut_driver_area;//一列一个驱动
    }

    return area_all;
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
              std::vector<double> latency_energy_area,
              std::vector<int> &array_num_latency,
              std::vector<int> &array_num_energy,
              std::vector<int> &array_num_area,
              vector<lut_arr> &array_list1,
              vector<sa_arr> &array_list2,
              vector<magic_arr> &array_list3) {
    //优化延迟
    if (array_num_latency.empty())
        array_num_latency.resize(3);
    if (array_num_energy.empty())
        array_num_energy.resize(3);
    if (array_num_area.empty())
        array_num_area.resize(3);
    if (latency_energy_area[0] < best_latency[0]) {
        best_latency = latency_energy_area;
        array_num_latency[0] = array_list1.size();
        array_num_latency[1] = array_list2.size();
        array_num_latency[2] = array_list3.size();
    }
    if (latency_energy_area[1] < best_energy[1]) {
        best_energy = latency_energy_area;
        array_num_energy[0] = array_list1.size();
        array_num_energy[1] = array_list2.size();
        array_num_energy[2] = array_list3.size();
    }

    if (latency_energy_area[2] < best_area[2]) {
        best_area = latency_energy_area;
        array_num_area[0] = array_list1.size();
        array_num_area[1] = array_list2.size();
        array_num_area[2] = array_list3.size();
    }

}


//
// Created by shenlibo on 23-4-20.
//
