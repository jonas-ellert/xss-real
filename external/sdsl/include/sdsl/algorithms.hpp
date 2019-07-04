/* sdsl - succinct data structures library
    Copyright (C) 2008 Simon Gog

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/ .

    Modified by Jonas Ellert in March 2019:
    This file contains only the psv algorithm of the sdsl. A modified variant
    of the algorithm have been added, such that also next and previous smaller
    suffixes can be calculated.
*/
/*! \file algorithms.hpp
    \brief algorithms.hpp contains algorithms for suffixarrays.
	\author Simon Gog
*/

#pragma once

#include <stack>
#include <assert.h>

namespace sdsl
{

//! A helper class containing algorithms for succinct data structures.
/*!
	\author Simon Gog
 */
namespace algorithm
{

// original sdsl nsv
template<class RandomAccessContainer1, class RandomAccessContainer2>
void calculate_nsv(const RandomAccessContainer1& a, RandomAccessContainer2& nsv)
{
  assert(nsv.size() == a.size());
  if (a.empty())
    return;
  nsv[nsv.size()-1] = 0;
  std::stack<typename RandomAccessContainer1::size_type> nsv_index;
  typename RandomAccessContainer1::value_type min_element = a[nsv.size()-1];
  for (typename RandomAccessContainer1::size_type i=nsv.size(); i > 0; --i) {
    if (a[i-1] <= min_element) {
      while (!nsv_index.empty())
        nsv_index.pop();
      min_element = a[i-1];
      nsv[i-1] = 0;
      nsv_index.push(i-1);
    } else { // a[i] > min_element => stack will not be empty
      while (a[nsv_index.top()] >= a[i-1])
        nsv_index.pop();
      nsv[i-1] = nsv_index.top();
      nsv_index.push(i-1);
    }
  }
}

// modified to take suffix comparisons
template<class SuffixComparison, class RandomAccessContainer>
void calculate_nss(SuffixComparison& compare, RandomAccessContainer& nss)
{
  if (nss.empty())
    return;
  nss[nss.size()-1] = nss.size();
  std::stack<typename RandomAccessContainer::size_type> nss_index;
  typename RandomAccessContainer::size_type min_idx = nss.size()-1;
  nss_index.push(min_idx);
  for (typename RandomAccessContainer::size_type i=nss.size() - 1; i > 0; --i) {
    if (compare(i - 1, min_idx)) {
      while (!nss_index.empty())
        nss_index.pop();
      min_idx = i-1;
      nss[i-1] = nss.size();
      nss_index.push(i-1);
    } else { // a[i] > min_element => stack will not be empty
      while (compare(i - 1, nss_index.top()))
        nss_index.pop();
      nss[i-1] = nss_index.top();
      nss_index.push(i-1);
    }
  }
}

// original sdsl psv
template<class RandomAccessContainer1, class RandomAccessContainer2>
void calculate_psv(const RandomAccessContainer1& a, RandomAccessContainer2& psv)
{
  assert(psv.size() == a.size());
  if (a.empty())
    return;
  psv[0] = psv.size();
  assert(psv[0] == psv.size());
  std::stack<typename RandomAccessContainer1::size_type> psv_index;
  typename RandomAccessContainer1::value_type min_element = a[0];
  for (typename RandomAccessContainer1::size_type i=0; i < a.size(); ++i) {
    if (a[i] <= min_element) {
      while (!psv_index.empty())
        psv_index.pop();
      min_element = a[i];
      psv[i] = a.size();
      psv_index.push(i);
    } else { // a[i] > min_element => stack will not be empty
      while (a[psv_index.top()] >= a[i])
        psv_index.pop();
      psv[i] = psv_index.top();
      psv_index.push(i);
    }
  }
}

// modified to take suffix comparisons
template<class SuffixComparison, class RandomAccessContainer>
void calculate_pss(SuffixComparison &compare, RandomAccessContainer& pss) {
  if (pss.empty())
    return;
  std::stack<typename RandomAccessContainer::size_type> pss_index;
  typename RandomAccessContainer::size_type min_idx = 0;
  pss[min_idx] = pss.size();
  pss_index.push(min_idx);
  for (typename RandomAccessContainer::size_type i = 1; i < pss.size(); ++i) {
    if (compare(i, min_idx)) {
      while (!pss_index.empty())
        pss_index.pop();
      min_idx = i;
      pss[i] = pss.size();
      pss_index.push(i);
    } else { // a[i] > min_element => stack will not be empty
      while (compare(i, pss_index.top()))
        pss_index.pop();
      pss[i] = pss_index.top();
      pss_index.push(i);
    }
  }
}

} // end namespace algorithm

} // end namespace sdsl
