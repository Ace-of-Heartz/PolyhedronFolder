//
// Created by ace on 2024.12.27..
//

#include "InMemoryTokenizer.h"

void InMemoryTokenizer::SetData( const char* ptr, size_t Length ) noexcept
{
    this->currentPtr = ptr;
    this->endPtr = ptr + Length;
}

std::string_view InMemoryTokenizer::NextToken( bool onlySameLine ) noexcept
{
    for ( ;currentPtr < endPtr
            && std::isspace(static_cast<unsigned char>(*currentPtr) ); ++currentPtr )
    {
        if ( onlySameLine && *currentPtr == '\n' )
        {
            return std::string_view();
        }
    }
    const char* tPtr = currentPtr;
    std::size_t tLength = 0;

    while ( currentPtr < endPtr
            && !std::isspace(static_cast<unsigned char>(*currentPtr) ) )
    {
        currentPtr++;
        tLength++;
    }

    return std::string_view( tPtr, tLength );
}

void InMemoryTokenizer::ToNextLine() noexcept
{
    while ( currentPtr < endPtr && *currentPtr != '\n' )
        currentPtr++;
    currentPtr++;
}

InMemoryTokenizer::operator bool() const noexcept
{
    return currentPtr < endPtr;
}

void InMemoryTokenizer::ClearData() noexcept {
    delete [] currentPtr;
}
