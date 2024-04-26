#pragma once

#include <algorithm>
#include <cassert>
#include <map>
#include <memory>
#include <omp.h>
#include <set>

#include "mesh.h"
#include "vector.h"

class QuadricErrorMetrics {
  QuadricErrorMetrics();
  QuadricErrorMetrics(const QuadricErrorMetrics &) = delete;

  static QuadricErrorMetrics *getInstance() {
    static QuadricErrorMetrics qem;
    return &qem;
  }

  double calculateError(const Vertex *) const;
  void sumQuadrics(double a[4][4], const double b[4][4]) const;
  double calculateEdgeCost(const Edge *) const;

  bool collapseEdge(Edge *);

  void calculateQuadrics(Mesh *) const;
  void calculateEdgeCosts(Mesh *) const;
  void simplifyImplementation(Mesh *, float, int, int);

public:
  static void simplify(Mesh *mesh, float goal = 0.5, int noOfBlocks = 32,
                       int noOfThreads = 32) {
    std::cout << std::endl;
    getInstance()->calculateQuadrics(mesh);
    std::cout << std::endl;
    getInstance()->calculateEdgeCosts(mesh);
    std::cout << std::endl;
    getInstance()->simplifyImplementation(mesh, goal, noOfBlocks, noOfThreads);
  }
};
