#include "parser.h"
#include "bigint.h"
#include <sstream>

int charToDigit(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'Z') return 10 + (c - 'A');
  if (c >= 'a' && c <= 'z') return 10 + (c - 'a');
  return -1;
}

BigInteger integerFromBase(const std::string& integer, int p) {
  BigInteger res(0);
  BigInteger base(p);
  BigInteger p_power(1);
  for (int i = (int)integer.size() - 1; i >= 0; --i) {
    int d = charToDigit(integer[i]);
    if (d < 0 || d >= p) {
      throw std::invalid_argument("Invalid digit for base " + std::to_string(p));
    }
    BigInteger digit(d);
    res = res + digit * p_power;
    p_power = p_power * base;
  }
  return res;
}

BigInteger fractionalFromBase(const std::string& fraction, int p) {
  BigInteger base(p);
  BigInteger p_power(1);
  BigInteger res(0);
  for (char c : fraction) {
    int d = charToDigit(c);
    if (d < 0 || d >= p) {
      throw std::invalid_argument("Invalid digit for base " + std::to_string(p));
    }
    p_power = p_power * base;
    BigInteger digit(d);
    res = res * base + digit;
  }
  return res;
}

BigFraction parse_p_number(const std::string& input, int p) {
  size_t dot = input.find('.');
  if (dot == std::string::npos) {
    BigInteger I = integerFromBase(input, p);
    return BigFraction(I);
  }

  std::string integer = input.substr(0, dot);
  std::string fraction = input.substr(dot + 1);

  BigInteger I = integerFromBase(integer, p);
  BigInteger F = fractionalFromBase(fraction, p);
  int k = fraction.size();

  BigInteger p_power(1);
  for (int i = 0; i < k; ++i) p_power = p_power * p;

  BigInteger num = I * p_power + F;
  BigInteger den = p_power;

  return BigFraction(num, den);
}
