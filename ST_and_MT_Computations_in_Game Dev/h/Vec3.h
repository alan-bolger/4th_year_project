// --------------------------------------------
// | Vec3.h                                   |
// | -----------------------------------------|
// | Vec3 is a very small library for simple  |
// | vector maths.                            |
// --------------------------------------------

#ifndef VEC3_H
#define VEC3_H

#include <cmath>

template<typename T>
class Vec3
{
public:
    T x;
    T y;
    T z;

    Vec3() : x(T(0)), y(T(0)), z(T(0)) { }
    Vec3(T xx) : x(xx), y(xx), z(xx) { }
    Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { }

    /// <summary>
    /// Normalise.
    /// </summary>
    /// <returns>Result.</returns>
    Vec3 &normalize()
    {
        T norm2 = length2();

        if (norm2 > 0)
        {
            T invNorm = 1 / std::sqrt(norm2);

            x *= invNorm;
            y *= invNorm;
            z *= invNorm;
        }

        return *this;
    }

    /// <summary>
    /// Multiply by a scalar.
    /// </summary>
    /// <param name="s">Scalar.</param>
    /// <returns>Result.</returns>
    Vec3<T> operator * (const T &s) const 
    { 
        return Vec3<T>(x * s, y * s, z * s); 
    }

    /// <summary>
    /// Multiply by another vector.
    /// </summary>
    /// <param name="vec">Vector.</param>
    /// <returns>Result.</returns>
    Vec3<T> operator * (const Vec3<T> &vec) const 
    { 
        return Vec3<T>(x * vec.x, y * vec.y, z * vec.z);
    }

    /// <summary>
    /// Dot product.
    /// </summary>
    /// <param name="vec">Vector.</param>
    /// <returns>Result.</returns>
    T dot(const Vec3<T> &vec) const
    { 
        return x * vec.x + y * vec.y + z * vec.z;
    }

    /// <summary>
    /// Subtraction.
    /// </summary>
    /// <param name="vec">Vector.</param>
    /// <returns>Result.</returns>
    Vec3<T> operator - (const Vec3<T> &vec) const
    { 
        return Vec3<T>(x - vec.x, y - vec.y, z - vec.z);
    }

    /// <summary>
    /// Addition.
    /// </summary>
    /// <param name="vec">Vector.</param>
    /// <returns>Result.</returns>
    Vec3<T> operator + (const Vec3<T> &vec) const
    { 
        return Vec3<T>(x + vec.x, y + vec.y, z + vec.z);
    }

    /// <summary>
    /// Plus equals.
    /// </summary>
    /// <param name="vec">Vector.</param>
    /// <returns>Result.</returns>
    Vec3<T> &operator += (const Vec3<T> &vec)
    { 
        x += vec.x, y += vec.y, z += vec.z;
        
        return *this; 
    }

    /// <summary>
    /// Multiply equals.
    /// </summary>
    /// <param name="vec">Vector.</param>
    /// <returns>Result.</returns>
    Vec3<T> &operator *= (const Vec3<T> &vec)
    { 
        x *= vec.x, y *= vec.y, z *= vec.z;
        
        return *this;
    }

    /// <summary>
    /// Negate.
    /// </summary>
    /// <returns>Result.</returns>
    Vec3<T> operator - () const
    { 
        return Vec3<T>(-x, -y, -z);
    }

    /// <summary>
    /// Length squared.
    /// </summary>
    /// <returns>Result.</returns>
    T length2() const 
    { 
        return x * x + y * y + z * z;
    }

    /// <summary>
    /// Length.
    /// </summary>
    /// <returns>Result.</returns>
    T length() const 
    { 
        return std::sqrt(length2());
    }
};

using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;

#endif // !VEC3_H