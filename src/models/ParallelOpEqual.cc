/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "ParallelOpEqual.hh"
#include "ScalarData.hh"
#include "GetNumberOfThreads.hh"
#include <future>
#include <memory>


template <typename U>
FPECheck::FPEFlag_t OpEqualPacket<U>::getFPEFlag() const
{
  return fpeFlag_;
}

template <typename U>
size_t OpEqualPacket<U>::getNumberProcessed() const
{
  return num_processed_;
}

template <typename U>
OpEqualPacket<U>::OpEqualPacket(U &opequaltask) : opEqualTask_(opequaltask), fpeFlag_(FPECheck::getClearedFlag()), num_processed_(0)
{
}

//http://software.intel.com/en-us/forums/intel-threading-building-blocks/topic/70959/
template <typename U>
void OpEqualPacket<U>::join(const OpEqualPacket<U> &omp)
{
  fpeFlag_        = FPECheck::combineFPEFlags(fpeFlag_, omp.fpeFlag_);
  num_processed_ += omp.num_processed_;
}

template <typename U>
void OpEqualPacket<U>::operator()(size_t vbeg, size_t vend)
{
  fpeFlag_ = FPECheck::getClearedFlag();
  ///// This should not be called in the main thread!!!!!
  ///// otherwise preexisting exceptions would not be processed
  ///// This can be asserted since we already know the main thread id on application startup
  FPECheck::ClearFPE();
  opEqualTask_(vbeg, vend);
  fpeFlag_       = FPECheck::getFPEFlags();
  num_processed_ = vend - vbeg;
}


//// TODO: synchronize with MathPacket
template <typename U>
void OpEqualRun(U &task, size_t vlen)
{
  const size_t num_threads = ThreadInfo::GetNumberOfThreads();
  const size_t task_size   = ThreadInfo::GetMinimumTaskSize();

  if ((num_threads > 1) && vlen > task_size)
  {
    OpEqualPacket<U> MyPacket(task);

    std::vector<std::shared_ptr<OpEqualPacket<U>>> packets;
    std::vector<std::future<void>> futures;

    ///// TODO: This code should be generalized and made available for all thread scheduling
    ///// e.g. GetSchedule function for queue
    const size_t step = vlen / num_threads;
    size_t b = 0;
    size_t e = (step == 0) ? vlen : step;
    while (b < e)
    {
      auto packet = std::shared_ptr<OpEqualPacket<U>>(new OpEqualPacket<U>(task));
      packets.push_back(packet);
      futures.push_back(std::async(std::launch::async, OpEqualRange<OpEqualPacket<U> >(*packet, b, e)));

      b = e;
      e += step;
      //// If we have 2 threads, take odd off so don't need additional queue member
      //// or just use a magic number
      if (e > (vlen - 2))
      {
        e = vlen;
      }
    }


    // process the futures
    for (auto &p : futures)
    {
      p.get();
    }

    // get the results
    for (auto &p : packets)
    {
      MyPacket.join(*p);
    }

    if (FPECheck::CheckFPE(MyPacket.getFPEFlag()))
    {
      //// Raise FPE in the main thread
      FPECheck::raiseFPE(MyPacket.getFPEFlag());
    }

  }
  else
  {
    task(0, vlen);
  }
}


template <typename U>
OpEqualRange<U>::OpEqualRange(U &mp, size_t b, size_t e) :
  opEqualPacket_(mp), beg_(b), end_(e)
{
}

template <typename U>
void OpEqualRange<U>::operator()()
{
  opEqualPacket_(beg_, end_);
}

#define DBLTYPE double
#include "ParallelOpEqualInstantiate.cc"

#ifdef DEVSIM_EXTENDED_PRECISION
#undef  DBLTYPE
#define DBLTYPE float128
#include "Float128.hh"
#include "ParallelOpEqualInstantiate.cc"
#endif

