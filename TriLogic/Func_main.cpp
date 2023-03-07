#include "mainfunc.h"
#include <algorithm>
using namespace rapidjson;
using namespace std;
//设计空间探索的限制
double cycle_limit=500;//运行时间(速度)限制，单位：周期
double area_limit=1000;//面积限制，单位：
double power_limit=10;//功耗限制，单位：
//定义数据的位数，这个后期要修改，从CDFG中得到
int bit_num_operand=32;//暂时定义为32
//优化目标
int target=1;//1速度，2面积，3功耗
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
//                cout<<"分支节点依赖"<<endl<<"条件节点:"<<con<<"  size of true:"<<sizeofT<<endl;
//                cout<<"T：";
                for(int there=0;there<sizeofT;there++)
                {
//                    cout<<d[i]["Statement_when_true"][there].GetInt()<<"   ";
                    Node *a=find_node_by_number(nodes,d[i]["Statement_when_true"][there].GetInt());
//                    if(a!=NULL && a->control==NULL)//a不是空节点,且当前控制节点没有指向
                    if(a!=NULL && (a->control==NULL || a->control->node_id<con))
                    {
                        if (a->control==NULL)//如果是第一次设置控制节点
                            inDegree[a->node_id]++;//节点a的入度++
                        a->control=con_node;
                        TvsF.insert(pair<int, bool>(a->node_id, true));//构建互斥
//                        cout<<"存在"<<con<<"到"<<a->node_id<<"的控制依赖"<<endl;
                    }
                }
//                cout<<endl<<"F：";
                for(int there=0;there<sizeofF;there++)
                {
//                    cout<<d[i]["Statement_when_false"][there].GetInt()<<"   ";
                    Node *b=find_node_by_number(nodes,d[i]["Statement_when_false"][there].GetInt());
                    if(b!=NULL && (b->control==NULL || b->control->node_id<con))//b不是空节点,且当前控制节点没有指向
                    {
                        if (b->control==NULL)
                            inDegree[b->node_id]++;//节点b的入度++
                        b->control=con_node;
                        TvsF.insert(pair<int, bool>(b->node_id, false));//构建互斥
//                        cout<<"存在"<<con<<"到"<<b->node_id<<"的控制依赖"<<endl;
                    }
                }
//                cout<<endl;
        }
        if(type_id==4)//类型为loop
        {
                //loop的依赖比较独特，即需要依赖前面的执行结果，又需要依赖循环内对数据的改变结果
                //先输出控制依赖
                int con=d[i]["Condition"].GetInt();//条件节点的位置
                Node* con_node= find_node_by_number(nodes,con);
//                cout<<"loop节点依赖"<<endl<<"条件节点："<<con<<endl;
                int sizeofL=d[i]["Statement_loop"].Size();//LOOP节点数目
                for(int there=0;there<sizeofL;there++)
                {
                    int there_id=d[i]["Statement_loop"][there].GetInt();
//                    cout<<there_id<<endl;
                    //1.该节点可能是OP，没有Dst，所以要先对类型进行判断
                    //2.条件节点判断的输入也可能有多个，最好写成函数，直接套用OP之间的依赖
                    //问题：there_id可能根本不在nodes中，nodes的依赖可能有很多，需要找到他的直接依赖
                    Node* a= find_node_by_number(nodes,there_id);
                    if(a!=NULL && (a->control==NULL || a->control->node_id<con))//a不是空节点，且a当前控制节点没有指向或者是更大一级别的
                    {
                        if (a->control==NULL)//如果第一次设置控制节点
                            inDegree[a->node_id]++;//入度++
                        a->control=con_node;
//                        cout<<"loop节点"<<there_id<<"的控制依赖为"<<con_node->node_id<<endl;

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
//                        cout<<"节点"<<i+1<<"输入数量为string2，in1数据依赖："<<dst_id[name_in1]<<"->"<<i+1<<endl;
                    }
                    if(dst_id[name_in2]&&(!input_depend[1])&&(dst_id[name_in2]<i+1))//存在
                    {
                        Node *b= find_node_by_number(nodes,dst_id[name_in2]);//当前输入依赖的节点2
                        input_depend[1]=true;
                        c->depend2=b;
                        inDegree[c->node_id]++;//入度++
//                        cout<<"节点"<<i+1<<"输入数量为string2，in2数据依赖："<<dst_id[name_in2]<<"->"<<i+1<<endl;
//                        break;
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
//                        cout<<"节点"<<i+1<<"输入数量为string1，数据依赖："<<dst_id[input_name]<<"->"<<i+1<<endl;
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
//                    cout<<"节点"<<i+1<<"的输入为int，"<<"数据依赖："<<d[i]["Input"].GetInt()<<"->"<<i+1<<endl;
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
//                    cout<<"节点"<<i+1<<"的输入为string，"<<"数据依赖："<<dst_id[input_name]<<"->"<<i+1<<endl;
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
     * 构建初始的控制步
     *******************************/
    topologicalSort(nodes,inDegree,controlstep,id_pos);//函数使用，现在得到了controlstep vector<vector<Node*>>

    cout<<"cout the control step:"<<endl<<endl;
    for (int i = 0; i < controlstep.size(); ++i) {
        cout<<"control step"<<i+1<<"大小为"<<controlstep[i].size()<<endl;
        for (int j = 0; j < controlstep[i].size(); ++j) {
            cout<< controlstep[i][j].node_id<<":"<<controlstep[i][j].operator_name<<endl;
        }
    }

    /*开始新的循环
     * 在初始控制步中加入逻辑函数
     * 调度算子
     * 先写单个循环，然后在最外层套上一个大的DSE迭代
     * */
    //我们规定，只能使用4/6输入的LUT
    //VHDL中，逻辑操作都是按位的，需要由一位操作拼接
    struct array{
        bool is_using;//当前正在使用
        int logic_type;//阵列的类型
        int row_num;//大小，行数
        int col_num;//大小，列数
        double write_number;//写次数，以行数为计数单位
        vector<int> store_node;//存储的节点操作数ID,如果是LUT模式，则代表其可以完成的运算
    };
    vector<array> array_list;//存储阵列类型的向量
    for(int i=0;i<controlstep.size();i++)
    {
        for (int j = 0; j <controlstep[i].size() ; ++j) {
            //一层层遍历控制步，获取操作数，获取算子，分配阵列，计算速度、功耗、面积
            int type_operation= op2int(controlstep[i][j].operator_name);
            //数据位数，bit_num_operand
            array arr_now;//新建一个阵列
            //如果是LUT模式,需要阵列存放数据和计算结果
            arr_now.logic_type=1;//将其设置为LUT模式
            switch (arr_now.logic_type) {
                case 1://LUT，以2*操作数位数为行数
                    if (2*bit_num_operand<=32)
                        arr_now.row_num=32;
                    else
                        arr_now.row_num=64;//设定最大是64
                    break;
                case 2://sa，以操作数位数为列数
                    if (bit_num_operand<=32)
                        arr_now.col_num=32;
                    else
                        arr_now.col_num=64;//设定最大是64
                    break;
                case 3://magic,以操作数
                    if (bit_num_operand<=32)
                        arr_now.row_num=32;
                    else
                        arr_now.row_num=64;//设定最大是64
                    break;
                default:
                    break;
            }
            arr_now.col_num=arr_now.row_num;//默认阵列是方形的
            arr_now.store_node.push_back(controlstep[i][j].node_id);//加入节点
            //对于LUT，逻辑节点的类型最多为三个





        }
    }

    return 0;
}

