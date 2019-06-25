#pragma once
#include <queue>
#include <bitset>

class KeyBoard
{
	friend class Window;
public:
	///////////// SubClass Event /////////////
	class Event
	{
	public:
		enum class Type // У нас в классе есть поля с типом опеределнном во встроенном подклассе
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event()
			:
			type(Type::Invalid),
			code(0u)
		{}
		Event(Type type, unsigned char code)
			:
			type(type),
			code(code)
		{}
		bool IsPress() const
		{
			return type == Type::Press;
		}
		bool IsRelease() const
		{
			return type == Type::Release;
		}
		bool IsValid() const
		{
			return type != Type::Invalid;
		}
		unsigned char GetCode() const
		{
			return code;
		}
	};

public:
	// KeyBoard constructors
	KeyBoard() = default;
	KeyBoard(const KeyBoard&) = delete;
	KeyBoard& operator=(const KeyBoard&) = delete;

	// KeyBoard public methods

	bool KeyIsPressed(unsigned char keycode) const; // check if key is pressed - kyecode is just indes in bitset
	Event ReadKey();
	bool KeysIsEmpty() const;
	void FlushKey();

	char ReadChar();
	bool CharIsEmpty() const;
	void FlushChar();
	void Flush();

	void EnableAutorepeat();
	void DisableAutorepeat();
	bool AutorepeatIsEnabled() const;

private:
	// KeyBoard private methods - will be exposed to all class friends
	// В нашем случае классом другом является Window
	// Window будет иметь доступ к приватным полям и методам
	void OnKeyPressed(unsigned char keycode);
	void OnKeyReleased(unsigned char keycode);
	void OnChar(char character);
	void ClearState();

	// STATIC
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer);

	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
private:
	// KeyBoard private vars
	bool autorepeatEnabled = false;
	int t = 10;
	std::bitset<nKeys> keystates;
	//keystates[1] = true;
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;
};

