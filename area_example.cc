#include "include/timer.h"
#include "include/maxtree_trie.h"
#include "tests/random_elements.h"
#include <cassert>
#include <iomanip>

// for up to 4.2 gigapixel images, 32-bit indices can be used
using index_t = uint32_t;
// throughput decreases when using 64-bit indices
// using index_t = uint64_t;

// data type of pixel values
using value_t = float;

int main()
{
  constexpr size_t width = 3333U;
  constexpr size_t height = 5555U;
  constexpr size_t depth = 1U;
  constexpr size_t n = width * height * depth;

  value_t *values = new value_t[n];
  index_t *parents = new index_t[n];
  index_t *areas = new index_t[n];

  std::default_random_engine generator;  
  mxt::random_elements(values, n, generator);

  // ensure that the node with index 0 is the root
  values[0] = std::numeric_limits<value_t>::min();

  // initial number of pixels in each set
  for (size_t i = 0; i < n; ++i)
  {
    areas[i] = 1U;
  }

  // when merging two pixel sets
  auto merge = [=](index_t to, index_t from) ALWAYS_INLINE
  {
    areas[to] += areas[from];
  };

  std::cout <<
    "Pixel count: " <<
    std::setprecision(3) <<
    (double)n <<
    '\n';

  {
    mxt::Timer t;
    mxt::maxtree_trie(values, parents, merge, width, height);

    // other examples

    // 3-D and 6-neighbor connectivity if depth > 1
    // mxt::maxtree_trie(values, parents, merge, width, height, depth);

    // 2-D and 8-neighbor connectivity
    // mxt::maxtree_trie_2d_8n(values, parents, merge, width, height);

    std::cout << n / 1e6 / t.stop() << " megapixel/s\n";
  }  

  assert(areas[0] == n);

  delete[] areas;
  delete[] parents;
  delete[] values;
}
