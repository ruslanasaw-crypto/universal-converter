#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <cstring>

class BigInteger {
public:
  static const int BASE = 1000000000;  // 10^9

  BigInteger();
  explicit BigInteger(int64_t x);
  explicit BigInteger(const std::string& s);
  BigInteger(int small);

  std::string toString() const;
  bool isZero() const;

         // Сравнения
  bool operator==(const BigInteger& other) const;
  bool operator!=(const BigInteger& other) const;
  bool operator<(const BigInteger& other) const;
  bool operator<=(const BigInteger& other) const;
  bool operator>(const BigInteger& other) const;
  bool operator>=(const BigInteger& other) const;

         // Арифметика
  BigInteger operator+(const BigInteger& other) const;
  BigInteger operator-(const BigInteger& other) const;
  BigInteger operator*(const BigInteger& other) const;
  BigInteger operator*(int small) const;
  BigInteger operator/(const BigInteger& other) const;
  BigInteger operator%(const BigInteger& other) const;

         // Сравнения с int
  bool operator==(int small) const;
  bool operator!=(int small) const;
  bool operator<(int small) const;
  bool operator<=(int small) const;
  bool operator>(int small) const;
  bool operator>=(int small) const;

         // Дружественные функции
  friend bool operator==(int small, const BigInteger& a);
  friend bool operator!=(int small, const BigInteger& a);
  friend bool operator<(int small, const BigInteger& a);
  friend bool operator<=(int small, const BigInteger& a);
  friend bool operator>(int small, const BigInteger& a);
  friend bool operator>=(int small, const BigInteger& a);

         // Оператор вывода
  friend std::ostream& operator<<(std::ostream& os, const BigInteger& bi);
  friend std::istream& operator>>(std::istream& is, BigInteger& bi);

private:
  std::vector<int> digits;  // младшие разряды first

  void normalize();
  void stripTrailingZeroes();
  std::pair<BigInteger, BigInteger> longDivide(const BigInteger& b) const;
};

