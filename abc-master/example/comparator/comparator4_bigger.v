module comparator4_bigger(
    input [3:0] a,
    input [3:0] b,
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
