from graphviz import Digraph
from hdlConvertorAst.hdlAst import HdlIdDef, HdlModuleDec, HdlModuleDef, HdlStmProcess,\
    HdlStmBlock, HdlStmAssign, HdlOp, HdlStmIf, HdlStmWhile, iHdlExpr, HdlValueId, HdlValueInt, HdlOpType
from hdlConvertorAst.to.json import ToJson
import json
from hdlConvertorAst.to.vhdl.vhdl2008 import ToVhdl2008
import os
import sys

from hdlConvertor import HdlConvertor
from hdlConvertorAst.language import Language

# TEST_DIR = os.path.join("..", "tests", "verilog")

TEST_DIR = os.path.join("..", "HDL")
filenames = [os.path.join(TEST_DIR, "RCA23.vhd"), ]
include_dirs = []
# 定义存储字典的列表
dic = []
c = HdlConvertor()
# c为HdlConvertor类的实例化
# note that there is also Language.VERILOG_2005, Language.SYSTEM_VERILOG_2017 and others
# d是对文件test.vhd的VHDL2008解析，这里应该已经生成了AST
d = c.parse(filenames, Language.VHDL_2008, include_dirs,
            hierarchyOnly=False, debug=False)
# print(d)

# tv = ToVhdl2008(sys.stdout)
# #这个函数：将AST重新转到VHDL2008
# #tv是类ToVhdl2008的实例化对象，d是HdlContext,tv调用输出
# tv.visit_HdlContext(d)


tj = ToJson()
j = tj.visit_HdlContext(d)
# j就是解析之后的VHDL代码,输出其类型为<list>
print(json.dumps(j, sort_keys=True,
                 indent=4, separators=(',', ': ')))
# 以上函数会对生成的json进行可视化友好的输出
# d的类型<'hdlConvertorAst.hdlAst._structural.HdlContext'>
# print('type of j=:', type(j), '\n')
for o in d.objs:
    print('type of d.objs:\n',type(o),'\n',o)
# obj有两种类型：
# <class 'hdlConvertorAst.hdlAst._structural.HdlModuleDec'> 声明，entity部分
# <class 'hdlConvertorAst.hdlAst._structural.HdlModuleDef'> 定义，architecture部分


def Node_ID(N):
    N = N+1
    # print('Node_ID  :', N)
    return N


def DoOp(src, N):
    N = Node_ID(N)
    # 在这里直接创建字典，输出也是字典
    dic.append(dict(Node_ID=N, Type='Op', Input=src.ops, Operation=src.fn))
    print(dic[N-1], '\n')
    return N


def DoAssign(body, N):
     # 定义函数处理Assign类型
    if isinstance(body.src, HdlOp):
        N = DoOp(body.src, N)
        N_src = N  # 记录表达式节点的位置
    else:
        N_src = body.src  # 暂时先这样记录，为了直接使用N_src作输出
    N = Node_ID(N)
    dic.append(dict(Node_ID=N, Type='Assign', Input=N_src,
                    Operation='SEQ', Dst=body.dst))
    print(dic[N-1], '\n')
    return N

def DoIf(body, N):
    # 定义函数处理If类型
    #先定义好list_true和false放在前面
    list_true = []
    list_false = []
    # cond类型：
    if body.cond:
        N=DoOp(body.cond,N)
        N_cond = N  # 记录条件的位置
    if body.if_true:
        #把condition的节点加上
        # list_true = []
        True_start = N
        # print('---Statements When True---')
        N = DoBlock(body.if_true, N)
        True_end = N
        for i in range(True_start+1, True_end+1):
            list_true.append(i)
    # elif类型：列表[元组]，现将其改成list类型。可能为空，要先做判断
    if body.elifs:  # 列表不为空
        #false指向elifs或者if_false
        body.elifs[0] = list(body.elifs[0])
        print('Type     : Elsif')
        N = DoOp(body.elifs[0][0], N) #处理elseif的条件节点
        elsif_op=N
        N = DoBlock(body.elifs[0][1], N)
    if body.if_false:
        # list_false = [] #初始化列表
        false_start = N
        print('---Statements When False---   ',N)
        N = DoBlock(body.if_false, N)
        false_end = N
        for i in range(false_start+1, false_end+1):
            list_false.append(i)
    # cond类型：包含条件和if判断两部分，其中if部分要输出T和F分别的指向
    # 输出条件节点
    # N = DoOp(body.cond, N)
    
    # 输出分支节点
    N = Node_ID(N)
    #这里要多一步判断，因为可能没有list_false
    dic.append(dict(Node_ID=N, condition=N_cond,Type='Branch',
                    Statement_when_true=list_true, Statement_when_false=list_false))
    print(dic[N-1], '\n')
    return N


def DoWhile(body, N):
    # 定义函数处理While类型
    # 循环也是分支结构节点，在条件判断下增加一个分支节点，将loop内容输出
    # 进入Block中的节点，和返回Block的节点之间存在一个数量差，就是循环中的节点数量
    
    N = DoOp(body.cond, N)
    N_cond = N  # 记录条件的节点号码
    N0 = N  # 起始节点
    N = DoBlock(body.body, N)
    N1 = N  # 终止节点
    N = Node_ID(N)
    # 应该创建一个列表，记录LOOP中的所有节点号码然后输出，因此先处理内部，在处理外部

    list = []
    for i in range(N0+1, N1+1, 1):
        list.append(i)
    dic.append(dict(Node_ID=N, Type='Loop',
                    Condition=N_cond, Statement_loop=list))
    # print('End Loop\n')
    print(dic[N-1], '\n')
    return N


def DoBlock(block, N):
    for m in block.body:
        # 需要递归调用Block，加上对body类型的条件判断引用其他函数
        if isinstance(m, HdlStmAssign):
            N = DoAssign(m, N)
        if isinstance(m, HdlStmIf):
            N = DoIf(m, N)
        if isinstance(m, HdlStmWhile):
            N = DoWhile(m, N)
        if isinstance(m, HdlStmBlock):
            N = DoBlock(m, N)
    return N


class MyEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, HdlValueId):
            return str(obj)
        elif isinstance(obj, HdlOpType):
            return str(obj)
        elif isinstance(obj, HdlValueInt):
            return str(obj)
        else:
            return super(MyEncoder, self).default(obj)


for o in d.objs:
    # 如果obj是声明，即为entity，输出port名
    # isinstance函数，判断实例对象是否是已知的类型
    # if isinstance(o, HdlModuleDec):
    #     for i in o.ports:
    #         print(i.name)
    # 如果obj是定义，即为architecture
    if isinstance(o, HdlModuleDef):
        print('\n')
        for i in o.objs:
            if isinstance(i, HdlStmProcess):
                # 找到HdlStmProcess
                N = 0
                DoBlock(i.body, N)
                # 改成string类型输出
                print(json.dumps(dic, sort_keys=True, cls=MyEncoder,
                                 indent=4, separators=(',', ': ')))
                # 将输出设置为json文件，其中包含节点信息，可以由此进行CDFG的绘制
                out_file = open("RCA23.json", "w")
                json.dump(dic, out_file, indent=4,
                          sort_keys=True, cls=MyEncoder)
                out_file.close()
    # HdlStmProcess的body是HdlStmBlock
    # HdlStmBlock的body是[],列表类型list

