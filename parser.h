#pragma once
#include "bigfrac.h"
#include <string>

BigFraction parse_p_number(const std::string& input, int p);
BigInteger integerFromBase(const std::string& integer, int p);      // целая часть в p
BigInteger fractionalFromBase(const std::string& fraction, int p);  // дробная часть в p
