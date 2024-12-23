#include "logic_func.h"
#include <deque>

//magic和sa的写覆盖
//应该将需要写覆盖多少行也加入，会出现只有一部分覆盖的情况
void write_cover(int op_type, Nodes *now, int pos_array, int pos_id, int row_need,
                 vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    //cout<<"write_over 0"<<endl;
    deque<int> erase_node_list;//队列头部先擦除，pop_front()
    //cout<<"write_over 1"<<endl;
    switch (pos_array) {
        case 2: {
            //cout<<"write_over 2"<<endl;
            //先存储出度为0的，然后是其他地方存的有的（这个cap_array_cover函数中有）
            for (auto i:array_list2[pos_id].store_node) {
                //cout<<"write_over 3"<<endl;
                if (find_node_by_number(i)->out_degree == 0) //节点出度为0，不再被需要
                {
                    //cout<<"write_over 4"<<endl;
                    erase_node_list.push_front(find_node_by_number(i)->node_id);
                    //cout<<"write_over 5"<<endl;
                }
                //出度不为0，但是多个地方存的都有
                if (find_node_by_number(i)->out_degree > 0 && num_node_position(find_node_by_number(i)) > 1) {
                    //cout<<"write_over 6"<<endl;
                    erase_node_list.push_back(i);
                    //cout<<"write_over 7"<<endl;
                }
            }
        }
            break;
        case 3: {
            //cout<<"write_over 8"<<endl;
            //先存储出度为0的，然后是其他地方存的有的（这个cap_array_cover函数中有）
            for (auto i:array_list3[pos_id].store_node) {
                //cout<<"write_over 9"<<endl;
                if (find_node_by_number(i)->out_degree == 0) //节点出度为0，不再被需要
                {
                    //cout<<"write_over 10"<<endl;
                    erase_node_list.push_front(i);
                    //cout<<"write_over 11"<<endl;
                }
                //出度不为0，但是多个地方存的都有
                if (find_node_by_number(i)->out_degree > 0 && num_node_position(find_node_by_number(i)) > 1) {
                    //cout<<"write_over 12"<<endl;
                    erase_node_list.push_back(i);
                    //cout<<"write_over 13"<<endl;
                }
            }
        }
            break;
        default:
            break;

    }
    //按顺序覆盖，阵列的存储节点、节点的写回阵列都要更新
    //擦除的阵列的写回表  阵列存储表也要更新

    if (!erase_node_list.empty())
    {
        cout<< erase_node_list.size() << endl;
    }
    if (op_type == 0)  //正常的节点变量写入，只需要占用一行
    {
        //写入sa
        //cout<<"write_over 14"<<endl;
        if (pos_array == 2) {
            //cout<<"write_over 15"<<endl;
            int erase_node = erase_node_list.front();
            //cout<<"write_over 16"<<endl;
            //第一个出队
            erase_node_list.pop_front();
            //cout<<"write_over 17"<<endl;
            //将该阵列从被擦除的节点的写回表中擦除
            wb_erase(find_node_by_number(erase_node), pos_array, pos_id);
            //cout<<"write_over 18"<<endl;
            //更新do_type和finish_id
            if (find_node_by_number(erase_node)->do_type == 6 &&
                find_node_by_number(erase_node)->finish_id == pos_id) {
                //cout<<"write_over 19"<<endl;
                //写回表空了，初始化为0
                if (wb_empty(find_node_by_number(erase_node))) {
                    //cout<<"write_over 20"<<endl;
                    find_node_by_number(erase_node)->do_type = 0;
                    find_node_by_number(erase_node)->finish_id = 0;
                    //cout<<"write_over 21"<<endl;
                    if (find_node_by_number(erase_node)->out_degree > 0) {
                        //cout<<"write_over 22"<<endl;
                        write_back(pos_array, pos_id, find_node_by_number(erase_node), array_list1, array_list2,
                                   array_list3);
                        //cout<<"write_over 23"<<endl;
                    }
                } else {
                    //更新do_type finish_id
                    vector<int> do_type_ready;
                    for (int i = 0; i < 6; ++i) {
                        //cout<<"write_over 24"<<endl;
                        if (!find_node_by_number(erase_node)->wb_pos[i].empty()) {
                            //cout<<"write_over 25"<<endl;
                            switch (i) {
                                case 0:
                                {
                                    //cout<<"write_over 26"<<endl;
                                    do_type_ready.push_back(-1);
                                    //cout<<"write_over 27"<<endl;
                                }
                                    break;
                                case 2:
                                {
                                    //cout<<"write_over 28"<<endl;
                                    do_type_ready.push_back(6);
                                    //cout<<"write_over 29"<<endl;
                                }
                                    break;
                                case 3:
                                {
                                    //cout<<"write_over 30"<<endl;
                                    do_type_ready.push_back(3);
                                    //cout<<"write_over 31"<<endl;
                                }
                                    break;
                                case 4:
                                {
                                    //cout<<"write_over 32"<<endl;
                                    do_type_ready.push_back(4);
                                    //cout<<"write_over 33"<<endl;
                                }
                                    break;
                                case 5:
                                {
                                    //cout<<"write_over 34"<<endl;
                                    do_type_ready.push_back(5);
                                    //cout<<"write_over 35"<<endl;
                                }
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                    //cout<<"write_over 36"<<endl;
                    int do_type_chosen = do_type_ready[rand() % do_type_ready.size()];
                    //cout<<"write_over 37"<<endl;
                    find_node_by_number(erase_node)->do_type = do_type_chosen;
                    //cout<<"write_over 38"<<endl;
                    //do_type_chosen不能用来对wb_pos索引
                    //先进行变换
                    if (do_type_chosen==-1) //reg
                    {
                        //cout<<"write_over 39"<<endl;
                        int wb_i_size = find_node_by_number(erase_node)->wb_pos[0].size();
                        //cout<<"write_over 40"<<endl;
                        find_node_by_number(erase_node)->finish_id = find_node_by_number(
                                erase_node)->wb_pos[0][rand() % wb_i_size];
                        //cout<<"write_over 41"<<endl;
                    }
                    else if(do_type_chosen==6) //sa存储
                    {
                        //cout<<"write_over 42"<<endl;
                        int wb_i_size = find_node_by_number(erase_node)->wb_pos[2].size();
                        //cout<<"write_over 43"<<endl;
                        find_node_by_number(erase_node)->finish_id = find_node_by_number(
                                erase_node)->wb_pos[2][rand() % wb_i_size];
                        //cout<<"write_over 44"<<endl;
                    }
                    else {
                        //cout<<"write_over 45"<<endl;
                        int wb_i_size = find_node_by_number(erase_node)->wb_pos[do_type_chosen].size();
                        //cout<<"write_over 46"<<endl;
                        find_node_by_number(erase_node)->finish_id = find_node_by_number(
                                erase_node)->wb_pos[do_type_chosen][rand() % wb_i_size];
                        //cout<<"write_over 47"<<endl;
                    }
                }
            }
            //将此节点从阵列的存储节点中擦除
            //cout<<"write_over 48"<<endl;
            array_erase_node(erase_node, pos_array, pos_id, array_list1, array_list2, array_list3);
            //cout<<"write_over 49"<<endl;
            //更新阵列的存储节点表
            array_add_node(pos_array, pos_id, now, array_list1, array_list2, array_list3);
            //cout<<"write_over 50"<<endl;
            //更新时间
            double time_n = time_now(array_list1, array_list2, array_list3, now, pos_array, pos_id);
            //cout<<"write_over 51"<<endl;
            time_update(0, 6, pos_id, time_n, now, array_list1, array_list2, array_list3);
            //cout<<"write_over 52"<<endl;
            //更新能量
            energy_update(0, 6, pos_id, array_list1, array_list2, array_list3);
            //cout<<"write_over 53"<<endl;
            //更新节点的do_type,存储到sa阵列：6
            now->do_type = 6;
            //更新节点的finish_id
            now->finish_id = pos_id;
            //更新节点的写回表
            now->wb_pos[2].push_back(pos_id);
            //cout<<"write_over 54"<<endl;

        }
        //写入magic
        if (pos_array == 3) {
            //取第一个
            //cout<<"write_over 55"<<endl;
            int erase_node = erase_node_list.front();
            //第一个出队
            //cout<<"write_over 56"<<endl;
            erase_node_list.pop_front();
            //cout<<"write_over 57"<<endl;
            //将该阵列从此节点的写回表中擦除
            wb_erase(find_node_by_number(erase_node), pos_array, pos_id);
            //cout<<"write_over 58"<<endl;
            //更新被擦除节点的参数 do_type和finish_id
            if (find_node_by_number(erase_node)->do_type == 3 &&
                find_node_by_number(erase_node)->finish_id == pos_id) {
                //写回表空了，初始化为0
                //cout<<"write_over 59"<<endl;
                if (wb_empty(find_node_by_number(erase_node))) {
                    //cout<<"write_over 60"<<endl;
                    find_node_by_number(erase_node)->do_type = 0;
                    //cout<<"write_over 61"<<endl;
                    find_node_by_number(erase_node)->finish_id = 0;
                    //cout<<"write_over 62"<<endl;
                    if (find_node_by_number(erase_node)->out_degree > 0) {
                        //cout<<"write_over 63"<<endl;
                        write_back(pos_array, pos_id, find_node_by_number(erase_node), array_list1, array_list2,
                                   array_list3);
                        //cout<<"write_over 64"<<endl;
                    }
                } else {
                    //更新do_type finish_id
                    //cout<<"write_over 65"<<endl;
                    vector<int> do_type_ready;
                    for (int i = 0; i < 6; ++i) {
                        //cout<<"write_over 66"<<endl;
                        if (!find_node_by_number(erase_node)->wb_pos[i].empty()) {
                            switch (i) {
                                case 0:
                                {
                                    //cout<<"write_over 67"<<endl;
                                    do_type_ready.push_back(-1);
                                    //cout<<"write_over 68"<<endl;
                                }
                                    break;
                                case 2:
                                {
                                    //cout<<"write_over 69"<<endl;
                                    do_type_ready.push_back(6);
                                    //cout<<"write_over 70"<<endl;
                                }
                                    break;
                                case 3:
                                {
                                    //cout<<"write_over 71"<<endl;
                                    do_type_ready.push_back(3);
                                    //cout<<"write_over 72"<<endl;
                                }
                                    break;
                                case 4:
                                {
                                    //cout<<"write_over 73"<<endl;
                                    do_type_ready.push_back(4);
                                    //cout<<"write_over 74"<<endl;
                                }
                                    break;
                                case 5:
                                {
                                    //cout<<"write_over 75"<<endl;
                                    do_type_ready.push_back(5);
                                    //cout<<"write_over 76"<<endl;
                                }
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                    int do_type_chosen = do_type_ready[rand() % do_type_ready.size()];
                    //cout<<"write_over 77"<<endl;
                    find_node_by_number(erase_node)->do_type = do_type_chosen;
                    //cout<<"write_over 78"<<endl;
                    //do_type_chosen不能用来对wb_pos索引
                    //先进行变换
                    if (do_type_chosen==-1) //reg
                    {
                        //cout<<"write_over 79"<<endl;
                        int wb_i_size = find_node_by_number(erase_node)->wb_pos[0].size();
                        //cout<<"write_over 80"<<endl;
                        find_node_by_number(erase_node)->finish_id = find_node_by_number(
                                erase_node)->wb_pos[0][rand() % wb_i_size];
                        //cout<<"write_over 81"<<endl;
                    }
                    else if(do_type_chosen==6) //sa存储
                    {
                        //cout<<"write_over 82"<<endl;
                        int wb_i_size = find_node_by_number(erase_node)->wb_pos[2].size();
                        //cout<<"write_over 83"<<endl;
                        find_node_by_number(erase_node)->finish_id = find_node_by_number(
                                erase_node)->wb_pos[2][rand() % wb_i_size];
                        //cout<<"write_over 84"<<endl;
                    }
                    else {
                        //cout<<"write_over 85"<<endl;
                        int wb_i_size = find_node_by_number(erase_node)->wb_pos[do_type_chosen].size();
                        //cout<<"write_over 86"<<endl;
                        find_node_by_number(erase_node)->finish_id = find_node_by_number(
                                erase_node)->wb_pos[do_type_chosen][rand() % wb_i_size];
                        //cout<<"write_over 87"<<endl;
                    }
                }
            }
            //将此节点从阵列的存储节点中擦除
            //cout<<"write_over 88"<<endl;
            array_erase_node(erase_node, pos_array, pos_id, array_list1, array_list2, array_list3);
            //cout<<"write_over 89"<<endl;
            //更新阵列的存储节点表
            array_add_node(pos_array, pos_id, now, array_list1, array_list2, array_list3);
            //cout<<"write_over 90"<<endl;
            //更新时间
            double time_n = time_now(array_list1, array_list2, array_list3, now, pos_array, pos_id);
            //cout<<"write_over 91"<<endl;
            time_update(op_type, pos_array, pos_id, time_n, now, array_list1, array_list2, array_list3);
            //cout<<"write_over 92"<<endl;
            //更新能量
            energy_update(op_type, pos_array, pos_id, array_list1, array_list2, array_list3);
            //cout<<"write_over 93"<<endl;
            //更新节点的do_type,存储到magic阵列：3
            now->do_type = 3;
            //更新节点的finish_id
            now->finish_id = pos_id;
            //更新节点的写回表
            now->wb_pos[3].push_back(pos_id);
        }
    } else //执行操作中的立即数写入,只覆盖，删除节点即可
    {
        //特殊情况：add等，要覆盖多个
        //需要使用变量：row_need
        if (pos_array == 2) {
            for (int i = 0; i < row_need; ++i) {
                //cout<<"write_over 94"<<endl;
                //取第一个
                int erase_node = erase_node_list.front();
                //cout<<"write_over 95"<<endl;
                //第一个出队
                erase_node_list.pop_front();
                //cout<<"write_over 96"<<endl;
                //将该阵列从被擦除的节点的写回表中擦除
                wb_erase(find_node_by_number(erase_node), pos_array, pos_id);
                //cout<<"write_over 97"<<endl;
                //更新被擦除节点的do_type和finish_id
                if (find_node_by_number(erase_node)->do_type == 6 &&
                    find_node_by_number(erase_node)->finish_id == pos_id) {
                    //cout<<"write_over 98"<<endl;
                    //写回表空了，初始化为0
                    if (wb_empty(find_node_by_number(erase_node))) {
                        //cout<<"write_over 99"<<endl;
                        find_node_by_number(erase_node)->do_type = 0;
                        //cout<<"write_over 100"<<endl;
                        find_node_by_number(erase_node)->finish_id = 0;
                        //cout<<"write_over 101"<<endl;
                        if (find_node_by_number(erase_node)->out_degree > 0) {
                            //cout<<"write_over 102"<<endl;
                            write_back(pos_array, pos_id, find_node_by_number(erase_node), array_list1, array_list2,
                                       array_list3);
                            //cout<<"write_over 103"<<endl;
                        }
                    } else {
                        //更新do_type finish_id
                        vector<int> do_type_ready;
                        //cout<<"write_over 104"<<endl;
                        for (int i = 0; i < 6; ++i) {
                            //cout<<"write_over 105"<<endl;
                            if (!find_node_by_number(erase_node)->wb_pos[i].empty()) {
                                switch (i) {
                                    case 0:
                                    {
                                        //cout<<"write_over 106"<<endl;
                                        do_type_ready.push_back(-1);
                                        //cout<<"write_over 107"<<endl;
                                    }
                                        break;
                                    case 2:
                                    {
                                        //cout<<"write_over 108"<<endl;
                                        do_type_ready.push_back(6);
                                        //cout<<"write_over 109"<<endl;
                                    }
                                        break;
                                    case 3:
                                    {
                                        //cout<<"write_over 110"<<endl;
                                        do_type_ready.push_back(3);
                                        //cout<<"write_over 111"<<endl;
                                    }
                                        break;
                                    case 4:
                                    {
                                        //cout<<"write_over 112"<<endl;
                                        do_type_ready.push_back(4);
                                        //cout<<"write_over 113"<<endl;
                                    }
                                        break;
                                    case 5:
                                    {
                                        //cout<<"write_over 114"<<endl;
                                        do_type_ready.push_back(5);
                                        //cout<<"write_over 115"<<endl;
                                    }
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                        int do_type_chosen = do_type_ready[rand() % do_type_ready.size()];
                        //cout<<"write_over 116"<<endl;
                        find_node_by_number(erase_node)->do_type = do_type_chosen;
                        //cout<<"write_over 117"<<endl;
                        //do_type_chosen不能用来对wb_pos索引
                        //先进行变换
                        if (do_type_chosen==-1) //reg
                        {
                            //cout<<"write_over 118"<<endl;
                            int wb_i_size = find_node_by_number(erase_node)->wb_pos[0].size();
                            //cout<<"write_over 119"<<endl;
                            find_node_by_number(erase_node)->finish_id = find_node_by_number(
                                    erase_node)->wb_pos[0][rand() % wb_i_size];
                            //cout<<"write_over 120"<<endl;
                        }
                        else if(do_type_chosen==6) //sa存储
                        {
                            //cout<<"write_over 121"<<endl;
                            int wb_i_size = find_node_by_number(erase_node)->wb_pos[2].size();
                            //cout<<"write_over 122"<<endl;
                            find_node_by_number(erase_node)->finish_id = find_node_by_number(
                                    erase_node)->wb_pos[2][rand() % wb_i_size];
                            //cout<<"write_over 123"<<endl;
                        }
                        else {
                            //cout<<"write_over 124"<<endl;
                            int wb_i_size = find_node_by_number(erase_node)->wb_pos[do_type_chosen].size();
                            //cout<<"write_over 125"<<endl;
                            find_node_by_number(erase_node)->finish_id = find_node_by_number(
                                    erase_node)->wb_pos[do_type_chosen][rand() % wb_i_size];
                            //cout<<"write_over 126"<<endl;
                        }
                    }
                }
                //将此节点从阵列的存储节点中擦除
                //cout<<"write_over 127"<<endl;
                array_erase_node(erase_node, pos_array, pos_id, array_list1, array_list2, array_list3);
                //cout<<"write_over 128"<<endl;
            }
        }
        if (pos_array == 3) {
            for (int i = 0; i < row_need; ++i) {
                //取第一个
                //cout<<"write_over 129"<<endl;
                int erase_node = erase_node_list.front();
                //cout<<"write_over 130"<<endl;
                //第一个出队
                erase_node_list.pop_front();
                //cout<<"write_over 131"<<endl;
                //将该阵列从此节点的写回表中擦除
                wb_erase(find_node_by_number(erase_node), pos_array, pos_id);
                //cout<<"write_over 132"<<endl;
                //更新被擦除节点的参数 do_type和finish_id
                if (find_node_by_number(erase_node)->do_type == 3 &&
                    find_node_by_number(erase_node)->finish_id == pos_id) {
                    //cout<<"write_over 133"<<endl;
                    //写回表空了，初始化为0
                    if (wb_empty(find_node_by_number(erase_node))) {
                        //cout<<"write_over 134"<<endl;
                        find_node_by_number(erase_node)->do_type = 0;
                        find_node_by_number(erase_node)->finish_id = 0;
                        if (find_node_by_number(erase_node)->out_degree > 0) {
                            //cout<<"write_over 135"<<endl;
                            write_back(pos_array, pos_id, find_node_by_number(erase_node), array_list1, array_list2,
                                       array_list3);
                            //cout<<"write_over 136"<<endl;
                        }
                    } else {
                        //更新do_type finish_id
                        vector<int> do_type_ready;
                        for (int i = 0; i < 6; ++i) {
                            //cout<<"write_over 137"<<endl;
                            if (!find_node_by_number(erase_node)->wb_pos[i].empty()) {
                                switch (i) {
                                    case 0:
                                    {
                                        //cout<<"write_over 138"<<endl;
                                        do_type_ready.push_back(-1);
                                        //cout<<"write_over 139"<<endl;
                                    }
                                        break;
                                    case 2:
                                    {
                                        //cout<<"write_over 140"<<endl;
                                        do_type_ready.push_back(6);
                                        //cout<<"write_over 141"<<endl;
                                    }
                                        break;
                                    case 3:
                                    {
                                        //cout<<"write_over 142"<<endl;
                                        do_type_ready.push_back(3);
                                        //cout<<"write_over 143"<<endl;
                                    }
                                        break;
                                    case 4:
                                    {
                                        //cout<<"write_over 144"<<endl;
                                        do_type_ready.push_back(4);
                                        //cout<<"write_over 145"<<endl;
                                    }
                                        break;
                                    case 5:
                                    {
                                        //cout<<"write_over 146"<<endl;
                                        do_type_ready.push_back(5);
                                        //cout<<"write_over 147"<<endl;
                                    }
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                        int do_type_chosen = do_type_ready[rand() % do_type_ready.size()];
                        //cout<<"write_over 148"<<endl;
                        find_node_by_number(erase_node)->do_type = do_type_chosen;
                        //cout<<"write_over 149"<<endl;
                        //do_type_chosen不能用来对wb_pos索引
                        //先进行变换
                        if (do_type_chosen==-1) //reg
                        {
                            int wb_i_size = find_node_by_number(erase_node)->wb_pos[0].size();
                            //cout<<"write_over 150"<<endl;
                            find_node_by_number(erase_node)->finish_id = find_node_by_number(
                                    erase_node)->wb_pos[0][rand() % wb_i_size];
                            //cout<<"write_over 151"<<endl;
                        }
                        else if(do_type_chosen==6) //sa存储
                        {
                            int wb_i_size = find_node_by_number(erase_node)->wb_pos[2].size();
                            //cout<<"write_over 152"<<endl;
                            find_node_by_number(erase_node)->finish_id = find_node_by_number(
                                    erase_node)->wb_pos[2][rand() % wb_i_size];
                            //cout<<"write_over 153"<<endl;
                        }
                        else {
                            int wb_i_size = find_node_by_number(erase_node)->wb_pos[do_type_chosen].size();
                            //cout<<"write_over 154"<<endl;
                            find_node_by_number(erase_node)->finish_id = find_node_by_number(
                                    erase_node)->wb_pos[do_type_chosen][rand() % wb_i_size];
                            //cout<<"write_over 155"<<endl;
                        }
                    }
                }
                //将此节点从阵列的存储节点中擦除
                array_erase_node(erase_node, pos_array, pos_id, array_list1, array_list2, array_list3);
                //cout<<"write_over 156"<<endl;
            }
        }

    }

}


//写回 from_type表示写回的类型 -1:Reg 1:lut-out 2:sa-out 3:magic存储 4:lut-buffer 5：sa-buffer 6:sa存储
//back-type应该和wb_pos对应 0:reg 1:lut 2:sa 3:ma 4:lut-latch 5.sa-buffer
//更新：阵列的存储节点，时间，能量；节点的写回表，do_type,finish_id,时间
void write_back(int from_type, int from_id, Nodes *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                vector<magic_arr> &array_list3, int back_type, int back_id) {
//写到buffer中，如果buffer中已经有数据了如何处理？

    //先列出magic和sa所有能写的阵列，按照优先级进行排序
    auto cap = [&](int array_type, int array_id) {
        return cap_array_lost(array_type, array_id, array_list1, array_list2, array_list3) +
               cap_array_cover(array_type, array_id, array_list1, array_list2, array_list3);
    };
//    //cout<<"write_back--find_no_using"<<endl;
    vector<int> sa_no_using = find_no_using(0, 2, array_list1, array_list2, array_list3);
    vector<int> sa_waiting = waiting_array_list(0, 2, array_list1, array_list2, array_list3);
    //cout<<"write back -1 "<<endl;
    vector<int> sa_list;
    for (auto i: sa_no_using) {
        if (cap(2, i) > 0)
            sa_list.push_back(i);
    }
    for (auto i: sa_waiting) {
        if (cap(2, i) > 0)
            sa_list.push_back(i);
    }
    vector<int> ma_no_using = find_no_using(0, 3, array_list1, array_list2, array_list3);
    vector<int> ma_waiting = waiting_array_list(0, 3, array_list1, array_list2, array_list3);
    //cout<<"write back -2 "<<endl;
    vector<int> ma_list;
    for (int i=0; i<ma_no_using.size();i++) {
        if (cap(3, ma_no_using[i]) > 0)
            ma_list.push_back(ma_no_using[i]);
    }
    for (int i=0;i< ma_waiting.size();i++) {
        if (cap(3, ma_waiting[i]) > 0)
            ma_list.push_back(ma_waiting[i]);
    }
    //cout<<"write back -3 "<<endl;

    if (back_type == -2) { //没有指定写回的位置
        switch (from_type) {
            case -1: //来自寄存器，要写到阵列中
            {
                vector<int> ready_type = {2, 3};//sa和magic
                vector<int> ready_array;
                int write_type = ready_type[rand() % ready_type.size()];
                if (write_type == 2) {
                    //将所有能进行写入的阵列全部列出，进行DSE
                    ready_array = sa_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(2, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(-1, -1, now, array_list1, array_list2, array_list3, 2, pos);
                }
                if (write_type == 3) {
                    //将所有能进行写入的阵列全部列出，进行DSE
                    ready_array = ma_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(3, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(-1, -1, now, array_list1, array_list2, array_list3, 3, pos);
                }

            }
                break;
            case 1: //来自lut-out,写回顺位：4 lut-latch、3 ma\2 sa、0 reg
            {
//                vector<int> ready_type = {4, 2, 3, 0}; //写回的顺位
                vector<int> ready_type = {4, 2, 3}; //写回的顺位
                vector<int> ready_array;
                int write_type = ready_type[rand() % ready_type.size()];
                if (write_type == 4) //buffer,只写回自己阵列
                {
                    write_back(1, from_id, now, array_list1, array_list2, array_list3, 4, from_id);
                }
                if (write_type == 2) {
                    //将所有能进行写入的阵列全部列出，进行DSE
                    ready_array = sa_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(2, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(1, from_id, now, array_list1, array_list2, array_list3, 2, pos);
                }
                if (write_type == 3) {
                    //将所有能进行写入的阵列全部列出，进行DSE
                    ready_array = ma_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(3, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(1, from_id, now, array_list1, array_list2, array_list3, 3, pos);
                }
                if (write_type == 0) //写回寄存器
                {
                    write_back(1, from_id, now, array_list1, array_list2, array_list3, 0, -1);
                }

            }
                break;
            case 2://来自sa-out,写回顺位：sa-buffer、sa、reg、ma
            {
//                vector<int> ready_type = {5, 2, 3, 0};
                vector<int> ready_type = {5, 2, 3};
                vector<int> ready_array;
                int write_type = ready_type[rand() % ready_type.size()];
                //选择要写回的类型，如果选择了buffer
                if (write_type == 5) //buffer
                {
                    write_back(2, from_id, now, array_list1, array_list2, array_list3, 5, from_id);
                }
                if (write_type == 2) //sa
                {
                    //将所有能进行写入的阵列全部列出，进行DSE
                    ready_array = sa_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(2, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(2, from_id, now, array_list1, array_list2, array_list3, 2, pos);
                }
                if (write_type == 3) //magic
                {
                    //将所有能进行写入的阵列全部列出，进行DSE
                    ready_array = ma_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(3, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(2, from_id, now, array_list1, array_list2, array_list3, 3, pos);
                }
                if (write_type == 0) //写回寄存器
                {
                    write_back(2, from_id, now, array_list1, array_list2, array_list3, 0, -1);
                }

            }
                break;
            case 3: //来自magic存储 ,写回顺位：寄存器0、其他magic 3、sa存储 2
            {
//                vector<int> ready_type = {0, 2, 3};
                vector<int> ready_type = {2, 3};
                vector<int> ready_array;
                int write_type = ready_type[rand() % ready_type.size()];
                if (write_type == 0) {
                    write_back(3, from_id, now, array_list1, array_list2, array_list3, 0, -1);
                }
                if (write_type == 2) //写回sa
                {
                    ready_array = sa_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(2, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(3, from_id, now, array_list1, array_list2, array_list3, 2, pos);

                }
                if (write_type == 3) //写回magic
                {
                    ready_array = ma_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(3, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(3, from_id, now, array_list1, array_list2, array_list3, 3, pos);
                }


            }
                break;
            case 4: //来自lut-buffer。写回顺位：寄存器0、其他magic 3、sa存储 2
            {
//                vector<int> ready_type = {0, 2, 3};
                vector<int> ready_type = {2, 3};
                vector<int> ready_array;
                int write_type = ready_type[rand() % ready_type.size()];
                if (write_type == 0) {
                    write_back(4, from_id, now, array_list1, array_list2, array_list3, 0, -1);
                }
                if (write_type == 2) //写回sa
                {
                    ready_array = sa_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(2, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(4, from_id, now, array_list1, array_list2, array_list3, 2, pos);

                }
                if (write_type == 3) //写回magic
                {
                    ready_array = ma_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(3, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(4, from_id, now, array_list1, array_list2, array_list3, 3, pos);
                }

            }
                break;
            case 5: //来自sa-buffer。写回顺位：寄存器0、其他magic 3、sa存储 2
            {
//                vector<int> ready_type = {0, 2, 3};
                vector<int> ready_type = {2, 3};
                vector<int> ready_array;
                int write_type = ready_type[rand() % ready_type.size()];
                if (write_type == 0) {
                    write_back(5, from_id, now, array_list1, array_list2, array_list3, 0, -1);
                }
                if (write_type == 2) //写回sa
                {
                    ready_array = sa_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(2, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(5, from_id, now, array_list1, array_list2, array_list3, 2, pos);

                }
                if (write_type == 3) //写回magic
                {
                    ready_array = ma_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(3, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(5, from_id, now, array_list1, array_list2, array_list3, 3, pos);
                }


            }
                break;
            case 6: //来自sa存储，写回顺位：寄存器0、其他magic 3、sa存储 2
            {
//                vector<int> ready_type = {0, 2, 3};
                vector<int> ready_type = {2, 3};
                vector<int> ready_array;
                int write_type = ready_type[rand() % ready_type.size()];
                if (write_type == 0) {
                    write_back(6, from_id, now, array_list1, array_list2, array_list3, 0, -1);
                }
                if (write_type == 2) //写回sa
                {
                    ready_array = sa_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(2, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(6, from_id, now, array_list1, array_list2, array_list3, 2, pos);

                }
                if (write_type == 3) //写回magic
                {
                    ready_array = ma_list;
                    if (ready_array.empty())
                        ready_array.push_back(build(3, 0, array_list1, array_list2, array_list3));
                    int pos = ready_array[rand() % ready_array.size()];
                    write_back(6, from_id, now, array_list1, array_list2, array_list3, 3, pos);
                }

            }
                break;
            default:
                break;
        }
    } else //指定写回的位置
    {
        //cout<<"case " <<from_type<<endl;
        switch (from_type) {
            //在指定位置对数据交换次数进行更新

            case -1: //寄存器读，阵列写 2 3
            {

                //cout<<"start "<<endl;
                array_data_transfer(0, -1, back_type, back_id,array_list1, array_list2,array_list3);
                //cout<<"end "<<endl;
                //从寄存器中读取，更新能量

                Reg_sum.read_num_sum++;
                //将读取时间更新到节点身上
                //使用读时间更新函数
                double time_n = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                read_time_update(-1, -1, time_n, now, array_list1, array_list2, array_list3);
//                now->end_time += reg.reg_read_time;
                //更新整体的Reg能量
                read_energy_update(-1, -1, now, array_list1, array_list2, array_list3);
                //判断是否需要写覆盖
                if ((back_type == 2 || back_type == 3) &&
                    cap_array_lost(back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                    write_cover(0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                    return;
                }
                // 更新阵列写时间
                time_n = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                //更新时间,节点，阵列
                if (back_type==2)
                    time_update(0, 6, back_id, time_n, now, array_list1, array_list2, array_list3);
                else if (back_type==0)
                    time_update(0, -1, back_id, time_n, now, array_list1, array_list2, array_list3);
                else
                    time_update(0, back_type, back_id, time_n, now, array_list1, array_list2, array_list3);
                //阵列写：更新阵列能量
                if (back_type==2)
                    energy_update(0, 6, back_id, array_list1, array_list2, array_list3);
                else if (back_type==0)
                    energy_update(0, -1, back_id, array_list1, array_list2, array_list3);
                else
                    energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);

                //更新写回表
                now->wb_pos[back_type].push_back(back_id);
                //更新阵列的结束(执行)类型和位置
                now->do_type = back_type;
                //有两个特殊的处理一下
                if (back_type == 0) now->do_type = -1; //reg
                if (back_type == 2) now->do_type = 6;   //sa存储
                now->finish_id = back_id;
                //更新阵列存储节点
                array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);

            }
                break;
            case 1: //来自lut-out
            {
                switch (back_type) {
                    case 0://写回寄存器
                    {
                        //cout<<"1-0 start "<<endl;
                        array_data_transfer(1, from_id, 0, -1,array_list1, array_list2,array_list3);
                        //cout<<"1-0 end "<<endl;
                        //更新寄存器写次数
                        Reg_sum.write_num_sum++;
                        //更新寄存器能量
                        energy_update(0, -1, -1, array_list1, array_list2, array_list3);
                        //时间
                        double time_n = time_now(array_list1, array_list2, array_list3, now, -1, -1);
                        //更新时间,节点，阵列
                        time_update(0, -1, -1, time_n, now, array_list1, array_list2, array_list3);
                        //更新节点写回表
                        now->wb_pos[0].push_back(-1);

                        //更新阵列的结束(执行)类型和位置
                        now->do_type = -1;//表示写回的是寄存器
                        now->finish_id = back_id;

                    }
                        break;
                    case 2://写回sa
                    {
                        //cout<<"1-2 start "<<endl;
                        array_data_transfer(1,from_id,2,back_id,array_list1,array_list2,array_list3);
                        //cout<<"1-2 end "<<endl;
                        //阵列写次数++
                        array_list2[back_id].write_number++;
                        //判断是否需要写覆盖
                        if (cap_array_lost(back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                            return;
                        }
                        //时间
                        double time_n = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                        //更新时间,节点，阵列
                        time_update(0, 6, back_id, time_n, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, 6, back_id, array_list1, array_list2, array_list3);
                        //更新节点
                        //更新阵列的结束(执行)类型和位置
                        now->do_type = 6;//代表写回的是sa的存储中
                        now->finish_id = back_id;
                        //更新写回表
                        now->wb_pos[back_type].push_back(back_id);
                        array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);
                    }
                        break;
                    case 3: //写回magic
                    {
                        //cout<<"1-3 start "<<endl;
                        array_data_transfer(1,from_id,3,back_id,array_list1,array_list2,array_list3);
                        //cout<<"1-3 end "<<endl;
                        //阵列写次数++
                        array_list3[back_id].write_number++;
                        //判断是否需要写覆盖
                        if (cap_array_lost(back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                            return;
                        }
                        //时间
                        double time_n = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                        //更新时间,节点，阵列
                        time_update(0, back_type, back_id, time_n, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);
                        //更新阵列的结束(执行)类型和位置
                        now->do_type = 3;//代表写回的是sa的存储中
                        now->finish_id = back_id;
                        //更新写回表
                        now->wb_pos[back_type].push_back(back_id);
                        array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);

                    }
                        break;
                    case 4://lut-out 1 写回lut-buffer 4
                    {
                        //首先要判断buffer中有没有元素，其写回表、出度是否为0
                        //更新buffer伴随着写覆盖问题，写回表中的原来的buffer也要删除掉
                        //buffer：不被覆盖就可以一直存储着
                        if (!array_list1[back_id].lut_latch.empty() &&
                            (array_list1[back_id].lut_latch.back() != now->node_id)) {
                            //先将现在buffer对应的节点[不是now]的写回表中的这个buffer擦除掉
                            wb_erase(find_node_by_number(array_list1[back_id].lut_latch.back()), back_type, back_id);
                            //buffer被擦除了，do_type和finish_id是否也需要更新？
                            //如果其do_type和finish_id都是被擦除的，则将其do_type和finish_id都改为0
                            if (find_node_by_number(array_list1[back_id].lut_latch.back())->do_type==4
                                &&find_node_by_number(array_list1[back_id].lut_latch.back())->finish_id==back_id)
                            {
                                find_node_by_number(array_list1[back_id].lut_latch.back())->do_type = 0;
                                find_node_by_number(array_list1[back_id].lut_latch.back())->finish_id = 0;
                            }

                            //出度>0，写回表为空
                            if (find_node_by_number(array_list1[back_id].lut_latch.back())->out_degree > 0 &&
                                wb_empty(find_node_by_number(array_list1[back_id].lut_latch.back())))
                                //找个地方写回，from_type=4,lut-buffer
                            {
                                //从4.lut-buffer出去的，应该只能写回reg-sa-magic
                                write_back(4, back_id, find_node_by_number(array_list1[back_id].lut_latch.back()),
                                           array_list1, array_list2, array_list3);
                            }
                        }
                        //清空buffer
                        array_list1[back_id].lut_latch.clear();
                        //更新buffer
                        array_list1[back_id].lut_latch.push_back(now->node_id);//新的buffer输出
                        buffer_sum.buffer_write_sum++;//更新buffer写次数
                        energy_update(0, 4, back_id, array_list1, array_list2, array_list3); //更新buffer能量
                        //更新节点的end_time
                        double time_n = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                        time_update(0, back_type, back_id, time_n, now, array_list1, array_list2, array_list3);

                        //更新节点的结束(执行)类型和位置
                        now->do_type = 4;//代表写回的是lut-buffer
                        now->finish_id = back_id;
                        //更新写回表
                        //更新节点的写回表
                        now->wb_pos[back_type].push_back(back_id);

                    }
                        break;
                    default:
                        break;
                }

            }
                break;
            case 2: //来自sa-out 2
            {
                switch (back_type) {
                    case 0://来自sa-out 2 写回寄存器 0/-1
                    {
                        //cout<<"2-0 start "<<endl;
                        array_data_transfer(2, from_id, 0, -1,array_list1, array_list2,array_list3);
                        //cout<<"2-0 end "<<endl;
                        Reg_sum.write_num_sum++;
                        energy_update(0, -1, -1, array_list1, array_list2, array_list3);
                        //时间
                        double time_n = time_now(array_list1, array_list2, array_list3, now, -1, back_id);
                        //更新时间,节点，阵列
                        time_update(0, -1, -1, time_n, now, array_list1, array_list2, array_list3);
                        //更新节点的结束(执行)类型和位置
                        now->do_type = -1;//代表写回的是寄存器
                        now->finish_id = back_id;
                        //更新写回表
                        //阵列写：更新阵列能量
                        now->wb_pos[0].push_back(-1);
                    }
                        break;
                    case 2://来自sa-out 2 写回sa(6)
                    {
                        //cout<<"2-2 start "<<endl;
                        array_data_transfer(2,from_id,2,back_id,array_list1,array_list2,array_list3);
                        //cout<<"2-2 end "<<endl;
                        //阵列写次数++
                        array_list2[back_id].write_number++;
                        //判断是否需要写覆盖
                        if (cap_array_lost(back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                            return;
                        }
                        //时间
                        double time_n = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                        //更新时间,节点，阵列
                        time_update(0, 6, back_id, time_n, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, 6, back_id, array_list1, array_list2, array_list3);
                        //更新节点的结束(执行)类型和位置
                        now->do_type = 6;//代表写回的是sa存储
                        now->finish_id = back_id;
                        now->wb_pos[back_type].push_back(back_id);
                        array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);
                    }
                        break;
                    case 3: //来自sa-out 2 写回magic
                    {
                        //cout<<"2-3 start "<<endl;
                        array_data_transfer(2,from_id,3,back_id,array_list1,array_list2,array_list3);
                        //cout<<"2-3 end "<<endl;
                        //阵列写次数++
                        array_list3[back_id].write_number++;
                        //判断是否需要写覆盖
                        if (cap_array_lost(back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                            return;
                        }
                        //时间
                        double time_n = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                        //更新时间,节点，阵列
                        time_update(0, back_type, back_id, time_n, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);
                        //更新节点的结束(执行)类型和位置
                        now->do_type = 3;//代表写回的是magic
                        now->finish_id = back_id;
                        now->wb_pos[back_type].push_back(back_id);
                        array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);

                    }
                        break;
                    case 5://sa-out 写回sa-buffer
                    {
                        //首先要判断buffer中有没有元素，其写回表、出度是否为0
                        //更新buffer伴随着写覆盖问题，写回表中的原来的buffer也要删除掉
                        //buffer：不被覆盖就可以一直存储着
                        if (!array_list2[back_id].sa_buffer.empty() &&
                            (array_list2[back_id].sa_buffer.back() != now->node_id)) {
                            //先将现在buffer对应的节点[不是now]的写回表中的这个buffer擦除掉
                            wb_erase(find_node_by_number(array_list2[back_id].sa_buffer.back()), back_type, back_id);
                            //更改do_type和finish_id
                            if (find_node_by_number(array_list2[back_id].sa_buffer.back())->do_type==5
                                &&find_node_by_number(array_list2[back_id].sa_buffer.back())->finish_id==back_id)
                            {
                                find_node_by_number(array_list2[back_id].sa_buffer.back())->do_type = 0;
                                find_node_by_number(array_list2[back_id].sa_buffer.back())->finish_id = 0;
                            }
                            //出度>0，写回表为空
                            if (find_node_by_number(array_list2[back_id].sa_buffer.back())->out_degree > 0 &&
                                wb_empty(find_node_by_number(array_list2[back_id].sa_buffer.back())))
                                //找个地方写回，from_type=5,sa-buffer
                            {
                                write_back(5, back_id, find_node_by_number(array_list2[back_id].sa_buffer.back()), array_list1, array_list2, array_list3);
                            }

                        }
                        //将buffer中的原来的元素擦除掉
                        array_list2[back_id].sa_buffer.clear();
                        //更新buffer
                        array_list2[back_id].sa_buffer.push_back(now->node_id);//新的buffer输出
                        buffer_sum.buffer_write_sum++;//更新buffer写次数
                        energy_update(0, 5, back_id, array_list1, array_list2, array_list3); //更新buffer能量
                        //更新节点的end_time
                        double time_n = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                        time_update(0, back_type, back_id, time_n, now, array_list1, array_list2, array_list3);
                        //更新节点的结束(执行)类型和位置
                        now->do_type = 5;//代表写回的是sa-buffer
                        now->finish_id = back_id;
                        //更新节点的写回表
                        now->wb_pos[back_type].push_back(back_id);

                    }
                        break;
                    default:
                        break;
                }
            }
                break;
            case 3: //来自magic存储，magic读，寄存器、sa写
            {
                switch (back_type) {
                    case 0://来自magic 3 写回寄存器 0/-1
                    {
                        //cout<<"3-0 start "<<endl;
                        array_data_transfer(0, -1, back_type, back_id,array_list1, array_list2,array_list3);
                        //cout<<"3-0 end "<<endl;
                        //从阵列中读数据
                        array_list3[from_id].read_number++;
                        //阵列读时间更新
                        double time_n = time_now(array_list1, array_list2, array_list3, now, from_type, from_id);
                        read_time_update(3, from_id, time_n, now, array_list1, array_list2, array_list3);
                        //读能量更新
                        read_energy_update(3, from_id, now, array_list1, array_list2, array_list3);
                        //将数据写到寄存器中
                        Reg_sum.write_num_sum++;
                        //更新寄存器写时间
                        time_n = time_now(array_list1, array_list2, array_list3, now, from_type, from_id);
                        time_update(0, -1, -1, time_n, now, array_list1, array_list2, array_list3);
                        //更新寄存器能量
                        energy_update(0, -1, -1, array_list1, array_list2, array_list3);
                        //更新节点的结束(执行)类型和位置
                        now->do_type = -1;//代表写回的是寄存器
                        now->finish_id = back_id;
                        now->wb_pos[0].push_back(-1);
                    }
                        break;
                    case 2://来自magic 3 写回sa
                    {
                        //cout<<"3-2 start "<<endl;
                        array_data_transfer(3,from_id,2,back_id,array_list1,array_list2,array_list3);
                        //cout<<"3-2 end "<<endl;
                        //从阵列中读数据
                        array_list3[from_id].read_number++;
                        //判断是否需要写覆盖
                        if (cap_array_lost(back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                            return;
                        }
                        //阵列读时间更新
                        double time = time_now(array_list1, array_list2, array_list3, now, from_type, from_id);
                        time = max(time, time_now(array_list1, array_list2, array_list3, now, back_type, back_id));
                        read_time_update(3, from_id, time, now, array_list1, array_list2, array_list3);
                        //读能量更新
                        read_energy_update(3, from_id, now, array_list1, array_list2, array_list3);

                        //阵列写次数++
                        array_list2[back_id].write_number++;
                        //时间
                        time = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                        //更新时间,节点，阵列
                        time_update(0, 6, back_id, time, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, 6, back_id, array_list1, array_list2, array_list3);
                        //更新节点的结束(执行)类型和位置
                        now->do_type = 6;//代表写回的是sa阵列
                        now->finish_id = back_id;
                        //更新节点的写回表
                        now->wb_pos[back_type].push_back(back_id);
                        //更新阵列的存储表
                        array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);
                    }
                        break;
                    case 3: //来自magic 3 写回magic
                    {

                        //前提，两个magic不一样
                        if (from_id == back_id) break;
                        //cout<<"3-3 start "<<endl;
                        array_data_transfer(3,from_id,3,back_id,array_list1,array_list2,array_list3);
                        //cout<<"3-3 end "<<endl;
                        //判断是否需要写覆盖
                        if (cap_array_lost(back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                            return;
                        }
                        //从阵列中读数据
                        array_list3[from_id].read_number++;
                        //阵列读时间更新
                        double time = time_now(array_list1, array_list2, array_list3, now, from_type, from_id);

                        read_time_update(3, from_id, time, now, array_list1, array_list2, array_list3);
                        //读能量更新
                        read_energy_update(3, from_id, now, array_list1, array_list2, array_list3);

                        //阵列写次数++
                        array_list3[back_id].write_number++;
                        //时间
                        time = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                        //更新时间,节点，阵列
                        time_update(0, back_type, back_id, time, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);
                        //更新节点的结束(执行)类型和位置
                        now->do_type = 3;//代表写回的是magic
                        now->finish_id = back_id;
                        //更新节点的写回表
                        now->wb_pos[back_type].push_back(back_id);
                        //更新阵列的存储表
                        array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);

                    }
                        break;
                    default:
                        break;
                }
            }
                break;
            case 4: //来自lut-buffer，类似于寄存器，buffer读，阵列、寄存器写
            {
                //cout<<"4-- start "<<endl;
                array_data_transfer(1,from_id,back_type,back_id,array_list1,array_list2,array_list3);
                //cout<<"4-- end "<<endl;
                //从buffer中读取，更新能量
                buffer_sum.buffer_read_sum++;
                //将读取时间更新到节点身上
                //使用读时间更新函数
                double time_n = time_now(array_list1, array_list2, array_list3, now);
                read_time_update(4, from_id, time_n, now, array_list1, array_list2, array_list3);
//                now->end_time += reg.reg_read_time;
                //更新整体的buffer能量
                read_energy_update(4, from_id, now, array_list1, array_list2, array_list3);
                //判断是否有阵列写覆盖
                if ((back_type == 2 || back_type == 3) &&
                    cap_array_lost(back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                    write_cover(0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                    return;
                }
                // 更新阵列写时间
                time_n = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                //更新时间,节点，阵列
                if (back_type==2)
                    time_update(0, 6, back_id, time_n, now, array_list1, array_list2, array_list3);
                else if (back_type==0)
                    time_update(0, -1, back_id, time_n, now, array_list1, array_list2, array_list3);
                else
                    time_update(0, back_type, back_id, time_n, now, array_list1, array_list2, array_list3);
                //阵列写：更新阵列能量
                if (back_type==2)
                    energy_update(0, 6, back_id, array_list1, array_list2, array_list3);
                else if (back_type==0)
                    energy_update(0, -1, back_id, array_list1, array_list2, array_list3);
                else
                    energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);

                //更新阵列的结束(执行)类型和位置
                now->do_type = back_type;
                //有两个特殊的处理一下
                if (back_type == 0) now->do_type = -1; //reg
                if (back_type == 2) now->do_type = 6;   //sa存储
                now->finish_id = back_id;
                //更新写回表
                now->wb_pos[back_type].push_back(back_id);
                //更新阵列存储节点
                array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);

            }
                break;
            case 5: //来自sa-buffer,只能写回寄存器、sa、magic
            {
                //cout<<"5-- start "<<endl;
                array_data_transfer(2,from_id,back_type,back_id,array_list1,array_list2,array_list3);
                //cout<<"5-- end "<<endl;
                //从buffer中读取，更新能量
                buffer_sum.buffer_read_sum++;
                //将读取时间更新到节点身上
                //使用读时间更新函数
                double time_n = time_now(array_list1, array_list2, array_list3, now);
                read_time_update(5, from_id, time_n, now, array_list1, array_list2, array_list3);
//                now->end_time += reg.reg_read_time;
                //更新整体的buffer能量
                read_energy_update(5, from_id, now, array_list1, array_list2, array_list3);
                //判断是否有阵列写覆盖
                if ((back_type == 2 || back_type == 3) &&
                    cap_array_lost(back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                    //调用写覆盖要直接返回
                    write_cover(0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                    return ;
                }
                // 更新阵列写时间
                time_n = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                //更新时间,节点，阵列
                if (back_type==2)
                    time_update(0, 6, back_id, time_n, now, array_list1, array_list2, array_list3);
                else if (back_type==0)
                    time_update(0, -1, back_id, time_n, now, array_list1, array_list2, array_list3);
                else
                    time_update(0, back_type, back_id, time_n, now, array_list1, array_list2, array_list3);
                //阵列写：更新阵列能量
                if (back_type==2)
                    energy_update(0, 6, back_id, array_list1, array_list2, array_list3);
                else if (back_type==0)
                    energy_update(0, -1, back_id, array_list1, array_list2, array_list3);
                else
                    energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);

                now->wb_pos[back_type].push_back(back_id);
                //更新阵列的结束(执行)类型和位置
                now->do_type = back_type;
                //有两个特殊的处理一下
                if (back_type == 0) now->do_type = -1; //reg
                if (back_type == 2) now->do_type = 6;   //sa存储
                now->finish_id = back_id;
                //更新阵列存储节点
                array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);

            }
                break;
            case 6: //来自sa存储6，类似于magic3
            {
                switch (back_type) {
                    case 0://来自sa存储6 写回寄存器 0/-1
                    {
                        //cout<<"6-0 start "<<endl;
                        array_data_transfer(2, from_id, 0, -1,array_list1, array_list2,array_list3);
                        //cout<<"6-0 end "<<endl;
                        //从阵列中读数据
                        array_list2[from_id].read_number++;
                        //阵列读时间更新
                        double time_n = time_now(array_list1, array_list2, array_list3, now, from_type, from_id);
                        read_time_update(6, from_id, time_n, now, array_list1, array_list2, array_list3);
                        //读能量更新
                        read_energy_update(6, from_id, now, array_list1, array_list2, array_list3);
                        //将数据写到寄存器中
                        Reg_sum.write_num_sum++;
                        //更新寄存器写时间
                        time_n = time_now(array_list1, array_list2, array_list3, now);
                        time_update(0, -1, -1, time_n, now, array_list1, array_list2, array_list3);
                        //更新寄存器能量
                        energy_update(0, -1, -1, array_list1, array_list2, array_list3);
                        //更新阵列的结束(执行)类型和位置
                        now->do_type = -1;
                        now->finish_id = back_id;
                        now->wb_pos[0].push_back(-1);
                    }
                        break;
                    case 2://来自sa存储6 写回另一个sa
                    {
                        //cout<<"6-2 start "<<endl;
                        array_data_transfer(2,from_id,2,back_id,array_list1,array_list2,array_list3);
                        //cout<<"6-2 end "<<endl;
                        //前提，两个sa不一样
                        if (from_id == back_id) break;
                        //从阵列中读数据
                        array_list2[from_id].read_number++;
                        //阵列读时间更新
                        double time_n = time_now(array_list1, array_list2, array_list3, now, from_type, from_id);
                        read_time_update(6, from_id, time_n, now, array_list1, array_list2, array_list3);
                        //读能量更新
                        read_energy_update(6, from_id, now, array_list1, array_list2, array_list3);

                        //阵列写次数++
                        array_list2[back_id].write_number++;
                        //判断是否有阵列写覆盖
                        if ((back_type == 2 || back_type == 3) &&
                            cap_array_lost(back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                            return;
                        }
                        //时间
                        time_n = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                        //更新时间,节点，阵列
                        time_update(0, 6, back_id, time_n, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, 6, back_id, array_list1, array_list2, array_list3);
                        //更新阵列的结束(执行)类型和位置
                        now->do_type = 6;
                        now->finish_id = back_id;
                        //更新节点的写回表
                        now->wb_pos[back_type].push_back(back_id);
                        //更新阵列的存储表
                        array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);
                    }
                        break;
                    case 3: //来自sa存储6 写回magic
                    {
                        //cout<<"6-3 start "<<endl;
                        array_data_transfer(2,from_id,3,back_id,array_list1,array_list2,array_list3);
                        //cout<<"6-3 end "<<endl;
                        //从阵列中读数据
                        array_list3[from_id].read_number++;
                        //阵列读时间更新
                        double time_n = time_now(array_list1, array_list2, array_list3, now, from_type, from_id);
                        read_time_update(6, from_id, time_n, now, array_list1, array_list2, array_list3);
                        //读能量更新
                        read_energy_update(6, from_id, now, array_list1, array_list2, array_list3);

                        //阵列写次数++
                        array_list3[back_id].write_number++;
                        //判断是否有阵列写覆盖
                        if ((back_type == 2 || back_type == 3) &&
                            cap_array_lost(back_type, back_id, array_list1, array_list2, array_list3) == 0) {
                            write_cover(0, now, back_type, back_id, 1, array_list1, array_list2, array_list3);
                            return;
                        }
                        //时间
                        time_n = time_now(array_list1, array_list2, array_list3, now, back_type, back_id);
                        //更新时间,节点，阵列
                        time_update(0, back_type, back_id, time_n, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);
                        //更新阵列的结束(执行)类型和位置
                        now->do_type = back_type;
                        now->finish_id = back_id;
                        //更新节点的写回表
                        now->wb_pos[back_type].push_back(back_id);
                        //更新阵列的存储表
                        array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);

                    }
                        break;
                    default:
                        break;
                }
            }
                break;
            default:
                break;
        }

    }

}



//
// Created by shenlibo on 23-4-26.
//
