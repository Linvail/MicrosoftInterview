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
#include <numeric> // for accumulate

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
    // 42. Trapping Rain Water (Hard)
    //
    // There are 3 major ways to solve this.
    // 1. Dynamic programming
    // 2. Two pointer
    // 3. Stack.
    //
    // The first two ways are made based on a fact:
    // When you know the left boundary and the valley, you just need to know if there a
    // boundary on the right that is equal or higher than the left boundary. You don't
    // need to know the position of right boundary.
    // For example, height[1] = 2, height[2] = 1, height[x] = 2+ (x > 2). We can know
    // position 1 can trap one unit of water.
    //
    // To develop DP solution, we need the the maximum height of left and right of each
    // position.
    //-----------------------------------------------------------------------------------

    // Using stack.
    int trap(vector<int>& height)
    {
        // Use to store the left slope and valley/basin.
        // For example,  2 -> 1. If the next is 2 (greater the top - 1), there is valley.

        stack<size_t> positions;
        const size_t len = height.size();
        int i = 0;
        int result = 0;

        while (i < len)
        {
            if (positions.empty() || height[positions.top()] >= height[i])
            {
                positions.push(i);
                i++;
            }
            else
            {
                const size_t valley = positions.top();
                positions.pop();

                // Empty means no left boundary. There is no valid valley.
                // It also means that the height[i] is higher than the old left boundary.
                // We discarded the old left boundary. The new one will be inserted in the next loop.
                if (positions.empty())
                {
                    // Keep scanning the next..
                    continue;
                }

                const int left = height[positions.top()];
                const size_t maxBoundary = min(left, height[i]);
                const size_t diff = maxBoundary - height[valley];
                const size_t area = i - positions.top() - 1;

                result += diff * area;

                // Note that we don't increment i and don't change the stack.
                // In the next loop, we will decide to pop the stack or insert the current position.
            }
        }

        return result;
    }

    // Using dynamic programming
    int trap_dp(vector<int>& height)
    {
        const size_t len = height.size();
        // dp[i] : the maximum height of the left side.
        vector<int> dp(len, 0);
        int maxH = 0;
        // dp[0] must be 0.
        for (int i = 0; i < len; ++i)
        {
            dp[i] = maxH;
            maxH = max(maxH, height[i]);
        }

        int result = 0;
        // Now we need the max height of the right side.
        // However, what we actually need is the minimum of maximum height of the left/right side.
        // Therefore, it is possible to re-use dp[i] to store the min.
        maxH = 0;
        for (int i = len - 1; i >= 0; --i)
        {
            dp[i] = min(dp[i], maxH);
            maxH = max(maxH, height[i]);

            if (dp[i] > height[i])
            {
                result += dp[i] - height[i];
            }
        }

        return result;
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
    // Space: O(1)O(1)
    //-----------------------------------------------------------------------------------

    int minOperations(vector<int>& nums1, vector<int>& nums2)
    {
        if (nums1.size() * 6 < nums2.size() || nums2.size() * 6 < nums1.size())
            return -1;

        int sum1 = accumulate(begin(nums1), end(nums1), 0);
        int sum2 = accumulate(begin(nums2), end(nums2), 0);
        if (sum1 > sum2)
            return minOperations(nums2, nums1);

        int ans = 0;
        // increases in nums1 & decreases in nums2
        vector<int> count(6);

        for (const int num : nums1)
            ++count[6 - num];

        for (const int num : nums2)
            ++count[num - 1];

        for (int i = 5; sum2 > sum1;) {
            while (count[i] == 0)
                --i;
            sum1 += i;
            --count[i];
            ++ans;
        }

        return ans;
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

        // Input: height = [0,1,0,2,1,0,1,3,2,1,2,1]
        // Output: 6
        //vector<int> intV = { 0,1,0,2,1,0,1,3,2,1,2,1 };
        // Input: height = [4,2,0,3,2,5]
        // Output: 9
        vector<int> intV = { 4,2,0,3,2,5 };
        cout << "Result of Trapping Rain Water(stack): " << trap(intV) << endl;
        cout << "\n\n";

        intV = { 0,1,0,2,1,0,1,3,2,1,2,1 };
        cout << "Result of Trapping Rain Water(DP): " << trap_dp(intV) << endl;
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
        intV = { 1, 2, 3, 4, 1 };
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

        vector<int> intV2 = { 1,1,2,2,2,2 };
        intV = { 1,2,3,4,5,6 };

        cout << "Result of Equal Sum Arrays With Minimum Number of Operations: " << minOperations(intV2, intV) << endl;
    }
}

