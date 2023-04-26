#include "logic_func.h"
//当前能用的阵列表,针对当前type,如果为空，证明都用不了
//此时sa和magic不用考虑容量，让决定id的去考虑，只需要把可用的放进队列去
vector<int> find_no_using(int op_type, int decide_array_type, vector<lut_arr> &array_list1,
                          vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    vector<int> find_no_using;
    switch (decide_array_type) {
        case 1://找LUT,需要算子支持，并且能用
        {
            bool module_need=false;
            for (auto i :array_list1)
                if (!i.is_using)//空闲，算子不可用，但是还有空间，可以添加算子
                {
                    //TODO:模块调用 1 2 3 11 12 13
                    if (op_type==1||op_type==2||op_type==3||op_type==11||op_type==12||op_type==13)  module_need=true;
                    //1. no module_need
                    if (!module_need)
                    {
                        // have func
                        if (i.op_type.find(op_type)!=i.op_type.end()
                            || i.op_type.empty()
                            || i.op_type.find(op_type)==i.op_type.end() && i.op_type.size()<3)
                            find_no_using.push_back(i.array_id);

                    }
                    else
                    {
                        //2.need module
                        //必须功能相同
                        if (i.op_type.find(op_type)!=i.op_type.end())
                            find_no_using.push_back(i.array_id);

                    }


                }
            //没有可用的，跳过
        }
            break;
        case 2://找SA
        {
            for (const auto& i : array_list2) {
//                if (cap_array_lost(decide_array_type, i, nodes, array_list1, array_list2, array_list3) < 2)
//                    continue; //判断空间是否够用放在外面
                if (!i.is_using)//空闲
                    find_no_using.push_back(i.array_id);
                //没有可用的，跳过
            }

        }
            break;
        case 3: {
            for (const auto& i :array_list3) {
                if (!i.is_using)//空闲
                    find_no_using.push_back(i.array_id);
                //没有可用的，跳过
            }
        }
            break;
        default:
            break;
    }
    return find_no_using;
}

//阵列等待表,front是等待时间最短的,按照等待时间排序
//只以等待时间来排序，容量的事由后续的函数考虑
vector<int> waiting_array_list(int op_type, int decide_array_type, vector<lut_arr> &array_list1,
                               vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    vector<int> waiting_array_list;
    priority_queue<Array, vector<Array>, CompareArray> pq;//优先队列，小顶堆
    switch (decide_array_type) {
        case 1://找LUT,需要算子支持，并且能用
        {
            bool module_need=false;
            if (op_type==1||op_type==2||op_type==3||op_type==11||op_type==12||op_type==13)  module_need=true;
            for (auto &i: array_list1) {
                if (i.is_using)//不空闲
                {
                    if (!module_need)
                    {

                        if (i.op_type.find(op_type)!=i.op_type.end()
                            || i.op_type.empty()
                            || i.op_type.find(op_type)==i.op_type.end() && i.op_type.size()<3)
                            pq.push(i);

                    }
                    else
                    {
                        if (i.op_type.find(op_type)!=i.op_type.end())
                            pq.push(i);
                    }
                }
            }
        }
            break;
        case 2://找SA
        {
            for (auto &i: array_list2) {
                if (i.is_using)//不空闲
                    pq.push(i);
                //没有可用的，跳过
            }

        }
            break;
        case 3: {

            for (auto &i: array_list3) {
                if (i.is_using)//不空闲
                    pq.push(i);
                //没有可用的，跳过
            }

        }
            break;
        default:
            break;
    }

    if (pq.empty()) return waiting_array_list;

    for (int i = 0; i < pq.size(); ++i) {
        waiting_array_list.push_back(pq.top().array_id);//向量首部是结束时间最小的
        pq.pop();
    }
    return waiting_array_list;

}


//
// Created by shenlibo on 23-4-19.
//
