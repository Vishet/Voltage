#include "Mouse.h"

#include "VoltageWindows.h"

std::pair<int, int> Mouse::GetPos() const
{
    return { x, y };
}

int Mouse::GetPosX() const
{
    return x;
}

int Mouse::GetPosY() const
{
    return y;
}

bool Mouse::IsLMBPressed() const
{
    return lmbPressed;
}

bool Mouse::IsRMBPressed() const
{
    return rmbPressed;
}

Mouse::Event Mouse::Read()
{
    if (buffer.size() > 0)
    {
        Event event = buffer.front();
        buffer.pop();
        return event;
    }
    else
        return Event();
}

bool Mouse::IsEmpty() const
{
    return buffer.empty();
}

bool Mouse::IsInWindow() const
{
    return inWindow;
}

void Mouse::Flush()
{
    buffer = std::queue<Event>();
}

void Mouse::OnMove(int x, int y)
{
    this->x = x;
    this->y = y;

    buffer.emplace(Event(Event::Move, *this));
    TrimBuffer();
}

void Mouse::OnLMBPressed()
{
    lmbPressed = true;

    buffer.emplace(Event(Event::LMBPress, *this));
    TrimBuffer();
}

void Mouse::OnRMBPressed()
{
    rmbPressed = true;

    buffer.emplace(Event(Event::RMBPress, *this));
    TrimBuffer();
}

void Mouse::OnLMBReleased()
{
    lmbPressed = false;

    buffer.emplace(Event(Event::LMBRelease, *this));
    TrimBuffer();
}

void Mouse::OnRMBReleased()
{
    rmbPressed = false;

    buffer.emplace(Event(Event::RMBRelease, *this));
    TrimBuffer();
}

void Mouse::OnWheel(int delta)
{
    wheelCarry += delta;

    while (wheelCarry >= WHEEL_DELTA)
    {
        wheelCarry -= 120;
        OnWheelUp();
    }

    while (wheelCarry <= -WHEEL_DELTA)
    {
        wheelCarry += 120;
        OnWheelDown();
    }
}

void Mouse::OnWheelUp()
{
    buffer.emplace(Event(Event::WheelUp, *this));
    TrimBuffer();
}

void Mouse::OnWheelDown()
{
    buffer.emplace(Event(Event::WheelDown, *this));
    TrimBuffer();
}

void Mouse::OnEnter()
{
    inWindow = true;
    buffer.emplace(Event(Event::Enter, *this));
    TrimBuffer();
}

void Mouse::OnLeave()
{
    inWindow = false;
    buffer.emplace(Event(Event::Leave, *this));
    TrimBuffer();
}

void Mouse::TrimBuffer()
{
    while (buffer.size() > bufferSize)
    {
        buffer.pop();
    }
}
