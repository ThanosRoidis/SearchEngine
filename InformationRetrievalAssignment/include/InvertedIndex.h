#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H


#include <utility>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <mutex>

using namespace std;

typedef struct Node{
    int docID;

    float TF;

    int freq;

    list<int> positions;

} Node;


class InvertedIndex
{
    private:

        bool supportsExactMatching;

        unordered_map<string,list<Node>*> dictionary;

        unordered_map<string,float> IDF;

        vector<unsigned short> docsMaxFreq;

        vector<float> docsMagnitudes;

    public:

        static mutex printMutex;

         static mutex atoiMutex;

        InvertedIndex(int noTotalDocs,bool supportsExactMatching);

        virtual ~InvertedIndex();

        void add(string word, int documentID,int position);

        void addDocument(string documentLine);

        void calculateTF();

        void calculateDocumentsMaximumFrequency();

        void calculateDocumentsVectors(vector<unordered_map<string,float>> &documentsVector);

        void joinIndex(InvertedIndex *otherIndex);

        vector<pair<float,int>> executeQuery(int queryID,int k,string queryText);

        void finalize();

        void storeToFile(string file);

        void print();


};

#endif // INVERTEDINDEX_H
