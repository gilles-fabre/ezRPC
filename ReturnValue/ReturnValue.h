#pragma once

#include <string>
#include <optional>
#include <cassert>
#include <sstream>

#include "ErrorCodes.h"

using namespace std;

/**
* Instances of this class can be used to return both a value or an error code. It provides
* a more detailled information than a simple boolean, is more explicit than exceptions, 
* and avoids using an output parameters to return the actual result of a method/function
* in conjunction with a return error code/boolean.
* T is the carried actual result of the called method function if E::ErrorCode::None, else
* it is not set (std::optional). E is the enum class of possible error codes. E::ErrorCode::None,
* E::ErrorCode::EmptyResult and E::ErrorCode::END are the sole required values. Subclasses shall
* extend those error codes to return accurate errors.
*/
template <typename T, class E>
class	ReturnValue {
  pair<optional<T>, int>* m_valueP;

public:
  ReturnValue() {
      m_valueP = new pair<optional<T>, int>{ {}, static_cast<int>(E::ErrorCode::EmptyResult) };
  }
  template <typename C>
  ReturnValue(C error) {
      m_valueP = new pair<optional<T>, int>{ {}, static_cast<int>(error) };
  }
  ReturnValue(T result) {
      m_valueP = new pair<optional<T>, int>{ result, static_cast<int>(E::ErrorCode::None)};
  }
  template <typename C>
  ReturnValue(T result, C error) {
      m_valueP = new pair<optional<T>, int>{ result, static_cast<int>(error) };
  }
  ReturnValue(const ReturnValue& other) {
      m_valueP = NULL;
      *this = other;
  }
  ReturnValue(ReturnValue&& other) noexcept {
      m_valueP= NULL;
      *this = other;
  }
  ~ReturnValue() {
      delete m_valueP;
      m_valueP = NULL;
  }
  ReturnValue& operator =(const ReturnValue& other) {
      delete m_valueP;
      m_valueP = new pair <optional<T>, int>{ other.m_valueP->first, other.m_valueP->second };
      return *this;
  }
  ReturnValue& operator =(ReturnValue&& other) noexcept {
      m_valueP = other.m_valueP;
      other.m_valueP = NULL;
      return *this;
  }

  /**
  * \brief Returns true if the ReturnValue carries an error. In this case, the
  *        T value shall not be used.
  * \return true in the case an error is returned by the called function/method.
  */
  bool IsError() {
    return m_valueP->second != E::None;
  }

  /**
  * \brief E::ErrorCode::{error} accessors.
  * \return E::ErrorCode::None if no error was encountered, E::ErrorCode::EmptyResult if the 
  *         function/method didn't process the call.
  */
  template <typename EC>
  operator EC() {
    return static_cast<EC>(m_valueP->second);
  }

  template <typename EC>
  EC GetError() {
    return static_cast<EC>(m_valueP->second);
  }

  string GetErrorString() {
    int i = m_valueP->second;
    assert(i >= 0 || i < static_cast<int>(E::ErrorCode::END));
    return E::m_errors[i];
  }

  /**
  * \brief T value accessors.
  * \return the T value returned by the called function/method is no error was
  *         encountered, undefined else.
  */
  operator T& () {
    return *(m_valueP->first);
  }

  T& GetResult() {
    return *(m_valueP->first);
  }

  /**
  * \brief returns the human readable string representation of the ReturnValue. 
  */
  operator string() {
    return ToString();
  }

  string ToString() {
    int i = m_valueP->second;
    assert(i >= 0 || i < static_cast<int>(E::ErrorCode::END));
    stringstream ss;
    ss << "{" << *(m_valueP->first) << "," << E::m_errors[i] << "}";
    return ss.str();
  }
};

