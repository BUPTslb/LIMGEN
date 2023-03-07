#include<iostream>
#include<fstream>
#include<sstream>
#include<map>
#include<SDL2/SDL.h>
#include<typeinfo>
#include <algorithm>
#include"rapidjson/document.h"
#include"logic_func.h"
#include "vector"
#include <queue>

using namespace std;
//定义节点类型
struct Node{
    int node_id;//节点ID,用来代表节点
    string operator_name;//算子，只算有操作的，用来选逻辑族，也能判断是不是写操作
    Node* depend1=NULL;//两个数据依赖指针,初始定义为NULL
    Node* depend2=NULL;//指向前面
    Node* control=NULL;//控制依赖
};

int Type2node(string type);//Type2node函数的声明
Node * find_node_by_number(std::vector<Node>& nodes, int node_id);//寻找节点指针的函数声明
//topo排序
void topologicalSort(vector<Node> nodes,map<int,int> &inDegree,\
                        vector<vector<Node>>& controlstep,\
                        map<int,int> &id_pos) ;//函数声明
void ComIndegree(queue<int> &nodeQueue,map<int,int> &inDegree,\
                        vector<Node> &nodes,map<int,int> &id_pos);//子函数声明
//将算子转换成符号
int op2int(string operation);

//
// Created by shenlibo on 23-3-7.
//

#ifndef SYNTHESIS_MAINFUNC_H
#define SYNTHESIS_MAINFUNC_H

#endif //SYNTHESIS_MAINFUNC_H
