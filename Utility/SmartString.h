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
#include <map>
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

        char* backingString;
        int stringSize;
        int precision;
        int memorySize;

        /*!
         * @brief destroys the underlying string and resets sizes. Precision is maintained.
         */
        void destroy()
        {
            if(backingString != nullptr)
            {
                delete[] backingString;
                stringSize    = 0;
                backingString = nullptr;
                memorySize    = 0;
            }
        }

        /*!
         * @brief Destroys the existing string and creates a new one of size \p size.
         * @param size - size of string to create
         */
        void initialize(const int size)
        {
            destroy();
            stringSize                = size;
            backingString             = new char[stringSize + 1];
            backingString[stringSize] = '\0';
            memorySize                = stringSize + 1;
        }

        /*!
         * @brief extend the size of the string by the number of \p charsToAdd. The null character is added to the end of the string,
         *        but any other added characters will not be initialized. If \p addToFront is true, the additional characters will be
         *        added to the front of the string instead of the back.
         * @param charsToAdd - number of characters to increase the string size by
         * @param addToFront - If true, characters will be added to the front of the string
         */
        void extend(const int charsToAdd, const bool addToFront)
        {
            if(charsToAdd < 1)
            {
                return;
            }

            int newSize   = stringSize + charsToAdd + 1;
            char* newBackingString = nullptr;

            if(memorySize <= newSize)
            {
                if(memorySize <= 0)
                {
                    memorySize = 1;
                }
                while(memorySize <= newSize)
                {
                    memorySize *= 2;
                }
                newBackingString = new char[memorySize];
            }

            if(newBackingString != nullptr)
            {
                int offset = addToFront ? charsToAdd : 0;
                for(int i = offset; i < stringSize + offset; i++)
                {
                    newBackingString[i] = backingString[i - offset];
                }
                newBackingString[stringSize + offset] = '\0';
                delete[] backingString;
                backingString = newBackingString;
            }
            else if(addToFront)
            {
                for(int i = stringSize; i >= 0; i--)
                {
                    backingString[i + charsToAdd] = backingString[i];
                }
            }

            stringSize += charsToAdd;
        }

        /*!
         * @brief default version of SmartString::extend(const int, const bool). Additional characters
         *        will be added to the back of the string.
         * @param charsToAdd - number of characters to increase the string size by
         */
        inline void extend(const int charsToAdd)
        {
            extend(charsToAdd, false);
        }

        /*!
         * @brief Mostly just here so that templated functions can call SmartString::calculateSize(const char*) const
         *        without worrying about type mismatches.
         * @param str - String to calculate the size of
         * @return The length of the string
         */
        static inline int calculateSize(const std::string& str)
        {
            return str.length();
        }

        /*!
         * @brief Mostly just here so that templated functions can call SmartString::calculateSize(const char*) const
         *        without worrying about type mismatches.
         * @param str - String to calculate the size of
         * @return The length of the string
         */
        static inline int calculateSize(const SmartString& str)
        {
            return str.length();
        }

        /*!
         * @brief Calculates the size of the given char*. Simply searches for the first instance of '\0'.
         *        Will likely seg fault if there is no null character in the string.
         * @param str - String to calculate the size of
         * @return The length of the string
         */
        static int calculateSize(const char* str)
        {
            int size = 0;
            while(str[size] != '\0')
            {
                size++;
            }
            return size;
        }

        static const char* getDataPointer(const char* str)
        {
            return str;
        }

        static const char* getDataPointer(const std::string& str)
        {
            return str.data();
        }

        static const char* getDataPointer(const SmartString& str)
        {
            return str.backingString;
        }

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
         * @brief Finds the absolute value of a number. Didn't want to include a math
         *        library just for one absolute value function.
         * @tparam T - Any arithmetic type.
         * @param val - Value to calculate the absolute value of.
         * @return The absolute value of \p val
         */
        template <typename T>
        static inline T abs(const T val)
        {
            static_assert(std::is_arithmetic<T>::value, "arithmetic type required.");
            return val < 0 ? val * -1 : val;
        }

        /*!
         * @brief Finds the number of format arguments present in the current string.
         *        Format arguments are of the form {0}, {1}, etc.
         * @return The number of format arguments
         */
        int getNumArguments() const
        {
            int location;
            int count = 0;
            do {
                SmartString arg;
                arg << "{" << count << "}";
                location = findSubstring(arg);
                if(location >= 0)
                {
                    count++;
                }
            } while(location >= 0);
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

        template<typename T>
        SmartString& appendHelper(const T& t)
        {
            int initialSize = stringSize;
            int size = calculateSize(t);
            extend(size);
            std::memcpy(backingString + initialSize, getDataPointer(t), size);
            backingString[stringSize] = '\0';
            return *this;
        }

        template<typename T>
        SmartString& prependHelper(const T& t)
        {
            int size = calculateSize(t);
            extend(size, true);
            std::memcpy(backingString, getDataPointer(t), size);
            return *this;
        }

        inline void checkBounds(const int lowerBound, const int upperBound) const
        {
            if(lowerBound < 0) throw std::out_of_range("Given lower bound is less than 0");
            if(upperBound > stringSize) throw std::out_of_range("Given upper bound is greater than the string size");
        }

    public:
        SmartString() : backingString(nullptr), stringSize(0), precision(5), memorySize(0) {}
        SmartString(const std::string& init) : SmartString()
        {
            if(init.length() != 0)
            {
                initialize(static_cast<int>(init.length()));
                std::memcpy(backingString, init.data(), stringSize);
            }
        }
        SmartString(std::string&& init) : SmartString()
        {
            if(init.length() != 0)
            {
                initialize(static_cast<int>(init.length()));
                std::memcpy(backingString, init.data(), stringSize);
            }
        }
        SmartString(const char init) : SmartString()
        {
            initialize(1);
            stringSize       = 1;
            backingString[0] = init;
        }
        SmartString(const char* init) : SmartString()
        {
            if(init != nullptr)
            {
                initialize(calculateSize(init));
                std::memcpy(backingString, init, stringSize);
            }
        }
        SmartString(const std::stringstream& init) : SmartString()
        {
            const std::string str(init.str());
            if(str.length() != 0)
            {
                initialize(static_cast<int>(str.length()));
                std::memcpy(backingString, str.data(), stringSize);
            }
            precision = init.precision();
        }
        SmartString(std::stringstream&& init) : SmartString()
        {
            const std::string str(init.str());
            if(str.length() != 0)
            {
                initialize(static_cast<int>(str.length()));
                std::memcpy(backingString, str.data(), stringSize);
            }
            precision = init.precision();
        }
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
        SmartString(const int numChars, const char fill) : SmartString()
        {
            initialize(numChars);
            for(int i = 0; i < numChars; i++)
            {
                backingString[i] = fill;
            }
        }

        SmartString(const SmartString& other) : SmartString()
        {
            initialize(other.stringSize);
            std::memcpy(backingString, other.backingString, stringSize);
            precision = other.precision;
        }
        SmartString(SmartString&& other) noexcept
        {
            backingString       = other.backingString;
            stringSize          = other.stringSize;
            precision           = other.precision;
            memorySize          = other.memorySize;
            other.backingString = nullptr;
        }

        ~SmartString()
        {
            destroy();
        }

        explicit inline operator std::string() const { return str(); }
        explicit inline operator char*() const { return c_str(); }
        explicit operator std::stringstream() const
        {
            std::stringstream result;
            result << backingString;
            return result;
        }

        SmartString& operator=(const SmartString& rhs)
        {
            if(&rhs != this)
            {
                initialize(rhs.stringSize);
                std::memcpy(backingString, rhs.backingString, stringSize);
                precision = rhs.precision;
            }
            return *this;
        }

        SmartString& operator=(SmartString&& rhs) noexcept
        {
            if(&rhs != this)
            {
                destroy();
                precision         = rhs.precision;
                stringSize        = rhs.stringSize;
                backingString     = rhs.backingString;
                rhs.precision     = 5;
                rhs.stringSize    = 0;
                rhs.backingString = nullptr;
            }
            return *this;
        }

        SmartString& operator=(const std::string& rhs)
        {
            initialize(static_cast<int>(rhs.length()));
            std::memcpy(backingString, rhs.data(), stringSize);
            return *this;
        }

        SmartString& operator=(std::string&& rhs)
        {
            initialize(static_cast<int>(rhs.length()));
            std::memcpy(backingString, rhs.data(), stringSize);
            return *this;
        }

        SmartString& operator=(const char* rhs)
        {
            initialize(calculateSize(rhs));
            std::memcpy(backingString, rhs, stringSize);
            return *this;
        }

        inline SmartString& operator=(const std::stringstream& rhs)
        {
            return (*this = rhs.str());
        }

        inline SmartString& operator=(std::stringstream&& rhs)
        {
            return (*this = rhs.str());
        }

        inline SmartString& append(const char* str)
        {
            return appendHelper<const char*>(str);
        }

        inline SmartString& prepend(const char* str)
        {
            return prependHelper<const char*>(str);
        }

        inline SmartString& append(const std::string& str)
        {
            return appendHelper<const std::string&>(str);
        }

        inline SmartString& prepend(const std::string& str)
        {
            return prependHelper<const std::string&>(str);
        }

        inline SmartString& append(const std::stringstream& strm)
        {
            return append(strm.str());
        }

        inline SmartString& prepend(const std::stringstream& strm)
        {
            return prepend(strm.str());
        }

        inline SmartString& append(const SmartString& str)
        {
            return appendHelper<const SmartString&>(str);
        }

        inline SmartString& append(SmartString&& str)
        {
            return appendHelper<SmartString&&>(str);
        }

        inline SmartString& prepend(const SmartString& str)
        {
            return prependHelper<const SmartString&>(str);
        }

        inline SmartString& prepend(SmartString&& str)
        {
            return prependHelper<SmartString&&>(str);
        }

        SmartString& append(const char c)
        {
            extend(1);
            backingString[stringSize - 1] = c;
            backingString[stringSize]     = '\0';

            return *this;
        }

        SmartString& prepend(const char c)
        {
            extend(1, true);
            backingString[0] = c;

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

            double rightOfDecimal = abs(val) - abs(leftOfDecimal);
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
            auto leftOfDecimal = static_cast<unsigned int>(abs(val));

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
            return !(*this == str);
        }

        inline bool operator!=(const std::stringstream& str) const
        {
            return !(*this == str);
        }

        inline bool operator!=(const std::string& str) const
        {
            return !(*this == str);
        }

        inline bool operator!=(const char* str) const
        {
            return !(*this == str);
        }

        bool operator==(const SmartString& str) const
        {
            if(stringSize != str.stringSize)
            {
                return false;
            }
            for(int i = 0; i < stringSize; i++)
            {
                if(str.backingString[i] != backingString[i])
                {
                    return false;
                }
            }
            return true;
        }

        inline bool operator==(const std::stringstream& str) const
        {
            return (*this == str.str());
        }

        bool operator==(const std::string& str) const
        {
            if(stringSize != str.size())
            {
                return false;
            }
            for(int i = 0; i < stringSize; i++)
            {
                if(str[i] != backingString[i])
                {
                    return false;
                }
            }
            return true;
        }

        bool operator==(const char* str) const
        {
            if(stringSize != calculateSize(str))
            {
                return false;
            }
            for(unsigned int i = 0; i < stringSize; i++)
            {
                if(str[i] != backingString[i])
                {
                    return false;
                }
            }
            return true;
        }

        bool operator<(const SmartString& str) const
        {
            if(str.isEmpty()) return false;
            if(isEmpty()) return true;

            unsigned int min = stringSize < str.stringSize ? stringSize : str.stringSize;
            unsigned int i   = 0;
            while(i < min && backingString[i] == str.backingString[i]) { i++; }

            return backingString[i] < str.backingString[i];
        }

        template <typename T>
        inline bool operator<(const T& str) const
        {
            return (*this < SmartString(str));
        }

        inline bool operator<=(const SmartString& str) const
        {
            return (this->str() <= str.str());
        }

        template <typename T>
        inline bool operator<=(const T& str) const
        {
            return (*this <= SmartString(str));
        }

        inline bool operator>(const SmartString& str) const
        {
            return (this->str() > str.str());
        }

        template <typename T>
        inline bool operator>(const T& str) const
        {
            return (*this > SmartString(str));
        }

        inline bool operator>=(const SmartString& str) const
        {
            return (this->str() >= str.str());
        }

        template <typename T>
        inline bool operator>=(const T& str) const
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            return (*this >= SmartString(str));
        }

        inline char& operator[](const unsigned int index) const
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
            smartString = SmartString(temp);
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
            smartString = SmartString(temp);
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
            smartString = SmartString(temp);
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
            smartString = SmartString(temp);
            return in;
        }

        inline void setPrecision(const int newPrecision)
        {
            precision = newPrecision > 0 ? newPrecision : 1;
        }

        inline int getPrecision() const
        {
            return precision;
        }

        char getLast() const
        {
            if(stringSize > 0)
            {
                return backingString[stringSize - 1];
            }
            return '\0';
        }

        char getFirst() const
        {
            if(stringSize > 0)
            {
                return backingString[0];
            }
            return '\0';
        }

        // start and end are both inclusive.
        SmartString getSubstring(const int startLocation, const int endLocation) const
        {
            checkBounds(startLocation, endLocation);

            const int substringSize = endLocation - startLocation + 1;
            SmartString result;
            result.initialize(substringSize);
            std::memcpy(result.backingString, backingString + startLocation, substringSize);
            result[substringSize] = '\0';
            return result;
        }

        template <typename T>
        inline T getSubstring(const int startLocation, const int endLocation) const
        {
            static_assert(is_castable<SmartString, T>::value, "SmartString must be convertible to an object of type T");
            return static_cast<T>(getSubstring(startLocation, endLocation));
        }

        // Uses the Knuth-Morris-Pratt algorithm for finding the substring
        int findSubstring(const int startingLocation, const SmartString& target) const
        {
            if(startingLocation < 0)
            {
                return -1;
            }

            std::vector<int> table = createKMPTable(target);
            int currentLocation    = startingLocation;
            int targetLocation     = 0;

            while(currentLocation < stringSize)
            {
                if(target[targetLocation] == backingString[currentLocation])
                {
                    targetLocation++;
                    currentLocation++;
                    if(targetLocation == target.length())
                    {
                        return currentLocation - targetLocation;
                    }
                }
                else
                {
                    targetLocation = table[targetLocation];
                    if(targetLocation < 0)
                    {
                        currentLocation++;
                        targetLocation++;
                    }
                }
            }
            return -1;
        }

        inline int findSubstring(const SmartString& target) const
        {
            return findSubstring(0, target);
        }

        template <typename T>
        inline int findSubstring(const T& target) const
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            return findSubstring(0, SmartString(target));
        }

        template <typename T>
        inline bool contains(const T& target) const
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            return findSubstring(target) >= 0;
        }

        template <typename T>
        unsigned int count(const T& target) const
        {
            static_assert(std::is_convertible<T, SmartString>::value, "T must be convertible to a SmartString");
            unsigned int numInstances = 0;
            SmartString targ(target);
            int location = findSubstring(targ);
            while(location >= 0 && location < length())
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
        std::vector<T> split(const U& target) const
        {
            static_assert(is_castable<SmartString, T>::value, "SmartString must be convertible to an object of type T");
            static_assert(std::is_convertible<U, SmartString>::value, "U must be convertible to a SmartString");

            std::vector<T> result;
            SmartString temp(*this);
            SmartString targ(target);
            int location = temp.findSubstring(targ);
            while(location >= 0)
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
            while(index < stringSize && toStrip.contains(backingString[index]))
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
            int index           = static_cast<int>(stringSize) - 1;
            while(index >= 0 && toStrip.contains(backingString[index]))
            {
                index--;
            }
            remove(index + 1, static_cast<int>(stringSize) - 1);
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
            checkBounds(startLocation, endLocation);

            int numRemovedCharacters = (endLocation - startLocation + 1);
            for(int j = endLocation + 1; j < stringSize; j++)
            {
                backingString[j - numRemovedCharacters] = backingString[j];
            }
            stringSize = stringSize - numRemovedCharacters;
            backingString[stringSize] = '\0';
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
            int location = findSubstring(target);
            if(location < 0 || isEmpty() || target.isEmpty())
            {
                return *this;
            }

            // since we know target is a substring at this point, we can guarantee newSize will be at least the size of newSubstring.length()
            int newSize = stringSize + newSubstring.length() - target.length();
            while(memorySize <= newSize)
            {
                memorySize *= 2;
            }
            char* tempPointer = new char[memorySize];

            for(int i = 0; i < location; i++)
            {
                tempPointer[i] = backingString[i];
            }
            for(int i = 0; i < newSubstring.length(); i++)
            {
                tempPointer[location + i] = newSubstring[i];
            }
            for(int i = location + target.length(); i < stringSize; i++)
            {
                tempPointer[i + newSubstring.length() - target.length()] = backingString[i];
            }
            tempPointer[newSize] = '\0';

            delete[] backingString;
            backingString = tempPointer;
            stringSize    = newSize;

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
                if(num > 9 || num < 0)
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
                if(num > 9 || num < 0)
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

        inline std::string str() const
        {
            if(stringSize > 0)
                return { backingString };
            return std::string{""};
        }

        inline char* c_str() const
        {
            // need the extra character for the null terminator
            char* result = new char[stringSize + 1];
            std::memcpy(result, backingString, stringSize+1);
            return result;
        }

        inline std::stringstream sstream() const
        {
            std::stringstream result;
            result << *this;
            return result;
        }

        inline int length() const
        {
            return stringSize;
        }

        inline bool isEmpty() const
        {
            return stringSize == 0;
        }

        inline size_t memoryFootPrint() const
        {
            return memorySize * sizeof(char);
        }
    };
}// namespace Utilities

#endif//UTILITYCODE_SMARTSTRING_H
