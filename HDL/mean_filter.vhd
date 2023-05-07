library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity testvhdl is
port (
clk : in std_logic;
rst : in std_logic;
start : in std_logic;
done : out std_logic;
input : in std_logic_vector(7 downto 0);
output : out std_logic_vector(7 downto 0)
);
end entity testvhdl;

architecture Behavioral of testvhdl is
-- 声明变量
signal r, c : bit;
signal sum : bit_vector(7 downto 0);
signal pixels : bit_vector(7 downto 0);
signal b : bit_vector(7 downto 0);
signal shift_reg : bit_vector(7 downto 0);
signal shift_reg_cnt : bit_vector(1 downto 0);
begin

-- 过程描述均值滤波算法
process(clk)
begin
        input <= 10101111;
        r <= 0;
        c <= 0;
        sum <= 0;
        pixels <= 0;
        b  <=  0;
        shift_reg  <=  0;
        shift_reg_cnt  <=  0;
        done <= '0';
        pixels  <=  input;
        sum  <=  sum + pixels;
        b  <=  pixels;
        shift_reg<=pixels;
        shift_reg_cnt <= shift_reg_cnt + 1;
        if shift_reg_cnt = 3 then -- 满3个像素进行计算
            sum <=sum / 9;
            output <= sum; --将sum调整为8位，交给output
            sum <= sum - b;
            r <= r + 1;
            if r = 3 then
                r <= 0;
                c <= c + 1;
            end if;
            if c = 3 then
                done <= '1'; -- 计算完成
            end if;
            shift_reg_cnt <= 0;
        end if;
        done <= '0';

end process;

end architecture Behavioral;