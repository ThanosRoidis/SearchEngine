#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <InvertedIndex.h>
#include <sys/time.h>

using namespace std;



mutex documentsMutex;
ifstream input;

volatile int documentsCounter;
volatile int documentsSize;

int previousPercent;

string getNextDocument()
{
    string line;

    documentsMutex.lock();
    if(documentsCounter >= documentsSize)
    {
        line = "";
    }
    else
    {
        std::getline(input, line);
        documentsCounter++;

        //print percent
        int percent = (int)(documentsCounter*100.0/documentsSize);
        //if(previousPercent != percent){
        previousPercent = percent;
        cout<<"\b\b\b";
        if(percent < 10) cout<<'0';
        cout<<percent<<"%";
        if(percent == 100) cout<<endl;
        //}

    }
    documentsMutex.unlock();

    return line;
}







void buildInvertedIndex(InvertedIndex *index)
{
    string document = getNextDocument();
    while(document != "")
    {
        index->addDocument(document);
        document = getNextDocument();
    }
    index->calculateTF();
}





void joinThread(thread& t)
{
    t.join();
}

void joinAllThreads(std::vector<std::thread>& threads)
{
    for_each(threads.begin(),threads.end(),joinThread);
}



void testFunction(unordered_map<int,float> *test_map,int pos)
{

    for(int j=pos; j < 80; j+=8)
    {
        for(int i=0; i < 10000000; i++)
        {
            (*test_map)[j] = (*test_map)[j] + 1;
        }
    }
}




int main()
{

    input.open("documents/documents50.txt");
    std::string line;
    std::getline(input, line);
    documentsSize = atoi(line.c_str());
    documentsCounter = 0;
    previousPercent = 0;

    cout<<"Total Documents: "<<documentsSize<<endl;

    int noConcurrentThreads = 8;//thread::hardware_concurrency();
    cout << noConcurrentThreads << " concurrent threads are supported.\n";



    cout<<"Building Inverted Index: 00%";






    vector<InvertedIndex*> indexes(noConcurrentThreads);
    for(int i = 0 ; i < noConcurrentThreads; i++)
    {
        indexes[i] = new InvertedIndex(documentsSize,true);
    }



    struct timeval startTime,endTime;
    gettimeofday(&startTime,NULL);


    vector<thread> threads(noConcurrentThreads);

    unordered_map<int,float> test_map;
    for(int i = 0; i < 80 ; i++){
        test_map[i] = 0;
    }



    for(int i=0; i<threads.size(); ++i)
    {
        threads[i] = thread(testFunction,&test_map,i);//thread(buildInvertedIndex,indexes[i]);
    }

    joinAllThreads(threads);


    for(unordered_map<int,float>::iterator mapIt = test_map.begin(); mapIt != test_map.end(); ++mapIt)
    {
        cout<<mapIt->first<<" -> "<<mapIt->second<<endl;
    }



    input.close();
    /*
    //print indexes

    cout<<endl;
    for(unsigned i = 0 ; i < indexes.size(); i++)
    {
        indexes[i]->print();
        cout<<endl;
    }



    //join all the indexes to the first index in indexes[0]
    for(int i=1; i<indexes.size(); i++)
    {
        indexes[0]->joinIndex(indexes[i]);
    }
    indexes[0]->finalize();


    gettimeofday(&endTime,NULL);

    long startTotalMicro = startTime.tv_sec * 1000000 + startTime.tv_usec;

    long endTotalMicro = endTime.tv_sec * 1000000 + endTime.tv_usec;

    long microseconds = endTotalMicro - startTotalMicro ;

    double t = microseconds / 1000000.0;


    cout<<"Total Time: "<< t <<endl;
    //indexes[0]->print();


    input.open("queries.txt");
    int totalQueries;

    input>>totalQueries;

    for(int i=0; i < totalQueries ; i++){
        int queryID,k;
        char dummy;
        string queryText;
        input>>queryID>>k;//>>dummy;
        input.get(dummy);
        std::getline(input,queryText);


        cout<<queryID<<endl;
        cout<<k<<endl;
        cout<<queryText<<endl;

        vector<pair<float,int>> results = indexes[0]->executeQuery(queryID,k,queryText);
        cout<<"Top "<<k<<" results are:"<<endl;
        for(int j = 0; j < results.size() ; j++){
            cout<<results[j].second<<" "<<results[j].first<<endl;
        }

    }





    /*
        //Calculate the documents Vectors
        vector<unordered_map<string,float>> documentsVectors(documentsSize+1);
        indexes[0]->calculateDocumentsVectors(documentsVectors);

        for(unordered_map<string,float>::iterator mapIt = documentsVectors[5].begin(); mapIt != documentsVectors[1].end(); ++mapIt)
        {
            cout<<mapIt->first<<"->"<<mapIt->second<<endl;
        }
    */

}
