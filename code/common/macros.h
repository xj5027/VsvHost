#pragma once

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)                                                         \
  if (p) {                                                                     \
    delete p;                                                                  \
    p = nullptr;                                                               \
  }
#endif // !SAFE_DELETE

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)                                                   \
  if (p) {                                                                     \
    delete[] p;                                                                \
    p = nullptr;                                                               \
  }
#endif // !SAFE_DELETE_ARRAY

#define DISALLOW_COPY(TypeName)                                                \
  TypeName(const TypeName &) = delete;                                         \
  TypeName &operator=(const TypeName &) = delete;
