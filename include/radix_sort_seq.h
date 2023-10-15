#pragma once

#include "common.h"
#include "exclusive_sum.h"
#include "unsigned_conversion.h"
#include "sort_item.h"
#include <cstring>

NAMESPACE_MXT

constexpr size_t histo_sz_log2 = 8U;

template<typename uvalue_t_t>
constexpr size_t radix_sort_n_digits()
{
  return div_roundup(sizeof(uvalue_t_t) * CHAR_BIT, histo_sz_log2);
}

template<
  typename Sorted,
  typename Item,
  typename InitialItemF,
  typename LastItemF>
class RadixSortSeq
{
public:  
  static constexpr size_t histo_sz = 1U << histo_sz_log2;
  static constexpr size_t histo_mask = histo_sz - 1U;

  using index_t = size_t;
  using sorted_t = Sorted;
  using item_t = Item;
  using histogram_t = index_t[histo_sz];
  using uvalue_t = typename Item::uvalue_t;

  static constexpr size_t n_digits = radix_sort_n_digits<uvalue_t>();

  RadixSortSeq(
    sorted_t* sorted,
    item_t* sort_space_1,
    item_t* sort_space_2,
    size_t len,
    InitialItemF const& f_initial_item,
    LastItemF const& f_last_item);

private:
  template <typename out_t, typename item_f, typename out_f>
  void scatter_digit(
    out_t* RESTRICT output, 
    unsigned shift,
    index_t* const h,
    item_f const& f_item,
    out_f const& f_out);

  void scatter_digits();
  void add_value_to_histograms(uvalue_t u);
  void exclusive_sums();
  void make_offsets();

  sorted_t* RESTRICT sorted_;
  item_t* RESTRICT items1_;
  item_t* RESTRICT items2_;
  size_t const len_;
  InitialItemF const& f_initial_item_;
  LastItemF const& f_last_item_;
  histogram_t hs_[n_digits];
};

template<
  typename sorted_t,
  typename item_t,
  typename initial_item_f,
  typename last_item_f>
RadixSortSeq<sorted_t, item_t, initial_item_f, last_item_f>::
RadixSortSeq(
  sorted_t* sorted,
  item_t* sort_space_1,
  item_t* sort_space_2,
  size_t len,
  initial_item_f const& f_initial_item,
  last_item_f const& f_last_item) :
  sorted_(sorted),
  items1_(sort_space_1),
  items2_(sort_space_2),
  len_(len),
  f_initial_item_(f_initial_item),
  f_last_item_(f_last_item)
{
  make_offsets();
  scatter_digits();
}

template<
  typename sorted_t,
  typename item_t,
  typename initial_item_f,
  typename last_item_f>
template <typename out_t, typename item_f, typename out_f>
void RadixSortSeq<sorted_t, item_t, initial_item_f, last_item_f>::
scatter_digit(
  out_t* RESTRICT output, 
  unsigned shift,
  index_t* const h,
  item_f const& f_item,
  out_f const& f_out)
{
  for (size_t i = 0; i < len_; ++i)
  {
    item_t const& item = f_item(i);
    uvalue_t u = item.unsigned_value() >> shift;
    index_t out = h[u & histo_mask]++;            
    f_out(output[out], item);
  }
}

template<
  typename sorted_t,
  typename item_t,
  typename initial_item_f,
  typename last_item_f>
void RadixSortSeq<sorted_t, item_t, initial_item_f, last_item_f>::
scatter_digits()
{
  if (n_digits == 1)
  {
    scatter_digit(sorted_, 0U, hs_[0], f_initial_item_, f_last_item_);
    return;
  }

  auto const& f_item =
    [=](index_t i) ALWAYS_INL_L(item_t const&)
    {
      return items2_[i];
    }; 

  auto const& f_out =
    [=](item_t& out, item_t const& item) ALWAYS_INLINE
    {
      out = item;
    };  

    // first digit
  scatter_digit(items1_, 0U, hs_[0], f_initial_item_, f_out);
  std::swap(items1_, items2_);

  for (unsigned d = 1; d < n_digits - 1; ++d)
  {     
    histogram_t& h = hs_[d];
    scatter_digit(items1_, d * histo_sz_log2, h, f_item, f_out);
    std::swap(items1_, items2_);
  }

  unsigned const shift = histo_sz_log2 * (n_digits - 1);
  histogram_t& h = hs_[n_digits - 1];

    // last digit
  scatter_digit(sorted_, shift, h, f_item, f_last_item_);
}

template<
  typename sorted_t,
  typename item_t,
  typename initial_item_f,
  typename last_item_f>
ALWAYS_INLINE_F
void RadixSortSeq<sorted_t, item_t, initial_item_f, last_item_f>::
add_value_to_histograms(uvalue_t u)
{
  for (unsigned d = 0; d != n_digits; ++d)
  {
    histogram_t& h = hs_[d];
    ++h[u & histo_mask];
    u >>= histo_sz_log2;
  }  
}

template<
  typename sorted_t,
  typename item_t,
  typename initial_item_f,
  typename last_item_f>
void RadixSortSeq<sorted_t, item_t, initial_item_f, last_item_f>::
exclusive_sums()
{
  for (unsigned d = 0; d != n_digits; ++d)
  {    
    histogram_t& h = hs_[d];
    exclusive_sum(h, h + histo_sz);
  }    
}

template<
  typename sorted_t,
  typename item_t,
  typename initial_item_f,
  typename last_item_f>
void RadixSortSeq<sorted_t, item_t, initial_item_f, last_item_f>::
make_offsets()
{
  memset(&hs_[0][0], 0, n_digits * sizeof(histogram_t));

  for (size_t i = 0; i != len_; ++i)
  {
    add_value_to_histograms(f_initial_item_(i).unsigned_value());
  }

  exclusive_sums();
}

template<
  typename sorted_t,
  typename item_t,
  typename initial_item_f,
  typename last_item_f>
void radix_sort_seq(
  sorted_t* sorted,
  size_t len,
  item_t* sort_space_1,
  item_t* sort_space_2,
  initial_item_f const& f_initial_item,
  last_item_f const& f_last_item)
{
  if (len == 0) return;

  RadixSortSeq<sorted_t, item_t, initial_item_f, last_item_f> sorter(
      sorted, sort_space_1, sort_space_2, len, f_initial_item, f_last_item);    
}


NAMESPACE_MXT_END