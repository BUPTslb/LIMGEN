#ifndef SYNTHESIS_MAINFUNC_H
#define SYNTHESIS_MAINFUNC_H

#include<iostream>
#include<fstream>
#include<sstream>
#include<map>
#include<SDL2/SDL.h>
#include<typeinfo>
#include <algorithm>
#include"rapidjson/document.h"
#include <vector>
#include <queue>
#include <set>

using namespace std;
//定义节点类型
struct Node {
    int node_id;//节点ID,用来代表节点
    string operator_name;//算子，只算有操作的，用来选逻辑族，也能判断是不是写操作
    Node *depend1 = NULL;//两个数据依赖指针,初始定义为NULL
    Node *depend2 = NULL;//指向前面
    Node *control = NULL;//控制依赖
    double start_time;//开始时间
    double end_time;//结束时间
    int do_type;//执行的类型 -1 REG 1 LUT 2 SA 3 MA 4 LUT-OUT 5 SA-BUFFER
    int finish_id;//节点执行完（输出）的位置
    int out_degree;//出度
    //节点存储表,结构体中不能对向量对象进行初始化
    vector<vector<int>> wb_pos;//存储节点执行完的位置
    //存储节点写回的位置,0:register,1:lut,2:sa,3:ma
};

//阵列基类
class Array {
public:
    int array_id;
    bool is_using;//当前正在使用
    int array_type;//阵列的类型
    int row_num;//大小，行数
    int col_num;//大小，列数
    double read_number;//读次数，以操作数数量为计数单位
    double write_number;//写次数，以行数为计数单位
    double start_time;//存储两个变量，开始时间：s,结束时间：o,o可以更新
    double over_time;
};

//继承
struct lut_arr : public Array {
    bool lut_m;//是否用作存储
    int lut_latch;//记录当前输出
    set<int> op_type;//存放当前LUT支持的操作类型，最大为3，如果有非按位运算，最大为1
    //当LUT用作存储时，这个这个表用来存节点操作数
};

struct magic_arr : public Array {
    vector<int> store_node;//存储的节点操作数ID
};

struct sa_arr : public Array {
    int sa_buffer;//sa暂存当前输出
    int sa_direct;//接收直接输入
    vector<int> store_node;//存储的节点操作数ID
};

int Type2node(string type);//Type2node函数的声明
Node *find_node_by_number(std::vector<Node> &nodes, int node_id);//寻找节点指针的函数声明
//topo排序
void topologicalSort(vector<Node> nodes, map<int, int> &inDegree, \
                        vector<vector<Node>> &controlstep, \
                        map<int, int> &id_pos);//函数声明
void ComIndegree(queue<int> &nodeQueue, map<int, int> &inDegree, \
                        vector<Node> &nodes, map<int, int> &id_pos);//子函数声明
//将算子转换成符号
int op2int(string operation);


void redirectCoutToFile(vector<vector<Node *>> control_step, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                        vector<magic_arr> &array_list3);

void writeVectorToFile(const std::vector<int>& A) ;

#endif //SYNTHESIS_MAINFUNC_H
