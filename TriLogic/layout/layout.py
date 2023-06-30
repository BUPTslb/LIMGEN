import matplotlib.pyplot as plt

# 创建一个新的图形
fig, ax = plt.subplots()

# 创建方框并设置位置和大小
rect1 = plt.Rectangle((1, 1), 2, 2, fc='lightblue')
rect2 = plt.Rectangle((4, 3), 3, 2, fc='lightgreen')

# 添加方框到图形中
ax.add_patch(rect1)
ax.add_patch(rect2)

# 连接方框之间的连线
line = plt.Line2D([2, 4], [2, 4], color='red')

# 添加连线到图形中
ax.add_line(line)

# 设置坐标轴范围
ax.set_xlim(0, 8)
ax.set_ylim(0, 8)

# 隐藏坐标轴刻度
ax.set_xticks([])
ax.set_yticks([])

# 保存图形为PNG图片
plt.savefig('graph.png', dpi=300)

# 关闭图形
plt.close()
