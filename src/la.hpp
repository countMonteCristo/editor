#ifndef LA_HPP_

#include <stddef.h>
#include <iostream>
#include <cassert>


template <typename T>
struct Vec2 {
    Vec2(T _x1, T _x2) : x(_x1), y(_x2) {}
    Vec2() : x(0), y(0) {}
    ~Vec2() {}

    T x;
    T y;

    Vec2 operator+(const Vec2<T>& v) const {
        return Vec2(x + v.x, y + v.y);
    }
    Vec2 operator-(const Vec2<T>& v) const {
        return Vec2(x - v.x, y - v.y);
    }
    Vec2 operator*(const Vec2<T>& v) const {
        return Vec2(x * v.x, y * v.y);
    }
    Vec2 operator/(const Vec2<T>& v) const {
        return Vec2(x / v.x, y / v.y);
    }

    Vec2& operator+=(const Vec2<T>& v) {
        x += v.x;
        y += v.y;
        return *this;
    }
    Vec2& operator-=(const Vec2<T>& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    Vec2& operator*=(const Vec2<T>& v) {
        x *= v.x;
        y *= v.y;
        return *this;
    }
    Vec2& operator/=(const Vec2<T>& v) {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    bool operator==(const Vec2<T>& v) const {
        return (x == v.x) && (y==v.y);
    }
};

typedef Vec2<int> Vec2i;
typedef Vec2<unsigned int> Vec2u;
typedef Vec2<size_t> Vec2s;
typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vec2<T> vec) {
    os << "(" << vec.x <<"," << vec.y << ")";
    return os;
}

template <typename T>
T bounded(T min_value, T max_value, T value) {
    assert(max_value >= min_value);
    if ( value < min_value )
        return min_value;
    if ( value > max_value )
        return max_value;
    return value;
}

#define LA_HPP_
#endif // LA_HPP_
