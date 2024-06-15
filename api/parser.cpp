#include "parser.h"
#include <iostream>
#include <cstdio>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h" 
#include "rapidjson/error/en.h"

using namespace rapidjson;
using namespace std;

vector<pair<string, string>> parse(const string& input, const vector<string>& values) {
    vector<pair<string, string>> result;
  
    // Parse the JSON document 
    rapidjson::Document doc; 
    doc.Parse(input.c_str()); 
  
    // Check if the document is valid 
    if (doc.HasParseError()) { 
        std::cerr << "Error: failed to parse JSON" << GetParseError_En(doc.GetParseError())
                  << std::endl; 
        return result; 
    } 

   /* for (int i; i < values.size(); i++) {
        auto value = values[i].c_str();
        if (doc.HasMember(value) && )
    }*/

    return result;
}
