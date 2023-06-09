#pragma once

namespace Flux {

	class Window;

	struct WindowCreateInfo
	{
		uint32 Width = 1280;
		uint32 Height = 720;
		std::string Title = "Flux Engine";
		bool Maximized = false;
		bool Resizable = true;
		bool Decorated = true;

		Window* ParentWindow = nullptr;
	};

	using WindowHandle = void*;
	using WindowMenu = void*;

	typedef std::function<void()> WindowCloseCallback;
	typedef std::function<void(bool)> WindowMinimizeCallback;
	typedef std::function<void(uint32, uint32)> WindowSizeCallback;
	typedef std::function<void(WindowMenu, uint32)> WindowMenuCallback;
	typedef std::function<void(const char** paths, uint32)> WindowDropCallback;
	typedef std::function<void(int32, int32, int32, int32)> KeyCallback;
	typedef std::function<void(int32, int32, int32)> MouseButtonCallback;
	typedef std::function<void(float, float)> MouseMoveCallback;

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual WindowMenu CreateMenu() const = 0;
		virtual bool SetMenu(WindowMenu menu) const = 0;
		virtual bool AddMenu(WindowMenu menu, uint32 itemID = 0, const char* name = "", bool disabled = false) const = 0;
		virtual bool AddMenuSeparator(WindowMenu menu) const = 0;
		virtual bool AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name = "", bool disabled = false) const = 0;

		virtual void SetVisible(bool visible) const = 0;
		virtual bool IsVisible() const = 0;

		virtual void AddCloseCallback(const WindowCloseCallback& callback) = 0;
		virtual void AddMinimizeCallback(const WindowMinimizeCallback& callback) = 0;
		virtual void AddSizeCallback(const WindowSizeCallback& callback) = 0;
		virtual void AddMenuCallback(const WindowMenuCallback& callback) = 0;
		virtual void AddDropCallback(const WindowDropCallback& callback) = 0;
		virtual void AddKeyCallback(const KeyCallback& callback) = 0;
		virtual void AddMouseButtonCallback(const MouseButtonCallback& callback) = 0;
		virtual void AddMouseMoveCallback(const MouseMoveCallback& callback) = 0;

		virtual uint32 GetWidth() const = 0;
		virtual uint32 GetHeight() const = 0;

		virtual WindowHandle GetNativeHandle() const = 0;

		static Unique<Window> Create(const WindowCreateInfo& createInfo);
	};

}