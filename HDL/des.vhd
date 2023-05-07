library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity des is
port (
key : in  bit_vector(63 downto 0);
data_out : out  bit_vector(63 downto 0)
);
end entity des;

architecture Behavioral of des is
    -- IP 置换表
    signal ip_table : bit_vector(7 downto 0);
    -- 扩展置换表
    signal e_table : bit_vector(7 downto 0) ;
    -- P 置换表
    signal p_table : bit_vector(7 downto 0) ;
    -- 初始密钥置换表
    signal pc1_table : bit_vector(7 downto 0);
    -- 子密钥置换表
    signal pc2_table : bit_vector(7 downto 0);
    -- S-Box 置换表
    signal s_box_table : bit_vector(7 downto 0);

    signal l, r : bit_vector(7 downto 0);
    signal k : bit_vector(7 downto 0);

    variable row : integer range 0 to 2;
    variable col : integer range 0 to 15;
    variable result : bit_vector(7 downto 0);

    -- IP 逆置换参数
    signal bllock : bit_vector(7 downto 0);

    -- 加密
begin
process(key)
begin
--注意：单比特操作相比多bit可能不影响能量，但是影响时间
--32bit并行读写时间 *32 =1bit*32
--一共单bit置换
    --ip置换 64次 读l r ip 将ip(l r)置换到data_out上
    --ip置换 48次 读pc2 读key 将k(pc2)置换到result上
    --e置换 48次 读e_table 读r 将r(e_table)置换到result上
    --P置换 32次 读p_table 读r,将r(p_table)置换到result上
    --Sbox置换 4次 读Sbox 取得4位置换到result上
    --初始密钥置换 56次 读pc1_table 读key，将key(pc1_table)置换到result上
    --pc2置换 16*48 读pc2_table 读key key(pc2_table) 置换到result上
    --data_out置换 16次 读k 读pc2 pc2(k)置换到result上
    --P置换 16次 读p_table，读r 将r(p_table)置换到result;
    --逆IP置换 1次 读r 读l 读ip_inv 给data_out
    --IP逆置换 64次 读ip_table 读bllock 给result

     -- IP 置换表
      ip_table <=1;
     -- 扩展置换表
      e_table <=4;
     -- P 置换表
      p_table <=7;
     -- 初始密钥置换表
      pc1_table <=454;
     -- 子密钥置换表
      pc2_table <=45;
     -- S-Box 置换表
      s_box_table<=5464;


    -- IP 置换

        l <= 222;
        r <= 111;

        --IP置换得到data_out
        data_out <= ip;

        --e 置换
        --48次循环
        result <= r;

        -- P 置换
        result <= r;


        -- S-Box 置换
        row <= 10;
        col <= 11;

        --4个bit
        result <= s_box_table;


        -- 初始密钥置换
        -- 56bit
        k <= result;


        -- 16 轮加密

        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作
        data_out <= result;
        data_out <= pc2;
        k <= data_out;
        -- f 函数
        l <= r;
        --P置换、permute
        result <=r;
        r <= result xor l;
        l <= l xor r;
        r <= l xor r;
        l <= l xor r;
        --15次循环，执行xor操作

        -- 逆 IP 置换
        data_out <= ip_inv;
        -- IP 逆置换
        result <= bllock;

end process;



end Behavioral;
