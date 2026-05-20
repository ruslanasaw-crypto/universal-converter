#pragma once
#include "bigint.h"
#include <stdexcept>

BigInteger gcd(BigInteger a, BigInteger b);

class BigFraction {
public:
  BigFraction();
  BigFraction(BigInteger num, BigInteger den);
  BigFraction(BigInteger num);
  BigFraction(int num);

  BigInteger num() const { return num_; }
  BigInteger den() const { return den_; }

         // Сравнения
  bool operator==(const BigFraction& other) const;
  bool operator!=(const BigFraction& other) const;
  bool operator<(const BigFraction& other) const;
  bool operator<=(const BigFraction& other) const;
  bool operator>(const BigFraction& other) const;
  bool operator>=(const BigFraction& other) const;

         // Арифметика
  BigFraction operator+(const BigFraction& other) const;
  BigFraction operator-(const BigFraction& other) const;
  BigFraction operator*(const BigFraction& other) const;
  BigFraction operator/(const BigFraction& other) const;

  BigInteger integerPart() const;
  BigFraction remainder() const;

private:
  BigInteger num_, den_;
  void reduce();
};
