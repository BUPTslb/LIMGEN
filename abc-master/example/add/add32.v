module add32(
    input [31:0] a, // 输入a，64位
    input [31:0] b, // 输入b，64位
    output [31:0] sum // 输出sum，64位
);

assign sum = a + b; // 将a和b相加并赋值给sum

endmodule// 64位加法