// NextGreaterInteger.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>     // std::string, std::to_string

/*
Question: Given an integer N, returns the smallest integer greater than N that does not contain two identical consecutive digits

Examples :

N = 55, Answer : 56
N = 1765 ,Answer: 1767
N = 44432, Answer: 45010
N = 9, Answer: 10
N = 98, Answer: 101
N = 1998, Answer: 2010 <-- most tricky
N = 123998, Answer: 124010
Constraints:

1 <= N <= 1000,000,000
*/

std::string buildRestString( int aDigitCount )
{
    std::string result;

    bool even = false;
    while( aDigitCount > 0 )
    {
        if( even )
        {
            result += '1';
        }
        else
        {
            result += '0';
        }
        even = !even;
        aDigitCount--;
    }

    return result;
}

//! Idea:
//! Firstly, we should add 1 because we want a greater number.
//! Then, deal with the issue of "no identical consecutive digits".
//! Scan from the initial, if found two identical digits, add 1 to the 2nd one (need to consider 9).
//! For the rest rear digits, we should make them be 01010...
int getSmallestGreaterThan( int aNumber )
{
    using namespace std;
    string theNumberString = to_string( aNumber + 1 );

    for( int i = 1; i < theNumberString.length(); ++i )
    {
        if( theNumberString[i] == theNumberString[i - 1] )
        {
            if( theNumberString[i] == '9' )
            {
                // We are now at the fist 'xyz99' digits pair, so 'z' must not be 9.
                // 'xyz9' + 1 must become xy(z+1)0. We need to consider the case: y = z + 1.
                // We can handle it by calling this function recursively for 'xyz9'.
                int firstPartNumber = std::stoi( theNumberString.substr( 0, i ) );
                string firstPart = to_string( getSmallestGreaterThan( firstPartNumber ) );

                theNumberString = firstPart + "1" + buildRestString( theNumberString.length() - i - 1 );
            }
            else
            {
                int digit = theNumberString[i] - '0' + 1;
                theNumberString = theNumberString.substr( 0, i ) + to_string( digit ) + buildRestString( theNumberString.length() - i - 1 );
                break;
            }
        }
    }

    return std::stoi( theNumberString );
}

int main()
{
    std::cout << "NextGreaterInteger for 55: " << getSmallestGreaterThan( 55 ) << "\n";
    std::cout << "NextGreaterInteger for 1765: " << getSmallestGreaterThan( 1765 ) << "\n";
    std::cout << "NextGreaterInteger for 44432: " << getSmallestGreaterThan( 44432 ) << "\n";
    std::cout << "NextGreaterInteger for 99: " << getSmallestGreaterThan( 99 ) << "\n";
    std::cout << "NextGreaterInteger for 98: " << getSmallestGreaterThan( 98 ) << "\n";
    std::cout << "NextGreaterInteger for 1998: " << getSmallestGreaterThan( 1998 ) << "\n";
    std::cout << "NextGreaterInteger for 132998: " << getSmallestGreaterThan( 132998 ) << "\n";
    std::cout << "NextGreaterInteger for 9900: " << getSmallestGreaterThan( 9900 ) << "\n";
}
