// Design.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include <list>
#include <map>
#include <unordered_map>
#include <string>

using namespace std;

//-----------------------------------------------------------------------------------
// 146. LRU Cache
//
// The functions get and put must each run in O(1) average time complexity.
//
// To achieve, we need to remove an element from the middle and move it to the end.
// We also need insert and erase in constant time complexity.
// So, the std::list is a good choice for the internal storage.
// Furthermore, we also need O(1) direct access, so we need a hash table to find the
// element in the std::list.
//-----------------------------------------------------------------------------------

class LRUCache
{
public:

    LRUCache(int capacity)
        : m_capacity(capacity)
    {

    }

    int get(int key)
    {
        auto index = m_hashTable.find(key);
        if (index == m_hashTable.end())
        {
            return -1;
        }

        // Transfers only the element pointed by 3rd parameter from 2nd parameter into the container.
        // Even if we move the element, the iterator won't become invalid.
        m_storage.splice(m_storage.begin(), m_storage, index->second);

        return m_storage.begin()->second;
    }

    void put(int key, int value)
    {
        auto index = m_hashTable.find(key);
        // If it already exists, we need to update its value and move it to the front.
        // We can delete it first.
        if (index != m_hashTable.end())
        {
            m_storage.erase(index->second);
        }

        m_storage.push_front(pair<int, int>(key, value));
        m_hashTable[key] = m_storage.begin();

        if (m_storage.size() > m_capacity)
        {
            int deadKey = m_storage.back().first;
            m_storage.pop_back();
            m_hashTable.erase(deadKey);
        }
    }

private:
    int m_capacity;
    // <key, value>
    list<pair<int, int>> m_storage;
    // <key, iterator to the element in m_storage>
    unordered_map<int, list<pair<int, int>>::iterator> m_hashTable;
};

//-----------------------------------------------------------------------------------
// 460. LFU Cache
//
// The functions get and put must each run in O(1) average time complexity.
//
// Now every object has a use counter.
// If we let the object holds the use counter, and keep objects in a descending
// sequence by use counter, it is not easy to achieve O(1) average time complexity.
// It takes O(log(n)) to find the new position after updating the use counter.
//
// One solution is to collect all objects that have same use counter together, and
// put them in a hash map of frequency to a list of objects.
//-----------------------------------------------------------------------------------

class LFUCache
{
public:

    using VALUE_TYPE = int;
    using KEY_TYPE = int;
    using FREQ_TYPE = uint32_t;

    LFUCache(int capacity)
        : m_capacity(capacity)
        , m_leastFrequency(0)
    {
    }

    int get(int key)
    {
        if (m_storage.find(key) == m_storage.end())
        {
            return -1;
        }

        int freq = m_storage[key].second;
        // Remove the old frequency.
        m_hashFrequencyObject[freq].erase(m_hashKeyIterator[key]);
        // Increment the frequency and update the hash.
        freq++;
        m_storage[key].second = freq;
        m_hashFrequencyObject[freq].push_back(key);
        m_hashKeyIterator[key] = --m_hashFrequencyObject[freq].end();

        // Update m_leastFrequency.
        if (m_hashFrequencyObject[m_leastFrequency].empty())
        {
            m_leastFrequency++;
        }

        return m_storage[key].first;
    }


    void put(int key, int value)
    {
        if (m_capacity <= 0)
        {
            return;
        }

        // In get(), the frequency will be incremented.
        if (get(key) != -1)
        {
            m_storage[key].first = value;
            return;
        }

        // Not exist, time to insert it...

        // For this problem, when there is a tie (i.e., two or more keys with the same frequency),
        // the least recently used key would be invalidated.
        if (m_storage.size() >= m_capacity )
        {
            // Remove the least frequently used object.
            KEY_TYPE deadkey = m_hashFrequencyObject[m_leastFrequency].front();
            m_hashKeyIterator.erase(deadkey);
            m_storage.erase(deadkey);
            m_hashFrequencyObject[m_leastFrequency].pop_front();
        }

        m_storage[key].first = value;
        m_storage[key].second = 1;
        m_hashFrequencyObject[1].push_back(key);
        m_hashKeyIterator[key] = --m_hashFrequencyObject[1].end();

        // A new object just arrived.
        m_leastFrequency = 1;
    }

private:
    int m_capacity;
    FREQ_TYPE m_leastFrequency;

    // <key, <value, frequency> >
    unordered_map<KEY_TYPE, pair<VALUE_TYPE, FREQ_TYPE>> m_storage;
    // <key, iterator to the value in m_hashFrequency>
    unordered_map<KEY_TYPE, list<KEY_TYPE>::iterator> m_hashKeyIterator;
    // <frequency, list of object's key>
    unordered_map<FREQ_TYPE, list<KEY_TYPE>> m_hashFrequencyObject;
};

//-----------------------------------------------------------------------------------
// 588. Design In-Memory File System
//-----------------------------------------------------------------------------------

class FileSystem
{
public:

    class File
    {
    public:

        File(const string& name, bool isDir)
            : m_isDirectory(isDir)
            , m_name(name)
        {}

        string m_content;
        bool m_isDirectory;
        string m_name;
        map<string, shared_ptr<File>> m_children;
    };

    FileSystem() : m_root( new File("/", true) )
    {
    }

    vector<string> ls(string path)
    {
        vector<string> result;
        shared_ptr<File> current = getFile(path);

        if (!current)
        {
            return result;
        }

        if (!current->m_isDirectory)
        {
            result.emplace_back(current->m_name);
        }
        else
        {
            for (auto& f : current->m_children)
            {
                result.emplace_back(f.first);
            }
        }

        return result;
    }

    void mkdir(string path)
    {
        vector<string> tokens = split(path, '/');

        shared_ptr<File> current = m_root;
        for (auto& p : tokens)
        {
            if (current->m_children.count(p) == 0)
            {
                shared_ptr<File> newDir(new File(p, true));
                current->m_children[p] = newDir;
            }
            current = current->m_children[p];
        }
    }

    void addContentToFile(string filePath, string content)
    {
        shared_ptr<File> current = getFile(filePath, true);
        if (current && !current->m_isDirectory)
        {
            current->m_content += content;
        }
    }

    string readContentFromFile(string filePath)
    {
        shared_ptr<File> current = getFile(filePath);
        string content;

        if (current && !current->m_isDirectory)
        {
            content = current->m_content;
        }
        return content;
    }

private:
    shared_ptr<File> getFile(const string& path, bool create = false)
    {
        vector<string> tokens = split(path, '/');

        shared_ptr<File> current = m_root;
        shared_ptr<File> parent = nullptr;
        for (int i = 0; i < tokens.size(); ++i)
        {
            parent = current;
            current = current->m_children[tokens[i]];
        }

        if (create && !current)
        {
            current.reset( new File(tokens.back(), false) );
            parent->m_children[current->m_name] = current;
        }

        return current;
    }

    vector<string> split(const string& path, const char delim)
    {
        vector<string> tokens;
        istringstream is(path);
        string token;
        while (getline(is, token, delim))
        {
            if (!token.empty())
            {
                tokens.emplace_back(token);
            }
        }
        return tokens;
    }

private:

    shared_ptr<File> m_root;
};

//-----------------------------------------------------------------------------------
// 362. Design Hit Counter
//
// * 1 <= timestamp <= 2 * 109
// * All the calls are being made to the system in chronological order(i.e., timestamp
//   is monotonically increasing).
// * At most 300 calls will be made to hit and getHits.
// Follow up: What if the number of hits per second could be huge? Does your design
// scale?
//
// To address the follow-up, we cannot allocate space for every hit.
// Since getHits(timestamp) only requires 300 seconds before timestamp and timestamp
// is also monotonically increasing, all records in 300 seconds ago are useless.
//-----------------------------------------------------------------------------------

class HitCounter
{
public:
    HitCounter()
        : timestamps(300, 0)
        , hits(300, 0)
    {
    }

    void hit(int timestamp)
    {
        int index = timestamp % 300;

        if (timestamps[index] != timestamp)
        {
            // This means that 300 seconds passed. The original timestamp is useless.
            timestamps[index] = timestamp;
            hits[index] = 1;
        }
        else
        {
            hits[index]++;
        }
    }

    int getHits(int timestamp)
    {
        int result = 0;
        for (int i = 0; i < 300; ++i)
        {
            if (timestamps[i] + 300 > timestamp)
            {
                result += hits[i];
            }
        }
        return result;
    }

private:
    vector<int> timestamps;
    vector<int> hits;
};

//-----------------------------------------------------------------------------------
// 208. Implement Trie (Prefix Tree)
//-----------------------------------------------------------------------------------
class Trie
{
public:
    // Since it's called Prefix Tree, let's do this by using trees.
    // Note that the node doesn't need a 'value' member.
    class TreeNode
    {
    public:
        TreeNode() : m_children(26, nullptr) {}
        bool m_isWord = false;
        vector<TreeNode*> m_children;
    };

    Trie()
    {
        m_root = new TreeNode();
    }

    void insert(string word)
    {
        if (!word.empty())
        {
            TreeNode* current = m_root;
            for (const auto& c : word)
            {
                const int idx = c - 'a';
                if (!current->m_children[idx])
                {
                    current->m_children[idx] = new TreeNode();
                }
                current = current->m_children[idx];
            }
            current->m_isWord = true;
        }
    }

    bool search(string word)
    {
        if (word.empty())
        {
            return false;
        }

        TreeNode* current = m_root;
        for (const auto& c : word)
        {
            const int idx = c - 'a';
            if (!current->m_children[idx])
            {
                return false;
            }
            current = current->m_children[idx];
        }

        return current->m_isWord;
    }

    bool startsWith(string prefix)
    {
        if (prefix.empty())
        {
            return false;
        }

        TreeNode* current = m_root;
        for (const auto& c : prefix)
        {
            const int idx = c - 'a';
            if (!current->m_children[idx])
            {
                return false;
            }
            current = current->m_children[idx];
        }

        return current != nullptr;

    }

private:
    TreeNode* m_root;
};


int main()
{
    std::cout << "Design!\n";

    // 146. LRU Cache
    // Input: ["LRUCache", "put", "put", "get", "put", "get", "put", "get", "get", "get"]
    //        [[2], [1, 1], [2, 2], [1], [3, 3], [2], [4, 4], [1], [3], [4]]
    // Output: [null, null, null, 1, null, -1, null, -1, 3, 4]

    LRUCache lRUCache(2);
    lRUCache.put(1, 1); // cache is {1=1}
    lRUCache.put(2, 2); // cache is {1=1, 2=2}
    int result = lRUCache.get(1);    // return 1
    lRUCache.put(3, 3); // LRU key was 2, evicts key 2, cache is {1=1, 3=3}
    result = lRUCache.get(2);    // returns -1 (not found)
    lRUCache.put(4, 4); // LRU key was 1, evicts key 1, cache is {4=4, 3=3}
    result = lRUCache.get(1);    // return -1 (not found)
    result = lRUCache.get(3);    // return 3
    result = lRUCache.get(4);    // return 4

    // 460. LFU Cache
    // Input
    // ["LFUCache", "put", "put", "get", "put", "get", "get", "put", "get", "get", "get"]
    // [[2], [1, 1], [2, 2], [1], [3, 3], [2], [3], [4, 4], [1], [3], [4]]
    // Output
    // [null, null, null, 1, null, -1, 3, null, -1, 3, 4]


    LFUCache lfu(2);
    lfu.put(1, 1);   // cache=[1,_], cnt(1)=1
    lfu.put(2, 2);   // cache=[2,1], cnt(2)=1, cnt(1)=1
    cout << "get(1) " << lfu.get(1) << endl;      // return 1
                     // cache=[1,2], cnt(2)=1, cnt(1)=2
    lfu.put(3, 3);   // 2 is the LFU key because cnt(2)=1 is the smallest, invalidate 2.
                     // cache=[3,1], cnt(3)=1, cnt(1)=2
    cout << "get(2) " << lfu.get(2) << endl;      // return -1 (not found)
    cout << "get(3) " << lfu.get(3) << endl;      // return 3
                     // cache=[3,1], cnt(3)=2, cnt(1)=2
    lfu.put(4, 4);   // Both 1 and 3 have the same cnt, but 1 is LRU, invalidate 1.
                     // cache=[4,3], cnt(4)=1, cnt(3)=2
    cout << "get(1) " << lfu.get(1) << endl;      // return -1 (not found)
    cout << "get(3) " << lfu.get(3) << endl;      // return 3
                     // cache=[3,4], cnt(4)=1, cnt(3)=3
    cout << "get(4) " << lfu.get(4) << endl;      // return 4
                     // cache=[4,3], cnt(4)=2, cnt(3)=3

    // Input:
    // ["FileSystem", "ls", "mkdir", "addContentToFile", "ls", "readContentFromFile"]
    // [[], ["/"], ["/a/b/c"], ["/a/b/c/d", "hello"], ["/"], ["/a/b/c/d"]]
    // Output:
    // [null, [], null, null, ["a"], "hello"]
    FileSystem fileSystem;
    fileSystem.ls("/"); // return []
    fileSystem.mkdir("/a/b/c");
    fileSystem.addContentToFile("/a/b/c/d", "hello");
    fileSystem.ls("/");  // return ["a"]
    fileSystem.readContentFromFile("/a/b/c/d"); // return "hello"

    cout << "\n\n";

    // 362. Design Hit Counter
    HitCounter hitCounter;
    hitCounter.hit(1);       // hit at timestamp 1.
    hitCounter.hit(2);       // hit at timestamp 2.
    hitCounter.hit(3);       // hit at timestamp 3.
    cout << "getHits for 4: " << hitCounter.getHits(4) << endl;   // get hits at timestamp 4, return 3.
    hitCounter.hit(300);     // hit at timestamp 300.
    cout << "getHits for 300: " << hitCounter.getHits(300) << endl; // get hits at timestamp 300, return 4.
    cout << "getHits for 301: " << hitCounter.getHits(301) << endl; // get hits at timestamp 301, return 3.

    // 208. Implement Trie (Prefix Tree)
    Trie trie;
    trie.insert("apple");
    trie.search("apple");   // return True
    trie.search("app");     // return False
    trie.startsWith("app"); // return True
    trie.insert("app");
    trie.search("app");     // return True
}
