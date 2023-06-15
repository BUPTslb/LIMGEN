# LIMGEN：HLS of RRAM LIM
2022/9/7
this is the beginning of my research life!
## 主要内容 Main Content
在RRAM存内逻辑领域，当前有三种实现方式：在阵列中计算，依靠外围电路计算，使用RRAM阵列构建FPGA，我们的框架能够利用三种逻辑架构的优势，对行为级算法执行高层次综合，生成异构架构。
结合存算一体的性质，架构综合和数据映射将会同时完成。
框架中集成了模拟器，能够对异构架构/单一架构性能进行评估。

In the area of RRAM logic-in-memory, there are currently three implementations: computation in arrays, computation relying on peripheral circuits, and building FPGAs using RRAM arrays. Our framework is able to take advantage of all three logic architectures to perform high-level synthesis of behavior-level algorithms and generate heterogeneous architectures.
Combined with the nature of storage and computation as one, architecture synthesis and data mapping will be done simultaneously.
A simulator is integrated in the framework to evaluate the performance of heterogeneous architectures/single architectures.


## 流程 Process
1. 行为级算法描述 VHDL Behavior-level description
2. CDFG中间表示 Control Data Flow Diagram as the intermediate representation
3. 静态分析控制步 Control Step Analysis
4. 算子调度 Operator Scheduling
5. 数据映射 Data Mapping
6. 阵列生成 Array Generation
7. 布局和布线 Placement and Routing (模拟退火 Simulated Annealing)
8. 1-7设计空间探索 Design Space Exploration
9. 评估和对比实验 Evaluation and Comparison Experiments

## 引用 reference
本工作将发表在ICCAD'2023上，引用请参考一下格式：

This work will be published in ICCAD'2023, please refer to the following format for citation: