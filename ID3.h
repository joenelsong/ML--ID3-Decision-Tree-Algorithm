#ifndef ID3_H
#define ID3_H

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <math.h>
#include <set>
#include <iterator>
#include <fstream>


class ID3
{
public:
   // Constructors
   ID3(int num, std::string filename);
   virtual ~ID3();

   void ReadDataFile(int mode);
   int Branch(int pos, std::set<int> valid_data, std::set<int> valid_attributes);
   void WriteModel(int idx, char *filename);
   void WriteLine(int idx);
   void EvaluateModel();
   int EvalulateModel_Step(int pos, int i);
   

   //void set_NUM_ATTRIBUTES(int num);

    // step() // should be able to calculate the optimal root node
   // will then want to run step() on the left and ride side of the selected attribute and recursively keep running this to make a tree
   //Entropy()
   //Gain()

   //PrintTree()

public:
    std::string dataFileName;
    int NUM_ATTRIBUTES;
    std::string *attribute_names;
    std::vector<std::vector<int> > data;
    FILE *pFile;
  
    std::string *tree; // make array to store binary tree where root is idx[0] and left child = 2^depth and right child = 2^depth + 1
    std::string *tree_model_labels;
    //std::string tree[ (int)pow(2, NUM_ATTRIBUTES) ]; // make array to store binary tree where root is idx[0] and left child = 2^depth and right child = 2^depth + 1   
};



#endif