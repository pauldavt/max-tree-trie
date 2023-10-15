#pragma once

#include "common.h"
#include "bits.h"

NAMESPACE_MXT

class BitArray
{
public:
  BitArray(size_t length);
  ~BitArray();
  BitArray(BitArray const&) = delete;
  BitArray& operator=(BitArray const&) = delete;

  size_t length() const;
  void set(size_t i);
  void clear(size_t i);
  bool is_set(size_t i) const;
  void clear();
  void set_range(size_t x_begin, size_t x_end_inclusive);

private:
  constexpr size_t n_words() const
  {
    return div_roundup(length_, bits_per_word);
  }

  size_t* data_;
  size_t length_;
};

ALWAYS_INLINE_F size_t BitArray::length() const
{
  return length_;
}

ALWAYS_INLINE_F void BitArray::set(size_t i)
{
  size_t word_nr = i / bits_per_word;
  size_t bit_nr = i % bits_per_word;

  mxt::set_bit(data_[word_nr], bit_nr);
}

ALWAYS_INLINE_F void BitArray::clear(size_t i)
{
  size_t word_nr = i / bits_per_word;
  size_t bit_nr = i % bits_per_word;

  mxt::clear_bit(data_[word_nr], bit_nr);
}

ALWAYS_INLINE_F bool BitArray::is_set(size_t i) const
{    
  size_t word_nr = i / bits_per_word;
  size_t bit_nr = i % bits_per_word;

  return mxt::is_bit_set(data_[word_nr], bit_nr);
}

NAMESPACE_MXT_END