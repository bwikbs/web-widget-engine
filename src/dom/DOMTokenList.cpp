#include "StarFishConfig.h"
#include "DOMTokenList.h"
#include "dom/Document.h"
#include "dom/Element.h"

namespace StarFish {

void DOMTokenList::tokenize(std::vector<String*, gc_allocator<String*> >* tokens, String* src)
{
    tokens->clear();

    const char* data = src->utf8Data();
    size_t length = strlen(data);

    bool isWhiteSpaceState = true;

    std::string str;
    bool inQuotationMarks = false;
    for (size_t i = 0; i < length; i++) {
        if (data[i] == '"' || data[i] == '\'') {
            if (inQuotationMarks)
                inQuotationMarks = false;
            else
                inQuotationMarks = true;
        }
        if (isWhiteSpaceState) {
            if (!String::isSpaceOrNewline(data[i])) {
                isWhiteSpaceState = false;
                str += data[i];
            } else
                continue;
        } else {
            if (String::isSpaceOrNewline(data[i]) && !inQuotationMarks) {
                isWhiteSpaceState = true;
                tokens->push_back(String::fromUTF8(str.data(), str.length()));
                str.clear();
            } else {
                str += data[i];
            }
        }
    }

    if (str.length()) {
        tokens->push_back(String::fromUTF8(str.data(), str.length()));
    }
}

unsigned long DOMTokenList::length()
{
    std::vector<String*, gc_allocator<String*> > tokens;
    String* src = m_element->getAttribute(m_localName);
    tokenize(&tokens, src);
    return tokens.size();
}

String* DOMTokenList::item(unsigned long index)
{
    std::vector<String*, gc_allocator<String*> > tokens;
    String* src = m_element->getAttribute(m_localName);
    tokenize(&tokens, src);
    if (index < tokens.size())
        return tokens.at(index);
    return nullptr;
}

bool DOMTokenList::contains(String* token)
{
    validateToken(token);

    std::vector<String*, gc_allocator<String*> > tokens;
    String* src = m_element->getAttribute(m_localName);
    tokenize(&tokens, src);
    for (unsigned i = 0; i < tokens.size(); i++) {
        if (tokens.at(i)->equals(token))
            return true;
    }
    return false;
}

String* DOMTokenList::addSingleToken(String* src, std::vector<String*, gc_allocator<String*> >* tokens, String* token)
{
    bool matched = false;
    for (unsigned j = 0; j < tokens->size(); j++) {
        if (token->equals(tokens->at(j))) {
            matched = true;
            break;
        }
    }
    if (!matched) {
        if (src->length() > 0)
            return src->concat(String::spaceString)->concat(token);
        else
            return src->concat(token);
    }
    return src;
}

void DOMTokenList::add(std::vector<String*, gc_allocator<String*> >* tokensToAdd)
{
    String* str = m_element->getAttribute(m_localName);
    std::vector<String*, gc_allocator<String*> > tokens;
    tokenize(&tokens, str);
    for (unsigned i = 0; i < tokensToAdd->size(); i++) {
        validateToken(tokensToAdd->at(i));
        str = addSingleToken(str, &tokens, tokensToAdd->at(i));
    }
    m_element->setAttribute(m_localName, str);
}

int DOMTokenList::checkMatchedTokens(bool* matchFlags, std::vector<String*, gc_allocator<String*> >* tokens, String* token)
{
    int count = 0;
    for (unsigned i = 0; i < tokens->size(); i++) {
        if (tokens->at(i)->equals(token)) {
            matchFlags[i] = true;
            count++;
        }
    }
    return count;
}

void DOMTokenList::remove(String* token)
{
    String* src = m_element->getAttribute(m_localName);
    String* dst;
    std::vector<String*, gc_allocator<String*> > tokens;
    tokenize(&tokens, src);

    bool* matchFlags = new bool[tokens.size()];
    int matchCount = checkMatchedTokens(matchFlags, &tokens, token);

    if (matchCount > 0) {
        bool isEmpty = true;
        for (unsigned i = 0; i < tokens.size(); i++) {
            if (!matchFlags[i]) {
                if (isEmpty) {
                    dst = tokens.at(i);
                    isEmpty = false;
                } else {
                    dst = dst->concat(String::spaceString)->concat(tokens.at(i));
                }
            }
        }
        m_element->setAttribute(m_localName, dst);
    }
}

void DOMTokenList::remove(std::vector<String*, gc_allocator<String*> >* tokensToRemove)
{
    String* src = m_element->getAttribute(m_localName);
    String* dst;
    std::vector<String*, gc_allocator<String*> > tokens;
    tokenize(&tokens, src);
    bool* matchFlags = new bool[tokens.size()];
    int matchCount = 0;
    for (unsigned i = 0; i < tokensToRemove->size(); i++) {
        validateToken(tokensToRemove->at(i));
        matchCount += checkMatchedTokens(matchFlags, &tokens, tokensToRemove->at(i));
    }

    if (matchCount > 0) {
        bool isEmpty = true;
        for (unsigned i = 0; i < tokens.size(); i++) {
            if (!matchFlags[i]) {
                if (isEmpty) {
                    dst = tokens.at(i);
                    isEmpty = false;
                } else {
                    dst = dst->concat(String::spaceString)->concat(tokens.at(i));
                }
            }
        }
        m_element->setAttribute(m_localName, dst);
    }
}

bool DOMTokenList::toggle(String* token, bool isForced, bool forceValue)
{
    validateToken(token);
    std::vector<String*, gc_allocator<String*> > tokens;
    String* str = m_element->getAttribute(m_localName);
    tokenize(&tokens, str);
    bool needAdd = false;
    if (isForced) {
        if (forceValue) {
            needAdd = true;
        }
    } else {
        bool* matchFlags = new bool[tokens.size()];
        int matchCount = checkMatchedTokens(matchFlags, &tokens, token);
        if (matchCount == 0) {
            needAdd = true;
        }
    }
    if (needAdd) {
        str = addSingleToken(str, &tokens, token);
        m_element->setAttribute(m_localName, str);
    } else
        remove(token);
    return needAdd;
}

// Throw Exceptions
void DOMTokenList::validateToken(String* token)
{
    std::string stdToken = token->utf8Data();
    if (stdToken.length() == 0)
        throw new DOMException(m_element->document()->scriptBindingInstance(), DOMException::Code::SYNTAX_ERR);
    auto f = [] (char c) { return std::isspace(static_cast<unsigned char>(c)); };
    if (std::find_if(stdToken.begin(), stdToken.end(), f) != stdToken.end())
        throw new DOMException(m_element->document()->scriptBindingInstance(), DOMException::Code::INVALID_CHARACTER_ERR);
}
}
