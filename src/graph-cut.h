#include "disparity-algorithm.h"

class GraphCutDisparity : public DisparityAlgorithm {
private:
  typedef int node_index;
  typedef float edge_weight;

  edge_weight Cp;
  edge_weight V_smooth;


  typedef struct _Correspondence {
    int x;
    int y;
    int d; // disparity
  } Correspondence;

  StereoPair *pair;

  cv::Mat disparities;
  // disparities.at<int>(y, x) = d
  // type CV_32SC1
  // disparities = cv::Mat(m, n, CV_32SC1)

  bool is_active(Correspondence c);

  bool is_valid(Correspondence c, int alpha);
  // within image boundary (use pair variable)
  // active or has disparity alpha

  edge_weight data_cost(Correspondence c); // Matt
  // Squared error

  edge_weight occ_cost(Correspondence c);

  edge_weight smooth_cost(Correspondence c);

  node_index get_index(Correspondence c); // Matt

  void add_node(Correspondence c); // Matt
  // add node to the graph

  void add_edge(Correspondence c1, Correspondence c2, edge_weight w); // Matt
  // add edge to the graph from c1 to c2
  // use get_index(...)


  std::vector<Correspondence> get_neighbors(Correspondence c);
  // x +- 1, y +- 1, where is_valid

  std::vector<Correspondence> get_conflicts(Correspondence c, int alpha);
  // return correspondences that
  //   - share start or end pixel
  //   - have disparity alpha
  //   - where is_valid
  //   - not the same as c

  void add_active_nodes(int alpha);

  void add_alpha_nodes(int alpha);

  void add_neighbor_edges(Correspondence c, int alpha);
  void add_all_neighbor_edges(int alpha);

  void add_conflict_edges(Correspondence c, int alpha);
  void add_all_conflict_edges(int alpha);


  void initialize_graph(); // Matt
  void update_correspondences(); // Matt
  bool run_alpha_expansion(int alpha); // Matt
  // true if values changed

  bool run_iteration(); // Matt
  // true if values changed

public:
  GraphCutDisparity& compute(StereoPair &pair);
};
