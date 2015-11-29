#include "graph-cut.h"

GraphCutDisparity::edge_weight GraphCutDisparity::data_cost(Correspondence c) { return 0;}
  // Squared error

void GraphCutDisparity::add_node(Correspondence c) { return;} // Matt
  // add node to the graph

void GraphCutDisparity::add_edge(Correspondence c1, Correspondence c2, edge_weight w)
{
  return;
} // Matt
  // add edge to the graph from c1 to c2
  // use get_index(...)

  void GraphCutDisparity::initialize_graph() {return;} // Matt
  void GraphCutDisparity::update_correspondences() {return;} // Matt
  bool GraphCutDisparity::run_alpha_expansion(int alpha) {return false;} // Matt
  // true if values changed

  bool GraphCutDisparity::run_iteration() {return false;} // Matt
  // true if values changed