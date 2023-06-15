#include "logic_func.h"
#include "dse.h"
#include <random>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std;

//需求：阵列列表place_list(找到每个阵列的大小)，传输次数矩阵data_transfer[][](计算距离后×次数)
//布局矩阵Place_array[x_max][y_max]，获取路径上的阵列id
//两个阵列id
double calculateDistance(int arr1_id,int arr2_id,vector<Array_place> place_list,int **data_transfer,int **Place_array) {
    // 使用曼哈顿距离计算两个阵列之间的距离
    int x1=place_list[arr1_id].pos_x;
    int y1=place_list[arr1_id].pos_y;
    int x2=place_list[arr2_id].pos_x;
    int y2=place_list[arr2_id].pos_y;
    //默认x1<=x2
    if(x1>x2){
        int temp1=x1;
        x1=x2;
        x2=temp1;
        int temp2=y1;
        y1=y2;
        y2=temp2;
    }
    //注意，place_array中可能会有-1的情况，需要判断，此时不能将其作为索引
    double distance=0;
    for(int i=x1;i<x2;i++){
        if (Place_array[i][y1]!=-1)
            distance+=place_list[Place_array[i][y1]].array_height;
        else
            distance+=64;
        if (distance<0)
            cout<<"error!"<<endl;
    }
    if (y1<y2)
    {
        for (int i = y1; i < y2; ++i) {
            if (Place_array[x2][i]!=-1)
                distance+=place_list[Place_array[x2][i]].array_width;
            else
                distance+=64;
        }
    }
    else
    {
        for (int i = y2; i < y1; ++i) {
            if (Place_array[x2][i]!=-1)
                distance+=place_list[Place_array[x2][i]].array_width;
            else
                distance+=64;
        }
    }
    distance+=(place_list[arr2_id].array_height+place_list[arr1_id].array_height
            +place_list[arr1_id].array_width+place_list[arr2_id].array_width)/2;
    distance*=data_transfer[arr1_id][arr2_id];
//    //cout<<"distance:"<<distance<<endl;
    return distance;

}
//vector<Array_place> place_list,int **data_transfer,int **Place_array
double calculateTotalDistance(vector<Array_place> place_list,int **data_transfer,int **Place_array) {
    // 计算矩阵之间的总传输距离
    double totalDistance = 0.0;
    for (int i = 0; i < place_list.size(); ++i) {
        for (int j = i+1; j < place_list.size(); ++j) {
            totalDistance+=calculateDistance(i,j,place_list,data_transfer,Place_array);
        }
    }
    return totalDistance;
}

//int **data_transfer,int **Place_array
void initializeArrayPlacement(vector<Array_place> &place_list, int num_array,int **Place_array, int max_width, int max_height) {
    // 随机生成初始状态的阵列布局
    std::srand(std::time(0));  // 设置随机数种子
    for (int i = 0; i < num_array; ++i) {
        place_list[i].pos_x = std::rand() % max_width;
        place_list[i].pos_y = std::rand() % max_height;
        if (Place_array[place_list[i].pos_x][place_list[i].pos_y]>=0)
        {
            i--;
            continue;
        } else
        {
            Place_array[place_list[i].pos_x][place_list[i].pos_y]=i;
        }

    }
}

void simulateAnnealing(vector<Array_place> &place_list,int **Place_array,int **data_transfer, int num_array,
                       int max_width, int max_height, double initialTemperature, double finalTemperature,
                       double coolingRate, int maxIterations, double threshold) {

    //初始化布局
    initializeArrayPlacement(place_list, num_array,Place_array,  max_width, max_height);
    //得到初始的布局Place_array
    double currentTemperature = initialTemperature;
    //计算当前总距离
    double currentDistance = 0;
    currentDistance = calculateTotalDistance(place_list,data_transfer,Place_array);
    //定义最佳布局
    int **bestPlacement = new int*[max_width];
    for (int i = 0; i < max_width; ++i) {
        bestPlacement[i] = new int[max_height];
        for (int j = 0; j < max_height; ++j) {
            bestPlacement[i][j] = Place_array[i][j];
        }
    }
    //定义最佳距离
    double bestDistance = currentDistance;
    //定义迭代次数
    int iterations = 0;
    while (currentTemperature > finalTemperature && iterations < maxIterations && currentDistance > threshold) {
        // 随机选择两个阵列进行位置交换
        int randomIndex1 = std::rand() % num_array;
        int randomIndex2 = std::rand() % num_array;
        while(randomIndex1 == randomIndex2) {
            randomIndex2 = std::rand() % num_array;
        }


        // 保存当前位置
        int currentPosX = place_list[randomIndex1].pos_x;
        int currentPosY = place_list[randomIndex1].pos_y;

        // 更新位置
        place_list[randomIndex1].pos_x = place_list[randomIndex2].pos_x;
        place_list[randomIndex1].pos_y = place_list[randomIndex2].pos_y;
        place_list[randomIndex2].pos_x = currentPosX;
        place_list[randomIndex2].pos_y = currentPosY;
        //更新布局的矩阵
        Place_array[currentPosX][currentPosY]=randomIndex2;//原来1的位置现在是2
        Place_array[place_list[randomIndex1].pos_x][place_list[randomIndex1].pos_y]=randomIndex1;//原来2的位置现在是1
        // 计算更新后的距离
        double newDistance = calculateTotalDistance(place_list,data_transfer,Place_array);

        // 计算能量变化
        double energyDelta = newDistance - currentDistance;

        // 判断是否接受更新
        if (energyDelta < 0 || std::exp(-energyDelta / currentTemperature) > static_cast<double>(std::rand()) / RAND_MAX) {
            // 接受更新
            currentDistance = newDistance;

        } else {
            // 恢复原来的位置
            place_list[randomIndex2].pos_x = place_list[randomIndex1].pos_x;
            place_list[randomIndex2].pos_y = place_list[randomIndex1].pos_y;
            place_list[randomIndex1].pos_x = currentPosX;
            place_list[randomIndex1].pos_y = currentPosY;
            Place_array[currentPosX][currentPosY]=randomIndex1;//还原1的位置
            Place_array[place_list[randomIndex2].pos_x][place_list[randomIndex2].pos_y]=randomIndex2;//还原2的位置
        }

        // 更新最佳布局
        if (currentDistance < bestDistance) {
            bestDistance = currentDistance;
            for (int i = 0; i < max_height; ++i) {
                for (int j = 0; j < max_width; ++j) {
                    bestPlacement[i][j] = Place_array[i][j];
                }
            }
        }

        // 降低温度
        currentTemperature *= coolingRate;

        iterations++;
    }

    // 输出最佳布局和距离
    std::cout << "Best Placement: \n";

    for (int i = 0; i < max_height; ++i) {
        for (int j = 0; j < max_width; ++j) {
            cout<<bestPlacement[i][j]<<" ";
        }
        cout<<endl;
    }
    std::cout << "Best Distance: " << bestDistance << "\n";

    // 释放内存
    for (int i = 0; i < max_height; ++i) {
        delete [] bestPlacement[i];
    }
    delete[] bestPlacement;
}










//
// Created by shenlibo on 23-4-12.
//
