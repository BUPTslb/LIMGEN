// 定义模块名和端口
module div4 (
    input signed [3:0] dividend,
    input signed [3:0] divisor,
    output signed [3:0] quotient,
    output signed [3:0] remainder,
);
 
    assign quotient = dividend / divisor;
    assign remainder = dividend % divisor;

endmodule
