#pragma once

#define LinkBodyDivPoint (UINT)LINK_BONE_STRING::Ponytail_A_1
#define LerpCos(ratio)  (1.f - cosf(ratio * XM_PI)) / 2.f
#define LerpCircle(ratio) (1.f - sqrtf(1.f - ratio * ratio))


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

enum class LINK_DAMAGED_TYPE
{
	NONE,
	SMALL,
	MEDIUM,
	LARGE,
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

    LAT_SWORD_EQUIP_ON,
    LAT_SWORD_EQUIP_OFF,
    LAT_SWORD_LOCKON_WAIT,
    LAT_SWORD_ATTACK_S1,
    LAT_SWORD_ATTACK_S2,
    LAT_SWORD_ATTACK_S3,
    LAT_SWORD_ATTACK_SF,
    LAT_SWORD_MOVE_RUN,

    LAT_LOCKON_WALK_F,
    LAT_LOCKON_WALK_B,
    LAT_LOCKON_WALK_L,
    LAT_LOCKON_WALK_R,
    LAT_LOCKON_RUN_F,
    LAT_LOCKON_RUN_B,
    LAT_LOCKON_RUN_L,
    LAT_LOCKON_RUN_R,

	LAT_BOW_EQUIP_ON,
	LAT_BOW_EQUIP_OFF,
	LAT_BOW_EQUIP_ON_RUN,
	LAT_BOW_EQUIP_OFF_RUN,
	LAT_BOW_LOCKON_WAIT,
	LAT_BOW_ATTACK_LOAD,
	LAT_BOW_ATTACK_CHARGE,
	LAT_BOW_ATTACK_SHOOT,

	LAT_DAMAGE_S_F,
	LAT_DAMAGE_S_B,
	LAT_DAMAGE_S_L,
	LAT_DAMAGE_S_R,
	LAT_DAMAGE_M_F,
	LAT_DAMAGE_M_B,
	LAT_DAMAGE_M_L,
	LAT_DAMAGE_M_R,

	LAT_EQUIP_ITEM_L_ON,
	LAT_EQUIP_ITEM_L_OFF,

	LAT_SWORD_GUARD_WAIT,
	LAT_SWORD_GUARD_HIT,
	LAT_SWORD_GUARD_JUST,

	LAT_LOCKON_STEP_F_ST,
	LAT_LOCKON_STEP_F_ED,
	LAT_LOCKON_STEP_B_ST,
	LAT_LOCKON_STEP_B_ED,
	LAT_LOCKON_STEP_L_ST,
	LAT_LOCKON_STEP_L_ED,
	LAT_LOCKON_STEP_R_ST,
	LAT_LOCKON_STEP_R_ED,

	LAT_LOCKON_JUMP_B_ST,
	LAT_LOCKON_JUMP_B_ED,

	LAT_SWORD_ATTACK_JUST,
	LAT_SWORD_ATTACK_JUST_RUSH1,
	LAT_SWORD_ATTACK_JUST_RUSH2,
	LAT_SWORD_ATTACK_JUST_RUSH3,
	LAT_SWORD_ATTACK_JUST_RUSH4,
	LAT_SWORD_ATTACK_JUST_RUSH5,

	LAT_WARP,
	LAT_DIE,
	LAT_DIE_RUN,

	LAT_BOW_MOVE_RUN_UPPER,

	LAT_CREATE_BOMB,
	LAT_BOMB_THROW_SPHERE,
	LAT_BOMB_THROW_CUBE,

    LAT_END,
};

extern const wchar_t* LINK_ANIM_WCHAR[(UINT)LINK_ANIM_TYPE::LAT_END];

// if 32bit is not enought change to 64bit long int
enum LINK_ANIM_CONDITION
{
    LAC_KEY_WSAD				= 0x00000001,
    LAC_KEY_SPACE				= 0x00000002,
    LAC_KEY_SHIFT				= 0x00000004,
    LAC_KEY_N1					= 0x00000008,
	LAC_KEY_N2					= 0x00000010,
	LAC_KEY_N3					= 0x00000020,
	LAC_KEY_F					= 0x00000040,
    LAC_KEY_LBTN				= 0x00000080,
    LAC_KEY_LBTN_COMBO			= 0x00000100,
    LAC_KEY_RBTN				= 0x00000200,
    LAC_MODE_WALK				= 0x00000400,
    LAC_MODE_RUN				= 0x00000800,
	LAC_MODE_LOCKON				= 0x00001000,
    LAC_ANIM_FINISHED			= 0x00002000,
    LAC_TOE_L_FRONT				= 0x00004000,
    LAC_TURN_BACK				= 0x00008000,
    LAC_GROUNDED				= 0x00010000,
	LAC_DAMAGED_FRONT			= 0x00020000,
	LAC_DAMAGED_BACK			= 0x00040000,
	LAC_DAMAGED_LEFT			= 0x00080000,
	LAC_DAMAGED_RIGHT			= 0x00100000,
	LAC_DAMAGED_SMALL			= 0x00200000,
	LAC_DAMAGED_MEDIUM			= 0x00400000,
	LAC_DAMAGED_LARGE			= 0x00800000,
	LAC_DEAD					= 0x01000000,
	LAC_EQUIP_SWORD				= 0x02000000,
	LAC_EQUIP_BOW				= 0x04000000,
	LAC_EQUIP_SHIELD			= 0x08000000,
	LAC_SHIELD_GUARD			= 0x10000000,
	LAC_CAN_JUST_ATTACK_START	= 0x20000000,
	LAC_BOMB_SPHERE				= 0x40000000,
	LAC_BOMB_CUBE				= 0x80000000,
};

enum LINK_ANIM_PREFERENCE
{
    LAP_REPEAT          = 0x00000001,
    LAP_BLEND           = 0x00000002,
    LAP_COMBO           = 0x00000004,
	LAP_ATTACK			= 0x00000008,
	LAP_EQUIP_SWORD		= 0x00000010,  // not use
	LAP_EQUIP_BOW		= 0x00000020,  // not use
	LAP_EQUIP_SHIELD	= 0x00000040,  // not use
	LAP_KEEP_LOCKON		= 0x00000080,
	LAP_BOW_CHARGE		= 0x00000100,
	LAP_INVINCIBLE		= 0x00000200,
	LAP_JUST_ATK_FINISH = 0x00000400,
	LAP_JUST_ATK		= 0x00000800,
	LAP_AIR				= 0x00001000,
};

enum LINK_MODE
{
    LINK_MODE_WALK      = 0x00000001,
    LINK_MODE_RUN       = 0x00000002,
	LINK_MODE_DASH		= 0x00000004,
	LINK_MODE_LOCKON	= 0x00000008,
};

enum LINK_STRING
{
    LINK_STRING_GROUND_CHECKER,
    LINK_STRING_LINK,
	LINK_STRING_LOCKON_RADAR,
	LINK_STRING_SWORD,
	LINK_STRING_BOW,
	LINK_STRING_SHIELD,
	LINK_STRING_ANYSTATE_NODE_KEY,
	LINK_STRING_UI_HP,
	LINK_STRING_UI_CROSSHAIR,
	LINK_STRING_EFFECT_PARRYING,
	LINK_STRING_EFFECT_SCREEN_FLASH,
    LINK_STRING_END,
};

extern const wchar_t* LINK_STRING_WCHAR[(UINT)LINK_STRING::LINK_STRING_END];

enum class LINK_BONE_STRING
{
	Root,
	Skl_Root,
	Spine_1,
	Spine_2,
	Clavicle_L,
	Arm_1_L,
	Arm_1_Assist_L,
	Arm_2_L,
	Elbow_L,
	Wrist_Assist_L,
	Wrist_L,
	Finger_A_1_L,
	Finger_A_2_L,
	Finger_A_3_L,
	Finger_B_1_L,
	Finger_B_2_L,
	Finger_B_3_L,
	Finger_C_1_L,
	Finger_C_2_L,
	Finger_C_3_L,
	Finger_D_1_L,
	Finger_D_2_L,
	Finger_D_3_L,
	Finger_E_1_L,
	Finger_E_2_L,
	Finger_E_3_L,
	Weapon_L,
	Clavicle_Assist_L, // shai gol
	Clavicle_R,
	Arm_1_R,
	Arm_1_Assist_R,
	Arm_2_R,
	Elbow_R,
	Wrist_Assist_R,
	Wrist_R,
	Finger_A_1_R,
	Finger_A_2_R,
	Finger_A_3_R,
	Finger_B_1_R,
	Finger_B_2_R,
	Finger_B_3_R,
	Finger_C_1_R,
	Finger_C_2_R,
	Finger_C_3_R,
	Finger_D_1_R,
	Finger_D_2_R,
	Finger_D_3_R,
	Finger_E_1_R,
	Finger_E_2_R,
	Finger_E_3_R,
	Weapon_R,
	Clavicle_Assist_R,
	Neck,
	Head,
	Ear_L,
	Ear_R,
	Face_Root,
	Cheek_D_L,
	Cheek_D_R,
	Cheek_U_L,
	Cheek_U_R,
	Chin,
	Chin_Point,
	Lip_D,
	Lip_D_L_1,
	Lip_D_R_1,
	Teeth_D,
	Eye_In_L,
	Eye_In_R,
	Eye_Out_L,
	Eye_Out_R,
	Eyeball_L,
	Eyeball_R,
	Eyebrow_In_L,
	Eyebrow_In_R,
	Eyebrow_Mid_L,
	Eyebrow_Mid_R,
	Eyebrow_Out_L,
	Eyebrow_Out_R,
	Eyelid_D_L,
	Eyelid_D_R,
	Eyelid_U_L,
	Eyelid_U_R,
	Lip_U,
	Lip_U_L_1,
	Lip_U_L_2,
	Lip_U_R_1,
	Lip_U_R_2,
	Nose,
	Teeth_U,
	Hair_Root,
	Ponytail_A_1, // end of upper
	Pod_A,
	Waist,
	Belt_A_1,
	Belt_A_2,
	Belt_A_3,
	Belt_C_1,
	Belt_C_2,
	Belt_C_3,
	Belt_C_4,
	Belt_C_5,
	Belt_C_6,
	Belt_C_7,
	Pod_B,
	Belt_C_8,
	Leg_1_L,
	Knee_L,
	Leg_2_L,
	Ankle_Assist_L,
	Ankle_L,
	Toe_L,
	Leg_1_R,
	Knee_R,
	Leg_2_R,
	Ankle_Assist_R,
	Ankle_R,
	Toe_R,
	Poach_A,
	Pod_C,
	Weapon_Root,
	END
};

extern const wchar_t* LINK_BONE_WCHAR[(UINT)LINK_BONE_STRING::END];