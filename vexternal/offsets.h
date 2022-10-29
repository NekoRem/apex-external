#pragma once

#define OFFSET_GLOBALVARS 0x137fe20 // global_vars
#define OFFSET_ENTITYLIST 0x1a9e758 // cl_entitylist
#define OFFSET_LOCAL_ENT 0x1e38060 // LocalPlayer
#define OFFSET_NAMELIST 0xba30ab0 // NameList
#define OFFSET_RENDER 0x1e9a540 // ViewRender
#define OFFSET_MATRIX 0x5f2f6e // ViewMatrix
#define OFFSET_INPUT 0x1db97a0 // .?AVCInput@@
#define OFFSET_DISABLE_BLOOM 0x01dc74a0 // mat_disable_bloom
#define OFFSET_THIRD_PERSON 0x01a04db0 // thirdperson_override

#define OFFSET_NAME_INDEX 0x38
#define OFFSET_SHIELD 0x170 // m_shieldHealth
#define OFFSET_MAX_SHIELD 0x174 // m_shieldHealthMax
#define OFFSET_HEALTH 0x43c // m_iHealth
#define OFFSET_TEAM 0x44c // m_iTeamNum
#define OFFSET_NAME 0x589 // m_iName
#define OFFSET_LIFE_STATE 0x798 // m_lifeState
#define OFFSET_VISIBLE_TIME 0x1700
#define OFFSET_BLEED_OUT_STATE 0x26d0 // m_bleedoutState
#define OFFSET_OBSERVING_TARGET 0x34d0 // m_hObserverTarget
#define OFFSET_ZOOM_FOV 0x1730 + 0xb8 //m_playerData + m_curZoomFOV
#define OFFSET_VECMINS 0x4d0 // m_vecMins
#define OFFSET_VECMAXS 0x4dc // m_vecMaxs
#define OFFSET_DIVING 0x4630 // m_skydiveIsDiving
#define OFFSET_FLAGS 0x98 // m_fFlags

#define OFFSET_IN_JUMP 0x07546398 // in_jump

#define OFFSET_BULLET_SPEED 0x1f28 // CWeaponX!m_flProjectileSpeed
#define OFFSET_BULLET_GRAVITY (OFFSET_BULLET_SPEED + 0x8)

#define OFFSET_AMMO_IN_CLIP 0x1624 // m_ammoInClip
#define OFFSET_WEAPON_NAME_INDEX 0x1804 // m_weaponNameIndex
#define OFFSET_CURRENT_WEAPON 0x1a6c // m_latestPrimaryWeapons
#define OFFSET_ORIGIN 0x4 // m_vecAbsOrigin
#define OFFSET_BONES 0xf50 // m_bConstrainBetweenEndpoints || m_nForceBone + 0x48
#define OFFSET_AIMPUNCH 0x2498 // m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle
#define OFFSET_CAMERAPOS 0x1f40
#define OFFSET_VIEWANGLES 0x253c - 0x14 // m_ammoPoolCapacity - 0x14
#define OFFSET_BREATH_ANGLES (OFFSET_VIEWANGLES - 0x10)
#define OFFSET_ARMOR_TYPE 0x45bc // m_armorType

#define OFFSET_SCRIPT_ID 0x1628 // m_customScriptInt
#define OFFSET_SCRIPT_NAME_ID 0x690 // m_scriptNameIndex
#define OFFSET_WEAPON_READY_TIME 0x16a8 // m_nextReadyTime

#define OFFSET_NSKIN 0xe48 // m_nSkin
#define OFFSET_SKIN_MODEL 0xe4c // m_skinMod

#define OFFSET_SIGNIFIER 0x580 // m_iSignifierName
#define OFFSET_ABS_VELOCITY 0x140 // C_Player : m_vecAbsVelocity

// DT_HighlightSettings
#define OFFSET_GLOW_DISTANCE 0x3b4
#define OFFSET_GLOW_COLOR 0x1d0 // m_highlightParams + 0x18
#define OFFSET_GLOW_MODE 0x2c4 // m_highlightFunctionBits + 0x4
#define OFFSET_GLOW_ENABLE 0x3c8
#define OFFSET_GLOW_THROUGH_WALLS 0x3d0