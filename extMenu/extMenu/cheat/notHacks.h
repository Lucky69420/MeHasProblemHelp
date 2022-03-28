#pragma once
#include "memory.h"

namespace notHacks
{
	//run visual hacs
	void VisualsThread(const Memory& mem) noexcept;

	//run other hacs like RCS
	void RCSThread(const Memory& mem) noexcept;

	void TrigerThread(const Memory& mem) noexcept;

	void AimThread(const Memory& mem) noexcept;

}