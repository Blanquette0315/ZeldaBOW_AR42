#include "pch.h"

#include "CLevelMgr.h"

wstring GetRelativePath(const wstring& _strBase, const wstring& _strPath)
{
	// find함수로 Path경로(전체경로) 내에 Base경로가 들어있지 않는다면 -1을 반환한다.
	// 따라서 들어있지 않는다면 (겹치는 내용이 아니라면) 텅비어있는 wstring을 반환한다.
	wstring strRelativePath;
	if (-1 == _strPath.find(_strBase))
	{
		return strRelativePath;
	}

	// find결과 base가 path내에 있다면, 해당 부분을 잘라서 상대경로만 반환한다.
	strRelativePath = _strPath.substr(_strBase.length(), _strPath.length());
	return strRelativePath;
}

string WStringToString(const wstring& _str)
{
	return string(_str.begin(), _str.end());
}

wstring StringToWString(const string& _str)
{
	return wstring(_str.begin(), _str.end());
}

int GetSizeofFormat(DXGI_FORMAT _eFormat)
{
	int iRetByte = 0;
	switch (_eFormat)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		iRetByte = 128;
		break;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		iRetByte = 96;
		break;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		iRetByte = 64;
		break;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		iRetByte = 32;
		break;
	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		iRetByte = 16;
		break;
	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		iRetByte = 8;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
		iRetByte = 128;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R1_UNORM:
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		iRetByte = 64;
		break;
		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		iRetByte = 32;
		break;
		// These are compressed, but bit-size information is unclear.
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		iRetByte = 32;
		break;
	case DXGI_FORMAT_UNKNOWN:
	default:
		return -1;
	}

	return iRetByte / 8;
}

#include "CGameObject.h"
bool IsValid(CGameObject*& _pObj)
{
	if (nullptr == _pObj) // nullptr이 들어있었으면 없다는 뜻이므로 유효하지 않음.
		return false;

	if (_pObj->IsDead()) // 죽었으면 해당 pObj에 nullptr을 넣어 해제해준다.
	{
		_pObj = nullptr;
		return false;
	}
	return true; // 사용해도 괜찮음.
}

Matrix GetMatrixFromFbxMatrix(FbxAMatrix& _mat)
{
	Matrix mat;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat.m[i][j] = (float)_mat.Get(i, j);
		}
	}
	return mat;
}


void SaveStringToFile(const string& _str, FILE* _pFile)
{
	// 문자열 길이 저장
	size_t iLen = _str.length();
	fwrite(&iLen, sizeof(size_t), 1, _pFile);
	// 들어온 string을 문자열 길이만큼 저장.
	fwrite(_str.data(), sizeof(char), iLen, _pFile);
}

void LoadStringFromFile(string& _str, FILE* _pFile)
{
	// 문자열 길이 읽기
	size_t iLen = 0;
	fread(&iLen, sizeof(size_t), 1, _pFile);

	// 문자열 길이만큼 읽어와 char배열로 복구
	char szBuffer[256] = {};
	fread(szBuffer, sizeof(char), iLen, _pFile);

	_str = szBuffer;
}

void SaveWStringToFile(const wstring& _str, FILE* _pFile)
{
	size_t iLen = _str.length();
	fwrite(&iLen, sizeof(size_t), 1, _pFile);
	fwrite(_str.data(), sizeof(wchar_t), iLen, _pFile);
}

void LoadWStringFromFile(wstring& _str, FILE* _pFile)
{
	size_t iLen = 0;
	fread(&iLen, sizeof(size_t), 1, _pFile);

	wchar_t szBuffer[256] = {};
	fread(szBuffer, sizeof(wchar_t), iLen, _pFile);

	_str = szBuffer;
}

#include "CGameObject.h"
#include "CTransform.h"
#include "CEventMgr.h"
#include "CRes.h"

void Instantiate(CGameObject* _pNewObj, Vec3 _vWorldPos, int _iLayerIdx)
{
	_pNewObj->Transform()->SetRelativePos(_vWorldPos);

	tEvent evn = {};
	evn.eType = EVENT_TYPE::CREATE_OBJECT;
	evn.wParam = (DWORD_PTR)_pNewObj;
	evn.lParam = (DWORD_PTR)_iLayerIdx;

	CEventMgr::GetInst()->AddEvent(evn);
}

void AddRes(CRes* _Res, RES_TYPE _Type)
{
	_Res->AddRef();

	tEvent evn = {};
	evn.eType = EVENT_TYPE::ADD_RES;
	evn.wParam = (DWORD_PTR)_Type;
	evn.lParam = (DWORD_PTR)_Res;
	CEventMgr::GetInst()->AddEvent(evn);
}

void DeleteRes(CRes* _Res, RES_TYPE _Type)
{
	_Res->AddRef();

	tEvent evn = {};
	evn.eType = EVENT_TYPE::DELETE_RES;
	evn.wParam = (DWORD_PTR)_Type;
	evn.lParam = (DWORD_PTR)_Res;
	CEventMgr::GetInst()->AddEvent(evn);
}

#include "CLevel.h"
void ChangeLevel(CLevel* _NextLevel)
{
	tEvent ChangeLevel = {};
	ChangeLevel.eType = EVENT_TYPE::CHANGE_LEVEL;
	ChangeLevel.wParam = (DWORD_PTR)_NextLevel;
	CEventMgr::GetInst()->AddEvent(ChangeLevel);
}

void ChangeLevelAndPlay(CLevel* _NextLevel)
{
	tEvent ChangeLevel = {};
	ChangeLevel.eType = EVENT_TYPE::CHANGE_LEVEL;
	ChangeLevel.wParam = (DWORD_PTR)_NextLevel;
	CEventMgr::GetInst()->AddEvent(ChangeLevel);

	tEvent ChangeLevelState = {};
	ChangeLevelState.eType = EVENT_TYPE::CHANGE_LEVEL_STATE;
	ChangeLevelState.wParam = (DWORD_PTR)LEVEL_STATE::PLAY;
	CEventMgr::GetInst()->AddEvent(ChangeLevelState);
}

#include "CRenderMgr.h"

// 나중에 UI에서 RECT를 고르면 반지름을 넣어주지 못하도록 막아줄 것이다.
void DebugDrawRect(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(DEBUG_SHAPE::RECT, _vColor, _vPosition, _vScale, _vRotation, 0.f, 0.f, _fDuration);
}

// 나중에 UI에서 Circle을 고르면 스케일과 로테이션을 넣지 못하게 막을 것이다.
void DebugDrawCircle(Vec4 _vColor, Vec3 _vPosition, float _fRadius, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(DEBUG_SHAPE::CIRCLE, _vColor, _vPosition, Vec3::Zero, Vec3::Zero, _fRadius, 0.f, _fDuration);
}

void DebugDrawCube(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(DEBUG_SHAPE::CUBE, _vColor, _vPosition, _vScale, _vRotation, 0.f, 0.f, _fDuration);
}

void DebugDrawSphere(Vec4 _vColor, Vec3 _vPosition, float _fRadius, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(DEBUG_SHAPE::SPHERE, _vColor, _vPosition, Vec3::Zero, Vec3::Zero, _fRadius, 0.f, _fDuration);
}

void DebugDrawCone(Vec4 _vColor, Vec3 _vPosition, Vec3 _vRotation, float _fRadius, float _fAngle, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(DEBUG_SHAPE::CONE, _vColor, _vPosition, Vec3::Zero, _vRotation, _fRadius, _fAngle, _fDuration);
}

void DebugDrawFrustum(Vec4 _vColor, Vec3 _vPosition, Vec3 _vRotation, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(DEBUG_SHAPE::FRUSTUM, _vColor, _vPosition, Vec3::Zero, _vRotation, 0.f, 0.f, _fDuration);
}

void DebugDrawCylinder(Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fDuration)
{
	CRenderMgr::GetInst()->DebugDraw(DEBUG_SHAPE::CYLINDER, _vColor, _vPosition, _vScale, _vRotation, 0.f, 0.f, _fDuration);
}

const char* ToString(RES_TYPE _type)
{
	return RES_TYPE_CHAR[(UINT)_type];
}

const wchar_t* ToWString(RES_TYPE _type)
{
	return RES_TYPE_WCHAR[(UINT)_type];
}

const char* ToString(COMPONENT_TYPE _type)
{
	return COMPONENT_TYPE_CHAR[(UINT)_type];
}

const wchar_t* ToWString(COMPONENT_TYPE _type)
{
	return COMPONENT_TYPE_WCHAR[(UINT)_type];
}

inline wstring ConvertStrToWstr(const string& _str)
{
	return wstring(_str.begin(), _str.end());
}

inline string ConvertWstrToStr(const wstring& _wstr)
{
	return string(_wstr.begin(), _wstr.end());
}

vector<wstring> ConvertStrToWstrVec(const vector<string>& _vecStr)
{
	vector<wstring> vecWstr;

	for (size_t i = 0; i < _vecStr.size(); ++i)
	{
		vecWstr.push_back(ConvertStrToWstr(_vecStr[i]));
	}
	return vecWstr;
}

vector<string> ConvertWstrToStrVec(const vector<wstring>& _vecWstr)
{
	vector<string> vecStr;

	for (size_t i = 0; i < _vecWstr.size(); ++i)
	{
		vecStr.push_back(ConvertWstrToStr(_vecWstr[i]));
	}
	return vecStr;
}

#include "CCamera.h"

bool PickingObj(CGameObject*& _pObjOut)
{
	const vector<CGameObject*>& vecObj = CFrustum::GetInFrustumObjs();

	// Clear static dist member to find nearest obj.
	CTransform::ClearBoundingBoxDist();
	
	for (size_t i = 0; i < vecObj.size(); ++i)
	{
		CGameObject* pTmp = nullptr;
		pTmp = vecObj[i]->Transform()->CheckRay(CRenderMgr::GetInst()->GetMainCam()->GetRay());
		if (pTmp)
		{
			_pObjOut = pTmp;
		}
	}
	if (_pObjOut)
		return true;
	else
		return false;
}


CGameObject* FindObjectByName(const wstring& _name)
{
	return CLevelMgr::GetInst()->GetCurLevel()->FindObjectByName(_name);
}

void QuaternionToEuler(Vec4 _vQRot, Vec3& _vERot)
{
	// roll (x-axis rotation)
	double sinr_cosp = 2 * (_vQRot.w * _vQRot.x + _vQRot.y * _vQRot.z);
	double cosr_cosp = 1 - 2 * (_vQRot.x * _vQRot.x + _vQRot.y * _vQRot.y);
	_vERot.x = (float)std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = std::sqrt(1 + 2 * (_vQRot.w * _vQRot.y - _vQRot.x * _vQRot.z));
	double cosp = std::sqrt(1 - 2 * (_vQRot.w * _vQRot.y - _vQRot.x * _vQRot.z));
	_vERot.y = (float)(2 * std::atan2(sinp, cosp) - XM_PI / 2);

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (_vQRot.w * _vQRot.z + _vQRot.x * _vQRot.y);
	double cosy_cosp = 1 - 2 * (_vQRot.y * _vQRot.y + _vQRot.z * _vQRot.z);
	_vERot.z = (float)std::atan2(siny_cosp, cosy_cosp);
}

bool closeEnough(const float& a, const float& b
	, const float& epsilon = std::numeric_limits<float>::epsilon())
{
	return (epsilon > std::abs(a - b));
}

Vec3 DecomposeRotMat(const Matrix& _matRot)
{
	// _mat 을 분해 후 다시 행렬 만들기	
	Vec4 vMat[4];

	vMat[0] = Vec4(_matRot._11, _matRot._12, _matRot._13, _matRot._14);
	vMat[1] = Vec4(_matRot._21, _matRot._22, _matRot._23, _matRot._24);
	vMat[2] = Vec4(_matRot._31, _matRot._32, _matRot._33, _matRot._34);
	vMat[3] = Vec4(_matRot._41, _matRot._42, _matRot._43, _matRot._44);

	/*XMStoreFloat4(&vMat[0], _matRot._11);
	XMStoreFloat4(&vMat[1], _matRot.r[1]);
	XMStoreFloat4(&vMat[2], _matRot.r[2]);
	XMStoreFloat4(&vMat[3], _matRot.r[3]);*/

	Vec3 vNewRot;
	if (closeEnough(vMat[0].z, -1.0f)) {
		float x = 0; //gimbal lock, value of x doesn't matter
		float y = XM_PI / 2;
		float z = x + atan2f(vMat[1].x, vMat[2].x);
		vNewRot = Vec3{ x, y, z };
	}
	else if (closeEnough(vMat[0].z, 1.0f)) {
		float x = 0;
		float y = -XM_PI / 2;
		float z = -x + atan2f(-vMat[1].x, -vMat[2].x);
		vNewRot = Vec3{ x, y, z };
	}
	else { //two solutions exist
		float y1 = -asinf(vMat[0].z);
		float y2 = XM_PI - y1;

		float x1 = atan2f(vMat[1].z / cosf(y1), vMat[2].z / cosf(y1));
		float x2 = atan2f(vMat[1].z / cosf(y2), vMat[2].z / cosf(y2));

		float z1 = atan2f(vMat[0].y / cosf(y1), vMat[0].x / cosf(y1));
		float z2 = atan2f(vMat[0].y / cosf(y2), vMat[0].x / cosf(y2));

		//choose one solution to return
		//for example the "shortest" rotation
		if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2)))
		{
			vNewRot = Vec3{ x1, y1, z1 };
		}
		else {
			vNewRot = Vec3{ x2, y2, z2 };
		}
	}
	return vNewRot;
}

bool CalBit(UINT _originBit, UINT _cmpBits, BIT_FUNC_OPT _opt, UINT _CompareBit)
{
	switch (_opt)
	{
	case BIT_LEAST_ONE:
	{
		if (_originBit & _cmpBits)
			return true;
		return false;
	}
	break;

	case BIT_EQUAL:
	{
		if (_originBit == _cmpBits)
			return true;
		return false;
	}
	break;

	case BIT_INCLUDE:
	{
		if ((_originBit & _cmpBits) == _cmpBits)
			return true;
		return false;
	}
	break;

	case BIT_EQUAL_SELECTED_BIT:
	{
		RemoveBit(_cmpBits, ~_CompareBit);
		RemoveBit(_originBit, ~_CompareBit);

		if (_originBit == _cmpBits)
			return true;
		return false;
	}
	break;
	}
}

void AddBit(UINT& _src, UINT _add)
{
	_src |= _add;
}

void RemoveBit(UINT& _src, UINT _remove)
{
	_src &= ~_remove;
}