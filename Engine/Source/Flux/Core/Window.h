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

	using WindowHandle = void*;

	typedef std::function<void()> WindowCloseCallback;

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual void SetVisible(bool visible) const = 0;
		virtual bool IsVisible() const = 0;

		virtual void AddCloseCallback(const WindowCloseCallback& callback) = 0;

		virtual uint32 GetWidth() const = 0;
		virtual uint32 GetHeight() const = 0;

		virtual WindowHandle GetNativeHandle() const = 0;

		static Unique<Window> Create(const WindowCreateInfo& createInfo);
	};

}