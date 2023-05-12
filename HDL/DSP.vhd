library ieee;
use ieee.std_logic_1164.all;

entity dsp is
    generic (
        WIDTH : integer := 8; -- 输入信号宽度
        COEF_WIDTH : integer := 4 -- 系数宽度
    );
    port (
        clk : in std_logic; -- 时钟信号
        rst : in std_logic; -- 复位信号
        x : in std_logic_vector(WIDTH-1 downto 0); -- 输入信号
        y : out std_logic_vector(WIDTH-1 downto 0) -- 输出信号
    );
end entity dsp;

architecture behavior of dsp is
    signal coef : std_logic_vector(COEF_WIDTH-1 downto 0); -- 系数信号
    signal acc : std_logic_vector(WIDTH-1 downto 0); -- 累加器信号
    signal shift : integer range 0 to WIDTH-1; -- 右移位数信号
begin
    process (clk)
    begin
        if rising_edge(clk) then
            if rst = '1' then -- 复位
                acc <= (others => '0'); -- 累加器清零
                shift <= 0; -- 右移位数清零
            else -- 非复位状态
                acc <= std_logic_vector(unsigned(acc) + unsigned(x) * unsigned(coef)); -- 累加器更新，使用unsigned类型进行计算
                y <= acc(WIDTH-1 downto 0); -- 输出信号
                shift <= shift + 1; -- 右移位数更新
            end if;
        end if;
    end process;
end behavior;
