// Explicitly Omit GNU copyright

#include <cstddef>
#include <cxxabi.h>
#include <new>

namespace __cxxabiv1 {

extern "C" void __cxa_vec_ctor(
    void* array_address, std::size_t element_count,
    std::size_t element_size,
    __cxa_cdtor_type constructor,
    __cxa_cdtor_type destructor)
{
  if (!constructor) return;
  char* ptr = static_cast<char*>(array_address);
  for (std::size_t i = 0; i < element_count; ++i, ptr += element_size)
    constructor(ptr);
}

extern "C" void __cxa_vec_dtor(
    void* array_address, std::size_t element_count,
    std::size_t element_size,
    __cxa_cdtor_type destructor)
{
  if (!destructor) return;
  char* ptr = static_cast<char*>(array_address)
            + element_count * element_size;
  for (std::size_t i = element_count; i > 0; --i)
  {
    ptr -= element_size;
    destructor(ptr);
  }
}

extern "C" void __cxa_vec_cleanup(
    void* array_address, std::size_t element_count,
    std::size_t element_size,
    __cxa_cdtor_type destructor)
{
  __cxa_vec_dtor(array_address, element_count, element_size, destructor);
}

extern "C" void __cxa_vec_cctor(
    void* dest_array, void* src_array,
    std::size_t element_count, std::size_t element_size,
    __cxa_cdtor_return_type (*constructor)(void*, void*),
    __cxa_cdtor_type destructor)
{
  if (!constructor) return;
  char* dest = static_cast<char*>(dest_array);
  char* src = static_cast<char*>(src_array);
  for (std::size_t i = 0; i < element_count; ++i, dest += element_size, src += element_size)
    constructor(dest, src);
}

extern "C" void* __cxa_vec_new(
    std::size_t element_count, std::size_t element_size,
    std::size_t padding_size,
    __cxa_cdtor_type constructor,
    __cxa_cdtor_type destructor)
{
  std::size_t total = element_count * element_size + padding_size;
  void* mem = ::operator new[](total);
  if (!mem) return nullptr;

  char* array = static_cast<char*>(mem);
  if (padding_size)
  {
    *(std::size_t*)mem = element_count;
    array = static_cast<char*>(mem) + padding_size;
  }

  if (constructor)
  {
    char* ptr = array;
    for (std::size_t i = 0; i < element_count; ++i, ptr += element_size)
      constructor(ptr);
  }

  return array;
}

extern "C" void* __cxa_vec_new2(
    std::size_t element_count, std::size_t element_size,
    std::size_t padding_size,
    __cxa_cdtor_type constructor,
    __cxa_cdtor_type destructor,
    void* (*alloc)(std::size_t),
    void (*dealloc)(void*))
{
  std::size_t total = element_count * element_size + padding_size;
  void* mem = alloc(total);
  if (!mem) return nullptr;

  char* array = static_cast<char*>(mem);
  if (padding_size)
  {
    *(std::size_t*)mem = element_count;
    array = static_cast<char*>(mem) + padding_size;
  }

  if (constructor)
  {
    char* ptr = array;
    for (std::size_t i = 0; i < element_count; ++i, ptr += element_size)
      constructor(ptr);
  }

  return array;
}

extern "C" void* __cxa_vec_new3(
    std::size_t element_count, std::size_t element_size,
    std::size_t padding_size,
    __cxa_cdtor_type constructor,
    __cxa_cdtor_type destructor,
    void* (*alloc)(std::size_t),
    void (*dealloc)(void*, std::size_t))
{
  std::size_t total = element_count * element_size + padding_size;
  void* mem = alloc(total);
  if (!mem) return nullptr;

  char* array = static_cast<char*>(mem);
  if (padding_size)
  {
    *(std::size_t*)mem = element_count;
    array = static_cast<char*>(mem) + padding_size;
  }

  if (constructor)
  {
    char* ptr = array;
    for (std::size_t i = 0; i < element_count; ++i, ptr += element_size)
      constructor(ptr);
  }

  return array;
}

extern "C" void __cxa_vec_delete(
    void* array_address, std::size_t element_size,
    std::size_t padding_size,
    __cxa_cdtor_type destructor)
{
  if (!array_address) return;

  char* mem = static_cast<char*>(array_address);
  if (padding_size)
  {
    mem -= padding_size;
    std::size_t count = *(std::size_t*)mem;
    if (destructor)
    {
      char* ptr = static_cast<char*>(array_address)
                + count * element_size;
      for (std::size_t i = count; i > 0; --i)
      {
        ptr -= element_size;
        destructor(ptr);
      }
    }
    ::operator delete[](mem);
  }
  else
  {
    if (destructor)
      destructor(array_address);
    ::operator delete[](array_address);
  }
}

extern "C" void __cxa_vec_delete2(
    void* array_address, std::size_t element_size,
    std::size_t padding_size,
    __cxa_cdtor_type destructor,
    void (*dealloc)(void*))
{
  if (!array_address) return;

  char* mem = static_cast<char*>(array_address);
  if (padding_size)
  {
    mem -= padding_size;
    std::size_t count = *(std::size_t*)mem;
    if (destructor)
    {
      char* ptr = static_cast<char*>(array_address)
                + count * element_size;
      for (std::size_t i = count; i > 0; --i)
      {
        ptr -= element_size;
        destructor(ptr);
      }
    }
    dealloc(mem);
  }
  else
  {
    if (destructor)
      destructor(array_address);
    dealloc(array_address);
  }
}

extern "C" void __cxa_vec_delete3(
    void* array_address, std::size_t element_size,
    std::size_t padding_size,
    __cxa_cdtor_type destructor,
    void (*dealloc)(void*, std::size_t))
{
  if (!array_address) return;

  char* mem = static_cast<char*>(array_address);
  std::size_t total = element_size;
  if (padding_size)
  {
    mem -= padding_size;
    std::size_t count = *(std::size_t*)mem;
    total = count * element_size + padding_size;
    if (destructor)
    {
      char* ptr = static_cast<char*>(array_address)
                + count * element_size;
      for (std::size_t i = count; i > 0; --i)
      {
        ptr -= element_size;
        destructor(ptr);
      }
    }
    dealloc(mem, total);
  }
  else
  {
    if (destructor)
      destructor(array_address);
    dealloc(array_address, element_size);
  }
}

} // namespace __cxxabiv1
