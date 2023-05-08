#include "pch.h"
#include "CMeshRender.h"

#include "CGameObject.h"
#include "CTransform.h"

#include "CMesh.h"
#include "CMaterial.h"

#include "CAnimator2D.h"
#include "CAnimator3D.h"

#include "CDevice.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER)
	, m_iOption(0)
{
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::tick()
{
}

void CMeshRender::finaltick()
{

	if (m_iOption == 1)
	{
		if (m_vecCheck.empty())
			m_vecCheck.resize(GetMtrlCount(), false);
		
		const vector<CGameObject*>& vecChild = GetOwner()->GetChildObject();
		bool bCheck = false;

		if (vecChild.size() != m_vecPrevChild.size())
		{
			m_vecPrevChild = vecChild;
			bCheck = true;
		}
		else
		{
			for (int i = 0; i < m_vecPrevChild.size(); ++i)
			{
				if (m_vecPrevChild[i] != vecChild[i])
				{
					m_vecPrevChild = vecChild;
					bCheck = true;
					break;
				}
			}
		}

		if (bCheck)
		{
			m_vecCheck.assign(m_vecCheck.size(), false);
			for (int i = 0; i < vecChild.size(); ++i)
			{
				if (vecChild[i]->Animator3D())
				{
					if (vecChild[i]->MeshRender())
					{
						for (int j = 0; j < vecChild[i]->MeshRender()->GetMtrlCount(); ++j)
						{
							const wstring& strKeyChild = vecChild[i]->MeshRender()->GetCurMaterial(j)->GetName();
							for (int k = 0; k < GetMtrlCount(); ++k)
							{
								const wstring& strKey = GetCurMaterial(k)->GetName();

								// same part rendering
								if (strKeyChild.find(strKey) != wstring::npos)
								{
									m_vecCheck[k] = true;
								}
							}
						}
					}
				}
			}
		}
	}
}

void CMeshRender::render()
{
	if (!IsActivate())
		return;

	if (nullptr == GetMesh())
		return;

	// ��ġ���� ������Ʈ
	Transform()->UpdateData();

	// Animator2D ������Ʈ�� �ִٸ�
	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	// Animator3D ������Ʈ
	if (Animator3D())
	{
		Animator3D()->UpdateData();

		for (size_t i = 0; i < GetMtrlCount(); ++i)
		{
			if (nullptr == GetCurMaterial(i))
				continue;
			GetCurMaterial(i)->SetAnim3D(true); // Animation Mesh �˸���
			GetCurMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
		}
	}

	// ����� ���� ������Ʈ
	UINT iSubsetCount = GetMesh()->GetSubsetCount();

	for (int i = 0; i < iSubsetCount; ++i)
	{
		if (nullptr != GetCurMaterial(i))
		{
			if (m_iOption == 1)
			{
				if (m_vecCheck[i] == false)
				{
					GetCurMaterial(i)->UpdateData();

					// ����� �޽� ������Ʈ �� ������
					GetMesh()->render(i);
				}
			}
			else
			{
				GetCurMaterial(i)->UpdateData();

				// ����� �޽� ������Ʈ �� ������
				GetMesh()->render(i);
			}
		}
	}

	// ����� �ؽ��� �������� ����
	CMaterial::Clear();

	// Animator2D ������Ʈ�� �־�����, ���� �������� Ŭ����
	if (Animator2D())
	{
		Animator2D()->Clear();
	}

	if (Animator3D())
		Animator3D()->ClearData();
}

void CMeshRender::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "MESHRENDER";
	_emitter << YAML::Value << YAML::BeginMap;
	_emitter << YAML::Key << "Option";
	_emitter << YAML::Value << m_iOption;
	CRenderComponent::SaveToYAML(_emitter);
	_emitter << YAML::EndMap;
}

void CMeshRender::LoadFromYAML(YAML::Node& _node)
{
	YAML::Node node = _node["MESHRENDER"];
	SAFE_LOAD_FROM_YAML(int, m_iOption, _node["MESHRENDER"]["Option"]);
	CRenderComponent::LoadFromYAML(node);
	if (Animator3D())
	{
		UINT i = GetMesh()->GetAnimClip()->at(0).iEndFrame;
		Animator3D()->SetFrameLimit(GetMesh()->GetAnimClip()->at(0).iEndFrame);
	}
}


