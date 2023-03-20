#include "mainfunc.h"
using namespace std;
//定义函数，判断节点的类型
/*节点中包含的字符串，用作匹配*/
std::string op= "Op";
std::string assign="Assign";
std::string branch="Branch";
std::string loop="Loop";
//算子
std::string seq="SEQ";//赋值
std::string l_less="HdlOpType.LT";//比较
std::string l_bigger="HdlOpType.GT";
std::string l_shiftL="HdlOpType.SLL";//移位
std::string l_shiftR="HdlOpType.SRL";
std::string l_nor="HdlOpType.NOR";
std::string l_xor="HdlOpType.XOR";
std::string l_and="HdlOpType.AND";
std::string l_or="HdlOpType.OR";
std::string l_not="HdlOpType.NEG";//非
std::string l_mul="HdlOpType.MUL";//乘法
std::string l_add="HdlOpType.ADD";//加法

int Type2node(std::string type){
    if(type==op)
        return 1;
    if(type==assign)
        return 2;
    if(type==branch)
        return 3;
    if(type==loop)
        return 4;
    return 0;
}
//根据节点id，返回节点指针，好像和id_pos功能冲突了
Node * find_node_by_number(std::vector<Node>& nodes, int node_id) {
    auto it = std::find_if(nodes.begin(), nodes.end(),
                           [node_id](const Node& node) {
                               return node.node_id == node_id;
                           });

    if (it != nodes.end()) {
        return &(*it);//返回指针指向的变量的地址
    } else {
        return nullptr;
    }
}
//计算入度的函数,直接对nodes进行修改
void ComIndegree(queue<int> &nodeQueue,map<int,int> &inDegree,vector<Node> &nodes,map<int,int> &id_pos){
    if (nodeQueue.empty())
        return;
    //nodeQueue存放当前入度为零的点的id
    while (!nodeQueue.empty())
    {
        int now_id=nodeQueue.front();
        nodeQueue.pop();//出队
        int node_pos=id_pos[now_id];//获得节点的位置
        //从当前位置开始循环
        for (int i = node_pos; i < nodes.size(); ++i) {
            if (nodes[i].control&&nodes[i].control->node_id == now_id){

                nodes[i].control=NULL;
//                if (nodes[i].control==NULL)
//                    cout<<"当前节点"<<nodes[i].node_id<<"对节点"<<now_id<<"存在控制依赖，已消除"<<endl;
            }
            if (nodes[i].depend1&&nodes[i].depend1->node_id==now_id){
                nodes[i].depend1=NULL;
//                if (nodes[i].depend1==NULL)
//                    cout<<"当前节点"<<nodes[i].node_id<<"对节点"<<now_id<<"存在数据依赖，已消除"<<endl;
            }
            if (nodes[i].depend2&&nodes[i].depend2->node_id==now_id){
                nodes[i].depend2=NULL;
//                if (nodes[i].depend2==NULL)
//                    cout<<"当前节点"<<nodes[i].node_id<<"对节点"<<now_id<<"存在数据依赖，已消除"<<endl;
            }
            //如果指针依赖全为空，则将其入度设置为0
            //问题在这！！不能用now_id,now_id已经是0了
            if (inDegree[nodes[i].node_id]>0 &&nodes[i].control==NULL && \
                    nodes[i].depend1==NULL && nodes[i].depend2==NULL)
            {
                inDegree[nodes[i].node_id]=0;
//                cout<<"已经将节点"<<nodes[i].node_id<<"的入度设置为0"<<endl;
            }
        }
    }
    //现在得到了一个新的inDegree
}
//拓朴排序，返回节点指针
//递归调用，获取控制步：vector<vector<Node*>>
void topologicalSort(vector<Node> nodes,map<int,int> &inDegree,\
                        vector<vector<Node>>& controlstep,\
                        map<int,int> &id_pos) {
//    cout<<"迭代次数："<<test<<endl;
    vector<Node> stepnow;//当前控制步，存放节点指针
    queue<int>  nodeQueue;//存放度为0的节点
    // 计算每个节点的入度
    // 将入度为0的节点加入队列
    //由于node是指针，只需要获取nodes中节点的地址即可
    for (auto it =inDegree.begin(); it !=inDegree.end();it++)
    {
//        cout<<it->first<<"  "<<it->second<<endl;//当前hash表的值
        int now_pos=id_pos[it->first];//当前要入队的节点在nodes中的位置
//        cout<<"当前要入队的节点在nodes中的位置"<<now_pos<<"当前要入队的节点"<<nodes[now_pos].node_id<<endl;
        if (it->second == 0) {
            stepnow.push_back(nodes[id_pos[it->first]]);
            nodeQueue.push(it->first);//将当前入度为0的节点号码入队
//            cout<<"新入队的元素为："<<nodeQueue.back()<<endl;
//            cout<<"新进入step的元素ID："<<nodes[id_pos[it->first]].node_id<<endl;
            it->second=-1;//将当前的值设为-1，表示已经入队了
        }
    }
    //将stepnow加入到控制步中
    if(!stepnow.empty())
    {
        controlstep.push_back(stepnow);
    }
    // 执行拓扑排序,有许多节点依赖于入度为0的节点
    //control,depend1,depend2(都是指针)=inDegree[]为0的点
    bool stop_topoSort=nodeQueue.empty();//根据队列是否为空判断是否该停止循环
    ComIndegree(nodeQueue,inDegree,nodes,id_pos);//更新inDegree
    //现在得到了新的inDegree，可以进行递归了
    //防止无限循环,如果map中还有大于等于0的点
    if (!stop_topoSort) {
        topologicalSort(nodes, inDegree,controlstep,id_pos);
    }
}
int op2int(string operation){
    if(operation==seq)  return 0;//赋值，写操作
    if(operation==l_less)  return 1;
    if(operation==l_bigger)  return 2;
    if(operation==l_shiftL)  return 3;
    if(operation==l_shiftR)  return 4;
    if(operation==l_and)  return 5;
    if(operation==l_or)  return 6;
    if(operation==l_not)  return 7;
    if(operation==l_nor)  return 8;
    if(operation==l_xor)  return 9;
    if(operation==l_add)  return 10;
    if(operation==l_mul)  return 11;
}




//
// Created by shenlibo on 23-3-7.
//