library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity FIR_filter is
    generic (
        N : natural := 7; -- 滤波器阶数
        WIDTH : natural := 16; -- 输入和输出数据位宽
        COEFFICIENTS : std_logic_vector((N-1)*WIDTH-1 downto 0) := (others => '0') -- 滤波器系数
    );
    port (
        clk : in std_logic;
        reset : in std_logic;
        x : in std_logic_vector(15 downto 0); -- 输入信号
        y : out std_logic_vector(15 downto 0) -- 输出信号
    );
end FIR_filter;

architecture behavior of FIR_filter is
    signal shift_reg : std_logic_vector(15 downto 0); -- 移位寄存器
    signal coefficients : std_logic_vector(15 downto 0); -- 系数寄存器

begin
    process (clk, reset)
    begin
        if reset = '1' then
            shift_reg <= (others => '0');
            coefficients <= COEFFICIENTS;
        elsif rising_edge(clk) then
            -- 移位寄存器移位
            shift_reg <= x & shift_reg(N-2 downto 0);

            -- 计算输出
            y <= std_logic_vector(
                signed(shift_reg) * signed(coefficients)
            );
        end if;
    end process;
end behavior;
