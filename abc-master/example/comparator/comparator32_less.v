module comparator32_less(
    input [31:0] a,
    input [31:0] b,
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
