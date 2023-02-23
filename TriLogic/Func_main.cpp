#include<iostream>
#include<fstream>
#include<sstream>
#include<map>
#include<SDL2/SDL.h>
#include<typeinfo>
#include <algorithm>
#include"rapidjson/document.h"
#include"logic_func.h"
#include "vector"
#include <queue>
#include <algorithm>
using namespace rapidjson;
using namespace std;

//设计空间探索的限制
double cycle_limit=500;//运行时间(速度)限制，单位：周期
double area_limit=1000;//面积限制，单位：
double power_limit=10;//功耗限制，单位：
//测试迭代次数
int test=0;
/*节点中包含的字符串，用作匹配*/
string op= "Op";
string assign="Assign";
string branch="Branch";
string loop="Loop";
//绘制节点依赖关系的图形
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
//定义节点类型
struct Node{
    int node_id;//节点ID,用来代表节点
    string operator_name;//算子，只算有操作的，用来选逻辑族，也能判断是不是写操作
    Node* depend1=NULL;//两个数据依赖指针,初始定义为NULL
    Node* depend2=NULL;//指向前面
    Node* control=NULL;//控制依赖
};

int Type2node(string type);//Type2node函数的声明
Node * find_node_by_number(std::vector<Node>& nodes, int node_id);//寻找节点指针的函数声明

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
    //定义控制步
    vector<vector<Node>> controlstep;

    //构建键值对，存储
    map<string,int> operator_num;//各个算子出现的次数
    map<string,int> dst_id;//Dst出现的最后位置
    map<string,int> type_num;
    map<int,int> op_map;//存储op类型的id索引关系，input做键,建立node后可被优化
    map<int,int> id_pos;//存储节点id在nodes中的位置,好像和find_node_by_number冲突了
    map<int,bool> TvsF;//Branch斥互表, 遇到分支节点，正确和错误的不能相互依赖，每次遇到branch要清空重写
    map<int,int> inDegree;//入度表，记录每个节点的入度，id对应入度，数据1、2+直接控制，最大为3
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
    //第一次遍历，生成操作节点，并加入nodes向量
    for(int i=0;i<json_size;i++){
        int type_id= Type2node(d[i]["Type"].GetString());
        if(type_id==1||type_id==2)
        {
            Node n;
            n.node_id=d[i]["Node_ID"].GetInt();
            n.operator_name=d[i]["Operation"].GetString();
            nodes.push_back(n);
            inDegree[n.node_id]=0;//初始化节点的入度
            id_pos[n.node_id]=nodes.size()-1;//新节点在nodes中的位置
        }
        //统计各个OP操作的数量
        if(type_id==1)
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
   //关键算法-算子调度
   //输出控制步骤和节点
   //表明其可以并行执行
   ******************************/
    
    for(int i=json_size-1;i>0;i--)
    {
        //遍历得出数据前后依赖关系
        //输出形式：节点->节点，没有依赖则不含“->”
        //1.判断input是否为节点ID
        //2.判断input是否为之前节点的Dst
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
            //对互斥表进行清空重建
                TvsF.clear();
                int con=d[i]["condition"].GetInt();//条件节点的位置
                Node* con_node= find_node_by_number(nodes,con);//条件节点的指针
                int sizeofT=d[i]["Statement_when_true"].Size();//T的节点数目
                int sizeofF=d[i]["Statement_when_false"].Size();//F的节点数目
                cout<<"分支节点依赖"<<endl<<"条件节点:"<<con<<"  size of true:"<<sizeofT<<endl;
                cout<<"T：";
                for(int there=0;there<sizeofT;there++)
                {
                    cout<<d[i]["Statement_when_true"][there].GetInt()<<"   ";
                    Node *a=find_node_by_number(nodes,d[i]["Statement_when_true"][there].GetInt());
//                    if(a!=NULL && a->control==NULL)//a不是空节点,且当前控制节点没有指向
                    if(a!=NULL && (a->control==NULL || a->control->node_id<con))
                    {
                        if (a->control==NULL)//如果是第一次设置控制节点
                            inDegree[a->node_id]++;//节点a的入度++
                        a->control=con_node;
                        TvsF.insert(pair<int, bool>(a->node_id, true));//构建互斥
                        cout<<"存在"<<con<<"到"<<a->node_id<<"的控制依赖"<<endl;
                    }
                }
                cout<<endl<<"F：";
                for(int there=0;there<sizeofF;there++)
                {
                    cout<<d[i]["Statement_when_false"][there].GetInt()<<"   ";
                    Node *b=find_node_by_number(nodes,d[i]["Statement_when_false"][there].GetInt());
                    if(b!=NULL && (b->control==NULL || b->control->node_id<con))//b不是空节点,且当前控制节点没有指向
                    {
                        if (b->control==NULL)
                            inDegree[b->node_id]++;//节点b的入度++
                        b->control=con_node;
                        TvsF.insert(pair<int, bool>(b->node_id, false));//构建互斥
                        cout<<"存在"<<con<<"到"<<b->node_id<<"的控制依赖"<<endl;
                    }
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
                for(int there=0;there<sizeofL;there++)
                {
                    int there_id=d[i]["Statement_loop"][there].GetInt();
                    cout<<there_id<<endl;
                    //1.该节点可能是OP，没有Dst，所以要先对类型进行判断
                    //2.条件节点判断的输入也可能有多个，最好写成函数，直接套用OP之间的依赖
                    //问题：there_id可能根本不在nodes中，nodes的依赖可能有很多，需要找到他的直接依赖
                    Node* a= find_node_by_number(nodes,there_id);
                    if(a!=NULL && (a->control==NULL || a->control->node_id<con))//a不是空节点，且a当前控制节点没有指向或者是更大一级别的
                    {
                        if (a->control==NULL)//如果第一次设置控制节点
                            inDegree[a->node_id]++;//入度++
                        a->control=con_node;
                        cout<<"loop节点"<<there_id<<"的控制依赖为"<<con_node->node_id<<endl;

                    }
                }
        }

        for (int j = i-1; j >= 0; j--)//j比i小
        {
            //如果j有输出，j的Dst位置更新到hash表中
            if(d[j].HasMember("Dst"))
            {
                string key_dst=d[j]["Dst"].GetString();
                //非（已经存在，且值比j+1大,但是比i+1小）
                if(!(dst_id[key_dst]&&(dst_id[key_dst]>j+1)&&(dst_id[key_dst]<i+1)))
                {
                    //还要满足i+1和j+1不同在互斥表中,主要是，i+1false且j+1true
                    if(!(TvsF[i+1]==false && TvsF[j+1]== true)) {
                        dst_id[key_dst] = j + 1;
                    }
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
                        inDegree[c->node_id]++;//当前节点入度++
                        cout<<"节点"<<i+1<<"输入数量为string2，in1数据依赖："<<dst_id[name_in1]<<"->"<<i+1<<endl;
                    }
                    if(dst_id[name_in2]&&(!input_depend[1])&&(dst_id[name_in2]<i+1))//存在
                    {
                        Node *b= find_node_by_number(nodes,dst_id[name_in2]);//当前输入依赖的节点2
                        input_depend[1]=true;
                        c->depend2=b;
                        inDegree[c->node_id]++;//入度++
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
                        inDegree[c->node_id]++;//入度++
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
                    inDegree[c->node_id]++;//入度++
                    cout<<"节点"<<i+1<<"的输入为int，"<<"数据依赖："<<d[i]["Input"].GetInt()<<"->"<<i+1<<endl;
                }
                //如果input是数，搜索hash表（立即数和变量名都会被表示为string）
                if(d[i]["Input"].IsString()&& !input_depend[0])
                {
                    input_depend[0]=true;
                    string input_name=d[i]["Input"].GetString();
                    Node *a= find_node_by_number(nodes,dst_id[input_name]);
                    c->depend1=a;
                    if(dst_id[input_name]!=0)//利用立即数此值为0的性质修改入度
                        inDegree[c->node_id]++;//入度++
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
     * 循环对象为nodes,vector<Node>
     * 构建控制步
     * 使用的逻辑不同，控制步也会不同，这时要开始设计空间探索
     *******************************/
    void topologicalSort(vector<Node> nodes,map<int,int> &inDegree,\
                        vector<vector<Node>>& controlstep,\
                        map<int,int> &id_pos) ;//函数声明
    void ComIndegree(queue<int> &nodeQueue,map<int,int> &inDegree,\
                        vector<Node> &nodes,map<int,int> &id_pos);//子函数声明

    topologicalSort(nodes,inDegree,controlstep,id_pos);//函数使用，现在得到了controlstep vector<vector<Node*>>
//    测试，验证节点中的函数指向是否正确--不正确
    cout<< "节点30在nodes中的位置为："<<id_pos[30]<<"其控制依赖为"<<nodes[id_pos[30]].control->node_id<<endl;//取节点29在nodes中的位置，其control指向的节点，的node_id应该等于28

    cout<<"cout the control step:"<<endl;
    for (int i = 0; i < controlstep.size(); ++i) {
        cout<<"control step"<<i+1<<"大小为"<<controlstep[i].size()<<endl;
        for (int j = 0; j < controlstep[i].size(); ++j) {
            cout<< controlstep[i][j].node_id<<"  ";
        }
        cout<<endl;
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
//根据节点id，返回节点指针，好像和id_pos功能冲突了
Node * find_node_by_number(std::vector<Node>& nodes, int node_id) {
    auto it = std::find_if(nodes.begin(), nodes.end(),
                           [node_id](const Node& node) {
                               return node.node_id == node_id;
                           });

    if (it != nodes.end()) {
        return &(*it);//返回指针指向的变量的地址
    } else {
        return nullptr;
    }
}
//计算入度的函数,直接对nodes进行修改
void ComIndegree(queue<int> &nodeQueue,map<int,int> &inDegree,vector<Node> &nodes,map<int,int> &id_pos){
    if (nodeQueue.empty())
        return;
    //nodeQueue存放当前入度为零的点的id
    while (!nodeQueue.empty())
    {
        int now_id=nodeQueue.front();
        cout<<"nodeQueue:  "<<now_id<<endl;
        nodeQueue.pop();//出队
        int node_pos=id_pos[now_id];//获得节点的位置
        //从当前位置开始循环
        for (int i = node_pos; i < nodes.size(); ++i) {
            if (nodes[i].control&&nodes[i].control->node_id == now_id){

                nodes[i].control=NULL;
                if (nodes[i].control==NULL)
                    cout<<"当前节点"<<nodes[i].node_id<<"对节点"<<now_id<<"存在控制依赖，已消除"<<endl;
            }
            if (nodes[i].depend1&&nodes[i].depend1->node_id==now_id){
                nodes[i].depend1=NULL;
                if (nodes[i].depend1==NULL)
                    cout<<"当前节点"<<nodes[i].node_id<<"对节点"<<now_id<<"存在数据依赖，已消除"<<endl;
            }
            if (nodes[i].depend2&&nodes[i].depend2->node_id==now_id){
                nodes[i].depend2=NULL;
                if (nodes[i].depend2==NULL)
                    cout<<"当前节点"<<nodes[i].node_id<<"对节点"<<now_id<<"存在数据依赖，已消除"<<endl;
            }
            //如果指针依赖全为空，则将其入度设置为0
            //问题在这！！不能用now_id,now_id已经是0了
            if (inDegree[nodes[i].node_id]>0 &&nodes[i].control==NULL && \
                    nodes[i].depend1==NULL && nodes[i].depend2==NULL)
            {
                inDegree[nodes[i].node_id]=0;
                cout<<"已经将节点"<<nodes[i].node_id<<"的入度设置为0"<<endl;
            }

        }

    }
    //现在得到了一个新的inDegree
}
//拓朴排序，返回节点指针
//递归调用，获取控制步：vector<vector<Node*>>
void topologicalSort(vector<Node> nodes,map<int,int> &inDegree,\
                        vector<vector<Node>>& controlstep,\
                        map<int,int> &id_pos) {
    test++;
    cout<<"迭代次数："<<test<<endl;
    vector<Node> stepnow;//当前控制步，存放节点指针
    queue<int>  nodeQueue;//存放度为0的节点
    // 计算每个节点的入度
    // 将入度为0的节点加入队列
    //由于node是指针，只需要获取nodes中节点的地址即可
    for (auto it =inDegree.begin(); it !=inDegree.end();it++)
    {
        cout<<it->first<<"  "<<it->second<<endl;//当前hash表的值
        int now_pos=id_pos[it->first];//当前要入队的节点在nodes中的位置
//        cout<<"当前要入队的节点在nodes中的位置"<<now_pos<<"当前要入队的节点"<<nodes[now_pos].node_id<<endl;
        if (it->second == 0) {
            stepnow.push_back(nodes[id_pos[it->first]]);
            nodeQueue.push(it->first);//将当前入度为0的节点号码入队
            cout<<"新入队的元素为："<<nodeQueue.back()<<endl;
            cout<<"新进入step的元素ID："<<nodes[id_pos[it->first]].node_id<<endl;
            it->second=-1;//将当前的值设为-1，表示已经入队了
        }
    }
    //将stepnow加入到控制步中
    if(!stepnow.empty())
    {
        controlstep.push_back(stepnow);
        cout<<"第一个控制步骤大小："<<controlstep[0].size()<<endl;
    }
    // 执行拓扑排序,有许多节点依赖于入度为0的节点
    //control,depend1,depend2(都是指针)=inDegree[]为0的点
    cout<<"调用topo排序"<<endl;
    bool stop_topoSort=nodeQueue.empty();//根据队列是否为空判断是否该停止循环
    ComIndegree(nodeQueue,inDegree,nodes,id_pos);//更新inDegree
    //现在得到了新的inDegree，可以进行递归了
    //防止无限循环,如果map中还有大于等于0的点
    if (!stop_topoSort) {
        topologicalSort(nodes, inDegree,controlstep,id_pos);
    }
}
