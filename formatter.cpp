#include "formatter.h"
#include <map>
#include <stdexcept>
#include <algorithm>
#include <sstream>

std::string digitToBaseString(int digit) {
  if (digit < 0) {
    throw std::invalid_argument("Negative digit");
  }
  if (digit < 10) {
    return std::string(1, char('0' + digit));
  }
  if (digit < 36) {
    return std::string(1, char('A' + digit - 10));
  }
  return "[" + std::to_string(digit) + "]";
}

std::string integerToBaseString(const BigInteger& value, int base) {
  if (base < 2) {
    throw std::invalid_argument("Base must be >= 2");
  }

  if (value.isZero()) {
    return "0";
  }

  BigInteger cur = value;
  BigInteger b(base);
  std::vector<int> digits;

  while (!cur.isZero()) {
    BigInteger rem = cur % b;
    int digit = std::stoi(rem.toString());
    digits.push_back(digit);
    cur = cur / b;
  }

  std::string res;
  for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
    res += digitToBaseString(*it);
  }
  return res;
}

FractionToBaseResult fractionToBase(const BigFraction& value, int base, std::size_t precision_limit) {
  if (base < 2) {
    throw std::invalid_argument("Base must be >= 2");
  }

  FractionToBaseResult result;
  result.integer_part = integerToBaseString(value.integerPart(), base);

  BigFraction frac = value.remainder();

  if (frac.num().isZero()) {
    return result;
  }

  std::map<BigInteger, std::size_t> seen;
  std::vector<int> fraction_digits;

  BigInteger rem = frac.num();
  BigInteger den = frac.den();
  BigInteger b(base);

  while (!rem.isZero() && fraction_digits.size() < precision_limit) {
    auto it = seen.find(rem);
    if (it != seen.end()) {
      result.has_period = true;
      size_t period_start = it->second;

      for (size_t i = 0; i < period_start && i < fraction_digits.size(); ++i) {
        result.fraction_part += digitToBaseString(fraction_digits[i]);
      }
      for (size_t i = period_start; i < fraction_digits.size(); ++i) {
        result.period_part += digitToBaseString(fraction_digits[i]);
      }
      return result;
    }

    seen[rem] = fraction_digits.size();
    rem = rem * b;

    BigInteger digitBig = rem / den;
    int digit = std::stoi(digitBig.toString());
    rem = rem % den;

    fraction_digits.push_back(digit);

           // Проверка на превышение лимита
    if (result.fraction_part.length() + result.period_part.length() >= MAX_OUTPUT_LENGTH) {
      result.truncated = true;
      break;
    }
  }

  for (int d : fraction_digits) {
    result.fraction_part += digitToBaseString(d);
  }

  return result;
}

std::string numberToBaseString(const BigFraction& value, int base) {
  FractionToBaseResult r = fractionToBase(value, base, MAX_OUTPUT_LENGTH);

  std::string out = r.integer_part;

  if (!r.fraction_part.empty() || r.has_period) {
    out += ".";

    size_t max_len = MAX_OUTPUT_LENGTH - out.length();
    if (r.fraction_part.length() > max_len) {
      r.fraction_part = r.fraction_part.substr(0, max_len);
      out += r.fraction_part + "...";
      out += "\n\n[ВНИМАНИЕ] Полная запись результата слишком длинная. ";
      out += "Показаны первые " + std::to_string(MAX_OUTPUT_LENGTH) + " символов.";
      return out;
    }

    out += r.fraction_part;

    if (r.has_period && !r.period_part.empty()) {
      out += "(";
      if (out.length() + r.period_part.length() > MAX_OUTPUT_LENGTH) {
        size_t remaining = MAX_OUTPUT_LENGTH - out.length();
        out += r.period_part.substr(0, remaining) + "...)";
        out += "\n\n[ВНИМАНИЕ] Полная запись результата слишком длинная.";
      } else {
        out += r.period_part + ")";
      }
    }
  }

  if (r.truncated && out.length() < MAX_OUTPUT_LENGTH) {
    out += "\n\n[ВНИМАНИЕ] Полная запись результата слишком длинная.";
  }

  return out;
}
