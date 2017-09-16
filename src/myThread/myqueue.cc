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

#include "myqueue.hh"
#include "myworker.hh"
#include "dsAssert.hh"
#include "OutputStream.hh"
#include <sstream>

myqueue::myqueue(size_t num, size_t mts) : num_threads_(num), minimum_task_size_(mts), threadExit_(0)
{
}

myqueue::~myqueue()
{
  ExitThreads();
}

void myqueue::ExitThreads()
{
  threadMutex_.lock();
  threadExit_ = 1;
  threadCondition_.broadcast();
  threadMutex_.unlock();

  for (size_t i = 0; i < threads.size(); ++i)
  {
    threads[i]->join();
  }
  threads.clear();
  //// prevents future threads from exiting immediately
  threadExit_ = 0;
}

void myqueue::StartThreads()
{
  dsAssert(threads.empty(), "UNEXPECTED");
  threadExit_ = 0;
  for (size_t i=0; i < num_threads_; ++i)
  {
    threads.push_back(myworker_ptr(new myworker(threadMutex_, threadCondition_, threadQueue_, threadExit_)));
//	std::cout << "create thread" << endl;
    threads.back()->start();
  }
//    std::cout << "done create thread" << endl;
  std::ostringstream os;
  os << "Using " << num_threads_ << " threads.\n";
  os << "Using " << minimum_task_size_ << " minimum task size.\n";
  OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
}

void myqueue::ChangeNumberThreads(size_t newnum)
{
  if (newnum < num_threads_)
  {
    //// All threads share the same condition variable
    ExitThreads();
    num_threads_ = newnum;
    StartThreads();
  }
  else
  {
    const size_t num_to_start = newnum - num_threads_;
    for (size_t i = 0; i < num_to_start; ++i)
    {
      myworker_ptr myp = myworker_ptr(new myworker(threadMutex_, threadCondition_, threadQueue_, threadExit_));
      threads.push_back(myp);
      myp->start();
    }
    num_threads_ = newnum;
    std::ostringstream os;
    os << "Using " << num_threads_ << " threads.\n";
    OutputStream::WriteOut(OutputStream::OutputType::INFO, os.str());
  }
}


void myqueue::AddTask(mypacket_ptr npp)
{

  threadMutex_.lock();
  threadQueue_.push_back(npp);
  threadCondition_.broadcast();
  threadMutex_.unlock();
}

void myqueue::AddTasks(std::vector<mypacket_ptr> npps)
{

  threadMutex_.lock();
  for (size_t i = 0; i < npps.size(); ++i)
  {
    threadQueue_.push_back(npps[i]);
  }
  threadCondition_.broadcast();
  threadMutex_.unlock();
}

