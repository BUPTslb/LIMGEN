// 这是一个2位乘法器的Verilog代码实现
module mul2(
    input [1:0] a,
    input [1:0] b,
    output reg [3:0] p
);

    always @(*) begin
        p = a * b;
    end

endmodule

