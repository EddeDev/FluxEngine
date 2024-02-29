#pragma once

#include "Flux/Runtime/Core/Events/Event.h"

#include "Flux/Runtime/Scene/Scene.h"
#include "Flux/Runtime/Scene/Entity.h"

namespace Flux {

	class EditorWindow : public ReferenceCounted
	{
	public:
		virtual ~EditorWindow() {}

		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		void SetScene(Ref<Scene> scene) { m_Scene = scene; }
	protected:
		Ref<Scene> m_Scene;

		Vector2 m_Padding = { 8.0f, 8.0f };
		Vector2 m_ItemSpacing = { 8.0f, 4.0f };

		bool m_NoDecoration = false;
		bool m_NoScrollbar = false;
		bool m_NoScrollWithMouse = false;

		friend class EditorWindowManager;
	};

	struct EditorWindowData
	{
		Ref<EditorWindow> Window;
		size_t HashCode = 0;
		std::string_view Title;
		bool IsOpen = false;
	};

	class EditorWindowManager
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnUpdate();
		static void OnImGuiRender();
		static void OnEvent(Event& event);

		static void SetActiveScene(Ref<Scene> scene) { s_ActiveScene = scene; }

		template<typename T, typename... TArgs>
		static Ref<T> AddWindow(std::string_view title, bool open = true, TArgs&&... args)
		{
			static_assert(std::is_base_of<EditorWindow, T>::value);

			size_t hashCode = typeid(T).hash_code();
			if (auto it = s_Windows.find(hashCode); it != s_Windows.end())
			{
				FLUX_WARNING_CATEGORY("Editor Window Manager", "Window '{0}' already exists! ({1})", it->second.Title, it->second.HashCode);
				return it->second.Window.As<T>();
			}

			FLUX_TRACE_CATEGORY("Editor Window Manager", "Creating window '{0}' ({1})", title, hashCode);

			Ref<T> window = Ref<T>::Create(std::forward<TArgs>(args)...);

			auto& windowData = s_Windows[hashCode];
			windowData.Window = window;
			windowData.HashCode = hashCode;
			windowData.Title = title;
			windowData.IsOpen = open;

			return window;
		}

		template<typename T>
		static Ref<T> GetWindow()
		{
			static_assert(std::is_base_of<EditorWindow, T>::value);

			size_t hashCode = typeid(T).hash_code();

			auto it = s_Windows.find(hashCode);
			if (it != s_Windows.end())
				return it->second.Window.As<T>();

			return nullptr;
		}
	private:
		inline static std::unordered_map<size_t, EditorWindowData> s_Windows;
		inline static Ref<Scene> s_ActiveScene;
	};

}