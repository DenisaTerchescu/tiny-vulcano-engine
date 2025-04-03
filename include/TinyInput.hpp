#pragma once
#include <glm/vec2.hpp>

struct TinyButton
{

	char pressed = 0;
	char held = 0;
	char released = 0;
	char state = -1;

	enum
	{
		A = 0,
		B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		NR0, NR1, NR2, NR3, NR4, NR5, NR6, NR7, NR8, NR9,
		Space,
		Enter,
		Escape,
		Up,
		Down,
		Left,
		Right,
		LeftCtrl,
		Tab,
		LeftShift,
		LeftAlt,
		BUTTONS_COUNT, 
	};

    void updateTinyButton()
    {
        if (state == 1)
        {
            if (held)
            {
                pressed = false;
            }
            else
            {
                pressed = true;
            }

            held = true;
            released = false;
        }
        else if (state == 0)
        {
            held = false;
            pressed = false;
            released = true;
        }
        else
        {
            pressed = false;
            released = false;
        }

        state = -1;
    }


};


struct TinyInput {

	TinyButton keyBoard[TinyButton::BUTTONS_COUNT] = {};
	TinyButton leftMouse = {};
	TinyButton rightMouse = {};

	glm::ivec2 mousePos = {};
	glm::ivec2 lastPos = {};
	glm::ivec2 delta = {};

	void resetTinyInput()
	{
		*this = {};
	}

	void updateTinyInput()
	{
        for (int i = 0; i < TinyButton::BUTTONS_COUNT; i++)
        {
            keyBoard[i].updateTinyButton();
        }

        leftMouse.updateTinyButton();
        rightMouse.updateTinyButton();
	}

};

