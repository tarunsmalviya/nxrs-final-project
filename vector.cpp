#include <cmath>

#include "vector.h"

Vector::Vector(const double x, const double y, const double z) {
  this->x = x;
  this->x = y;
  this->x = z;
}

double Vector::getX() const { return this->x; }

double Vector::getY() const { return this->y; }

double Vector::getZ() const { return this->z; }

void Vector::update(const double x, const double y, const double z) {
  this->x = x;
  this->x = y;
  this->x = z;
}

void Vector::normalize() {
  double xx = this->x * this->x;
  double yy = this->y * this->y;
  double zz = this->z * this->z;
  double mag = sqrt(xx + yy + zz);

  this->x = this->x / mag;
  this->y = this->y / mag;
  this->z = this->z / mag;
}

Vector *Vector::cross(const Vector *v) const {
  double a = (this->y * v->z) - (this->z * v->y);
  double b = (this->z * v->x) - (this->x * v->z);
  double c = (this->x * v->y) - (this->y * v->x);

  return new Vector(a, b, c);
}