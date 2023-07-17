#include "mainfunc.h"
#include "parameter.h"

using namespace std;
//定义函数，判断节点的类型
/*节点中包含的字符串，用作匹配*/
std::string op= "Op";
std::string assign="Assign";
std::string branch="Branch";
std::string loop="Loop";
//算子
std::string seq="SEQ";//赋值
std::string l_eq="HdlOpType.EQ";//比较
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
std::string l_div="HdlOpType.DIV";//除法，应该不会使用


//将nodes中的链接关系设置到nodes2中，对nodes2进行初始化
void reset_nodes2()
{
    //主要初始化指针相关的
    nodes2=nodes; //值相等了
    //设置指针
    for (int i = 0; i < nodes.size(); ++i) {
        if (nodes[i].depend1!= nullptr)
        {
            nodes2[i].depend1=find_node_by_number(nodes[i].depend1->node_id);
        }
        if (nodes[i].depend2!= nullptr)
        {
            nodes2[i].depend2=find_node_by_number(nodes[i].depend2->node_id);
        }
        if (nodes[i].control!= nullptr)
        {
            nodes2[i].control=find_node_by_number(nodes[i].control->node_id);
        }
    }
}


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
Nodes * find_node_by_number(int node_id) {
    auto it = std::find_if(nodes2.begin(), nodes2.end(),
                           [node_id](const Nodes& node) {
                               return node.node_id == node_id;
                           });

    if (it != nodes2.end()) {
        return &(*it);//返回指针指向的变量的地址
    } else {
        return nullptr;
    }
}

Nodes * find_node_by_number1(int node_id) {
    auto it = std::find_if(nodes.begin(), nodes.end(),
                           [node_id](const Nodes& node) {
                               return node.node_id == node_id;
                           });

    if (it != nodes.end()) {
        return &(*it);//返回指针指向的变量的地址
    } else {
        return nullptr;
    }
}

int op2int(string operation){
    if (operation == seq)           return 0;//赋值，写操作
    if (operation == l_eq)          return 1;//相等
    if (operation == l_less)        return 2;
    if (operation == l_bigger)      return 3;
    if (operation == l_shiftL)      return 4;//逻辑左移，沒有用到
    if (operation == l_shiftR)      return 5;//逻辑右移，沒有用到
    if (operation == l_and)         return 6;//sa ma
    if (operation == l_or)          return 7;//sa ma
    if (operation == l_not)         return 8;//sa ma
    if (operation == l_nor)         return 9;//ma
    if (operation == l_xor)         return 10;//sa ma
    if (operation == l_add)         return 11;
    if (operation == l_mul)         return 12;
    if (operation == l_div)         return 13;
    return -1;
}


vector<Lut_Record> lut_record= lut_records();
vector<Lut_Record> lut_records()
{
    vector<Lut_Record> records; // 存储所有记录的向量
    ifstream lutfs;
    lutfs.open("..\\TriLogic\\database\\database_lut.txt");
    if (lutfs.is_open())
    {
        cout << "Opening file Successfull!" << endl;
        string line;
        while (getline(lutfs, line)) { // 逐行读取文件内容
//            size_t pos = 0;
//            string token;
//            int count = 0;
            Lut_Record item{};
            while (lutfs >> item.op_type >> item.data_bits >> item.lut4_num >> item.lut4_level >> item.lut6_num >> item.lut6_level){
                records.push_back(item);
                cout<<item.op_type<<" "<<item.data_bits<<" "<<item.lut4_num<<" "<<item.lut4_level<<" "<<item.lut6_num<<" "<<item.lut6_level<<endl;
            }
        }
    }
    else
    {
        cout<< "Opening file Failed!" << endl;
    }
    lutfs.close();

    return records;
}
//使用的lut数量
int lut_num_op(int op_type,int lut_type)
{
    for (auto &i : lut_record) {
        if (i.op_type==op_type && i.data_bits==bit_num_operand)
        {
            if (lut_type==4)
                return i.lut4_num;
            if (lut_type==6)
            {
                cout<<"lut_6"<<i.lut6_num<<endl;
                return i.lut6_num;
            }
        }
    }
    return bit_num_operand;//其他按位操作

}
//使用的lut级别数
int lut_level_op(int op_type,int lut_type){
    for (auto &i : lut_record) {
        if (i.op_type==op_type && i.data_bits==bit_num_operand)
        {
            if (lut_type==4)
                return i.lut4_level;
            if (lut_type==6)
                return i.lut6_level;
        }
    }
    return 1;//未定义的操作
}




//
// Created by shenlibo on 23-3-7.
//
