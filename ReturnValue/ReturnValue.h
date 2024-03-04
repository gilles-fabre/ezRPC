#pragma once

#include <string>
#include <optional>
#include <cassert>
#include <sstream>

#include "ErrorCodes.h"

using namespace std;

template <typename T, class E>
class	ReturnValue {
  pair<optional<T>, int> m_value;

public:
  ReturnValue() : m_value{ {}, static_cast<int>(E::ErrorCode::EmptyResult) } {}
  template <typename C>
  ReturnValue(C error) : m_value{ {}, static_cast<int>(error) } {}
  ReturnValue(T result) : m_value{ result, static_cast<int>(E::ErrorCode::None) } {}
  template <typename C>
  ReturnValue(T result, C error) : m_value{ result, static_cast<int>(error) } {}
  ReturnValue(const ReturnValue& other) {
    *this = other;
  }
  ReturnValue(ReturnValue&& other) noexcept {
    *this = std::move(other);
  }
  ReturnValue& operator =(const ReturnValue& other) {
    m_value = other.value;
    return *this;
  }
  ReturnValue& operator =(ReturnValue&& other) noexcept {
    m_value = std::move(other.m_value);
    return *this;
  }

  bool IsError() {
    return m_value.second != E::None;
  }

  template <typename E>
  operator E() {
    return static_cast<E>(m_value.second);
  }

  template <typename E>
  E GetError() {
    return static_cast<E>(m_value.second);
  }

  string GetErrorString() {
    int i = m_value.second;
    assert(i >= 0 || i < static_cast<int>(E::ErrorCode::END));
    return E::m_errors[i];
  }

  operator T& () {
    return *m_value.first;
  }

  T& GetResult() {
    return *m_value.first;
  }

  operator string() {
    return ToString();
  }

  string ToString() {
    int i = m_value.second;
    assert(i >= 0 || i < static_cast<int>(E::ErrorCode::END));
    stringstream ss;
    ss << "{" << *m_value.first << "," << E::m_errors[i] << "}";
    return ss.str();
  }
};

