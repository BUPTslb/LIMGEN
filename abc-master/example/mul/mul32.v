// 这是一个64位乘法器的Verilog代码实现
module mul32(
    input [31:0] a,
    input [31:0] b,
    output reg [63:0] p
);

    always @(*) begin
        p = a * b;
    end

endmodule
