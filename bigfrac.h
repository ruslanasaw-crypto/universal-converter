#pragma once
#include "bigint.h"
#include <iostream>

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
  BigFraction operator-(const BigFraction& other) const;  // a >= b
  BigFraction operator*(const BigFraction& other) const;
  BigFraction operator/(const BigFraction& other) const;  // other != 0
  // Добавьте этот метод в класс BigFraction (в .h файл)
  std::string toDecimal(int precision = 10) const;  // precision - кол-во знаков после запятой

         // Или перегруженный оператор для вывода в десятичном виде
  friend std::ostream& operator<<(std::ostream& out, const BigFraction& other);
  friend std::istream& operator>>(std::istream& is, BigFraction& other);
  // Упрощения
  std::string toString() const;  // дробный вид
  std::string toDecimalString(int precision = 10) const;
  BigInteger integerPart() const;   // целая часть: a / b
  BigFraction remainder() const;    // a % b как дробь

private:
  BigInteger num_, den_;
  void reduce();
};


