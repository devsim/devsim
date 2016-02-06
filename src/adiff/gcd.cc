/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
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

    x = abs(x);
    y = abs(y);
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
