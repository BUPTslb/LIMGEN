module comparator_bigger(
    input [63:0] a,
    input [63:0] b,
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
