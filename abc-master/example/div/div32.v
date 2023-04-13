// 定义模块名和端口
module div32 (
    input signed [31:0] dividend,
    input signed [31:0] divisor,
    output signed [31:0] quotient,
    output signed [31:0] remainder,
);
 
    assign quotient = dividend / divisor;
    assign remainder = dividend % divisor;

endmodule
