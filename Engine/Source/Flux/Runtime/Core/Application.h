#pragma once

namespace Flux {

	class Application
	{
	public:
		Application(std::string_view debugName);
		virtual ~Application() = default;

		virtual void OnInit() {}
		virtual void OnExit() {}
		virtual void OnUpdate() {}

		const std::string& GetDebugName() const { return m_DebugName; }
	private:
		std::string m_DebugName;
	};

}