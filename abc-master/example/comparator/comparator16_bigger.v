module comparator16_bigger(
    input [15:0] a,
    input [15:0] b,
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
