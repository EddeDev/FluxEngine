#pragma once

#include "WindowMenu.h"

namespace Flux {

	struct WindowCreateInfo
	{
		uint32 Width = 1280;
		uint32 Height = 720;
		std::string Title = "Flux Engine";
		bool Maximized = false;
	};

	typedef std::function<void()> WindowCloseCallback;
	typedef std::function<void(uint32, uint32)> WindowSizeCallback;

	class Window
	{
	public:
		virtual void ProcessEvents() const = 0;

		virtual void AddCloseCallback(const WindowCloseCallback& callback) = 0;
		virtual void AddSizeCallback(const WindowSizeCallback& callback) = 0;

		virtual void SetMenu(Shared<WindowMenu> menu) = 0;

		static Unique<Window> Create(const WindowCreateInfo& createInfo);
	};

}