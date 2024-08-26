# LIMGEN：HLS of RRAM LIM
## Main Content

In the area of RRAM logic-in-memory, there are currently three implementations: computation in arrays, computation relying on peripheral circuits, and building FPGAs using RRAM arrays. Our framework is able to take advantage of all three logic architectures to perform high-level synthesis of behavior-level algorithms and generate heterogeneous architectures.
Combined with the nature of storage and computation as one, architecture synthesis and data mapping will be done simultaneously.
A simulator is integrated in the framework to evaluate the performance of heterogeneous architectures/single architectures.


## Process
1. VHDL Behavior-level description
2. Control Data Flow Diagram as the intermediate representation
3. Control Step Analysis
4. Operator Scheduling
5. Data Mapping
6. Array Generation
7. Design Space Exploration
8. Evaluation and Comparison Experiments

## Reference
This work is published in ICCAD'2023, bibtex citation format :


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


## Future work：
- compilation strategy (read, write, excute)
- graph of placement
- bandwidth and bitwidth of data

## Installation
VHDL CDFG:run hdlAst/CdfgGen.py
Compiler：hdlConvertorAst和hdlConvertor

install hdlConvertorAst：

```bash
sudo pip3 install hdlConvertorAst
```

install hdlConvertor：

```bash
# we recommend to use gcc>=9, Visual Studio 2017 or equivalent
sudo apt install build-essential uuid-dev cmake default-jre python3 python3-dev python3-pip libantlr4-runtime-dev antlr4 ninja-build
```

install this lib:

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

other options:

```bash
python setup.py install --prefix /tmp/python_install/ --build-type Debug -- -DANTLR_JAR_LOCATION=/antlr-4.7.1-complete.jar -- VERBOSE=1
```

only c++:

```bash
mkdir build && cd build
cmake .. && cmake . --build
cpack # to generate .deb package
```

**how to use**

```python
TEST_DIR = os.path.join("..", "HDL")
filenames = [os.path.join(TEST_DIR, "test.vhd"), ]
```

put .vhd im /HDL, rename as test.vhd

run:

```python
python3 CdfgGen.py
```

