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

#include "MathPacket.hh"
#include "MathWrapper.hh"

#include "myThreadPool.hh"
#include "myqueue.hh"

namespace Eqomfp {

const MathPacket &MathPacketRange::GetMathPacket()
{
  return mathPacket_;
}

FPECheck::FPEFlag_t MathPacket::getFPEFlag() const
{
  return fpeFlag_;
}

const std::string &MathPacket::getErrorString() const
{
  return errorString_;
}

size_t MathPacket::getNumberProcessed() const
{
  return num_processed_;
}

MathPacket::MathPacket(const MathWrapper &wrapper, const std::vector<double> &dvals, const std::vector<const std::vector<double> *> &vvals, std::vector<double> &result) : wrapperClass_(wrapper), dvals_(dvals), vvals_(vvals), result_(result), fpeFlag_(FPECheck::getClearedFlag()), num_processed_(0)
{
}

//http://software.intel.com/en-us/forums/intel-threading-building-blocks/topic/70959/
void MathPacket::join(const MathPacket &omp)
{
  fpeFlag_        = FPECheck::combineFPEFlags(fpeFlag_, omp.fpeFlag_);
  num_processed_ += omp.num_processed_;
  if (errorString_ != omp.errorString_)
  {
    errorString_ += omp.errorString_;
  }
}

void MathPacket::operator()(size_t vbeg, size_t vend)
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


std::string MathPacketRun(const MathWrapper &func, const std::vector<double> &dvals, const std::vector<const std::vector<double> *> &vvals, std::vector<double> &result, size_t vlen)
{
  std::string errorString;

  //// get from the queue
  myThreadPool &pool = myThreadPool::GetInstance();
  myqueue      &queue = pool.GetQueue();
  const size_t num_threads = queue.GetNumberThreads();
  const size_t task_size = queue.GetMinimumTaskSize();

  if ((num_threads > 1) && vlen > task_size)
  {
    Eqomfp::MathPacket MyPacket(func, dvals, vvals, result);

    mymutex     cmutex;
    mycondition ccondition;
    size_t      num_processed(0);

    std::vector<mypacket_ptr> packets;

    ///// TODO: This code should be generalized and made available for all thread scheduling
    ///// e.g. GetSchedule function for queue
    const size_t step = vlen / num_threads;
    size_t b = 0;
    size_t e = (step == 0) ? vlen : step;
    while (b < e)
    {
      mypacket_ptr packet = mypacket_ptr(new MathPacketRange(MyPacket, b, e, cmutex, ccondition, num_processed, vlen));
      packets.push_back(packet);

      b = e;
      e += step;
      //// If we have 2 threads, take odd off so don't need additional queue member
      //// or just use a magic number
      if (e > (vlen - 2))
      {
        e = vlen;
      }
    }

    queue.AddTasks(packets);

    
    cmutex.lock();
    while (num_processed < vlen)
    {
        ccondition.wait(cmutex);
    }
    cmutex.unlock();

    for (size_t i = 0; i < packets.size(); ++i)
    {
      MyPacket.join(static_cast<MathPacketRange &>(*packets[i]).GetMathPacket());
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

MathPacketRange::MathPacketRange(MathPacket mp, size_t b, size_t e,
    mymutex &mutex,
    mycondition &cond,
    size_t &count,
    size_t mcount) : mathPacket_(mp), beg_(b), end_(e), mutex_(mutex), cond_(cond), count_(count), max_count_(mcount)
{
}


void MathPacketRange::run()
{
  mathPacket_(beg_, end_);

  mutex_.lock();
  count_ += (end_ - beg_);
  if (count_ == max_count_)
  {
    cond_.broadcast();
  }
  mutex_.unlock();
}

}

