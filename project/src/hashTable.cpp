#include "hashTable.hpp"

template <typename TKey>
class ExtendibleHash
{
private:
    string fileN;
    string indexN;
    unordered_map<string, long> index;
    unordered_map<string, long> indexSize;
    hash<TKey> _hash;
    long freeListBucket = -1;
    string hashFunction(TKey key)
    {
        auto hash_code = _hash(key);
        string hashCodeString = bitset<HASH_HEIGHT>(hash_code).to_string();
        return hashCodeString;
    }
    void initIndex();
    void readIndex();
    void writeIndex();
    void insertInFreeList(fstream &f, int &accesos);
    void divideBucket(fstream &f, Bucket<TKey> &bucket, long pos, int &accesos);
   //--mas funciones--
   //.
   //.
    public:
     ExtendibleHash() {}
     ~ExtendibleHash() {}
     void showall() {}
     RecordHash<TKey> *search(TKey key, int &accesos) {}
     void insert(RecordHash<TKey> record, int &accesos) {}
     bool erase(TKey key) {}
  // -- mas funciones--
};
