<h1 align="left">🌳 🌐 MST Server</h1>

*This repository hosts a multithreaded server that allows you to construct weighted graphs and find their Minimum Spanning Trees (MST) along with various statistics about them.*

---

## ✨ Features

This server supports the following operations:

- **NewGraph**: `number_of_vertices`, `number_of_edges` (with `src,dst,weight` format for edges) <br>
- **RandomGraph**: `size`, `density` <br>
- **AddNode**: `id` <br>
- **RemoveNode**: `id` <br>
- **Connect**: `src,dst,weight` <br>
- **Disconnect**: `src,dst` <br>
- **MST Solver**: Two algorithms are available – **Prim** and **Kruskal**.

---

## 🔍 MST Solver

The `MST` command computes the Minimum Spanning Tree and provides the following statistics:

- **Total Weight**: Sum of all the edge weights in the MST.
- **Min Weighted Edge**: The smallest weight of any edge in the tree.
- **Max Distance**: The longest path between any two nodes in the tree.
- **Avg Distance**: The average path length between all connected nodes.

### ⚙️ Algorithms:
- **Prim's Algorithm**
- **Kruskal's Algorithm**

---

## 🧑‍💻 Test Run

Follow these steps to build, run & test the server:

### compile
```bash
make
```
### run
```bash
cd Server/
./server
```
### test
```bash
cd test/
./dos
```

---


## 🌲 MST 101. What is a Minimum Spanning Tree ?

A **Minimum Spanning Tree (MST)** is a subset of edges in a weighted graph that connects all vertices with no cycles and the smallest possible total edge weight.

### Why is MST Important? 
MSTs are key in network design to connect points with minimal cost (e.g. designing circuits).
They also help solve problems like the **Traveling Salesman Problem (TSP)** by optimizing routes.

### Nice Example
<img src="https://upload.wikimedia.org/wikipedia/commons/d/d2/Minimum_spanning_tree.svg" alt="Graph Example" width="500" />

---


## 🖥️ A Bit About the Server

The server is **multithreaded** and operates according to two key design patterns:

### 1. Leader-Follower
- Utilizes a **thread pool** where one leader thread waits for incoming clients.
- Upon a new client connection, the leader assigns the task of accepting the new client to a worker thread in the pool.
- This approach ensures that clients experience **no delay** in service, as the leader can immediately attend to them.

### 2. Pipeline
- Each request is processed in three distinct sequential steps:
1. **Graph Editing**
2. **Finding the MST**
3. **Calculating Statistics**
  
- When a new request enters the pipeline, it locks the pipeline until the tree is found, ensuring synchronized access to the graph.
- While one client's request is at step 1, another can be processed at step 3 in parallel, optimizing performance and response time.
- This design allows the server to efficiently handle multiple client requests while maintaining a smooth and responsive user experience!