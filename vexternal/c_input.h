#pragma once

struct s_input {
    uintptr_t m_pVftable;                   // 0x00
    bool      m_bTrackIRAvailable;          // 0x04
    bool      m_bMouseInitialized;          // 0x05
    bool      m_bMouseActive;               // 0x06
    bool      m_bJoystickAdvancedInit;      // 0x07
    uint8_t   Unk1[44];                     // 0x08
    uintptr_t m_pKeys;                      // 0x34
    uint8_t   Unk2[100];                    // 0x38
    bool      m_bCameraInterceptingMouse;   // 0x9C
    bool      m_bCameraInThirdPerson;       // 0x9D
    bool      m_bCameraMovingWithMouse;     // 0x9E
    c_vec3 m_vecCameraOffset;            // 0xA0
    bool      m_bCameraDistanceMove;        // 0xAC
    int32_t   m_nCameraOldX;                // 0xB0
    int32_t   m_nCameraOldY;                // 0xB4
    int32_t   m_nCameraX;                   // 0xB8
    int32_t   m_nCameraY;                   // 0xBC
    bool      m_bCameraIsOrthographic;      // 0xC0
    c_vec3 m_vecPreviousViewAngles;      // 0xC4
    c_vec3 m_vecPreviousViewAnglesTilt;  // 0xD0
    float     m_flLastForwardMove;          // 0xDC
    int32_t   m_nClearInputState;           // 0xE0
    uint8_t   Unk3[0x8];                    // 0xE4
    uintptr_t m_pCommands;                  // 0xEC
    uintptr_t m_pVerifiedCommands;          // 0xF0
} g_pinput;