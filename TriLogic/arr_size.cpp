#include"logic_func.h"
using namespace std;

//将尺寸设置为>=操作数的2的幂
unsigned int arr_size() {
    unsigned int arr_size;
    unsigned m = 1;
    while (m < bit_num_operand) {
        m <<= 1; //向左移动一位数字
    }
    arr_size = m;

    return arr_size;
}












