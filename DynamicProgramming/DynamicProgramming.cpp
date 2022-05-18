// DynamicProgramming.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <algorithm>

#include "LeetCodeUtil.h"

using namespace std;

//-----------------------------------------------------------------------------------
// 568. Maximum Vacation Days (Hard)
//-----------------------------------------------------------------------------------
/*-----------------------------------------------------------------------------------
* LeetCode wants to give one of its best employees the option to travel among n cities to collect algorithm problems.
But all work and no play makes Jack a dull boy, you could take vacations in some particular cities and weeks.
Your job is to schedule the traveling to maximize the number of vacation days you could take, but there are certain rules and restrictions you need to follow.

Rules and restrictions:

You can only travel among n cities, represented by indexes from 0 to n - 1. Initially, you are in the city indexed 0 on Monday.
The cities are connected by flights. The flights are represented as an n x n matrix (not necessarily symmetrical),
called flights representing the airline status from the city i to the city j.
If there is no flight from the city i to the city j, flights[i][j] == 0; Otherwise, flights[i][j] == 1. Also, flights[i][i] == 0 for all i.

You totally have k weeks (each week has seven days) to travel.
You can only take flights at most once per day and can only take flights on each week's Monday morning.
Since flight time is so short, we do not consider the impact of flight time.

For each city, you can only have restricted vacation days in different weeks, given an n x k matrix called days representing this relationship.
For the value of days[i][j], it represents the maximum days you could take a vacation in the city i in the week j.
You could stay in a city beyond the number of vacation days, but you should work on the extra days, which will not be counted as vacation days.
If you fly from city A to city B and take the vacation on that day, the deduction towards vacation days will count towards the vacation days of city B in that week.
We do not consider the impact of flight hours on the calculation of vacation days.

Given the two matrices flights and days, return the maximum vacation days you could take during k weeks.
-----------------------------------------------------------------------------------*/

// This is a hard dynamic programming question.
// The core of solving the dynamic programming problem is to write the state transition equation.
// Idea:
// Define maxVacationInCity[w] as the vacation already taken so far and we are on Monday in city w.
// We advance week until the last week. The maximum value within maxVacationInCity[] is the answer.
// To update this array, we need to consider:
// a. Stay in the same city. maxVacationInCity[i] + days[i][w]. For city i, week w.
// b. Fly from another city. maxVacationInCity[j] + days[i][w]. For city j, week w.
// We need to update every maxVacationInCity[i] by considering traveling from maxVacationInCity[j] in every week.
// So, we need 3 loops.
int maxVacationDays(vector<vector<int>>& flights, vector<vector<int>>& days)
{
    const int cityCount = flights.size();
    const int weekCount = days[0].size();

    vector<int> maxVacationInCity(cityCount, INT_MIN);
    maxVacationInCity[0] = 0;

    for (int w = 0; w < weekCount; ++w)
    {
        // We need to look up the data of previous week when we update the current week.
        // So, we cannot pollute maxVacationInCity. We need to a temp vector to store the new data.
        // After the loop ends, we assign temp to maxVacationInCity.
        vector<int> temp(cityCount, INT_MIN);
        for (int i = 0; i < cityCount; ++i)
        {
            for (int j = 0; j < cityCount; ++j)
            {
                // Stay in the same city or fly from j to i.
                if (i == j || flights[j][i] == 1)
                {
                    temp[i] = max(temp[i], maxVacationInCity[j] + days[i][w]);
                }
            }
        }
        swap(maxVacationInCity, temp);
    }

    return *max_element(maxVacationInCity.begin(), maxVacationInCity.end());
}

// 588. Design In-Memory File System

class FileSystem
{
public:
    FileSystem()
    {

    }

    vector<string> ls(string path)
    {

    }

    void mkdir(string path)
    {

    }

    void addContentToFile(string filePath, string content)
    {

    }

    string readContentFromFile(string filePath)
    {

    }
};


int main()
{
    std::cout << "Dynamic programming\n";

    // Input: flights = [[0,1,1],[1,0,1],[1,1,0]], days = [[1,3,1],[6,0,3],[3,3,3]]
    // Output: 12

    vector<vector<int>> flights =
    {
        {0, 1, 1},
        {1, 0, 1},
        {1, 1, 0}
    };
    vector<vector<int>> days =
    {
        {1, 3, 1},
        {6, 0, 3},
        {3, 3, 3}
    };

    cout << "Result of Maximum Vacation Days:: " << maxVacationDays(flights, days) << endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
