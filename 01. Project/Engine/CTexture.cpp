#include "pch.h"
#include "CTexture.h"

#include "CDevice.h"

#include <DirectXTK/DDSTextureLoader.h>


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

//int CTexture::Load(const wstring& _strFilePath)
//{
//	HRESULT hr = E_FAIL;
//
//	wchar_t szExt[50] = {};
//	_wsplitpath_s(_strFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szExt, 50);
//
//	wstring strExt = szExt;
//
//	if (strExt == L".dds" || strExt == L".DDS")
//	{
//		hr = LoadFromDDSFile(_strFilePath.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, nullptr, m_Image);
//	}
//
//	else if (strExt == L".tga" || strExt == L".TGA")
//	{
//		hr = LoadFromTGAFile(_strFilePath.c_str(), nullptr, m_Image);
//	}
//
//	else // WIC (.png, jpg, jpeg, bmp )
//	{
//		hr = LoadFromWICFile(_strFilePath.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, nullptr, m_Image);
//	}
//
//	if (FAILED(hr))
//		return hr;
//
//	// sysmem -> GPU
//	hr = CreateShaderResourceView(DEVICE,
//		m_Image.GetImages(),
//		m_Image.GetImageCount(),
//		m_Image.GetMetadata(),
//		m_SRV.GetAddressOf());
//
//	m_SRV->GetResource((ID3D11Resource**)m_Tex2D.GetAddressOf());
//	m_Tex2D->GetDesc(&m_Desc);
//
//	return hr;
//}

int CTexture::Load(const wstring& _strFilePath)
{
	return Load(_strFilePath, 1);
}

int CTexture::Load(const wstring& _strFilePath, int _iMipLevel)
{
	wchar_t strBuff[50] = {};
	_wsplitpath_s(_strFilePath.c_str(), 0, 0, 0, 0, 0, 0, strBuff, 50);
	wstring strExt = strBuff;

	HRESULT hRet = S_OK;

	if (strExt == L".dds" || strExt == L".DDS")
	{
		// .dds .DDS
		hRet = LoadFromDDSFile(_strFilePath.c_str(), DirectX::DDS_FLAGS_FORCE_RGB, nullptr, m_Image);
	}
	else if (strExt == L".tga" || strExt == L".TGA")
	{
		// .tga .TGA
		hRet = LoadFromTGAFile(_strFilePath.c_str(), nullptr, m_Image);
	}
	else
	{
		// .png .jpg .jpeg .bmp
		hRet = LoadFromWICFile(_strFilePath.c_str(), WIC_FLAGS_IGNORE_SRGB, nullptr, m_Image);
	}

	if (FAILED(hRet))
	{
		wsprintf(strBuff, L"에러코드 : %d", hRet);
		MessageBox(nullptr, strBuff, L"텍스쳐 로딩 실패", MB_OK);
		return hRet;
	}

	if (m_Image.GetMetadata().format == DXGI_FORMAT_BC3_UNORM)
	{
		hRet = CreateDDSTextureFromFile(DEVICE, _strFilePath.c_str(), (ID3D11Resource**)m_Tex2D.GetAddressOf(), m_SRV.GetAddressOf());
		return hRet; 
	}

	// Texture2D 생성
	m_Desc.Format = m_Image.GetMetadata().format;

	if (m_Image.GetMetadata().IsCubemap())
	{
		m_Desc.MipLevels = 1;	// 0 ==> 가능한 모든 밉맵이 저장 될 수 있는 공간이 만들어짐
		m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		m_Desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	}
	else
	{
		m_Desc.MipLevels = _iMipLevel;// MAX_MIP;	// 0 ==> 가능한 모든 밉맵이 저장 될 수 있는 공간이 만들어짐	
		m_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		m_Desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	m_Desc.ArraySize = m_Image.GetMetadata().arraySize;

	m_Desc.SampleDesc.Count = 1;
	m_Desc.SampleDesc.Quality = 0;

	m_Desc.Usage = D3D11_USAGE_DEFAULT;		
	m_Desc.CPUAccessFlags = 0;

	m_Desc.Width = m_Image.GetMetadata().width;
	m_Desc.Height = m_Image.GetMetadata().height;

	HRESULT hr = DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf());

	// 원본데이터(밉맵 레벨 0) 를 각 칸에 옮긴다.	
	for (int i = 0; i < m_Desc.ArraySize; ++i)
	{
		// GPU 에 데이터 옮기기(밉맵 포함)
		UINT iSubresIdx = D3D11CalcSubresource(0, i, m_Desc.MipLevels);

		CONTEXT->UpdateSubresource(m_Tex2D.Get(), iSubresIdx, nullptr
			, m_Image.GetImage(0, i, 0)->pixels
			, m_Image.GetImage(0, i, 0)->rowPitch
			, m_Image.GetImage(0, i, 0)->slicePitch);
	}

	// SRV 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC viewdesc = {};
	viewdesc.Format = m_Desc.Format;

	if (m_Image.GetMetadata().IsCubemap())
	{
		viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	}
	else if (1 < m_Desc.ArraySize)
	{
		viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	}
	else
	{
		viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	}

	if (1 < m_Desc.ArraySize)
	{
		viewdesc.Texture2DArray.ArraySize = m_Desc.ArraySize;
		viewdesc.Texture2DArray.MipLevels = m_Desc.MipLevels;
		viewdesc.Texture2DArray.MostDetailedMip = 0;
	}
	else
	{
		viewdesc.Texture2DArray.ArraySize = 1;
		viewdesc.Texture2D.MipLevels = m_Desc.MipLevels;
		viewdesc.Texture2D.MostDetailedMip = 0;
	}

	DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &viewdesc, m_SRV.GetAddressOf());

	// 밉맵 생성
	if (false == m_Image.GetMetadata().IsCubemap())
	{
		CONTEXT->GenerateMips(m_SRV.Get());
	}

	m_Tex2D->GetDesc(&m_Desc);
}

void CTexture::GenerateMip(UINT _iMipLevel)
{
	m_Tex2D = nullptr;
	m_SRV = nullptr;
	m_RTV = nullptr;
	m_DSV = nullptr;
	m_UAV = nullptr;

	// Texture2D 생성
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Format = m_Image.GetMetadata().format;

	if (m_Image.GetMetadata().IsCubemap())
	{
		tDesc.MipLevels = 1;	// 0 ==> 가능한 모든 밉맵이 저장 될 수 있는 공간이 만들어짐
		tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	}
	else
	{
		tDesc.MipLevels = _iMipLevel;// MAX_MIP;	// 0 ==> 가능한 모든 밉맵이 저장 될 수 있는 공간이 만들어짐	
		tDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	tDesc.ArraySize = m_Image.GetMetadata().arraySize;

	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;

	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.CPUAccessFlags = 0;

	tDesc.Width = m_Image.GetMetadata().width;
	tDesc.Height = m_Image.GetMetadata().height;

	HRESULT hr = DEVICE->CreateTexture2D(&tDesc, nullptr, m_Tex2D.GetAddressOf());

	// 원본데이터(밉맵 레벨 0) 를 각 칸에 옮긴다.	
	for (int i = 0; i < tDesc.ArraySize; ++i)
	{
		// GPU 에 데이터 옮기기(밉맵 포함)
		UINT iSubresIdx = D3D11CalcSubresource(0, i, tDesc.MipLevels);

		CONTEXT->UpdateSubresource(m_Tex2D.Get(), iSubresIdx, nullptr
			, m_Image.GetImage(0, i, 0)->pixels
			, m_Image.GetImage(0, i, 0)->rowPitch
			, m_Image.GetImage(0, i, 0)->slicePitch);
	}

	// Create RTV
	DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());

	// SRV 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC viewdesc = {};
	viewdesc.Format = tDesc.Format;

	if (m_Image.GetMetadata().IsCubemap())
	{
		viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	}
	else if (1 < tDesc.ArraySize)
	{
		viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	}
	else
	{
		viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	}

	if (1 < tDesc.ArraySize)
	{
		viewdesc.Texture2DArray.ArraySize = tDesc.ArraySize;
		viewdesc.Texture2DArray.MipLevels = tDesc.MipLevels;
		viewdesc.Texture2DArray.MostDetailedMip = 0;
	}
	else
	{
		viewdesc.Texture2DArray.ArraySize = 1;
		viewdesc.Texture2D.MipLevels = tDesc.MipLevels;
		viewdesc.Texture2D.MostDetailedMip = 0;
	}

	DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &viewdesc, m_SRV.GetAddressOf());

	// 밉맵 생성
	if (false == m_Image.GetMetadata().IsCubemap())
	{
		CONTEXT->GenerateMips(m_SRV.Get());
	}

	m_Tex2D->GetDesc(&m_Desc);
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

	CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);

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

int CTexture::CreateArrayTexture(const vector<Ptr<CTexture>>& _vecTex, int _iMapLevel)
{
	m_Desc = _vecTex[0]->GetDesc();
	m_Desc.ArraySize = (UINT)_vecTex.size();
	m_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	m_Desc.MipLevels = _iMapLevel;

	HRESULT hr = DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf());
	if (FAILED(hr))
		return hr;

	// 원본데이터(밉맵 레벨 0) 를 각 칸에 옮긴다.	
	for (int i = 0; i < _vecTex.size(); ++i)
	{
		UINT iOffset = D3D11CalcSubresource(0, i, _iMapLevel);
		CONTEXT->UpdateSubresource(m_Tex2D.Get(), iOffset, nullptr, _vecTex[i]->GetSysMem()
			, _vecTex[i]->GetRowPitch(), _vecTex[i]->GetSlicePitch());
	}

	// Shader Resource View 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC viewdesc = {};
	viewdesc.Format = m_Desc.Format;
	viewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewdesc.Texture2DArray.MipLevels = _iMapLevel;
	viewdesc.Texture2DArray.MostDetailedMip = 0;
	viewdesc.Texture2DArray.ArraySize = _vecTex.size();

	DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &viewdesc, m_SRV.GetAddressOf());

	// 밉맵 생성
	CONTEXT->GenerateMips(m_SRV.Get());
	m_Tex2D->GetDesc(&m_Desc);

	return hr;
}

void CTexture::SaveTexture(const wstring& _strRelativePath)
{
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	strFilePath += L"texture\\engine\\" + _strRelativePath + L".png";
	ScratchImage image;
	HRESULT hr = CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), image);
	assert(!FAILED(hr));
	//SaveToDDSFile(*image.GetImages(), DDS_FLAGS_NONE, strFilePath.c_str());
	SaveToWICFile(*image.GetImages(), WIC_FLAGS_NONE, GetWICCodec(WIC_CODEC_PNG), strFilePath.c_str());
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
	CONTEXT->CSSetShaderResources(_iRegisterNum, 1, &pSRV);
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
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->CSSetShaderResources(_iRegisterNum, 1, &pSRV);

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