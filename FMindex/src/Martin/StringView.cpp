#include "StringView.hpp"

#include <algorithm>

using namespace std;

StringView::StringView(const string& s) : viewedString(s)
{
    viewBegin = 0;
    length = s.length();
    viewEnd = viewBegin+length-1;
    prefix = suffix = "";
    reversed = false;
}

StringView::StringView(const string& s, Index start, Index length) : viewedString(s)
{
    viewBegin = start;
    this->length = length;
    viewEnd = viewBegin+length-1; 
    prefix = suffix = "";
    reversed = false;
}

void StringView::addPrefix(const string& prefix)
{
    this->prefix = prefix + this->prefix;
    length += prefix.length();
}

void StringView::addSuffix(const string& suffix)
{
    this->suffix = this->suffix + suffix;
    length += suffix.length();
} 
    
const char& StringView::charAt(Index i)
{
    if (i < prefix.length())
        return prefix[i];
    if (i >= length - suffix.length())
        return suffix[i - (length-suffix.length())];
    if (!reversed)
        return viewedString[viewBegin + i - prefix.length()];
    else
        return viewedString[viewEnd - (i-prefix.length())];
}

Index StringView::getLength()
{
    return length;
}

void StringView::reverse()
{
    reversed = !reversed;
    string oldPrefix = prefix;
    prefix = suffix;
    suffix = oldPrefix;
    std::reverse(prefix.begin(), prefix.end());
    std::reverse(suffix.begin(), suffix.end());
}



#include <iostream>

int main()
{
    string s = "012345";
    StringView view = StringView(s, 1, s.length()-1);
    
    view.addPrefix("abcd");
    view.addPrefix("z");
    view.addSuffix("efg");
    view.reverse();
    
    for (int i = 0; i < view.getLength(); i++)
        cout << view.charAt(i) << endl;
    return 0;
}
