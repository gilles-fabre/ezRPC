
#include <iostream>
#include <algorithm>

#include "ReturnValue.h"

/*
class SomeClass {
public:
  ReturnValue<bool, BasicErrors> isEven(int value) {
    ReturnValue<bool, BasicErrors> r;

    if (value == 0)
      r = ReturnValue<bool, BasicErrors>{ BasicErrors::ErrorCode::BadArgument };
    else
      r = ReturnValue<bool, BasicErrors>{ !(value % 2) };

    return r;
  }
};

class SomeOtherClass {
public:
  ReturnValue<int, BasicErrors> length(string value) {
    ReturnValue<int, BasicErrors> r;

    if (value.empty())
      r = ReturnValue<int, BasicErrors>{ BasicErrors::ErrorCode::BadArgument };
    else
      r = ReturnValue<int, BasicErrors>{ static_cast<int>(value.length()) };

    return r;
  }
};

class SomeThirdClass {
public:
  ReturnValue<bool, BasicErrors> is_alphanum(string value) {
    ReturnValue<bool, BasicErrors> r;

    if (value.empty())
      r = ReturnValue<bool, BasicErrors>{ BasicErrors::ErrorCode::BadArgument };
    else {
      const char* p = value.c_str();
      r = ReturnValue<bool, BasicErrors>{
          std::all_of(p,
                      p + value.length(),
                      [](char c) {
                          return std::isalnum(c);
                      }) };
    }

    return r;
  }

  ReturnValue<int, BasicErrors> length(string value) {
    ReturnValue<int, BasicErrors> r;

    if (value.empty())
      r = ReturnValue<int, BasicErrors>{ BasicErrors::ErrorCode::BadArgument };
    else
      r = ReturnValue<int, BasicErrors>{ static_cast<int>(value.length()) };

    return r;
  }
};

class OneLastClass {
public:
  ReturnValue<int, ExtendedErrors> factorial(int value) {
    ReturnValue<int, ExtendedErrors> r;

    if (value > 100)
      r = ReturnValue<int, ExtendedErrors>{ ExtendedErrors::ErrorCode::Overflow };
    else if (value <= 1)
      r = ReturnValue<int, ExtendedErrors>{ 1 };
    else {
      int f = (int)factorial(value - 1);
      r = ReturnValue<int, ExtendedErrors>{ value * f };
    }

    return r;
  }
};

int main(int argc, char** argv) {
  SomeClass s;
  ReturnValue<bool, BasicErrors> v = s.isEven(0);
  cout << "isEvent(0) -> " << v.ToString() << endl;
  cout << "isEvent(0).error code/result : " << static_cast<int>((BasicErrors::ErrorCode)v) << "/" << (bool)v << endl;
  v = s.isEven(1);
  cout << "isEvent(1) -> " << v.ToString() << endl;
  cout << "isEvent(1).error code/result : " << static_cast<int>((BasicErrors::ErrorCode)v) << "/" << (bool)v << endl;
  v = s.isEven(2);
  cout << "isEvent(2) -> " << v.ToString() << endl;
  cout << "isEvent(3).error code/result : " << static_cast<int>((BasicErrors::ErrorCode)v) << "/" << (bool)v << endl;

  SomeOtherClass s2;
  ReturnValue<int, BasicErrors> v2 = s2.length("");
  cout << "length(\"\") -> " << v2.ToString() << endl;
  cout << "length(\"\").error code/result : " << static_cast<int>((BasicErrors::ErrorCode)v2) << "/" << (int)v2 << endl;
  v2 = s2.length("une chaine");
  cout << "length(\"une chaine\") -> " << v2.ToString() << endl;
  cout << "length(\"une chaine\").error code/result : " << static_cast<int>((BasicErrors::ErrorCode)v2) << "/" << (int)v2 << endl;
  v2 = s2.length("une autre chaine");
  cout << "length(\"une autre chaine\") -> " << v2.ToString() << endl;
  cout << "length(\"une autre chaine\").error code/result : " << static_cast<int>((BasicErrors::ErrorCode)v2) << "/" << (int)v2 << endl;

  SomeThirdClass s3;
  ReturnValue<int, BasicErrors> v3 = s3.length("");
  cout << "length(\"\") -> " << v3.ToString() << endl;
  cout << "length(\"\").error code/result : " << static_cast<int>((BasicErrors::ErrorCode)v3) << "/" << (int)v3 << endl;
  v3 = s3.length("une chaine");
  cout << "length(\"une chaine\") -> " << v3.ToString() << endl;
  cout << "length(\"une chaine\").error code/result : " << static_cast<int>((BasicErrors::ErrorCode)v3) << "/" << (int)v3 << endl;
  v3 = s3.length("une autre chaine");
  cout << "length(\"une autre chaine\") -> " << v3.ToString() << endl;
  cout << "length(\"une autre chaine\").error code/result : " << static_cast<int>((BasicErrors::ErrorCode)v3) << "/" << (int)v3 << endl;

  ReturnValue<bool, BasicErrors> v4 = s3.is_alphanum("1234abcd");
  cout << "is_alphanum(\"1234abcd\") -> " << v4.ToString() << endl;
  cout << "is_alphanum(\"1234abcd\").error code/result : " << static_cast<int>((BasicErrors::ErrorCode)v4) << "/" << (int)v4 << endl;
  v4 = s3.is_alphanum("une chaine");
  cout << "is_alphanum(\"une chaine\") -> " << v4.ToString() << endl;
  cout << "is_alphanum(\"une chaine\").error code/result : " << static_cast<int>((BasicErrors::ErrorCode)v3) << "/" << (int)v4 << endl;
  v4 = s3.is_alphanum("une autre chaine");
  cout << "is_alphanum(\"une autre chaine\") -> " << v4.ToString() << endl;
  cout << "is_alphanum(\"une autre chaine\").error code/result : " << static_cast<int>((BasicErrors::ErrorCode)v3) << "/" << (int)v4 << endl;

  OneLastClass s4;
  ReturnValue<int, ExtendedErrors> v5 = s4.factorial(1000);
  cout << "factorial(1000) -> " << v5.ToString() << endl;
  cout << "factorial(1000).error code/result : " << static_cast<int>((ExtendedErrors::ErrorCode)v5) << "/" << (int)v5 << endl;
  v5 = s4.factorial(10);
  cout << "factorial(10) -> " << v5.ToString() << endl;
  cout << "factorial(10).error code/result : " << static_cast<int>((ExtendedErrors::ErrorCode)v5) << "/" << (int)v5 << endl;
  v5 = s4.factorial(0);
  cout << "factorial(0) -> " << v5.ToString() << endl;
  cout << "factorial(0).error code/result : " << static_cast<int>((ExtendedErrors::ErrorCode)v5) << "/" << (int)v5 << endl;

  return 0;
}
*/