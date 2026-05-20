#include"bigint.h"

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

BigInteger::BigInteger(int small) : BigInteger(static_cast<int64_t>(small)) {
}

BigInteger::BigInteger(const std::string &s) {
  if (s.empty() || s == "0") {
    digits = {0};
    return;
  }
  int res = 0;
  int pow10 = 1;
  for (int i = (int) s.size() - 1; i >= 0; --i) {
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
  for (int i = (int) digits.size() - 1; i >= 0; --i) {
    char buf[10];
    std::sprintf(buf, "%d", digits[i]);
    if (!res.empty() && digits[i] < 100000000) {
      // Добавляем ведущие нули
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

// Сравнения
bool BigInteger::operator==(const BigInteger &other) const {
  return digits == other.digits;
}

bool BigInteger::operator!=(const BigInteger &other) const {
  return !(*this == other);
}

bool BigInteger::operator<(const BigInteger &other) const {
  if (digits.size() != other.digits.size())
    return digits.size() < other.digits.size();
  for (int i = (int) digits.size() - 1; i >= 0; --i) {
    if (digits[i] != other.digits[i])
      return digits[i] < other.digits[i];
  }
  return false;
}

bool BigInteger::operator<=(const BigInteger &other) const {
  return (*this < other) || (*this == other);
}

bool BigInteger::operator>(const BigInteger &other) const {
  return !(*this <= other);
}

bool BigInteger::operator>=(const BigInteger &other) const {
  return !(*this < other);
}

// Арифметика
BigInteger BigInteger::operator+(const BigInteger &other) const {
  BigInteger res;
  int carry = 0;
  int n = std::max(digits.size(), other.digits.size());
  res.digits.resize(n + 1);

  for (int i = 0; i < n + 1; ++i) {
    int x = (i < (int) digits.size()) ? digits[i] : 0;
    int y = (i < (int) other.digits.size()) ? other.digits[i] : 0;
    int sum = x + y + carry;
    res.digits[i] = sum % BASE;
    carry = sum / BASE;
  }
  res.stripTrailingZeroes();
  return res;
}

BigInteger BigInteger::operator-(const BigInteger &other) const {
  if (*this < other) {
    throw std::domain_error("Subtraction would produce negative result");
  }
  BigInteger res;
  res.digits.resize(digits.size());
  int borrow = 0;
  for (int i = 0; i < (int) digits.size(); ++i) {
    int x = digits[i] - borrow;
    int y = (i < (int) other.digits.size()) ? other.digits[i] : 0;
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
  for (int i = 0; i < (int) digits.size(); ++i) {
    int64_t p = (int64_t) digits[i] * small + carry;
    res.digits[i] = (int) (p % BASE);
    carry = p / BASE;
  }
  if (carry) res.digits[digits.size()] = (int) carry;
  res.stripTrailingZeroes();
  return res;
}

BigInteger BigInteger::operator*(const BigInteger &other) const {
  BigInteger res;
  res.digits.resize(digits.size() + other.digits.size(), 0);
  for (int i = 0; i < (int) digits.size(); ++i) {
    int64_t carry = 0;
    for (int j = 0; j < (int) other.digits.size(); ++j) {
      int64_t p = (int64_t) digits[i] * other.digits[j] + res.digits[i + j] + carry;
      res.digits[i + j] = (int) (p % BASE);
      carry = p / BASE;
    }
    if (carry) {
      res.digits[i + other.digits.size()] += (int) carry;
    }
  }
  res.stripTrailingZeroes();
  return res;
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
  q.digits.resize(a.digits.size(), 0);  // частное
  r.digits = {0};

         // идём по цифрам слева направо (от старших)
  for (int i = (int)a.digits.size() - 1; i >= 0; --i) {
    // сдвигаем r на одну цифру влево (r = r * BASE)
    if (!(r.digits.size() == 1 && r.digits[0] == 0)) {
      r.digits.insert(r.digits.begin(), 0);
    }
    r.digits[0] = a.digits[i];
    r.normalize();

           // бинарный поиск по цифре частного: 0 <= d < BASE
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

BigInteger BigInteger::operator/(const BigInteger &other) const {
  if (other.isZero()) {
    throw std::domain_error("Division by zero");
  }
  return longDivide(other).first;
}

BigInteger BigInteger::operator%(const BigInteger &other) const {
  if (other.isZero()) {
    throw std::domain_error("Division by zero");
  }
  return longDivide(other).second;
}

// Сравнения с int
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

// Обратные сравнения с int
bool operator==(int small, const BigInteger &a) {
  return a == small;
}

bool operator!=(int small, const BigInteger &a) {
  return a != small;
}

bool operator<(int small, const BigInteger &a) {
  return BigInteger(small) < a;
}

bool operator<=(int small, const BigInteger &a) {
  return BigInteger(small) <= a;
}

bool operator>(int small, const BigInteger &a) {
  return BigInteger(small) > a;
}

bool operator>=(int small, const BigInteger &a) {
  return BigInteger(small) >= a;
}


std::ostream& operator<<(std::ostream& os, const BigInteger& bi) {
  if (bi.digits.empty() || (bi.digits.size() == 1 && bi.digits[0] == 0)) {
    os << '0';
    return os;
  }

         // Выводим старший разряд (без ведущих нулей)
  os << bi.digits.back();

         // Выводим остальные разряды с ведущими нулями до 9 цифр
  char buffer[10];
  for (int i = (int)bi.digits.size() - 2; i >= 0; --i) {
    snprintf(buffer, sizeof(buffer), "%09d", bi.digits[i]);
    os << buffer;
  }

  return os;
}
std::istream& operator>>(std::istream& is, BigInteger& bi) {
  std::string s;
  is >> s;
  bi = BigInteger(s);
  return is;
}
