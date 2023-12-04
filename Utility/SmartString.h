//
// Created by molberding on 9/15/2017.
//

#ifndef UTILITYCODE_SMARTSTRING_H
#define UTILITYCODE_SMARTSTRING_H

#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <array>
#include <cmath>
#include <map>
#include <algorithm>
#include <utility>
#include <iterator>
#include <type_traits>


namespace Utilities
{
    class SmartString
    {
    private:
        template<typename From, typename To, typename = void>
        struct is_castable : std::false_type { };

        template<typename From, typename To>
        struct is_castable<From, To, typename std::enable_if<std::is_convertible<
        decltype(static_cast<To>(std::declval<From>())), To>::value>::type>
        : std::true_type { };

        constexpr static int DEFAULT_PRECISION = 5;

        std::string backingString;
        int precision;

        /*!
         * @brief Converts a single to digit to its character equivalent. This assumes
         *        \p digit is a single digit number. Use at your own risk.
         * @param digit - Single digit number to convert
         * @return The character equivalent of \p digit.
         */
        static inline char digitToChar(const unsigned int digit)
        {
            return static_cast<char>(digit + static_cast<unsigned int>('0'));
        }

        /*!
         * @brief Converts a given character to its numerical equivalent. This assumes
         *        \p c is a character between '0' and '9'. Use at your own risk.
         * @param c - character to convert
         * @return The numerical equivalent of \p c
         */
        static inline unsigned int charToDigit(const char c)
        {
            return static_cast<unsigned int>(c) - static_cast<unsigned int>('0');
        }

        /*!
         * @brief Finds the number of format arguments present in the current string.
         *        Format arguments are of the form {0}, {1}, etc.
         * @return The number of format arguments
         */
        [[nodiscard]] int getNumArguments() const
        {
            std::string::size_type location;
            int count = 0;
            do {
                SmartString arg;
                arg << "{" << count << "}";
                location = findSubstring(arg);
                if(location != std::string::npos)
                {
                    count++;
                }
            } while(location != std::string::npos);
            return count;
        }

        std::vector<std::vector<int>> createBoyerMooreBadCharacterTable(const SmartString& targetWord)
        {
            static const int alphabetSize = 256;
            std::vector<std::vector<int>> badCharacterTable(alphabetSize);

            for(int i = 0; i < alphabetSize; i++)
            {
                badCharacterTable[i].assign(targetWord.length(), -1);
            }

            for(int i = 0; i < targetWord.length(); i++)
            {
                int previouslyEncounteredCharacterIndex = -1;
                for(int j = i - 1; j >= 0; j--)
                {
                    if(targetWord[i] == targetWord[j])
                    {
                        previouslyEncounteredCharacterIndex = j;
                        break;
                    }
                }
                badCharacterTable[static_cast<int>(targetWord[i])][i] = previouslyEncounteredCharacterIndex;
            }

            return badCharacterTable;
        }

        /*!
         * @brief Creates the pre-processing table used in the Knuth-Morris-Pratt algorithm for \p targetWord.
         * @param targetWord - The word to be searched
         * @return The pre-processing table for \p targetWord
         */
        static std::vector<int> createKMPTable(const SmartString& targetWord)
        {
            std::vector<int> table(targetWord.length(), 0);
            table[0]             = -1;
            int currentPosition  = 1;
            int currentCandidate = 0;

            while(currentPosition < targetWord.length())
            {
                if(targetWord[currentPosition] == targetWord[currentCandidate])
                {
                    table[currentPosition] = table[currentCandidate];
                }
                else
                {
                    table[currentPosition] = currentCandidate;
                    currentCandidate       = table[currentCandidate];

                    while(currentCandidate >= 0 && targetWord[currentCandidate] != targetWord[currentPosition])
                    {
                        currentCandidate = table[currentCandidate];
                    }
                }
                currentCandidate++;
                currentPosition++;
            }

            return table;
        }

        /*!
         * @brief Terminal version of SmartString::formatHelper(const int, const T&, Args...).
         * @tparam T - Any type that is convertible to a SmartString
         * @param depth - The format number to be replaced.
         * @param t - The value to replace {depth} with
         */
        template <unsigned int depth, typename T>
        void formatHelper(const T& t)
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            SmartString target;
            target << "{" << depth << "}";
            replaceAll(target, t);
        }

        /*!
         * @brief Helper function to be used by SmartString::format() and its derivatives.
         * @tparam T - Any type that is convertible to a SmartString
         * @tparam Args - Any additional arguments
         * @param depth - The format number to be replaced.
         * @param t - the value to replace {depth} with
         * @param args - additional arguments
         */
        template <unsigned int depth, typename T, typename... Args>
        void formatHelper(const T& t, const Args& ...args)
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            SmartString target;
            target << "{" << depth << "}";
            replaceAll(target, t);
            formatHelper<depth + 1>(args...);
        }

        inline void checkBounds(const int lowerBound, const int upperBound) const
        {
            if(lowerBound < 0) throw std::out_of_range("Given lower bound is less than 0");
            if(upperBound > backingString.length()) throw std::out_of_range("Given upper bound is greater than the string size");
        }

    public:
        SmartString() : backingString{}, precision{DEFAULT_PRECISION} { }
        SmartString(const std::string& init) : backingString{init}, precision{DEFAULT_PRECISION} { }
        SmartString(std::string&& init) : backingString{init}, precision{DEFAULT_PRECISION} { }
        SmartString(const char init) : backingString{init}, precision{DEFAULT_PRECISION} { }
        SmartString(const char* init) : backingString{init}, precision{DEFAULT_PRECISION} { }
        SmartString(const std::stringstream& init) : backingString{init.str()}, precision{static_cast<int>(init.precision())} { }
        SmartString(std::stringstream&& init) : backingString{init.str()}, precision{static_cast<int>(init.precision())} { }
        explicit inline SmartString(const unsigned int init) : SmartString()
        {
            append(init);
        }
        explicit inline SmartString(const int init) : SmartString()
        {
            append(init);
        }
        explicit inline SmartString(const double init) : SmartString()
        {
            append(init);
        }
        explicit inline SmartString(const float init) : SmartString()
        {
            append(init);
        }
        SmartString(const int numChars, const char fill) : backingString(numChars, fill), precision{DEFAULT_PRECISION} { }

        SmartString(const SmartString& other) = default;
        SmartString(SmartString&& other) noexcept = default;
        ~SmartString() = default;
        SmartString& operator=(const SmartString& rhs) = default;
        SmartString& operator=(SmartString&& rhs) noexcept = default;

        explicit inline operator std::string() const { return backingString; }
        explicit inline operator char*() const { return c_str(); }
        explicit operator std::stringstream() const { return std::stringstream{backingString}; }

        // iterator exposure
        auto begin()   { return std::begin(backingString);   }
        auto end()     { return std::end(backingString);     }
        auto rbegin()  { return std::rbegin(backingString);  }
        auto rend()    { return std::rend(backingString);    }
        [[nodiscard]] auto cbegin()  const { return std::cbegin(backingString);  }
        [[nodiscard]] auto cend()    const { return std::cend(backingString);    }
        [[nodiscard]] auto crbegin() const { return std::crbegin(backingString); }
        [[nodiscard]] auto crend()   const { return std::crend(backingString);   }

        SmartString& operator=(const std::string& rhs)
        {
            backingString = rhs;
            precision = DEFAULT_PRECISION;
            return *this;
        }

        SmartString& operator=(std::string&& rhs)
        {
            backingString = rhs;
            precision = DEFAULT_PRECISION;
            return *this;
        }

        SmartString& operator=(const char* rhs)
        {
            backingString = rhs;
            precision = DEFAULT_PRECISION;
            return *this;
        }

        inline SmartString& operator=(const std::stringstream& rhs)
        {
            backingString = rhs.str();
            precision = static_cast<int>(rhs.precision());
            return *this;
        }

        inline SmartString& operator=(std::stringstream&& rhs)
        {
            backingString = rhs.str();
            precision = static_cast<int>(rhs.precision());
            return *this;
        }

        inline SmartString& append(const std::string& str)
        {
            backingString.append(str);
            return *this;
        }

        inline SmartString& prepend(const std::string& str)
        {
            backingString.insert(begin(), str.begin(), str.end());
            return *this;
        }

        inline SmartString& append(const char* str)
        {
            backingString.append(str);
            return *this;
        }

        inline SmartString& prepend(const char* str)
        {
            return prepend(std::string(str));
        }

        inline SmartString& append(const std::stringstream& strm)
        {
            backingString.append(strm.str());
            return *this;
        }

        inline SmartString& prepend(const std::stringstream& strm)
        {
            return prepend(strm.str());
        }

        inline SmartString& append(const SmartString& str)
        {
            backingString.append(str.backingString);
            return *this;
        }

        inline SmartString& append(SmartString&& str)
        {
            backingString.append(str.backingString);
            return *this;
        }

        inline SmartString& prepend(const SmartString& str)
        {
            return prepend(str.backingString);
        }

        inline SmartString& prepend(SmartString&& str)
        {
            return prepend(str.backingString);
        }

        SmartString& append(const char c)
        {
            backingString += c;
            return *this;
        }

        SmartString& prepend(const char c)
        {
            backingString.insert(begin(), c);
            return *this;
        }

        SmartString& append(const unsigned int val)
        {
            SmartString temp;
            unsigned int num = val;
            do {
                unsigned int digit = num % 10;
                temp.prepend(digitToChar(digit));
                num /= 10;
            } while(num > 0);
            return append(temp);
        }

        inline SmartString& prepend(const unsigned int val)
        {
            return prepend(SmartString(val));
        }

        SmartString& append(const int val)
        {
            SmartString temp;
            bool isNegative  = val < 0;
            unsigned int num = abs(val);

            do {
                unsigned int digit = num % 10;
                temp.prepend(digitToChar(digit));
                num /= 10;
            } while(num > 0);

            if(isNegative)
            {
                temp.prepend('-');
            }
            return append(temp);
        }

        inline SmartString& prepend(const int val)
        {
            return prepend(SmartString(val));
        }

        SmartString& append(const double val, const unsigned int valPrecision)
        {
            SmartString temp;
            int leftOfDecimal = static_cast<int>(val);

            temp.append(leftOfDecimal);
            temp.append('.');

            double rightOfDecimal = std::abs(val) - abs(leftOfDecimal);
            for(int i = 0; i < valPrecision; i++)
            {
                auto digit = static_cast<unsigned int>(rightOfDecimal * 10);
                temp.append(digitToChar(digit));
                rightOfDecimal = (rightOfDecimal * 10) - digit;
            }
            return append(temp);
        }

        SmartString& append(const double val)
        {
            /*
             * Any trailing zeroes will be removed when this method is used,
             * unless there are only trailing zeroes, in which case one zero
             * will be left. If you would like to include trailing zeroes,
             * use the append method which specifies the precision.
             */
            append(val, precision);
            if(getLast() == '0')
            {
                rstrip('0');
            }
            if(getLast() == '.')
            {
                append('0');
            }
            return *this;
        }

        SmartString& prepend(const double val, const unsigned int valPrecision)
        {
            SmartString temp;
            temp.append(val, valPrecision);
            return prepend(temp);
        }

        SmartString& prepend(const double val)
        {
            SmartString temp;
            temp.append(val);
            return prepend(temp);
        }

        SmartString& append(const float val, const unsigned int valPrecision)
        {
            SmartString temp;
            auto leftOfDecimal = static_cast<unsigned int>(std::abs(val));

            if(val < 0)
            {
                temp.append('-');
            }
            temp.append(leftOfDecimal);
            temp.append('.');

            float rightOfDecimal = val - static_cast<float>(leftOfDecimal);
            for(int i = 0; i < valPrecision; i++)
            {
                auto digit = static_cast<unsigned int>(rightOfDecimal * 10);
                temp.append(digitToChar(digit));
                rightOfDecimal = (rightOfDecimal * 10) - static_cast<float>(digit);
            }
            return append(temp);
        }

        SmartString& append(const float val)
        {
            append(val, precision);
            if(getLast() == '0')
            {
                rstrip('0');
            }
            if(getLast() == '.')
            {
                append('0');
            }
            return *this;
        }

        SmartString& prepend(const float val, const unsigned int valPrecision)
        {
            SmartString temp;
            temp.append(val, valPrecision);
            return prepend(temp);
        }

        SmartString& prepend(const float val)
        {
            return prepend(SmartString(val));
        }

        template <typename T>
        SmartString& operator<<(const T& t)
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            return append(t);
        }

        template <typename T>
        inline SmartString& operator+=(const T& t)
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            return append(t);
        }

        template <typename T>
        SmartString operator+(const T& t) const
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            SmartString result(*this);
            result.append(t);
            return result;
        }

        inline bool operator!=(const SmartString& str) const
        {
            return backingString != str.backingString;
        }

        inline bool operator!=(const std::stringstream& strm) const
        {
            return backingString != strm.str();
        }

        inline bool operator!=(const std::string& str) const
        {
            return backingString != str;
        }

        inline bool operator!=(const char* str) const
        {
            return backingString != std::string(str);
        }

        bool operator==(const SmartString& str) const
        {
            return backingString == str.backingString;
        }

        inline bool operator==(const std::stringstream& strm) const
        {
            return backingString == strm.str();
        }

        bool operator==(const std::string& str) const
        {
            return backingString == str;
        }

        bool operator==(const char* str) const
        {
            return backingString == std::string(str);
        }

        bool operator<(const SmartString& str) const
        {
            return backingString < str.backingString;
        }

        template <typename T>
        inline bool operator<(const T& str) const
        {
            return (*this < SmartString(str));
        }

        inline bool operator<=(const SmartString& str) const
        {
            return (backingString <= str.backingString);
        }

        template <typename T>
        inline bool operator<=(const T& str) const
        {
            return (*this <= SmartString(str));
        }

        inline bool operator>(const SmartString& str) const
        {
            return backingString > str.backingString;
        }

        template <typename T>
        inline bool operator>(const T& str) const
        {
            return (*this > SmartString(str));
        }

        inline bool operator>=(const SmartString& str) const
        {
            return backingString >= str.backingString;
        }

        template <typename T>
        inline bool operator>=(const T& str) const
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            return (*this >= SmartString(str));
        }

        inline char& operator[](const size_t index)
        {
            return backingString[index];
        }

        inline char operator[](const size_t index) const
        {
            return backingString[index];
        }

        template <typename T>
        friend T operator+(const T left, const SmartString& right)
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            SmartString result(left);
            result.append(right);
            return static_cast<T>(result);
        }

        template <typename T>
        inline friend bool operator==(const T& left, const SmartString& right)
        {
            return (right == left);
        }

        template <typename T>
        inline friend bool operator!=(const T& left, const SmartString& right)
        {
            return (right != left);
        }

        template <typename T>
        inline friend bool operator<(const T& left, const SmartString& right)
        {
            return (right > left);
        }

        template <typename T>
        inline friend bool operator>(const T& left, const SmartString& right)
        {
            return (right < left);
        }

        template <typename T>
        inline friend bool operator<=(const T& left, const SmartString& right)
        {
            return (right >= left);
        }

        template <typename T>
        inline friend bool operator>=(const T& left, const SmartString& right)
        {
            return (right <= left);
        }

        friend std::ostream& operator<<(std::ostream& out, const SmartString& smartString)
        {
            out << smartString.backingString;
            return out;
        }

        friend std::istream& operator>>(std::istream& in, SmartString& smartString)
        {
            std::string temp;
            in >> temp;
            smartString.backingString = temp;
            return in;
        }

        friend std::ofstream& operator<<(std::ofstream& out, const SmartString& smartString)
        {
            out << smartString.backingString;
            return out;
        }

        friend std::ifstream& operator>>(std::ifstream& in, SmartString& smartString)
        {
            std::string temp;
            in >> temp;
            smartString.backingString = temp;
            return in;
        }

        friend std::fstream& operator<<(std::fstream& out, const SmartString& smartString)
        {
            out << smartString.backingString;
            return out;
        }

        friend std::fstream& operator>>(std::fstream& in, SmartString& smartString)
        {
            std::string temp;
            in >> temp;
            smartString.backingString = temp;
            return in;
        }

        friend std::stringstream& operator<<(std::stringstream& out, const SmartString& smartString)
        {
            out << smartString.backingString;
            return out;
        }

        friend std::stringstream& operator>>(std::stringstream& in, SmartString& smartString)
        {
            std::string temp;
            in >> temp;
            smartString.backingString = temp;
            return in;
        }

        inline void setPrecision(const int newPrecision)
        {
            precision = newPrecision > 0 ? newPrecision : 1;
        }

        [[nodiscard]] inline int getPrecision() const
        {
            return precision;
        }

        [[nodiscard]] char getLast() const
        {
            return backingString.back();
        }

        [[nodiscard]] char getFirst() const
        {
            return backingString.front();
        }

        [[nodiscard]] SmartString getSubstring(const std::string::size_type startLocation, const std::string::size_type endLocation) const
        {
            return backingString.substr(startLocation, endLocation - startLocation + 1);
        }

        template <typename T>
        inline T getSubstring(const std::string::size_type startLocation, const std::string::size_type endLocation) const
        {
            static_assert(is_castable<SmartString, T>::value, "SmartString must be convertible to an object of type T");
            return static_cast<T>(getSubstring(startLocation, endLocation));
        }

        [[nodiscard]] std::string::size_type findSubstring(const std::string::size_type startingLocation, const SmartString& target) const
        {
            return backingString.find(target.backingString, startingLocation);
        }

        [[nodiscard]] inline std::string::size_type findSubstring(const SmartString& target) const
        {
            return findSubstring(0, target);
        }

        template <typename T>
        [[nodiscard]] inline std::string::size_type findSubstring(const T& target) const
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            return findSubstring(0, SmartString(target));
        }

        template <typename T>
        [[nodiscard]] inline bool contains(const T& target) const
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            return findSubstring(target) != std::string::npos;
        }

        template <typename T>
        unsigned int count(const T& target) const
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            unsigned int numInstances = 0;
            SmartString targ(target);
            std::string::size_type location = findSubstring(targ);
            while(location != std::string::npos && location < length())
            {
                numInstances++;
                location = findSubstring(location + 1, targ);
            }
            return numInstances;
        }

        template<typename T>
        std::vector<T> split() const
        {
            return split<T>(" ");
        }

        template <typename T, typename U>
        [[nodiscard]] std::vector<T> split(const U& target) const
        {
            static_assert(is_castable<SmartString, T>::value, "SmartString must be convertible to an object of type T");
            static_assert(std::is_convertible<U, SmartString>::value, "U must be convertible to a SmartString");

            std::vector<T> result;
            SmartString temp(*this);
            SmartString targ(target);
            std::string::size_type location = temp.findSubstring(targ);
            while(location != std::string::npos)
            {
                if(location == 0)
                {
                    result.push_back("");
                    temp.remove(0, 0);
                }
                else
                {
                    result.push_back(temp.getSubstring<T>(0, location - 1));
                    temp.remove(0, location + targ.length() - 1);
                }
                location = temp.findSubstring(targ);
            }
            if(temp.length() > 0)
            {
                result.push_back(static_cast<T>(temp));
            }
            return result;
        }

        template <typename T, typename U, typename V>
        static T join(const std::vector<U>& list, const V& separator)
        {
            static_assert(is_castable<SmartString, T>::value, "SmartString must be castable to an object of type T");
            static_assert(std::is_convertible<U, SmartString>::value, "U must be convertible to a SmartString");
            static_assert(std::is_convertible<V, SmartString>::value, "V must be convertible to a SmartString");

            SmartString temp;
            for(int i = 0; i < list.size() - 1; i++)
            {
                temp.append(list[i]);
                temp.append(separator);
            }
            temp.append(list[list.size() - 1]);
            return static_cast<T>(temp);
        }

        template <typename T, typename U, typename V>
        static T join(const U* list, int listSize, const V& separator)
        {
            static_assert(is_castable<SmartString, T>::value, "SmartString must be castable to an object of type T");
            static_assert(std::is_convertible<U, SmartString>::value, "U must be convertible to a SmartString");
            static_assert(std::is_convertible<V, SmartString>::value, "V must be convertible to a SmartString");

            SmartString temp;
            for(int i = 0; i < listSize - 1; i++)
            {
                temp.append(list[i]);
                temp.append(separator);
            }
            temp.append(list[listSize - 1]);
            return static_cast<T>(temp);
        }

        inline SmartString& lstrip()
        {
            lstrip(whitespace());
            return *this;
        }

        inline SmartString& rstrip()
        {
            rstrip(whitespace());
            return *this;
        }

        inline SmartString& strip()
        {
            lstrip(whitespace());
            rstrip(whitespace());
            return *this;
        }

        template <typename T>
        SmartString& lstrip(const T& chars)
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            const SmartString toStrip(chars);
            int index           = 0;
            while(index < length() && toStrip.contains(backingString[index]))
            {
                index++;
            }
            remove(0, index - 1);
            return *this;
        }

        template <typename T>
        SmartString& rstrip(const T& chars)
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            const SmartString toStrip(chars);
            int index           = static_cast<int>(length()) - 1;
            while(index >= 0 && toStrip.contains(backingString[index]))
            {
                index--;
            }
            remove(index + 1, static_cast<int>(length()) - 1);
            return *this;
        }

        template <typename T>
        inline SmartString& strip(const T& chars)
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            lstrip(chars);
            rstrip(chars);
            return *this;
        }

        // start and end are both inclusive.
        SmartString& remove(const int startLocation, const int endLocation)
        {
            if(endLocation < startLocation) return *this;
            if(endLocation == startLocation) {
                backingString.erase(startLocation, 1);
                return *this;
            }
            backingString.erase(startLocation, endLocation - startLocation + 1);
            return *this;
        }

        inline SmartString& remove(const SmartString& target)
        {
            return replace(target, "");
        }

        template <typename T>
        inline SmartString& remove(const T& target)
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            return remove(SmartString(target));
        }

        SmartString& removeAll(const SmartString& target)
        {
            bool changed;
            do {
                SmartString old(*this);
                remove(target);
                changed = (old != *this);
            } while(changed);
            return *this;
        }

        template <typename T>
        inline SmartString& removeAll(const T& target)
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            return removeAll(SmartString(target));
        }

        SmartString& replace(const SmartString& target, const SmartString& newSubstring)
        {
            if(isEmpty() || target.isEmpty()) return *this;

            std::string::size_type location = findSubstring(target);
            if(location == std::string::npos)
            {
                return *this;
            }

            backingString.replace(location, target.length(), newSubstring.backingString);
            return *this;
        }

        template <typename T, typename U>
        inline SmartString& replace(const T& target, const U& newSubstring)
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            static_assert(std::is_convertible<U, SmartString>::value, "U must be convertible to a SmartString");
            return replace(SmartString(target), SmartString(newSubstring));
        }

        SmartString& replaceAll(const SmartString& target, const SmartString& newSubstring)
        {
            bool changed;
            do {
                SmartString old = *this;
                replace(target, newSubstring);
                changed = (old != *this);
            } while(changed);

            return *this;
        }

        template <typename T, typename U>
        inline SmartString& replaceAll(const T& target, const U& newSubstring)
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            static_assert(std::is_convertible<U, SmartString>::value, "U must be convertible to a SmartString");
            return replaceAll(SmartString(target), SmartString(newSubstring));
        }

        template <typename... Args>
        inline SmartString& format(const Args& ... args)
        {
            formatHelper<0>(args...);
            return *this;
        }

        template <typename T, typename... Args>
        inline T getFormatted(const Args& ... args) const
        {
            static_assert(is_castable<SmartString, T>::value, "SmartString must be convertible to an object of type T");
            SmartString result(*this);
            result.formatHelper<0>(args...);
            return static_cast<T>(result);
        }

        template <typename T, typename U, typename... Args>
        static T format(const U& source, const Args& ... args)
        {
            static_assert(is_castable<SmartString, T>::value, "SmartString must be convertible to an object of type T");
            static_assert(std::is_convertible<U, SmartString>::value, "U must be convertible to a SmartString");
            SmartString result(source);
            result.formatHelper<0>(args...);
            return static_cast<T>(result);
        }

        // Assumes a-z and A-Z are contiguous. will break if they aren't.
        SmartString& toUpper()
        {
            constexpr auto a = static_cast<int>('a');
            constexpr auto A = static_cast<int>('A');
            constexpr auto z = static_cast<int>('z');
            for(unsigned int i = 0; i < length(); i++)
            {
                const auto letterVal = static_cast<int>(static_cast<unsigned char>(backingString[i]));
                if(letterVal >= a && letterVal <= z)
                {
                    backingString[i] = static_cast<char>(letterVal - a + A);
                }
            }
            return *this;
        }

        // Assumes a-z and A-Z are contiguous. will break if they aren't.
        SmartString& toLower()
        {
            constexpr auto a = static_cast<int>('a');
            constexpr auto A = static_cast<int>('A');
            constexpr auto Z = static_cast<int>('Z');
            for(int i = 0; i < length(); i++)
            {
                const auto letterVal = static_cast<int>(static_cast<unsigned char>(backingString[i]));
                if(letterVal >= A && letterVal <= Z)
                {
                    backingString[i] = static_cast<char>(letterVal - A + a);
                }
            }
            return *this;
        }

        template <typename T, typename U>
        static bool tryConvert(const T& source, U& out)
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            static_assert(std::is_arithmetic<U>::value, "U must be an arithmetic type");

            SmartString src(source);
            if(src.isEmpty())
            {
                return false;
            }
            int sign = 1;
            if(src[0] == '-')
            {
                sign = -1;
                src.lstrip("-");
            }

            SmartString leftOfDecimal;
            SmartString rightOfDecimal;
            std::vector<SmartString> results = src.split<SmartString>(".");
            double answer = 0;

            if(results.size() > 2 || results.empty())
            {
                return false;
            }
            else if(results.size() == 2)
            {
                rightOfDecimal = results[1];
            }
            leftOfDecimal = results[0];

            int multiplier = 1;
            for(int i = static_cast<int>(leftOfDecimal.length()) - 1; i >= 0; i--)
            {
                unsigned int num = charToDigit(leftOfDecimal[i]);
                if(num > 9)
                {
                    return false;
                }
                answer += (num * multiplier);
                multiplier *= 10;
            }

            double divisor = 10;
            for(unsigned int i = 0; i < rightOfDecimal.length(); i++)
            {
                unsigned int num = charToDigit(rightOfDecimal[i]);
                if(num > 9)
                {
                    return false;
                }
                answer += (num / divisor);
                divisor *= 10;
            }

            out = static_cast<U>(answer * sign);
            return true;
        }

        template <typename T, typename U>
        static T convert(const U& source)
        {
            static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
            static_assert(std::is_convertible<U, SmartString>::value, "U must be convertible to a SmartString");

            T temp;
            bool result = tryConvert(source, temp);
            if(result)
            {
                return temp;
            }

            SmartString errorStream;
            errorStream << "The given string could not be parsed into a valid number: " << source;
            throw std::invalid_argument(errorStream.str());
        };

        template <typename T>
        inline bool tryConvert(T& out) const
        {
            static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
            return tryConvert(*this, out);
        }

        template <typename T>
        T convert() const
        {
            static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
            T temp;
            bool result = tryConvert(temp);
            if(result)
            {
                return temp;
            }
            SmartString errorStream;
            errorStream << "This string could not be parsed into a valid number: " << backingString;
            throw std::invalid_argument(errorStream.str());
        };

        inline static SmartString whitespace()
        {
            return {" \t\n\r\x0b\x0c"};
        }

        template <typename T>
        inline static T whitespace()
        {
            static_assert(is_castable<SmartString, T>::value, "SmartString must be convertible to an object of type T");
            return static_cast<T>(whitespace());
        }

        [[nodiscard]] inline std::string str() const
        {
            return std::string{backingString};
        }

        [[nodiscard]] inline char* c_str() const
        {
            // need the extra character for the null terminator
            char* result = new char[backingString.length() + 1];
            std::memcpy(result, &backingString[0], backingString.length()+1);
            return result;
        }

        [[nodiscard]] inline std::stringstream sstream() const
        {
            std::stringstream result;
            result << backingString;
            return result;
        }

        [[nodiscard]] inline size_t length() const
        {
            return backingString.length();
        }

        [[nodiscard]] inline bool isEmpty() const
        {
            return backingString.empty();
        }

        SmartString& reverse()
        {
            std::reverse(begin(), end());
            return *this;
        }
    };
}// namespace Utilities

#endif//UTILITYCODE_SMARTSTRING_H
