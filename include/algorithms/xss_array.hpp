#pragma once

#include <algorithms/duval.hpp>
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
      index_type max_lce = lce;
      index_type max_lce_j = j;

      while (text[j + lce] > text[i + lce]) {
        index_type ltop = L.top();
        if constexpr (nss_enabled)
          nss[j] = i;
        H.pop();
        L.pop();
        j = H.top();

        if (ltop == lce) {
          lce = get_lce(j, i, lce);
          max_lce = lce;
          max_lce_j = j;
        } else {
          lce = std::min(lce, ltop);
        }
      }

      if constexpr (pss_enabled)
        pss[i] = j;
      H.push(i);
      L.push(lce);

      if (unlikely(max_lce > 64)) {
        j = max_lce_j;
        const index_type distance = i - j;
        bool j_smaller_i = text[j + max_lce] < text[i + max_lce];

        // RUN EXTENSION
        if (unlikely(max_lce >= 2 * distance)) {
          const index_type repetitions = max_lce / distance - 1;
          // INCREASING RUN (set start positions of repetitions)
          if (j_smaller_i) {
            for (index_type r = 0; r < repetitions; ++r) {
              i += distance;
              if constexpr (pss_enabled)
                pss[i] = i - distance;
              max_lce -= distance;
              H.push(i);
              L.push(max_lce);
            }
          }
          // DECREASING RUN (set start positions of repetitions)
          else {
            for (index_type r = 0; r < repetitions; ++r) {
              i += distance;
              if constexpr (nss_enabled)
                nss[i] = i + distance;
              if constexpr (pss_enabled)
                pss[i] = pss[j];
            }
            if constexpr (nss_enabled)
              nss[i] = 0;
            H.pop();
            H.push(i);
          }
          // FOR BOTH INCREASING AND DECREASING RUNS (fill in between start
          // positions)
          for (index_type k = j + distance; k < i;) {
            const index_type next_limit = k + distance;
            for (++k; k < next_limit; ++k) {
              if constexpr (nss_enabled)
                nss[k] = nss[k - distance] + distance;
              if constexpr (pss_enabled)
                pss[k] = pss[k - distance] + distance;
            }
          }
        }
        // AMORTIZED LOOK-AHEAD
        else {
          const index_type ell = div<4>(max_lce);
          index_type anchor = ell;

          // check if gamm_ell is an extended lyndon run
          const auto lce_str = &(text[i]);
          const auto duval =
              is_extended_lyndon_run(&(lce_str[ell]), max_lce - ell);

          // try to extend the lyndon run as far as possible to the left
          if (duval.first > 0) {
            const index_type period = duval.first;
            const auto repetition_eq = [&](const index_type l,
                                           const index_type r) {
              for (index_type k = 0; k < period; ++k)
                if (unlikely(lce_str[l + k] != lce_str[r + k]))
                  return false;
              return true;
            };
            int64_t lhs = ell + duval.second - period;
            while (lhs >= 0 && repetition_eq(lhs, lhs + period)) {
              lhs -= period;
            }
            anchor = std::min(ell, (index_type)(lhs + 2 * period));
          }

          //           if(anchor < ell)
//          std::cout << "AL " << j << " " << i << " " << max_lce << " " << ell
//                    << " " << anchor << std::endl;

          for (index_type k = 1; k < anchor; ++k) {
            if constexpr (nss_enabled)
              if (nss[j + k] < j + anchor) {
                nss[i + k] = nss[j + k] + distance;
//                std::cout << i + k << " " << nss[i + k] << " " << nss[j + k]
//                          << std::endl;
              }
            if constexpr (pss_enabled)
              pss[i + k] = pss[j + k] + distance;
          }

          if constexpr (nss_enabled) {
            for (index_type k = 1; k < anchor; ++k) {
              if (nss[i + k] == 0) {
                L.push(get_lce(H.top(), i + k));
                H.push(i + k);
              }
            }
          } else {
            for (index_type k = 1; k < anchor; ++k) {
              while (H.top() != pss[i + k]) {
                H.pop();
                L.pop();
              }
              L.push(get_lce(H.top(), i + k));
              H.push(i + k);
            }
          }

          i += anchor - 1;
        }
      }
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