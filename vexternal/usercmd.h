#pragma once

struct s_usercmd
{
    uintptr_t pVft;                // 0x00
    int32_t   m_iCmdNumber;        // 0x04
    int32_t   m_iTickCount;        // 0x08
    c_vec3 m_vecViewAngles;        // 0x0C
    c_vec3 m_vecAimDirection;      // 0x18
    float     m_flForwardmove;     // 0x24
    float     m_flSidemove;        // 0x28
    float     m_flUpmove;          // 0x2C
    int32_t   m_iButtons;          // 0x30
    uint8_t   m_bImpulse;          // 0x34
    uint8_t   Pad1[3];
    int32_t   m_iWeaponSelect;     // 0x38
    int32_t   m_iWeaponSubtype;    // 0x3C
    int32_t   m_iRandomSeed;       // 0x40
    int16_t   m_siMouseDx;         // 0x44
    int16_t   m_siMouseDy;         // 0x46
    bool      m_bHasBeenPredicted; // 0x48
    uint8_t   Pad2[27];
}; s_usercmd* g_pusercmd = nullptr;

enum cmd_buttons {
	in_attack = (1 << 0),
	in_jump = (1 << 1),
	in_duck = (1 << 2),
	in_forward = (1 << 3),
	in_back = (1 << 4),
	in_use = (1 << 5),
	in_cancel = (1 << 6),
	in_left = (1 << 7),
	in_right = (1 << 8),
	in_moveleft = (1 << 9),
	in_moveright = (1 << 10),
	in_attack2 = (1 << 11),
	in_run = (1 << 12),
	in_reload = (1 << 13),
	in_alt1 = (1 << 14),
	in_alt2 = (1 << 15),
	in_score = (1 << 16),
	in_speed = (1 << 17),
	in_walk = (1 << 18),
	in_zoom = (1 << 19),
	in_weapon1 = (1 << 20),
	in_weapon2 = (1 << 21),
	in_bullrush = (1 << 22),
	in_grenade1 = (1 << 23),
	in_grenade2 = (1 << 24),
	in_attack3 = (1 << 25)
};