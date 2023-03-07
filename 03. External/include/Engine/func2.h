#pragma once

// ====================== JM ======================================
wstring ConvertStrToWstr(const string& _str);
string ConvertWstrToStr(const wstring& _wstr);
vector<wstring> ConvertStrToWstrVec(const vector<string>& _vecStr);
vector<string> ConvertWstrToStrVec(const vector<wstring>& _vecWstr);

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
// ================================================================

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

// Relative Path 가져오기
wstring GetRelativePath(const wstring& _strBase, const wstring& _strPath);

string WStringToString(const wstring& _str);
wstring StringToWString(const string& _str); // 민수 추가

int GetSizeofFormat(DXGI_FORMAT _eFormat);

// 게임 오브젝트 유효성 검사
class CGameObject;
bool IsValid(CGameObject*& _pObj);

// 커포넌트용 유효성 체크
class CEntity;
inline bool IsValid(CEntity* _pCom)
{
	if (nullptr == _pCom)
		return false;
	return true;
}

// ====================
// = File Save / Load =
// ====================
void SaveStringToFile(const string& _str, FILE* _pFile);
void LoadStringFromFile(string& _str, FILE* _pFile);

void SaveWStringToFile(const wstring& _str, FILE* _pFile);
void LoadWStringFromFile(wstring& _str, FILE* _pFile);

// 자신이 어떤 리소스를 참조하고 있었는지 저장하는 함수
#include "CResMgr.h"
#include "Ptr.h"
template<typename T>
void SaveResourceRef(Ptr<T> _res, YAML::Emitter& _emitter)
{
	// 참조중인 리소스가 없었다면 없는대로 nullptr이 들어갈 것이다.
	int bExist = !!_res.Get();
	_emitter << YAML::Key << "Exist";
	_emitter << YAML::Value << bExist;
	// 참조중인 리소스가 있었다면, 어떤 리소스를 가지고 있었는지 키와 경로를 저장한다.
	if (bExist)
	{
		_emitter << YAML::Key << "ResKey";
		_emitter << YAML::Value << WStringToString(_res->GetKey());
		_emitter << YAML::Key << "ResRelativePath";
		_emitter << YAML::Value << WStringToString(_res->GetRelativePath());
	}
}

template<typename T>
void LoadResourceRef(Ptr<T>& _Res, YAML::Node& _node)
{
	int bExist = _node["Exist"].as<int>();

	if (bExist)
	{
		wstring strKey, strRelativePath;
		strKey = StringToWString(_node["ResKey"].as<string>());
		strRelativePath = StringToWString(_node["ResRelativePath"].as<string>());
		
		_Res = CResMgr::GetInst()->Load<T>(strKey, strRelativePath);
	}
}

// Component, Resource Type enum class 값에 따른 char와 wchar로의 변환 함수
const char* ToString(RES_TYPE);
const wchar_t* ToWString(RES_TYPE);

const char* ToString(COMPONENT_TYPE);
const wchar_t* ToWString(COMPONENT_TYPE);

// ========================
// =   Event 관련 함수    =
// ========================
class CGameObject;
void Instantiate(CGameObject* _pNewObj, Vec3 _vWorldPos, int _iLayerIdx = 0);
class CLevel;
void ChangeLevel(CLevel* _NextLevel);
void ChangeLevelAndPlay(CLevel* _NextLevel);

// ==================
// =   Debug Draw   =
// ==================
#ifdef _DEBUG
// 지속 시간을 0으로 넣어주는 이유는 Collider2D에서 매 Finaltick 마다 호출해주기 때문에 0이 들어가지 않으면 중첩이 발생한다.
void DebugDrawRect(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration = 0.f);
void DebugDrawCircle(Vec4 _vColor, Vec3 _vPosition, float _fRadius, float _fDuration = 0.f);
void DebugDrawCube(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration = 0.f);
void DebugDrawSphere(Vec4 _vColor, Vec3 _vPosition, float _fRadius, float _fDuration = 0.f);
void DebugDrawCone(Vec4 _vColor, Vec3 _vPosition, Vec3 _vRotation, float _fRadius, float _fAngle, float _fDuration = 0.f);
void DebugDrawFrustum(Vec4 _vColor, Vec3 _vPosition, Vec3 _vRotation, float _fDuration = 0.f);
#endif