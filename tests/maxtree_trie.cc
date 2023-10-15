#include "../include/timer.h"
#include "../include/maxtree_trie.h"
#include "../include/radix_sort_seq.h"
#include "check_equiv.h"
#include "random_elements.h"
#include "maxtree_union_find.h"
#include <cassert>

using index_t = uint32_t;
using value_t = uint32_t;

std::default_random_engine generator;

void check_2d_4n()
{
  constexpr index_t width = 1024U;
  constexpr index_t height = 1024U;

  constexpr size_t n = width * height;

  index_t *parents = new index_t[n];
  index_t *parents2 = new index_t[n];
  value_t *values = new value_t[n];

  auto merge = [](index_t to, index_t from) ALWAYS_INLINE {};

  mxt::random_elements(values, n, generator);

  mxt::maxtree_trie(values, parents, merge, width, height, 1U);
  mxt::maxtree_union_find(values, parents2, width, height, 1U, 4U);

  mxt::check_equiv(parents, n, parents2, values);

  delete[] values;
  delete[] parents2;
  delete[] parents;
}

void check_2d_8n()
{
  constexpr index_t width = 1024U;
  constexpr index_t height = 1024U;

  constexpr size_t n = width * height;

  index_t *parents = new index_t[n];
  index_t *parents2 = new index_t[n];
  value_t *values = new value_t[n];

  auto merge = [](index_t to, index_t from) ALWAYS_INLINE {};

  mxt::random_elements(values, n, generator);
  mxt::maxtree_trie(values, parents, merge, width, height, 1U, 8U);
  mxt::maxtree_union_find(values, parents2, width, height, 1U, 8U);

  mxt::check_equiv(parents, n, parents2, values);

  delete[] values;
  delete[] parents2;
  delete[] parents;
}

void check_3d_6n()
{
  constexpr index_t width = 100U;
  constexpr index_t height = 100U;
  constexpr index_t depth = 100U;

  constexpr size_t n = width * height * depth;

  index_t *parents = new index_t[n];
  index_t *parents2 = new index_t[n];
  value_t *values = new value_t[n];

  auto merge = [](index_t to, index_t from) ALWAYS_INLINE {};

  mxt::random_elements(values, n, generator);
  mxt::maxtree_trie(values, parents, merge, width, height, depth);
  mxt::maxtree_union_find(values, parents2, width, height, depth, 6U);

  mxt::check_equiv(parents, n, parents2, values);

  delete[] values;
  delete[] parents2;
  delete[] parents;
}



int main()
{
  std::cout << "checking 2-D 4-neighbors\n\n";
  check_2d_4n();
  std::cout << "\nchecking 2-D 8-neighbors\n\n";
  check_2d_8n();
  std::cout << "\nchecking 3-D 6-neighbors\n\n";
  check_3d_6n();
}
