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

#ifndef MATHPACKET_HH
#define MATHPACKET_HH

#include "FPECheck.hh"
#include "mypacket.hh"

#include <string>
#include <vector>

class mycondition;
class mymutex;

namespace Eqomfp {
class MathWrapper;
class MathPacket;

class MathPacket {
  public:
    MathPacket(const MathWrapper &, const std::vector<double> &, const std::vector<const std::vector<double> *> &, std::vector<double> &);

    //// We are done with this unit when this function returns.
    void operator()(const size_t rbeg, const size_t rend);

    void join(const MathPacket &);

    FPECheck::FPEFlag_t getFPEFlag() const;
    const std::string & getErrorString() const;
    size_t              getNumberProcessed() const;
  private:
    MathPacket();
//    ~MathPacket();
//    MathPacket(const MathPacket &);

    MathPacket &operator=(const MathPacket &);

    const MathWrapper         &wrapperClass_;
    const std::vector<double>                       &dvals_;
    const std::vector<const std::vector<double> *>  &vvals_;
    std::vector<double>                             &result_;
    std::string                                      errorString_;
    FPECheck::FPEFlag_t                              fpeFlag_;
    size_t                                           num_processed_;
};


std::string
MathPacketRun(const MathWrapper &, const std::vector<double> &, const std::vector<const std::vector<double> *> &, std::vector<double> &, size_t);

class MathPacketRange : public mypacket
{
  public:
    //// Copy so we have fpe status of each one
    MathPacketRange(MathPacket, size_t, size_t,
        mymutex &,
        mycondition &,
        size_t &,
        size_t
    );
    void run();

    //// intent is to join the copy with the original
    const MathPacket &GetMathPacket();

    ~MathPacketRange() {}
  private:

    MathPacket mathPacket_;
    size_t     beg_;
    size_t     end_;
    /// Mutex to lock when updating count
    mymutex     &mutex_;
    /// Condition variable to signal when done
    mycondition &cond_;
    // Count is number of items processed
    size_t      &count_;
    size_t       max_count_;
  
};
}
#endif
