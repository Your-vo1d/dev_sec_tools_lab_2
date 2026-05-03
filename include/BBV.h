#ifndef BBV_H
#define BBV_H

#include <iostream>
#include "Allocator.h"

typedef unsigned char bbyte_t;

class X
{
    bbyte_t* ptr;
    int index;
public:
    X();
    X(bbyte_t* vec, int k);
    X operator=(int k);
    operator int();
    X operator=(X& v);
    DECLARE_ALLOCATOR
};


class BBV
{
    friend X;
    bbyte_t* vec;
    int size;
    int len;
public:
    ~BBV();
    BBV();
    BBV(int size);
    BBV(const char* str);
    BBV(BBV& V);
    void Init(const char* str);
    void Set0(int k);
    void Set1(int k);
    BBV operator = (BBV& V);
    BBV operator = (const char* str);
    bool operator ==(BBV& V);
    BBV operator | (BBV& V);
    BBV operator & (BBV& V);
    BBV operator ^ (BBV& V);
    BBV operator ~ ();
    BBV operator >> (int k);
    BBV operator << (int k);
    X operator [] (int k);
    operator char*();
    int getWeight();
    int getSize();
    friend std::ostream & operator << (std::ostream &r, BBV& V);
    friend std::istream & operator >> (std::istream &r, BBV& V);

    DECLARE_ALLOCATOR
};

#endif // BBV_H
