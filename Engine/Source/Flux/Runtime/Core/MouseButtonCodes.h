#pragma once

#include "BaseTypes.h"

#pragma region MouseButtonCodes
#define FLUX_MOUSE_BUTTON_1      0
#define FLUX_MOUSE_BUTTON_2      1
#define FLUX_MOUSE_BUTTON_3      2
#define FLUX_MOUSE_BUTTON_4      3
#define FLUX_MOUSE_BUTTON_5      4
#define FLUX_MOUSE_BUTTON_6      5
#define FLUX_MOUSE_BUTTON_7      6
#define FLUX_MOUSE_BUTTON_8      7
#define FLUX_MOUSE_BUTTON_LAST   FLUX_MOUSE_BUTTON_8
#define FLUX_MOUSE_BUTTON_LEFT   FLUX_MOUSE_BUTTON_1
#define FLUX_MOUSE_BUTTON_RIGHT  FLUX_MOUSE_BUTTON_2
#define FLUX_MOUSE_BUTTON_MIDDLE FLUX_MOUSE_BUTTON_3
#pragma endregion MouseButtonCodes

namespace Flux {

	enum class MouseButtonCode : uint8
	{
		Button1 = FLUX_MOUSE_BUTTON_1,
		Button2 = FLUX_MOUSE_BUTTON_2,
		Button3 = FLUX_MOUSE_BUTTON_3,
		Button4 = FLUX_MOUSE_BUTTON_4,
		Button5 = FLUX_MOUSE_BUTTON_5,
		Button6 = FLUX_MOUSE_BUTTON_6,
		Button7 = FLUX_MOUSE_BUTTON_7,
		Button8 = FLUX_MOUSE_BUTTON_8,

		ButtonLast = Button8,
		ButtonLeft = Button1,
		ButtonRight = Button2,
		ButtonMiddle = Button3
	};

}