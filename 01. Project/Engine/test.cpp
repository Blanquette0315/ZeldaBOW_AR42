#include "pch.h"
#include "test.h"

#include "CKeyMgr.h"
#include "CTimeMgr.h"
#include "CPathMgr.h"
#include "CResMgr.h"

#include "CDevice.h"

#include "CConstBuffer.h"
#include "CMesh.h"
#include "CGrapicsShader.h"

#include "CGameObject.h"
#include "CTransform.h"

Vec3	g_vPos;

void init()
{
	// InputLayout
	// IA 단계에 전달됨
	// 정점 구조 정보
	D3D11_INPUT_ELEMENT_DESC arrLayout[2] = {};
}

void tick()
{
	// 키 입력으로 삼각형 움직이기
	if (KEY_PRESSED(KEY::LEFT))
	{
		g_vPos.x -= DT * 1.f;
	}

	if (KEY_PRESSED(KEY::RIGHT))
	{
		g_vPos.x += DT * 1.f;
	}

	if (KEY_PRESSED(KEY::UP))
	{
		g_vPos.y += DT * 1.f;
	}

	if (KEY_PRESSED(KEY::DOWN))
	{
		g_vPos.y -= DT * 1.f;
	}

	Vec4 vPos = Vec4(g_vPos, 0.f);

	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);
	pCB->SetData(&vPos);
}

void render()
{
	// 필요한 상수버퍼 리소스 바인딩
	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);
	pCB->UpdateData(PIPELINE_STAGE::VS | PIPELINE_STAGE::PS);

	// 사용할 그래픽스 셰이더 설정
	CGrapicsShader* pShader = CResMgr::GetInst()->FindRes<CGrapicsShader>(L"TestShader");
	pShader->UpdateData();

	CMesh* pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh");
	pMesh->render();
}

void release()
{
}
