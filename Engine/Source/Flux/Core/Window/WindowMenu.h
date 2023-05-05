#pragma once

namespace Flux {

	using WindowMenuCallback = std::function<void()>;

	struct WindowMenu
	{
	public:
		WindowMenu(bool isSeparator = false)
			: m_IsSeparator(isSeparator) {}
		WindowMenu(std::string_view name, const WindowMenuCallback& callback, WindowMenu* parent)
			: m_Name(name), m_Callback(callback), m_Parent(parent)
		{
		}

		Shared<WindowMenu> AddMenu(std::string_view name, const WindowMenuCallback& callback = {})
		{
			Shared<WindowMenu> childMenu = CreateShared<WindowMenu>(name, callback, this);
			m_Children.push_back(childMenu);

			WindowMenu* rootMenu = this;
			while (rootMenu != nullptr)
			{
				if (rootMenu->m_Parent)
					rootMenu = rootMenu->m_Parent;
				else
					break;
			}

			childMenu->m_ID = rootMenu->m_ID;
			rootMenu->m_ID++;

			return childMenu;
		}

		Shared<WindowMenu> AddSeparator()
		{
			Shared<WindowMenu> childMenu = CreateShared<WindowMenu>(true);
			m_Children.push_back(childMenu);

			WindowMenu* rootMenu = this;
			while (rootMenu != nullptr)
			{
				if (rootMenu->m_Parent)
					rootMenu = rootMenu->m_Parent;
				else
					break;
			}

			childMenu->m_ID = rootMenu->m_ID;
			rootMenu->m_ID++;

			return childMenu;
		}

		WindowMenuCallback& GetCallback() { return m_Callback; }
		const WindowMenuCallback& GetCallback() const { return m_Callback; }

		const std::string& GetName() const { return m_Name; }
		uint32 GetID() const { return m_ID; }
		bool IsSeparator() const { return m_IsSeparator; }
		bool HasParent() const { return m_Parent != nullptr; }
		WindowMenu* GetParent() const { return m_Parent; }
		const std::vector<Shared<WindowMenu>>& GetChildren() const { return m_Children; }

		static Shared<WindowMenu> Create() { return CreateShared<WindowMenu>(); }
	private:
		std::string m_Name;
		uint32 m_ID = 0;
		bool m_IsSeparator = false;
		WindowMenuCallback m_Callback;
		WindowMenu* m_Parent = nullptr;
		std::vector<Shared<WindowMenu>> m_Children;
	};

}