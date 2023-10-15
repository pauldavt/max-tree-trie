#pragma once

#include <cstdlib>
#include <cstdint>
#include "common.h"
#include "bits.h"

NAMESPACE_MXT

template <typename Index>
class TrieQueue
{
public:
  using index_t = Index;

  TrieQueue(index_t max_rank, bool clear_data = true);
  ~TrieQueue();

  void init(index_t max_rank, bool clear_data);
  void clear();
  void insert(index_t rank);
  void remove();

  constexpr index_t top() const
  {
    return trie_max_;
  }

  constexpr bool empty() const
  {
    return levels_[root_level_][0U] == 0;
  }

  index_t data_size(index_t max_rank);

private:
  static constexpr unsigned max_levels =
    (sizeof(index_t) * CHAR_BIT + bits_per_word_log2 - 1U) / bits_per_word_log2;

  constexpr index_t num_words(index_t value)
  {
    return (value >> bits_per_word_log2) + index_t(1);
  }

  size_t* levels_[max_levels];
  size_t* data_;
  index_t trie_max_;
  index_t data_sz_;
  uint_fast8_t root_level_;
};

template <typename index_t>
TrieQueue<index_t>::TrieQueue(index_t max_rank, bool clear_data)
{
  data_sz_ = data_size(max_rank);
  data_ = new size_t[data_sz_];

  init(max_rank, clear_data);
}

template <typename index_t>
void TrieQueue<index_t>::init(index_t max_rank, bool clear_data)
{
  if (clear_data)
  {
    clear();
  }

  size_t* data_ptr = data_;
  uint_fast8_t level = 0U;
  while (max_rank > bits_word_mask)
  {
    levels_[level++] = data_ptr;
    data_ptr += num_words(max_rank);
    max_rank >>= bits_per_word_log2;
  }

  root_level_ = level;
  levels_[level] = data_ptr;
}

template <typename index_t>
TrieQueue<index_t>::~TrieQueue()
{
  delete[] data_;
}

template <typename index_t>
void TrieQueue<index_t>::clear()
{
  trie_max_ = 0U;
  std::fill(data_, data_ + data_sz_, 0U);
}


template <typename index_t>
index_t TrieQueue<index_t>::data_size(index_t max_rank)
{
  index_t sum = 0;

  while (max_rank)
  {
    sum += num_words(max_rank);
    max_rank >>= bits_per_word_log2;
  }

  return sum;
}

template <typename index_t>
ALWAYS_INLINE_F void TrieQueue<index_t>::insert(index_t rank)
{
  if (rank > trie_max_)
  {
    trie_max_ = rank;
  }

  bool do_continue = true;
  for (uint_fast8_t level = 0; level <= root_level_ && do_continue; ++level)
  {
    uint_fast8_t bit_nr = rank & bits_word_mask;
    rank >>= bits_per_word_log2;
    do_continue = levels_[level][rank] == 0;
    set_bit(levels_[level][rank], bit_nr);
  }
}

template <typename index_t>
ALWAYS_INLINE_F void TrieQueue<index_t>::remove()
{
  bool do_continue = true;
  uint_fast8_t level;
  for (level = 0; level <= root_level_ && do_continue; ++level)
  {
    uint_fast8_t bit_nr = trie_max_ & bits_word_mask;
    trie_max_ >>= bits_per_word_log2;
    clear_bit(levels_[level][trie_max_], bit_nr);
    do_continue = levels_[level][trie_max_] == 0;
  }

  if (empty()) return;

  for (; level--;)
  {
    trie_max_ = mxt::log2(levels_[level][trie_max_]) +
      (trie_max_ << bits_per_word_log2);
  }
}

NAMESPACE_MXT_END
