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

``` -color [INT] ``` : 0 for running BS method, 1 for running CBS method (**default setting**), 2 for running ACBS method, 3 for running DCBS method.

###  📊 Run
Run the main experiment (**Section 5.2**) on `toydata.txt` by the following command:
```
./build/app -f ./datasets/toydata.txt -p 3 -q 2 -test_sets 10 -T 100000 -color 1 > results/outToydata 2> results/logToydata
```

Run the Ablation Study experiment (**Section 6.1**) on `toydata.txt` by the following command:
```
./build/app -f ./datasets/toydata.txt -p 3 -q 2 -test_sets 10 -T 100000 -color 0 > results/outToydata 2> results/logToydata
```

### 🗃️ Dataset 

Please take a look at the real-world datasets used in our experiments using the following links.


| Dataset (Abbr.)         | Download link                                                |
| :---------------------- | ------------------------------------------------------------ |
| github (GH)             | [Link](http://konect.cc/networks/github/)         |
| StackOF (SO)            | [Link](http://konect.cc/networks/stackexchange-stackoverflow/)           |
| Twitter (Wut)           | [Link](http://konect.cc/networks/munmun_twitterex_ut/) |
| IMDB (IMDB)             | [Link](http://konect.cc/networks/komarix-imdb/)              |
| Actor2 (Actor2)         | [Link](http://konect.cc/networks/actor2/)             |
| Amazon (AR)             | [Link](http://konect.cc/networks/amazon-ratings/)       |
| DBLP (DBLP)             | [Link](http://konect.cc/networks/dblp-author/)            |
| Epinions (ER)           | [Link](http://konect.cc/networks/epinions-rating/)         |
| Wikipedia-edits-de (DE) | [Link](http://konect.cc/networks/edit-dewiki/)               |
