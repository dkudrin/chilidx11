#include "KeyBoard.h"



KeyBoard::Event KeyBoard::ReadKey()
{
	if (keybuffer.size() > 0U)
	{
		KeyBoard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	else
	{
		return KeyBoard::Event(); // returns invalid
	}
}

bool KeyBoard::KeysIsEmpty() const
{
	return keybuffer.empty();
}

void KeyBoard::FlushKey()
{
	keybuffer = std::queue<Event>();
}

char KeyBoard::ReadChar()
{
	if (charbuffer.size() > 0u)
	{
		unsigned char charcode = charbuffer.front();
		charbuffer.pop();
		return charcode;
	}
	else
	{
		return 0;
	}
}

bool KeyBoard::CharIsEmpty() const
{
	return charbuffer.empty();
}

void KeyBoard::FlushChar()
{
	charbuffer = std::queue<char>();
}

void KeyBoard::Flush()
{
	FlushKey();
	FlushChar();
}

void KeyBoard::EnableAutorepeat()
{
	autorepeatEnabled = true;
}

void KeyBoard::DisableAutorepeat()
{
	autorepeatEnabled = false;
}

bool KeyBoard::AutorepeatIsEnabled() const
{
	return autorepeatEnabled;
}

void KeyBoard::OnKeyPressed(unsigned char keycode)
{
	keystates[keycode] = true;
	keybuffer.push(KeyBoard::Event(KeyBoard::Event::Type::Press, keycode));
	TrimBuffer(keybuffer);
}

bool KeyBoard::KeyIsPressed(unsigned char keycode) const
{
	return keystates[keycode];
}

void KeyBoard::OnKeyReleased(unsigned char keycode)
{
	keystates[keycode] = false;
	keybuffer.push(KeyBoard::Event(KeyBoard::Event::Type::Press, keycode));
	TrimBuffer(keybuffer);
}

void KeyBoard::OnChar(char character)
{
	charbuffer.push(character);
	TrimBuffer(charbuffer);
}

void KeyBoard::ClearState()
{
	keystates.reset();
}

template<typename T>
void KeyBoard::TrimBuffer(std::queue<T>& buffer)
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}