entity test is
	port(
		in1:  in bit_vector(7 downto 0);
		in2:  in bit_vector(7 downto 0);
		out1: in bit_vector(7 downto 0);
		out2: in bit_vector(7 downto 0);
		clk:  in bit
	);	
end test;

architecture Behavioral of test is
signal A: bit_vector(7 downto 0);
signal B: bit_vector(7 downto 0);
signal C: bit_vector(7 downto 0);
signal D: bit_vector(7 downto 0);
begin
--当前只处理两个操作数的情况，多个呢？
process(clk)
begin
	A <= in1 and in2;
	B <= in1 or in2;
	C <= A xor B ;
	D <= C sll 1;
--有elseif的当前不太会处理
	if   A<0    then
		A <= not A;
	-- elsif A=0	then
	-- 	A <= B;
	-- 	A <=A+B;
	else
		A<= not A;
	end if;

	if D>0 then
	    D<=D sll 1;
	else
	    D<=D srl 1;
	end if;

--立即数
	A <= -1;
	while A<0 loop
	    A <= A srl 1;
	end loop;

--嵌套
	while D>0 loop
	    if D < 5 then
	        if A >0 then
	            D<=D+A;
	        end if;
	    end if;

		D <= D sll 1;
	end loop ; -- identifier


	out1 <=A nor C ;
	out2 <=B nor D;	

end process;

end Behavioral;