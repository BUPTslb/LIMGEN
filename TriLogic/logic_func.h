//#pragma once //只被编译一次
#include<string>
#include <vector>
#include "mainfunc.h"
using namespace std;

//阵列基类
class Array{
public:
    bool is_using;//当前正在使用
    int array_type;//阵列的类型
    int row_num;//大小，行数
    int col_num;//大小，列数
    double read_number;//读次数，以操作数数量为计数单位
    double write_number;//写次数，以行数为计数单位
    map<char,double> use_time;//存储两个变量，开始时间：s,结束时间：o,o可以更新
};

//继承
struct lut_arr : public Array{
    bool lut_m;//是否用作存储
    int lut_latch;//记录当前输出
    map<int,bool> op_type;//存放当前LUT支持的操作类型，最大为3，如果有非按位运算，最大为1
    //当LUT用作存储时，这个这个表用来存节点操作数
};

struct magic_arr : public Array{
    vector<int> store_node;//存储的节点操作数ID
};

struct sa_arr : public Array{
    int sa_buffer;//sa暂存当前输出
    int sa_direct;//接收直接输入
    vector<int> store_node;//存储的节点操作数ID
};

/*中层：cache一致性*/
//cache一致性函数,输入执行阵列类型，ID,当前节点存储表，输出是否命中
bool cache_like(int array_type,int array_id,vector<vector<int>> &wb_pos);

//节点执行表更新策略,返回值表示是否需要更新
bool update_wb_pos(bool cache_like,int pos_input,int array_type,int pos_array);

//流水线
/*中层：阵列行为*/
void find_input(int &array_type,int &array_id,int op_type,Node* node_depend=nullptr,int cycle=0);//寻找操作数来源,node_depend指向nodes中的节点

//决定执行阵列的类型
int decide_array_type(int op_type,int design_target);//由算子支持和设计目标共同决定

//决定执行阵列的ID,输入操作数个数1false2true,输入参数带默认值，-1表示无
int decide_array_id(int op_type,vector<Node> &nodes,int decide_array_type,vector<lut_arr> &array_list1,vector<sa_arr> &array_list2,\
            vector<magic_arr> &array_list3,int input1_type=0,int input1_id=0,int input2_type=0,int input2_id=0);

//立即数，随便找一个能用的就行
int find_no_using(int op_type,vector<Node> &nodes,int decide_array_type,vector<lut_arr> &array_list1,vector<sa_arr> &array_list2,\
                                                                                vector<magic_arr> &array_list3);

//等待、建立逻辑，等待过程如何反映在代码中？？
int build(int decide_array_type,int op_type,vector<lut_arr> &array_list1,vector<sa_arr> &array_list2,\
                                                            vector<magic_arr> &array_list3);

//计算阵列剩余
int cap_array_lost(int decide_array_type,int decide_array_id,vector<Node> &nodes,vector<lut_arr> &array_list1,\
                            vector<sa_arr> &array_list2,vector<magic_arr> &array_list3);
//可覆盖容量
int cap_array_cover(int decide_array_type,int decide_array_id,vector<Node> &nodes,vector<lut_arr> &array_list1,\
                            vector<sa_arr> &array_list2,vector<magic_arr> &array_list3);

//需要单独写一个函数，得出计算每一个op需要的剩余容量

//数据读函数,输入：各阵列表，执行的运算节点
//目的：找到输入数据依赖的“阵列“，完成数据搬移：需要增加读就读++，需要移动写就写++
// 操作：修改阵列读写和时间参数
void data_read(int input_type,int input_id,int decide_array_type,int decide_array_id,int *Register,\
                    vector<lut_arr> &array_list1,vector<sa_arr> &array_list2,vector<magic_arr> &array_list3);

//判断节点的写回表中有当前阵列：目的是判断阵列存储中是否有数据
bool is_in_wb(int array_type,int array_id,Node *node_now);

//写入逻辑
void input_logic(int input1_type,int input1_id,int input2_type,int input2_id,int decide_array_type,int decide_array_id,\
       Node *now,int *Register,vector<lut_arr> &array_list1,vector<sa_arr> &array_list2,vector<magic_arr> &array_list3);

                 //输出逻辑
void output_logic(int decide_array_type,int decide_array_id,int op_type,Node *now,\
                  vector<lut_arr> &array_list1,vector<sa_arr> &array_list2,vector<magic_arr> &array_list3);

//lut执行逻辑
void op_lut(int op_type,Node* now,vector<lut_arr> &array_list1);

//sa执行逻辑
void op_sa(int op_type,Node* now,vector<sa_arr> &array_list2);

//sa执行逻辑
void op_magic(int op_type,Node* now,vector<magic_arr> &array_list3);

//出度更新
void out_degree(Node *now);

//出度为0的节点对应阵列的擦除,更新节点的写回表和阵列的存储节点
void erase_array(Node* now,vector<lut_arr> &array_list1,vector<sa_arr> &array_list2,vector<magic_arr> &array_list3);

//写覆盖
void write_cover();


//LUT计算函数：
void com_lut(int type_operation,int bit_num_operand,int op_num,vector<lut_arr> &array_list1,\
                                    vector<sa_arr> &array_list2,vector<magic_arr> &array_list3);

//sa计算函数
void com_sa(int type_operation,int bit_num_operand,int op_num,vector<lut_arr> &array_list1,\
                                    vector<sa_arr> &array_list2,vector<magic_arr> &array_list3);

//magic计算函数
void com_magic(int type_operation,int bit_num_operand,int op_num,vector<lut_arr> &array_list1,\
                                    vector<sa_arr> &array_list2,vector<magic_arr> &array_list3);


//阵列尺寸设定函数，一定是方形的
unsigned int arr_size(int logic_type,unsigned int bit_num_operand);



//查找表
//AES中的乘法
class Lut
{
public:
    double Burn_cycle(char type,int bl,int wl);//烧录时间
    double Burn_power(char type,int bl,int wl);//烧录功耗

};
class Magic
{
    public:
        //需要输入进行该操作的数字的输入个数、位宽
        //单位执行周期函数
        int Magic_cycle(int type);
        double Magic_power(int type,int bl,int wl);

};
class SA
{
    public:
        bool sa_using;//SA占用情况，读写执行都要占用最下方的SA
        int SA_cycle(int type);
        double SA_power(int type,int bl,int wl);

};
