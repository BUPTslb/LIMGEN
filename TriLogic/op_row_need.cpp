#include"logic_func.h"
//需要单独写一个函数，得出计算每一个op需要的剩余容量,应该还需要比特数
//假设两个操作数都在阵列中
int op_row_need(int op_type, int decide_array_type) {
    int row_need=0;
    switch (decide_array_type) {
        case 1://LUT输出需要的LUT数量
        {
            int lut_type = 6;//4--6
//            int lut_type=4;
            int lut_num = lut_num_op(op_type, lut_type);
            return lut_num;   //LUT应该和阵列级联以及位数有关，要具体分析，待定
        }
        case 2://SA
        {
            if (op_type == 11)//add
                return 3; //ab a+b a^b
            if (op_type == 0)//写回，write_back
                return 1;
            else
                return 0;
        }
        case 3://MAGIC，这里把需要写回的结果的位置也算上了，避免执行完了没地方写回
        {
            if (op_type == 11)
                return 3;//C L S
            else
                return 1; //S
        }
        default:
            break;

    }
    return row_need;

}
//
// Created by Lenovo on 2023/6/28.
//
