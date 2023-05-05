#ifndef SYNTHESIS_LOGIC_FUNC_H
#define SYNTHESIS_LOGIC_FUNC_H

//#pragma once //只被编译一次
#include<string>
//#include <vector>
#include "mainfunc.h"
#include <set>
#include <cstdlib>
#include <ctime>

using namespace std;

/*中层：cache一致性*/
//cache一致性函数,输入执行阵列类型，ID,当前节点存储表，输出是否命中
//bool cache_like(int array_type, int array_id, vector<vector<int>> &wb_pos);

//节点执行表更新策略,返回值表示是否需要更新
//bool update_wb_pos(bool cache_like, int pos_input, int array_type, int pos_array);

//流水线
//时间特性,更新阵列的开始时间，结束时间
double
time_now(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3, Node *node_now,
         int decide_array_type = -2, int decide_array_id = -2);

double time_only_lut(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3, Node *node_now,
                     int decide_array_type = -2, int decide_array_id = -2);
double time_only_sa(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3, Node *node_now,
                     int decide_array_type = -2, int decide_array_id = -2);
double time_only_magic(vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3, Node *node_now,
                     int decide_array_type = -2, int decide_array_id = -2);

//update the time of do_array
void time_update(int op_type, int do_type, int array_id, double time_now, Node *node_now,
                 vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//read time
void read_time_update(int array_type, int array_id, double time_now, Node *now, vector<lut_arr> &array_list1,
                      vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//update the energy of array
void read_energy_update(int array_type, int array_id, Node *node_now,
                        vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//数据读函数,输入：各阵列表，执行的运算节点
//目的：找到输入数据依赖的“阵列“，完成数据搬移：需要增加读就读++，需要移动写就写++
// 操作：修改阵列读写和时间参数
void data_read(int No_depend, int &input_type, int &input_id, int decide_array_type, int decide_array_id, Node *now,
               vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

void data_read_lut(int No_depend, int &input_type, int &input_id, int decide_array_type, int decide_array_id, Node *now,
               vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);
void data_read_sa(int No_depend, int &input_type, int &input_id, int decide_array_type, int decide_array_id, Node *now,
               vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);
void data_read_magic(int No_depend, int &input_type, int &input_id, int decide_array_type, int decide_array_id, Node *now,
               vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

void energy_update(int op_type, int do_type, int array_id, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                   vector<magic_arr> &array_list3);

/*中层：阵列行为*/
void find_input(int &array_type, int &array_id, Node *node_depend = nullptr);//寻找操作数来源,node_depend指向nodes中的节点

//决定执行阵列的类型
int decide_array_type(int op_type, int design_target);//由算子支持和设计目标共同决定

//决定执行阵列的ID,输入操作数个数1false2true,输入参数带默认值，-1表示无
int decide_array_id(int op_type, Node *node_now, int decide_array_type,
                    vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                    vector<magic_arr> &array_list3, int input1_type = 0, int input1_id = 0, int input2_type = 0,
                    int input2_id = 0);

//优先队列，堆顶元素的over_time是最小的
struct CompareArray {
    bool operator()(const Array &a1, const Array &a2) {
        return a1.over_time < a2.over_time;
    }
};

//当前不再使用的能用的阵列表
vector<int> find_no_using(int op_type, int decide_array_type, vector<lut_arr> &array_list1,
                          vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//当前在使用的能用的阵列表
vector<int> waiting_array_list(int op_type, int decide_array_type, vector<lut_arr> &array_list1,
                               vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//等待、建立逻辑，等待过程如何反映在代码中？？
int build(int decide_array_type, int op_type,
          vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//计算阵列剩余
int cap_array_lost(int decide_array_type, int decide_array_id, vector<lut_arr> &array_list1,
                   vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//可覆盖容量
int cap_array_cover(int decide_array_type, int decide_array_id, vector<lut_arr> &array_list1,
                    vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//sa、magic
int capped_now(int decide_array_type, int decide_array_id, vector<lut_arr> &array_list1,
               vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//需要单独写一个函数，得出计算每一个op需要的剩余容量
int op_row_need(int op_type, int decide_array_type);


//判断节点的写回表中有当前阵列：目的是判断阵列存储中是否有数据
bool is_in_wb(int array_type, int array_id, Node *node_now);

//节点的写回表为空
bool wb_empty(Node *now);


//写入逻辑
void
input_logic(int operand_num, int input1_type, int input1_id, int input2_type, int input2_id, int decide_array_type,
            int decide_array_id,
            Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

void
input_logic_lut(int operand_num, int input1_type, int input1_id, int input2_type, int input2_id, int decide_array_type,
            int decide_array_id,
            Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

void
input_logic_sa(int operand_num, int input1_type, int input1_id, int input2_type, int input2_id, int decide_array_type,
            int decide_array_id,
            Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

void
input_logic_magic(int operand_num, int input1_type, int input1_id, int input2_type, int input2_id, int decide_array_type,
            int decide_array_id,
            Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//输出逻辑
void output_logic(int decide_array_type, int decide_array_id, int op_type, Node *now,
                  vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);
void output_logic_lut(int decide_array_type, int decide_array_id, int op_type, Node *now,
                  vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);
void output_logic_sa(int decide_array_type, int decide_array_id, int op_type, Node *now,
                  vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);
void output_logic_magic(int decide_array_type, int decide_array_id, int op_type, Node *now,
                  vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//lut执行逻辑
void op_lut(int op_type, int decide_array_id, Node *now, double time_now, vector<lut_arr> &array_list1,
            vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);
void op_lut_only(int op_type, int decide_array_id, Node *now, double time_now, vector<lut_arr> &array_list1,
                 vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//sa执行逻辑
void op_sa(int op_type, int decide_array_id, Node *now, double time_now, vector<lut_arr> &array_list1,
           vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);
void op_sa_only(int op_type, int decide_array_id, Node *now, double time_now, vector<lut_arr> &array_list1,
                vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);
//magic执行逻辑
//TODO:控制节点的OP类型，其结果也要存储
void op_magic(int op_type, int decide_array_id, Node *now, double time_now, vector<lut_arr> &array_list1,
              vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);
void op_magic_only(int op_type, int decide_array_id, Node *now, double time_now, vector<lut_arr> &array_list1,
                   vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);
//出度更新
void out_degree(Node *now);

//出度为0的节点对应阵列的擦除,更新节点的写回表和阵列的存储节点
//void erase_array(Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//num of node stored
int num_node_position(Node *now);

//TODO:写覆盖,应该有目标阵列才对
//row_need,代表需要写覆盖的行数
void write_cover(int op_type, Node *now, int pos_array, int pos_id, int row_need,
                 vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//将节点写回表中的该阵列、buffer、reg擦除掉
void wb_erase(Node *now, int erase_type, int erase_id);

//from_type表示写回的类型 0:Reg 1:lut-out 2:sa-out 3:magic存储 4:sa-buffer 5：lut-buffer 6:sa存储
void write_back(int from_type, int from_id, Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                vector<magic_arr> &array_list3, int back_type = -2, int back_id = -2);
void write_back_lut(int from_type, int from_id, Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                    vector<magic_arr> &array_list3, int back_type = -2, int back_id = -2);

void write_back_sa(int from_type, int from_id, Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                   vector<magic_arr> &array_list3, int back_type = -2, int back_id = -2);

void write_back_magic(int from_type, int from_id, Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                      vector<magic_arr> &array_list3, int back_type = -2, int back_id = -2);

//擦除阵列中存储的节点
void array_erase_node(int erase_node, int array_type, int array_id,
                      vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//更新阵列的存储节点，写入
void array_add_node(int array_type, int array_id, Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                    vector<magic_arr> &array_list3);

//阵列尺寸设定函数，一定是方形的
unsigned int arr_size();


//查找表
//AES中的乘法

#endif