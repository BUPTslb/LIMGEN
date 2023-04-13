module shift16_r1(
    input [15:0] a, // 输入a，2
    output [15:0] out // 输出sum
);

assign out = a / 2; // 逻辑右移

endmodule// 64位加法