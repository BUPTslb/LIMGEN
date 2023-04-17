module add16(
    input [15:0] a, // 输入a，64位
    input [15:0] b, // 输入b，64位
    output [15:0] sum // 输出sum，64位
);

assign sum = a + b; // 将a和b相加并赋值给sum

endmodule// 64位加法