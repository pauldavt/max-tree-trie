#include "../include/radix_sort_seq.h"
#include "../include/sort_item.h"
#include "../include/timer.h"
#include "random_elements.h"
#include <cassert>

using index_t = uint32_t;

constexpr index_t N = 1024U * 1024U;

template <typename Value, typename Index>
void validate_sort(Value* values, Index* rank_to_index, size_t n)
{
  Index* counts = new Index[n];
  std::fill(counts, counts + n, Index(0));

  // Check if rank_to_index is a permutation
  for (size_t i = 0; i < n; ++i)
  {
    ++counts[rank_to_index[i]];
  }

  for (size_t i = 0; i < n; ++i)
  {
    assert(counts[i] == 1U);
  }

  delete[] counts;

  // Check order
  for (size_t i = 1; i < n; ++i)
  {
    assert(values[rank_to_index[i]] > values[rank_to_index[i - 1]] || 
      (values[rank_to_index[i]] == values[rank_to_index[i - 1]] &&
      rank_to_index[i] > rank_to_index[i - 1]));
  }
}


template <typename value_t>
void check_value_t(char const* s)
{  
  using uvalue_t = decltype(mxt::unsigned_conversion(value_t(0)));

  using sort_pair_t = mxt::SortPair<uvalue_t, index_t>;

  std::cout << "sizeof(sort_pair_t) = " << sizeof(sort_pair_t) << '\n';
  
  value_t* values = new value_t[N];
  index_t* sorted = new index_t[N];
  sort_pair_t* aux_1 = new sort_pair_t[N];
  sort_pair_t* aux_2 = new sort_pair_t[N];

  std::default_random_engine generator;

  mxt::random_elements(values, N, generator);

  auto const& f_item =
    [=](index_t i) ALWAYS_INL_L(sort_pair_t)
    {
      return {mxt::unsigned_conversion(values[i]), i};
    };  

  auto const& f_out =
    [=](index_t& out, sort_pair_t const& item) ALWAYS_INLINE
    {
      out = item.data();
    };

  for (size_t i = 0; i < N; ++i)
  {
    sorted[i] = 0;
  }

  {
    mxt::Timer t;
    mxt::radix_sort_seq(sorted, N, aux_1, aux_2, f_item, f_out);

    printf("Sorted %.2e random %s values in %f seconds (seq)\n", (double)N, s, t.stop());
  }  

  validate_sort(values, sorted, N);

  delete[] sorted;
  delete[] aux_2;
  delete[] aux_1;
  delete[] values;
}

int main()
{
  check_value_t<int8_t>("int8_t");
  check_value_t<uint8_t>("uint8_t");
  check_value_t<int16_t>("int16_t");
  check_value_t<uint16_t>("uint16_t");
  check_value_t<int32_t>("int32_t");
  check_value_t<uint32_t>("uint32_t");
  check_value_t<int64_t>("int64_t");
  check_value_t<uint64_t>("uint64_t");
  check_value_t<float>("float"); 
  check_value_t<double>("double");
}
