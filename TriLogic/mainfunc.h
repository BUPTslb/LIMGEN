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

using namespace std;
//定义节点类型
struct Node{
    int node_id;//节点ID,用来代表节点
    string operator_name;//算子，只算有操作的，用来选逻辑族，也能判断是不是写操作
    Node* depend1=NULL;//两个数据依赖指针,初始定义为NULL
    Node* depend2=NULL;//指向前面
    Node* control=NULL;//控制依赖
    double start_time;//开始时间
    double end_time;//结束时间
    int do_type;//执行的类型 -1 REG 1 LUT 2 SA 3 MA 4 LUT-OUT 5 SA-BUFFER
    int finish_id;//节点执行完（输出）的位置
    int out_degree;//出度
    //节点存储表,结构体中不能对向量对象进行初始化
    vector<vector<int>> wb_pos;//存储节点执行完的位置
    //存储节点写回的位置,0:register,1:lut,2:sa,3:ma
};

int Type2node(string type);//Type2node函数的声明
Node * find_node_by_number(std::vector<Node>& nodes, int node_id);//寻找节点指针的函数声明
//topo排序
void topologicalSort(vector<Node> nodes,map<int,int> &inDegree,\
                        vector<vector<Node>> &controlstep,\
                        map<int,int> &id_pos) ;//函数声明
void ComIndegree(queue<int> &nodeQueue,map<int,int> &inDegree,\
                        vector<Node> &nodes,map<int,int> &id_pos);//子函数声明
//将算子转换成符号
int op2int(string operation);

#endif //SYNTHESIS_MAINFUNC_H
