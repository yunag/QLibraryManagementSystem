#ifndef INVOKER_H
#define INVOKER_H

#include <QMetaObject>
#include <QPromise>

template <class Object, class Func, class... Args>
auto invokeAsFuture(Object &context, Func func, Args &&...args) {
  using ReturnType = decltype((context.*func)(args...));

  QPromise<ReturnType> promise;
  auto future = promise.future();

  /* TODO: Perfect forward capture */
  QMetaObject::invokeMethod(
    &context,
    [=, &context, promise = std::move(promise)]() mutable {
      try {
        if constexpr (!std::is_void<ReturnType>::value) {
          auto res = (context.*func)(std::forward<Args>(args)...);
          promise.addResult(std::move(res));
        } else {
          (context.*func)(std::forward<Args>(args)...);
        }
      } catch (...) {
        promise.setException(std::current_exception());
      }
      promise.finish();
    },
    Qt::QueuedConnection);

  return future;
}

#endif /* !INVOKER_H */
