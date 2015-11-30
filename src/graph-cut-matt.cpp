#include "graph-cut.h"
#include "opencv2/core/core.hpp"

using namespace cv;

GraphCutDisparity& GraphCutDisparity::compute(StereoPair &_pair)
{
  pair = &_pair;

  return *this;
}

GraphCutDisparity::GraphCutDisparity() {
  return;
}

bool GraphCutDisparity::is_active(Correspondence c) {
  return (pair->disparity_left.at<int>(c.y, c.x) == c.d);
}

// within image boundary
// active or has disparity alpha
bool GraphCutDisparity::is_valid(Correspondence c, int alpha) {
  return (
    c.x >= 0 and
    c.y >= 0 and
    c.x < pair->cols and
    c.y < pair->rows and
    (
      is_active(c) or (c.d == alpha)
    )
  );
}


inline int square(int x) {return x * x;}

// squared error
GraphCutDisparity::edge_weight GraphCutDisparity::data_cost(Correspondence c)
{

  Vec3b col1 = pair->left.at<Vec3b>(c.y, c.x);
  Vec3b col2 = pair->right.at<Vec3b>(c.y, c.x + c.d);

  int cost_sum = 0;
  for (int channel = 0; channel < 3; ++channel) {
    cost_sum += square(col1[0] - col2[0]);
  }

  return cost_sum;
}

long GraphCutDisparity::correspondence_hash(Correspondence c)
{
  long hash_key = c.x;

  hash_key *= pair->rows;
  hash_key += c.y;

  hash_key *= (max_disparity - min_disparity + 1);
  hash_key += (c.d - min_disparity);
  
  return hash_key;
}

GraphCutDisparity::node_index GraphCutDisparity::get_index(Correspondence c)
{
  long hash_key = correspondence_hash(c);
  
  // TODO
  return 0;
}

void GraphCutDisparity::add_node(Correspondence c)
{
  return;
  // TODO
}
// add node to the graph

void GraphCutDisparity::add_edge(Correspondence c1, Correspondence c2, edge_weight w)
{
  return;
  // TODO
}
// add edge to the graph from c1 to c2
// use get_index(...)

void GraphCutDisparity::add_source_edge(Correspondence c, edge_weight w)
{
  return;
  // TODO
}

void GraphCutDisparity::add_sink_edge(Correspondence c, edge_weight w)
{
  return;
  // TODO
}

void GraphCutDisparity::add_active_nodes(int alpha)
{
  return;
  // TODO
}


void add_alpha_nodes(int alpha)
{
  return;
  // TODO
}

void GraphCutDisparity::add_all_neighbor_edges(int alpha)
{
  return;
  // TODO
}

void add_all_conflict_edges(int alpha)
{
  return;
  // TODO
}


void GraphCutDisparity::initialize_graph()
{
  return;
  // TODO
}

void update_correspondences()
{
  return;
  // TODO
}

bool run_alpha_expansion(int alpha)
{
  return false;
  // TODO
}
// true if values changed

bool run_iteration()
{
  return false;
  // TODO
}
// true if values changed