#pragma once

#include <cmath>

struct Vector2D
{
    float x, y;
};

struct Vector3D
{
    float x, y, z;

    Vector3D operator-( const Vector3D& input ) const
    {
        return Vector3D{ x - input.x, y - input.y, z - input.z };
    }

    float DotProduct( Vector3D input ) const
    {
        return ( x * input.x ) + ( y * input.y ) + ( z * input.z );
    }

    float Length() const
    {
        return sqrt( ( x * x ) + ( y * y ) + ( z * z ) );
    }

    float Length2D() const
    {
        return sqrt( ( x * x ) + ( y * y ) );
    }

    float Distance( Vector3D input ) const
    {
        return ( *this - input ).Length();
    }

    float Distance2D( Vector3D input ) const
    {
        return ( *this - input ).Length2D();
    }

    void NormalizeAngles()
    {
        while (y > 89.0f)
            y -= 180.0f;

        while (y < -89.0f)
            y += 180.0f;

        while (x > 360.0f)
            x -= 720.0f;

        while (x < 0.0f)
            x += 720.0f;
    }
};

typedef Vector3D QAngle;

struct Matrix4x4
{
    inline Matrix4x4(){}
    inline Matrix4x4(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33 )
    {
        Init(
                m00, m01, m02, m03,
                m10, m11, m12, m13,
                m20, m21, m22, m23,
                m30, m31, m32, m33
        );
    }

    inline void Init(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33
    )
    {
        m[ 0 ][ 0 ] = m00;
        m[ 0 ][ 1 ] = m01;
        m[ 0 ][ 2 ] = m02;
        m[ 0 ][ 3 ] = m03;

        m[ 1 ][ 0 ] = m10;
        m[ 1 ][ 1 ] = m11;
        m[ 1 ][ 2 ] = m12;
        m[ 1 ][ 3 ] = m13;

        m[ 2 ][ 0 ] = m20;
        m[ 2 ][ 1 ] = m21;
        m[ 2 ][ 2 ] = m22;
        m[ 2 ][ 3 ] = m23;

        m[ 3 ][ 0 ] = m30;
        m[ 3 ][ 1 ] = m31;
        m[ 3 ][ 2 ] = m32;
        m[ 3 ][ 3 ] = m33;
    }
    Matrix4x4 Transpose( ) const
    {
        return Matrix4x4(
                m[ 0 ][ 0 ], m[ 1 ][ 0 ], m[ 2 ][ 0 ], m[ 3 ][ 0 ],
                m[ 0 ][ 1 ], m[ 1 ][ 1 ], m[ 2 ][ 1 ], m[ 3 ][ 1 ],
                m[ 0 ][ 2 ], m[ 1 ][ 2 ], m[ 2 ][ 2 ], m[ 3 ][ 2 ],
                m[ 0 ][ 3 ], m[ 1 ][ 3 ], m[ 2 ][ 3 ], m[ 3 ][ 3 ] );
    }
    float m[ 4 ][ 4 ];
};