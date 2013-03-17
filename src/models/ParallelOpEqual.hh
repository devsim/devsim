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

#ifndef PARALLEL_OP_EQUAL_HH
#define PARALLEL_OP_EQUAL_HH

//// Only for the typedef
#include "FPECheck.hh"
#include "mypacket.hh"
class mymutex;
class mycondition;

#include <vector>

template <typename U>
struct SerialVectorVectorOpEqual {
  SerialVectorVectorOpEqual(std::vector<double> &v0, const std::vector<double> &v1, const U &op) :
    vec0_(v0), vec1_(v1), op_(op) {}

  void operator()(const size_t b, const size_t e)
  {
    double *v0 = &vec0_[b];
    const double *v1 = &vec1_[b];
    for (size_t i = b; i < e; ++i)
    {
      op_(*(v0++), *(v1++));
    }
  }

  std::vector<double> &vec0_;
  const std::vector<double> &vec1_;
  const U &op_;
};

template <typename U>
struct SerialVectorScalarOpEqual {
  SerialVectorScalarOpEqual(std::vector<double> &v0, double s, const U &op) :
    vec0_(v0), scalar_(s), op_(op) {}

  void operator()(const size_t b, const size_t e)
  {
    double *v0 = &vec0_[b];
    for (size_t i = b; i < e; ++i)
    {
      op_(*(v0++), scalar_);
    }
  }

  std::vector<double> &vec0_;
  const double scalar_;
  const U &op_;
};

template <typename U>
class OpEqualPacket {
  public:
    //// TODO: refactor MathPacket so not all the arguments are in the constructor
    //// of course this can no longer be const
    explicit OpEqualPacket(U &);

    //// We are done with this unit when this function returns.
    void operator()(const size_t rbeg, const size_t rend);

    void join(const OpEqualPacket<U> &);

    FPECheck::FPEFlag_t getFPEFlag() const;
    size_t              getNumberProcessed() const;

  private:
    OpEqualPacket();
    OpEqualPacket &operator=(const OpEqualPacket &);

    U                    opEqualTask_;
    FPECheck::FPEFlag_t  fpeFlag_;
    size_t               num_processed_;
};

template <typename U> void
OpEqualRun(U &, size_t /*length*/);

template <typename U>
class OpEqualRange : public mypacket
{
  public:
    //// Copy so we have fpe status of each one
    OpEqualRange(U, size_t, size_t, mymutex &, mycondition &, size_t &, size_t);

    void run();

    //// intent is to join the copy with the original
    OpEqualPacket<U> &GetOpEqualPacket();

    ~OpEqualRange() {}

  private:

    OpEqualPacket<U> opEqualPacket_;
    size_t           beg_;
    size_t           end_;
    /// Mutex to lock when updating count
    mymutex     &mutex_;
    /// Condition variable to signal when done
    mycondition &cond_;
    // Count is number of items processed
    size_t      &count_;
    // Count total count of items processed
    size_t       max_count_;
  
};

#endif

