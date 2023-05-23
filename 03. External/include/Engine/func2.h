#pragma once

class CGameObject;
// ====================== JM ======================================

wstring ConvertStrToWstr(const string& _str);
string ConvertWstrToStr(const wstring& _wstr);
vector<wstring> ConvertStrToWstrVec(const vector<string>& _vecStr);
vector<string> ConvertWstrToStrVec(const vector<wstring>& _vecWstr);

bool PickingObj(CGameObject*& _pObjOut);
uint32_t GetLayerBitFromIdx(WORD _idx);
uint32_t GetLayerMaskBitFromIdx(WORD _idx);

DirectX::XMFLOAT3 GetEulerAngleFromDirection(const DirectX::XMVECTOR& direction);

template <typename T>
T lerp(const T& start, const T& end, float t)
{
	return start + t * (end - start);
}

template <typename T1, typename T2>
int GetMapIdx(const map<T1, T2>& _map, T2 _key);

template<typename T1, typename T2>
inline int GetMapIdx(const map<T1, T2>& _map, T1 _key)
{
	int idx = 0;
	typename map<T1, T2>::const_iterator iter = _map.cbegin();
	for (; iter != _map.cend(); ++iter)
	{
		if (iter->first == _key)
		{
			return idx;
		}
		++idx;
	}

	return 0;
}

CGameObject* FindObjectByName(const wstring& _name);

enum BIT_FUNC_OPT
{
	BIT_LEAST_ONE,
	BIT_EQUAL,
	BIT_INCLUDE,
	BIT_EQUAL_SELECTED_BIT,
};

bool CalBit(UINT _originBit, UINT _cmpBits, BIT_FUNC_OPT _opt, UINT _CompareBit = 0);

void AddBit(UINT& _src, UINT _add);
void RemoveBit(UINT& _src, UINT _remove);

// ================================================================

// ===== Phys Engine To Engine Function =====
// Quaternion to Euler Angle
void QuaternionToEuler(Vec4 _vQRot, Vec3& _vERot);

Vec3 DecomposeRotMat(const Matrix& _matRot);

template<typename T>
void Safe_Del_Vec(vector<T*>& _vec)
{
	typename vector<T*>::iterator iter = _vec.begin();
	for (; iter != _vec.end(); ++iter)
	{
		if (nullptr != (*iter))
			delete (*iter);
	}
}

template<typename T1, typename T2>
void Safe_Del_Map(map<T1, T2>& _map)
{
	typename map<T1, T2>::iterator iter = _map.begin();

	for (; iter != _map.end(); ++iter)
	{
		if (nullptr != iter->second)
			delete iter->second;
	}
	_map.clear();
}

template<typename T, int SIZE>
void Safe_Del_Array(T* (&_arr)[SIZE])
{
	for (int i = 0; i < SIZE; ++i)
	{
		if (nullptr != _arr[i])
			delete _arr[i];
	}
}

// Relative Path ��������
wstring GetRelativePath(const wstring& _strBase, const wstring& _strPath);

string WStringToString(const wstring& _str);
wstring StringToWString(const string& _str); // �μ� �߰�

int GetSizeofFormat(DXGI_FORMAT _eFormat);

// ���� ������Ʈ ��ȿ�� �˻�
class CGameObject;
bool IsValid(CGameObject*& _pObj);

// Ŀ����Ʈ�� ��ȿ�� üũ
class CEntity;
inline bool IsValid(CEntity* _pCom)
{
	if (nullptr == _pCom)
		return false;
	return true;
}

Matrix GetMatrixFromFbxMatrix(FbxAMatrix& _mat);

// ====================
// = File Save / Load =
// ====================
void SaveStringToFile(const string& _str, FILE* _pFile);
void LoadStringFromFile(string& _str, FILE* _pFile);

void SaveWStringToFile(const wstring& _str, FILE* _pFile);
void LoadWStringFromFile(wstring& _str, FILE* _pFile);

// �ڽ��� � ���ҽ��� �����ϰ� �־����� �����ϴ� �Լ�
#include "CResMgr.h"
#include "Ptr.h"
template<typename T>
void SaveResourceRef(Ptr<T> _res, YAML::Emitter& _emitter)
{
	//// �������� ���ҽ��� �����ٸ� ���´�� nullptr�� �� ���̴�.
	//int bExist = !!_res.Get();
	//_emitter << YAML::Key << "Exist";
	//_emitter << YAML::Value << bExist;
	//// �������� ���ҽ��� �־��ٸ�, � ���ҽ��� ������ �־����� Ű�� ��θ� �����Ѵ�.

	if (_res.Get())
	{
		_emitter << YAML::Key << "ResKey";
		_emitter << YAML::Value << WStringToString(_res->GetKey());
		_emitter << YAML::Key << "ResRelativePath";
		_emitter << YAML::Value << WStringToString(_res->GetRelativePath());
	}
}

template<typename T>
void SaveResourceRefEX(Ptr<T> _res, YAML::Emitter& _emitter, const string& _markingName)
{
	//// �������� ���ҽ��� �����ٸ� ���´�� nullptr�� �� ���̴�.
	//int bExist = !!_res.Get();
	//_emitter << YAML::Key << "Exist";
	//_emitter << YAML::Value << bExist;
	//// �������� ���ҽ��� �־��ٸ�, � ���ҽ��� ������ �־����� Ű�� ��θ� �����Ѵ�.

	if (_res.Get())
	{
		_emitter << YAML::Key << _markingName + "ResKey";
		_emitter << YAML::Value << WStringToString(_res->GetKey());
		_emitter << YAML::Key << _markingName + "ResRelativePath";
		_emitter << YAML::Value << WStringToString(_res->GetRelativePath());
	}
}

template<typename T>
void LoadResourceRef(Ptr<T>& _Res, YAML::Node& _node)
{
	wstring strKey, strRelativePath;

	if (_node["ResKey"].IsDefined())
	{
		strKey = StringToWString(_node["ResKey"].as<string>());
	}

	if (_node["ResRelativePath"].IsDefined())
	{
		strRelativePath = StringToWString(_node["ResRelativePath"].as<string>());
	}

	if (!strKey.empty())
	{
		_Res = CResMgr::GetInst()->Load<T>(strKey, strRelativePath);
	}
}

template<typename T>
void LoadResourceRefEX(Ptr<T>& _Res, YAML::Node& _node, const string& _markingName)
{
	wstring strKey, strRelativePath;

	if (_node[_markingName + "ResKey"].IsDefined())
	{
		strKey = StringToWString(_node[_markingName + "ResKey"].as<string>());
	}

	if (_node[_markingName + "ResRelativePath"].IsDefined())
	{
		strRelativePath = StringToWString(_node[_markingName + "ResRelativePath"].as<string>());
	}

	if (!strKey.empty())
	{
		_Res = CResMgr::GetInst()->Load<T>(strKey, strRelativePath);
	}
}

// Component, Resource Type enum class ���� ���� char�� wchar���� ��ȯ �Լ�
const char* ToString(RES_TYPE);
const wchar_t* ToWString(RES_TYPE);

const char* ToString(COMPONENT_TYPE);
const wchar_t* ToWString(COMPONENT_TYPE);

// ========================
// =   Event ���� �Լ�    =
// ========================
class CGameObject;
void Instantiate(CGameObject* _pNewObj, Vec3 _vWorldPos, int _iLayerIdx = 0);
void AddChild(CGameObject* _pParent, CGameObject* _pChild);
class CLevel;
void ChangeLevel(CLevel* _NextLevel);
void ChangeLevelAndPlay(CLevel* _NextLevel);

void MakeParent(CGameObject* _pChildObj);
void MakeParentReserve(CGameObject* _pChildObj);
void AddComponent(CGameObject* _pObj, COMPONENT_TYPE _eType);
void TimeSlow(bool _bActive, float _fRatio = 2.f);
void TimeSlowAffectedObj(bool _bActive, CGameObject* _pObj);

class CRes;
void AddRes(CRes* _Res, RES_TYPE _Type);
void DeleteRes(CRes* _Res, RES_TYPE _Type);

// ==================
// =   Debug Draw   =
// ==================
#ifdef _DEBUG
// ���� �ð��� 0���� �־��ִ� ������ Collider2D���� �� Finaltick ���� ȣ�����ֱ� ������ 0�� ���� ������ ��ø�� �߻��Ѵ�.
void DebugDrawRect(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration = 0.f);
void DebugDrawCircle(Vec4 _vColor, Vec3 _vPosition, float _fRadius, float _fDuration = 0.f);
void DebugDrawCube(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration = 0.f);
void DebugDrawSphere(Vec4 _vColor, Vec3 _vPosition, float _fRadius, float _fDuration = 0.f);
void DebugDrawCone(Vec4 _vColor, Vec3 _vPosition, Vec3 _vRotation, float _fRadius, float _fAngle, float _fDuration = 0.f);
void DebugDrawFrustum(Vec4 _vColor, Vec3 _vPosition, Vec3 _vRotation, float _fDuration = 0.f);
void DebugDrawCylinder(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration = 0.f);
#endif