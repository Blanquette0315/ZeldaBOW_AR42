#pragma once

#include "CPathMgr.h"

#include "CMesh.h"
#include "CTexture.h"
#include "CGraphicsShader.h"
#include "CComputeShader.h"
#include "CMaterial.h"
#include "CPrefab.h"
#include "CSound.h"
#include "CMeshData.h"

class CResMgr
	: public CSingleton<CResMgr>
{
private:
	map<wstring, Ptr<CRes>>				m_arrRes[(UINT)RES_TYPE::END];
	vector<D3D11_INPUT_ELEMENT_DESC>	m_vecLayoutInfo;
	UINT								m_iLayoutOffset;

	bool								m_bChanged;

public:
	void tick() { m_bChanged = false; }

	bool IsChanged() { return m_bChanged; }

	template<typename T>
	void AddRes(const wstring& _strKey, T* _pRes);
	void AddRes(const wstring& _strKey, RES_TYPE _type, CRes* _pRes);

	template<typename T>
	Ptr<T> FindRes(const wstring& _strKey);
	Ptr<CRes> FindRes(const wstring& _strKey, RES_TYPE _type);

	template<typename T>
	Ptr<T> Load(const wstring& _strKey, const wstring& _strRelativePath);

	template<typename T>
	wstring GetNewResName();

	Ptr<CTexture> LoadTexture(const wstring& _strKey, const wstring& _strRelativePath, int _iMapLevel);
	Ptr<CTexture> CreateTexture(const wstring& _strKey, UINT _iWidth, UINT _iHeight, DXGI_FORMAT _FORMAT, UINT _iBindFlag);
	Ptr<CTexture> CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _Tex2D);

	Ptr<CMeshData> LoadFBX(const wstring& _strPath);

	const map<wstring, Ptr<CRes>>& GetResource(RES_TYPE _eType)
	{
		return m_arrRes[(UINT)_eType];
	}

public:
	void init();

	const vector<D3D11_INPUT_ELEMENT_DESC>& GetInputLayoutInfo() { return m_vecLayoutInfo; }

	void CreateConeMesh(float radius, float height, size_t tessellation);
	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, vector<Vtx>& Vertcies, vector<UINT>& Indices);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, vector<Vtx>& Vertcies, vector<UINT>& Indices);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, vector<Vtx>& Vertcies, vector<UINT>& Indices);

private:
	void InitSound();
	void CreateDefaultMesh();
	void CreateDefaultTexture();
	void CreateDefaultGrapicsShader();
	void CreateDefaultComputeShader();
	void CreateDefaultMaterial();
	void AddInputLayout(DXGI_FORMAT _eFormat, const char* _strSemanticName);

	bool DeleteRes(RES_TYPE _Type, const wstring& _strKey);

public:
	CResMgr();
	~CResMgr();

	friend class CEventMgr;
};


template<typename T>
RES_TYPE GetType()
{
	if (typeid(T).hash_code() == typeid(CMesh).hash_code())
	{
		return RES_TYPE::MESH;
	}

	if (typeid(T).hash_code() == typeid(CMeshData).hash_code())
	{
		return RES_TYPE::MESHDATA;
	}

	if (typeid(T).hash_code() == typeid(CTexture).hash_code())
	{
		return RES_TYPE::TEXTURE;
	}

	if (typeid(T).hash_code() == typeid(CComputeShader).hash_code())
	{
		return RES_TYPE::COMPUTE_SHADER;
	}

	if (typeid(T).hash_code() == typeid(CGraphicsShader).hash_code())
	{
		return RES_TYPE::GRAPHICS_SHADER;
	}

	if (typeid(T).hash_code() == typeid(CMaterial).hash_code())
	{
		return RES_TYPE::MATERIAL;
	}

	if (typeid(T).hash_code() == typeid(CPrefab).hash_code())
	{
		return RES_TYPE::PREFAB;
	}

	if (typeid(T).hash_code() == typeid(CSound).hash_code())
	{
		return RES_TYPE::SOUND;
	}

	else
	{
		return RES_TYPE::END;
	}
}

template<typename T>
inline void CResMgr::AddRes(const wstring& _strKey, T* _pRes)
{
	RES_TYPE eResType = GetType<T>();

	CRes* pRes = FindRes<T>(_strKey).Get();
	if(pRes)
		return;
	//assert(!pRes);

	_pRes->SetKey(_strKey);
	m_arrRes[(UINT)eResType].insert(make_pair(_strKey, _pRes));
	m_bChanged = true;
}

template<typename T>
Ptr<T> CResMgr::FindRes(const wstring& _strKey)
{
	RES_TYPE eResType = ::GetType<T>();

	map<wstring, Ptr<CRes>>::iterator iter = m_arrRes[(UINT)eResType].find(_strKey);

	if (iter == m_arrRes[(UINT)eResType].end())
	{
		return nullptr;
	}

	return (T*)iter->second.Get();
}

template<typename T>
Ptr<T> CResMgr::Load(const wstring& _strKey, const wstring& _strRelativePath)
{
	RES_TYPE eResType = GetType<T>();

	CRes* pResource = FindRes<T>(_strKey).Get();

	if (nullptr != pResource)
		return (T*)pResource;

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	pResource = new T;
	if (FAILED(pResource->Load(strFilePath)))
	{
		MessageBox(nullptr, strFilePath.c_str(), L"리소스 로딩 실패", MB_OK);
		return nullptr;
	}

	pResource->SetRelativePath(_strRelativePath);
	AddRes<T>(_strKey, (T*)pResource);

	return (T*)pResource;
}

// Editor에서 재질등의 Res를 만들 때 디폴트 이름을 넣어주는 함수이다.
template<typename T>
inline wstring CResMgr::GetNewResName()
{
	RES_TYPE type = GetType<T>();

	wstring strName = L"New ";

	// 사운드나 Shader의 경우 우리가 Editor로 만들지 않기 때문에 제외해야 한다.
	// 또 타입별로 붙어야하는 디폴트 이름이 다르기 때문에 switch case문으로 분기처리 해주었다.
	switch (type)
	{
	case RES_TYPE::PREFAB:
		break;
	case RES_TYPE::MESHDATA:
		break;
	case RES_TYPE::COMPUTE_SHADER:
		break;
	case RES_TYPE::MATERIAL:
		strName += L"Material ";
		break;
	case RES_TYPE::MESH:
		break;
	case RES_TYPE::TEXTURE:
		break;
	case RES_TYPE::SOUND:
		break;
	case RES_TYPE::GRAPHICS_SHADER:
		break;
	case RES_TYPE::END:
		break;
	default:
		break;
	}

	// 디폴트 이름 뒤에는 숫자를 붙여줄 것이다.
	UINT Count = 1;
	// while문으로 반복하면서 동일한 이름으로 등록된 리소스가 있다면, 다음 숫자로 넘버링해줄 것이다.
	while (true)
	{
		wstring Number = std::to_wstring(Count);

		wstring strTemp = strName;
		strTemp += Number;

		if (nullptr != FindRes<T>(strTemp))
		{
			++Count;
		}
		else
		{
			strName = strTemp;
			break;
		}
	}

	return strName;
}
