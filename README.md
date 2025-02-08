# Approximate Biclique Counting

This repository contains C++ codes and datasets for the paper:

> Scalable Approximate Biclique Counting over Large Bipartite Graphs

Note that we have compared our method with the BCList++, [EPivoter](https://github.com/LightWant/biclique/), and [EP/Zz++](https://github.com/LightWant/biclique/) , where EPivoter and EP/Zz++ are open-source. The implementation of BCList++ is obtained from https://github.com/LightWant/biclique/.

### 🚀 Quick Start
#### 1. Environment requirements

```
gcc version >= 9.4.0
```
#### 2. How to compile
```
cd build && cmake .. && make
```

#### 3. File Description
```main.cpp``` Program entrance and argument parsing.

```graph.h``` Implementation of graph structure.

```coloring.h``` Implementation of coloring method.

```counting.h``` Implementation of dynamic programming for counting the number of $(p,q)$-brooms.

```sampling.h``` Implementation of sampling algorithms.

#### 4. Parameters

``` -f [STRING] ``` : file path

``` -p [INT] -q [INT] ``` : parameters of biclique.

``` -test_sets [INT] ``` : the number of runs for experiment.

``` -T [INT] ``` : the sampling size.

``` -color [INT] ``` : 1 for running CBS method and 0 for running BS method.

###  📊 Run
Run the main experiment (**Section 5.1**) on `toydata.txt` by the following command:
```
./build/app -f ./datasets/toydata.txt -p 3 -q 2 -test_sets 10 -T 100000 -color 1 > results/outToydata 2> results/logToydata
```

Run the Ablation Study experiment (**Section A.1**) on `toydata.txt` by the following command:
```
./build/app -f ./datasets/toydata.txt -p 3 -q 2 -test_sets 10 -T 100000 -color 0 > results/outToydata 2> results/logToydata
```

