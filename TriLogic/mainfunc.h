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
#include"logic_func.h"
#include "vector"
#include <queue>

using namespace std;
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

#endif //SYNTHESIS_MAINFUNC_H
