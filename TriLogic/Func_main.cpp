#include<iostream>
#include<fstream>
#include<sstream>
#include<map>
#include<typeinfo>
#include"rapidjson/document.h"
#include"logic_func.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace std;

//设计空间探索的限制
double cycle_limit=500;//运行时间(速度)限制，单位：周期
double area_limit=1000;//面积限制，单位：
double power_limit=10;//功耗限制，单位：
/*节点中包含的字符串，用作匹配*/
string op= "Op";
string assign="Assign";
string branch="Branch";
string loop="Loop";

int main()
{
    //三种阵列使用的数量
    int Anum_magic;
    int Anum_sa; 
    int Anum_lut;
    //总的功耗
    double power;
    //总的周期
    int cycle;
    //总的面积
    double area;
    int Type2node(string type);
    /*******************************
    //使用各项资源的数量
    //控制器、解码器等
    //如何布局，要给出示意图来
    //使用剪枝算法进行DSE
    //没有使用过的逻辑/算术运算要进行拆解
    ********************************/

    //**********各类型节点数量****************
    int num_op=0;
    int num_assign=0;
    int num_loop=0;
    int num_branch=0;
    //构建键值对，存储
    map<string,int> operator_num;//各个算子出现的次数
    map<string,int> dst_id;//Dst出现的最后位置
    map<string,int> type_num;
    map<int,int> op_map;//存储op类型的id索引关系，input做键
    
    //1.先读一个节点让其输出
    ifstream file("../hdlAst/hdlast.json");
    stringstream buffer;
    buffer << file.rdbuf();
    string content(buffer.str());
    // 解析 JSON 数据
    Document d;
    d.Parse(content.c_str());
    // 测试rapidjson功能区*******************************************
        cout<<d[0]["Input"][1].GetString()<<endl;
        cout<<"IsArray的功能："<<d[1]["Input"].IsArray()<<endl;
        cout<<"IsString的功能："<<d[0]["Input"].IsString()<<endl;
        cout<<"IsString的功能："<<d[0]["Input"][1].IsString()<<endl;
        cout<<"IsString的功能："<<d[0]["Node_ID"].IsString()<<endl;
        cout<<"HasMember的功能："<<d[0].HasMember("Dst")<<endl;
    //***********************************************************    
    int json_size=d.Size();//整个数组的尺寸（节点的数量）
    
    //遍历json文件
    cout<<"The number of CDFG = "<<json_size<<endl;
    cout<<d[0]["Type"].GetString()<<endl;
    /************************************************
    //寻找可以并行操作的命令行
    //一样的变量同时写进两个不同的阵列中
    //不同的变量/执行也毫不相干，直接放在不同的阵列中执行
    //进行重新排序，然后再进行逻辑模式的套用  
    ************************************************/

    for(int i=0;i<json_size;i++){

        if(d[i]["Type"].GetString()== op)
        {
            num_op++;
            string key_op=d[i]["Operation"].GetString();
            //operator_num.insert(std::pair<string, int>(d[i]["Type"].GetString(),1));
            //调整hash表
            if(operator_num[key_op])//已经有该键
                operator_num[key_op]++;
            else
                operator_num[key_op]=1;
        }
        
        
    }
    cout<<"操作数hash表的大小："<<operator_num.size()<<endl;
    for(map<string,int>::iterator it=operator_num.begin();it!=operator_num.end();it++)
   {
       cout<<it->first<<" "<<it->second<<endl;
   }
//    cout<<"各个Dst操作数的位置"<<endl;
//     for(map<string,int>::iterator it=dst_id.begin();it!=dst_id.end();it++)
//    {
//        cout<<it->first<<" "<<it->second<<endl;
//    }
   /*****************************
   //关键算法
   //算子调度
   //输出控制步骤和节点
   //表明其可以并行执行
   ******************************/
    //如果全部串行
    int minnum_array=1;
    //先确定控制步的数目
    int comtrol_num=num_op;//初始为Op的数量
    
    for(int i=json_size-1;i>0;i--)
    {
        //遍历得出数据前后依赖关系
        //输出形式：节点->节点，没有依赖则不含“->”
        //1.判断input是否为节点ID
        //2.判断input是否为之前节点的Dst
        //使用的数据结构：？
        // cout<<"Typeof Input:"<<typeid(d[1]["Input"]).name()<<endl;
        int num_input;
        int type_id;

            //Input的数量和类型
            //Branch和loop没有input
            //如果有“Input”，得到input的数量
        type_id=Type2node(d[i]["Type"].GetString());
        map<int,bool> input_depend;//构建hash表，存储当前输入，如果前面找到了前驱依赖，值为true
        if(d[i].HasMember("Input"))
        {
            num_input=d[i]["Input"].IsArray() ? d[i]["Input"].Size() : 1; 
        }
        

        for (int j = i-1; j >= 0; j--)//j比i小
        {
            //如果j有输出，j的Dst位置更新到hash表中
            if(d[j].HasMember("Dst"))
            {
                string key_dst=d[j]["Dst"].GetString();
                if(dst_id[key_dst]&&(dst_id[key_dst]>j+1))//已经存在，且值比j大
                    break;
                else //不存在，或者位置错了
                    dst_id[key_dst]=j+1;
            }
            
            switch (type_id)
            {
            case 1://类型为Op，input的类型都是string
            {
                //设定：输入最多为两个
                // string name_dst;
                if(num_input==2)
                {    
                    string name_in1=d[i]["Input"][0].GetString();
                    string name_in2=d[i]["Input"][1].GetString();
                    int id_dst;
                    if(dst_id[name_in1]&&(!input_depend[0]))//存在且未被输出
                    {   
                        input_depend[0]=true;
                        cout<<"输入数量为string2，in1存在数据依赖："<<dst_id[name_in1]<<"->"<<i+1<<"  j= "<<j<<endl;
                    }
                    if(dst_id[name_in2]&&(!input_depend[1]))//存在
                    {
                        input_depend[1]=true;
                        cout<<"输入数量为string2，in2存在数据依赖："<<dst_id[name_in2]<<"->"<<i+1<<"  j="<<j<<endl;
                        break;  
                    }      
                }
                else //Op输入数量为1
                {
                    
                    string input_name=d[i]["Input"][0].GetString();
                    if(dst_id[input_name])//存在
                    {
                        input_depend[0]=true;
                        cout<<"输入数量为string1，存在数据依赖："<<j+1<<"->"<<dst_id[input_name]<<endl;
                    }
                    
                }
            }
                break;
            case 2://类型为assign
            {
                int a=0;
            }
                break;
            case 3://类型为branch
            {
                int b=1;
            }
                break;
            
            case 4://类型为loop
            {
                int c=1;
            }
                break;
            default:
                break;
            }            

        }        

    }
    return 0;
}

//定义函数，判断节点的类型
int Type2node(string type){
        if(type==op)
            return 1;
        if(type==assign)
            return 2;
        if(type==branch)
            return 3;
        if(type==loop)
            return 4;
        return 0;
}