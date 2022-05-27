#include "LinkedLists.h"
#include "LeetCodeUtil.h"

#include <stack>
#include <queue>
#include <unordered_map>

namespace LinkedLists
{
    using namespace std;
    using namespace LeetCodeUtil;

    //-----------------------------------------------------------------------------------
    // 445. Add Two Numbers II
    //
    // Follow up: Could you solve it without reversing the input lists?
    //
    // If we can reverse the input lists, this question is the same as
    // "2. Add Two Numbers".
    // Since we cannot, we choose to use two stacks.
    //-----------------------------------------------------------------------------------
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2)
    {
        stack<int> stack1;
        stack<int> stack2;
        ListNode* dummy = new ListNode( -1 );

        // Fill the two stacks.
        while (l1)
        {
            stack1.push(l1->val);
            l1 = l1->next;
        }
        while (l2)
        {
            stack2.push(l2->val);
            l2 = l2->next;
        }

        bool carry = false;
        while (!stack1.empty() || !stack2.empty())
        {
            int value1 = 0;
            if (!stack1.empty())
            {
                value1 = stack1.top();
                stack1.pop();
            }
            int value2 = 0;
            if (!stack2.empty())
            {
                value2 = stack2.top();
                stack2.pop();
            }

            const int sum = value1 + value2 + ( carry ? 1 : 0 );
            carry = sum >= 10;

            ListNode* newNode = new ListNode(carry ? sum - 10 : sum);
            newNode->next = dummy->next;
            dummy->next = newNode;
        }

        if (carry)
        {
            ListNode* newNode = new ListNode(1);
            newNode->next = dummy->next;
            dummy->next = newNode;
        }

        return dummy->next;
    }

    //-----------------------------------------------------------------------------------
    // 23. Merge k Sorted Lists (Hard)
    //-----------------------------------------------------------------------------------

    // We need a function to merge two lists.
    // We have done it in "21. Merge Two Sorted Lists", which can be done by recursion or
    // iteration. To avoid overflow, we will use the iterative one here.
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2)
    {
        ListNode* dummy = new ListNode(-1);
        ListNode* current = dummy;

        while (list1 && list2)
        {
            if (list1->val < list2->val)
            {
                current->next = new ListNode(list1->val);
                list1 = list1->next;
            }
            else
            {
                current->next = new ListNode(list2->val);
                list2 = list2->next;
            }
            current = current->next;
        }

        if (list1)
        {
            current->next = list1;
        }
        else if (list2)
        {
            current->next = list2;
        }

        return dummy->next;
    }

    // Solution 1: Apply Divide and Conquer algorithm on the lists.
    // Solution 2: Use min-heap.

    // Divide and Conquer
    // bottom-up: Merge 2 lists at a time until we get the last one list.
    // Top down: Separate the lists into 2 sub-lists. Call this function recursively
    // on the 2 sub-lists.

    ListNode* mergeKLists_helper(vector<ListNode*>& lists, int left, int right);

    ListNode* mergeKLists(vector<ListNode*>& lists)
    {
        return mergeKLists_helper(lists, 0, lists.size() - 1);
    }

    ListNode* mergeKLists_helper(vector<ListNode*>& lists, int left, int right)
    {
        // Termination condition
        if (left > right)
        {
            return nullptr;
        }
        if (left == right)
        {
            return lists[left];
        }

        const int mid = left + ( right - left ) / 2;

        ListNode* leftNode = mergeKLists_helper(lists, left, mid);
        ListNode* rightNode = mergeKLists_helper(lists, mid + 1, right);

        return mergeTwoLists(leftNode, rightNode);
    }

    ListNode* mergeKLists_min_heap(vector<ListNode*>& lists)
    {
        auto comp = [](ListNode*& list1, ListNode*& list2)
        {
            return list1->val > list2->val;
        };

        priority_queue<ListNode*, vector<ListNode*>, decltype( comp )> nodeQueue(comp);

        for (auto list : lists)
        {
            if (list) // This check is important.
            {
                nodeQueue.push(list);
            }
        }

        ListNode* dummy = new ListNode(-1);
        ListNode* current = dummy;
        while (!nodeQueue.empty())
        {
            current->next = nodeQueue.top();
            nodeQueue.pop();
            current = current->next;

            if (current->next)
            {
                nodeQueue.push(current->next);
            }
        }

        return dummy->next;
    }

    //-----------------------------------------------------------------------------------
    // 138. Copy List with Random Pointer
    //
    // The hard problem is how to deal with the random pointer, which might point to
    // a node that we do not create it.
    //
    // Two solutions:
    // 1. Build a hash map to record the mapping between original node and cloned node.
    // 2. Do 3 times iteration. Firstly, create a clone after the original node.
    // Secondly, set the random pointer. Thirdly, break links between new nodes and old
    // nodes.
    //-----------------------------------------------------------------------------------

    class Node
    {
    public:
        int val;
        Node* next;
        Node* random;

        Node(int _val)
        {
            val = _val;
            next = nullptr;
            random = nullptr;
        }
    };

    Node* copyRandomList(Node* head)
    {
        if (!head)
        {
            return nullptr;
        }

        Node* root = new Node(head->val);
        Node* iterOfNew = root;
        Node* iterOfOld = head->next;

        // <Old node, new node>
        unordered_map<Node*, Node*> nodeMap;
        nodeMap[head] = root;

        // Clone the remaining node.
        while (iterOfOld)
        {
            iterOfNew->next = new Node(iterOfOld->val);
            nodeMap[iterOfOld] = iterOfNew->next;

            iterOfOld = iterOfOld->next;
            iterOfNew = iterOfNew->next;
        }

        // Set random points.
        // Reset iterOfNew and iterOfOld.
        iterOfNew = root;
        iterOfOld = head;
        while (iterOfOld)
        {
            iterOfNew->random = iterOfOld->random ? nodeMap[iterOfOld->random] : nullptr;

            iterOfOld = iterOfOld->next;
            iterOfNew = iterOfNew->next;
        }

        return root;
    }

    void TestLinkedLists()
    {
        cout << endl;

        // 445. Add Two Numbers II
        // Input: l1 = [7,2,4,3], l2 = [5,6,4]
        // Output: [7, 8, 0, 7]
        vector<int> v1 = { 7,2,4,3 };
        vector<int> v2 = { 5,6,4 };
        ListNode* l1 = BuildLinkedListFromVector(v1);
        ListNode* l2 = BuildLinkedListFromVector(v2);
        ListNode* result = addTwoNumbers(l1, l2);

        cout << "Result of Add Two Numbers II: " << endl;
        PrintListNode(result);
        delete l1;
        delete l2;
        delete result;

        // 21. Merge Two Sorted Lists
        // Input: list1 = [1,4,5], list2 = [1,3,4]
        // Output: [1, 1, 3, 4, 4, 5]
        v1 = { 1,4,5 };
        v2 = { 1,3,4 };
        l1 = BuildLinkedListFromVector(v1);
        l2 = BuildLinkedListFromVector(v2);
        result = mergeTwoLists(l1, l2);
        cout << "Result of Merge Two Sorted Lists: " << endl;
        PrintListNode(result);

        // 23. Merge k Sorted Lists (Hard)
        // Input: lists = [[1,4,5],[1,3,4],[2,6]]
        // Output: [1, 1, 2, 3, 4, 4, 5, 6]
        vector<int> v3 = { 2, 6 };
        ListNode* l3 = BuildLinkedListFromVector(v3);
        vector<ListNode*> lists = { l1, l2, l3 };
        //result = mergeKLists(lists);
        result = mergeKLists_min_heap(lists);
        cout << "Result of Merge k Sorted Lists: " << endl;
        PrintListNode(result);
        delete result;

        // 138. Copy List with Random Pointer
        // Input: head = [[7,null],[13,0],[11,4],[10,2],[1,0]]
        // Output: [[7, null], [13, 0], [11, 4], [10, 2], [1, 0]]
        Node node7(7);
        Node node13(13);
        Node node11(11);
        Node node10(10);
        Node node1(1);
        node7.next = &node13;
        node13.next = &node11;
        node11.next = &node10;
        node10.next = &node1;

        node13.random = &node7;
        node11.random = &node1;
        node10.random = &node11;
        node1.random = &node7;
        Node* nodeResult = copyRandomList(&node7);
        cout << endl;
        // result is a deep copy, so we are supposed to delete it.
        // However, this is just a test.
    }
}

