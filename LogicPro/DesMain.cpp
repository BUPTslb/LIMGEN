/*des.c*/
#include <math.h>
#include <stdlib.h>
#include <string.h>
//#include <bitset>
#include <iostream>
//#include "bit.cpp"
using namespace std;

/*定义一个密钥置换的映射*/
static const int DesTransform[56] =
        {
                57,49,41,33,25,17,9,1,58,50,42,34,26,18,
                10,2,59,51,43,35,27,19,11,3,60,52,44,36,
                63,55,47,39,31,23,15,7,62,54,46,38,30,22,
                14,6,61,53,45,37,29,21,13,5,28,20,12,4
        };
/*定义用于计算子密钥的旋转次数*/
static const int DesRotations[16] =
        {
                1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
        };
/*定义用于子密钥置换选择的映射*/
static const int DesPermuted[48] =
        {
                14,17,11,24,1,5,3,28,15,6,21,10,
                23,19,12,4,26,8,16,7,27,20,13,2,
                41,52,31,37,47,55,30,40,51,45,33,48,
                44,49,39,56,34,53,46,42,50,36,29,32
        };
/*定义用于数据块初始化转换的映射*/
static const int DesInitial[64] =
        {
                58,50,42,34,26,18,10,2,60,52,44,36,28,20,12,4,
                62,54,46,38,30,22,14,6,64,56,48,40,32,24,16,8,
                57,49,41,33,25,17,9,1,59,51,43,35,27,19,11,3,
                61,53,45,37,29,21,13,5,63,55,47,39,31,23,15,7
        };
/*定义用于数据块扩展转换的映射*/
static const int DesExpansion[48] =
        {
                32,1,2,3,4,5,4,5,6,7,8,9,
                8,9,10,11,12,13,12,13,14,15,16,17,
                16,17,18,19,20,21,22,23,24,25,
                24,25,26,27,28,29,28,29,30,31,32,1
        };
/*定义用于数据块中S盒转换的S盒表*/
static const int DesSbox[8][4][16] =
        {
                {
                        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
                        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
                        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
                        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13},
                },
                {
                        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
                        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
                        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
                        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9},
                },
                {
                        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
                        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
                        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
                        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12},
                },
                {
                        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
                        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
                        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
                        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14},
                },
                {
                        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
                        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
                        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
                        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3},
                },
                {
                        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
                        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
                        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
                        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13},
                },
                {
                        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
                        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
                        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
                        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12},
                },
                {
                        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
                        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
                        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
                        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11},
                }
        };
/*定义用于数据块转换的P盒映射表*/
static const int DesPbox[32] =
        {

        };
/*定义用于数据块最终转换的映射*/
static const int DesFinal[64] =
        {

        };

/*比特运算相关函数声明*/
/*
static bool bit_get(std::string bits, int i);//获取bit位
static bool bit_set(std::string *pString, int i, bool get);//设置相应位
static void bit_rot_left(std::string *lkey, int i, int j);//旋转
static void bit_xor(std::string *fblk, std::string *string, std::string *xblk, int i);//xor

static bool bit_set(std::string *pString, int i, bool get){


}

static bool bit_get(unsigned char bits, int i) {
    bool bx;
    if(((bits >> i) & 1) == 1)//右移i位&1
    {
        bx = 1;
        cout<<"b移位："<<(bits>>2)<<endl;
    }
    else
        bx = 0;
    return bx;
}

static void bit_rot_left(std::string *lkey, int i, int j) {

}

static void bit_xor(std::string *fblk, std::string *string, std::string *xblk, int i) {

}
*/
int bit_get(const unsigned char *bits, int pos)
{
    unsigned char  mask;
    int            i;

    /*设置掩码*/
    mask = 0x80;
    for(i=0; i<(pos % 8); i++)
        mask = mask >> 1;
    /*用位与运算获取对应的位*/
    return (((mask & bits[(int)(pos / 8)]) == mask)? 1:0);
}
/*bit_set  设置缓冲区bits中位于pos位的状态*/
void bit_set(unsigned char *bits, int pos, int state)
{
    unsigned char mask;
    int           i;

    /*设置掩码*/
    mask = 0x80;
    for(i=0; i<(pos % 8); i++)
        mask=mask>>1;

    /*依据state设置位*/
    if(state)
        bits[pos/8] = bits[pos/8] | mask;
    else
        bits[pos/8] = bits[pos/8] & (~mask);

    return;
}
/*bit_xor  按位异或运算*/
void bit_Xor(const unsigned char *bits1,const unsigned char *bits2,unsigned char *bitsx,int size)
{
    int i;
    /*计算两个缓冲区的按位异或*/
    for(i=0;i<size;i++)
    {
        if(bit_get(bits1,i) != bit_get(bits2,i))
            bit_set(bitsx,i,1);
        else
            bit_set(bitsx,i,0);
    }
    return;
}
/*bit_rot_left 轮转缓冲区bits(含size位)，将位值向左移count位*/
void bit_rot_left(unsigned char *bits,int size,int count)
{
    int  fbit,lbit,i,j;

    /*将缓冲区向左轮转指定位数*/
    if(size > 0)
    {
        for(j=0; j<count; j++)
        {
            for(i=0; i<=((size-1)/8); i++)
            {
                /*获得要从当前字节偏移的位*/
                lbit = bit_get(&bits[i],0);
                if(i==0)
                {
                    /*保存要从首字节移动到后面的位*/
                    fbit = lbit;
                }
                else
                {
                    /*将前一字节最右边的位设置为当前字节最左边的位*/
                    bit_set(&bits[i-1],7,lbit);
                }

                /*将当前字节向左移动*/
                bits[i] = bits[i] << 1;
            }
            /*将缓冲区最右边的位设置为从第一个字节偏移的位*/
            bit_set(bits,size-1,fbit);
        }
    }
    return;
}
/*permute函数  用于转换、改变位序列*/

//bits:密钥    mapping:转换表    n:输出的位数
static void permute(unsigned char *bits, const int *mapping, int n)
{
    unsigned char *temp;//一个temp一个字节，8个共64bit数据
    int i;

    /*使用n位映射映射缓冲区*/
    //ceil函数，返回大于或等于 x 的最小的整数值
    //memset(void *str, int c, size_t n)，将n个c替换到str前面n位
    memset(temp, 0, (int)ceil(n/8));

    for(i=0; i<n; i++)
        //bit_set:把temp中i位置设置
        //bit_get：按照mapping获取bits中位数
        bit_set(temp, i, bit_get(bits,mapping[i]-1));

    memcpy(bits, temp, (int)ceil(n/8));
    return;
}


/*des_main函数  加密或解密数据的计算函数*/
static int des_main(const unsigned char *source, unsigned char *target, const unsigned char *key)
{
    static unsigned char subkeys[16][7];
    //uc:8bit
    unsigned char temp[8],//64bit数据
            lkey[4],
            rkey[4],
            lblk[6],//左边32bit
            rblk[6],//右边32bit
            fblk[6],//32bit结果
            xblk[6],
            sblk;
    int row,col,i,j,k,p;

    /*如果key等于NULL，重用上次调用时计算出来的子密钥，否则计算子密钥*/
    if(key != NULL)
    {
        /*创建一个key的副本*/
        memcpy(temp,key,8);

        /*将key转换并压缩至56位*/
        permute(temp,DesTransform,56);

        /*将key分为两个28位的组*/
        memset(lkey,0,4);
        memset(rkey,0,4);

        for(j=0; j<28; j++)
            bit_set(lkey, j, bit_get(temp,j));

        for(j=0; j<28; j++)
            bit_set(rkey, j, bit_get(temp,j+28));

        /*计算每一轮的子密钥*/
        for(i=0; i<16; i++)
        {
            /*根据定义好的位数对每一块进行旋转*/
            //bit_rot_left,旋转操作。DesRotations定义子密钥的旋转次数
            bit_rot_left(lkey,28,DesRotations[i]);
            bit_rot_left(rkey,28,DesRotations[i]);

            /*重新合并两个块*/
            for(j=0; j<28; j++)
                bit_set(subkeys[i],j,bit_get(lkey,j));

            for(j=0; j<28; j++)
                bit_set(subkeys[i],j+28,bit_get(rkey,j));

            /*对子密钥做转换选择并压缩至48位*/
            permute(subkeys[i],DesPermuted,48);
        }
    }
    /*创建source参数的副本*/
    memcpy(temp, source, 8);

    /*初始转换数据块*/
    permute(temp, DesInitial, 64);

    /*将源数据块分为大小为32位的左右两个数据块*/
    memcpy(lblk, &temp[0], 4);
    memcpy(rblk, &temp[4], 4);

    /*加密或解密源数据*/
    for(i=0; i<16; i++)
    {
        /*开始f缓冲冲的计算*/
        memcpy(fblk,rblk,4);

        /*置换、扩展右数据块的拷贝，使其达到48位*/
        permute(fblk, DesExpansion, 48);


        /*加密数据，子密钥组以递增的顺序应用*/
        bit_Xor(fblk, subkeys[i], xblk, 48);
        memcpy(fblk, xblk, 6);


        /*执行S盒替换*/
        p=0;
        for(j=0; j<8; j++)
        {
            /*计算出S盒表中的行号和列号*/
            row = (bit_get(fblk,(j*6)+0)*2) + (bit_get(fblk,(j*6)+5)*1);
            col = (bit_get(fblk,(j*6)+1)*8) + (bit_get(fblk,(j*6)+2)*4) +
                  (bit_get(fblk,(j*6)+3)*2) + (bit_get(fblk,(j*6)+4)*1);
            /*为当前的6位数据块做S盒替换*/
            sblk = (unsigned char)DesSbox[j][row][col];
            for (k=4; k<8; k++)
            {
                bit_set(fblk,p,bit_get(&sblk,k));
                p++;
            }
        }

        /*为f缓冲区执行P盒替换*/
        permute(fblk, DesPbox, 32);

        /*计算左数据块与f缓冲区的异或值*/
        bit_Xor(lblk, fblk, xblk, 32);

        /*设置本轮的左数据块*/
        memcpy(lblk, rblk, 4);
        /*设置本轮的右数据块*/
        memcpy(rblk, xblk, 4);
    }

    /*将目标正文设置为重新连接的左右两个数据块*/
    memcpy(&target[0], rblk, 4);
    memcpy(&target[4], lblk, 4);

    /*执行最终置换*/
    permute(target, DesFinal, 64);

    return 0;
}
/*des_encipher DES加密数据*/
void des_encipher(const unsigned char *plaintext, unsigned char *ciphertext, const unsigned char *key)
{
    des_main(plaintext, ciphertext, key);
    return;
}

int main()
{
    const unsigned char plaintext[]="shenlibo";
    const unsigned char key[]="12345678";
    unsigned char ciphertext;
des_encipher(plaintext,&ciphertext,key);
cout<<"palintext="<<plaintext
    <<endl<<ciphertext;
return 0;
}









