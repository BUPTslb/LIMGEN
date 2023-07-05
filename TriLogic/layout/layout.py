import matplotlib.pyplot as plt
import json

# 创建一个新的图形
fig, ax = plt.subplots()
plt.axis('equal')
plt.axis('square')
# 隐藏坐标轴刻度
ax.set_xticks([])
ax.set_yticks([])
#1.读取json文档，获取阵列的个数n_array和线的个数n_line
#2.根据阵列的个数n_array、尺寸（width、length）和线的个数n_line，确定图形的大小(坐标范围)
#3.根据阵列的个数n_array和线的个数n_line，确定阵列的位置和大小

# 设置坐标轴范围
ax.set_xlim(0, 20)
ax.set_ylim(0, 20)


# 创建方框并设置位置和大小
#语法：Rectangle((x, y), width, height, **kwargs)
rect1 = plt.Rectangle((1, 3), 2, 2, fc='lightblue')
rect2 = plt.Rectangle((4, 3), 2, 2, fc='lightgreen')
rect3 = plt.Rectangle((10, 5), 2, 2, fc='red')


# 添加方框到图形中
ax.add_patch(rect1)
ax.add_patch(rect2)
ax.add_patch(rect3)

# 绘制连线
#语法：Line2D(xdata, ydata, **kwargs)
x1 = [3, 4]
y1 = [4, 4]
line1 = plt.Line2D(x1, y1, lw=2.5, color='black')
ax.add_line(line1)
#绘制rect2和rect3之间的连线
x2 = [6, 10,10]
y2 = [10, 10,2]
line2 = plt.Line2D(x2, y2, lw=2.5, color='black')
ax.add_line(line2)







# 保存图形为PNG图片
plt.savefig('graph.png', dpi=300)

# 关闭图形
plt.close()
