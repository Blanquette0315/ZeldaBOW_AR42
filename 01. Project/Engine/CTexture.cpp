#include "pch.h"
#include "CTexture.h"

#include "CDevice.h"

CTexture::CTexture(bool _bEngineRes)
	: CRes(RES_TYPE::TEXTURE, _bEngineRes)
	, m_Desc{}
{
}

CTexture::~CTexture()
{
}


void CTexture::Save(const wstring& _strRelativePath)
{
	// CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);
	// SaveToWICFile(m_Image
}

int CTexture::Load(const wstring& _strFilePath)
{
	HRESULT hr = E_FAIL;

	wchar_t szExt[50] = {};
	_wsplitpath_s(_strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, 50);

	wstring strExt = szExt;

	if (strExt == L".dds" || strExt == L".DDS")
	{
		hr = LoadFromDDSFile(_strFilePath.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, nullptr, m_Image);
	}

	else if (strExt == L".tga" || strExt == L".TGA")
	{
		hr = LoadFromTGAFile(_strFilePath.c_str(), nullptr, m_Image);
	}

	else // WIC (.png, jpg, jpeg, bmp )
	{
		hr = LoadFromWICFile(_strFilePath.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, nullptr, m_Image);
	}

	if (FAILED(hr))
		return hr;

	// sysmem -> GPU
	hr = CreateShaderResourceView(DEVICE,
		m_Image.GetImages(),
		m_Image.GetImageCount(),
		m_Image.GetMetadata(),
		m_SRV.GetAddressOf());

	m_SRV->GetResource((ID3D11Resource**)m_Tex2D.GetAddressOf());
	m_Tex2D->GetDesc(&m_Desc);

	return hr;
}

void CTexture::Create(UINT _iWidth, UINT _iHeight, DXGI_FORMAT _Format, UINT _iBindFlag)
{
	m_Desc.BindFlags = _iBindFlag;

	m_Desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	m_Desc.CPUAccessFlags = 0;

	m_Desc.Format = _Format;
	m_Desc.Width = _iWidth;
	m_Desc.Height = _iHeight;
	m_Desc.ArraySize = 1;

	m_Desc.SampleDesc.Count = 1;
	m_Desc.SampleDesc.Quality = 0;

	m_Desc.MipLevels = 1;
	m_Desc.MiscFlags = 0;

	HRESULT hr = S_OK;
	hr = DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf());
	// 바인드 플레그에 따른 View 제작
	// DS의 BindFlag는 다른 플래그와 혼용해 사용할 수 없다. 따라서 우리도 분기를 아에 나눠주었다.
	// BindFlag를 혼용해 Texture2D를 생성하려해도 생성이 실패된다.
	if (_iBindFlag & D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL)
	{
		hr = DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
		assert(!FAILED(hr));
	}
	else
	{
		if (_iBindFlag & D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET)
		{
			hr = DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());
			assert(!FAILED(hr));
		}

		if (_iBindFlag & D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE)
		{
			// SRV 생성을 위한 Desc 채우기
			D3D11_SHADER_RESOURCE_VIEW_DESC temDesc = {};
			temDesc.Format = m_Desc.Format;
			temDesc.Texture2D.MipLevels = 1;
			temDesc.Texture2D.MostDetailedMip = 0;
			temDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;

			hr = DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &temDesc, m_SRV.GetAddressOf());
			assert(!FAILED(hr));
		}

		if (_iBindFlag & D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS)
		{
			// UAV 생성을 위한 Desc 채우기
			D3D11_UNORDERED_ACCESS_VIEW_DESC temDesc = {};
			temDesc.Format = m_Desc.Format;
			temDesc.Texture2D.MipSlice = 0;
			temDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;

			hr = DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), &temDesc, m_UAV.GetAddressOf());
			assert(!FAILED(hr));
		}
	}
}

void CTexture::Create(ComPtr<ID3D11Texture2D> _Tex2D)
{
	m_Tex2D = _Tex2D;
	m_Tex2D->GetDesc(&m_Desc);
	
	HRESULT hr = S_OK;

	// View 생성
	if (m_Desc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL)
	{
		hr = DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
		assert(!FAILED(hr));
	}
	else
	{
		if (m_Desc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET)
		{
			hr = DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());
			assert(!FAILED(hr));
		}

		if (m_Desc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE)
		{
			// SRV 생성을 위한 Desc 채우기
			D3D11_SHADER_RESOURCE_VIEW_DESC temDesc = {};
			temDesc.Format = m_Desc.Format;
			temDesc.Texture2D.MipLevels = 1;
			temDesc.Texture2D.MostDetailedMip = 0;
			temDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;

			hr = DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &temDesc, m_SRV.GetAddressOf());
			assert(!FAILED(hr));
		}

		if (m_Desc.BindFlags & D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS)
		{
			// UAV 생성을 위한 Desc 채우기
			D3D11_UNORDERED_ACCESS_VIEW_DESC temDesc = {};
			temDesc.Format = m_Desc.Format;
			temDesc.Texture2D.MipSlice = 0;
			temDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;

			hr = DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), &temDesc, m_UAV.GetAddressOf());
			assert(!FAILED(hr));
		}
	}
}

void CTexture::UpdateData(UINT _iRegisterNum, UINT _iPipelineStage)
{
	if ((UINT)PIPELINE_STAGE::VS & _iPipelineStage)
		CONTEXT->VSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::HS & _iPipelineStage)
		CONTEXT->HSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::DS & _iPipelineStage)
		CONTEXT->DSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::GS & _iPipelineStage)
		CONTEXT->GSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());

	if ((UINT)PIPELINE_STAGE::PS & _iPipelineStage)
		CONTEXT->PSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
}

void CTexture::Clear(UINT _iRegisterNum)
{
	ID3D11ShaderResourceView* pSRV = nullptr;

	CONTEXT->VSSetShaderResources(_iRegisterNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(_iRegisterNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(_iRegisterNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(_iRegisterNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(_iRegisterNum, 1, &pSRV);
}

void CTexture::UpdateData_CS(UINT _iRegisterNum, bool _bShaderRes)
{
	if (_bShaderRes)
	{
		CONTEXT->CSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
	else
	{
		assert(m_UAV.Get());

		// 초기값은 디폴트로 -1을 넣은다.
		UINT i = -1;
		CONTEXT->CSSetUnorderedAccessViews(_iRegisterNum, 1, m_UAV.GetAddressOf(), &i);
	}
}

void CTexture::Clear_CS(UINT _iRegisterNum)
{
	ID3D11UnorderedAccessView* pUAV = nullptr;
	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(_iRegisterNum, 1, &pUAV, &i);
}


void CTexture::GetPixelVector(vector<vector<tBGRA>>& _inVec)
{
	uint8_t* pixelMem = m_Image.GetPixels();
	const Image* image = m_Image.GetImages();

	for (int width = 0; width < image->width; ++width)
	{
		static vector<tBGRA>vecTmp;
		static tBGRA tmp = {};
		for (int height = 0; height < image->height; ++height)
		{
			tmp = {};

			tmp.b = (pixelMem[0 + (height * image->width + width) * 4]);
			tmp.g = (pixelMem[1 + (height * image->width + width) * 4]);
			tmp.r = (pixelMem[2 + (height * image->width + width) * 4]);
			tmp.a = (pixelMem[3 + (height * image->width + width) * 4]);

			vecTmp.push_back(tmp);
		}
		_inVec.push_back(vecTmp);
		vecTmp.clear();
	}
}