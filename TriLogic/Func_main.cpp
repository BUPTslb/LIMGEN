#include<iostream>
#include<fstream>
#include<sstream>
#include<map>
#include<typeinfo>
#include <algorithm>
#include"rapidjson/document.h"
#include"logic_func.h"
#include "vector"
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
//定义节点类型
struct Node{
    int node_id;//节点ID,用来代表节点
    string operator_name;//算子，只算有操作的，用来选逻辑族，也能判断是不是写操作
    Node* depend1;//两个数据依赖指针
    Node* depend2;//指向前面
    Node* control;//控制依赖
};
//定义控制步节点的结构
struct ControlStep{
    int step_number;
    vector<Node*> nodes;
};

int Type2node(string type);//Type2node函数的声明
Node * find_node_by_number(const std::vector<Node>& nodes, int node_id);//寻找节点指针的函数声明

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
    vector<Node> nodes;//节点类型的向量，里面可以放下一个个节点

    //构建键值对，存储
    map<string,int> operator_num;//各个算子出现的次数
    map<string,int> dst_id;//Dst出现的最后位置
    map<string,int> type_num;
    map<int,int> op_map;//存储op类型的id索引关系，input做键,建立node后可被优化
    
    //1.先读一个节点让其输出
    ifstream file("../hdlAst/hdlast2.json");
    stringstream buffer;
    buffer << file.rdbuf();
    string content(buffer.str());
    // 解析 JSON 数据
    Document d;
    d.Parse(content.c_str());
    // 测试rapidjson功能区*******************************************
        cout<<d[14]["Node_ID"].GetInt()<<endl;
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
    //第一次遍历，生成操作节点
    for(int i=0;i<json_size;i++){
        int type_id= Type2node(d[i]["Type"].GetString());
        if(type_id==1||type_id==2)
        {
            Node n;
            n.node_id=d[i]["Node_ID"].GetInt();
            n.operator_name=d[i]["Operation"].GetString();
            nodes.push_back(n);
        }
    }

    for(int i=0;i<json_size;i++){

        if(d[i]["Type"].GetString()== op)
        {
            num_op++;
            string key_op=d[i]["Operation"].GetString();
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

   /*****************************
   //关键算法
   //算子调度
   //输出控制步骤和节点
   //表明其可以并行执行
   ******************************/
    //如果全部串行
    int minnum_array=1;
    //先确定控制步的数目
    int control_num=num_op;//初始为Op的数量
    cout<<"假设控制步数量为control_num："<<control_num<<endl;
    
    for(int i=json_size-1;i>0;i--)
    {
        //遍历得出数据前后依赖关系
        //输出形式：节点->节点，没有依赖则不含“->”
        //1.判断input是否为节点ID
        //2.判断input是否为之前节点的Dst
        //使用的数据结构：？
        int num_input;
        int type_id;//是否应该做成一个表？直接读取到每个节点的类型
            //Input的数量和类型
            //Branch和loop没有input
            //如果有“Input”，得到input的数量
        type_id=Type2node(d[i]["Type"].GetString());
        Node *c= find_node_by_number(nodes,i+1);//c表示当前节点
        map<int,bool> input_depend;//每次i都构建hash表，存储当前输入（最多两个），如果前面找到了前驱依赖，值为true
        if(d[i].HasMember("Input"))
        {
            num_input=d[i]["Input"].IsArray() ? d[i]["Input"].Size() : 1; 
        }
        //条件节点中的依赖关系都已经有了，属于控制依赖，只需要输出一次，不用多次遍历
        if(type_id==3)//类型为branch,存在控制依赖
        {
                int con=d[i]["condition"].GetInt();//条件节点的位置
                Node* con_node= find_node_by_number(nodes,con);//条件节点的指针
                int sizeofT=d[i]["Statement_when_true"].Size();//T的节点数目
                int nodeT[sizeofT];                
                int sizeofF=d[i]["Statement_when_false"].Size();//F的节点数目
                int nodeF[sizeofF];
                cout<<"分支节点依赖"<<endl<<"条件节点:"<<con<<endl;
                cout<<"T：";
                for(int there=0;there<sizeofT;there++)
                {
                    cout<<d[i]["Statement_when_true"][there].GetInt()<<"   ";
                    Node *a=find_node_by_number(nodes,d[i]["Statement_when_true"][there].GetInt());
                    a->control=con_node;//分支节点的控制指针指向条件节点
                }
                cout<<endl<<"F：";
                for(int there=0;there<sizeofF;there++)
                {
                    cout<<d[i]["Statement_when_false"][there].GetInt()<<"   ";
                    Node *b=find_node_by_number(nodes,d[i]["Statement_when_false"][there].GetInt());
                    b->control=con_node;//分支节点的控制指针指向条件节点
                }
                cout<<endl;
            
        }
        if(type_id==4)//类型为loop
        {
                //loop的依赖比较独特，即需要依赖前面的执行结果，又需要依赖循环内对数据的改变结果
                //先输出控制依赖
                int con=d[i]["Condition"].GetInt();//条件节点的位置
                Node* con_node= find_node_by_number(nodes,con);
                cout<<"loop节点依赖"<<endl<<"条件节点："<<con<<endl;
                int sizeofL=d[i]["Statement_loop"].Size();//LOOP节点数目
//                int loop_depend=0;
                for(int there=0;there<sizeofL;there++)
                {
                    int there_id=d[i]["Statement_loop"][there].GetInt();
                    cout<<there_id<<endl;
                    //1.该节点可能是OP，没有Dst，所以要先对类型进行判断
                    //2.条件节点判断的输入也可能有多个，最好写成函数，直接套用OP之间的依赖
                    Node* a= find_node_by_number(nodes,there_id);
                    a->control=con_node;
                }
        
        }

        for (int j = i-1; j >= 0; j--)//j比i小
        {
            //如果j有输出，j的Dst位置更新到hash表中
            if(d[j].HasMember("Dst"))
            {
                string key_dst=d[j]["Dst"].GetString();
                //非（已经存在，且值比j大,但是比i+1小）
                if(!(dst_id[key_dst]&&(dst_id[key_dst]>j+1)&&(dst_id[key_dst]<i+1)))
                    {
                      dst_id[key_dst]=j+1;
                    }                    
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
                    if(dst_id[name_in1]&&(!input_depend[0])&&(dst_id[name_in1]<i+1))//存在且未被输出且节点对应关系正确
                    {
                        Node *a= find_node_by_number(nodes,dst_id[name_in1]);//当前输入依赖的节点1
                        input_depend[0]=true;
                        c->depend1=a;//当前节点的输入依赖于a
                        cout<<"节点"<<i+1<<"输入数量为string2，in1数据依赖："<<dst_id[name_in1]<<"->"<<i+1<<endl;
                    }
                    if(dst_id[name_in2]&&(!input_depend[1])&&(dst_id[name_in2]<i+1))//存在
                    {
                        Node *b= find_node_by_number(nodes,dst_id[name_in2]);//当前输入依赖的节点2
                        input_depend[1]=true;
                        c->depend2=b;
                        cout<<"节点"<<i+1<<"输入数量为string2，in2数据依赖："<<dst_id[name_in2]<<"->"<<i+1<<endl;
                        break;  
                    }      
                }
                else //Op输入数量为1
                {
                    string input_name=d[i]["Input"][0].GetString();

                    if(dst_id[input_name]&&(dst_id[input_name]<i+1)&&(!input_depend[0]))//存在且值比i+1小且没输出过
                    {
                        Node *a= find_node_by_number(nodes,dst_id[input_name]);//指向输入依赖
                        input_depend[0]=true;
                        c->depend1=a;
                        cout<<"节点"<<i+1<<"输入数量为string1，数据依赖："<<dst_id[input_name]<<"->"<<i+1<<endl;
                    }   
                }
            }
                break;
            case 2://类型为assign,默认只有一个输入
            {
                //如果input是ID，则直接输出
                if(d[i]["Input"].IsInt()&& !input_depend[0])
                {
                    Node *a= find_node_by_number(nodes,d[i]["Input"].GetInt());
                    input_depend[0]=true;
                    c->depend1=a;
                    cout<<"节点"<<i+1<<"的输入为int，"<<"数据依赖："<<d[i]["Input"].GetInt()<<"->"<<i+1<<endl;
                }
                //如果input是数，搜索hash表（立即数和变量名都会被表示为string）
                if(d[i]["Input"].IsString()&& !input_depend[0])
                {
                    input_depend[0]=true;
                    string input_name=d[i]["Input"].GetString();
                    Node *a= find_node_by_number(nodes,dst_id[input_name]);
                    c->depend1=a;
                    cout<<"节点"<<i+1<<"的输入为string，"<<"数据依赖："<<dst_id[input_name]<<"->"<<i+1<<endl;
                }
            }
                break;            
        
            default:
                break;
            }            

        }        

    }

    /**************************
     * 下面进行新一轮循环
     * 循环对象为nodes
     * 构建控制步
     *******************************/



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
//根据节点id，返回节点指针
Node * find_node_by_number(vector<Node>& nodes, int node_id) {
    auto it = std::find_if(nodes.begin(), nodes.end(),
                           [node_id](const Node& node) {
                               return node.node_id == node_id;
                           });

    if (it != nodes.end()) {
        return &(*it);
    } else {
        return nullptr;
    }
}
