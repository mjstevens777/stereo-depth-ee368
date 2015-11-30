#include "graph-cut.h"
#include "opencv2/core/core.hpp"

#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/graph/read_dimacs.hpp>

using namespace cv;

GraphCutDisparity& GraphCutDisparity::compute(StereoPair &_pair)
{
  pair = &_pair;

  pair->disparity_left = cv::Mat(pair->rows, pair->cols, CV_32S);
  pair->disparity_right = cv::Mat(pair->rows, pair->cols, CV_32S);

  pair->disparity_left.setTo(NULL_DISPARITY);
  pair->disparity_right.setTo(NULL_DISPARITY);

  while (run_iteration()) {}

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
    (c.x + c.d) >= 0 and
    c.y >= 0 and
    c.x < pair->cols and
    (c.x + c.d) < pair->cols and
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

  Vec3f col1 = pair->left.at<Vec3f>(c.y, c.x);
  Vec3f col2 = pair->right.at<Vec3f>(c.y, c.x + c.d);

  return square(cv::norm(col1 - col2));
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
  return hash_to_graph_index[hash_key];
}

GraphCutDisparity::Vertex GraphCutDisparity::get_vertex(Correspondence c)
{
  node_index i = get_index(c);
  return boost::vertex(i, g);
}

void GraphCutDisparity::add_node(Correspondence c)
{
  long hash_key = correspondence_hash(c);
  Vertex node = boost::add_vertex(g);
  node_index idx = boost::get(vertex_indices, node);
  hash_to_graph_index[hash_key] = idx;
}
// add node to the graph

void GraphCutDisparity::add_edge(Correspondence c1, Correspondence c2, edge_weight w)
{
  Vertex u = get_vertex(c1);
  Vertex v = get_vertex(c2);

  Edge e, e_reverse;
  tie(e, std::ignore) = boost::add_edge(u, v, g);
  put(capacities, e, w);
  put(residual_capacities, e, 0);

  bool found;
  tie(e_reverse, found) = edge(v, u, g);
  if (found) {
    put(reverse_edges, e, e_reverse);
    put(reverse_edges, e_reverse, e);
  }
}
// add edge to the graph from c1 to c2

void GraphCutDisparity::add_source_edge(Correspondence c, edge_weight w)
{
  Vertex v = get_vertex(c);

  Edge e, e_reverse;
  tie(e, std::ignore) = boost::add_edge(source, v, g);
  tie(e_reverse, std::ignore) = boost::add_edge(v, source, g);
  put(capacities, e, w);
  put(capacities, e_reverse, 0);
  put(residual_capacities, e, 0);
  put(residual_capacities, e_reverse, 0);
  put(reverse_edges, e, e_reverse);
  put(reverse_edges, e_reverse, e);
  return;
}

void GraphCutDisparity::add_sink_edge(Correspondence c, edge_weight w)
{
  Vertex u = get_vertex(c);

  Edge e, e_reverse;
  tie(e, std::ignore) = boost::add_edge(u, sink, g);
  tie(e_reverse, std::ignore) = boost::add_edge(sink, u, g);
  put(capacities, e, w);
  put(capacities, e_reverse, 0);
  put(residual_capacities, e, 0);
  put(residual_capacities, e_reverse, 0);
  put(reverse_edges, e, e_reverse);
  put(reverse_edges, e_reverse, e);
  return;
}

void GraphCutDisparity::for_each_active(std::function<void(Correspondence)> fn, int alpha) {
  for (int y = 0; y < pair->rows; y++) {
    for (int x = 0; x < pair->cols; x++) {
      Correspondence c;
      c.d = pair->disparity_left.at<int>(y, x);
      if (c.d != NULL_DISPARITY and c.d != alpha) {
        c.x = x;
        c.y = y;
        fn(c);
      }
    }
  }
}

void GraphCutDisparity::for_each_alpha(std::function<void(Correspondence)> fn, int alpha) {
  for (int y = 0; y < pair->rows; y++) {
    for (int x = 0; x < pair->cols; x++) {
      Correspondence c;
      c.x = x;
      c.y = y;
      c.d = alpha;
      if (is_valid(c, alpha)) fn(c);
    }
  }
}

void GraphCutDisparity::add_active_nodes(int alpha)
{
  for_each_active(
    [this, alpha](Correspondence c) { add_active_node(c, alpha); }
    , alpha
  );
}


void GraphCutDisparity::add_alpha_nodes(int alpha)
{
  for_each_alpha(
    [this, alpha](Correspondence c) { add_alpha_node(c, alpha); }
    , alpha
  );
}

void GraphCutDisparity::add_all_neighbor_edges(int alpha)
{
  for_each_active(
    [this, alpha](Correspondence c) { add_neighbor_edges(c, alpha); }
    , alpha
  );
  for_each_alpha(
    [this, alpha](Correspondence c) { add_neighbor_edges(c, alpha); }
    , alpha
  );
}

void GraphCutDisparity::add_all_conflict_edges(int alpha)
{
  for_each_active(
    [this, alpha](Correspondence c) { add_conflict_edges(c, alpha); }
    , alpha
  );
}


void GraphCutDisparity::initialize_graph()
{
  // Clear and reset properties
  g.clear();
  capacities = get(boost::edge_capacity_t(), g);
  residual_capacities = get(boost::edge_residual_capacity_t(), g);
  reverse_edges = get(boost::edge_reverse_t(), g);
  vertex_indices = get(boost::vertex_index_t(), g);
  colors = get(boost::vertex_color_t(), g);

  hash_to_graph_index.clear();

  // Add source / sink
  source = boost::add_vertex(g);
  sink = boost::add_vertex(g);

  return;
}

bool GraphCutDisparity::update_correspondences(int alpha)
{
  Color black = boost::color_traits<Color>::black();

  bool changed;
  for_each_active(
    [this, black, &changed](Correspondence c) {

      Vertex node = get_vertex(c);
      Color col = boost::get(colors, node);
      if (col == black) // still active
        return;
      // change to inactive
      changed = true;
      pair->disparity_left.at<int>(c.y, c.x) = NULL_DISPARITY;
      pair->disparity_right.at<int>(c.y, c.x + c.d) = NULL_DISPARITY;
    }
    , alpha
  );

  for_each_active(
    [this, alpha, black, &changed](Correspondence c) {

      bool was_active = is_active(c);
      Vertex node = get_vertex(c);
      Color col = boost::get(colors, node);
      bool now_active = (col != black);

      if (now_active != was_active)
        changed = true;

      if (now_active) {
        pair->disparity_left.at<int>(c.y, c.x) = alpha;
        pair->disparity_right.at<int>(c.y, c.x + c.d) = alpha;
      } else {
        pair->disparity_left.at<int>(c.y, c.x) = NULL_DISPARITY;
        pair->disparity_right.at<int>(c.y, c.x + c.d) = NULL_DISPARITY;
      }

    }
    , alpha
  );
    
  return changed;
}
// true if values changed

bool GraphCutDisparity::run_alpha_expansion(int alpha)
{
  initialize_graph();

  add_active_nodes(alpha);
  add_alpha_nodes(alpha);
  add_all_conflict_edges(alpha);
  add_all_neighbor_edges(alpha);

  boykov_kolmogorov_max_flow(g, source, sink);

  return update_correspondences(alpha);
}
// true if values changed

bool GraphCutDisparity::run_iteration()
{
  bool improved = false;
  for (int alpha = min_disparity; alpha <= max_disparity; alpha++) {
    improved = improved || run_alpha_expansion(alpha);
  }
  return improved;
}
// true if values changed