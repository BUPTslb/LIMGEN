#include"logic_func.h"
//只有着一个函数可以建立阵列**定义建立逻辑，对于LUT,考虑加入操作
int build(int decide_array_type, int op_type, vector<lut_arr> &array_list1, \
                vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    int build;
    //写一个匿名函数
    auto f=[](const int &a){
        float b=a%32;
        int floor=a/32;
        if (b>0)
            floor+=1;
        int c=floor*32;
        return c;
    };
    switch (decide_array_type) {
        case 1: {
            lut_arr now1;
            //如果是lut4,数字为16
            //初始化id
            now1.array_id = array_list1.size();
            //初始化is_using
            now1.is_using = false;
            //初始化面积

            now1.row_num = 64;//代表是lut6

            now1.col_num = bit_num_operand;//这里只代表输出的位数，不代表真实的列数
            now1.lut_num = f(lut_num_op(op_type, 6));//lut-6数量
            cout<<"bulid lut size:"<<now1.lut_num<<endl;
            //初始化读写次数
            now1.read_number = 0;
            now1.write_number = 0;
            //初始化时间
            now1.start_time = 0;
            now1.over_time = 0;
            //初始化能量
            now1.energy = 0;
            //初始化lut-out
            now1.lut_out = -1;//将新建阵列的out设置为-1
            //设置功能。
            now1.op_type = -1;
            if (op_type != 0)
                now1.op_type=op_type;

            array_list1.push_back(now1);
            build = array_list1.size() - 1;

        }
            break;
        case 2: {
            sa_arr now2;
            //初始化id
            now2.array_id = array_list2.size();
            //初始化is_using
            now2.is_using = false;
            now2.add_use=false;
            if (op_type==11)
                now2.add_use= true;
            //初始化外围电路的类型
            int sa_type_ready[2]={1,2};
            int type_chosen=rand()%2;

            now2.sa_type=sa_type_ready[type_chosen];
            //初始化大小
            if (bit_num_operand<=32)
            {
                now2.row_num = 32;
                now2.col_num = 32;
            }
            else
            {
                now2.row_num = 64;
                now2.col_num = 64;
            }
//            now2.row_num = bit_num_operand;
//            now2.col_num = bit_num_operand;
            //初始化读写次数
            now2.read_number = 0;
            now2.write_number = 0;
            //初始化时间
            now2.start_time = 0;
            now2.over_time = 0;
            //初始化能量
            now2.energy = 0;
            //sa_direct -1是寄存器 >=0是节点
            now2.sa_direct = -2;//将直接输入设置为-2
            now2.sa_out = -1;//将新建阵列的输出设置为-1，>=0为节点
            array_list2.push_back(now2);
            build = array_list2.size() - 1;


        }
            break;
        case 3: {
            magic_arr now3;
            //初始化id
            now3.array_id = array_list3.size();
            //初始化is_using
            now3.is_using = false;
            now3.add_use= false;
            if (op_type==11)
                now3.add_use=true;
            //初始化大小
            if (bit_num_operand<=32)
            {
                now3.row_num = 32;
                now3.col_num = 32;
            }
            else
            {
                now3.row_num = 64;
                now3.col_num = 64;
            }
//            now3.row_num = 64;
//            now3.col_num = 64;
            //初始化读写次数
            now3.read_number = 0;
            now3.write_number = 0;
            //初始化时间
            now3.start_time = 0;
            now3.over_time = 0;
            //初始化能量
            now3.energy = 0;
            array_list3.push_back(now3);
            build = array_list3.size() - 1;

        }
            break;
        default:
            break;
    }
    return build;
}


//
// Created by Lenovo on 2023/6/28.
//
