//
// Created by Lenovo on 2023/6/13.
//
#include "logic_func.h"
double calculateDistance(int arr1_id,int arr2_id,vector<Array_place> place_list,int **data_transfer,int **Place_array);
double calculateTotalDistance(vector<Array_place> place_list,int **data_transfer,int **Place_array);
void initializeArrayPlacement(vector<Array_place> &place_list, int num_array,int **Place_array, int max_width, int max_height);
void simulateAnnealing(vector<Array_place> &place_list,int **Place_array,int **data_transfer, int num_array,
                       int max_width, int max_height, double initialTemperature, double finalTemperature,
                       double coolingRate, int maxIterations, double threshold);

#ifndef SYNTHESIS_DES_H
#define SYNTHESIS_DES_H

#endif //SYNTHESIS_DES_H
