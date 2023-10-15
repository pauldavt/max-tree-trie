#pragma once

#include "common.h"

NAMESPACE_MXT

#pragma pack(push, 1)
template <typename UValue, typename Data>
struct SortPair
{
  using uvalue_t = UValue;

  ALWAYS_INLINE_F SortPair() {}  
  ALWAYS_INLINE_F SortPair(UValue val, Data data) : val_(val), data_(data) {}

  ALWAYS_INLINE_F UValue unsigned_value() const
  {
    return val_;
  }  

  ALWAYS_INLINE_F Data data() const
  {
    return data_;
  }

  ALWAYS_INLINE_F void set_value(UValue val)
  {
    val_ = val;
  }

  ALWAYS_INLINE_F void set_data(Data data)
  {
    data_ = data;
  }

private:
  UValue val_;
  Data data_;
};
#pragma pack(pop)

template <typename UValue>
struct SortValue
{
  using uvalue_t = UValue;

  ALWAYS_INLINE_F SortValue() {}
  ALWAYS_INLINE_F SortValue(UValue val) : val_(val) {}

  ALWAYS_INLINE_F UValue unsigned_value() const
  {
    return val_;
  }  

  ALWAYS_INLINE_F UValue data() const
  {
    return data_;
  }

  ALWAYS_INLINE_F void set_value(UValue val)
  {
    val_ = val;
  }

  ALWAYS_INLINE_F void set_data(UValue data)
  {
    data_ = data;
  }  
  
private:
  union
  {
    UValue val_;
    UValue data_;
  };
};

NAMESPACE_MXT_END