#pragma once

#include <algorithm>
#include <cstring>
#include <float.h>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

class Vertex;
class Face;
class Edge;
class Volume;
class Mesh;

/******************************************************************************/

class Vertex {
  int id;
  double x, y, z;
  bool removed;

  std::set<Vertex *> neighbourVertices; // TODO: Cleared

  std::set<Face *> faces; // TODO: Cleared

  std::set<Edge *> outgoingEdges; // from  // TODO: Cleared
  std::set<Edge *> incomingEdges; // to  // TODO: Cleared

public:
  double Q[4][4];

  Vertex() = delete;
  Vertex(const int, const double, const double, const double);
  Vertex(Vertex &);

  bool operator<(Vertex &);
  bool operator>(Vertex &);
  bool operator==(Vertex &);

  int getId() const;
  double getX() const;
  double getY() const;
  double getZ() const;
  const std::set<Vertex *> &getNeighbourVertices() const;
  const std::set<Face *> &getFaces() const;
  const std::set<Edge *> &getOutgoingEdges() const;
  const std::set<Edge *> &getIncomingEdges() const;

  void setId(int);

  void addFace(Face *);
  void addOutgoingEdge(Edge *);
  void addIncomingEdge(Edge *);

  void update(const Vertex *);

  void remove();
  void removeFace(Face *);
  void removeOutgoingEdge(Edge *);
  void removeIncomingEdge(Edge *);
  bool isRemoved() const;

  bool hasFaces() const;
  Edge *getEdgeWithMinCost() const;
};

/******************************************************************************/

class Face {
  int id;
  int noOfVertices;
  bool removed;

  std::vector<Vertex *> vertices; // TODO: Cleared
  std::set<Edge *> edges;         // TODO: Cleared

public:
  Face() = delete;
  Face(const int, const int);

  bool operator<(Face &);
  bool operator>(Face &);
  bool operator==(Face &);

  int getId() const;
  int getNoOfVertices() const;
  const Vertex *getVertex(int) const;
  const std::vector<Vertex *> &getVertices() const;
  const std::set<Edge *> &getEdges() const;

  void setVertex(int, Vertex *);

  void addVertex(Vertex *);
  void addEdge(Edge *);

  void replaceVertex(Vertex *, Vertex *);

  void remove();
  void removeEdge(Edge *);
  bool isRemoved() const;
  bool isValid() const;
};

/******************************************************************************/

class Edge {
  int id;
  Vertex *v1;
  Vertex *v2;
  bool removed;
  bool modified;

  double cost;
  Vertex *placement;
  std::set<Face *> faces; // TODO: Cleared

  void updatePlacement();

public:
  Edge() = delete;
  Edge(const int, Vertex *, Vertex *);

  bool operator<(Edge &);
  bool operator>(Edge &);
  bool operator==(Edge &);

  int getId() const;
  const Vertex *getV1() const;
  const Vertex *getV2() const;
  const double getCost() const;
  const Vertex *getPlacement() const;
  const std::set<Face *> &getFaces() const;

  void setV1(Vertex *);
  void setV2(Vertex *);
  void setCost(double c);

  void addFace(Face *);

  void modifiy();
  bool isModified() const;

  void remove();
  void removeFace(Face *);
  bool isRemoved() const;
};

/******************************************************************************/

class Volume {
  double minX, minY, minZ;
  double maxX, maxY, maxZ;

public:
  Volume();

  double getXDim() const;
  double getYDim() const;
  double getZDim() const;

  void setMin(double, double, double);
  void setMax(double, double, double);
};

/******************************************************************************/

class Mesh {
  int noOfVertices;
  int noOfFaces;
  int noOfEdges;

  Volume volume;

  std::vector<Vertex *> vertices;
  std::vector<Face *> faces;
  std::vector<Edge *> edges;

  void readVertices(const FILE *);
  void readFaces(const FILE *);
  void readEdges(const FILE *);

  void read(const char *);
  void write(const char *);

public:
  Mesh();
  Mesh(const char *inputFile);

  const int getNoOfVertices() const;
  const int getNoOfFaces() const;
  const int getNoOfEdges() const;
  const std::vector<Vertex *> &getVertices() const;
  const std::vector<Face *> &getFaces() const;
  const std::vector<Edge *> &getEdges() const;

  void saveAsOFF(const char *);
};
