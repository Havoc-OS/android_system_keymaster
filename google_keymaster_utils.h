/*
 * Copyright 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SYSTEM_KEYMASTER_GOOGLE_KEYMASTER_UTILS_H_
#define SYSTEM_KEYMASTER_GOOGLE_KEYMASTER_UTILS_H_

namespace keymaster {

/**
 * Convert the specified time value into "Java time", which is a signed 64-bit integer representing
 * elapsed milliseconds since Jan 1, 1970.
 */
inline int64_t java_time(time_t time) {
    // The exact meaning of a time_t value is implementation-dependent.  If this code is ported to a
    // platform that doesn't define it as "seconds since Jan 1, 1970 UTC", this function will have
    // to be revised.
    return time * 1000;
}

/*
 * Array Manipulation functions.  This set of templated inline functions provides some nice tools
 * for operating on c-style arrays.  C-style arrays actually do have a defined size associated with
 * them, as long as they are not allowed to decay to a pointer.  These template methods exploit this
 * to allow size-based array operations without explicitly specifying the size.  If passed a pointer
 * rather than an array, they'll fail to compile.
 */

/**
 * Return the size in bytes of the array \p a.
 */
template <typename T, size_t N> inline size_t array_size(const T (&a)[N]) { return sizeof(a); }

/**
 * Return the number of elements in array \p a.
 */
template <typename T, size_t N> inline size_t array_length(const T (&)[N]) { return N; }

/**
 * Duplicate the array \p a.  The memory for the new array is malloced and the caller takes
 * responsibility.  Note that the dup is necessarily returned as a pointer, so size is lost.  Call
 * array_length() on the original array to discover the size.
 */
template <typename T, size_t N> inline T* dup_array(const T (&a)[N]) {
    T* dup = new T[N];
    if (dup != NULL) {
        memcpy(dup, &a, array_size(a));
    }
    return dup;
}

/**
 * Copy the contents of array \p arr to \p dest.
 */
template <typename T, size_t N> inline void copy_array(const T (&arr)[N], T* dest) {
    for (size_t i = 0; i < N; ++i)
        dest[i] = arr[i];
}

/**
 * Search array \p a for value \p val, returning true if found.  Note that this function is
 * early-exit, meaning that it should not be used in contexts where timing analysis attacks could be
 * a concern.
 */
template <typename T, size_t N> inline bool array_contains(const T (&a)[N], T val) {
    for (size_t i = 0; i < N; ++i) {
        if (a[i] == val) {
            return true;
        }
    }
    return false;
}

/**
 * Eraser clears buffers.  Construct it with a buffer or object and the destructor will ensure that
 * it is zeroed.
 */
class Eraser {
  public:
    /* Not implemented.  If this gets used, we want a link error. */
    template <typename T> explicit Eraser(T* t);

    template <typename T>
    explicit Eraser(T& t)
        : buf_(reinterpret_cast<uint8_t*>(&t)), size_(sizeof(t)) {}

    template <size_t N> explicit Eraser(uint8_t (&arr)[N]) : buf_(arr), size_(N) {}

    Eraser(void* buf, size_t size) : buf_(static_cast<uint8_t*>(buf)), size_(size) {}
    ~Eraser() {
        while (size_-- > 0)
            *buf_++ = 0;
    }

  private:
    uint8_t* buf_;
    size_t size_;
};

}  // namespace keymaster

#endif  // SYSTEM_KEYMASTER_GOOGLE_KEYMASTER_UTILS_H_