#include "bigfrac.h"

BigInteger gcd(BigInteger a, BigInteger b) {
  while (!b.isZero()) {
    BigInteger r = a % b;
    a = b;
    b = r;
  }
  return a;
}

BigFraction::BigFraction() : num_(0), den_(1) {}

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

BigFraction::BigFraction(BigInteger num) : num_(num), den_(1) {}

BigFraction::BigFraction(int num) : BigFraction(BigInteger(num)) {}

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
  if (rem < 0) {
    rem = rem + den_;
  }
  return BigFraction(rem, den_);
}

bool BigFraction::operator==(const BigFraction& other) const {
  return num_ * other.den_ == den_ * other.num_;
}

bool BigFraction::operator!=(const BigFraction& other) const {
  return !(*this == other);
}

bool BigFraction::operator<(const BigFraction& other) const {
  return num_ * other.den_ < other.num_ * den_;
}

bool BigFraction::operator<=(const BigFraction& other) const {
  return (*this < other) || (*this == other);
}

bool BigFraction::operator>(const BigFraction& other) const {
  return other < *this;
}

bool BigFraction::operator>=(const BigFraction& other) const {
  return !(*this < other);
}

BigFraction BigFraction::operator+(const BigFraction& other) const {
  BigInteger num = num_ * other.den_ + other.num_ * den_;
  BigInteger den = den_ * other.den_;
  return BigFraction(num, den);
}

BigFraction BigFraction::operator-(const BigFraction& other) const {
  if (*this < other) {
    throw std::domain_error("Subtraction would produce negative fraction");
  }
  BigInteger num = num_ * other.den_ - other.num_ * den_;
  BigInteger den = den_ * other.den_;
  return BigFraction(num, den);
}

BigFraction BigFraction::operator*(const BigFraction& other) const {
  BigInteger num = num_ * other.num_;
  BigInteger den = den_ * other.den_;
  return BigFraction(num, den);
}

BigFraction BigFraction::operator/(const BigFraction& other) const {
  if (other.num_.isZero())
    throw std::domain_error("Division by zero");
  BigInteger num = num_ * other.den_;
  BigInteger den = den_ * other.num_;
  return BigFraction(num, den);
}
