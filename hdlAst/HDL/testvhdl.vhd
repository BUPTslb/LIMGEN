library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
entity testvhdl is
	port(
		in1: in bit_vector(7 downto 0);
		in2: in bit_vector(7 downto 0);
		out1: in bit_vector(7 downto 0);
		out2: in bit_vector(7 downto 0);
		clk: in bit
				
	);	
end testvhdl;

architecture Behavioral of testvhdl is
signal A,B,C,D: bit_vector(7 downto 0);
begin
process(clk)
begin
	A <= in1 and in2;
	B <= in1 or in2;
	C <= (not A) xor B ;
	D <= C sll 1;
	out1 <=A nor C ;
	out2 <=B nor D;	
end process

end Behavioral;
