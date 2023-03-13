#include"logic_func.h"

using namespace std;

unsigned int arr_size(int logic_type,unsigned int bit_num_operand){
    unsigned int arr_size;
    switch (logic_type) {
        case 1://lut,只有四输入和六输入
            arr_size = (2*bit_num_operand <= 16) ? 16 : 64;
            break;
        case 2://sa,列数=位数
        {
            unsigned m = 1;
            while (m < bit_num_operand) {
                m <<= 1;
            }
            arr_size = (m <= 64) ? m : 64;
            break;
        }
        case 3://尽量等于2的幂
        {
            unsigned m = 1;
            while (m < bit_num_operand) {
                m <<= 1;
            }
            arr_size = (m <= 64) ? m : 64;
            break;
        }
        default:
            break;

    }
}
void com_lut(int type_operation,int bit_num_operand,int op_num, \
    vector<lut_arr> &array_list1,vector<sa_arr> &array_list2,vector<magic_arr> &array_list3)
{
    //注：我没有考虑数据是从哪读到这里的
    if (array_list1.empty())// 当前阵列表为空
    {
        lut_arr now;//新建阵列
        now.logic_type=1;//lut
        now.is_using=1;
        array_list1.push_back(now);
    }
    //对算子进行筛选
    if (type_operation==0)//赋值操作，写操作
    {


    }else if(type_operation==1||type_operation==2)//比较操作
    {


    }else if(type_operation==3||type_operation==4)
    {


    }else if(type_operation<=9 && type_operation>=5)//按位操作
    {
        //搜索之前的阵列，是否支持这个操作
        //如果其现在是未使用状态，并可以使用，则过
        for (int i = 0; i < array_list1.size(); ++i) {
            //该阵列当前未使用,且功能合适
            if(!array_list1[i].is_using && array_list1.back().op_type[type_operation]){
                //只增加时间和功耗
                array_list1[i].read_number++;
            }
        }

    }else if(type_operation==10)//向量加法操作
    {


    } else if (type_operation==11)//乘法操作
    {


    }

}


double Lut::Burn_cycle(char type,int bl,int wl)//烧录时间
{

            


}
double Lut::Burn_power(char type,int bl,int wl)//烧录功耗
{


}


int Magic::Magic_cycle(int type)
{
    int cycle_time;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type)
    {
        case 8://NOR
            cycle_time=1;
            break;
        case 7://NOT
            cycle_time=1;
            break;
        case 5://AND
            cycle_time=2;
            break;
        case 6://OR
            cycle_time=1;
            break;
        case 9://XOR
            cycle_time=3;
            break;
        case 10://+
            cycle_time=10;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_time;

}
double Magic::Magic_power(int type,int bl,int wl)
{
    int cycle_power;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type)
    {
        case 8://NOR
            cycle_power=10*wl;
            break;
        case 7://NOT
            cycle_power=5*wl;
            break;
        case 5://AND
            cycle_power=20*wl;
            break;
        case 6://OR
            cycle_power=5*wl;
            break;
        case 9://XOR
            cycle_power=30*wl;
            break;
        case 10://+
            cycle_power=100*wl;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_power;
}
int SA::SA_cycle(int type)
{
    int cycle_time;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type)
    {
        case 7://NOT
            cycle_time=1;
            break;
        case 5://AND
            cycle_time=1;
            break;
        case 6://OR
            cycle_time=1;
            break;
        case 9://XOR
            cycle_time=1;
            break;
        case 10://+
            cycle_time=10;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_time;
}
double SA::SA_power(int type,int bl,int wl)
{
    int cycle_power;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type)
    {
        case 7://NOT
            cycle_power=5*wl;
            break;
        case 5://AND
            cycle_power=20*wl;
            break;
        case 6://OR
            cycle_power=5*wl;
            break;
        case 9://XOR
            cycle_power=30*wl;
            break;
        case 10://+
            cycle_power=100*wl;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_power;
}