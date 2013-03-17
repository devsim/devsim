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

#ifndef MYCONDITION_HH
#define MYCONDITION_HH
class mymutex;
/**
 * Creates a condition variable.  This condition variable is passed by
 * reference to all classes who require it.  Therefore, it must not go out of
 * scope until the last thread requiring it is finished.
 */
class mycondition {
    public:
	mycondition();
	~mycondition();
#if 0
	/// signal only one of the threads blocking on this variable
	void signal();
#endif
	/// signal on all of the threads blocking on this variable
	void broadcast();

	void wait(mymutex &);
    private:

	/// friendship is needed since we can tell a mutex to lock on its
	/// associated condition variable
	void *cond;
};

#endif
