/*=========================================================================

  Program: ID3 algorithm
  Keywords: Machine Learning, Decision trees
  Author: Joey Nelson
  Date: 1/21/2016
  
  Psuedo (https://en.wikipedia.org/wiki/ID3_algorithm):

  ID3 (Examples, Target_Attribute, Attributes)
    Create a root node for the tree
    If all examples are positive, Return the single-node tree Root, with label = +.
    If all examples are negative, Return the single-node tree Root, with label = -.
    If number of predicting attributes is empty, then Return the single node tree Root,
    with label = most common value of the target attribute in the examples.
    Otherwise Begin
        A ← The Attribute that best classifies examples.
        Decision Tree attribute for Root = A.
        For each possible value, vi, of A,
            Add a new tree branch below Root, corresponding to the test A = vi.
            Let Examples(vi) be the subset of examples that have the value vi for A
            If Examples(vi) is empty
                Then below this new branch add a leaf node with label = most common target value in the examples
            Else below this new branch add the subtree ID3 (Examples(vi), Target_Attribute, Attributes – {A})
    End
    Return Root


    Example: ./id3 <train> <test> <model>
        <train> is the name of a file containing training data
        <test> contains test data to be labeled
        <model> is the filename where you will save the model for the decision tree.

  

=========================================================================*/
//
// This examples demonstrates the effect of specular lighting.
//

#include <ID3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <math.h>
#include <set>
#include <iterator>


//ID3::set_NUM_ATTRIBUTES(int num)
//{
//    NUM_ATTRIBUTES = num;
//}
ID3::ID3(int num, std::string filename)
{
    // Initialize variables
    NUM_ATTRIBUTES = num;
    dataFileName = filename;

    // Initialize attribute array
    attribute_names = new std::string[NUM_ATTRIBUTES];

    // Initialize tree array
    int treeSize = pow(2, NUM_ATTRIBUTES+1); // plus 1 so there are enough emptys to be found for termination conditions
    tree = new std::string[ treeSize ]; // make array to store binary tree where root is idx[0] and left child = 2^depth and right child = 2^depth + 1
    tree_model_labels = new std::string[ treeSize ];
    tree[0] = "ROOT";
    for (int i=1; i < treeSize; i++)
    {
        tree[i] = "EMPTY";
        tree_model_labels[i] = "EMPTY";
    }

    //fprintf(stderr, "%s\n", tree[0].c_str());
}
ID3::~ID3()
{   
    delete [] attribute_names;
    delete [] tree;
}


//*******************************************************************************************************************************************************************************************
//        I. Read Input File
//              i) Parse away header
//              ii) Parse actual data 
//*******************************************************************************************************************************************************************************************
void ID3::ReadDataFile(int mode)
{
    std::ifstream infile;
    infile.open(dataFileName); // open file name
  

    // i) Parse away header
    int i;
    std::string s = "";
    for (i =0 ; i < NUM_ATTRIBUTES-2; i++)
    {
        getline (infile, s, ','); // skip text header data
        attribute_names[i] = s;
        std::cout << s;
    }
    getline (infile, s, '\n'); // skip text header data
    attribute_names[i] = s;
    std::cout << s <<std::endl;


    // ii) Parse actual data 
    while ( !infile.eof() )
    {
        std::vector<int> row;

        // Parse actual data
        for (int p =0 ; p < NUM_ATTRIBUTES-1; p++)
        {
            getline (infile, s, ','); // skip text header data
            
            if (s == "") // prevent std:stoi on empty string from crashing
                break;
            row.push_back( std::stoi(s) );
            //std::cout << s;
        }
        getline (infile, s, '\n'); // skip text header data
        if (s == "") // prevent std:stoi on empty string from crashing
            break;
        row.push_back( std::stoi(s) );
        //std::cout << s <<std::endl;
        data.push_back(row);
    }

    if (mode == 0) // Training Mode
    {
        std::set<int> valid_data;
        for (int d = 0; d < data.size(); d++)
        {
            //fprintf(stderr, "inserting %d\n",i);
            valid_data.insert(d);
        }
        std::set<int> valid_attributes;
        for (int a = 0; a < NUM_ATTRIBUTES -2; a++)
        {
            valid_attributes.insert(a);
        }
        std::string root = "root";

        tree[0] = root;
        Branch(0, valid_data, valid_attributes);

    }
    else if (mode == 1)
    {
        EvaluateModel();
    }
    else {
        fprintf(stderr, "Invalid Mode Selection for ReadDataFile()");
        exit(-1);
    }

    

}
// ****************************************************************************
//  Function: Branch
//
//  Arguments:
//
//      valid_data: an array of integers that specify which rows of data can be used down this branch
//      valid_attributes: an array of integers that specify which attributes can be used down this branch
//      pos: needs to know what index to update on the tree so this refers to that      
//
//  Returns:  -1 for base cases; errors: -99.
//
// ****************************************************************************
int ID3::Branch(int pos, std::set<int> valid_data, std::set<int> valid_attributes)
{
    
    // Stuff we'll want for recursive Branch calls
    std::string returnValue = "ERROR_nothing_returned_string";
    std::set<int> new_split_1_set;
    std::set<int> new_split_0_set;

    // Error checking
    if (valid_data.empty())
    {
        //fprintf(stderr, "ERROR: valid_data is Empty!\n");
        return -1;
    }

    // Sum the classification total   
    int sum_class_yeses=0;
    //for (int t1 = 0; t1 <data.size()-1; t1++ )
    for (auto t1 : valid_data)
    {
        //std::cout<< data[t1][20];;
        sum_class_yeses += data[t1][NUM_ATTRIBUTES-1]; // classification attribute
    }

    // Test 1: If all examples are negative, Return the single-node tree Root, with label = -.
    if (sum_class_yeses == 0)
    {
         //fprintf(stderr, "all - \n");
        returnValue = tree[pos] + "0"; 
        //fprintf(stderr, "Modifying parent tree[%d] to %s \n", (2*pos), returnValue.c_str() );
        tree[pos] = returnValue;
        tree_model_labels[pos] = "Zero";

    }
    // Test 2: If all examples are positive, Return the single-node tree Root, with label = +.
    else if ( sum_class_yeses == valid_data.size())
    {
        //fprintf(stderr, "all + \n");
        returnValue = tree[pos] + "1";
        //fprintf(stderr, "Modifying parent tree[%d] to %s \n", (2*pos), returnValue.c_str() ); 
        tree[pos] = returnValue;
        tree_model_labels[pos] = "One";
    }
    // Test 3:  If number of predicting attributes is empty, then Return the single node tree Root, with label = most common value of the target attribute in the examples.
    else if ( valid_attributes.empty() ) // tests if attribute set is empty
    {
        //fprintf(stderr, "no attributes left \n");
        int ret = round( (float) sum_class_yeses / valid_data.size() );

        returnValue = tree[pos] + std::to_string(ret);
        //fprintf(stderr, "Modifying parent tree[%d] to %s \n", (pos), returnValue.c_str() );
        tree[pos] = returnValue; 
        if (ret == 1)
        {
            tree_model_labels[pos] = "One";
        }
        else
        {
            tree_model_labels[pos] = "Zero";
        }
        
    }
    else 
    {
        //*******************************************************************************************************************************************************************************************
        //        III. Calculate Info Gain for all attributes
        //              i) Calculate entropy for remaining set
        //              ii) Split data into true(1) and false(0) sets
        //              iii) Calculate entropy for true data
        //              iv) Calculate entropy for false data
        //              v) Calculate info gain
        //              
        //*******************************************************************************************************************************************************************************************
        
        ////              i) Calculate entropy for remaining set

        int total = (float)valid_data.size();
        int num_yes = sum_class_yeses;
        int num_no = total - sum_class_yeses; // num_yes previously calculated

        float class_entropy = (-1) * (float)num_yes/total * log2( (float)num_yes/total )  +  (-1) * (float)num_no/total * log2( (float)num_no/total ) ;

       // fprintf(stderr, "New class(%s) size :%d { yes:%d, no:%d }\n", tree[pos].c_str(), total, num_yes, num_no );
        //std::cout << "Entropy of class is " << class_entropy << std::endl;

        float max_info_gain = -99;
        int max_info_gain_idx = NUM_ATTRIBUTES-1;
        //fprintf(stderr, "valid attributes.size() = %d \n", valid_attributes->size());


        int split_1_yeses, split_0_yeses, split_1_nos, split_0_nos;

        for (auto atr_idx : valid_attributes ) // iterate through attributes
        {
            //std::cerr << "LINE#: " << __LINE__  << std::endl; // DEBUG // DEBUG // DEBUG // DEBUG // DEBUG // DEBUG // DEBUG // DEBUG // DEBUG // DEBUG  
            
        ////           ii) Split data into true(1) and false(0) sets

            std::vector<int> attr_split_1_set = std::vector<int>();
            std::vector<int> attr_split_0_set = std::vector<int>();

            std::set<int> temp_split_1_set;
            std::set<int> temp_split_0_set;

            num_yes = 0; // reset num yeses
            for (auto ri : valid_data) // iterate through data rows
            {
                //fprintf(stderr, "ri = %d\n", ri);
                num_yes+= data[ri][atr_idx];

                if (data[ri][atr_idx] == 1) // populate true set
                {
                    attr_split_1_set.push_back(data[ri][NUM_ATTRIBUTES-1]);
                    temp_split_1_set.insert(ri); // build new split data set
                }       
                else if (data[ri][atr_idx] == 0) // populate false set
                {
                   attr_split_0_set.push_back(data[ri][NUM_ATTRIBUTES-1]);
                   temp_split_0_set.insert(ri); // build new split data set
                }
                else
                {
                    fprintf(stderr, "data[ri][atr_idx] = %d\n",data[ri][atr_idx]);
                    fprintf(stderr, "ERROR DATA GOING TO NO SET\n");
                    exit(99);
                }
            }

            //std::cout << "split 1: " << attr_split_1_set.size() << " split 0: " << attr_split_0_set.size() << std::endl;


        ////              iii) Calculate entropy for true data

            split_1_yeses = 0;
            for (int i = 0; i < attr_split_1_set.size() ; i++)
            {
                split_1_yeses += attr_split_1_set[i];
            }
            split_1_nos = attr_split_1_set.size() - split_1_yeses;

            //fprintf(stderr, "Split_1:%d { yes:%d, no:%d }\n", attr_split_1_set.size(), split_1_yeses, split_1_nos );

            // calculate split_1_entropy
            float first_part;
            float second_part; 
            float split_1_entropy;


            if (split_1_yeses == 0 || attr_split_1_set.size() == 0){
                first_part = 0;
            }
            else {
                first_part = (-1) * (float)split_1_yeses/attr_split_1_set.size() * log2( (float)split_1_yeses/attr_split_1_set.size() );
            }
            
            if (split_0_yeses == 0 || attr_split_1_set.size() == 0) {
                second_part = 0;       
            }
            else {
                second_part = (-1) * (float)split_1_nos/attr_split_1_set.size() * log2( (float)split_1_nos/attr_split_1_set.size() );
            }        

            split_1_entropy =  first_part + second_part;
            
            //fprintf(stderr, "split_1_entropy = %.5f\n",split_1_entropy);



        ////              iv) Calculate entropy for false data

            split_0_yeses = 0;
            for (int i = 0; i < attr_split_0_set.size() ; i++)
            {
                split_0_yeses += attr_split_0_set[i];
            }
            split_0_nos = attr_split_0_set.size() - split_0_yeses;
            
            //fprintf(stderr, "Split_0:%d { yes:%d, no:%d }\n", attr_split_0_set.size(), split_0_yeses, split_0_nos );

            // calculate split_0_entropy
            float split_0_entropy;

            if (split_0_yeses == 0 || attr_split_0_set.size() == 0) {
                first_part = 0;
            }
            else {
                first_part = (-1) * (float)split_0_yeses/attr_split_0_set.size() * log2( (float)split_0_yeses/attr_split_0_set.size() );
            }
            if (split_0_nos == 0 || attr_split_0_set.size() == 0) {
                second_part = 0;
            }
            else {
                second_part = (-1) * (float)split_0_nos/attr_split_0_set.size() * log2( (float)split_0_nos/attr_split_0_set.size() );
            }            

            split_0_entropy = first_part + second_part;
            
            
            //fprintf(stderr, "split_0_entropy = %.5f\n",split_0_entropy);
           

        ////              v) Calculate info gain

            first_part = (-1) * (float)attr_split_1_set.size()/total * split_1_entropy; 
            second_part =  (-1) * (float)attr_split_0_set.size()/total * split_0_entropy; 

            float info_gain = class_entropy + first_part + second_part;
            //fprintf(stderr, "attribute( %s ) infogain = %.5f\n", attribute_names[atr_idx].c_str(), info_gain);

            if (info_gain > max_info_gain)
            {
                max_info_gain = info_gain;
                max_info_gain_idx = atr_idx;
                new_split_1_set = temp_split_1_set;
                new_split_0_set = temp_split_0_set;
            }

        } // end for loop through attributes

        if (max_info_gain == -99)
        {
            fprintf(stderr, "ERROR: max_info_gain was not set!: \n");
            return -99;
        }
        //fprintf(stderr, "Max info gain is ( %s ) at = %.3f\n", attribute_names[max_info_gain_idx].c_str(), max_info_gain);
        


        //std::set<int> new_valid_data = valid_data; 
        //std::set<int> valid_attributes

        // Recurse down the left side of tree i.e. the 1 or True side
        returnValue = attribute_names[max_info_gain_idx] + " = 1 : ";
        //fprintf(stderr, "Writing: %s to left branch @ tree[%d]\n",returnValue.c_str(), (2*pos+1) );
        tree[2*pos+1] = returnValue;
        tree_model_labels[2*pos+1] = attribute_names[max_info_gain_idx];

        int new_pos_1 = 2*pos+1;
        std::set<int> new_attribute_set = valid_attributes;
        new_attribute_set.erase(max_info_gain_idx);

        //if (new_split_1_set.empty())
        int recurse_right_test = Branch(new_pos_1, new_split_1_set, new_attribute_set);


        // Recurse down the right side of tree i.e. the 0 or False side
        if (recurse_right_test == -1)
        { 
            // Terminate Early
            return 1;
        }
        else
        {
            // Traverse Right
            returnValue = attribute_names[max_info_gain_idx] + " = 0 : ";
            //fprintf(stderr, "Writing %s to right branch @ tree[%d]\n", returnValue.c_str(), (2*pos+2) );
            tree[2*pos+2] = returnValue;
            tree_model_labels[2*pos+2] = attribute_names[max_info_gain_idx]; 

            int new_pos_0 = 2*pos+2;

            Branch(new_pos_0, new_split_0_set, new_attribute_set);
        }

        return 0;
         
    } // end else

    return -1; // -1 means returned from base condition
}

void ID3::WriteModel(int idx, char *output_file_name)
{
    //fs.open(output_file_name, std::fstream::out | std::fstream::app);
    pFile = fopen (output_file_name, "w");
    WriteLine(idx);
    fclose (pFile);

}
void ID3::WriteLine(int idx)
{
    std::string pad = std::string(log2(idx+1), '|');
    fprintf(pFile, "%s%s\n", pad.c_str(), tree[idx].c_str());

    if ( strcmp(tree_model_labels[2*idx+1].c_str(), "EMPTY") != 0)
    {
        //fprintf(stderr, "Writing: tree[%d]\n", (2*idx+1));
        WriteLine(2*idx+1);
    }
    if (strcmp(tree[2*idx+2].c_str(), "EMPTY") != 0)
    {
        //fprintf(stderr, "Writing: tree[%d]\n", (2*idx+2));
        WriteLine(2*idx+2);
    }
    return;
}

void ID3::EvaluateModel()
{
    int sum_correct_answers = 0;
    for (int i = 0; i<data.size(); i++)
    {
        sum_correct_answers = EvalulateModel_Step(0, i)

        fprintf(stderr, "Percentage Correct = (%d/%d)\n", sum_correct_answers, data.size());
    }
}


int ID3::EvalulateModel_Step(int pos, int i)
{
    // base case: final value 1
    std::string attrib_or_value = tree_model_labels[pos];
    if ( strcmp(attrib_or_value, "One") == 0)
    {
        if (data[i][NUM_ATTRIBUTES-1] == 1)
        return 1;
    }
    // base case: final value is 0
    else if ( strcmp(attrib_or_value, "Zero") == 0)
    {
        if (data[i][NUM_ATTRIBUTES-1] == 0)
        return 0;
    }
    // Go to left child
    else if ( ddata[i][std::stoi(attrib_or_value)] == 1 )
    {
        EvaluateModel(2*pos+1, 1);
    }
    else if ( ddata[i][std::stoi(attrib_or_value)] == 0 )
    {
        EvaluateModel(2*pos+2, 1);
    }
    return 0;
}



int main(int argc, char *argv[])
{
    ID3 test(21, argv[1]);
    test.ReadDataFile(0); // 0 for training mode
    test.WriteModel(0, argv[3]);
    
    return 0;
}