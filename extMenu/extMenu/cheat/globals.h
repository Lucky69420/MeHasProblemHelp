#pragma once
#include <cstdint>
#include <cstddef>
#include "vector.h"

namespace globals
{
	inline std::uintptr_t clientAdress = 0;
	inline std::uintptr_t engineAdress = 0;

	inline bool glow = false;
	inline float glowColor[] = { 1.f, 0.f, 0.f, 1.f };

	inline bool radar = false;
	inline bool RCS = false;
	inline bool Triger = false;
	inline bool aim = false;
	inline float aimFOV[] = { 1.f };
	inline float aimSMOOTHING[] = { 1.f };
}

namespace offsets
{
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDB35DC;
	constexpr ::std::ptrdiff_t dwGlowObjectManager = 0x5317308;
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DCEEAC;
	constexpr ::std::ptrdiff_t m_iCrosshairId = 0x11838;
	constexpr ::std::ptrdiff_t dwClientState_GetLocalPlayer = 0x180;

	constexpr ::std::ptrdiff_t m_iTeamNum = 0xF4;
	constexpr ::std::ptrdiff_t m_lifeState = 0x25F;
	constexpr ::std::ptrdiff_t m_iGlowIndex = 0x10488;
	constexpr ::std::ptrdiff_t m_bSpotted = 0x93D;
	constexpr ::std::ptrdiff_t dwForceAttack = 0x31FF3C0;
	constexpr ::std::ptrdiff_t m_dwBoneMatrix = 0x26A8;
	constexpr ::std::ptrdiff_t m_bSpottedByMask = 0x980;
	constexpr ::std::ptrdiff_t m_bDormant = 0xED;

	constexpr ::std::ptrdiff_t dwClientState = 0x58CFC4;
	constexpr ::std::ptrdiff_t dwClientState_ViewAngles = 0x4D90;
	constexpr ::std::ptrdiff_t m_aimPunchAngle = 0x303C;
	constexpr ::std::ptrdiff_t m_iShotsFired = 0x103E0;
	constexpr ::std::ptrdiff_t m_vecOrigin = 0x138;
	constexpr ::std::ptrdiff_t m_vecViewOffset = 0x108;
}

struct Vector2
{
	float x = { }, y = { };
};

constexpr Vector3 CalculateAngle(
	const Vector3& localPosition,
	const Vector3& enemyPosition,
	const Vector3& viewAngles) noexcept

{
	return ((enemyPosition - localPosition).ToAngle() - viewAngles);
}