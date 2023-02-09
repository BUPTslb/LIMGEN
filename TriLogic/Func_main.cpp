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
        cout<<"IsArray的功能："<<d[0]["Input"].IsArray()<<endl;
        cout<<"IsString的功能："<<d[0]["Input"].IsString()<<endl;
        cout<<"IsString的功能："<<d[0]["Input"][1].IsString()<<endl;
        cout<<"IsString的功能："<<d[0]["Node_ID"].IsString()<<endl;
        cout<<"HasMember的功能："<<d[0].HasMember("Dst")<<endl;
    //***********************************************************    
    int json_size=d.Size();//整个数组的尺寸（节点的数量）
    //构建键值对，存储
    map<string,int> operator_num;//各个算子出现的次数
    map<string,int> dst_id;//Dst出现的最后位置
    map<string,int> type_num;
    //遍历json文件
    cout<<"The number of CDFG = "<<json_size<<endl;
    cout<<d[0]["Type"].GetString()<<endl;
    /************************************************
    //寻找可以并行操作的命令行
    //一样的变量同时写进两个不同的阵列中
    //不同的变量/执行也毫不相干，直接放在不同的阵列中执行
    //进行重新排序，然后再进行逻辑模式的套用  
    ************************************************/

    //**********各类型节点数量****************
    int num_op=0;
    int num_assign=0;
    int num_loop=0;
    int num_branch=0;
    //**************************************
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
   cout<<"各个Dst操作数的位置"<<endl;
    for(map<string,int>::iterator it=dst_id.begin();it!=dst_id.end();it++)
   {
       cout<<it->first<<" "<<it->second<<endl;
   }
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
        for (size_t j = i-1; j >= 0; j--)
        {
            if(d[j].HasMember("Dst"))
            {
                string key_dst=d[j]["Dst"].GetString();
                if(dst_id[key_dst]&&(dst_id[key_dst]<=i))//已经存在，且值比i+1小
                    break;
                else //不存在，或者位置错了
                    dst_id[key_dst]=j+1;
            }
            //Input的数量和类型
            //Branch和loop没有input
            //如果有“Input”，得到input的数量
            int num_input;
            if(d[j].HasMember("Input"))
                num_input=d[j]["Input"].IsArray() ? d[j]["Input"].Size() : 1;
            
            switch (Type2node(d[j]["Type"].GetString()))
            {
            case 1://类型为Op
            {
                //设定：输入最多为两个
                string name_dst;
                if(num_input==2)
                {    
                    string name_in1=d[j]["Input"][0].GetString();
                    string name_in2=d[j]["Input"][1].GetString();
                    int id_dst;
                    if(dst_id[name_in1])//存在
                        cout<<"存在数据依赖："<<j+1<<"->"<<dst_id[name_in1]<<endl;
                    if(dst_id[name_in2])//存在
                        cout<<"存在数据依赖："<<j+1<<"->"<<dst_id[name_in2]<<endl;
          
                    // if(d[i].HasMember("Dst") ? d[i]["Dst"].IsString():0)
                    // {
                    //         name_dst=d[i]["Dst"].GetString();//Dst是string类型，有的没有dst
                    //         if(name_in1==name_dst||name_in2==name_dst)
                    //         cout<<"存在数据依赖："<<j+1<<"->"<<i+1<<endl;
                    // }                   
                }
                else //输入数量为1，可能是string也可能是int
                {
                    //判断
                    if(d[j]["Input"][0].IsInt())
                    {
                        int input_id= d[j]["Input"][0].GetInt();
                        if(input_id==i+1)
                            cout<<"存在数据依赖："<<j+1<<"->"<<i+1<<endl;
                    }
                    else
                    {
                        string input_name=d[j]["Input"][0].GetString();
                        if(dst_id[input_name])//存在
                            cout<<"存在数据依赖："<<j+1<<"->"<<dst_id[input_name]<<endl;
 
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