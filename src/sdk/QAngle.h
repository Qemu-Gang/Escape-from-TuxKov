#pragma once

#include "Vector.h"
#include "math.h"

enum {
    PITCH = 0,    // up / down
    YAW,        // left / right
    ROLL        // fall over
};

class QAngleByValue;
class QAngle {
public:
    // Members
    float x, y, z;

    // Construction/destruction
    QAngle( void );

    QAngle( float X, float Y, float Z );
    //      QAngle(RadianEuler const &angles);      // evil auto type promotion!!!

    QAngle(const QAngle&) = default;

    // Allow pass-by-value
    operator QAngleByValue&() { return *( ( QAngleByValue* ) ( this ) ); }

    operator const QAngleByValue&() const { return *( ( const QAngleByValue* ) ( this ) ); }

    // Initialization
    void Init( float ix = 0.0f, float iy = 0.0f, float iz = 0.0f );

    void Random( float minVal, float maxVal );

    // Got any nasty NAN's?
    bool IsValid() const { return (!isnan(x) && !isnan(y) && !isnan(z)); }

    void Invalidate();

    // array access...
    float operator[]( int i ) const;

    float& operator[]( int i );

    // Base address...
    float* Base();

    float const* Base() const;

    // equality
    bool operator==( const QAngle& v ) const;

    bool operator!=( const QAngle& v ) const;

    // arithmetic operations
    QAngle& operator+=( const QAngle& v );

    QAngle& operator-=( const QAngle& v );

    QAngle& operator*=( float s );

    QAngle& operator/=( float s );

    // Get the vector's magnitude.
    float Length() const;

    float LengthSqr() const;

    void Normalize();
    
    // negate the QAngle components
    //void  Negate();

    // No assignment operators either...
    QAngle& operator=( const QAngle& src );

#ifndef VECTOR_NO_SLOW_OPERATIONS
    // copy constructors

    // arithmetic operations
    QAngle operator-( void ) const;

    QAngle operator+( const QAngle& v ) const;

    QAngle operator-( const QAngle& v ) const;

    QAngle operator*( float fl ) const;

    QAngle operator/( float fl ) const;

#else

    private:

#endif
};

//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------
inline QAngle::QAngle( void ) {
#ifdef _DEBUG
    #ifdef VECTOR_PARANOIA
    // Initialize to NAN to catch errors
    x = y = z = VEC_T_NAN;
#endif
#endif
}

inline QAngle::QAngle( float X, float Y, float Z ) {
    x = X;
    y = Y;
    z = Z;
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------
inline void QAngle::Init( float ix, float iy, float iz ) {
    x = ix;
    y = iy;
    z = iz;
}

inline void QAngle::Random( float minVal, float maxVal ) {
    x = minVal + ( ( float ) rand() / RAND_MAX ) * ( maxVal - minVal );
    y = minVal + ( ( float ) rand() / RAND_MAX ) * ( maxVal - minVal );
    z = minVal + ( ( float ) rand() / RAND_MAX ) * ( maxVal - minVal );
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------
inline QAngle& QAngle::operator=( const QAngle& vOther ) {
    x = vOther.x;
    y = vOther.y;
    z = vOther.z;
    return *this;
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------
inline bool QAngle::operator==( const QAngle& src ) const {
    return ( src.x == x ) && ( src.y == y ) && ( src.z == z );
}

inline bool QAngle::operator!=( const QAngle& src ) const {
    return ( src.x != x ) || ( src.y != y ) || ( src.z != z );
}

//-----------------------------------------------------------------------------
// standard utils operations
//-----------------------------------------------------------------------------
inline QAngle& QAngle::operator+=( const QAngle& v ) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

inline QAngle& QAngle::operator-=( const QAngle& v ) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

inline QAngle& QAngle::operator*=( float fl ) {
    x *= fl;
    y *= fl;
    z *= fl;
    return *this;
}

inline QAngle& QAngle::operator/=( float fl ) {
    Assert( fl != 0.0f );
    float oofl = 1.0f / fl;
    x *= oofl;
    y *= oofl;
    z *= oofl;
    return *this;
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline float* QAngle::Base() {
    return ( float* ) this;
}

inline float const* QAngle::Base() const {
    return ( float const* ) this;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline float& QAngle::operator[]( int i ) {
    Assert( ( i >= 0 ) && ( i < 3 ) );
    return ( ( float* ) this )[i];
}

inline float QAngle::operator[]( int i ) const {
    Assert( ( i >= 0 ) && ( i < 3 ) );
    return ( ( const float* ) this )[i];
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
inline float QAngle::Length() const {
    return ( float ) FastSqrt( LengthSqr() );
}


inline float QAngle::LengthSqr() const {
    return x * x + y * y + z * z;
}


//-----------------------------------------------------------------------------
// arithmetic operations (SLOW!!)
//-----------------------------------------------------------------------------
#ifndef VECTOR_NO_SLOW_OPERATIONS

inline QAngle QAngle::operator-( void ) const {
    return QAngle( -x, -y, -z );
}

inline QAngle QAngle::operator+( const QAngle& v ) const {
    QAngle res;
    res.x = x + v.x;
    res.y = y + v.y;
    res.z = z + v.z;
    return res;
}

inline QAngle QAngle::operator-( const QAngle& v ) const {
    QAngle res;
    res.x = x - v.x;
    res.y = y - v.y;
    res.z = z - v.z;
    return res;
}

inline QAngle QAngle::operator*( float fl ) const {
    QAngle res;
    res.x = x * fl;
    res.y = y * fl;
    res.z = z * fl;
    return res;
}

inline QAngle QAngle::operator/( float fl ) const {
    QAngle res;
    res.x = x / fl;
    res.y = y / fl;
    res.z = z / fl;
    return res;
}

inline QAngle operator*( float fl, const QAngle& v ) {
    return v * fl;
}

#endif // VECTOR_NO_SLOW_OPERATIONS


//QANGLE SUBTRAC
inline void QAngleSubtract( const QAngle& a, const QAngle& b, QAngle& c ) {
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
}

//QANGLEADD
inline void QAngleAdd( const QAngle& a, const QAngle& b, QAngle& c ) {
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
}

inline void QAngle::Normalize( ){
    while ( x > 89.0f )
        x -= 180.0f;

    while ( x < -89.0f )
        x += 180.0f;

    while ( y > 180.0f )
        y -= 360.0f;

    while ( y < -180.0f )
        y += 360.0f;

    z = 0.0f;
}
