#include "format.h"

#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = seconds % 60;

  // Pad with 0 manually
  std::string h = (hours < 10 ? "0" : "") + std::to_string(hours);
  std::string m = (minutes < 10 ? "0" : "") + std::to_string(minutes);
  std::string s = (secs < 10 ? "0" : "") + std::to_string(secs);

  return (h + ":" + m + ":" + s);
}