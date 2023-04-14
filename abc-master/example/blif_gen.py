import os

# 设置yosys命令
yosys_cmd = "yosys -p 'read_verilog {}.v; hierarchy -check -top {}; proc; opt; fsm; opt; memory; opt;  techmap; opt; \
 synth -top {} -lut 4; write_blif ISCAS85/out/{}_lut4.blif; synth -top {} -lut 6; write_blif ISCAS85/out/{}_lut6.blif'"

# 遍历example文件夹中的所有.v文件
for file_name in os.listdir("ISCAS85"):
    if file_name.endswith(".v"):
        # 获取文件名（不包括后缀）
        name = os.path.splitext(file_name)[0]
        # 执行yosys命令
        os.system(yosys_cmd.format("ISCAS/" + name, name, name, name, name, name))
