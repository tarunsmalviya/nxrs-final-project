#include "mesh.h"
#include <cassert>

/******************************************************************************/
/* Vertex */

Vertex::Vertex(const int id, const double x, const double y, const double z) {
  this->id = id;
  this->x = x;
  this->y = y;
  this->z = z;
  this->removed = false;
  this->neighbourVertices.insert(this);

  memset(this->Q, 0, sizeof(double) * 16);
}

Vertex::Vertex(Vertex &v) {
  this->id = v.id;
  this->x = v.x;
  this->y = v.y;
  this->z = v.z;
  this->removed = v.removed;

  memcpy(this->Q, v.Q, sizeof(double) * 16);
}

bool Vertex::operator<(Vertex &v) { return this->id < v.id; }

bool Vertex::operator>(Vertex &v) { return this->id > v.id; }

bool Vertex::operator==(Vertex &v) { return this->id == v.id; }

int Vertex::getId() const { return this->id; }

double Vertex::getX() const { return this->x; }

double Vertex::getY() const { return this->y; }

double Vertex::getZ() const { return this->z; }

const std::set<Vertex *> &Vertex::getNeighbourVertices() const {
  return this->neighbourVertices;
}

const std::set<Face *> &Vertex::getFaces() const { return this->faces; }

const std::set<Edge *> &Vertex::getOutgoingEdges() const {
  return this->outgoingEdges;
}

const std::set<Edge *> &Vertex::getIncomingEdges() const {
  return this->incomingEdges;
}

void Vertex::setId(int id) { this->id = id; }

void Vertex::addFace(Face *f) { this->faces.insert(f); }

void Vertex::addOutgoingEdge(Edge *e) {
  this->neighbourVertices.insert((Vertex *)e->getV2());
  this->outgoingEdges.insert(e);
}

void Vertex::addIncomingEdge(Edge *e) {
  this->neighbourVertices.insert((Vertex *)e->getV1());
  this->incomingEdges.insert(e);
}

void Vertex::update(const Vertex *v) {
  this->x = v->x;
  this->y = v->y;
  this->z = v->z;
  memcpy(this->Q, v->Q, sizeof(double) * 16);
}

void Vertex::remove() {
  if (this->removed) {
    return;
  }

  this->neighbourVertices.clear();
  this->outgoingEdges.clear();
  this->incomingEdges.clear();

  this->removed = true;
}

void Vertex::removeFace(Face *f) { this->faces.erase(f); }

void Vertex::removeOutgoingEdge(Edge *e) {
  this->neighbourVertices.erase((Vertex *)e->getV1());
  this->neighbourVertices.erase((Vertex *)e->getV2());
  this->outgoingEdges.erase(e);
}

void Vertex::removeIncomingEdge(Edge *e) {
  this->neighbourVertices.erase((Vertex *)e->getV1());
  this->neighbourVertices.erase((Vertex *)e->getV2());
  this->incomingEdges.erase(e);
}

bool Vertex::isRemoved() const { return this->removed; }

bool Vertex::hasFaces() const { return this->faces.size() > 0; }

Edge *Vertex::getEdgeWithMinCost() const {
  Edge *edgeWithMinCost = NULL;
  for (Edge *e : this->outgoingEdges) {
    if (edgeWithMinCost == NULL ||
        (!e->isRemoved() && e->getCost() < edgeWithMinCost->getCost())) {
      edgeWithMinCost = e;
    }
  }
  for (Edge *e : this->incomingEdges) {
    if (edgeWithMinCost == NULL ||
        (!e->isRemoved() && e->getCost() < edgeWithMinCost->getCost())) {
      edgeWithMinCost = e;
    }
  }

  return edgeWithMinCost;
}

/******************************************************************************/
/* Face */

Face::Face(const int id, const int noOfVertices) {
  this->id = id;
  this->noOfVertices = noOfVertices;
  this->removed = false;
  this->vertices.reserve(noOfVertices);
}

bool Face::operator<(Face &f) { return this->id < f.id; }

bool Face::operator>(Face &f) { return this->id > f.id; }

bool Face::operator==(Face &f) { return this->id == f.id; }

int Face::getId() const { return this->id; }

int Face::getNoOfVertices() const { return this->noOfVertices; }

const Vertex *Face::getVertex(int id) const {
  return id < this->noOfVertices ? this->vertices[id] : NULL;
}
const std::vector<Vertex *> &Face::getVertices() const {
  return this->vertices;
}

const std::set<Edge *> &Face::getEdges() const { return this->edges; }

void Face::setVertex(int id, Vertex *v) { this->vertices[id] = v; }

void Face::addVertex(Vertex *v) { this->vertices.push_back(v); }

void Face::addEdge(Edge *e) { this->edges.insert(e); }

void Face::replaceVertex(Vertex *v1, Vertex *v2) {
  for (int i = 0; i < this->getNoOfVertices(); i++) {
    if (this->vertices[i] == v1) {
      this->vertices[i] = v2;
    }
  }
}

void Face::remove() {
  if (this->removed) {
    return;
  }

  // Remove this face from all its associated vertices
  for (Vertex *v : this->vertices) {
    v->removeFace(this);
  }
  this->vertices.clear();

  // Remove this face from all its associated edges
  for (Edge *e : this->edges) {
    e->removeFace(this);
  }
  this->edges.clear();

  this->removed = true;
}

void Face::removeEdge(Edge *e) { this->edges.erase(e); }

bool Face::isRemoved() const { return this->removed; }

bool Face::isValid() const { return vertices.size() > 0; }

/******************************************************************************/
/* Edge */

void Edge::updatePlacement() {
  if (this->placement) {
    delete this->placement;
  }
  assert(v1 && v2);
  this->placement =
      new Vertex(0, (v1->getX() + v2->getX()) / 2,
                 (v1->getY() + v2->getY()) / 2, (v1->getZ() + v2->getZ()) / 2);
}

Edge::Edge(const int id, Vertex *v1, Vertex *v2) {
  this->id = id;
  this->v1 = v1;
  this->v2 = v2;
  this->removed = false;
  this->modified = false;
  this->cost = 0.0;
  this->placement = NULL;
  updatePlacement();
}

bool Edge::operator<(Edge &e) { return this->id < e.id; }

bool Edge::operator>(Edge &e) { return this->id > e.id; }

bool Edge::operator==(Edge &e) { return this->id == e.id; }

int Edge::getId() const { return this->id; }

const Vertex *Edge::getV1() const { return this->v1; }

const Vertex *Edge::getV2() const { return this->v2; }

const double Edge::getCost() const { return this->cost; }

const Vertex *Edge::getPlacement() const { return this->placement; }

const std::set<Face *> &Edge::getFaces() const { return this->faces; }

void Edge::setV1(Vertex *v) {
  this->v1 = v;
  updatePlacement();
}

void Edge::setV2(Vertex *v) {
  this->v2 = v;
  updatePlacement();
}

void Edge::setCost(double c) { this->cost = c; }

void Edge::addFace(Face *f) { this->faces.insert(f); }

void Edge::modifiy() { this->modified = true; }

bool Edge::isModified() const { return this->modified; }

void Edge::remove() {
  if (this->removed) {
    return;
  }

  // Remove this edge from the v1 and v2 vertex
  this->v1->removeOutgoingEdge(this);
  this->v2->removeIncomingEdge(this);

  // Remove this edge from all its associated faces
  for (Face *f : this->faces) {
    f->removeEdge(this);
  }
  this->faces.clear();

  this->removed = true;
}

void Edge::removeFace(Face *f) { this->faces.erase(f); }

bool Edge::isRemoved() const { return this->removed; }

/******************************************************************************/
/* Volume */

Volume::Volume() {
  this->minX = DBL_MAX;
  this->minY = DBL_MAX;
  this->minZ = DBL_MAX;

  this->maxX = DBL_MIN;
  this->maxY = DBL_MIN;
  this->maxZ = DBL_MIN;
}

double Volume::getXDim() const { return maxX - minX; }

double Volume::getYDim() const { return maxY - minY; }

double Volume::getZDim() const { return maxZ - minZ; }

void Volume::setMin(double x, double y, double z) {
  if (x < this->minX) {
    this->minX = x;
  }
  if (y < this->minY) {
    this->minY = y;
  }
  if (z < this->minZ) {
    this->minZ = z;
  }
}

void Volume::setMax(double x, double y, double z) {
  if (x > this->maxX) {
    this->maxX = x;
  }
  if (y > this->maxY) {
    this->maxY = y;
  }
  if (z > this->maxZ) {
    this->maxZ = z;
  }
}

/******************************************************************************/
/* Mesh */

void Mesh::readVertices(const FILE *file) {
  std::cout << "Reading vertices... ";

  FILE *f = (FILE *)file;
  double x, y, z;
  for (int i = 0; i < this->noOfVertices; i++) {
    if (fscanf(f, "%lf %lf %lf\n", &x, &y, &z) != 3) {
      std::cout << "Failed!" << std::endl;
      std::cerr << std::endl
                << "Error:  Invalid input "
                   "file format. Only OFF "
                   "(Object File "
                   "Format) (.off) files are "
                   "accepted."
                << std::endl;
      exit(14);
    }

    this->volume.setMin(x, y, z);
    this->volume.setMax(x, y, z);
    this->vertices.push_back(new Vertex(i, x, y, z));
  }

  std::cout << "Done" << std::endl;
}

void Mesh::readFaces(const FILE *file) {
  std::cout << "Reading faces... ";

  FILE *f = (FILE *)file;
  int nv, v1, v2, v3;
  for (int i = 0; i < this->noOfFaces; i++) {
    if (fscanf(f, "%d %d %d %d\n", &nv, &v1, &v2, &v3) != 4) {
      std::cout << "Failed!" << std::endl;
      std::cerr << std::endl
                << "Error:  Invalid input "
                   "file format. Only OFF "
                   "(Object File "
                   "Format) (.off) files are "
                   "accepted."
                << std::endl;
      exit(15);
    }

    Face *face = new Face(i, nv);
    face->addVertex(this->vertices.at(v1));
    face->addVertex(this->vertices.at(v2));
    face->addVertex(this->vertices.at(v3));

    this->vertices.at(v1)->addFace(face);
    this->vertices.at(v2)->addFace(face);
    this->vertices.at(v3)->addFace(face);

    this->faces.push_back(face);
  }

  std::cout << "Done" << std::endl;
}

void Mesh::readEdges(const FILE *file) {
  std::cout << "Populating edges... ";

  std::vector<Vertex *> vertices;
  vertices.reserve(3);

  int map[3][2] = {{0, 1}, {0, 2}, {1, 2}};

  int eid = 0;
  for (Face *face : this->faces) {
    vertices.clear();
    for (int i = 0; i < face->getNoOfVertices(); i++) {
      vertices.push_back((Vertex *)face->getVertex(i));
    }
    std::sort(vertices.begin(), vertices.end());

    for (int i = 0; i < 3; i++) {
      Vertex *v1 = vertices[map[i][0]];
      Vertex *v2 = vertices[map[i][1]];

      const std::set<Edge *> &v1OutgoingEdges = v1->getOutgoingEdges();

      Edge *e = NULL;
      for (Edge *oe : v1OutgoingEdges) {
        if (oe->getV2() == v2) {
          e = oe;
          // Edge already exists, add this face to the edge
          e->addFace(face);
          face->addEdge(e);
        }
      }

      if (!e) {
        // Edge does not exist
        Edge *e = new Edge(eid, v1, v2);
        e->addFace(face);

        v1->addOutgoingEdge(e);
        v2->addIncomingEdge(e);

        face->addEdge(e);

        this->edges.push_back(e);

        eid++;
      }
    }
  }

  this->noOfEdges = eid;

  std::cout << "Done" << std::endl;
}

void Mesh::read(const char *inputFile) {
  int rv;
  char buffer[256];

  FILE *file = fopen(inputFile, "r");
  if (!file) {
    std::cerr << std::endl
              << "Error:  Unable to read "
                 "input file. Please check "
                 "the file "
                 "path and permissions."
              << std::endl;
    exit(11);
  }

  rv = fscanf(file, "%s\n", buffer);
  if (!rv || strncmp("OFF", buffer, 3)) {
    std::cerr << std::endl
              << "Error:  Invalid input file "
                 "format. Only OFF (Object "
                 "File "
                 "Format) (.off) files are "
                 "accepted."
              << std::endl;
    exit(12);
  }

  rv = fscanf(file, "%d %d %d\n", &this->noOfVertices, &this->noOfFaces,
              &this->noOfEdges);
  if (rv != 3) {
    std::cerr << std::endl
              << "Error:  Invalid input file "
                 "format. Only OFF (Object "
                 "File "
                 "Format) (.off) files are "
                 "accepted."
              << std::endl;
    exit(13);
  }

  std::cout << std::endl;
  this->readVertices(file);
  this->readFaces(file);
  this->readEdges(file);

  std::cout << std::endl;
  std::cout << "Number Of Vertex(s) : " << this->noOfVertices << std::endl;
  std::cout << "Number Of Face(s)   : " << this->noOfFaces << std::endl;
  std::cout << "Number Of Edge(s)   : " << this->noOfEdges << std::endl;

  std::cout << std::endl;
  std::cout << "Volume Dimensions   : [" << this->volume.getXDim() << ", "
            << this->volume.getYDim() << ", " << this->volume.getZDim() << "]"
            << std::endl;

  fclose(file);
}

void Mesh::write(const char *inputFile) {
  std::cout << std::endl;
  std::cout << "Saving mesh in OFF format... ";

  FILE *file = fopen(inputFile, "w");
  if (!file) {
    std::cerr << std::endl
              << "Error:  Unable to "
                 "create output file."
              << std::endl;
    exit(16);
  }

  this->vertices.erase(
      std::remove_if(this->vertices.begin(), this->vertices.end(),
                     [](const Vertex *v) { return v->isRemoved(); }),
      this->vertices.end());

  this->faces.erase(
      std::remove_if(this->faces.begin(), this->faces.end(),
                     [](const Face *f) { return f->isRemoved(); }),
      this->faces.end());

  fprintf(file, "OFF\n");
  fprintf(file, "%d %d %d\n", (int)this->vertices.size(),
          (int)this->faces.size(), 0);

  for (int i = 0; i < this->vertices.size(); i++) {
    Vertex *v = this->vertices[i];
    if (v != NULL) {
      v->setId(i);
      fprintf(file, "%lf %lf %lf\n", v->getX(), v->getY(), v->getZ());
    }
  }

  for (int i = 0; i < this->faces.size(); i++) {
    Face *f = this->faces[i];
    if (f != NULL && f->getVertices().size() == 3) {
      const Vertex *v1 = f->getVertex(0);
      const Vertex *v2 = f->getVertex(1);
      const Vertex *v3 = f->getVertex(2);
      fprintf(file, "%d %d %d %d\n", f->getNoOfVertices(), v1->getId(),
              v2->getId(), v3->getId());
    }
  }

  fclose(file);

  std::cout << "Done" << std::endl;
}

Mesh::Mesh(const char *inputFile) { read(inputFile); }

const int Mesh::getNoOfVertices() const { return this->noOfVertices; }

const int Mesh::Mesh::getNoOfFaces() const { return this->noOfFaces; }

const int Mesh::getNoOfEdges() const { return this->noOfEdges; }

const std::vector<Vertex *> &Mesh::getVertices() const {
  return this->vertices;
}

const std::vector<Face *> &Mesh::getFaces() const { return this->faces; }

const std::vector<Edge *> &Mesh::getEdges() const { return this->edges; }

void Mesh::saveAsOFF(const char *outputFile) { this->write(outputFile); }