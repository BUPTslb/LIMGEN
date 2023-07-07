import matplotlib.pyplot as plt
import numpy as np

import json

with open('output.json') as f:
    data = json.load(f)
n_array = len(data)
print(type(data))
# data每一个阵列都是一个字典


x_num = int(np.ceil(np.sqrt(len(data))))
y_num = x_num
# 获取初始的各矩阵左下标的坐标，并存储为列表
axises = np.zeros((x_num, y_num, 2))
# 一个专门的矩阵存储阵列的宽度和长度
sizes = np.zeros((x_num, y_num, 2))
for i in data:
    sizes[i['X']][i['Y']][0] = i['WIDTH']
    sizes[i['X']][i['Y']][1] = i['HEIGHT']
# 创建布局矩阵
connect = np.full((x_num, y_num), -1)
every_x = np.zeros(x_num)
every_y = np.zeros(y_num)
for i in range(x_num):
    if i == 0:
        every_x[i] = 4 * n_array
    else:
        every_x[i] = every_x[i - 1] + max(sizes[i - 1, :, 0]) + 4 * n_array

for i in range(y_num):
    if i == 0:
        every_y[i] = 4 * n_array
    else:
        every_y[i] = every_y[i - 1] + max(sizes[:, i - 1, 1]) + 4 * n_array

Y = []
X = []
for i in data:
    connect[i['X']][i['Y']] = i['ID']
    # 获取边缘X Y的数值
    while len(Y) <= i['X']:
        Y.append(0)
    while len(X) <= i['Y']:
        X.append(0)
    Y[i['X']] += i['HEIGHT']
    X[i['Y']] += i['WIDTH']
print(connect)
print(X)
print(Y)

Y_max = max(Y)
X_max = max(X)
# 生成左下角坐标矩阵
# 按照先确定x，在确定y的顺序，每一列的x坐标全部对齐，按照最大值+n_array的方式计算
for i in range(x_num):
    for j in range(y_num):
        axises[i][j][0] = every_x[i]
        axises[i][j][1] = every_y[j]

print(axises)
print(sizes)

# 创建一个新的图形
fig, ax = plt.subplots()
plt.axis('equal')
plt.axis('square')
# 隐藏坐标轴刻度
ax.set_xticks([])
ax.set_yticks([])
# 1.读取json文档，获取阵列的个数n_array和线的个数n_line
# 2.根据阵列的个数n_array、尺寸（width、length）和线的个数n_line，确定图形的大小(坐标范围)
# 3.根据阵列的个数n_array和线的个数n_line，确定阵列的位置和大小

# 绘制阵列
# 设置坐标轴范围

print("X_Y_MAX", X_max, Y_max)
ax.set_xlim(0, X_max + (1 + x_num) * 4 * n_array)
ax.set_ylim(0, Y_max + (1 + y_num) * 4 * n_array)


# 创建方框并设置位置和大小
# 语法：Rectangle((x, y), width, height, **kwargs)
def draw_rect(i, x, y):
    print("ID:", i['ID'], "i_X", i['X'], "i_Y", i['Y'])
    if i['TYPE'] == 1:
        rect = plt.Rectangle((x, y), i['WIDTH'], i['HEIGHT'], fc='lightcoral', edgecolor='black')
    elif i['TYPE'] == 2:
        rect = plt.Rectangle((x, y), i['WIDTH'], i['HEIGHT'], fc='lightgreen', edgecolor='black')
    elif i['TYPE'] == 3:
        rect = plt.Rectangle((x, y), i['WIDTH'], i['HEIGHT'], fc='lightblue', edgecolor='black')
    ax.add_patch(rect)


# 绘制阵列
for i in data:
    draw_rect(i, axises[i['X']][i['Y']][0], axises[i['X']][i['Y']][1])

# 当前可用的数组
# axises[][][]:每个阵列左下角的坐标
# sizes[][][]:每个阵列的宽度和长度
# connect[][]:每个位置上阵列的ID
# every_x[]:每一列左侧对齐的x坐标
# every_y[]:每一行下侧对齐的y坐标
# x的路径标记数组
label_x = np.zeros(X_max + (1 + x_num) * 4 * n_array)
label_y = np.zeros(Y_max + (1 + y_num) * 4 * n_array)


# j在i的位置，从i到j，定义走线函数，左上0、上1、右上2、右3、右下4、下5、左下6、左7：0、1、2、3、4、5、6、7
def route_decide(i, j):
    if axises[i['X']][i['Y']][0] == axises[data[j]['X']][data[j]['Y']][0]:
        if axises[i['X']][i['Y']][1] > axises[data[j]['X']][data[j]['Y']][1]:
            return 5
        else:
            return 1
    elif axises[i['X']][i['Y']][1] == axises[data[j]['X']][data[j]['Y']][1]:
        if axises[i['X']][i['Y']][0] > axises[data[j]['X']][data[j]['Y']][0]:
            return 7
        else:
            return 3
    elif axises[i['X']][i['Y']][0] > axises[data[j]['X']][data[j]['Y']][0]:
        if axises[i['X']][i['Y']][1] > axises[data[j]['X']][data[j]['Y']][1]:
            return 6
        else:
            return 0
    elif axises[i['X']][i['Y']][0] < axises[data[j]['X']][data[j]['Y']][0]:
        if axises[i['X']][i['Y']][1] > axises[data[j]['X']][data[j]['Y']][1]:
            return 4
        else:
            return 2


#路由宗旨，上下出，左右进
def routing0(i, j):
# j在i的左上方，从i到j，右下到左上，先向上，再向左走到j右侧一列，再向上走到j
# 需要5个坐标(i的上侧坐标，向上走一段的坐标，向左走一段的坐标，再向上走一段的坐标（不能走到j的右侧），j的右侧连接坐标)
    print("j", j)
    x1 = [axises[i['X']][i['Y']][0] + i['WIDTH'] / 2, axises[data[j]['X']][data[j]['Y']][0] + data[j]['WIDTH'] / 2]
    print("x1", x1)
    y1 = [axises[i['X']][i['Y']][1] + i['HEIGHT'] / 2,
        axises[data[j]['X']][data[j]['Y']][1] + data[j]['HEIGHT'] / 2]
    print("y1", y1)
    if i['CONNECT_LINE'][j] == 0:
        return
    line1 = plt.Line2D(x1, y1, lw=0.5+5*np.log2(i['CONNECT_LINE'][j])/n_array, color='black')
    ax.add_line(line1)


def routing1(i, j):
# j在i的上方，从i到j，下到上，先向上，再向右走到右侧空地，向上走到j右侧，再连接j
# 需要5个坐标(i的上侧坐标，向上走一段的坐标，向右走一段的坐标，再向上走一段的坐标（不能走到j的右侧），j的右侧连接坐标)

    return 0

def routing2(i, j):
# j在i的右上方，从i到j，左下到右上，先向上，再向右走到j左侧一列，再向上走到j左侧，再连接j
# 需要5个坐标(i的上侧坐标，向上走一段的坐标，向右侧走一段的坐标，再向上走一段的坐标（不能走到j的左侧），j的左侧连接坐标)

    return 0

def routing3(i, j):
# j在i的右方，从i到j，左到右，先向下，再向右走到j左侧一列，再向上走到j左侧，再向右连接j
# 需要5个坐标(i的下侧坐标，向下走一段的坐标，向右走一段的坐标，向上走一段的坐标，再向右j的左侧连接坐标)

    return 0

def routing4(i, j):
# j在i的右下方，从i到j，左上到右下，先向下，再向右走到j左侧空地，再向下走到j左侧，再连接j
# 需要5个坐标(i的下侧坐标，向下走一段的坐标，向右走一段的坐标，再向下走一段的坐标（不能走到j的左侧），j的左侧连接坐标)

    return 0

def routing5(i, j):
# j在i的下方，从i到j，上到下，先向下，再向右走到右侧空地，再向下走到j右侧，再连接j
# 需要5个坐标(i的下侧坐标，向下走一段的坐标，向右走一段的坐标，再向下走一段的坐标（不能走到j的右侧），j的右侧连接坐标)

    return 0

def routing6(i, j):
# j在i的左下方，从i到j，右上到左下，先向下，再向左走到j右侧空地，再向下走到j右侧，再连接j
# 需要5坐标(i的下侧坐标，向下走一段的坐标，向左走一段的坐标，再向下走一段的坐标（不能走到j的右侧），j的右侧连接坐标)

    return 0

def routing7(i, j):
# j在i的左方，从i到j，右到左，先向下，再向左走到j右侧空地，再向上走到j右侧，再连接j
# 需要5坐标(i的下侧坐标，向下走一段的坐标，向左走一段的坐标，再向上走一段的坐标（不能走到j的右侧），j的右侧连接坐标)

    return 0



for i in data:
    for j in range(i['ID'] + 1, n_array):
        # 对位置做出判断
        if route_decide(i,j) == 0:
            routing0(i, data[j])
        elif route_decide(i, j) == 1:
            routing1(i, data[j])
        elif route_decide(i, j) == 2:
            routing2(i, data[j])
        elif route_decide(i, j) == 3:
            routing3(i, data[j])
        elif route_decide(i,j) == 4:
            routing4(i, data[j])
        elif route_decide(i, j) == 5:
            routing5(i, data[j])
        elif route_decide(i, j) == 6:
            routing6(i, data[j])
        elif route_decide(i, j) == 7:
            routing7(i, data[j])
        else:
            continue



# 保存图形为PNG图片
plt.savefig('graph.png', dpi=300)

# 关闭图形
plt.close()
