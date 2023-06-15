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
    //cout<<"no error decide_array_id op_row_need"<<endl;

    //未使用的阵列和等待的阵列,这里没有
    vector<int> array_no_using = find_no_using(op_type, decide_array_type, array_list1, array_list2, array_list3);
    //cout<<"no error decide_array_id find_no_using"<<endl;

    vector<int> array_wait = waiting_array_list(op_type, decide_array_type, array_list1, array_list2, array_list3);
    //cout<<"no error decide_array_id waiting_array_list"<<endl;

    //检验越界
    //如果都是空的，或者lut的功能不匹配,则需要新建阵列
    if (array_no_using.empty() && array_wait.empty())
    {
        //cout<<"decide_array_id  0"<<endl;
        int new_id=build(decide_array_type, op_type,array_list1,array_list2, array_list3);
        //cout<<"no error decide_array_id build"<<endl;
        array_id.push_back(new_id);
        //cout<<"decide_array_id  1"<<endl;
    }
    //cout<<"decide_array_id  2"<<endl;

    //定义总容量的匿名函数
    auto cap = [&](int array_id) {
        return cap_array_lost(decide_array_type, array_id, array_list1, array_list2, array_list3) +
               cap_array_cover(decide_array_type, array_id, array_list1, array_list2, array_list3);
    };

    if (operand_num == 1)//如果只有一个操作数,基本上执行的都是not操作
    {
        //cout<<"decide_array_id  3"<<endl;
        //看这一个操作数来自哪里
        if (input1_type == -1) {
            //cout<<"decide_array_id  4"<<endl;
            switch (decide_array_type) {
                case 1://LUT NOT
//立即数，找一个能用的即可,或者等待时间最小的，获取等待时间
                {
                    //cout<<"decide_array_id  5"<<endl;
                    if (!array_no_using.empty()) {
                        //cout<<"decide_array_id  6"<<endl;
                        array_id.insert(array_id.end(), array_no_using.begin(), array_no_using.end());//加入备选列表
                        //cout<<"decide_array_id  7"<<endl;
                    }
                    if (!array_wait.empty()) {
                        //cout<<"decide_array_id  8"<<endl;
                        array_id.insert(array_id.end(), array_wait.begin(), array_wait.end());//加入备选列表
                        //cout<<"decide_array_id  9"<<endl;
                    }
                    if (array_id.empty()) //无可用的，新建，加入被选列表
                    {
                        //cout<<"decide_array_id  10"<<endl;
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                        //cout<<"decide_array_id  11"<<endl;
                        array_id.push_back(new_array);
                        //cout<<"decide_array_id  12"<<endl;
                    }

                }
                    break;
                case 2://SA NOT 立即数 考虑容量
                {
                    //cout<<"decide_array_id  13"<<endl;
                    if (!array_no_using.empty()) {
                        //cout<<"decide_array_id  14"<<endl;
                        for (int i=0;i< array_no_using.size();i++) //随便找一个能用的
                        {
                            //cout<<"decide_array_id  15"<<endl;
                            if (cap(array_no_using[i]) >= row_need) {
                                //cout<<"decide_array_id  16"<<endl;
                                array_id.push_back(array_no_using[i]);
                                //cout<<"decide_array_id  17"<<endl;
                                break;
                            }
                        }
                    }
                    if (!array_wait.empty()) {
                        //cout<<"decide_array_id  18"<<endl;
                        for (int i=0;i< array_wait.size();i++) //随便找一个能用的
                        {
                            //cout<<"decide_array_id  19"<<endl;
                            if (cap(array_wait[i]) >= row_need) {
                                //cout<<"decide_array_id  20"<<endl;
                                array_id.push_back(array_wait[i]);
                                //cout<<"decide_array_id  21"<<endl;
                                break;
                            }
                        }
                    }
                    if (array_id.empty())//找不到可用的，新建
                    {
                        //cout<<"decide_array_id  22"<<endl;
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);//新建
                        //cout<<"decide_array_id  23"<<endl;
                        array_id.push_back(new_array);
                        //cout<<"decide_array_id  24"<<endl;
                    }
                }
                    break;
                case 3://MAGIC NOT 立即数
                {
                    //cout<<"decide_array_id  25"<<endl;
                    if (!array_no_using.empty()) {
                        //cout<<"decide_array_id  26"<<endl;
                        for (int i=0;i< array_no_using.size();i++) //随便找一个能用的
                        {
                            //cout<<"decide_array_id  27"<<endl;
                            if (cap(array_no_using[i]) >= row_need + 1) {
                                //cout<<"decide_array_id  28"<<endl;
                                array_id.push_back(array_no_using[i]);
                                //cout<<"decide_array_id  29"<<endl;
                                break;
                            }
                        }
                    }
                    if (!array_wait.empty())//no_using找不到
                    {
                        //cout<<"decide_array_id  30"<<endl;
                        for (int  i=0;i< array_wait.size();i++) {
                            //cout<<"decide_array_id  31"<<endl;
                            if (cap(array_wait[i]) >= row_need + 1) {
                                //cout<<"decide_array_id  32"<<endl;
                                array_id.push_back(array_wait[i]);
                                //cout<<"decide_array_id  33"<<endl;
                                break;
                            }
                        }
                    }
                    if (array_id.empty())//找不到可用的，新建
                    {
                        //cout<<"decide_array_id  34"<<endl;
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);//新建
                        //cout<<"decide_array_id  35"<<endl;
                        array_id.push_back(new_array);
                        //cout<<"decide_array_id  36"<<endl;
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
                        //cout<<"decide_array_id  39"<<endl;
                        array_id.push_back(input1_id);
                        //cout<<"decide_array_id  40"<<endl;
                    }
                    //other 可用阵列
                    if (!array_no_using.empty()) {
                        //cout<<"decide_array_id  41"<<endl;
                        array_id.insert(array_id.end(), array_no_using.begin(), array_no_using.end());//加入备选列表
                        //cout<<"decide_array_id  42"<<endl;
                    }
                    if (!array_wait.empty()) {
                        //cout<<"decide_array_id  43"<<endl;
                        array_id.insert(array_id.end(), array_wait.begin(), array_wait.end());//加入备选列表
                        //cout<<"decide_array_id  44"<<endl;
                    }
                    if (array_id.empty()) //无可用的，新建，加入被选列表
                    {
                        //cout<<"decide_array_id  45"<<endl;
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                        //cout<<"decide_array_id  46"<<endl;
                        array_id.push_back(new_array);
                        //cout<<"decide_array_id  47"<<endl;
                    }

                } else//sa和magic,要保证剩余空间充足
                {
                    //cout<<"decide_array_id  48"<<endl;
                    //只有一个操作数,尽量在操作数阵列中执行,不需要改变容量
                    if (cap(input1_id) >= row_need)
                    {
                        //cout<<"decide_array_id  49"<<endl;
                        array_id.push_back(input1_id);
                        //cout<<"decide_array_id  50"<<endl;
                    }
                    else //other可用阵列，需要改变容量
                    {
                        //cout<<"decide_array_id  51"<<endl;
                        if (!array_no_using.empty()) {
                            //cout<<"decide_array_id  52"<<endl;
                            for (int i=0;i< array_no_using.size();i++) //no_using
                            {
                                //cout<<"decide_array_id  53"<<endl;
                                if (decide_array_type == 2 && cap(array_no_using[i]) >= row_need ||
                                    decide_array_type == 3 && cap(array_no_using[i]) >= row_need + 1) {
                                    //cout<<"decide_array_id  54"<<endl;
                                    array_id.push_back(array_no_using[i]);
                                    //cout<<"decide_array_id  55"<<endl;
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
                        if (array_id.empty())//找不到可用的，新建
                        {
                            //cout<<"decide_array_id  60"<<endl;
                            int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                            //cout<<"decide_array_id  61"<<endl;
                            array_id.push_back(new_array);
                            //cout<<"decide_array_id  62"<<endl;
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
                    if (array_id.empty()) //无可用的，新建，加入被选列表
                    {
                        //cout<<"decide_array_id  68"<<endl;
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                        //cout<<"decide_array_id  69"<<endl;
                        array_id.push_back(new_array);
                        //cout<<"decide_array_id  70"<<endl;
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
                        //cout<<"decide_array_id  76"<<endl;
                        for (int i=0;i< array_wait.size();i++) {
                            //cout<<"decide_array_id 77 "<<endl;
                            if (decide_array_type == 2 && cap(array_wait[i]) >= row_need ||
                                decide_array_type == 3 && cap(array_wait[i]) >= row_need + 1) {
                                //cout<<"decide_array_id 78 "<<endl;
                                array_id.push_back(array_wait[i]);
                                //cout<<"decide_array_id  79"<<endl;
                            }
                        }
                    }
                    if (array_id.empty())//找不到可用的，新建
                    {
                        //cout<<"decide_array_id 80 "<<endl;
                        int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                        //cout<<"decide_array_id  81"<<endl;
                        array_id.push_back(new_array);
                        //cout<<"decide_array_id  82"<<endl;
                    }

                }
            }
        }
    } else//two operands
    {
        //cout<<"decide_array_id 83 "<<endl;
        if (input1_type == 1 && decide_array_type == 1 ||
            input1_type == 4 && decide_array_type == 1 ||
            input1_type == 2 && decide_array_type == 2 ||
            input1_type == 5 && decide_array_type == 2 ||
            input1_type == 6 && decide_array_type == 2 ||
            input1_type == 3 && decide_array_type == 3 )//类型same，看能否在1中运行
        {
            //cout<<"decide_array_id  84"<<endl;
            if (decide_array_type == 1)//LUT,无关，但还是尽量在本节点进行
            {
                //cout<<"decide_array_id 85 "<<endl;
                //1算子支持 或者 算子不支持但是可以加进去
                if (array_list1[input1_id].op_type == op_type)
                {
                    //cout<<"decide_array_id 85 "<<endl;
                    array_id.push_back(input1_id);
                    //cout<<"decide_array_id  86"<<endl;
                }
            } else //SA\magic
            {
                //尽量在本阵列执行，如果容量不够，考虑其他阵列
                if ((decide_array_type == 2 && cap(input1_id) >= row_need) ||
                    (decide_array_type == 3 && cap(input1_id) >= row_need + 1))
                {
                    //cout<<"decide_array_id  87"<<endl;
                    array_id.push_back(input1_id);
                    //cout<<"decide_array_id 88 "<<endl;
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
                    //cout<<"decide_array_id  89"<<endl;
                    array_id.push_back(input2_id);
                    //cout<<"decide_array_id  90"<<endl;
                }
            } else //SA\magic
            {
                //尽量在本阵列执行，如果容量不够，考虑其他阵列
                if ((decide_array_type == 2 && cap(input2_id) >= row_need) ||
                    (decide_array_type == 3 && cap(input2_id) >= row_need + 1))
                {
                    //cout<<"decide_array_id  91"<<endl;
                    array_id.push_back(input2_id);
                    //cout<<"decide_array_id  92"<<endl;
                }

            }

        }
        //other array not1 not2 找个能用的即可
        if (decide_array_type == 1) {
            if (!array_no_using.empty()) {
                //cout<<"decide_array_id  93"<<endl;
                array_id.insert(array_id.end(), array_no_using.begin(), array_no_using.end());//加入备选列表
                //cout<<"decide_array_id  94"<<endl;
            }
            if (!array_wait.empty()) {
                //cout<<"decide_array_id  95"<<endl;
                array_id.insert(array_id.end(), array_wait.begin(), array_wait.end());//加入备选列表
                //cout<<"decide_array_id  96"<<endl;
            }
            if (array_id.empty()) //无可用的，新建，加入被选列表
            {
                //cout<<"decide_array_id  97"<<endl;
                int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                //cout<<"decide_array_id  98"<<endl;
                array_id.push_back(new_array);
                //cout<<"decide_array_id  99"<<endl;
            }

        } else//sa和magic,要保证剩余空间充足
        {
            //array no using is not empty
            if (!array_no_using.empty()) {
                //cout<<"decide_array_id  100"<<endl;
                for (int i=0;i< array_no_using.size();i++) //随便找一个能用的
                {
                    //cout<<"decide_array_id 101 "<<endl;
                    if ((decide_array_type == 2 && cap(array_no_using[i]) >= row_need + 1) ||
                        (decide_array_type == 3 && cap(array_no_using[i]) >= row_need + 2)) {
                        //cout<<"decide_array_id  102"<<endl;
                        array_id.push_back(array_no_using[i]);
                        //cout<<"decide_array_id  103"<<endl;
                    }
                }
            }
            if (!array_wait.empty()) {
                //cout<<"decide_array_id 104 "<<endl;
                for (int i=0;i< array_wait.size();i++) //随便找一个能用的
                {
                    //cout<<"decide_array_id  105"<<endl;
                    if ((decide_array_type == 2 && cap(array_wait[i]) >= row_need + 1) ||
                        (decide_array_type == 3 && cap(array_wait[i]) >= row_need + 2)) {
                        //cout<<"decide_array_id 106 "<<endl;
                        array_id.push_back(array_wait[i]);
                        //cout<<"decide_array_id 107 "<<endl;
                    }
                }
            }
            //1.empty no using 2.no cap is enough
            if (array_id.empty()) {
                //cout<<"decide_array_id  108"<<endl;
                int new_array = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                //cout<<"decide_array_id  109"<<endl;
                array_id.push_back(new_array);
                //cout<<"decide_array_id  110"<<endl;
            }
        }
    }

    if (array_no_using.empty())
    {
        int dse_build=rand()%5;
        if (dse_build==0)
        {
            //cout<<"decide_array_id  111"<<endl;
            int new_id=build(decide_array_type, op_type,array_list1,array_list2, array_list3);
            //cout<<"decide_array_id  112"<<endl;
            array_id.push_back(new_id);
            //cout<<"decide_array_id  113"<<endl;
            decide_array_id =new_id;
            //cout<<"decide_array_id  114"<<endl;
        }else
        {
            //cout<<"decide_array_id  115"<<endl;
            decide_array_id = array_id[rand() % array_id.size()];//使用随机数
            //cout<<"decide_array_id 116 "<<endl;
        }

    }
    else
    {
        //cout<<"decide_array_id 117 "<<endl;
        decide_array_id = array_id[rand() % array_id.size()];//使用随机数
        //cout<<"decide_array_id 118 "<<endl;
    }


    return decide_array_id;
}


//
// Created by shenlibo on 23-4-19.
//
