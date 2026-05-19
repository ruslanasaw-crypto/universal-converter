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


BigInteger gcd(BigInteger a, BigInteger b) {
  while (!b.isZero()) {
    BigInteger r = a % b;
    a = b;
    b = r;
  }
  return a;
}

BigFraction::BigFraction() : num_(0), den_(1) {
}

BigFraction::BigFraction(BigInteger num, BigInteger den) {
  if (den.isZero())
    throw std::domain_error("Denominator cannot be zero");

  num_ = num;
  den_ = den;
  if (den_ < 0) {
    num_ = num_ * BigInteger(-1);
    den_ = den_ * BigInteger(-1);
  }
  reduce();
}

BigFraction::BigFraction(BigInteger num) : num_(num), den_(1) {
}

void BigFraction::reduce() {
  BigInteger g = gcd(num_, den_);
  if (!g.isZero()) {
    num_ = num_ / g;
    den_ = den_ / g;
  }
}

BigInteger BigFraction::integerPart() const {
  return num_ / den_;
}

BigFraction BigFraction::remainder() const {
  BigInteger q = integerPart();
  BigInteger rem = num_ - q * den_;
  return BigFraction(rem, den_);
}

// Сравнения
bool BigFraction::operator==(const BigFraction &other) const {
  return num_ * other.den_ == den_ * other.num_;
}

bool BigFraction::operator!=(const BigFraction &other) const {
  return !(*this == other);
}

bool BigFraction::operator<(const BigFraction &other) const {
  return num_ * other.den_ < other.num_ * den_;
}

bool BigFraction::operator<=(const BigFraction &other) const {
  return (*this < other) || (*this == other);
}

bool BigFraction::operator>(const BigFraction &other) const {
  return other < *this;
}

bool BigFraction::operator>=(const BigFraction &other) const {
  return !(*this < other);
}

// Арифметика — без побочных изменений num_/den_ в аргументах
BigFraction BigFraction::operator+(const BigFraction &other) const {
  BigInteger num = num_ * other.den_ + other.num_ * den_;
  BigInteger den = den_ * other.den_;
  return BigFraction(num, den);
}

BigFraction BigFraction::operator-(const BigFraction &other) const {
  if (*this < other) {
    throw std::domain_error("Subtraction would produce negative fraction");
  }
  BigInteger num = num_ * other.den_ - other.num_ * den_;
  BigInteger den = den_ * other.den_;
  return BigFraction(num, den);
}

BigFraction BigFraction::operator*(const BigFraction &other) const {
  BigInteger num = num_ * other.num_;
  BigInteger den = den_ * other.den_;
  return BigFraction(num, den);
}

BigFraction BigFraction::operator/(const BigFraction &other) const {
  if (other.num_.isZero())
    throw std::domain_error("Division by zero");
  BigInteger num = num_ * other.den_;
  BigInteger den = den_ * other.num_;
  return BigFraction(num, den);
}

BigFraction::BigFraction(int num) : BigFraction(BigInteger(num)) {
}

// Метод для преобразования в десятичную строку
std::string BigFraction::toDecimal(int precision) const {
  if (num_.isZero()) return "0";

  BigInteger intPart = num_ / den_;
  BigInteger rem = num_ % den_;

  std::string result = intPart.toString();

  if (precision > 0 && !rem.isZero()) {
    result += ".";

    BigInteger current = rem;
    for (int i = 0; i < precision; ++i) {
      current = current * 10;
      BigInteger digit = current / den_;
      result += digit.toString();
      current = current % den_;

      if (current.isZero()) break;
    }
  }

  return result;
}

std::istream &operator>>(std::istream &is, BigFraction &frac) {
  std::string token;
  is >> token;

  size_t slash = token.find('/');
  if (slash == std::string::npos) {
    // ввод без знаменателя: x  ->  x/1
    BigInteger num(token);
    frac = BigFraction(num, BigInteger(1));
  } else {
    std::string num_str = token.substr(0, slash);
    std::string den_str = token.substr(slash + 1);
    BigInteger num(num_str);
    BigInteger den(den_str);
    frac = BigFraction(num, den);
  }
  return is;
}

std::string BigFraction::toString() const {
  if (den_ == 1) {
    return num_.toString();
  }
  return num_.toString() + "/" + den_.toString();
}

std::string BigFraction::toDecimalString(int precision) const {
  if (num_.isZero()) return "0";

  BigInteger intPart = num_ / den_;
  BigInteger rem = num_ % den_;

  std::string result = intPart.toString();

  if (precision > 0 && !rem.isZero()) {
    result += ".";

    BigInteger current = rem;
    for (int i = 0; i < precision; ++i) {
      current = current * 10;
      BigInteger digit = current / den_;
      result += digit.toString();
      current = current % den_;

      if (current.isZero()) break;
    }
  }

  return result;
}

// Оператор вывода оставляем для дробного вида
std::ostream& operator<<(std::ostream& out, const BigFraction& other) {
  out << other.toDecimalString();
  return out;
}
