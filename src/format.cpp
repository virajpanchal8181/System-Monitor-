#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {     
    long hour = 0;
    long min = 0;
    long sec = 0;
    
    hour = seconds / 3600;
    seconds = seconds % 3600;
    min = seconds / 60;  
   
    seconds = seconds % 60;
    sec = seconds;

    string output = std::to_string(hour) + ": " +  std::to_string(min) + ": " + std::to_string(sec);

    return output; 
  
  }