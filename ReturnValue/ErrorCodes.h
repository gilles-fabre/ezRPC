#pragma once 

class BasicErrors {
public:
  typedef enum _ErrorCode { None, EmptyResult, BadArgument, AllocationError, END } ErrorCode;
  static constexpr const char* m_errors[] = { "None", "EmptyResult", "BadArgument", "AllocationError" };
};


class ExtendedErrors {
public:
  typedef enum _ErrorCode { None, EmptyResult, BadArgument, AllocationError, Overflow, END } ErrorCode;
  static constexpr const char* m_errors[] = { "None", "EmptyResult", "BadArgument", "AllocationError", "Overflow" };
};


