#pragma once

#include "common.h"
#include "bit_array.h"
#include "sort_item.h"
#include "radix_sort_seq.h"
#include "trie_queue.h"
#include <cassert>

NAMESPACE_MXT

template <typename index_t, typename functor_t>
class MaxtreeTrie;

template<typename value_t, typename index_t, typename functor_t>
void maxtree_trie(
  value_t *values,
  index_t *parents,
  functor_t const &merge,
  size_t width,
  size_t height,
  size_t depth = 1U,
  size_t n_neighbors = 0)
{
  using visited_t = BitArray;
  using queue_t = TrieQueue<index_t>;
  using uvalue_t = decltype(unsigned_conversion(value_t(0)));
  using sort_pair_t = SortPair<uvalue_t, index_t>;

  index_t n = width * height * depth;
  assert(n != 0);
  
  if (n_neighbors == 0)
  {
    n_neighbors = depth > 1U ? 6U : 4U;
  }

  index_t n_digits = radix_sort_n_digits<uvalue_t>();

  // sort pairs are used to reduce the number of random accesses in radix sort
  // when optimizing for memory usage, only node indices are better
  sort_pair_t *aux1 = nullptr;
  sort_pair_t *aux2 = nullptr;

  if (n_digits > 1)
  {
    aux1 = new sort_pair_t[n];

    if (n_digits > 2)
    {
      aux2 = new sort_pair_t[n];
    }
  }

  auto const &f_initial_item = [=](index_t i) ALWAYS_INL_L(sort_pair_t)
  {
    return {unsigned_conversion(values[i]), i};
  };

  auto const &f_out = [=](index_t &out, sort_pair_t const &item) ALWAYS_INLINE
  {
    out = item.data();
  };

  index_t *rank_to_index = new index_t[n];

  radix_sort_seq(rank_to_index, n, aux1, aux2, f_initial_item, f_out);  

  delete[] aux2;
  delete[] aux1;

  index_t *index_to_rank = parents;

  for (size_t rank = 0; rank < n; ++rank)
  {
    index_to_rank[rank_to_index[rank]] = rank;
  }

  visited_t visited(n);
  queue_t queue(n - 1U, false);

  MaxtreeTrie<index_t, functor_t> mt(
    width,
    height,
    depth,
    n_neighbors,
    parents,
    rank_to_index,
    index_to_rank,
    &visited,
    &queue,
    merge);

  delete[] rank_to_index;
}

template<typename value_t, typename index_t, typename functor_t>
void maxtree_trie_2d_8n(
  value_t *values,
  index_t *parents,
  functor_t const &merge,
  size_t width,
  size_t height)
{
  maxtree_trie(values, parents, merge, width, height, 1U, 8U);
}

template <typename index_t, typename functor_t>
class MaxtreeTrie
{
public:
  using visited_t = BitArray;
  using queue_t = TrieQueue<index_t>;

  MaxtreeTrie(
    index_t width,
    index_t height,
    index_t depth,
    index_t n_neighbors,
    index_t* parents,
    index_t* rank_to_index,
    index_t* index_to_rank,
    visited_t* visited,
    queue_t* queue,
    functor_t const &merge);

private:
  
  bool check_neighbor_2d(
    index_t &current,
    index_t &current_rank,
    index_t neighbor,
    index_t &x,
    index_t &y);
  bool check_neighbor_3d(
    index_t &current,
    index_t &current_rank,
    index_t neighbor,
    index_t &x,
    index_t &y,
    index_t &z);

  bool to_remaining_maximum_2d_4n(
    index_t &current,
    index_t &current_rank,
    index_t &x,
    index_t &y);
  bool to_remaining_maximum_2d_8n(
    index_t &current,
    index_t &current_rank,
    index_t &x,
    index_t &y);
  bool to_remaining_maximum_3d_6n(
    index_t &current,
    index_t &current_rank,
    index_t &x,
    index_t &y,
    index_t &z);

  void main_loop_2d_4n();
  void main_loop_2d_8n();
  void main_loop_3d_6n();

  index_t width_;
  index_t height_;
  index_t depth_;
  index_t* parents_;
  index_t* RESTRICT rank_to_index_;
  index_t* index_to_rank_;
  visited_t& visited_;
  queue_t& queue_;
  functor_t const &merge_;
};

template <typename index_t, typename functor_t>
MaxtreeTrie<index_t, functor_t>::MaxtreeTrie(
  index_t width,
  index_t height,
  index_t depth,
  index_t n_neighbors,
  index_t* parents,
  index_t* rank_to_index,
  index_t* index_to_rank,
  visited_t* visited,
  queue_t* queue,
  functor_t const &merge) : 
  width_(width),
  height_(height),
  depth_(depth),
  parents_(parents),
  rank_to_index_(rank_to_index),
  index_to_rank_(index_to_rank),
  visited_(*visited),
  queue_(*queue),
  merge_(merge)
{
  assert(width > 0 && height > 0 && depth > 0);

  visited_.clear();
  queue_.clear();

  if (depth == 1)
  {
    if (n_neighbors == 4U)
    {
      main_loop_2d_4n();
      return;
    }

    assert (n_neighbors == 8U);
    main_loop_2d_8n();
    return;
  }

  assert (n_neighbors == 6U);
  main_loop_3d_6n();
}

template <typename index_t, typename functor_t>
ALWAYS_INLINE_F
bool MaxtreeTrie<index_t, functor_t>::check_neighbor_2d(
  index_t &current,
  index_t &current_rank,
  index_t next,
  index_t &x,
  index_t &y)
{
  visited_.set(next);
  index_t next_rank = index_to_rank_[next];

  if (next_rank > current_rank)
  {
    queue_.insert(current_rank);
    current = next;
    current_rank = next_rank;
    x = current % width_;
    y = current / width_;
    return true;
  }

  queue_.insert(next_rank);  
  return false;
}

#define CHECK_NEIGHBOR_2D(cond, offset) \
{ \
  index_t _neighbor = current + (offset); \
  \
  if ((cond) && !visited_.is_set(_neighbor) && \
    check_neighbor_2d(current, current_rank, _neighbor, x, y)) \
  { \
    return true; \
  } \
}

template <typename index_t, typename functor_t>
ALWAYS_INLINE_F
bool MaxtreeTrie<index_t, functor_t>::to_remaining_maximum_2d_4n(
  index_t &current,
  index_t &current_rank,
  index_t &x,
  index_t &y)
{
  CHECK_NEIGHBOR_2D(x > 0, -1U)
  CHECK_NEIGHBOR_2D(x < width_ - 1U, 1U)
  CHECK_NEIGHBOR_2D(y > 0, -width_)
  CHECK_NEIGHBOR_2D(y < height_ - 1U, width_)

  return false;
}

template <typename index_t, typename functor_t>
ALWAYS_INLINE_F
bool MaxtreeTrie<index_t, functor_t>::to_remaining_maximum_2d_8n(
  index_t &current,
  index_t &current_rank,
  index_t &x,
  index_t &y)
{
  CHECK_NEIGHBOR_2D(x > 0, -1U)
  CHECK_NEIGHBOR_2D(x < width_ - 1U, 1U)

  CHECK_NEIGHBOR_2D(x > 0 && y > 0, -width_ - 1U)
  CHECK_NEIGHBOR_2D(y > 0, -width_)
  CHECK_NEIGHBOR_2D(x < width_ - 1U && y > 0, -width_ + 1U)

  CHECK_NEIGHBOR_2D(x > 0 && y < height_ - 1U, width_ - 1U)
  CHECK_NEIGHBOR_2D(y < height_ - 1U, width_)
  CHECK_NEIGHBOR_2D(x < width_ - 1U && y < height_ - 1U, width_ + 1U)

  return false;
}

template <typename index_t, typename functor_t>
void MaxtreeTrie<index_t, functor_t>::main_loop_2d_4n()
{
  index_t current_rank = 0;
  index_t current = rank_to_index_[current_rank];
  visited_.set(current);
  index_t x = current % width_;
  index_t y = current / width_;

  while (true)
  {
    while (to_remaining_maximum_2d_4n(current, current_rank, x, y))
      ;

    if (queue_.empty()) break;

    index_t parent_rank = queue_.top();
    queue_.remove();
    index_t parent = rank_to_index_[parent_rank];
    merge_(parent, current);
    parents_[current] = parent;      
    current = parent;
    current_rank = parent_rank;
    x = current % width_;
    y = current / width_;
  }

  parents_[current] = current;
}

template <typename index_t, typename functor_t>
void MaxtreeTrie<index_t, functor_t>::main_loop_2d_8n()
{
  index_t current_rank = 0;
  index_t current = rank_to_index_[current_rank];
  visited_.set(current);
  index_t x = current % width_;
  index_t y = current / width_;

  while (true)
  {
    while (to_remaining_maximum_2d_8n(current, current_rank, x, y))
      ;

    if (queue_.empty()) break;

    index_t parent_rank = queue_.top();
    queue_.remove();
    index_t parent = rank_to_index_[parent_rank];
    merge_(parent, current);
    parents_[current] = parent;      
    current = parent;
    current_rank = parent_rank;
    x = current % width_;
    y = current / width_;
  }

  parents_[current] = current;
}


template <typename index_t, typename functor_t>
ALWAYS_INLINE_F
bool MaxtreeTrie<index_t, functor_t>::check_neighbor_3d(
  index_t &current,
  index_t &current_rank,
  index_t next,
  index_t &x,
  index_t &y,
  index_t &z)
{
  visited_.set(next);
  index_t next_rank = index_to_rank_[next];

  if (next_rank > current_rank)
  {
    queue_.insert(current_rank);
    current = next;
    current_rank = next_rank;
    x = current % width_;
    y = (current % (width_ * height_)) / width_;
    z = current / (width_ * height_);
    return true;
  }

  queue_.insert(next_rank);  
  return false;
} 

#define CHECK_NEIGHBOR_3D(cond, offset) \
{ \
  index_t _neighbor = current + (offset); \
  \
  if ((cond) && !visited_.is_set(_neighbor) && \
    check_neighbor_3d(current, current_rank, _neighbor, x, y, z)) \
  { \
    return true; \
  } \
}

template <typename index_t, typename functor_t>
ALWAYS_INLINE_F
bool MaxtreeTrie<index_t, functor_t>::to_remaining_maximum_3d_6n(
  index_t &current,
  index_t &current_rank,
  index_t &x,
  index_t &y,
  index_t &z)
{
  CHECK_NEIGHBOR_3D(x > 0, -1U)
  CHECK_NEIGHBOR_3D(x < width_ - 1U, 1U)
  CHECK_NEIGHBOR_3D(y > 0, -width_)
  CHECK_NEIGHBOR_3D(y < height_ - 1U, width_)
  CHECK_NEIGHBOR_3D(z > 0, -(width_ * height_))
  CHECK_NEIGHBOR_3D(z < depth_ - 1U, width_ * height_)

  return false;
}

template <typename index_t, typename functor_t>
void MaxtreeTrie<index_t, functor_t>::main_loop_3d_6n()
{
  index_t current_rank = 0;
  index_t current = rank_to_index_[current_rank];
  visited_.set(current);
  index_t x = current % width_;
  index_t y = (current % (width_ * height_)) / width_;
  index_t z = current / (width_ * height_);

  while (true)
  {
    while (to_remaining_maximum_3d_6n(current, current_rank, x, y, z))
      ;

    if (queue_.empty()) break;

    index_t parent_rank = queue_.top();
    queue_.remove();
    index_t parent = rank_to_index_[parent_rank];
    merge_(parent, current);
    parents_[current] = parent;      
    current = parent;
    current_rank = parent_rank;
    x = current % width_;
    y = (current % (width_ * height_)) / width_;
    z = current / (width_ * height_);
  }

  parents_[current] = current;
}

NAMESPACE_MXT_END