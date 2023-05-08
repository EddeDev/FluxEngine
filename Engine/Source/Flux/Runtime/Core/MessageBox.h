#pragma once

#include "BaseTypes.h"

namespace Flux {

	enum class MessageBoxButtons : uint8
	{
		None = 0,

		AbortRetryIgnore,
		Ok,
		OkCancel,
		RetryCancel,
		YesNo,
		YesNoCancel
	};

	enum class MessageBoxIcon : uint8
	{
		None = 0,

		Asterisk,
		Error,
		Exclamation,
		Hand,
		Information,
		Question,
		Stop,
		Warning
	};

	enum class MessageBoxResult : uint8
	{
		Abort = 0,
		Cancel,
		Ignore,
		No,
		None,
		Ok,
		Retry,
		Yes
	};

}