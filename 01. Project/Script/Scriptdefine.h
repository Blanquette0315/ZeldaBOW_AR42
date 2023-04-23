#pragma once

enum class CAMERA_SELECTION
{
    LINK,
    END,
};

enum class LINK_FRONT_TOE
{
    LEFT,
    RIGHT,
    END,
};

enum class LINK_DIRECTION
{
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    NONE,
    END,
};

enum LINK_ANIM_TYPE
{
    LAT_WAIT,
    LAT_WALK,
    LAT_WALK_ED_L,
    LAT_WALK_ED_R,
    LAT_RUN,
    LAT_RUN_BRAKE_L,
    LAT_RUN_BRAKE_R,
    LAT_RUN_ED_L,
    LAT_RUN_ED_R,
    LAT_DASH,
    LAT_DASH_BRAKE_L,
    LAT_DASH_BRAKE_R,
    LAT_JUMP_L,
    LAT_JUMP_R,
    LAT_LAND_L,
    LAT_LAND_R,
    LAT_END
};

extern const wchar_t* LINK_ANIM_WCHAR[(UINT)LINK_ANIM_TYPE::LAT_END];

enum LINK_ANIM_CONDITION
{
    LAC_KEY_WSAD        = 0x00000001,
    LAC_KEY_SPACE       = 0x00000002,
    LAC_KEY_SHIFT       = 0x00000004,

    LAC_MODE_WALK       = 0x00000008,
    LAC_MODE_RUN        = 0x00000010,

    LAC_ANIM_FINISHED   = 0x00000020,

    LAC_TOE_L_FRONT     = 0x00000040,

    LAC_TURN_BACK       = 0x00000080,

    LAC_GROUNDED        = 0x00000100,

    //GROUND_TO_AERIAL = 0x00000020,
    //AERIAL_TO_GROUND = 0x00000040,
    //SPEED_Y_POSITIVE = 0x00000080,
    //SPEED_Y_TURN = 0x00000100, // Bit 안들어오게 해놓음
    //COMBO_PROGRESS = 0x00000200,
    //MOUSE_LEFT = 0x00000400,
    //SPEED_Y_NEGATIVE = 0x00000800,
    //KEY_SHIFT = 0x00001000,
    //CAN_DASH = 0x00002000,
    //HP_DOWN = 0x00004000,
    //HP_ZERO = 0x00008000,
    //CAN_JUMP_ATTACK = 0x00010000,
    //KEY_W = 0x00020000,
    //CAN_AIR_ATTACK = 0x00040000,
    //KEY_S = 0x00080000,
};

enum LINK_ANIM_PREFERENCE
{
    LAP_REPEAT          = 0x00000001,
};

enum class LINK_MODE
{
    WALK,
    RUN,
    END
};

enum class LINK_STRING
{
    LINK_STRING_GROUND_CHECKER,
    END
};

extern const wchar_t* LINK_STRING_WCHAR[(UINT)LINK_STRING::END];