#include "bigint.h"

BigInteger::BigInteger() {
  digits = {0};
}

BigInteger::BigInteger(int64_t x) {
  if (x == 0) {
    digits = {0};
    return;
  }
  uint64_t val = x > 0 ? x : -x;
  while (val > 0) {
    digits.push_back(val % BASE);
    val /= BASE;
  }
}

BigInteger::BigInteger(int small) : BigInteger(static_cast<int64_t>(small)) {}

BigInteger::BigInteger(const std::string& s) {
  if (s.empty() || s == "0") {
    digits = {0};
    return;
  }
  int res = 0;
  int pow10 = 1;
  for (int i = (int)s.size() - 1; i >= 0; --i) {
    res += (s[i] - '0') * pow10;
    pow10 *= 10;
    if (pow10 >= 1000000000 || i == 0) {
      digits.push_back(res);
      res = 0;
      pow10 = 1;
    }
  }
  normalize();
}

void BigInteger::normalize() {
  stripTrailingZeroes();
  if (digits.empty()) digits = {0};
}

void BigInteger::stripTrailingZeroes() {
  while (digits.size() > 1 && digits.back() == 0)
    digits.pop_back();
}

std::string BigInteger::toString() const {
  if (isZero()) return "0";
  std::string res;
  for (int i = (int)digits.size() - 1; i >= 0; --i) {
    char buf[10];
    std::sprintf(buf, "%d", digits[i]);
    if (!res.empty() && digits[i] < 100000000) {
      int leadingZeros = 9 - std::strlen(buf);
      res += std::string(leadingZeros, '0');
    }
    res += buf;
  }
  return res;
}

bool BigInteger::isZero() const {
  return digits.size() == 1 && digits[0] == 0;
}

bool BigInteger::operator==(const BigInteger& other) const {
  return digits == other.digits;
}

bool BigInteger::operator!=(const BigInteger& other) const {
  return !(*this == other);
}

bool BigInteger::operator<(const BigInteger& other) const {
  if (digits.size() != other.digits.size())
    return digits.size() < other.digits.size();
  for (int i = (int)digits.size() - 1; i >= 0; --i) {
    if (digits[i] != other.digits[i])
      return digits[i] < other.digits[i];
  }
  return false;
}

bool BigInteger::operator<=(const BigInteger& other) const {
  return (*this < other) || (*this == other);
}

bool BigInteger::operator>(const BigInteger& other) const {
  return !(*this <= other);
}

bool BigInteger::operator>=(const BigInteger& other) const {
  return !(*this < other);
}

BigInteger BigInteger::operator+(const BigInteger& other) const {
  BigInteger res;
  int carry = 0;
  int n = std::max(digits.size(), other.digits.size());
  res.digits.resize(n + 1);

  for (int i = 0; i < n + 1; ++i) {
    int x = (i < (int)digits.size()) ? digits[i] : 0;
    int y = (i < (int)other.digits.size()) ? other.digits[i] : 0;
    int sum = x + y + carry;
    res.digits[i] = sum % BASE;
    carry = sum / BASE;
  }
  res.stripTrailingZeroes();
  return res;
}

BigInteger BigInteger::operator-(const BigInteger& other) const {
  if (*this < other) {
    throw std::domain_error("Subtraction would produce negative result");
  }
  BigInteger res;
  res.digits.resize(digits.size());
  int borrow = 0;
  for (int i = 0; i < (int)digits.size(); ++i) {
    int x = digits[i] - borrow;
    int y = (i < (int)other.digits.size()) ? other.digits[i] : 0;
    if (x < y) {
      res.digits[i] = x + BASE - y;
      borrow = 1;
    } else {
      res.digits[i] = x - y;
      borrow = 0;
    }
  }
  res.stripTrailingZeroes();
  return res;
}

BigInteger BigInteger::operator*(int small) const {
  if (small == 0) return BigInteger(0);
  BigInteger res;
  int64_t carry = 0;
  res.digits.resize(digits.size() + 1);
  for (int i = 0; i < (int)digits.size(); ++i) {
    int64_t p = (int64_t)digits[i] * small + carry;
    res.digits[i] = (int)(p % BASE);
    carry = p / BASE;
  }
  if (carry) res.digits[digits.size()] = (int)carry;
  res.stripTrailingZeroes();
  return res;
}

BigInteger BigInteger::operator*(const BigInteger& other) const {
  return karatsuba(other);
}

BigInteger BigInteger::karatsuba(const BigInteger& other) const {
  return karatsubaMultiply(this->digits, other.digits);
}

std::vector<int> BigInteger::addVectors(const std::vector<int>& a, const std::vector<int>& b) const {
  std::vector<int> res(std::max(a.size(), b.size()) + 1, 0);
  int carry = 0;
  for (size_t i = 0; i < res.size(); ++i) {
    int x = (i < a.size()) ? a[i] : 0;
    int y = (i < b.size()) ? b[i] : 0;
    int sum = x + y + carry;
    res[i] = sum % BASE;
    carry = sum / BASE;
  }
  while (res.size() > 1 && res.back() == 0) res.pop_back();
  return res;
}

std::vector<int> BigInteger::subtractVectors(const std::vector<int>& a, const std::vector<int>& b) const {
  std::vector<int> res(a.size(), 0);
  int borrow = 0;
  for (size_t i = 0; i < a.size(); ++i) {
    int x = a[i] - borrow;
    int y = (i < b.size()) ? b[i] : 0;
    if (x < y) {
      res[i] = x + BASE - y;
      borrow = 1;
    } else {
      res[i] = x - y;
      borrow = 0;
    }
  }
  while (res.size() > 1 && res.back() == 0) res.pop_back();
  return res;
}

BigInteger BigInteger::karatsubaMultiply(const std::vector<int>& a, const std::vector<int>& b) const {
  if (a.empty() || b.empty()) return BigInteger(0);
  if (a.size() <= KARATSUBA_THRESHOLD || b.size() <= KARATSUBA_THRESHOLD) {
    BigInteger res;
    res.digits.resize(a.size() + b.size(), 0);
    for (size_t i = 0; i < a.size(); ++i) {
      int64_t carry = 0;
      for (size_t j = 0; j < b.size(); ++j) {
        int64_t p = (int64_t)a[i] * b[j] + res.digits[i + j] + carry;
        res.digits[i + j] = (int)(p % BASE);
        carry = p / BASE;
      }
      if (carry) {
        res.digits[i + b.size()] += (int)carry;
      }
    }
    res.stripTrailingZeroes();
    return res;
  }

  size_t m = std::max(a.size(), b.size()) / 2;

  std::vector<int> a0(a.begin(), a.begin() + std::min(m, a.size()));
  std::vector<int> a1(a.begin() + std::min(m, a.size()), a.end());
  std::vector<int> b0(b.begin(), b.begin() + std::min(m, b.size()));
  std::vector<int> b1(b.begin() + std::min(m, b.size()), b.end());

  BigInteger z0 = karatsubaMultiply(a0, b0);
  BigInteger z2 = karatsubaMultiply(a1, b1);

  std::vector<int> a1a0 = addVectors(a1, a0);
  std::vector<int> b1b0 = addVectors(b1, b0);
  BigInteger z1 = karatsubaMultiply(a1a0, b1b0);

  // z1 = z1 - z0 - z2
  BigInteger temp = z1 - z0;
  z1 = temp - z2;

  // Сдвиг и сложение
  BigInteger result = z0;

  // Добавляем z1 * BASE^m
  if (!z1.isZero()) {
    BigInteger shifted_z1 = z1.shiftLeft(m);
    result = result + shifted_z1;
  }

  // Добавляем z2 * BASE^(2m)
  if (!z2.isZero()) {
    BigInteger shifted_z2 = z2.shiftLeft(2 * m);
    result = result + shifted_z2;
  }

  return result;
}

BigInteger BigInteger::shiftLeft(size_t m) const {
  if (isZero()) return *this;
  BigInteger result = *this;
  result.digits.insert(result.digits.begin(), m, 0);
  result.stripTrailingZeroes();
  return result;
}

std::pair<BigInteger, BigInteger> BigInteger::longDivide(const BigInteger& b) const {
  if (b.isZero()) {
    throw std::domain_error("Division by zero");
  }
  if (*this < b) {
    return {BigInteger(0), *this};
  }

  BigInteger a = *this;
  BigInteger q, r;
  q.digits.resize(a.digits.size(), 0);
  r.digits = {0};

  for (int i = (int)a.digits.size() - 1; i >= 0; --i) {
    if (!(r.digits.size() == 1 && r.digits[0] == 0)) {
      r.digits.insert(r.digits.begin(), 0);
    }
    r.digits[0] = a.digits[i];
    r.normalize();

    int low = 0;
    int high = BASE;
    int digit = 0;

    while (low <= high) {
      int mid = (low + high) / 2;
      BigInteger test = b * mid;
      if (test <= r) {
        digit = mid;
        low = mid + 1;
      } else {
        high = mid - 1;
      }
    }

    q.digits[i] = digit;
    BigInteger prod = b * digit;
    r = r - prod;
  }

  q.stripTrailingZeroes();
  return {q, r};
}

BigInteger BigInteger::operator/(const BigInteger& other) const {
  if (other.isZero()) {
    throw std::domain_error("Division by zero");
  }
  return longDivide(other).first;
}

BigInteger BigInteger::operator%(const BigInteger& other) const {
  if (other.isZero()) {
    throw std::domain_error("Division by zero");
  }
  return longDivide(other).second;
}

bool BigInteger::operator==(int small) const {
  return *this == BigInteger(small);
}

bool BigInteger::operator!=(int small) const {
  return *this != BigInteger(small);
}

bool BigInteger::operator<(int small) const {
  return *this < BigInteger(small);
}

bool BigInteger::operator<=(int small) const {
  return *this <= BigInteger(small);
}

bool BigInteger::operator>(int small) const {
  return *this > BigInteger(small);
}

bool BigInteger::operator>=(int small) const {
  return *this >= BigInteger(small);
}

bool operator==(int small, const BigInteger& a) {
  return a == small;
}

bool operator!=(int small, const BigInteger& a) {
  return a != small;
}

bool operator<(int small, const BigInteger& a) {
  return BigInteger(small) < a;
}

bool operator<=(int small, const BigInteger& a) {
  return BigInteger(small) <= a;
}

bool operator>(int small, const BigInteger& a) {
  return BigInteger(small) > a;
}

bool operator>=(int small, const BigInteger& a) {
  return BigInteger(small) >= a;
}
