#pragma once

#include <utility>
#include <queue>

class Mouse
{
	friend class Window;

public:
	class Event
	{
	public:
		enum Type
		{
			LMBPress,
			LMBRelease,
			RMBPress,
			RMBRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};

	private:
		Type type;
		bool lmbPressed;
		bool rmbPressed;
		int x;
		int y;

	public:
		Event() : type(Type::Invalid), lmbPressed( false ), rmbPressed( false ), x( 0 ), y( 0 ) {}
		Event(Type type, const Mouse& parentMouse) :
			type( type ),
			lmbPressed( parentMouse.lmbPressed ), rmbPressed( parentMouse.rmbPressed ),
			x( parentMouse.GetPosX() ), y( parentMouse.GetPosY() )
		{}

		bool isValid() const { return type != Type::Invalid; }
		Type GetType() const { return type; }
		std::pair<int, int> GetPos() const { return { x, y }; }
		int GetPosX() const { return x; }
		int GetPosY() const { return y; }
		bool IsLMBPressed() const { return lmbPressed; }
		bool isRMBPressed() const { return rmbPressed; }
	};

private:
	static constexpr unsigned int bufferSize = 16u;
	int x = 0, y = 0;
	bool lmbPressed = false, rmbPressed = false;
	bool inWindow = false;
	std::queue<Event> buffer;
	int wheelCarry = 0;

public:
	Mouse() = default;
	
	std::pair<int, int> GetPos() const;
	int GetPosX() const;
	int GetPosY() const;
	bool IsLMBPressed() const;
	bool IsRMBPressed() const;
	Mouse::Event Read();
	bool IsEmpty() const;
	bool IsInWindow() const;
	void Flush();

private:
	void OnMove(int x, int y);
	void OnLMBPressed();
	void OnRMBPressed();
	void OnLMBReleased();
	void OnRMBReleased();
	void OnWheel(int delta);
	void OnWheelUp();
	void OnWheelDown();
	void OnEnter();
	void OnLeave();
	void TrimBuffer();
};