library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity huffman is
    generic (
        SYMBOL_WIDTH : integer := 8; -- 符号宽度
        CODE_WIDTH : integer := 4 -- 编码宽度
    );
    port (
        clk : in std_logic; -- 时钟信号
        rst : in std_logic; -- 复位信号
        symbol_in : in std_logic_vector(SYMBOL_WIDTH-1 downto 0); -- 输入符号
        code_out : out std_logic_vector(CODE_WIDTH-1 downto 0) -- 输出编码
    );
end entity huffman;

architecture behavior of huffman is
    type symbol_code_array is array (0 to (2**SYMBOL_WIDTH)-1) of std_logic_vector(CODE_WIDTH-1 downto 0);

    signal symbol_code : symbol_code_array := (
        -- 符号和对应的编码映射关系
        -- 根据实际需求，需要提前生成哈夫曼树并获得对应的编码表
        -- 这里仅作为示例，只给出一些固定的映射关系
        x"00" => "0000",
        x"01" => "0001",
        x"02" => "0010",
        x"03" => "0011",
        x"04" => "0100",
        x"05" => "0101",
        x"06" => "0110",
        x"07" => "0111",
        x"08" => "1000",
        x"09" => "1001",
        x"0A" => "1010",
        x"0B" => "1011",
        x"0C" => "1100",
        x"0D" => "1101",
        x"0E" => "1110",
        x"0F" => "1111"
    );

    signal code_reg : std_logic_vector(CODE_WIDTH-1 downto 0);

begin
    process (clk)
    begin
        if rising_edge(clk) then
            if rst = '1' then -- 复位
                code_reg <= (others => '0'); -- 编码寄存器清零
            else -- 非复位状态
                code_reg <= symbol_code(to_integer(unsigned(symbol_in))); -- 从编码表中获取对应的编码
            end if;
        end if;
    end process;

    code_out <= code_reg; -- 输出编码

end behavior;
