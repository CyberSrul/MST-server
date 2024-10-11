#pragma once
#include <unordered_map>
#include <unordered_set>
using namespace std;



class Graph
{
public:

unsigned int vx_count();
unsigned int edge_count();

void addNode(int node);
void removeNode(int node);
void connect(int src, int dst, float weight);
void disconnect(int src, int dst);
bool connected(int src, int dst);

unordered_set<int>::const_iterator begin() const;
unordered_set<int>::const_iterator end()   const;

const unordered_map<int, float>& Neighbors_of(int node) const;

private:

unsigned int edge_num = 0;
unordered_set<int> vertecies;
unordered_map<int, unordered_map<int, float>> edges;
};


Graph RandomGraph(int n, float p, int s);