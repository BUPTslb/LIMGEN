// 这是一个64位乘法器的Verilog代码实现
module mul64(
    input [63:0] a,
    input [63:0] b,
    output reg [127:0] p
);

    always @(*) begin
        p = a * b;
    end

endmodule

// ${INSERT_HERE}