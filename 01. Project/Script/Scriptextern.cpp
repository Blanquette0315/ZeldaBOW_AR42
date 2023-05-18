#include "pch.h"

extern const wchar_t* LINK_ANIM_WCHAR[(UINT)LINK_ANIM_TYPE::LAT_END] = 
{
    L"Wait",
    L"Walk",
    L"Walk_ED_L",
    L"Walk_ED_R",
    L"Run",
    L"Run_Brake_L",
    L"Run_Brake_R",
    L"Run_ED_L",
    L"Run_ED_R",
    L"Dash",
    L"Dash_Brake_L",
    L"Dash_Brake_R",
    L"Jump_L",
    L"Jump_R",
    L"Land_L",
    L"Land_R",
    L"Sword_Equip_On",
    L"Sword_Equip_Off",
    L"Sword_Lockon_Wait",
    L"Sword_Attack_S1",
    L"Sword_Attack_S2",
    L"Sword_Attack_S3",
    L"Sword_Attack_SF",
    L"Sword_Move_Run",
    L"Lockon_Walk_F",
    L"Lockon_Walk_B",
    L"Lockon_Walk_L",
    L"Lockon_Walk_R",
    L"Lockon_Run_F",
    L"Lockon_Run_B",
    L"Lockon_Run_L",
    L"Lockon_Run_R",
	L"Bow_Equip_On",
	L"Bow_Equip_Off",
	L"Bow_Equip_On_Run",
	L"Bow_Equip_Off_Run",
	L"Bow_Lockon_Wait",
	L"Bow_Attack_Load",
	L"Bow_Attack_Charge",
	L"Bow_Attack_Shoot",
	L"Damage_S_F",
	L"Damage_S_B",
	L"Damage_S_L",
	L"Damage_S_R",
	L"Damage_M_F",
	L"Damage_M_B",
	L"Damage_M_L",
	L"Damage_M_R",
	L"Equip_Item_L_On",
	L"Equip_Item_L_Off",
	L"Sword_Guard_Wait",
	L"Sword_Guard_Hit",
	L"Sword_Guard_Just",
	L"Lockon_Step_F_St",
	L"Lockon_Step_F_Ed",
	L"Lockon_Step_B_St",
	L"Lockon_Step_B_Ed",
	L"Lockon_Step_L_St",
	L"Lockon_Step_L_Ed",
	L"Lockon_Step_R_St",
	L"Lockon_Step_R_Ed",
	L"Lockon_Jump_B_St",
	L"Lockon_Jump_B_Ed",
	L"Sword_Attack_Just",
	L"Sword_Attack_Just_Rush1",
	L"Sword_Attack_Just_Rush2",
	L"Sword_Attack_Just_Rush3",
	L"Sword_Attack_Just_Rush4",
	L"Sword_Attack_Just_Rush5",
	L"Warp",
	L"Die",
	L"Die_Run",
	L"Bow_Move_Run_Upper",
	L"Create_Bomb",
	L"Bomb_Throw_Sphere",
	L"Bomb_Throw_Cube",
};

extern const wchar_t* LINK_STRING_WCHAR[(UINT)LINK_STRING::LINK_STRING_END] =
{
    L"Link_GroundChecker",
    L"Link",
	L"Link_LockOnRadar",
	L"Link_Sword",
	L"Link_Bow",
	L"Link_Shield",
	L"Link_AnyState",
};

extern const wchar_t* LINK_BONE_WCHAR[(UINT)LINK_BONE_STRING::END] =
{
	L"Root",
	L"Skl_Root",
	L"Spine_1",
	L"Spine_2",
	L"Clavicle_L",
	L"Arm_1_L",
	L"Arm_1_Assist_L",
	L"Arm_2_L",
	L"Elbow_L",
	L"Wrist_Assist_L",
	L"Wrist_L",
	L"Finger_A_1_L",
	L"Finger_A_2_L",
	L"Finger_A_3_L",
	L"Finger_B_1_L",
	L"Finger_B_2_L",
	L"Finger_B_3_L",
	L"Finger_C_1_L",
	L"Finger_C_2_L",
	L"Finger_C_3_L",
	L"Finger_D_1_L",
	L"Finger_D_2_L",
	L"Finger_D_3_L",
	L"Finger_E_1_L",
	L"Finger_E_2_L",
	L"Finger_E_3_L",
	L"Weapon_L",
	L"Clavicle_Assist_L"
	L"Clavicle_R",
	L"Arm_1_R",
	L"Arm_1_Assist_R",
	L"Arm_2_R",
	L"Elbow_R",
	L"Wrist_Assist_R",
	L"Wrist_R",
	L"Finger_A_1_R",
	L"Finger_A_2_R",
	L"Finger_A_3_R",
	L"Finger_B_1_R",
	L"Finger_B_2_R",
	L"Finger_B_3_R",
	L"Finger_C_1_R",
	L"Finger_C_2_R",
	L"Finger_C_3_R",
	L"Finger_D_1_R",
	L"Finger_D_2_R",
	L"Finger_D_3_R",
	L"Finger_E_1_R",
	L"Finger_E_2_R",
	L"Finger_E_3_R",
	L"Weapon_R",
	L"Clavicle_Assist_R",
	L"Neck",
	L"Head",
	L"Ear_L",
	L"Ear_R",
	L"Face_Root",
	L"Cheek_D_L",
	L"Cheek_D_R",
	L"Cheek_U_L",
	L"Cheek_U_R",
	L"Chin",
	L"Chin_Point",
	L"Lip_D",
	L"Lip_D_L_1",
	L"Lip_D_R_1",
	L"Teeth_D",
	L"Eye_In_L",
	L"Eye_In_R",
	L"Eye_Out_L",
	L"Eye_Out_R",
	L"Eyeball_L",
	L"Eyeball_R",
	L"Eyebrow_In_L",
	L"Eyebrow_In_R",
	L"Eyebrow_Mid_L",
	L"Eyebrow_Mid_R",
	L"Eyebrow_Out_L",
	L"Eyebrow_Out_R",
	L"Eyelid_D_L",
	L"Eyelid_D_R",
	L"Eyelid_U_L",
	L"Eyelid_U_R",
	L"Lip_U",
	L"Lip_U_L_1",
	L"Lip_U_L_2",
	L"Lip_U_R_1",
	L"Lip_U_R_2",
	L"Nose",
	L"Teeth_U",
	L"Hair_Root",
	L"Ponytail_A_1",
	L"Pod_A",
	L"Waist",
	L"Belt_A_1",
	L"Belt_A_2",
	L"Belt_A_3",
	L"Belt_C_1",
	L"Belt_C_2",
	L"Belt_C_3",
	L"Belt_C_4",
	L"Belt_C_5",
	L"Belt_C_6",
	L"Belt_C_7",
	L"Pod_B",
	L"Belt_C_8",
	L"Leg_1_L",
	L"Knee_L",
	L"Leg_2_L",
	L"Ankle_Assist_L",
	L"Ankle_L",
	L"Toe_L",
	L"Leg_1_R",
	L"Knee_R",
	L"Leg_2_R",
	L"Ankle_Assist_R",
	L"Ankle_R",
	L"Toe_R",
	L"Poach_A",
	L"Pod_C",
	L"Weapon_Root",
};