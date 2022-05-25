// Math.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <algorithm> // For min().
#include <cstdlib> // For abs().

//-----------------------------------------------------------------------------------
// 1344. Angle Between Hands of a Clock
//
// Hints:
// * The tricky part is determining how the minute hand affects the position of the
// hour hand.
// * Calculate the angles separately then find the difference.
//-----------------------------------------------------------------------------------

double angleClock(int hour, int minutes)
{
    // For minute hand, one small scale is one minute and
    // one small scale is 6 degree.
    double minuteHandAngle = minutes * 6;
    // For minute hand, one big scale is one hour and
    // one big scale is 30 degree.
    // Now calculate the affection minutes do to hourHandAnggle.

    // 1 <= hour <= 12
    hour = hour == 12 ? 0 : hour;
    double hourHandAnggle = hour * 30.0 + ( minutes / 60.0 ) * 30.0;
    double diff = abs(minuteHandAngle - hourHandAnggle);
    return std::min(diff, 360 - diff);
}

int main()
{
    std::cout << "Math!\n";

    // 1344. Angle Between Hands of a Clock
    // Input: hour = 12, minutes = 30
    // Output : 165
    std::cout << "Result of Angle Between Hands of a Clock: " << angleClock(3, 15) << std::endl;
    std::cout << "\n";
}
