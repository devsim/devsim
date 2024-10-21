#ifndef DS_MODEL_FACTORY_HH
#define DS_MODEL_FACTORY_HH
#include <utility>
template <typename T>
class dsModelFactory {
    public:
      template <typename ... Args>
      static T * create(Args &&...args)
      {
        return new T(std::forward<Args>(args)...);
      }
};

#endif
