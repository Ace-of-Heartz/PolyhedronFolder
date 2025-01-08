//
// Created by ace on 2024.12.27..
//

#ifndef INMEMORYTOKENIZER_H
#define INMEMORYTOKENIZER_H

#include <string>

class InMemoryTokenizer
{
public:
    InMemoryTokenizer() = default;
    void SetData( const char* ptr, size_t Length ) noexcept;
    std::string_view NextToken( bool onlySameLine = false ) noexcept;
    void ToNextLine() noexcept;
    operator bool() const noexcept;
    void ClearData() noexcept;
private:
    const char* currentPtr = nullptr;
    const char* endPtr = nullptr;
};



#endif //INMEMORYTOKENIZER_H
