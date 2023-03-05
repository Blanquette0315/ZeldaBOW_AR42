#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"

CMesh::CMesh(bool _bEngineRes)
	: CRes(RES_TYPE::MESH, _bEngineRes)
	, m_tVBDesc{}
	, m_tIBDesc{}
	, m_iIdxCount(0)
{
}

CMesh::~CMesh()
{
}

void CMesh::render()
{
	UpdateData();
	CONTEXT->DrawIndexed(m_iIdxCount, 0, 0);
}

void CMesh::render_particle(UINT _iCount)
{
	// IA 단계에 Mesh 전달하기
	UpdateData();
	// 인스턴싱을 이용한 Draw 요청
	CONTEXT->DrawIndexedInstanced(m_iIdxCount, _iCount, 0, 0, 0);
}

int CMesh::Create(void* _pVtxSystem, size_t _iVtxCount, void* _pIdxSysmem, size_t _iIdxCount)
{
	HRESULT hr = S_OK;

	// System -> GPU	
	m_tVBDesc.ByteWidth = sizeof(Vtx) * _iVtxCount;
	m_tVBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	m_tVBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tVBDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tSubData = {};
	tSubData.pSysMem = _pVtxSystem;

	hr = DEVICE->CreateBuffer(&m_tVBDesc, &tSubData, &m_VB);

	// IndexBuffer 만들기	
	m_iIdxCount = _iIdxCount;
	m_tIBDesc.ByteWidth = sizeof(UINT) * m_iIdxCount;
	m_tIBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	m_tIBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tIBDesc.CPUAccessFlags = 0;

	tSubData = {};
	tSubData.pSysMem = _pIdxSysmem;

	hr = DEVICE->CreateBuffer(&m_tIBDesc, &tSubData, &m_IB);

	return hr;
}

void CMesh::UpdateData()
{
	UINT iStride = sizeof(Vtx);
	UINT iOffset = 0;
	CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &iStride, &iOffset);
	CONTEXT->IASetIndexBuffer(m_IB.Get(), DXGI_FORMAT_R32_UINT, 0);
}

