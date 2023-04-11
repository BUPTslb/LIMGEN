module comparator_LUT(a, b, gt, eq, lt);
  parameter WIDTH = 32;
  input [WIDTH-1:0] a;
  input [WIDTH-1:0] b;
  output gt, eq, lt;

  (* rom_style = "distributed" *)
  (* parallel_case = "1" *)
  (* full_case = "1" *)
  (* memstyle = "lut" *)
  reg [2**WIDTH-1:0] LUT;
  integer i;

  always @* begin
    for (i = 0; i < 2**WIDTH; i = i + 1) begin
      if (a > b) begin
        LUT[i] = 3'b100; // a > b
      end else if (a == b) begin
        LUT[i] = 3'b010; // a == b
      end else begin
        LUT[i] = 3'b001; // a < b
      end
    end
  end

  assign gt = LUT[{a-b+2**WIDTH-1}];
  assign eq = LUT[{a-b+2**WIDTH-1}];
  assign lt = LUT[{a-b+2**WIDTH-1}];

endmodule

