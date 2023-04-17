#include"logic_func.h"

using namespace std;

unsigned int arr_size(int logic_type, unsigned int bit_num_operand) {
    unsigned int arr_size;
    switch (logic_type) {
        case 1://lut,只有四输入和六输入
            arr_size = (2 * bit_num_operand <= 16) ? 16 : 64;
            break;
        case 2://sa,列数=位数
        {
            unsigned m = 1;
            while (m < bit_num_operand) {
                m <<= 1;
            }
            arr_size = (m <= 64) ? m : 64;

        }
            break;
        case 3://尽量等于2的幂
        {
            unsigned m = 1;
            while (m < bit_num_operand) {
                m <<= 1;
            }
            arr_size = (m <= 64) ? m : 64;

        }
            break;
        default:
            break;

    }
    return arr_size;
}


//现在已经决定了从哪里找操作数，在哪里执行，可以直接进行时间更新
double time_now(int op_type, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                   vector<magic_arr> &array_list3, Node *node_now) {
    //函数执行的操作：
    //执行阵列：更新开始时间和结束时间，将使用的阵列置为is_using=true
    //其他阵列，如果当前执行阵列的开始时间比其结束时间要大,将is_using置为false
    double time_now=0;
    if (node_now->depend1)
    {
        double time1=0;
        if (node_now->depend1->do_type!=-1)
            time1=node_now->depend1->end_time;
        time_now=time1;
    }
    if (node_now->depend2)
    {
        double time2=0;
        if (node_now->depend2->do_type!=-1)
            time2=node_now->depend2->end_time;
        time_now= max(time_now,time2);
    }
    if (node_now->control)
    {
        double time3=node_now->control->end_time;
        time_now= max(time_now,time3);
    }
    //update the is_using of array
    for (auto i:array_list1) {
        if (i.over_time<time_now)
            i.is_using= false;
    }
    for (auto i:array_list2) {
        if (i.over_time<time_now)
            i.is_using= false;
    }
    for (auto i:array_list3) {
        if (i.over_time<time_now)
            i.is_using= false;
    }

    return time_now;

}

void time_update(int op_type,int decide_array_type, int decide_array_id, double time_now,Node* node_now,
                 vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3, int bit_num)
{
    //set start time of node
    node_now->start_time=time_now;
    if (decide_array_id==-1&&decide_array_type==-1)   {
        node_now->end_time=time_now+1;
        return;
    }
    switch (decide_array_type) {
        case 1:
        {
            array_list1[decide_array_id].start_time=time_now;
            if (op_type==0||(op_type>=5&&op_type<=9))
                node_now->end_time=time_now+1;
            if (op_type==1||op_type==2)
                node_now->end_time= time_now+1;
            if (op_type==3||op_type==4)
                node_now->end_time=time_now+bit_num;
            if (op_type==10)
                node_now->end_time=time_now+bit_num*2+4;
            if (op_type==11)
                node_now->end_time=time_now+(bit_num*2+4)*bit_num;

            array_list1[decide_array_id].over_time=node_now->end_time;
            //set is_using
            array_list1[decide_array_id].is_using=true;
        }
            break;
        case 2://SA
        {
            array_list2[decide_array_id].start_time=time_now;
            if (op_type==0||op_type==5||op_type==6||op_type==7||op_type==9)
            {
                node_now->end_time=time_now+1;
//                cout<<"node : "<<node_now->node_id<<" end time ="<<node_now->end_time<<endl;
            }
            if (op_type==1||op_type==2||op_type==3||op_type==4)
                node_now->end_time=time_now+bit_num;
            if (op_type==8)//nor
                node_now->end_time=time_now+2;
            if (op_type==10)
                node_now->end_time=time_now+bit_num*2+4;
            if (op_type==11)
                node_now->end_time=time_now+(bit_num*2+4)*bit_num;

            array_list2[decide_array_id].over_time=node_now->end_time;
            //set is_using
            array_list2[decide_array_id].is_using=true;
        }
            break;
        case 3://MAGIC
        {
            array_list2[decide_array_id].start_time=time_now;
            if (op_type==0||op_type==6||op_type==7||op_type==8)
                node_now->end_time=time_now+1;
            if (op_type==5||op_type==9)
                node_now->end_time=time_now+2;
            if (op_type==10)
                node_now->end_time=time_now+bit_num*2+4;
            if (op_type==11)
                node_now->end_time=time_now+(bit_num*2+4)*bit_num;

            array_list3[decide_array_id].over_time=node_now->end_time;
            //set is_using
            array_list3[decide_array_id].is_using=true;
        }
            break;
        default:
            break;

    }
}

//寻找输入操作数来源：阵列（ID）/寄存器
void find_input(int &array_type, int &array_id, int op_type, Node *node_depend, int cycle) {
    if (node_depend == nullptr || node_depend->do_type == -1)//立即数,或者（A=立即数）
    {
        if (op_type == 0 || op_type == 7)//  one operand : = not
            return;
        else //two operand
        {
            array_type = -1;
            return;
        }
    }
    //depend is not null:
    //应该还有其他情况，若写回了多个阵列，应该以现在能用的或者数据搬移最小的为准，从wb_pos中挑选一个
    if (node_depend->do_type == 1 || node_depend->do_type == 4)  //LUT LUT-OUT
    {
        array_type = 1;
        array_id = node_depend->finish_id;


    } else if (node_depend->do_type == 2 || node_depend->do_type == 5) //SA SA-BUFFER
    {
        array_type = 2;
        array_id = node_depend->finish_id;
    } else //MAGIC
    {
        array_type = 3;
        array_id = node_depend->finish_id;
//        array_id=node_depend->wb_pos[2].back();
    }

}

//先验方式决定？
//TODO：DSE,决定阵列的执行类型
int decide_array_type(int op_type, int design_target) {
    int decide_array_type = 0;
    if (1 <= op_type && op_type <= 3 || op_type == 12 || op_type == 13) {
        decide_array_type = 1;//比较，乘法,除法 直接用lut
    }
//    else
//    {
//        decide_array_type=2;
//    }
    //DSE决定

    return decide_array_type;
}

//定义执行阵列的id
//这里的id 是开始id
int decide_array_id(int op_type, int bit_num_operand, Node* node_now,vector<Node> &nodes, int decide_array_type, \
                    vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3, \
                    int input1_type, int input1_id, int input2_type, int input2_id) {
/*当前的寻找策略
 * 如果操作数存放位置与执行阵列的类型相同，就在此执行
 * 否则找一个同类型能用的执行
 * 如果暂时都在使用，则新建一个阵列
 * 方案二：等待阵列执行完毕，寻找一个阵列执行，这样可以提供好面积利用率，待补充
 * */
    double time=time_now(op_type,array_list1,array_list2,array_list3,node_now);//update is_using of array_list
    int decide_array_id = -1;
    int row_need = op_row_need(op_type, decide_array_type, bit_num_operand);//执行当前操作需要的资源数
    vector<int> array_no_using = find_no_using(op_type, nodes, decide_array_type, array_list1, array_list2,
                                               array_list3);
    vector<int> array_wait = waiting_array_list(op_type, nodes, decide_array_type, array_list1, array_list2,
                                                array_list3);
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
                    if (!array_no_using.empty())
                        decide_array_id = array_no_using.front();//不等待，存在DSE,寻找哪个阵列最好
                    else if (!array_wait.empty())
                        decide_array_id = array_wait.front();//等待，存在DSE,寻找哪个阵列最好
                    else
                        decide_array_id = build(decide_array_type, op_type, array_list1, array_list2, array_list3);//新建
                }
                    break;
                case 2://SA NOT
                {
                    if (!array_no_using.empty()) {
                        for (int i = 0; i < array_no_using.size(); ++i) //随便找一个能用的
                        {
                            if (cap(array_no_using[i]) > row_need) {
                                decide_array_id = array_no_using[i];//不等待，存在DSE,寻找哪个阵列最好
                                break;
                            }
                        }

                    }
                    if (decide_array_id == -1)//no_using找不到
                    {
                        for (int i = 0; i < array_wait.size(); ++i) {
                            if (cap(array_no_using[i]) > row_need) {
                                decide_array_id = array_no_using[i];//不等待，存在DSE,寻找哪个阵列最好
                                break;
                            }
                        }
                    }
                    if (decide_array_id == -1)//找不到可用的，新建
                    {
                        decide_array_id = build(decide_array_type, op_type, array_list1, array_list2, array_list3);//新建
                    }
                }
                    break;
                case 3://MAGIC NOT
                {
                    if (!array_no_using.empty()) {
                        for (int i = 0; i < array_no_using.size(); ++i) //随便找一个能用的
                        {
                            if (cap(array_no_using[i]) > row_need) {
                                decide_array_id = array_no_using[i];//不等待，存在DSE,寻找哪个阵列最好
                                break;
                            }
                        }
                    }
                    if (decide_array_id == -1)//no_using找不到
                    {
                        for (int i = 0; i < array_wait.size(); ++i) {
                            if (cap(array_no_using[i]) > row_need) {
                                decide_array_id = array_no_using[i];//不等待，存在DSE,寻找哪个阵列最好
                                break;
                            }
                        }
                    }
                    if (decide_array_id == -1)//找不到可用的，新建
                    {
                        decide_array_id = build(decide_array_type, op_type, array_list1, array_list2, array_list3);//新建
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
                        return input1_id;
                        //算子不支持：寻找可用阵列
                    else {
                        if (!array_no_using.empty())
                            decide_array_id = array_no_using.front();//不等待，存在DSE,寻找哪个阵列最好
                        else if (!array_wait.empty())
                            decide_array_id = array_wait.front();//等待，存在DSE,寻找哪个阵列最好
                        else
                            decide_array_id = build(decide_array_type, op_type, array_list1, array_list2,
                                                    array_list3);//新建
                    }

                } else//sa和magic,要保证剩余空间充足
                {
                    //只有一个操作数
                    if ((decide_array_type == 2 && cap(input1_id) > row_need) ||
                        (decide_array_type == 3 && cap(input1_id) > row_need))
                        return input1_id;
                    else //空间不够，寻找可用阵列
                    {
                        if (!array_no_using.empty()) {
                            for (int i = 0; i < array_no_using.size(); ++i) //随便找一个能用的
                            {
                                if (cap(array_no_using[i]) > row_need) {
                                    decide_array_id = array_no_using[i];//不等待，存在DSE,寻找哪个阵列最好
                                    break;
                                }
                            }
                        }
                        if (decide_array_id == -1)//no_using找不到
                        {
                            for (int i = 0; i < array_wait.size(); ++i) {
                                if (cap(array_no_using[i]) > row_need) {
                                    decide_array_id = array_no_using[i];//不等待，存在DSE,寻找哪个阵列最好
                                    break;
                                }
                            }
                        }
                        if (decide_array_id == -1)//找不到可用的，新建
                        {
                            decide_array_id = build(decide_array_type, op_type, array_list1, array_list2,
                                                    array_list3);//新建
                        }
                    }

                }
            } else//类型不同，找一个能用的
            {
                if (decide_array_type == 1) {
                    if (!array_no_using.empty())
                        decide_array_id = array_no_using.front();//不等待，存在DSE,寻找哪个阵列最好
                    else if (!array_wait.empty())
                        decide_array_id = array_wait.front();//等待，存在DSE,寻找哪个阵列最好
                    else
                        decide_array_id = build(decide_array_type, op_type, array_list1, array_list2, array_list3);//新建

                } else//sa和magic,要保证剩余空间充足
                {
                    if (!array_no_using.empty()) {
                        for (int i = 0; i < array_no_using.size(); ++i) //随便找一个能用的
                        {
                            if (cap(array_no_using[i]) > row_need) {
                                decide_array_id = array_no_using[i];//不等待，存在DSE,寻找哪个阵列最好
                                break;
                            }
                        }
                    }
                    if (decide_array_id == -1)//no_using找不到
                    {
                        for (int i = 0; i < array_wait.size(); ++i) {
                            if (cap(array_wait[i]) > row_need) {
                                decide_array_id = array_wait[i];//不等待，存在DSE,寻找哪个阵列最好
                                break;
                            }
                        }
                    }
                    if (decide_array_id == -1)//找不到可用的，新建
                    {
                        decide_array_id = build(decide_array_type, op_type, array_list1, array_list2, array_list3);//新建
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
                    decide_array_id=input1_id;
            } else //SA\magic
            {
                //尽量在本阵列执行，如果容量不够，考虑其他阵列
                if ((decide_array_type == 2 && cap(input1_id) > row_need) ||
                    (decide_array_type == 3 && cap(input1_id) > row_need))
                    decide_array_id=input1_id;

            }
        }
        if (input2_type == decide_array_type) //看能否在2中运行
        {
            if (decide_array_type == 1)//LUT,无关，但还是尽量在本节点进行
            {
                if (array_list1[input2_id].op_type.find(op_type) != array_list1[input2_id].op_type.end())//2算子支持
                    decide_array_id= input2_id;
            } else //SA\magic
            {
                //尽量在本阵列执行，如果容量不够，考虑其他阵列
                if ((decide_array_type == 2 && cap(input2_id) > row_need) ||
                    (decide_array_type == 3 && cap(input2_id) > row_need))
                    decide_array_id= input2_id;

            }

        }
        if (decide_array_id == -1)//都不行，找一个可用的阵列
        {
            if (decide_array_type == 1) {

                if (!array_no_using.empty())
                    decide_array_id = array_no_using.front();//不等待，存在DSE,寻找哪个阵列最好
                else if (!array_wait.empty())
                    decide_array_id = array_wait.front();//等待，存在DSE,寻找哪个阵列最好
                else
                    decide_array_id = build(decide_array_type, op_type, array_list1, array_list2, array_list3);//新建

            } else//sa和magic,要保证剩余空间充足
            {
                //array no using is not empty
                if (!array_no_using.empty()) {
                    for (int i = 0; i < array_no_using.size(); ++i) //随便找一个能用的
                    {
                        if (cap(array_no_using[i]) > row_need) {
                            decide_array_id = array_no_using[i];//不等待，存在DSE,寻找哪个阵列最好
                            break;
                        }
                    }
                }
                //1.empty no using 2.no cap is enough
                if (decide_array_id == -1)//no_using找不到
                {
                    for (int i = 0; i < array_wait.size(); ++i) {
                        if (cap(array_wait[i]) > row_need) {
                            decide_array_id = array_wait[i];//不等待，存在DSE,寻找哪个阵列最好
                            break;
                        }
                    }
                }
                //wait list,no using list can't find,build
                if (decide_array_id == -1)
                {
                    decide_array_id = build(decide_array_type, op_type, array_list1, array_list2, array_list3);//新建
                }
            }
        }
    }
    time_update(op_type,decide_array_type,decide_array_id,time,node_now,array_list1,array_list2,array_list3, bit_num_operand);
    return decide_array_id;
}


//当前能用的阵列表,针对当前type,如果为空，证明都用不了
//此时sa和magic不用考虑容量，让决定id的去考虑，只需要把可用的放进队列去
vector<int> find_no_using(int op_type, vector<Node> &nodes, int decide_array_type, vector<lut_arr> &array_list1,
                          vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    vector<int> find_no_using;
    switch (decide_array_type) {
        case 1://找LUT,需要算子支持，并且能用
        {
            for (auto i :array_list1)
                if (!i.is_using)//空闲，算子不可用，但是还有空间，可以添加算子
                {
                    if (i.op_type.find(op_type) != i.op_type.end() ||
                        i.op_type.empty() && (op_type < 6 || op_type > 10) ||
                        (i.op_type.find(op_type) == i.op_type.end() &&
                         i.op_type.size() < 4 && op_type <= 10 && op_type >= 6)
                            ) {
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
vector<int> waiting_array_list(int op_type, vector<Node> &nodes, int decide_array_type, vector<lut_arr> &array_list1,
                               vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    vector<int> waiting_array_list;
    priority_queue<Array, vector<Array>, CompareArray> pq;//优先队列，小顶堆
    switch (decide_array_type) {
        case 1://找LUT,需要算子支持，并且能用
        {
            for (auto &i: array_list1) {

                if (i.is_using)//不空闲
                {
                    if (i.op_type.find(op_type) != i.op_type.end() ||
                        i.op_type.empty() && (op_type < 6 || op_type>10) ||
                        i.op_type.find(op_type) == i.op_type.end() &&
                        i.op_type.size() < 4 && op_type <= 10 && op_type >= 6
                            ) {
                        pq.push(i);
                    }

                }
            }
        }
            break;
        case 2://找SA
        {

            for (auto &i: array_list2) {

                if (i.is_using)//空闲
                    pq.push(i);
                //没有可用的，跳过
            }

        }
            break;
        case 3: {

            for (auto &i: array_list3) {

                if (i.is_using)//空闲
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


//只有着一个函数可以建立阵列**定义建立逻辑，对于LUT,考虑加入操作
int build(int decide_array_type, int op_type, vector<lut_arr> &array_list1, \
                vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    int build;
    switch (decide_array_type) {
        case 1: {
            lut_arr now1;
            now1.op_type.insert(op_type);
            now1.array_id = array_list1.size();
            array_list1.push_back(now1);
            build = array_list1.size() - 1;

        }
            break;
        case 2: {
            sa_arr now2;
            now2.array_id = array_list2.size();
            array_list2.push_back(now2);
            build = array_list2.size() - 1;

        }
            break;
        case 3: {
            magic_arr now3;
            now3.array_id = array_list3.size();
            array_list3.push_back(now3);
            build = array_list3.size() - 1;

        }
            break;
        default:
            break;
    }
    return build;
}

//计算当前还剩下多少能用,返回其数字：计算立即数
int cap_array_lost(int decide_array_type, int decide_array_id, vector<Node> &nodes, vector<lut_arr> &array_list1, \
                            vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    int cap_array = 0;
    switch (decide_array_type) {
        case 1://LUT, 计算还能够放下几个运算
        {
            auto it = array_list1[decide_array_id].op_type.begin();
            if (*it == 10 || *it == 11 || *it == 3 || *it == 4)
                return 0;
            else
                cap_array = 3 - array_list1[decide_array_id].op_type.size();
            cout << "lut当前的容量为" << cap_array << endl;
        }
            break;
        case 2://SA
        {
            //根据store_node,计算实际容量,有修改空间
            cap_array = array_list2[decide_array_id].row_num - array_list2[decide_array_id].store_node.size();
            cap_array = max(0, cap_array);
        }
            break;
        case 3://MAGIC
        {
            //节点存入magic有两种情况:1.要更新写回= 2.操作中有立即数，类型为op
            int stored = 0;
            //magic中，立即数也会被写入，因此要全部统计
            for (int i = 0; i < array_list3[decide_array_id].store_node.size(); ++i) {
                int op = op2int(find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->operator_name);
                if (op == 0 || op == 7) // =或者not
                {
                    stored++;
                } //需要根据其中有无立即数来具体判断存储的数量
                else //2个操作数
                {
                    if (find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->depend1 ==
                        nullptr)
                        stored++;
                    if (find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->depend2 ==
                        nullptr)
                        stored++;
                    if (op == 10) stored += 2;
                }

            }
            cap_array = max(0, array_list3[decide_array_id].row_num - stored);
        }
            break;
        default:
            break;

    }
    return cap_array;

}

//可覆盖容量,立即数也被算在内，因为OP类型只有立即数会被存入，ASS可以直接=立即数
//对于多步骤的运算，其容量不能单纯的由操作数的个数决定：如add需要使用5行
int cap_array_cover(int decide_array_type, int decide_array_id, vector<Node> &nodes, vector<lut_arr> &array_list1, \
                            vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    int cap_array = 0;
    switch (decide_array_type) {
        case 1://LUT, 直接返回0
        {
            return 0;
        }
        case 2://SA，只计算出度为0的
        {
            //op类型被存入只有一种可能：存入的是立即数，SA只会存入一个立即数
//根据store_node,计算实际容量,有修改空间
            for (int i = 0; i < array_list2[decide_array_id].store_node.size(); ++i) {
                if (find_node_by_number(nodes, array_list2[decide_array_id].store_node[i])->out_degree ==
                    0) //节点出度为0，不再被需要
                {
                    cap_array++;
//等被覆盖的时候再擦除，无意义的擦除浪费资源
                }
                if (find_node_by_number(nodes, array_list2[decide_array_id].store_node[i])->out_degree >
                    1) //节点出度>1，在多个阵列中存储的都有，也可以被覆盖
                {
                    //DSE,擦除还是不擦除，都可能影响整体的性能
                    cap_array++;
                }
            }

        }
            break;
        case 3://MAGIC
        {
//节点存入magic有两种情况:1.要更新写回= 2.操作中有立即数，类型为op
            for (int i = 0; i < array_list3[decide_array_id].store_node.size(); ++i) {
                int op = op2int(find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->operator_name);
                if (find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->out_degree ==
                    0) //节点出度为0，不再被需要
                {
                    if (op == 0 || op == 7) { cap_array++; }
                    else if (op == 10) {
                        cap_array += 2;//一定有的：L和C
                        if (find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->depend1 ==
                            nullptr)
                            cap_array++;//一个立即数
                        if (find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->depend2 ==
                            nullptr)
                            cap_array++;//2个立即数

                    }//加法占用了5row,ABCLS
                    else {
                        if (find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->depend1 ==
                            nullptr)
                            cap_array++;//一个立即数
                        if (find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->depend2 ==
                            nullptr)
                            cap_array++;//2个立即数
                    }
                }
                if (find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->out_degree > 1) //节点出度大于等于2，
                {
                    //存在DSE
                    if (op == 0 || op == 7) { cap_array++; }
                    else if (op == 10) {
                        cap_array += 2;//一定有的：L和C
                        if (find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->depend1 ==
                            nullptr)
                            cap_array++;//一个立即数
                        if (find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->depend2 ==
                            nullptr)
                            cap_array++;//2个立即数

                    }//加法占用了5row,ABCLS
                    else {
                        if (find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->depend1 ==
                            nullptr)
                            cap_array++;//一个立即数
                        if (find_node_by_number(nodes, array_list3[decide_array_id].store_node[i])->depend2 ==
                            nullptr)
                            cap_array++;//2个立即数
                    }
                }
            }
        }
            break;
        default:
            break;
    }
    return cap_array;
}

//需要单独写一个函数，得出计算每一个op需要的剩余容量,应该还需要比特数
int op_row_need(int op_type, int decide_array_type, int bit_num_operand) {
    switch (decide_array_type) {
        case 1://LUT输出需要的LUT数量
        {
            //TODO:DSE,lut_type
            int lut_type=6;//4--6
//            int lut_type=4;
            int lut_num=lut_num_op(op_type,bit_num_operand,lut_type);
            return lut_num;   //LUT应该和阵列级联以及位数有关，要具体分析，待定
        }
        case 2://SA
        {
            if (op_type == 8)//这里表示需要写回
                return 0;
            //待定
            return 1;//先假设至少需要1
        }
        case 3://MAGIC，这里把需要写回的结果的位置也算上了，避免执行完了没地方写回
        {
            if (op_type == 0)
                return 1;
            if (op_type == 11)
                return 5;//A B C L S
            if (op_type == 8)//not
                return 2;
            return 3;//其余的一律返回3

        }
        default:
            break;

    }

}


//定义数据读函数,SA、lUT需要比较是否是当前输出
void data_read(int input_type, int input_id, int decide_array_type, int decide_array_id, int *Register, \
                vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    if (input_type == 5 || input_type == 4 || input_type == -1)//S-BUFFER || lut-out ||register
    {
        Register[0]++;//寄存器读++
        //读数
        return;
    }
    if (input_type == 3)//MA 存储
    {
        if (input_type == decide_array_type && input_id == decide_array_id)
            return;
        else
            array_list3[input_id].read_number++;//从阵列中读取数据，读++
        //读数
        return;
    } else if (input_type == 2)//SA 存储
    {
        if (decide_array_type == input_type && decide_array_id == input_id) {
            return;
        } else {
            array_list2[input_id].read_number++;
        }
        return;
    } else//LUT 或者 没有input2
    {
        return;
        //直接连线
    }
//以上都没有讨论直接连线的情况..
}

//判断节点的写回表中有当前阵列
bool is_in_wb(int array_type, int array_id, Node *node_now) {
    //array_type 1 2 3
    switch (array_type) {
        case 1: {
            for (int i = 0; i < node_now->wb_pos[0].size(); ++i) {
                if (node_now->wb_pos[0][i] == array_id)
                    return true;
            }
            return false;
        }
            break;
        case 2: {
            for (int i = 0; i < node_now->wb_pos[1].size(); ++i) {
                if (node_now->wb_pos[1][i] == array_id)
                    return true;
            }
            return false;
        }
            break;
        case 3: {
            for (int i = 0; i < node_now->wb_pos[2].size(); ++i) {
                if (node_now->wb_pos[2][i] == array_id)
                    return true;
            }
            return false;
        }
            break;
        default:
            break;

    }
    return false;
}

//写入逻辑
void
input_logic(int input1_type, int input1_id, int input2_type, int input2_id, int decide_array_type, int decide_array_id, \
        Node *now, int *Register, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
            vector<magic_arr> &array_list3) {
    switch (decide_array_type) {
        case 1://LUT
            return;//LUT输入无需写入

        case 2: //LUT
        {
            //先讨论操作数1,如果在本SA阵列中（不是单纯的立即数、节点的写回表中有此阵列）
            if (input1_type == 2 && input1_id == decide_array_id &&
                is_in_wb(decide_array_type, decide_array_id, now->depend1)) {
                if (input2_type == 0 || (input2_type == 2 && input2_id == decide_array_id &&
                                         is_in_wb(decide_array_type, decide_array_id, now->depend2)))//输入2不存在或在阵列中
                {
                    return;
                }
                //DSE位置,下方2选1
                //2存在且2不在本阵列中，输入direct
                array_list2[decide_array_id].sa_direct = input2_id;//输入到激活行
                //或者写入
                array_list2[decide_array_id].write_number++;
                if (now->depend2)//不是立即数
                {
                    array_list2[decide_array_id].store_node.push_back(now->depend2->node_id);//阵列的存储节点
                    //更新wb_pos
                    now->depend2->wb_pos[1].push_back(decide_array_id);
                } else //是立即数,只更新store_node,不更新写回表
                {
                    array_list2[decide_array_id].store_node.push_back(now->node_id);
                }
            } else //操作数1不在阵列中
            {
                //先讨论操作数2在不在阵列中
                if (input2_type == 0 || (input2_type == 2 && input2_id == decide_array_id &&
                                         is_in_wb(decide_array_type, decide_array_id, now->depend2))) {
                    //DSE位置,下方2选1
                    //1输入direct
                    array_list2[decide_array_id].sa_direct = input1_id;//输入到激活行
                    //或者写入
                    array_list2[decide_array_id].write_number++;
                    if (now->depend1)//不是立即数
                    {
                        array_list2[decide_array_id].store_node.push_back(now->depend1->node_id);//阵列的存储节点
                        //更新wb_pos
                        now->depend1->wb_pos[1].push_back(decide_array_id);
                    } else //是立即数,只更新store_node,不更新写回表
                    {
                        array_list2[decide_array_id].store_node.push_back(now->node_id);
                    }
                } else //1,2都不在本阵列中
                {
                    //下方存在DSE，1D2W 2D1W 1W2W三种
                    //在写回时候，如果有一个是立即数，则将立即数直连，将变量写回
                    if (now->depend1 && now->depend2) //都不是立即数，DSE: 1D2W 2D1W 1W2W三种
                    {
                        //1D2W
                        array_list2[decide_array_id].sa_direct = input1_id;//1输入到激活行

                        array_list2[decide_array_id].write_number++;
                        array_list2[decide_array_id].store_node.push_back(now->depend2->node_id);//阵列的存储节点
                        //更新wb_pos
                        now->depend2->wb_pos[1].push_back(decide_array_id);

                    } else if (now->depend1) //2是立即数，1不是,将1写入
                    {
                        array_list2[decide_array_id].sa_direct = input2_id;

                        array_list2[decide_array_id].write_number++;
                        array_list2[decide_array_id].store_node.push_back(now->depend1->node_id);//阵列的存储节点
                        //更新wb_pos
                        now->depend1->wb_pos[1].push_back(decide_array_id);

                    } else if (now->depend2) //1是立即数，2不是
                    {
                        array_list2[decide_array_id].sa_direct = input1_id;

                        array_list2[decide_array_id].write_number++;
                        array_list2[decide_array_id].store_node.push_back(now->depend2->node_id);//阵列的存储节点
                        //更新wb_pos
                        now->depend1->wb_pos[1].push_back(decide_array_id);

                    } else //1 2 都是立即数
                    {
                        //随意将一个写入即可
                        array_list2[decide_array_id].sa_direct = input1_id;

                        //是立即数,只更新store_node,不更新写回表
                        //store_node存放当前“op”类型的节点id,表示存入了一个立即数
                        array_list2[decide_array_id].store_node.push_back(now->node_id);

                    }
                }
            }
        }
            break;
        case 3: //MAGIC
        {
            if (input1_type == 3 && input1_id == decide_array_id) //1在阵列中
            {
                if (input2_type == 0 || (input2_type == 3 && input2_id == decide_array_id)) //2也在阵列中 或者 2不存在
                {
                    return;
                } else //2不在阵列中
                {
                    array_list3[decide_array_id].write_number++;
                    //如果不是立即数，将2的节点存到store_node中，更新wb_pos
                    if (now->depend2) {
                        array_list3[decide_array_id].store_node.push_back(now->depend2->node_id);
                        now->depend2->wb_pos[2].push_back(decide_array_id);
                    } else //是立即数,只更新store_node,不更新写回表
                    {
                        array_list3[decide_array_id].store_node.push_back(now->node_id);
                    }
                }
            } else //1不在阵列中
            {
                //2不存在 || 2 在阵列中
                if (input2_type == 0 || input2_type == 3 && input2_id == decide_array_id) {
                    //将1写入阵列
                    array_list3[decide_array_id].write_number++;
                    //如果不是立即数，将1的节点存到store_node中，更新wb_pos
                    if (now->depend1) {
                        array_list3[decide_array_id].store_node.push_back(now->depend1->node_id);
                        now->depend1->wb_pos[2].push_back(decide_array_id);
                    } else //是立即数,只更新store_node,不更新写回表
                    {
                        array_list2[decide_array_id].store_node.push_back(now->node_id);
                    }
                } else  //1 2 都存在 且 都不在阵列中
                {
                    array_list3[decide_array_id].write_number += 2;
                    //不是立即数，找到其所在节点，更新store_node,更新wb_pos
                    if (now->depend1) {
                        array_list3[decide_array_id].store_node.push_back(now->depend1->node_id);
                        now->depend1->wb_pos[2].push_back(decide_array_id);
                    }
                    if (now->depend2) {
                        array_list3[decide_array_id].store_node.push_back(now->depend2->node_id);
                        now->depend2->wb_pos[2].push_back(decide_array_id);
                    }
                    if (!now->depend1 || !now->depend2) //存在立即数
                    {
                        array_list3[decide_array_id].store_node.push_back(now->node_id);
                    }
                }
            }
        }
            break;
        default:
            break;

    }

}

void output_logic(int decide_array_type, int decide_array_id, int op_type, Node *now, \
                vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    //执行期间对阵列的影响
    //对节点：需要调用模块时，更新finish_id
    //时间参数的影响
    //1.是否需要调用模块 LUT SA MA 都有设计好的模块供调用
    switch (decide_array_type) {
        case 1: //LUT
        {
            op_lut(op_type, decide_array_id, now, array_list1);
            //需要一个函数判断是否需要调用模块
            //根据op_type，进行每一个阵列下的操作：lut_op\sa_op\ma_op

            return;
        }

        case 2: {
            op_sa(op_type, decide_array_id, now, array_list2);
            return;
        }

        case 3: {
            op_magic(op_type, decide_array_id, now, array_list3);
            return;
        }

        default:
            return;
    }

}


//lut执行逻辑
void op_lut(int op_type, int decide_array_id, Node *now, vector<lut_arr> &array_list1) {
    now->do_type = 1;
    now->finish_id = decide_array_id;

}

//sa执行逻辑
void op_sa(int op_type, int decide_array_id, Node *now, vector<sa_arr> &array_list2) {
    now->do_type = 2;
    now->finish_id = decide_array_id;//模块调用怎么写？
}

//magic执行逻辑
void op_magic(int op_type, int decide_array_id, Node *now, vector<magic_arr> &array_list3) {
    now->do_type = 3;
    now->finish_id = decide_array_id;//模块调用怎么写？对操作进行拆解
}

//更新节点的出度
void out_degree(Node *now) {
    if (now->depend1)
        now->depend1->out_degree--;
    if (now->depend2)
        now->depend2->out_degree--;
    if (now->control)
        now->control->out_degree--;
}
//写覆盖,执行写操作时候
//更新项：节点的、阵列的。
//哪些可以更新：出度为0的节点可以更新，在多个阵列中存储的出度不为0的节点也可以更新
void write_cover()
{


}




