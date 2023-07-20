#ifndef SYNTHESIS_MAINFUNC_H
#define SYNTHESIS_MAINFUNC_H

#include<iostream>
#include<fstream>
#include<sstream>
#include<map>
#include<typeinfo>
#include <algorithm>
#include"rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "parameter.h"
#include <queue>
#include <set>

using namespace std;
//设计目标
const int design_target = 2;//1速度，2面积，3功耗
//设计空间探索的限制
const double cycle_limit = 500;//运行时间(速度)限制，单位：周期
const double area_limit = 1000;//面积限制，单位：
const double power_limit = 10;//功耗限制，单位：
//定义数据的位数，这个后期要修改，从CDFG中得到
const int bit_num_operand = 16;//操作数的位数，全局变量，暂时定义为16

//定义节点类型
struct Nodes {
    int node_id;//节点ID,用来代表节点
    string operator_name;//算子，只算有操作的，用来选逻辑族，也能判断是不是写操作
    Nodes *depend1 = NULL;//两个数据依赖指针,初始定义为NULL
    Nodes *depend2 = NULL;//指向前面
    Nodes *control = NULL;//控制依赖
    double start_time;//开始时间
    double end_time;//结束时间
    int do_type;//执行的类型 -1 REG 1 LUT 2 SA 3 MA 4 LUT-latch 5 SA-BUFFER 6 SA存储
    int finish_id;//节点执行完（输出）的位置
    int out_degree;//出度
    //节点存储表,结构体中不能对向量对象进行初始化
    vector<vector<int>> wb_pos;//存储节点执行完的位置
    //存储节点写回的位置,0:register,1:lut,2:sa,3:ma,4:lut_latch,5:sa-buffer
    Nodes(){
        wb_pos.resize(6);
    }
};

struct Array_place{
    int array_id;//阵列ID
    int array_type;//阵列类型
    int array_width;
    int array_height;
    int pos_x;//阵列位置
    int pos_y;//阵列位置
    std::vector<int> connect_line;//连接线
};

extern vector<Nodes> nodes;//节点类型的向量，里面可以放下一个个节点

extern vector<Nodes> nodes2;

extern vector<Array_place> place_list;;

void reset_nodes2();


//定义函数，输入操作类型，操作数位数，lut类型，输出数量和级数
int lut_num_op(int op_type, int lut_type);

int lut_level_op(int op_type, int lut_type);

//阵列基类
class Array {
public:
    int array_id;
    bool is_using;//当前正在使用
    int row_num;//大小，行数
    int col_num;//大小，列数
    double read_number;//读次数，以操作数数量为计数单位
    double write_number;//写次数，以行数为计数单位
    double start_time;
    double over_time;
    double energy;//在RRAM/lux上消耗的能量
    //与array_list中阵列的数据交换次数表，从xx阵列中拿数据
    vector<vector<int>> data_exchange;//0:register,1:lut,2:sa,3:ma

    Array(){
        data_exchange.resize(4);
    }

};

//继承
struct lut_arr : public Array {
    vector<int> lut_latch;//记录buffer锁存
    int lut_out;//当前的输出
//    set<int> op_type;//存放当前LUT支持的操作类型，最大为3，如果有非按位运算，最大为1
    int op_type;//存放当前LUT支持的操作类型，最大为3，如果有非按位运算，最大为1
    int lut_num;//正常情况=列数，调用模块时候=模块使用的lut数量

};

struct sa_arr : public Array {
    vector<int> sa_buffer;//sa暂存当前输出
    int sa_type; // 1,csa 2,dsa 在建立的时候就需要确定
    int sa_direct;//接收直接输入
    int sa_out;//当前sa的输出
    bool add_use;
    vector<int> store_node;//存储的节点操作数ID,规定：只存储=
    ~sa_arr(){
        sa_buffer.clear();
    }
};

struct magic_arr : public Array {
    bool add_use;
    vector<int> store_node;//存储的节点操作数ID，规定：只存储=
    ~magic_arr(){
        store_node.clear();
    }
};

int Type2node(string type);//Type2node函数的声明
//在构建节点关系的时候用1
Nodes *find_node_by_number1(int node_id);//寻找节点指针的函数声明
Nodes *find_node_by_number(int node_id);//寻找节点指针的函数声明
//topo排序
void topologicalSort(vector<Nodes> nodes, map<int, int> &inDegree,
                     vector<vector<Nodes>> &controlstep, map<int, int> &id_pos);//函数声明
void ComIndegree(queue<int> &nodeQueue, map<int, int> &inDegree,
                 vector<Nodes> &nodes, map<int, int> &id_pos);//子函数声明
//将算子转换成符号
int op2int(string operation);

//控制步处理函数
std::vector<double>
control_step(vector<vector<Nodes *>> &controlstep2, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
             vector<magic_arr> &array_list3);

std::vector<double>
only_lut(vector<vector<Nodes *>> controlstep2, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
         vector<magic_arr> &array_list3);

std::vector<double>
only_sa(vector<vector<Nodes *>> controlstep2, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
        vector<magic_arr> &array_list3);

std::vector<double>
only_magic(vector<vector<Nodes *>> controlstep2, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
           vector<magic_arr> &array_list3);



//全部运行完之后
//计算架构性能参数
//延迟,所有时间中的最大值
double
latency_all(std::vector<lut_arr> &array_list1, std::vector<sa_arr> &array_list2, std::vector<magic_arr> &array_list3);

//面积，阵列+buffer+Reg
double
area_all(std::vector<lut_arr> &array_list1, std::vector<sa_arr> &array_list2, std::vector<magic_arr> &array_list3);

double area_all_lut(std::vector<lut_arr> &array_list1, std::vector<sa_arr> &array_list2, std::vector<magic_arr> &array_list3);

//能耗,阵列+buffer+Reg
double
energy_all(std::vector<lut_arr> &array_list1, std::vector<sa_arr> &array_list2, std::vector<magic_arr> &array_list3);

void get_best(std::vector<double> &best_latency,
              std::vector<double> &best_energy,
              std::vector<double> &best_area,
              std::vector<double> latency_energy,
              std::vector<int> &array_num_latency,
              std::vector<int> &array_num_energy,
              std::vector<int> &array_num_area,
              std::vector<lut_arr> &array_list1,
              std::vector<sa_arr> &array_list2,
              std::vector<magic_arr> &array_list3);

#endif //SYNTHESIS_MAINFUNC_H
