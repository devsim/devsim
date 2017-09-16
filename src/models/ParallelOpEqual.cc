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

#include "ParallelOpEqual.hh"
#include "myThreadPool.hh"
#include "myqueue.hh"
#include "ScalarData.hh"


template <typename U>
OpEqualPacket<U> &OpEqualRange<U>::GetOpEqualPacket()
{
  return opEqualPacket_;
}

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
  //// get from the queue
  myThreadPool &pool = myThreadPool::GetInstance();
  myqueue      &queue = pool.GetQueue();
  const size_t num_threads = queue.GetNumberThreads();
  const size_t task_size = queue.GetMinimumTaskSize();

  if ((num_threads > 1) && vlen > task_size)
  {
    OpEqualPacket<U> MyPacket(task);

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
      mypacket_ptr packet = mypacket_ptr(new OpEqualRange<OpEqualPacket<U> >(MyPacket, b, e, cmutex, ccondition, num_processed, vlen));
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
      MyPacket.join(static_cast<OpEqualRange<U> &>(*packets[i]).GetOpEqualPacket());
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
OpEqualRange<U>::OpEqualRange(U mp, size_t b, size_t e,
    mymutex &mutex,
    mycondition &cond,
    size_t &count, size_t mcount) : opEqualPacket_(mp), beg_(b), end_(e), mutex_(mutex), cond_(cond), count_(count), max_count_(mcount)
{
}

template <typename U>
void OpEqualRange<U>::run()
{
  opEqualPacket_(beg_, end_);

  mutex_.lock();
  count_ += (end_ - beg_);
  if (count_ == max_count_)
  {
    cond_.broadcast();
  }
  mutex_.unlock();
}

#define DBLTYPE double
#include "ParallelOpEqualInstantiate.cc"

#ifdef DEVSIM_EXTENDED_PRECISION
#undef  DBLTYPE
#define DBLTYPE float128
#include "Float128.hh"
#include "ParallelOpEqualInstantiate.cc"
#endif

