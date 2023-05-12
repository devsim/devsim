/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef PARALLEL_OP_EQUAL_HH
#define PARALLEL_OP_EQUAL_HH

//// Only for the typedef
#include "FPECheck.hh"

#include <vector>

template <typename U, typename DoubleType>
struct SerialVectorVectorOpEqual {
  SerialVectorVectorOpEqual(std::vector<DoubleType> &v0, const std::vector<DoubleType> &v1, const U &op) :
    vec0_(v0), vec1_(v1), op_(op) {}

  void operator()(const size_t b, const size_t e)
  {
    DoubleType *v0 = &vec0_[b];
    const DoubleType *v1 = &vec1_[b];
    for (size_t i = b; i < e; ++i)
    {
      op_(*(v0++), *(v1++));
    }
  }

  std::vector<DoubleType> &vec0_;
  const std::vector<DoubleType> &vec1_;
  const U &op_;
};

template <typename U, typename DoubleType>
struct SerialVectorScalarOpEqual {
  SerialVectorScalarOpEqual(std::vector<DoubleType> &v0, DoubleType s, const U &op) :
    vec0_(v0), scalar_(s), op_(op) {}

  void operator()(const size_t b, const size_t e)
  {
    DoubleType *v0 = &vec0_[b];
    for (size_t i = b; i < e; ++i)
    {
      op_(*(v0++), scalar_);
    }
  }

  std::vector<DoubleType> &vec0_;
  const DoubleType scalar_;
  const U &op_;
};

template <typename U>
class OpEqualPacket {
  public:
    explicit OpEqualPacket(U &);

    //// We are done with this unit when this function returns.
    void operator()(const size_t rbeg, const size_t rend);

    void join(const OpEqualPacket<U> &);

    FPECheck::FPEFlag_t getFPEFlag() const;
    size_t              getNumberProcessed() const;

  private:
    OpEqualPacket();
    OpEqualPacket &operator=(const OpEqualPacket &) = delete;
    OpEqualPacket(const OpEqualPacket &) = delete;

    U                    opEqualTask_;
    FPECheck::FPEFlag_t  fpeFlag_;
    size_t               num_processed_;
};

template <typename U> void
OpEqualRun(U &, size_t /*length*/);

template <typename U>
class OpEqualRange
{
  public:
    //// Copy so we have fpe status of each one
    OpEqualRange(U &, size_t /*beg*/, size_t /*end*/);

    void operator()();

    ~OpEqualRange() {}

  private:

    U            &opEqualPacket_;
    size_t           beg_;
    size_t           end_;
};

#endif

