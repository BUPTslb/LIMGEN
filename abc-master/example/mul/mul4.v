// 这是一个64位乘法器的Verilog代码实现
module mul4(
    input [3:0] a,
    input [3:0] b,
    output reg [7:0] p
);

    always @(*) begin
        p = a * b;
    end

endmodule
