/***
DEVSIM
Copyright 2024 DEVSIM LLC

SPDX-License-Identifier: Apache-2.0
***/

#ifndef DS_MODEL_FACTORY_HH
#define DS_MODEL_FACTORY_HH
#include <utility>
#include <memory>
template <typename T>
class dsModelFactory {
    public:
      template <typename ... Args>
      static std::shared_ptr<T> create(Args &&...args)
      {
        auto ret = std::shared_ptr<T>(new T(std::forward<Args>(args)...));
        ret->init();
        return ret;
      }
};

#endif
