#include "parser.h"
#include <stdexcept>
#include <cctype>
#include <vector>
#include <algorithm>

static int charToDigit(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'Z') return 10 + (c - 'A');
  if (c >= 'a' && c <= 'z') return 10 + (c - 'a');
  return -1;
}

static void validateNumberInBrackets(const std::string& numStr) {
  if (numStr.empty()) {
    throw std::invalid_argument("Пустые квадратные скобки [] недопустимы");
  }
  for (char c : numStr) {
    if (!std::isdigit(c)) {
      throw std::invalid_argument("В квадратных скобках допускаются только цифры");
    }
  }
}

static int parseDigit(const std::string& s, size_t& pos) {
  if (pos >= s.size()) {
    throw std::invalid_argument("Неожиданный конец строки");
  }

  if (s[pos] == '[') {
    pos++;
    size_t end = s.find(']', pos);
    if (end == std::string::npos) {
      throw std::invalid_argument("Незакрытая квадратная скобка");
    }
    std::string numStr = s.substr(pos, end - pos);
    pos = end + 1;
    validateNumberInBrackets(numStr);
    return std::stoi(numStr);
  }

  char c = s[pos];
  int digit = charToDigit(c);
  if (digit < 0) {
    throw std::invalid_argument("Недопустимая цифра: '" + std::string(1, c) + "'");
  }
  pos++;
  return digit;
}

static void validateDigits(const std::string& s, int base) {
  size_t pos = 0;
  while (pos < s.size()) {
    int digit = parseDigit(s, pos);
    if (digit >= base) {
      throw std::invalid_argument("Цифра " + std::to_string(digit) +
                                  " недопустима в системе с основанием " + std::to_string(base));
    }
  }
}

static BigInteger parseBaseInteger(const std::string& s, int base) {
  if (s.empty()) return BigInteger(0);
  size_t pos = 0;
  BigInteger result(0);
  BigInteger b(base);
  while (pos < s.size()) {
    int digit = parseDigit(s, pos);
    result = result * b + BigInteger(digit);
  }
  return result;
}

BigFraction parseBaseNumber(const std::string& input, int base) {
  if (base < 2 || base > 500) {
    throw std::invalid_argument("Основание должно быть от 2 до 500");
  }

  std::string s = input;
  s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());

  if (s.empty()) {
    throw std::invalid_argument("Входная строка пуста");
  }

         // Проверки формата
  if (s[0] == '.') {
    throw std::invalid_argument("Отсутствует целая часть");
  }
  if (s.back() == '.') {
    throw std::invalid_argument("Отсутствует дробная часть после точки");
  }
  if (std::count(s.begin(), s.end(), '.') > 1) {
    throw std::invalid_argument("Более одной точки");
  }

  size_t dot = s.find('.');
  size_t open = s.find('(');
  size_t close = s.rfind(')');

         // Проверка скобок
  if (open != std::string::npos) {
    if (close == std::string::npos) {
      throw std::invalid_argument("Незакрытая круглая скобка");
    }
    if (dot == std::string::npos) {
      throw std::invalid_argument("Период может быть только в дробной части");
    }
    if (open < dot) {
      throw std::invalid_argument("Скобка периода должна быть после точки");
    }
    if (close + 1 < s.size()) {
      throw std::invalid_argument("Символы после закрывающей скобки периода недопустимы");
    }
  }

  std::string intPartStr = (dot == std::string::npos) ? s : s.substr(0, dot);
  std::string fracPart = (dot == std::string::npos) ? "" : s.substr(dot + 1);

  if (dot != std::string::npos && fracPart.empty()) {
    throw std::invalid_argument("После точки нет цифр");
  }

  std::string nonRep;
  std::string rep;

  if (open == std::string::npos) {
    nonRep = fracPart;
  } else {
    size_t relOpen = open - (dot + 1);
    if (relOpen > fracPart.size()) {
      throw std::invalid_argument("Некорректная позиция скобки");
    }
    nonRep = fracPart.substr(0, relOpen);
    rep = fracPart.substr(relOpen + 1, close - open - 1);
    if (rep.empty()) {
      throw std::invalid_argument("Период не может быть пустым");
    }
  }

         // Валидация цифр
  if (!intPartStr.empty()) validateDigits(intPartStr, base);
  if (!nonRep.empty()) validateDigits(nonRep, base);
  if (!rep.empty()) validateDigits(rep, base);

  BigInteger I = parseBaseInteger(intPartStr, base);
  BigInteger b(base);

  auto getDigits = [&](const std::string& str) -> std::vector<int> {
    size_t pos = 0;
    std::vector<int> digits;
    while (pos < str.size()) {
      digits.push_back(parseDigit(str, pos));
    }
    return digits;
  };

  if (rep.empty()) {
    if (nonRep.empty()) {
      return BigFraction(I);
    }
    std::vector<int> digits = getDigits(nonRep);
    BigInteger fracValue(0);
    for (int digit : digits) {
      fracValue = fracValue * b + BigInteger(digit);
    }
    BigInteger pow(1);
    for (size_t i = 0; i < digits.size(); ++i) {
      pow = pow * b;
    }
    BigInteger num = I * pow + fracValue;
    return BigFraction(num, pow);
  }

         // Периодическая дробь
  std::vector<int> nonRepDigits = getDigits(nonRep);
  std::vector<int> repDigits = getDigits(rep);

  BigInteger nonRepValue(0);
  for (int digit : nonRepDigits) {
    nonRepValue = nonRepValue * b + BigInteger(digit);
  }

  BigInteger repValue(0);
  for (int digit : repDigits) {
    repValue = repValue * b + BigInteger(digit);
  }

  BigInteger powNonRep(1);
  for (size_t i = 0; i < nonRepDigits.size(); ++i) {
    powNonRep = powNonRep * b;
  }

  BigInteger powRep(1);
  for (size_t i = 0; i < repDigits.size(); ++i) {
    powRep = powRep * b;
  }

  BigInteger repDen = powRep - BigInteger(1);
  BigInteger num = I * powNonRep * repDen + nonRepValue * repDen + repValue;
  BigInteger den = powNonRep * repDen;

  return BigFraction(num, den);
}
