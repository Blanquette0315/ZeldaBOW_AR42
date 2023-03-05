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

	m_tDesc.ByteWidth = _iElementSize * _iElementCount;	// 버퍼 전체 크기
	m_tDesc.StructureByteStride = _iElementSize;		// 버퍼 요소 하나 크기 (정보 하나하나의 간격)
	m_tDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;			// 구조화 버퍼 추가 플래그 (구조화 버퍼 전용 플래그)
	m_tDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tDesc.CPUAccessFlags = 0;

	if (SB_TYPE::SRV_ONLY == m_eType)
	{
		m_tDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;	// Texture Register Binding : 텍스처 레지스터에 바인딩을 걸기 위해 만들기 때문에 바인드 플래그는 텍스처이다.
	}

	// UnorderedAccess Flag가 있으면 CPU읽기 옵션이 불가능하다.(이건 정해져 있다.)
	// UnorderedAccess Flag가 있으면 Usage는 반듯이 디폴트로 해주어야 한다. 그렇지 않을 경우 버퍼 생성부분에서 오류가 발생함.
	// 11.14 변경. SRV, UAV모두 usage와 CPUAccessFlag를 디폴트로 바꾸고, 쓰기 읽기가 필요한 경우 따로 만들어준다.
	else if (SB_TYPE::UAV_INC == m_eType)
	{
		m_tDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	}
	

	// 초기화 정보가 없다면 그냥 만들고, 있다면 그 데이터를 기반으로 초기화를 해준 뒤 버퍼를 생성해준다.
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

	// 버퍼 생성 실패
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

	// 쓰기, 읽기 전용 버퍼
	if (_CPUAccess)
	{
		m_tDesc.ByteWidth = _iElementSize * _iElementCount;
		m_tDesc.StructureByteStride = _iElementSize;
		m_tDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		m_tDesc.Usage = D3D11_USAGE_DYNAMIC;				// 사용 용도
															// : 구조화 버퍼는 정보를 레지스터에 바인딩 걸어 바뀌는 값들에 대한 Shader에서 처리를 해주기 위해 만든다.
		m_tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU접근
															// 해당 CPUAccess Flag는 읽기와 쓰기를 혼용해 사용할 수 없다.
															// 때문에 버퍼를 2개로 나누어 만든 것이다.

		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, nullptr, m_WriteBuffer.GetAddressOf())))
		{
			assert(nullptr);
			return E_FAIL;
		}

		m_tDesc.Usage = D3D11_USAGE_DEFAULT; // 원래 Read는 스테이징이랑 짝꿍인데, 모종의 이유때문에 디폴트를 사용.
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
	// 공간이 모자라면 추가할당 하면서 초기화한다.
	if (m_iElementCount < _iElementCount)
	{
		Create(m_iElementSize, _iElementCount, m_eType, _pSrc);
	}
	// 공간이 충분하다면, 데이터 전송
	else
	{
		// SystemMem -> 쓰기 버퍼
		D3D11_MAPPED_SUBRESOURCE temMapSub = {};

		CONTEXT->Map(m_WriteBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &temMapSub);
		memcpy(temMapSub.pData, _pSrc, m_iElementSize * _iElementCount);
		CONTEXT->Unmap(m_WriteBuffer.Get(), 0);

		// 메인 버퍼를 수정하기 위해 쓰기버퍼에 데이터를 수정한 뒤, 메인버퍼로 옮겨준다.
		// 즉, GPU메모리 내에서의 데이터 전달. (GPU 메모리 간 데이터 복사는 빠르다.)
		// 쓰기 버퍼 -> 메인 버퍼
		CONTEXT->CopyResource(m_SB.Get(), m_WriteBuffer.Get());
	}
}

void CStructuredBuffer::GetData(void* _pDst, UINT _iSizeByte)
{
	// _iSizeByte : 요구하는 데이터 사이즈가 0이 들어오면 버퍼 전체의 데이터를 읽어온다.
	// 만약 메인 구조화 버퍼에 5000의 사이즈 만큼 데이터가 있는데, 6000을 달라고하는게 말이 안되기 때문에 예외처리를 해주었다.
	assert(!(_iSizeByte != 0 && _iSizeByte < m_iElementSize* m_iElementCount));

	// 데이터를 가져오는 방식은 메인버퍼의 내용을 읽기 버퍼로 옮긴 뒤, 읽기 버퍼의 내용을 CPU의 SystemMemory로 옮겨온다.
	// 메인 버퍼 -> 읽기 버퍼
	CONTEXT->CopyResource(m_ReadBuffer.Get(), m_SB.Get());

	// 읽기 버퍼 -> SystemMem
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
	// t레지스터에 바인딩 걸 경우
	if (_bShaderRes)
	{
		CONTEXT->CSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
	// u레지스터에 바인딩 할 경우
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

	// u레지스터는 0~7번까지만 사용이 가능하기 때문에 t레지스터와 동일하게 최근 번호를 사용하면, 문제가 된다.
	// t레지스터는 15번도 사용이 가능하기 때문에
	CONTEXT->CSSetShaderResources(m_iRecentRegisterNumRW, 1, &pSRV);
	ID3D11UnorderedAccessView* pUAV = nullptr;
	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(m_iRecentRegisterNumRW, 1, &pUAV, &i);
}
