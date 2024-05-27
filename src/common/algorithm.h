#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <algorithm>
#include <iterator>

namespace algorithm {

template <typename T, typename Containter, typename Lambda>
T transform(const Containter &container, Lambda &&lambda) {
  T res;
  std::transform(std::begin(container), std::end(container),
                 std::back_inserter(res), lambda);
  return res;
}

}  // namespace algorithm

#endif /* !ALGORITHM_H */
