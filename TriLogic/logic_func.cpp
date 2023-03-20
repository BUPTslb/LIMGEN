#include"logic_func.h"

using namespace std;

unsigned int arr_size(int logic_type,unsigned int bit_num_operand){
    unsigned int arr_size;
    switch (logic_type) {
        case 1://lut,只有四输入和六输入
            arr_size = (2*bit_num_operand <= 16) ? 16 : 64;
            break;
        case 2://sa,列数=位数
        {
            unsigned m = 1;
            while (m < bit_num_operand) {
                m <<= 1;
            }
            arr_size = (m <= 64) ? m : 64;
            break;
        }
        case 3://尽量等于2的幂
        {
            unsigned m = 1;
            while (m < bit_num_operand) {
                m <<= 1;
            }
            arr_size = (m <= 64) ? m : 64;
            break;
        }
        default:
            break;

    }
}
void com_lut(int type_operation,int bit_num_operand,int op_num, \
    vector<lut_arr> &array_list1,vector<sa_arr> &array_list2,vector<magic_arr> &array_list3)
{
    //注：我没有考虑数据是从哪读到这里的
    if (array_list1.empty())// 当前阵列表为空
    {
        lut_arr now;//新建阵列
        now.array_type=1;//lut
        now.is_using=1;
        array_list1.push_back(now);
    }
    //对算子进行筛选
    if (type_operation==0)//赋值操作，写操作
    {


    }else if(type_operation==1||type_operation==2)//比较操作
    {


    }else if(type_operation==3||type_operation==4)
    {


    }else if(type_operation<=9 && type_operation>=5)//按位操作
    {
        //搜索之前的阵列，是否支持这个操作
        //如果其现在是未使用状态，并可以使用，则过
        for (int i = 0; i < array_list1.size(); ++i) {
            //该阵列当前未使用,且功能合适
            if(!array_list1[i].is_using && array_list1.back().op_type[type_operation]){
                //只增加时间和功耗
                array_list1[i].read_number++;
            }
        }

    }else if(type_operation==10)//向量加法操作
    {


    } else if (type_operation==11)//乘法操作
    {


    }

}


double Lut::Burn_cycle(char type,int bl,int wl)//烧录时间
{

            


}
double Lut::Burn_power(char type,int bl,int wl)//烧录功耗
{


}


int Magic::Magic_cycle(int type)
{
    int cycle_time;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type)
    {
        case 8://NOR
            cycle_time=1;
            break;
        case 7://NOT
            cycle_time=1;
            break;
        case 5://AND
            cycle_time=2;
            break;
        case 6://OR
            cycle_time=1;
            break;
        case 9://XOR
            cycle_time=3;
            break;
        case 10://+
            cycle_time=10;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_time;

}
double Magic::Magic_power(int type,int bl,int wl)
{
    int cycle_power;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type)
    {
        case 8://NOR
            cycle_power=10*wl;
            break;
        case 7://NOT
            cycle_power=5*wl;
            break;
        case 5://AND
            cycle_power=20*wl;
            break;
        case 6://OR
            cycle_power=5*wl;
            break;
        case 9://XOR
            cycle_power=30*wl;
            break;
        case 10://+
            cycle_power=100*wl;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_power;
}
int SA::SA_cycle(int type)
{
    int cycle_time;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type)
    {
        case 7://NOT
            cycle_time=1;
            break;
        case 5://AND
            cycle_time=1;
            break;
        case 6://OR
            cycle_time=1;
            break;
        case 9://XOR
            cycle_time=1;
            break;
        case 10://+
            cycle_time=10;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_time;
}
double SA::SA_power(int type,int bl,int wl)
{
    int cycle_power;
    //可否构建一个hash表用来存各逻辑操作的周期？
    switch (type)
    {
        case 7://NOT
            cycle_power=5*wl;
            break;
        case 5://AND
            cycle_power=20*wl;
            break;
        case 6://OR
            cycle_power=5*wl;
            break;
        case 9://XOR
            cycle_power=30*wl;
            break;
        case 10://+
            cycle_power=100*wl;
            break;
        default:
            break;
    }
    //如果遇到不支持的算子怎么办
    return cycle_power;
}
//cache一致性函数定义
bool cache_like(int array_type,int array_id,vector<vector<int>> &wb_pos){
    if (array_type==1)  return false;
    else if (array_type==2)
    {
        //遍历wb_pos[1],如果=id就命中
        if (wb_pos[1].empty())    return false;
        for (int i = 0; i < wb_pos[1].size(); ++i) {
            if (wb_pos[1][i]==array_id)
                return true;
        }
    }
    else
    {
        //遍历wb_pos[2],如果=id就命中
        if (wb_pos[2].empty())    return false;
        for (int i = 0; i < wb_pos[2].size(); ++i) {
            if (wb_pos[2][i]==array_id)
                return true;
        }
    }

}
//定义节点执行表更新策略
bool update_wb_pos(bool cache_like,int pos_input,int array_type,int pos_array)
{
    //cache命中 || 阵列类型为LUT
    if (cache_like || array_type==1)
        return false;
    else //不命中且在SA/MAGIC
        return true;
}
//寻找输入操作数的类型ID
void find_input(int &array_type,int &array_id,int op_type,Node* node_depend,int cycle){
    if (node_depend== nullptr)
    {
        if (op_type==0||op_type==7)//  one operand : = not
            return;
        else //two operand
        {
            array_type = -1;
            return;
        }
    }
    //添加迭代条件
    if (!node_depend->wb_pos[0].empty())
    {//遍历wb_pos
            int lut_depend_size=node_depend->wb_pos[0].size();
            array_type=1;
            array_id=node_depend->wb_pos[0].back();
    }
    if (!node_depend->wb_pos[1].empty())
    {//遍历wb_pos
        int sa_depend_size=node_depend->wb_pos[1].size();
        array_type=2;
        array_id=node_depend->wb_pos[1].back();
    }
    if (!node_depend->wb_pos[2].empty())
    {//遍历wb_pos
        int ma_depend_size=node_depend->wb_pos[2].size();
        array_type=3;
        array_id=node_depend->wb_pos[2].back();
    }
}
//定义决定执行阵列类型,**
int decide_array_type(int op_type,int design_target)
{
    int decide_array_type=0;
    if (1<=op_type && op_type<=4 || op_type==11)
    {
        decide_array_type=1;//移位，比较，选择lut
    }
    else if (op_type==8)
    {
        decide_array_type=3;
    }
    else if (op_type==9)
    {
        decide_array_type=2;
    }
    else
    {
        switch (design_target) {
            case 0:
                decide_array_type=1;
                break;
            case 1:
                decide_array_type=2;
                break;
            case 2:
                decide_array_type=3;
                break;
            default:
                break;
        }
    }

    return decide_array_type;
}
//定义执行阵列的id
int decide_array_id(int decide_array_type,\
                    vector<lut_arr> &array_list1,vector<sa_arr> &array_list2,vector<magic_arr> &array_list3, \
                    int input1_type,int input1_id,int input2_type,int input2_id)
{
/*当前的寻找策略
 * 如果操作数存放位置与执行阵列的类型相同，就在此执行
 * 否则找一个同类型能用的执行
 * 如果暂时都在使用，则新建一个阵列
 * 方案二：等待阵列执行完毕，寻找一个阵列执行，这样可以提供好面积利用率，待
 * */
    int decide_array_id=0;
    if (input2_type==0)//如果只有一个操作数,基本上执行的都是not操作
    {
        if(input1_type==-1)
        {
            //立即数，找一个能用的即可
            decide_array_id=find_no_using(decide_array_type,array_list1,array_list2,array_list3);
            //如果在等待，返回-1，建立新阵列
            if (decide_array_id==-1)
                decide_array_id=build(decide_array_type,array_list1,array_list2,array_list3);
        }
        else//不是立即数
        {
            if (input1_type==decide_array_type)//类型same
            {
                decide_array_id=input1_id;
            }
            else//unsame
            {
                decide_array_id=find_no_using(decide_array_type,array_list1,array_list2,array_list3);
                if (decide_array_id==-1)
                    decide_array_id=build(decide_array_type,array_list1,array_list2,array_list3);
            }
        }
    }
    else//two operands
    {
        if(input1_type==decide_array_type)
        {
            decide_array_id=input1_id;
        }
        else if(input2_type==decide_array_type)
        {
            decide_array_id=input2_id;
        }
        else//find a  no_using
        {
            decide_array_id=find_no_using(decide_array_type,array_list1,array_list2,array_list3);
            if (decide_array_id==-1)
                decide_array_id=build(decide_array_type,array_list1,array_list2,array_list3);
        }
    }
    return decide_array_id;
}
//找一个能用的
int find_no_using(int decide_array_type,vector<lut_arr> &array_list1,vector<sa_arr> &array_list2,vector<magic_arr> &array_list3)
{
    int find_no_using=-1;
    switch (decide_array_type) {
        case 1:
            for (int i = 0; i < array_list1.size(); ++i) {
                if (!array_list1[i].is_using)
                    find_no_using=i;
            }
            break;
        case 2:
            for (int i = 0; i < array_list2.size(); ++i) {
                if (!array_list2[i].is_using)
                    find_no_using=i;
            }
            break;
        case 3:
            for (int i = 0; i < array_list3.size(); ++i) {
                if (!array_list3[i].is_using)
                    find_no_using=i;
            }
            break;
        default:
            break;
    }
    return find_no_using;
}
//定义建立逻辑
int build(int decide_array_type,vector<lut_arr> &array_list1,\
                vector<sa_arr> &array_list2,vector<magic_arr> &array_list3)
{
    int build;
    switch (decide_array_type) {
        case 1:{
                lut_arr now1;
                array_list1.push_back(now1);
                build=array_list1.size()-1;
            break;
        }
        case 2:{
            sa_arr now2;
            array_list2.push_back(now2);
            build=array_list2.size()-1;
            break;
        }
        case 3: {
            magic_arr now3;
            array_list3.push_back(now3);
            build=array_list3.size()-1;
            break;
        }
        default:
            break;
    }
    return build;
}
//定义数据读函数,SA、lUT需要比较是否是当前输出
void data_read(int input_type,int op_id,int input_id,vector<lut_arr> &array_list1,vector<sa_arr> &array_list2,vector<magic_arr> &array_list3)
{
    if (input_type==3)//MA 存储
    {

        //读数逻辑
    }
    else if (input_type==2)
    {
        if (array_list2[input_id].sa_latch==op_id)
        {
                //直接连线
        }
        else{
            //读数逻辑
        }
    }
    else//LUT,register
    {
        if (array_list1[input_id].lut_out==op_id)
        {
            //直接连线
        }else{
            //读数逻辑
        }
    }

}