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
        return mergeKLists_helper(lists, 0, static_cast<int>(lists.size() - 1));
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
        /*
        auto comp = [](ListNode*& list1, ListNode*& list2)
        {
            return list1->val > list2->val;
        };

        priority_queue<ListNode*, vector<ListNode*>, decltype( comp )> nodeQueue(comp);
        */

        struct ListNodeComp
        {
            bool operator()(ListNode*& list1, ListNode*& list2) const
            {
                return list1->val > list2->val;
            }
        };

        priority_queue<ListNode*, vector<ListNode*>, ListNodeComp> nodeQueue;

        for (auto list : lists)
        {
            if (list)
            {
                nodeQueue.push(list);
            }
        }

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
    // The hard part is how to deal with the random pointer, which might point to
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

    //-----------------------------------------------------------------------------------
    // 143. Reorder List
    // Topic: two pointers, stack
    // Idea:
    // Two approaches:
    // A:
    //   1. Use two pointers to find the middle one. Cut right there to get two separated
    //      linked list.
    //   2. Reverse the 2nd list.
    //   3. Insert the 2nd list into 1st list (one by one separately).
    //
    // B:
    //   1. Use stack. Push all nodes into the stack.
    //   2. Pop the top one. Cut out the incoming link and insert to the position after
    //      head.
    //   3. Continue to pop the next. We need to pop (n-1) / 2 nodes, n is the count of
    //      all nodes.
    //-----------------------------------------------------------------------------------
    void reorderList(ListNode* head)
    {
        // This question is meaningful only if there are 3 nodes.
        if (!head || !head->next || !head->next->next)
        {
            return;
        }

        ListNode* fastPtr = head;
        ListNode* slowPtr = head;
        while (fastPtr->next && fastPtr->next->next)
        {
            slowPtr = slowPtr->next;
            fastPtr = fastPtr->next->next;
        }
        // n  n  n  n
        //    ^
        // slowPtr stays on the middle node.
        // Reuse fastPtr. Let it be the head of the right list.
        fastPtr = slowPtr->next;
        // Cut trailing link of the 1st list.
        slowPtr->next = nullptr;

        // Reverse the right list. slowPtr will become the head of the right list.
        slowPtr = nullptr;
        while (fastPtr)
        {
            //         1 -> 2 -> null
            // null <- 1 <- 2 <-
            ListNode* tempPtr = fastPtr->next;
            fastPtr->next = slowPtr;

            slowPtr = fastPtr;
            fastPtr = tempPtr;
        }

        // Combine the right list and the left list.
        // 1     2     3
        //    5     4
        while (head && slowPtr)
        {
            ListNode* nextNodeL = head->next;
            head->next = slowPtr;
            slowPtr = slowPtr->next;
            head->next->next = nextNodeL;
            head = nextNodeL;
        }
    }

    // 1 - 2 - 3 - 4 - 5
    // 1 - 2 - 3 - 4 -
    //   5
    // 1 - 5 - 2 - 3 -
    //           4
    void reorderList_stack(ListNode* head)
    {
        // This question is meaningful only if there are 3 nodes.
        if (!head || !head->next || !head->next->next)
        {
            return;
        }

        stack<ListNode*> nodeStack;

        ListNode* cur = head;
        while (cur!= nullptr)
        {
            nodeStack.push(cur);
            cur = cur->next;
        }
        // This equation is obtained by observation.
        int countToPop = ( static_cast<int>(nodeStack.size()) - 1 ) / 2;

        while (countToPop > 0)
        {
            cur = nodeStack.top();
            nodeStack.pop();

            ListNode* nextNode = head->next;
            head->next = cur;
            head->next->next = nextNode;
            head = nextNode;

            countToPop--;
        }
        nodeStack.top()->next = nullptr;
    }

    //-----------------------------------------------------------------------------------
    // 24. Swap Nodes in Pairs
    //-----------------------------------------------------------------------------------
    class Solution24
    {
    public:
        ListNode* swapPairs(ListNode* head, ListNode* prevNode = nullptr)
        {
            if (!head) return nullptr;

            if (!head->next) return head;

            ListNode* newHead = head->next;
            head->next = swapPairs(head->next->next, head);
            newHead->next = head;

            if (prevNode)
            {
                prevNode->next = newHead;
            }

            return newHead;
        }
    };

    //-----------------------------------------------------------------------------------
    // Test Function
    //-----------------------------------------------------------------------------------
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

        // 138. Copy List with Random Pointer (Medium)
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

        // 143. Reorder List (Medium)
        // Input: head = [1, 2, 3, 4, 5]
        // Output : [1, 5, 2, 4, 3]
        ListNode* head = LeetCodeUtil::BuildLinkedListFromVector({ 1, 2, 3, 4, 5 });
        reorderList_stack(head);
        cout << "\n143. Reorder List: " << endl;
        LeetCodeUtil::PrintListNode(head);
        cout << endl;

        // 24. Swap Nodes in Pairs (Medium)
        // Input: head = [1,2,3,4]
        // Output: [2, 1, 4, 3]
        Solution24 sol24;
        head = LeetCodeUtil::BuildLinkedListFromVector({ 1,2,3,4 });
        cout << "\n24. Swap Nodes in Pairs: " << endl;
        head = sol24.swapPairs(head);
        LeetCodeUtil::PrintListNode(head);
        cout << endl;
    }
}

