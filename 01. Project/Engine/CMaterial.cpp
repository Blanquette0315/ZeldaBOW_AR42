#include "pch.h"
#include "CMaterial.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CGraphicsShader.h"

#include "CPathMgr.h"


CMaterial::CMaterial(bool _bEngineRes)
	: CRes(RES_TYPE::MATERIAL, _bEngineRes)
	, m_tConst{}
	, m_arrTex{}
{
}

CMaterial::CMaterial(const CMaterial& _other)
	: CRes(_other)
	, m_tConst(_other.m_tConst)
	, m_pShader(_other.m_pShader)
	, m_arrTex{}
{
	for (UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
	{
		m_arrTex[i] = _other.m_arrTex[i];
	}
}

CMaterial::~CMaterial()
{
}

void CMaterial::SetScalarParam(SCALAR_PARAM _eScalarType, void* _pData)
{
	switch (_eScalarType)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
		m_tConst.iArr[_eScalarType] = *((int*)_pData);
		break;
	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
		m_tConst.fArr[_eScalarType - FLOAT_0] = *((float*)_pData);
		break;
	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
		m_tConst.v2Arr[_eScalarType - VEC2_0] = *((Vec2*)_pData);
		break;
	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
		m_tConst.v4Arr[_eScalarType - VEC4_0] = *((Vec4*)_pData);
		break;
	case MAT_0:
	case MAT_1:
	case MAT_2:
	case MAT_3:
		m_tConst.matArr[_eScalarType - MAT_0] = *((Matrix*)_pData);
		break;
	}
}

void CMaterial::GetScalarParam(SCALAR_PARAM _eScalarType, void* _pOut)
{
	switch (_eScalarType)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
		*((int*)_pOut) = m_tConst.iArr[_eScalarType];
		break;
	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
		*((float*)_pOut) = m_tConst.fArr[_eScalarType - FLOAT_0];
		break;
	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
		*((Vec2*)_pOut) = m_tConst.v2Arr[_eScalarType - VEC2_0];
		break;
	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
		*((Vec4*)_pOut) = m_tConst.v4Arr[_eScalarType - VEC4_0];
		break;
	case MAT_0:
	case MAT_1:
	case MAT_2:
	case MAT_3:
		*((Matrix*)_pOut) = m_tConst.matArr[_eScalarType - MAT_0];
		break;
	}
}

void CMaterial::SetTexParam(TEX_PARAM _eTex, Ptr<CTexture> _pTex)
{
	m_arrTex[_eTex] = _pTex;
}

void CMaterial::UpdateData()
{
	// Texture 업데이트
	for (UINT i = 0; i < TEX_END; ++i)
	{
		// 바인딩을 걸기위해 존재하는 Texture가 있으면 1을 없으면 0을 넣어서 확인해 볼 수 있게 함.
		if (nullptr != m_arrTex[i])
		{
			m_arrTex[i]->UpdateData(i, ALL_STAGE);
			m_tConst.HasTex[i] = 1;
		}
		else
		{
			m_tConst.HasTex[i] = 0;
		}
	}

	// 상수 업데이트
	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_tConst);
	pCB->UpdateData(PIPELINE_STAGE::ALL_STAGE);

	// Shader 업데이트
	m_pShader->UpdateData();
}

void CMaterial::Clear()
{
	for (UINT i = 0; i < TEX_END; ++i)
	{
		CTexture::Clear(i);
	}
}

void CMaterial::Save(const wstring& _strRelativePath)
{
	// 파일과 1:1 매칭 확인
	// 확인을 해서 true가 나오면 저장을 하고, false가 반환되면 저장을 안하고 중단을 건다.
	if (!CheckRelativePath(_strRelativePath))
		assert(nullptr);

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	YAML::Emitter emitter;
	emitter << YAML::BeginMap;

	// 재질 이름 저장
	CEntity::SaveToYAML(emitter);

	// Key, RelativePath 저장
	SaveKeyPath(emitter);

	// 참조 쉐이더 정보 저장
	// Shader는 우리가 항상 수동으로 코딩해서 만들기 때문에 이미 ResMgr에 등록이 되어있을 것이다. 항상.
	// 따라서 shader는 키값만 저장해 찾아서 사용하는 방식을 채택했다.
	emitter << YAML::Key << "MaterialShader";
	emitter << YAML::Value << YAML::BeginMap;
	SaveResourceRef(m_pShader, emitter);
	emitter << YAML::EndMap;

	// 상수, 참조 텍스쳐 정보 저장
	// Shader가 없다면 전달하는 값도 의미가 없기 때문에 체크해주고 저장한다.
	if (nullptr != m_pShader)
	{
		// 상수 파라미터는 그대로 있는것을 저장하면 된다.
		emitter << YAML::Key << "ConstIntArr";
		emitter << YAML::Value << YAML::BeginSeq << m_tConst.iArr[0] << m_tConst.iArr[1] << m_tConst.iArr[2] << m_tConst.iArr[3] << YAML::EndSeq;
		emitter << YAML::Key << "ConstFloatArr";
		emitter << YAML::Value << YAML::BeginSeq << m_tConst.fArr[0] << m_tConst.fArr[1] << m_tConst.fArr[2] << m_tConst.fArr[3] << YAML::EndSeq;
		emitter << YAML::Key << "ConstVec2Arr";
		emitter << YAML::Value << YAML::BeginSeq << m_tConst.v2Arr[0] << m_tConst.v2Arr[1] << m_tConst.v2Arr[2] << m_tConst.v2Arr[3] << YAML::EndSeq;
		emitter << YAML::Key << "ConstVec4Arr";
		emitter << YAML::Value << YAML::BeginSeq << m_tConst.v4Arr[0] << m_tConst.v4Arr[1] << m_tConst.v4Arr[2] << m_tConst.v4Arr[3] << YAML::EndSeq;
		emitter << YAML::Key << "ConstMatrixArr";
		emitter << YAML::Value << YAML::BeginSeq << m_tConst.matArr[0] << m_tConst.matArr[1] << m_tConst.matArr[2] << m_tConst.matArr[3] << YAML::EndSeq;
		emitter << YAML::Key << "ConstHasTex";
		emitter << YAML::Value << YAML::BeginSeq;
		for (UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
		{
			emitter << m_tConst.HasTex[i];
		}
		emitter << YAML::EndSeq;

		for (UINT i = 0; i < TEX_PARAM::TEX_END; ++i)
		{
			// 텍스처 파라미터는 참조중인 Texture를 SaveResourceRef를 이용해 저장한다.
			emitter << YAML::Key << "MaterialShaderTexture" + std::to_string(i);
			emitter << YAML::Value << YAML::BeginMap;
			SaveResourceRef(m_arrTex[i], emitter);
			emitter << YAML::EndMap;
		}
	}

	emitter << YAML::EndMap;

	std::ofstream fout(strFilePath);
	fout << emitter.c_str();
}

int CMaterial::Load(const wstring& _strFilePath)
{
	// 파일 읽기
	YAML::Node rootNode = YAML::LoadFile(WStringToString(_strFilePath));

	// 이름 불러오기
	CEntity::LoadFromYAML(rootNode);

	// Key, RelativePath 불러오기
	LoadKeyPath(rootNode);

	// 참조 쉐이더 복구
	YAML::Node node = rootNode["MaterialShader"];
	LoadResourceRef(m_pShader, node);

	// 상수, 참조 텍스쳐 정보 복구
	if (nullptr != m_pShader)
	{
		tMtrlConst tConst;

		for (UINT i = 0; i < 4; ++i)
		{
			m_tConst.iArr[i] = rootNode["ConstIntArr"][i].as<int>();
			m_tConst.fArr[i] = rootNode["ConstFloatArr"][i].as<float>();
			m_tConst.v2Arr[i] = rootNode["ConstVec2Arr"][i].as<Vec2>();
			m_tConst.v4Arr[i] = rootNode["ConstVec4Arr"][i].as<Vec4>();
			m_tConst.matArr[i] = rootNode["ConstMatrixArr"][i].as<Matrix>();
		}
		for (UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
		{
			m_tConst.HasTex[i] = rootNode["ConstHasTex"][i].as<int>();
		}
		
		for (UINT i = 0; i < TEX_PARAM::TEX_END; ++i)
		{
			node = rootNode["MaterialShaderTexture" + std::to_string(i)];
			LoadResourceRef(m_arrTex[i], node);
		}
	}

	return S_OK; 
}
