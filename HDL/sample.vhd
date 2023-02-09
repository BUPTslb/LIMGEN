entity sample is
    port(   in1,in2:in integer ;
            cond:in bit;
            fout:out integer);
end sample;

architecture Behavioral of sample is
    variable v1,v2,v3:integer;
    begin
    process
    begin
        v1:=in1;
        v2:=in2;
        while(v1<v2)loop
            v2:=v1;
           if(cond='1')then
                v3:=v1+v2;
                v2:=v1;
           else
                v3:=v1-v2;
           end if;
        end loop;
      fout<=v3;
    end process;
end Behavioral;