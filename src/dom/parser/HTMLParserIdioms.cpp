/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "StarFishConfig.h"
#include "HTMLParserIdioms.h"

#include <limits>

namespace StarFish {

template <typename CharType>
static String* stripLeadingAndTrailingHTMLSpaces(String* string, const CharType* characters, unsigned length)
{
    unsigned numLeadingSpaces = 0;
    unsigned numTrailingSpaces = 0;

    for (; numLeadingSpaces < length; ++numLeadingSpaces) {
        if (isNotHTMLSpace<CharType>(characters[numLeadingSpaces]))
            break;
    }

    if (numLeadingSpaces == length)
        return String::emptyString;
        // return string.isNull() ? string : emptyAtom.string();

    for (; numTrailingSpaces < length; ++numTrailingSpaces) {
        if (isNotHTMLSpace<CharType>(characters[length - numTrailingSpaces - 1]))
            break;
    }

    STARFISH_ASSERT(numLeadingSpaces + numTrailingSpaces < length);

    if (!(numLeadingSpaces | numTrailingSpaces))
        return string;

    return string->substring(numLeadingSpaces, length - (numLeadingSpaces + numTrailingSpaces));
}

String* stripLeadingAndTrailingHTMLSpaces(String* string)
{
    unsigned length = string->length();

    if (!length)
        return String::emptyString;

    if (string->isASCIIString())
        return stripLeadingAndTrailingHTMLSpaces<char>(string, string->asASCIIString()->data(), length);

    return stripLeadingAndTrailingHTMLSpaces<char32_t>(string, string->asUTF32String()->data(), length);
}
/*
String serializeForNumberType(const Decimal& number)
{
    if (number.isZero()) {
        // Decimal::toString appends exponent, e.g. "0e-18"
        return number.isNegative() ? "-0" : "0";
    }
    return number.toString();
}

String serializeForNumberType(double number)
{
    // According to HTML5, "the best representation of the number n as a floating
    // point number" is a string produced by applying ToString() to n.
    return String::numberToStringECMAScript(number);
}

Decimal parseToDecimalForNumberType(const String& string, const Decimal& fallbackValue)
{
    // See HTML5 2.5.4.3 `Real numbers.' and parseToDoubleForNumberType

    // String::toDouble() accepts leading + and whitespace characters, which are not valid here.
    const UChar firstCharacter = string[0];
    if (firstCharacter != '-' && firstCharacter != '.' && !isASCIIDigit(firstCharacter))
        return fallbackValue;

    const Decimal value = Decimal::fromString(string);
    if (!value.isFinite())
        return fallbackValue;

    // Numbers are considered finite IEEE 754 single-precision floating point values.
    // See HTML5 2.5.4.3 `Real numbers.'
    // FIXME: We should use numeric_limits<double>::max for number input type.
    const Decimal floatMax = Decimal::fromDouble(std::numeric_limits<float>::max());
    if (value < -floatMax || value > floatMax)
        return fallbackValue;

    // We return +0 for -0 case.
    return value.isZero() ? Decimal(0) : value;
}

double parseToDoubleForNumberType(const String& string, double fallbackValue)
{
    // See HTML5 2.5.4.3 `Real numbers.'

    // String::toDouble() accepts leading + and whitespace characters, which are not valid here.
    UChar firstCharacter = string[0];
    if (firstCharacter != '-' && firstCharacter != '.' && !isASCIIDigit(firstCharacter))
        return fallbackValue;

    bool valid = false;
    double value = string.toDouble(&valid);
    if (!valid)
        return fallbackValue;

    // NaN and infinity are considered valid by String::toDouble, but not valid here.
    if (!std::isfinite(value))
        return fallbackValue;

    // Numbers are considered finite IEEE 754 single-precision floating point values.
    // See HTML5 2.5.4.3 `Real numbers.'
    if (-std::numeric_limits<float>::max() > value || value > std::numeric_limits<float>::max())
        return fallbackValue;

    // The following expression converts -0 to +0.
    return value ? value : 0;
}

template <typename CharacterType>
static bool parseHTMLIntegerInternal(const CharacterType* position, const CharacterType* end, int& value)
{
    // Step 3
    int sign = 1;

    // Step 4
    while (position < end) {
        if (!isHTMLSpace<CharacterType>(*position))
            break;
        ++position;
    }

    // Step 5
    if (position == end)
        return false;
    ASSERT(position < end);

    // Step 6
    if (*position == '-') {
        sign = -1;
        ++position;
    } else if (*position == '+')
        ++position;
    if (position == end)
        return false;
    ASSERT(position < end);

    // Step 7
    if (!isASCIIDigit(*position))
        return false;

    // Step 8
    StringBuilder digits;
    while (position < end) {
        if (!isASCIIDigit(*position))
            break;
        digits.append(*position++);
    }

    // Step 9
    bool ok;
    if (digits.is8Bit())
        value = sign * charactersToIntStrict(digits.characters8(), digits.length(), &ok);
    else
        value = sign * charactersToIntStrict(digits.characters16(), digits.length(), &ok);
    return ok;
}

// http://www.whatwg.org/specs/web-apps/current-work/#rules-for-parsing-integers
bool parseHTMLInteger(const String& input, int& value)
{
    // Step 1
    // Step 2
    unsigned length = input.length();
    if (!length || input.is8Bit()) {
        const LChar* start = input.characters8();
        return parseHTMLIntegerInternal(start, start + length, value);
    }

    const UChar* start = input.characters16();
    return parseHTMLIntegerInternal(start, start + length, value);
}

template <typename CharacterType>
static bool parseHTMLNonNegativeIntegerInternal(const CharacterType* position, const CharacterType* end, unsigned& value)
{
    // Step 3
    while (position < end) {
        if (!isHTMLSpace<CharacterType>(*position))
            break;
        ++position;
    }

    // Step 4
    if (position == end)
        return false;
    ASSERT(position < end);

    // Step 5
    if (*position == '+')
        ++position;

    // Step 6
    if (position == end)
        return false;
    ASSERT(position < end);

    // Step 7
    if (!isASCIIDigit(*position))
        return false;

    // Step 8
    StringBuilder digits;
    while (position < end) {
        if (!isASCIIDigit(*position))
            break;
        digits.append(*position++);
    }

    // Step 9
    bool ok;
    if (digits.is8Bit())
        value = charactersToUIntStrict(digits.characters8(), digits.length(), &ok);
    else
        value = charactersToUIntStrict(digits.characters16(), digits.length(), &ok);
    return ok;
}


// http://www.whatwg.org/specs/web-apps/current-work/#rules-for-parsing-non-negative-integers
bool parseHTMLNonNegativeInteger(const String& input, unsigned& value)
{
    // Step 1
    // Step 2
    unsigned length = input.length();
    if (length && input.is8Bit()) {
        const LChar* start = input.characters8();
        return parseHTMLNonNegativeIntegerInternal(start, start + length, value);
    }

    const UChar* start = input.characters16();
    return parseHTMLNonNegativeIntegerInternal(start, start + length, value);
}

static const char charsetString[] = "charset";
static const size_t charsetLength = sizeof("charset") - 1;

String extractCharset(const String& value)
{
    size_t pos = 0;
    unsigned length = value.length();

    while (pos < length) {
        pos = value.find(charsetString, pos, false);
        if (pos == kNotFound)
            break;

        pos += charsetLength;

        // Skip whitespace.
        while (pos < length && value[pos] <= ' ')
            ++pos;

        if (value[pos] != '=')
            continue;

        ++pos;

        while (pos < length && value[pos] <= ' ')
            ++pos;

        char quoteMark = 0;
        if (pos < length && (value[pos] == '"' || value[pos] == '\'')) {
            quoteMark = static_cast<char>(value[pos++]);
            ASSERT(!(quoteMark & 0x80));
        }

        if (pos == length)
            break;

        unsigned end = pos;
        while (end < length && ((quoteMark && value[end] != quoteMark) || (!quoteMark && value[end] > ' ' && value[end] != '"' && value[end] != '\'' && value[end] != ';')))
            ++end;

        if (quoteMark && (end == length))
            break; // Close quote not found.

        return value.substring(pos, end - pos);
    }

    return "";
}

enum Mode {
    None,
    Charset,
    Pragma,
};

WTF::TextEncoding encodingFromMetaAttributes(const HTMLAttributeList& attributes)
{
    bool gotPragma = false;
    Mode mode = None;
    String charset;
    parseHTMLInteger
    for (HTMLAttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter) {
        const String& attributeName = iter->first;
        const String& attributeValue = AtomicString(iter->second);

        if (threadSafeMatch(attributeName, http_equivAttr)) {
            if (equalIgnoringCase(attributeValue, "content-type"))
                gotPragma = true;
        } else if (charset.isEmpty()) {
            if (threadSafeMatch(attributeName, charsetAttr)) {
                charset = attributeValue;
                mode = Charset;
            } else if (threadSafeMatch(attributeName, contentAttr)) {
                charset = extractCharset(attributeValue);
                if (charset.length())
                    mode = Pragma;
            }
        }
    }

    if (mode == Charset || (mode == Pragma && gotPragma))
        return WTF::TextEncoding(stripLeadingAndTrailingHTMLSpaces(charset));

    return WTF::TextEncoding();
}

static bool threadSafeEqual(const StringImpl* a, const StringImpl* b)
{
    if (a == b)
        return true;
    if (a->hash() != b->hash())
        return false;
    return equalNonNull(a, b);
}

bool threadSafeMatch(const QualifiedName& a, const QualifiedName& b)
{
    return threadSafeEqual(a.localName().impl(), b.localName().impl());
}

bool threadSafeMatch(const String& localName, const QualifiedName& qName)
{
    return threadSafeEqual(localName.impl(), qName.localName().impl());
}

StringImpl* findStringIfStatic(const UChar* characters, unsigned length)
{
    // We don't need to try hashing if we know the string is too long.
    if (length > StringImpl::highestStaticStringLength())
        return 0;
    // computeHashAndMaskTop8Bits is the function StringImpl::hash() uses.
    unsigned hash = StringHasher::computeHashAndMaskTop8Bits(characters, length);
    const WTF::StaticStringsTable& table = StringImpl::allStaticStrings();
    ASSERT(!table.isEmpty());

    WTF::StaticStringsTable::const_iterator it = table.find(hash);
    if (it == table.end())
        return 0;
    // It's possible to have hash collisions between arbitrary strings and
    // known identifiers (e.g. "bvvfg" collides with "script").
    // However ASSERTs in StringImpl::createStatic guard against there ever being collisions
    // between static strings.
    if (!equal(it->value, characters, length))
        return 0;
    return it->value;
}

*/

}
