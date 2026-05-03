#ifndef BOOLINTERVAL_H
#define BOOLINTERVAL_H

#include <string>
#include "BBV.h"
#include "Allocator.h"

class BoolInterval
{
public:
    BBV vec;
    BBV dnc;
    explicit BoolInterval(size_t len = 8);
    BoolInterval(const char *vec_in, const char *dnc_in);
    explicit BoolInterval(const char *vector);
    BoolInterval(BBV &vec_in, BBV &dnc_in);
    void setInterval(BBV &vec, BBV &dnc);

    BoolInterval &operator=(BoolInterval &ibv);
    bool operator == (BoolInterval &ibv);
    bool operator != (BoolInterval &ibv);
    operator std::string();
    int length();
    int rang();

    bool isOrthogonal(BoolInterval &ibv);
    bool isEqualComponent(BoolInterval &ibv);
    BoolInterval &mergeInterval(BoolInterval &ibv);
    bool isIntersection(BoolInterval &ibv);
    bool isAbsorb(BoolInterval &ibv);

    char getValue(int ix);
    void setValue(char value, int ix);

	DECLARE_ALLOCATOR
};

#endif // BOOLINTERVAL_H
