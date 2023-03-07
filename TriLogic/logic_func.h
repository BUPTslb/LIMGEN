#pragma once //只被编译一次
#include<string>
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
        int Magic_cycle(char type);
        double Magic_power(char type,int bl,int wl);

};
class SA
{
    public:
        bool sa_using;//SA占用情况，读写执行都要占用最下方的SA
        int SA_cycle(char type);
        double SA_power(char type,int bl,int wl);

};
