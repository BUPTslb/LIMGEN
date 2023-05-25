-- VHDL实体声明
entity ImageBinarization is
  generic (
    WIDTH  : positive := 8;    -- 图像宽度
    HEIGHT : positive := 8     -- 图像高度
  );
  port (
    inputImage  : in  std_logic_vector(63 downto 0);   -- 输入图像
    outputImage : out std_logic_vector(63 downto 0)    -- 输出图像
  );
end entity ImageBinarization;

-- VHDL体定义
architecture Behavioral of ImageBinarization is
begin
  process(inputImage)
    variable pixelValue : std_logic;
  begin
    inputImage<=15223;
    outputImage<=0;

    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;
    pixelValue <=not inputImage;
          -- 图像二值化逻辑
    pixelValue <= pixelValue xor 1;
    outputImage <= not outputImage;







  end process;
end architecture Behavioral;
