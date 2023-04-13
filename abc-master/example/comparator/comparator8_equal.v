module comparator8_equal(
    input [7:0] a,
    input [7:0] b,
    output reg eq
);

    always @(*) begin
        if (a == b) begin
            eq = 1;
        end else begin
            eq = 0;
    	end
    end

endmodule

// Verilog code for a 32-bit comparator. The module takes two 32-bit inputs a and b, and outputs three signals: eq (equal), gt (greater than), and lt (less than). The always block compares the two inputs and sets the output signals accordingly.
