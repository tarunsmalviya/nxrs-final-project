#pragma once

class Vector {
  double x;
  double y;
  double z;

public:
  Vector() = delete;
  Vector(const double, const double, const double);

  double getX() const;
  double getY() const;
  double getZ() const;
  void update(const double, const double, const double);

  void normalize();
  Vector *cross(const Vector *v) const;
};