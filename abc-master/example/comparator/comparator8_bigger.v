module comparator8_bigger(
    input [7:0] a,
    input [7:0] b,
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
