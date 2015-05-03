#pragma once

#include <iostream>

/*
   Copyright (c) 2006 Henry Strickland & Ryan Seto

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.

   (* http://www.opensource.org/licenses/mit-license.php *)
   */

class fixed {
private:
    int	g; // the guts

    const static int BP= 16;  // how many low bits are right of Binary Point
    const static int BP2= BP*2;  // how many low bits are right of Binary Point
    const static int BPhalf= BP/2;  // how many low bits are right of Binary Point

    static double STEP() { return 1.0 / (1<<BP); }  // smallest step we can represent

    // for private construction via guts
    enum fixedRaw { RAW };
    fixed(fixedRaw, int guts) : g(guts) {}


public:
    fixed() : g(0) {}
    fixed(const fixed& a) : g( a.g ) {}
    /*fixed(float a) : g( int(a / (float)STEP()) ) {}
    fixed(double a) : g( int(a / (double)STEP()) ) {}*/
    fixed(int a) : g( a << BP ) {}
    //explicit fixed(size_t s) : fixed((int)s) {}
    fixed& operator =(const fixed& a) { g= a.g; return *this; }
    /*fixed& operator =(float a) { g= fixed(a).g; return *this; }
    fixed& operator =(double a) { g= fixed(a).g; return *this; }*/
    fixed& operator =(int a) { g= fixed(a).g; return *this; }

    float toFloat() const { return g * (float)STEP(); }
    double toDouble() const { return g * (double)STEP(); }
    int toInt() const { return g>>BP; }

    fixed abs() const { return fixed(RAW, std::abs(g)); }

    //operator float() const { return toFloat(); } 

    static fixed fromFloat(float f) {
        return fixed(RAW,f / (float)STEP());
    }

    fixed operator +() const { return fixed(RAW,g); }
    fixed operator -() const { return fixed(RAW,-g); }

    fixed operator +(const fixed& a) const { return fixed(RAW, g + a.g); }
    fixed operator -(const fixed& a) const { return fixed(RAW, g - a.g); }
#if 1
    // more acurate, using long long
    fixed operator *(const fixed& a) const { return fixed(RAW,  (int)( ((long long)g * (long long)a.g ) >> BP)); }
#else
    // faster, but with only half as many bits right of binary point
    fixed operator *(const fixed& a) const { return fixed(RAW, (g>>BPhalf) * (a.g>>BPhalf) ); }
#endif
    fixed operator /(const fixed& a) const { 
        return fixed(RAW, int( (((long long)g << BP2) / (long long)(a.g)) >> BP) );
    }

    /*fixed operator +(float a) const { return fixed(RAW, g + fixed(a).g); }
    fixed operator -(float a) const { return fixed(RAW, g - fixed(a).g); }
    fixed operator *(float a) const { return fixed(RAW, (g>>BPhalf) * (fixed(a).g>>BPhalf) ); }
    fixed operator /(float a) const { return fixed(RAW, int( (((long long)g << BP2) / (long long)(fixed(a).g)) >> BP) ); }

    fixed operator +(double a) const { return fixed(RAW, g + fixed(a).g); }
    fixed operator -(double a) const { return fixed(RAW, g - fixed(a).g); }
    fixed operator *(double a) const { return fixed(RAW, (g>>BPhalf) * (fixed(a).g>>BPhalf) ); }
    fixed operator /(double a) const { return fixed(RAW, int( (((long long)g << BP2) / (long long)(fixed(a).g)) >> BP) ); }*/

    fixed& operator +=(fixed a) { return *this = *this + a; return *this; }
    fixed& operator -=(fixed a) { return *this = *this - a; return *this; }
    fixed& operator *=(fixed a) { return *this = *this * a; return *this; }
    fixed& operator /=(fixed a) { return *this = *this / a; return *this; }

    fixed& operator +=(int a) { return *this = *this + (fixed)a; return *this; }
    fixed& operator -=(int a) { return *this = *this - (fixed)a; return *this; }
    fixed& operator *=(int a) { return *this = *this * (fixed)a; return *this; }
    fixed& operator /=(int a) { return *this = *this / (fixed)a; return *this; }

    /*fixed& operator +=(float a) { return *this = *this + a; return *this; }
    fixed& operator -=(float a) { return *this = *this - a; return *this; }
    fixed& operator *=(float a) { return *this = *this * a; return *this; }
    fixed& operator /=(float a) { return *this = *this / a; return *this; }

    fixed& operator +=(double a) { return *this = *this + a; return *this; }
    fixed& operator -=(double a) { return *this = *this - a; return *this; }
    fixed& operator *=(double a) { return *this = *this * a; return *this; }
    fixed& operator /=(double a) { return *this = *this / a; return *this; }*/

    bool operator ==(const fixed& a) const { return g == a.g; }
    bool operator !=(const fixed& a) const { return g != a.g; }
    bool operator <=(const fixed& a) const { return g <= a.g; }
    bool operator >=(const fixed& a) const { return g >= a.g; }
    bool operator  <(const fixed& a) const { return g  < a.g; }
    bool operator  >(const fixed& a) const { return g  > a.g; }

    /*bool operator ==(float a) const { return g == fixed(a).g; }
    bool operator !=(float a) const { return g != fixed(a).g; }
    bool operator <=(float a) const { return g <= fixed(a).g; }
    bool operator >=(float a) const { return g >= fixed(a).g; }
    bool operator  <(float a) const { return g  < fixed(a).g; }
    bool operator  >(float a) const { return g  > fixed(a).g; }

    bool operator ==(double a) const { return g == fixed(a).g; }
    bool operator !=(double a) const { return g != fixed(a).g; }
    bool operator <=(double a) const { return g <= fixed(a).g; }
    bool operator >=(double a) const { return g >= fixed(a).g; }
    bool operator  <(double a) const { return g  < fixed(a).g; }
    bool operator  >(double a) const { return g  > fixed(a).g; }*/
};

/*inline fixed operator +(float a, const fixed& b) { return fixed(a)+b; }
inline fixed operator -(float a, const fixed& b) { return fixed(a)-b; }
inline fixed operator *(float a, const fixed& b) { return fixed(a)*b; }
inline fixed operator /(float a, const fixed& b) { return fixed(a)/b; }

inline bool operator ==(float a, const fixed& b) { return fixed(a) == b; }
inline bool operator !=(float a, const fixed& b) { return fixed(a) != b; }
inline bool operator <=(float a, const fixed& b) { return fixed(a) <= b; }
inline bool operator >=(float a, const fixed& b) { return fixed(a) >= b; }
inline bool operator  <(float a, const fixed& b) { return fixed(a)  < b; }
inline bool operator  >(float a, const fixed& b) { return fixed(a)  > b; }



inline fixed operator +(double a, const fixed& b) { return fixed(a)+b; }
inline fixed operator -(double a, const fixed& b) { return fixed(a)-b; }
inline fixed operator *(double a, const fixed& b) { return fixed(a)*b; }
inline fixed operator /(double a, const fixed& b) { return fixed(a)/b; }

inline bool operator ==(double a, const fixed& b) { return fixed(a) == b; }
inline bool operator !=(double a, const fixed& b) { return fixed(a) != b; }
inline bool operator <=(double a, const fixed& b) { return fixed(a) <= b; }
inline bool operator >=(double a, const fixed& b) { return fixed(a) >= b; }
inline bool operator  <(double a, const fixed& b) { return fixed(a)  < b; }
inline bool operator  >(double a, const fixed& b) { return fixed(a)  > b; }*/


inline int& operator +=(int& a, const fixed& b) { a = ((fixed)a + b).toInt(); return a; }
inline int& operator -=(int& a, const fixed& b) { a = ((fixed)a - b).toInt(); return a; }
inline int& operator *=(int& a, const fixed& b) { a = ((fixed)a * b).toInt(); return a; }
inline int& operator /=(int& a, const fixed& b) { a = ((fixed)a / b).toInt(); return a; }

/*inline float& operator +=(float& a, const fixed& b) { a = a + b; return a; }
inline float& operator -=(float& a, const fixed& b) { a = a - b; return a; }
inline float& operator *=(float& a, const fixed& b) { a = a * b; return a; }
inline float& operator /=(float& a, const fixed& b) { a = a / b; return a; }

inline double& operator +=(double& a, const fixed& b) { a = a + b; return a; }
inline double& operator -=(double& a, const fixed& b) { a = a - b; return a; }
inline double& operator *=(double& a, const fixed& b) { a = a * b; return a; }
inline double& operator /=(double& a, const fixed& b) { a = a / b; return a; }*/

