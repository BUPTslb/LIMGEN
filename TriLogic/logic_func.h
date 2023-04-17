//#pragma once //只被编译一次
#include<string>
#include <vector>
#include "mainfunc.h"
#include <set>

using namespace std;

/*中层：cache一致性*/
//cache一致性函数,输入执行阵列类型，ID,当前节点存储表，输出是否命中
//bool cache_like(int array_type, int array_id, vector<vector<int>> &wb_pos);

//节点执行表更新策略,返回值表示是否需要更新
//bool update_wb_pos(bool cache_like, int pos_input, int array_type, int pos_array);

//流水线
//时间特性,更新阵列的开始时间，结束时间
double time_now(int op_type, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                vector<magic_arr> &array_list3, Node *node_now);
//update the time of do_array
void time_update(int op_type,int array_type, int array_id, double time_now,Node *node_now,
                 vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3, int bit_num=32);
/*中层：阵列行为*/
void find_input(int &array_type, int &array_id, int op_type,
                Node *node_depend = nullptr, int cycle = 0);//寻找操作数来源,node_depend指向nodes中的节点

//决定执行阵列的类型
int decide_array_type(int op_type, int design_target);//由算子支持和设计目标共同决定

//决定执行阵列的ID,输入操作数个数1false2true,输入参数带默认值，-1表示无
int decide_array_id(int op_type, int bit_num_operand, Node* node_now,vector<Node> &nodes, int decide_array_type,
                    vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                    vector<magic_arr> &array_list3, int input1_type = 0, int input1_id = 0, int input2_type = 0,
                    int input2_id = 0);

//优先队列，堆顶元素的over_time是最小的
struct CompareArray {
    bool operator()(const Array &a1, const Array &a2) {
        return a1.over_time > a2.over_time;
    }
};

//当前不再使用的能用的阵列表
vector<int> find_no_using(int op_type, vector<Node> &nodes, int decide_array_type, vector<lut_arr> &array_list1,
                          vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//当前在使用的能用的阵列表
vector<int> waiting_array_list(int op_type, vector<Node> &nodes, int decide_array_type, vector<lut_arr> &array_list1,
                               vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//等待、建立逻辑，等待过程如何反映在代码中？？
int build(int decide_array_type, int op_type, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
          vector<magic_arr> &array_list3);

//计算阵列剩余
int cap_array_lost(int decide_array_type, int decide_array_id, vector<Node> &nodes, vector<lut_arr> &array_list1,
                   vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//可覆盖容量
int cap_array_cover(int decide_array_type, int decide_array_id, vector<Node> &nodes, vector<lut_arr> &array_list1,
                    vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//需要单独写一个函数，得出计算每一个op需要的剩余容量
int op_row_need(int op_type, int decide_array_type, int bit_num_operand);

//数据读函数,输入：各阵列表，执行的运算节点
//目的：找到输入数据依赖的“阵列“，完成数据搬移：需要增加读就读++，需要移动写就写++
// 操作：修改阵列读写和时间参数
void data_read(int input_type, int input_id, int decide_array_type, int decide_array_id, int *Register,
               vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//判断节点的写回表中有当前阵列：目的是判断阵列存储中是否有数据
bool is_in_wb(int array_type, int array_id, Node *node_now);

//写入逻辑
void
input_logic(int input1_type, int input1_id, int input2_type, int input2_id, int decide_array_type, int decide_array_id,
            Node *now, int *Register, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
            vector<magic_arr> &array_list3);

//输出逻辑
void output_logic(int decide_array_type, int decide_array_id, int op_type, Node *now,
                  vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//lut执行逻辑
void op_lut(int op_type, int decide_array_id, Node *now, vector<lut_arr> &array_list1);

//sa执行逻辑
void op_sa(int op_type, int decide_array_id, Node *now, vector<sa_arr> &array_list2);

//sa执行逻辑
void op_magic(int op_type, int decide_array_id, Node *now, vector<magic_arr> &array_list3);

//出度更新
void out_degree(Node *now);

//出度为0的节点对应阵列的擦除,更新节点的写回表和阵列的存储节点
//void erase_array(Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//TODO:写覆盖
void write_cover();


//LUT计算函数：
void com_lut(int type_operation, int bit_num_operand, int op_num, vector<lut_arr> &array_list1,
             vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//sa计算函数
void com_sa(int type_operation, int bit_num_operand, int op_num, vector<lut_arr> &array_list1,
            vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);

//magic计算函数
void com_magic(int type_operation, int bit_num_operand, int op_num, vector<lut_arr> &array_list1,
               vector<sa_arr> &array_list2, vector<magic_arr> &array_list3);


//阵列尺寸设定函数，一定是方形的
unsigned int arr_size(int logic_type, unsigned int bit_num_operand);


//查找表
//AES中的乘法
