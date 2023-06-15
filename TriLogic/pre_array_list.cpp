#include "logic_func.h"
//当前能用的阵列表,针对当前type,如果为空，证明都用不了
//此时sa和magic不用考虑容量，让决定id的去考虑，只需要把可用的放进队列去
vector<int> find_no_using(int op_type, int decide_array_type, vector<lut_arr> &array_list1,
                          vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    vector<int> find_no_using;
    //cout<<"find no using vector<int> find_no_using no error"<<endl;
    switch (decide_array_type) {
        case 1://找LUT,需要算子支持，并且能用
        {
            //cout<<"decide_array_type= 1"<<endl;
            //第一步，判断有没有阵列
            if (array_list1.empty())
            {
                //cout<<"find no using 1"<<endl;
                return find_no_using;
            }else
                //cout<<"lut num now = "<<array_list1.size()<<endl;
            //有阵列

            for (int i=0;i<array_list1.size();i++)
                //空闲，并且算子可以使用
                if (!array_list1[i].is_using)//空闲，算子不可用，但是还有空间，可以添加算子
                {
                    //cout<<array_list1[i].array_id<<" is not using"<<endl;
                        //必须功能相同
                   if (array_list1[i].op_type == op_type)
                   {
                       find_no_using.push_back(array_list1[i].array_id);
                       //cout<<"find_no_using 1 push_back no error"<<endl;
                   }

                }
            //没有可用的，跳过
        }
            break;
        case 2://找SA
        {
            //cout<<"decide_array_type= 2"<<endl;
            //第一步，判断有没有阵列
            if (array_list2.empty())
            {
                //cout<<"find no using 2"<<endl;
                return find_no_using;
            }
            //有sa阵列
            for (int i=0;i< array_list2.size();i++) {
                if (!array_list2[i].is_using)//空闲
                {
                    find_no_using.push_back(array_list2[i].array_id);
                    //cout<<"find_no_using 2 push_back no error"<<endl;
                }
                //没有可用的，跳过
            }

        }
            break;
        case 3: {
            //cout<<"decide_array_type= 3"<<endl;
            //第一步，判断有没有阵列
            if (array_list3.empty())
            {
                //cout<<"find no using 3"<<endl;
                return find_no_using;
            }
            for (int i=0;i<array_list3.size();i++) {
                if (!array_list3[i].is_using)//空闲
                {
                    find_no_using.push_back(array_list3[i].array_id);
                    //cout<<"find_no_using 3 push_back no error"<<endl;
                }
                //没有可用的，跳过
            }
        }
            break;
        default:
            break;
    }
    //cout<<"find_no_using return"<<endl;
    return find_no_using;
}

//阵列等待表,front是等待时间最短的,按照等待时间排序
//只以等待时间来排序，容量的事由后续的函数考虑
vector<int> waiting_array_list(int op_type, int decide_array_type, vector<lut_arr> &array_list1,
                               vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    vector<int> waiting_array_list;
    //cout<<" vector<int> waiting_array_list; "<<endl;
    priority_queue<Array, vector<Array>, CompareArray> pq;//优先队列，小顶堆
    //cout<<"  waiting_array_list; 1"<<endl;
    switch (decide_array_type) {
        case 1://找LUT,需要算子支持，并且能用
        {
            //cout<<"  waiting_array_list; 2"<<endl;
            //第一步，判断有没有阵列
            if (array_list1.empty())
            {
//                //cout<<"当前还没有lut阵列"<<endl;
                //cout<<"  waiting_array_list; 3"<<endl;
                return waiting_array_list;
            }
            //cout<<"  waiting_array_list; 4"<<endl;
            for (auto &i: array_list1) {
                if (i.is_using)//不空闲
                {
                    //不需要模块
                    //cout<<"  waiting_array_list; 5"<<endl;
                    if (i.op_type == op_type)
                            pq.push(i);

                }
            }
        }
            break;
        case 2://找SA
        {
            //第一步，判断有没有阵列
            if (array_list2.empty())
            {
                //cout<<"  waiting_array_list; 6"<<endl;
                return waiting_array_list;
            }
            //有sa阵列
            for (auto &i: array_list2) {
                if (i.is_using)//不空闲
                {
                    pq.push(i);
                    //cout<<"  waiting_array_list; 7"<<endl;
                }
                //没有可用的，跳过
            }

        }
            break;
        case 3: {
//第一步，判断有没有阵列
            if (array_list3.empty())
            {
                //cout<<"  waiting_array_list; 8"<<endl;
                return waiting_array_list;
            }
            //有sa阵列
            for (auto &i: array_list3) {
                if (i.is_using)//不空闲
                {
                    pq.push(i);
                    //cout<<"  waiting_array_list; 9"<<endl;
                }
                //没有可用的，跳过
            }

        }
            break;
        default:
            break;
    }

    if (pq.empty()) {
        //cout<<"  waiting_array_list; 10"<<endl;
        return waiting_array_list;
    }

    //cout<<"waiting_array_list pq.size = "<<pq.size()<<endl;
    for (int i = 0; i < pq.size(); ++i) {
        waiting_array_list.push_back(pq.top().array_id);//向量首部是结束时间最小的
        //cout<<"  waiting_array_list; 11"<<endl;
        if (!pq.empty())
        {
            pq.pop();
            //cout<<"  waiting_array_list; 12"<<endl;
        }
    }
    return waiting_array_list;

}


//
// Created by shenlibo on 23-4-19.
//
