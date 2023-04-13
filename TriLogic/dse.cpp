#include "logic_func.h"
#include <random>

// 定义可变参数的范围
const double min_param = 0.0;  // 可变参数的最小值
const double max_param = 1.0;  // 可变参数的最大值

// 定义设计限制的范围
const double min_area = 0.0;  // 面积的限制最小值
const double max_area = 100.0;  // 面积的限制最大值
const double min_time = 0.0;  // 时间的限制最小值
const double max_time = 100.0;  // 时间的限制最大值
const double min_energy = 0.0;  // 能耗的限制最小值
const double max_energy = 100.0;  // 能耗的限制最大值

// 定义遗传算法的参数
const int population_size = 100;  // 种群大小
const int num_generations = 100;  // 迭代的代数
const double crossover_rate = 0.8;  // 交叉率
const double mutation_rate = 0.1;  // 变异率

// 定义帕累托图的类，用于比较结果的优劣
class ParetoFrontier {
public:
    void add_solution(const vector<double>& solution) {
        solutions.push_back(solution);
    }

    vector<vector<double>> get_pareto_front() {
        vector<vector<double>> pareto_front;
        for (const auto& solution : solutions) {
            bool is_dominated = false;
            for (const auto& other : solutions) {
                if (dominates(other, solution)) {
                    is_dominated = true;
                    break;
                }
            }
            if (!is_dominated) {
                pareto_front.push_back(solution);
            }
        }
        return pareto_front;
    }

private:
    bool dominates(const vector<double>& a, const vector<double>& b) {
        // 检查a是否支配b
        bool is_dominated = true;
        for (int i = 0; i < a.size(); ++i) {
            if (a[i] > b[i]) {
                is_dominated = false;
                break;
            }
        }
        return is_dominated;
    }

    vector<vector<double>> solutions;
};

// 定义可变参数的编码和解码函数
vector<double> encode_params() {
    vector<double> encoded_params;
    for (int i = 0; i < 10; ++i) {
        encoded_params.push_back(min_param + (max_param - min_param) * static_cast<double>(rand()) / RAND_MAX);
    }
    return encoded_params;
}

void decode_params(const vector<double>& encoded_params, vector<double>& decoded_params) {
    decoded_params = encoded_params;
}

// 评估函数，计算目标函数值和检查设计限制
void evaluate(const vector<double>& params, double& area, double& time, double& energy) {
    // TODO: 根据实际问题，计算目标函数值和检查设计限制
    // 将计算结果存储在 area, time, energy 中
    // 示例中使用随机生成的值作为示范

    // 随机生成目标函数值和设计限制的值
    area = min_area + (max_area - min_area) * static_cast<double>(rand()) / RAND_MAX;
    time = min_time + (max_time - min_time) * static_cast<double>(rand()) / RAND_MAX;
    energy = min_energy + (max_energy - min_energy) * static_cast<double>(rand()) / RAND_MAX;
}
// 定义遗传算法的主函数
int main() {
    // 初始化随机种子
    srand(time(0));

    // 初始化帕累托图
    ParetoFrontier pareto_frontier;

    // 初始化种群
    vector<vector<double>> population;
    for (int i = 0; i < population_size; ++i) {
        vector<double> params = encode_params();
        population.push_back(params);
    }

    // 迭代优化过程
    for (int generation = 0; generation < num_generations; ++generation) {
        // 存储当前代的目标函数值和设计限制
        vector<double> areas;
        vector<double> times;
        vector<double> energies;

        // 评估种群中每个个体的目标函数值和设计限制
        for (const auto& params : population) {
            double area, time, energy;
            evaluate(params, area, time, energy);
            areas.push_back(area);
            times.push_back(time);
            energies.push_back(energy);
        }

        // 更新帕累托图
        for (int i = 0; i < population_size; ++i) {
            vector<double> solution = {areas[i], times[i], energies[i]};
            pareto_frontier.add_solution(solution);
        }

        // 获取当前帕累托图
        vector<vector<double>> pareto_front = pareto_frontier.get_pareto_front();

        // 输出当前代的帕累托图结果
        cout << "Generation " << generation << " Pareto Front: " << endl;
        for (const auto& solution : pareto_front) {
            cout << "Area: " << solution[0] << ", Time: " << solution[1] << ", Energy: " << solution[2] << endl;
        }

        // 生成下一代种群
        vector<vector<double>> next_generation;
        while (next_generation.size() < population_size) {
            // 选择父代个体
            int parent1_idx = rand() % population_size;
            int parent2_idx = rand() % population_size;

            // 交叉操作
            vector<double> child;
            for (int i = 0; i < 10; ++i) {
                if (static_cast<double>(rand()) / RAND_MAX < crossover_rate) {
                    child.push_back(population[parent1_idx][i]);
                } else {
                    child.push_back(population[parent2_idx][i]);
                }
            }

            //
            // 变异操作
            //TODO:设置max_params 和 min_params的范围
            vector<double> max_params;
            vector<double> min_params;
            for (int i = 0; i < 10; ++i) {
                if (static_cast<double>(rand()) / RAND_MAX < mutation_rate) {
                    child[i] = child[i] + (max_params[i] - min_params[i]) * (static_cast<double>(rand()) / RAND_MAX - 0.5);
                    // 确保变异后的参数在取值范围内
                    child[i] = max(min_params[i], min(max_params[i], child[i]));
                }
            }

            // 将子代个体加入下一代种群
            next_generation.push_back(child);
        }

        // 更新种群
        population = next_generation;
    }

    return 0;
}




//
// Created by shenlibo on 23-4-12.
//
