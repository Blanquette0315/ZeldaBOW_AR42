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
	// IA �ܰ迡 ���޵�
	// ���� ���� ����
	D3D11_INPUT_ELEMENT_DESC arrLayout[2] = {};
}

void tick()
{
	// Ű �Է����� �ﰢ�� �����̱�
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
	// �ʿ��� ������� ���ҽ� ���ε�
	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);
	pCB->UpdateData(PIPELINE_STAGE::VS | PIPELINE_STAGE::PS);

	// ����� �׷��Ƚ� ���̴� ����
	CGrapicsShader* pShader = CResMgr::GetInst()->FindRes<CGrapicsShader>(L"TestShader");
	pShader->UpdateData();

	CMesh* pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh");
	pMesh->render();
}

void release()
{
}
