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

	// 경로
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	
	wstring RelativePath = L"level\\";
	if (_customName.empty())
		RelativePath += _Level->GetName();
	else
		RelativePath += _customName;
	RelativePath += L".lv";
	strFilePath += RelativePath;

	// 상대 경로 지정
	_Level->SetRelativePath(RelativePath);

	// 파일 쓰기
	YAML::Emitter emitter;
	emitter << YAML::BeginMap;

	// 레벨 이름 및 상대 경로 저장
	emitter << YAML::Key << "Level";
	emitter << YAML::Value << YAML::BeginMap;
	_Level->SaveToYAML(emitter);
	emitter << YAML::EndMap;

	// 레벨의 레이어 저장
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		emitter << YAML::Key << "Layer" + to_string(i);
		emitter << YAML::Value << YAML::BeginMap;
		// 레이어 이름 저장
		CLayer* pLayer = _Level->GetLayer(i);
		pLayer->SaveToYAML(emitter);

		// 레이어에 있는 부모 오브젝트 저장
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
	// 경로
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += _strRelativePath;

	string strPath = WStringToString(strFilePath);
	// 파일 읽기
	YAML::Node rootNode = YAML::LoadFile(strPath);
	
	// 레벨 이름 및 상대 경로 로딩
	
	CLevel* pLevel = new CLevel;
	YAML::Node node = rootNode["Level"];
	pLevel->LoadFromYAML(node);

	// 레벨의 레이어
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 레이어 이름
		CLayer* pLayer = pLevel->GetLayer(i);
		node = rootNode["Layer" + to_string(i)];
		pLayer->LoadFromYAML(node);

		// 레이어에 있는 부모 오브젝트
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
	// 오브젝트 이름 저장
	_Object->SaveToYAML(_emitter);

	// 오브젝트 소유 컴포넌트 저장
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

	// Script 개수, 각 각의 Script 이름을 저장한다.
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
		// Script의 이름을 저장한다.
		// Script의 enum값을 저장해버리면, 스크립트가 추가될때마다 순서가 바뀌어서 문제가 되어 이름을 저장해
		// 이름으로 Script를 찾아오도록 만들었다.
		_emitter << YAML::Key << "ScriptName";
		_emitter << YAML::Value << WStringToString(CScriptMgr::GetScriptName(vecScript[i]));

		// Script의 데이터를 저장한다.
		vecScript[i]->SaveToYAML(_emitter);
		_emitter << YAML::EndMap;
	}
	_emitter << YAML::EndMap;

	// 자식 오브젝트
	const vector<CGameObject*> vecChild = _Object->GetChildObject();

	// 마찬가지로 자식 오브젝트 vector를 가져와서 개수를 저장한다.
	_emitter << YAML::Key << "ChildObj";
	_emitter << YAML::Value << YAML::BeginMap;
	size_t iChildCount = vecChild.size();
	_emitter << YAML::Key << "ChildCount";
	_emitter << YAML::Value << iChildCount;

	// 이후 반복문을 돌면서 SaveGameObject를 호출해 재귀적으로 호출하면서 모든 자식을 순회한다.
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

	// 오브젝트 이름
	pObject->LoadFromYAML(_node);

	// 오브젝트 소유 컴포넌트
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
		// 이름 로딩
		wstring strScriptName = StringToWString(_node["Script"]["Script" + to_string(i)]["ScriptName"].as<string>());

		// 알아낸 이름으로 스크립트 가져오기
		CScript* pNewScript = CScriptMgr::GetScript(strScriptName);
		YAML::Node ScriptNode = _node["Script"]["Script" + to_string(i)];
		pNewScript->LoadFromYAML(ScriptNode);

		// 오브젝트에 스크립트 넣어주기
		pObject->AddComponent(pNewScript);
	}


	// 자식 오브젝트
	size_t iChildCount = _node["ChildObj"]["ChildCount"].as<size_t>();

	for (size_t i = 0; i < iChildCount; ++i)
	{
		YAML::Node ChildObjNode = _node["ChildObj"]["ChildObj" + to_string(i)];
		CGameObject* pChild = LoadGameObject(ChildObjNode);
		pObject->AddChild(pChild);
	}

	return pObject;
}