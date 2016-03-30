#include "InvertedIndex.h"

#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <mutex>
#include <math.h>



using namespace std;

mutex InvertedIndex::printMutex;

mutex InvertedIndex::atoiMutex;

InvertedIndex::InvertedIndex(int noTotalDocs,bool supportsExactMatching)
{
    docsMaxFreq.resize(noTotalDocs + 1,0);
    docsMagnitudes.resize(noTotalDocs+ 1,0);
    this->supportsExactMatching = supportsExactMatching;

}

InvertedIndex::~InvertedIndex()
{
    for( unordered_map<string,list<Node>*>::iterator mapIt = dictionary.begin(); mapIt != dictionary.end(); ++mapIt)
    {
        delete mapIt->second;
    }
    dictionary.clear();
}


void InvertedIndex::add(string word, int documentID, int posInDoc)
{
    //If the word doesnt exist, add it to the Inverted Index and add the document to the word's list of documents
    if( dictionary[word] == nullptr)
    {
        dictionary[word] = new list<Node>();
        Node n;
        n.docID = documentID;
        n.freq = 1;
        n.TF = 0;
        if(supportsExactMatching){
            n.positions.push_back(posInDoc);
        }
        dictionary[word]->push_back(n);
        return;
    }

    //If the document doesnt exist in the word's list of document(at the end of the list), add it
    if(dictionary[word]->back().docID != documentID)
    {
        Node n;
        n.docID = documentID;
        n.freq = 1;
        n.TF = 0;
        if(supportsExactMatching){
            n.positions.push_back(posInDoc);
        }
        dictionary[word]->push_back(n);
        return;
    }


    //If the document exists in the word's list of document(at the end of the list),
    //then increase the word's frequency in the document and add the word's position
    if(dictionary[word]->back().docID == documentID)
    {
        dictionary[word]->back().freq++;
        if(supportsExactMatching) {
            dictionary[word]->back().positions.push_back(posInDoc);
        }
    }
}


void InvertedIndex::addDocument(string documentLine)
{
    //find the document ID
    int c=0;
    while(documentLine[c] != ' ')
    {
        c++;
    }

    int docID = 0;
    for(int i = 0; i < c ; i++){
        docID = docID * 10;
        docID += documentLine[i] - '0';
    }

    docsMaxFreq[docID] = 1;
    //parse string

    for(int i = 0 ; i < documentLine.length(); i++)
    {
        //If lowercase or space, continue to next character
        if((documentLine[i] >= 'a' && documentLine[i] <= 'z') || documentLine[i] == ' ')
        {
            continue;
        }
        //If uppercase, convert to lowercase
        if(documentLine[i] >= 'A' && documentLine[i] <= 'Z')
        {
            documentLine[i] += 32;
            continue;
        }
        //Else convert to space character
        documentLine[i] = ' ';
    }




    istringstream iss(documentLine);
    vector<string> tokens;
    copy(istream_iterator<string>(iss),istream_iterator<string>(),back_inserter(tokens));


    for(int i = 0 ; i < tokens.size() ; i++)
    {
        this->add(tokens[i],docID,i);
    }
}



void InvertedIndex::joinIndex(InvertedIndex *otherIndex)
{

    if(otherIndex->dictionary.size() == 0) return;

    for( unordered_map<string,list<Node>*>::iterator mapIt = otherIndex->dictionary.begin(); mapIt != otherIndex->dictionary.end(); ++mapIt)
    {
        //Get the word
        string word = mapIt->first;

        if( dictionary[word] == nullptr)
        {
            dictionary[word] = new list<Node>();
        }

        //Add to the list's end of this dictionaries word
        dictionary[word]->splice(dictionary[word]->end(), *(mapIt->second));
    }


    //Transfer the documents Max frequences
    for(int i = 0; i < otherIndex->docsMaxFreq.size(); i++)
    {
        if(otherIndex->docsMaxFreq[i] > 0)
        {
            docsMaxFreq[i] = otherIndex->docsMaxFreq[i];
        }
    }
}


void InvertedIndex::calculateDocumentsMaximumFrequency()
{
    //Calculate the maximum raw frequency of any term for each document
    for( unordered_map<string,list<Node>*>::iterator mapIt = dictionary.begin(); mapIt != dictionary.end(); ++mapIt)
    {
        list<Node> *documentEntries = mapIt->second;
        //Gia ka8e document pou vre8ike h leksh
        for(list<Node>::iterator listIt = documentEntries->begin(); listIt != documentEntries->end(); ++listIt)
        {
            if(docsMaxFreq[listIt->docID] < listIt->freq)
            {
                docsMaxFreq[listIt->docID] = listIt->freq;
            }
        }
    }
}



void InvertedIndex::calculateTF(){
    calculateDocumentsMaximumFrequency();

    for( unordered_map<string,list<Node>*>::iterator mapIt = dictionary.begin(); mapIt != dictionary.end(); ++mapIt)
    {
        list<Node> *documentEntries = mapIt->second;

        //Gia ka8e document pou vre8ike h leksh
        for(list<Node>::iterator listIt = documentEntries->begin(); listIt != documentEntries->end(); ++listIt)
        {
            int docID = listIt->docID;
            listIt->TF = 1.0 * listIt->freq / docsMaxFreq[docID];
        }
    }
}






void InvertedIndex::finalize()
{
    //Calculate the IDF of each word and the increase the sum of each document's vector
    for( unordered_map<string,list<Node>*>::iterator mapIt = dictionary.begin(); mapIt != dictionary.end(); ++mapIt)
    {
        string word = mapIt->first;
        int noDocsThatContainWord = mapIt->second->size();

        IDF[mapIt->first] = log(1.0 * (docsMagnitudes.size() - 1)/noDocsThatContainWord);

        list<Node> *documentEntries = mapIt->second;
        //Increase the mangnitude of all the documents that contain the word magnitude
        for(list<Node>::iterator listIt = documentEntries->begin(); listIt != documentEntries->end(); ++listIt)
        {
            int docID = listIt->docID;
            float tmp = listIt->TF * IDF[word];
            docsMagnitudes[docID] += tmp * tmp;
        }
    }

    //sqrt of sum
    for(int i = 1; i < docsMagnitudes.size(); i++){
        docsMagnitudes[i] = sqrt(docsMagnitudes[i]);
    }
}




vector<pair<float,int>> InvertedIndex::executeQuery(int queryID,int k,string queryText){
    vector<pair<float,int>> results;




    return results;
}





void InvertedIndex::calculateDocumentsVectors(vector<unordered_map<string,float>> &documentsVectors)
{

    for( unordered_map<string,list<Node>*>::iterator mapIt = dictionary.begin(); mapIt != dictionary.end(); ++mapIt)
    {
        string word = mapIt-> first;
        list<Node> *documentEntries = mapIt->second;

        //Gia ka8e document pou vre8ike h leksh
        for(list<Node>::iterator listIt = documentEntries->begin(); listIt != documentEntries->end(); ++listIt)
        {
            int freq  = listIt->freq;
            int docID = listIt->docID;
            float tf = (1.0 * freq) / docsMaxFreq[docID];

            float idf = docsMaxFreq.size() - 1;
            idf = idf / documentEntries->size();
            idf = log(idf);
            /*
            if(documentsVectors[docID] == nullptr){
                documentsVectors[docID] = new unordered_map<string,float>();
                cout<<"New"<<endl;
            }else{
                cout<<"No"<<endl;
            }*/
            //if(documentsVectors[docID][word] == 0) cout<<"NULL";
            documentsVectors[docID][word] = tf * idf;
            //unordered_map<string,float> *themap = documentsVectors[docID];
            //(*themap)[word] = tf * log(tmp);
        }
    }
}






void InvertedIndex::print()
{

    cout<<"Inverted Index:"<<endl;
    cout<<"Number Of Words: "<<dictionary.size()<<endl;
    int noDocs = 0;
    for(int i=0; i < docsMaxFreq.size() ; i++)
    {

        if(docsMaxFreq[i] != 0)
        {
            //cout<<i<<",";
            noDocs++;
        }
    }
    cout<<endl;
    cout<<"Number Of Documents: "<<noDocs<<endl;



    for( unordered_map<string,list<Node>*>::iterator mapIt = dictionary.begin(); mapIt != dictionary.end(); ++mapIt)
    {

        cout<<mapIt->first<<"-->  ";

        for(list<Node>::iterator listIt = mapIt->second->begin(); listIt != mapIt->second->end(); ++listIt )
        {
            cout<<listIt->docID<<" "<<" "<<listIt->TF<<" "<<listIt->freq<<":";
            cout<<"(";
            for(list<int>::iterator posIt = listIt->positions.begin(); posIt != listIt->positions.end(); ++posIt)
            {
                cout<<*posIt<<",";
            }
            cout<<") | ";


        }
        cout<<endl;
    }


    for(int i = 0; i < docsMaxFreq.size() ; i++)
    {
        //cout<< docsMaxFreq[i] << ',';
    }
    cout<<endl;


    for(int i = 1; i < docsMaxFreq.size() ; i++)
    {
        //cout<<i<<" -> "<<docsMagnitudes[i] << endl;
    }
    cout<<endl;
}







