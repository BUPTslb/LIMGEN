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
本工作发表在ICCAD'2023上，BIBTEX引用格式：

This work has be published in ICCAD'2023, bibtex citation format :


“
@INPROCEEDINGS{10323837,
  author={Shen, Libo and Long, Boyu and Liu, Rui and Zhang, Xiaoyu and Han, Yinhe and Chen, Xiaoming},
  booktitle={2023 IEEE/ACM International Conference on Computer Aided Design (ICCAD)}, 
  title={LIM-GEN: A Data-Guided Framework for Automated Generation of Heterogeneous Logic-in-Memory Architecture}, 
  year={2023},
  volume={},
  number={},
  pages={1-9},
  doi={10.1109/ICCAD57390.2023.10323837}}
”


## 后续工作：
为了证明布局布线的合理性，需要使用其他方式（公认常用的布局布线策略，比如禁忌搜索等）与现有的模拟退火进行比较
使用DOT图绘制出最后的布局布线结果，以便于观察，数据的大小也要考虑，可以用python的networkx库绘制，线的粗细来表示数据传输的大小
考虑传输数据的大小时，要在前端更加细粒度的解析代码，对数据位数进行标注
考虑传输数据的大小主要是因为这和数据传输线的位宽有关，而数据传输线的位宽是由数据的大小决定的，所以要在前端解析代码时，对数据的大小进行标注，这样在后端就可以根据数据的大小来确定数据传输线的位宽，从而确定数据传输线的粗细

## 安装 Installation
生成vhdl代码的控制数据流图，运行hdlAst/CdfgGen.py
编译器依赖：hdlConvertorAst和hdlConvertor

安装hdlConvertorAst：

```bash
sudo pip3 install hdlConvertorAst
```

安装hdlConvertor：

```bash
# we recommend to use gcc>=9, Visual Studio 2017 or equivalent
sudo apt install build-essential uuid-dev cmake default-jre python3 python3-dev python3-pip libantlr4-runtime-dev antlr4 ninja-build
```

安装这个库

```bash
# note this may be older version than you see in repo
sudo pip3 install hdlConvertor

# or download repository and run
sudo pip3 install --upgrade --force-reinstall --no-cache-dir git+https://github.com/Nic30/hdlConvertorAst.git
sudo pip3 install -r requirements.txt
sudo python3 setup.py install

# if you are using version from git rather uninstall
# old library first if required
# sudo pip3 uninstall hdlConvertor
```

安装程序还支持其他可能有用的选项

```bash
python setup.py install --prefix /tmp/python_install/ --build-type Debug -- -DANTLR_JAR_LOCATION=/antlr-4.7.1-complete.jar -- VERBOSE=1
```

您也可以只安装 C++ 库/生成 .deb 包（没有特定的，只是普通的基于 cmake 的库）

```bash
mkdir build && cd build
cmake .. && cmake . --build
cpack # to generate .deb package
```

**用法**

```python
TEST_DIR = os.path.join("..", "HDL")
filenames = [os.path.join(TEST_DIR, "test.vhd"), ]
```

将相应的VHDL代码放入HDL文件夹中，并修改test.vhd为对应名称。

运行

```python
python3 CdfgGen.py
```

