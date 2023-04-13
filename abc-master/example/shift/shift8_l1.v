module shift8_l1(
    input [7:0] a, // 输入a，2
    output [7:0] out // 输出sum
);

assign out = a * 2; // 逻辑左移

endmodule// 64位加法

