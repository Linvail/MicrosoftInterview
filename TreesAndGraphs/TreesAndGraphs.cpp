#include "TreesAndGraphs.h"
#include "LeetCodeUtil.h"

#include <unordered_map>
#include <stack>

namespace TreesAndGraphs
{
    using namespace LeetCodeUtil;

    //-----------------------------------------------------------------------------------
    // 235. Lowest Common Ancestor of a Binary Search Tree (Easy)
    //
    // If root's value is greater than both p and q, p and q must be in the left tree.
    // If root's value is less than both p and q, p and q must be in the right tree.
    // If root's value is in between p and q, root is the LCA.
    //-----------------------------------------------------------------------------------

    TreeNode* lowestCommonAncestor_BST(TreeNode* root, TreeNode* p, TreeNode* q)
    {
        while (true)
        {
            if (root->val > max(p->val, q->val))
            {
                root = root->left;
            }
            else if (root->val < min(p->val, q->val))
            {
                root = root->right;
            }
            else
            {
                break;
            }
        }

        return root;
    }

    //-----------------------------------------------------------------------------------
    // 236. Lowest Common Ancestor of a Binary Tree (Medium)
    //
    // Observation:
    // p and q could be
    // 1. in left or right sub-tree separately
    // 2. both in left sub-tree.
    // 3. both in right sub-tree.
    // For case 2 and 3, they are suitable cases for a recursive function.
    // How should we design the recursive function to cover all cases?
    // In case 2, the call for right sub-tree should return null.
    // In case 3, the call for left sub-tree should return null.
    // So, the only case that both calls would return non-null is the case 1, in which
    // the calls would return p and q. In this case, the LCA is the root.
    //
    //-----------------------------------------------------------------------------------

    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q)
    {
        if (!root || p == root || q == root)
        {
            return root;
        }

        TreeNode* left = lowestCommonAncestor(root->left, p, q);
        if (left && left != p && left != q)
        {
            return left;
        }

        // When we reach here, we already know 'left' is either null, p, or q.
        TreeNode* right = lowestCommonAncestor(root->right, p, q);

        // Root is LCA if both are not null.
        if (right && left)
        {
            return root;
        }

        // Only one is null, return it.
        return left ? left : right;
    }

    //-----------------------------------------------------------------------------------
    // 133. Clone Graph
    //
    // The crucial part is how to avoid unnecessary clone of the same node.
    // We can use a hash map to store the mapping between old nodes and new nodes.
    // This problem is similar to "138. Copy List with Random Pointer.
    //
    // We can use DFS to traverse the graph.
    //-----------------------------------------------------------------------------------

    class Solution133
    {
    public:
        class Node
        {
        public:
            int val;
            vector<Node*> neighbors;

            Node()
            {
                val = 0;
                neighbors = vector<Node*>();
            }
            Node(int _val)
            {
                val = _val;
                neighbors = vector<Node*>();
            }
            Node(int _val, vector<Node*> _neighbors)
            {
                val = _val;
                neighbors = _neighbors;
            }
        };

        Node* cloneGraph_DFS(Node* node, unordered_map<Node*, Node*>& nodeMap)
        {
            if (!node)
            {
                return nullptr;
            }
            // This node has been cloned before, return it directly.
            if (nodeMap.count(node) != 0)
            {
                return nodeMap[node];
            }

            Node* newNode = new Node(node->val);
            nodeMap[node] = newNode;
            for (auto& adj : node->neighbors)
            {
                newNode->neighbors.push_back(cloneGraph_DFS(adj, nodeMap));
            }

            return newNode;
        }

        Node* cloneGraph(Node* node)
        {
            unordered_map<Node*, Node*> nodeMap;
            return cloneGraph_DFS(node, nodeMap);
        }
    };

    //-----------------------------------------------------------------------------------
    // 545. Boundary of Binary Tree
    //
    // We can do this by recursion or iteration.
    //
    //-----------------------------------------------------------------------------------

    // This function can collect left bound nodes, left nodes, and right bounds.
    // When we just need leaf nodes, the 2nd and 3rd parameter are both false.
    void boundaryOfBinaryTreeHelper
        (
        TreeNode* root,
        bool seekLeftBound,
        bool seekRightBound,
        vector<int>& result
        )
    {
        if (!root)
        {
            return;
        }
        // Leaf
        if (!root->left && !root->right)
        {
            result.push_back(root->val);
            return;
        }

        if (seekLeftBound)
        {
            result.push_back(root->val);
        }

        boundaryOfBinaryTreeHelper(root->left, seekLeftBound, seekRightBound && root->right == nullptr, result);
        boundaryOfBinaryTreeHelper(root->right, seekLeftBound && root->left == nullptr, seekRightBound, result);

        // The right boundary needs to be output in reverse order, so this must be put
        // after the recursion.

        if (seekRightBound)
        {
            result.push_back(root->val);
        }
    }

    vector<int> boundaryOfBinaryTree(TreeNode* root)
    {
        vector<int> result;
        // Note that when root has no child, it is also a left but we cannot add it twice.
        if (root)
        {
            result.push_back(root->val);
        }

        boundaryOfBinaryTreeHelper(root->left, true, false, result);
        boundaryOfBinaryTreeHelper(root->right, false, true, result);

        return result;
    }

    vector<int> boundaryOfBinaryTreeIteration(TreeNode* root)
    {
        vector<int> result;

        if (!root)
        {
            return result;
        }

        TreeNode* left = root->left;
        TreeNode* right = root->right;
        TreeNode* current = root;

        // root is not considered as leaf if it has at least one node.
        if (left || right)
        {
            result.push_back(root->val);
        }

        // Collect left boundary
        while (left)
        {
            // if not leaf
            if (left->left || left->right)
            {
                result.push_back(left->val);
            }
            left = left->left ? left->left : left->right;
        }

        // Collect leaves from left to right.
        stack<TreeNode*> nodeStack;
        while (current || !nodeStack.empty())
        {
            if (current)
            {
                nodeStack.push(current);
                if (!current->left && !current->right)
                {
                    result.push_back(current->val);
                }
                current = current->left;
            }
            else
            {
                // No any left node...
                current = nodeStack.top();
                nodeStack.pop();
                current = current->right;
            }
        }

        vector<int> rightBoundaryTemp;
        // Collect right boundary
        while (right)
        {
            if (right->left || right->right)
            {
                rightBoundaryTemp.push_back(right->val);
            }

            right = right->right ? right->right : right->left;
        }

        result.insert(result.end(), rightBoundaryTemp.rbegin(), rightBoundaryTemp.rend());

        return result;
    }

    //-----------------------------------------------------------------------------------
    // 1448. Count Good Nodes in Binary Tree
    //-----------------------------------------------------------------------------------

    int goodNodes_helper(TreeNode* root, int maxValueAlongPath)
    {
        if (!root)
        {
            return 0;
        }

        if (root->val >= maxValueAlongPath )
        {
            return 1 + goodNodes_helper(root->left, root->val) +
                goodNodes_helper(root->right, root->val);
        }
        else
        {
            return goodNodes_helper(root->left, maxValueAlongPath) +
                goodNodes_helper(root->right, maxValueAlongPath);
        }
    }

    int goodNodes(TreeNode* root)
    {
        return goodNodes_helper(root, INT_MIN);
    }

    int goodNodes_iteration(TreeNode* root)
    {
        if (!root)
        {
            return 0;
        }

        int result = 0;
        stack<int> maxValueAlongPath;
        stack<TreeNode*> nodeStack;
        int currentMax = INT_MIN;
        TreeNode* current = root;

        while (current || !nodeStack.empty())
        {
            if (current)
            {
                currentMax = max(current->val, currentMax);
                maxValueAlongPath.push(currentMax);
                nodeStack.push(current);
                current = current->left;
            }
            else
            {
                current = nodeStack.top();
                nodeStack.pop();
                currentMax = maxValueAlongPath.top();
                maxValueAlongPath.pop();
                if (current->val >= currentMax)
                {
                    result++;
                }

                current = current->right;
            }
        }

        return result;
    }


    void TestTreesAndGraphs()
    {
        // 235. Lowest Common Ancestor of a Binary Search Tree (Easy)
        // Input: root = [6,2,8,0,4,7,9,null,null,3,5], p = 2, q = 3
        // Output: 2
        string treeString = "[6,2,8,0,4,7,9,null,null,3,5]";
        TreeNode* root = LeetCodeUtil::BuildTreeFromLevelOrderString(treeString);
        TreeNode* result = lowestCommonAncestor_BST(root, root->left, root->left->right->left);
        cout << "Result of Lowest Common Ancestor of a Binary Search Tree: " << result->val << endl;
        DeleteTree(root);

        // 236. Lowest Common Ancestor of a Binary Tree (Medium)
        // Input: root = [3, 5, 1, 6, 2, 0, 8, null, null, 7, 4], p = 5, q = 1
        // Output : 3
        treeString = "[3, 5, 1, 6, 2, 0, 8, null, null, 7, 4]";
        root = LeetCodeUtil::BuildTreeFromLevelOrderString(treeString);
        result = lowestCommonAncestor(root, root->left, root->right);
        cout << "Result of Lowest Common Ancestor of a Binary Tree: " << result->val << endl;
        DeleteTree(root);

        // 545. Boundary of Binary Tree
        // Input: root = [1, null, 2, 3, 4]
        // Output : [1, 3, 4, 2]
        // Input: root = [1,2,3,4,5,6,null,null,null,7,8,9,10]
        // Output : [1,2,4,7,8,9,10,6,3]

        #if 0
        treeString = "[-64,12,18,-4,-53,null,76,null,-51,null,null,-93,3,null,-31,47,null,3,53,-81,33,4,null,-51,-44,-60,11,null,null,null,null,78,null,-35, \
                    -64,26,-81,-31,27,60,74,null,null,8,-38,47,12,-24,null,-59,-49,-11,-51,67,null,null,null,null,null,null,null,-67,null,-37,-19,10,-55,72,null,null,null,-70,17, \
                    -4,null,null,null,null,null,null,null,3,80,44,-88,-91,null,48,-90,-30,null,null,90,-34,37,null,null,73,-38,-31,-85,-31,-96,null,null,-18,67,34,72,null,-17,-77, \
                    null,56,-65,-88,-53,null,null,null,-33,86,null,81,-42,null,null,98,-40,70,-26,24,null,null,null,null,92,72,-27,null,null,null,null,null,null,-67,null,null,null, \
                    null,null,null,null,-54,-66,-36,null,-72,null,null,43,null,null,null,-92,-1,-98,null,null,null,null,null,null,null,39,-84,null,null,null,null,null,null,null,null, \
                    null,null,null,null,null,-93,null,null,null,98]";
        #endif

        treeString = "[1,2,3,4,5,6,null,null,null,7,8,9,10]";
        root = LeetCodeUtil::BuildTreeFromLevelOrderString(treeString);
        //auto resultV = boundaryOfBinaryTree(root);
        auto resultV = boundaryOfBinaryTreeIteration(root);
        cout << "Result of Boundary of Binary Tree: " << endl;
        printVector(resultV);
        DeleteTree(root);

        // 1448. Count Good Nodes in Binary Tree
        // Input: root = [3,1,4,3,null,1,5]
        // Output: 4
        treeString = "[3,1,4,3,null,1,5]";
        root = LeetCodeUtil::BuildTreeFromLevelOrderString(treeString);
        cout << "Result of Count Good Nodes in Binary Tree: " << goodNodes(root) << ". Expect: 4" << endl;

        DeleteTree(root);
    }
}