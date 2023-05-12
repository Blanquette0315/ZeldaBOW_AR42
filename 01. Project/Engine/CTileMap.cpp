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

void CTileMap::render(UINT _iSubset)
{
	render();
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

	m_TexWH.x = (float)m_AtlasTex->GetWidth();
	m_TexWH.y = (float)m_AtlasTex->GetHeight();
}

void CTileMap::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "TILEMAP";
	_emitter << YAML::Value << YAML::BeginMap;

	CRenderComponent::SaveToYAML(_emitter);
	_emitter << YAML::Key << "TileMapTexture";
	_emitter << YAML::Value << YAML::BeginMap;
	SaveResourceRef<CTexture>(m_AtlasTex, _emitter);
	_emitter << YAML::EndMap;
	_emitter << YAML::Key << "TexWH";
	_emitter << YAML::Value << m_TexWH;
	_emitter << YAML::Key << "TileCount";
	_emitter << YAML::Value << m_vTileCount;
	_emitter << YAML::Key << "vecTile";
	_emitter << YAML::Value << m_vecTile;

	_emitter << YAML::EndMap;
}

void CTileMap::LoadFromYAML(YAML::Node& _node)
{
	YAML::Node node = _node["TILEMAP"];
	CRenderComponent::LoadFromYAML(node);
	m_TexWH = node["TexWH"].as<Vec2>();
	m_vTileCount = node["TileCount"].as<Vec2>();
	
	// vector�� reserve�� resize�� ����
	// reserve�� ���� ���뷮�� �÷��ִ� ���̴�. (ĳ�۽�Ƽ�� �Ҵ� �޾Ƶд�.)
	// resize�� ������ �����Ͱ� ���� index ������ �����Ҵ� �޴´�.
	// �� �� �ڼ��� ������ �ʿ䰡 �ִ�. (������ ������ �� ����)
	// ������ ���� reserve�� ���ص� �Ǿ ���� (���� �������� �ּ��� ���ܵ�)
	// Ÿ���� ���� �þ�� TileMap���� ��ü������ resize�� ���ֱ� ������ ������ ����.
	size_t iTileCount = (size_t)(m_vTileCount.x * m_vTileCount.y);
	SetTileCount((UINT)m_vTileCount.x, (UINT)m_vTileCount.y);
	// ���Ϸκ��� Ÿ�� ������ �о ���Ϳ� ���
	m_vecTile = node["vecTile"].as<vector<tTile>>();

	// Ÿ�������� 1�� �̻� ������, ������ ������ üũ --> render ���� �����͸� ����ȭ���۷� �̵�
	if (0 < m_vecTile.size())
		m_bDataChanged = true;

	node = _node["TILEMAP"]["TileMapTexture"];
	LoadResourceRef<CTexture>(m_AtlasTex, node);
}