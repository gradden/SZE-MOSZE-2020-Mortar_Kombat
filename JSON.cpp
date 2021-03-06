#include "JSON.h"

//INSTRUCTIONS: Specify mode = "inclusive" for an allowed character set. Exclusive is default.
std::string::size_type findNext(std::string &s, char target, std::unordered_set<char> set, std::string mode = "")
{
    long pos = s.find(target);
    if (pos == -1)
    {
        throw std::runtime_error("1: Expected token not found!");
        return -1;
    }

    if (mode == "inclusive")
    {
        for (char c : s.substr(0, pos))
        {
            if (set.find(c) == set.end())
            {
                throw std::runtime_error("2: Unexpected token!");
                return -1;
            }
        }
    }
    else
    {
        for (char c : s.substr(0, pos))
        {
            if (set.find(c) != set.end())
            {
                throw std::runtime_error("3: Unexpected token!");
                return -1;
            }
        }
    }
    return pos;
}

void checkString(std::string& s)
{
    if (s.length() == 0) //only two '"' signs --> error
    {
        throw std::runtime_error("4: Empty string!");
    }
    std::unordered_set<char> backslashChars = {'"', '\\'};
    for (std::string::size_type pos = 0; pos < s.length(); pos++)
    {
        if (backslashChars.find(s[pos]) != backslashChars.end() && s[pos - 1] != '\\') //backslashchar has no '\' before it --> error
        {
            throw std::runtime_error("5: Unrecognized value!");
        }
        if (s[pos] == '{' || s[pos] == '}') //these mókusos brackets are not allowed --> error - (Names for various bracket symbols, [https://en.wikipedia.org/wiki/Bracket])
        {
            throw std::runtime_error("6: Unrecognized value!");
        }
    }
}

std::any string2any(std::string& s)
{
	if (s[0] == '"') //starts with '"' --> can be string
	{
		if (s[s.length() - 1] != '"') //does not end with '"' --> error
		{
			throw std::runtime_error("7: Unrecognized value!");
			return -1;
		}
		s.erase(0, 1).erase(s.length() - 1); //remove '"' signs
		checkString(s);
		return s; //is string
	}

	else if (s == "true") return true; //is boolean
	else if (s == "false") return false; //is boolean
	else if (s == "null") return nullptr; //is null pointer

    else //all correct non-numeric types covered --> can be numeric
    {
        std::unordered_set<char> numChars = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '-'};
        for (char c : s.substr(1, s.length() - 1))
        {
            if (numChars.find(c) == numChars.end()) //has non numeric chars -> error
            {
                throw std::runtime_error("8: Unrecognized value!");
                return -1;
            }
        }
        return stof(s); //is numeric
    }
}

std::map<std::string, std::any> parseString(std::string& s)
{
    std::map<std::string, std::any> map;
    std::string key, valueString;
    std::any value;
    std::string::size_type pos;
    std::unordered_set<char> spacingChars = {' ', '\n', '\t', '\r', '\v'}; //whitespace + nonprintables
    std::unordered_set<char> pairEndingChars = {',', '}'};

    s.erase(0, findNext(s, '{', spacingChars, "inclusive") + 1); //remove first '{'
    bool thereIsMore;
    do
    {
        s.erase(0, findNext(s, '"', spacingChars, "inclusive") + 1); //remove key starting '"'
        pos = findNext(s, '"', {'{', '}'}); //find key closing '"'
        key = s.substr(0, pos); //pass key
        checkString(key);
        if (map.find(key) != map.end()) //key already exists
        {
            throw std::runtime_error("9: Duplicate keys!");
        }
        s.erase(0, pos + 1); //remove key

        s.erase(0, findNext(s, ':', spacingChars, "inclusive") + 1); //remove ':' operator
        pos = 0; //reset position
        while(spacingChars.find(s[pos]) != spacingChars.end() && pos < s.length()) pos++; //find value begginging (or incorrect content end)
        if (pairEndingChars.find(s[pos]) != pairEndingChars.end()) //':' is followed directly by ',' or '}'
        {
            throw std::runtime_error("10: No value found!");
        }
        s.erase(0, pos); //remove everything before value
		
	if (s[0] == '"') //starts as a string
	{
		pos = s.substr(1, s.length()).find('"'); //skip inside of string
	}
	else
	{
		pos = 0; //just reset position
	}
	while (pairEndingChars.find(s[pos]) == pairEndingChars.end() && pos < s.length()) pos++; //find value end if non-string (or incorrect content end)
        valueString = s.substr(0, pos); //pass value string
        s.erase(0, pos); //remove everything before value

        while(spacingChars.find(valueString[valueString.length() - 1]) != spacingChars.end()) //last character is spacing character
        {
            valueString = valueString.substr(0, valueString.size() - 1); //remove last character
        }
        value = string2any(valueString); //give value the real type

        map.insert(make_pair(key, value)); //add pair to map

        thereIsMore = s[0] == ',' ? true : false; //check for more data (',')
        if (thereIsMore)
        {
            s.erase(0, 1); //remove ','
        }
    }
    while (thereIsMore); //listing continues

    s.erase(0, findNext(s, '}', spacingChars, "inclusive") + 1); //find closing '}'
    pos = 0;
    while (s[pos] != '\0') //check for non correct chars in remaining part
    {
        if (spacingChars.find(s[pos]) == spacingChars.end())
        {
            throw std::runtime_error("11: Unexpected token!");
        }
        pos++;
    }
    return map;
}
    
std::map<std::string, std::any> JSON::parse(std::string inputString, bool isFile)
{
    if (isFile)
    {
        std::ifstream file(inputString);
        if (file.good())
        {
            std::string line;
            inputString = "";
            while (getline(file, line))
            {
                inputString += line;
            }
        }
        else
        {
            throw std::runtime_error("12: Unable to open file!");
        }
    }
    return parseString(inputString);
}

std::map<std::string, std::any> JSON::parse(std::istream &stream)
{
    std::istreambuf_iterator<char> eos;
    std::string s(std::istreambuf_iterator<char>(stream), eos);
    return parse(s);
}

JSON JSON::parseFromFile(std::string fileName)
{
	return(JSON(JSON::parse(fileName, true)));
}
