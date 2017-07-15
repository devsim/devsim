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

#ifndef MATHPACKET_HH
#define MATHPACKET_HH

#include "FPECheck.hh"
#include "mypacket.hh"

#include <string>
#include <vector>

class mycondition;
class mymutex;

namespace Eqomfp {

template <typename DoubleType>
class MathWrapper;

template <typename DoubleType>
class MathPacket {
  public:
    MathPacket(const MathWrapper<DoubleType> &, const std::vector<DoubleType> &, const std::vector<const std::vector<DoubleType> *> &, std::vector<DoubleType> &);

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

    const MathWrapper<DoubleType>         &wrapperClass_;
    const std::vector<DoubleType>                       &dvals_;
    const std::vector<const std::vector<DoubleType> *>  &vvals_;
    std::vector<DoubleType>                             &result_;
    std::string                                      errorString_;
    FPECheck::FPEFlag_t                              fpeFlag_;
    size_t                                           num_processed_;
};


template <typename DoubleType>
std::string MathPacketRun(const MathWrapper<DoubleType> &, const std::vector<DoubleType> &, const std::vector<const std::vector<DoubleType> *> &, std::vector<DoubleType> &, size_t);

template <typename DoubleType>
class MathPacketRange : public mypacket
{
  public:
    //// Copy so we have fpe status of each one
    MathPacketRange(MathPacket<DoubleType>, size_t, size_t,
        mymutex &,
        mycondition &,
        size_t &,
        size_t
    );
    void run();

    //// intent is to join the copy with the original
    const MathPacket<DoubleType> &GetMathPacket();

    ~MathPacketRange() {}
  private:

    MathPacket<DoubleType> mathPacket_;
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
