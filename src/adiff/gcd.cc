/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include <iostream>
#include <cmath>
using namespace std;
// actually do it
// large must be first
int gcd_do(int large, int small)
{
    cout << "gcd " << large << " " << small << endl;
    int rem = large % small;

    if (rem == 0)
    {
        return small;
    }
    else if ((rem == 1) && (small > 10000))
    {
        // precision of 1 part in 5
        return small;
    }

    return gcd_do(small, rem);
}

// for now expect positive ints
// doesn't handle zeros
int gcd(int x, int y)
{
    if (x == y)
    {
        return x;
    }

    x = std::abs(x);
    y = std::abs(y);
    if (x < y)
    {
        return gcd_do(y,x);
    }

    return gcd_do(x,y);

}
int main()
{
    // Binary euclid algorithm is probably faster
    // Make sure don't surpass any 32 bit limits
    int x = 1333333333;
    int y = 1000000000;
    int g = gcd(x,y);
    cout << g << endl;

    // in integer arithmetic we filter out any pesky remainders
    cout << "x / y = " << x/g << " / " << y/g << endl;


}
