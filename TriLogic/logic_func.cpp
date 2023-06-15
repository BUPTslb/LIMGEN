#include"logic_func.h"

using namespace std;

//将尺寸设置为>=操作数的2的幂
unsigned int arr_size() {
    unsigned int arr_size;
    unsigned m = 1;
    while (m < bit_num_operand) {
        m <<= 1; //向左移动一位数字
    }
    arr_size = m;

    return arr_size;
}


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


//只有着一个函数可以建立阵列**定义建立逻辑，对于LUT,考虑加入操作
int build(int decide_array_type, int op_type, vector<lut_arr> &array_list1, \
                vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    int build;
    switch (decide_array_type) {
        case 1: {
            lut_arr now1;
            //如果是lut4,数字为16
            //初始化id
            now1.array_id = array_list1.size();
            //初始化is_using
            now1.is_using = false;
            //初始化面积
            now1.row_num = 64;//代表是lut6
            now1.col_num = bit_num_operand;//这里只代表输出的位数，不代表真实的列数
            now1.lut_num = lut_num_op(op_type, 6);//lut-6数量
            lut_level_op(op_type, 6);//执行但前运算需要的级别数
            //初始化读写次数
            now1.read_number = 0;
            now1.write_number = 0;
            //初始化时间
            now1.start_time = 0;
            now1.over_time = 0;
            //初始化能量
            now1.energy = 0;
            //初始化lut-out
            now1.lut_out = -1;//将新建阵列的out设置为-1
            //设置功能。
            now1.op_type = -1;
            if (op_type != 0)
                now1.op_type=op_type;

            array_list1.push_back(now1);
//            //cout << "当前array_list1的大小为：" << array_list1.size() << endl;
            build = array_list1.size() - 1;

        }
            break;
        case 2: {
            sa_arr now2;
            //初始化id
            now2.array_id = array_list2.size();
            //初始化is_using
            now2.is_using = false;
            now2.add_use=false;
            if (op_type==11)
                now2.add_use= true;
            //初始化外围电路的类型
            int sa_type_ready[2]={1,2};
            int type_chosen=rand()%2;

            now2.sa_type=sa_type_ready[type_chosen];
            //初始化大小
            now2.row_num = bit_num_operand;
            now2.col_num = bit_num_operand;
            //初始化读写次数
            now2.read_number = 0;
            now2.write_number = 0;
            //初始化时间
            now2.start_time = 0;
            now2.over_time = 0;
            //初始化能量
            now2.energy = 0;
            //sa_direct -1是寄存器 >=0是节点
            now2.sa_direct = -2;//将直接输入设置为-2
            now2.sa_out = -1;//将新建阵列的输出设置为-1，>=0为节点
            array_list2.push_back(now2);
            build = array_list2.size() - 1;

//            //cout<<"new SA array "<<now2.start_time<<" time "<<now2.over_time<<endl;

        }
            break;
        case 3: {
            magic_arr now3;
            //初始化id
            now3.array_id = array_list3.size();
            //初始化is_using
            now3.is_using = false;
            now3.add_use= false;
            if (op_type==11)
                now3.add_use=true;
            //初始化大小
            now3.row_num = 64;
            now3.col_num = 64;
            //初始化读写次数
            now3.read_number = 0;
            now3.write_number = 0;
            //初始化时间
            now3.start_time = 0;
            now3.over_time = 0;
            //初始化能量
            now3.energy = 0;
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
int op_row_need(int op_type, int decide_array_type) {
    int row_need=0;
    switch (decide_array_type) {
        case 1://LUT输出需要的LUT数量
        {
            int lut_type = 6;//4--6
//            int lut_type=4;
            int lut_num = lut_num_op(op_type, lut_type);
            return lut_num;   //LUT应该和阵列级联以及位数有关，要具体分析，待定
        }
        case 2://SA
        {
            if (op_type == 11)//add
                return 3; //ab a+b a^b
            if (op_type == 0)//写回，write_back
                return 1;
            else
                return 0;
        }
        case 3://MAGIC，这里把需要写回的结果的位置也算上了，避免执行完了没地方写回
        {
            if (op_type == 11)
                return 3;//C L S
            else
                return 1; //S
        }
        default:
            break;

    }
    return row_need;

}

//定义数据读函数,SA、lUT需要比较是否是当前输出
//需要写入阵列的直接执行写入函数write_back()
void data_read(int No_depend, int &input_type, int &input_id, int decide_array_type, int decide_array_id, Nodes *now,
               vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    //如果当前操作数来自寄存器，需要调用寄存器读函数
    //时间和能量加在哪里？
    //根据当前节点，获取当前的时间
    double time_n = time_now(array_list1, array_list2, array_list3, now, decide_array_type, decide_array_id);
    //cout << "no error data_read time_now" << endl;
    //从数据读取开始，op节点开始执行
    if (now->start_time == 0)
    {
        //cout<<"data_read 1"<<endl;
        now->start_time = time_n;
        //cout<<"data_read 2"<<endl;
    }
    if (input_type == -1)//register
    {

        //cout<<"data_read 3"<<endl;
        //调用数据读取函数
        //更新读取次数
        Reg_sum.read_num_sum++;
        //cout<<"data_read 4"<<endl;
        //更新数据传输次数
        array_data_transfer(-1, input_id, decide_array_type, decide_array_id,array_list1,array_list2, array_list3);
        //cout<<"data_read 5"<<endl;
        //更新时间
        read_time_update(-1, -1, time_n, now, array_list1, array_list2, array_list3);
        //cout<<"data_read 6"<<endl;
        //更新能量
        read_energy_update(-1, -1, now, array_list1, array_list2, array_list3);
        //cout<<"data_read 7"<<endl;
        //读数
        return;
    }
    if (input_type == 1 || input_type == 4) //来自lut阵列
    {
        //增加数据交换次数
        //cout<<"data_read 8"<<endl;
        array_data_transfer(1, input_id, decide_array_type, decide_array_id,array_list1,array_list2, array_list3);
        //cout<<"data_read 9"<<endl;
        //两种情况
        //1.是lut的输出
        if (No_depend == 1) //depend1
        {
            //cout<<"data_read 10"<<endl;
            //依赖的是lut-out
            if (input_type == 1) //来自lut的输出，写回表肯定是空的
            {
                //cout<<"data_read 11"<<endl;
                if (now->start_time != now->depend1->end_time
                    && array_list1[input_id].lut_out != now->depend1->node_id)//对不上
                {
                    //cout<<"data_read 12"<<endl;
                    //将out写回
                    write_back(input_type, input_id, now->depend1, array_list1, array_list2, array_list3);
                    //cout<<"data_read 13"<<endl;
                    //写回到哪里从哪里接着读取
                    //重新调用find_input
                    if (now->depend1 != nullptr)
                    {
                        //cout<<"data_read 14"<<endl;
                        find_input(input_type, input_id, now->depend1);
                        //cout<<"data_read 15"<<endl;
                    }
                    else {
                        //cout<<"data_read 16"<<endl;
                        //立即数
                        input_type = -1;
                        input_id = -1;
                        //cout<<"data_read 17"<<endl;
                    }
                    //修改input_type和input_id继续调用函数
                    //cout<<"data_read 18"<<endl;
                    data_read(1, input_type, input_id, decide_array_type, decide_array_id, now,
                              array_list1, array_list2, array_list3);
                    //cout<<"data_read 19"<<endl;

                }
                //时间刚好,先不写回
                //数据直接传输过去
            } else //来自lut-buffer 4
            {
                //cout<<"data_read 20"<<endl;
                //调用数据读取函数
                //更新buffer的读次数
                buffer_sum.buffer_read_sum++;
                //cout<<"data_read 21"<<endl;
                //更新时间
                read_time_update(4, input_id, time_n, now, array_list1, array_list2, array_list3);
                //cout<<"data_read 22"<<endl;
                //更新能量
                read_energy_update(4, input_id, now, array_list1, array_list2, array_list3);
                //cout<<"data_read 23"<<endl;
            }

            return;
        }
        if (No_depend == 2) //depend2
        {
            //cout<<"data_read 24"<<endl;
            //依赖的是lut-out
            if (input_type == 1) //来自lut的输出,写回表肯定是空的
            {
                //cout<<"data_read 25"<<endl;
                if (now->start_time != now->depend2->end_time
                    && array_list1[input_id].lut_out != now->depend2->node_id)//对不上
                {
                    //cout<<"data_read 26"<<endl;
                    //将out写回
                    write_back(input_type, input_id, now->depend2, array_list1, array_list2, array_list3);
                    //cout<<"data_read 27"<<endl;
                    //写回到哪里从哪里接着读取
                    //重新调用find_input
                    if (now->depend2 != nullptr)
                    {
                        //cout<<"data_read 28"<<endl;
                        find_input(input_type, input_id, now->depend2);
                        //cout<<"data_read 29"<<endl;
                    }
                    else {
                        //cout<<"data_read 30"<<endl;
                        //立即数
                        input_type = -1;
                        input_id = -1;
                        //cout<<"data_read 31"<<endl;
                    }
                    //修改input_type和input_id继续调用函数
                    //cout<<"data_read 32"<<endl;
                    data_read(2, input_type, input_id, decide_array_type, decide_array_id, now,
                              array_list1, array_list2, array_list3);
                    //cout<<"data_read 33"<<endl;

                }
                //时间刚好,先不写回
                //数据直接传输过去
            } else //来自lut-buffer 4
            {
                //调用数据读取函数
                //更新次数
                //cout<<"data_read 34"<<endl;
                buffer_sum.buffer_read_sum++;
                //cout<<"data_read 35"<<endl;
                //更新时间
                read_time_update(4, input_id, time_n, now, array_list1, array_list2, array_list3);
                //cout<<"data_read 36"<<endl;
                //更新能量
                read_energy_update(4, input_id, now, array_list1, array_list2, array_list3);
                //cout<<"data_read 37"<<endl;
            }
        }
    }

    if (input_type == 2 || input_type == 5 || input_type == 6) //sa三种情况 2:sa-out 5:sa-buffer 6:sa存储
    {
        //cout<<"data_read 38"<<endl;
        //从sa中拿数据，修改数据
        array_data_transfer(2, input_id, decide_array_type, decide_array_id,array_list1,array_list2, array_list3);
        //cout<<"data_read 39"<<endl;
        if (No_depend == 1) {
            //cout<<"data_read 40"<<endl;
            if (input_type == 2) //来自是sa_out,写回表肯定是空的
            {
                //cout<<"data_read 41"<<endl;
                if (now->start_time != now->depend1->end_time
                    && array_list2[input_id].sa_out != now->depend1->node_id)//对不上
                {
                    //cout<<"data_read 42"<<endl;
                    //将out写回
                    write_back(input_type, input_id, now->depend1, array_list1, array_list2, array_list3);
                    //cout<<"data_read 43"<<endl;
                    //写回到哪里从哪里接着读取
                    //重新调用find_input
                    if (now->depend1 != nullptr)
                    {
                        //cout<<"data_read 44"<<endl;
                        find_input(input_type, input_id, now->depend1);
                        //cout<<"data_read 45"<<endl;
                    }
                    else {
                        //cout<<"data_read 46"<<endl;
                        //立即数
                        input_type = -1;
                        input_id = -1;
                        //cout<<"data_read 47"<<endl;
                    }
                    //修改input_type和input_id继续调用函数
                    //cout<<"data_read 48"<<endl;
                    data_read(1, input_type, input_id, decide_array_type, decide_array_id, now,
                              array_list1, array_list2, array_list3);
                    //cout<<"data_read 49"<<endl;

                }
            }
            if (input_type == 5) //来自sa_buffer
            {
                //cout<<"data_read 50"<<endl;
                //是buffer证明没有被擦除
                //调用数据读取函数
                //更新buffer的读次数
                buffer_sum.buffer_read_sum++;
                //cout<<"data_read 51"<<endl;
                //更新时间
                read_time_update(5, input_id, time_n, now, array_list1, array_list2, array_list3);
                //更新能量
                //cout<<"data_read 52"<<endl;
                read_energy_update(5, input_id, now, array_list1, array_list2, array_list3);
                //读数
                //cout<<"data_read 53"<<endl;
                return;
            }
            if (input_type == 6) //来自sa存储 被写回到了阵列中
            {
                //cout<<"data_read 54"<<endl;
                //看阵列是否是本阵列，不对应时需要读取！！
                if (decide_array_type != 2 || decide_array_id != input_id) {
                    //cout<<"data_read 55"<<endl;
                    array_list2[input_id].read_number++;
                    //cout<<"data_read 56"<<endl;
                    //更新时间
                    read_time_update(6, input_id, time_n, now, array_list1, array_list2, array_list3);
                    //更新能量
                    //cout<<"data_read 57"<<endl;
                    read_energy_update(6, input_id, now, array_list1, array_list2, array_list3);
                    //cout<<"data_read 58"<<endl;
                    return;

                }

            }
        } else {
            if (input_type == 2) //来自sa_out,写回表肯定是空的
            {
                //cout<<"data_read 59"<<endl;
                if (now->start_time != now->depend2->end_time
                    && array_list2[input_id].sa_out != now->depend2->node_id)//对不上
                {
                    //cout<<"data_read 60"<<endl;
                    //将out写回
                    write_back(input_type, input_id, now->depend2, array_list1, array_list2, array_list3);
                    //cout<<"data_read 61"<<endl;
                    //写回到哪里从哪里接着读取
                    //重新调用find_input
                    if (now->depend2 != nullptr)
                    {
                        //cout<<"data_read 62"<<endl;
                        find_input(input_type, input_id, now->depend2);
                        //cout<<"data_read 63"<<endl;
                    }
                    else {
                        //cout<<"data_read 64"<<endl;
                        //立即数
                        input_type = -1;
                        input_id = -1;
                        //cout<<"data_read 65"<<endl;
                    }
                    //修改input_type和input_id继续调用函数

                    //cout<<"data_read 66"<<endl;
                    data_read(2, input_type, input_id, decide_array_type, decide_array_id, now,
                              array_list1, array_list2, array_list3);
                    //cout<<"data_read 67"<<endl;

                }


            }
            if (input_type == 5) //来自sa_buffer
            {
                //cout<<"data_read 68"<<endl;
                //是buffer证明没有被擦除
                //调用数据读取函数
                //更新buffer的读次数
                buffer_sum.buffer_read_sum++;
                //cout<<"data_read 69"<<endl;
                //更新时间
                read_time_update(5, input_id, time_n, now, array_list1, array_list2, array_list3);
                //cout<<"data_read 70"<<endl;
                //更新能量
                read_energy_update(5, input_id, now, array_list1, array_list2, array_list3);
                //cout<<"data_read 71"<<endl;
                //读数
                return;

            }
            if (input_type == 6) //来自sa存储
            {
                //cout<<"data_read 72"<<endl;
                //看阵列是否是本阵列，不对应时需要读取！！
                if (decide_array_type != 2 || decide_array_id != input_id) {
                    //cout<<"data_read 73"<<endl;
                    array_list2[input_id].read_number++;
                    //cout<<"data_read 74"<<endl;
                    //更新时间
                    read_time_update(6, input_id, time_n, now, array_list1, array_list2, array_list3);
                    //cout<<"data_read 75"<<endl;
                    //更新能量
                    read_energy_update(6, input_id, now, array_list1, array_list2, array_list3);
                    //cout<<"data_read 76"<<endl;
                    return;
                }

            }
        }

        //读数
        return;
    }

    if (input_type == 3)//MA 存储
    {
        //增加数据交换次数
        //cout<<"data_read 77"<<endl;
        array_data_transfer(3, input_id, decide_array_type, decide_array_id,array_list1,array_list2, array_list3);
        //cout<<"data_read 78"<<endl;
        //看阵列是否是本阵列，不对应时需要读取！！
        if (decide_array_type != input_type || decide_array_id != input_id) {
            //cout<<"data_read 79"<<endl;
            array_list3[input_id].read_number++;
            //cout<<"data_read 80"<<endl;
            //更新时间
            read_time_update(input_type, input_id, time_n, now, array_list1, array_list2, array_list3);
            //cout<<"data_read 81"<<endl;
            //更新能量
            read_energy_update(input_type, input_id, now, array_list1, array_list2, array_list3);
            //cout<<"data_read 82"<<endl;
            return;
        }
    }


//以上都没有讨论直接连线的情况..
}


//更新节点的出度，一个节点入度最多是3，出度可以很大
//这个节点执行完毕了，其依赖的节点的出度就都会-1
void out_degree(Nodes *now) {
    if (now->depend1)
        now->depend1->out_degree--;
    if (now->depend2)
        now->depend2->out_degree--;
    if (now->control)
        now->control->out_degree--;
}

//数据存储的位置有几个
int num_node_position(Nodes *now) {
    int num = 0;
    for (const auto &i: now->wb_pos) {
        if (!i.empty())
            num += i.size();
    }
    return num;
}





