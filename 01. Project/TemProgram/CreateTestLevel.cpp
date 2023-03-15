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

#include "CCameraScript.h"

#include "CSaveLoadMgr.h"

void CreateTestLevel()
{
//#ifdef _DEBUG
//	CLevel* Level = CSaveLoadMgr::GetInst()->LoadLevel(L"level\\Test Level2.lv");
//	
//	ChangeLevel(Level);
//	return;
//#else
//	CLevel* Level = CSaveLoadMgr::GetInst()->LoadLevel(L"level\\MainLevel.lv");
//	CLevelMgr::GetInst()->RegisterLevel(Level->GetRelativePath(), Level);
//	ChangeLevelAndPlay(Level);
//	return;
//#endif

	CLevel* pLevel = new CLevel;

	// Layer 이름 설정
	pLevel->GetLayer(1)->SetName(L"Player");
	pLevel->GetLayer(2)->SetName(L"PlayerProjectile");
	pLevel->GetLayer(3)->SetName(L"Monster");
	pLevel->GetLayer(4)->SetName(L"MonsterProjectile");

	// Camera Object 추가
	CGameObject* pCamObj = new CGameObject;
	pCamObj->SetName(L"MainCamera");

	pCamObj->AddComponent(new CTransform);
	pCamObj->AddComponent(new CCamera);

	pCamObj->Camera()->SetLayerMaskAll();
	pCamObj->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);

	pLevel->AddGameObject(pCamObj, 0);

	// Directional Light 추가
	// 보통 방향성 광원은 낮일때는 0.7, 밤에는 0.2정도를 두고 사용한다.
	// 광원은 그려질 필요가 없기 때문에 CRenderMesh는 추가하지 않는다.
	// Directional Light는 방향 정보가 필요 없기 때문에 넣지 않았다.
	CGameObject* pDirLight = new CGameObject;
	pDirLight->SetName(L"DirectionalLight");

	pDirLight->AddComponent(new CTransform);
	pDirLight->AddComponent(new CLight3D);

	pDirLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pDirLight->Light3D()->SetLightSpecular(Vec3(0.4f, 0.4f, 0.4f));
	pDirLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));
	pDirLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);

	pLevel->AddGameObject(pDirLight, 0);


	// Point Light 추가
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

	// Spot Light 추가
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

	// SkyBox 추가
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

	pSkyBox->SetFrustumCul(true);

	pLevel->AddGameObject(pSkyBox, 0);

	// GameObject 초기화
	CGameObject* pObject = nullptr;

	pObject = new CGameObject;
	pObject->SetName(L"Player");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CRigidBody);
	pObject->AddComponent(new CPlayerScript);
	//Sphere
	pObject->Transform()->SetRelativePos(Vec3(0.f, 500.f, 400.f));
	pObject->Transform()->SetRelativeScale(Vec3(256.f, 256.f, 256.f));
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));
	pObject->MeshRender()->GetCurMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	pObject->MeshRender()->GetCurMaterial()->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));

	pObject->RigidBody()->UpdateTransformData(RIGIDCOLLIDER_TYPE::SPHERE, false, true);
	pObject->RigidBody()->SetMass(10.f);
	pObject->RigidBody()->SetRestitution(0.f);
	//pObject->RigidBody()->SetStaticFriction(0.f);
	//pObject->RigidBody()->SetDynamicFriction(0.f);
	pObject->RigidBody()->SetLockAxis_Rot(true, true, true);
	pObject->RigidBody()->SetGravityOption(true);
	//pObject->RigidBody()->CreateActor();

	pLevel->AddGameObject(pObject, 0);

	// 데칼 생성
	CGameObject* pDecal = new CGameObject;
	pDecal->SetName(L"Decal");

	pDecal->AddComponent(new CTransform);
	pDecal->AddComponent(new CDecal);

	pDecal->Transform()->SetRelativePos(0.f, 500.f, 0.f);
	pDecal->Transform()->SetRelativeScale(250.f, 250.f, 250.f);
	
	pDecal->Decal()->SetRenderType(true);
	pDecal->Decal()->SetDecalTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\MagicCircle.png"));

	pLevel->AddGameObject(pDecal, 0);

	// 벽 생성
	pObject = new CGameObject;
	pObject->SetName(L"Plane_0");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CRigidBody);

	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 400.f));
	pObject->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1.f));
	pObject->Transform()->SetRelativeRotation(Vec3(XM_PI / 2.f, 0.f, 0.f));
	
	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));

	pObject->RigidBody()->UpdateTransformData(RIGIDCOLLIDER_TYPE::CUBE, true);
	pObject->RigidBody()->SetLockAxis_Pos(true, true, true);
	pObject->RigidBody()->SetStaticFriction(0.f);
	pObject->RigidBody()->SetDynamicFriction(0.f);
	pObject->RigidBody()->SetRestitution(0.f);
	pObject->RigidBody()->SetColliderFilter(FILTER_GROUP::eGround);
	//pObject->RigidBody()->CreateActor();

	pLevel->AddGameObject(pObject, 0);

	pObject = new CGameObject;
	pObject->SetName(L"Plane_1");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->Transform()->SetRelativePos(Vec3(0.f, 1000.f, 400.f));
	pObject->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1.f));
	pObject->Transform()->SetRelativeRotation(Vec3(-XM_PI / 2.f, 0.f, 0.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));


	//Instantiate(pObject, Vec3(0.f, 500.f, 400.f), 0);
	pLevel->AddGameObject(pObject, 0);

	pObject = new CGameObject;
	pObject->SetName(L"Plane_2");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CRigidBody);

	pObject->Transform()->SetRelativePos(Vec3(0.f, 500.f, 900.f));
	pObject->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1.f));
	pObject->Transform()->SetRelativeRotation(Vec3(0.f, 0.f, 0.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));

	pObject->RigidBody()->UpdateTransformData(RIGIDCOLLIDER_TYPE::CUBE, true);
	pObject->RigidBody()->SetLockAxis_Pos(true, true, true);
	pObject->RigidBody()->SetRestitution(0.f);
	//pObject->RigidBody()->CreateActor();

	//Instantiate(pObject, Vec3(0.f, 0.f, 900.f), 0);
	pLevel->AddGameObject(pObject, 0);

	pObject = new CGameObject;
	pObject->SetName(L"Plane_3");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->Transform()->SetRelativePos(Vec3(500.f, 500.f, 400.f));
	pObject->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1.f));
	pObject->Transform()->SetRelativeRotation(Vec3(0.f, XM_PI / 2.f, 0.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));

	//Instantiate(pObject, Vec3(500.f, 0.f, 400.f), 0);
	pLevel->AddGameObject(pObject, 0);

	pObject = new CGameObject;
	pObject->SetName(L"Plane_4");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);

	pObject->Transform()->SetRelativePos(Vec3(-500.f, 500.f, 400.f));
	pObject->Transform()->SetRelativeScale(Vec3(1000.f, 1000.f, 1.f));
	pObject->Transform()->SetRelativeRotation(Vec3(0.f, -XM_PI / 2.f, 0.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3D_DeferredMtrl"));

	//Instantiate(pObject, Vec3(-500.f, 0.f, 400.f), 0);
	pLevel->AddGameObject(pObject, 0);

	// Particle Object
	CGameObject* pParticle = new CGameObject;
	pParticle->SetName(L"Particle");
	pParticle->AddComponent(new CTransform);
	pParticle->AddComponent(new CParticleSystem);
	
	pParticle->Transform()->SetRelativePos(Vec3(0.f, 0.f, 100.f));
	
	pParticle->ParticleSystem()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"ParticleRenderMtrl"));
	pParticle->ParticleSystem()->SetCS((CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateShader").Get());
	pParticle->ParticleSystem()->GetCurMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"Bubbles50px"));

	pParticle->ParticleSystem()->SetMinMaxLifeTime(Vec2(0.2f, 1.f));
	pParticle->ParticleSystem()->SetMinMaxSpeed(Vec2(100.f, 300.f));
	pParticle->ParticleSystem()->Set_SE_Color(Vec4(0.24f, 0.28f, 0.8f, 1.f), Vec4(0.78f, 0.74f, 0.9f, 1.f));
	pParticle->ParticleSystem()->Set_SE_Scale(Vec4(50.f, 50.f, 1.f, 0.f), Vec4(10.f, 10.f, 1.f, 0.f));
	pParticle->ParticleSystem()->SetWorldSpawn(true);
	
	pParticle->ParticleSystem()->SetFrequency(5.f);
	pParticle->ParticleSystem()->SetSpawnRange(100.f);
	
	pLevel->AddGameObject(pParticle, 0);

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


	// 충돌 레이어 지정
	CCollisionMgr::GetInst()->CollisionLayerCheck(0, 1);

	// Level 저장
	//pLevel->SetName(L"Test Level2");
	//CSaveLoadMgr::GetInst()->SaveLevel(pLevel);
	//CLevelMgr::GetInst()->RegisterLevel(pLevel->GetRelativePath(), pLevel);

	// 레벨 설정
	// 만든 테스트 레벨을 지금 레벨로 전환
	ChangeLevel(pLevel);
}
