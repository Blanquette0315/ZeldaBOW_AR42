#include "pch.h"
#include "GraphicsShaderUI.h"

#include <Engine/CGraphicsShader.h>

GraphicsShaderUI::GraphicsShaderUI()
	: ResUI("GraphicsShader##UI", RES_TYPE::GRAPHICS_SHADER)
	, m_eTopology(D3D10_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, m_eDomainType(SHADER_DOMAIN::NONE)
	, m_eRSType(RS_TYPE::CULL_NONE)
	, m_eDSType(DS_TYPE::LESS)
	, m_eBSType(BS_TYPE::DEFAULT)
	, m_bVS(false)
	, m_bHS(false)
	, m_bDS(false)
	, m_bGS(false)
	, m_bPS(false)
	, m_bUpdateOnce(false)
{
}

GraphicsShaderUI::~GraphicsShaderUI()
{
}

void GraphicsShaderUI::update()
{
	ResUI::update();

	if (GetTarget().Get() != nullptr)
	{
		// dynamic cast로 CRes가 아닌 CGraphicsShader로 변경해 데이터를 가져옴.
		// 이때, dnynamic cast가 실패할 경우, 잘못되었다고 판단해 assert를 건다.
		CGraphicsShader* pTargetShader = dynamic_cast<CGraphicsShader*>(GetTarget().Get());
		assert(pTargetShader);

		if (!m_bUpdateOnce)
		{
			m_eTopology = pTargetShader->GetTopology();
			m_eDomainType = pTargetShader->GetDomain();
			m_eRSType = pTargetShader->GetRSType();
			m_eDSType = pTargetShader->GetDSType();
			m_eBSType = pTargetShader->GetBSType();

			m_bUpdateOnce = true;
		}

		m_bVS = pTargetShader->USE_VS();
		m_bHS = pTargetShader->USE_HS();
		m_bDS = pTargetShader->USE_DS();
		m_bGS = pTargetShader->USE_GS();
		m_bPS = pTargetShader->USE_PS();
	}
}

void GraphicsShaderUI::render_update()
{
	ResUI::render_update();

	// Topology 출력
	char Topology[50] = "";
	switch (m_eTopology)
	{
	case D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP:
	{
		sprintf_s(Topology, 50, "##D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP");
	}
	break;

	case D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
	{
		sprintf_s(Topology, 50, "D3D11_PRIMITIVE_TOPOLOGY_POINTLIST");
	}
	break;

	case D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
	{
		sprintf_s(Topology, 50, "D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST");
	}
	break;

	case D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
	{
		sprintf_s(Topology, 50, "D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST");
	}
	break;

	default:
	{
		sprintf_s(Topology, 50, "없음 !! 해당 UI render_update에서 추가해주어야 한다.");
	}
	break;
	}
	ImGui::Text("Topology"); ImGui::SameLine(); ImGui::InputText("##TopologyInputText", Topology, 50, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	// Shader Domain 출력
	char ShaderDomain[50] = "";
	switch (m_eDomainType)
	{
	case SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQUE:
	{
		sprintf_s(ShaderDomain, 50, "DOMAIN_DEFERRED_OPAQUE");
	}
		break;

	case SHADER_DOMAIN::DOMAIN_DEFERRED_MASK:
	{
		sprintf_s(ShaderDomain, 50, "DOMAIN_DEFERRED_MASK");
	}
		break;

	case SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL:
	{
		sprintf_s(ShaderDomain, 50, "DOMAIN_DEFERRED_DECAL");
	}
		break;

	case SHADER_DOMAIN::DOMAIN_OPAQUE:
	{
		sprintf_s(ShaderDomain, 50, "DOMAIN_OPAQUE");
	}
		break;

	case SHADER_DOMAIN::DOMAIN_MASK:
	{
		sprintf_s(ShaderDomain, 50, "DOMAIN_MASK");
	}
		break;

	case SHADER_DOMAIN::DOMAIN_DECAL:
	{
		sprintf_s(ShaderDomain, 50, "DOMAIN_DECAL");
	}
		break;

	case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
	{
		sprintf_s(ShaderDomain, 50, "DOMAIN_TRANSPARENT");
	}
		break;

	case SHADER_DOMAIN::DOMAIN_POST_PROCESS:
	{
		sprintf_s(ShaderDomain, 50, "DOMAIN_POST_PROCESS");
	}
		break;

	case SHADER_DOMAIN::DOMAIN_LIGHT:
	{
		sprintf_s(ShaderDomain, 50, "DOMAIN_LIGHT");
	}
		break;

	case SHADER_DOMAIN::NONE:
	{
		sprintf_s(ShaderDomain, 50, "NONE");
	}
		break;
	}
	ImGui::Text("Domain  "); ImGui::SameLine(); ImGui::InputText("##DomainInputText", ShaderDomain, 50, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	// RS_TYPE 출력
	char Rs_type[50] = "";
	switch (m_eRSType)
	{
	case RS_TYPE::CULL_BACK: 
	{
		sprintf_s(Rs_type, 50, "CULL_BACK");
	}
		break;

	case RS_TYPE::CULL_FRONT:
	{
		sprintf_s(Rs_type, 50, "CULL_FRONT");
	}
		break;

	case RS_TYPE::CULL_NONE:
	{
		sprintf_s(Rs_type, 50, "CULL_NONE");
	}
		break;

	case RS_TYPE::WIRE_FRAME:
	{
		sprintf_s(Rs_type, 50, "WIRE_FRAME");
	}
		break;
	}
	ImGui::Text("RS_TYPE "); ImGui::SameLine(); ImGui::InputText("##RS_TYPEInputText", Rs_type, 50, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	// BS_TYPE 출력
	char Bs_type[50] = "";
	switch (m_eBSType)
	{
	case BS_TYPE::DEFAULT: 
	{
		sprintf_s(Bs_type, 50, "DEFAULT");
	}
		break;

	case BS_TYPE::ALPHABLEND:
	{
		sprintf_s(Bs_type, 50, "ALPHABLEND");
	}
		break;

	case BS_TYPE::ONE_ONE:
	{
		sprintf_s(Bs_type, 50, "ONE_ONE");
	}
		break;
	}
	ImGui::Text("BS_TYPE "); ImGui::SameLine(); ImGui::InputText("##BS_TYPEInputText", Bs_type, 50, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	// DS_TYPE 출력
	char Ds_type[50] = "";
	switch (m_eDSType)
	{
	case DS_TYPE::LESS:
	{
		sprintf_s(Ds_type, 50, "LESS");
	}
		break;

	case DS_TYPE::LESS_EQUAL:
	{
		sprintf_s(Ds_type, 50, "LESS_EQUAL");
	}
		break;
	case DS_TYPE::GREATER:
	{
		sprintf_s(Ds_type, 50, "GREATER");
	}
		break;

	case DS_TYPE::NO_WRITE:
	{
		sprintf_s(Ds_type, 50, "NO_WRITE");
	}
		break;

	case DS_TYPE::NO_TEST_NO_WRITE:
	{
		sprintf_s(Ds_type, 50, "NO_TEST_NO_WRITE");
	}
		break;
	}
	ImGui::Text("DS_TYPE "); ImGui::SameLine(); ImGui::InputText("##DS_TYPEInputText", Ds_type, 50, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	// PipeLineStage
	ImGui::Text("USE_VS  "); ImGui::SameLine(); ImGui::Checkbox("##USE_VSCHB", &m_bVS);
	ImGui::Text("USE_HS  "); ImGui::SameLine(); ImGui::Checkbox("##USE_HSCHB", &m_bHS);
	ImGui::Text("USE_DS  "); ImGui::SameLine(); ImGui::Checkbox("##USE_DSCHB", &m_bDS);
	ImGui::Text("USE_GS  "); ImGui::SameLine(); ImGui::Checkbox("##USE_GSCHB", &m_bGS);
	ImGui::Text("USE_PS  "); ImGui::SameLine(); ImGui::Checkbox("##USE_PSCHB", &m_bPS);
}