#include"logic_func.h"
/*******************************************************
     可以使用hash表模拟LUT
     hash表的构建过程就类似于LUT的烧录过程，只不过加上损耗参数
     hash表的查找过程就相当于LUT的搜索过程
*********************************************************/
double Lut::Burn_cycle(char type,int bl,int wl)//烧录时间
{

            


}
double Lut::Burn_power(char type,int bl,int wl)//烧录功耗
{


}


int Magic::Magic_cycle(char type)
{
    int cycle_time;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type)
    {
        case 'NOR'://NOR
            cycle_time=1;
            break;
        case 'NOT'://NOT
            cycle_time=1;
            break;
        case 'AND'://AND
            cycle_time=2;
            break;
        case 'OR'://OR
            cycle_time=1;
            break;
        case 'XOR'://XOR
            cycle_time=3;
            break;
        case '+'://+
            cycle_time=10;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_time;

}
double Magic::Magic_power(char type,int bl,int wl)
{
    int cycle_power;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type)
    {
        case 'NOR'://NOR
            cycle_power=10*wl;
            break;
        case 'NOT'://NOT
            cycle_power=5*wl;
            break;
        case 'AND'://AND
            cycle_power=20*wl;
            break;
        case 'OR'://OR
            cycle_power=5*wl;
            break;
        case 'XOR'://XOR
            cycle_power=30*wl;
            break;
        case '+'://+
            cycle_power=100*wl;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_power;
}
int SA::SA_cycle(char type)
{
    int cycle_time;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type)
    {
        case 'NOT'://NOR
            cycle_time=1;
            break;
        case 'AND'://AND
            cycle_time=1;
            break;
        case 'OR'://OR
            cycle_time=1;
            break;
        case 'XOR'://XOR
            cycle_time=1;
            break;
        case '+'://+
            cycle_time=10;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_time;
}
double SA::SA_power(char type,int bl,int wl)
{
    int cycle_power;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type)
    {
        case 'NOT'://NOT
            cycle_power=5*wl;
            break;
        case 'AND'://AND
            cycle_power=20*wl;
            break;
        case 'OR'://OR
            cycle_power=5*wl;
            break;
        case 'XOR'://XOR
            cycle_power=30*wl;
            break;
        case '+'://+
            cycle_power=100*wl;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_power;
}