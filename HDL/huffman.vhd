library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity HuffmanEncoder is
    generic (
        SYMBOL_COUNT : positive := 8
    );
    port (
        clk : in std_logic;
        reset : in std_logic;
        input_data : in std_logic_vector(SYMBOL_COUNT - 1 downto 0);
        encoded_data : out std_logic_vector(SYMBOL_COUNT - 1 downto 0);
        valid : out std_logic
    );
end entity HuffmanEncoder;

architecture behavioral of HuffmanEncoder is

    type SymbolRecord is record
        symbol : std_logic_vector(SYMBOL_COUNT - 1 downto 0);
        frequency : natural;
        code : std_logic_vector(SYMBOL_COUNT - 1 downto 0);
        code_length : natural;
    end record;

    type SymbolTable is array (0 to SYMBOL_COUNT - 1) of SymbolRecord;

    signal symbol_table : SymbolTable;
    signal symbol_idx : natural range 0 to SYMBOL_COUNT - 1 := 0;

    procedure GenerateHuffmanCode(
        input_symbols : in SymbolTable;
        output_symbols : out SymbolTable
    ) is
        variable temp_symbols : SymbolTable := input_symbols;
        variable merged_symbols : SymbolTable;
        variable min_freq1, min_freq2 : natural;
        variable min_idx1, min_idx2 : natural;
        variable merged_idx : natural := SYMBOL_COUNT;

    begin
        merged_symbols := temp_symbols;

        for i in 0 to SYMBOL_COUNT - 2 loop
            min_freq1 := natural'high;
            min_freq2 := natural'high;
            min_idx1 := 0;
            min_idx2 := 0;

            for j in 0 to merged_idx - 1 loop
                if merged_symbols(j).frequency < min_freq1 then
                    min_freq2 := min_freq1;
                    min_idx2 := min_idx1;
                    min_freq1 := merged_symbols(j).frequency;
                    min_idx1 := j;
                elsif merged_symbols(j).frequency < min_freq2 then
                    min_freq2 := merged_symbols(j).frequency;
                    min_idx2 := j;
                end if;
            end loop;

            merged_symbols(merged_idx).frequency := min_freq1 + min_freq2;
            merged_symbols(merged_idx).symbol := (others => '0');
            merged_symbols(merged_idx).code := (others => '0');
            merged_symbols(merged_idx).code_length := merged_symbols(min_idx1).code_length + 1;
            merged_symbols(min_idx1).code(merged_symbols(min_idx1).code_length) := '0';
            merged_symbols(min_idx2).code(merged_symbols(min_idx2).code_length) := '1';

            merged_idx := merged_idx + 1;
        end loop;

        output_symbols := merged_symbols;
    end procedure GenerateHuffmanCode;

begin

    process (clk)
        variable temp_table : SymbolTable;
        variable encoded_table : SymbolTable;
    begin
        if rising_edge(clk) then
            if reset = '1' then
                -- 初始化符号表
                for i in 0 to SYMBOL_COUNT - 1 loop
                    temp_table(i).symbol := (others => '0');
                    temp_table(i).frequency := 0;
                    temp_table(i).code := (others => '0');
                    temp_table(i).code_length := 0;
                end loop;
                symbol_table <= temp_table;
                valid <= '0';
            -- 计算符号频率
            else if input_data(symbol_idx) = '1' then
            temp_table(symbol_idx).frequency := temp_table(symbol_idx).frequency + 1;
            end if;
                        if symbol_idx = SYMBOL_COUNT - 1 then
                            -- 生成哈夫曼编码
                            GenerateHuffmanCode(temp_table, encoded_table);

                            -- 将编码数据写入输出端口
                            for i in 0 to SYMBOL_COUNT - 1 loop
                                temp_table(i).code := encoded_table(i).code;
                                temp_table(i).code_length := encoded_table(i).code_length;
                            end loop;

                            encoded_data <= temp_table(symbol_idx).code;
                            valid <= '1';
                        end if;

                        symbol_idx <= symbol_idx + 1;
                    end if;
                end if;
            end process;
end architecture behavioral;
