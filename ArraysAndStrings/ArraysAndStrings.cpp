// ArraysAndStrings.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "LeetCodeUtil.h"

#include <stack>
#include <string>
#include <algorithm>
#include <map>
#include <queue>
#include <list>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <numeric> // for accumulate
#include <cctype> // for toupper / tolower / isupper / islower

namespace ArraysAndStrings
{
    using namespace std;

    //-----------------------------------------------------------------------------------
    // 151. Reverse Words in a String
    // Given an input string s, reverse the order of the words.
    //
    // Note that s may contain leading or trailing spaces or multiple spaces between two
    // words. The returned string should only have a single space separating the words.
    // Do not include any extra spaces.
    //
    // Follow-up: If the string data type is mutable in your language, can you solve it
    // in-place with O(1) extra space?
    //-----------------------------------------------------------------------------------
    string reverseWords(string s)
    {
        // If we have trim() and split() method like Java, this will be easier.
        // To achieve in-place is more difficult.

        // Step 1. Reverse entire string,
        // Step 2. Scan string from left to right. Reverse each word and deal with extra spaces at the same time.
        // Step 3. Resize the string because it might shrink.

        std::reverse(s.begin(), s.end());

        // The position we are currently writing at.
        int cursor = 0;
        const size_t len = s.length();

        for (int i = 0; i < len; ++i)
        {
            if (s[i] != ' ')
            {
                // Not the first word. Put a space between each word.
                if (cursor != 0)
                {
                    s[cursor] = ' ';
                    cursor++;
                }

                // Find the range of a whole word.
                int j = i;
                while (s[j] != ' ' && j < len)
                {
                    s[cursor] = s[j]; // This is fine because cursor is always less than j.
                    cursor++;
                    j++;
                }

                reverse(s.begin() + cursor - ( j - i ), s.begin() + cursor);
                i = j;
            }
        }

        s.resize(cursor);
        return s;
    }

    //-----------------------------------------------------------------------------------
    // 186. Reverse Words in a String II
    // Given a character array s, reverse the order of the words.
    // Your code must solve the problem in - place, i.e.without allocating extra space.
    //-----------------------------------------------------------------------------------

    template<typename T>
    void reverseWordsII(vector<T>& s)
    {
        reverse(s.begin(), s.end());

        const size_t len = s.size();
        int stringBegin = 0;
        for (int i = 0; i <= len; ++i)
        {
            if (i == len || s[i] == " ")
            {
                reverse(s.begin() + stringBegin, s.begin() + i);
                stringBegin = i + 1;
            }
        }
    }

    //-----------------------------------------------------------------------------------
    // 54. Spiral Matrix
    //
    // m == matrix.length
    // n == matrix[i].length
    // 1 <= m, n <= 10
    // - 100 <= matrix[i][j] <= 100
    //
    // Solution 1: Define and adjust boundaries.
    // Solution 2: Set the visited cell to invalid.
    //-----------------------------------------------------------------------------------

    // Solution 1
    vector<int> spiralOrder(vector<vector<int>>& matrix)
    {
        const size_t m = matrix.size();
        const size_t n = matrix[0].size();

        int boundaryLeft = 0;
        int boundaryRight = n - 1;
        int boundaryTop = 0;
        int boundaryBottom = m - 1;

        int currentIndex = 0;
        vector<int> result( m * n, 0 );
        while (true)
        {
            // Scan toward right.
            for (int i = boundaryLeft; i <= boundaryRight; ++i)
            {
                result[currentIndex] = matrix[boundaryTop][i];
                currentIndex++;
            }

            boundaryTop++;
            if (boundaryTop > boundaryBottom)
            {
                break;
            }

            // Scan toward down.
            for (int i = boundaryTop; i <= boundaryBottom; ++i)
            {
                result[currentIndex] = matrix[i][boundaryRight];
                currentIndex++;
            }

            boundaryRight--;
            if (boundaryRight < boundaryLeft)
            {
                break;
            }

            // Scan toward right.
            for (int i = boundaryRight; i >= boundaryLeft; --i)
            {
                result[currentIndex] = matrix[boundaryBottom][i];
                currentIndex++;
            }

            boundaryBottom--;
            if (boundaryBottom < boundaryTop)
            {
                break;
            }

            // Scan toward top.
            for (int i = boundaryBottom; i >= boundaryTop; --i)
            {
                result[currentIndex] = matrix[i][boundaryLeft];
                currentIndex++;
            }

            boundaryLeft++;
            if (boundaryLeft > boundaryRight)
            {
                break;
            }
        }

        return result;
    }

    //-----------------------------------------------------------------------------------
    // 1405. Longest Happy String
    //
    // Solution: Greedy
    //-----------------------------------------------------------------------------------

    const static int maxConsecutiveConstraint = 2;

    class CharCount
    {
    public:
        int m_count;
        int m_consecutiveUsedCount;
        char m_char;

        CharCount(char c, int count)
            : m_char(c)
            , m_count(count)
            , m_consecutiveUsedCount(0)
        {}

        bool operator<(CharCount& right) const
        {
            return m_count > right.m_count;
        }
    };

    inline void resetConsecutiveUsableCount(vector<CharCount>& charQueue)
    {
        for (auto& charObj : charQueue)
        {
            charObj.m_consecutiveUsedCount = 0;
        }
    }

    string longestDiverseString(int a, int b, int c)
    {
        int totalCount = a + b + c;

        vector<CharCount> charQueue;
        if (a > 0)
        {
            charQueue.push_back(CharCount('a', a));
        }
        if (b > 0)
        {
            charQueue.push_back(CharCount('b', b));
        }
        if (c > 0)
        {
            charQueue.push_back(CharCount('c', c));
        }


        string result;
        for (; totalCount > 0; --totalCount)
        {
            sort(charQueue.begin(), charQueue.end());

            // Check the maximum consecutive constraint.
            int i = 0;
            for (; i < charQueue.size(); ++i)
            {
                if (charQueue[i].m_consecutiveUsedCount < maxConsecutiveConstraint &&
                    charQueue[i].m_count > 0 )
                {
                    break;
                }
            }

            if (i == charQueue.size())
            {
                // No any char can be used.
                break;
            }

            auto& nextChar = charQueue[i];
            result.push_back(nextChar.m_char);
            nextChar.m_count--;

            // If this char cannot be used anymore, remove it.
            // It should make sort() run faster.
            if (nextChar.m_count == 0)
            {
                charQueue.erase(charQueue.begin() + i);
                resetConsecutiveUsableCount(charQueue);
            }
            else
            {
                int oldUsedCount = nextChar.m_consecutiveUsedCount;
                resetConsecutiveUsableCount(charQueue);
                nextChar.m_consecutiveUsedCount = oldUsedCount + 1;
            }
        }

        return result;
    }

    //-----------------------------------------------------------------------------------
    // 1386. Cinema Seat Allocation
    //
    // Seat:
    // 1 2 3 || 4 5 6 7 || 8 9 10
    // || stands for aisle.
    //
    // A four-person group occupies four adjacent seats in one single row. Seats across
    // an aisle (such as [3,3] and [3,4]) are not considered to be adjacent, but there
    // is an exceptional case on which an aisle split a four-person group, in that case,
    // the aisle split a four-person group in the middle, which means to have two
    // people on each side.
    //
    // So, 1234 doesn't count. Seat1 is irrelevant.
    //
    // I got Memory Limit Exceeded when I use a vector of n integer.
    //-----------------------------------------------------------------------------------

    class Solution1386
    {
    public:

        enum SeatInRow
        {
            Seat1  = 0b0000000001,
            Seat2  = 0b0000000010,
            Seat3  = 0b0000000100,
            Seat4  = 0b0000001000,
            Seat5  = 0b0000010000,
            Seat6  = 0b0000100000,
            Seat7  = 0b0001000000,
            Seat8  = 0b0010000000,
            Seat9  = 0b0100000000,
            Seat10 = 0b1000000000,
        };

        int maxNumberOfFamilies(int n, vector<vector<int>>& reservedSeats)
        {
            sort(reservedSeats.begin(), reservedSeats.end());

            int rowState = 0;
            int sum = 0;
            int hasReservedRowCount = 0;

            for (int i = 0; i < reservedSeats.size(); ++i)
            {
                const vector<int>& seat = reservedSeats[i];
                const int rowNumber = seat[0];

                rowState |= 1 << ( seat[1] - 1 );

                if (i == reservedSeats.size() - 1 || reservedSeats[i + 1][0] != rowNumber)
                {
                    hasReservedRowCount++;
                    // Need to check whether 2345 and 6789 is empty.
                    const int bitmask2345 = SeatInRow::Seat2 | SeatInRow::Seat3 | SeatInRow::Seat4 | SeatInRow::Seat5;
                    const int bitmask6789 = SeatInRow::Seat6 | SeatInRow::Seat7 | SeatInRow::Seat8 | SeatInRow::Seat9;
                    // If the both above are not available, check 4567.
                    const int bitmask4567 = SeatInRow::Seat4 | SeatInRow::Seat5 | SeatInRow::Seat6 | SeatInRow::Seat7;

                    int count = 0;
                    if (( rowState & bitmask2345 ) == 0)
                    {
                        count++;
                    }
                    if (( rowState & bitmask6789 ) == 0)
                    {
                        count++;
                    }

                    if (count == 0 && ( rowState & bitmask4567 ) == 0)
                    {
                        count++;
                    }

                    sum += count;

                    rowState = 0;
                }
            }

            sum += ( n - hasReservedRowCount ) * 2;

            return sum;
        }
    };

    //-----------------------------------------------------------------------------------
    // 1647. Minimum Deletions to Make Character Frequencies Unique
    //
    // Solution: Greedy.
    //
    // Collect all frequencies and sort then.
    // Iterate from greatest to least. Decrease each frequency if we need to.
    // Alternatively, we can use std::set to check if a frequency already exists.
    //
    // The answer is the difference between s.length() - sum of all frequencies.
    //-----------------------------------------------------------------------------------

    int minDeletions(string s)
    {
        vector<int> frequencies(26, 0);
        for (char c : s)
        {
            frequencies[c - 'a']++;
        }

        sort(frequencies.begin(), frequencies.end());

        // Using std::set is convenient but seems not fast.
        set<int> allowedFrequencies;
        int result = 0;
        for (int i = 25; i >= 0; i--)
        {
            if (frequencies[i] == 0)
            {
                break;
            }

            while (frequencies[i] != 0 && allowedFrequencies.find(frequencies[i]) != allowedFrequencies.end())
            {
                result++;
                frequencies[i]--;
            }
            allowedFrequencies.insert(frequencies[i]);
        }

        return result;
    }

    //-----------------------------------------------------------------------------------
    // 1653. Minimum Deletions to Make String Balanced
    //
    // It is said that there are at least 2 ways to solve this.
    // 1. Stack
    // 2. Dynamic programming
    // None of them is easy to understand.
    //
    //-----------------------------------------------------------------------------------

    // Dynamic programming
    // Idea: For a certain b at position i. We can delete all 'a' after it, or we can
    // delete all 'b' before it. Both ways can make a balanced string. They may requires
    // different steps. The min of them is the answer.
    // "aababbab"
    //    ^
    int minimumDeletions(string s)
    {
        int aAfter = 0;
        for (auto c : s)
        {
            if (c == 'a')
            {
                aAfter++;
            }
        }
        int bBefore = 0;
        // Initial to count of b. Meaning we could need to delete all b.
        int result = s.size() - aAfter;

        for (int i = 0; i < s.size(); ++i)
        {
            if (s[i] == 'a')
            {
                aAfter--;
            }
            else
            {
                result = min(result, aAfter + bBefore);
                bBefore++;
            }
        }

        return result;
    }

    // Stack solution is ever harder to understand.
    // We make a stack that only store 'b'. When encounter 'a', we pop one b out without pushing 'a'.
    // However, it doesn't mean we delete one 'b' from the string.
    int minimumDeletions_stack(string s)
    {
        stack<int> bStore;
        int result = 0;

        for (auto c : s)
        {
            if (c == 'b')
            {
                bStore.push(c);
            }
            else
            {
                if (c == 'a' && !bStore.empty() && bStore.top() == 'b')
                {
                    bStore.pop();
                    result++;
                }
            }
        }

        return result;
    }

    //-----------------------------------------------------------------------------------
    // 1578. Minimum Time to Make Rope Colorful
    //
    // Input: colors = "aabaa", neededTime = [1,2,3,4,1]
    // Output: 2
    //-----------------------------------------------------------------------------------
    int minCost(string colors, vector<int>& neededTime)
    {
        int sum = 0;
        int i = 0;
        while( i < colors.size() - 1 )
        {
            int j = i + 1;

            if (colors[i] != colors[j])
            {
                i++;
                continue;
            }

            while (colors[i] == colors[j])
            {
                j++;
            }

            int maxTime = 0;
            for (int k = i; k < j; ++k)
            {
                maxTime = max(maxTime, neededTime[k]);
                sum += neededTime[k];
            }
            sum -= maxTime;

            i = j;
        }

        return sum;
    }

    //-----------------------------------------------------------------------------------
    // 1275. Find Winner on a Tic Tac Toe Game
    //-----------------------------------------------------------------------------------

    string tictactoe(vector<vector<int>>& moves)
    {
        vector<int> rows(3, 0);
        vector<int> columns(3, 0);
        int diagonal = 0;
        int antidiagonal = 0;

        int player = 1;
        for (const auto& move : moves)
        {
            int const row = move[0];
            int const col = move[1];

            rows[row] += player;
            columns[col] += player;

            if (row == col)
            {
                diagonal += player;
            }
            if (row == 3 - col - 1)
            {
                antidiagonal += player;
            }

            player *= -1;

            if (3 == abs(rows[row]) || 3 == abs(columns[col]) || 3 == abs(diagonal) || 3 == abs(antidiagonal))
            {
                return player == 1 ? "A" : "B";
            }
        }

        return moves.size() == 9 ? "Draw" : "Pending";
    }

    //-----------------------------------------------------------------------------------
    // 1267. Count Servers that Communicate
    //
    // To avoid reduplication, we should increment 1 for each sever.
    // We don't need to know the position of the another server. So, we just need to
    // count the number of servers in each row/column.
    //-----------------------------------------------------------------------------------
    int countServers(vector<vector<int>>& grid)
    {
        const int rowCount = grid.size();
        const int columnCount = grid[0].size();
        unordered_map<int, int> serverCountInRow;
        unordered_map<int, int> serverCountInColumn;

        for (int i = 0; i < rowCount; ++i)
        {
            for (int j = 0; j < columnCount; ++j)
            {
                if (grid[i][j] == 1)
                {
                    serverCountInRow[i]++;
                    serverCountInColumn[j]++;
                }
            }
        }

        int result = 0;
        for (int i = 0; i < rowCount; ++i)
        {
            for (int j = 0; j < columnCount; ++j)
            {
                if (grid[i][j] == 1 && (serverCountInRow[i] > 1 || serverCountInColumn[j] > 1) )
                {
                    result++;
                }
            }
        }

        return result;
    }

    //-----------------------------------------------------------------------------------
    // 394. Decode String
    //
    // Can be solved by stack or recursion (for the string within []).
    // Input: s = "3[a2[c]]"
    // Output: "accaccacc"
    //-----------------------------------------------------------------------------------
    string decodeString(string s)
    {
        stack<int> numbers;
        stack<string> strings;

        string tempString;
        int tempCount = 0;
        int i = 0;
        while (i < s.size())
        {
            if (s[i] >= '0' && s[i] <= '9')
            {
                tempCount = tempCount * 10 + (s[i] - '0');
            }
            else if (s[i] == '[')
            {
                numbers.push(tempCount);
                tempCount = 0;
                strings.push(tempString);
                tempString.clear();
            }
            else if (s[i] == ']')
            {
                int count = numbers.top();
                numbers.pop();

                string topString;
                if (!strings.empty())
                {
                    topString = strings.top();
                    strings.pop();
                }

                while (count > 0)
                {
                    topString += tempString;
                    count--;
                }
                tempString = topString;
            }
            else
            {
                tempString.push_back(s[i]);
            }

            i++;
        }

        return tempString;
    }

    //-----------------------------------------------------------------------------------
    // 1775. Equal Sum Arrays With Minimum Number of Operations
    // Greedy, counting, hash table.
    //
    // Time: O(m + n) while m is the size of nums1 and n i the size of nums2
    // Space: O(1)
    //-----------------------------------------------------------------------------------
    int minOperations(vector<int>& nums1, vector<int>& nums2)
    {
        // Deal with the situation of impossible.
        // Try to maximize the short list and minimize the long list. If the sum of short
        // list is still greater than the sum of long list, it is impossible to find a
        // solution.
        const int len1 = nums1.size();
        const int len2 = nums2.size();
        if (len1 * 6 < len2 || len1 > len2 * 6)
        {
            return -1;
        }

        int sum1 = std::accumulate(nums1.begin(), nums1.end(), 0);
        int sum2 = std::accumulate(nums2.begin(), nums2.end(), 0);

        if (sum1 == sum2)
        {
            return 0;
        }

        // We need to handle the great/small list separately.
        const vector<int>& greatList = sum1 > sum2 ? nums1 : nums2;
        const vector<int>& smallList = sum1 <= sum2 ? nums1 : nums2;

        // Let sum2 greater.
        if (sum1 > sum2)
        {
            swap(sum1, sum2);
        }

        // Calculate the power of change.
        // Every element can be changed to 1~6. The power of change is the value of that
        // change can contribute. For example, changing 1 to 6, the power of change is
        // 6 - 1 = 5, changing 1 to 4, the value is 3.
        // We need to collect all powers within the two lists.
        // The range of power is 0~5.
        vector<uint32_t> powerOfChange(6);

        // Collect power of change from greatList.
        for (const auto& i : greatList)
        {
            powerOfChange[i - 1]++;
        }
        // Collect power of change from smallList.
        for (const auto& i : smallList)
        {
            powerOfChange[6 - i]++;
        }

        int steps = 0;
        // Use power of change to offset the difference between two lists.
        for (int p = 5; p > 0 && sum1 < sum2;)
        {
            // If there is no power at p, try to find smaller powers.
            if (powerOfChange[p] == 0)
            {
                p--;
            }
            // Use one power, so decrease the number of power by 1.
            sum1 += p;
            powerOfChange[p]--;
            steps++;
        }

        return steps;
    }

    //-----------------------------------------------------------------------------------
    // 984. String Without AAA or BBB
    // Greedy
    //-----------------------------------------------------------------------------------

    inline void addChar(int* count, char c, string* output, bool justOne = false)
    {
        if (*count >= 2 && !justOne)
        {
            *count -= 2;
            output->push_back(c);
            output->push_back(c);
        }
        else if(*count > 0)
        {
            *count -= 1;
            output->push_back(c);
        }
    }

    string strWithout3a3b(int a, int b)
    {
        string result;
        while (a + b)
        {
            if (a > b)
            {
                addChar(&a, 'a', &result);
                addChar(&b, 'b', &result, true);
            }
            else if( a < b )
            {
                addChar(&b, 'b', &result);
                addChar(&a, 'a', &result, true);
            }
            else
            {
                if (!result.empty() && result.back() == 'a')
                {
                    while (a > 0)
                    {
                        result.append("ba");
                        a--;
                    }
                    break;
                }
                else
                {
                    while (b > 0)
                    {
                        result.append("ab");
                        b--;
                    }
                    break;
                }
            }
        }

        return result;
    }

    //-----------------------------------------------------------------------------------
    // 153. Find Minimum in Rotated Sorted Array (Medium)
    // You must write an algorithm that runs in O(log n) time.
    // Topic: binary search.
    //
    // Related question:
    // 33. Search in Rotated Sorted Array (Medium)
    // 81. Search in Rotated Sorted Array II (Medium) - Allow duplicates
    // 154. Find Minimum in Rotated Sorted Array II (Hard) - Allow duplicates
    //-----------------------------------------------------------------------------------

    // Using iterative binary search.
    int findMin(vector<int>& nums)
    {
        // Seeing O(log n), binary search instantly comes to our mind.
        // To use binary search, we need to know which part (left or right) to continue.
        // Observation:
        // 0 1 2 4 5 6 7
        // 7 0 1 2 4 5 6
        // 6 7 0 1 2 4 5
        // 5 6 7 0 1 2 4
        // 4 5 6 7 0 1 2
        // 2 4 5 6 7 0 1
        // 1 2 4 5 6 7 0
        //       ^
        // As we can see, when mid is less than the right-most one, the minimum value
        // must be in the left side or in the middle. Otherwise it is in right side.
        int left = 0;
        int right = nums.size() - 1;

        while (left < right)
        {
            const int mid = left + ( right - left ) / 2;
            // Use <=, not <.
            if (nums[mid] <= nums[right])
            {
                right = mid;
            }
            else
            {
                left = mid + 1;
            }

        }
        return nums[right];
    }

    class Solution153
    {
    public:
        int findMin(vector<int>& nums)
        {
            return helper(nums, 0, (int)nums.size() - 1);
        }

        int helper(vector<int>& nums, int start, int end)
        {
            // This sub-list is sorted, return the smallest.
            if (nums[start] <= nums[end])
            {
                return nums[start];
            }

            const int mid = start + ( end - start ) / 2;

            return min(helper(nums, start, mid), helper(nums, mid + 1, end));
        }
    };

    //-----------------------------------------------------------------------------------
    // 1763. Longest Nice Substring (Easy)
    //
    // If we want to use Divide and Conquer, this is more than Easy.
    //-----------------------------------------------------------------------------------

    class Solution1763
    {
    public:

        string longestNiceSubstring(string s)
        {
            // If a character doesn't have its pair(upper or lower) character, the LNS must
            // appears on its left or right.
            const int len = s.size();

            // For each char, record whether this char's upper/lower both exists.
            unordered_map<char, bool> characterHash;
            for (int i = 0; i < s.size(); ++i)
            {
                const char c = s[i];
                unordered_map<char, bool>::iterator thisChar = characterHash.find(c);
                unordered_map<char, bool>::iterator pair =
                    isupper(c) ? characterHash.find(tolower(c)) : characterHash.find(toupper(c));

                if (thisChar != characterHash.end())
                {
                    // This char already exists.
                    if (pair != characterHash.end())
                    {
                        pair->second = true;
                        thisChar->second = true;
                    }
                }
                else
                {
                    if (pair != characterHash.end())
                    {
                        characterHash[c] = true;
                        pair->second = true;
                    }
                    else
                    {
                        characterHash[c] = false;
                    }
                }
            }

            char badChar = '0';
            for (const auto& pair : characterHash)
            {
                if (!pair.second)
                {
                    badChar = pair.first;
                }
            }
            int lastBadCharIndex = s.find_last_of(badChar);

            if (lastBadCharIndex == string::npos)
            {
                return s;
            }
            else
            {
                string sub1 = longestNiceSubstring(s.substr(0, lastBadCharIndex));
                string sub2 = lastBadCharIndex < len - 1 ? longestNiceSubstring(s.substr(lastBadCharIndex + 1)) : "";
                return sub1.length() >= sub2.length() ? sub1 : sub2;
            }
        }

    };

    //-----------------------------------------------------------------------------------
    // 1546. Maximum Number of Non-Overlapping Subarrays With Sum Equals Target
    // Topic: Greedy, prefix sum, hash table.
    //-----------------------------------------------------------------------------------
    int maxNonOverlapping(vector<int>& nums, int target)
    {
        // Prefix sum is an important concept in this question.
        // It accumulates the sum of all numbers before and including i.
        // For example, for array: [-1, 3, 5, 1, 4, 2, -9], we have prefix sum as follows.
        // [-1, 2, 7, 8, 12, 14, 5]
        // And the algorithm below work like this:
        // Iterate the array, when at i, if we found ( prefixSum[i] - 6 ) exists in the
        // prefixSum. We know we can exclude the elements that compose the
        // ( prefixSum[i] - 6 ).
        // For example, when we meet 8, 8 - 6= 2, which is at prefixSum[1].
        // So, we know nums[0~1] are not in the result array. The result array starts from
        // 2 and ends at 3, whrere we meet 8.
        unordered_map<int, int> prefixSum;

        prefixSum[0] = -1;
        int sum = 0;
        int res = 0;
        int last = -1; // The right boundary of the latest subarray.
        for (int i = 0; i < nums.size(); i++)
        {
            sum += nums[i];
            // At 3, 8 - 6 = 2. 2 exists and its index is 1. Set last to 3.
            // At 5, 14 6 - 8. 8 exists and its index is 3. 3 is not greater than 3, so it counts.
            if (prefixSum.find(sum - target) != prefixSum.end() && prefixSum[sum - target] >= last)
            {
                res++;
                last = i;
            }
            prefixSum[sum] = i;
        }
        return res;
    }

    //-----------------------------------------------------------------------------------
    // Test function
    //-----------------------------------------------------------------------------------
    void TestArraysAndStrings()
    {
        // Input: s = "the sky is blue"
        // Output: "blue is sky the"
        string input = " the   sky is blue ";
        cout << "Result of Reverse Words in a String: " << reverseWords(input) << endl;

        vector<string> charV = { "t","h","e"," ","s","k","y"," ","i","s"," ","b","l","u","e" };
        reverseWordsII(charV);
        cout << "Result of Reverse Words in a String II: " << endl;
        LeetCodeUtil::printVector(charV);
        cout << "\n\n";

        // Input: matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
        // Output : [1, 2, 3, 6, 9, 8, 7, 4, 5]
        vector<vector<int>> matrix = { {1, 2, 3} ,{4, 5, 6}, {7, 8, 9} };
        auto resultIntV = spiralOrder(matrix);
        cout << "Result of Spiral Matrix:" << endl;
        LeetCodeUtil::printVector(resultIntV);
        cout << "\n\n";

        // Input: matrix = [[1,2,3,4],[5,6,7,8],[9,10,11,12]]
        // Output: [1, 2, 3, 4, 8, 12, 11, 10, 9, 5, 6, 7]
        matrix = { {1,2,3,4} ,{5,6,7,8},{9,10,11,12} };
        resultIntV = spiralOrder(matrix);
        cout << "Result of Spiral Matrix:" << endl;
        LeetCodeUtil::printVector(resultIntV);
        cout << "\n\n";

        // 1405. Longest Happy String
        // Input: a = 1, b = 1, c = 7
        // Output: "ccaccbcc"
        cout << "Result of Longest Happy String: " << longestDiverseString(1, 1, 7) << endl;
        cout << "\n";

        // 1386. Cinema Seat Allocation
        // Input: n = 3, reservedSeats = [[1,2],[1,3],[1,8],[2,6],[3,1],[3,10]]
        // Output: 4
        //int n = 3;
        //vector<vector<int>> reservedSeats = { {1, 2} ,{1, 3}, {1, 8}, {2, 6}, {3, 1}, {3, 10} };
        // Input: n = 4, reservedSeats = [[4, 3], [1, 4], [4, 6], [1, 7]]
        // Output : 4
        int n = 4;
        vector<vector<int>> reservedSeats = { {4, 3} ,{1, 4}, {4, 6}, {1, 7} };
        Solution1386 s1386;
        cout << "Result of Cinema Seat Allocation: " << s1386.maxNumberOfFamilies(n, reservedSeats) << endl;
        cout << "\n";

        // 1647. Minimum Deletions to Make Character Frequencies Unique
        // Input: s = "aaabbbcc"
        // Output: 2
        // Input: s = "bbcebab"
        // Output: 2
        input = "bbcebab";
        cout << "Result of Minimum Deletions to Make Character Frequencies Unique: " << minDeletions(input) << endl;
        cout << "\n";

        // 1653. Minimum Deletions to Make String Balanced
        // Input: s = "aababbab"
        // Output: 2
        // Input: s = "bbaaaaabb"
        // Output: 2
        input = "a";
        cout << "Result of Minimum Deletions to Make String Balanced: " << minimumDeletions(input) << endl;
        cout << "\n";

        // 1578. Minimum Time to Make Rope Colorful
        // Input: colors = "aabaa", neededTime = [1,2,3,4,1]
        // Output: 2
        input = "aabaa";
        vector<int> intV = { 1, 2, 3, 4, 1 };
        cout << "Result of Minimum Time to Make Rope Colorful: " << minCost(input, intV) << endl;
        cout << "\n";

        // 1275. Find Winner on a Tic Tac Toe Game
        // Input: moves = [[0, 0], [1, 1], [2, 0], [1, 0], [1, 2], [2, 1], [0, 1], [0, 2], [2, 2]]
        // Output : "Draw"
        string matrixString = "[[0, 0], [1, 1], [2, 0], [1, 0], [1, 2], [2, 1], [0, 1], [0, 2], [2, 2]";
        LeetCodeUtil::BuildIntMatrixFromString(matrixString, &matrix);
        cout << "Result of Find Winner on a Tic Tac Toe Game: " << tictactoe(matrix) << endl;
        cout << "\n";

        // 1267. Count Servers that Communicate
        // Input: grid = [[1,1,0,0],[0,0,1,0],[0,0,1,0],[0,0,0,1]]
        // Output: 4
        matrixString = "[[1,1,0,0],[0,0,1,0],[0,0,1,0],[0,0,0,1]]";
        LeetCodeUtil::BuildIntMatrixFromString(matrixString, &matrix);
        cout << "Result of Count Servers that Communicate: " << countServers(matrix) << endl;
        cout << "\n";

        // 394. Decode String
        //Input: s = "3[a2[c]]"
        //Output : "accaccacc"
        input = "3[a2[c]]";
        cout << "Result of Decode String: " << decodeString("2[abc]3[cd]ef") << endl;
        cout << "\n";

        // 1775. Equal Sum Arrays With Minimum Number of Operations
        // Input: nums1 = [1, 2, 3, 4, 5, 6], nums2 = [1, 1, 2, 2, 2, 2]
        // Output : 3
        // Input: [5, 2, 1, 5, 2, 2, 2, 2, 4, 3, 3, 5], [1, 4, 5, 5, 6, 3, 1, 3, 3]
        // Output: 1`
        vector<int> intV2 = { 5, 2, 1, 5, 2, 2, 2, 2, 4, 3, 3, 5 };
        intV = { 1, 4, 5, 5, 6, 3, 1, 3, 3 };
        cout << "Result of Equal Sum Arrays With Minimum Number of Operations: " << minOperations(intV2, intV) << endl;
        cout << "\n";

        // 984. String Without AAA or BBB
        // Input: a = 4, b = 1
        // Output: "aabaa"
        cout << "Result of String Without AAA or BBB: " << strWithout3a3b(3, 3) << endl;
        cout << "\n";

        // 153. Find Minimum in Rotated Sorted Array (Medium)
        // Input: nums = [3,4,5,1,2]
        // Output: 1
        Solution153 sol153;
        intV = { 3,4,5,1,2 };
        cout << "Result of Find Minimum in Rotated Sorted Array: " << sol153.findMin(intV) << endl;
        cout << "\n";

        // 1763. Longest Nice Substring
        // Input: s = "YazaAay"
        // Output: "aAa"
        Solution1763 sol1763;
        input = "YazAaAaAay";
        cout << "Result of Longest Nice Substring: " << sol1763.longestNiceSubstring(input) << endl;
        cout << "\n";

        // 1546. Maximum Number of Non-Overlapping Subarrays With Sum Equals Target
        // Input: nums = [-1, 3, 5, 1, 4, 2, -9], target = 6
        // Output : 2, [5,1], [4,2]
        intV = { -1, 3, 5, 1, 4, 2, -9};
        cout << "Result of Maximum Number of Non-Overlapping Subarrays With Sum Equals Target: " << maxNonOverlapping(intV, 6) << endl;
        cout << "\n";
    }
}

