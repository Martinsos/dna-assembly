/**
 * @author Martin Sosic (sosic.martin@gmail.com)
 */

#ifndef STRINGVIEW_HPP
#define STRINGVIEW_HPP

#include <string>

using namespace std;

typedef int Index;

/**
 * StringView is slightly changed view of another string with minimum copying involved.
 * StringView can not affect or change viewed string.
 * We can add suffix, preffix, or limit our view to substring of viewed string.
 * When making a view of another string, no copying is done.
 * When adding prefix or suffix they are actually stored, so prefix and suffix should not be big.
 */
class StringView
{
  public:
    /**
     * Create a view of given string.
     * Space and time complexity: O(1).
     */
    StringView(const string& s);
    /**
     * Create a view of given string, starting from character at position
     * start and ending at position start+length-1, inclusive.
     * Space and time complexity: O(1).
     */
    StringView(const string& s, Index start, Index length);
    
    /**
     * Add prefix. Prefix is actually copied and stored in StringView.
     * After prefix is added, rewind is automatically called.
     */
    void addPrefix(const string& prefix);
    /**
     * Add suffix. Suffix is actually copied and stored in StringView.
     * After prefix is added, rewind is automatically called.
     */
    void addSuffix(const string& suffix);
    
    /**
     * Returns char at position i. Positions start from 0, from first character of prefix.
     */
    char charAt(Index i);
    
    /**
     * Returns length of preffix, viewed string and suffix together.
     * Time complexity: O(1);
     */
    Index getLength();
    
    /**
     * Reverse view. Only prefix and suffix are really reversed (and swapped).
     */
    void reverse();

  private:
    const string& viewedString;
    Index viewBegin, viewEnd; /* viewBegin is position of first character that we watch in viewedString, viewBegin is position of last character that we watch in viewedString */
    Index length;
    bool reversed; /* true if viewedString should be reversed */
    string prefix;
    string suffix;
};

#endif // STRINGVIEW_HPP
