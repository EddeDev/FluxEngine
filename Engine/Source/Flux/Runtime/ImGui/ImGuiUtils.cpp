#include "FluxPCH.h"
#include "ImGuiUtils.h"

#include "FontAwesome.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Core/RefCounting.h"

#include <stack>

namespace Flux {

	static char s_IDStringBuffer[16] = "##";
	static uint32 s_IDCounter = 0;
	static uint32 s_IDStackCounter = 0;

	static const char* s_DefaultFloatFormat = "%.2f";
	static const char* s_DefaultIntegerFormat = "%d";
	static std::stack<const char*> s_FormatStack;

	const char* UI::CreateUniqueID()
	{
		char* buffer = s_IDStringBuffer + 2;
		itoa(s_IDCounter, buffer, 16);
		s_IDCounter++;
		return s_IDStringBuffer;
	}

	void UI::PushID()
	{
		ImGui::PushID(s_IDStackCounter);
		s_IDStackCounter++;

		// Reset ID counter
		s_IDCounter = 0;
	}

	void UI::PopID()
	{
		ImGui::PopID();
		s_IDStackCounter--;
	}

	void UI::BeginPropertyGrid()
	{
		PushID();
		ImGui::Columns(2);
	}

	void UI::EndPropertyGrid()
	{
		ImGui::Columns(1);
		PopID();
	}

	void UI::PushFormat(const char* format)
	{
		s_FormatStack.push(format);
	}

	void UI::PopFormat(uint32 count)
	{
		for (uint32 i = 0; i < count; i++)
			s_FormatStack.pop();
	}

	static const char* GetFormat(const char* defaultFormat)
	{
		if (s_FormatStack.empty())
			return defaultFormat;
		return s_FormatStack.top();
	}

	static void BeginProperty(std::string_view label)
	{
		ImGui::Text(label.data());
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1.0f);
	}

	static void EndProperty()
	{
		ImGui::PopItemWidth();
		ImGui::NextColumn();
	}

	bool UI::Property(std::string_view label, int32& value, float speed, int32 minValue, int32 maxValue)
	{
		BeginProperty(label);
		bool modified = ImGui::DragInt(CreateUniqueID(), &value, speed, minValue, maxValue, GetFormat(s_DefaultIntegerFormat));
		EndProperty();
		return modified;
	}

	bool UI::Property(std::string_view label, float& value, float speed, float minValue, float maxValue)
	{
		BeginProperty(label);
		bool modified = ImGui::DragFloat(CreateUniqueID(), &value, speed, minValue, maxValue, GetFormat(s_DefaultFloatFormat));
		EndProperty();
		return modified;
	}

	bool UI::Property(std::string_view label, Vector2& value, float speed, float minValue, float maxValue)
	{
		BeginProperty(label);
		bool modified = ImGui::DragFloat2(CreateUniqueID(), value.GetPointer(), speed, minValue, maxValue, GetFormat(s_DefaultFloatFormat));
		EndProperty();
		return modified;
	}

	bool UI::Property(std::string_view label, Vector3& value, float speed, float minValue, float maxValue)
	{
		BeginProperty(label);
		bool modified = ImGui::DragFloat3(CreateUniqueID(), value.GetPointer(), speed, minValue, maxValue, GetFormat(s_DefaultFloatFormat));
		EndProperty();
		return modified;
	}

	bool UI::Property(std::string_view label, Vector4& value, float speed, float minValue, float maxValue)
	{
		BeginProperty(label);
		bool modified = ImGui::DragFloat4(CreateUniqueID(), value.GetPointer(), speed, minValue, maxValue, GetFormat(s_DefaultFloatFormat));
		EndProperty();
		return modified;
	}

	bool UI::SearchBar(std::string_view id, std::string_view description, Buffer& buffer, const ImVec2& size)
	{
		bool modified = false;

		ImGuiWindowFlags windowFlags = 0;
		windowFlags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		windowFlags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
		windowFlags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
		windowFlags |= ImGuiWindowFlags_AlwaysUseWindowPadding;

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
		ImGui::BeginChild(fmt::format("##{0}_SearchBar", id).c_str(), size, true, windowFlags);
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 1.0f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3.5f);
		ImGui::PushFont(Engine::Get().GetImGuiRenderer()->GetFont(FontType::FontAwesome, FontWeight::Solid));
		if (strlen((char*)buffer.Data) == 0)
		{
			ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
			ImGui::PushStyleColor(ImGuiCol_Text, { textColor.x, textColor.y, textColor.z, 0.8f });
			ImGui::Text(FLUX_ICON_MAGNIFYING_GLASS);
			ImGui::PopStyleColor();
		}
		else
		{
			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImVec2 xMarkSize = ImGui::CalcTextSize(FLUX_ICON_XMARK);
			xMarkSize.x += 0.5f;
			if (ImGui::InvisibleButton(fmt::format("##{0}_SearchBar_Clear", id).c_str(), xMarkSize))
			{
				buffer.FillWithZeros();
				modified |= true;
			}
			ImGui::SetCursorPos(cursorPos);

			ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
			ImGui::PushStyleColor(ImGuiCol_Text, { textColor.x, textColor.y, textColor.z, ImGui::IsItemHovered() ? 1.0f : 0.8f });
			ImGui::Text(FLUX_ICON_XMARK);
			ImGui::PopStyleColor();
		}
		ImGui::PopFont();

		ImGui::SameLine();

		ImGui::SetCursorPos({ 32.0f, ImGui::GetCursorPos().y - 3.0f });
		ImVec2 inputFieldPos = ImGui::GetCursorPos();
		ImVec2 inputFieldSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 2.0f };
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		modified |= ImGui::InputTextEx(fmt::format("##{0}_SearchBar_Input", id).c_str(), nullptr, (char*)buffer.Data, buffer.Size, inputFieldSize, 0);
		ImGui::PopStyleVar();

		if (strlen((char*)buffer.Data) == 0)
		{
			ImGui::SetCursorPosX(inputFieldPos.x + 4.0f);
			ImGui::SetCursorPosY(inputFieldPos.y + 2.5f);

			ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
			ImGui::PushStyleColor(ImGuiCol_Text, { textColor.x, textColor.y, textColor.z, 0.3f });
			ImGui::Text(description.data());
			ImGui::PopStyleColor();
		}

		ImGui::EndChild();

		return modified;
	}

	bool UI::BreadcrumbBar(std::string_view id, const std::filesystem::path& rootDirectory, const std::filesystem::path& currentDirectory, std::filesystem::path* newDirectory /*= nullptr*/)
	{
		bool modified = false;

		std::vector<std::filesystem::path> directories;
		std::filesystem::path directory = currentDirectory;
		while (directory != rootDirectory.parent_path())
		{
			directories.push_back(directory);
			directory = directory.parent_path();
		}

		ImGui::PushFont(UI::GetFont(FontType::FontAwesome, FontWeight::Solid));
		ImGui::TextUnformatted(FLUX_ICON_FOLDER_CLOSED);
		ImGui::PopFont();

		for (auto it = directories.rbegin(); it != directories.rend(); it++)
		{
			ImGui::PushFont(UI::GetFont(FontType::FontAwesome, FontWeight::Solid));
			ImGui::SameLine();
			ImGui::TextUnformatted(FLUX_ICON_ANGLE_RIGHT);
			ImGui::PopFont();

			std::string directoryName = it->filename().stem().string();
			std::string idString = fmt::format("##{0}_{1}", id, directoryName.c_str());

			if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				ImGui::OpenPopup(idString.c_str());

			if (ImGui::BeginPopup(idString.c_str()))
			{
				ImGui::TextUnformatted(directoryName.c_str());
				ImGui::EndPopup();
			}

			ImGui::SameLine();

			if (*it == currentDirectory)
				ImGui::PushFont(UI::GetFont(FontType::Default, FontWeight::Bold, (uint32)ImGui::GetFontSize()));
			ImGui::TextUnformatted(directoryName.c_str());
			if (*it == currentDirectory)
				ImGui::PopFont();

			if (ImGui::IsItemClicked())
			{
				if (newDirectory)
					*newDirectory = *it;
				modified = true;
			}
		}

		return modified;
	}

	void UI::Image(Ref<Texture> texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1)
	{
		Engine::Get().GetImGuiRenderer()->Image(texture, size, uv0, uv1);
	}

	ImFont* UI::GetFont(FontType type, FontWeight weight, uint32 size)
	{
		return Engine::Get().GetImGuiRenderer()->GetFont(type, weight, size);
	}

}