#pragma once

namespace Flux {

	enum class WindowMode : uint8
	{
		Windowed = 0,
		BorderlessWindowed,
		Fullscreen
	};

	struct WindowCreateInfo
	{
		uint32 Width = 1280;
		uint32 Height = 720;
		std::string Title = "Flux Engine";
		bool Resizable = true;
		bool Decorated = true;
		WindowMode Mode = WindowMode::Windowed;
	};

	enum class CursorShape : uint8
	{
		None = 0,

		Arrow,
		IBeam,
		Cross,
		Hand,
		ResizeEW,
		ResizeNS,
		ResizeNWSE,
		ResizeNESW,
		ResizeAll,
		NotAllowed
	};

	using WindowHandle = void*;

	typedef std::function<void()> WindowCloseCallback;
	typedef std::function<void(uint32, uint32)> WindowSizeCallback;
	typedef std::function<void(bool)> WindowFocusCallback;
	typedef std::function<void(int32, int32, int32, int32)> KeyCallback;
	typedef std::function<void(char32)> CharCallback;
	typedef std::function<void(int32, int32, int32)> MouseButtonCallback;
	typedef std::function<void(float, float)> MouseMoveCallback;
	typedef std::function<void(float, float)> MouseWheelCallback;

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual void SetVisible(bool visible) const = 0;
		virtual bool IsVisible() const = 0;

		virtual void SetCursorShape(CursorShape shape) = 0;
		virtual CursorShape GetCursorShape() const = 0;

		virtual void AddCloseCallback(const WindowCloseCallback& callback) = 0;
		virtual void AddSizeCallback(const WindowSizeCallback& callback) = 0;
		virtual void AddFocusCallback(const WindowFocusCallback& callback) = 0;
		virtual void AddKeyCallback(const KeyCallback& callback) = 0;
		virtual void AddCharCallback(const CharCallback& callback) = 0;
		virtual void AddMouseButtonCallback(const MouseButtonCallback& callback) = 0;
		virtual void AddMouseMoveCallback(const MouseMoveCallback& callback) = 0;
		virtual void AddMouseWheelCallback(const MouseWheelCallback& callback) = 0;

		virtual uint32 GetWidth() const = 0;
		virtual uint32 GetHeight() const = 0;

		virtual WindowHandle GetNativeHandle() const = 0;

		static Unique<Window> Create(const WindowCreateInfo& createInfo);
	};

}