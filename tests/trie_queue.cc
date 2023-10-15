#include "../include/trie_queue.h"
#include "../include/timer.h"
#include <iostream>
#include <cassert>

const size_t N = 1025 * 1027;
using Value = uint32_t;
using Index = uint32_t;

int main()
{
  Value* pixels = new Value[N];

  for (Index i = 0; i != N; ++i)
  {
    pixels[i] = i;
  }

  double avg = 0;
  for (size_t i = 0; i != 50; ++i)
  {
    std::random_shuffle(pixels, pixels + N);

    {
      mxt::Timer t;

      mxt::TrieQueue<Index> queue(N - 1);

      for (Index i = 0; i != N; ++i)
      {
        queue.insert(pixels[i]);
      }
        
      for (Index i = N; i--;)
      {
        Index val = queue.top();
        queue.remove();
        
        assert(val == i);    
      }

      avg += (t.stop() - avg) / (i + 1.0);

      assert(queue.empty());
    }
  }

  printf("avg time = %f\n", avg);
  
  std::cout << "Success.\n";
  
  delete[] pixels;  
}