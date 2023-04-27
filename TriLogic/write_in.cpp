#include "logic_func.h"

//写覆盖
void write_cover(int op_type, Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                 vector<magic_arr> &array_list3) {


}

void wb_erase(Node *now, int erase_type, int erase_id) {
    cout << "before erase: " << now->wb_pos[erase_type].size() << endl;
    for (int i = 0; i < now->wb_pos[erase_type].size(); i++) {
        if (now->wb_pos[erase_type][i] == erase_id)
            now->wb_pos[erase_type].erase(now->wb_pos[erase_type].begin() + i);
    }
    cout << "after erase: " << now->wb_pos[erase_type].size() << endl;

}

//写回 from_type表示写回的类型 -1:Reg 1:lut-out 2:sa-out 3:magic存储 4:lut-buffer 5：sa-buffer 6:sa存储
//back-type应该和wb_pos对应 0:reg 1:lut 2:sa 3:ma 4:lut-latch 5.sa-buffer
void write_back(int from_type, int from_id, Node *now, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                vector<magic_arr> &array_list3, int back_type, int back_id) {
//TODO:可以写回多个地方：buffer/reg/ma/sa,DSE决定吧,所有的ready
//写到buffer中，如果buffer中已经有数据了如何处理？
//更新写回表
//更新阵列执行时间
//更新能量
    //先列出magic和sa所有能写的阵列，按照优先级进行排序
    auto cap = [&](int array_type, int array_id) {
        return cap_array_lost(array_type, array_id, array_list1, array_list2, array_list3) +
               cap_array_cover(array_type, array_id, array_list1, array_list2, array_list3);
    };
    vector<int> sa_no_using = find_no_using(0, 2, array_list1, array_list2, array_list3);
    vector<int> sa_waiting = waiting_array_list(0, 2, array_list1, array_list2, array_list3);
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
    vector<int> ma_list;
    for (auto i: ma_no_using) {
        if (cap(2, i) > 0)
            ma_list.push_back(i);
    }
    for (auto i: ma_waiting) {
        if (cap(2, i) > 0)
            ma_list.push_back(i);
    }

    if (back_type == -2) { //没有指定写回的位置
        switch (from_type) {
            case -1: //来自寄存器，要写到阵列中
            {
                //TODO:DSE
                vector<int> ready_type = {2, 3};//sa和magic
                vector<int> ready_array;
                int write_pos = ready_type[0];
                if (write_pos == 2) {
                    //将所有能进行写入的阵列全部列出，进行DSE
                    //TODO：想一想写回的优先级？
                    ready_array = sa_list;
                    int pos = ready_array[0];
                    write_back(-1, -1, now, array_list1, array_list2, array_list3, 2, pos);
                }
                if (write_pos == 3) {
                    //将所有能进行写入的阵列全部列出，进行DSE
                    ready_array = ma_list;
                    int pos = ready_array[0];
                    write_back(-1, -1, now, array_list1, array_list2, array_list3, 3, pos);
                }

            }
                break;
            case 1: //来自lut-out,写回顺位：lut-latch、ma\sa、reg
            {
                vector<int> ready = {4, 2, 3, 0};


            }
                break;
            case 2://来自sa-out,写回顺位：sa-buffer、sa、reg、ma
            {
                vector<int> ready = {5, 2, 3, 0};
                int chosen = ready[0];
                //选择要写回的类型，如果选择了buffer
                if (chosen == 5) //buffer
                {
                    if (!array_list2[now->depend1->finish_id].sa_buffer.empty()) //buffer中有数据
                    {
                        //将buffer中的数据写到别的地方,顺位：sa、ma、reg
                        vector<int> buffer_ready = {2, 3, 0};
                        int buffer_chosen = buffer_ready[0];//sa
                        if (buffer_chosen == 2) {
                            //最好写回本阵列
                            //如果本阵列的容量+可覆盖量!=0，可以写入
                            int cap =
                                    cap_array_lost(2, now->depend1->finish_id, array_list1, array_list2, array_list3) +
                                    cap_array_cover(2, now->depend1->finish_id, array_list1, array_list2, array_list3);
                            if (cap > 0) {

                            }


                        }


                    }
                }

            }
                break;
            case 3: //来自magic存储
            {

            }
                break;
            case 4: //来自lut-buffer
            {

            }
                break;
            case 5: //来自sa-buffer
            {


            }
                break;
            case 6: //来自sa存储
            {

            }
                break;
            default:
                break;
        }
    } else //指定写回的位置
    {
        switch (from_type) {
            case -1: //寄存器读，阵列写
            {
                //从寄存器中读取，更新能量
                Reg_sum.read_num_sum++;
                //将读取时间更新到节点身上
                //使用读时间更新函数
                double time = time_now(array_list1, array_list2, array_list3, now);
                read_time_update(-1,-1,time,now,array_list1,array_list2,array_list3);
//                now->end_time += reg.reg_read_time;
                //更新整体的Reg能量
                read_energy_update(-1, -1, now, array_list1, array_list2, array_list3);
                // 更新阵列写时间
                time = time_now(array_list1, array_list2, array_list3, now);
                //更新时间,节点，阵列
                time_update(0, back_type, back_id, time, now, array_list1, array_list2, array_list3);
                //阵列写：更新阵列能量
                energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);
                //更新写回表
                now->wb_pos[back_type].push_back(back_id);
                //更新阵列存储节点
                array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);

            }
                break;
            case 1: //来自lut-out
            {
                switch (back_type) {
                    case 0://写回寄存器
                    {
                        Reg_sum.write_num_sum++;
                        energy_update(0, -1, -1, array_list1, array_list2, array_list3);
                        //时间
                        double time = time_now(array_list1, array_list2, array_list3, now);
                        //更新时间,节点，阵列
                        time_update(0, -1, -1, time, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        now->wb_pos[0].push_back(-1);
                    }
                        break;
                    case 2://写回sa
                    {
                        //阵列写次数++
                        array_list2[back_id].write_number++;
                        //时间
                        double time = time_now(array_list1, array_list2, array_list3, now);
                        //更新时间,节点，阵列
                        time_update(0, back_type, back_id, time, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);
                        now->wb_pos[back_type].push_back(back_id);
                        array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);
                    }
                        break;
                    case 3: //写回magic
                    {
                        //阵列写次数++
                        array_list3[back_id].write_number++;
                        //时间
                        double time = time_now(array_list1, array_list2, array_list3, now);
                        //更新时间,节点，阵列
                        time_update(0, back_type, back_id, time, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);
                        now->wb_pos[back_type].push_back(back_id);
                        array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);

                    }
                        break;
                    case 4://lut-out 写回lut-buffer
                    {
                        //首先要判断buffer中有没有元素，其写回表、出度是否为0
                        //更新buffer伴随着写覆盖问题，写回表中的原来的buffer也要删除掉
                        //buffer：不被覆盖就可以一直存储着
                        if (!array_list1[back_id].lut_latch.empty() && array_list1[back_id].lut_latch.back() != now->node_id) {
                            //先将现在buffer对应的节点[不是now]的写回表中的这个buffer擦除掉
                            wb_erase(find_node_by_number(array_list1[back_id].lut_latch.back()), back_type, back_id);
                            //出度>0，写回表为空
                            if (find_node_by_number(array_list1[back_id].lut_latch.back())->out_degree > 0 &&
                                wb_empty(find_node_by_number(array_list1[back_id].lut_latch.back())))
                                //找个地方写回，from_type=4,lut-buffer
                            {
                                write_back(4, back_id, now, array_list1, array_list2, array_list3);
                            }
                        }
                        //清空buffer
                        array_list1[back_id].lut_latch.clear();
                        //更新buffer
                        array_list1[back_id].lut_latch.push_back(now->node_id);//新的buffer输出
                        buffer_sum.buffer_write_sum++;//更新buffer写次数
                        energy_update(0, 4, back_id, array_list1, array_list2, array_list3); //更新buffer能量
                        //更新节点的end_time
                        double time= time_now(array_list1,array_list2,array_list3, now);
                        time_update(0,back_type,back_id,time,now,array_list1,array_list2,array_list3);
//                        now->end_time += buffer.buffer_write_time;
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
                        Reg_sum.write_num_sum++;
                        energy_update(0, -1, -1, array_list1, array_list2, array_list3);
                        //时间
                        double time = time_now(array_list1, array_list2, array_list3, now);
                        //更新时间,节点，阵列
                        time_update(0, -1, -1, time, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        now->wb_pos[0].push_back(-1);
                    }
                        break;
                    case 2://来自sa-out 2 写回sa
                    {
                        //阵列写次数++
                        array_list2[back_id].write_number++;
                        //时间
                        double time = time_now(array_list1, array_list2, array_list3, now);
                        //更新时间,节点，阵列
                        time_update(0, back_type, back_id, time, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);
                        now->wb_pos[back_type].push_back(back_id);
                        array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);
                    }
                        break;
                    case 3: //来自sa-out 2 写回magic
                    {
                        //阵列写次数++
                        array_list3[back_id].write_number++;
                        //时间
                        double time = time_now(array_list1, array_list2, array_list3, now);
                        //更新时间,节点，阵列
                        time_update(0, back_type, back_id, time, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);
                        now->wb_pos[back_type].push_back(back_id);
                        array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);

                    }
                        break;
                    case 5://sa-out 写回sa-buffer
                    {
                        //首先要判断buffer中有没有元素，其写回表、出度是否为0
                        //更新buffer伴随着写覆盖问题，写回表中的原来的buffer也要删除掉
                        //buffer：不被覆盖就可以一直存储着
                        if (!array_list2[back_id].sa_buffer.empty() && array_list2[back_id].sa_buffer.back() != now->node_id) {
                            //先将现在buffer对应的节点[不是now]的写回表中的这个buffer擦除掉
                            wb_erase(find_node_by_number(array_list2[back_id].sa_buffer.back()), back_type, back_id);
                            //出度>0，写回表为空
                            if (find_node_by_number(array_list2[back_id].sa_buffer.back())->out_degree > 0 &&
                                wb_empty(find_node_by_number(array_list2[back_id].sa_buffer.back())))
                                //找个地方写回，from_type=5,sa-buffer
                            {
                                write_back(5, back_id, now, array_list1, array_list2, array_list3);
                            }

                        }
                        //将buffer中的原来的元素擦除掉
                        array_list2[back_id].sa_buffer.clear();
                        //更新buffer
                        array_list2[back_id].sa_buffer.push_back(now->node_id);//新的buffer输出
                        buffer_sum.buffer_write_sum++;//更新buffer写次数
                        energy_update(0, 5, back_id, array_list1, array_list2, array_list3); //更新buffer能量
                        //更新节点的end_time
                        double time= time_now(array_list1,array_list2,array_list3, now);
                        time_update(0,back_type,back_id,time,now,array_list1,array_list2,array_list3);
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
                        //从阵列中读数据
                        array_list3[from_id].read_number++;
                        //阵列读时间更新
                        double time = time_now(array_list1, array_list2, array_list3, now);
                        read_time_update(3,from_id,time,now,array_list1,array_list2,array_list3);
                        //读能量更新
                        read_energy_update(3,from_id,now,array_list1,array_list2,array_list3);
                        //将数据写到寄存器中
                        Reg_sum.write_num_sum++;
                        //更新寄存器写时间
                        time = time_now(array_list1, array_list2, array_list3, now);
                        time_update(0,-1,-1,time,now,array_list1,array_list2,array_list3);
                        //更新寄存器能量
                        energy_update(0,-1,-1,array_list1,array_list2,array_list3);

                        now->wb_pos[0].push_back(-1);
                    }
                        break;
                    case 2://来自magic 3 写回sa
                    {
                        //从阵列中读数据
                        array_list3[from_id].read_number++;
                        //阵列读时间更新
                        double time = time_now(array_list1, array_list2, array_list3, now);
                        read_time_update(3,from_id,time,now,array_list1,array_list2,array_list3);
                        //读能量更新
                        read_energy_update(3,from_id,now,array_list1,array_list2,array_list3);

                        //阵列写次数++
                        array_list2[back_id].write_number++;
                        //时间
                        time = time_now(array_list1, array_list2, array_list3, now);
                        //更新时间,节点，阵列
                        time_update(0, back_type, back_id, time, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);
                        //更新节点的写回表
                        now->wb_pos[back_type].push_back(back_id);
                        //更新阵列的存储表
                        array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);
                    }
                        break;
                    case 3: //来自magic 3 写回magic
                    {
                        //前提，两个magic不一样
                        if(from_id==back_id) break;
                        //从阵列中读数据
                        array_list3[from_id].read_number++;
                        //阵列读时间更新
                        double time = time_now(array_list1, array_list2, array_list3, now);
                        read_time_update(3,from_id,time,now,array_list1,array_list2,array_list3);
                        //读能量更新
                        read_energy_update(3,from_id,now,array_list1,array_list2,array_list3);

                        //阵列写次数++
                        array_list3[back_id].write_number++;
                        //时间
                        time = time_now(array_list1, array_list2, array_list3, now);
                        //更新时间,节点，阵列
                        time_update(0, back_type, back_id, time, now, array_list1, array_list2, array_list3);
                        //阵列写：更新阵列能量
                        energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);
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
                //从buffer中读取，更新能量
                buffer_sum.buffer_read_sum++;
                //将读取时间更新到节点身上
                //使用读时间更新函数
                double time = time_now(array_list1, array_list2, array_list3, now);
                read_time_update(4,from_id,time,now,array_list1,array_list2,array_list3);
//                now->end_time += reg.reg_read_time;
                //更新整体的Reg能量
                read_energy_update(4, from_id, now, array_list1, array_list2, array_list3);
                // 更新阵列写时间
                time = time_now(array_list1, array_list2, array_list3, now);
                //更新时间,节点，阵列
                if (back_type==0)    back_type=-1; //单独处理写回寄存器的类型
                time_update(0, back_type, back_id, time, now, array_list1, array_list2, array_list3);
                //阵列写：更新阵列能量
                energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);
                //更新写回表
                if (back_type==-1)    back_type=0;//将寄存器类型改回0
                now->wb_pos[back_type].push_back(back_id);
                //更新阵列存储节点
                array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);

            }
                break;
            case 5: //来自sa-buffer
            {
                //从buffer中读取，更新能量
                buffer_sum.buffer_read_sum++;
                //将读取时间更新到节点身上
                //使用读时间更新函数
                double time = time_now(array_list1, array_list2, array_list3, now);
                read_time_update(5,from_id,time,now,array_list1,array_list2,array_list3);
//                now->end_time += reg.reg_read_time;
                //更新整体的Reg能量
                read_energy_update(5, from_id, now, array_list1, array_list2, array_list3);
                // 更新阵列写时间
                time = time_now(array_list1, array_list2, array_list3, now);
                //更新时间,节点，阵列
                if (back_type==0)    back_type=-1; //单独处理写回寄存器的类型
                time_update(0, back_type, back_id, time, now, array_list1, array_list2, array_list3);
                //阵列写：更新阵列能量
                energy_update(0, back_type, back_id, array_list1, array_list2, array_list3);
                //更新写回表
                if (back_type==-1)    back_type=0;//将寄存器类型改回0
                now->wb_pos[back_type].push_back(back_id);
                //更新阵列存储节点
                array_add_node(back_type, back_id, now, array_list1, array_list2, array_list3);

            }
                break;
            case 6: //来自sa存储，类似于magic3
            {

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
