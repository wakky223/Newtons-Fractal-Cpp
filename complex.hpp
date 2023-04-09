#pragma once
#include <cmath>
#include <string>

struct complex {
    complex(double _re, double _im = 0);
    complex();
    double re, im;

    double size();
    double arg();
    bool operator == (complex a);
};

bool isnanIEEE754(complex a);

std::string string(complex a);

complex abs(complex a );

bool operator > (const complex& a,const complex& b);

bool operator < (const complex& a,const complex& b);

complex operator + (const complex& a,const complex& b);

complex operator + (const complex& a,const double& b);

complex operator - (const complex& a,const complex& b);

complex operator - (const complex& a,const double& b);

complex operator - (const double& a,const complex& b);

complex operator * (const complex& a,const complex& b);

complex operator * (const complex& a,const double& b);

complex operator * (const double& a,const complex& b);

complex operator / (const complex& a,const complex& b);

complex operator / (const complex& a,const double& b);

complex operator / (const double& a,const complex& b);

complex sin(const complex& x);

complex cos(const complex& x);

complex sec(const complex& x);

complex csc(const complex& x);

complex tan(const complex& x);

complex cot(const complex& x);

complex log(complex a,int m);

complex log(complex a);

complex logBase(complex a,int base);

complex pow(complex a, double b);

complex pow(complex a, complex b);