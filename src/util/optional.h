#pragma once

template <typename T>
class optional {
  T data;
  bool active;
  
public:
  optional(): active(false) {}
  optional(const T& data): data(data), active(true) {}
  
  operator bool() const { return active; }
  T& operator *() const { return data; }
  const T* operator ->() const { return &data; }
  optional<T>& operator =(const optional<T>& other) {
    active = other.active;
    data = other.data;
    return *this;
  }
};
