/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef VECTOROPACKET_HH
#define VECTOROPACKET_HH
#include "mypacket.hh"
#include "mymutex.hh"
#include "mycondition.hh"
//#include <vector>
class mymutex;
class mycondition;
/**
* Assume input and output queue are the same type
* T is the type of input data
* U is the type class for the functor, it should take an input and output
* vector of type T.
* May need to provide additional typename V for the data to functor
*/
#if 0
			typename std::vector<T>::iterator /*ibeg*/,
			typename std::vector<T>::iterator /*iend*/,
			typename std::vector<T>::iterator /*obeg*/,
#endif
template <typename T, typename U>
class vectorpacket : public mypacket {
    public:
	vectorpacket(
			T /*ibeg*/,
			T /*iend*/,
			T /*obeg*/,
			U /*functor class*/,
			mymutex &,
			mycondition &,
			size_t &
		    );
	void run();
	/// iterator which marks the beginning of input data
	T ibeg;
	/// iterator which marks one past the end of input data
	T iend;
	/// iterator which marks one beginning of the output data
	T obeg;
	/// functor class (may contain data)
	U     functor; /* does the magic*/
	/// Mutex to lock when updating count
	mymutex     &mutex;
	/// Condition variable to signal when done
	mycondition &cond;
	// Count is number of items processed
	size_t      &count;
};

template <typename T, typename U>
vectorpacket<T, U>::vectorpacket(
		T ib/*ibeg*/,
		T ie/*iend*/,
		T ob /*obeg*/,
		U f/*functor class*/,
		mymutex &m,
		mycondition &c,
		size_t &cnt
	    ) :
	    ibeg(ib),
	    iend(ie),
	    obeg(ob),
	    functor(f),
	    mutex(m),
	    cond(c),
	    count(cnt)
{
}

template <typename T, typename U>
void vectorpacket<T, U>::run()
{
    T i = ibeg; 
    T o = obeg; 
    for ( ; i != iend; ++i, ++o)
    {
	functor(*i, *o);
    }
    size_t cnt = o - obeg;
    mutex.lock();
    count += cnt;
//    std::cout << "processed " << count << std::endl;
    cond.broadcast();
    mutex.unlock();
}
#endif

