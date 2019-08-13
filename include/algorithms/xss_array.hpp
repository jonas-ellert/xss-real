#pragma once

#include <data_structures/lce/lce_naive.hpp>
#include <stack>

class xss_array {
private:
  template <bool pss_enabled,
            bool nss_enabled,
            typename value_type,
            typename index_type>
  static void run_internal(const value_type* text,
                           [[maybe_unused]] index_type* pss,
                           [[maybe_unused]] index_type* nss,
                           const uint64_t n) {

    std::stack<index_type> H;
    std::stack<index_type> L;
    H.push(0);
    L.push(0);

//    auto compare = lce_naive<value_type>::get_suffix_compare(text, n);
    auto get_lce = lce_naive<value_type>::get_lce(text, n);

    if constexpr (pss_enabled) {
      pss[0] = n;
      pss[n - 1] = n;
    }
    if constexpr (nss_enabled) {
      nss[0] = n - 1;
      nss[n - 1] = n;
    }

    // STACK MUST CONTAIN 0

    index_type j;
    for (index_type i = 1; i < n - 1; ++i) {
      j = i - 1;

      while (text[j] > text[i]) {
        if constexpr (nss_enabled)
          nss[j] = i;
        H.pop();
        L.pop();
        j = H.top();
      }

      index_type lce = get_lce(j, i);

      while (text[j + lce] > text[i + lce]) {
        lce = std::min(lce, L.top());
        if constexpr (nss_enabled)
          nss[j] = i;
        H.pop();
        L.pop();
        j = H.top();
        lce = get_lce(j, i, lce);
      }
      
      H.push(i);
      L.push(lce);
      if constexpr (pss_enabled)
        pss[i] = j;
    }
    if constexpr (nss_enabled) {
      while ((j = H.top()) > 0) {
        nss[j] = n - 1;
        H.pop();
      }
    }
  }

public:
  template <typename value_type, typename index_type>
  static void run(const value_type* text,
                  index_type* pss,
                  index_type* nss,
                  const uint64_t n) {
    if (pss == nullptr && nss == nullptr) {
      return;
    }
    if (pss == nullptr) {
      run_internal<false, true>(text, pss, nss, n);
    } else if (nss == nullptr) {
      run_internal<true, false>(text, pss, nss, n);
    } else {
      run_internal<true, true>(text, pss, nss, n);
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

  template <typename value_type, typename index_type>
  static void run(const value_type*, nullptr_t, nullptr_t, const index_type) {}
};