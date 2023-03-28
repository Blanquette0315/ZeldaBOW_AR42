#include "pch.h"
#include "CSaveLoadMgr.h"

#include <Engine/CPathMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>
#include <Engine/CComponent.h>
#include <Engine/GlobalComponent.h>
#include <Engine/CCollisionMgr.h>
#include <Engine/CPrefab.h>

#include <Script/CScriptMgr.h>
#include <Engine/CScript.h>

CSaveLoadMgr::CSaveLoadMgr()
{
}

CSaveLoadMgr::~CSaveLoadMgr()
{
}

void CSaveLoadMgr::init()
{
	CPrefab::Save_GameObject_Func = &CSaveLoadMgr::SaveGameObject;
	CPrefab::Load_GameObject_Func = &CSaveLoadMgr::LoadGameObject;
}

void CSaveLoadMgr::SaveLevel(CLevel* _Level, const wstring& _customName)
{
	assert(_Level);

	// ���
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	
	wstring RelativePath = L"level\\";
	if (_customName.empty())
		RelativePath += _Level->GetName();
	else
		RelativePath += _customName;
	RelativePath += L".lv";
	strFilePath += RelativePath;

	// ��� ��� ����
	_Level->SetRelativePath(RelativePath);

	// ���� ����
	YAML::Emitter emitter;
	emitter << YAML::BeginMap;

	// ���� �̸� �� ��� ��� ����
	emitter << YAML::Key << "Level";
	emitter << YAML::Value << YAML::BeginMap;
	_Level->SaveToYAML(emitter);
	emitter << YAML::EndMap;

	// ������ ���̾� ����
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		emitter << YAML::Key << "Layer" + to_string(i);
		emitter << YAML::Value << YAML::BeginMap;
		// ���̾� �̸� ����
		CLayer* pLayer = _Level->GetLayer(i);
		pLayer->SaveToYAML(emitter);

		// ���̾ �ִ� �θ� ������Ʈ ����
		const vector<CGameObject*>& vecParentObj = pLayer->GetParentObject();

		size_t iObjectCount = vecParentObj.size();
		emitter << YAML::Key << "Size";
		emitter << YAML::Value << iObjectCount;
		emitter << YAML::Key << "ParentObj";
		emitter << YAML::Value << YAML::BeginMap;
		for (size_t j = 0; j < vecParentObj.size(); ++j)
		{
			emitter << YAML::Key << "ParentObj" + to_string(j);
			emitter << YAML::Value << YAML::BeginMap;
			SaveGameObject(vecParentObj[j], emitter);
			emitter << YAML::EndMap;
		}
		emitter << YAML::EndMap;

		emitter << YAML::EndMap;
	}

	emitter << YAML::EndMap;

	std::ofstream fout(strFilePath);
	fout << emitter.c_str();
}

CLevel* CSaveLoadMgr::LoadLevel(wstring _strRelativePath)
{
	// ���
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	string strPath = WStringToString(strFilePath);
	// ���� �б�
	YAML::Node rootNode = YAML::LoadFile(strPath);
	
	// ���� �̸� �� ��� ��� �ε�
	
	CLevel* pLevel = new CLevel;
	YAML::Node node = rootNode["Level"];
	pLevel->LoadFromYAML(node);

	// ������ ���̾�
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// ���̾� �̸�
		CLayer* pLayer = pLevel->GetLayer(i);
		node = rootNode["Layer" + to_string(i)];
		pLayer->LoadFromYAML(node);

		// ���̾ �ִ� �θ� ������Ʈ
		size_t iObjectCount = rootNode["Layer" + to_string(i)]["Size"].as<size_t>();

		for (size_t j = 0; j < iObjectCount; ++j)
		{
			YAML::Node ParentObjNode = rootNode["Layer" + to_string(i)]["ParentObj"]["ParentObj" + to_string(j)];
			CGameObject* pObject = LoadGameObject(ParentObjNode);
			pLayer->AddGameObject(pObject);
		}
	}
	return pLevel;
}

void CSaveLoadMgr::SaveGameObject(CGameObject* _Object, YAML::Emitter& _emitter)
{
	// ������Ʈ �̸� ����
	_Object->SaveToYAML(_emitter);

	// ������Ʈ ���� ������Ʈ ����
	_emitter << YAML::Key << "Component";
	_emitter << YAML::Value << YAML::BeginMap;
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		CComponent* pComponent = _Object->GetComponent((COMPONENT_TYPE)i);
		if (nullptr != pComponent)
		{
			pComponent->SaveToYAML(_emitter);
		}
	}
	_emitter << YAML::EndMap;

	// Script ����, �� ���� Script �̸��� �����Ѵ�.
	_emitter << YAML::Key << "Script";
	_emitter << YAML::Value << YAML::BeginMap;
	const vector<CScript*>& vecScript = _Object->GetScripts();
	size_t iScriptCount = vecScript.size();
	_emitter << YAML::Key << "ScriptCount";
	_emitter << YAML::Value << iScriptCount;

	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		_emitter << YAML::Key << "Script" + to_string(i);
		_emitter << YAML::Value << YAML::BeginMap;
		// Script�� �̸��� �����Ѵ�.
		// Script�� enum���� �����ع�����, ��ũ��Ʈ�� �߰��ɶ����� ������ �ٲ� ������ �Ǿ� �̸��� ������
		// �̸����� Script�� ã�ƿ����� �������.
		_emitter << YAML::Key << "ScriptName";
		_emitter << YAML::Value << WStringToString(CScriptMgr::GetScriptName(vecScript[i]));

		// Script�� �����͸� �����Ѵ�.
		vecScript[i]->SaveToYAML(_emitter);
		_emitter << YAML::EndMap;
	}
	_emitter << YAML::EndMap;

	// �ڽ� ������Ʈ
	const vector<CGameObject*> vecChild = _Object->GetChildObject();

	// ���������� �ڽ� ������Ʈ vector�� �����ͼ� ������ �����Ѵ�.
	_emitter << YAML::Key << "ChildObj";
	_emitter << YAML::Value << YAML::BeginMap;
	size_t iChildCount = vecChild.size();
	_emitter << YAML::Key << "ChildCount";
	_emitter << YAML::Value << iChildCount;

	// ���� �ݺ����� ���鼭 SaveGameObject�� ȣ���� ��������� ȣ���ϸ鼭 ��� �ڽ��� ��ȸ�Ѵ�.
	for (size_t i = 0; i < iChildCount; ++i)
	{
		_emitter << YAML::Key << "ChildObj" + to_string(i);
		_emitter << YAML::Value << YAML::BeginMap;
		SaveGameObject(vecChild[i], _emitter);
		_emitter << YAML::EndMap;
	}
	_emitter << YAML::EndMap;
}

CGameObject* CSaveLoadMgr::LoadGameObject(YAML::Node& _node)
{
	CGameObject* pObject = new CGameObject;

	// ������Ʈ �̸�
	pObject->LoadFromYAML(_node);

	// ������Ʈ ���� ������Ʈ
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		COMPONENT_TYPE type = (COMPONENT_TYPE)i;
		string strComponentName = ToString(type);
		if (_node["Component"][strComponentName].IsDefined())
		{
			CComponent* pComponent = nullptr;

			switch (type)
			{
			case COMPONENT_TYPE::TRANSFORM:
				pComponent = new CTransform;
				break;

			case COMPONENT_TYPE::CAMERA:
				pComponent = new CCamera;
				break;

			case COMPONENT_TYPE::COLLIDER:
				pComponent = new CCollider;
				break;

			case COMPONENT_TYPE::ANIMATOR2D:
				break;

			case COMPONENT_TYPE::LIGHT2D:
				pComponent = new CLight2D;
				break;

			case COMPONENT_TYPE::MESHRENDER:
				pComponent = new CMeshRender;
				break;

			case COMPONENT_TYPE::TILEMAP:
				pComponent = new CTileMap;
				break;

			case COMPONENT_TYPE::PARTICLESYSTEM:
				pComponent = new CParticleSystem;
				break;

			case COMPONENT_TYPE::LIGHT3D:
				pComponent = new CLight3D;
				break;
			case COMPONENT_TYPE::RIGIDBODY:
				pComponent = new CRigidBody;
				break;
			case COMPONENT_TYPE::ANIMATOR3D:
				break;
			case COMPONENT_TYPE::SKYBOX:
				pComponent = new CSkyBox;
				break;
			case COMPONENT_TYPE::DECAL:
				pComponent = new CDecal;
				break;
			case COMPONENT_TYPE::LANDSCAPE:
				pComponent = new CLandScape;
				break;
			}
			YAML::Node componentNode = _node["Component"];
			pObject->AddComponent(pComponent);
			pComponent->LoadFromYAML(componentNode);
		}
	}

	// Script
	size_t iScriptCount = _node["Script"]["ScriptCount"].as<size_t>();

	for (size_t i = 0; i < iScriptCount; ++i)
	{
		// �̸� �ε�
		wstring strScriptName = StringToWString(_node["Script"]["Script" + to_string(i)]["ScriptName"].as<string>());

		// �˾Ƴ� �̸����� ��ũ��Ʈ ��������
		CScript* pNewScript = CScriptMgr::GetScript(strScriptName);
		YAML::Node ScriptNode = _node["Script"]["Script" + to_string(i)];
		pNewScript->LoadFromYAML(ScriptNode);

		// ������Ʈ�� ��ũ��Ʈ �־��ֱ�
		pObject->AddComponent(pNewScript);
	}


	// �ڽ� ������Ʈ
	size_t iChildCount = _node["ChildObj"]["ChildCount"].as<size_t>();

	for (size_t i = 0; i < iChildCount; ++i)
	{
		YAML::Node ChildObjNode = _node["ChildObj"]["ChildObj" + to_string(i)];
		CGameObject* pChild = LoadGameObject(ChildObjNode);
		pObject->AddChild(pChild);
	}

	return pObject;
}