// 定义模块名和端口
module div16 (
    input signed [15:0] dividend,
    input signed [15:0] divisor,
    output signed [15:0] quotient,
    output signed [15:0] remainder,
);
 
    assign quotient = dividend / divisor;
    assign remainder = dividend % divisor;

endmodule
