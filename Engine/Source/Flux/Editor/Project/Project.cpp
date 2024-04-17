#include "FluxPCH.h"
#include "Project.h"

#include <yaml-cpp/yaml.h>

namespace YAML {

	template<>
	struct convert<Flux::Vector2>
	{
		static Node encode(const Flux::Vector2& rhs)
		{
			Node node;
			node.push_back(rhs.X);
			node.push_back(rhs.Y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Flux::Vector2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.X = node[0].as<float>();
			rhs.Y = node[1].as<float>();
			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Flux::Vector2& v)
	{
		out << Flow;
		out << BeginSeq << v.X << v.Y << EndSeq;
		return out;
	}

	template<>
	struct convert<Flux::Vector3>
	{
		static Node encode(const Flux::Vector3& rhs)
		{
			Node node;
			node.push_back(rhs.X);
			node.push_back(rhs.Y);
			node.push_back(rhs.Z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Flux::Vector3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.X = node[0].as<float>();
			rhs.Y = node[1].as<float>();
			rhs.Z = node[2].as<float>();
			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Flux::Vector3& v)
	{
		out << Flow;
		out << BeginSeq << v.X << v.Y << v.Z << EndSeq;
		return out;
	}

	template<>
	struct convert<Flux::Vector4>
	{
		static Node encode(const Flux::Vector4& rhs)
		{
			Node node;
			node.push_back(rhs.X);
			node.push_back(rhs.Y);
			node.push_back(rhs.Z);
			node.push_back(rhs.W);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Flux::Vector4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.X = node[0].as<float>();
			rhs.Y = node[1].as<float>();
			rhs.Z = node[2].as<float>();
			rhs.W = node[3].as<float>();
			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Flux::Vector4& v)
	{
		out << Flow;
		out << BeginSeq << v.X << v.Y << v.Z << v.W << EndSeq;
		return out;
	}

	template<>
	struct convert<Flux::Quaternion>
	{
		static Node encode(const Flux::Quaternion& rhs)
		{
			Node node;
			node.push_back(rhs.X);
			node.push_back(rhs.Y);
			node.push_back(rhs.Z);
			node.push_back(rhs.W);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Flux::Quaternion& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.X = node[0].as<float>();
			rhs.Y = node[1].as<float>();
			rhs.Z = node[2].as<float>();
			rhs.W = node[3].as<float>();
			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Flux::Quaternion& v)
	{
		out << Flow;
		out << BeginSeq << v.X << v.Y << v.Z << v.W << EndSeq;
		return out;
	}

	template<>
	struct convert<Flux::IntRect>
	{
		static Node encode(const Flux::IntRect& rhs)
		{
			Node node;
			node.push_back(rhs.MinX);
			node.push_back(rhs.MinY);
			node.push_back(rhs.MaxX);
			node.push_back(rhs.MaxY);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, Flux::IntRect& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.MinX = node[0].as<int32>();
			rhs.MinY = node[1].as<int32>();
			rhs.MaxX = node[2].as<int32>();
			rhs.MaxY = node[3].as<int32>();
			return true;
		}
	};

	Emitter& operator<<(Emitter& out, const Flux::IntRect& v)
	{
		out << Flow;
		out << BeginSeq << v.MinX << v.MinY << v.MaxX << v.MaxY << EndSeq;
		return out;
	}

}

namespace Flux {

	Project::Project(const std::filesystem::path& path)
	{
		FLUX_VERIFY(!s_ActiveProject);
		FLUX_VERIFY(std::filesystem::is_directory(path));

		s_ActiveProject = this;

		m_Name = path.filename().stem().string();

		m_ProjectDirectory = path;
		m_AssetsDirectory = path / "Assets";

		CreateDirectory(m_ProjectDirectory);
		CreateDirectory(m_AssetsDirectory);

		LoadFromFile();
	}

	Project::~Project()
	{
		s_ActiveProject = nullptr;
	}

	void Project::LoadFromFile()
	{
		std::string projectName = "FluxProject";
		std::string projectFileName = projectName + ".fluxproj";
		std::filesystem::path projectFile = m_ProjectDirectory / projectFileName;

		YAML::Node node = YAML::LoadFile(projectFile.string());

		if (!node["Project"])
		{
			FLUX_VERIFY(false, "Invalid project file!");
			return;
		}

		auto settingsNode = node["Settings"];
		m_Settings.EditorCameraPosition = settingsNode["EditorCameraPosition"].as<Vector3>();
		m_Settings.EditorCameraRotation = settingsNode["EditorCameraRotation"].as<Vector3>();
	}

	void Project::SaveToFile()
	{
		// TODO: Temp
		std::string projectName = "FluxProject";
		std::string projectFileName = projectName + ".fluxproj";
		std::filesystem::path projectFile = m_ProjectDirectory / projectFileName;

		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value << projectName;

		out << YAML::Key << "Settings" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "EditorCameraPosition" << YAML::Value << m_Settings.EditorCameraPosition;
		out << YAML::Key << "EditorCameraRotation" << YAML::Value << m_Settings.EditorCameraRotation;
		out << YAML::EndMap;

		out << YAML::EndMap;

		std::ofstream stream(projectFile);
		stream << out.c_str();
	}

	Ref<Project> Project::LoadFromFile(const std::filesystem::path& path)
	{
		return Ref<Project>::Create(path);
	}

	void Project::CreateDirectory(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			FLUX_TRACE("Creating directory: {0}", path.string());
			std::filesystem::create_directories(path);
		}
	}
}