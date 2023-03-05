#include "pch.h"
#include "CTileMap.h"

#include "CResMgr.h"
#include "CTransform.h"
#include "CStructuredBuffer.h"

CTileMap::CTileMap()
	: CRenderComponent(COMPONENT_TYPE::TILEMAP)
	, m_bDataChanged(false)
{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"TileMapMtrl"));

	m_TileBuffer = new CStructuredBuffer;
}

CTileMap::~CTileMap()
{
	if (nullptr != m_TileBuffer)
		delete m_TileBuffer;
}

void CTileMap::SetTileCount(UINT _iWidth, UINT _iHeight)
{
	m_vTileCount = Vec2((float)_iWidth, (float)_iHeight);

	m_vecTile.clear();
	m_vecTile.resize(_iWidth * _iHeight);

	if (m_TileBuffer->GetElementCount() < _iWidth * _iHeight)
	{
		m_TileBuffer->Create(sizeof(tTile), _iWidth * _iHeight, SB_TYPE::SRV_ONLY, nullptr, true);
	}
	Initialize();

	m_bDataChanged = true;
}

const wstring& CTileMap::GetTextureName()
{
	if (nullptr != m_AtlasTex)
	{
		return m_AtlasTex.Get()->GetKey();
	}
}

void CTileMap::finaltick()
{
}

void CTileMap::render()
{
	Transform()->UpdateData();

	if (m_bDataChanged)
	{
		m_TileBuffer->SetData(m_vecTile.data(), (UINT)(m_vTileCount.x * m_vTileCount.y));
	}
	
	m_TileBuffer->UpdateData(17, PIPELINE_STAGE::PS);

	GetCurMaterial()->SetScalarParam(VEC2_0, &m_vTileCount);
	GetCurMaterial()->SetTexParam(TEX_0, m_AtlasTex);
	GetCurMaterial()->UpdateData();

	GetMesh()->render();
}

void CTileMap::Initialize()
{
	Vec2 vLeftTop = Vec2(0.f, 0.f);
	Vec2 vSlice = Vec2(64.f, 64.f);

	vLeftTop /= Vec2(m_TexWH.x, m_TexWH.y);
	vSlice /= Vec2(m_TexWH.x, m_TexWH.y);

	for (size_t i = 0; i < m_vecTile.size(); ++i)
	{
		m_vecTile[i].vLeftTop = vLeftTop;
		m_vecTile[i].vSlice = vSlice;
		int a = 0;
	}
}

void CTileMap::SetTileAtlas(Ptr<CTexture> _AtlasTex)
{
	m_AtlasTex = _AtlasTex;

	m_TexWH.x = m_AtlasTex->GetWidth();
	m_TexWH.y = m_AtlasTex->GetHeight();
}



void CTileMap::SaveToFile(FILE* _File)
{
	CRenderComponent::SaveToFile(_File);

	SaveResourceRef<CTexture>(m_AtlasTex, _File);

	fwrite(&m_TexWH, sizeof(Vec2), 1, _File);
	fwrite(&m_vTileCount, sizeof(Vec2), 1, _File);
	fwrite(m_vecTile.data(), sizeof(tTile), m_vecTile.size(), _File);
}

void CTileMap::LoadFromFile(FILE* _File)
{
	CRenderComponent::LoadFromFile(_File);

	LoadResourceRef<CTexture>(m_AtlasTex, _File);
	fread(&m_TexWH, sizeof(Vec2), 1, _File);
	fread(&m_vTileCount, sizeof(Vec2), 1, _File);

	// vector의 reserve와 resize의 차이
	// reserve는 단지 수용량만 늘려주는 것이다. (캐퍼시티를 할당 받아둔다.)
	// resize는 실제로 데이터가 들어가는 index 공간을 동적할당 받는다.
	// 좀 더 자세히 정리할 필요가 있다. (면접때 질문이 잘 들어옴)
	// 구조를 보니 reserve를 안해도 되어서 변경 (공부 목적으로 주석은 남겨둠)
	// 타일의 수가 늘어나면 TileMap에서 자체적으로 resize를 해주기 때문에 문제가 없다.
	size_t iTileCount = (size_t)(m_vTileCount.x * m_vTileCount.y);
	SetTileCount((UINT)m_vTileCount.x, (UINT)m_vTileCount.y);
	// 파일로부터 타일 정보를 읽어서 벡터에 기록
	for (size_t i = 0; i < iTileCount; ++i)
	{
		tTile tile = {};
		fread(&tile, sizeof(tTile), 1, _File);
		m_vecTile[i] = tile;
	}

	// 타일정보가 1개 이상 있으면, 데이터 변경점 체크 --> render 에서 데이터를 구조화버퍼로 이동
	if (0 < m_vecTile.size())
		m_bDataChanged = true;
}