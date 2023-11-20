#pragma once

#ifdef FLUX_PLATFORM_WINDOWS

#include "Flux/Core/Window.h"

namespace Flux {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowCreateInfo& createInfo);
		virtual ~WindowsWindow();

		virtual void SetVisible(bool visible) const override;
		virtual bool IsVisible() const override;

		virtual void SetCursorShape(CursorShape shape);
		virtual CursorShape GetCursorShape() const override { return m_CursorShape; }

		virtual void AddCloseCallback(const WindowCloseCallback& callback) override;
		virtual void AddSizeCallback(const WindowSizeCallback& callback) override;
		virtual void AddFocusCallback(const WindowFocusCallback& callback) override;
		virtual void AddKeyCallback(const KeyCallback& callback) override;
		virtual void AddCharCallback(const CharCallback& callback) override;
		virtual void AddMouseButtonCallback(const MouseButtonCallback& callback) override;
		virtual void AddMouseMoveCallback(const MouseMoveCallback& callback) override;
		virtual void AddMouseWheelCallback(const MouseWheelCallback& callback) override;

		virtual uint32 GetWidth() const override { return m_Width; }
		virtual uint32 GetHeight() const override { return m_Height; }

		virtual WindowHandle GetNativeHandle() const override { return m_WindowHandle; }

		int32 ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		ThreadID m_ThreadID = 0;

		std::atomic<HWND> m_WindowHandle = NULL;

		std::atomic<uint32> m_Width = 0;
		std::atomic<uint32> m_Height = 0;
		std::string m_Title;

		std::unordered_map<CursorShape, HCURSOR> m_CursorImageMap;
		std::atomic<CursorShape> m_CursorShape = CursorShape::Arrow;

		std::vector<WindowCloseCallback> m_CloseCallbacks;
		std::vector<WindowSizeCallback> m_SizeCallbacks;
		std::vector<WindowFocusCallback> m_FocusCallbacks;
		std::vector<KeyCallback> m_KeyCallbacks;
		std::vector<CharCallback> m_CharCallbacks;
		std::vector<MouseButtonCallback> m_MouseButtonCallbacks;
		std::vector<MouseMoveCallback> m_MouseMoveCallbacks;
		std::vector<MouseWheelCallback> m_MouseWheelCallbacks;

		WCHAR m_HighSurrogate = 0;
	};
}

#endif