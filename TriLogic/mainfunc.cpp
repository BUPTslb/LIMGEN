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
Node * find_node_by_number(vector<Node> &nodes2,int node_id) {
    auto it = std::find_if(nodes2.begin(), nodes2.end(),
                           [node_id](const Node& node) {
                               return node.node_id == node_id;
                           });

    if (it != nodes2.end()) {
        return &(*it);//返回指针指向的变量的地址
    } else {
        return nullptr;
    }
}
//Node & find_node_by_number(vector<Node> &nodes2,int node_id) {
//    auto it = std::find_if(nodes2.begin(), nodes2.end(),
//                           [node_id](const Node& node) {
//                               return node.node_id == node_id;
//                           });
//
//    if (it != nodes2.end()) {
//        return *it;//返回指针指向的变量的地址
//    } throw std::invalid_argument("Node not found");
//}

int op2int(string operation){
    if (operation == seq)           return 0;//赋值，写操作
    if (operation == l_eq)          return 1;//相等
    if (operation == l_less)        return 2;
    if (operation == l_bigger)      return 3;
    if (operation == l_shiftL)      return 4;
    if (operation == l_shiftR)      return 5;
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

//database
vector<Lut_Record> lut_record= lut_records();
vector<Lut_Record> lut_records()
{
    std::ifstream infile("database_lut.txt");
    std::vector<Lut_Record> records; // 存储所有记录的向量
    std::string line;
    while (std::getline(infile, line)) { // 逐行读取文件内容
        size_t pos = 0;
        std::string token;
        int count = 0;
        Lut_Record item{};
        while (infile >> item.op_type >> item.data_bits >> item.lut4_num >> item.lut4_level >> item.lut6_num >> item.lut6_level){
            records.push_back(item);
        }
    }
    return records;
}
//使用的lut数量
int lut_num_op(int op_type,int lut_type)
{
    for (auto i : lut_record) {
        if (i.op_type==op_type && i.data_bits==bit_num_operand)
        {
            if (lut_type==4)
                return i.lut4_num;
            if (lut_type==6)
                return i.lut6_num;
        }
    }
    return bit_num_operand;//其他按位操作

}
//使用的lut级别数
int lut_level_op(int op_type,int lut_type){
    for (auto i : lut_record) {
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

//out the report
void redirectCoutToFile(vector<vector<Node *>> control_step, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                        vector<magic_arr> &array_list3)
{
    cout<<"start to create report"<<endl;
    ofstream outfile;
    outfile.open("report.txt", ios::out | ios::trunc );
    if (outfile.is_open())
    {
        cout<<"open report.txt"<<endl;
        outfile << "**************************************************"<< endl;
        outfile<< "number of array_lut:" << array_list1.size() << endl;
        for (auto &i: array_list1) {
            outfile << "op_type of lut_array" << i.array_id << endl;
            outfile << "op-num of lut" << i.array_id << "=" << i.op_type.size() << endl;
            outfile << "The op of this lut:  ";
            for (auto &j: i.op_type) {
                outfile << j << "  ";
            }
            outfile<<endl<<"time of this lut:"<<"start time: "<<i.start_time<<"  over time: "<<i.over_time<<endl;

        }
        outfile << "**************************************************"<< endl;
        outfile << "number of array_sa:" << array_list2.size() << endl;
        for (auto &i: array_list2) {
            outfile << "number of stored_node in sa-array-" << i.array_id << "=" << i.store_node.size() << endl;
            for (auto &j: i.store_node) {
                outfile << j << "  ";
            }
            outfile<<endl<<"time of this sa:"<<"start time: "<<i.start_time<<"  over time: "<<i.over_time<<endl;
        }
        outfile << "**************************************************"<< endl;
        outfile << "number of array_magic:" << array_list3.size() << endl;
        for (auto &i: array_list3) {
            outfile << "number of stored_node in magic-array-" << i.array_id << "=" << i.store_node.size() << endl;
            for (auto &j: i.store_node) {
                outfile << j <<"  ";
            }
            outfile<<endl;
        }
        outfile << "**************************************************"<< endl;
        outfile << "the end time of node:" << endl;
        for (auto &i: control_step) {
            for (auto j: i) {
                outfile<< "node-id: " << j->node_id << endl
                       << "depend1 over time =" << (j->depend1 ? j->depend1->end_time : 0) << endl
                       << "depend2 over time =" << (j->depend2 ? j->depend2->end_time : 0) << endl
                       << "control over time =" << (j->control ? j->control->end_time : 0) << endl
                       << "start_time = " << j->start_time << endl
                       << "do_array_type=" << j->do_type << " do_id=" << j->finish_id << endl
                       << "end time = " << j->end_time << endl;
            }
        }
        outfile.close();
    }
    cout<<"close the file"<<endl;

}




//
// Created by shenlibo on 23-3-7.
//
