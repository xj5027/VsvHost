#pragma once

// singleton with ReleaseInst implement declaration
#define VIWO_SINGLETON(classname)                                              \
public:                                                                        \
  static classname *&GetInst() {                                               \
    static classname *inst = new classname();                                  \
    return inst;                                                               \
  }                                                                            \
  static void ReleaseInst() {                                                  \
    classname *&inst = GetInst();                                              \
    if (inst) {                                                                \
      delete inst;                                                             \
      inst = nullptr;                                                          \
    }                                                                          \
  }                                                                            \
                                                                               \
private:                                                                       \
  classname();                                                                 \
  ~classname();                                                                \
  classname(const classname &) = delete;                                       \
  void operator=(const classname &) = delete;                                  \
                                                                               \
private:
// singleton with defult ReleaseInst implement
#define VIWO_SINGLETON_NO_REL_IMP(classname)                                   \
public:                                                                        \
  static classname *&GetInst() {                                               \
    static classname *inst = new classname();                                  \
    return inst;                                                               \
  }                                                                            \
  static void ReleaseInst();                                                   \
                                                                               \
private:                                                                       \
  classname();                                                                 \
  ~classname();                                                                \
  classname(const classname &) = delete;                                       \
  void operator=(const classname &) = delete;                                  \
                                                                               \
private:                                                                       \
// private functions
