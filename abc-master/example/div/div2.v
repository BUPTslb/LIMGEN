// 定义模块名和端口
module div2 (
    input signed [1:0] dividend,
    input signed [1:0] divisor,
    output signed [1:0] quotient,
    output signed [1:0] remainder,
);
 
    assign quotient = dividend / divisor;
    assign remainder = dividend % divisor;

endmodule
