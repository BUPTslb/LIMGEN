#include "logic_func.h"

//先验方式决定？
int decide_array_type(int op_type, int design_target) {
    unsigned seed = time(0);
    //cout<<"in decide_array_type 1"<<endl;
    srand(seed);
    //cout<<"in decide_array_type 2"<<endl;

    int dse = rand() % 3;
    //cout<<"in decide_array_type 3"<<endl;

    int type_list[3] = {1, 2, 3};
    //cout<<"in decide_array_type 4"<<endl;

    int decide_array_type = type_list[dse];
    //cout<<"in decide_array_type 5"<<endl;

    if (1 <= op_type && op_type <= 3 || op_type == 12 || op_type == 13) {
        decide_array_type = 1;//比较，乘法,除法 直接用lut
        //cout<<"in decide_array_type 6"<<endl;

    }
    //cout<<"in decide_array_type 7"<<endl;
    //cout<<"decide_array_type = "<<decide_array_type<<endl;
    return decide_array_type;
}

//定义执行阵列的id
//这里的id 是开始id
int decide_array_id(int op_type, Nodes *node_now, int decide_array_type, \
                    vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3, \
                    int input1_type, int input1_id, int input2_type, int input2_id) {
/*当前的寻找策略
 * 如果操作数存放位置与执行阵列的类型相同，就在此执行
 * 否则找一个同类型能用的执行
 * 如果暂时都在使用，则新建一个阵列
 * 方案二：等待阵列执行完毕，寻找一个阵列执行，这样可以提供好面积利用率，待补充
 * 附加策略：写均衡
 *      如果可选的两个阵列与其他阵列相比写次数过多了，就选择写次数少的
 *      按照一定的概率新建阵列
 * */
    int operand_num = 2;
    if (op_type == 0 || op_type == 8) {
        operand_num = 1;
    }
    int decide_array_id;
    //可以选择的阵列
    vector<int> array_id;
    //执行当前操作需要的资源数,此时假设操作数都在阵列中
    int row_need = op_row_need(op_type, decide_array_type);//后续要更改

    //未使用的阵列和等待的阵列,这里没有
    vector<int> array_no_using = find_no_using(op_type, decide_array_type, array_list1, array_list2, array_list3);

    vector<int> array_wait = waiting_array_list(op_type, decide_array_type, array_list1, array_list2, array_list3);

    //如果都是空的，或者lut的功能不匹配,则需要新建阵列
    if (array_no_using.empty() && array_wait.empty())
    {
        int new_id=build(decide_array_type, op_type,array_list1,array_list2, array_list3);
        return new_id;
    }

    //定义总容量的匿名函数
    auto cap = [&](int array_id) {
        return cap_array_lost(decide_array_type, array_id, array_list1, array_list2, array_list3) +
               cap_array_cover(decide_array_type, array_id, array_list1, array_list2, array_list3);
    };

    if (operand_num == 1)//如果只有一个操作数,基本上执行的都是not操作
    {
        //看这一个操作数来自哪里
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
                        return new_array;
                    }

                }
                    break;
                case 2://SA NOT 立即数 考虑容量
                {
                    if (!array_no_using.empty()) {
                        for (int i=0;i< array_no_using.size();i++) //随便找一个能用的
                        {
                            if (cap(array_no_using[i]) >= row_need) {
                                array_id.push_back(array_no_using[i]);
                                break;
                            }
                        }
                    }
                    if (!array_wait.empty()) {
                        for (int i=0;i< array_wait.size();i++) //随便找一个能用的
                        {
                            if (cap(array_wait[i]) >= row_need) {
                                array_id.push_back(array_wait[i]);
                                break;
                            }
                        }
                    }
                    if (array_id.empty())//找不到可用的，新建
                    {
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);//新建
                        return new_array;
                    }
                }
                    break;
                case 3://MAGIC NOT 立即数
                {
                    if (!array_no_using.empty()) {
                        for (int i=0;i< array_no_using.size();i++) //随便找一个能用的
                        {
                            if (cap(array_no_using[i]) >= row_need + 1) {
                                array_id.push_back(array_no_using[i]);
                                break;
                            }
                        }
                    }
                    if (!array_wait.empty())//no_using找不到
                    {
                        for (int  i=0;i< array_wait.size();i++) {
                            if (cap(array_wait[i]) >= row_need + 1) {
                                array_id.push_back(array_wait[i]);
                                break;
                            }
                        }
                    }
                    if (array_id.empty())//找不到可用的，新建
                    {
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);//新建
                        return new_array;
                    }
                }
                    break;
                default:
                    break;

            }
        } else//来自阵列 A= NOT B
        {
            if (input1_type == 1 && decide_array_type == 1 ||
                input1_type == 4 && decide_array_type == 1 ||
                input1_type == 2 && decide_array_type == 2 ||
                input1_type == 5 && decide_array_type == 2 ||
                input1_type == 6 && decide_array_type == 2 ||
                input1_type == 3 && decide_array_type == 3 )//类型same
            {
                //cout<<"decide_array_id  37"<<endl;
                //如果是lut,还是需要判断,尽量在本阵列执行
                if (decide_array_type == 1) {
                    //cout<<"decide_array_id  38"<<endl;
//                    if (array_list1[input1_id].op_type.find(op_type) != array_list1[input1_id].op_type.end()) //算子支持
                    if (array_list1[input1_id].op_type == op_type) //算子支持
                    {
                        array_id.push_back(input1_id);
                    }
                    //other 可用阵列
                    if (!array_no_using.empty()) {
                        array_id.insert(array_id.end(), array_no_using.begin(), array_no_using.end());//加入备选列表
                    }
                    if (!array_wait.empty()) {
                        array_id.insert(array_id.end(), array_wait.begin(), array_wait.end());//加入备选列表
                    }

                } else//sa和magic,要保证剩余空间充足
                {
                    //只有一个操作数,尽量在操作数阵列中执行,不需要改变容量
                    if (cap(input1_id) >= row_need)
                    {
                        array_id.push_back(input1_id);
                    }
                    else //other可用阵列，需要改变容量
                    {
                        if (!array_no_using.empty()) {
                            for (int i=0;i< array_no_using.size();i++) //no_using
                            {
                                if (decide_array_type == 2 && cap(array_no_using[i]) >= row_need ||
                                    decide_array_type == 3 && cap(array_no_using[i]) >= row_need + 1) {
                                    array_id.push_back(array_no_using[i]);
                                }
                            }
                        }
                        if (!array_wait.empty())//wait_list
                        {
                            //cout<<"decide_array_id  56"<<endl;
                            for (int i=0;i< array_wait.size();i++) {
                                //cout<<"decide_array_id  57"<<endl;
                                if (decide_array_type == 2 && cap(array_wait[i]) >= row_need ||
                                    decide_array_type == 3 && cap(array_wait[i]) >= row_need + 1) {
                                    //cout<<"decide_array_id  58"<<endl;
                                    array_id.push_back(array_wait[i]);
                                    //cout<<"decide_array_id  59"<<endl;
                                }
                            }
                        }
                    }

                }
            } else//类型不同，找一个能用的，ma需要改变容量
            {
                //cout<<"decide_array_id  63"<<endl;
                if (decide_array_type == 1) {
                    //other 可用阵列
                    if (!array_no_using.empty()) {
                        //cout<<"decide_array_id  64"<<endl;
                        array_id.insert(array_id.end(), array_no_using.begin(), array_no_using.end());//加入备选列表
                        //cout<<"decide_array_id  65"<<endl;
                    }
                    if (!array_wait.empty()) {
                        //cout<<"decide_array_id  66"<<endl;
                        array_id.insert(array_id.end(), array_wait.begin(), array_wait.end());//加入备选列表
                        //cout<<"decide_array_id  67"<<endl;
                    }
                } else//sa和magic,要保证剩余空间充足
                {
                    //cout<<"decide_array_id  71"<<endl;
                    if (!array_no_using.empty()) {
                        //cout<<"decide_array_id  72"<<endl;
                        for (int i=0;i< array_no_using.size();i++) //no_using
                        {
                            //cout<<"decide_array_id 73 "<<endl;
                            if (decide_array_type == 2 && cap(array_no_using[i]) >= row_need ||
                                decide_array_type == 3 && cap(array_no_using[i]) >= row_need + 1) {
                                //cout<<"decide_array_id  74"<<endl;
                                array_id.push_back(array_no_using[i]);
                                //cout<<"decide_array_id  75"<<endl;
                            }
                        }
                    }
                    if (!array_wait.empty())//wait_list
                    {
                        for (int i=0;i< array_wait.size();i++) {
                            if (decide_array_type == 2 && cap(array_wait[i]) >= row_need ||
                                decide_array_type == 3 && cap(array_wait[i]) >= row_need + 1) {
                                array_id.push_back(array_wait[i]);
                            }
                        }
                    }

                }
            }
        }
    } else//two operands
    {
        if (input1_type == 1 && decide_array_type == 1 ||
            input1_type == 4 && decide_array_type == 1 ||
            input1_type == 2 && decide_array_type == 2 ||
            input1_type == 5 && decide_array_type == 2 ||
            input1_type == 6 && decide_array_type == 2 ||
            input1_type == 3 && decide_array_type == 3 )//类型same，看能否在1中运行
        {
            if (decide_array_type == 1)//LUT,无关，但还是尽量在本节点进行
            {
                //1算子支持 或者 算子不支持但是可以加进去
                if (array_list1[input1_id].op_type == op_type)
                {
                    array_id.push_back(input1_id);
                }
            } else //SA\magic
            {
                //尽量在本阵列执行，如果容量不够，考虑其他阵列
                if ((decide_array_type == 2 && cap(input1_id) >= row_need) ||
                    (decide_array_type == 3 && cap(input1_id) >= row_need + 1))
                {
                    array_id.push_back(input1_id);
                }
            }
        }
        if (input2_type == 1 && decide_array_type == 1 ||
            input2_type == 4 && decide_array_type == 1 ||
            input2_type == 2 && decide_array_type == 2 ||
            input2_type == 5 && decide_array_type == 2 ||
            input2_type == 6 && decide_array_type == 2 ||
            input2_type == 3 && decide_array_type == 3 )//类型same//看能否在2中运行
        {
            if (decide_array_type == 1)//LUT,无关，但还是尽量在本节点进行
            {
                if (array_list1[input2_id].op_type == op_type)//2算子支持
                {
                    array_id.push_back(input2_id);
                }
            } else //SA\magic
            {
                //尽量在本阵列执行，如果容量不够，考虑其他阵列
                if ((decide_array_type == 2 && cap(input2_id) >= row_need) ||
                    (decide_array_type == 3 && cap(input2_id) >= row_need + 1))
                {
                    array_id.push_back(input2_id);
                }

            }

        }
        //other array not1 not2 找个能用的即可
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
                return new_array;
            }

        } else//sa和magic,要保证剩余空间充足
        {
            //array no using is not empty
            if (!array_no_using.empty()) {
                for (int i=0;i< array_no_using.size();i++) //随便找一个能用的
                {
                    if ((decide_array_type == 2 && cap(array_no_using[i]) >= row_need + 1) ||
                        (decide_array_type == 3 && cap(array_no_using[i]) >= row_need + 2)) {
                        array_id.push_back(array_no_using[i]);
                    }
                }
            }
            if (!array_wait.empty()) {
                for (int i=0;i< array_wait.size();i++) //随便找一个能用的
                {
                    if ((decide_array_type == 2 && cap(array_wait[i]) >= row_need + 1) ||
                        (decide_array_type == 3 && cap(array_wait[i]) >= row_need + 2)) {
                        array_id.push_back(array_wait[i]);
                    }
                }
            }
            //1.empty no using 2.no cap is enough
            if (array_id.empty()) {
                int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                return new_array;
            }
        }
    }

    for (auto i:array_list2) {
        cout<< "array_list2:" << i.array_id<<" write_num: "<<i.write_number << endl;
    }
    for (auto i:array_list3) {
        cout<< "array_list3:" << i.array_id<<" write_num: "<<i.write_number << endl;
    }
    cout<<"over id"<<endl;

    if (array_no_using.empty())
    {
        int dse_build=rand()%5;//0.2的概率新建
        if (dse_build==0)
        {
            int new_id=build(decide_array_type, op_type,array_list1,array_list2, array_list3);
            return new_id;
        }else
        {
            decide_array_id = array_id[rand() % array_id.size()];//使用随机数
        }

    }
    else
    {
        decide_array_id = array_id[rand() % array_id.size()];//使用随机数
    }


    return decide_array_id;
}


//
// Created by shenlibo on 23-4-19.
//
