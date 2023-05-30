library ieee;
use ieee.std_logic_1164.all;

entity Bitweaving is
    generic (
        DATA_WIDTH : positive := 8;  -- 数据宽度
        COLUMN_COUNT : positive := 4  -- 列数
    );
    port (
        clk : in std_logic;                  -- 时钟
        reset : in std_logic;                -- 复位信号
        input_data : in std_logic_vector(DATA_WIDTH - 1 downto 0);  -- 输入数据
        output_data : out std_logic_vector(DATA_WIDTH - 1 downto 0)  -- 输出数据
    );
end Bitweaving;

architecture Behavioral of Bitweaving is
    type ColumnType is array (0 to COLUMN_COUNT - 1) of std_logic_vector(DATA_WIDTH - 1 downto 0);
    signal columns : ColumnType;  -- 列信号

begin
    process (clk, reset)
    begin
        if reset = '1' then
            -- 复位时将所有列清零
            for i in 0 to COLUMN_COUNT - 1 loop
                columns(i) <= (others => '0');
            end loop;
        elsif rising_edge(clk) then
            -- 按位进行数据存储
            for i in 0 to COLUMN_COUNT - 1 loop
                columns(i) <= columns(i)(DATA_WIDTH - 2 downto 0) & input_data(0);
            end loop;
        end if;
    end process;

    -- 输出数据为各列的最低位连接而成
    output_data <= columns(COLUMN_COUNT - 1)(0);
    for i in COLUMN_COUNT - 2 downto 0 loop
        output_data <= columns(i)(0) & output_data;
    end loop;
end Behavioral;
