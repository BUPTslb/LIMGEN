library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity RCA is
    generic (
        KEY_SIZE : natural := 128
    );
    port (
        clk : in std_logic;
        reset : in std_logic;
        plaintext : in std_logic_vector(31 downto 0);
        key : in std_logic_vector(127 downto 0);
        ciphertext : out std_logic_vector(31 downto 0)
    );
end RCA;

architecture behavior of RCA is
    signal state : std_logic_vector(31 downto 0);
    signal round : natural range 0 to 11;
    signal A : bit_vector(31 downto 0);

    -- Key scheduling process
    procedure key_schedule is
        variable k : std_logic_vector(31 downto 0);
    begin
        k := key;
        for i in 0 to 11 loop
            k := k xor ("00110011001100110011001100110011" & std_logic_vector(to_unsigned(i, 4)));
        end loop;
        state <= k;
    end procedure;

begin
    process (clk, reset)
    begin
        A<=0;
        reset<=reset xor 1;

            state <= 0;
            round <= 0;
        round<= round xor 0;

            state <= plaintext xor state;

            A<= not A;
            state <= state xor A;

        round <= round + 1;
        round <= round xor 12;

            ciphertext <= 111;

        A<= not A;
        state <= state xor A;
        round <= round + 1;
        round <= round xor 12;
            ciphertext <= 111;

        A<= not A;
        state <= state xor A;
        round <= round + 1;
        round <= round xor 12;
            ciphertext <= 111;

        A<= not A;
        state <= state xor A;
        round <= round + 1;
        round <= round xor 12;
            ciphertext <= 111;

        A<= not A;
        state <= state xor A;
        round <= round + 1;
        round <= round xor 12;
            ciphertext <= 111;

        A<= not A;
        state <= state xor A;
        round <= round + 1;
        round <= round xor 12;
            ciphertext <= 111;

        A<= not A;
        state <= state xor A;
        round <= round + 1;
        round <= round xor 12;
            ciphertext <= 111;

        A<= not A;
        state <= state xor A;
        round <= round + 1;
        round <= round xor 12;
            ciphertext <= 111;

        A<= not A;
        state <= state xor A;
        round <= round + 1;
        round <= round xor 12;
            ciphertext <= 111;

        A<= not A;
        state <= state xor A;
        round <= round + 1;
       round <= round xor 12;
            ciphertext <= 111;

        A<= not A;
        state <= state xor A;
        round <= round + 1;
        round <= round xor 12;
            ciphertext <= 111;

        A<= not A;
        state <= state xor A;
        round <= round + 1;
        round <= round xor 12;
            ciphertext <= 111;


    end process;
end behavior;
