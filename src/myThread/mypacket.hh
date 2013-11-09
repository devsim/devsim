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

#ifndef MYPACKET_HH
#define MYPACKET_HH
#include <memory>
class mypacket;
typedef std::shared_ptr<mypacket> mypacket_ptr;

/**
 * This class is the basis for any work to be queued by
 * the myworker class.
 *
 * Any derived class must provide a run method to be executed.
 * See vectorpacket for an implementation where data is actually provided.
 */
class mypacket{
    public:
	virtual ~mypacket() = 0;
	virtual void run() = 0;
};

#endif
