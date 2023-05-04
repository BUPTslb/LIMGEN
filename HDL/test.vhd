entity test is
	port(
		i1:  in bit_vector(7 downto 0);
		i2:  in bit_vector(7 downto 0);
		out1: in bit_vector(7 downto 0);
		out2: in bit_vector(7 downto 0);
		clk:  in bit
	);	
end test;

architecture Behavioral of test is
signal in1:bit_vector(7 downto 0);
signal in1:bit_vector(7 downto 0);
signal A: bit_vector(7 downto 0);
signal B: bit_vector(7 downto 0);
signal C: bit_vector(7 downto 0);
signal D: bit_vector(7 downto 0);
begin
--当前只处理两个操作数的情况，多个呢？
process(clk)
begin
    in1 <= i1;
    in2 <= i2;
	A <= in1 and in2;
	A <= in1 * 2;
	B <= in1 or in2;
	C <= A xor B ;
	D <= C sll 1;
--有elseif的当前不太会处理
	if   A<0    then
		A <= not A;
	else
		A<= not A;
	end if;

	if   B=0    then
    		B <= not B;
    end if;

	if D>0 then
	    D<=D sll 1;
	else
	    D<=D srl 1;
	end if;

--立即数
	A <= 10010111;
	B <= 123;
	C <= 555;
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