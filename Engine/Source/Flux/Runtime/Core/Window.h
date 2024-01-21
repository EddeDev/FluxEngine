#pragma once

#include "Events/EventManager.h"

namespace Flux {

	enum class WindowMode : uint8
	{
		Windowed = 0,
		BorderlessWindowed,
		Fullscreen
	};

	class Window;

	struct WindowCreateInfo
	{
		uint32 Width = 1280;
		uint32 Height = 720;
		std::string Title = "Flux Engine";
		bool Resizable = true;
		bool Decorated = true;
		WindowMode Mode = WindowMode::Windowed;

		Ref<Window> ParentWindow;
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
	using WindowMenu = void*;

	class Window : public ReferenceCounted
	{
	public:
		virtual ~Window() {}

		virtual void SetVisible(bool visible) const = 0;
		virtual bool IsVisible() const = 0;

		virtual WindowMenu CreateMenu() const = 0;
		virtual bool SetMenu(WindowMenu menu) const = 0;
		virtual bool AddMenu(WindowMenu menu, uint32 menuID = 0, const char* name = "", bool disabled = false) const = 0;
		virtual bool AddMenuSeparator(WindowMenu menu) const = 0;
		virtual bool AddPopupMenu(WindowMenu menu, WindowMenu childMenu, const char* name = "", bool disabled = false) const = 0;

		virtual void SetCursorShape(CursorShape shape) = 0;
		virtual CursorShape GetCursorShape() const = 0;

		virtual EventManager& GetEventManager() = 0;

		virtual uint32 GetWidth() const = 0;
		virtual uint32 GetHeight() const = 0;

		virtual WindowHandle GetNativeHandle() const = 0;

		static Ref<Window> Create(const WindowCreateInfo& createInfo);
	};

}