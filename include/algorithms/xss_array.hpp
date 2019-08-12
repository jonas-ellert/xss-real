#pragma once

#include <data_structures/lce/lce_naive.hpp>
#include <stack>

class xss_array {
private:
  template <bool pss_enabled,
            bool nss_enabled,
            typename value_type,
            typename index_type,
            typename stack_type>
  static void run_internal(const value_type* text,
                           [[maybe_unused]] index_type* pss,
                           [[maybe_unused]] index_type* nss,
                           stack_type& stack,
                           const uint64_t n) {
    auto compare = lce_naive<value_type>::get_suffix_compare(text, n);
    if constexpr (pss_enabled) {
      pss[0] = n;
      pss[n - 1] = n;
    }
    if constexpr (nss_enabled) {
      nss[0] = n - 1;
      nss[n - 1] = n;
    }

    // STACK MUST CONTAIN 0

    uint64_t j;
    for (uint64_t i = 1; i < n - 1; ++i) {
      j = i - 1;
      while (compare(i, j)) {
        if constexpr (nss_enabled)
          nss[j] = i;
        if constexpr (!pss_enabled) {
          stack.pop();
          j = stack.top();
        } else {
          j = pss[j];
        }
      }
      if constexpr (!pss_enabled) {
        stack.push(i);
      } else {
        pss[i] = j;
      }
    }
    if constexpr (nss_enabled) {
      if constexpr (!pss_enabled) {
        while ((j = stack.top()) > 0) {
          nss[j] = n - 1;
          stack.pop();
        }
      } else {
        j = n - 2;
        while (j > 0) {
          nss[j] = n - 1;
          j = pss[j];
        }
      }
    }
  }

public:
  template <typename value_type, typename index_type>
  static void run(const value_type* text,
                  index_type* pss,
                  index_type* nss,
                  const uint64_t n) {
    std::stack<index_type> stack;
    stack.push(0);

    if (pss == nullptr) {
      run_internal<false, true>(text, pss, nss, stack, n);
    } else if (nss == nullptr) {
      run_internal<true, false>(text, pss, nss, stack, n);
    } else {
      run_internal<true, true>(text, pss, nss, stack, n);
    }
  }

  template <typename value_type, typename index_type>
  static void
  run(const value_type* text, index_type* pss, nullptr_t, const uint64_t n) {
    run(text, pss, (index_type*) nullptr, n);
  }

  template <typename value_type, typename index_type>
  static void
  run(const value_type* text, nullptr_t, index_type* nss, const uint64_t n) {
    run(text, (index_type*) nullptr, nss, n);
  }

  template <typename value_type>
  static void run(const value_type*, nullptr_t, nullptr_t, const uint64_t) {}
};