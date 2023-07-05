#pragma once

#include "Flux/Runtime/Core/Application.h"
#include "Flux/Runtime/Renderer/RenderPipeline.h"

namespace Flux {

	struct OrthographicCamera
	{
	public:
		glm::mat4 ViewMatrix = glm::mat4(1.0f);
		glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 InverseViewProjectionMatrix = glm::mat4(1.0f);
	private:
		glm::vec2 Position = glm::vec2(0.0f);
		float ZoomLevel = 1.0f;
		float AspectRatio = 0.0f;
	public:
		OrthographicCamera() = default;
		OrthographicCamera(int32 width, int32 height)
		{
			SetViewportSize(width, height);
		}
	private:
		void RecalculateProjectionMatrix()
		{
			ProjectionMatrix = glm::ortho(-AspectRatio * ZoomLevel, AspectRatio * ZoomLevel, -ZoomLevel, ZoomLevel, -1.0f, 1.0f);
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			InverseViewProjectionMatrix = glm::inverse(ViewProjectionMatrix);
		}

		void RecalculateViewMatrix()
		{
			ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(Position, 0.0f)));
			ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			InverseViewProjectionMatrix = glm::inverse(ViewProjectionMatrix);
		}
	public:
		void SetViewportSize(int32 width, int32 height)
		{
			AspectRatio = (float)width / (float)height;
			RecalculateProjectionMatrix();
		}

		void SetPosition(const glm::vec2& position)
		{
			Position = position;
			RecalculateViewMatrix();
		}

		glm::vec2 GetPosition() const
		{
			return Position;
		}

		void SetZoomLevel(float zoomLevel)
		{
			ZoomLevel = zoomLevel;
			RecalculateProjectionMatrix();
		}

		float GetZoomLevel() const
		{
			return ZoomLevel;
		}

		float GetAspectRatio() const
		{
			return AspectRatio;
		}
	};

	class EditorApplication : public Application
	{
	public:
		EditorApplication();
		virtual ~EditorApplication();

		virtual void OnInit() override;
		virtual void OnExit() override;
		virtual void OnUpdate() override;
	private:
		void OnMenuCallback(WindowMenu menu, uint32 menuID);
	private:
		enum MenuItem : uint32
		{
			// File
			Menu_File_NewProject,
			Menu_File_OpenProject,
			Menu_File_SaveProject,
			Menu_File_Restart,
			Menu_File_Exit,

			// Edit
			Menu_Edit_Preferences,

			// About
			Menu_About_AboutFluxEngine
		};

		std::unordered_map<MenuItem, Unique<Window>> m_Windows;

		Ref<RenderPipeline> m_RenderPipeline;
		Ref<Texture2D> m_Texture;

		OrthographicCamera m_OrthoCamera;
	};

}