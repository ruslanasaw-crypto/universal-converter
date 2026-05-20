#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cstring>

class BigInteger {
public:
  static const int BASE = 1000000000;  // 10^9
  static const size_t KARATSUBA_THRESHOLD = 32;

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

         // Умножение Карацубы
  BigInteger karatsuba(const BigInteger& other) const;

  // Сдвиг на m цифр влево (умножение на BASE^m)
  BigInteger shiftLeft(size_t m) const;

         // Дружественные функции
  friend bool operator==(int small, const BigInteger& a);
  friend bool operator!=(int small, const BigInteger& a);
  friend bool operator<(int small, const BigInteger& a);
  friend bool operator<=(int small, const BigInteger& a);
  friend bool operator>(int small, const BigInteger& a);
  friend bool operator>=(int small, const BigInteger& a);

private:
  std::vector<int> digits;  // младшие разряды first

  void normalize();
  void stripTrailingZeroes();
  std::pair<BigInteger, BigInteger> longDivide(const BigInteger& b) const;
  BigInteger karatsubaMultiply(const std::vector<int>& a, const std::vector<int>& b) const;
  std::vector<int> addVectors(const std::vector<int>& a, const std::vector<int>& b) const;
  std::vector<int> subtractVectors(const std::vector<int>& a, const std::vector<int>& b) const;
};
