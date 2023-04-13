module comparator32_bigger(
    input [31:0] a,
    input [31:0] b,
    output reg gt
);
    always @(*) begin
         if (a > b) begin
            gt = 1;
         end else begin
            gt = 0;
	 end
	 end
endmodule
