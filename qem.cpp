#include "qem.h"
#include "mesh.h"

QuadricErrorMetrics::QuadricErrorMetrics() {}

double QuadricErrorMetrics::calculateError(const Vertex *vertex) const {
  double cost = 0.0;
  double vQ[4] = {0};
  double v[4] = {vertex->getX(), vertex->getY(), vertex->getZ(), 1};

  // v'(row vector) dot Q (4x4 matrix)
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      vQ[i] += v[j] * vertex->Q[j][i];
    }
  }

  // vQ (row vector) dot v (column vector)
  for (int i = 0; i < 4; ++i) {
    cost += vQ[i] * v[i];
  }
  return cost;
}

/* Add quadric matrix b to quadric matrix a */
void QuadricErrorMetrics::sumQuadrics(double a[4][4],
                                      const double b[4][4]) const {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      a[i][j] += b[i][j];
    }
  }
}

double QuadricErrorMetrics::calculateEdgeCost(const Edge *edge) const {
  Vertex *placement = (Vertex *)edge->getPlacement();
  // Cost is given by v'Qv, where v is placement vertex
  memcpy(placement->Q, edge->getV1()->Q, sizeof(double) * 16);
  this->sumQuadrics(placement->Q, edge->getV2()->Q);

  return this->calculateError(placement);
}

bool QuadricErrorMetrics::collapseEdge(Edge *edgeToBeCollapsed) {
  bool collapsed = false;

  // if (edgeToBeCollapsed->isModified()) {
  //   return collapsed;
  // }

  Vertex *v1 = (Vertex *)edgeToBeCollapsed->getV1();
  Vertex *v2 = (Vertex *)edgeToBeCollapsed->getV2();
  assert(v1 && v2);

  // TODO: Check if the cost of edge has changed
  if (!v1->hasFaces() || !v2->hasFaces()) {
    return collapsed;
  }

  // ---------------------------------------------------------------------------
  /* Remove faces associated with the collapsed edge */
  auto ef = edgeToBeCollapsed->getFaces();
  std::vector<Face *> facesToBeRmoved(ef.begin(), ef.end());
  for (Face *f : facesToBeRmoved) {
    f->remove();
  }

  // ---------------------------------------------------------------------------
  /* Remove the collapsed edge */
  edgeToBeCollapsed->remove();

  // ---------------------------------------------------------------------------
  /* Set the v2 vertex to the value of edge->placement */
  v2->update(edgeToBeCollapsed->getPlacement());

  // ---------------------------------------------------------------------------
  /* Update all edges of the v1 vertex */
  std::map<Vertex *, Edge *> v2NeighbourMap;
  for (Edge *ie : v2->getIncomingEdges()) {
    assert(ie && ie != edgeToBeCollapsed);
    v2NeighbourMap[(Vertex *)ie->getV1()] = ie;
  }
  for (Edge *oe : v2->getOutgoingEdges()) {
    assert(oe && oe != edgeToBeCollapsed);
    v2NeighbourMap[(Vertex *)oe->getV2()] = oe;
  }

  // Update the edge->v2 vertex to v2 for all incoming edges of v1, and add the
  // edge to v2
  Edge *de = NULL;
  for (Edge *ie : v1->getIncomingEdges()) {
    assert(ie && ie != edgeToBeCollapsed);

    if (v2NeighbourMap.count((Vertex *)ie->getV1())) {
      de = ie;
    } else {
      ie->setV2(v2);
      v2->addIncomingEdge(ie);
    }
  }
  if (de) {
    de->remove();
  }

  // Update the edge->v1 vertex to v2 for all outgoing edges of v1, and add
  // the edge to v2
  de = NULL;
  for (Edge *oe : v1->getOutgoingEdges()) {
    assert(oe && oe != edgeToBeCollapsed);

    if (v2NeighbourMap.count((Vertex *)oe->getV2())) {
      de = oe;
    } else {
      oe->setV1(v2);
      v2->addOutgoingEdge(oe);
    }
  }
  if (de) {
    de->remove();
  }

  // ---------------------------------------------------------------------------
  /* Update all faces of the v1 vertex */

  for (Face *f : v1->getFaces()) {
    if (f->getEdges().size() == 2) {
      for (Vertex *v : f->getVertices()) {
        if (v2NeighbourMap.count(v)) {
          f->addEdge(v2NeighbourMap[v]);
        }
      }
    }
    f->replaceVertex(v1, v2);
    v2->addFace(f);
  }

  // ---------------------------------------------------------------------------
  /* Remove v1 vertex */
  v1->remove();
  collapsed = true;

  // ---------------------------------------------------------------------------
  // Finally, update the cost of all edges of v2 vertex
  double cost = 0.0;
  for (Edge *e : v2->getOutgoingEdges()) { // from
    e->modifiy();
    double cost = this->calculateEdgeCost(e);
    e->setCost(cost);
  }

  for (Edge *e : v2->getIncomingEdges()) { // from
    e->modifiy();
    double cost = this->calculateEdgeCost(e);
    e->setCost(cost);
  }

  return collapsed;
}

void QuadricErrorMetrics::calculateQuadrics(Mesh *mesh) const {
  std::cout << "Calculating quadrics... ";

  double Kp[4][4];
  double x, y, z, d;
  Vector v0v1(0, 0, 0), v0v2(0, 0, 0);

  for (Vertex *vertex : mesh->getVertices()) {
    for (Face *face : vertex->getFaces()) {
      // Calculate v0v1
      x = face->getVertex(1)->getX() - face->getVertex(0)->getX();
      y = face->getVertex(1)->getY() - face->getVertex(0)->getY();
      z = face->getVertex(1)->getZ() - face->getVertex(0)->getZ();
      v0v1.update(x, y, z);

      // Calculate v0v2
      x = face->getVertex(2)->getX() - face->getVertex(0)->getX();
      y = face->getVertex(2)->getY() - face->getVertex(0)->getY();
      z = face->getVertex(2)->getZ() - face->getVertex(0)->getZ();
      v0v2.update(x, y, z);

      std::unique_ptr<Vector> v(v0v1.cross(&v0v2));
      // Normalize so that x² + y² + z² = 1
      v->normalize();

      // Apply v0 to find parameter d of equation
      d = (v->getX() * face->getVertex(0)->getX()) +
          (v->getY() * face->getVertex(0)->getY()) +
          (v->getZ() * face->getVertex(0)->getZ());
      d *= -1;

      // Initialize plane
      double plane[4] = {v->getX(), v->getY(), v->getZ(), d};

      // For this plane, the fundamental quadric Kp is the product of vectors
      // plane and plane'
      for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
          Kp[i][j] += plane[i] * plane[j];
        }
      }

      sumQuadrics(vertex->Q, Kp);
    }
  }

  std::cout << "Done" << std::endl;
}

void QuadricErrorMetrics::calculateEdgeCosts(Mesh *mesh) const {
  std::cout << "Calculating edge costs... ";

  double cost = 0.0;
  for (Edge *edge : mesh->getEdges()) {
    double cost = this->calculateEdgeCost(edge);
    edge->setCost(cost);
  }

  std::cout << "Done" << std::endl;
}

void QuadricErrorMetrics::simplifyImplementation(Mesh *mesh, float goal,
                                                 int noOfBlocks = 32,
                                                 int noOfThreads = 32) {
  int noOfVertices = mesh->getNoOfVertices();
  auto vertices = mesh->getVertices();

  int progress = 0;
  int failures = 0;
  int target = goal * noOfVertices;
  int blockSize = noOfVertices / noOfThreads;
  std::cout << "Simplifying [target = " << noOfVertices - target
            << " vertex(s)]... ";

  std::set<Vertex *> globalWorkSet;

  omp_set_num_threads(noOfThreads);

#pragma omp parallel for
  for (int i = 0; i < noOfThreads; i++) {
    int tl_startIndex = (blockSize * i);
    int tl_length =
        blockSize + ((i == noOfThreads - 1) ? noOfVertices % noOfThreads : 0);
    assert(tl_startIndex + tl_length <= noOfVertices);

    Vertex *tl_v;
    std::set<Vertex *> tl_tmpSet;
    std::set<Vertex *> tl_localWorkSet;
    std::set<Vertex *> tl_neighbourSet;

    srand(time(0));
    while (progress < target) {
      int tl_offset = rand() % tl_length;
      int tl_index = tl_startIndex + tl_offset;
      assert(tl_index < noOfVertices);

      tl_v = vertices[tl_index];

      /*
        Skip this iteration if the selected vertex:
        1. has already been removed
        2. has zero faces
      */
      if (tl_v->isRemoved() || !tl_v->hasFaces()) {
#pragma omp atomic
        failures++;
        continue;
      }

#pragma omp critical
      {
        /*
          1. Check if this is the first iteration of the thread; if so, skip.
             Otherwise, proceed to the second condition.
             Note that the neighbour set <tl_neighbourSet> is initially empty.

          2. If an edge was collapsed in the previous iteration, remove all
          its neighboring vertices from the global work set <globalWorkSet>.
        */
        if (tl_neighbourSet.size() && !tl_tmpSet.size()) {
          std::set_difference(globalWorkSet.begin(), globalWorkSet.end(),
                              tl_neighbourSet.begin(), tl_neighbourSet.end(),
                              std::inserter(tl_tmpSet, tl_tmpSet.begin()));
          globalWorkSet.swap(tl_tmpSet);
        }

        tl_tmpSet.clear();
        tl_neighbourSet = tl_v->getNeighbourVertices();
        std::set_intersection(globalWorkSet.begin(), globalWorkSet.end(),
                              tl_neighbourSet.begin(), tl_neighbourSet.end(),
                              std::inserter(tl_tmpSet, tl_tmpSet.begin()));

        if (!tl_tmpSet.size())
          globalWorkSet.insert(tl_neighbourSet.begin(), tl_neighbourSet.end());
      }

      bool status = false;
      if (!tl_tmpSet.size()) {
        Edge *edgeWithMinCost = tl_v->getEdgeWithMinCost();
        assert(edgeWithMinCost != NULL);
        status = this->collapseEdge(edgeWithMinCost);
      }

      if (status) {
#pragma omp atomic
        progress++;
      } else {
#pragma omp atomic
        failures++;
      }
    }
  }

  std::cout << "Done [" << failures << " failure(s)]" << std::endl;
}