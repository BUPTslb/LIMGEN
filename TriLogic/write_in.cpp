#include "logic_func.h"
//写覆盖
void write_cover(int op_type,Node *now,vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3)
{



}

//写回 from_type表示写回的类型 -1:Reg 1:lut-out 2:sa-out 3:magic存储 4:sa-buffer 5：lut-buffer 6:sa存储
//back-type应该和wb_pos对应 0:reg 1:lut 2:sa 3:ma 4:lut-latch 5.sa-buffer
void write_back(int from_type,int from_id,Node *now,vector<lut_arr> &array_list1, vector<sa_arr> &array_list2, vector<magic_arr> &array_list3,int back_type,int back_id)
{
//TODO:可以写回多个地方：buffer/reg/ma/sa,DSE决定吧,所有的ready
//写到buffer中，如果buffer中已经有数据了如何处理？
//更新写回表
//更新阵列执行时间
//更新能量
    if (back_type==-2){ //没有指定写回的位置
        switch (from_type) {
            case -1: //来自寄存器，要写到阵列中
            {
                //TODO:DSE
                vector<int> ready={2,3};//sa和magic

            }
                break;
            case 1: //来自lut-out,写回顺位：lut-latch、ma\sa、reg
            {
                vector<int> ready={4,2,3,0};

            }
                break;
            case 2://来自sa-out,写回顺位：sa-buffer、sa、ma、reg
            {
                vector<int> ready={5,2,3,0};
                int chosen=ready[0];
                //选择要写回的类型，如果选择了buffer
                if (chosen==5) //buffer
                {
                    if (!array_list2[now->depend1->finish_id].sa_buffer.empty()) //buffer中有数据
                    {
                        //将buffer中的数据写到别的地方,顺位：sa、ma、reg
                        vector<int> buffer_ready={2,3,0};
                        int buffer_chosen=buffer_ready[0];//sa
                        if (buffer_chosen==2)
                        {
                            //最好写回本阵列
                            //如果本阵列的容量+可覆盖量!=0，可以写入
                            int cap=cap_array_lost(2,now->depend1->finish_id,array_list1,array_list2,array_list3)+
                                    cap_array_cover(2,now->depend1->finish_id,array_list1,array_list2,array_list3);
                            if (cap>0)
                            {
                                write_back(2,now,array_list1,array_list2,array_list3,buffer_chosen,int back_id);
                            }


                        }


                    }
                }

            }
                break;
            case 3:
            {

            }
                break;
            case 4:
            {

            }
                break;
            case 5:
            {


            }
                break;
            case 6:
            {

            }
                break;
            default:
                break;
        }
    }
    else //指定写回的位置
    {
        switch (from_type) {
            case 0: //来自寄存器，要写到阵列中
            {


            }
                break;
            case 1: //来自lut-out
            {
                switch (back_type) {
                    case 0://写回寄存器
                    {
                        Reg_sum.write_num_sum++;
                        Reg_sum.write_energy_sum+=reg.reg_write_energy;
                        now->wb_pos[0].push_back(-1);
                    }
                        break;
                    case 1:

                        break;
                    case 2:

                        break;
                    case 3:

                        break;
                    case 4:

                        break;
                    case 5:

                        break;
                    default:
                        break;
                }

            }
                break;
            case 2:
            {
                if (!array_list2[now->depend1->finish_id].sa_buffer.empty()) //buffer中有数据
                {

                }
            }
                break;
            case 3:
            {

            }
                break;
            case 4:
            {

            }
                break;
            case 5:
            {


            }
                break;
            case 6:
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
