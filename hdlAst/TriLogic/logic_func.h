#pragma once //只被编译一次
#include<string>
class Magic
{
    public:
        int num_wl;//字线数
        int num_bl;//位线数
        int num_sl;//选线数
        int cycle;//周期数
        double power;//功耗
        double area;//面积
        int num_sa;//使用的sa数量

        //需要输入进行该操作的数字的输入个数、位宽
        //单位执行周期函数
        int Magic_cycle(char type);
        double Magic_power(char type,int bl,int wl);

};
class SA
{
    public:
        int num_wl;//字线数
        int num_bl;//位线数
        int num_sl;//选线数
        int cycle;//周期数
        double power;//功耗
        double area;//面积
        int num_sa;//使用的sa数量

        bool sa_using;//SA占用情况，读写执行都要占用最下方的SA
        int SA_cycle(char type);
        double SA_power(char type,int bl,int wl);

};
//查找表
//AES中的乘法
class Lut
{
    public:
        int num_wl;//字线数
        int num_bl;//位线数
        int num_sl;//选线数
        int cycle_write;//写周期周期
        int cycle=1;//读周期数，即运行周期
        double power;//功耗
        double area;//面积
        int num_sa;//使用的sa数量
    
        double Burn_cycle(char type,int bl,int wl);//烧录时间
        double Burn_power(char type,int bl,int wl);//烧录功耗


};