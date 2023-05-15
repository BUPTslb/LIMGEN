--编码算法
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity arithmetic_encoder is
    Port ( clk      : in std_logic;
           reset    : in std_logic;
           input    : in std_logic_vector(31 downto 0);
           output   : out std_logic_vector(31 downto 0));
end arithmetic_encoder;

architecture Behavioral of arithmetic_encoder is

    signal low  : bit_vector(7 downto 0);
    signal high : bit_vector(7 downto 0);
    signal code : bit_vector(7 downto 0);
    signal range_low : bit_vector(7 downto 0);
    signal range_high : bit_vector(7 downto 0);
    signal count: bit_vector(7 downto 0);
    signal cum_freq : bit_vector(7 downto 0);
    signal symbol_freq : bit_vector(7 downto 0);
    signal range_size :bit_vector(7 downto 0);
    signal total_freq :bit_vector(7 downto 0);

begin
    process (clk)
begin
            low<=0;
            high<=0;
            code<=0;
            M<=16;
            P<=16;
            input<=0;
            cum_freq<=20;
            count<=10;
            range_low<=5;
            range_high<=20;
            range_size<=15;
            symbol_freq<=3;
            total_freq<=5;


            low <= 0;
            high <= P+1;
            code <= 0;

            range_size <= high + low ;
            range_size <= range_size + 1;

            range_high <= range_low + cum_freq;

            cum_freq <= not symbol_freq;
            cum_freq <= cum_freq + symbol_freq;
            cum_freq <= cum_freq + symbol_freq;
            cum_freq <= cum_freq + symbol_freq;
            cum_freq <= cum_freq + symbol_freq;
            cum_freq <= cum_freq + symbol_freq;
            cum_freq <= not total_freq;
            cum_freq <=cum_freq + total_freq;
            cum_freq <=cum_freq + total_freq;
            cum_freq <=cum_freq + total_freq;
            cum_freq <=cum_freq + total_freq;
            cum_freq <=cum_freq + total_freq;
            cum_freq <=cum_freq + total_freq;
            cum_freq <=cum_freq + total_freq;
            cum_freq <=cum_freq + total_freq;
            cum_freq <=cum_freq + total_freq;
            cum_freq <=cum_freq + total_freq;
            cum_freq <=cum_freq + total_freq;
            cum_freq <=cum_freq + total_freq;
            cum_freq <=cum_freq + total_freq;


            range_high <= range_high +1;
            input <= input xor 2;
            high <= not range_high;
            low <= range_high + 1;

            output <= 101010;
            code <= code + 101010;
            code <= code + 1;
            count <= count + 1;
            high <= high +101010;
            high <= high +1;
            low <= low+2020;
            code <= code +2020;
            code <= code + 1;
            count <= count + 1;
            high <= high +2020;
            high <= high +1;
            low <= low +2020 ;
            code <= code +2020;
            code <= code + 1;
            count <= count + 1;
            high <= high +1211;
            high <= high +1;
            low <= low+22;
            code <= code +32;
            code <= code + 1;
            count <= count + 1;
            high <= high +55;
            high <= high +1;
            low <= low +55;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high +2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low  + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;
            code <= code + 2;
            code <= code + 1;
            count <= count + 1;
            high <= high + 2;
            high <= high +1;
            low <= low + 2;

            code <= code + 2 ;
            code <= code + 1 ;
            count <= count + 1;
            output<= 101010101;
            count <= count + 32;


    end process;

end Behavioral;