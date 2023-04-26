#include "logic_func.h"
#include <filesystem>

using namespace rapidjson;
using namespace std;

int main() {
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
    int num_op = 0;
    int num_assign = 0;
    int num_loop = 0;
    int num_branch = 0;
    //定义控制步
    vector<vector<Node>> controlstep;

    //构建键值对，存储
    map<string, int> operator_num;//各个算子出现的次数
    map<string, int> dst_id;//Dst出现的最后位置
    map<string, int> type_num;
    map<int, int> op_map;//存储op类型的id索引关系，input做键,建立node后可被优化
    map<int, int> id_pos;//存储节点id在nodes中的位置,好像和find_node_by_number冲突了
    map<int, bool> TvsF;//Branch斥互表, 遇到分支节点，正确和错误的不能相互依赖，每次遇到branch要清空重写
    map<int, int> inDegree;//入度表，记录每个节点的入度，id对应入度，数据1、2+直接控制，最大为3
    //1.先读一个节点让其输出
    ifstream file("../hdlAst/hdlast2.json");
    stringstream hdlast;
    hdlast << file.rdbuf();
    string content(hdlast.str());
    // 解析 JSON 数据
    Document d;
    d.Parse(content.c_str());
    // 测试rapidjson功能区*******************************************
    cout << d[14]["Node_ID"].GetInt() << endl;
    //***********************************************************    
    int json_size = d.Size();//整个数组的尺寸（节点的数量）

    //遍历json文件
    cout << "The number of CDFG = " << json_size << endl;
    cout << d[0]["Type"].GetString() << endl;
    /************************************************
    //寻找可以并行操作的命令行
    //一样的变量同时写进两个不同的阵列中
    //不同的变量/执行也毫不相干，直接放在不同的阵列中执行
    //进行重新排序，然后再进行逻辑模式的套用  
    ************************************************/
    //第一次遍历，生成操作节点，并加入nodes向量
    for (int i = 0; i < json_size; i++) {
        int type_id = Type2node(d[i]["Type"].GetString());
        if (type_id == 1 || type_id == 2) {
            Node n;
            n.node_id = d[i]["Node_ID"].GetInt();
            n.operator_name = d[i]["Operation"].GetString();
            n.wb_pos.resize(6);//节点存储表，初始化为3，其中0：Reg,1：lut，2：sa, 3:magic,4:lut-buffer,5:sa-buffer
            n.out_degree = 0;//初始化出度
            inDegree[n.node_id] = 0;//初始化节点的入度
            nodes.push_back(n);//将节点加入节点向量
            id_pos[n.node_id] = nodes.size() - 1;//新节点在nodes中的位置

        }
        //统计各个OP操作的数量
        if (type_id == 1) {
            num_op++;
            string key_op = d[i]["Operation"].GetString();
            //调整hash表
            if (operator_num[key_op])//已经有该键
                operator_num[key_op]++;
            else
                operator_num[key_op] = 1;
        }
    }

    cout << "操作数hash表的大小：" << operator_num.size() << endl;

    for (auto it = operator_num.begin(); it != operator_num.end(); it++) {
        cout << it->first << " " << it->second << endl;
    }

    /*****************************
    //关键算法-算子调度
    //输出控制步骤和节点
    //表明其可以并行执行
    ******************************/

    for (int i = json_size - 1; i > 0; i--) {
        //遍历得出数据前后依赖关系
        //输出形式：节点->节点，没有依赖则不含“->”
        //1.判断input是否为节点ID
        //2.判断input是否为之前节点的Dst
        int num_input;
        int type_id;//是否应该做成一个表？直接读取到每个节点的类型
        //Input的数量和类型
        //Branch和loop没有input
        //如果有“Input”，得到input的数量
        type_id = Type2node(d[i]["Type"].GetString());
        Node *c = find_node_by_number(nodes, i + 1);//c表示当前节点
        map<int, bool> input_depend;//每次i都构建hash表，存储当前输入（最多两个），如果前面找到了前驱依赖，值为true
        if (d[i].HasMember("Input")) {
            num_input = d[i]["Input"].IsArray() ? d[i]["Input"].Size() : 1;
        }
        //条件节点中的依赖关系都已经有了，属于控制依赖，只需要输出一次，不用多次遍历
        if (type_id == 3)//类型为branch,存在控制依赖
        {
            //对互斥表进行清空重建
            TvsF.clear();
            int con = d[i]["condition"].GetInt();//条件节点的位置,其类型是op
            Node *con_node = find_node_by_number(nodes, con);//条件节点的指针
            int sizeofT = d[i]["Statement_when_true"].Size();//T的节点数目
            int sizeofF = d[i]["Statement_when_false"].Size();//F的节点数目
            //TODO：HOW do control node?
            con_node->out_degree = sizeofF + sizeofT;//更新条件节点的出度,让其相加

            cout << "分支节点依赖" << endl << "条件节点:" << con << "  size of true:" << sizeofT << endl;
            cout << "T：";

            for (int there = 0; there < sizeofT; there++) {
                cout << d[i]["Statement_when_true"][there].GetInt() << "   ";
                Node *a = find_node_by_number(nodes, d[i]["Statement_when_true"][there].GetInt());
//a不是空节点,且当前控制节点没有指向
                if (a != NULL && (a->control == NULL || a->control->node_id < con)) {
                    if (a->control == NULL)//如果是第一次设置控制节点
                        inDegree[a->node_id]++;//节点a的入度++
                    a->control = con_node;
                    TvsF.insert(pair<int, bool>(a->node_id, true));//构建互斥
                    cout << "存在" << con << "到" << a->node_id << "的控制依赖" << endl;
                }

            }
            cout << endl << "F：";
            for (int there = 0; there < sizeofF; there++) {
                cout << d[i]["Statement_when_false"][there].GetInt() << "   ";
                Node *b = find_node_by_number(nodes, d[i]["Statement_when_false"][there].GetInt());

                if (b != NULL && (b->control == NULL || b->control->node_id < con))//b不是空节点,且当前控制节点没有指向
                {
                    if (b->control == NULL)
                        inDegree[b->node_id]++;//节点b的入度++
                    b->control = con_node;
                    TvsF.insert(pair<int, bool>(b->node_id, false));//构建互斥
                    cout << "存在" << con << "到" << b->node_id << "的控制依赖" << endl;
                }
            }
//                cout<<endl;
        }
        if (type_id == 4)//类型为loop
        {
            //loop的依赖比较独特，即需要依赖前面的执行结果，又需要依赖循环内对数据的改变结果
            //先输出控制依赖
            int con = d[i]["Condition"].GetInt();//条件节点的位置
            Node *con_node = find_node_by_number(nodes, con);

            cout << "loop节点依赖" << endl << "条件节点：" << con << endl;
            int sizeofL = d[i]["Statement_loop"].Size();//LOOP节点数目
            con_node->out_degree += sizeofL;//更新条件节点的出度

            for (int there = 0; there < sizeofL; there++) {
                int there_id = d[i]["Statement_loop"][there].GetInt();
//                    cout<<there_id<<endl;
                //1.该节点可能是OP，没有Dst，所以要先对类型进行判断
                //2.条件节点判断的输入也可能有多个，最好写成函数，直接套用OP之间的依赖
                //问题：there_id可能根本不在nodes中，nodes的依赖可能有很多，需要找到他的直接依赖
                Node *a = find_node_by_number(nodes, there_id);
                if (a != NULL && (a->control == NULL || a->control->node_id < con))//a不是空节点，且a当前控制节点没有指向或者是更大一级别的
                {
                    if (a->control == NULL)//如果第一次设置控制节点
                        inDegree[a->node_id]++;//入度++
                    a->control = con_node;
                    cout << "loop节点" << there_id << "的控制依赖为" << con_node->node_id << endl;

                }
            }
        }

        for (int j = i - 1; j >= 0; j--)//j比i小
        {
            //如果j有输出，j的Dst位置更新到hash表中
            if (d[j].HasMember("Dst")) {
                string key_dst = d[j]["Dst"].GetString();
                //非（已经存在，且值比j+1大,但是比i+1小）
                if (!(dst_id[key_dst] && (dst_id[key_dst] > j + 1) && (dst_id[key_dst] < i + 1))) {
                    //还要满足i+1和j+1不同在互斥表中,主要是，i+1false且j+1true
                    if (!(TvsF[i + 1] == false && TvsF[j + 1] == true)) {
                        dst_id[key_dst] = j + 1;
                    }
                }
            }

            switch (type_id) {

                case 1://类型为Op，input的类型都是string
                {
                    //设定：输入最多为两个
                    // string name_dst;
                    if (num_input == 2) {
                        string name_in1 = d[i]["Input"][0].GetString();
                        string name_in2 = d[i]["Input"][1].GetString();
                        if (dst_id[name_in1] && (!input_depend[0]) && (dst_id[name_in1] < i + 1))//存在且未被输出且节点对应关系正确
                        {
                            Node *a = find_node_by_number(nodes, dst_id[name_in1]);//当前输入依赖的节点1
                            if (a != nullptr)
                                a->out_degree++;//依赖的节点的出度++

                            input_depend[0] = true;
                            c->depend1 = a;//当前节点的输入依赖于a
                            inDegree[c->node_id]++;//当前节点入度++
//                            cout<<"节点"<<i+1<<"输入数量为string2，in1数据依赖："<<dst_id[name_in1]<<"->"<<i+1<<endl;
                            cout << "节点" << c->node_id << "输入数量为string2，in1数据依赖：" << a->node_id << "->"
                                 << c->node_id << endl;

                        }
                        if (dst_id[name_in2] && (!input_depend[1]) && (dst_id[name_in2] < i + 1))//存在
                        {
                            Node *b = find_node_by_number(nodes, dst_id[name_in2]);//当前输入依赖的节点2
                            if (b != nullptr)
                                b->out_degree++;

                            input_depend[1] = true;
                            c->depend2 = b;
                            inDegree[c->node_id]++;//入度++
//                        cout<<"节点"<<i+1<<"输入数量为string2，in2数据依赖："<<dst_id[name_in2]<<"->"<<i+1<<endl;
                            cout << "节点" << c->node_id << "输入数量为string2，in2数据依赖：" << b->node_id << "->"
                                 << c->node_id << endl;
//                        break;
                        }
                    } else //Op输入数量为1
                    {
                        string input_name = d[i]["Input"][0].GetString();

                        if (dst_id[input_name] && (dst_id[input_name] < i + 1) && (!input_depend[0]))//存在且值比i+1小且没输出过
                        {
                            Node *a = find_node_by_number(nodes, dst_id[input_name]);//指向输入依赖
                            if (a != nullptr)
                                a->out_degree++;

                            input_depend[0] = true;
                            c->depend1 = a;
                            inDegree[c->node_id]++;//入度++
//                        cout<<"节点"<<i+1<<"输入数量为string1，数据依赖："<<dst_id[input_name]<<"->"<<i+1<<endl;
                            cout << "节点" << c->node_id << "输入数量为string1，数据依赖：" << a->node_id << "->"
                                 << c->node_id << endl;
                        }
                    }
                }
                    break;
                case 2://类型为assign,默认只有一个输入
                {

                    //如果input是ID，则直接输出
                    if (d[i]["Input"].IsInt() && !input_depend[0]) {
                        Node *a = find_node_by_number(nodes, d[i]["Input"].GetInt());
                        if (a != nullptr)
                            a->out_degree++;

                        input_depend[0] = true;
                        c->depend1 = a;
                        inDegree[c->node_id]++;//入度++
                        cout << "节点" << i + 1 << "的输入为int，" << "数据依赖：" << d[i]["Input"].GetInt() << "->"
                             << i + 1 << endl;
                    }

                    //如果input是数，搜索hash表（立即数和变量名都会被表示为string）
                    if (d[i]["Input"].IsString() && !input_depend[0]) {
                        input_depend[0] = true;
                        string input_name = d[i]["Input"].GetString();
                        Node *a = find_node_by_number(nodes, dst_id[input_name]);
                        c->depend1 = a;
                        if (a != nullptr)
                            a->out_degree++;
                        if (dst_id[input_name] != 0)//利用立即数此值为0的性质修改入度
                            inDegree[c->node_id]++;//入度++
                        cout << "节点" << i + 1 << "的输入为string，" << "数据依赖：" << dst_id[input_name] << "->"
                             << i + 1 << endl;
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
    topologicalSort(nodes, inDegree, controlstep, id_pos);//函数使用，现在得到了controlstep vector<vector<Node>>,但是没有依赖关系
    vector<vector<Node *>> controlstep2;
    for (int i = 0; i < controlstep.size(); ++i) {
        vector<Node *> stepnow;//当前控制步，存放节点指针
        for (int j = 0; j < controlstep[i].size(); ++j) {
            stepnow.push_back(find_node_by_number(nodes, controlstep[i][j].node_id));//换成真正的依赖关系
        }
        controlstep2.push_back(stepnow);
    }

    for (int i = 0; i < controlstep.size(); ++i) {
        cout << "control step" << i + 1 << "大小为" << controlstep[i].size() << endl;
        for (int j = 0; j < controlstep[i].size(); ++j) {
            cout << controlstep2[i][j]->node_id << ":" << controlstep2[i][j]->operator_name << endl;
            cout << controlstep2[i][j]->node_id << "的依赖为"
                 << (controlstep2[i][j]->depend1 ? controlstep2[i][j]->depend1->node_id : 0) << " "
                 << (controlstep2[i][j]->depend2 ? controlstep2[i][j]->depend2->node_id : 0)
                 << endl;
        }
    }

    /*开始新的循环
     * 在初始控制步中加入逻辑函数
     * 调度算子
     * 先写单个循环，然后在最外层套上一个大的DSE迭代
     * */
    //我们规定，只能使用4/6输入的LUT
    //VHDL中，逻辑操作都是按位的，需要由一位操作拼接
    vector<lut_arr> array_list1;//lut阵列表
    vector<sa_arr> array_list2;//sa阵列表
    vector<magic_arr> array_list3;//magic阵列表

    //先判断设计目标，按照设计目标来循环给约束

    for (int i = 0; i < controlstep2.size(); i++) {
        cout << "step：" << i << endl;
        for (int j = 0; j < controlstep2[i].size(); ++j) {
            cout << "node_id：" << controlstep2[i][j]->node_id << endl
                 << "depend1：" << (controlstep2[i][j]->depend1 ? controlstep2[i][j]->depend1->node_id : 0) << "  "
                 << "depend2：" << (controlstep2[i][j]->depend2 ? controlstep2[i][j]->depend2->node_id : 0) << endl;
            //一层层遍历控制步，获取操作数，获取算子，分配阵列，计算速度、功耗、面积
            int type_operation = op2int(controlstep2[i][j]->operator_name);
            //运算阵列的大小，可能不对
            unsigned int lut_size = arr_size();
            unsigned int sa_size = arr_size();
            unsigned int magic_size = arr_size();
            //数据位数，bit_num_operand
            //先不管立即数
            //多种方式遍历，然后再剪枝
            //对于当前算子，目标是：找一个阵列/新建一个阵列，然后计算其参数增加，就结束
            //第一步，取操作数
            int input1_type = -1, input2_type = 0, input1_id = -1, input2_id = -1;
            //寻找的是节点存储的ID,或者执行完毕的输出位置
            find_input(input1_type, input1_id, type_operation, controlstep2[i][j]->depend1);//-1 R 1 lut 2 sa 3 magic
            find_input(input2_type, input2_id, type_operation, controlstep2[i][j]->depend2);


            //先讨论写回的情况:
            //假设初始输入和立即数被放在外部的寄存器中，因为我们无法索引他（in1 in2 没有id）
            if (type_operation == 0)//等号，其操作数为op或者立即数
                //不一定有依赖，如果常用的立即数，会将其交给寄存器
            {
                //如果没有依赖，交给寄存器
                if (controlstep2[i][j]->depend1 == nullptr) {
                    Reg_sum.write_num_sum++;//寄存器写
                    controlstep2[i][j]->do_type = -1;
                    controlstep2[i][j]->finish_id = -1;//证明写到了寄存器中
                    cout << "node_id：" << controlstep2[i][j]->node_id << "  operator_name："
                         << controlstep2[i][j]->operator_name << endl;
                    cout << "do_type： " << -1 << "  finish_id：" << -1 << endl;
                    //更新出度
                    out_degree(controlstep2[i][j]);
                    double time = time_now(array_list1, array_list2, array_list3, controlstep2[i][j]);
//update the time of do_array
                    time_update(0, controlstep2[i][j]->do_type, controlstep2[i][j]->finish_id, time,
                                controlstep2[i][j], array_list1, array_list2, array_list3);
                    //update the energy of reg
                    energy_update(0, -1, -1, controlstep2[i][j], array_list1, array_list2, array_list3);
                    //更新wb_pos,表示写到了寄存器中
                    controlstep2[i][j]->wb_pos[0].push_back(-1);
                    continue;//进行下一个循环
                }

                //有依赖,假设不会出现A=B这种直接赋值，则依赖一定来自OP, from array
                //统一，更改节点的执行结束id和do_type
                cout << "depend1 of  " << controlstep2[i][j]->node_id << " = " << controlstep2[i][j]->depend1->node_id
                     << endl;
                controlstep2[i][j]->finish_id = controlstep2[i][j]->depend1->finish_id;
                cout << "finish_id of depend1:" << controlstep2[i][j]->depend1->finish_id << endl;
                double time = time_now(array_list1, array_list2, array_list3, controlstep2[i][j]);
//update the time of do_array
                time_update(0, controlstep2[i][j]->depend1->do_type, controlstep2[i][j]->finish_id, time,
                            controlstep2[i][j], array_list1, array_list2, array_list3);
                //如果操作数来自MAGIC阵列,写回阵列中
                if (controlstep2[i][j]->depend1->do_type == 3) {
                    //节点行为
                    controlstep2[i][j]->do_type = 3;
                    controlstep2[i][j]->finish_id=controlstep2[i][j]->depend1->finish_id;
                    controlstep2[i][j]->wb_pos[3].push_back(controlstep2[i][j]->depend1->finish_id);//将阵列加入写回表,magic
                    //阵列行为：写++，添加存储节点
                    array_list3[controlstep2[i][j]->depend1->finish_id].write_number++;
                    //阵列写，但是不用能量更新，因为magic执行时直接写了，这里的写能量被算在执行能量中
//                    energy_update(type_operation,3,controlstep2[i][j]->depend1->finish_id, controlstep2[i][j],array_list1, array_list2, array_list3);
                    array_list3[controlstep2[i][j]->depend1->finish_id].store_node.push_back(controlstep2[i][j]->node_id);

                }
                if (controlstep2[i][j]->depend1->do_type == 2)//SA
                {
                    //TODO：先假设其执行类型为sa buffer,但是不更新时间能量，到再次使用时候再更新
                    //目前这个节点的写回表是空的
                    controlstep2[i][j]->do_type = 5;//SA BUFFER
                    //阵列行为：
                    //现在的sa-out是谁？
                    if (array_list2[controlstep2[i][j]->depend1->finish_id].sa_out)
                        {
                        int sa_out_now=array_list2[controlstep2[i][j]->depend1->finish_id].sa_out;
                        //判断其出度,假设出度不为0,并且没有被写回，将被覆盖掉，要找个地方写回
                        //先写回，再执行当前的=操作
                            if (find_node_by_number(nodes,sa_out_now)->out_degree>0 && find_node_by_number(nodes,sa_out_now)->wb_pos.empty())
                            {
                                //TODO：需要一个写回函数

                                    write_back(2,controlstep2[i][j],array_list1,array_list2,array_list3);//写回当前阵列

                            }
                    }

                    array_list2[controlstep2[i][j]->depend1->finish_id].sa_out=controlstep2[i][j]->node_id;
                }
                if (controlstep2[i][j]->depend1->do_type == 1)//LUT
                {
                    //TODO：先假设其执行类型为lut buffer,但是不更新时间能量，到再次使用时候再更新
                    //node
                    controlstep2[i][j]->do_type = 4;//LUT-OUT
                    //array
                    array_list1[controlstep2[i][j]->depend1->finish_id].lut_out = controlstep2[i][j]->node_id;
                }
                cout << "node_id：" << controlstep2[i][j]->node_id << "  operator_name："
                     << controlstep2[i][j]->operator_name << endl;
                cout << "do_type： " << controlstep2[i][j]->do_type << "  finish_id：" << controlstep2[i][j]->finish_id
                     << endl;
                //更新出度
                out_degree(controlstep2[i][j]);

                continue;//进行下一个循环
            }//下面的操作没有=了

            //先决定执行类型
            int do_array_type = 0, do_array_id = -1;//执行阵列的类型,id
            do_array_type = decide_array_type(type_operation, design_target);//决定执行阵列类型
            //更新节点的do_type,执行节点的do_type只有123
            controlstep2[i][j]->do_type = do_array_type;
            //如果要执行的类型当前没有阵列，则建立
            if (do_array_type == 1 && array_list1.empty() || do_array_type == 2 && array_list2.empty() ||
                do_array_type == 3 && array_list3.empty())
                do_array_id = build(do_array_type, type_operation, array_list1, array_list2, array_list3);
            //决定执行阵列的id
            do_array_id = decide_array_id(type_operation, controlstep2[i][j], nodes, do_array_type,
                                          array_list1, array_list2, array_list3, input1_type, input1_id, input2_type,
                                          input2_id);
            cout << "node_id：" << controlstep2[i][j]->node_id << "  operator_name：" << controlstep2[i][j]->operator_name
                 << endl;
            cout << "do_type： " << do_array_type << "  finish_id：" << do_array_id << endl;
            /*现在已知：操作数1类型，id；操作数2类型，id；执行阵列类型，执行阵列id；
            * 现在还没有进行读写执行
             *  节点的内容修改：
             *  阵列的内容修改：
            * */
            //从阵列中将数据读出，修改阵列/寄存器的读参数
            data_read(input1_type, input1_id, do_array_type, do_array_id, array_list1, array_list2,
                      array_list3);
            data_read(input2_type, input2_id, do_array_type, do_array_id, array_list1, array_list2,
                      array_list3);
            //将数据输入到执行阵列：input逻辑
            input_logic(input1_type, input1_id, input2_type, input2_id, do_array_type, do_array_id, controlstep2[i][j],
                        array_list1, array_list2, array_list3);
            //执行运算,要更新finish_id
            output_logic(do_array_type, do_array_id, type_operation, controlstep2[i][j], array_list1, array_list2,
                         array_list3);

            cout << "finish_id of this：" << controlstep2[i][j]->finish_id << endl;
            //写回在最开始，不用在执行
            //只要有写操作，就存在-内部有覆盖的情况，更新出度，清除被覆盖的节点，节点出度为0，找到存储他的阵列，将其擦除
            //补充控制节点，设计比较器件 ，看是否需要加其他运算器，如ALU
            //设计DSE

            //更新出度
            out_degree(controlstep2[i][j]);


        }

    }
    redirectCoutToFile(controlstep2, array_list1, array_list2, array_list3);
    return 0;
}




