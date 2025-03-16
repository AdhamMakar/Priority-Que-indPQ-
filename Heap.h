#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
using namespace std;

int nextPrime( int n );

class IndPQ {
private:
    // Custom hash function for strings
    static size_t hashString(const string &str, size_t tableSize) {
        size_t hashVal = 0;
        for (char ch : str) {
            hashVal = 37 * hashVal + static_cast<size_t>(ch);
        }
        return hashVal % tableSize;
    }

    // BiMap class from assignment 2 (Hmap)
    class BiMap {
    private:
        struct HashEntryKey {
            string key;
            int value;
            enum EntryType { ACTIVE, EMPTY, DELETED } info;
            HashEntryKey(const string &k = "", int v = 0, EntryType i = EMPTY)
                : key(k), value(v), info(i) {}
        };

        struct HashEntryVal {
            int key;
            string value;
            enum EntryType { ACTIVE, EMPTY, DELETED } info;
            HashEntryVal(int k = 0, const string &v = "", EntryType i = EMPTY)
                : key(k), value(v), info(i) {}
        };

        vector<HashEntryKey> keyTable;
        vector<HashEntryVal> valTable;
        int currentSize;

        bool isActiveKey(int pos) const { return keyTable[pos].info == HashEntryKey::ACTIVE; }
        bool isActiveVal(int pos) const { return valTable[pos].info == HashEntryVal::ACTIVE; }

        int findPosKey(const string &key, size_t (*hashFunc)(const string &, size_t)) const {
            int offset = 1;
            int currentPos = hashFunc(key, keyTable.size());

            while (keyTable[currentPos].info != HashEntryKey::EMPTY && keyTable[currentPos].key != key) {
                currentPos += offset; 
                offset += 2;
                if (currentPos >= keyTable.size())
                    currentPos -= keyTable.size();
            }

            return currentPos;
        }

        int findPosVal(int val, size_t (*hashFunc)(int, size_t)) const {
            int offset = 1;
            int currentPos = hashFunc(val, valTable.size());

            while (valTable[currentPos].info != HashEntryVal::EMPTY && valTable[currentPos].key != val) {
                currentPos += offset; // Quadratic probing
                offset += 2;
                if (currentPos >= valTable.size())
                    currentPos -= valTable.size();
            }

            return currentPos;
        }

        void rehash() {
            vector<HashEntryKey> oldKeyTable = keyTable;
            vector<HashEntryVal> oldValTable = valTable;

            keyTable.resize(nextPrime(2 * oldKeyTable.size()));
            valTable.resize(nextPrime(2 * oldValTable.size()));

            for (auto &entry : keyTable)
                entry.info = HashEntryKey::EMPTY;
            for (auto &entry : valTable)
                entry.info = HashEntryVal::EMPTY;

            currentSize = 0;
            for (auto &entry : oldKeyTable)
                if (entry.info == HashEntryKey::ACTIVE)
                    insert(entry.key, entry.value);
        }

    public:
        BiMap(int size = 5) : keyTable(nextPrime(size)), valTable(nextPrime(size)), currentSize(0) {
            for (auto &entry : keyTable)
                entry.info = HashEntryKey::EMPTY;
            for (auto &entry : valTable)
                entry.info = HashEntryVal::EMPTY;
        }

        bool isActive(int pos) const {
            return isActiveKey(pos);
        }
         //new get state function to use in the ddisplay
        string getEntryState(size_t index) const { 
            if (keyTable[index].info == HashEntryKey::ACTIVE) {
                return "ACTIVE";
            } else if (keyTable[index].info == HashEntryKey::EMPTY) {
                return "EMPTY";
            } else if (keyTable[index].info == HashEntryKey::DELETED) {
                return "DELETED";
            }
            return "UNKNOWN";
        }

        //new get function to get the Bimap array for ddisplay
        const vector<HashEntryKey>& getKeyTable() const {
            return keyTable;
        }

        bool insert(const string &key, int value) {
            int keyPos = findPosKey(key, hashString);
            int valPos = findPosVal(value, [](int val, size_t tableSize) { return val % tableSize; });

            if (isActiveKey(keyPos) || isActiveVal(valPos))
                return false; // Key or value already exists

            if (keyTable[keyPos].info != HashEntryKey::DELETED)
                ++currentSize;

            keyTable[keyPos] = HashEntryKey(key, value, HashEntryKey::ACTIVE);
            valTable[valPos] = HashEntryVal(value, key, HashEntryVal::ACTIVE);

            if (currentSize > keyTable.size() / 2)
                rehash();

            return true;
        }

        bool removeKey(const string &key) {
            int keyPos = findPosKey(key, hashString);
            if (!isActiveKey(keyPos))
                return false;

            int value = keyTable[keyPos].value;
            int valPos = findPosVal(value, [](int val, size_t tableSize) { return val % tableSize; });

            keyTable[keyPos].info = HashEntryKey::DELETED;
            valTable[valPos].info = HashEntryVal::DELETED;

            return true;
        }

        int getValue(const string &key) const {
            int keyPos = findPosKey(key, hashString);
            if (!isActiveKey(keyPos))
                throw runtime_error("Key not found");
            return keyTable[keyPos].value;
        }

        string getKey(int value) const {
            int valPos = findPosVal(value, [](int val, size_t tableSize) { return val % tableSize; });
            if (!isActiveVal(valPos))
                throw runtime_error("Value not found");
            return valTable[valPos].value;
        }
    };

    //Heap class
    class Heap {
    private:
        struct HeapEntry {
            string taskid;
            int priority;
        };

        vector<HeapEntry> heapArray; //saves the taskid and priority for the heap class
        BiMap taskToIndex; // connects Heaap to BiMap
        
        // percolate functions
        void percolateUp(int index) {
            while (index > 0) {
                int parent = (index - 1) / 2;
                if (heapArray[index].priority >= heapArray[parent].priority)
                    break;
                swap(heapArray[index], heapArray[parent]);
                taskToIndex.insert(heapArray[index].taskid, index);
                taskToIndex.insert(heapArray[parent].taskid, parent);
                index = parent;
            }
        }

        void percolateDown(int index) {
            int child;
            int size = heapArray.size();
            while (2 * index + 1 < size) {
                child = 2 * index + 1;
                if (child + 1 < size && heapArray[child + 1].priority < heapArray[child].priority)
                    ++child;
                if (heapArray[index].priority <= heapArray[child].priority)
                    break;
                swap(heapArray[index], heapArray[child]);
                taskToIndex.insert(heapArray[index].taskid, index);
                taskToIndex.insert(heapArray[child].taskid, child);
                index = child;
            }
        }

    public:
        // Member functions
        Heap() = default; //empty constructor

        // member function to get the contents of heaparray for ddisplay
        const vector<HeapEntry>& getHeapArray() const {
            return heapArray;
        }
        
        // member function to get the contents of Bimap array for ddisplay
        const BiMap& getTaskToIndex() const {
            return taskToIndex;
        }

        //heap insert function saves the inserted values into heap array as the last element and call for the percolate up to order heap array and calls on insert for BiMap
        void insert(const string &taskid, int priority) {
            heapArray.push_back({taskid, priority});
            taskToIndex.insert(taskid, heapArray.size() - 1);
            percolateUp(heapArray.size() - 1);
        }

        //Removes and returns the lowest priority Task
        string deleteMin() {
            string minTask = heapArray[0].taskid;
            taskToIndex.removeKey(minTask);
            heapArray[0] = heapArray.back();
            heapArray.pop_back();
            if (!heapArray.empty()) {
                taskToIndex.insert(heapArray[0].taskid, 0);
                percolateDown(0);
            }
            return minTask;
        }

        //Changes the priority of a Task
        void updatePriority(const string &taskid, int newPriority) {
            int index = taskToIndex.getValue(taskid);
            heapArray[index].priority = newPriority;
            percolateUp(index);
            percolateDown(index);
        }

        bool isEmpty() const { return heapArray.empty(); }
        int size() const { return heapArray.size(); }
    };

    Heap heap; // connects indPQ class to Heap class

public:
    IndPQ() = default;//constructor

    //indPQ insert calls on Heap Insert
    void insert(const string &taskid, int priority) {
        heap.insert(taskid, priority);
    }

    //indPQ deletemin calls on Heap deleteMin
    string deleteMin() {
        return heap.deleteMin();
    }

    //indpq updatePriority calls on heap updatePriority
    void updatePriority(const string &taskid, int newPriority) {
        heap.updatePriority(taskid, newPriority);
    }

    //calls on heap isempty()
    bool isEmpty() const {
        return heap.isEmpty();
    }

    // calls on heap size()
    int size() const {
        return heap.size();
    }

    //calls on updatepriority and deletemin to remove a task
    void remove(const string &taskid) {
        heap.updatePriority(taskid, -1); // Set priority to -1 (lowest)
        heap.deleteMin(); // Remove the task
    }

    //calls on isEmpty() and deleteMin() to delete all entries
    void clear() {
        while (!heap.isEmpty())
            heap.deleteMin();
    }

    //display
    void display() const {
        cout << "Task ID\tPriority\n";
        for (const auto &entry : heap.getHeapArray()) {
            cout << entry.taskid << "\t" << entry.priority << endl;
        }
    }
    //ddisplay
    void ddisplay() const {
        cout << "Heap Array:\n";
        const auto& heapArray = heap.getHeapArray();
        for (size_t i = 0; i < heapArray.size(); ++i) {
            cout << "Index " << i << ": " << heapArray[i].taskid << " (" << heapArray[i].priority << ")\n";
        }
    
        cout << "\nTask to Index Map:\n";
        const auto& keyTable = heap.getTaskToIndex().getKeyTable();
        for (size_t i = 0; i < keyTable.size(); ++i) {
            cout << "Index " << i << ": ";
            if (heap.getTaskToIndex().getEntryState(i) == "ACTIVE") {
                cout << keyTable[i].key << " -> " << keyTable[i].value << " (ACTIVE)";
            } else {
                cout << heap.getTaskToIndex().getEntryState(i);
            }
            cout << endl;
        }
    }
};    