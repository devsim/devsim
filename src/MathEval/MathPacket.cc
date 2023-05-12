/***
DEVSIM
Copyright 2013 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#include "MathPacket.hh"
#include "MathWrapper.hh"
#include "GetNumberOfThreads.hh"
#include <future>
#include <memory>

#ifdef DEVSIM_EXTENDED_PRECISION
#include "Float128.hh"
#endif

namespace Eqomfp {

template <typename DoubleType>
FPECheck::FPEFlag_t MathPacket<DoubleType>::getFPEFlag() const
{
  return fpeFlag_;
}

template <typename DoubleType>
const std::string &MathPacket<DoubleType>::getErrorString() const
{
  return errorString_;
}

template <typename DoubleType>
size_t MathPacket<DoubleType>::getNumberProcessed() const
{
  return num_processed_;
}

template <typename DoubleType>
MathPacket<DoubleType>::MathPacket(const MathWrapper<DoubleType> &wrapper, const std::vector<DoubleType> &dvals, const std::vector<const std::vector<DoubleType> *> &vvals, std::vector<DoubleType> &result) : wrapperClass_(wrapper), dvals_(dvals), vvals_(vvals), result_(result), fpeFlag_(FPECheck::getClearedFlag()), num_processed_(0)
{
}

//http://software.intel.com/en-us/forums/intel-threading-building-blocks/topic/70959/
template <typename DoubleType>
void MathPacket<DoubleType>::join(const MathPacket &omp)
{
  fpeFlag_        = FPECheck::combineFPEFlags(fpeFlag_, omp.fpeFlag_);
  num_processed_ += omp.num_processed_;
  if (errorString_ != omp.errorString_)
  {
    errorString_ += omp.errorString_;
  }
}

template <typename DoubleType>
void MathPacket<DoubleType>::operator()(size_t vbeg, size_t vend)
{
  fpeFlag_ = FPECheck::getClearedFlag();
  ///// This should not be called in the main thread!!!!!
  ///// otherwise preexisting exceptions would not be processed
  ///// This can be asserted since we already know the main thread id on application startup
  FPECheck::ClearFPE();
  wrapperClass_.Evaluate(dvals_, vvals_, errorString_, result_, vbeg, vend);
  fpeFlag_       = FPECheck::getFPEFlags();
  num_processed_ = vend - vbeg;
}


template <typename DoubleType>
std::string MathPacketRun(const MathWrapper<DoubleType> &func, const std::vector<DoubleType> &dvals, const std::vector<const std::vector<DoubleType> *> &vvals, std::vector<DoubleType> &result, size_t vlen)
{
  std::string errorString;

  const size_t num_threads = ThreadInfo::GetNumberOfThreads();
  const size_t task_size   = ThreadInfo::GetMinimumTaskSize();

  if ((num_threads > 1) && vlen > task_size)
  {
    Eqomfp::MathPacket<DoubleType> MyPacket(func, dvals, vvals, result);

    std::vector<std::shared_ptr<Eqomfp::MathPacket<DoubleType>>> packets;
    std::vector<std::future<void>> futures;

    const size_t step = vlen / num_threads;
    size_t b = 0;
    size_t e = (step == 0) ? vlen : step;
    while (b < e)
    {
      auto packet = std::shared_ptr<Eqomfp::MathPacket<DoubleType>>(new MathPacket<DoubleType>(func, dvals, vvals, result));
      packets.push_back(packet);
      futures.push_back(std::async(std::launch::async, MathPacketRange<DoubleType>(*packet, b, e)));

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

    errorString += MyPacket.getErrorString();
    if (FPECheck::CheckFPE(MyPacket.getFPEFlag()))
    {
      //// Raise FPE in the main thread
      FPECheck::raiseFPE(MyPacket.getFPEFlag());
    }

  }
  else
  {
    func.Evaluate(dvals, vvals, errorString, result, 0, vlen);
  }

  return errorString;
}

template <typename DoubleType>
MathPacketRange<DoubleType>::MathPacketRange(MathPacket<DoubleType> &mp, size_t b, size_t e)
  : mathPacket_(mp), beg_(b), end_(e)
{
}


template <typename DoubleType>
void MathPacketRange<DoubleType>::operator()()
{
  mathPacket_(beg_, end_);
}

template class MathPacket<double>;
template std::string MathPacketRun(const MathWrapper<double> &, const std::vector<double> &, const std::vector<const std::vector<double> *> &, std::vector<double> &, size_t);
#ifdef DEVSIM_EXTENDED_PRECISION
template class MathPacket<float128>;
template std::string MathPacketRun(const MathWrapper<float128> &, const std::vector<float128> &, const std::vector<const std::vector<float128> *> &, std::vector<float128> &, size_t);
#endif
}

