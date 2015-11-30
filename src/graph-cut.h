#pragma once
#include "disparity-algorithm.h"

#include <map>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

#include <climits>

#include <functional>

class GraphCutDisparity : public DisparityAlgorithm {
private:
  typedef int node_index;
  typedef int edge_weight;

  int NULL_DISPARITY = 0;

  edge_weight Cp;
  edge_weight V_smooth;

  int min_disparity;
  int max_disparity;

  typedef struct _Correspondence {
    int x;
    int y;
    int d; // disparity
  } Correspondence;

#include "graph-cut-extra-matt.h"

#include "graph-cut-extra-lucas.h"

  StereoPair *pair;

  bool is_active(Correspondence c); // Matt

  bool is_valid(Correspondence c, int alpha); // Matt
  // within image boundary (use pair variable)
  // active or has disparity alpha

  edge_weight data_cost(Correspondence c); // Matt
  // Squared error

  edge_weight occ_cost(Correspondence c); // Lucas

  edge_weight smooth_cost(Correspondence c); // Lucas

  node_index get_index(Correspondence c); // Matt

  void add_node(Correspondence c); // Matt
  // add node to the graph

  void add_edge(Correspondence c1, Correspondence c2, edge_weight w_uv, edge_weight w_vu); // Matt
  // add edge to the graph from c1 to c2
  // use get_index(...)

  void add_source_edge(Correspondence c, edge_weight w); // Matt
  void add_sink_edge(Correspondence c, edge_weight w); // Matt


  std::vector<Correspondence> get_neighbors(Correspondence c, int alpha); // Lucas
  // x +- 1, y +- 1, where is_valid

  std::vector<Correspondence> get_conflicts(Correspondence c, int alpha); // Lucas
  // return correspondences that
  //   - share start or end pixel
  //   - have disparity alpha
  //   - where is_valid
  //   - not the same as c

  void add_active_node(Correspondence c, int alpha); // Lucas
  void add_active_nodes(int alpha); // Matt

  void add_alpha_node(Correspondence c, int alpha); // Lucas
  void add_alpha_nodes(int alpha); // Matt

  void add_neighbor_edges(Correspondence c, int alpha); // Lucas
  void add_all_neighbor_edges(int alpha); // Matt

  void add_conflict_edges(Correspondence c, int alpha); // Lucas
  void add_all_conflict_edges(int alpha); // Matt


  void initialize_graph(); // Matt
  bool update_correspondences(int alpha); // Matt
  // true if values changed
  bool run_alpha_expansion(int alpha); // Matt
  // true if values changed

  bool run_iteration(); // Matt
  // true if values changed

public:
  GraphCutDisparity& compute(StereoPair &pair);
  GraphCutDisparity();
};
