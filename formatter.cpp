#include "formatter.h"

std::string bigDigitToQChar(int d) {
  if (d < 10) return std::string(1, '0' + d);
  if (d < 36) return std::string(1, 'A' + (d - 10));
  return "[" + std::to_string(d) + "]";
}

std::string integerToBaseQ(const BigInteger& integer, int q) {
  if (integer.isZero()) return "0";
  BigInteger x = integer;
  BigInteger base(q);
  std::string result;
  while (!x.isZero()) {
    BigInteger rem = x % base;
    int r = std::stoi(rem.toString());
    result += bigDigitToQChar(r);
    x = x / base;
  }
  std::reverse(result.begin(), result.end());
  return result;
}
