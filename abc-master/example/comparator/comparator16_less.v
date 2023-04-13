module comparator16_less(
    input [15:0] a,
    input [15:0] b,
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
