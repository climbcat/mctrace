/*
MIT License

Copyright (c) 2025 Jakob Garde

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#ifndef __JG_CBUI_H__
#define __JG_CBUI_H__


#include <math.h>
#include "jg_baselayer.h"
#ifndef __GTYPES_H__
#define __GTYPES_H__


//
// Colors


#define RGBA_BLACK      0, 0, 0, 255
#define RGBA_WHITE      255, 255, 255, 255
#define RGBA_GRAY_75    192, 192, 192, 255
#define RGBA_GRAY_50    128, 128, 128, 255
#define RGBA_GRAY_25    64, 64, 64, 255
#define RGBA_RED        255, 0, 0, 255
#define RGBA_GREEN      0, 255, 0, 255
#define RGBA_BLUE       0, 0, 255, 255
#define RGBA_YELLOW     255, 255, 0, 255


#define BYTES_RGB 3
#define BYTES_RGBA 4


struct Color {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    u32 GetAsU32() {
        return * (u32*) this;
    }
    inline
    bool IsZero() {
        return r == 0 && g == 0 && b == 0 && a == 0;
    }
    inline
    bool IsNonZero() {
        return ! IsZero();
    }
};
void PrintColorInline(Color c) {
    printf("%hhx %hhx %hhx %hhx", c.r, c.g, c.b, c.a);
}
Color ColorRandom() {
    Color c;
    c.r = RandMinMaxU(100, 255);
    c.g = RandMinMaxU(100, 255);
    c.b = RandMinMaxU(100, 255);
    c.a = 255;
    return c;
}
Color ColorWhite() {
    return Color { RGBA_WHITE };
}
Color ColorBlack() {
    return Color { RGBA_BLACK };
}
Color ColorGray(f32 grayness) {
    u8 g = (u8) floor(grayness * 255);
    return Color { g, g, g, 255 };
}
Color ColorRed() {
    return Color { RGBA_RED };
}
Color ColorGreen() {
    return Color { RGBA_GREEN };
}
Color ColorBlue() {
    return Color { RGBA_BLUE };
}
Color ColorYellow() {
    return Color { RGBA_YELLOW };
}


//
// Rectangles


struct Rect {
    u16 width;
    u16 height;
    s16 left;
    s16 top;

    void Print() {
        printf("rect: w: %u, h: %u, left: %d, top: %d\n", width, height, left, top);
    }
};


Rect InitRectangle(u16 width, u16 height, u16 left = 0, u16 top = 0) {
    Rect r;
    r.width = width;
    r.height = height;
    r.left = left;
    r.top = top;
    return r;
}
Rect RectangleCrop(Rect us, Rect other) {
    Rect rect = InitRectangle(other.width, other.height, other.left, other.top);
    bool occluded = false;
    bool partially_occluded = false;

    // cases where other is completely outside of us
    if (other.left > us.left + us.width) { // to the right of us
        rect.left = us.left + us.width;
        rect.width = 0;
        occluded = true;
    }
    if (other.left + other.width < us.left) { // to the left of us
        rect.left = us.left;
        rect.width = 0;
        occluded = true;
    }
    if (other.top > us.top + us.height) { // above us
        rect.top = us.top + us.height;
        rect.height = 0;
        occluded = true;
    }
    if (other.top + other.height < us.top) { // below us
        rect.top = us.top;
        rect.height = 0;
        occluded = true;
    }
    if (occluded) {
        return rect;
    }

    // at least partially visible
    if (other.left < us.left) {
        rect.left = us.left;
        s16 diff = us.left - other.left;
        rect.width = rect.width - diff;
    }
    if (other.top < us.top) {
        rect.top = us.top;
        s16 diff = us.top - other.top;
        rect.height = rect.height - diff;
    }
    if (other.left + other.width > us.left + us.width) {
        rect.width = us.top + us.width;
    }
    if (other.top + other.height > us.top + us.height) {
        rect.height = us.top + us.height;
    }
    return rect;
}


//
//  Image type structs


struct ImageB {
    s32 width;
    s32 height;
    u8 *img;
};

struct ImageRGBX {
    u32 width;
    u32 height;
    u32 pixel_size;
    u8 *img;
};
ImageRGBX InitImageRGBX(void *data, u32 width, u32 height, u32 pixel_size) {
    ImageRGBX img;
    img.width = width;
    img.height = height;
    img.pixel_size = pixel_size;
    img.img = (u8*) data;
    return img;
}

struct ImageRGBA {
    s32 width;
    s32 height;
    Color *img;
};
ImageRGBA InitImageRGBA(s32 w, s32 h, void *buffer) {
    return ImageRGBA { w, h, (Color*) buffer };
}

struct ImageF32 {
    s32 width;
    s32 height;
    f32 *data;
};


#endif


#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__


//
//  Vector 2 & 3


struct Vector2_u16 {
    u16 x;
    u16 y;
};

struct Vector2_s16 {
    s16 x;
    s16 y;
};

struct Vector3i {
    u32 i1;
    u32 i2;
    u32 i3;
};

struct Vector2i {
    s32 i1;
    s32 i2;
};

struct Vector2u {
    u32 i1;
    u32 i2;
};

struct Vector2s {
    u32 x;
    u32 y;
};

struct Vector2f {
    f32 x;
    f32 y;
    inline
    static Vector2f Add(Vector2f *a, Vector2f *b) {
        return Vector2f { a->x + b->x, a->y + b->y };
    }
};

inline
Vector2f operator+(Vector2f u, Vector2f v) {
    return Vector2f::Add(&u, &v);
}

Vector2f Vector2f_Zero() {
    return {};
}

struct Matrix4f {
    float m[4][4];
};

struct Vector4f {
    float x;
    float y;
    float z;
    float w;
};

struct Vector3f {
    float x;
    float y;
    float z;

    inline
    bool IsNonZero() {
        return abs(x) > 0.0001f || abs(y) > 0.0001f || abs(z) > 0.0001f;
    }
    inline bool IsZero() {
        return ! IsNonZero();
    }
    inline
    float Norm() {
        return sqrt(x*x + y*y + z*z);
    }
    inline
    float NormSquared() {
        return x*x + y*y + z*z;
    }
    inline
    void ScalarProductOn(float f) {
        x *= f;
        y *= f;
        z *= f;
    }
    inline
    void AddBy(Vector3f v) {
        x += v.x;
        y += v.y;
        z += v.z;
    }
    inline
    void SubtractBy(Vector3f v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }
    inline
    void Normalize() {
        float f = 1 / Norm();
        x *= f;
        y *= f;
        z *= f;
    }
    inline
    Vector3f Unit() {
        float f = 1 / Norm();
        Vector3f unit = {};
        unit.x = x * f;
        unit.y = y * f;
        unit.z = z * f;
        return unit;
    }
    inline
    void Invert() {
        x *= -1;
        y *= -1;
        z *= -1;
    }
    inline
    float Dot(Vector3f v) {
        return x*v.x + y*v.y + z*v.z;
    }
    inline
    Vector3f Cross(Vector3f v) {
        return Vector3f { y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x };
    }

    // static versions
    inline
    static Vector3f Zero() {
        return Vector3f { 0, 0, 0 };
    }
    inline
    // left-handed: Left, Up, Forward
    static Vector3f Left() {
        return Vector3f { 1, 0, 0 };
    }
    inline
    static Vector3f Up() {
        return Vector3f { 0, 1, 0 };
    }
    inline
    static Vector3f Forward() {
        return Vector3f { 0, 0, 1 };
    }
    inline
    static Vector3f X() {
        return Vector3f { 1, 0, 0 };
    }
    inline
    static Vector3f Y() {
        return Vector3f { 0, 1, 0 };
    }
    inline
    static Vector3f Z() {
        return Vector3f { 0, 0, 1 };
    }
    inline
    static float NormSquared(Vector3f a) {
        return a.x*a.x + a.y*a.y + a.z*a.z;
    }
    inline
    static float Norm(Vector3f a) {
        return sqrt(Vector3f::NormSquared(a));
    }
    inline
    static Vector3f ScalarProduct(float f, Vector3f *a) {
        return Vector3f { f*a->x, f*a->y, f*a->z };
    }
    inline
    static Vector3f Normalize(Vector3f a) {
        float norm_inv = 1 / Vector3f::Norm(a);
        return Vector3f { norm_inv * a.x, norm_inv * a.y, norm_inv * a.z };
    }
    inline
    static Vector3f Subtract(Vector3f *a, Vector3f *b) {
        return Vector3f { a->x - b->x, a->y - b->y, a->z - b->z };
    }
    inline
    static Vector3f Add(Vector3f *a, Vector3f *b) {
        return Vector3f { a->x + b->x, a->y + b->y, a->z + b->z };
    }
    inline
    static float Dot(Vector3f *a, Vector3f *b) {
        return a->z*b->z + a->z*b->z + a->z*b->z;
    }
    inline
    static Vector3f Cross(Vector3f *a, Vector3f *b) {
        return Vector3f { a->y*b->z - a->z*b->y, a->z*b->x - a->x*b->z, a->x*b->y - a->y*b->x };
    }
};

inline
Vector3f operator+(Vector3f u, Vector3f v) {
    return Vector3f::Add(&u, &v);
}

inline
Vector3f operator-(Vector3f u, Vector3f v) {
    return Vector3f::Subtract(&u, &v);
}

inline
bool operator==(Vector3f u, Vector3f v) {
    return (u.x == v.x) && (u.y == v.y) && (u.z == v.z);
}

inline
Vector3f operator*(float f, Vector3f v) {
    return Vector3f::ScalarProduct(f, &v);
}

inline
Vector3f operator-(Vector3f v) {
    return Vector3f::ScalarProduct(-1, &v);
}

Vector3f x_hat { 1, 0, 0 };
Vector3f y_hat { 0, 1, 0 };
Vector3f z_hat { 0, 0, 1 };

Vector3f Vector3f_Zero() {
    return Vector3f { 0, 0, 0 };
}

Vector3f Vector3f_Ones() {
    return Vector3f { 1, 1, 1 };
}

Vector3f SphericalCoordsY(float theta, float phi, float radius) {
    Vector3f v;
    v.x = radius * sin(theta) * cos(phi);
    v.y = radius * cos(theta);
    v.z = radius * sin(theta) * sin(phi);
    return v;
}


//
//  Matrix4f


inline
Matrix4f Matrix4f_Zero() {
    Matrix4f m;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m.m[i][j] = 0;
        }
    }
    return m;
}

inline
Matrix4f Matrix4f_One() {
    Matrix4f m;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m.m[i][j] = 1;
        }
    }
    return m;
}

inline
Matrix4f Matrix4f_Identity() {
    Matrix4f m;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i == j) {
                m.m[i][j] = 1;
            }
            else {
                m.m[i][j] = 0;
            }
        }
    }
    return m;
}

inline
bool Matrix4f_IsIdentity(Matrix4f m) {
    bool is_zero =
        m.m[0][0] == 1 &&
        m.m[0][1] == 0 &&
        m.m[0][2] == 0 &&
        m.m[0][3] == 0 &&

        m.m[1][0] == 0 &&
        m.m[1][1] == 1 &&
        m.m[1][2] == 0 &&
        m.m[1][3] == 0 &&

        m.m[2][0] == 0 &&
        m.m[2][1] == 0 &&
        m.m[2][2] == 1 &&
        m.m[2][3] == 0 &&

        m.m[3][0] == 0 &&
        m.m[3][1] == 0 &&
        m.m[3][2] == 0 &&
        m.m[3][3] == 1;
    return is_zero;
}

inline
Matrix4f Matrix4f_Diagonal(Vector4f d) {
    Matrix4f m = Matrix4f_Zero();
    m.m[0][0] = d.x;
    m.m[1][1] = d.y;
    m.m[2][2] = d.z;
    m.m[3][3] = d.w;
    return m;
}

inline
Matrix4f Matrix4f_Transpose(Matrix4f *m) {
    Matrix4f result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m->m[j][i];
        }
    }
    return result;
}

inline
Matrix4f Matrix4f_Transpose(Matrix4f m) {
    Matrix4f result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = m.m[j][i];
        }
    }
    return result;
}

inline
Matrix4f Matrix4f_Multiply(Matrix4f *a, Matrix4f *b) {
    Matrix4f result = {};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                result.m[i][j] += a->m[i][k]*b->m[k][j];
            }
        }
    }
    return result;
}

inline
Vector4f Matrix4f_MultVector(Matrix4f *a, Vector4f *v) {
    Vector4f result;
    result.x = a->m[0][0]*v->x + a->m[0][1]*v->y + a->m[0][2]*v->z + a->m[0][3]*v->w;
    result.y = a->m[1][0]*v->x + a->m[1][1]*v->y + a->m[1][2]*v->z + a->m[1][3]*v->w;
    result.z = a->m[2][0]*v->x + a->m[2][1]*v->y + a->m[2][2]*v->z + a->m[2][3]*v->w;
    result.w = a->m[3][0]*v->x + a->m[3][1]*v->y + a->m[3][2]*v->z + a->m[3][3]*v->w;
    return result;
}

inline
bool Matrix4f_Equals(Matrix4f *a, Matrix4f *b) {
    bool result = true;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result &= a->m[i][j] == b->m[i][j];
        }
    }
    return result;
}

inline
Matrix4f operator*(Matrix4f a, Matrix4f b) {
    return Matrix4f_Multiply(&a, &b);
}

inline
Vector4f operator*(Matrix4f m, Vector4f v) {
    return Matrix4f_MultVector(&m, &v);
}

inline
bool operator==(Matrix4f a, Matrix4f b) {
    return Matrix4f_Equals(&a, &b);
}

inline
void MatrixNf_Transpose(float *dest, float *src, u32 dims) {
    for (u32 row = 0; row < dims; ++row) {
        for (u32 col = 0; col < dims; ++col) {
            dest[row*dims + col] = src[col*dims + row];
        }
    }
}

inline
void MatrixNf_Multiply(float *dest, float *a, float *b, u32 dims) {
    for (u32 i = 0; i < dims; ++i) {
        for (u32 j = 0; j < dims; ++j) {
            u32 I = i*dims;
            dest[I + j] = 0;
            for (u32 k = 0; k < dims; ++k) {
                dest[I + j] += a[I + k]*b[k*dims + j];
            }
        }
    }
}

inline
void MatrixNf_MultVector(float *dest, float *a, float *v, u32 dims) {
    for (u32 i = 0; i < dims; ++i) {
        dest[i] = 0;
        for (u32 k = 0; k < dims; ++k) {
            dest[i] += a[i*dims + k]*v[k];
        }
    }
}

Matrix4f Matrix4f_FlipX() {
    Matrix4f flip = Matrix4f_Identity();
    flip.m[0][0] = -1;
    return flip;
}

Matrix4f Matrix4f_FlipY() {
    Matrix4f flip = Matrix4f_Identity();
    flip.m[1][1] = -1;
    return flip;
}

Matrix4f Matrix4f_FlipZ() {
    Matrix4f flip = Matrix4f_Identity();
    flip.m[2][2] = -1;
    return flip;
}


//
// Transform


Matrix4f TransformBuild(Vector3f axis, float angle_rads, Vector3f translate = {0, 0, 0}) {
    Matrix4f result = Matrix4f_Zero();

    float epsilon_f = 0.0000001f;
    assert( abs(axis.Norm() - 1) < epsilon_f );

    // build rot from [axis, angle] - see https://en.wikipedia.org/wiki/Rotation_matrix#Axis_and_angle
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    float c = cos(angle_rads);
    float s = sin(angle_rads);

    result.m[0][0] = x*x*(1 - c) + c;
    result.m[0][1] = x*y*(1 - c) - z*s;
    result.m[0][2] = x*z*(1 - c) + y*s;

    result.m[1][0] = y*x*(1 - c) + z*s;
    result.m[1][1] = y*y*(1 - c) + c;
    result.m[1][2] = y*z*(1 - c) - x*s;

    result.m[2][0] = z*x*(1 - c) - y*s;
    result.m[2][1] = z*y*(1 - c) + x*s;
    result.m[2][2] = z*z*(1 - c) + c;

    // translation
    result.m[0][3] = translate.x;
    result.m[1][3] = translate.y;
    result.m[2][3] = translate.z;
    result.m[3][3] = 1;

    return result;
}

Matrix4f TransformBuildRotateX(float angle_rads) {
    return TransformBuild(x_hat, angle_rads);
}

Matrix4f TransformBuildRotateY(float angle_rads) {
    return TransformBuild(y_hat, angle_rads);
}

Matrix4f TransformBuildRotateZ(float angle_rads) {
    return TransformBuild(z_hat, angle_rads);
}

Matrix4f TransformBuildTranslationOnly(Vector3f translate) {
    return TransformBuild(Vector3f {1, 0, 0}, 0, translate);
}

Matrix4f TransformBuildTranslation(Vector3f translate) {
    return TransformBuild(Vector3f {1, 0, 0}, 0, translate);
}

Matrix4f TransformGetInverse(Matrix4f *a) {
    // M^{-1}: row 0-2: R^{-1}, - R^{-1} * t; row 3: 0^, 1

    // 0^, 1
    Matrix4f result = Matrix4f_Zero();
    result.m[3][3] = 1;

    // R^{-1}
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result.m[i][j] = a->m[j][i];
        }
    }

    // R^{-1} * t
    Vector3f t_inv;
    t_inv.x = result.m[0][0]*a->m[0][3] + result.m[0][1]*a->m[1][3] + result.m[0][2]*a->m[2][3];
    t_inv.y = result.m[1][0]*a->m[0][3] + result.m[1][1]*a->m[1][3] + result.m[1][2]*a->m[2][3];
    t_inv.z = result.m[2][0]*a->m[0][3] + result.m[2][1]*a->m[1][3] + result.m[2][2]*a->m[2][3];

    // *= -1 and copy
    result.m[0][3] = - t_inv.x;
    result.m[1][3] = - t_inv.y;
    result.m[2][3] = - t_inv.z;

    return result;
}

inline
Matrix4f TransformGetInverse(Matrix4f a) {
    return TransformGetInverse(&a);
}

inline
Vector3f TransformGetTranslation(Matrix4f transform) {
    Vector3f result { transform.m[0][3], transform.m[1][3], transform.m[2][3] };
    return result;
}

inline
Matrix4f TransformSetTranslation(const Matrix4f transform, const Vector3f translation) {
    Matrix4f result = transform;
    result.m[0][3] = translation.x;
    result.m[1][3] = translation.y;
    result.m[2][3] = translation.z;
    return result;
}

inline
Vector3f TransformPoint(Matrix4f *a, Vector3f *v) {
    Vector3f result;

    // rot / trans
    result.x = a->m[0][0]*v->x + a->m[0][1]*v->y + a->m[0][2]*v->z + a->m[0][3];
    result.y = a->m[1][0]*v->x + a->m[1][1]*v->y + a->m[1][2]*v->z + a->m[1][3];
    result.z = a->m[2][0]*v->x + a->m[2][1]*v->y + a->m[2][2]*v->z + a->m[2][3];

    return result;
}

inline
Vector3f TransformPoint(Matrix4f a, Vector3f v) {
    Vector3f result;

    // rot / trans
    result.x = a.m[0][0]*v.x + a.m[0][1]*v.y + a.m[0][2]*v.z + a.m[0][3];
    result.y = a.m[1][0]*v.x + a.m[1][1]*v.y + a.m[1][2]*v.z + a.m[1][3];
    result.z = a.m[2][0]*v.x + a.m[2][1]*v.y + a.m[2][2]*v.z + a.m[2][3];

    return result;
}

// TODO: how do I build a vector that does this ?
inline
Vector3f TransformInversePoint(Matrix4f *a, Vector3f *v) {
    Vector3f r;
    Vector3f tmp;

    // translate back
    tmp.x = v->x - a->m[0][3];
    tmp.y = v->y - a->m[1][3];
    tmp.z = v->z - a->m[2][3];

    // rotate back (transpose)
    r.x = a->m[0][0]*tmp.x + a->m[1][0]*tmp.y + a->m[2][0]*tmp.z;
    r.y = a->m[0][1]*tmp.x + a->m[1][1]*tmp.y + a->m[2][1]*tmp.z;
    r.z = a->m[0][2]*tmp.x + a->m[1][2]*tmp.y + a->m[2][2]*tmp.z;

    return r;
}

inline
Vector3f TransformInversePoint(Matrix4f a, Vector3f v) {
    Vector3f r;
    Vector3f tmp;

    // translate back
    tmp.x = v.x - a.m[0][3];
    tmp.y = v.y - a.m[1][3];
    tmp.z = v.z - a.m[2][3];

    // rotate back (transpose)
    r.x = a.m[0][0]*tmp.x + a.m[1][0]*tmp.y + a.m[2][0]*tmp.z;
    r.y = a.m[0][1]*tmp.x + a.m[1][1]*tmp.y + a.m[2][1]*tmp.z;
    r.z = a.m[0][2]*tmp.x + a.m[1][2]*tmp.y + a.m[2][2]*tmp.z;

    return r;
}

inline
Vector3f TransformDirection(Matrix4f *a, Vector3f *d) {
    Vector3f result;

    // rotate
    result.x = a->m[0][0]*d->x + a->m[0][1]*d->y + a->m[0][2]*d->z;
    result.y = a->m[1][0]*d->x + a->m[1][1]*d->y + a->m[1][2]*d->z;
    result.z = a->m[2][0]*d->x + a->m[2][1]*d->y + a->m[2][2]*d->z;

    return result;
}

inline
Vector3f TransformDirection(Matrix4f a, Vector3f d) {
    Vector3f result;

    // just rotate
    result.x = a.m[0][0]*d.x + a.m[0][1]*d.y + a.m[0][2]*d.z;
    result.y = a.m[1][0]*d.x + a.m[1][1]*d.y + a.m[1][2]*d.z;
    result.z = a.m[2][0]*d.x + a.m[2][1]*d.y + a.m[2][2]*d.z;

    return result;
}

inline
Vector3f TransformInverseDirection(Matrix4f *a, Vector3f *d) {
    Vector3f result;

    // rotate back
    result.x = a->m[0][0]*d->x + a->m[1][0]*d->y + a->m[2][0]*d->z;
    result.y = a->m[0][1]*d->x + a->m[1][1]*d->y + a->m[2][1]*d->z;
    result.z = a->m[0][2]*d->x + a->m[1][2]*d->y + a->m[2][2]*d->z;

    // TODO: scale back
    return result;
}

inline
Vector3f TransformInverseDirection(Matrix4f a, Vector3f d) {
    Vector3f result;

    // rotate back
    result.x = a.m[0][0]*d.x + a.m[1][0]*d.y + a.m[2][0]*d.z;
    result.y = a.m[0][1]*d.x + a.m[1][1]*d.y + a.m[2][1]*d.z;
    result.z = a.m[0][2]*d.x + a.m[1][2]*d.y + a.m[2][2]*d.z;

    // TODO: scale back
    return result;
}

Matrix4f TransformBuildLookRotationYUp(Vector3f at, Vector3f from) {
    Vector3f forward = at - from;
    forward.Normalize();
    Vector3f left = y_hat.Cross(forward);
    left.Normalize();
    Vector3f right = - left;
    Vector3f up = forward.Cross(left);
    up.Normalize();

    Matrix4f lookrot = Matrix4f_Identity();
    lookrot.m[0][0] = right.x;
    lookrot.m[1][0] = right.y;
    lookrot.m[2][0] = right.z;
    lookrot.m[0][1] = up.x;
    lookrot.m[1][1] = up.y;
    lookrot.m[2][1] = up.z;
    lookrot.m[0][2] = forward.x;
    lookrot.m[1][2] = forward.y;
    lookrot.m[2][2] = forward.z;

    return lookrot;
}


//
//  Quaternions


struct Quat {
    float w;
    float x;
    float y;
    float z;
};

Quat Quat_Identity() {
    Quat i { 1, 0, 0, 0 };
    return i;
}
Quat QuatAxisAngle(Vector3f axis, float angle) {
    float c = cos(angle * 0.5f);
    float s = sin(angle * 0.5f);
    Quat q;
    q.w = c;
    q.x = axis.x * s;
    q.y = axis.y * s;
    q.z = axis.z * s;
    return q;
}
inline
Quat QuatFromVector(Vector3f v) {
    Quat t { 0.0f, v.x, v.y, v.z };
    return t;
}
inline
Quat QuatInverse(Quat q) {
    Quat t { q.w, -q.x, -q.y, -q.z };
    return t;
}
Quat QuatMult(Quat q1, Quat q2) {
    Quat t;
    t.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
    t.x = q1.w*q2.x + q1.x*q2.w - q1.y*q2.z + q1.z*q2.y;
    t.y = q1.w*q2.y + q1.x*q2.z + q1.y*q2.w - q1.z*q2.x;
    t.z = q1.w*q2.z - q1.x*q2.y + q1.y*q2.x + q1.z*q2.w;
    return t;
}
Vector3f QuatRotate(Quat q, Vector3f v) {
    Quat q_inv = QuatInverse(q);
    Quat q_v = QuatFromVector(v);
    Quat q_v_rot = QuatMult(q_inv, QuatMult(q, q_v));

    Vector3f v_rot { q_v_rot.x, q_v_rot.y, q_v_rot.z };
    return v_rot;
}
Matrix4f TransformQuaternion(Quat q) {
    Matrix4f m = Matrix4f_Identity();
    m.m[0][0] = q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z;
    m.m[0][1] = 2*q.x*q.y - 2*q.w*q.z;
    m.m[0][2] = 2*q.x*q.z + 2*q.w*q.y;
    m.m[1][0] = 2*q.x*q.y + 2*q.w*q.z;
    m.m[1][1] = q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z;
    m.m[1][2] = 2*q.y*q.z - 2*q.w*q.x;
    m.m[2][0] = 2*q.x*q.z - 2*q.w*q.y;
    m.m[2][1] = 2*q.y*q.z + 2*q.w*q.x;
    m.m[2][2] = q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z;
    m.m[3][3] = 1;
    return m;
}
inline
float Matrix4f_Trace(Matrix4f m) {
    float trace = m.m[0][0] + m.m[1][1] + m.m[2][2] + 1;
    return trace;
}
Quat QuatFromTransform(Matrix4f m) {
    assert(Matrix4f_Trace(m) > 0.0f);

    Quat q;
    q.w = sqrt( Matrix4f_Trace(m) ) * 0.5f;
    q.x = (m.m[2][1] - m.m[1][2]) / (4 * q.w);
    q.y = (m.m[0][2] - m.m[2][0]) / (4 * q.w);
    q.z = (m.m[1][0] - m.m[0][1]) / (4 * q.w);
    return q;
}
inline
Quat QuatScalarMult(Quat q, float s) {
    Quat t { s * q.w, s * q.x, s * q.y, s * q.z };
    return t;
}
inline
Quat QuatSum(Quat q1, Quat q2) {
    Quat t { q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z };
    return t;
}
inline
float QuatInnerProduct(Quat q1, Quat q2) {
    float dotprod = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
    return dotprod;
}
Quat Slerp(Quat q1, Quat q2, float t) {
    // TODO: not robust with q1 == q2, please fix
    assert(t >= 0.0f && t <= 1.0f);

    float theta = acos( QuatInnerProduct(q1, q2) );
    float f1 = sin((1 - t)*theta) / sin(theta);
    float f2 = sin(t*theta) / sin(theta);

    Quat q = QuatSum( QuatScalarMult(q1, f1), QuatScalarMult(q2, f2) );
    return q;
}


//
//  Typical transformation builders


inline
Matrix4f TransformBuildMVP(Matrix4f model, Matrix4f view, Matrix4f proj) {
    Matrix4f mvp = proj * TransformGetInverse( view ) * model;
    return mvp;
}

inline
Matrix4f TransformBuildMVP(Matrix4f model, Matrix4f vp) {
    Matrix4f mvp = vp * model;
    return mvp;
}

inline
Matrix4f TransformBuildViewProj(Matrix4f view, Matrix4f proj) {
    Matrix4f mvp = proj * TransformGetInverse( view );
    return mvp;
}

inline
Matrix4f TransformBuildOrbitCam(Vector3f center, float theta_degs, float phi_degs, float radius, Vector3f *campos_out = NULL) {
    Vector3f campos = center + SphericalCoordsY(theta_degs*deg2rad, phi_degs*deg2rad, radius);
    Matrix4f view = TransformBuildTranslationOnly(campos) * TransformBuildLookRotationYUp(center, campos);

    if (campos_out) {
        *campos_out = campos;
    }
    return view;
}

Matrix4f PerspectiveMatrixOpenGL(f32 farr, f32 nearr, f32 fov, f32 aspect, bool flip_x = true, bool flip_y = false, bool flip_z = true) {
    // gather values
    float f = farr;
    float n = nearr;
    float r = nearr * sin(fov / 2 * deg2rad);
    float l = -r;
    float b = r / aspect;
    float t = -b;

    // populate
    Matrix4f m = Matrix4f_Zero();
    m.m[0][0] = 2 * n / (r - l);
    m.m[0][2] = (r + l) / (r - l);
    m.m[1][1] = 2 * n / (t - b);
    m.m[1][2] = (t + b) / (t - b);
    m.m[2][2] = -(f + n) / (f - n);
    m.m[2][3] = -2 * f * n / (f - n);
    m.m[3][2] = -1;

    // flip the axes (flip to suit desired axis configurations)
    Matrix4f flip = Matrix4f_Identity();
    if (flip_x) {
        flip.m[0][0] = -1 * flip.m[0][0];
        m = flip * m;
    }
    if (flip_y) {
        flip.m[1][1] = -1 * flip.m[1][1];
        m = flip * m;
    }
    if (flip_z) {
        flip.m[2][2] = -1 * flip.m[2][2];
        m = flip * m;
    }

    return m;
}

inline
Vector3f TransformPerspective(Matrix4f p, Vector3f v) {
    Vector4f v_hom { v.x, v.y, v.z, 1 }; // homogeneous coordinates
    Vector4f v_clip = p * v_hom; // clipping space
    Vector3f result { v_clip.x / v_clip.w, v_clip.y / v_clip.w, v_clip.z / v_clip.w }; // ndc coordinates

    return result;
}


//
//  Ray


struct Ray {
    // points: (x, y, z, 1)
    // directions: (x, y, z, 0)
    Vector3f pos;
    Vector3f dir;

    inline
    static Ray Zero() {
        return Ray { Vector3f::Zero(), Vector3f::Zero() };
    }
};
Ray TransformRay(Matrix4f *a, Ray *r) {
    return Ray { TransformPoint(a, &r->pos), TransformDirection(a, &r->dir) };
}
inline
Ray TransformRay(Matrix4f a, Ray r) {
    return Ray { TransformPoint(a, r.pos), TransformDirection(a, r.dir) };
}
inline
Ray TransformInverseRay(Matrix4f a, Ray r) {
    return Ray { TransformInversePoint(a, r.pos), TransformInverseDirection(a, r.dir) };
}


//
//  Projection & camera model


struct LensParams {
    float fL; // focal length, typically 24 - 200 [mm]
    float N; // f-number, 1.4 to 60 dimensionless []
    float c; // circle of confusion (diameter), 0.03 [mm]
    float w; // sensor width, 35.9 [mm]
    float h; // sensor height, 24 [mm]
};

struct Perspective {
    float fov; // [degs] (horizontal field of view)
    float aspect; // [1] (width divided by height)
    float dist_near; // [m]
    float dist_far; // [m]
    Matrix4f proj;

    Ray PlaneGetLRTB(f32 fov_aspect, f32 sign) {
        Ray plane = {};
        plane.dir = { dist_far, 0, dist_far * sin(fov_aspect / 2 * deg2rad)};
        plane.dir.Normalize();
        plane.dir.x *= sign;
        return plane;
    }

    Ray PlaneRight() {
        return PlaneGetLRTB(fov * aspect, 1);
    }

    Ray PlaneLeft() {
        return PlaneGetLRTB(fov * aspect, -1);
    }

    Ray PlaneTop() {
        return PlaneGetLRTB(fov, 1);
    }

    Ray PlaneBottom() {
        return PlaneGetLRTB(fov, -1);
    }

    Ray PlaneNear() {
        Ray plane = {};
        plane.pos.z = dist_near;
        plane.dir.z = 1;
        return plane;
    }

    Ray PlaneFar() {
        Ray plane = {};
        plane.pos.z = dist_far;
        plane.dir.z = -1;
        return plane;
    }
};


void PerspectiveSetAspectAndP(Perspective *proj, u32 width = 0, u32 height = 0) {
    if (width != 0 && height != 0) {
        f32 aspect_new = width / (f32) height;

        if (aspect_new != proj->aspect) {
            proj->aspect = aspect_new;
            proj->proj = PerspectiveMatrixOpenGL(proj->dist_near, proj->dist_far, proj->fov, proj->aspect, false, true, false);
        }
    }
}

Perspective ProjectionInit(u32 width, u32 height) {
    Perspective proj = {};
    proj.fov = 90;
    proj.dist_near = 0.01f;
    proj.dist_far = 10.0f;
    PerspectiveSetAspectAndP(&proj, width, height);

    return proj;
}


//
// Plane / Line / Point / Triangle Helpers


bool PointSideOfPlane(Vector3f point, Ray plane) {
    // returns true if point is in the R3-halfspace defined by plane normal

    Vector3f diff = (plane.pos - point);
    diff.Normalize();
    f32 cos_angle = diff.Dot(plane.dir);

    if (cos_angle <= 0) {
        return true;
    }
    else {
        return false;
    }
}

Vector3f RayPlaneIntersect(Ray ray, Vector3f plane_origo, Vector3f plane_normal, f32 *t_at = NULL) {
    f32 dot = plane_normal.Dot(ray.dir);
    if (abs(dot) > 0.0001f) {
        f32 t = (plane_origo - ray.pos).Dot(plane_normal) / dot;
        if (t_at) {
            *t_at = t;
        }

        Vector3f result = ray.pos + t * ray.dir;
        return result;
    }
    else {
        return {};
    }
}

Vector3f PointToLine(Vector3f point, Vector3f line_origo, Vector3f line_direction) {
    Vector3f diff = point - line_origo;
    f32 coeff = diff.Dot(line_direction);
    Vector3f proj = line_origo + coeff * line_direction;

    return proj;
}

f32 PointToLineDist(Vector3f point, Vector3f line_origo, Vector3f line_direction) {
    Vector3f diff = point - line_origo;
    f32 coeff = diff.Dot(line_direction);
    Vector3f proj = line_origo + coeff * line_direction;

    return proj.Norm();
}

bool PerpendicularUnitVectors(Vector3f v1, Vector3f v2) {
    bool perpendicular = abs(v1.Dot(v2) - 1) > 0.000f;

    return perpendicular;
}

bool LineToLineDist(Vector3f line1_origo, Vector3f line1_dir, Vector3f line2_origo, Vector3f line2_dir, f32 *dist) {
    if (PerpendicularUnitVectors(line1_dir, line2_dir)) {
        if (dist) {
            *dist = PointToLineDist(line1_origo, line2_origo, line2_dir);
        }

        return false;
    }
    else {
        Vector3f n = line1_dir.Cross(line2_dir);
        n.Normalize();
        if (dist) {
            *dist = n.Dot(line1_origo - line2_origo);
        }

        return true;
    }
}

Vector3f PointToPlane(Vector3f point, Vector3f plane_origo, Vector3f plane_normal) {
    Vector3f delta = point - plane_origo;
    f32 dot = delta.Dot(plane_normal);
    Vector3f result = point - dot * plane_normal;

    return result;
}


bool RayCastTriangle(Ray r, Vector3f v1, Vector3f v2, Vector3f v3, Vector3f *hit)
{
    Vector3f plane_hit = RayPlaneIntersect(r, v1, (v2 - v1).Cross(v3 - v1));

    Vector3f v1h = plane_hit - v1;
    Vector3f v2h = plane_hit - v2;
    Vector3f v3h = plane_hit - v3;
    v1h.Normalize();
    v2h.Normalize();
    v3h.Normalize();

    float a1 = acos(v1h.Dot(v2h));
    float a2 = acos(v2h.Dot(v3h));
    float a3 = acos(v3h.Dot(v1h));

    bool did_hit = abs(a1 + a2 + a3 - 2 * PI) < 0.0001f;
    if (did_hit && hit) {
        *hit = plane_hit;
    }
    return did_hit;
}


//
// Utility


void PrintTransform(Matrix4f m) {
    printf("[");
    for (int i = 0; i < 4; ++i) {
        if (i > 0) {
            printf(" ");
        }
        for (int j = 0; j < 4; ++j) {
            printf(" %f", m.m[i][j]);
        }
        if (i < 3) {
            printf("\n");
        }
    }
    printf(" ]\n\n");
}

void PopulateMatrixRandomly(Matrix4f *m) {
    RandInit();
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m->m[i][j] = RandMinMaxI_f32(0, 9);
        }
    }
}


#endif


#ifndef __CAMERA_H__
#define __CAMERA_H__


Ray CameraGetRayWorld(Matrix4f view, f32 fov, f32 aspect, f32 x_frac = 0, f32 y_frac = 0) {
    // get the shoot-ray from the camera in world coordinates

    f32 fov2 = sin(deg2rad * fov * 0.5f);
    Vector3f dir = {};
    dir.x = - 2.0f * fov2 * x_frac;
    dir.y = - 2.0f * fov2 / aspect * y_frac;
    dir.z = 1;
    dir.Normalize();

    Ray shoot = {};
    shoot.pos = TransformPoint(view, Vector3f_Zero());
    shoot.dir = TransformDirection(view, dir);

    return shoot;
}

Vector3f CameraGetPointAtDepth(Matrix4f view, f32 fov, f32 aspect, Vector3f at_depth, f32 x_frac = 0, f32 y_frac = 0) {
    f32 depth_loc = TransformInversePoint(view, at_depth).z;
    Vector3f plane_origo = { 0.0f, 0.0f, depth_loc };
    Vector3f plane_normal = { 0.0f, 0.0f, 1.0f };
    Vector3f world = RayPlaneIntersect(CameraGetRayWorld(view, fov, aspect, x_frac, y_frac), TransformPoint(view, plane_origo), TransformPoint(view, plane_normal));

    return world;
}

struct OrbitCamera {
    f32 theta;
    f32 phi;
    f32 phi_loc;
    f32 radius;
    f32 mouse2rot = 0.4f;
    f32 mouse2pan = 0.01f;
    Matrix4f view;

    // pan
    Vector3f drag_anchor;
    Vector3f center_anchor;
    Matrix4f view_anchor;
    bool drag;

    void SetRelativeTo(Matrix4f transform, f32 radius = 0) {
        if (radius > 0) {
            this->radius = radius;
        }

        Update( TransformGetTranslation(transform) );

        Vector3f x_rot = TransformDirection(transform, x_hat);
        x_rot.y = 0;
        x_rot.Normalize();

        f32 phi_loc_new = acos(x_rot.x) * rad2deg;
        f32 phi_delpha = -1 * (phi_loc_new - phi_loc);

        phi += phi_delpha;
        phi_loc = phi_loc_new;
    }

    Vector3f Position() {
        Vector3f position = TransformGetTranslation(view);
        return position;
    }

    Vector3f Center() {
        Vector3f position = TransformGetTranslation(view);
        Vector3f cam_forward_w = TransformDirection(view, z_hat);
        Vector3f center = position + radius * cam_forward_w;
        return center;
    }

    void Update(Vector3f center) {
        Vector3f campos_relative = SphericalCoordsY(theta*deg2rad, phi*deg2rad, radius);
        view = TransformBuildTranslationOnly(center + campos_relative) * TransformBuildLookRotationYUp(center, center + campos_relative);
    }
};

OrbitCamera OrbitCameraInit() {
    OrbitCamera cam = {};

    cam.theta = 60;
    cam.phi = 35;
    cam.radius = 4;

    cam.view = TransformBuildOrbitCam(Vector3f_Zero(), cam.theta, cam.phi, cam.radius);
    return cam;
}

inline f32 _ScrollMult(f32 value) {
    if (value == 0) {
        return 1.0;
    }
    else {
        return sqrt(abs(value));
    }
}

Vector3f CameraGetPointInPlane(Matrix4f view, f32 fov, f32 aspect, Vector3f plane_origo_w, Vector3f plane_normal_w, f32 x_frac = 0, f32 y_frac = 0) {
    Ray m_w = CameraGetRayWorld(view, fov, aspect, x_frac, y_frac);
    Vector3f hit_w = RayPlaneIntersect(m_w, plane_origo_w, plane_normal_w);

    return hit_w;
}

static f32 _ClampTheta(f32 theta_degs, f32 min = 0.0001f, f32 max = 180 - 0.0001f) {
    f32 clamp_up = MinF32(theta_degs, max);
    f32 result = MaxF32(clamp_up, min);
    return result;
}

void OrbitCameraRotateZoom(OrbitCamera *cam, f32 dx, f32 dy, bool do_rotate, f32 scroll_y_offset) {
    Vector3f initial_center = cam->Center();
    if (do_rotate) {
        cam->theta = _ClampTheta(cam->theta - dy * cam->mouse2rot);
        cam->phi += - dx * cam->mouse2rot;
    }
    else if (scroll_y_offset < 0) {
        f32 mult = _ScrollMult(scroll_y_offset);
        cam->radius *= 1.1f * mult;
    }
    else if (scroll_y_offset > 0) {
        f32 mult = _ScrollMult(scroll_y_offset);
        cam->radius /= 1.1f * mult;
    }
    cam->Update(initial_center);
}

void OrbitCameraPanInPlane(OrbitCamera *cam, f32 fov, f32 aspect, f32 cursor_x_frac, f32 cursor_y_frac, bool enable, bool disable) {
    Vector3f plane_origo_w = { 0, 0, 0 };
    Vector3f plane_normal_w = { 0, 1, 0 };

    if (disable) {
        cam->view_anchor = {};
        cam->drag_anchor = {};
        cam->center_anchor = {};
        cam->drag = false;
    }
    else if (enable) {
        cam->view_anchor = cam->view;
        cam->center_anchor = cam->Center();
        cam->drag_anchor = CameraGetPointInPlane(cam->view_anchor, fov, aspect, plane_origo_w, plane_normal_w, cursor_x_frac, cursor_y_frac);
        cam->drag = true;
    }
    else if (cam->drag == true) {
        Vector3f cam_drag = CameraGetPointInPlane(cam->view_anchor, fov, aspect, plane_origo_w, plane_normal_w, cursor_x_frac, cursor_y_frac);
        Vector3f new_center = cam->center_anchor - (cam_drag - cam->drag_anchor);
        cam->Update(new_center);
    }
}


#endif


#ifndef __SCENEGRAPH_H__
#define __SCENEGRAPH_H__


struct Transform;


struct SceneGraphHandle {
    MPool pool;
    Transform *root;
};


struct Transform {
    Matrix4f t_loc;
    Matrix4f t_world;
    u16 next;
    u16 first;
    u16 parent;
    u16 index;

    inline
    Transform *Next(SceneGraphHandle *sg) {
        return (Transform*) PoolIdx2Ptr(&sg->pool, next);
    }

    inline
    Transform *First(SceneGraphHandle *sg) {
        return (Transform*) PoolIdx2Ptr(&sg->pool, first);
    }

    inline
    Transform *Parent(SceneGraphHandle *sg) {
        if (parent) {
            return (Transform*) PoolIdx2Ptr(&sg->pool, parent);
        }
        else{
            return sg->root;
        }
    }

    void AppendChild(SceneGraphHandle *sg, Transform *c) {
        if (first == 0) {
            first = c->index;
        }
        else {
            Transform *n = First(sg);
            while (n->next) {
                n = n->Next(sg);
            }
            n->next = c->index;
        }
        c->parent = index;
    }

    void RemoveChild(SceneGraphHandle *sg, Transform *t) {
        if (first == t->index) {
            first = t->next;
        }
        else {
            // find prev
            Transform *prev = NULL;
            Transform *c = First(sg);

            while (c) {
                if (c->index == t->index && prev) {
                    assert(c == t);

                    prev->next = t->next;
                }
                else {
                    prev = c;
                    c = c->Next(sg);
                }
            }
        }
    }
};


SceneGraphHandle SceneGraphInit(MArena *a_dest, s32 cap = 256) {
    SceneGraphHandle sg = {};

    sg.pool = PoolCreate(a_dest, sizeof(Transform), cap + 1);
    // root at index-0
    sg.root = (Transform*) PoolAlloc(&sg.pool);
    sg.root->t_loc = Matrix4f_Identity();
    sg.root->t_world = Matrix4f_Identity();

    return sg;
}

Transform *SceneGraphAlloc(SceneGraphHandle *sg, Transform *parent = NULL) {
    Transform *t = (Transform*) PoolAlloc(&sg->pool);
    t->index = (u16) PoolPtr2Idx(&sg->pool, t);
    t->t_loc = Matrix4f_Identity();
    t->t_world = Matrix4f_Identity();

    assert(t->index != 0);

    if (!parent) {
        parent = sg->root;
    }
    parent->AppendChild(sg, t);

    return t;
}

void SceneGraphFree(SceneGraphHandle *sg, Transform *t) {
    t->Parent(sg)->RemoveChild(sg, t);

    // relinquish child branches -> to root
    Transform *c = t->First(sg);
    Transform *nxt = c;
    while (nxt) {
        c = nxt;
        nxt = c->Next(sg);

        c->next = 0;
        sg->root->AppendChild(sg, c);
    }

    PoolFree(&sg->pool, t);
}

void SGUpdateRec(SceneGraphHandle *sg, Transform *t, Transform *p) {
    while (t) {
        t->t_world = p->t_world * t->t_loc;

        // iterate children
        if (t->first) {
            SGUpdateRec(sg, t->First(sg), t);
        }

        // iterate siblings
        t = t->Next(sg);
    }
}

void SceneGraphUpdate(SceneGraphHandle *sg) {
    Transform *r = sg->root;

    // initialize the starting point
    r->t_world = r->t_loc;

    // walk the tree
    if (r->first) {
        SGUpdateRec(sg, r->First(sg), r);
    }
}

void SceneGraphSetRotParent(SceneGraphHandle *sg, Transform *t, Transform *p_rot) {
    // p_rot is the rotational parent
    // t's parent pointer is the proper parent, whose translation is to be applied

    // we need the world matrices of p_rot and p:
    // (Because p_rot has an accumulated rotation above it, which we need to bake into our local matrix)
    // NOTE: Possibly, the rot-parent could be baked into the SceneGraphUpdate call, possibly.
    SceneGraphUpdate(sg);

    // our world translation matrix
    Vector3f our_w_transl_v3 = TransformGetTranslation(t->t_world);
    Matrix4f our_w_transl = TransformBuildTranslation(our_w_transl_v3);

    // our local rotation matrix
    Matrix4f our_l_rot = TransformSetTranslation(t->t_loc, { 0, 0, 0 });

    // the rotational parent's world (accumulated) rotation: Its world matrix with translation set to zero.
    Matrix4f prot_w_rot = p_rot->t_world;
    prot_w_rot = TransformSetTranslation(prot_w_rot, {0, 0, 0} );

    // our world rotation matrix
    Matrix4f our_w_rot = prot_w_rot * our_l_rot;

    // our world matrix is found by combining our translation and rotation matrices: 
    t->t_world = our_w_transl * our_w_rot;

    // recover our local matrix wrt. the primary "at-rel" parent (p, not p_rot)
    Matrix4f w_to_p = TransformGetInverse( t->Parent(sg)->t_world );
    t->t_loc = w_to_p * t->t_world;
}


#endif


#ifndef __CBUI_H__
#define __CBUI_H__


#define CBUI_VERSION_MAJOR 0
#define CBUI_VERSION_MINOR 2
#define CBUI_VERSION_PATCH 5


void CbuiAssertVersion(u32 major, u32 minor, u32 patch) {
    if (
        CBUI_VERSION_MAJOR != major ||
        CBUI_VERSION_MINOR != minor ||
        CBUI_VERSION_PATCH != patch
    ) {
        assert(1 == 0 && "cbui version check failed");
    }
}

void CbuiPrintVersion() {
    printf("%d.%d.%d\n", CBUI_VERSION_MAJOR, CBUI_VERSION_MINOR, CBUI_VERSION_PATCH);
}


#endif


#ifndef __COLOR_H__
#define __COLOR_H__


#define COLOR_RED (( Color { 255, 0, 0, 255 } ))
#define COLOR_GREEN (( Color { 0, 255, 0, 255 } ))
#define COLOR_GREEN_50 (( Color { 0, 128, 0, 255 } ))
#define COLOR_YELLOW (( Color { 255, 255, 0, 255 } ))
#define COLOR_YELLOW2 (( Color { 240, 240, 50, 255 } ))
#define COLOR_BLUE (( Color {  0, 0, 255, 255 } ))
#define COLOR_BLACK (( Color { 0, 0, 0, 255 } ))
#define COLOR_WHITE (( Color { 255, 255, 255, 255 } ))
#define COLOR_GRAY (( Color { 128, 128, 128, 255 } ))
#define COLOR_GRAY_20 (( Color { 50, 50, 50, 255 } ))
#define COLOR_GRAY_30 (( Color { 83, 83, 83, 255 } ))
#define COLOR_GRAY_50 (( Color { 128, 128, 128, 255 } ))
#define COLOR_GRAY_60 (( Color { 150, 150, 150, 255 } ))
#define COLOR_GRAY_75 (( Color { 192, 192, 192, 255 } ))
#define COLOR_GRAY_80 (( Color { 217, 217, 217, 255 } ))


//
// these arrays took how many bottles of water to convert from python into C-style?


u8 colormap_paletted_jet[64][4] = {
    {  0,   0, 143, 255}, {  0,   0, 159, 255}, {  0,   0, 175, 255}, {  0,   0, 191, 255},
    {  0,   0, 207, 255}, {  0,   0, 223, 255}, {  0,   0, 239, 255}, {  0,   0, 255, 255},
    {  0,  16, 255, 255}, {  0,  32, 255, 255}, {  0,  48, 255, 255}, {  0,  64, 255, 255},
    {  0,  80, 255, 255}, {  0,  96, 255, 255}, {  0, 112, 255, 255}, {  0, 128, 255, 255},
    {  0, 143, 255, 255}, {  0, 159, 255, 255}, {  0, 175, 255, 255}, {  0, 191, 255, 255},
    {  0, 207, 255, 255}, {  0, 223, 255, 255}, {  0, 239, 255, 255}, {  0, 255, 255, 255},
    { 16, 255, 239, 255}, { 32, 255, 223, 255}, { 48, 255, 207, 255}, { 64, 255, 191, 255},
    { 80, 255, 175, 255}, { 96, 255, 159, 255}, {112, 255, 143, 255}, {128, 255, 128, 255},
    {143, 255, 112, 255}, {159, 255,  96, 255}, {175, 255,  80, 255}, {191, 255,  64, 255},
    {207, 255,  48, 255}, {223, 255,  32, 255}, {239, 255,  16, 255}, {255, 255,   0, 255},
    {255, 239,   0, 255}, {255, 223,   0, 255}, {255, 207,   0, 255}, {255, 191,   0, 255},
    {255, 175,   0, 255}, {255, 159,   0, 255}, {255, 143,   0, 255}, {255, 128,   0, 255},
    {255, 112,   0, 255}, {255,  96,   0, 255}, {255,  80,   0, 255}, {255,  64,   0, 255},
    {255,  48,   0, 255}, {255,  32,   0, 255}, {255,  16,   0, 255}, {255,   0,   0, 255},
    {239,   0,   0, 255}, {223,   0,   0, 255}, {207,   0,   0, 255}, {191,   0,   0, 255},
    {175,   0,   0, 255}, {159,   0,   0, 255}, {143,   0,   0, 255}, {128,   0,   0, 255}
};

u8 colormap_paletted_autumn[64][4] = {
    {255,   0,   0, 255}, {255,   4,   0, 255}, {255,   8,   0, 255}, {255,  12,   0, 255},
    {255,  16,   0, 255}, {255,  20,   0, 255}, {255,  24,   0, 255}, {255,  28,   0, 255},
    {255,  32,   0, 255}, {255,  36,   0, 255}, {255,  40,   0, 255}, {255,  45,   0, 255},
    {255,  49,   0, 255}, {255,  53,   0, 255}, {255,  57,   0, 255}, {255,  61,   0, 255},
    {255,  65,   0, 255}, {255,  69,   0, 255}, {255,  73,   0, 255}, {255,  77,   0, 255},
    {255,  81,   0, 255}, {255,  85,   0, 255}, {255,  89,   0, 255}, {255,  93,   0, 255},
    {255,  97,   0, 255}, {255, 101,   0, 255}, {255, 105,   0, 255}, {255, 109,   0, 255},
    {255, 113,   0, 255}, {255, 117,   0, 255}, {255, 121,   0, 255}, {255, 125,   0, 255},
    {255, 130,   0, 255}, {255, 134,   0, 255}, {255, 138,   0, 255}, {255, 142,   0, 255},
    {255, 146,   0, 255}, {255, 150,   0, 255}, {255, 154,   0, 255}, {255, 158,   0, 255},
    {255, 162,   0, 255}, {255, 166,   0, 255}, {255, 170,   0, 255}, {255, 174,   0, 255},
    {255, 178,   0, 255}, {255, 182,   0, 255}, {255, 186,   0, 255}, {255, 190,   0, 255},
    {255, 194,   0, 255}, {255, 198,   0, 255}, {255, 202,   0, 255}, {255, 206,   0, 255},
    {255, 210,   0, 255}, {255, 215,   0, 255}, {255, 219,   0, 255}, {255, 223,   0, 255},
    {255, 227,   0, 255}, {255, 231,   0, 255}, {255, 235,   0, 255}, {255, 239,   0, 255},
    {255, 243,   0, 255}, {255, 247,   0, 255}, {255, 251,   0, 255}, {255, 255,   0, 255}
};


/*
        'jet'  : np.array([[  0,   0, 143, 255], [  0,   0, 159, 255], [  0,   0, 175, 255], [  0,   0, 191, 255], [  0,   0, 207, 255], [  0,   0, 223, 255], [  0,   0, 239, 255], [  0,   0, 255, 255], [  0,  16, 255, 255], [  0,  32, 255, 255], [  0,  48, 255, 255], [  0,  64, 255, 255], [  0,  80, 255, 255], [  0,  96, 255, 255], [  0, 112, 255, 255], [  0, 128, 255, 255], [  0, 143, 255, 255], [  0, 159, 255, 255], [  0, 175, 255, 255], [  0, 191, 255, 255], [  0, 207, 255, 255], [  0, 223, 255, 255], [  0, 239, 255, 255], [  0, 255, 255, 255], [ 16, 255, 239, 255], [ 32, 255, 223, 255], [ 48, 255, 207, 255], [ 64, 255, 191, 255], [ 80, 255, 175, 255], [ 96, 255, 159, 255], [112, 255, 143, 255], [128, 255, 128, 255], [143, 255, 112, 255], [159, 255,  96, 255], [175, 255,  80, 255], [191, 255,  64, 255], [207, 255,  48, 255], [223, 255,  32, 255], [239, 255,  16, 255], [255, 255,   0, 255], [255, 239,   0, 255], [255, 223,   0, 255], [255, 207,   0, 255], [255, 191,   0, 255], [255, 175,   0, 255], [255, 159,   0, 255], [255, 143,   0, 255], [255, 128,   0, 255], [255, 112,   0, 255], [255,  96,   0, 255], [255,  80,   0, 255], [255,  64,   0, 255], [255,  48,   0, 255], [255,  32,   0, 255], [255,  16,   0, 255], [255,   0,   0, 255], [239,   0,   0, 255], [223,   0,   0, 255], [207,   0,   0, 255], [191,   0,   0, 255], [175,   0,   0, 255], [159,   0,   0, 255], [143,   0,   0, 255], [128,   0,   0, 255]], dtype=np.ubyte),
        'autumn'  : np.array([[255,   0,   0, 255], [255,   4,   0, 255], [255,   8,   0, 255], [255,  12,   0, 255], [255,  16,   0, 255], [255,  20,   0, 255], [255,  24,   0, 255], [255,  28,   0, 255], [255,  32,   0, 255], [255,  36,   0, 255], [255,  40,   0, 255], [255,  45,   0, 255], [255,  49,   0, 255], [255,  53,   0, 255], [255,  57,   0, 255], [255,  61,   0, 255], [255,  65,   0, 255], [255,  69,   0, 255], [255,  73,   0, 255], [255,  77,   0, 255] , [255,  81,   0, 255], [255,  85,   0, 255], [255,  89,   0, 255], [255,  93,   0, 255], [255,  97,   0, 255], [255, 101,   0, 255], [255, 105,   0, 255], [255, 109,   0, 255], [255, 113,   0, 255], [255, 117,   0, 255], [255, 121,   0, 255], [255, 125,   0, 255], [255, 130,   0, 255], [255, 134,   0, 255], [255, 138,   0, 255], [255, 142,   0, 255], [255, 146,   0, 255], [255, 150,   0, 255], [255, 154,   0, 255], [255, 158,   0, 255], [255, 162,   0, 255], [255, 166,   0, 255], [255, 170,   0, 255], [255, 174,   0, 255], [255, 178,   0, 255], [255, 182,   0, 255], [255, 186,   0, 255], [255, 190,   0, 255], [255, 194,   0, 255], [255, 198,   0, 255], [255, 202,   0, 255], [255, 206,   0, 255], [255, 210,   0, 255], [255, 215,   0, 255], [255, 219,   0, 255], [255, 223,   0, 255], [255, 227,   0, 255], [255, 231,   0, 255], [255, 235,   0, 255], [255, 239,   0, 255], [255, 243,   0, 255], [255, 247,   0, 255], [255, 251,   0, 255], [255, 255,   0, 255]], dtype=np.ubyte),

// remaining maps to convert:

        'bone'  : np.array([[  0,   0,   1, 255], [  4,   4,   6, 255], [  7,   7,  11, 255], [ 11,  11,  16, 255], [ 14,  14,  21, 255], [ 18,  18,  26, 255], [ 21,  21,  31, 255], [ 25,  25,  35, 255], [ 28,  28,  40, 255], [ 32,  32,  45, 255], [ 35,  35,  50, 255], [ 39,  39,  55, 255], [ 43,  43,  60, 255], [ 46,  46,  65, 255], [ 50,  50,  70, 255], [ 53,  53,  74, 255], [ 57,  57,  79, 255], [ 60,  60,  84, 255], [ 64,  64,  89, 255], [ 67,  67,  94, 255], [ 71,  71,  99, 255], [ 74,  74, 104, 255], [ 78,  78, 108, 255], [ 81,  81, 113, 255], [ 85,  86, 117, 255], [ 89,  91, 120, 255], [ 92,  96, 124, 255], [ 96, 101, 128, 255], [ 99, 106, 131, 255], [103, 111, 135, 255], [106, 116, 138, 255], [110, 120, 142, 255], [113, 125, 145, 255], [117, 130, 149, 255], [120, 135, 152, 255], [124, 140, 156, 255], [128, 145, 159, 255], [131, 150, 163, 255], [135, 155, 166, 255], [138, 159, 170, 255], [142, 164, 174, 255], [145, 169, 177, 255], [149, 174, 181, 255], [152, 179, 184, 255], [156, 184, 188, 255], [159, 189, 191, 255], [163, 193, 195, 255], [166, 198, 198, 255], [172, 202, 202, 255], [178, 205, 205, 255], [183, 209, 209, 255], [189, 213, 213, 255], [194, 216, 216, 255], [200, 220, 220, 255], [205, 223, 223, 255], [211, 227, 227, 255], [216, 230, 230, 255], [222, 234, 234, 255], [227, 237, 237, 255], [233, 241, 241, 255], [238, 244, 244, 255], [244, 248, 248, 255], [249, 251, 251, 255], [255, 255, 255, 255]], dtype=np.ubyte),
        'colorcube'  : np.array([[ 85,  85,   0, 255], [ 85, 170,   0, 255], [ 85, 255,   0, 255], [170,  85,   0, 255], [170, 170,   0, 255], [170, 255,   0, 255], [255,  85,   0, 255], [255, 170,   0, 255], [255, 255,   0, 255], [  0,  85, 128, 255], [  0, 170, 128, 255], [  0, 255, 128, 255], [ 85,   0, 128, 255], [ 85,  85, 128, 255], [ 85, 170, 128, 255], [ 85, 255, 128, 255], [170,   0, 128, 255], [170,  85, 128, 255], [170, 170, 128, 255], [170, 255, 128, 255], [255,   0, 128, 255], [255,  85, 128, 255], [255, 170, 128, 255], [255, 255, 128, 255], [  0,  85, 255, 255], [  0, 170, 255, 255], [  0, 255, 255, 255], [ 85,   0, 255, 255], [ 85,  85, 255, 255], [ 85, 170, 255, 255], [ 85, 255, 255, 255], [170,   0, 255, 255], [170,  85, 255, 255], [170, 170, 255, 255], [170, 255, 255, 255], [255,   0, 255, 255], [255,  85, 255, 255], [255, 170, 255, 255], [ 43,   0,   0, 255], [ 85,   0,   0, 255], [128,   0,   0, 255], [170,   0,   0, 255], [213,   0,   0, 255], [255,   0,   0, 255], [  0,  43,   0, 255], [  0,  85,   0, 255], [  0, 128,   0, 255], [  0, 170,   0, 255], [  0, 213,   0, 255], [  0, 255,   0, 255], [  0,   0,  43, 255], [  0,   0,  85, 255], [  0,   0, 128, 255], [  0,   0, 170, 255], [  0,   0, 213, 255], [  0,   0, 255, 255], [  0,   0,   0, 255], [ 36,  36,  36, 255], [ 73,  73,  73, 255], [109, 109, 109, 255], [146, 146, 146, 255], [182, 182, 182, 255], [219, 219, 219, 255], [255, 255, 255, 255]], dtype=np.ubyte),
        'cool'  : np.array([[  0, 255, 255, 255], [  4, 251, 255, 255], [  8, 247, 255, 255], [ 12, 243, 255, 255], [ 16, 239, 255, 255], [ 20, 235, 255, 255], [ 24, 231, 255, 255], [ 28, 227, 255, 255], [ 32, 223, 255, 255], [ 36, 219, 255, 255], [ 40, 215, 255, 255], [ 45, 210, 255, 255], [ 49, 206, 255, 255], [ 53, 202, 255, 255], [ 57, 198, 255, 255], [ 61, 194, 255, 255], [ 65, 190, 255, 255], [ 69, 186, 255, 255], [ 73, 182, 255, 255], [ 77, 178, 255, 255], [ 81, 174, 255, 255], [ 85, 170, 255, 255], [ 89, 166, 255, 255], [ 93, 162, 255, 255], [ 97, 158, 255, 255], [101, 154, 255, 255], [105, 150, 255, 255], [109, 146, 255, 255], [113, 142, 255, 255], [117, 138, 255, 255], [121, 134, 255, 255], [125, 130, 255, 255], [130, 125, 255, 255], [134, 121, 255, 255], [138, 117, 255, 255], [142, 113, 255, 255], [146, 109, 255, 255], [150, 105, 255, 255], [154, 101, 255, 255], [158,  97, 255, 255], [162,  93, 255, 255], [166,  89, 255, 255], [170,  85, 255, 255], [174,  81, 255, 255], [178,  77, 255, 255], [182,  73, 255, 255], [186,  69, 255, 255], [190,  65, 255, 255], [194,  61, 255, 255], [198,  57, 255, 255], [202,  53, 255, 255], [206,  49, 255, 255], [210,  45, 255, 255], [215,  40, 255, 255], [219,  36, 255, 255], [223,  32, 255, 255], [227,  28, 255, 255], [231,  24, 255, 255], [235,  20, 255, 255], [239,  16, 255, 255], [243,  12, 255, 255], [247,   8, 255, 255], [251,   4, 255, 255], [255,   0, 255, 255]], dtype=np.ubyte),
        'copper'  : np.array([[  0,   0,   0, 255], [  5,   3,   2, 255], [ 10,   6,   4, 255], [ 15,   9,   6, 255], [ 20,  13,   8, 255], [ 25,  16,  10, 255], [ 30,  19,  12, 255], [ 35,  22,  14, 255], [ 40,  25,  16, 255], [ 46,  28,  18, 255], [ 51,  32,  20, 255], [ 56,  35,  22, 255], [ 61,  38,  24, 255], [ 66,  41,  26, 255], [ 71,  44,  28, 255], [ 76,  47,  30, 255], [ 81,  51,  32, 255], [ 86,  54,  34, 255], [ 91,  57,  36, 255], [ 96,  60,  38, 255], [101,  63,  40, 255], [106,  66,  42, 255], [111,  70,  44, 255], [116,  73,  46, 255], [121,  76,  48, 255], [126,  79,  50, 255], [132,  82,  52, 255], [137,  85,  54, 255], [142,  89,  56, 255], [147,  92,  58, 255], [152,  95,  60, 255], [157,  98,  62, 255], [162, 101,  64, 255], [167, 104,  66, 255], [172, 108,  68, 255], [177, 111,  70, 255], [182, 114,  72, 255], [187, 117,  75, 255], [192, 120,  77, 255], [197, 123,  79, 255], [202, 126,  81, 255], [207, 130,  83, 255], [212, 133,  85, 255], [218, 136,  87, 255], [223, 139,  89, 255], [228, 142,  91, 255], [233, 145,  93, 255], [238, 149,  95, 255], [243, 152,  97, 255], [248, 155,  99, 255], [253, 158, 101, 255], [255, 161, 103, 255], [255, 164, 105, 255], [255, 168, 107, 255], [255, 171, 109, 255], [255, 174, 111, 255], [255, 177, 113, 255], [255, 180, 115, 255], [255, 183, 117, 255], [255, 187, 119, 255], [255, 190, 121, 255], [255, 193, 123, 255], [255, 196, 125, 255], [255, 199, 127, 255]], dtype=np.ubyte),
        'gray'  : np.array([[  0,   0,   0, 255], [  4,   4,   4, 255], [  8,   8,   8, 255], [ 12,  12,  12, 255], [ 16,  16,  16, 255], [ 20,  20,  20, 255], [ 24,  24,  24, 255], [ 28,  28,  28, 255], [ 32,  32,  32, 255], [ 36,  36,  36, 255], [ 40,  40,  40, 255], [ 45,  45,  45, 255], [ 49,  49,  49, 255], [ 53,  53,  53, 255], [ 57,  57,  57, 255], [ 61,  61,  61, 255], [ 65,  65,  65, 255], [ 69,  69,  69, 255], [ 73,  73,  73, 255], [ 77,  77,  77, 255], [ 81,  81,  81, 255], [ 85,  85,  85, 255], [ 89,  89,  89, 255], [ 93,  93,  93, 255], [ 97,  97,  97, 255], [101, 101, 101, 255], [105, 105, 105, 255], [109, 109, 109, 255], [113, 113, 113, 255], [117, 117, 117, 255], [121, 121, 121, 255], [125, 125, 125, 255], [130, 130, 130, 255], [134, 134, 134, 255], [138, 138, 138, 255], [142, 142, 142, 255], [146, 146, 146, 255], [150, 150, 150, 255], [154, 154, 154, 255], [158, 158, 158, 255], [162, 162, 162, 255], [166, 166, 166, 255], [170, 170, 170, 255], [174, 174, 174, 255], [178, 178, 178, 255], [182, 182, 182, 255], [186, 186, 186, 255], [190, 190, 190, 255], [194, 194, 194, 255], [198, 198, 198, 255], [202, 202, 202, 255], [206, 206, 206, 255], [210, 210, 210, 255], [215, 215, 215, 255], [219, 219, 219, 255], [223, 223, 223, 255], [227, 227, 227, 255], [231, 231, 231, 255], [235, 235, 235, 255], [239, 239, 239, 255], [243, 243, 243, 255], [247, 247, 247, 255], [251, 251, 251, 255], [255, 255, 255, 255]], dtype=np.ubyte),
        'hot'  : np.array([[ 11,   0,   0, 255], [ 21,   0,   0, 255], [ 32,   0,   0, 255], [ 43,   0,   0, 255], [ 53,   0,   0, 255], [ 64,   0,   0, 255], [ 74,   0,   0, 255], [ 85,   0,   0, 255], [ 96,   0,   0, 255], [106,   0,   0, 255], [117,   0,   0, 255], [128,   0,   0, 255], [138,   0,   0, 255], [149,   0,   0, 255], [159,   0,   0, 255], [170,   0,   0, 255], [181,   0,   0, 255], [191,   0,   0, 255], [202,   0,   0, 255], [213,   0,   0, 255], [223,   0,   0, 255], [234,   0,   0, 255], [244,   0,   0, 255], [255,   0,   0, 255], [255,  11,   0, 255], [255,  21,   0, 255], [255,  32,   0, 255], [255,  43,   0, 255], [255,  53,   0, 255], [255,  64,   0, 255], [255,  74,   0, 255], [255,  85,   0, 255], [255,  96,   0, 255], [255, 106,   0, 255], [255, 117,   0, 255], [255, 128,   0, 255], [255, 138,   0, 255], [255, 149,   0, 255], [255, 159,   0, 255], [255, 170,   0, 255], [255, 181,   0, 255], [255, 191,   0, 255], [255, 202,   0, 255], [255, 213,   0, 255], [255, 223,   0, 255], [255, 234,   0, 255], [255, 244,   0, 255], [255, 255,   0, 255], [255, 255,  16, 255], [255, 255,  32, 255], [255, 255,  48, 255], [255, 255,  64, 255], [255, 255,  80, 255], [255, 255,  96, 255], [255, 255, 112, 255], [255, 255, 128, 255], [255, 255, 143, 255], [255, 255, 159, 255], [255, 255, 175, 255], [255, 255, 191, 255], [255, 255, 207, 255], [255, 255, 223, 255], [255, 255, 239, 255], [255, 255, 255, 255]], dtype=np.ubyte),
        'hsv'  : np.array([[255,   0,   0, 255], [255,  24,   0, 255], [255,  48,   0, 255], [255,  72,   0, 255], [255,  96,   0, 255], [255, 120,   0, 255], [255, 143,   0, 255], [255, 167,   0, 255], [255, 191,   0, 255], [255, 215,   0, 255], [255, 239,   0, 255], [247, 255,   0, 255], [223, 255,   0, 255], [199, 255,   0, 255], [175, 255,   0, 255], [151, 255,   0, 255], [128, 255,   0, 255], [104, 255,   0, 255], [ 80, 255,   0, 255], [ 56, 255,   0, 255], [ 32, 255,   0, 255], [  8, 255,   0, 255], [  0, 255,  16, 255], [  0, 255,  40, 255], [  0, 255,  64, 255], [  0, 255,  88, 255], [  0, 255, 112, 255], [  0, 255, 135, 255], [  0, 255, 159, 255], [  0, 255, 183, 255], [  0, 255, 207, 255], [  0, 255, 231, 255], [  0, 255, 255, 255], [  0, 231, 255, 255], [  0, 207, 255, 255], [  0, 183, 255, 255], [  0, 159, 255, 255], [  0, 135, 255, 255], [  0, 112, 255, 255], [  0,  88, 255, 255], [  0,  64, 255, 255], [  0,  40, 255, 255], [  0,  16, 255, 255], [  8,   0, 255, 255], [ 32,   0, 255, 255], [ 56,   0, 255, 255], [ 80,   0, 255, 255], [104,   0, 255, 255], [128,   0, 255, 255], [151,   0, 255, 255], [175,   0, 255, 255], [199,   0, 255, 255], [223,   0, 255, 255], [247,   0, 255, 255], [255,   0, 239, 255], [255,   0, 215, 255], [255,   0, 191, 255], [255,   0, 167, 255], [255,   0, 143, 255], [255,   0, 120, 255], [255,   0,  96, 255], [255,   0,  72, 255], [255,   0,  48, 255], [255,   0,  24, 255]], dtype=np.ubyte),
        'parula'  : np.array([[ 53,  42, 135, 255], [ 54,  48, 147, 255], [ 54,  55, 160, 255], [ 53,  61, 173, 255], [ 50,  67, 186, 255], [ 44,  74, 199, 255], [ 32,  83, 212, 255], [ 15,  92, 221, 255], [  3,  99, 225, 255], [  2, 104, 225, 255], [  4, 109, 224, 255], [  8, 113, 222, 255], [ 13, 117, 220, 255], [ 16, 121, 218, 255], [ 18, 125, 216, 255], [ 20, 129, 214, 255], [ 20, 133, 212, 255], [ 19, 137, 211, 255], [ 16, 142, 210, 255], [ 12, 147, 210, 255], [  9, 152, 209, 255], [  7, 156, 207, 255], [  6, 160, 205, 255], [  6, 164, 202, 255], [  6, 167, 198, 255], [  7, 169, 194, 255], [ 10, 172, 190, 255], [ 15, 174, 185, 255], [ 21, 177, 180, 255], [ 29, 179, 175, 255], [ 37, 181, 169, 255], [ 46, 183, 164, 255], [ 56, 185, 158, 255], [ 66, 187, 152, 255], [ 77, 188, 146, 255], [ 89, 189, 140, 255], [101, 190, 134, 255], [113, 191, 128, 255], [124, 191, 123, 255], [135, 191, 119, 255], [146, 191, 115, 255], [156, 191, 111, 255], [165, 190, 107, 255], [174, 190, 103, 255], [183, 189, 100, 255], [192, 188,  96, 255], [200, 188,  93, 255], [209, 187,  89, 255], [217, 186,  86, 255], [225, 185,  82, 255], [233, 185,  78, 255], [241, 185,  74, 255], [248, 187,  68, 255], [253, 190,  61, 255], [255, 195,  55, 255], [254, 200,  50, 255], [252, 206,  46, 255], [250, 211,  42, 255], [247, 216,  38, 255], [245, 222,  33, 255], [245, 228,  29, 255], [245, 235,  24, 255], [246, 243,  19, 255], [249, 251,  14, 255]], dtype=np.ubyte),
        'pink'  : np.array([[ 30,   0,   0, 255], [ 50,  26,  26, 255], [ 64,  37,  37, 255], [ 75,  45,  45, 255], [ 85,  52,  52, 255], [ 94,  59,  59, 255], [102,  64,  64, 255], [110,  69,  69, 255], [117,  74,  74, 255], [123,  79,  79, 255], [130,  83,  83, 255], [136,  87,  87, 255], [141,  91,  91, 255], [147,  95,  95, 255], [152,  98,  98, 255], [157, 102, 102, 255], [162, 105, 105, 255], [167, 108, 108, 255], [172, 111, 111, 255], [176, 114, 114, 255], [181, 117, 117, 255], [185, 120, 120, 255], [189, 123, 123, 255], [194, 126, 126, 255], [195, 132, 129, 255], [197, 138, 131, 255], [199, 144, 134, 255], [201, 149, 136, 255], [202, 154, 139, 255], [204, 159, 141, 255], [206, 164, 144, 255], [207, 169, 146, 255], [209, 174, 148, 255], [211, 178, 151, 255], [212, 183, 153, 255], [214, 187, 155, 255], [216, 191, 157, 255], [217, 195, 160, 255], [219, 199, 162, 255], [220, 203, 164, 255], [222, 207, 166, 255], [223, 211, 168, 255], [225, 215, 170, 255], [226, 218, 172, 255], [228, 222, 174, 255], [229, 225, 176, 255], [231, 229, 178, 255], [232, 232, 180, 255], [234, 234, 185, 255], [235, 235, 191, 255], [237, 237, 196, 255], [238, 238, 201, 255], [240, 240, 206, 255], [241, 241, 211, 255], [243, 243, 216, 255], [244, 244, 221, 255], [245, 245, 225, 255], [247, 247, 230, 255], [248, 248, 234, 255], [250, 250, 238, 255], [251, 251, 243, 255], [252, 252, 247, 255], [254, 254, 251, 255], [255, 255, 255, 255]], dtype=np.ubyte),
        'spring'  : np.array([[255,   0, 255, 255], [255,   4, 251, 255], [255,   8, 247, 255], [255,  12, 243, 255], [255,  16, 239, 255], [255,  20, 235, 255], [255,  24, 231, 255], [255,  28, 227, 255], [255,  32, 223, 255], [255,  36, 219, 255], [255,  40, 215, 255], [255,  45, 210, 255], [255,  49, 206, 255], [255,  53, 202, 255], [255,  57, 198, 255], [255,  61, 194, 255], [255,  65, 190, 255], [255,  69, 186, 255], [255,  73, 182, 255], [255,  77, 178, 255], [255,  81, 174, 255], [255,  85, 170, 255], [255,  89, 166, 255], [255,  93, 162, 255], [255,  97, 158, 255], [255, 101, 154, 255], [255, 105, 150, 255], [255, 109, 146, 255], [255, 113, 142, 255], [255, 117, 138, 255], [255, 121, 134, 255], [255, 125, 130, 255], [255, 130, 125, 255], [255, 134, 121, 255], [255, 138, 117, 255], [255, 142, 113, 255], [255, 146, 109, 255], [255, 150, 105, 255], [255, 154, 101, 255], [255, 158,  97, 255], [255, 162,  93, 255], [255, 166,  89, 255], [255, 170,  85, 255], [255, 174,  81, 255], [255, 178,  77, 255], [255, 182,  73, 255], [255, 186,  69, 255], [255, 190,  65, 255], [255, 194,  61, 255], [255, 198,  57, 255], [255, 202,  53, 255], [255, 206,  49, 255], [255, 210,  45, 255], [255, 215,  40, 255], [255, 219,  36, 255], [255, 223,  32, 255], [255, 227,  28, 255], [255, 231,  24, 255], [255, 235,  20, 255], [255, 239,  16, 255], [255, 243,  12, 255], [255, 247,   8, 255], [255, 251,   4, 255], [255, 255,   0, 255]], dtype=np.ubyte),
        'summer'  : np.array([[  0, 128, 102, 255], [  4, 130, 102, 255], [  8, 132, 102, 255], [ 12, 134, 102, 255], [ 16, 136, 102, 255], [ 20, 138, 102, 255], [ 24, 140, 102, 255], [ 28, 142, 102, 255], [ 32, 144, 102, 255], [ 36, 146, 102, 255], [ 40, 148, 102, 255], [ 45, 150, 102, 255], [ 49, 152, 102, 255], [ 53, 154, 102, 255], [ 57, 156, 102, 255], [ 61, 158, 102, 255], [ 65, 160, 102, 255], [ 69, 162, 102, 255], [ 73, 164, 102, 255], [ 77, 166, 102, 255], [ 81, 168, 102, 255], [ 85, 170, 102, 255], [ 89, 172, 102, 255], [ 93, 174, 102, 255], [ 97, 176, 102, 255], [101, 178, 102, 255], [105, 180, 102, 255], [109, 182, 102, 255], [113, 184, 102, 255], [117, 186, 102, 255], [121, 188, 102, 255], [125, 190, 102, 255], [130, 192, 102, 255], [134, 194, 102, 255], [138, 196, 102, 255], [142, 198, 102, 255], [146, 200, 102, 255], [150, 202, 102, 255], [154, 204, 102, 255], [158, 206, 102, 255], [162, 208, 102, 255], [166, 210, 102, 255], [170, 212, 102, 255], [174, 215, 102, 255], [178, 217, 102, 255], [182, 219, 102, 255], [186, 221, 102, 255], [190, 223, 102, 255], [194, 225, 102, 255], [198, 227, 102, 255], [202, 229, 102, 255], [206, 231, 102, 255], [210, 233, 102, 255], [215, 235, 102, 255], [219, 237, 102, 255], [223, 239, 102, 255], [227, 241, 102, 255], [231, 243, 102, 255], [235, 245, 102, 255], [239, 247, 102, 255], [243, 249, 102, 255], [247, 251, 102, 255], [251, 253, 102, 255], [255, 255, 102, 255]], dtype=np.ubyte),
        'winter'  : np.array([[  0,   0, 255, 255], [  0,   4, 253, 255], [  0,   8, 251, 255], [  0,  12, 249, 255], [  0,  16, 247, 255], [  0,  20, 245, 255], [  0,  24, 243, 255], [  0,  28, 241, 255], [  0,  32, 239, 255], [  0,  36, 237, 255], [  0,  40, 235, 255], [  0,  45, 233, 255], [  0,  49, 231, 255], [  0,  53, 229, 255], [  0,  57, 227, 255], [  0,  61, 225, 255], [  0,  65, 223, 255], [  0,  69, 221, 255], [  0,  73, 219, 255], [  0,  77, 217, 255], [  0,  81, 215, 255], [  0,  85, 213, 255], [  0,  89, 210, 255], [  0,  93, 208, 255], [  0,  97, 206, 255], [  0, 101, 204, 255], [  0, 105, 202, 255], [  0, 109, 200, 255], [  0, 113, 198, 255], [  0, 117, 196, 255], [  0, 121, 194, 255], [  0, 125, 192, 255], [  0, 130, 190, 255], [  0, 134, 188, 255], [  0, 138, 186, 255], [  0, 142, 184, 255], [  0, 146, 182, 255], [  0, 150, 180, 255], [  0, 154, 178, 255], [  0, 158, 176, 255], [  0, 162, 174, 255], [  0, 166, 172, 255], [  0, 170, 170, 255], [  0, 174, 168, 255], [  0, 178, 166, 255], [  0, 182, 164, 255], [  0, 186, 162, 255], [  0, 190, 160, 255], [  0, 194, 158, 255], [  0, 198, 156, 255], [  0, 202, 154, 255], [  0, 206, 152, 255], [  0, 210, 150, 255], [  0, 215, 148, 255], [  0, 219, 146, 255], [  0, 223, 144, 255], [  0, 227, 142, 255], [  0, 231, 140, 255], [  0, 235, 138, 255], [  0, 239, 136, 255], [  0, 243, 134, 255], [  0, 247, 132, 255], [  0, 251, 130, 255], [  0, 255, 128, 255]], dtype=np.ubyte),
*/


Color ColorMapGet(f32 value, u8 colormap_paletted[64][4]) {
    if (value < 0) {
        value = 0;
    }
    else if (value > 1) {
        value = 1;
    }

    s32 idx_interp = round(value * (64 - 1));

    if (idx_interp < 0) {
        idx_interp = 0;
    }
    else if (idx_interp > 63) {
        idx_interp = 63;
    }

    Color *color = (Color*) &colormap_paletted[idx_interp];

    return *color;
}


#endif


#ifndef __SPRITE_H__
#define __SPRITE_H__


enum TextureType {
    TT_UNDEF,
    TT_RGBA,
    TT_8BIT,

    TT_COUNT
};

struct Texture {
    TextureType tpe;
    s32 width;
    s32 height;
    s32 px_sz;
    u8 *data;
};

struct Frame {
    s32 w;
    s32 h;
    f32 u0;
    f32 u1;
    f32 v0;
    f32 v1;
    f32 x0;
    f32 y0;
    f32 duration;
    Color color;
    u64 tex_id;
};

struct Animation {
    Str name;
    s32 top;
    s32 width;
    s32 height;
    Array<Frame> frames;
    Array<f32> durations;
};

struct SpriteSheet {
    Str name;
    Str filename;
    u64 texture_id;
    Array<Animation> animations;

    // helpers
    s32 top_accum;
    s32 tex_width;
    s32 tex_height;

    // TODO: strlist containing the animation names
};


static SpriteSheet *active_sheet;
static Animation *active_animation;


SpriteSheet *SS_Sheet(MArena *a_dest, HashMap *map_dest, HashMap *map_textures, Str filename, Str sheet_name, s32 data_width, s32 data_height, s32 animation_cnt) {
    assert(active_sheet == NULL);

    Texture *texture = (Texture*) ArenaAlloc(a_dest, sizeof(Texture));
    texture->tpe = TT_RGBA;
    texture->px_sz = 4;
    texture->width = data_width;
    texture->height = data_height;
    texture->data = (u8*) LoadFileFSeek(a_dest, filename);

    u64 texture_id = HashStringValue(sheet_name);
    MapPut(map_textures, texture_id, texture);

    active_sheet = (SpriteSheet*) ArenaAlloc(a_dest, sizeof(SpriteSheet));
    active_sheet->filename = StrPush(a_dest, filename);
    active_sheet->name = StrPush(a_dest, sheet_name);
    active_sheet->animations = InitArray<Animation>(a_dest, animation_cnt);
    active_sheet->texture_id = texture_id;
    active_sheet->tex_width = data_width;
    active_sheet->tex_height = data_height;
    MapPut(map_dest, active_sheet->name, active_sheet);

    return active_sheet;
}
SpriteSheet *SS_Sheet(MArena *a_dest, HashMap *map_dest, HashMap *map_textures, const char *filename, const char *sheet_name, s32 data_width, s32 data_height, s32 animation_cnt) {
    return SS_Sheet(a_dest, map_dest, map_textures, StrL(filename), StrL(sheet_name), data_width, data_height, animation_cnt);
}

void SS_Animation(MArena *a_dest, Str name, s32 width, s32 height, s32 frames_cnt) {
    assert(active_sheet);
    assert(active_animation == NULL);

    Animation animation = {};

    animation.name = StrPush(a_dest, name);
    animation.width = width;
    animation.height = height;
    animation.top = active_sheet->top_accum;
    animation.frames = InitArray<Frame>(a_dest, frames_cnt);
    animation.durations = InitArray<f32>(a_dest, frames_cnt);

    s32 y = animation.top;
    f32 v0 = y / (f32) height;
    f32 v1 = (y + height) / (f32) height;

    for (s32 i = 0; i < frames_cnt; ++i) {
        s32 x = i * width;

        Frame f = {};
        f.w = width;
        f.h = height;
        f.u0 = x / (f32) active_sheet->tex_width;
        f.u1 = (x + width) / (f32) active_sheet->tex_width;
        f.v0 = y / (f32) active_sheet->tex_height;
        f.v1 = (y + height) / (f32) active_sheet->tex_height;
        f.tex_id = active_sheet->texture_id;

        animation.frames.Add(f);
    }

    active_animation = active_sheet->animations.Add(animation);
    active_sheet->top_accum += height;

    // safeguard - close the sprite sheet config
    if (active_sheet->animations.len == active_sheet->animations.max) {
        active_sheet = NULL;
    }
}
void SS_Animation(MArena *a_dest, const char *name, s32 width, s32 height, s32 frames_cnt) {
    return SS_Animation(a_dest, StrL(name), width, height, frames_cnt);
}

void SS_FrameDuration(f32 duration) {
    assert(active_animation);

    active_animation->durations.Add(duration);

    // safeguard - close the animation config
    if (active_animation->durations.len == active_animation->durations.max) {
        active_animation = NULL;
    }
}

void SS_CloseSheet() {
    assert(active_sheet == NULL);
    assert(active_animation == NULL);
}


void SS_Print(SpriteSheet *sheet) {
    StrPrint("loaded sheet: ", sheet->name, "");
    StrPrint(" (", sheet->filename, ")\n");

    for (s32 i = 0; i < sheet->animations.len; ++i) {
        Animation a = sheet->animations.arr[i];
        StrPrint("    ", a.name, ": ");

        for (s32 i = 0; i < a.durations.len; ++i) {
            printf("%.0f ", a.durations.arr[i]);
        }
        printf("\n");
    }
}


static Frame frame_zero;
Frame GetAnimationFrame(HashMap *map, Str sheet_name, s32 animation_idx, s32 frame_idx, f32 *frame_duration) {
    assert(frame_duration);

    SpriteSheet *sheet = (SpriteSheet*) MapGet(map, sheet_name);
    if (sheet == NULL) {
        frame_zero = {};
        return frame_zero;
    }

    assert( StrEqual(sheet_name, sheet->name) );

    Animation animation = sheet->animations.arr[animation_idx];
    Frame result = animation.frames.arr[frame_idx];
    *frame_duration = animation.durations.arr[frame_idx];

    return result;
}


//
//  Legacy Sprite type


typedef Frame Sprite;


Sprite SpriteTexture_32it(MArena *a_dest, const char* name, s32 w, s32 h, f32 x0, f32 y0, HashMap *map_textures, Color **buffer_out) {
    assert(buffer_out);

    // 1) pushes an inlined Texture struct and a buffer to a_dest
    // 2) returns a sprite that connects to this buffer
    // 3) The sprite can be pushed to a list for defered blitting on top of the UI elements

    u64 key = HashStringValue(name);

    Texture _tex = {};
    Texture *tex = (Texture*) ArenaPush(a_dest, &_tex, sizeof(Texture));
    tex->tpe = TT_RGBA;
    tex->width = w;
    tex->height = h;
    tex->px_sz = 1;
    tex->data = (u8*) ArenaAlloc(a_dest, w * h * sizeof(Color));
    *buffer_out = (Color*) tex->data;

    memset(*buffer_out, 255, w*h*sizeof(Color));

    Sprite s = {};
    s.color = COLOR_RED;
    s.w = w;
    s.h = h;
    s.u0 = 0;
    s.u1 = 1;
    s.v0 = 0;
    s.v1 = 1;
    s.x0 = x0;
    s.y0 = y0;
    s.tex_id = key;    
    MapPut(map_textures, key, tex);

    return s;
}


void SpriteTextureFill(Sprite stex, Color *buffer, Color fill) {
    for (s32 j = 0; j < stex.h; j++) {
        for (s32 i = 0; i < stex.w; i++) {
            buffer[ stex.w * j + i ] = fill;
        }
    }
}


#endif


#ifndef __RASTER_H__
#define __RASTER_H__


//
//  Line rasterization
//


inline
bool _CullScreenCoords(u32 pos_x, u32 pos_y, u32 w, u32 h) {
    // returns true if the coordinate is out of range
    bool not_result = pos_x >= 0 && pos_x < w && pos_y >= 0 && pos_y < h;
    return !not_result;
}


Color SampleTexture(f32 x, f32 y, Color col_default, s32 src_width, s32 src_height, Color *src_buffer);


inline
Color BlendColors(Color background, Color color) {
    Color color_blended = {};
    if (color.a != 0) {
        f32 alpha = (1.0f * color.a) / 255;
        color_blended.r = (u8) (floor( alpha*color.r ) + floor( (1-alpha)*background.r ));
        color_blended.g = (u8) (floor( alpha*color.g ) + floor( (1-alpha)*background.g ));
        color_blended.b = (u8) (floor( alpha*color.b ) + floor( (1-alpha)*background.b ));
        color_blended.a = 255;
    }
    return color_blended;
}


void RenderLineRGBA(u8* image_buffer, u16 w, u16 h, s16 ax, s16 ay, s16 bx, s16 by, Color color) {

    // initially working from a to b
    // there are four cases:
    // 1: slope <= 1, ax < bx
    // 2: slope <= 1, ax > bx 
    // 3: slope > 1, ay < by
    // 4: slope > 1, ay > by 

    f32 slope_ab = (f32) (by - ay) / (bx - ax);
    Color *buff = (Color*) image_buffer;

    if (abs(slope_ab) <= 1) {
        // draw by x
        f32 slope = slope_ab;

        // swap?
        if (ax > bx) {
            u16 swapx = ax;
            u16 swapy = ay;

            ax = bx;
            ay = by;
            bx = swapx;
            by = swapy;
        }

        s16 x, y;
        u32 pix_idx;
        for (s32 i = 0; i <= bx - ax; ++i) {
            x = ax + i;
            y = ay + (s16) floor(slope * i);

            if (_CullScreenCoords(x, y, w, h)) {
                continue;
            }            

            pix_idx = x + y*w;
            buff[pix_idx] = BlendColors(buff[pix_idx], color);
        }
    }
    else {
        // draw by y
        f32 slope_inv = 1 / slope_ab;

        // swap a & b ?
        if (ay > by) {
            u16 swapx = ax;
            u16 swapy = ay;

            ax = bx;
            ay = by;
            bx = swapx;
            by = swapy;
        }

        s16 x, y;
        u32 pix_idx;
        for (u16 i = 0; i <= by - ay; ++i) {
            y = ay + i;
            x = ax + (s16) floor(slope_inv * i);

            if (_CullScreenCoords(x, y, w, h)) {
                continue;
            }

            pix_idx = x + y*w;
            buff[pix_idx] = BlendColors(buff[pix_idx], color);
        }
    }
}

inline
u32 GetXYIdx(f32 x, f32 y, u32 stride) {
    u32 idx = floor(x) + stride * floor(y);
    return idx;
}

void RenderPoint(u8 *image_buffer, Vector3f point_ndc, u32 w, u32 h, Color color = COLOR_RED) {
    f32 x = (point_ndc.x + 1) / 2 * w;
    f32 y = (point_ndc.y + 1) / 2 * h;
    ((Color*) image_buffer)[ GetXYIdx(x, y, w) ] = color;
}

void RenderFatPoint3x3(u8 *image_buffer, Matrix4f view, Matrix4f proj, Vector3f point, u32 w, u32 h, Color color = COLOR_RED) {
    Vector3f point_cam = TransformInversePoint(view, point);

    Ray view_plane = { Vector3f { 0, 0, 0.1 }, Vector3f { 0, 0, 1 } };
    Ray view_plane_far = { Vector3f { 0, 0, 1 }, Vector3f { 0, 0, 1 } };

    if (PointSideOfPlane(point_cam, view_plane) == false) {
        return;
    }
    Vector3f point_ndc = TransformPerspective(proj, point_cam);

    f32 x = (point_ndc.x + 1) / 2 * w;
    f32 y = (point_ndc.y + 1) / 2 * h;

    for (s32 i = -1; i < 2; ++i) {
        for (s32 j = -1; j < 2; ++j) {
            if (x + i < 0 || y + j < 0) {
                continue;;
            }
            if (x + i >= w || y + j >= h) {
                continue;;
            }
            ((Color*) image_buffer)[ GetXYIdx(x + i, y + j, w) ] = color;
        }
    }
}


bool PlaneBooleanOnLineSegment(Ray plane, Vector3f *p1, Vector3f *p2) {
    Vector3f segment_dir = *p2 - *p1;
    segment_dir.Normalize();
    Ray segment_ray = { *p1, segment_dir };

    bool p1_behind = (*p1 - plane.pos).Dot(plane.dir) < 0;
    bool p2_behind = (*p2 - plane.pos).Dot(plane.dir) < 0;

    if (p1_behind && p2_behind) {
        return false;
    }
    else if (p1_behind) {
        // intersect p1

        *p1 = RayPlaneIntersect(segment_ray, plane.pos, plane.dir);
    }
    else if (p2_behind) {
        // intersect p2

        *p2 = RayPlaneIntersect(segment_ray, plane.pos, plane.dir);
    }
    return true;
}


inline
void RenderLineSegment(u8 *image_buffer, Matrix4f view, Perspective persp, Vector3f p1, Vector3f p2, u32 w, u32 h, Color color, bool do_fat_style = false) {

    Vector3f p1_cam = TransformPoint(view, p1);
    Vector3f p2_cam = TransformPoint(view, p2);

    bool is_visible_n = PlaneBooleanOnLineSegment(persp.PlaneNear(), &p1_cam, &p2_cam);
    bool is_visible_l = PlaneBooleanOnLineSegment(persp.PlaneLeft(), &p1_cam, &p2_cam);
    bool is_visible_r = PlaneBooleanOnLineSegment(persp.PlaneRight(), &p1_cam, &p2_cam);
    bool is_visible_t = PlaneBooleanOnLineSegment(persp.PlaneTop(), &p1_cam, &p2_cam);
    bool is_visible_b = PlaneBooleanOnLineSegment(persp.PlaneBottom(), &p1_cam, &p2_cam);
    bool is_visible = is_visible_n && is_visible_l && is_visible_r && is_visible_t && is_visible_b;

    if (is_visible) {
        Vector3f p1_ndc = TransformPerspective(persp.proj, p1_cam);
        Vector3f p2_ndc = TransformPerspective(persp.proj, p2_cam);

        Vector2f a = {};
        a.x = (p1_ndc.x + 1) / 2 * w;
        a.y = (p1_ndc.y + 1) / 2 * h;
        Vector2f b = {};
        b.x = (p2_ndc.x + 1) / 2 * w;
        b.y = (p2_ndc.y + 1) / 2 * h;

        RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, color);



        if (do_fat_style == false) {
            RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, color);
        }
        else {
            RenderLineRGBA(image_buffer, w, h, a.x, a.y, b.x, b.y, color);
            RenderLineRGBA(image_buffer, w, h, a.x+1, a.y, b.x+1, b.y, color);
            RenderLineRGBA(image_buffer, w, h, a.x, a.y+1, b.x, b.y+1, color);
        }
    }
}


//
//  Blitting


inline
Color SampleTexture(f32 x, f32 y, Color col_default, s32 src_width, s32 src_height, Color *src_buffer) {
    s32 i = (s32) round(src_width * x);
    s32 j = (s32) round(src_height * y);
    if (i < 0 || i >= src_width || j < 0 || j >= src_height) {
        return col_default;
    }
    u32 idx = src_width * j + i;
    Color b = src_buffer[idx];
    return b;
}

inline
void Blit32Bit(s32 width, s32 height, s32 left, s32 top, f32 u0, f32 u1, f32 v0, f32 v1, s32 src_width, s32 src_height, Color *src_buffer, s32 dest_width, s32 dest_height, Color *dest) {

    assert(dest_height >= width);
    assert(dest_width >= height);

    f32 q_scale_x = (u1 - u0) / width;
    f32 q_scale_y = (v1 - v0) / height;

    // i,j          : target coords
    // i_img, j_img : img coords

    for (s32 j = 0; j < height; ++j) {
        s32 j_img = j + top;
        if (j_img < 0 || j_img > dest_height) {
            continue;
        }

        for (s32 i = 0; i < width; ++i) {
            s32 i_img = left + i;
            if (i_img < 0 || i_img > dest_width) {
                continue;
            }
            f32 x = u0 + i * q_scale_x;
            f32 y = v0 + j * q_scale_y;

            // TODO: how do we regularize this code?
            Color color_src = SampleTexture(x, y, Color { 0, 0, 0, 255 }, src_width, src_height, src_buffer);

            if (color_src.a != 0) {
                // rudimentary alpha-blending
                s32 idx = j_img * dest_width + i_img;
                Color color_background = dest[idx];

                f32 alpha = (1.0f * color_src.a) / 255;
                Color color_blended;
                color_blended.r = (u8) (floor( alpha*color_src.r ) + floor( (1-alpha)*color_background.r ));
                color_blended.g = (u8) (floor( alpha*color_src.g ) + floor( (1-alpha)*color_background.g ));
                color_blended.b = (u8) (floor( alpha*color_src.b ) + floor( (1-alpha)*color_background.b ));
                color_blended.a = 255;

                dest[idx] = color_blended;
            }
        }
    }
}

inline
void BlitFill(s32 q_w, s32 q_h, f32 q_x0, f32 q_y0, Color q_color, s32 dest_width, s32 dest_height, Color* dest) {
    s32 j_img;
    s32 i_img;
    u32 idx;
    for (s32 j = 0; j < q_h; ++j) {
        j_img = j + q_y0;
        if (j_img < 0 || j_img > dest_height) {
            continue;
        }

        for (s32 i = 0; i < q_w; ++i) {
            i_img = q_x0 + i;
            if (i_img < 0 || i_img > dest_width) {
                continue;
            }

            idx = j_img * dest_width + i_img;
            Color color_src = q_color;


            if (q_color.a == 123) {
                printf("her\n");
            }

            if (color_src.a != 0) {
                // rudimentary alpha-blending
                s32 idx = j_img * dest_width + i_img;
                Color color_background = dest[idx];

                f32 alpha = (1.0f * color_src.a) / 255;
                Color color_blended;
                color_blended.r = (u8) (floor( alpha*color_src.r ) + floor( (1-alpha)*color_background.r ));
                color_blended.g = (u8) (floor( alpha*color_src.g ) + floor( (1-alpha)*color_background.g ));
                color_blended.b = (u8) (floor( alpha*color_src.b ) + floor( (1-alpha)*color_background.b ));
                color_blended.a = 255;

                dest[idx] = color_blended;
            }
        }
    }
}


inline
u8 SampleTexture(f32 x, f32 y, s32 src_width, s32 src_height, u8 *src_buffer) {
    u32 i = (s32) round(src_width * x);
    u32 j = (s32) round(src_height * y);
    u32 idx = src_width * j + i;
    u8 b = src_buffer[idx];
    return b;
}

inline
void Blit8Bit(s32 width, s32 height, f32 x0, f32 y0, f32 q_u0, f32 v0, f32 u1, f32 v1, Color color, s32 src_width, s32 src_height, u8 *src_buffer, s32 dest_width, s32 dest_height, Color* dest_buffer) {
    // i,j          : target coords
    // i_img, j_img : img coords

    f32 q_scale_x = (u1 - q_u0) / width;
    f32 q_scale_y = (v1 - v0) / height;

    s32 stride_dest = dest_width;

    for (s32 j = 0; j < height; ++j) {
        s32 j_dest = j + y0;
        if (j_dest < 0 || j_dest > dest_height) {
            continue;
        }

        for (s32 i = 0; i < width; ++i) {
            s32 i_dest = x0 + i;
            if (i_dest < 0 || i_dest > dest_width) {
                continue;
            }
            f32 x = q_u0 + i * q_scale_x;
            f32 y = v0 + j * q_scale_y;
            if (u8 alpha_byte = SampleTexture(x, y, src_width, src_height, src_buffer)) {
                // rudimentary alpha-blending
                u32 idx = (u32) (j_dest * dest_width + i_dest);
                Color color_background = dest_buffer[idx];

                f32 alpha = (1.0f * alpha_byte) / 255;
                Color color_blended;
                color_blended.r = (u8) (floor( alpha*color.r ) + floor( (1-alpha)*color_background.r ));
                color_blended.g = (u8) (floor( alpha*color.g ) + floor( (1-alpha)*color_background.g ));
                color_blended.b = (u8) (floor( alpha*color.b ) + floor( (1-alpha)*color_background.b ));
                color_blended.a = 255;

                dest_buffer[idx] = color_blended;
            }
        }
    }
}


//
//  Render / control buffer API


Array<Sprite> g_sprite_buffer;


void SpriteBufferInit(MArena *a_dest, u32 max_quads = 2048) {
    g_sprite_buffer = InitArray<Sprite>(a_dest, max_quads);
}

void SpriteBufferPush(Sprite sprite) {
    g_sprite_buffer.Add(sprite);
}

void SpriteBufferBlitAndClear(HashMap map_textures, s32 dest_width, s32 dest_height, u8 *dest_buffer) {

    for (s32 i = 0; i < g_sprite_buffer.len; ++i) {
        Sprite s = g_sprite_buffer.arr[i];
        Texture *s_texture = (Texture*) MapGet(&map_textures, s.tex_id);

        if (s_texture == NULL) {
            BlitFill(s.w, s.h, s.x0, s.y0, s.color, dest_width, dest_height, (Color*) dest_buffer);
        }
        else if (s_texture && s_texture->tpe == TT_8BIT ) {
            Blit8Bit(s.w, s.h, s.x0, s.y0, s.u0, s.v0, s.u1, s.v1, s.color, s_texture->width, s_texture->height, s_texture->data, dest_width, dest_height, (Color*) dest_buffer);
        }
        else if (s_texture && s_texture->tpe == TT_RGBA) {
            Blit32Bit(s.w, s.h, s.x0, s.y0, s.u0, s.u1, s.v0, s.v1, s_texture->width, s_texture->height, (Color*) s_texture->data, dest_width, dest_height, (Color*) dest_buffer);
        }

        else {
            assert("WARN: Attempt to blit unknown texture type\n");
        }
    }

    g_sprite_buffer.len = 0;
}

void SpriteArrayBlit(Array<Sprite> sprites, HashMap map_textures, s32 dest_width, s32 dest_height, Color *dest_buffer) {
    for (s32 i = 0; i < sprites.len; ++i) {
        Sprite s = sprites.arr[i];
        Texture *s_texture = (Texture*) MapGet(&map_textures, s.tex_id);

        if (s_texture == NULL) {
            BlitFill(s.w, s.h, s.x0, s.y0, s.color, dest_width, dest_height, dest_buffer);
        }
        else if (s_texture && s_texture->tpe == TT_8BIT ) {
            Blit8Bit(s.w, s.h, s.x0, s.y0, s.u0, s.v0, s.u1, s.v1, s.color, s_texture->width, s_texture->height, s_texture->data, dest_width, dest_height, dest_buffer);
        }
        else if (s_texture && s_texture->tpe == TT_RGBA) {
            Blit32Bit(s.w, s.h, s.x0, s.y0, s.u0, s.u1, s.v0, s.v1, s_texture->width, s_texture->height, (Color*) s_texture->data, dest_width, dest_height, dest_buffer);
        }

        else {
            assert("WARN: Attempt to blit unknown texture type\n");
        }
    }
}


#endif


#ifndef __WIREFRAME_H__
#define __WIREFRAME_H__


#include "math.h"


struct Wireframe;
Array<Vector3f> WireframeRawSegments(MArena *a_dest, Wireframe *wf);


enum WireFrameType {
    // do not collide
    WFT_AXIS,
    WFT_PLANE,

    // do collide
    WFT_EYE,
    WFT_BOX,
    WFT_CYLINDER,
    WFT_SPHERE,

    WFT_SEGMENTS,

    WFT_COUNT,
};

enum WireFrameRenderStyle {
    WFR_SLIM,
    WFR_FAT,
};

struct Wireframe {
    Matrix4f transform;
    Vector3f dimensions;
    WireFrameType type;
    WireFrameRenderStyle style;
    Color color;
    Vector3f aabox_min;
    Vector3f aabox_max;
    Array<Vector3f> segments;
    bool disabled;

    void CalculateAABox() {
        for (s32 i = 0; i < segments.len; ++i) {
            Vector3f p = segments.arr[i];
            if (i == 0) {
                aabox_min = p;
                aabox_max = p;
            }
            aabox_min.x = MinF32(p.x, aabox_min.x);
            aabox_min.y = MinF32(p.y, aabox_min.y);
            aabox_min.z = MinF32(p.z, aabox_min.z);
            aabox_max.x = MaxF32(p.x, aabox_max.x);
            aabox_max.y = MaxF32(p.y, aabox_max.y);
            aabox_max.z = MaxF32(p.z, aabox_max.z);
        }
    }
    Vector3f Center() {
        Vector3f center = TransformGetTranslation(transform);
        return center;
    }
    f32 SizeBallpark() {
        f32 largest_radius = MaxF32( MaxF32(dimensions.x, dimensions.y), dimensions.z);
        return largest_radius * 2;
    }
};

Wireframe CreatePlaneDetailed(f32 size_x, f32 size_z, s32 nbeams_x) {
    Wireframe box = {};

    box.transform = Matrix4f_Identity();
    box.type = WFT_PLANE;
    box.dimensions = { size_x, (f32) nbeams_x, size_z };
    box.color = COLOR_GRAY;

    return box;
}

Wireframe CreatePlane(f32 size) {
    return CreatePlaneDetailed(size, size, 6);
}

Wireframe CreateCylinder(f32 radius, f32 height) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_CYLINDER;
    f32 diameter = 2*radius;
    box.dimensions = { radius, 0.5f*height, radius };
    box.color = COLOR_GREEN;

    return box;
}

Wireframe CreateSphere(f32 radius) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_SPHERE;
    box.dimensions = { radius, radius, radius };
    box.color = COLOR_BLUE;

    return box;
}

Wireframe CreateEye(f32 width, f32 depth) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_EYE;
    box.dimensions = { 0.5f*width, 0.5f*width, depth };
    box.color = COLOR_BLACK;

    return box;
}

Wireframe CreateAABox(f32 width, f32 height, f32 depth) {
    Wireframe box = {};
    box.transform = Matrix4f_Identity();
    box.type = WFT_BOX;
    box.dimensions = { 0.5f*width, 0.5f*height, 0.5f*depth };
    box.color = COLOR_BLUE;

    return box;
}

Wireframe CreateAAAxes(f32 len = 1.0f) {
    Wireframe axis = {};
    axis.transform = Matrix4f_Identity();
    axis.type = WFT_AXIS;
    axis.dimensions = { len, len, len };
    axis.color = COLOR_GRAY;

    return axis;
}

Wireframe CreateAABoundingBox(MArena *a_dest, Wireframe obj, f32 add_margin = 0) {
    if (obj.segments.len == 0) {
        printf("WARN: no line segments available for bounding box\n");
        return {};
    }

    obj.CalculateAABox();
    Vector3f margin = { add_margin, add_margin, add_margin };
    Vector3f min = obj.aabox_min - margin;
    Vector3f max = obj.aabox_max + margin;

    f32 width = max.x - min.x;
    f32 height = max.y - min.y;
    f32 depth = max.z - min.z;

    Vector3f center = {};
    center.x = min.x + width / 2;
    center.y = min.y + height / 2;
    center.z = min.z + depth / 2;

    Wireframe aabox = CreateAABox(width, height, depth);
    WireframeRawSegments(a_dest, &aabox);
    aabox.color = obj.color;
    aabox.style = obj.style;
    aabox.transform = obj.transform * TransformBuildTranslation(center);

    return aabox;
}


inline
bool FZero(f32 f) {
    bool result = abs(f) < 0.0001f;
    return result;
}
inline
bool FRange(f32 val, f32 min, f32 max) {
    bool result = val >= min && val <= max;
    return result;
}
bool BoxCollideSLAB(Ray global, Wireframe wf, Vector3f *hit_in = NULL, Vector3f *hit_out = NULL) {
    TimeFunction;

    Ray loc = TransformInverseRay(wf.transform, global);
    Vector3f p = loc.pos;
    Vector3f d = loc.dir;
    Vector3f dims = wf.dimensions;

    bool intersect;
    f32 t_close;
    f32 t_far;

    bool zero_x = FZero(d.x);
    bool zero_y = FZero(d.y);
    bool zero_z = FZero(d.z);

    f32 t_low_x = (-dims.x - p.x) / d.x;
    f32 t_high_x = (dims.x - p.x) / d.x;
    f32 t_low_y = (- dims.y - p.y) / d.y;
    f32 t_high_y = (dims.y - p.y) / d.y;
    f32 t_low_z = (- dims.z - p.z) / d.z;
    f32 t_high_z = (dims.z - p.z) / d.z;

    f32 t_close_x = MinF32(t_low_x, t_high_x);
    f32 t_far_x = MaxF32(t_low_x, t_high_x);
    f32 t_close_y = MinF32(t_low_y, t_high_y);
    f32 t_far_y = MaxF32(t_low_y, t_high_y);
    f32 t_close_z = MinF32(t_low_z, t_high_z);
    f32 t_far_z = MaxF32(t_low_z, t_high_z);

    t_close = MaxF32(MaxF32(t_close_x, t_close_y), t_close_z);
    t_far = MinF32(MinF32(t_far_x, t_far_y), t_far_z);
    intersect = t_close <= t_far;

    if (intersect && hit_in) { *hit_in = TransformPoint(wf.transform, loc.pos + t_close * loc.dir); }
    if (intersect && hit_out) { *hit_out = TransformPoint(wf.transform, loc.pos + t_far * loc.dir); }

    return intersect;
}

bool WireFrameCollide(Ray global, Wireframe wf, Vector3f *hit_in = NULL, Vector3f *hit_out = NULL) {
    if (wf.disabled) {
        return false;
    }

    Ray loc = TransformInverseRay(wf.transform, global);
    Vector3f sz = wf.dimensions;

    if (wf.type == WFT_BOX) {

        return BoxCollideSLAB(global, wf, hit_in, hit_out);
    }

    else if (wf.type == WFT_CYLINDER) {
        // TODO: improve cylinder intersection to be exact

        bool boxed = BoxCollideSLAB(global, wf, hit_in, hit_out);
        bool cylhit = true;
        if (hit_in && hit_out) {
            f32 dist;
            LineToLineDist(global.pos, global.dir, {0, 1, 0}, {}, &dist);
            f32 radius = sz.x;
            cylhit = dist < radius;
        }

        return boxed && cylhit;
    }

    else if (wf.type == WFT_EYE) {
        // TODO: impl. triangle-based scheme

        return BoxCollideSLAB(global, wf, hit_in, hit_out);
    }

    else if (wf.type == WFT_SPHERE) {

        Vector3f center = {};
        Vector3f closest = PointToLine(center, loc.pos, loc.dir);
        f32 dist = (center - closest).Norm();
        f32 radius = wf.dimensions.x;
        if (dist <= radius) {

            //  Consider the triangle [center, closest, hit_in], then:
            //  dist^2 + surf_2^2 == radius^2

            f32 surf_2 = sqrt(radius*radius - dist*dist);
            if (hit_in) {
                Vector3f hit_in_loc = closest - surf_2 * loc.dir;
                *hit_in = TransformPoint(wf.transform, hit_in_loc);
            }
            if (hit_out) {
                Vector3f hit_out_loc = closest + surf_2 * loc.dir;
                *hit_out = TransformPoint(wf.transform, hit_out_loc);
            }

            return true;
        }
        else {
            return false;
        }
    }

    else {
        return false;
    }
}

Array<Vector3f> WireframeRawSegments(MArena *a_dest, Wireframe *wf) {
    TimeFunction;

    Array<Vector3f> anchors = {};
    Vector3f sz = wf->dimensions;

    if (wf->type == WFT_SEGMENTS) {

        // don't touch the wireframe
    }

    else if (wf->type == WFT_AXIS) {

        anchors = InitArray<Vector3f>(a_dest, 6);

        Vector3f origo = {0.0f, 0.0f, 0.0f};
        Vector3f x = {sz.x, 0.0f, 0.0f};
        Vector3f y = {0.0f, sz.y, 0.0f};
        Vector3f z = {0.0f, 0.0f, sz.z};

        anchors.Add(origo);
        anchors.Add(x);
        anchors.Add(origo);
        anchors.Add(y);
        anchors.Add(origo);
        anchors.Add(z);

        wf->segments = anchors;
    }

    else if (wf->type == WFT_PLANE) {

        // local coordinates is the x-z plane at y == 0 with nbeams internal cross-lines x and z
        f32 rx = 0.5f * sz.x;
        s32 nbeams_x = sz.y;
        f32 cell_sz = sz.x / (nbeams_x - 1);
        s32 nbeams_z = floor(sz.z / cell_sz + 1);
        f32 rz = 0.5f * (nbeams_z - 1) * cell_sz;

        Vector3f ulc = { -rx, 0, rz };
        Vector3f lrc = { rx, 0, -rz };
        Vector3f llc = { -rx, 0, -rz };

        // insider beams
        Vector3f xhat = { cell_sz, 0, 0 };
        Vector3f zhat = { 0, 0, cell_sz };

        s32 nanchors = (nbeams_x + nbeams_z) * 2; // num_beams * num_dimensions * anchs_per_point
        anchors = InitArray<Vector3f>(a_dest, nanchors);

        for (s32 i = 0; i < nbeams_x; ++i) {
            anchors.Add(ulc + i* xhat);
            anchors.Add(llc + i* xhat);
        }
        for (s32 i = 0; i < nbeams_z; ++i) {
            anchors.Add(llc + i * zhat);
            anchors.Add(lrc + i * zhat);
        }
        assert(anchors.len == nanchors);

        wf->segments = anchors;
    }

    else if (wf->type == WFT_BOX) {

        anchors = InitArray<Vector3f>(a_dest, 24);

        Vector3f ppp = { sz.x, sz.y, sz.z };
        Vector3f ppm = { sz.x, sz.y, -sz.z };
        Vector3f pmp = { sz.x, -sz.y, sz.z };
        Vector3f pmm = { sz.x, -sz.y, -sz.z };
        Vector3f mpp = { -sz.x, sz.y, sz.z };
        Vector3f mpm = { -sz.x, sz.y, -sz.z };
        Vector3f mmp = { -sz.x, -sz.y, sz.z };
        Vector3f mmm = { -sz.x, -sz.y, -sz.z };

        anchors.Add(ppp);
        anchors.Add(ppm);
        anchors.Add(ppm);
        anchors.Add(pmm);
        anchors.Add(pmm);
        anchors.Add(pmp);
        anchors.Add(pmp);
        anchors.Add(ppp);

        anchors.Add(mpp);
        anchors.Add(mpm);
        anchors.Add(mpm);
        anchors.Add(mmm);
        anchors.Add(mmm);
        anchors.Add(mmp);
        anchors.Add(mmp);
        anchors.Add(mpp);

        anchors.Add(ppp);
        anchors.Add(mpp);
        anchors.Add(ppm);
        anchors.Add(mpm);
        anchors.Add(pmm);
        anchors.Add(mmm);
        anchors.Add(pmp);
        anchors.Add(mmp);

        wf->segments = anchors;
    }

    else if (wf->type == WFT_SPHERE) {

        u32 len_prev = anchors.len;
        f32 r = sz.x;

        u32 nlatt = 6;
        u32 nlong = 6;

        u32 cnt = (nlatt * 2 + nlatt * nlong * 2) + (nlong * 2 + nlong * nlatt * 2);
        anchors = InitArray<Vector3f>(a_dest, cnt);

        Vector3f center = {};
        Vector3f north = { 0, r, 0 };
        Vector3f south = { 0, -r, 0 };

        for (u32 i = 0; i < nlatt; ++i) {
            f32 theta = PI / nlatt * i;

            Vector3f pt_0 = SphericalCoordsY(theta, 0, r);
            anchors.Add(pt_0);
            for (u32 j = 0; j < nlong; ++j) {
                f32 phi = 2 * PI / nlong * (j % nlong);

                Vector3f pt = SphericalCoordsY(theta, phi, r);
                anchors.Add(pt);
                anchors.Add(pt);
            } 
            anchors.Add(pt_0);
        }

        for (u32 j = 0; j < nlong; ++j) {
            f32 phi = 2 * PI / nlong * j;

            anchors.Add(north);
            for (u32 i = 0; i < nlatt; ++i) {
                f32 theta = PI / nlatt * i;

                Vector3f pt = SphericalCoordsY(theta, phi, r);
                anchors.Add(pt);
                anchors.Add(pt);

            }
            anchors.Add(south);
        }

        wf->segments = anchors;
    }

    else if (wf->type == WFT_CYLINDER) {

        f32 r = sz.x;
        f32 h2 = sz.y;

        s32 nbars = 8;
        u32 cnt = nbars * 2 + nbars * 4;
        anchors = InitArray<Vector3f>(a_dest, cnt);

        Vector3f up_prev = {};
        Vector3f lw_prev = {};
        Vector3f up_first = {};
        Vector3f lw_first = {};
        for (u32 i = 0; i < nbars; ++i) {
            f32 theta = 2 * 3.14159265f / 8 * i;
            Vector3f up = { r * cos(theta), h2, r * sin(theta) };
            Vector3f lw = { r * cos(theta), - h2, r * sin(theta) };

            anchors.Add(up);
            anchors.Add(lw);

            if (i == 0) {
                up_first = up;
                lw_first = lw;
            }
            else if (i > 0) {
                anchors.Add(up_prev);
                anchors.Add(up);
                anchors.Add(lw_prev);
                anchors.Add(lw);
            }
            if (i == (nbars - 1)) {
                anchors.Add(up);
                anchors.Add(up_first);
                anchors.Add(lw);
                anchors.Add(lw_first);
            }
            up_prev = up;
            lw_prev = lw;
        }

        wf->segments = anchors;
    }

    else if (wf->type == WFT_EYE) {

        anchors = InitArray<Vector3f>(a_dest, 16);

        f32 w2 = sz.x;
        f32 d = sz.z;

        Vector3f urc = { w2, w2, d };
        Vector3f ulc = { - w2, w2, d };
        Vector3f lrc = { w2, - w2, d };
        Vector3f llc = { - w2, - w2, d };
        Vector3f point = {};

        anchors.Add(urc);
        anchors.Add(ulc);
        anchors.Add(ulc);
        anchors.Add(llc);
        anchors.Add(llc);
        anchors.Add(lrc);
        anchors.Add(lrc);
        anchors.Add(urc);

        anchors.Add(urc);
        anchors.Add(point);
        anchors.Add(ulc);
        anchors.Add(point);
        anchors.Add(llc);
        anchors.Add(point);
        anchors.Add(lrc);
        anchors.Add(point);

        wf->segments = anchors;
    }

    else {
        printf("WARN: Unknown wireframe type\n");
    }

    return anchors;
}


Array<Vector3f> WireframeLineSegments(MArena *a_dest, Array<Wireframe> wireframes) {
    Array<Vector3f> anchors_all = InitArray<Vector3f>(a_dest, 0);

    for (u32 i = 0; i < wireframes.len; ++i) {
        Array<Vector3f> anchors;
        anchors = WireframeRawSegments(a_dest, wireframes.arr + i);
        anchors_all.len += anchors.len;

        // points delivered in local coords
    }

    anchors_all.max = anchors_all.len;
    return anchors_all;
}


inline
s32 _GetNextNonDisabledWireframeIndex(u32 idx_prev, Array<Wireframe> wireframes) {
    idx_prev++;
    if (wireframes.len <= idx_prev) {
        return -1;
    }

    while ((wireframes.arr + idx_prev)->disabled == true) {
        idx_prev++;

        if (wireframes.len <= idx_prev) {
            return -1;
        }
    }
    return idx_prev;
}


void RenderWireframe(u8 *image_buffer, Matrix4f view, Perspective persp, u32 w, u32 h, Wireframe wf) {
    Ray view_plane = { Vector3f { 0, 0, 0.1 }, Vector3f { 0, 0, 1 } };
    Matrix4f view_inv = TransformGetInverse(view);
    Matrix4f l2v = view_inv * wf.transform;

    for (u32 i = 0; i < wf.segments.len / 2; ++i) {
        Vector3f a = wf.segments.arr[2*i];
        Vector3f b = wf.segments.arr[2*i + 1];

        RenderLineSegment(image_buffer, l2v, persp, a, b, w, h, wf.color, wf.style == WFR_FAT);
    }
}


#endif


#ifndef __QUAD_H__
#define __QUAD_H__


//
//  Quads modelled as six vertices


struct QuadVertex {
    Vector2f pos;
    Vector2f tex;
    u64 tex_id;
    Color col;
};


inline
QuadVertex InitQuadVertex(Vector2f pos, Vector2f tex, Color color, u64 texture_id) {
    QuadVertex v = {};
    v.pos = pos;
    v.tex = tex;
    v.col = color;
    v.tex_id = texture_id;
    return v;
}

struct Quad { // renderable six-vertex quad
    QuadVertex verts[6];

    inline
    u64 GetTextureId() {
        return verts[0].tex_id;
    }
    inline
    Color GetColor() {
        return verts[0].col;
    }
    inline
    void SetColor(Color color) {
        for (s32 i = 0; i < 6; ++i) {
            verts[i].col = color;
        }
    }
    inline
    f32 GetWidth() {
        f32 x0 = verts[2].pos.x;
        f32 x1 = verts[0].pos.x;
        f32 width = x1 - x0;
        return width;
    }
    inline
    f32 GetHeight() {
        f32 y0 = verts[0].pos.y;
        f32 y1 = verts[2].pos.y;
        f32 width = y1 - y0;
        return width;
    }
    inline
    f32 GetX0() {
        f32 x0 = verts[2].pos.x;
        return x0;
    }
    inline
    f32 GetY0() {
        f32 y0 = verts[0].pos.y;
        return y0;
    }
    inline
    f32 GetX1() {
        f32 x1 = verts[0].pos.x;
        return x1;
    }
    inline
    f32 GetY1() {
        f32 y1 = verts[1].pos.y;
        return y1;
    }
    inline
    f32 GetTextureScaleX(s32 dest_width) {
        f32 u0 = verts[2].tex.x;
        f32 u1 = verts[0].tex.x;
        f32 scale_x = (u1 - u0) / dest_width;
        return scale_x;
    }
    inline
    f32 GetTextureScaleY(s32 dest_height) {
        f32 v0 = verts[0].tex.y;
        f32 v1 = verts[2].tex.y;
        f32 scale_y = (v1 - v0) / dest_height;
        return scale_y;
    }
    inline
    f32 GetTextureU0() {
        f32 u0 = verts[2].tex.x;
        return u0;
    }
    inline
    f32 GetTextureU1() {
        f32 u1 = verts[0].tex.x;
        return u1;
    }
    inline
    f32 GetTextureV0() {
        f32 v0 = verts[0].tex.y;
        return v0;
    }
    inline
    f32 GetTextureV1() {
        f32 v1 = verts[2].tex.y;
        return v1;
    }
    f32 GetTextureWidth() {
        f32 u0 = verts[2].tex.x;
        f32 u1 = verts[0].tex.x;
        return u1 - u0;
    }
    inline
    f32 GetTextureHeight() {
        f32 v0 = verts[0].tex.y;
        f32 v1 = verts[2].tex.y;
        return v1 - v0;
    }
};

Quad QuadSolid(f32 w, f32 h, f32 x0, f32 y0, Color c) {
    // lays down two three-vertex triangles: T1 = [ urc->lrc->llc ] and T2 = [ llc->ulc->urc ]
    // ulc: upper-left corner (etc.)
    Quad qh = {};
    f32 x1 = x0 + w;
    f32 y1 = y0 + h;

    Vector2f ulc_pos { x0, y0 };
    Vector2f urc_pos { x1, y0 };
    Vector2f lrc_pos { x1, y1 };
    Vector2f llc_pos { x0, y1 };

    qh.verts[0] = InitQuadVertex( urc_pos, { 0, 0 }, c, 0 );
    qh.verts[1] = InitQuadVertex( lrc_pos, { 0, 0 }, c, 0 );
    qh.verts[2] = InitQuadVertex( llc_pos, { 0, 0 }, c, 0 );
    qh.verts[3] = InitQuadVertex( llc_pos, { 0, 0 }, c, 0 );
    qh.verts[4] = InitQuadVertex( ulc_pos, { 0, 0 }, c, 0 );
    qh.verts[5] = InitQuadVertex( urc_pos, { 0, 0 }, c, 0 );

    return qh;
}

Quad QuadTextured(Sprite s, s32 x0, s32 y0, u64 texture_id) {
    // lays down two three-vertex triangles: T1 = [ urc->lrc->llc ] and T2 = [ llc->ulc->urc ]
    // ulc: upper-left corner (etc.)

    Quad qh = {};
    s32 x1 = x0 + s.w;
    s32 y1 = y0 + s.h;

    Vector2f ulc_pos { (f32) x0, (f32) y0 };
    Vector2f urc_pos { (f32) x1, (f32) y0 };
    Vector2f lrc_pos { (f32) x1, (f32) y1 };
    Vector2f llc_pos { (f32) x0, (f32) y1 };

    Vector2f ulc_tex { (f32) s.u0, (f32) s.v0 };
    Vector2f urc_tex { (f32) s.u1, (f32) s.v0 };
    Vector2f lrc_tex { (f32) s.u1, (f32) s.v1 };
    Vector2f llc_tex { (f32) s.u0, (f32) s.v1 };

    Color no_color = { 0, 0, 0, 255 };

    qh.verts[0] = InitQuadVertex( urc_pos, urc_tex, no_color, texture_id );
    qh.verts[1] = InitQuadVertex( lrc_pos, lrc_tex, no_color, texture_id );
    qh.verts[2] = InitQuadVertex( llc_pos, llc_tex, no_color, texture_id );
    qh.verts[3] = InitQuadVertex( llc_pos, llc_tex, no_color, texture_id );
    qh.verts[4] = InitQuadVertex( ulc_pos, ulc_tex, no_color, texture_id );
    qh.verts[5] = InitQuadVertex( urc_pos, urc_tex, no_color, texture_id );

    return qh;
}

inline
Quad QuadOffset(Quad *q, s16 x, s16 y, Color color, u64 texture_id) {
    Quad out = {};
    for (u32 i = 0; i < 6; ++i) {
        QuadVertex v = *(q->verts + i);
        v.pos.x += x;
        v.pos.y += y;
        v.col = color;
        v.tex_id = texture_id;
        out.verts[i] = v;
    }
    return out;
}


#endif


#ifndef __RESOURCE_H__
#define __RESOURCE_H__


enum ResourceType {
    RST_FONT,
    RST_SPRITE,

    RST_CNT
};
void PrintResourceType(ResourceType tpe) {
    if (tpe == RST_FONT) {
        printf("font\n");
    }
    else if (tpe == RST_SPRITE) {
        printf("sprite map\n");
    }
    else {
        printf("_unknown_\n");
    }
}


struct ResourceHdr {
    ResourceType tpe;
    u32 data_sz;
    u32 next;
    char name[64];
    char key_name[64];

    ResourceHdr *GetInlinedNext() {
        if (next == 0) {
            return NULL;
        }
        ResourceHdr *nxt =  (ResourceHdr*) ((u8*) this + next);
        return nxt;
    }
    u8 *GetInlinedData() {
        u8 *dta =  (u8*) this + sizeof(ResourceHdr);
        assert( (dta + data_sz) == (u8*) GetInlinedNext() || GetInlinedNext() == NULL );

        return dta;
    }
};


struct ResourceStreamHandle {
    u32 cnt;
    u32 cnt_tpe[RST_CNT];
    StrLst *names[RST_CNT];
    StrLst *key_names[RST_CNT];

    ResourceHdr *first;
    ResourceHdr *prev;
    ResourceHdr *current;
};


// embeded resource
#define MAX_RESOURCE_CNT 999
extern "C" {
    extern char _binary_all_res_start[];
}


ResourceStreamHandle ResourceStreamLoadAndOpen(MArena *a_tmp, MArena *a_dest, const char *filename, bool put_strs_inline = true) {

    ResourceStreamHandle hdl = {};
    hdl.first =  (ResourceHdr *) &_binary_all_res_start[0];

    HashMap map_names = InitMap(a_tmp, MAX_RESOURCE_CNT);
    HashMap map_keynames = InitMap(a_tmp, MAX_RESOURCE_CNT);

    ResourceHdr *res = hdl.first;

    StrSetArenas(a_dest, NULL);
    while (res) {
        assert(hdl.cnt < MAX_RESOURCE_CNT && "artificial resources load count limit reached");

        hdl.prev = res;
        hdl.cnt++;
        hdl.cnt_tpe[res->tpe]++;

        // check keyname uniqueness & record unique names and keynames
        u64 key = HashStringValue(res->key_name);
        if (MapGet(&map_keynames, key)) {
            assert( MapGet(&map_keynames, key) == 0 && "resource key duplicate");
        }
        if (put_strs_inline) {
            hdl.key_names[res->tpe] = StrLstPush(res->key_name, hdl.key_names[res->tpe]);
        }
        MapPut(&map_names, key, res);
        key = HashStringValue(res->name);
        if (MapGet(&map_names, key) == 0) {
            MapPut(&map_names, key, res);
            if (put_strs_inline) {
                hdl.names[res->tpe] = StrLstPush(res->name, hdl.names[res->tpe]);
            }
        }

        // check
        res->GetInlinedData();

        // iter
        res = res->GetInlinedNext();
    }
    StrPopArenas();

    printf("opened resource file '%s': %u entries (", filename, hdl.cnt);
    for (u32 i = 0; i < RST_CNT; ++i) {
        printf("%u", hdl.cnt_tpe[i]);
        if (i + 1 < RST_CNT) {
            printf(", ");
        }
        if (hdl.key_names[i]) {
            hdl.key_names[i] = hdl.key_names[i]->first;
            hdl.names[i] = hdl.names[i]->first;
        }
    }
    printf(")\n");

    return hdl;
}

void ResourceStreamPushData(MArena *a_dest, ResourceStreamHandle *stream, ResourceType tpe, char *name, char *key_name, void *data, u32 data_sz) {
    assert(stream != NULL);

    stream->current = (ResourceHdr*) ArenaAlloc(a_dest, sizeof(ResourceHdr));
    stream->current->tpe = tpe;
    stream->current->data_sz = data_sz;
    memcpy(stream->current->name, key_name, strlen(name));
    memcpy(stream->current->key_name, key_name, strlen(key_name));
    if (stream->prev) {
        stream->prev->next = (u32) ((u8*) stream->current - (u8*) stream->prev);
    }
    stream->prev = stream->current;
    if (stream->first == NULL) {
        stream->first = stream->current;
    }
    ArenaPush(a_dest, data, data_sz);
}

void ResourceStreamPushData(MArena *a_dest, ResourceStreamHandle *stream, ResourceType tpe, char *name, const char *key_name, void *data, u32 data_sz) {
    return ResourceStreamPushData(a_dest, stream, tpe, (char*) name, (char*) key_name, data, data_sz);
}

void ResourceStreamPushDataExtra(MArena *a_dest, ResourceStreamHandle *stream, void *data, u32 data_sz) {
    ArenaPush(a_dest, data, data_sz);
    stream->current->data_sz += data_sz;
}

void ResourceStreamSave(ResourceStreamHandle *stream, const char *filename = "all.res") {
    assert(stream->first != NULL);
    assert(stream->prev != NULL);

    void *data = stream->first;
    u32 last_sz = stream->prev->data_sz + sizeof(ResourceHdr);
    u32 data_sz = (u32) ((u8*) stream->prev - (u8*) stream->first + last_sz);

    SaveFile(filename, data, data_sz);
}


#endif


#ifndef __FONT_H__
#define __FONT_H__


//
//  FontAtlas


struct FontAtlas {
    Texture texture;
    u32 sz_px;
    u32 cell_width;
    List<Sprite> glyphs;
    char name_font[32];
    char name_font_and_sz[32];
    u64 hash;

    // previously known as da GLYPH PLOTTA !
    s32 ln_height;
    s32 ln_measured;
    s32 ln_ascend;
    s32 ln_descend;
    List<u8> advance_x;
    List<u8> x_lsb;
    List<s8> y_ascend;

    Sprite glyphs_mem[128];
    u8 advance_x_mem[128];
    u8 x_lsb_mem[128];
    s8 y_ascend_mem[128];
    Quad cooked_mem[128];

    s32 GetLineBaseOffset() {
        return ln_measured - ln_descend;
    }
    Str GetFontName() {
        return Str { this->name_font, (u32) strlen(this->name_font) };
    }
    void Print() {
        printf("font_sz %u, bitmap_sz %u %u, cell_w %u, ln_height %u, ln_ascend %u, glyphs %u, data ptrs %p %p\n", sz_px, texture.width, texture.height, cell_width, ln_height, ln_ascend, glyphs.len, glyphs.lst, texture.data);
    }
};

FontAtlas *FontAtlasLoadBinaryStream(u8 *base_ptr, u32 sz_data) {
    FontAtlas *atlas = (FontAtlas*) base_ptr;
    u32 sz_base = sizeof(FontAtlas);
    u32 sz_bitmap = atlas->texture.width * atlas->texture.height * atlas->texture.px_sz;

    assert(sz_data == sz_base + sz_bitmap && "sanity check data size");

    // set pointers
    atlas->glyphs = { atlas->glyphs_mem, 128 };
    atlas->texture.data = base_ptr + sz_base;
    atlas->advance_x = { &atlas->advance_x_mem[0], 128 };
    atlas->x_lsb = { &atlas->x_lsb_mem[0], 128 };
    atlas->y_ascend = { &atlas->y_ascend_mem[0], 128 };
    atlas->hash = HashStringValue(atlas->name_font_and_sz);

    return atlas;
};

FontAtlas *FontAtlasLoadBinary128(MArena *a_dest, char *filename, u32 *sz = NULL) {
    u64 sz_file;
    u8 *base_ptr = (u8*) LoadFileMMAP(filename, &sz_file);
    u32 sz_alloc = (u32) sz_file;
    base_ptr = (u8*) ArenaPush(a_dest, base_ptr, sz_alloc); // move to read-write memory location
    if (sz != NULL) {
        *sz = sz_alloc;
    }

    return FontAtlasLoadBinaryStream(base_ptr, sz_alloc);
};

void FontAtlasSaveBinary128(MArena *a_tmp, char *filename, FontAtlas atlas) {
    u32 sz_base = sizeof(FontAtlas);
    u32 sz_bitmap = atlas.texture.width * atlas.texture.height * atlas.texture.px_sz;

    FontAtlas *atlas_inlined = (FontAtlas*) ArenaPush(a_tmp, &atlas, sz_base);
    ArenaPush(a_tmp, atlas_inlined->texture.data, sz_bitmap);

    // invalidate pointers
    atlas_inlined->glyphs.lst = 0;
    atlas_inlined->advance_x.lst = 0;
    atlas_inlined->x_lsb.lst = 0;
    atlas_inlined->y_ascend.lst = 0;
    atlas_inlined->texture.data = 0;

    SaveFile(filename, (u8*) atlas_inlined, sz_base + sz_bitmap);
}

void GlyphPlotterPrint(FontAtlas *plt) {
    printf("ln_height: %d\n", plt->ln_height);
    printf("ln_ascend: %d\n", plt->ln_ascend);
    for (u32 i = 0; i - plt->advance_x.len; ++i) {
        u8 adv_x = plt->advance_x.lst[i];
        printf("%d ", adv_x);
    }
    printf("tex_w: %d\n", plt->texture.width);
    printf("tex_h: %d\n", plt->texture.height);
    printf("tex_px_sz: %d\n", plt->texture.px_sz);
    printf("tex_ptr: %lu\n", (u64) plt->texture.data);
    printf("\n");
}


enum FontSize {
    FS_10,
    FS_12,
    FS_14,
    FS_16,
    FS_18,
    FS_24,
    FS_30,
    FS_36,
    FS_48,
    FS_60,
    FS_72,
    FS_84,

    FS_CNT,
};

u32 FontSizeToPx(FontSize font_size) {
    u32 sz_px = 0;
    switch (font_size) {
        case FS_10: sz_px = 10; break;
        case FS_12: sz_px = 12; break;
        case FS_14: sz_px = 14; break;
        case FS_16: sz_px = 16; break;
        case FS_18: sz_px = 18; break;
        case FS_24: sz_px = 24; break;
        case FS_30: sz_px = 30; break;
        case FS_36: sz_px = 36; break;
        case FS_48: sz_px = 48; break;
        case FS_60: sz_px = 60; break;
        case FS_72: sz_px = 72; break;
        case FS_84: sz_px = 84; break;
        default: break;
    }
    return sz_px;
}

FontSize FontSizeFromPx(u32 sz_px) {
    FontSize fs = FS_18;
    switch (sz_px) {
        case 10 : fs = FS_10; break;
        case 12 : fs = FS_12; break;
        case 14 : fs = FS_14; break;
        case 16 : fs = FS_16; break;
        case 18 : fs = FS_18; break;
        case 24 : fs = FS_24; break;
        case 30 : fs = FS_30; break;
        case 36 : fs = FS_36; break;
        case 48 : fs = FS_48; break;
        case 60 : fs = FS_60; break;
        case 72 : fs = FS_72; break;
        case 84 : fs = FS_84; break;
        default: break;
    }
    return fs;
}


//
//  Font related globals


// TODO: move these things to imui.h where they are used for that API
static HashMap *g_font_map;
static FontAtlas *g_current_font;


FontAtlas *SetFontAndSize(FontSize font_size, Str font_name) {
    // font name
    font_name = StrCat(font_name, "_");

    // size
    char buff[8];
    sprintf(buff, "%.2u", FontSizeToPx(font_size));
    Str key_name = StrCat(font_name, StrL(buff));

    // get by key
    u64 key = HashStringValue(StrZ(key_name));
    u64 val = MapGet(g_font_map, key);
    g_current_font = (FontAtlas*) val;
    return g_current_font;
}

FontAtlas *UI_SetFont(Str font_name) {
    assert(g_current_font != NULL);

    return SetFontAndSize( FontSizeFromPx(g_current_font->sz_px), font_name);
}

FontAtlas *UI_SetFontSize(FontSize font_size) {
    assert(g_current_font != NULL);

    return SetFontAndSize( font_size, g_current_font->GetFontName());
}

FontSize UI_GetFontSize() {
    s32 sz_px = g_current_font->sz_px;
    switch (sz_px) {
        case 10: return FS_10; break;
        case 12: return FS_12; break;
        case 14: return FS_14; break;
        case 16: return FS_16; break;
        case 18: return FS_18; break;
        case 24: return FS_24; break;
        case 30: return FS_30; break;
        case 36: return FS_36; break;
        case 48: return FS_48; break;
        case 60: return FS_60; break;
        case 72: return FS_72; break;
        case 84: return FS_84; break;
        default: return FS_CNT;
    }
}

static FontSize g_font_sz_default;
FontSize GetDefaultFontSize() {
    return g_font_sz_default;
}
void SetDefaultFontSize(FontSize fs) {
    g_font_sz_default = fs;
}


// TODO: export helper functions to string.h


inline
bool IsWhiteSpace(char c) {
    bool result = (c == ' ' || c == '\n' || c == '\t');
    return result;
}
inline
bool IsWhiteSpace(Str s) {
    assert(s.len > 0);
    return IsWhiteSpace(s.str[0]);
}
inline
bool IsNewLine(char c) {
    bool result = (c == '\n');
    return result;
}
inline
bool IsNewLine(Str s) {
    assert(s.len > 0);
    return IsNewLine(s.str[0]);
}
inline
bool IsAscii(char c) {
    bool result = c >= 0 && c < 128;
    return result;
}
inline
Str StrInc(Str s, u32 inc) {
    assert(inc <= s.len);
    s.str += inc;
    s.len -= inc;
    return s;
}


s32 TextLineWidth(FontAtlas *plt, Str txt) {
    s32 pt_x = 0;
    s32 w_space = plt->advance_x.lst[' '];

    for (u32 i = 0; i < txt.len; ++i) {
        // while words
        char c = txt.str[i];

        if (c == ' ') {
            pt_x += w_space;
            continue;
        }
        if (IsAscii(c) == false) {
            continue;
        }

        pt_x += plt->advance_x.lst[c];
    }

    return pt_x;
}

s32 TextLineHeight(FontAtlas *plt) {
    return plt->ln_measured;
}

void TextPositionLine(Str txt, s32 box_l, s32 box_t, s32 box_w, s32 box_h, s32 align_horiz, s32 align_vert, s32 *txt_l, s32 *txt_t, s32 *txt_w, s32 *txt_h) {
    FontAtlas *plt = g_current_font;

    *txt_w = 0;
    *txt_h = plt->ln_measured; // single-line height

    s32 w_space = plt->advance_x.lst[' '];
    for (u32 i = 0; i < txt.len; ++i) {

        char c = txt.str[i];
        if (c == ' ') {
            *txt_w += w_space;
            continue;
        }
        if (IsAscii(c) == false) {
            continue;
        }

        *txt_w += plt->advance_x.lst[c];
    }

    s32 box_x = box_l + box_w / 2;
    s32 box_y = box_t + box_h / 2;

    // text rect center
    s32 txt_x;
    s32 txt_y;

    if (align_horiz == 0) { // alight horizontal center
        txt_x = box_x;
    }
    else if (align_horiz > 0) { // alight horizontal left
        txt_x = box_l + *txt_w / 2;
    }
    else if (align_horiz < 0) { // alight horizontal right
        s32 box_r = box_l + box_w;
        txt_x = box_r - *txt_w / 2;
    }
    if (align_vert == 0) { // alight vertical center
        txt_y = box_y;
    }
    else if (align_vert > 0) { // alight vertical top
        txt_y = box_t + *txt_h / 2;
    }
    else if (align_vert < 0) { // alight vertical bottom
        s32 box_b = box_t + box_h;
        txt_y = box_b - *txt_h / 2;
    }

    *txt_l = txt_x - *txt_w / 2;
    *txt_t = txt_y - *txt_h / 2;
}


// TODO: use floats to position characters


void TextPlot(Str txt, s32 box_l, s32 box_t, s32 box_w, s32 box_h, s32 *sz_x, s32 *sz_y, Color color, s32 align_h = 0, s32 align_v = 0) {
    assert(g_current_font != NULL && "init text plotters first");
    FontAtlas *plt = g_current_font;

    s32 txt_l;
    s32 txt_t;
    TextPositionLine(txt, box_l, box_t, box_w, box_h, align_h, align_v, &txt_l, &txt_t, sz_x, sz_y);

    // position the quads
    s32 pt_x = txt_l;
    s32 pt_y = txt_t + plt->GetLineBaseOffset();
    s32 w_space = plt->advance_x.lst[' '];
    u64 plt_key = plt->hash;

    for (u32 i = 0; i < txt.len; ++i) {
        char c = txt.str[i];

        if (c == ' ') {
            pt_x += w_space;
            continue;
        }
        if (IsAscii(c) == false) {
            continue;
        }

        Sprite s = plt->glyphs_mem[c];

        Frame f = {};
        f.w = s.w;
        f.h = s.h;
        f.u0 = s.u0;
        f.u1 = s.u1;
        f.v0 = s.v0;
        f.v1 = s.v1;
        f.x0 = s.x0 + pt_x;
        f.y0 = s.y0 + pt_y;

        f.color = color;
        f.tex_id = plt_key;

        pt_x += plt->advance_x.lst[c];
        SpriteBufferPush(f);
    }
}


Array<Sprite> TextPlot(MArena *a_dest, Str txt, s32 box_l, s32 box_t, s32 box_w, s32 box_h, s32 *sz_x, s32 *sz_y, Color color, s32 align_h = 0, s32 align_v = 0) {
    assert(g_current_font != NULL && "init text plotters first");
    FontAtlas *plt = g_current_font;

    s32 txt_l;
    s32 txt_t;
    TextPositionLine(txt, box_l, box_t, box_w, box_h, align_h, align_v, &txt_l, &txt_t, sz_x, sz_y);
    Array<Sprite> sprites = InitArray<Sprite>(a_dest, txt.len);

    // position the quads
    s32 pt_x = txt_l;
    s32 pt_y = txt_t + plt->GetLineBaseOffset();
    s32 w_space = plt->advance_x.lst[' '];
    u64 plt_key = plt->hash;

    for (u32 i = 0; i < txt.len; ++i) {
        char c = txt.str[i];

        if (c == ' ') {
            pt_x += w_space;
            continue;
        }
        if (IsAscii(c) == false) {
            continue;
        }

        Sprite glyph = plt->glyphs_mem[c];

        Frame f = {};
        f.w = glyph.w;
        f.h = glyph.h;
        f.u0 = glyph.u0;
        f.u1 = glyph.u1;
        f.v0 = glyph.v0;
        f.v1 = glyph.v1;
        f.x0 = glyph.x0 + pt_x;
        f.y0 = glyph.y0 + pt_y;

        f.color = color;
        f.tex_id = plt_key;

        pt_x += plt->advance_x.lst[c];
        sprites.Add(f);
    }

    return sprites;
}

#endif


#ifndef __IMUI_H__
#define __IMUI_H__


//
//  UI Panel quad layout
//


void PanelPlot(f32 l, f32 t, f32 w, f32 h, f32 sz_border, Color col_border = { RGBA_GRAY_75 }, Color col_pnl = { RGBA_WHITE } )
{
    if (sz_border >= w / 2 || sz_border >= w / 2) {
        return;
    }

    if (col_pnl.a > 0) {
        // push the background
        Frame background = {};
        background.w = w - 2*sz_border;
        background.h = h - 2*sz_border;
        background.x0 = l + sz_border;
        background.y0 = t + sz_border;
        background.color = col_pnl;

        SpriteBufferPush(background);
    }

    if (sz_border > 0.0f) {
        // left side
        Frame border = {};
        border.w = sz_border;
        border.h = h;
        border.x0 = l;
        border.y0 = t;
        border.color = col_border;
        SpriteBufferPush(border);

        // right side
        border = {};
        border.w = sz_border;
        border.h = h;
        border.x0 = l + w - sz_border;
        border.y0 = t;
        border.color = col_border;
        SpriteBufferPush(border);

        // top side
        border = {};
        border.w = w;
        border.h = sz_border;
        border.x0 = l;
        border.y0 = t;
        border.color = col_border;
        SpriteBufferPush(border);

        // top side
        border = {};
        border.w = w;
        border.h = sz_border;
        border.x0 = l;
        border.y0 = t + h - sz_border;
        border.color = col_border;
        SpriteBufferPush(border);
    }

}


//
//  Immediate-Mode User Interface
//


//
//  Tree structure is built every turn
//  
//  How the tree structure links:
//      - siblings are iterated by next
//      - sub-branches are created from a node using first
//      - all nodes (except root) have parent set
//


struct CollRect {
    f32 x0;
    f32 x1;
    f32 y0;
    f32 y1;

    inline
    bool DidCollide(f32 x, f32 y) {
        bool bx = (x >= x0 && x <= x1);
        bool by = (y >= y0 && y <= y1);
        return bx && by;
    }
    s32 Width() {
        f32 width = x1 - x0;
        return (s32) floor(width);
    }
    s32 Height() {
        f32 height = y1 - y0;
        return (s32) floor(height);
    }
};


enum WidgetAlignmentFLags {
    WA_PASSIVE = 0,

    WA_TOP_LEFT = 1 << 0,
    WA_TOP_RIGHT = 1 << 1,
    WA_BOTTOM_LEFT = 1 << 2,
    WA_BOTTOM_RIGHT = 1 << 3,

    WA_CENTV_LEFT = 1 << 4,
    WA_CENTV_RIGHT = 1 << 5,
    WA_TOP_CENTH = 1 << 6,
    WA_BOTTOM_CENTH = 1 << 7,

    WA_CENTER = 1 << 8
};


enum WidgetFlags {
    WF_PASSIVE = 0,

    WF_DRAW_BACKGROUND_AND_BORDER = 1 << 0,
    WF_DRAW_TEXT = 1 << 1,
    WF_CAN_COLLIDE = 1 << 2,

    WF_LAYOUT_CENTER = 1 << 10,
    WF_LAYOUT_HORIZONTAL = 1 << 11,
    WF_LAYOUT_VERTICAL = 1 << 12,
    WF_ALIGN_LEFT_OR_TOP = 1 << 13,
    WF_ALIGN_RIGHT_OR_BOTTOM = 1 << 14,
    WF_ALIGN_CENTER = 1 << 15,

    WF_EXPAND_HORIZONTAL = 1 << 16,
    WF_EXPAND_VERTICAL = 1 << 17,

    WF_ABSREL_POSITION = 1 << 18
};

bool WidgetIsLayout(u32 features) {
    bool result =
        features & WF_LAYOUT_CENTER ||
        features & WF_LAYOUT_HORIZONTAL ||
        features & WF_LAYOUT_VERTICAL ||
    false;
    return result;
}


struct Widget {
    Widget *next;       // sibling in the branch
    Widget *first;      // child sub-branch first
    Widget *parent;     // parent of the branch

    u64 hash_key;       // hash for frame-boundary persistence
    u64 frame_touched;  // expiration date

    f32 x0;
    f32 y0;
    f32 w;
    f32 h;
    f32 w_max;
    f32 h_max;

    f32 w_child_sum;
    f32 w_child_max;
    f32 h_child_sum;
    f32 h_child_max;

    u32 features_flg;
    u32 alignment_flg;

    // panels / labels
    Str text;
    FontSize sz_font;
    s32 sz_border;
    Color col_bckgrnd;
    Color col_text;
    Color col_border;
    u32 padding;

    // cursor interaction
    Color col_hot;
    Color col_active;
    bool hot;
    bool active;
    bool clicked;
    CollRect rect;

    Str DBG_tag;

    void CollRectClear() {
        rect = {};
    }
    void SetCollisionRectUsingX0Y0WH() {
        rect.x0 = x0;
        rect.x1 = x0 + w;
        rect.y0 = y0;
        rect.y1 = y0 + h;
    }
    void SetFlag(WidgetFlags f) {
        features_flg = features_flg |= f;
    }
};

void WidgetSetFlag(Widget *wgt, u32 flag) {
    wgt->features_flg |= flag;
}


//
//  Core


static MArena _g_a_imui;
static MArena *g_a_imui;
static MPoolT<Widget> _g_p_widgets;
static MPoolT<Widget> *g_p_widgets;
static Stack<Widget*> _g_s_widgets;
static Stack<Widget*> *g_s_widgets;

static HashMap _g_m_widgets;
static HashMap *g_m_widgets;

static Widget _g_w_root;
static Widget *g_w_layout;
static Widget *g_w_active;

static u64 *g_frameno_imui;


void WidgetTreeSibling(Widget *w) {
    if (g_w_layout->first != NULL) {
        Widget *sib = g_w_layout->first;
        while (sib->next != NULL) {
            sib = sib->next;
        }
        sib->next = w;
        w->parent = sib->parent;
    }
    else {
        g_w_layout->first = w;
        w->parent = g_w_layout;
    }
}

void WidgetTreeBranch(Widget *w) {
    if (g_w_layout->first != NULL) {
        Widget *sib = g_w_layout->first;
        while (sib->next != NULL) {
            sib = sib->next;
        }
        sib->next = w;
    }
    else {
        g_w_layout->first = w;
    }
    w->parent = g_w_layout;
    g_w_layout = w;
}

void WidgetTreePop() {
    Widget *parent = g_w_layout->parent;
    if (parent != NULL) {
        g_w_layout = parent;
    }
}


static bool g_ui_debugmode;
static bool g_ui_debugnames;

void UI_DebugMode(bool enable) {
    g_ui_debugmode = enable;
}

void UI_DebugNames(bool enable) {
    g_ui_debugnames = enable;
}


static f32 g_mouse_x;
static f32 g_mouse_y;
static bool g_mouse_down;
static bool g_mouse_pushed;
static bool g_mouse_coolided_last_frame;


void UI_Init(u32 width, u32 height, u64 *frameno) {
    if (g_a_imui != NULL) {
        assert(1 == 0 && "don't re-initialize imui");
    }
    else {
        g_frameno_imui = frameno;

        MArena _g_a_imui = ArenaCreate();
        g_a_imui = &_g_a_imui;

        u32 max_widgets = 1024;
        _g_p_widgets = PoolCreate<Widget>(max_widgets);
        g_p_widgets = &_g_p_widgets;

        _g_s_widgets = InitStack<Widget*>(g_a_imui, max_widgets);
        g_s_widgets = &_g_s_widgets;

        _g_m_widgets = InitMap(g_a_imui, max_widgets);
        g_m_widgets = &_g_m_widgets;

        _g_w_root = {};
        _g_w_root.features_flg |= WF_LAYOUT_HORIZONTAL;
        _g_w_root.w_max = width;
        _g_w_root.h_max = height,
        _g_w_root.x0 = 0;
        _g_w_root.y0 = 0;

        g_w_layout = &_g_w_root;
    }

    SetDefaultFontSize(FS_24);
}

void WidgetTreeSizeWrap_Rec(Widget *w, f32 *w_sum, f32 *h_sum, f32 *w_max, f32 *h_max) {
    // Recursively determines widget sizes by wrapping in child widgets. 
    // Sizes will be the minimal, and expander sizes will be expanded elsewhere.

    // There is an accumulated child size and a max child size.
    // Depending on the layou of the current widget, its actual size
    // is set to either the maximum child widget.
    // Or, if a panel has the WF_LAYOUT_H or WR_LAYOUT_V features, the sum of
    // each child's actual size.
    //
    // max & sum sizes are determined on descent, actual sizes are set on ascent.


    //
    // Descent: determine child_max and child_sum sizes


    *w_sum = 0;
    *h_sum = 0;
    *w_max = 0;
    *h_max = 0;

    Widget *ch = w->first;
    while (ch != NULL) {
        f32 w_sum_ch;
        f32 h_sum_ch;
        f32 w_max_ch;
        f32 h_max_ch;

        WidgetTreeSizeWrap_Rec(ch, &w_sum_ch, &h_sum_ch, &w_max_ch, &h_max_ch);

        if (ch->features_flg & WF_ABSREL_POSITION) {
            // do not count siblings with ABSREL
        }
        else {
            *w_sum += ch->w;
            *h_sum += ch->h;
            *w_max = MaxS32(*w_max, ch->w);
            *h_max = MaxS32(*h_max, ch->h);
        }

        ch = ch->next;
    }


    // Ascent: Assign actual size to current widget
    if (w->features_flg & WF_LAYOUT_CENTER) {
        if (w->w == 0 && !(w->features_flg & WF_EXPAND_HORIZONTAL)) { w->w = *w_max + 2*w->padding; }
        if (w->h == 0 && !(w->features_flg & WF_EXPAND_VERTICAL)) { w->h = *h_max + 2*w->padding; }
    }
    else if (w->features_flg & WF_LAYOUT_VERTICAL) {
        if (w->w == 0 && !(w->features_flg & WF_EXPAND_HORIZONTAL)) {
            w->w = MaxF32(w->w, *w_max) + 2*w->padding;
        }
        if (w->h == 0 && !(w->features_flg & WF_EXPAND_VERTICAL)) {
            w->h = *h_sum + 2*w->padding;
        }
    }
    else if (w->features_flg & WF_LAYOUT_HORIZONTAL) {
        if (w->w == 0 && !(w->features_flg & WF_EXPAND_HORIZONTAL)) {
            w->w = *w_sum + 2*w->padding;
        }
        if (w->h == 0 && !(w->features_flg & WF_EXPAND_VERTICAL)) {
            w->h = MaxF32(w->h, *h_max) + 2*w->padding;
        }
    }

    // or keep pre-sets
    else {
        *w_sum = w->w;
        *h_sum = w->h;
        *w_max = w->w;
        *h_max = w->h;
    }

    w->w_child_sum = *w_sum;
    w->w_child_max = *w_max;
    w->h_child_sum = *h_sum;
    w->h_child_max = *h_max;
}

void WidgetTreeExpanders_Rec(Widget *w) {
    Widget *ch = w->first;
    if (ch == NULL) {
        return;
    }


    // WARN: We can't use w->child_sum, because w might be a vertical layout !


    while (ch) {
        if (ch->features_flg & WF_EXPAND_VERTICAL) {
            if (!(w->features_flg & WF_LAYOUT_VERTICAL)) {
                ch->h = w->h - 2*w->padding;
            }
            else {
                ch->h = w->h - w->h_child_sum - 2*w->padding;
            }
        }

        if (ch->features_flg & WF_EXPAND_HORIZONTAL) {
            if (!(w->features_flg & WF_LAYOUT_HORIZONTAL)) {
                ch->w = w->w - 2*w->padding;
            }
            else {
                ch->w = w->w - w->w_child_sum - 2*w->padding;
            }
        }

        WidgetTreeExpanders_Rec(ch);

        ch = ch->next;
    }
}

List<Widget*> WidgetTreePositioningAndMouseInteraction(MArena *a_tmp, Widget *w_root) {
    List<Widget*> all_widgets = InitList<Widget*>(a_tmp, 0);
    Widget *w = w_root;

    while (w != NULL) {
        ArenaAlloc(a_tmp, sizeof(Widget*));
        all_widgets.Add(w);

        f32 pt_x = 0;
        f32 pt_y = 0;

        // with all widget sizes known, widgets can position their children
        Widget *ch = w->first;
        while (ch != NULL) { // iterate child widgets

            // set child position - if not absolutely positioned
            if ((ch->features_flg & WF_ABSREL_POSITION) == false) {

                ch->x0 = w->x0 + w->padding;
                ch->y0 = w->y0 + w->padding;

                if (w->features_flg & WF_LAYOUT_CENTER) {
                    ch->y0 = w->y0 + (w->h - ch->h) / 2;
                    ch->x0 = w->x0 + (w->w - ch->w) / 2;
                }

                else if (w->features_flg & WF_LAYOUT_HORIZONTAL) {
                    ch->x0 = w->x0 + w->padding + pt_x ;
                    pt_x += ch->w;

                    if (w->features_flg & WF_ALIGN_CENTER) {
                        ch->y0 = w->y0 + (w->h - ch->h) / 2;
                    }
                    else if (w->features_flg & WF_ALIGN_RIGHT_OR_BOTTOM) {
                        ch->y0 = w->y0 + (w->h - w->padding - ch->h);
                    }
                }

                else if (w->features_flg & WF_LAYOUT_VERTICAL) {
                    ch->y0 = w->y0 + w->padding + pt_y;
                    pt_y += ch->h;

                    if (w->features_flg & WF_ALIGN_CENTER) {
                        ch->x0 = w->x0 + (w->w - ch->w) / 2;
                    }
                    else if (w->features_flg & WF_ALIGN_RIGHT_OR_BOTTOM) {
                        ch->x0 = w->x0 + (w->w - w->padding - ch->w);
                    }
                }
            }

            if (ch->features_flg & WF_ABSREL_POSITION) {
                // basic offset wrt. parent
                if (ch->alignment_flg == 0) {
                    ch->x0 += w->x0 + w->padding;
                    ch->y0 += w->y0 + w->padding;
                }

                // alignment also specified
                else if (ch->alignment_flg & WA_TOP_LEFT) {
                    ch->x0 = w->x0 + ch->x0;
                    ch->y0 = w->y0 + ch->y0;
                }
                else if (ch->alignment_flg & WA_TOP_RIGHT) {
                    ch->x0 = w->x0 + ch->x0 + w->w - ch->w;
                    ch->y0 = w->y0 + ch->y0;
                }
                else if (ch->alignment_flg & WA_BOTTOM_LEFT) {
                    ch->x0 = w->x0 + ch->x0;
                    ch->y0 = w->y0 + ch->y0 + w->h - ch->h;
                }
                else if (ch->alignment_flg & WA_BOTTOM_RIGHT) {
                    ch->x0 = w->x0 + ch->x0 + w->w - ch->w;
                    ch->y0 = w->y0 + ch->y0 + w->h - ch->h;
                }
                else if (ch->alignment_flg & WA_CENTV_LEFT) {
                    ch->x0 = w->x0 + ch->x0;
                    ch->y0 = w->y0 + ch->y0 + (w->h - ch->h) / 2;
                }
                else if (ch->alignment_flg & WA_CENTV_RIGHT) {
                    ch->x0 = w->x0 + ch->x0 + w->w - ch->w;
                    ch->y0 = w->y0 + ch->y0 + (w->h - ch->h) / 2;
                }
                else if (ch->alignment_flg & WA_TOP_CENTH) {
                    ch->x0 = w->x0 + ch->x0 + (w->w - ch->w) / 2;
                    ch->y0 = w->y0 + ch->y0;
                }
                else if (ch->alignment_flg & WA_BOTTOM_CENTH) {
                    ch->x0 = w->x0 + ch->x0 + (w->w - ch->w) / 2;
                    ch->y0 = w->y0 + ch->y0 + w->h - ch->h;
                }
                else if (ch->alignment_flg & WA_CENTER) {
                    ch->x0 = w->x0 + ch->x0 + (w->w - ch->w) / 2;
                    ch->y0 = w->y0 + ch->y0 + (w->h - ch->h) / 2;
                }
            }


            // set the collision rect for next frame code-interleaved mouse collision
            ch->SetCollisionRectUsingX0Y0WH();

            ch->hot = false;
            ch->clicked = false;

            if (ch->features_flg & WF_CAN_COLLIDE || ch->features_flg & WF_DRAW_BACKGROUND_AND_BORDER) {
                if (ch->rect.DidCollide( (s32) g_mouse_x, (s32) g_mouse_y )) {
                    g_mouse_coolided_last_frame = true;
                }
            }

            if (ch->features_flg & WF_CAN_COLLIDE) {
                if (ch->rect.DidCollide( (s32) g_mouse_x, (s32) g_mouse_y )) {
                    ch->hot = true;

                    if (g_w_active == NULL) {
                        if (g_mouse_down) {
                            // enable active mode
                            g_w_active = ch;
                            ch->active = true;
                        }
                    }
                    else if (g_w_active != ch) {
                        // disable active mode
                        ch->active = false;
                    }
                }
                else {
                    ch->hot = false;
                }

                if (g_mouse_down == false) {
                    ch->active = false;
                    g_w_active = NULL; // this should be set in a more global place, and not in every iteration
                }

                if (g_mouse_pushed && ch->active) {
                    ch->clicked = true;
                }
            }

            // iter
            ch = ch->next;
        }

        // iter
        if (w->first != NULL) {
            if (w->next) {
                g_s_widgets->Push(w->next);
            }
            w = w->first;
        }
        else if (w->next) {
            w = w->next;
        }
        else {
            w = g_s_widgets->Pop();
        }
    }

    return all_widgets;
}

void WidgetTreeRenderToDrawcalls(List<Widget*> all_widgets) {


    // TODO: This should iterate the tree, and do something meaningful to figure out
    //      what should be rendered on top of what - proper interleaving of calls.


    for (u32 i = 0; i < all_widgets.len; ++i) {
        Widget *w = all_widgets.lst[i];

        if (g_ui_debugmode) {
            PanelPlot(w->x0, w->y0, w->w, w->h, 1, COLOR_BLACK, COLOR_WHITE);
        }
        else if (w->features_flg & WF_DRAW_BACKGROUND_AND_BORDER) {
            PanelPlot(w->x0, w->y0, w->w, w->h, w->sz_border, w->col_border, w->col_bckgrnd);
        }

        if (w->features_flg & WF_DRAW_TEXT) {
            UI_SetFontSize(w->sz_font);
            s32 w_out;
            s32 h_out;

            TextPlot(w->text, w->x0, w->y0, w->w, w->h, &w_out, &h_out, w->col_text);
        }
        else if (g_ui_debugnames) {
            s32 sz_x;
            s32 sz_y;
            TextPlot(w->DBG_tag, w->x0, w->y0, w->w, w->h, &sz_x, &sz_y, COLOR_BLACK);
        }

    }
}


void UI_FrameEnd(MArena *a_tmp, s32 width, s32 height, f32 mouse_x, f32 mouse_y, bool mouse_down, bool mouse_pushed) {
    g_mouse_x = mouse_x;
    g_mouse_y = mouse_y;
    g_mouse_down = mouse_down;
    g_mouse_pushed = mouse_pushed;
    g_mouse_coolided_last_frame = false;

    if (g_mouse_down == false) {
        g_w_active = NULL;
    }

    Widget *w = &_g_w_root;
    w->w_max = width;
    w->h_max = height;
    w->w = w->w_max;
    w->h = w->h_max;
    //w->DBG_tag = StrL("root");

    // size widgets to wrap tightly
    f32 w_sum_ch;
    f32 h_sum_ch;
    f32 w_max_ch;
    f32 h_max_ch;
    WidgetTreeSizeWrap_Rec(w, &w_sum_ch, &h_sum_ch, &w_max_ch, &h_max_ch);

    // size expanders
    WidgetTreeExpanders_Rec(w);


    // TODO: merge positioning and render passes:

    // position pass
    List<Widget*> all_widgets = WidgetTreePositioningAndMouseInteraction(a_tmp, w);
    // render pass
    WidgetTreeRenderToDrawcalls(all_widgets);


    // clean up pass
    _g_w_root.frame_touched = *g_frameno_imui;
    g_w_layout = &_g_w_root;
    for (u32 i = 0; i < all_widgets.len; ++i) {
        Widget *w = all_widgets.lst[i];

        // prune
        if (w->frame_touched < *g_frameno_imui) {
            s64 rm = MapRemove(g_m_widgets, w->hash_key);
            assert(rm >= 0);
            g_p_widgets->Free(w);
        }
        // save
        else {
            if (w->hash_key != 0) {
                MapPut(g_m_widgets, w->hash_key, w);
            }
            w->parent = NULL;
            w->first = NULL;
            w->next = NULL;
        }
    }
}


//
//  Builder API


Widget *WidgetGetCached(const char *text, bool *was_new = NULL) {
    u64 key = HashStringValue(text);
    Widget *w = (Widget*) MapGet(g_m_widgets, key);

    if (w == NULL) {
        w = g_p_widgets->Alloc();
        MapPut(g_m_widgets, key, w);
        w->hash_key = key;

        w->text = Str { (char*) text, (u32) strlen( (char*) text) };
        if (was_new) *was_new = true;
    }
    else {
        assert(key == w->hash_key);
        assert(w->frame_touched != *g_frameno_imui && "getting the same widget twice");
        if (was_new) *was_new = false;
    }

    w->frame_touched = *g_frameno_imui;

    return w;
}

Widget *WidgetGetNew(const char *text = NULL) {
    Widget *w = g_p_widgets->Alloc();
    assert(w->frame_touched == 0);
    if (text) {
        w->text = Str { (char*) text, (u32) strlen( (char*) text) };
    }

    return w;
}

bool UI_Button(const char *text, Widget **w_out = NULL, bool deactivated = false) {
    Widget *w  = WidgetGetCached(text);
    w->features_flg |= WF_DRAW_TEXT;
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_CAN_COLLIDE;
    w->w = 120;
    w->h = 50;
    w->sz_font = FS_24;

    if (deactivated) {
        w->sz_border = 1;
        w->col_bckgrnd = COLOR_WHITE;
        w->col_text = COLOR_GRAY;
        w->col_border = COLOR_BLACK;
    }
   else {
        if (w->active) {
            w->sz_border = 3;
            w->col_bckgrnd = COLOR_GRAY_75;
            w->col_text = COLOR_BLACK;
            w->col_border = COLOR_BLACK;
        }
        else if (w->hot) {
            w->sz_border = 3;
            w->col_bckgrnd = COLOR_WHITE;
            w->col_text = COLOR_BLACK;
            w->col_border = COLOR_BLACK;
        }
        else {
            w->sz_border = 1;
            w->col_bckgrnd = COLOR_WHITE;
            w->col_text = COLOR_BLACK;
            w->col_border = COLOR_BLACK;
        }
    }
 
    WidgetTreeSibling(w);

    if (w_out != NULL) {
        *w_out = w;
    }
    return w->clicked;
}

bool UI_ToggleButton(const char *text, bool *state, Widget **w_out = NULL) {
    Widget *w  = WidgetGetCached(text);
    w->features_flg |= WF_DRAW_TEXT;
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_CAN_COLLIDE;
    w->w = 120;
    w->h = 50;
    w->sz_font = FS_24;

    if (w->clicked) {
        *state = !(*state);
    }

    if (*state == true) {
        w->sz_border = 3;
        w->col_bckgrnd = ColorGray(0.8f);
        w->col_text = ColorBlack();
        w->col_border = ColorBlack();
    }
    else {
        w->sz_border = 1;
        w->col_bckgrnd = COLOR_WHITE;
        w->col_text = ColorBlack();
        w->col_border = ColorBlack();
    }

    WidgetTreeSibling(w);

    if (w_out != NULL) {
        *w_out = w;
    }
    return w->clicked;
}

bool UI_ToggleTabButton(const char *text, bool *state, Widget **w_out = NULL) {
    Widget *w  = WidgetGetCached(text);
    w->features_flg |= WF_DRAW_TEXT;
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_CAN_COLLIDE;

    w->col_text = ColorBlack();
    w->col_border = ColorBlack();
    w->sz_font = UI_GetFontSize();
    w->w = TextLineWidth(g_current_font, w->text) + 2;
    w->h = g_current_font->ln_measured + 4;

    if (w->clicked) {
        *state = !(*state);
    }

    if (*state == true) {
        w->sz_border = 1;
        w->col_bckgrnd = COLOR_WHITE;
    }
    else {
        w->sz_border = 1;
        w->col_bckgrnd = COLOR_GRAY_80;
    }

    WidgetTreeSibling(w);

    if (w_out != NULL) {
        *w_out = w;
    }
    return w->clicked;
}

Widget *UI_CoolPanel(s32 width, s32 height, bool center_h = true) {
    Widget *w = WidgetGetNew();
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_LAYOUT_VERTICAL;
    if (center_h) {
        w->features_flg |= WF_ALIGN_CENTER;
    }
    w->w = width;
    w->h = height;
    w->sz_border = 20;
    w->col_bckgrnd = ColorGray(0.9f);
    w->col_border = ColorGray(0.7f);

    WidgetTreeBranch(w);

    return w;
}

bool UI_CrossButton(const char *symbol, Widget **w_out = NULL) {
    Widget *x = WidgetGetCached(symbol);
    if (w_out) *w_out = x;

    x->features_flg |= WF_ABSREL_POSITION;
    x->features_flg |= WF_DRAW_TEXT;
    x->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    x->features_flg |= WF_CAN_COLLIDE;
    x->alignment_flg |= WA_TOP_RIGHT;
    x->sz_border = 1;
    x->col_border = COLOR_BLACK;
    x->col_bckgrnd = COLOR_WHITE;
    if (x->hot) {
        x->col_bckgrnd = COLOR_GRAY_75;
    }
    x->col_text = COLOR_BLACK;
    x->text = Str { (char*) symbol, 1 };
    x->sz_font = FS_18;
    x->w = 25;
    x->h = 25;

    WidgetTreeSibling(x);

    return x->clicked;
}

Widget *UI_CoolPopUp(s32 width, s32 height, s32 padding = 20, bool *close = NULL) {
    Widget *w = WidgetGetNew();
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_LAYOUT_CENTER;

    w->w = width;
    w->h = height;
    w->sz_border = 20;
    w->col_bckgrnd = ColorGray(0.9f);
    w->col_border = ColorGray(0.7f);

    WidgetTreeBranch(w);

    Widget *x;
    bool cross_clicked = UI_CrossButton("x", &x);
    if (close) *close = cross_clicked;
    x->w = 18;
    x->h = 18;
    x->x0 = -1;
    x->y0 = 1;
    x->sz_border = 0;
    if (x->hot) {
        x->col_bckgrnd = COLOR_GRAY_50;
    }
    else {
        x->col_bckgrnd = w->col_border;
    }

    Widget *i = WidgetGetNew();
    i->features_flg |= WF_LAYOUT_VERTICAL;
    i->w = width - padding * 2;
    i->h = height - padding * 2;

    WidgetTreeBranch(i);

    return i;
}

Widget *UI_Branch() {
    Widget *w = WidgetGetNew();

    WidgetTreeBranch(w);
    return w;
}

Widget *UI_Sibling() {
    Widget *w = WidgetGetNew();

    WidgetTreeSibling(w);
    return w;
}

Widget *UI_Center() {
    Widget *w = WidgetGetNew();

    w->features_flg |= WF_EXPAND_VERTICAL;
    w->features_flg |= WF_EXPAND_HORIZONTAL;
    w->features_flg |= WF_LAYOUT_CENTER;

    WidgetTreeBranch(w);
    return w;
}

Widget *UI_LayoutHorizontal(s32 align = 1) {
    Widget *w = WidgetGetNew();
    w->features_flg |= WF_LAYOUT_HORIZONTAL;

    if (align == 0) {
        w->features_flg |= WF_ALIGN_CENTER;
    }
    else if (align == -1) {
        w->features_flg |= WF_ALIGN_RIGHT_OR_BOTTOM;
    }

    WidgetTreeBranch(w);
    return w;
}

Widget *UI_LayoutVertical(s32 align = 1) {
    Widget *w = WidgetGetNew();
    w->features_flg |= WF_LAYOUT_VERTICAL;

    if (align == 0) {
        w->features_flg |= WF_ALIGN_CENTER;
    }
    else if (align == -1) {
        w->features_flg |= WF_ALIGN_RIGHT_OR_BOTTOM;
    }

    WidgetTreeBranch(w);
    return w;
}

Widget *UI_SpaceH(u32 width) {
    Widget *w = WidgetGetNew();
    w->w = width;

    WidgetTreeSibling(w);
    return w;
}

Widget *UI_SpaceV(u32 height) {
    Widget *w = WidgetGetNew();
    w->h = height;

    WidgetTreeSibling(w);
    return w;
}

Widget *UI_ExpanderH() {
    Widget *w = WidgetGetNew();
    w->features_flg |= WF_EXPAND_HORIZONTAL;

    WidgetTreeSibling(w);
    return w;
}

Widget *UI_ExpanderV() {
    Widget *w = WidgetGetNew();
    w->features_flg |= WF_EXPAND_VERTICAL;

    WidgetTreeSibling(w);
    return w;
}

Widget *UI_Label(const char *text, Color color = Color { RGBA_BLACK }) {
    Widget *w = WidgetGetNew(text);
    w->features_flg |= WF_DRAW_TEXT;

    w->sz_font = UI_GetFontSize();
    w->col_bckgrnd = ColorGray(0.9f);
    w->col_border = ColorBlack();
    w->col_text = color;

    w->w = TextLineWidth(g_current_font, w->text);
    w->h = g_current_font->ln_measured;

    WidgetTreeSibling(w);
    return w;
}

void UI_Pop() {
    WidgetTreePop();
}

bool UI_DidCollide() {
    return g_mouse_coolided_last_frame;
}

void (UI_SetCurrentLayout(Widget *w)) {
    // TODO: we should have a check here

    g_w_layout = w;
}


#endif


#ifndef __PLATFORM_GLFW_H__
#define __PLATFORM_GLFW_H__


#include <GL/glew.h>
#include <GLFW/glfw3.h>


//
//  OpenGL


void CheckShaderCompilationErrors(GLuint shader, const char *header_info) {
    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        printf("%s:%s\n", header_info, info_log);
    }
}

void CheckShaderLinkErrors(GLuint program, const char *header_info) {
    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, info_log);
        printf("%s:%s\n", header_info, info_log);
    }
}

void ShaderProgramLink(GLuint *program, const GLchar *vsh_src, const GLchar *fsh_src, const GLchar *frag_data_loc = "o_color") {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsh_src, NULL);
    glCompileShader(vs);
    CheckShaderCompilationErrors(vs, "vertex shader compilation error");

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsh_src, NULL);
    glCompileShader(fs);
    CheckShaderCompilationErrors(fs, "fragment shader compilation error");

    *program = glCreateProgram();
    glAttachShader(*program, vs);
    glAttachShader(*program, fs);
    glBindFragDataLocation(*program, 0, frag_data_loc);
    glLinkProgram(*program);
    CheckShaderLinkErrors(*program, "shader program link error");

    glDeleteShader(vs);
    glDeleteShader(fs);
    glUseProgram(*program);
}

struct ScreenProgram {
    // draws a texture to the screen
    GLuint program;
    GLuint vao;
    GLuint vbo;
    GLuint texture_id;

    const GLchar* vert_src = R"glsl(
        #version 330 core

        in vec2 position;
        in vec2 tex_coord;
        out vec2 coord;

        void main()
        {
            gl_Position = vec4(position, 0.0, 1.0);
            coord = tex_coord;
        }
    )glsl";
    const GLchar* frag_src = R"glsl(
        #version 330 core

        in vec2 coord;
        out vec4 o_color;
        uniform sampler2D sampler;

        void main()
        {
            o_color = texture(sampler, coord);
        }
    )glsl";

};

void ScreeProgramDraw(ScreenProgram *p, u8* imgbuffer, u32 width, u32 height) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(p->program);
    glBindVertexArray(p->vao);
    glBindBuffer(GL_ARRAY_BUFFER, p->vbo);

    glBindTexture(GL_TEXTURE_2D, p->texture_id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imgbuffer);

    u32 nverts = 4;
    glDrawArrays(GL_TRIANGLE_STRIP, 0, nverts);
    glBindVertexArray(0);
}

void ScreenProgramSetSize(u8* imgbuffer, u32 width, u32 height) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgbuffer);
    glViewport(0, 0, width, height);
}

ScreenProgram ScreenProgramInit(u8* imgbuffer, u32 width, u32 height) {
    ScreenProgram prog = {};

    ShaderProgramLink(&prog.program, prog.vert_src, prog.frag_src);
    glGenVertexArrays(1, &prog.vao);
    glBindVertexArray(prog.vao);

    // texture
    glGenTextures(1, &prog.texture_id);
    glBindTexture(GL_TEXTURE_2D, prog.texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUseProgram(prog.program);
    glBindVertexArray(prog.vao);
    glBindBuffer(GL_ARRAY_BUFFER, prog.vbo);
    ScreenProgramSetSize(imgbuffer, width, height);

    // quad
    float sqreen_quad_verts[] = {
        1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 1.0f
    };
    u32 stride = 4;
    u32 nverts = 4;
    glGenBuffers(1, &prog.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, prog.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * stride * nverts, &sqreen_quad_verts, GL_STATIC_DRAW);

    GLint pos_attr = glGetAttribLocation(prog.program, "position");
    glVertexAttribPointer(pos_attr, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), 0);
    glEnableVertexAttribArray(pos_attr);
    GLint tex_attr = glGetAttribLocation(prog.program, "tex_coord");
    glVertexAttribPointer(tex_attr, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*) (2 * sizeof(float)));
    glEnableVertexAttribArray(tex_attr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return prog;
}


//
//  glfw


struct MousePosition {
    f32 x;
    f32 y;
    f32 dx;
    f32 dy;
    f32 x_frac; // [-1, 1]
    f32 y_frac; // [-1, 1]
    f32 dx_frac;
    f32 dy_frac;
};

struct Button {
    u64 t_pushed; // used for calculating "click" timeout @ release
    u64 t_pushed_prev; // used for calculating "double-click" timeout @ release
    u32 pushes;
    bool dblclicked;
    bool clicked;
    bool pushed;
    bool released;
    bool ended_down;

    void FrameReset() {
        u64 t_pushed_cpy = t_pushed;
        u64 t_pushed_prev_cpy = t_pushed_prev;
        *this = {};
        t_pushed = t_pushed_cpy;
        t_pushed_prev = t_pushed_prev_cpy;
    }
};

struct Scroll {
    double yoffset_acc;
    u32 steps_down;
    u32 steps_up;
};

struct AsciiKeys {
    u8 keys_cnt;
    u8 keys_idx;
    char keys[32]; // max 32 keystrokes per frame ...

    void Put(char c) {
        if (keys_cnt < 16) {
            keys[keys_cnt++] = c;
        }
    }
    char Get(s32 *mods = NULL) {
        if (keys_cnt && (keys_idx < keys_cnt)) {
            char c = keys[keys_idx++];
            return c;
        }
        else {
            return 0;
        }
    }
};

struct ActionKeys {
    bool esc;
    bool tab;
    bool enter;
    bool backspace;
    bool del;
    bool space;
    bool left;
    bool right;
    bool up;
    bool down;
    bool mod_ctrl;
    bool mod_shift;
    bool mod_alt;
    u8 fkey;

    void ResetButKeepMods() {
        bool _mod_ctrl = mod_ctrl;
        bool _mod_shift = mod_shift;
        bool _mod_alt = mod_alt;
        *this = {};
        this->mod_ctrl = _mod_ctrl;
        this->mod_shift = _mod_shift;
        this->mod_alt = _mod_alt;
    }
};


struct PlafGlfw {
    GLFWwindow* window;
    bool fullscreen;
    char *title;

    MousePosition cursorpos;
    Button left;
    Button right;
    Scroll scroll;

    AsciiKeys keys;
    ActionKeys akeys;

    ScreenProgram screen;
    u32 width;
    u32 height;
    u32 width_cache;
    u32 height_cache;
    s32 window_xpos;
    s32 window_ypos;
    u8 *image_buffer;
};


inline PlafGlfw *_GlfwWindowToUserPtr(GLFWwindow* window) {
    PlafGlfw *plaf = (PlafGlfw*) glfwGetWindowUserPointer(window);
    return plaf;
}


#define T_CLICK_TIMEOUT_MYS 500000 // 500 ms
#define T_DBLCLICK_TIMEOUT_MYS 500000 // 500 ms
void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods) {
    PlafGlfw *plaf = _GlfwWindowToUserPtr(window);

    // get button
    Button *btn = NULL;
    if (button == GLFW_MOUSE_BUTTON_1) {
        btn = &plaf->left;
    }
    else if (button == GLFW_MOUSE_BUTTON_2) {
        btn = &plaf->right;
    }

    // set event
    if (action == GLFW_PRESS) {
        btn->pushed = true;
        btn->ended_down = true;

        // double-click @ mouse-down
        btn->t_pushed_prev = btn->t_pushed;
        btn->t_pushed = ReadSystemTimerMySec();
        u64 t_since_last_mdown = btn->t_pushed - btn->t_pushed_prev;

        if (btn->t_pushed_prev != 0 && (btn->t_pushed_prev > btn->t_pushed)) {
            //  TODO: use steady_clock
            //  In the event of a time skip (this used to be an assert)
            btn->t_pushed_prev = btn->t_pushed;
            return;
        }

        if (btn->t_pushed_prev && (t_since_last_mdown < T_DBLCLICK_TIMEOUT_MYS)) {
            btn->dblclicked = true;
            btn->t_pushed = 0;
            btn->t_pushed_prev = 0;
        }
    }
    else if (action == GLFW_RELEASE) {
        btn->released = true;
        btn->pushes++;

        // click @ mouse-up
        u64 t_released = ReadSystemTimerMySec();
        u64 t_since_last_mdown = t_released - btn->t_pushed;

        if (btn->t_pushed_prev != 0 && (btn->t_pushed_prev > btn->t_pushed)) {
            //  TODO: use steady_clock
            //  In the event of a time skip (this used to be an assert)
            btn->t_pushed_prev = btn->t_pushed;
            return;
        }

        if (btn->t_pushed && (t_since_last_mdown < T_CLICK_TIMEOUT_MYS)) {
            btn->clicked = true;
        }
    }
}

void MouseScrollCallBack(GLFWwindow* window, double xoffset, double yoffset) {
    PlafGlfw *plaf = _GlfwWindowToUserPtr(window);

    plaf->scroll.yoffset_acc += yoffset;
    if (yoffset > 0) {
        plaf->scroll.steps_up++;
    }
    else if (yoffset < 0) {
        plaf->scroll.steps_down++;
    }
}

void CharCallBack(GLFWwindow* window, u32 codepoint) {
    PlafGlfw *plf = _GlfwWindowToUserPtr(window);

    if (codepoint >= 0 && codepoint < 128) {
        char c = (u8) codepoint;
        plf->keys.Put(c);
    }
}

void KeyCallBack(GLFWwindow* window,  int key, int scancode, int action, int mods) {
    PlafGlfw *plf = _GlfwWindowToUserPtr(window);

    if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_LEFT_CONTROL) {
        plf->akeys.mod_ctrl = (action == GLFW_PRESS);
    }
    if (key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) {
        plf->akeys.mod_alt = (action == GLFW_PRESS);
    }
    if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
        plf->akeys.mod_shift = (action == GLFW_PRESS);
    }

    if (action == GLFW_PRESS) {
        if (key == 256) {
            plf->akeys.esc = true;
        }
        if (key == GLFW_KEY_TAB) {
            plf->akeys.tab = true;
        }
        else if (key == 257) {
            plf->akeys.enter = true;
        }
        else if (key == 259) {
            plf->akeys.backspace = true;
        }
        else if (key == 261) {
            plf->akeys.del = true;
        }
        else if (key == ' ') {
            plf->akeys.space = true;
        }
        else if (key == GLFW_KEY_LEFT) {
            plf->akeys.left = true;
        }
        else if (key == GLFW_KEY_RIGHT) {
            plf->akeys.right = true;
        }
        else if (key == GLFW_KEY_UP) {
            plf->akeys.up = true;
        }
        else if (key == GLFW_KEY_DOWN) {
            plf->akeys.down = true;
        }
        else if (key >= 290 && key <= 301) {
            // 290-301: F1 through F12
            plf->akeys.fkey = key - 289;
        }

        else if (key == 'C' && mods == GLFW_MOD_CONTROL) {
            printf("ctr-C\n");
        }
        else if (key == 'X' && mods == GLFW_MOD_CONTROL) {
            printf("ctr-X\n");
        }
        else if (key == 'Z' && mods == GLFW_MOD_CONTROL) {
            printf("ctr-Z\n");
        }
    }
}

void WindowResizeCallBack(GLFWwindow* window, int width, int height) {
    PlafGlfw *plf = _GlfwWindowToUserPtr(window);

    plf->width = width;
    plf->height = height;
    ScreenProgramSetSize(plf->image_buffer, width, height);
}


static PlafGlfw *g_plaf_glfw;
void PlafGlfwInit(PlafGlfw *plf, const char *title, u32 window_width, u32 window_height, u8* image_buffer) {
    *plf = {};
    plf->width = window_width;
    plf->height = window_height;
    plf->title = (char*) title;

    glfwInit();

    // opengl window & context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    plf->window = glfwCreateWindow(plf->width, plf->height, title, NULL, NULL);
    glfwMakeContextCurrent(plf->window);

    // glew
    glewExperimental = GL_TRUE;
    glewInit();

    // alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // input
    glfwSetCharCallback(plf->window, CharCallBack); // NOTE: potentially use glfwSetCharModsCallback to additionally get the mods
    glfwSetKeyCallback(plf->window, KeyCallBack);
    glfwSetMouseButtonCallback(plf->window, MouseButtonCallBack);
    glfwSetScrollCallback(plf->window, MouseScrollCallBack);
    glfwSetWindowUserPointer(plf->window, plf);

    // window resize
    glfwSetFramebufferSizeCallback(plf->window, WindowResizeCallBack);

    // shader
    plf->image_buffer = image_buffer;
    plf->screen = ScreenProgramInit(plf->image_buffer, plf->width, plf->height);

    // initialize mouse position values (dx and dy are initialized to zero)
    f64 mouse_x;
    f64 mouse_y;
    glfwGetCursorPos(plf->window, &mouse_x, &mouse_y);
    plf->cursorpos.x = (f32) mouse_x;
    plf->cursorpos.y = (f32) mouse_y;
    plf->cursorpos.x_frac = ((f32) mouse_x - (plf->width * 0.5f)) / plf->width;
    plf->cursorpos.y_frac = ((f32) mouse_y - (plf->height * 0.5f)) / plf->height;

    g_plaf_glfw = plf;
}

void PlafGlfwTerminate(PlafGlfw* plf) {
    glfwDestroyWindow(plf->window);
    glfwTerminate();
}

void PlafGlfwToggleFullscreen(PlafGlfw* plf) {
    plf->fullscreen = !plf->fullscreen;
    if (plf->fullscreen) {
        assert(plf->width_cache == 0);
        assert(plf->height_cache == 0);

        plf->width_cache = plf->width;
        plf->height_cache = plf->height;
        glfwGetWindowPos(plf->window, &plf->window_xpos, &plf->window_ypos);

        GLFWmonitor *monitor = glfwGetWindowMonitor(plf->window);

        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        plf->width = mode->width;
        plf->height = mode->height;

        glfwSetWindowMonitor(plf->window, monitor, 0, 0, plf->width, plf->height, GLFW_DONT_CARE);
    }
    else {
        plf->width = plf->width_cache;
        plf->height = plf->height_cache;

        plf->width_cache = 0;
        plf->height_cache = 0;

        // doesn't get us back into windowed
        //glfwSetWindowMonitor(plf->window, NULL, 0, 0, 0, 0, GLFW_DONT_CARE);
        // TODO: try creating a "windowed full screen" mode switch

        // destroy and re-create everything (!?!)
        glfwDestroyWindow(plf->window);
        glfwTerminate();
        PlafGlfwInit(plf, plf->title, plf->width, plf->height, plf->image_buffer);
    }

    ScreenProgramSetSize(plf->image_buffer, plf->width, plf->height);
}

void PlafGlfwPushBuffer(PlafGlfw* plf) {
    ScreeProgramDraw(&plf->screen, plf->image_buffer, plf->width, plf->height);
    glfwSwapBuffers(plf->window);
}

void PlafGlfwUpdate(PlafGlfw* plf) {
    if (plf->akeys.fkey == 10) {
        // toggle fullscreen

        PlafGlfwToggleFullscreen(plf);
    }

    plf->left.FrameReset();
    plf->right.FrameReset();
    plf->scroll = {};
    plf->keys = {};
    plf->akeys.ResetButKeepMods();

    plf->left.ended_down = (glfwGetMouseButton(plf->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS);
    plf->right.ended_down = (glfwGetMouseButton(plf->window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS);

    f64 mouse_x;
    f64 mouse_y;
    glfwGetCursorPos(plf->window, &mouse_x, &mouse_y);
    
    plf->cursorpos.dx = (f32) mouse_x - plf->cursorpos.x;
    plf->cursorpos.dy = (f32) mouse_y - plf->cursorpos.y;
    plf->cursorpos.x = (f32) mouse_x;
    plf->cursorpos.y = (f32) mouse_y;

    f32 x_frac = ((f32) mouse_x - plf->width * 0.5f) / plf->width;
    f32 y_frac = ((f32) mouse_y - plf->height * 0.5f) / plf->height;
    plf->cursorpos.dx_frac = plf->cursorpos.x_frac - x_frac;
    plf->cursorpos.dy_frac = plf->cursorpos.y_frac - y_frac;
    plf->cursorpos.x_frac = x_frac;
    plf->cursorpos.y_frac = y_frac;

    glfwPollEvents();
}


inline Button MouseLeft() { return g_plaf_glfw->left; }
inline Button MouseRight() { return g_plaf_glfw->right; }
inline Scroll MouseScroll() { return g_plaf_glfw->scroll; }
inline Vector2f MouseFrac() { return { g_plaf_glfw->cursorpos.x_frac, g_plaf_glfw->cursorpos.y_frac }; }
inline Vector2f CurserPos() { return { g_plaf_glfw->cursorpos.x, g_plaf_glfw->cursorpos.y }; }
inline Vector2f MouseFracDelta() { return { (f32) g_plaf_glfw->cursorpos.dx / g_plaf_glfw->width, (f32) g_plaf_glfw->cursorpos.dy / g_plaf_glfw->height }; }
inline char GetChar() { return g_plaf_glfw->keys.Get(); }
inline bool GetEscape() { return g_plaf_glfw->akeys.esc; }
inline bool GetTab() { return g_plaf_glfw->akeys.tab; }
inline bool GetEnter() { return g_plaf_glfw->akeys.enter; }
inline bool GetSpace() { return g_plaf_glfw->akeys.space; }
inline bool GetBackspace() { return g_plaf_glfw->akeys.backspace; }
inline bool GetDelete() { return g_plaf_glfw->akeys.del; }
inline bool GetLeft() { return g_plaf_glfw->akeys.left; }
inline bool GetRight() { return g_plaf_glfw->akeys.right; }
inline bool GetUp() { return g_plaf_glfw->akeys.up; }
inline bool GetDown() { return g_plaf_glfw->akeys.down; }

inline bool GetFKey(u32 *fval) {
    assert(fval != NULL);

    u8 fkey = g_plaf_glfw->akeys.fkey;
    if (fkey == 0) {
        return false;
    }
    else {
        *fval = fkey;
        return true;
    }
}

inline bool GetFKey(u32 fval) {
    if (g_plaf_glfw->akeys.fkey == fval) {
        return true;
    }
    else {
        return false;
    }
}

inline bool GetChar(char c) {
    for (s32 i = 0; i < g_plaf_glfw->keys.keys_cnt; ++i) {
        char key = g_plaf_glfw->keys.keys[i];
        if (key == c) {
            return true;
        }
    }
    return false;
}

inline bool ModCtrl() { return g_plaf_glfw->akeys.mod_ctrl; }
inline bool ModShift() { return g_plaf_glfw->akeys.mod_shift; }
inline bool ModAlt() { return g_plaf_glfw->akeys.mod_alt; }

bool GetWindowShouldClose(PlafGlfw *plf) { return glfwWindowShouldClose(plf->window); }


#endif


#ifndef __CBUI_INIT_H__
#define __CBUI_INIT_H__


#define IMG_BUFF_CHANNELS 4
#define IMG_BUFF_MAX_WIDTH 3840
#define IMG_BUFF_MAX_HEIGHT 2160


//
//  UI core state variables


struct CbuiState {
    MContext *ctx;
    u64 frameno;
    u64 dts[8];
    u64 t_framestart;
    u64 t_framestart_prev;
    f32 dt;
    f32 fr;
    bool running;

    HashMap map_textures;
    HashMap map_fonts;

    u8 *image_buffer;
    PlafGlfw plf;

    f32 TimeSince(f32 t) {
        return t_framestart - t; 
    }
};

static CbuiState cbui;

CbuiState *CbuiInit(const char *title, bool start_in_fullscreen) {
    MContext *ctx = InitBaselayer();

    cbui = {};
    cbui.running = true;
    cbui.image_buffer = (u8*) ArenaAlloc(ctx->a_life, IMG_BUFF_CHANNELS * IMG_BUFF_MAX_WIDTH * IMG_BUFF_MAX_HEIGHT);
    cbui.ctx = ctx;
    PlafGlfwInit(&cbui.plf, title, 1366, 768, cbui.image_buffer);
    cbui.t_framestart = ReadSystemTimerMySec();
    cbui.t_framestart_prev = cbui.t_framestart;

    UI_Init(cbui.plf.width, cbui.plf.height, &cbui.frameno);
    SpriteBufferInit(cbui.ctx->a_life);

    cbui.map_fonts = InitMap(cbui.ctx->a_life, MAX_RESOURCE_CNT);
    g_font_map = &cbui.map_fonts;
    cbui.map_textures = InitMap(cbui.ctx->a_life, MAX_RESOURCE_CNT);

    // load & check resource file
    ResourceStreamHandle hdl = ResourceStreamLoadAndOpen(cbui.ctx->a_tmp, cbui.ctx->a_life, "all.res");

    // map out the resources
    ResourceHdr *res = hdl.first;
    while (res) {
        // fonts
        if (res->tpe == RST_FONT) {
            FontAtlas *font = FontAtlasLoadBinaryStream(res->GetInlinedData(), res->data_sz);
            if (false) { font->Print(); }

            MapPut(&cbui.map_fonts, font->hash, font);
            MapPut(&cbui.map_textures, font->hash, &font->texture);
        }


        // sprite maps
        // TODO: do something else // load each sprite map individually
        /*
        else if (res->tpe == RST_SPRITE) {
            SpriteMap *smap = SpriteMapLoadStream((u8*) res->GetInlinedData(), res->data_sz);
            if (false) {

                printf("sprite map: %s, %s, count: %u, atlas w: %u, atlas h: %u\n", smap->map_name, smap->key_name, smap->sprites.len, smap->texture.width, smap->texture.height);
            }

            MapPut(&g_resource_map, smap->GetKey(), smap);
            MapPut(&g_texture_map, smap->GetKey(), &smap->texture);
        }
        */


        // other
        else {
            printf("WARN: unknown resource detected\n");
        }

        // iter
        res = res->GetInlinedNext();
    }
    SetFontAndSize(FS_30, hdl.names[RST_FONT]->GetStr());

    if (start_in_fullscreen) { PlafGlfwToggleFullscreen(&cbui.plf); }

    return &cbui;
}


#define FR_RUNNING_AVG_COUNT 4
void CbuiFrameStart() {
    // frame end
    XSleep(1);

    PlafGlfwUpdate(&cbui.plf);
    UI_FrameEnd(cbui.ctx->a_tmp, cbui.plf.width, cbui.plf.height, cbui.plf.cursorpos.x, cbui.plf.cursorpos.y, cbui.plf.left.ended_down, cbui.plf.left.pushed);
    cbui.running = cbui.running && !GetEscape() && !GetWindowShouldClose(&cbui.plf);


    // frame start
    ArenaClear(cbui.ctx->a_tmp);
    memset(cbui.image_buffer, 255, IMG_BUFF_CHANNELS * cbui.plf.width * cbui.plf.height);

    cbui.t_framestart = ReadSystemTimerMySec();
    cbui.dt = (cbui.t_framestart - cbui.t_framestart_prev) / 1000;
    cbui.dts[cbui.frameno % FR_RUNNING_AVG_COUNT] = cbui.dt;

    f32 sum = 0;
    for (s32 i = 0; i < FR_RUNNING_AVG_COUNT; ++i) { sum += cbui.dts[i]; }
    f32 dt_avg = sum / FR_RUNNING_AVG_COUNT;
    cbui.fr = 1.0f / dt_avg * 1000;
    cbui.t_framestart_prev = cbui.t_framestart;

    cbui.frameno++;
}

void CbuiFrameEnd() {
    SpriteBufferBlitAndClear(cbui.map_textures, cbui.plf.width, cbui.plf.height, cbui.image_buffer);
    PlafGlfwPushBuffer(&cbui.plf);
}

void CbuiExit() {
    PlafGlfwTerminate(&cbui.plf);
}


#endif


#endif // __JG_CBUI_H__
