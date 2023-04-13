// 这是一个64位乘法器的Verilog代码实现
module mul16(
    input [15:0] a,
    input [15:0] b,
    output reg [31:0] p
);

    always @(*) begin
        p = a * b;
    end

endmodule
