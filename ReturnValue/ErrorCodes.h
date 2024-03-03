#pragma once 

class BasicErrors {
public:
  typedef enum _ErrorCode { None, EmptyResult, BadArgument, END } ErrorCode;
  static constexpr const char* m_errors[] = { "None", "EmptyResult", "BadArgument" };
};


class ExtendedErrors {
public:
  typedef enum _ErrorCode { None, EmptyResult, BadArgument, Overflow, END } ErrorCode;
  static constexpr const char* m_errors[] = { "None", "EmptyResult", "BadArgument", "Overflow" };
};


