#pragma once

namespace Flux {

	enum class KeyCode : uint16
	{
		Space = 32,
		Apostrophe = 39,
		Comma = 44,
		Minus = 45,
		Period = 46,
		Slash = 47,
		Alpha0 = 48,
		Alpha1 = 49,
		Alpha2 = 50,
		Alpha3 = 51,
		Alpha4 = 52,
		Alpha5 = 53,
		Alpha6 = 54,
		Alpha7 = 55,
		Alpha8 = 56,
		Alpha9 = 57,
		Semicolon = 59,
		Equal = 61,
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,
		LeftBracket = 91,
		Backslash = 92,
		RightBracket = 93,
		GraveAccent = 96,
		World1 = 161,
		World2 = 162,
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,
		Keypad0 = 320,
		Keypad1 = 321,
		Keypad2 = 322,
		Keypad3 = 323,
		Keypad4 = 324,
		Keypad5 = 325,
		Keypad6 = 326,
		Keypad7 = 327,
		Keypad8 = 328,
		Keypad9 = 329,
		KeypadDecimal = 330,
		KeypadDivide = 331,
		KeypadMultiply = 332,
		KeypardSubtract = 333,
		KeypadAdd = 334,
		KeypadEnter = 335,
		KeypadEqual = 336,
		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348,
		Last = Menu
	};

	enum class MouseButtonCode : uint8
	{
		Button1 = 0,
		Button2 = 1,
		Button3 = 2,
		Button4 = 3,
		Button5 = 4,
		Button6 = 5,
		Button7 = 6,
		Button8 = 7,

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

#define FLUX_ACTION_RELEASE 0
#define FLUX_ACTION_PRESS 1
#define FLUX_ACTION_REPEAT 2

#define FLUX_KEY_UNKNOWN -1

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

	class Input
	{
	public:
		static void Init();
		static void Shutdown();

		static void Update();

		static bool GetKey(KeyCode key);
		static bool GetKeyDown(KeyCode key);
		static bool GetKeyUp(KeyCode key);

		static bool GetMouseButton(MouseButtonCode button);
		static bool GetMouseButtonDown(MouseButtonCode button);
		static bool GetMouseButtonUp(MouseButtonCode button);

		static glm::vec3 GetMousePosition();
	private:
		static void OnKeyEvent(int32 key, int32 scancode, int32 action, int32 mods);
		static void OnMouseButtonEvent(int32 button, int32 action, int32 mods);
	};

}