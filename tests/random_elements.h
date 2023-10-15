#pragma once

#include "../include/common.h"
#include <random>

NAMESPACE_MXT

template <typename float_t, typename functor_t>
void random_real_elements(float_t *array, size_t n, functor_t& generator)
{
  std::uniform_real_distribution<float_t> dist;

  for (size_t i = 0; i < n; ++i)
  {
    array[i] = dist(generator);
  } 
}

template <typename int_t, typename functor_t>
void random_int_elements(int_t *array, size_t n, functor_t& generator)
{
  std::uniform_int_distribution<int_t> dist;

  for (size_t i = 0; i < n; ++i)
  {
    array[i] = dist(generator);
  } 
}

template <typename functor_t>
void random_elements(uint8_t *array, size_t n, functor_t& generator)
{
  random_int_elements(array, n, generator);
}

template <typename functor_t>
void random_elements(uint16_t *array, size_t n, functor_t& generator)
{
  random_int_elements(array, n, generator);
}

template <typename functor_t>
void random_elements(uint32_t *array, size_t n, functor_t& generator)
{
  random_int_elements(array, n, generator);
}

template <typename functor_t>
void random_elements(uint64_t *array, size_t n, functor_t& generator)
{
  random_int_elements(array, n, generator);
}

template <typename functor_t>
void random_elements(int8_t *array, size_t n, functor_t& generator)
{
  random_int_elements(array, n, generator);
}

template <typename functor_t>
void random_elements(int16_t *array, size_t n, functor_t& generator)
{
  random_int_elements(array, n, generator);
}

template <typename functor_t>
void random_elements(int32_t *array, size_t n, functor_t& generator)
{
  random_int_elements(array, n, generator);
}

template <typename functor_t>
void random_elements(int64_t *array, size_t n, functor_t& generator)
{
  random_int_elements(array, n, generator);
}

template <typename functor_t>
void random_elements(float *array, size_t n, functor_t& generator)
{
  random_real_elements(array, n, generator);
}

template <typename functor_t>
void random_elements(double *array, size_t n, functor_t& generator)
{
  random_real_elements(array, n, generator);
}



NAMESPACE_MXT_END