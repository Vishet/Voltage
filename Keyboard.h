#pragma once

#include <queue>
#include <bitset>

class Keyboard
{
	friend class Window;

public:
	class Event 
	{
	public:
		enum Type
		{
			Press,
			Release,
			Invalid
		};

	private:
		Type type;
		unsigned char code;

	public:
		Event() : type(Type::Invalid), code(0u) {}
		Event(Type type, unsigned char code) : type(type), code(code) {}

		bool IsPress() const { return type == Type::Press; }
		bool IsRelease() const { return type == Type::Release; }
		bool IsValid() const { return type != Type::Invalid; }
		unsigned char GetCode() const { return code; }
	};

private:
	static constexpr unsigned int keysNumber = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autorepeatEnable = false;
	std::bitset<keysNumber> keyStates;

	std::queue<Event> keyBuffer;
	std::queue<char> charBuffer;

public:
	Keyboard() = default;

	bool IsKeyPressed(unsigned char keycode) const;
	Event ReadKey();
	bool IsKeyEmpty() const;
	void FlushKey();
	
	char ReadChar();
	bool IsCharEmpty();
	void FlushChar();

	void Flush();

	void EnableAutorepeat();
	void DisableAutorepeat();
	bool IsAutorepeatEnable() const;

private:
	void OnKeyPressed(unsigned char keycode);
	void OnKeyReleased(unsigned char keycode);
	void OnChar(char character);
	void ClearState();

	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer);
};
