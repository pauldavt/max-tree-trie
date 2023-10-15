#pragma once

#include "../include/common.h"

NAMESPACE_MXT

template <typename index_t, typename value_t>
index_t level_root(index_t* parents, index_t i, value_t const* values)
{
  index_t root = i;
  
  while (parents[root] != root && values[i] == values[parents[root]])
  {
    root = parents[root];
  }

  // Compress path.
  while (root != i)
  {
    index_t tmp = parents[i];
    parents[i] = root;
    i = tmp;
  }

  return root;  
}

template <typename index_t, typename value_t>
void check_equiv(index_t* parents1, size_t n, index_t* parents2, value_t const* values)
{
  // Compress paths.
  std::cout << "checking if the max-trees are equivalent...\n";

  std::cout << "compressing paths in the first parents array...\n";
  for (index_t i = 0; i < n; ++i)
  {
    level_root(parents1, i, values);
  }

  std::cout << "compressing paths in the second parents array...\n";
  for (index_t i = 0; i < n; ++i)
  {
    level_root(parents2, i, values);
  }

  std::cout << "changing component roots to the node with minimum index...\n";
  // Set root at each level to the node with the minimal index.
  for (index_t i = 0; i < n; ++i)
  {
    if (values[parents1[i]] == values[i] && i < parents1[i])
    {
      index_t tmp = parents1[parents1[i]];

      if (tmp == parents1[i])
      {
        tmp = i;
      }

      parents1[parents1[i]] = i;
      parents1[i] = tmp;
    }

    if (values[parents2[i]] == values[i] && i < parents2[i])
    {
      index_t tmp = parents2[parents2[i]];

      if (tmp == parents2[i])
      {
        tmp = i;
      }

      parents2[parents2[i]] = i;
      parents2[i] = tmp;
    }
  }

  // Check for each node if the connected components match & the parents.
  for (index_t i = 0; i < n; ++i)
  {
    index_t level_root1 = level_root(parents1, i, values);
    index_t level_root2 = level_root(parents2, i, values);
    assert(level_root1 == level_root2);    

    index_t level_root_parent1 = level_root(parents1, parents1[level_root1], values);
    index_t level_root_parent2 = level_root(parents2, parents2[level_root2], values);

    assert(level_root_parent1 == level_root_parent2);
  }

  std::cout << "seems equivalent\n";
}

NAMESPACE_MXT_END