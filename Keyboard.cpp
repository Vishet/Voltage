#include "Keyboard.h"

bool Keyboard::IsKeyPressed(unsigned char keycode) const
{
    return keyStates[keycode];
}

Keyboard::Event Keyboard::ReadKey()
{
    if (keyBuffer.size() > 0u)
    {
        Keyboard::Event e = keyBuffer.front();
        keyBuffer.pop();
        return e;
    }
    else
    {
        return Keyboard::Event();
    }
}

bool Keyboard::IsKeyEmpty() const
{
    return keyBuffer.empty();
}

void Keyboard::FlushKey()
{
    keyBuffer = std::queue<Event>();
}

char Keyboard::ReadChar()
{
    if (charBuffer.size() > 0u)
    {
        unsigned char charcode = charBuffer.front();
        charBuffer.pop();
        return charcode;
    }
    else return 0;
}

bool Keyboard::IsCharEmpty()
{
    return charBuffer.empty();
}

void Keyboard::FlushChar()
{
    charBuffer = std::queue<char>();
}

void Keyboard::Flush()
{
    FlushKey();
    FlushChar();
}

void Keyboard::EnableAutorepeat()
{
    autorepeatEnable = true;
}

void Keyboard::DisableAutorepeat()
{
    autorepeatEnable = false;
}

bool Keyboard::IsAutorepeatEnable() const
{
    return autorepeatEnable;
}

void Keyboard::OnKeyPressed(unsigned char keycode)
{
    keyBuffer.emplace(Event(Event::Press, keycode));
    keyStates.set(keycode, true);
    TrimBuffer(keyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode)
{
    keyBuffer.emplace(Event(Event::Release, keycode));
    keyStates.set(keycode, false);
    TrimBuffer(keyBuffer);
}

void Keyboard::OnChar(char character)
{
    charBuffer.push(character);
    TrimBuffer(charBuffer);
}

void Keyboard::ClearState()
{
    keyStates.reset();
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer)
{
    while (buffer.size() > bufferSize)
    {
        buffer.pop();
    }
}




