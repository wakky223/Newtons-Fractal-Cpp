#pragma once
#include <cmath>

struct complex {
    complex(double _re, double _im = 0) {
        re = _re;
        im = _im;
    }
    complex() {
        re = 0;
        im = 0;
    }
    double re, im;

    double size() {
        return sqrt(re * re + im * im);
    }
    double arg(){
        return atan2(re, im);
    }
    bool operator == (complex a) {
        if (a.re == re && a.im == im) {
            return true;
        }
        else {
            return false;
        }
    }
};

bool isnan(complex a) {
    return (std::isnan(a.re) || std::isnan(a.im));
}

std::string string(complex a){
    if (a.im == 0.0) 
        return std::to_string(a.re);
    if(a.re == 0.0)
        return std::to_string(a.im) + "i";
    if(a.im >= 0.0)
        return std::to_string(a.re) + " +" + std::to_string(a.im) + "i";

    return std::to_string(a.re) + " " + std::to_string(a.im) + "i";
}

complex abs(complex a ) {
    return complex(std::abs(a.re), std::abs(a.im));
}

complex operator + (const complex& a,const complex& b) {
    return complex(a.re + b.re, a.im + b.im);
}

complex operator + (const complex& a,const double& b) {
    return complex(a.re + b, a.im);
}

complex operator - (const complex& a,const complex& b) {
    return complex(a.re - b.re, a.im - b.im);
}

complex operator - (const complex& a,const double& b) {
    return complex(a.re - b, a.im);
}

complex operator - (const double& a,const complex& b) {
    return complex(a - b.re, -b.im);
}

complex operator * (const complex& a,const complex& b) {
    return complex((a.re * b.re) - (a.im * b.im), (a.im * b.re) + (a.re * b.im));
}

complex operator * (const complex& a,const double& b) {
    return complex((a.re * b), (a.im * b));
}

complex operator * (const double& a,const complex& b) {
    return complex((a * b.re), (a * b.im));
}

complex operator / (const complex& a,const complex& b) {
    double c = 1 / ((b.re * b.re) + (b.im * b.im));
    return complex(((a.re * b.re) + (a.im * b.im)) * c, ((-a.re * b.im) + (a.im * b.re)) * c);
}

complex operator / (const complex& a,const double& b) {
    return complex(a.re/b, a.im /b);
}

complex operator / (const double& a,const complex& b) {
    complex c;
    c.re = ((a * b.re)) / ((b.re * b.re) + (b.im * b.im));
    c.im = ((-a * b.im)) / ((b.re * b.re) + (b.im * b.im));
    return c;
}

complex sin(const complex& x){
    return complex(sin(x.re)*cosh(x.im),cos(x.re)*sinh(x.im));
}

complex cos(const complex& x){
    return complex(cos(x.re)*cosh(x.im),-sin(x.re)*sinh(x.im));
}

complex sec(const complex& x){
    return 1/cos(x);
}

complex csc(const complex& x){
    return 1/sin(x);
}

complex tan(const complex& x){
    return sin(x)/cos(x);
}

complex cot(const complex& x){
    return cos(x)/sin(x);
}


complex pow(complex a, double b){
    //return complex()
    throw 0;
} 

//TODO:
complex pow(complex a, complex b) {
    if(a.im == 0){
        return pow(a.re,b.re)*complex(cos(b.im),sin(b.im));
    }
    if(b.im == 0){

    }
    return 0;

}