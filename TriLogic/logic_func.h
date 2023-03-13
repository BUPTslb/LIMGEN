//#pragma once //只被编译一次
#include<string>
#include "map"
#include"vector"
using namespace std;
//阵列基类
class Array{
public:
    bool is_using;//当前正在使用
    int logic_type;//阵列的类型
    int row_num;//大小，行数
    int col_num;//大小，列数
    double read_number;//读次数，以操作数数量为计数单位
    double write_number;//写次数，以行数为计数单位
    map<char,double> use_time;//存储两个变量，开始时间：s,结束时间：o,o可以更新
};
//继承
struct lut_arr : public Array{
    bool lut_m;//是否用作存储
    map<int,bool> op_type;//存放当前LUT支持的操作类型
};
struct magic_arr : public Array{
    map<int,bool> unknown;//目前还不知道要用什么
    vector<int> store_node;//存储的节点操作数ID,如果是LUT模式，则代表其可以完成的运算
};
struct sa_arr : public Array{
    map<int,bool> unknown;//同上
    vector<int> store_node;//存储的节点操作数ID,如果是LUT模式，则代表其可以完成的运算
};
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
