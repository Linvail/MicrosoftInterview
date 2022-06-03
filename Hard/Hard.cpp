// Hard.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <iostream>

#include <stack>
#include <queue>
#include <vector>
#include <set>
#include <deque>

#include "LeetCodeUtil.h"

using namespace std;

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
// 317. Shortest Distance from All Buildings
//
// m == grid.length
// n == grid[i].length
// 1 <= m, n <= 50
//
// One crucial part is how to which a cell has the shortest path. We know it should
// be the one in the middle, but how to do determine it by doing math?
// Consider this matrix:
// 1 0 0 0 0 0 1
// 2 2 2 1 2 2 2
// Start from 3 buildings, we traverse and mark the distance we traveled. we get 3
// distance graphs:
//   1 2 3 4 5
//   3 2 1 2 3
//   5 4 3 2 1
// We sum each cell up, we get:
//   9 8 7 8 9
// 7 is the smallest, that is what we are looking for.
//
// We must use BFS to traverse because DFS may generated larger distance.
//
//-----------------------------------------------------------------------------------
class Solution317
{
public:

    class Cell
    {
    public:

        Cell(int value)
            : m_marker(value)
            , m_distance(0)
            , m_distanceSum(0)
        {}

        // 2: obstacle
        // 1: building
        // 0: land
        short m_marker;
        // Distance from the certain building. We start a BFS for every building.
        unsigned short m_distance;
        // Accumulated distance among all round.
        unsigned int m_distanceSum;
    };

    void clearDistanceRecord(vector<vector<Cell>>& cellGrid)
    {
        for (auto& row : cellGrid)
        {
            for (auto& cell : row)
            {
                cell.m_distance = 0;
            }
        }
    }

    int shortestDistance(vector<vector<int>>& grid)
    {
        const int m = grid.size();
        const int n = grid[0].size();

        vector<vector<Cell>> cellGrid;
        for (const auto& row : grid)
        {
            vector<Cell> cellColumn;
            for (const auto& land : row)
            {
                cellColumn.emplace_back(Cell(land));
            }
            cellGrid.push_back(cellColumn);
        }

        vector<pair<char, char>> moveSteps =
        {
            {1, 0},
            {0, 1},
            {-1, 0},
            {0, -1}
        };

        // For 1st building, the accessible cell is 0 (land). For 2nd building, the accessible cell is -1.
        // Note that if a land is not accessible from certain cell, it won't be visited anymore for the
        // next building. It is okay.
        int buildingMarker = 0;

        for (int i = 0; i < m; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                if (grid[i][j] != 1)
                {
                    continue;
                }

                // Do BFS.
                // pair<i, j>.
                using Coordinate = pair<unsigned char, unsigned char>;
                queue<Coordinate> bfsQueue;
                bfsQueue.push(Coordinate(i, j));
                while (!bfsQueue.empty())
                {
                    const int x = bfsQueue.front().first;
                    const int y = bfsQueue.front().second;
                    bfsQueue.pop();

                    for (const auto& move : moveSteps)
                    {
                        const int a = x + move.first;
                        const int b = y + move.second;
                        if (a >= 0 && a < m && b >= 0 && b < n && cellGrid[a][b].m_marker == buildingMarker)
                        {
                            // Change marker, so BFS won't access it again.
                            cellGrid[a][b].m_marker = buildingMarker - 1;
                            // If this cell is next to the building, we should set distance to 1.
                            cellGrid[a][b].m_distance = cellGrid[x][y].m_marker != 1 ? cellGrid[x][y].m_distance + 1 : 1;
                            // Accumulate distance.
                            cellGrid[a][b].m_distanceSum += cellGrid[a][b].m_distance;
                            // Continue BFS.
                            bfsQueue.push(Coordinate(a, b));
                        }
                    }
                }
                buildingMarker--;
            }
        }

        int miniDistanceEver = INT_MAX;
        for (int i = 0; i < m; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                if (cellGrid[i][j].m_marker == buildingMarker)
                {
                    miniDistanceEver = min(miniDistanceEver, static_cast<int>( cellGrid[i][j].m_distanceSum ));
                }
            }
        }

        return miniDistanceEver == INT_MAX ? -1 : miniDistanceEver;
    }
};

//-----------------------------------------------------------------------------------
// 995. Minimum Number of K Consecutive Bit Flips
// You are given a binary array nums and an integer k.
//
// A k - bit flip is choosing a subarray of length k from nums and simultaneously changing every 0 in the subarray to 1, and every 1 in the subarray to 0.
//
// Return the minimum number of k - bit flips required so that there is no 0 in the array.If it is not possible, return -1.
// A subarray is a contiguous part of an array.
//
// Input: A = [0,0,0,1,0,1,1,0], K = 3
// Output: 3
// Explanation :
// Flip A[0], A[1], A[2] : A becomes[1, 1, 1, 1, 0, 1, 1, 0]
// Flip A[4], A[5], A[6] : A becomes[1, 1, 1, 1, 1, 0, 0, 0]
// Flip A[5], A[6], A[7] : A becomes[1, 1, 1, 1, 1, 1, 1, 1]
//
// To solve this question, we can do this:
// Scan from left to right, if encounter 0, flip k bit, and continue scanning.
// This will solve the question but it will cause Time Limit Exceeded.
// To 'flip', we need to assign one element to another value. If K is very large, it
// takes much time.
// So, we don't want to do actual 'flip'. But if we don't flip, how do we know it
// latest value? It might be changed by previous flip. That's crucial part of this
// question.
// We can use a variable(flipped) to note the start point of a flip and use k to mark an area
// as flipped.
//
// [0,1,0] -> flip at 0 -> [1,0,1]. Set flipped to 1.
// For i = 1 ~ 2, we should check whether flipped is even or odd to decide its latest
// status.
// Note that we minus the number of flipping out of window. For example, when we are
// at nums[3], and flipping count in nums[0] should be removed from flipped.
//-----------------------------------------------------------------------------------

class Solution995
{
public:

    inline int getActualBit(int flippedCount, int originalValue)
    {
        if (flippedCount % 2 == 0) // 0 or even
        {
            return originalValue;
        }
        else
        {
            return originalValue == 0 ? 1 : 0;
        }
    }

    int minKBitFlips(vector<int>& nums, int k)
    {
        const int len = nums.size();
        int result = 0;
        int flippedCount = 0;
        for (int i = 0; i < len; ++i)
        {
            // The length of window is k. When out of window, update the flippedCount.
            if (i >= k)
            {
                // When we flip a bit, we increment it by 2. So by dividing 2, we know
                // how many times we flipped.
                flippedCount -= nums[i - k] / 2;
            }

            // Flip if the actual value is 0.
            if (getActualBit(flippedCount, nums[i]) == 0)
            {
                if (i + k > len)
                {
                    return -1;
                }

                // Increment by 2 when flip once. It may get flipped many times.
                nums[i] += 2;
                flippedCount++;
                result++;
            }
        }

        return result;
    }
};

//-----------------------------------------------------------------------------------
// 44. Wildcard Matching
// Dynamic programming, Greedy, Recursion.
//
// Given an input string (s) and a pattern (p), implement wildcard pattern matching with support for '?' and '*' where:
//
// '?' Matches any single character.
// '*' Matches any sequence of characters(including the empty sequence).
// The matching should cover the entire input string(not partial).
//-----------------------------------------------------------------------------------

class Solution44
{
public:
    bool isMatch(string s, string p)
    {
        // Let s uses index i. p uses index j.
        // dp[i + 1][j + 1] means that s[0 ~ i] matches p[0 ~ j].
        // State transition equation:
        // dp[i][j] = true if
        //   dp[i][j-1] = true and p[j - 1] = *.
        //   dp[i-1][j] = true and p[j - 1] = *.
        //   dp[i-1][j-1] = true and ( p[j - 1] = '?' or s[i - 1] = p[i - 1] )
        const int strLen = s.size();
        const int patternLen = p.size();

        // Special cases:
        // s and p are empty.
        // s is empty. p only consists of *.
        if (strLen == 0 && patternLen == 0)
        {
            return true;
        }

        // The size of dp is (strLen + 1) * (patternLen + 1) because we need to
        // consider the empty string cases - dp[0][0].
        vector<vector<bool>> dp(strLen + 1, vector<bool>(patternLen + 1, false));
        dp[0][0] = true;
        // Initial dp for the case the s is empty
        for (int j = 1; j <= patternLen; ++j)
        {
            if (p[j - 1] == '*')
            {
                dp[0][j] = dp[0][j - 1];
            }
        }


        for (int i = 1; i <= strLen; ++i)
        {
            for (int j = 1; j <= patternLen; ++j)
            {
                if (p[j - 1] == '*')
                {
                    dp[i][j] = dp[i - 1][j] || dp[i][j - 1];
                }
                else
                {
                    dp[i][j] = dp[i - 1][j - 1] && ( s[i - 1] == p[j - 1] || p[j - 1] == '?' );
                }
            }
        }

        return dp[strLen][patternLen];
    }
};

//-----------------------------------------------------------------------------------
// 10. Regular Expression Matching
// Dynamic programming, Recursion.
//
// This is even harder than "44. Wildcard Matching".
//
// abbbbb
// ab*
// a.*
//-----------------------------------------------------------------------------------

class Solution10
{
public:
    bool isMatch(string s, string p)
    {
        // dp[x][y] means that s[0 ~ x - 1] matches p[0 ~ y - 1]
        // dp[0][0] = true.
        // dp[x][y] = dp[x-1][y-1] and ( s[x-1] == p[y-1] or p[y-1] == '.' )
        //          = dp[x][y-2] and p[y-1] = '*' and p[y-2] doesn't occur (no repeat)
        //          = dp[x-1][y] and p[y-1] = '*' and ( p[y-2] == '.' || p[y-2] == s[x-1])

        const int strLen = s.size();
        const int patternLen = p.size();

        // The size of dp is (strLen + 1) * (patternLen + 1) because we need to
        // consider the empty string cases - dp[0][0].
        vector<vector<bool>> dp(strLen + 1, vector<bool>(patternLen + 1, false));
        dp[0][0] = true;

        // Note that i starts from 0 because even if s is empty. (.*) might match it.
        for (int i = 0; i <= strLen; ++i)
        {
            for (int j = 1; j <= patternLen; ++j)
            {
                if (j > 1 && p[j - 1] == '*')
                {
                    dp[i][j] = dp[i][j - 2] || ( i > 0 && dp[i - 1][j] && ( p[j - 2] == '.' || p[j - 2] == s[i - 1] ) );
                }
                else
                {
                    dp[i][j] = i > 0 && dp[i - 1][j - 1] && ( s[i - 1] == p[j - 1] or p[j - 1] == '.' );
                }
            }
        }

        return dp[strLen][patternLen];
    }
};

//-----------------------------------------------------------------------------------
// 4. Median of Two Sorted Arrays
//
// This is very hard.
//-----------------------------------------------------------------------------------
double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2)
{
    // There are several concepts/formulas we need to understand first.
    // 1.For a vector with N elements. To find the median, we can try to divide the
    // list into two sub-list.
    // For example, [1, 2, 3, 4] -> [1, 2] / [3, 4]. [1, 2, 3] -> [1, 2], [2, 3].
    //                                  ^     ^                       ^    ^
    //                                  L     R                       L    R
    // Define: L be the right most of the left list. R be the left most of the right list.
    // The median is: (L+R) / 2.
    //
    // 2. The index of L is: (N-1) / 2.  R is : N / 2.
    //
    // 3. The question gives two lists. Two find the median, we must need to cut two
    // lists into 4 lists, two on left and two on right. All elements on the left side
    // must be <= the any element on the right side, and both sides have the same number
    // of elements. For example:
    // 1       | 3 4 5 7
    // ^         ^
    // L1        R1
    // 1 2 2 2 | 2
    //       ^   ^
    //       L2  R2
    //
    // L1 <= R2 && L2 <= R1. The median is (max(L1, L2) + min(R1, R2)) / 2.
    //
    // 4. To figure out how to cut. We add a few imaginary 'positions' (represented as #'s)
    // in between numbers, and treat numbers as 'positions' as well.
    // [1 3 4 5 7]    ->    [# 1 # 3 # 4 # 5 # 7 #]        N1 = 5
    // index                 0 1 2 3 4 5 6 7 8 9 10        newN1 = 11
    //
    // [1 2 2 2]    ->    [# 1 # 2 # 2 # 2 #]        N2 = 4
    // index               0 1 2 3 4 5 6 7 8         newN2 = 9
    //
    // There are always exactly 2*N+1 'positions' regardless of length N. Therefore, the
    // middle cut should always be made on the Nth position (0-based).
    // Since index(L) = (N-1)/2 and index(R) = N/2 in this situation, we can infer that
    // index(L) = (CutPosition-1)/2, index(R) = (CutPosition)/2.
    // Note that 'L' is the index(L) is referring the index in the original array, and
    // CutPosition is referring the index in the modified array.
    // Therefore, the equation above establish the connection between the original array
    // and modified array.
    //
    // 5. There are 2N1 + 2N2 + 2 position. Each side has N1 + N2 positions.
    // When we cut at position C2 = K in A2, then the cut position in A1 must be C1 = N1 + N2 - k.
    // For instance, if C2 = 2, then we must have C1 = 4 + 5 - C2 = 7.
    // L1 = A1[(C1 - 1) / 2] = A1[6/2] = A1[3] = 5
    // R1 = A1[C1 / 2] = A1[7 / 2] = A1[3] = 5
    //
    // L2 = A2[( C2 - 1 ) / 2] = A2[(2 - 1) / 2] = A2[0] = 1
    // R2 = A2[C2 / 2] = A2[2 / 2] = A2[1] = 2
    //
    // [# 1 # 3 # 4 # (5/5) # 7 #]
    // [# 1 / 2 # 2 # 2 #]
    //
    // 6. Now we know C1 and C2 can be calculated mutually from each other. What we need to do is
    // try to find a C2 to make L1 <= R2 && L2 <= R1.
    // To get better performance, we make A1 is longer than A2, and then we move C2.
    //
    // We can use binary search to adjust C1/C2. When L1 > R2, we should move C1 toward left.
    // When L2 > R1, we should move C2 toward left.
    //
    // 7. Deal with corner cases.
    //    a. If one array is empty, just call the equation mentioned in the 3rd point.
    //    b. If the cut point make us fall out of the array, set it INT_MIN or INT_MAX, depending
    // on which sides.

    int len1 = nums1.size();
    int len2 = nums2.size();
    // Make sure A2 is the shorter one. To get better performance, we want to adjust the cut
    // point for the shorter list.
    if (len1 < len2)
    {
        return findMedianSortedArrays(nums2, nums1);
    }

    // Corner case, one is empty.
    if (len2 == 0)
    {
        return ( static_cast<double>(nums1[( len1 - 1 ) / 2]) + static_cast<double>(nums1[len1 / 2]) ) / 2.0;
    }

    // Note that left and right are indexes resides in the modified arrays.
    // The binary search is working the modified array of nums2 (aka A2).
    int left = 0;
    // The modified array of nums2 has len2 * 2 + 1 elements, so len2 * 2 is the last index.
    int right = len2 * 2;

    // It is <=, not < because we can cut on the edge to make either left or right list be empty.
    // For example, nums2 = [2]. Its modified array is [# 2 #]. We cut on the index 2 (2nd #), the result
    // are [2] and [].
    while (left <= right)
    {
        // As known as C2
        int cutPoint2 = ( left + right ) / 2;
        int cutPoint1 = len1 + len2 - cutPoint2; // The equation: C1 = N1 + N2 - C2.

        // L1
        double left1 = cutPoint1 == 0 ? INT_MIN : nums1[(cutPoint1 - 1) / 2];
        // R1
        double right1 = cutPoint1 == len1 * 2 ? INT_MAX : nums1[( cutPoint1 ) / 2];
        // L2
        double left2 = cutPoint2 == 0 ? INT_MIN : nums2[(cutPoint2 - 1) / 2];
        // R2
        double right2 = cutPoint2 == len2 * 2 ? INT_MAX : nums2[cutPoint2 / 2];

        if (left1 > right2)
        {
            // Move C1 left (C2 right).
            left = cutPoint2 + 1;
        }
        else if (left2 > right1)
        {
            // Move C1 left.
            right = cutPoint2 - 1;
        }
        else
        {
            return ( max(left1, left2) + min(right1, right2) ) / 2;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------------
// 212. Word Search II
// Given an m x n board of characters and a list of strings words, return all words
// on the board.
//
// The hint suggests that people should practice 208. Implement Trie (Prefix Tree).
//
// Recall: In "79. Word Search", we iterate every cell to do DFS + recursion to
// search ONE word.
//
//-----------------------------------------------------------------------------------

class Solution212
{
public:
    class TreeNode
    {
    public:
        TreeNode() : m_children(26, nullptr) {}
        vector<TreeNode*> m_children;
        string m_string;
    };

    class PrefixTree
    {
    public:
        PrefixTree() : m_root(new TreeNode()) {}

        void insert(const string& str)
        {
            if (!str.empty())
            {
                TreeNode* node = m_root;
                for (const auto& c : str)
                {
                    if (!node->m_children[c - 'a'])
                    {
                        node->m_children[c - 'a'] = new TreeNode();
                    }
                    node = node->m_children[c - 'a'];
                }
                node->m_string = str;
            }
        }

        inline TreeNode* getTreeNode(char c, TreeNode* root = nullptr)
        {
            if (!root)
            {
                root = m_root;
            }

            return root->m_children[c - 'a'];
        }

    private:
        TreeNode* m_root;
    };

public:
    void searchWord
        (
        const vector<vector<char>>& board,
        int x,
        int y,
        TreeNode* node,
        vector<vector<bool>>& visitedBoard,
        vector<string>& result
        )
    {
        if (!node->m_string.empty())
        {
            result.push_back(node->m_string);
            // We don't need to put two same string in the result, so
            // clear it.
            node->m_string.clear();
            // Do not return right now. Because a word may be a prefix of
            // another word, like eat and eating.
        }

        pair<int, int> moves[] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };
        visitedBoard[x][y] = true;

        for (const auto& move : moves)
        {
            int nx = x + move.first;
            int ny = y + move.second;

            if (nx >= 0 && nx < m_boardX && ny >= 0 && ny < m_boardY && !visitedBoard[nx][ny])
            {
                TreeNode* nextNode = m_prefixTree->getTreeNode(board[nx][ny], node);
                if (nextNode)
                {
                    searchWord(board, nx, ny, nextNode, visitedBoard, result);
                }
            }
        }
        visitedBoard[x][y] = false;
    }

    vector<string> findWords(vector<vector<char>>& board, vector<string>& words)
    {
        m_boardX = board.size();
        m_boardY = board[0].size();
        // It is not possible to change/re-use 'board', we have no choice to make a matrix here.
        vector<vector<bool>> visitedBoard(m_boardX, vector<bool>(m_boardY, false));
        vector<string> result;

        // Fill prefix tree.
        m_prefixTree = new PrefixTree();
        for (const auto& str : words)
        {
            m_prefixTree->insert(str);
        }

        for (int i = 0; i < m_boardX; ++i)
        {
            for (int j = 0; j < m_boardY; ++j)
            {
                TreeNode* node = m_prefixTree->getTreeNode(board[i][j]);
                if (node)
                {
                    searchWord(board, i, j, node, visitedBoard, result);
                }
            }
        }

        return result;
    }

    ~Solution212()
    {
        delete m_prefixTree;
    }

    int m_boardX = 0;
    int m_boardY = 0;
    PrefixTree* m_prefixTree = nullptr;
};

//-----------------------------------------------------------------------------------
// 154. Find Minimum in Rotated Sorted Array II
//
// Compare to "153. Find Minimum in Rotated Sorted Array", the nums allows duplicated
// elements.
//
// Topic: binary search.
//
// Related question:
// 33. Search in Rotated Sorted Array (Medium)
// 81. Search in Rotated Sorted Array II (Medium) - Allow duplicates
// 154. Find Minimum in Rotated Sorted Array II (Hard)
//-----------------------------------------------------------------------------------

class Solution154
{
public:
    int findMin(vector<int>& nums)
    {
        // Similar to "81. Search in Rotated Sorted Array II".
        // The countermeasure is the same: when mid's value is equal to the right's value,
        // decrease the right instead of assigning right to mid.
        int left = 0;
        int right = nums.size() - 1;
        // {1, 2, 2, 2, 0}
        // {2, 2, 0, 1, 2}
        // {2, 0, 1, 2, 2}
        while (left < right)
        {
            const int mid = left + ( right - left ) / 2;

            if (nums[mid] < nums[right])
            {
                right = mid;
            }
            else if (nums[mid] > nums[right])
            {
                left = mid + 1;
            }
            else
            {
                right--;
            }
        }

        return nums[right];
    }
};

//-----------------------------------------------------------------------------------
// 218. The Skyline Problem (Hard)
// Topics: many, ordered set is one of them.
//
// buildings[i] = [lefti, righti, heighti]
//
// Could use multiset
//-----------------------------------------------------------------------------------

class Solution218
{
public:
    vector<vector<int>> getSkyline(vector<vector<int>>& buildings)
    {
        // Idea: Observe the turning points. They occur when height changes.
        // Separate left(entrance) height and right(exit) height, because we need
        // different processes.

        // We need a storage to sort the building's height by x coordinate.
        // pair<index of height, height>. Negative height represents the left side of the building.
        // Positive height represents the right side of the building.
        vector<pair<int, int>> buildingLines;
        for (const auto& building : buildings)
        {
            buildingLines.push_back({ building[0], -building[2] });
            buildingLines.push_back({ building[1], building[2] });
        }
        // This will sort it by the 1st number of the pair - x coordinate.
        sort(buildingLines.begin(), buildingLines.end());

        // Use this set to sort the height.
        // When scanning from left to right. We want to fetch the highest.
        multiset<int> heights;
        int prevHeight = 0;
        vector<vector<int>> result;
        // This is important because we need the points on ground.
        heights.insert(0);
        for (const auto& buildingLine : buildingLines)
        {
            if (buildingLine.second < 0) // Left side of building
            {
                heights.insert(-buildingLine.second); // Save real positive height.
            }
            else
            {
                // Meet right side of building. Remove it.
                heights.erase(heights.find(buildingLine.second));
            }
            // The highest is on the back.
            int currHeight = *heights.rbegin();
            if (currHeight != prevHeight) // Only needs the point when highest number changes.
            {
                result.push_back({ buildingLine.first, currHeight });
                prevHeight = currHeight;
            }
        }

        return result;
    }
};

//-----------------------------------------------------------------------------------
// 273. Integer to English Words
// Topic: Recursion
// Similar questions: Integer to Roman
// 0 <= num <= 2^31 - 1 = 2,147,483,647  (just need Billion)
//
// Hints:
// * Group the number by thousands (3 digits).
// * Watch out for edge cases.
//-----------------------------------------------------------------------------------
class Solution273
{
public:
    string numberToWords(int num)
    {
        if (num == 0)
        {
            return "Zero";
        }

        int threeDigits = num % 1000;
        string result = convertHundred(threeDigits);
        vector<string> dictOf3Zeros = { "Thousand", "Million", "Billion" };

        for (int i = 0; i < 3; ++i)
        {
            // Remove last 3 digits and process the next 3 digits.
            num /= 1000;
            threeDigits = num % 1000;
            result = threeDigits ? convertHundred(threeDigits) + " " + dictOf3Zeros[i] + " " + result : result;
        }

        // Remove trailing spaces.
        while (result.back() == ' ')
        {
            result.pop_back();
        }

        return result;
    }

    // num at most have 3 digits.
    string convertHundred(int num)
    {
        // Dictionary of numbers 1~19
        vector<string> dict1 = { "", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen" };
        // Dictionary of numbers 20, 30...90.
        vector<string> dict2 = { "", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety" };
        string result;

        int digit3 = num / 100;
        int digit1And2 = num % 100;
        int digit1 = num % 10;

        if (digit3 > 0)
        {
            result = dict1[digit3] + " Hundred";
            if (digit1And2 > 0)
            {
                result.push_back(' ');
            }
        }

        //res = b < 20 ? v1[b] : v2[b / 10] + ( c ? " " + v1[c] : "" );
        if (digit1And2 > 0)
        {
            // 19 = Nineteen
            if (digit1And2 < 20)
            {
                result += dict1[digit1And2];
            }
            else
            {
                // 21 = Twenty One
                result += dict2[digit1And2 / 10];
                if (digit1 != 0)
                {
                    result.push_back(' ');
                    result += dict1[digit1];
                }
            }
        }

        return result;
    }
};

//-----------------------------------------------------------------------------------
// 295. Find Median from Data Stream
//-----------------------------------------------------------------------------------

class MedianFinder
{
public:
    MedianFinder()
        : m_biggerQueue()
        , m_smallerQueue()
    {
    }

    void addNum(int num)
    {
        if (m_biggerQueue.empty() || num > m_biggerQueue.top())
        {
            m_biggerQueue.push(num);
        }
        else
        {
            m_smallerQueue.push(num);
        }

        if (m_biggerQueue.size() > m_smallerQueue.size() + 1)
        {
            m_smallerQueue.push(m_biggerQueue.top());
            m_biggerQueue.pop();
        }
        else if (m_smallerQueue.size() > m_biggerQueue.size())
        {
            m_biggerQueue.push(m_smallerQueue.top());
            m_smallerQueue.pop();
        }
    }

    double findMedian()
    {
        if (m_biggerQueue.size() == m_smallerQueue.size())
        {
            return 0.5 * static_cast<double>( m_biggerQueue.top() + m_smallerQueue.top() );
        }
        else
        {
            return m_biggerQueue.top();
        }
    }

private:
    priority_queue<int, deque<int>, greater<int>> m_biggerQueue; // min heap
    priority_queue<int, deque<int>, less<int>> m_smallerQueue; // max heap
};

int main()
{
    std::cout << "Hard questions!\n";

    // 42. Trapping Rain Water (Hard)
    // Input: height = [0,1,0,2,1,0,1,3,2,1,2,1]
    // Output: 6
    //vector<int> intV = { 0,1,0,2,1,0,1,3,2,1,2,1 };
    // Input: height = [4,2,0,3,2,5]
    // Output: 9
    vector<int> intV = { 4,2,0,3,2,5 };
    cout << "Result of Trapping Rain Water(stack): " << trap(intV) << ". Expect: 9" << endl;
    cout << "\n\n";

    intV = { 0,1,0,2,1,0,1,3,2,1,2,1 };
    cout << "Result of Trapping Rain Water(DP): " << trap_dp(intV) << ". Expect: 6" << endl;
    cout << "\n\n";

    // 317. Shortest Distance from All Buildings
    // Input: grid = [[1,0,2,0,1],[0,0,0,0,0],[0,0,1,0,0]]
    // Output: 7
    Solution317 sol317;
    //string matrixString = "[[1,1,1,1,1,0],[0,0,0,0,0,1],[0,1,1,0,0,1],[1,0,0,1,0,1],[1,0,1,0,0,1],[1,0,0,0,0,1],[0,1,1,1,1,0]]";

    string matrixString = "[[0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,2,1,0,0,2,0,0,0,0,0,1,2,0,0,0,0,0,1,2,0,0,0,0,0,0,0,0,1,0,0,2,1,2,0,0],[0,0,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,2,1,0,2,0,0,2,0,0,0,1,1,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,2,2,0,0,1,2,1,0,0,0,0,0,2,0,2,1,0,0,0,0,0,0,0],[0,1,0,0,1,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,1,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,1,0,0,1,0,0,0,2,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,2,1,0],[2,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0],[1,0,2,0,0,1,0,0,2,0,0,1,0,0,0,1,0,0,0,2,0,2,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,1,0,0,2,0,0],[0,2,0,1,0,0,0,1,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0],[0,0,0,0,0,0,0,2,0,0,0,0,0,2,0,0,0,0,1,0,0,0,0,2,0,0,0,0,0,1,0,0,0,0,0,0,0,0,2,0,1,0,0,0,0,0,0,0,0,0],[2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,2,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,2,0,0,0,0,0],[0,0,0,2,0,0,0,2,0,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,2,0,2,0,0,1,0,0,0,1,2,0,0,0,0,0,0,1],[1,0,0,0,0,1,1,2,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,1,0,0,0,0,0,2,1,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,2,2,0,0,0,0,2,0,1,1,0,0],[0,2,0,0,0,1,1,0,0,0,2,0,0,0,0,0,0,0,0,1,0,1,0,0,0,2,1,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0],[0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,1,0,0,0,0,0,0,0,0,1,0,0,0,2,0,0,1,2,0,0,0,2,0,0,1],[0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2],[0,2,0,2,0,2,0,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,0,1,0,0,2,0,0,1,0,1,0,0,0,0,0,0,0,1,0,2,0,0,0,0,0,0,1],[0,0,0,0,1,0,0,1,0,0,1,2,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,2,0,2,0,0,0,1,0,2,0,0,0,0,0,1,2,2,0,0,0,0],[0,0,2,0,0,0,0,0,0,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,2],[0,0,1,0,0,0,1,0,2,0,0,0,0,0,0,0,0,0,0,2,0,1,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2,0,0,0,0,2,0,0,0,0,0,0],[0,0,0,0,1,2,0,0,0,0,0,1,2,0,0,0,2,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0],[0,2,0,1,0,0,0,2,2,2,0,0,0,0,0,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0],[0,0,0,2,0,1,0,0,0,0,0,2,0,1,2,0,0,0,0,0,0,2,0,0,0,0,2,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0],[2,0,0,0,2,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,1,0,0,0,0,0],[0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,2,0,2,0,2,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1],[0,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,1,2,0,0,0,0,0,1,0,0,0,0,0,0,0,2,0,0],[0,0,2,0,0,1,1,0,0,0,0,0,0,0,0,0,0,2,0,1,0,0,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,2],[1,0,0,2,1,0,0,0,0,0,0,0,2,1,0,0,0,0,0,2,0,0,0,0,2,0,0,0,0,0,1,0,0,0,0,2,0,0,0,0,0,1,0,1,0,0,0,0,0,2],[2,0,0,0,1,0,0,0,2,0,0,0,2,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,1,0,1,0,0,2,0,1,0,0,1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,2,1,0,2,0,0,0,0,0,1,2,0,0,1,0],[1,0,0,0,0,0,0,0,2,0,2,0,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2,2,0,0,0,0],[0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,2,0,0,0,1,0,0,2,0,1,2],[0,2,0,0,0,0,0,0,0,0,1,0,0,0,0,0,2,0,1,0,0,0,0,0,2,0,0,0,1,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,2,1,0,0,0],[0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,0,0,0,0,0,0,2,1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0],[0,1,0,0,0,0,0,0,2,1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,2,2,0,0,0,0,0,1,0,0,0,0],[0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2,1,1,0,0,1,0,0,0,0,0,0,0,0,2,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0],[0,0,1,0,1,0,0,0,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,2,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0],[0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,0,0],[0,0,0,0,2,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,0,1,2,0,0,0,0,0,0,0,0,0,0,0,1],[0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,2,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,2,0,1,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,2,1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,2,0,0,1,0,0,0,0,0,0,0,0,0,0],[0,0,1,0,0,0,2,2,0,2,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0],[0,0,2,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0],[0,0,0,0,0,1,0,0,0,0,0,1,1,2,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,2,0,0,0,0,0,2,0,2,0,0],[0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,1,0,0,2,2,0,0,2,0,0,0,1,1,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,1,0,0,1,2,0,0,2,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,1,0,0,2,0],[0,0,0,0,0,2,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,2,0,0,0,0,0,0,0,0,0,0,0,1,2,0,0,0,0,0,2,0,0,0,0,0,0,0],[0,0,0,0,2,0,0,0,1,1,0,0,0,0,0,0,2,0,0,2,0,0,0,1,1,0,0,0,0,2,0,0,2,0,2,0,2,0,0,0,0,0,0,0,0,0,0,1,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,0,0,1,0,0,0,2,0,0,2,0,1]]";
    //string matrixString = "[[1,0,2,0,1],[0,0,0,0,0],[0,0,1,0,0]]";
    vector<vector<int>> matrix;
    LeetCodeUtil::BuildIntMatrixFromString(matrixString, &matrix);
    cout << "Result of Shortest Distance from All Buildings: " << sol317.shortestDistance(matrix) << endl;
    cout << "\n";

    // 995. Minimum Number of K Consecutive Bit Flips
    // Input: A = [0,0,0,1,0,1,1,0], K = 3
    // Output: 3
    intV = { 0,0,0,1,0,1,1,0 };
    Solution995 sol995;
    cout << "Result of Minimum Number of K Consecutive Bit Flips: " << sol995.minKBitFlips(intV, 3) << endl;
    cout << "\n";

    // 44. Wildcard Matching
    // Input: s = "aa", p = "*"
    // Output : true
    string inputString = "";
    string inputPattern = "a";
    Solution44 sol44;
    cout << "Result of Wildcard Matching: " << sol44.isMatch(inputString, inputPattern) << endl;
    cout << "\n";

    // 10. Regular Expression Matching
    inputString = "aa";
    inputPattern = "a*";
    Solution10 sol10;
    cout << "Result of Regular Expression Matching: " << sol10.isMatch(inputString, inputPattern) << endl;
    cout << "\n";

    // 4. Median of Two Sorted Arrays
    //intV = { 1, 3, 4, 5, 7 };
    //vector<int> intV2 = { 1, 2, 2, 2 };
    intV = { 1, 3, 5  };
    vector<int> intV2 = { 2 };
    cout << "Result of Median of Two Sorted Arrays: " << findMedianSortedArrays(intV, intV2) << endl;

    // 212. Word Search II
    // Input: board = [["o","a","a","n"],["e","t","a","e"],["i","h","k","r"],["i","f","l","v"]], words = ["oath","pea","eat","rain"]
    // Output: ["eat", "oath"]
    // Input: board = "[[o,a,a,n],[i,t,a,e],[n,h,k,r],[g,f,l,v]]", words = ["oath","pea","eat","eating"]
    // Output: ["eat", "oath", "eating"]
    vector<vector<char>> charMatrix;
    LeetCodeUtil::BuildCharMatrixFromString("[[o,a,a,n],[i,t,a,e],[n,h,k,r],[g,f,l,v]]", &charMatrix);
    vector<string> words = { "oath","pea","eat","eating" };

    Solution212 sol212;
    auto availableWords = sol212.findWords(charMatrix, words);
    cout << "Result of Word Search II: " << endl;
    LeetCodeUtil::PrintVector(availableWords);
    cout << "\n";

    // 154. Find Minimum in Rotated Sorted Array II
    // Input: nums = [2,2,2,0,1]
    // Output: 0
    intV = { 1,3,5 };
    Solution154 sol154;
    cout << "Result of Find Minimum in Rotated Sorted Array II: " << sol154.findMin(intV) << endl;
    cout << "\n";

    // 273. Integer to English Words
    // Input: num = 1,234,567
    // Output: "One Million Two Hundred Thirty Four Thousand Five Hundred Sixty Seven"
    Solution273 sol273;
    cout << "Result of Integer to English Words: " << sol273.numberToWords(1000) << endl;
    cout << "\n";

    // 295. Find Median from Data Stream
    MedianFinder medianFinder;
    medianFinder.addNum(-1);
    cout << "findMedian: " << medianFinder.findMedian() << endl;
    medianFinder.addNum(-2);
    cout << "findMedian: " << medianFinder.findMedian() << endl;
    medianFinder.addNum(-3);
    cout << "findMedian: " << medianFinder.findMedian() << endl;
    medianFinder.addNum(-4);
    cout << "findMedian: " << medianFinder.findMedian() << endl;
    medianFinder.addNum(-5);
    cout << "findMedian: " << medianFinder.findMedian() << endl;

    // 218. The Skyline Problem (Hard)
    Solution218 sol218;
    // Input: buildings = [[2,9,10],[3,7,15],[5,12,12],[15,20,10],[19,24,8]]
    // Output: [[2, 10], [3, 15], [7, 12], [12, 0], [15, 10], [20, 8], [24, 0]]
    matrixString = "[[2,9,10],[3,7,15],[5,12,12],[15,20,10],[19,24,8]]";
    LeetCodeUtil::BuildIntMatrixFromString(matrixString, &matrix);
    auto resultVVI = sol218.getSkyline(matrix);
    cout << "\n218. The Skyline Problem (Hard):" << endl;
    LeetCodeUtil::PrintMatrix(resultVVI);

}
