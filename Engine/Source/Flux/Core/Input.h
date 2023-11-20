#pragma once

#pragma region KeyCodes
#define FLUX_KEY_UNKNOWN       -1
#define FLUX_KEY_SPACE         32
#define FLUX_KEY_APOSTROPHE    39 
#define FLUX_KEY_COMMA         44 
#define FLUX_KEY_MINUS         45 
#define FLUX_KEY_PERIOD        46 
#define FLUX_KEY_SLASH         47 
#define FLUX_KEY_0             48
#define FLUX_KEY_1             49
#define FLUX_KEY_2             50
#define FLUX_KEY_3             51
#define FLUX_KEY_4             52
#define FLUX_KEY_5             53
#define FLUX_KEY_6             54
#define FLUX_KEY_7             55
#define FLUX_KEY_8             56
#define FLUX_KEY_9             57
#define FLUX_KEY_SEMICOLON     59 
#define FLUX_KEY_EQUAL         61 
#define FLUX_KEY_A             65
#define FLUX_KEY_B             66
#define FLUX_KEY_C             67
#define FLUX_KEY_D             68
#define FLUX_KEY_E             69
#define FLUX_KEY_F             70
#define FLUX_KEY_G             71
#define FLUX_KEY_H             72
#define FLUX_KEY_I             73
#define FLUX_KEY_J             74
#define FLUX_KEY_K             75
#define FLUX_KEY_L             76
#define FLUX_KEY_M             77
#define FLUX_KEY_N             78
#define FLUX_KEY_O             79
#define FLUX_KEY_P             80
#define FLUX_KEY_Q             81
#define FLUX_KEY_R             82
#define FLUX_KEY_S             83
#define FLUX_KEY_T             84
#define FLUX_KEY_U             85
#define FLUX_KEY_V             86
#define FLUX_KEY_W             87
#define FLUX_KEY_X             88
#define FLUX_KEY_Y             89
#define FLUX_KEY_Z             90
#define FLUX_KEY_LEFT_BRACKET  91 
#define FLUX_KEY_BACKSLASH     92 
#define FLUX_KEY_RIGHT_BRACKET 93 
#define FLUX_KEY_GRAVE_ACCENT  96 
#define FLUX_KEY_WORLD_1       161
#define FLUX_KEY_WORLD_2       162
#define FLUX_KEY_ESCAPE        256
#define FLUX_KEY_ENTER         257
#define FLUX_KEY_TAB           258
#define FLUX_KEY_BACKSPACE     259
#define FLUX_KEY_INSERT        260
#define FLUX_KEY_DELETE        261
#define FLUX_KEY_RIGHT         262
#define FLUX_KEY_LEFT          263
#define FLUX_KEY_DOWN          264
#define FLUX_KEY_UP            265
#define FLUX_KEY_PAGE_UP       266
#define FLUX_KEY_PAGE_DOWN     267
#define FLUX_KEY_HOME          268
#define FLUX_KEY_END           269
#define FLUX_KEY_CAPS_LOCK     280
#define FLUX_KEY_SCROLL_LOCK   281
#define FLUX_KEY_NUM_LOCK      282
#define FLUX_KEY_PRINT_SCREEN  283
#define FLUX_KEY_PAUSE         284
#define FLUX_KEY_F1            290
#define FLUX_KEY_F2            291
#define FLUX_KEY_F3            292
#define FLUX_KEY_F4            293
#define FLUX_KEY_F5            294
#define FLUX_KEY_F6            295
#define FLUX_KEY_F7            296
#define FLUX_KEY_F8            297
#define FLUX_KEY_F9            298
#define FLUX_KEY_F10           299
#define FLUX_KEY_F11           300
#define FLUX_KEY_F12           301
#define FLUX_KEY_F13           302
#define FLUX_KEY_F14           303
#define FLUX_KEY_F15           304
#define FLUX_KEY_F16           305
#define FLUX_KEY_F17           306
#define FLUX_KEY_F18           307
#define FLUX_KEY_F19           308
#define FLUX_KEY_F20           309
#define FLUX_KEY_F21           310
#define FLUX_KEY_F22           311
#define FLUX_KEY_F23           312
#define FLUX_KEY_F24           313
#define FLUX_KEY_F25           314
#define FLUX_KEY_KP_0          320
#define FLUX_KEY_KP_1          321
#define FLUX_KEY_KP_2          322
#define FLUX_KEY_KP_3          323
#define FLUX_KEY_KP_4          324
#define FLUX_KEY_KP_5          325
#define FLUX_KEY_KP_6          326
#define FLUX_KEY_KP_7          327
#define FLUX_KEY_KP_8          328
#define FLUX_KEY_KP_9          329
#define FLUX_KEY_KP_DECIMAL    330
#define FLUX_KEY_KP_DIVIDE     331
#define FLUX_KEY_KP_MULTIPLY   332
#define FLUX_KEY_KP_SUBTRACT   333
#define FLUX_KEY_KP_ADD        334
#define FLUX_KEY_KP_ENTER      335
#define FLUX_KEY_KP_EQUAL      336
#define FLUX_KEY_LEFT_SHIFT    340
#define FLUX_KEY_LEFT_CONTROL  341
#define FLUX_KEY_LEFT_ALT      342
#define FLUX_KEY_LEFT_SUPER    343
#define FLUX_KEY_RIGHT_SHIFT   344
#define FLUX_KEY_RIGHT_CONTROL 345
#define FLUX_KEY_RIGHT_ALT     346
#define FLUX_KEY_RIGHT_SUPER   347
#define FLUX_KEY_MENU          348
#define FLUX_KEY_LAST          FLUX_KEY_MENU
#pragma endregion KeyCodes

#pragma region Modifiers
#define FLUX_MOD_SHIFT     0x0001
#define FLUX_MOD_CONTROL   0x0002
#define FLUX_MOD_ALT       0x0004
#define FLUX_MOD_SUPER     0x0008
#define FLUX_MOD_CAPS_LOCK 0x0010
#define FLUX_MOD_NUM_LOCK  0x0020
#define FLUX_MOD_MASK (FLUX_MOD_SHIFT | \
                       FLUX_MOD_CONTROL | \
                       FLUX_MOD_ALT | \
                       FLUX_MOD_SUPER | \
                       FLUX_MOD_CAPS_LOCK | \
                       FLUX_MOD_NUM_LOCK)
#pragma endregion Modifiers

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

#define FLUX_ACTION_RELEASE 0
#define FLUX_ACTION_PRESS 1
#define FLUX_ACTION_REPEAT 2

namespace Flux {

	enum class KeyCode : uint16
	{
		Space = FLUX_KEY_SPACE,
		Apostrophe = FLUX_KEY_APOSTROPHE,
		Comma = FLUX_KEY_COMMA,
		Minus = FLUX_KEY_MINUS,
		Period = FLUX_KEY_PERIOD,
		Slash = FLUX_KEY_SLASH,
		Alpha0 = FLUX_KEY_0,
		Alpha1 = FLUX_KEY_1,
		Alpha2 = FLUX_KEY_2,
		Alpha3 = FLUX_KEY_3,
		Alpha4 = FLUX_KEY_4,
		Alpha5 = FLUX_KEY_5,
		Alpha6 = FLUX_KEY_6,
		Alpha7 = FLUX_KEY_7,
		Alpha8 = FLUX_KEY_8,
		Alpha9 = FLUX_KEY_9,
		Semicolon = FLUX_KEY_SEMICOLON,
		Equal = FLUX_KEY_EQUAL,
		A = FLUX_KEY_A,
		B = FLUX_KEY_B,
		C = FLUX_KEY_C,
		D = FLUX_KEY_D,
		E = FLUX_KEY_E,
		F = FLUX_KEY_F,
		G = FLUX_KEY_G,
		H = FLUX_KEY_H,
		I = FLUX_KEY_I,
		J = FLUX_KEY_J,
		K = FLUX_KEY_K,
		L = FLUX_KEY_L,
		M = FLUX_KEY_M,
		N = FLUX_KEY_N,
		O = FLUX_KEY_O,
		P = FLUX_KEY_P,
		Q = FLUX_KEY_Q,
		R = FLUX_KEY_R,
		S = FLUX_KEY_S,
		T = FLUX_KEY_T,
		U = FLUX_KEY_U,
		V = FLUX_KEY_V,
		W = FLUX_KEY_W,
		X = FLUX_KEY_X,
		Y = FLUX_KEY_Y,
		Z = FLUX_KEY_Z,
		LeftBracket = FLUX_KEY_LEFT_BRACKET,
		Backslash = FLUX_KEY_BACKSLASH,
		RightBracket = FLUX_KEY_RIGHT_BRACKET,
		GraveAccent = FLUX_KEY_GRAVE_ACCENT,
		World1 = FLUX_KEY_WORLD_1,
		World2 = FLUX_KEY_WORLD_2,
		Escape = FLUX_KEY_ESCAPE,
		Enter = FLUX_KEY_ENTER,
		Tab = FLUX_KEY_TAB,
		Backspace = FLUX_KEY_BACKSPACE,
		Insert = FLUX_KEY_INSERT,
		Delete = FLUX_KEY_DELETE,
		Right = FLUX_KEY_RIGHT,
		Left = FLUX_KEY_LEFT,
		Down = FLUX_KEY_DOWN,
		Up = FLUX_KEY_UP,
		PageUp = FLUX_KEY_PAGE_UP,
		PageDown = FLUX_KEY_PAGE_DOWN,
		Home = FLUX_KEY_HOME,
		End = FLUX_KEY_END,
		CapsLock = FLUX_KEY_CAPS_LOCK,
		ScrollLock = FLUX_KEY_SCROLL_LOCK,
		NumLock = FLUX_KEY_NUM_LOCK,
		PrintScreen = FLUX_KEY_PRINT_SCREEN,
		Pause = FLUX_KEY_PAUSE,
		F1 = FLUX_KEY_F1,
		F2 = FLUX_KEY_F2,
		F3 = FLUX_KEY_F3,
		F4 = FLUX_KEY_F4,
		F5 = FLUX_KEY_F5,
		F6 = FLUX_KEY_F6,
		F7 = FLUX_KEY_F7,
		F8 = FLUX_KEY_F8,
		F9 = FLUX_KEY_F9,
		F10 = FLUX_KEY_F10,
		F11 = FLUX_KEY_F11,
		F12 = FLUX_KEY_F12,
		F13 = FLUX_KEY_F13,
		F14 = FLUX_KEY_F14,
		F15 = FLUX_KEY_F15,
		F16 = FLUX_KEY_F16,
		F17 = FLUX_KEY_F17,
		F18 = FLUX_KEY_F18,
		F19 = FLUX_KEY_F19,
		F20 = FLUX_KEY_F20,
		F21 = FLUX_KEY_F21,
		F22 = FLUX_KEY_F22,
		F23 = FLUX_KEY_F23,
		F24 = FLUX_KEY_F24,
		F25 = FLUX_KEY_F25,
		Keypad0 = FLUX_KEY_KP_0,
		Keypad1 = FLUX_KEY_KP_1,
		Keypad2 = FLUX_KEY_KP_2,
		Keypad3 = FLUX_KEY_KP_3,
		Keypad4 = FLUX_KEY_KP_4,
		Keypad5 = FLUX_KEY_KP_5,
		Keypad6 = FLUX_KEY_KP_6,
		Keypad7 = FLUX_KEY_KP_7,
		Keypad8 = FLUX_KEY_KP_8,
		Keypad9 = FLUX_KEY_KP_9,
		KeypadDecimal = FLUX_KEY_KP_DECIMAL,
		KeypadDivide = FLUX_KEY_KP_DIVIDE,
		KeypadMultiply = FLUX_KEY_KP_MULTIPLY,
		KeypardSubtract = FLUX_KEY_KP_SUBTRACT,
		KeypadAdd = FLUX_KEY_KP_ADD,
		KeypadEnter = FLUX_KEY_KP_ENTER,
		KeypadEqual = FLUX_KEY_KP_EQUAL,
		LeftShift = FLUX_KEY_LEFT_SHIFT,
		LeftControl = FLUX_KEY_LEFT_CONTROL,
		LeftAlt = FLUX_KEY_LEFT_ALT,
		LeftSuper = FLUX_KEY_LEFT_SUPER,
		RightShift = FLUX_KEY_RIGHT_SHIFT,
		RightControl = FLUX_KEY_RIGHT_CONTROL,
		RightAlt = FLUX_KEY_RIGHT_ALT,
		RightSuper = FLUX_KEY_RIGHT_SUPER,
		Menu = FLUX_KEY_MENU,
		Last = Menu
	};

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

	enum class KeyState : uint8
	{
		None = 0,
		Pressed,
		Released,
		Repeated
	};

	enum class MouseButtonState : uint8
	{
		None = 0,
		Pressed,
		Released
	};



}