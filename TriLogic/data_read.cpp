#include"logic_func.h"
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


//
// Created by Lenovo on 2023/6/28.
//
