#pragma once
#include <string>
#include "bigfrac.h"

const size_t MAX_OUTPUT_LENGTH = 100000;

struct FractionToBaseResult {
  std::string integer_part;
  std::string fraction_part;
  std::string period_part;
  bool has_period = false;
  bool truncated = false;
};

std::string digitToBaseString(int digit);
std::string integerToBaseString(const BigInteger& value, int base);
FractionToBaseResult fractionToBase(const BigFraction& value, int base, std::size_t precision_limit);
std::string numberToBaseString(const BigFraction& value, int base);
