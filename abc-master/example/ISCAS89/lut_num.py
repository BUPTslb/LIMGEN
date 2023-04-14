from collections import defaultdict
import re
# 使用正则表达式匹配不符合要求的字符串
# 读取.blif文件的.input一行，.input后面的都是输入，将每一个输入都设为正则表达式匹配项
# 在with open() as f语句块中，使用for循环遍历文件的每一行，当读取到以'.input'开头的行时，使用split()方法将该行按空格分割成一个列表，然后使用切片操作[1:]获取除了第一个元素（即'.input'）以外的所有元素，即输入的名称，然后使用join()方法将这些名称连接成一个字符串，中间用'|'隔开，最后使用re.compile()方法将该字符串编译成一个正则表达式对象，存储在pattern变量中。

with open(blif_file, 'r') as f:
    for line in f:
        if line.startswith('.input'):
            inputs = line.split()[1:]
            pattern = re.compile('|'.join(inputs))



def topo_sort(graph):
    # pattern =r'\$abc\$\d*+\$[a-b]\[\d*\]'
    in_degree = defaultdict(int)
    for node in graph:
        for neighbor in graph[node]:
            if neighbor.startswith('$abc$') and not re.match(pattern,neighbor):
                in_degree[node] += 1
            else:
                in_degree[node] = -1
                print(node," indegree = ",in_degree[node])

    result=[]
    i=0
    while i<20:
        # graph-=queue
        queue = [node for node in graph if in_degree[node] == 0]
        #set indegree =-1
        for j in queue:
            in_degree[j] = -1
        
        # print(f"queue {i},size = {len(queue)} \n",queue)
        result.append(len(queue))
        # result[i]=queue
        while queue:
            node = queue.pop(0)
            # print("pop of queue =",node)
            for j in graph:
                for neighbor in graph[j]: 
                    if neighbor==node:
                        in_degree[j] -= 1
        
        # print("length of graph:",len(graph))
        i=i+1
        
    # print("result of toposort: ",result)
    return result

def group_luts(blif_file):
    lut_groups = []
    graph = defaultdict(set)
    # pattern =r'\$abc\$\d*+\$[a-b]\[\d*\]'
    with open(blif_file, 'r') as f:
        for line in f:
            if line.startswith('.names'):
                lut_inputs = set()
                lut_outputs = set()
                elements = line.split()
                if len(elements) <= 2:
                    continue
                output = elements[-1] # 将最后一个元素赋值给output变量
                # print("output为：",output)
                inputs = elements[1:-1]
                lut_inputs.update(inputs)
                lut_outputs.add(output)
                for out in lut_outputs:
                    # if out.startswith('$abc$') and not re.match(pattern,out):
                    if not re.match(pattern,out):    
                        for input in lut_inputs:
                            graph[out].add(input)
    # print(graph)
                
    lut_groups = topo_sort(graph)
    res=0
    level=0
    for p in lut_groups:
        res+=p
        if p!=0:
            level+=1
    lut_groups.append(res)
    lut_groups.append(00)
    lut_groups.append(level)
    return lut_groups
  


import os

# 获取out文件夹中所有的.blif文件
blif_files = [f for f in os.listdir('out') if f.endswith('.blif')]

# 打开comparator.txt文件以写入模式
with open('iscas.txt', 'w') as f:
    # 遍历所有的.blif文件
    for blif_file in blif_files:
        # 调用group_luts函数并将结果写入comparator.txt文件中
        f.write(f'Results for {blif_file}:\n')
        results = group_luts(os.path.join('out', blif_file))
        f.write(str(results))
        f.write('\n\n')