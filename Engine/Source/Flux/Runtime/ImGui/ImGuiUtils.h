#pragma once

#include "ImGuiRenderer.h"

namespace Flux {

	namespace UI {

		const char* CreateUniqueID();

		void PushID();
		void PopID();

		void BeginPropertyGrid();
		void EndPropertyGrid();

		void PushFormat(const char* format);
		void PopFormat(uint32 count = 1);

		bool Property(std::string_view label, int32& value, float speed = 1.0f, int32 minValue = 0, int32 maxValue = 0);

		bool Property(std::string_view label, float& value, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f);
		bool Property(std::string_view label, Vector2& value, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f);
		bool Property(std::string_view label, Vector3& value, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f);
		bool Property(std::string_view label, Vector4& value, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f);

		bool SearchBar(std::string_view id, std::string_view description, Buffer& buffer, const ImVec2& size = { 400.0f, 25.0f });
		bool BreadcrumbBar(std::string_view id, const std::filesystem::path& rootDirectory, const std::filesystem::path& currentDirectory, std::filesystem::path* newDirectory = nullptr);

		void Image(Ref<Texture> texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0.0f, 0.0f), const ImVec2& uv1 = ImVec2(1.0f, 1.0f));
		
		ImFont* GetFont(FontType type = FontType::Default, FontWeight weight = FontWeight::Regular, uint32 size = 16);

	}

}