// 这是一个64位乘法器的Verilog代码实现
module mul8(
    input [7:0] a,
    input [7:0] b,
    output reg [15:0] p
);

    always @(*) begin
        p = a * b;
    end

endmodule
