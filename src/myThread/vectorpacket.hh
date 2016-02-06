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

