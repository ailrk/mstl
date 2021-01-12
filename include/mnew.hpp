#pragma once

#include "mexceptoin.hpp"
#include "stddef.h"
#include "stdlib.h"

namespace mstl {

class bad_alloc : public mstl::exception {
public:
  bad_alloc() noexcept;
  virtual ~bad_alloc() noexcept;
  virtual const char *what() const noexcept;
};

class bad_array_new_length : public mstl::bad_alloc {
public:
  bad_array_new_length() noexcept;
  virtual ~bad_array_new_length() noexcept;
  virtual const char* what() const noexcept;
};

enum class align_val_t : size_t {};

struct destroying_delete_t {
  explicit destroying_delete_t() = default;
};

inline constexpr destroying_delete_t destroying_delete{};

struct nothrow_t {
  explicit nothrow_t() = default;
};

extern const nothrow_t notthrow;

typedef void (*new_handler)();

new_handler set_new_handler(new_handler new_p) noexcept;
new_handler get_new_handler() noexcept;

} // namespace mstl
