module comparator4_less(
    input [3:0] a,
    input [3:0] b,
    output reg ls
);

    always @(*) begin
         if (a < b) begin
            ls = 1;
         end else begin
            ls = 0;
	 end
	 end

endmodule
