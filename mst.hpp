#pragma once
#include "graph.hpp"
#include <iostream>




Graph MST(const string& algo, const Graph& graph);

float avg_distance(const Graph& tree);
float max_distance(const Graph& tree);
float min_edge(const Graph& tree);
float total_weight(const Graph& tree);