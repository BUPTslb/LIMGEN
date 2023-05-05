#include "logic_func.h"
#include "parameter.h"

//现在已经决定了从哪里找操作数，在哪里执行，可以直接进行时间更新
//注意，现在只是获取开始的时间
//这里只根据依赖的阵列、节点来获取时间，对reg,buffer更新不需要调用，直接在节点上更改
double time_now(vector<Node> &nodes2,vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                vector<magic_arr> &array_list3, Node *node_now, int decide_array_type, int decide_array_id) {
    //函数执行的操作：
    //执行阵列：更新开始时间和结束时间，将使用的阵列置为is_using=true
    //其他阵列，如果当前执行阵列的开始时间比其结束时间要大,将is_using置为false
    double time_n = node_now->end_time;//有初始时间
    double time1 = 0; //depend1的时间
    double time2 = 0; //depend2的时间
    double time3 = 0; //control的时间
    double time_do_array = 0; //阵列的时间
    cout << "time_now函数中的decide_array_type = " << decide_array_type << endl;
    //假设确定了使用的阵列，那节点开始执行的时间就是使用阵列的结束时间
    //虽然操作并行可能会让op类型比=先执行，但时间和能量均不影响
    switch (decide_array_type) {
        case 1: {
            time_do_array = array_list1[decide_array_id].over_time;
        }
            break;
        case 2:
            time_do_array = array_list2[decide_array_id].over_time;
            break;
        case 3:
            time_do_array = array_list3[decide_array_id].over_time;
            break;
        default:
            break;
    }
    cout << "time_now函数中阵列的over_time: " << time_do_array << endl;
    int op_type = op2int(node_now->operator_name);//操作类型
    //先看控制依赖
    if (node_now->control != nullptr) {
        time3 = node_now->control->end_time;
        time_n = max(time_n, time3);
    }
    cout << "控制依赖 time3 =" << time3 << endl;
    if (op_type == 0) //写操作,只有一个数据依赖，可能有控制依赖
    {
        if (node_now->depend1 != nullptr) //有依赖 A=b A=B OP C
        {
            //有依赖，这个节点是某一个操作的输出，其开始、结束时间直接等于依赖的结束时间
            if (op2int(node_now->depend1->operator_name) != 0) //依赖的是一个操作
            {
                if (node_now->start_time == 0)
                    node_now->start_time == node_now->depend1->end_time;

                time1 = node_now->depend1->end_time;
            } else //依赖的是一个值节点，还需要看其阵列
            {
                if ((time_do_array > time1 || time3 > time1) &&
                    wb_empty(node_now->depend1)) //阵列结束时间、控制依赖更晚，对于sa_out和lut-out需要在buffer中保存
                {
//写回 from_type表示写回的类型 0:Reg 1:lut-out 2:sa-out 3:magic存储 4:sa-buffer 5：lut-buffer 6:sa存储
                    write_back(nodes2,node_now->depend1->do_type, node_now->depend1->finish_id, node_now, array_list1,
                               array_list2, array_list3);
                    time_n = time_now(nodes2,array_list1, array_list2, array_list3, node_now, decide_array_type,
                                      decide_array_id);//更新时间
                }
            }

            time_n = max(time_n, time1);
        }

    } else //op操作
    {
        if (node_now->depend1 != nullptr) //有依赖
        {
            time1 = node_now->depend1->end_time;
            cout << "time1 = " << time1 << endl;
            cout << "wb_empty(node_now->depend1)没有问题：" << wb_empty(node_now->depend1) << endl;
            if ((time_do_array > time1 || time3 > time1) &&
                wb_empty(node_now->depend1)) //控制依赖更晚，对于sa_out和lut-out需要在buffer中保存
            {
//写回 from_type表示写回的类型 0:Reg 1:lut-out 2:sa-out 3:magic存储 4:lut-buffer 5：sa-buffer 6:sa存储
                if (node_now->depend1->do_type == 1 || node_now->depend1->do_type == 2) {
                    write_back(nodes2,node_now->depend1->do_type, node_now->depend1->finish_id, node_now->depend1,
                               array_list1, array_list2, array_list3);
                }
                cout << "time_now中的write_back没问题" << endl;
                time_n = time_now(nodes2,array_list1, array_list2, array_list3, node_now, decide_array_type,
                                  decide_array_id);//更新时间
            }


            time_n = max(time1, time_n);
        }
        if (node_now->depend2 != nullptr) //2有依赖
        {
            time2 = node_now->depend2->end_time;
            cout << "time2 = " << time2 << endl;
            if ((time_do_array > time2 || time3 > time2) &&
                wb_empty(node_now->depend2)) //阵列结束时间、控制依赖更晚，对于sa_out和lut-out需要在buffer中保存
            {
//写回 from_type表示写回的类型 0:Reg 1:lut-out 2:sa-out 3:magic存储 4:sa-buffer 5：lut-buffer 6:sa存储
                write_back(nodes2,node_now->depend2->do_type, node_now->depend2->finish_id, node_now->depend2,
                           array_list1, array_list2, array_list3);
                time_n = time_now(nodes2,array_list1, array_list2, array_list3, node_now, decide_array_type,
                                  decide_array_id);//更新时间
            }
            time_n = max(time_n, time2);
        }
        time_n = max(time_n, time_do_array);//数据依赖，执行阵列可用的最大时间为当前时间
        //对控制和等待进行讨论
        //1和2如果有时间差，需要将其中一个存储起来
        //存储后时间会被更新
        //对立即数进行讨论
        auto ft = ([=] {
            if (node_now->depend1 == nullptr || node_now->depend2 == nullptr)
                return 0;
            else {
                if (time1 > time2 && wb_empty(node_now->depend2)) return 1;//存储2
                if (time2 > time1 && wb_empty(node_now->depend1)) return 2;//存储1
                if (time1 == time2) return 0;
            }
        });
        if (node_now->depend2 != nullptr) //将2存储起来，但是2可能是个立即数，也可能已经存了
        {
            if (ft() == 1) {
                write_back(nodes2,node_now->depend2->do_type, node_now->depend2->finish_id, node_now->depend2, array_list1,
                           array_list2,array_list3);//不确定写到哪了
                //更新时间
                time_n = time_now(nodes2,array_list1, array_list2, array_list3, node_now, decide_array_type, decide_array_id);

            }
            //存储其中一个节点，write_in
        }
        if (node_now->depend1 != nullptr) //2
        {
            if (ft() == 2) { //存储其中一个节点，write_in
                write_back(nodes2,node_now->depend1->do_type, node_now->depend1->finish_id, node_now->depend1, array_list1,
                           array_list2,
                           array_list3);//不确定写到哪了
                //更新时间
                time_n = time_now(nodes2,array_list1, array_list2, array_list3, node_now, decide_array_type, decide_array_id);
            }
        }
//需要写回的全部写回完毕

    }

    //update the is_using of array
    for (auto i: array_list1) {
        if (i.over_time < time_n)
            i.is_using = false;
    }
    for (auto i: array_list2) {
        if (i.over_time < time_n)
            i.is_using = false;
    }
    for (auto i: array_list3) {
        if (i.over_time < time_n)
            i.is_using = false;
    }


    return time_n;

}



//只有写操作和执行的时候才会调用
void time_update(vector<Node> &nodes2,int op_type, int do_type, int decide_array_id, double time_now, Node *node_now,
                 vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    //set start time of node
    if (node_now->start_time == 0 || op_type == 0)
        node_now->start_time = time_now;
    if (op_type == 0) //写的操作,目标肯定只有 -1 3 4 5 6
    {
        //对不合理情况进行判断
        if (do_type == 1 || do_type == 2)
            cout << "warning: time_update出现了错误类型：" << do_type << endl;
        switch (do_type) {
            case -1://REG
            {
                node_now->end_time = time_now + reg.reg_write_time;//寄存器写
                cout << "写到寄存器后的时间为" << node_now->end_time << endl;
            }
                break;
            case 3://MAGIC
            {
                array_list3[decide_array_id].start_time = time_now;
                double time_up = ma_latency(op_type);
                node_now->end_time = time_now + time_up;
                cout << "写入magic后的时间为：" << node_now->end_time << endl;
                array_list3[decide_array_id].over_time = node_now->end_time;
                //set is_using
                array_list3[decide_array_id].is_using = true;
            }
                break;
            case 4://lut buffer
            {
                node_now->end_time = time_now + buffer.buffer_write_time * bit_num_operand;
                cout << "写到lut-buffer后的时间为：" << node_now->end_time << endl;
            }
                break;
            case 5://sa buffer
            {
                node_now->end_time = time_now + buffer.buffer_write_time * bit_num_operand;
                cout << "写到sa-buffer后的时间为：" << node_now->end_time << endl;
            }
                break;
            default:
                break;
        }

    }
    else
    {
        switch (do_type) {
            //对不合理情况进行判断
            if (do_type!=1&&do_type!=2&&do_type!=3)
                cout<<"warning: time_update出现了错误类型："<<do_type<<endl;
            case 1://lut
            {
                double time_up = lut_latency(op_type);//使用lut执行当前操作，所需的时间
                array_list1[decide_array_id].start_time = time_now;
                node_now->end_time = time_now + time_up;
                cout << "lut执行后的时间为：" << node_now->end_time << endl;
                array_list1[decide_array_id].over_time = node_now->end_time;
                //set is_using
                array_list1[decide_array_id].is_using = true;
            }
                break;
            case 2://SA
            {
                array_list2[decide_array_id].start_time = time_now;
                double time_up = sa_latency(op_type, array_list2[decide_array_id].sa_type);
                node_now->end_time = time_now + time_up;
                cout << "sa执行后的时间为：" << node_now->end_time << endl;
                array_list2[decide_array_id].over_time = node_now->end_time;
                //set is_using
                array_list2[decide_array_id].is_using = true;
            }
                break;
            case 3://MAGIC
            {
                array_list3[decide_array_id].start_time = time_now;
                double time_up = ma_latency(op_type);
                node_now->end_time = time_now + time_up;
                cout << "magic执行后的时间为：" << node_now->end_time << endl;
                array_list3[decide_array_id].over_time = node_now->end_time;
                //set is_using
                array_list3[decide_array_id].is_using = true;
            }
                break;
            default:
                break;
        }
    }


}

//每次读取都得调用这个函数更新时间
//这里的array_type和写回表一样 -1 1 2 3 4 5
//读，主要更新的是阵列的时间。阵列的时间对节点的时间产生连锁反应
void read_time_update(vector<Node> &nodes2,int array_type, int array_id, double time_now, Node *now, vector<lut_arr> &array_list1,
                      vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {

    //对不合理情况进行判断
    if (array_type==1||array_type==2)
        cout<<"warning: read_time_update出现了错误类型："<<array_type<<endl;
    switch (array_type) {
        case -1://REG
        {
            //寄存器读时间++,将其加到节点上
            now->end_time = max(now->end_time, time_now) + reg.reg_read_time;
        }
            break;
        case 3://MAGIC
        {
            array_list3[array_id].start_time = max(array_list3[array_id].start_time, time_now);
            double time_up = rram.read_time;
            now->end_time = time_now + time_up;
            array_list3[array_id].over_time = now->end_time;
//set is_using
            array_list3[array_id].is_using = true;
        }
            break;
        case 4://lut buffer
        {
            now->end_time = max(now->end_time, time_now) + buffer.buffer_read_time;
        }
            break;
        case 5://sa buffer
        {
            now->end_time = max(now->end_time, time_now) + buffer.buffer_read_time;
        }
            break;
        case 6: //sa
        {
            array_list2[array_id].start_time = max(array_list2[array_id].start_time, time_now);
            double time_up = sa_latency(8,array_list2[array_id].sa_type);
            now->end_time = time_now + time_up;
            array_list2[array_id].over_time = now->end_time;
//set is_using
            array_list2[array_id].is_using = true;
        }
            break;
        default:
            break;

    }
}

void read_energy_update(vector<Node> &nodes2,int array_type, int array_id, Node *node_now,
                        vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3) {
    //对不合理情况进行判断
    if (array_type==1||array_type==2)
        cout<<"warning: read_energy_update出现了错误类型："<<array_type<<endl;
    //读的一定是数，-1.reg 3.magic 4.lut-buffer 5.sa-buffer 6.sa
    switch (array_type) {
        case -1://REG
        {
            Reg_sum.read_energy_sum += reg.reg_read_energy;
        }
            break;
        case 3: //magic
        {
            array_list3[array_id].energy += bit_num_operand * rram.read_energy;
        }
            break;
        case 4://sa buffer
        {
            buffer_sum.buffer_read_energy += buffer.buffer_read_energy * bit_num_operand;
        }
            break;
        case 5://lut buffer
        {
            buffer_sum.buffer_read_energy += buffer.buffer_read_energy * bit_num_operand;
        }
            break;
        case 6:  //sa读取，和not（8）损耗的能量是一样的
        {
            array_list2[array_id].energy += sa_energy(8,array_list2[array_id].sa_type);
        }
            break;

        default:
            break;
    }

}

void energy_update(vector<Node> &nodes2,int op_type, int do_type, int array_id, vector<lut_arr> &array_list1, vector<sa_arr> &array_list2,
                   vector<magic_arr> &array_list3) {
    if (op_type == 0) //写的操作,目标肯定只有 -1 3 4 5 6
    {
        //对不合理情况进行判断
        if (do_type==1||do_type==2)
            cout<<"warning: energy_update出现了错误类型："<<do_type<<endl;
        switch (do_type) {
            case -1://REG
            {
                Reg_sum.write_energy_sum += reg.reg_write_energy;
            }
                break;
            case 3: //magic
            {
                array_list3[array_id].energy += ma_energy(op_type);
            }
                break;
            case 4://sa buffer
            {
                buffer_sum.buffer_write_energy += buffer.buffer_write_energy * bit_num_operand;
            }
                break;
            case 5://lut buffer
            {
                buffer_sum.buffer_write_energy += buffer.buffer_write_energy * bit_num_operand;
            }
                break;
            case 6://sa
            {
                array_list2[array_id].energy+= sa_energy(0,array_list2[array_id].sa_type);
            }
                break;
            default:
                break;
        }
    }
    else //执行操作，目标应该只有 1 2 3
    {
        //对不合理情况进行判断
        if (do_type!=1&&do_type!=2&&do_type!=3)
            cout<<"warning: energy_update出现了错误类型："<<do_type<<endl;
        switch (do_type) {
            case 1: //lut执行
            {
                array_list1[array_id].energy += lut_energy(op_type);
            }
                break;
            case 2: //sa
            {
                array_list2[array_id].energy += sa_energy(op_type, array_list2[array_id].sa_type);
            }
                break;
            case 3: //magic
            {
                array_list3[array_id].energy += ma_energy(op_type);
            }
                break;
            default:
                break;
        }
    }


}



//
// Created by shenlibo on 23-4-19.
//
