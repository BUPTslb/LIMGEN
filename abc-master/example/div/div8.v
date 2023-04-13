// 定义模块名和端口
module div8 (
    input signed [7:0] dividend,
    input signed [7:0] divisor,
    output signed [7:0] quotient,
    output signed [7:0] remainder,
);
 
    assign quotient = dividend / divisor;
    assign remainder = dividend % divisor;

endmodule
