#include "pch.h"
#include "CStructuredBuffer.h"

#include "CDevice.h"

CStructuredBuffer::CStructuredBuffer()
	: m_eType(SB_TYPE::SRV_ONLY)
	, m_tDesc{}
	, m_iElementCount(0)
	, m_iElementSize(0)
	, m_iRecentRegisterNum(0)
	, m_iRecentRegisterNumRW(0)
{
}

CStructuredBuffer::~CStructuredBuffer()
{
}

int CStructuredBuffer::Create(UINT _iElementSize, UINT _iElementCount, SB_TYPE _eType, void* _pInitial, bool _CPUAccess)
{
	HRESULT hr = S_OK;

	m_SB = nullptr;
	m_SRV = nullptr;
	m_UAV = nullptr;
	m_WriteBuffer = nullptr;
	m_ReadBuffer = nullptr;

	m_eType = _eType;
	m_iElementSize = _iElementSize;
	m_iElementCount = _iElementCount;

	m_tDesc.ByteWidth = _iElementSize * _iElementCount;	// ���� ��ü ũ��
	m_tDesc.StructureByteStride = _iElementSize;		// ���� ��� �ϳ� ũ�� (���� �ϳ��ϳ��� ����)
	m_tDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;			// ����ȭ ���� �߰� �÷��� (����ȭ ���� ���� �÷���)
	m_tDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tDesc.CPUAccessFlags = 0;

	if (SB_TYPE::SRV_ONLY == m_eType)
	{
		m_tDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;	// Texture Register Binding : �ؽ�ó �������Ϳ� ���ε��� �ɱ� ���� ����� ������ ���ε� �÷��״� �ؽ�ó�̴�.
	}

	// UnorderedAccess Flag�� ������ CPU�б� �ɼ��� �Ұ����ϴ�.(�̰� ������ �ִ�.)
	// UnorderedAccess Flag�� ������ Usage�� �ݵ��� ����Ʈ�� ���־�� �Ѵ�. �׷��� ���� ��� ���� �����κп��� ������ �߻���.
	// 11.14 ����. SRV, UAV��� usage�� CPUAccessFlag�� ����Ʈ�� �ٲٰ�, ���� �бⰡ �ʿ��� ��� ���� ������ش�.
	else if (SB_TYPE::UAV_INC == m_eType)
	{
		m_tDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	}
	

	// �ʱ�ȭ ������ ���ٸ� �׳� �����, �ִٸ� �� �����͸� ������� �ʱ�ȭ�� ���� �� ���۸� �������ش�.
	if (nullptr == _pInitial)
	{
		hr = DEVICE->CreateBuffer(&m_tDesc, nullptr, m_SB.GetAddressOf());
	}
	else
	{
		D3D11_SUBRESOURCE_DATA temData = {};
		temData.pSysMem = _pInitial;

		hr = DEVICE->CreateBuffer(&m_tDesc, &temData, m_SB.GetAddressOf());
	}

	// ���� ���� ����
	if (FAILED(hr))
		return E_FAIL;

	// Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC temViewDesc = {};
	temViewDesc.BufferEx.NumElements = _iElementCount;
	temViewDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFEREX;

	hr = DEVICE->CreateShaderResourceView(m_SB.Get(), &temViewDesc, m_SRV.GetAddressOf());
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	if (SB_TYPE::UAV_INC == m_eType)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC temUAVDesc = {};

		temUAVDesc.Buffer.NumElements = _iElementCount;
		temUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

		if (FAILED(DEVICE->CreateUnorderedAccessView(m_SB.Get(), &temUAVDesc, m_UAV.GetAddressOf())))
		{
			return E_FAIL;
		}
	}

	// ����, �б� ���� ����
	if (_CPUAccess)
	{
		m_tDesc.ByteWidth = _iElementSize * _iElementCount;
		m_tDesc.StructureByteStride = _iElementSize;
		m_tDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		m_tDesc.Usage = D3D11_USAGE_DYNAMIC;				// ��� �뵵
															// : ����ȭ ���۴� ������ �������Ϳ� ���ε� �ɾ� �ٲ�� ���鿡 ���� Shader���� ó���� ���ֱ� ���� �����.
		m_tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU����
															// �ش� CPUAccess Flag�� �б�� ���⸦ ȥ���� ����� �� ����.
															// ������ ���۸� 2���� ������ ���� ���̴�.

		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, nullptr, m_WriteBuffer.GetAddressOf())))
		{
			assert(nullptr);
			return E_FAIL;
		}

		m_tDesc.Usage = D3D11_USAGE_DEFAULT; // ���� Read�� ������¡�̶� ¦���ε�, ������ ���������� ����Ʈ�� ���.
		m_tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, nullptr, m_ReadBuffer.GetAddressOf())))
		{
			assert(nullptr);
			return E_FAIL;
		}
	}

	return hr;
}

void CStructuredBuffer::SetData(void* _pSrc, UINT _iElementCount)
{
	// ������ ���ڶ�� �߰��Ҵ� �ϸ鼭 �ʱ�ȭ�Ѵ�.
	if (m_iElementCount < _iElementCount)
	{
		Create(m_iElementSize, _iElementCount, m_eType, _pSrc);
	}
	// ������ ����ϴٸ�, ������ ����
	else
	{
		// SystemMem -> ���� ����
		D3D11_MAPPED_SUBRESOURCE temMapSub = {};

		CONTEXT->Map(m_WriteBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &temMapSub);
		memcpy(temMapSub.pData, _pSrc, m_iElementSize * _iElementCount);
		CONTEXT->Unmap(m_WriteBuffer.Get(), 0);

		// ���� ���۸� �����ϱ� ���� ������ۿ� �����͸� ������ ��, ���ι��۷� �Ű��ش�.
		// ��, GPU�޸� �������� ������ ����. (GPU �޸� �� ������ ����� ������.)
		// ���� ���� -> ���� ����
		CONTEXT->CopyResource(m_SB.Get(), m_WriteBuffer.Get());
	}
}

void CStructuredBuffer::GetData(void* _pDst, UINT _iSizeByte)
{
	// _iSizeByte : �䱸�ϴ� ������ ����� 0�� ������ ���� ��ü�� �����͸� �о�´�.
	// ���� ���� ����ȭ ���ۿ� 5000�� ������ ��ŭ �����Ͱ� �ִµ�, 6000�� �޶���ϴ°� ���� �ȵǱ� ������ ����ó���� ���־���.
	assert(!(_iSizeByte != 0 && _iSizeByte < m_iElementSize* m_iElementCount));

	// �����͸� �������� ����� ���ι����� ������ �б� ���۷� �ű� ��, �б� ������ ������ CPU�� SystemMemory�� �Űܿ´�.
	// ���� ���� -> �б� ����
	CONTEXT->CopyResource(m_ReadBuffer.Get(), m_SB.Get());

	// �б� ���� -> SystemMem
	D3D11_MAPPED_SUBRESOURCE tMapSub = {};

	CONTEXT->Map(m_ReadBuffer.Get(), 0, D3D11_MAP_READ, 0, &tMapSub);

	if (0 == _iSizeByte)
	{
		memcpy(_pDst, tMapSub.pData, m_iElementSize * m_iElementCount);
	}
	else
	{
		memcpy(_pDst, tMapSub.pData, _iSizeByte);
	}

	CONTEXT->Unmap(m_ReadBuffer.Get(), 0);
}

void CStructuredBuffer::UpdateData(UINT _iRegisterNum, UINT _PipelineStage)
{
	m_iRecentRegisterNum = _iRegisterNum;

	if ((UINT)PIPELINE_STAGE::VS & _PipelineStage)
		CONTEXT->VSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::HS & _PipelineStage)
		CONTEXT->HSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::DS & _PipelineStage)
		CONTEXT->DSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::GS & _PipelineStage)
		CONTEXT->GSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::PS & _PipelineStage)
		CONTEXT->PSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
}

void CStructuredBuffer::UpdateData_CS(UINT _iRegisterNum, bool _bShaderRes)
{
	m_iRecentRegisterNumRW = _iRegisterNum;
	// t�������Ϳ� ���ε� �� ���
	if (_bShaderRes)
	{
		CONTEXT->CSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
	// u�������Ϳ� ���ε� �� ���
	else
	{
		UINT i = -1;
		CONTEXT->CSSetUnorderedAccessViews(_iRegisterNum, 1, m_UAV.GetAddressOf(), &i);
	}
}

void CStructuredBuffer::Clear()
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->VSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);

	// u�������ʹ� 0~7�������� ����� �����ϱ� ������ t�������Ϳ� �����ϰ� �ֱ� ��ȣ�� ����ϸ�, ������ �ȴ�.
	// t�������ʹ� 15���� ����� �����ϱ� ������
	CONTEXT->CSSetShaderResources(m_iRecentRegisterNumRW, 1, &pSRV);
	ID3D11UnorderedAccessView* pUAV = nullptr;
	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(m_iRecentRegisterNumRW, 1, &pUAV, &i);
}
