#include "notHacks.h"
#include "gui.h"
#include "globals.h"
#include "vector.h"

#include <thread>

void notHacks::VisualsThread(const Memory& mem) noexcept
{
	while (gui::isRunning)
	{
	    std::this_thread::sleep_for(std::chrono::milliseconds(1));

		const auto localPlayer = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);

		if (!localPlayer)
			continue;

		const auto glowManager = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwGlowObjectManager);

		if (!glowManager)
			continue;

		const auto localTeam = mem.Read<std::int32_t>(localPlayer + offsets::m_iTeamNum);

		for (auto i = 1; i <= 32; ++i)
		{
			const auto player = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwEntityList + i * 0x10);

			if (!player)
				continue;

			const auto team = mem.Read<std::int32_t>(player + offsets::m_iTeamNum);

			if (team == localTeam)
				continue;

			const auto lifeState = mem.Read<std::int32_t>(player + offsets::m_lifeState);

			if (lifeState != 0)
				continue;

			if (globals::glow)
			{
				const auto glowIndex = mem.Read<std::int32_t>(player + offsets::m_iGlowIndex);

				mem.Write(glowManager + (glowIndex * 0x38) + 0x8, globals::glowColor[0]); //red
				mem.Write(glowManager + (glowIndex * 0x38) + 0xC, globals::glowColor[1]); //green
				mem.Write(glowManager + (glowIndex * 0x38) + 0x10, globals::glowColor[2]); //blue
				mem.Write(glowManager + (glowIndex * 0x38) + 0x14, globals::glowColor[3]);// alpha

				mem.Write(glowManager + (glowIndex * 0x38) + 0x28, true);
				mem.Write(glowManager + (glowIndex * 0x38) + 0x29, false);
			}

			if (globals::radar)
				mem.Write(player + offsets::m_bSpotted, true);

		}
		
	}

}

void notHacks::RCSThread(const Memory& mem) noexcept
{
	while (gui::isRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (globals::RCS)
		{
			auto oldPunch = Vector2{ };

			const auto& localPlayer = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);
			const auto& shotsFired = mem.Read<std::int32_t>(localPlayer + offsets::m_iShotsFired);


			if (shotsFired)
			{
				const auto& clientState = mem.Read<std::uintptr_t>(globals::engineAdress + offsets::dwClientState);
				const auto& viewAngles = mem.Read<Vector2>(clientState + offsets::dwClientState_ViewAngles);

				const auto& aimPunch = mem.Read<Vector2>(localPlayer + offsets::m_aimPunchAngle);

				auto newAngles = Vector2
				{
					viewAngles.x + oldPunch.x - aimPunch.x * 2.f,
					viewAngles.y + oldPunch.y - aimPunch.y * 2.f
				};

				if (newAngles.x > 89.f)
					newAngles.x = 89.f;

				if (newAngles.x < -89.f)
					newAngles.x = -89.f;

				while (newAngles.y > 180.f)
					newAngles.y -= 360.f;

				while (newAngles.y < -180.f)
					newAngles.y += 360.f;

				mem.Write<Vector2>(clientState + offsets::dwClientState_ViewAngles, newAngles);

				oldPunch.x = aimPunch.x * 2.f;
				oldPunch.y = aimPunch.y * 2.f;

				
			}
			else
			{
				oldPunch.x = oldPunch.y = 0.f;
			}
		}
	}

}

void notHacks::TrigerThread(const Memory& mem) noexcept
{
	while (gui::isRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (globals::Triger)
		{

			if (!GetAsyncKeyState(VK_LSHIFT)) //triger key
				continue;

			const auto& localPlayer = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);
			const auto& localHealth = mem.Read<std::int32_t>(localPlayer + offsets::m_lifeState);

			if (localHealth)
				continue;

			const auto& crosshairId = mem.Read<std::int32_t>(localPlayer + offsets::m_iCrosshairId);

			if (!crosshairId || crosshairId > 64)
				continue;

			const auto& player = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwEntityList + (crosshairId - 1) * 0x10);

			if (mem.Read<std::int32_t>(player + offsets::m_lifeState))
				continue;

			if (mem.Read<std::int32_t>(player + offsets::m_iTeamNum) ==
				mem.Read<std::int32_t>(localPlayer + offsets::m_iTeamNum))
				continue;

			mem.Write<std::uintptr_t>(globals::clientAdress + offsets::dwForceAttack, 6);
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			mem.Write<std::uintptr_t>(globals::clientAdress + offsets::dwForceAttack, 4);

		}

	}

}




void notHacks::AimThread(const Memory& mem) noexcept
{
	while (gui::isRunning)
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (globals::aim)
		{
			// aimbot key
			if (!GetAsyncKeyState(VK_LSHIFT))
				continue;

			// get local player
			const auto localPlayer = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);
			const auto localTeam = mem.Read<std::int32_t>(localPlayer + offsets::m_iTeamNum);

			// eye position = origin + viewOffset
			const auto localEyePosition = mem.Read<Vector3>(localPlayer + offsets::m_vecOrigin) +
				mem.Read<Vector3>(localPlayer + offsets::m_vecViewOffset);

			const auto clientState = mem.Read<std::uintptr_t>(globals::engineAdress + offsets::dwClientState);

			const auto localPlayerId =
				mem.Read<std::int32_t>(clientState + offsets::dwClientState_GetLocalPlayer);

			const auto viewAngles = mem.Read<Vector3>(clientState + offsets::dwClientState_ViewAngles);
			const auto aimPunch = mem.Read<Vector3>(localPlayer + offsets::m_aimPunchAngle) * 2;

			// aimbot fov
			auto bestFov = globals::aimFOV[1];
			auto bestAngle = Vector3{ };

			for (auto i = 1; i <= 32; ++i)
			{
				const auto player = mem.Read<std::uintptr_t>(globals::clientAdress + offsets::dwEntityList + i * 0x10);

				if (mem.Read<std::int32_t>(player + offsets::m_iTeamNum) == localTeam)
					continue;

				if (mem.Read<bool>(player + offsets::m_bDormant))
					continue;

				if (mem.Read<std::int32_t>(player + offsets::m_lifeState))
					continue;

				if (mem.Read<std::int32_t>(player + offsets::m_bSpottedByMask) & (1 << localPlayerId))
				{
					const auto boneMatrix = mem.Read<std::uintptr_t>(player + offsets::m_dwBoneMatrix);

					// pos of player head in 3d space
					// 8 is the head bone index :)
					const auto playerHeadPosition = Vector3{
						mem.Read<float>(boneMatrix + 0x30 * 8 + 0x0C),
						mem.Read<float>(boneMatrix + 0x30 * 8 + 0x1C),
						mem.Read<float>(boneMatrix + 0x30 * 8 + 0x2C)
					};

					const auto angle = CalculateAngle(
						localEyePosition,
						playerHeadPosition,
						viewAngles + aimPunch
					);

					const auto fov = std::hypot(angle.x, angle.y);

					if (fov < bestFov)
					{
						bestFov = fov;
						bestAngle = angle;
					}
				}
			}

			// if we have a best angle, do aimbot
			if (!bestAngle.IsZero())
				mem.Write<Vector3>(clientState + offsets::dwClientState_ViewAngles, viewAngles + bestAngle / globals::aimSMOOTHING[1]); // smoothing
		}

	}

}