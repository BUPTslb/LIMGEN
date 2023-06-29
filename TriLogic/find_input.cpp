#include"logic_func.h"
//寻找输入操作数来源：阵列（ID）/寄存器
//寄存器和Buffer 可以直接取出
//阵列的out：时间对应可以直接取出
//阵列中存储：要占用阵列，等阵列执行结束后才能开始
//这里只是寻找，并不读取，所以不用更新时间和能量
//在数据读的函数中更新时间和能量
void find_input(int &array_type, int &array_id, Nodes *node_depend) {

    //存在依赖，这个依赖是一个节点
    //如果是out,看时间是否对等
    if (node_depend->do_type == 1 || node_depend->do_type == 2) {
        //判断当前的阵列输出是不是这个节点
        //如果不是，需要判断其出度和存储数量，将其存储到别的地方去
        array_type = node_depend->do_type;
        array_id = node_depend->finish_id;
    }
        //是存储
    else {
        //设置寻找表
        vector<int> type_list;
        //在写回表中,一共六种
        //0 reg 1 lut(一直为空) 2 sa存储 3 magic 4 lut-buffer 5 sa-buffer
        for (int i = 0; i < 6; ++i) {
            if (!node_depend->wb_pos[i].empty())
                type_list.push_back(i);
        }
        //type_list可能是空的
        array_type = type_list[rand() % (type_list.size())];
        array_id = node_depend->wb_pos[array_type][rand() % node_depend->wb_pos[array_type].size()];
        //对array_type进行转义
        if (array_type == 0) array_type = -1; //reg存储
        if (array_type == 2) array_type = 6; //sa存储
        if (array_type == 3) array_type = 3; //magic存储
        if (array_type == 4) array_type = 4; //lut-buffer存储
        if (array_type == 5) array_type = 5; //sa-buffer存储

    }

    //如果其写回表为空，可能是一个out,调用这个out即可



}
//
// Created by Lenovo on 2023/6/28.
//
