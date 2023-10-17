# KCCA

This repository contains C++ codes for the paper:

> A Counting-based Approach for Efficient 𝑘-Clique Densest Subgraph Discovery

## Introduction

In this paper, we study the problem of k-clique densest subgraph (CDS). k-clique densest subgraph (CDS) problem aims to detect a subgraph from a graph, such that the ratio of the number of k-cliques over its vertices is maximized. This problem has received plenty of attention in the literature, and is widely used in identifying larger ``near-cliques''. We develop an efficient approximation algorithm, by employing the state-of-the-art k-clique counting algorithm and proposing some optimization techniques. We have performed extensive experimental evaluation on 14 real-world large graphs and the results demonstrate the high efficiency of our algorithms.

## Environment

The codes of KCCA are implemented and tested under the following development environment:

- Hardware : Intel(R) Xeon(R) Gold 6338R 2.0GHz CPU and 512GB of memory
- Operation System : Ubuntu 20.04.4 LTS (GNU/Linux 5.13.0-40-generic x86_64)
- C++ Version: 14

## Datasets
We use 14 real-world datasets (bio-SC-GT,
econ-beacxc,
WikiTalk,
Slashdot, 
loc-gowalla,
DBLP,
web-Stanford, 
web-Google,
as-skitter,
Wikipedia-link,
zhishi-baidu,
ew-2013,
Orkut Social,
Friendster Social) from different domains, which are downloaded from the [Stanford Network Analysis Platform](http://snap.stanford.edu/data/), [Laboratory of Web Algorithmics](http://law.di.unimi.it/datasets.php), [Network Repository](https://networkrepository.com/network-data.php), and [Networks](http://konect.cc/networks/). Their detailed descriptions can also be found on these websites.

## How to Run the Codes


The implementations of both `KCCA-basic` and `KCCA` algorithms are included in the `kcca.cpp`, you can use arguments to switch these two algorithms. 

You can compile `kcca.cpp` in `KCCA/`:

`g++ kcca.cpp graph.cpp sct.cpp pava.cpp -O3 -o kcca`

and run this file by :

`./kcca <graph-path> <k> <max_iter> <output_path> <KCCA-basic/KCCA> <update roder> <approximation rate>`

The application of these arguments is as follows.

`graph-path` is the path to the file that contains the graph data. The graph file should be in txt format and organized as follows:

The first line of this file should contain two integers $n$ (the number of vertices) and $m$ (the number of edges) separated by a space. The index of vertices should be ranged from $0$ to $n - 1$.  Each of the following $m$ lines contains one edge by two integers separated by a space. There should be no multiple edges and self-loops in this file.

For example, here is a correct graph file with 4 vertices and 5 edges 

```
4 5
0 1
1 2
2 3
3 0
1 3
```

`k` is the k selected for k-cliques.

`max_iter` is a number of iterations the program takes. If you want the program to stop by a given approximation rate, please set this as 0.

`output_path` is the path to the file where the program will save its result.

`KCCA-basic/KCCA` is the option to switch algorithms, 0 stands for KCCA-basic, and 1 stands for KCCA.

`update order` is the option to switch the update order. Here are three options which are the same as the order given in Figure 6 in our paper. 

* 1 stands for degeneracy order

* 2 stands for depth order

* 3 stands for random order

For best performance, simply set this as 1. Note that this option does not work for the KCCA-basic algorithm

`approximation rate` is the threshold for approximation rate. The program will terminate itself once it reaches the given approximation rate. This should be set as a float number greater than 1. To use this argument, please set `max iter` as zero.

For example, if we want to run the KCCA-basic algorithm to approximate 5-cliques densest subgraph from the graph in `test.txt` taking 10 iterations and output the result to `result.txt`. The command should be:

`./kcca test.txt 5 10 result.txt 0 0 0`

If we want to run the KCCA algorithm to approximate 7-cliques densest subgraph from the graph in `test.txt` by an approximation rate of 1.01 using depth order and output the result to `result.txt`. The command should be:

`./kcca test.txt 7 0 result.txt 1 2 1.01`


