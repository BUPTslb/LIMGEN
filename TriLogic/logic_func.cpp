#include"logic_func.h"

using namespace std;

unsigned int arr_size() {
    unsigned int arr_size;
    unsigned m = 1;
    while (m < bit_num_operand) {
        m <<= 1;
    }
    arr_size = (m <= 64) ? m : 64;
    return arr_size;
}


//寻找输入操作数来源：阵列（ID）/寄存器
void find_input(int &array_type, int &array_id, int op_type, Node *node_depend, int cycle) {
    if (node_depend == nullptr || node_depend->do_type == -1)//立即数,或者（A=立即数）
    {
        if (op_type == 0 || op_type == 8)//  one operand : = not
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


//只有着一个函数可以建立阵列**定义建立逻辑，对于LUT,考虑加入操作
//TODO:build时将阵列的大小也确定,操作数的位数应该是一个全局变量
int build(int decide_array_type, int op_type, vector<lut_arr> &array_list1, \
                vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    int build;
    switch (decide_array_type) {
        case 1: {
            lut_arr now1;
            //设置功能。
            now1.op_type.insert(op_type);
            //TODO:如果是lut4,数字为16
            now1.row_num=64;
            now1.col_num=bit_num_operand;//这里只代表输出的位数，不代表真实的列数
            now1.lut_num= lut_num_op(op_type,6);//lut-6数量
            now1.lut_level= lut_level_op(op_type,6);//执行但前运算需要的级别数
            now1.array_id = array_list1.size();
            now1.energy=0;
            array_list1.push_back(now1);
            build = array_list1.size() - 1;

        }
            break;
        case 2: {
            sa_arr now2;
            now2.row_num=bit_num_operand;
            now2.col_num=bit_num_operand;
            now2.array_id = array_list2.size();
            now2.energy=0;
            array_list2.push_back(now2);
            build = array_list2.size() - 1;

        }
            break;
        case 3: {
            magic_arr now3;
            now3.row_num=bit_num_operand;
            now3.col_num=bit_num_operand;
            now3.array_id = array_list3.size();
            now3.energy=0;
            array_list3.push_back(now3);
            build = array_list3.size() - 1;

        }
            break;
        default:
            break;
    }
    return build;
}


//需要单独写一个函数，得出计算每一个op需要的剩余容量,应该还需要比特数
//假设两个操作数都在阵列中
int op_row_need(int op_type, int decide_array_type,Node *node_now) {
    switch (decide_array_type) {
        case 1://LUT输出需要的LUT数量
        {
            //TODO:DSE,lut_type
            int lut_type=6;//4--6
//            int lut_type=4;
            int lut_num=lut_num_op(op_type,lut_type);
            return lut_num;   //LUT应该和阵列级联以及位数有关，要具体分析，待定
        }
        case 2://SA
        {
            if (op_type==11)//add
                return 3; //ab a+b a^b
            if (op_type==0)//写回，write_back
                return 1;
            else
                return 0;
        }
        case 3://MAGIC，这里把需要写回的结果的位置也算上了，避免执行完了没地方写回
        {
            if (op_type == 11)
                return 3;//C L S
            //TODO:移位如何处理？shift
            else
                return 1;

        }
        default:
            break;

    }

}


//定义数据读函数,SA、lUT需要比较是否是当前输出
void data_read(int input_type, int input_id, int decide_array_type, int decide_array_id,
                vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    if (input_type == 5 || input_type == 4 || input_type == -1)//S-BUFFER || lut-out ||register
    {
        //寄存器读++
        Reg_sum.read_num_sum++;
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
        case 2: {
            for (int i = 0; i < node_now->wb_pos[1].size(); ++i) {
                if (node_now->wb_pos[1][i] == array_id)
                    return true;
            }
            return false;
        }
        case 3: {
            for (int i = 0; i < node_now->wb_pos[2].size(); ++i) {
                if (node_now->wb_pos[2][i] == array_id)
                    return true;
            }
            return false;
        }
        default:
            break;

    }
    return false;
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

//数据存储的位置有几个
int num_node_position(Node *now)
{
    int num=0;
    for (const auto& i : now->wb_pos) {
        if (!i.empty())
            num+=i.size();
    }
    return num;

}

//写覆盖,执行写操作时候
//更新项：节点的、阵列的。
//哪些可以更新：出度为0的节点可以更新，在多个阵列中存储的出度不为0的节点也可以更新
void write_cover()
{


}




