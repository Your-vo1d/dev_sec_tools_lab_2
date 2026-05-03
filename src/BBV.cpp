#include "BBV.h"
#include <iostream>
#include <cstring>
using namespace std;

IMPLEMENT_ALLOCATOR(X, 0, 0)
IMPLEMENT_ALLOCATOR(BBV, 0, 0)

BBV::~BBV()
{
    if (vec != NULL)
        delete[] vec;
}

BBV::BBV()
{
    vec = NULL;
    size = 0;
    len = 0;
}

BBV::BBV(int size_v)
{
    if (size_v > 0) {
        vec = new bbyte_t[(size_v - 1) / 8 + 1];
        if (vec != NULL) {
            len = size_v;
            size = (size_v - 1) / 8 + 1;
            for (int i = 0; i < size; i++)
                vec[i] = 0;
        } else
            throw 0;
    } else
        throw 0;
}

BBV::BBV(const char *str)
{
    if (str != NULL) {
        len = strlen(str);
        size = (len - 1) / 8 + 1;
        vec = new bbyte_t[size];
        if (vec != NULL) {
            int i = 0, j = 8, k = 0;
            bbyte_t mask = 1;
            vec[0] ^= vec[0];
            while (i < len) {
                if (j > 0) {
                    if (str[i] != '0')
                        vec[k] |= mask;
                    mask <<= 1;
                    j--;
                    i++;
                } else {
                    j = 8;
                    k++;
                    vec[k] ^= vec[k];
                    mask = 1;
                }
            }
        } else
            throw 0;
    } else
        throw 0;
}

BBV::BBV(BBV &V)
{
    if (V.vec != NULL) {
        size = V.size;
        len = V.len;
        vec = new bbyte_t[size];
        if (vec != NULL) {
            for (int i = 0; i < size; i++)
                vec[i] = V.vec[i];
        } else
            throw 0;
    }
}

void BBV::Init(const char *str)
{
    if (str != NULL) {
        len = strlen(str);
        size = (len - 1) / 8 + 1;
        if (vec != NULL)
            delete[] vec;
        vec = new bbyte_t[size];
        if (vec != NULL) {
            int i = 0, j = 8, k = 0;
            bbyte_t mask = 1;
            vec[0] ^= vec[0];
            while (i < len) {
                if (j > 0) {
                    if (str[i] != '0')
                        vec[k] |= mask;
                    mask <<= 1;
                    j--;
                    i++;
                } else {
                    j = 8;
                    k++;
                    vec[k] ^= vec[k];
                    mask = 1;
                }
            }
        } else
            throw 0;
    } else
        throw 1;
}

void BBV::Set0(int k)
{
    if (vec != NULL && k >= 0 && k < len) {
        bbyte_t mask = 1;
        int j = k / 8;
        if (k >= 7)
            k %= 8;
        mask <<= k;
        vec[j] &= ~mask;
    } else
        throw 1;
}

void BBV::Set1(int k)
{
    if (vec != NULL && k >= 0 && k < len) {
        bbyte_t mask = 1;
        int j = k / 8;
        if (k >= 7)
            k %= 8;
        mask <<= k;
        vec[j] |= mask;
    } else
        throw 1;
}

BBV BBV::operator=(BBV &V)
{
    if (this != &V) {
        len = V.len;
        size = V.size;
        if (vec != NULL)
            delete[] vec;
        vec = new bbyte_t[size];
        if (vec != NULL) {
            for (int i = 0; i < size; i++)
                vec[i] = V.vec[i];
        } else
            throw 0;
    }
    return *this;
}

BBV BBV::operator=(const char *str)
{
    if (str != NULL) {
        len = strlen(str);
        size = (len - 1) / 8 + 1;
        if (vec != NULL)
            delete[] vec;
        vec = new bbyte_t[size];
        if (vec != NULL) {
            int i = 0, j = 8, k = 0;
            bbyte_t mask = 1;
            vec[0] ^= vec[0];
            while (i < len) {
                if (j > 0) {
                    if (str[i] != '0')
                        vec[k] |= mask;
                    mask <<= 1;
                    j--;
                    i++;
                } else {
                    j = 8;
                    k++;
                    vec[k] ^= vec[k];
                    mask = 1;
                }
            }
        } else
            throw 0;
    } else
        throw 1;
    return *this;
}

bool BBV::operator==(BBV &V)
{
    if (vec != NULL && V.vec != NULL && V.len == len) {
        for (int i = 0; i < size; i++)
            if (V.vec[i] != vec[i])
                return false;
        return true;
    }
    return false;
}

BBV BBV::operator|(BBV &V)
{
    if (vec != NULL && V.vec != NULL && len == V.len) {
        BBV res(*this);
        for (int i = 0; i < size; i++)
            res.vec[i] = vec[i] | V.vec[i];
        return res;
    } else
        throw 2;
}

BBV BBV::operator&(BBV &V)
{
    if (vec != NULL && V.vec != NULL && len == V.len) {
        BBV res(*this);
        for (int i = 0; i < size; i++)
            res.vec[i] = vec[i] & V.vec[i];
        return res;
    } else
        throw 2;
}

BBV BBV::operator^(BBV &V)
{
    if (vec != NULL && V.vec != NULL && len == V.len) {
        BBV res(*this);
        for (int i = 0; i < size; i++)
            res.vec[i] = vec[i] ^ V.vec[i];
        return res;
    } else
        throw 2;
}

BBV BBV::operator~()
{
    BBV res(*this);
    if (vec != NULL) {
        for (int i = 0; i < size; i++)
            res.vec[i] = ~vec[i];
        int k = len % 8;
        if (k != 0)
            k = 8 - k;
        res.vec[size - 1] <<= k;
        res.vec[size - 1] >>= k;
    }
    return res;
}

BBV BBV::operator>>(int k)
{
    BBV res(*this);
    if (vec != NULL && k > 0) {
        for (int i = 0; i < size; i++)
            res.vec[i] ^= res.vec[i];
        int start = (k - 1) / 8;
        if (k % 8 == 0)
            start = k / 8;
        k %= 8;
        bbyte_t mask = 0;
        for (int i = 0; start + i < size; i++) {
            res.vec[start + i] = vec[i] << k;
            res.vec[start + i] |= mask >> (8 - k);
            mask = static_cast<bbyte_t>(-1);
            mask <<= 8 - k;
            mask = vec[i] & mask;
        }
        if (k % 8) {
            int t = (8 - len % 8);
            res.vec[size - 1] <<= t;
            res.vec[size - 1] >>= t;
        }
    }
    return res;
}

BBV BBV::operator<<(int k)
{
    BBV res(*this);
    if (vec != NULL && k > 0) {
        for (int i = 0; i < size; i++)
            res.vec[i] ^= res.vec[i];
        int start = size - 1 - (k - 1) / 8;
        if (k % 8 == 0)
            start = size - 1 - k / 8;
        k %= 8;
        bbyte_t mask = 0;
        for (int i = 0; start - i >= 0; i++) {
            res.vec[start - i] = vec[size - 1 - i] >> k;
            res.vec[start - i] |= mask << (8 - k);
            mask = static_cast<bbyte_t>(-1);
            mask >>= 8 - k;
            mask = vec[size - 1 - i] & mask;
        }
    }
    return res;
}

X BBV::operator[](int k)
{
    if (vec != NULL && k >= 0 && k < len) {
        X ptr(&(vec[k / 8]), k % 8);
        return ptr;
    } else
        throw 1;
}

BBV::operator char*()
{
    if (vec != NULL) {
        char *str = new char[len + 1];
        if (str != NULL) {
            int k = 0;
            bbyte_t mask = 1;
            for (int i = 0; i < len; i++) {
                if (i % 8 == 0 && i != 0) {
                    k++;
                    mask = 1;
                }
                str[i] = (vec[k] & mask) ? '1' : '0';
                mask <<= 1;
            }
            str[len] = '\0';
        } else
            throw 0;
        return str;
    }
    return NULL;
}

int BBV::getWeight()
{
    if (vec != NULL) {
        int count = 0;
        for (int i = 0; i < size; i++) {
            bbyte_t v = vec[i];
            while (v != 0) {
                count++;
                v &= (v - 1);
            }
        }
        return count;
    }
    return 0;
}

int BBV::getSize()
{
    return len;
}

X::X()
{
    ptr = NULL;
    index = 0;
}

X::X(bbyte_t *vec, int k)
{
    if (vec != NULL && k >= 0 && k <= 7) {
        ptr = vec;
        index = k;
    } else
        throw 1;
}

X X::operator=(int k)
{
    if (ptr != NULL) {
        bbyte_t mask = 1;
        mask <<= index;
        if (k == 0)
            *ptr &= ~mask;
        else
            *ptr |= mask;
    } else
        throw 1;
    return *this;
}

X::operator int()
{
    if (ptr != NULL) {
        bbyte_t mask = 1;
        mask <<= index;
        return (*ptr & mask) ? 1 : 0;
    } else
        throw 1;
}

X X::operator=(X &v)
{
    if (ptr != NULL && v.ptr != NULL) {
        bbyte_t mask = 1;
        mask <<= v.index;
        if (mask & *(v.ptr))
            *this = 1;
        else
            *this = 0;
    } else
        throw 1;
    return *this;
}

ostream &operator<<(ostream &r, BBV &V)
{
    if (V.vec != NULL) {
        int i = 0, j = 8, k = 0;
        bbyte_t mask = 1;
        while (i < V.len) {
            if (j > 0) {
                cout << ((V.vec[k] & mask) ? '1' : '0');
                mask <<= 1;
                j--;
                i++;
            } else {
                j = 8;
                k++;
                mask = 1;
            }
        }
        cout << '\n';
    }
    return r;
}

istream &operator>>(istream &r, BBV &V)
{
    int str_size;
    cout << "Input size for BBV:\n";
    cin >> str_size;
    while (str_size < 0) {
        cout << "Re-enter size for BBV:\n";
        cin >> str_size;
    }
    char *str = new char[str_size + 1];
    if (str != NULL) {
        cout << "Input BBV:\n";
        cin >> str;
        V.Init(str);
        V.len = str_size;
        V.size = (str_size - 1) / 8 + 1;
        delete[] str;
    } else
        throw 0;
    return r;
}
