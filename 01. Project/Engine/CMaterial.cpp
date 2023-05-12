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
	m_tConst.mtrl.vDiff = Vec4(1.f, 1.f, 1.f, 1.f);
	m_tConst.mtrl.vSpec = Vec4(1.f, 1.f, 1.f, 1.f);
	m_tConst.mtrl.vAmb = Vec4(1.f, 1.f, 1.f, 1.f);
	m_tConst.mtrl.vEmv = Vec4(1.f, 1.f, 1.f, 1.f);
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
	// Texture ������Ʈ
	for (UINT i = 0; i < TEX_END; ++i)
	{
		// ���ε��� �ɱ����� �����ϴ� Texture�� ������ 1�� ������ 0�� �־ Ȯ���� �� �� �ְ� ��.
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

	// ��� ������Ʈ
	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_tConst);
	pCB->UpdateData(PIPELINE_STAGE::ALL_STAGE);

	// Shader ������Ʈ
	m_pShader->UpdateData();
}

void CMaterial::UpdateData_Inst()
{
	// �ؽ��� ������Ʈ
	for (UINT i = 0; i < TEX_END; ++i)
	{
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

	// ��� ������Ʈ
	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_tConst);
	pCB->UpdateData(PIPELINE_STAGE::ALL_STAGE);

	// ���̴� ������Ʈ
	if (nullptr != m_pShader)
		m_pShader->UpdateData_Inst();
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
	// ���ϰ� 1:1 ��Ī Ȯ��
	// Ȯ���� �ؼ� true�� ������ ������ �ϰ�, false�� ��ȯ�Ǹ� ������ ���ϰ� �ߴ��� �Ǵ�.
	if (!CheckRelativePath(_strRelativePath))
		assert(nullptr);

	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	YAML::Emitter emitter;
	emitter << YAML::BeginMap;

	// ���� �̸� ����
	CEntity::SaveToYAML(emitter);

	// Key, RelativePath ����
	SaveKeyPath(emitter);

	// ���� ���̴� ���� ����
	// Shader�� �츮�� �׻� �������� �ڵ��ؼ� ����� ������ �̹� ResMgr�� ����� �Ǿ����� ���̴�. �׻�.
	// ���� shader�� Ű���� ������ ã�Ƽ� ����ϴ� ����� ä���ߴ�.
	emitter << YAML::Key << "MaterialShader";
	emitter << YAML::Value << YAML::BeginMap;
	SaveResourceRef(m_pShader, emitter);
	emitter << YAML::EndMap;

	// ���, ���� �ؽ��� ���� ����
	// Shader�� ���ٸ� �����ϴ� ���� �ǹ̰� ���� ������ üũ���ְ� �����Ѵ�.
	if (nullptr != m_pShader)
	{
		// ��� �Ķ���ʹ� �״�� �ִ°��� �����ϸ� �ȴ�.
		emitter << YAML::Key << "ConstMtrlDiff";
		emitter << YAML::Value << m_tConst.mtrl.vDiff;
		emitter << YAML::Key << "ConstMtrlSpec";
		emitter << YAML::Value << m_tConst.mtrl.vSpec;
		emitter << YAML::Key << "ConstMtrlAmb";
		emitter << YAML::Value << m_tConst.mtrl.vAmb;
		emitter << YAML::Key << "ConstMtrlEmv";
		emitter << YAML::Value << m_tConst.mtrl.vEmv;
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

		emitter << YAML::Key << "ConstUVCount";
		emitter << YAML::Value << m_tConst.iUVCount;
		for (UINT i = 0; i < TEX_PARAM::TEX_END; ++i)
		{
			// �ؽ�ó �Ķ���ʹ� �������� Texture�� SaveResourceRef�� �̿��� �����Ѵ�.
			emitter << YAML::Key << "MaterialShaderTexture" + std::to_string(i);
			emitter << YAML::Value << YAML::BeginMap;
			SaveResourceRef(m_arrTex[i], emitter);
			// arr size -> 7
			if (i < 7)
			{
				emitter << YAML::Key << "ConstTexUVIndex";
				emitter << YAML::Value << m_tConst.iTexUVIndex[i];
			}
			emitter << YAML::EndMap;
		}
	}

	emitter << YAML::EndMap;

	std::ofstream fout(strFilePath);
	fout << emitter.c_str();
}

int CMaterial::Load(const wstring& _strFilePath)
{
	// ���� �б�
	YAML::Node rootNode = YAML::LoadFile(WStringToString(_strFilePath));

	// �̸� �ҷ�����
	CEntity::LoadFromYAML(rootNode);

	// Key, RelativePath �ҷ�����
	LoadKeyPath(rootNode);

	// ���� ���̴� ����
	YAML::Node node = rootNode["MaterialShader"];
	LoadResourceRef(m_pShader, node);

	// ���, ���� �ؽ��� ���� ����
	if (nullptr != m_pShader)
	{
		SAFE_LOAD_FROM_YAML(Vec4, m_tConst.mtrl.vDiff, rootNode["ConstMtrlDiff"]);
		SAFE_LOAD_FROM_YAML(Vec4, m_tConst.mtrl.vSpec, rootNode["ConstMtrlSpec"]);
		SAFE_LOAD_FROM_YAML(Vec4, m_tConst.mtrl.vAmb, rootNode["ConstMtrlAmb"]);
		SAFE_LOAD_FROM_YAML(Vec4, m_tConst.mtrl.vEmv, rootNode["ConstMtrlEmv"]);
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
		
		SAFE_LOAD_FROM_YAML(int, m_tConst.iUVCount, rootNode["ConstUVCount"]);
		for (UINT i = 0; i < TEX_PARAM::TEX_END; ++i)
		{
			node = rootNode["MaterialShaderTexture" + std::to_string(i)];
			LoadResourceRef(m_arrTex[i], node);

			if(i < 7)
				SAFE_LOAD_FROM_YAML(int, m_tConst.iTexUVIndex[i], node["ConstTexUVIndex"]);
		}
	}
		
	return S_OK; 
}
