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

#ifndef MYQUEUE_HH
#define MYQUEUE_HH
#include "mymutex.hh"
#include "mycondition.hh"
#include "mypacket.hh"

#include <list>
#include <vector>

#include "dsmemory.hh"
class myworker;
typedef std::tr1::shared_ptr<myworker> myworker_ptr;

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
