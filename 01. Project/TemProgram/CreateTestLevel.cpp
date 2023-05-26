#include "pch.h"
#include "CreateTestLevel.h"

#include <Engine/CEventMgr.h>
#include <Engine/CResMgr.h>
#include <Engine/CCollisionMgr.h>

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CGameObject.h>
#include <Engine/GlobalComponent.h>

#include <Engine/CPaintShader.h>

#include <Script/CPlayerScript.h>
#include <Script/CMonsterAIScript.h>
#include <Script/CBokoblinScript.h>

#include "CCameraScript.h"

#include "CSaveLoadMgr.h"

void CreateTestLevel()
{
	//CLevel* pLevel = new CLevel;

	//Ptr<CMeshData> pMeshData = nullptr;
	//CGameObject* pObj = nullptr;
	//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\Default_Hair_Maya.fbx", 1);
	//pMeshData->Save(pMeshData->GetRelativePath());

	//pObj = pMeshData->Instantiate();
	//pObj->SetName(L"Default_Hair");
	//pObj->ChangePrefab();
	//// pLevel->AddGameObejct(pObj, 0);

	//CLevel* pLevel = CSaveLoadMgr::GetInst()->LoadLevel(L"level\\Village.lv");
	{
		CGameObject* pLandScape = new CGameObject;
		pLandScape->SetName(L"LandScape");

		pLandScape->AddComponent(new CTransform);
		pLandScape->AddComponent(new CLandScape);
		pLandScape->AddComponent(new CRigidBody);

		pLandScape->Transform()->SetRelativePos(1.f, 1.f, 1.f);
		pLandScape->Transform()->SetRelativeScale(100.f, 100.f, 100.f);
		pLandScape->LandScape()->SetFaceCount(16, 16);
		pLandScape->SetFrustumCul(true);

		//pLandScape->RigidBody()->UpdateTransformData(COLLIDER_TYPE::COLLIDER_TRI, true, true);
		pLandScape->RigidBody()->SetWorldPosition(0.f, 0.f, 0.f);
		pLandScape->RigidBody()->SetColliderType(COLLIDER_TYPE::COLLIDER_TRI);
		pLandScape->RigidBody()->SetColldierScaleSize(false);
		pLandScape->RigidBody()->SetKinematicOption(true);
		pLandScape->RigidBody()->SetDinamicOption(false);
		pLandScape->RigidBody()->SetGravityOption(false);
		pLandScape->RigidBody()->SetStaticFriction(0.f);
		pLandScape->RigidBody()->SetDynamicFriction(0.f);
		pLandScape->RigidBody()->SetRestitution(0.f);
		pLandScape->RigidBody()->SetQueryColliderFilter(FILTER_GROUP::eGround);

		pLevel->AddGameObject(pLandScape, 0);
	}
	ChangeLevel(pLevel);
	return;

	return;

	Ptr<CMeshData> pMeshData = nullptr;
	CGameObject* pObj = nullptr;

	

	// Layer �̸� ����
	pLevel->GetLayer(1)->SetName(L"Player");
	pLevel->GetLayer(2)->SetName(L"PlayerProjectile");
	pLevel->GetLayer(3)->SetName(L"Monster");
	pLevel->GetLayer(4)->SetName(L"MonsterProjectile");

	// Camera Object �߰�
	CGameObject* pCamObj = new CGameObject;
	pCamObj->SetName(L"MainCamera");

	pCamObj->AddComponent(new CTransform);
	pCamObj->AddComponent(new CCamera);

	pCamObj->Camera()->SetLayerMaskAll();
	pCamObj->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);

	pCamObj->Transform()->SetRelativePos(0.f, 0.f, -770.f);
	pCamObj->Transform()->SetRelativeRotation(XM_PI / 4.f, 0.f, 0.f);

	pLevel->AddGameObject(pCamObj, 0);

	// Directional Light �߰�
	// ���� ���⼺ ������ ���϶��� 0.7, �㿡�� 0.2������ �ΰ� ����Ѵ�.
	// ������ �׷��� �ʿ䰡 ���� ������ CRenderMesh�� �߰����� �ʴ´�.
	// Directional Light�� ���� ������ �ʿ� ���� ������ ���� �ʾҴ�.
	CGameObject* pDirLight = new CGameObject;
	pDirLight->SetName(L"DirectionalLight");

	pDirLight->AddComponent(new CTransform);
	pDirLight->AddComponent(new CLight3D);

	pDirLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pDirLight->Light3D()->SetLightSpecular(Vec3(0.4f, 0.4f, 0.4f));
	pDirLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));
	pDirLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);

	pLevel->AddGameObject(pDirLight, 0);


	// Point Light �߰�
	/*CGameObject* pPointLight = new CGameObject;
	pPointLight->SetName(L"PointLight");

	pPointLight->AddComponent(new CTransform);
	pPointLight->AddComponent(new CLight3D);

	pPointLight->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));


	pPointLight->Light3D()->SetLightColor(Vec3(1.f, 0.f, 0.f));
	pPointLight->Light3D()->SetLightSpecular(Vec3(0.4f, 0.4f, 0.4f));
	pPointLight->Light3D()->SetLightAmbient(Vec3(0.f, 0.f, 0.f));
	pPointLight->Light3D()->SetRadius(500.f);
	pPointLight->Light3D()->SetLightType(LIGHT_TYPE::POINT);

	pLevel->AddGameObject(pPointLight, 0);*/

	// Spot Light �߰�
	/*CGameObject* pSpotLight = new CGameObject;
	pSpotLight->SetName(L"SpotLight");

	pSpotLight->AddComponent(new CTransform);
	pSpotLight->AddComponent(new CLight3D);

	pSpotLight->Transform()->SetRelativePos(Vec3(0.f, 0.f, 400.f));
	pSpotLight->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));

	pSpotLight->Light3D()->SetLightType(LIGHT_TYPE::SPOT);

	pSpotLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pSpotLight->Light3D()->SetLightSpecular(Vec3(0.4f, 0.4f, 0.4f));
	pSpotLight->Light3D()->SetLightAmbient(Vec3(0.f, 0.f, 0.f));
	pSpotLight->Light3D()->SetRadius(2000.f);
	pSpotLight->Light3D()->SetInAngle(25);
	pSpotLight->Light3D()->SetOutAngle(45);

	pLevel->AddGameObject(pSpotLight, 0);*/

	// SkyBox �߰�
	CGameObject* pSkyBox = new CGameObject;
	pSkyBox->SetName(L"SkyBox");

	pSkyBox->AddComponent(new CTransform);
	pSkyBox->AddComponent(new CSkyBox);

	pSkyBox->Transform()->SetRelativePos(0.f, 500.f, 400.f);
	pSkyBox->Transform()->SetRelativeScale(300.f, 300.f, 300.f);

	//pSkyBox->SkyBox()->SetSkyBoxTex(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\Sky01.png"));
	//pSkyBox->SkyBox()->SetType(SKYBOX_TYPE::SPHERE);

	pSkyBox->SkyBox()->SetSkyBoxTex(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\skybox\\SkyWater.dds"));
	pSkyBox->SkyBox()->SetType(SKYBOX_TYPE::CUBE);

	pSkyBox->SetFrustumCul(false);

	pLevel->AddGameObject(pSkyBox, 0);

	// GameObject �ʱ�ȭ
	CGameObject* pObject = nullptr;

	//pObject = new CGameObject;
	//pObject->SetName(L"Player");

	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);
	//pObject->AddComponent(new CRigidBody);
	//pObject->AddComponent(new CCollider);
	//pObject->AddComponent(new CPlayerScript);
	////Sphere
	//pObject->Transform()->SetRelativePos(Vec3(0.f, 50.f, 0.f));
	//pObject->Transform()->SetRelativeScale(Vec3(25.f, 25.f, 25.f));
	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	//pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));
	//pObject->MeshRender()->GetCurMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	//pObject->MeshRender()->GetCurMaterial()->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));

	//pObject->RigidBody()->UpdateTransformData(COLLIDER_TYPE::COLLIDER_CAPSULE, false, true);
	//pObject->RigidBody()->SetCapsuleSize(6.4f, 12.8f);
	////pObject->RigidBody()->SetSphereSize(128.f);
	//pObject->RigidBody()->SetMass(10.f);
	//pObject->RigidBody()->SetRestitution(0.f);
	//pObject->RigidBody()->SetStaticFriction(0.6f);
	//pObject->RigidBody()->SetDynamicFriction(0.6f);
	//pObject->RigidBody()->SetCenterPoint(Vec3(0.f, 0.f, 1.f));
	//pObject->RigidBody()->SetLockAxis_Rot(true, true, true);
	//pObject->RigidBody()->SetGravityOption(true);

	//pObject->Collider()->SetColliderType(COLLIDER_TYPE::COLLIDER_CUBE);
	//pObject->Collider()->SetScale(Vec3(5.f, 2.f, 5.f));
	//pObject->Collider()->SetOffsetPos(Vec3(0.f, -12.8f, 0.f));
	////pObject->Collider()->SetCapsuleSize(65.f, 130.f);

	//pLevel->AddGameObject(pObject, 0);

	// LandScape �߰�
	CGameObject* pLandScape = new CGameObject;
	pLandScape->SetName(L"LandScape");
	
	pLandScape->AddComponent(new CTransform);
	pLandScape->AddComponent(new CLandScape);
	pLandScape->AddComponent(new CRigidBody);
	
	pLandScape->Transform()->SetRelativePos(1.f, 1.f, 1.f);
	pLandScape->Transform()->SetRelativeScale(100.f, 100.f, 100.f);
	pLandScape->LandScape()->SetFaceCount(16, 16);
	pLandScape->SetFrustumCul(true);
	
	//pLandScape->RigidBody()->UpdateTransformData(COLLIDER_TYPE::COLLIDER_TRI, true, true);
	pLandScape->RigidBody()->SetWorldPosition(0.f, 0.f, 0.f);
	pLandScape->RigidBody()->SetColliderType(COLLIDER_TYPE::COLLIDER_TRI);
	pLandScape->RigidBody()->SetColldierScaleSize(false);
	pLandScape->RigidBody()->SetKinematicOption(true);
	pLandScape->RigidBody()->SetDinamicOption(false);
	pLandScape->RigidBody()->SetGravityOption(false);
	pLandScape->RigidBody()->SetStaticFriction(0.f);
	pLandScape->RigidBody()->SetDynamicFriction(0.f);
	pLandScape->RigidBody()->SetRestitution(0.f);
	pLandScape->RigidBody()->SetQueryColliderFilter(FILTER_GROUP::eGround);
	
	pLevel->AddGameObject(pLandScape, 0);

	//return;

	//Ptr<CSound> pSound = CResMgr::GetInst()->FindRes<CSound>(L"sound\\shop.mp3");
	//pSound->Play(1, 0.3f, true, pObject);//Vec3(0.f, 500.f, 400.f));
	//pSound->PlayBGM(0.6f);

	//pObject = new CGameObject;
	//pObject->SetName(L"Player");

	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);
	//pObject->AddComponent(new CRigidBody);
	////pObject->AddComponent(new CCollider);
	////Sphere
	//pObject->Transform()->SetRelativePos(Vec3(0.f, 3000.f, 400.f));
	//pObject->Transform()->SetRelativeScale(Vec3(50.f, 50.f, 100.f));
	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	//pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));
	//pObject->MeshRender()->GetCurMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	//pObject->MeshRender()->GetCurMaterial()->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
	//
	//pObject->RigidBody()->UpdateTransformData(RIGIDCOLLIDER_TYPE::CUBE, false, true);
	//pObject->RigidBody()->SetMass(10.f);
	//pObject->RigidBody()->SetRestitution(0.f);
	//pObject->RigidBody()->SetStaticFriction(1.f);
	//pObject->RigidBody()->SetDynamicFriction(1.f);
	//pObject->RigidBody()->SetCenterPoint(Vec3(0.f,0.f,0.f));
	////pObject->RigidBody()->SetLockAxis_Rot(true, true, true);
	//pObject->RigidBody()->SetGravityOption(true);
	//
	////pObject->Collider()->SetColliderType(COLLIDER_TYPE::COLLIDER_SPHERE);
	////pObject->Collider()->SetRadius(128.f);

	//pLevel->AddGameObject(pObject, 0);

	// ============
	// FBX Loading
	// ============	
	{
		// Ptr<CMeshData> pMeshData = nullptr;
		// CGameObject* pObj = nullptr;
		// 
		// // TwnObj_Village_HatenoHouse_A_S_02
		// pMeshData = nullptr;
		// pObj = nullptr;
		// pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\TwnObj_Village_HatenoHouse_A_S_02.fbx");
		// pMeshData->Save(pMeshData->GetRelativePath());
		// 
		// pObj = pMeshData->Instantiate();
		// pObj->SetFrustumCul(false);
		// pObj->SetName(L"TwnObj_Village_HatenoHouse_A_S_02");
		// pObj->Transform()->SetRelativeScale(10.f, 10.f, 10.f);
		// 
		// pLevel->AddGameObject(pObj, 0);
		// 
		// // DgnObj_DungeonEntrance_B_01
		// pMeshData = nullptr;
		// pObj = nullptr;
		// pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\DgnObj_DungeonEntrance_B_01.fbx");
		// pMeshData->Save(pMeshData->GetRelativePath());
		// 
		// pObj = pMeshData->Instantiate();
		// pObj->SetFrustumCul(false);
		// pObj->SetName(L"DgnObj_DungeonEntrance_B_01");
		// pObj->Transform()->SetRelativeScale(10.f, 10.f, 10.f);
		// 
		// pLevel->AddGameObject(pObj, 0);
		// 
		// // TwnObj_Village_HatenoWell_A
		// pMeshData = nullptr;
		// pObj = nullptr;
		// pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\TwnObj_Village_HatenoWell_A.fbx");
		// pMeshData->Save(pMeshData->GetRelativePath());
		// 
		// pObj = pMeshData->Instantiate();
		// pObj->SetFrustumCul(false);
		// pObj->SetName(L"TwnObj_Village_HatenoWell_A");
		// pObj->Transform()->SetRelativeScale(10.f, 10.f, 10.f);
		// 
		// pLevel->AddGameObject(pObj, 0);

		//Ptr<CMeshData> pMeshData = nullptr;
		//CGameObject* pObj = nullptr;
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\Bokoblin_Red.fbx");
		//pMeshData->Save(pMeshData->GetRelativePath());
		//
		////pMeshData = CResMgr::GetInst()->FindRes<CMeshData>(L"meshdata\\Link_Maya.mdat");
		////Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"mesh\\Link_Maya\\Link_Maya.mesh");
		////pMesh->AddAnimationFromContainer(L"fbx\\Link.fbx");
		//
		//pObj = pMeshData->Instantiate();
		//pObj->AddComponent(new CRigidBody);
		//pObj->AddComponent(new CBokoblinScript);
		//pObj->SetFrustumCul(false);
		//pObj->SetName(L"Bokoblin_Red");
		//pObj->Transform()->SetRelativeScale(25.f, 25.f, 25.f);
		//pObj->Transform()->SetRelativePos(0.f, 0.f, -200.f);
		//pObj->RigidBody()->UpdateTransformData(COLLIDER_TYPE::COLLIDER_CAPSULE, false, true);
		//pObj->RigidBody()->SetCapsuleSize(4.f, 8.f);
		//pObj->RigidBody()->SetMass(10.f);
		//pObj->RigidBody()->SetRestitution(0.f);
		//pObj->RigidBody()->SetStaticFriction(0.6f);
		//pObj->RigidBody()->SetDynamicFriction(0.6f);
		//pObj->RigidBody()->SetCenterPoint(Vec3(0.f, 0.f, 1.f));
		//pObj->RigidBody()->SetLockAxis_Rot(true, true, true);
		//pObj->RigidBody()->SetGravityOption(true);
		//pLevel->AddGameObject(pObj, 0);
		////pObj->Animator3D()->Play(L"Take 001", true);
		//
		//CGameObject* pChild = new CGameObject;
		//pChild->SetName(L"MonsterAI");
		//pChild->AddComponent(new CTransform);
		//pChild->AddComponent(new CCollider);
		//pChild->AddComponent(new CMonsterAIScript);
		//pChild->Collider()->SetColliderType(COLLIDER_TYPE::COLLIDER_SPHERE);
		//pChild->Collider()->SetRadius(300.f);
		//pObj->AddChild(pChild);

		//Ptr<CMeshData> pMeshData = nullptr;
		//CGameObject* pObj = nullptr;
		//pMeshData = CResMgr::GetInst()->LoadFBX(L"fbx\\sw.fbx");
		//pMeshData->Save(pMeshData->GetRelativePath());
		//
		////pObj = pMeshData->Instantiate();
		////pObj->SetFrustumCul(false);
		////pObj->SetName(L"Map");
		////pObj->Transform()->SetRelativeScale(10.f, 10.f, 10.f);
		//
		//pObj = pMeshData->Instantiate();
		//pObj->AddComponent(new CRigidBody);
		//pObj->AddComponent(new CBokoblinScript);
		//pObj->SetFrustumCul(false);
		//pObj->SetName(L"Bokoblin_Red");
		//pObj->Transform()->SetRelativeScale(25.f, 25.f, 25.f);
		//pObj->Transform()->SetRelativePos(0.f, 0.f, -200.f);
		//pObj->RigidBody()->UpdateTransformData(COLLIDER_TYPE::COLLIDER_CAPSULE, false, true);
		//pObj->RigidBody()->SetCapsuleSize(4.f, 8.f);
		//pObj->RigidBody()->SetMass(10.f);
		//pObj->RigidBody()->SetRestitution(0.f);
		//pObj->RigidBody()->SetStaticFriction(0.6f);
		//pObj->RigidBody()->SetDynamicFriction(0.6f);
		//pObj->RigidBody()->SetCenterPoint(Vec3(0.f, 0.f, 1.f));
		//pObj->RigidBody()->SetLockAxis_Rot(true, true, true);
		//pObj->RigidBody()->SetGravityOption(true);
		//pLevel->AddGameObject(pObj, 0);
		////pObj->Animator3D()->Play(L"Take 001", true);
		//
		//CGameObject* pChild = new CGameObject;
		//pChild->SetName(L"MonsterAI");
		//pChild->AddComponent(new CTransform);
		//pChild->AddComponent(new CCollider);
		//pChild->AddComponent(new CMonsterAIScript);
		//pChild->Collider()->SetColliderType(COLLIDER_TYPE::COLLIDER_SPHERE);
		//pChild->Collider()->SetRadius(300.f);
		//pObj->AddChild(pChild);
	}

	// ��Į ����
	//CGameObject* pDecal = new CGameObject;
	//pDecal->SetName(L"Decal");
	//
	//pDecal->AddComponent(new CTransform);
	//pDecal->AddComponent(new CDecal);
	//
	//pDecal->Transform()->SetRelativePos(0.f, 500.f, 0.f);
	//pDecal->Transform()->SetRelativeScale(250.f, 250.f, 250.f);
	//
	//pDecal->Decal()->SetRenderType(true);
	//pDecal->Decal()->SetDecalTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\MagicCircle.png"));
	//
	//pLevel->AddGameObject(pDecal, 0);

	// �� ����
	//pObject = new CGameObject;
	//pObject->SetName(L"Plane_0");
	//
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);
	//pObject->AddComponent(new CRigidBody);
	//
	//pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
	//pObject->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));
	//pObject->Transform()->SetRelativeRotation(Vec3(XM_PI / 3.f, 0.f, 0.f));
	//
	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));
	//
	//pObject->RigidBody()->UpdateTransformData(COLLIDER_TYPE::COLLIDER_CUBE, true, true);
	//pObject->RigidBody()->SetColldierScaleSize(true);
	//pObject->RigidBody()->SetStaticFriction(0.f);
	//pObject->RigidBody()->SetDynamicFriction(0.f);
	//pObject->RigidBody()->SetRestitution(0.f);
	//pObject->RigidBody()->SetColliderFilter(FILTER_GROUP::eGround);
	//
	//pLevel->AddGameObject(pObject, 0);

	//pObject = new CGameObject;
	//pObject->SetName(L"Plane_1");

	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);

	//pObject->Transform()->SetRelativePos(Vec3(0.f, 1000.f, 400.f));
	//pObject->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1.f));
	//pObject->Transform()->SetRelativeRotation(Vec3(-XM_PI / 2.f, 0.f, 0.f));

	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));

	//pLevel->AddGameObject(pObject, 0);

	//pObject = new CGameObject;
	//pObject->SetName(L"Plane_2");

	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);
	//pObject->AddComponent(new CRigidBody);

	//pObject->Transform()->SetRelativePos(Vec3(0.f, 500.f, 900.f));
	//pObject->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1.f));
	//pObject->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));

	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));

	//pObject->RigidBody()->UpdateTransformData(COLLIDER_TYPE::COLLIDER_CUBE, true, true);
	//pObject->RigidBody()->SetColldierScaleSize(true);
	//pObject->RigidBody()->SetRestitution(0.f);
	//pObject->RigidBody()->SetColliderFilter(FILTER_GROUP::eWall);

	////Instantiate(pObject, Vec3(0.f, 0.f, 900.f), 0);
	//pLevel->AddGameObject(pObject, 0);

	//pObject = new CGameObject;
	//pObject->SetName(L"Plane_3");

	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);

	//pObject->Transform()->SetRelativePos(Vec3(500.f, 500.f, 400.f));
	//pObject->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1.f));
	//pObject->Transform()->SetRelativeRotation(Vec3(0.f, XM_PI / 2.f, 0.f));

	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));

	////Instantiate(pObject, Vec3(500.f, 0.f, 400.f), 0);
	//pLevel->AddGameObject(pObject, 0);

	//pObject = new CGameObject;
	//pObject->SetName(L"Plane_4");

	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);

	//pObject->Transform()->SetRelativePos(Vec3(-500.f, 500.f, 400.f));
	//pObject->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1.f));
	//pObject->Transform()->SetRelativeRotation(Vec3(0.f, -XM_PI / 2.f, 0.f));

	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));

	////Instantiate(pObject, Vec3(-500.f, 0.f, 400.f), 0);
	//pLevel->AddGameObject(pObject, 0);

	//pObject = new CGameObject;
	//pObject->SetName(L"Plane_5");
	//
	//pObject->AddComponent(new CTransform);
	//pObject->AddComponent(new CMeshRender);
	//pObject->AddComponent(new CRigidBody);
	//
	//pObject->Transform()->SetRelativePos(Vec3(0.f, -240.f, -500.f));
	//pObject->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1.f));
	//pObject->Transform()->SetRelativeRotation(Vec3(XM_PI / 2.f, 0.f, 0.f));
	//
	//pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	//pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));
	//
	//pObject->RigidBody()->UpdateTransformData(COLLIDER_TYPE::COLLIDER_CUBE, true, true);
	//pObject->RigidBody()->SetColldierScaleSize(true);
	//pObject->RigidBody()->SetStaticFriction(0.f);
	//pObject->RigidBody()->SetDynamicFriction(0.f);
	//pObject->RigidBody()->SetRestitution(0.f);
	//pObject->RigidBody()->SetColliderFilter(FILTER_GROUP::eGround);
	//
	//pLevel->AddGameObject(pObject, 0);

	//// Particle Object
	//CGameObject* pParticle = new CGameObject;
	//pParticle->SetName(L"Particle");
	//pParticle->AddComponent(new CTransform);
	//pParticle->AddComponent(new CParticleSystem);

	//pParticle->Transform()->SetRelativePos(Vec3(0.f, 0.f, 100.f));

	//pParticle->ParticleSystem()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleRenderMtrl"));
	//pParticle->ParticleSystem()->SetCS((CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateShader").Get());
	//pParticle->ParticleSystem()->GetCurMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"Bubbles50px"));

	//pParticle->ParticleSystem()->SetMinMaxLifeTime(Vec2(0.2f, 1.f));
	//pParticle->ParticleSystem()->SetMinMaxSpeed(Vec2(100.f, 300.f));
	//pParticle->ParticleSystem()->Set_SE_Color(Vec4(0.24f, 0.28f, 0.8f, 1.f), Vec4(0.78f, 0.74f, 0.9f, 1.f));
	//pParticle->ParticleSystem()->Set_SE_Scale(Vec4(50.f, 50.f, 1.f, 0.f), Vec4(10.f, 10.f, 1.f, 0.f));
	//pParticle->ParticleSystem()->SetWorldSpawn(true);

	//pParticle->ParticleSystem()->SetFrequency(5.f);
	//pParticle->ParticleSystem()->SetSpawnRange(100.f);

	//pLevel->AddGameObject(pParticle, 0);

	// PostProcess Object
	/*CGameObject* pPostProcess = new CGameObject;
	pPostProcess->SetName(L"PostProcess");

	pPostProcess->AddComponent(new CTransform);
	pPostProcess->AddComponent(new CMeshRender);

	pPostProcess->Transform()->SetRelativePos(Vec3(0.f, 0.f, 100.f));
	pPostProcess->Transform()->SetRelativeScale(Vec3(200.f, 200.f, 1.f));

	pPostProcess->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pPostProcess->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"PostProcessMtrl"));

	pLevel->AddGameObject(pPostProcess, 0);*/


	// �浹 ���̾� ����
	CCollisionMgr::GetInst()->CollisionLayerCheck(0, 1);

	// Level ����
	//pLevel->SetName(L"Test Level2");
	//CSaveLoadMgr::GetInst()->SaveLevel(pLevel);
	//CLevelMgr::GetInst()->RegisterLevel(pLevel->GetRelativePath(), pLevel);

	// ���� ����
	// ���� �׽�Ʈ ������ ���� ������ ��ȯ

}
