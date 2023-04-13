// 定义模块名和端口
module div64 (
    input signed [63:0] dividend,
    input signed [63:0] divisor,
    output signed [63:0] quotient,
    output signed [63:0] remainder,
);
 
    assign quotient = dividend / divisor;
    assign remainder = dividend % divisor;

endmodule


