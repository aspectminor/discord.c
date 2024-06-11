#include "parser.h"
#include <iostream>
#include <cstdio>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace std;

string parse(const string& input, const vector<string>& values) {
    Document d;
    d.Parse(input.c_str());

    for (auto& value : values) {
        if (d.HasMember(value.c_str()) && d[value.c_str()].IsInt()) {
            int intValue = d[value.c_str()].GetInt();
            d[value.c_str()].SetInt(intValue + 1);
        }
    }

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    return buffer.GetString();
}
