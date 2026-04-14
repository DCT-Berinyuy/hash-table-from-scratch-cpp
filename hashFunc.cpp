#include <iostream> // → for input/output (cin, cout)
#include <vector> // dynamic array (your buckets)
#include <functional> // Required for std::hash (the professional hash engine)
#include <string> // → for values
#include <algorithm>

using namespace std;

// ================= NODE STRUCTURE =================
// Using Templates <K, V> allows the user to use any data type (int, string, etc.)
template<typename K, typename V>
struct Node {
    K key;
    V value;
    Node* next; // Pointer for Chaining (Closed Addressing)

    // Constructor to quickly create a new node
    Node(K k, V v) : key(k), value(v), next(nullptr) {}
};

// ================= HASH TABLE CLASS =================
template<typename K, typename V>
class HashTable {
private:
    // A vector (array) where each slot is a pointer to the head of a Linked List
    vector<Node<K, V>*> buckets;
    size_t bucketCount;   // Current capacity of the array
    size_t elementCount;  // Current number of items stored
    float maxLoadFactor;  // The threshold to trigger a resize (75% full)

    // HELPER: Checks if a number is prime to improve distribution
    bool isPrime(size_t n) {
        if (n < 2) return false;
        for (size_t i = 2; i * i <= n; i++) {
            if (n % i == 0) return false;
        }
        return true;
    }

    // HELPER: Finds the next prime number to be the new table size
    size_t nextPrime(size_t n) {
        while (!isPrime(n)) n++;
        return n;
    }

    // HELPER: Converts any key into a valid index using the Division Method
    size_t getIndex(const K& key) {
        // std::hash is a built-in C++ tool that turns any object into a size_t number
        size_t hashValue = std::hash<K>{}(key);
        return hashValue % bucketCount;
    }

    // HELPER: Calculates the current "crowdedness" of the table
    float loadFactor() {
        return (float)elementCount / bucketCount;
    }

    // CORE LOGIC: Expands the table when it gets too full
    void rehash() {
        // 1. Calculate new size (double it and find the next prime)
        size_t newBucketCount = nextPrime(bucketCount * 2);
        cout << "\nRehashing triggered! Expanding table to " << newBucketCount << " buckets...\n";

        // 2. Create a temporary new table with the larger size
        vector<Node<K, V>*> newBuckets(newBucketCount, nullptr);

        // 3. Move every node from the old table to the new one
        for (auto head : buckets) {
            Node<K, V>* current = head;
            while (current) {
                Node<K, V>* nextNode = current->next; // Save the rest of the old chain

                // IMPORTANT: Recalculate index because 'bucketCount' has changed!
                size_t newIndex = std::hash<K>{}(current->key) % newBucketCount;

                // Insert the node at the HEAD of the new chain (fastest)
                current->next = newBuckets[newIndex];
                newBuckets[newIndex] = current;

                current = nextNode; // Move to the next node in the old list
            }
        }

        // 4. Swap the old table with the new larger one
        buckets = move(newBuckets);
        bucketCount = newBucketCount;
    }

public:
    // CONSTRUCTOR: Initializes the table with a prime size
    HashTable(size_t size = 11)
        : elementCount(0), maxLoadFactor(0.75f) {
        bucketCount = nextPrime(size);
        buckets.resize(bucketCount, nullptr);
        cout << "Initialized Hash Table with " << bucketCount << " buckets (nearest prime).\n";
    }

    // INSERT or UPDATE logic
    void insert(const K& key, const V& value) {
        size_t index = getIndex(key);
        Node<K, V>* current = buckets[index];

        // Search the chain: If key exists, update its value
        while (current) {
            if (current->key == key) {
                current->value = value;
                cout << "Updated key " << key << " at index " << index << endl;
                return;
            }
            current = current->next;
        }

        // Key doesn't exist: Create a new node and link it at the HEAD
        Node<K, V>* newNode = new Node<K, V>(key, value);
        newNode->next = buckets[index];
        buckets[index] = newNode;

        elementCount++;
        cout << "Inserted (" << key << ", " << value << ") at index " << index << endl;

        // Check if the table is getting too crowded
        if (loadFactor() > maxLoadFactor) {
            rehash();
        }
    }

    // SEARCH logic
    bool find(const K& key, V& result) {
        size_t index = getIndex(key);
        Node<K, V>* current = buckets[index];

        while (current) {
            if (current->key == key) {
                result = current->value; // Pass value back through reference
                return true;
            }
            current = current->next;
        }
        return false; // Key was not found
    }

    // DELETE logic (Standard Linked List node removal)
    bool remove(const K& key) {
        size_t index = getIndex(key);
        Node<K, V>* current = buckets[index];
        Node<K, V>* prev = nullptr;

        while (current) {
            if (current->key == key) {
                // If it's the first node in the chain
                if (prev == nullptr)
                    buckets[index] = current->next;
                else
                    prev->next = current->next; // Bypass the current node

                delete current; // Manually free the memory (Very important!)
                elementCount--;
                cout << "Deleted key " << key << " from index " << index << endl;
                return true;
            }
            prev = current;
            current = current->next;
        }

        cout << "Key not found.\n";
        return false;
    }

    // STATS: Finds the longest collision chain for performance analysis
    int getLongestChain() {
        int maxLen = 0;
        for (auto head : buckets) {
            int len = 0;
            Node<K, V>* current = head;
            while (current) {
                len++;
                current = current->next;
            }
            maxLen = max(maxLen, len);
        }
        return maxLen;
    }

    // VISUALIZATION
    void display() {
        cout << "\n=== HASH TABLE STATE ===\n";
        for (size_t i = 0; i < bucketCount; i++) {
            cout << "[" << i << "] ";
            Node<K, V>* current = buckets[i];
            while (current) {
                cout << "(" << current->key << ":" << current->value << ") -> ";
                current = current->next;
            }
            cout << "NULL\n";
        }
        cout << "Load Factor: " << loadFactor() << endl;
        cout << "Longest Chain: " << getLongestChain() << endl;
    }

    // DESTRUCTOR: Cleans up all dynamic memory to prevent leaks
    ~HashTable() {
        for (auto head : buckets) {
            while (head) {
                Node<K, V>* temp = head;
                head = head->next;
                delete temp;
            }
        }
    }
};

// ================= MAIN DRIVER =================
int main() {
    size_t initialSize;
    cout << "Enter initial hash table size: ";
    if (!(cin >> initialSize)) {
        cin.clear();
        cin.ignore(10000, '\n');
        initialSize = 11;
    }

    // Create a Table where Key = int and Value = string
    HashTable<int, string> ht(initialSize);

    int choice, key;
    string value;

    while (true) {
        cout << "\n[1] Insert  [2] Search  [3] Delete  [4] Display  [5] Exit\n";
        cout << "Choice: ";
        cin >> choice;

        if (choice == 5) break;

        switch (choice) {
            case 1:
                cout << "Enter key (int): "; cin >> key;
                cout << "Enter value (string): "; cin >> value;
                ht.insert(key, value);
                break;
            case 2:
                cout << "Enter key: "; cin >> key;
                if (ht.find(key, value)) cout << "Found Value: " << value << endl;
                else cout << "Not found\n";
                break;
            case 3:
                cout << "Enter key to delete: "; cin >> key;
                ht.remove(key);
                break;
            case 4:
                ht.display();
                break;
            default:
                cout << "Invalid option\n";
        }
    }

    return 0;
}
