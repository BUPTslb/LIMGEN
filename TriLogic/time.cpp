#include "logic_func.h"
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

//TODO:设置时间库，直接引用
void time_update(int op_type,int decide_array_type, int decide_array_id, double time_now,Node* node_now,
                 vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3)
{
    //set start time of node
    node_now->start_time=time_now;
    if (decide_array_id==-1&&decide_array_type==-1) //写到寄存器中
    {
        //TODO:设置为写到寄存器中的时间
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
                node_now->end_time=time_now+1;
            if (op_type==3||op_type==4)
                node_now->end_time=time_now+bit_num_operand;
            if (op_type==10)
                node_now->end_time=time_now+bit_num_operand*2+4;
            if (op_type==11)
                node_now->end_time=time_now+(bit_num_operand*2+4)*bit_num_operand;

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
                node_now->end_time=time_now+bit_num_operand;
            if (op_type==8)//nor
                node_now->end_time=time_now+2;
            if (op_type==10)
                node_now->end_time=time_now+bit_num_operand*2+4;
            if (op_type==11)
                node_now->end_time=time_now+(bit_num_operand*2+4)*bit_num_operand;

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
                node_now->end_time=time_now+bit_num_operand*2+4;
            if (op_type==11)
                node_now->end_time=time_now+(bit_num_operand*2+4)*bit_num_operand;

            array_list3[decide_array_id].over_time=node_now->end_time;
            //set is_using
            array_list3[decide_array_id].is_using=true;
        }
            break;
        default:
            break;

    }
}



//
// Created by shenlibo on 23-4-19.
//
