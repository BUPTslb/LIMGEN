library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
--数字图像处理算法：均值滤波
entity mean_filter23 is
port (
clk : in std_logic;
rst : in std_logic;
start : in std_logic;
done : out std_logic;
input : in std_logic_vector(7 downto 0);
output : out std_logic_vector(7 downto 0)
);
end entity mean_filter23;

architecture Behavioral of mean_filter23 is
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
        done <= 0;

        sum<=r xor c;--代替判断
        pixels  <= not input;
        sum  <=  sum + pixels;
        b  <=  not pixels;
        shift_reg<=not pixels;
        shift_reg_cnt <= shift_reg_cnt + 1;
        shift_reg_cnt <= shift_reg_cnt xor 3;


        --将乘法拆解成加法
        sum <=not pixels;
        sum <=sum + pixels;
        output <= not sum;
        sum <= output +pixel;
        sum <= sum + b;
        output <= sum + pixel;
        --将乘法拆解成加法
        sum <=not pixels;
        sum <=sum + pixels;
        output <= not sum; --将sum调
        sum <= output +pixel;
        sum <= sum + b;
        output <= sum + pixel;
        --将乘法拆解成加法
        sum <=not pixels;
        sum <=sum + pixels;
        output <= not sum; --将sum调
        sum <= output +pixel;
        sum <= sum + b;
        output <= sum + pixel;
        --将乘法拆解成加法
        sum <=not pixels;
        sum <=sum + pixels;
        output <= not sum; --将sum调
        sum <= output +pixel;
        sum <= sum + b;
        output <= sum + pixel;
        r <= r + 1;
        r <= r xor 3;

        r <= 0;
        c <= c + 1;
        c <= c xor r;
        r <= r xor 3;
        c <= c xor r;
        r <= r xor 3;
        c <= c xor r;
        r <= r xor 3;
        done <= 12; -- 计算完成

        shift_reg_cnt <= 0;

        done <= 0;

end process;

end architecture Behavioral;