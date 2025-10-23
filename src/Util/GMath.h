#pragma once

#include <cmath>
#include <concepts>
#include <type_traits>
#include <iostream>

// Restrict to arithmetic types only
template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<Arithmetic T>
struct Vector2 {
    T x, y;

    constexpr Vector2(T x = 0, T y = 0) noexcept : x(x), y(y) {}

    constexpr Vector2 operator+(const Vector2& other) const noexcept {
        return {x + other.x, y + other.y};
    }
    constexpr Vector2 operator-(const Vector2& other) const noexcept {
        return {x - other.x, y - other.y};
    }
    constexpr Vector2 operator*(T scalar) const noexcept {
        return {x * scalar, y * scalar};
    }
    constexpr Vector2 operator/(T scalar) const noexcept {
        return {x / scalar, y / scalar};
    }

    constexpr Vector2& operator+=(const Vector2& other) noexcept {
        x += other.x; y += other.y; return *this;
    }
    constexpr Vector2& operator-=(const Vector2& other) noexcept {
        x -= other.x; y -= other.y; return *this;
    }
    constexpr Vector2& operator*=(T scalar) noexcept {
        x *= scalar; y *= scalar; return *this;
    }
    constexpr Vector2& operator/=(T scalar) noexcept {
        x /= scalar; y /= scalar; return *this;
    }

    constexpr Vector2 operator-() const noexcept {
        return { -x, -y };
    }

    constexpr auto LengthSquared() const noexcept {
        return x * x + y * y;
    }
    auto Length() const noexcept {
        using std::sqrt;
        return sqrt(static_cast<double>(LengthSquared()));
    }
    constexpr Vector2<T> Normalized() const noexcept {
        auto len = Length();
        if (len == 0.0) return {0, 0};
        return {x / len, y / len};
    }
    constexpr auto Dot(const Vector2& other) const noexcept {
        return x * other.x + y * other.y;
    }

    constexpr bool operator==(const Vector2& o) const noexcept {
        return x == o.x && y == o.y;
    }
    constexpr bool operator!=(const Vector2& o) const noexcept {
        return !(*this == o);
    }

    Vector2 Lerp(const Vector2& to, double t) const noexcept {
        return {static_cast<T>(x + (to.x - x) * t),
                static_cast<T>(y + (to.y - y) * t)};
    }

    template<Arithmetic U>
    constexpr explicit(false) operator Vector2<U>() const noexcept {
        return {static_cast<U>(x), static_cast<U>(y)};
    }

    constexpr std::string ToString() const {
        return "{" + std::to_string(x) + ", " +
                     std::to_string(y) + "}";
    }
};

template <Arithmetic T, Arithmetic U>
constexpr auto operator*(T scalar, const Vector2<U>& b) noexcept {
    using R = std::common_type_t<T, U>;
    return Vector2<R>(b * scalar);
}

// Mixed-type arithmetic (e.g., Vector2<int> + Vector2<float>)
template<Arithmetic T, Arithmetic U>
constexpr auto operator+(const Vector2<T>& a, const Vector2<U>& b) noexcept {
    using R = std::common_type_t<T, U>;
    return Vector2<R>(static_cast<R>(a.x) + static_cast<R>(b.x),
                      static_cast<R>(a.y) + static_cast<R>(b.y));
}
template<Arithmetic T, Arithmetic U>
constexpr auto operator-(const Vector2<T>& a, const Vector2<U>& b) noexcept {
    using R = std::common_type_t<T, U>;
    return Vector2<R>(static_cast<R>(a.x) - static_cast<R>(b.x),
                      static_cast<R>(a.y) - static_cast<R>(b.y));
}

// Scalar * Vector2 (commutative)
template<Arithmetic T>
constexpr Vector2<T> operator*(T scalar, const Vector2<T>& v) noexcept {
    return {v.x * scalar, v.y * scalar};
}

// Stream output
template<Arithmetic T>
std::ostream& operator<<(std::ostream& os, const Vector2<T>& v) {
    return os << "Vector2(" << v.x << ", " << v.y << ")";
}

// ===========================
//  Vector3<T>
// ===========================
template<Arithmetic T>
struct Vector3 {
    T x, y, z;

    constexpr Vector3(T x = 0, T y = 0, T z = 0) noexcept : x(x), y(y), z(z) {}

    // ---------- Basic operators ----------
    constexpr Vector3 operator+(const Vector3& o) const noexcept {
        return {x + o.x, y + o.y, z + o.z};
    }
    constexpr Vector3 operator-(const Vector3& o) const noexcept {
        return {x - o.x, y - o.y, z - o.z};
    }
    constexpr Vector3 operator*(T s) const noexcept {
        return {x * s, y * s, z * s};
    }
    constexpr Vector3 operator/(T s) const noexcept {
        return {x / s, y / s, z / s};
    }

    constexpr Vector3& operator+=(const Vector3& o) noexcept {
        x += o.x; y += o.y; z += o.z; return *this;
    }
    constexpr Vector3& operator-=(const Vector3& o) noexcept {
        x -= o.x; y -= o.y; z -= o.z; return *this;
    }
    constexpr Vector3& operator*=(T s) noexcept {
        x *= s; y *= s; z *= s; return *this;
    }
    constexpr Vector3& operator/=(T s) noexcept {
        x /= s; y /= s; z /= s; return *this;
    }

    constexpr auto LengthSquared() const noexcept {
        return x * x + y * y + z * z;
    }
    auto Length() const noexcept {
        using std::sqrt;
        return sqrt(static_cast<long double>(LengthSquared()));
    }
    constexpr Vector3 Normalized() const noexcept {
        auto len = Length();
        if (len == 0.0) return {0, 0, 0};
        return {x / len, y / len, z / len};
    }
    constexpr auto Dot(const Vector3& o) const noexcept {
        return x * o.x + y * o.y + z * o.z;
    }
    constexpr Vector3 Cross(const Vector3& o) const noexcept {
        return {
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x
        };
    }

    // ---------- Comparison ----------
    constexpr bool operator==(const Vector3& o) const noexcept {
        return x == o.x && y == o.y && z == o.z;
    }
    constexpr bool operator!=(const Vector3& o) const noexcept {
        return !(*this == o);
    }

    Vector3 Lerp(const Vector3& to, double t) const noexcept {
        return {static_cast<T>(x + (to.x - x) * t),
                static_cast<T>(y + (to.y - y) * t),
                static_cast<T>(z + (to.z - z) * t)};
    }

    // ---------- Cross-type conversion ----------
    template<Arithmetic U>
    constexpr explicit(false) operator Vector3<U>() const noexcept {
        return {static_cast<U>(x), static_cast<U>(y), static_cast<U>(z)};
    }

    constexpr std::string ToString() const {
        return "{" + std::to_string(x) + ", " +
                     std::to_string(y) + ", " +
                     std::to_string(z) + "}";
    }
};

// Mixed-type arithmetic (Vector3<T> + Vector3<U>)
template<Arithmetic T, Arithmetic U>
constexpr auto operator+(const Vector3<T>& a, const Vector3<U>& b) noexcept {
    using R = std::common_type_t<T, U>;
    return Vector3<R>(
        static_cast<R>(a.x) + static_cast<R>(b.x),
        static_cast<R>(a.y) + static_cast<R>(b.y),
        static_cast<R>(a.z) + static_cast<R>(b.z)
    );
}
template<Arithmetic T, Arithmetic U>
constexpr auto operator-(const Vector3<T>& a, const Vector3<U>& b) noexcept {
    using R = std::common_type_t<T, U>;
    return Vector3<R>(
        static_cast<R>(a.x) - static_cast<R>(b.x),
        static_cast<R>(a.y) - static_cast<R>(b.y),
        static_cast<R>(a.z) - static_cast<R>(b.z)
    );
}

// Scalar * Vector3 (commutative)
template<Arithmetic T>
constexpr Vector3<T> operator*(T scalar, const Vector3<T>& v) noexcept {
    return {v.x * scalar, v.y * scalar, v.z * scalar};
}

// Stream output
template<Arithmetic T>
std::ostream& operator<<(std::ostream& os, const Vector3<T>& v) {
    return os << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
}

using Vector2i = Vector2<int>;
using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;
using Vector3i = Vector3<int>;
using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;

namespace MathUtils {
    template<Arithmetic T>
    T Lerp(T a, T b, float t) {
        return static_cast<T>(a + (b - a) * t);
    }

    template<Arithmetic T>
    T Clamp(T value, T min, T max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    template<Arithmetic T>
    T Wrap(T value, T min, T max) {
        T range = max - min;
        if (range <= 0) return min; // Invalid range
        while (value < min) value += range;
        while (value >= max) value -= range;
        return value;
    }

    template<Arithmetic T>
    T Distance(const Vector2<T>& a, const Vector2<T>& b) {
        return (b - a).Length();
    }

    template<Arithmetic T>
    T Distance(const Vector3<T>& a, const Vector3<T>& b) {
        return (b - a).Length();
    }

    constexpr float DegreesToRadians(float degrees) {
        return degrees * (3.14159265358979323846f / 180.0f);
    }

    constexpr float RadiansToDegrees(float radians) {
        return radians * (180.0f / 3.14159265358979323846f);
    }

    template<Arithmetic T>
    Vector2<T> Rotate(const Vector2<T>& v, float degrees) {
        float radians = DegreesToRadians(degrees);
        float cosA = std::cos(radians);
        float sinA = std::sin(radians);
        return {
            static_cast<T>(v.x * cosA - v.y * sinA),
            static_cast<T>(v.x * sinA + v.y * cosA)
        };
    }
};

template<Arithmetic T>
struct Rect2 {
    T x, y, width, height;

    constexpr Rect2(T x = 0, T y = 0, T width = 0, T height = 0) noexcept
        : x(x), y(y), width(width), height(height) {}

    constexpr bool Contains(const Vector2<T>& point) const noexcept {
        return point.x >= x && point.x <= (x + width) &&
               point.y >= y && point.y <= (y + height);
    }

    constexpr bool Intersects(const Rect2& other) const noexcept {
        return !(x > other.x + other.width || x + width < other.x ||
                 y > other.y + other.height || y + height < other.y);
    }

    constexpr Rect2 Intersection(const Rect2& other) const noexcept {
        T nx = std::max(x, other.x);
        T ny = std::max(y, other.y);
        T nwidth = std::min(x + width, other.x + other.width) - nx;
        T nheight = std::min(y + height, other.y + other.height) - ny;
        if (nwidth < 0 || nheight < 0) return {0, 0, 0, 0}; // No intersection
        return {nx, ny, nwidth, nheight};
    }

    constexpr bool operator==(const Rect2& o) const noexcept {
        return x == o.x && y == o.y && width == o.width && height == o.height;
    }
    constexpr bool operator!=(const Rect2& o) const noexcept {
        return !(*this == o);
    }

    constexpr std::string ToString() const noexcept {
        return "{" + std::to_string(x) + ", " +
                     std::to_string(y) + ", " +
                     std::to_string(width) + ", " +
                     std::to_string(height) + "}";
    }

    constexpr Vector2<T> Min() const {
        return { x, y };
    }

    constexpr Vector2<T> Max() const {
        return { x + width, y + height };
    }

    constexpr T Right() const { return x + width; }
    constexpr T Left() const { return x; }
    constexpr T Top() const { return y; }
    constexpr T Bottom() const { return y + height; }

    constexpr Vector2<T> Center() const {
        return { (x + width) / 2.0, (y + height) / 2.0 };
    }

    constexpr T CenterX() const { return (x + width) / 2.0; }
    constexpr T CenterY() const { return (y + height) / 2.0; }

    constexpr inline bool RayIntersectsRect(
        const Vector2<T>& origin,
        const Vector2<T>& dir,
        double& tNear
    ) const {
        double tmin = (Min().x - origin.x) / dir.x;
        double tmax = (Max().x - origin.x) / dir.x;

        if (tmin > tmax) std::swap(tmin, tmax);

        double tymin = (Min().y - origin.y) / dir.y;
        double tymax = (Max().y - origin.y) / dir.y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax)) return false;

        tNear = std::max(tmin, tymin);
        double tFar = std::min(tmax, tymax);

        return tFar >= 0.0 && tNear <= tFar;
    }

    Rect2 Translated(const Vector2<T>& v) const {
        return {x + v.x, y + v.y, width, height};
    }

    Vector2d GetOverlap(const Rect2& other) const {
        double dx = std::min(this->Right(), other.Right()) - std::max(this->Left(), other.Left());
        double dy = std::min(this->Bottom(), other.Bottom()) - std::max(this->Top(), other.Top());

        if (dx <= 0 || dy <= 0)
            return Vector2d(0, 0); // No overlap

        // Choose axis of least penetration
        return (dx < dy) ? Vector2d((this->Center().x < other.Center().x ? -dx : dx), 0)
                        : Vector2d(0, (this->Center().y < other.Center().y ? -dy : dy));
    }

    Rect2 Expanded(T r) const {
        return {Left() - r, Top() - r, width + 2*r, height + 2*r};
    }
};

using Rect2i = Rect2<int>;
using Rect2f = Rect2<float>;
using Rect2d = Rect2<double>;