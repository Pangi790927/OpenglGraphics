#ifndef MATHLIB_H_INCLUDED
#define MATHLIB_H_INCLUDED

#include <cmath>

#include "Point1.h"
#include "Point2.h"
#include "Point3.h"
#include "Point4.h"
#include "Matrix4f.h"

typedef Point1<float>  Point1f;
typedef Point1<int>    Point1i;
typedef Point1<double> Point1d;
typedef Point1<bool>   Point1b;

typedef Point2<float>  Point2f;
typedef Point2<int>    Point2i;
typedef Point2<double> Point2d;
typedef Point2<bool>   Point2b;

typedef Point3<float>  Point3f;
typedef Point3<int>    Point3i;
typedef Point3<double> Point3d;
typedef Point3<bool>   Point3b;

typedef Point4<float>  Point4f;
typedef Point4<int>    Point4i;
typedef Point4<double> Point4d;
typedef Point4<bool>   Point4b;

typedef Point4<float>  Vector4f;
typedef Point3<float>  Vector3f;
typedef Point2<float>  Vector2f;

#endif // MATHLIB_H_INCLUDED
