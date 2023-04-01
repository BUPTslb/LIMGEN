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


int Magic::Magic_cycle(int type) {
    int cycle_time;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type) {
        case 8://NOR
            cycle_time = 1;
            break;
        case 7://NOT
            cycle_time = 1;
            break;
        case 5://AND
            cycle_time = 2;
            break;
        case 6://OR
            cycle_time = 1;
            break;
        case 9://XOR
            cycle_time = 3;
            break;
        case 10://+
            cycle_time = 10;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_time;

}

double Magic::Magic_power(int type, int bl, int wl) {
    int cycle_power;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type) {
        case 8://NOR
            cycle_power = 10 * wl;
            break;
        case 7://NOT
            cycle_power = 5 * wl;
            break;
        case 5://AND
            cycle_power = 20 * wl;
            break;
        case 6://OR
            cycle_power = 5 * wl;
            break;
        case 9://XOR
            cycle_power = 30 * wl;
            break;
        case 10://+
            cycle_power = 100 * wl;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_power;
}

int SA::SA_cycle(int type) {
    int cycle_time;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type) {
        case 7://NOT
            cycle_time = 1;
            break;
        case 5://AND
            cycle_time = 1;
            break;
        case 6://OR
            cycle_time = 1;
            break;
        case 9://XOR
            cycle_time = 1;
            break;
        case 10://+
            cycle_time = 10;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_time;
}

double SA::SA_power(int type, int bl, int wl) {
    int cycle_power;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type) {
        case 7://NOT
            cycle_power = 5 * wl;
            break;
        case 5://AND
            cycle_power = 20 * wl;
            break;
        case 6://OR
            cycle_power = 5 * wl;
            break;
        case 9://XOR
            cycle_power = 30 * wl;
            break;
        case 10://+
            cycle_power = 100 * wl;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_power;
}

//cache一致性函数定义
bool cache_like(int array_type, int array_id, vector<vector<int>> &wb_pos) {
    if (array_type == 1) return false;
    else if (array_type == 2) {
        //遍历wb_pos[1],如果=id就命中
        if (wb_pos[1].empty()) return false;
        for (int i = 0; i < wb_pos[1].size(); ++i) {
            if (wb_pos[1][i] == array_id)
                return true;
        }
    } else {
        //遍历wb_pos[2],如果=id就命中
        if (wb_pos[2].empty()) return false;
        for (int i = 0; i < wb_pos[2].size(); ++i) {
            if (wb_pos[2][i] == array_id)
                return true;
        }
    }

}

//定义节点执行表更新策略
bool update_wb_pos(bool cache_like, int pos_input, int array_type, int pos_array) {
    //cache命中 || 阵列类型为LUT
    if (cache_like || array_type == 1)
        return false;
    else //不命中且在SA/MAGIC
        return true;
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

//定义决定执行阵列类型,**,
int decide_array_type(int op_type, int design_target) {
    int decide_array_type = 0;
    if (1 <= op_type && op_type <= 4 || op_type == 11) {
        decide_array_type = 1;//移位，比较，选择lut
    } else if (op_type == 8) {
        decide_array_type = 3;
    } else if (op_type == 9) {
        decide_array_type = 2;
    } else {
        switch (design_target) {
            case 0://速度，lut
                //需要综合考虑数据搬移和直接执行的速度比较
                decide_array_type = 1;
                break;
            case 1://面积，magic
                decide_array_type = 3;
                break;
            case 2://功耗，sa
                //需要考虑数据搬移和直接执行的功耗比较
                decide_array_type = 2;
                break;
            default:
                break;
        }
    }
    return decide_array_type;
}

//定义执行阵列的id
int decide_array_id(int op_type, vector<Node> &nodes, int decide_array_type, \
                    vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3, \
                    int input1_type, int input1_id, int input2_type, int input2_id) {
/*当前的寻找策略
 * 如果操作数存放位置与执行阵列的类型相同，就在此执行
 * 否则找一个同类型能用的执行
 * 如果暂时都在使用，则新建一个阵列
 * 方案二：等待阵列执行完毕，寻找一个阵列执行，这样可以提供好面积利用率，待补充
 * */
    int decide_array_id = -1;
    if (input2_type == 0)//如果只有一个操作数,基本上执行的都是not操作
    {
        if (input1_type == -1) {
            //立即数，找一个能用的即可,或者等待时间最小的，获取等待时间
            decide_array_id = find_no_using(op_type, nodes, decide_array_type, array_list1, array_list2, array_list3);
            //如果结束时间+当前执行时间都超过标准，则新建
            if (decide_array_id == -1) {
                return build(decide_array_type, op_type, array_list1, array_list2, array_list3);
            }

        } else//不是立即数
        {
            if (input1_type == decide_array_type)//类型same
            {
                //如果是lut,还是需要判断,尽量在本阵列执行
                if (decide_array_type == 1) {
                    if (array_list1[input1_id].op_type[op_type]) //算子支持
                        return input1_id;
                    else
                        decide_array_id = find_no_using(op_type, nodes, decide_array_type, array_list1, array_list2,
                                                        array_list3);

                } else {
                    //sa和magic,要保证剩余空间充足
                    int cap_lost = cap_array_lost(decide_array_type, input1_id, nodes, array_list1, array_list2, array_list3);//空闲容量
                    int cap_cover = cap_array_cover(decide_array_type, input1_id, nodes, array_list1, array_list2, array_list3);//可覆盖容量
                    if ((decide_array_type == 2 && cap_lost > 0) || (decide_array_type == 3 && cap_lost > 1))
                        return input1_id;

                }
                //都不行
                if (decide_array_id == -1) {
                    return build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                }
            } else//类型不同
            {
                decide_array_id = find_no_using(op_type, nodes, decide_array_type, array_list1, array_list2,
                                                array_list3);
                if (decide_array_id == -1) {
                    return build(decide_array_type, op_type, array_list1, array_list2, array_list3);
                }
            }
        }
    } else//two operands
    {
        if (input1_type == decide_array_type) {
            if (decide_array_type == 1)//LUT,无关，但还是尽量在本节点进行
            {
                if (array_list1[input1_id].op_type[op_type])
                    return input1_id;
                else
                    decide_array_id = find_no_using(op_type, nodes, decide_array_type, array_list1, array_list2,
                                                    array_list3);
                if (decide_array_id == -1)
                    return build(decide_array_type, op_type, array_list1, array_list2, array_list3);
            } else {
                //尽量在本阵列执行，如果容量不够，考虑其他阵列
                int cap_lost_1 = cap_array_lost(decide_array_type, input1_id, nodes, array_list1, array_list2, array_list3);
                int cap_cover_1= cap_array_cover(decide_array_type, input1_id, nodes, array_list1, array_list2, array_list3);
                if ((decide_array_type == 2 && cap_lost_1+cap_cover_1 > 0) || (decide_array_type == 3 && cap_lost_1+cap_cover_1 > 1))
                    return input1_id;

                //容量不够,考虑其他阵列
                //如果阵列2能用，返回阵列2
                if (input2_type == decide_array_type) {
                    int cap_lost_2 = cap_array_lost(decide_array_type, input1_id, nodes, array_list1, array_list2, array_list3);
                    int cap_cover_2= cap_array_cover(decide_array_type, input1_id, nodes, array_list1, array_list2, array_list3);
                    if ((decide_array_type == 2 && cap_lost_2+cap_cover_2 > 0) || (decide_array_type == 3 && cap_lost_2+cap_cover_2 > 1))
                        return input2_id;
                }
                //如果阵列2也不能用，新建，返回新建阵列的id
                return build(decide_array_type, op_type, array_list1, array_list2, array_list3);

            }

        } else if (input2_type == decide_array_type) //1类型不同，2类型一致
        {
            if (decide_array_type == 1)//LUT,无关，但还是尽量在本节点进行
            {
                if (array_list1[input2_id].op_type[op_type])
                    return input2_id;
                else
                    decide_array_id = find_no_using(op_type, nodes, decide_array_type, array_list1, array_list2,
                                                    array_list3);

                if (decide_array_id == -1)
                    return build(decide_array_type, op_type, array_list1, array_list2, array_list3);
            } else {
                int cap_lost = cap_array_lost(decide_array_type, input2_id, nodes, array_list1, array_list2, array_list3);
                int cap_cover = cap_array_lost(decide_array_type, input2_id, nodes, array_list1, array_list2, array_list3);

                if ((decide_array_type == 2 && cap_lost+cap_cover > 0) || (decide_array_type == 3 && cap_lost+cap_cover > 1))
                    return input2_id;

                //如果阵列2也不能用，新建，返回新建阵列的id
                return build(decide_array_type, op_type, array_list1, array_list2, array_list3);

            }

        } else//find a  no_using，12都不同
        {
            decide_array_id = find_no_using(op_type, nodes, decide_array_type, array_list1, array_list2, array_list3);
            if (decide_array_id == -1) {
                decide_array_id = build(decide_array_type, op_type, array_list1, array_list2, array_list3);
            }
        }
    }
    return decide_array_id;
}

//找一个能用的，或者结束时间最快的
int find_no_using(int op_type, vector<Node> &nodes, int decide_array_type, vector<lut_arr> &array_list1,
                  vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    int find_no_using = -1;//记录的是最快结束的阵列
    switch (decide_array_type) {
        case 1://找LUT,需要算子支持，并且能用
        {
            double min_over = array_list1[0].use_time['o'];
            for (int i = 0; i < array_list1.size(); ++i) {

                if (array_list1[i].use_time['o'] < min_over && array_list1[i].op_type[op_type]) {
                    find_no_using = i;
                    min_over = array_list1[i].use_time['o'];//结束时间
                }

                if (!array_list1[i].is_using && array_list1[i].op_type[op_type])//空闲，且算子可用
                    return i;
                //没有可用的，跳过
            }

        }
            break;
        case 2://找SA,需要容量够,
        {
            double min_over = array_list2[0].use_time['o'];
            for (int i = 0; i < array_list2.size(); ++i) {
                if (cap_array_lost(decide_array_type, i, nodes, array_list1, array_list2, array_list3) < 2)
                    continue;
                if (array_list2[i].use_time['o'] < min_over) {
                    find_no_using = i;
                    min_over = array_list2[i].use_time['o'];//结束时间
                }
                if (!array_list2[i].is_using)//空闲
                    return i;
                //没有可用的，跳过
            }

        }
            break;
        case 3: {
            double min_over = array_list3[0].use_time['o'];
            for (int i = 0; i < array_list3.size(); ++i) {

                if (cap_array_lost(decide_array_type, i, nodes, array_list1, array_list2, array_list3) < 3)
                    continue;

                if (array_list3[i].use_time['o'] < min_over) {
                    find_no_using = i;
                    min_over = array_list3[i].use_time['o'];//结束时间
                }

                if (!array_list1[i].is_using)//空闲
                    return i;
                //没有可用的，跳过
            }

        }
            break;
        default:
            break;
    }
    return find_no_using;
}

//定义建立逻辑，对于LUT,考虑加入操作
int build(int decide_array_type, int op_type, vector<lut_arr> &array_list1, \
                vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    int build;
    switch (decide_array_type) {
        case 1: {
            lut_arr now1;
            now1.op_type[op_type] = true;
            array_list1.push_back(now1);
            build = array_list1.size() - 1;

        }
            break;
        case 2: {
            sa_arr now2;
            array_list2.push_back(now2);
            build = array_list2.size() - 1;

        }
            break;
        case 3: {
            magic_arr now3;
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
            if (it->first == 10 || it->first == 11 || it->first == 3 || it->first == 4)
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
            cap_array= max(0,cap_array);
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
                    if (find_node_by_number(nodes,i)->depend1== nullptr)    stored++;
                    if (find_node_by_number(nodes,i)->depend2== nullptr)    stored++;
                    if(op==10)  stored+=2;
                }

            }
            cap_array = max(0,array_list3[decide_array_id].row_num - stored);
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
                if (find_node_by_number(nodes, i)->out_degree == 0) //节点出度为0，不再被需要
                {
                    cap_array++;
//等被覆盖的时候再擦除，无意义的擦除浪费资源
                }
                if (find_node_by_number(nodes, i)->out_degree > 1) //节点出度>1，在多个阵列中存储的都有，也可以被覆盖
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
                if (find_node_by_number(nodes, i)->out_degree == 0) //节点出度为0，不再被需要
                {
                    if (op == 0 || op == 7) { cap_array++; }
                    else if (op == 10)
                    {
                        cap_array+=2;//一定有的：L和C
                        if (find_node_by_number(nodes,i)->depend1== nullptr )   cap_array++;//一个立即数
                        if (find_node_by_number(nodes,i)->depend2== nullptr )   cap_array++;//2个立即数

                    }//加法占用了5row,ABCLS
                    else
                    {
                        if (find_node_by_number(nodes,i)->depend1== nullptr )   cap_array++;//一个立即数
                        if (find_node_by_number(nodes,i)->depend2== nullptr )   cap_array++;//2个立即数
                    }
                }
                if (find_node_by_number(nodes, i)->out_degree > 1) //节点出度大于等于2，
                {
                    //存在DSE
                    if (op == 0 || op == 7) { cap_array++; }
                    else if (op == 10)
                    {
                        cap_array+=2;//一定有的：L和C
                        if (find_node_by_number(nodes,i)->depend1== nullptr )   cap_array++;//一个立即数
                        if (find_node_by_number(nodes,i)->depend2== nullptr )   cap_array++;//2个立即数

                    }//加法占用了5row,ABCLS
                    else
                    {
                        if (find_node_by_number(nodes,i)->depend1== nullptr )   cap_array++;//一个立即数
                        if (find_node_by_number(nodes,i)->depend2== nullptr )   cap_array++;//2个立即数
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
            op_lut(op_type, now, array_list1);
            //需要一个函数判断是否需要调用模块
            //根据op_type，进行每一个阵列下的操作：lut_op\sa_op\ma_op
            return;
        }
        case 2: {
            op_sa(op_type, now, array_list2);
            return;
        }
        case 3: {
            op_magic(op_type, now, array_list3);
            return;
        }
        default:
            return;
    }

}


//lut执行逻辑
void op_lut(int op_type, Node *now, vector<lut_arr> &array_list1) {

    switch (op_type) {
        case 1: //比较，小于

            break;
        case 2: //比较，大于

            break;
        case 3: //逻辑左移

            break;
        case 4://逻辑右移

            break;
        case 5: //与

            break;
        case 6: //或

            break;
        case 7: //非

            break;
        case 8: //NOR

            break;
        case 9: //XOR

            break;
        case 10: //ADD

            break;
        case 11: //MUL

            break;
        default: //对于按位的逻辑运算，考虑将其合并到default中
            break;
    }
}

//sa执行逻辑
void op_sa(int op_type, Node *now, vector<sa_arr> &array_list2) {
    switch (op_type) {
        case 1: //比较，小于

            break;
        case 2: //比较，大于

            break;
        case 3: //逻辑左移

            break;
        case 4://逻辑右移

            break;
        case 5: //与

            break;
        case 6: //或

            break;
        case 7: //非

            break;
        case 8: //NOR

            break;
        case 9: //XOR

            break;
        case 10: //ADD

            break;
        case 11: //MUL

            break;
        default: //对于按位的逻辑运算，考虑将其合并到default中
            break;
    }
}

//sa执行逻辑
void op_magic(int op_type, Node *now, vector<magic_arr> &array_list3) {
    switch (op_type) {
        case 1: //比较，小于

            break;
        case 2: //比较，大于

            break;
        case 3: //逻辑左移

            break;
        case 4://逻辑右移

            break;
        case 5: //与

            break;
        case 6: //或

            break;
        case 7: //非

            break;
        case 8: //NOR

            break;
        case 9: //XOR

            break;
        case 10: //ADD

            break;
        case 11: //MUL

            break;
        default: //对于按位的逻辑运算，考虑将其合并到default中
            break;
    }
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

//擦除阵列中的无关节点
void erase_array(Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    if (now->out_degree == 0) {
        //清空其wb_pos
        //在阵列中删除其节点id
        if (!now->wb_pos[0].empty()) {
        }
        if (!now->wb_pos[1].empty()) {
            for (int i = 0; i < now->wb_pos[1].size(); ++i) {
                auto it = array_list2[now->wb_pos[1][i]].store_node.begin();
                while (*it != now->node_id) {
                    it++;
                }
                array_list2[now->wb_pos[1][i]].store_node.erase(it);//擦除
            }
        }
        if (!now->wb_pos[2].empty()) {
            for (int i = 0; i < now->wb_pos[2].size(); ++i) {
                auto it = array_list3[now->wb_pos[2][i]].store_node.begin();
                while (*it != now->node_id) {
                    it++;
                }
                array_list3[now->wb_pos[2][i]].store_node.erase(it);//擦除
            }
        }
        now->wb_pos[0].clear();//将写回表擦除
    }

}








