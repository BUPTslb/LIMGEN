#include "logic_func.h"

//先验方式决定？
//TODO：DSE,决定阵列的执行类型
int decide_array_type(int op_type, int design_target) {
    int decide_array_type = 0;
    if (1 <= op_type && op_type <= 3 || op_type == 12 || op_type == 13) {
        decide_array_type = 1;//比较，乘法,除法 直接用lut
    }
    return decide_array_type;
}

//定义执行阵列的id
//这里的id 是开始id
int decide_array_id(int op_type, Node *node_now, vector<Node> &nodes, int decide_array_type, \
                    vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3, \
                    int input1_type, int input1_id, int input2_type, int input2_id) {
/*当前的寻找策略
 * 如果操作数存放位置与执行阵列的类型相同，就在此执行
 * 否则找一个同类型能用的执行
 * 如果暂时都在使用，则新建一个阵列
 * 方案二：等待阵列执行完毕，寻找一个阵列执行，这样可以提供好面积利用率，待补充
 * */
    double time = time_now(op_type, array_list1, array_list2, array_list3, node_now);//update is_using of array_list
    vector<int> array_id;

    int row_need = op_row_need(op_type, decide_array_type, node_now);//执行当前操作需要的资源数,此时假设操作数都在阵列中

    vector<int> array_no_using = find_no_using(op_type, nodes, decide_array_type, array_list1, array_list2,
                                               array_list3);
    vector<int> array_wait = waiting_array_list(op_type, nodes, decide_array_type, array_list1, array_list2,
                                                array_list3);
    //定义总容量的匿名函数
    auto cap = [&](int array_id) {
        return cap_array_lost(decide_array_type, array_id, nodes, array_list1, array_list2,
                              array_list3) +
               cap_array_cover(decide_array_type, array_id, nodes, array_list1, array_list2,
                               array_list3);
    };

    if (input2_type == 0)//如果只有一个操作数,基本上执行的都是not操作
    {
        if (input1_type == -1) {
            switch (decide_array_type) {
                case 1://LUT NOT
//立即数，找一个能用的即可,或者等待时间最小的，获取等待时间
                {
                    if (!array_no_using.empty()) {
                        array_id.insert(array_id.end(), array_no_using.begin(), array_no_using.end());//加入备选列表
                    }
                    if (!array_wait.empty()) {
                        array_id.insert(array_id.end(), array_wait.begin(), array_wait.end());//加入备选列表
                    }
                    if (array_id.empty()) //无可用的，新建，加入被选列表
                    {
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                        array_id.push_back(new_array);
                    }

                }
                    break;
                case 2://SA NOT 立即数 考虑容量
                {
                    if (!array_no_using.empty()) {
                        for (int i: array_no_using) //随便找一个能用的
                        {
                            if (cap(i) > row_need) {
                                array_id.push_back(i);
                                break;
                            }
                        }

                    }
                    if (!array_wait.empty()) {
                        for (int i: array_wait) //随便找一个能用的
                        {
                            if (cap(i) > row_need) {
                                array_id.push_back(i);
                                break;
                            }
                        }

                    }
                    if (array_id.empty())//找不到可用的，新建
                    {
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);//新建
                        array_id.push_back(new_array);
                    }
                }
                    break;
                case 3://MAGIC NOT 立即数
                {
                    if (!array_no_using.empty()) {
                        for (int i: array_no_using) //随便找一个能用的
                        {
                            if (cap(i) > row_need + 1) {
                                array_id.push_back(i);
                                break;
                            }
                        }
                    }
                    if (!array_wait.empty())//no_using找不到
                    {
                        for (auto i: array_wait) {
                            if (cap(i) > row_need + 1) {
                                array_id.push_back(i);
                                break;
                            }
                        }
                    }
                    if (array_id.empty())//找不到可用的，新建
                    {
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);//新建
                        array_id.push_back(new_array);
                    }
                }
                    break;
                default:
                    break;

            }
        } else//不是立即数
        {
            if (input1_type == decide_array_type)//类型same
            {
                //如果是lut,还是需要判断,尽量在本阵列执行
                if (decide_array_type == 1) {
                    if (array_list1[input1_id].op_type.find(op_type) != array_list1[input1_id].op_type.end()) //算子支持
                        array_id.push_back(input1_id);
                    //other 可用阵列
                    if (!array_no_using.empty()) {
                        array_id.insert(array_id.end(), array_no_using.begin(), array_no_using.end());//加入备选列表
                    }
                    if (!array_wait.empty()) {
                        array_id.insert(array_id.end(), array_wait.begin(), array_wait.end());//加入备选列表
                    }
                    if (array_id.empty()) //无可用的，新建，加入被选列表
                    {
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                        array_id.push_back(new_array);
                    }

                } else//sa和magic,要保证剩余空间充足
                {
                    //只有一个操作数,尽量在操作数阵列中执行,不需要改变容量
                    if (cap(input1_id) >= row_need)
                        array_id.push_back(input1_id);
                    else //other可用阵列，需要改变容量
                    {
                        if (!array_no_using.empty()) {
                            for (int i: array_no_using) //no_using
                            {
                                if (decide_array_type == 2 && cap(i) >= row_need ||
                                    decide_array_type == 3 && cap(i) >= row_need + 1) {
                                    array_id.push_back(i);
                                }
                            }
                        }
                        if (!array_wait.empty())//wait_list
                        {
                            for (int i: array_wait) {
                                if (decide_array_type == 2 && cap(i) >= row_need ||
                                    decide_array_type == 3 && cap(i) >= row_need + 1) {
                                    array_id.push_back(i);
                                }
                            }
                        }
                        if (array_id.empty())//找不到可用的，新建
                        {
                            int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                            array_id.push_back(new_array);
                        }
                    }

                }
            } else//类型不同，找一个能用的，ma需要改变容量
            {
                if (decide_array_type == 1) {
                    //other 可用阵列
                    if (!array_no_using.empty()) {
                        array_id.insert(array_id.end(), array_no_using.begin(), array_no_using.end());//加入备选列表
                    }
                    if (!array_wait.empty()) {
                        array_id.insert(array_id.end(), array_wait.begin(), array_wait.end());//加入备选列表
                    }
                    if (array_id.empty()) //无可用的，新建，加入被选列表
                    {
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                        array_id.push_back(new_array);
                    }
                } else//sa和magic,要保证剩余空间充足
                {
                    if (!array_no_using.empty()) {
                        for (int i: array_no_using) //no_using
                        {
                            if (decide_array_type == 2 && cap(i) >= row_need ||
                                decide_array_type == 3 && cap(i) >= row_need + 1) {
                                array_id.push_back(i);
                            }
                        }
                    }
                    if (!array_wait.empty())//wait_list
                    {
                        for (int i: array_wait) {
                            if (decide_array_type == 2 && cap(i) >= row_need ||
                                decide_array_type == 3 && cap(i) >= row_need + 1) {
                                array_id.push_back(i);
                            }
                        }
                    }
                    if (array_id.empty())//找不到可用的，新建
                    {
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                        array_id.push_back(new_array);
                    }

                }
            }
        }
    } else//two operands
    {
        if (input1_type == decide_array_type)//看能否在1中运行
        {
            if (decide_array_type == 1)//LUT,无关，但还是尽量在本节点进行
            {
                if (array_list1[input1_id].op_type.find(op_type) != array_list1[input1_id].op_type.end())//1算子支持
                    array_id.push_back(input1_id);
            } else //SA\magic
            {
                //尽量在本阵列执行，如果容量不够，考虑其他阵列
                if ((decide_array_type == 2 && cap(input1_id) >= row_need) ||
                    (decide_array_type == 3 && cap(input1_id) >= row_need + 1))
                    array_id.push_back(input1_id);

            }
        }
        if (input2_type == decide_array_type) //看能否在2中运行
        {
            if (decide_array_type == 1)//LUT,无关，但还是尽量在本节点进行
            {
                if (array_list1[input2_id].op_type.find(op_type) != array_list1[input2_id].op_type.end())//2算子支持
                    array_id.push_back(input2_id);
            } else //SA\magic
            {
                //尽量在本阵列执行，如果容量不够，考虑其他阵列
                if ((decide_array_type == 2 && cap(input2_id) >= row_need) ||
                    (decide_array_type == 3 && cap(input2_id) >= row_need + 1))
                    array_id.push_back(input2_id);

            }

        }
        //other array not1 not2
        if (decide_array_type == 1) {
            if (!array_no_using.empty()) {
                array_id.insert(array_id.end(), array_no_using.begin(), array_no_using.end());//加入备选列表
            }
            if (!array_wait.empty()) {
                array_id.insert(array_id.end(), array_wait.begin(), array_wait.end());//加入备选列表
            }
            if (array_id.empty()) //无可用的，新建，加入被选列表
            {
                int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                array_id.push_back(new_array);
            }

        } else//sa和magic,要保证剩余空间充足
        {
            //array no using is not empty
            if (!array_no_using.empty()) {
                for (int i: array_no_using) //随便找一个能用的
                {
                    if ((decide_array_type == 2 && cap(i) >= row_need + 1) ||
                        (decide_array_type == 3 && cap(i) >= row_need + 2)) {
                        array_id.push_back(i);
                    }
                }
            }
            if (!array_wait.empty()) {
                for (int i: array_wait) //随便找一个能用的
                {
                    if ((decide_array_type == 2 && cap(i) >= row_need + 1) ||
                        (decide_array_type == 3 && cap(i) >= row_need + 2)) {
                        array_id.push_back(i);
                    }
                }
            }
            //1.empty no using 2.no cap is enough
            if (array_id.empty()) {
                int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                array_id.push_back(new_array);
            }

        }

    }

    //TODO:DSE,find a id from array_id [front > back]
    int decide_array_id = array_id.front();

    if (decide_array_type == 1) {
        //给lut添加功能
        if (array_list1[decide_array_id].op_type.find(op_type) == array_list1[decide_array_id].op_type.end())
            array_list1[decide_array_id].op_type.insert(op_type);
    }


    time_update(op_type, decide_array_type, decide_array_id, time, node_now, array_list1, array_list2, array_list3);
    return decide_array_id;
}


//
// Created by shenlibo on 23-4-19.
//
