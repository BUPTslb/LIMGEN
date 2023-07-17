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
print("connect ",connect)
#every_x和every_y分别存储每一列的x坐标和每一行的y坐标(统一化)
every_x = np.zeros(x_num)
print("every_x ",every_x)
every_y = np.zeros(y_num)
print("every_y ",every_y)
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

print("axises: ",axises)

print("size: ",sizes)

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

print("X_Y_MAX:", X_max, Y_max)
ax.set_xlim(0, X_max + n_array)
ax.set_ylim(0, Y_max + n_array)


# 创建方框并设置位置和大小
# 语法：Rectangle((x, y), width, height, **kwargs)
# x,y：矩形左下角的坐标
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

# 保存图形为PNG图片
plt.savefig('graph.png', dpi=300)

# 关闭图形
plt.close()