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

#ifndef MYQUEUE_HH
#define MYQUEUE_HH
#include "mymutex.hh"
#include "mycondition.hh"
#include "mypacket.hh"

#include <list>
#include <vector>

#include <memory>
class myworker;
typedef std::shared_ptr<myworker> myworker_ptr;

class myqueue
{
  public:
    explicit myqueue(size_t /*num_threads*/, size_t /*min_task_size*/);
    void StartThreads();
    void ExitThreads();
    ~myqueue();

    void AddTask(mypacket_ptr);

    void AddTasks(std::vector<mypacket_ptr>);

    size_t GetNumberThreads() const
    {
      return num_threads_;
    }

    size_t GetMinimumTaskSize() const
    {
      return minimum_task_size_;
    }

    void SetMinimumTaskSize(size_t newmts)
    {
      minimum_task_size_ = newmts;
    }

    void ChangeNumberThreads(size_t);
  private:
    myqueue();
    //~myqueue();
    myqueue(const myqueue &);
    myqueue &operator=(const myqueue &);

    std::list<mypacket_ptr>    threadQueue_;
    std::vector<myworker_ptr > threads;
    size_t                     num_threads_;
    size_t                     minimum_task_size_;
    mymutex                    threadMutex_;
    mycondition                threadCondition_;
    size_t                     threadExit_;

};
#endif
