#ifndef COMMON_H
#define COMMON_H

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Do a explicit (full) template specialization for pair<int, int>.
template <> struct std::hash<std::pair<int, int>>
{
  size_t operator()(std::pair<int, int> k) const throw()
  {
    return std::hash<int>()(k.first) * 31 + std::hash<int>()(k.second);
  }
};

// TODO: move to a more proper place.
enum Direction
{
  kUP,
  kDOWN,
  kLEFT,
  kRIGHT,
  kNONE
};

// Ref: http://stackoverflow.com/a/33099753
inline std::string &trim(std::string &s)
{
  const std::string expr = " \f\n\r\t\v";
  s.erase(0, s.find_first_not_of(expr));
  s.erase(s.find_last_not_of(expr) + 1);
  return s;
}

// Return a copy with uppercase, do not change the original string.
// Ref: http://stackoverflow.com/a/735215
inline std::string to_upper(const std::string &str)
{
  std::string upperStr = str;
  std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
  return upperStr;
}

// Ref: http://stackoverflow.com/a/236803
// Should not really be inline due to overhead, for details
// http://stackoverflow.com/a/13190476
// TODO: remove inline, only done because have defined it in the header.
inline std::vector<std::string> split(const std::string &s, char delimeter)
{
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elements;
  // Inline may handle some simple loops, but the decision is made by the
  // compiler depending on complexity.
  while (std::getline(ss, item, delimeter))
    elements.push_back(item);
  return elements;
}

#endif
