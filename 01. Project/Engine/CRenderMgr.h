#pragma once

class CCamera;
class CStructuredBuffer;
class CMRT;
#include "CLight2D.h";
#include "CLight3D.h"
#include "CTexture.h";

class CRenderMgr
	: public CSingleton<CRenderMgr>
{
private:
	CMRT* m_arrMRT[(UINT)MRT_TYPE::END];

	vector<CCamera*>		m_vecCam;			// 현재 레벨에 있는 모든 카메라
	CCamera*				m_EditorCam;		// 에디터용 카메라

	vector<CLight2D*>		m_vecLight2D;		// 현재 레벨에 있는 모든 2D 광원
	CStructuredBuffer*		m_pLight2DBuffer;	// 2D 광원 정보를 전달할 구조화 버퍼

	vector<CLight3D*>		m_vecLight3D;
	CStructuredBuffer*		m_pLight3DBuffer;

	vector<tDebugShapeInfo> m_DebugDrawInfo;

	Ptr<CTexture>			m_RTCopyTex;		// 랜더 타겟 복사 텍스처
												// PostProcess를 수행하기 위해서 랜더 타겟 텍스처가 필요한데, RenderTargetView이면서, SRV인것은 말이 안되기 때문에 복사본을 하나 만들어
												// 해당 사본이 SRV의 역할을 하고, RTT는 그대로 RTV로 바인딩을 건다.

	Ptr<CTexture>			m_PTCopyTex;

	vector<tFont>			m_arrFont;			// 폰트 출력을 위한 vector

public:
	void AddFont(tFont _tFont) { m_arrFont.push_back(_tFont); }
	void FontClear() { m_arrFont.clear(); }

public:
	void init();
	void tick();
	void render();

private:
	void render_game();
	void render_editor();

	void UpdateNoiseTexture();
	void UpdateLight2D();
	void UpdateLight3D();

	void CreateMRT();
	void ClearMRT();

public:
	void RegisterCamera(CCamera* _pCam) { m_vecCam.push_back(_pCam); }
	void RegisterEditorCamera(CCamera* _pCam) { m_EditorCam = _pCam; }

	void RegisterLight2D(CLight2D* _pLight2D) { m_vecLight2D.push_back(_pLight2D); }
	int RegisterLight3D(CLight3D* _pLight3D)
	{
		m_vecLight3D.push_back(_pLight3D);
		// 사이즈 -1을 반환해서 해당 라이트가 몇 번째 배열 요소인지 알려준다.
		return m_vecLight3D.size() - 1;
	}

	CCamera* GetMainCam();
	CMRT* GetMRT(MRT_TYPE _Type) { return m_arrMRT[(UINT)_Type]; }
	const vector<CLight3D*>& GetLight3D() { return m_vecLight3D; }

	// 렌더타겟을 카피텍스처로 복사
	void CopyRenderTarget();

	// 실험용 코드 PositionTargetTex 복사
	void CopyPositionTarget();

	// DebugDraw는 릴리즈 버전에서는 제외 되어야 하므로 ifdef을 이용해 처리해주었다.
	void DebugDraw(DEBUG_SHAPE _eShape, Vec4 _vColor, Vec3 _vPosition, Vec3 _vScale, Vec3 _vRotation, float _fRadius, float _fAngle, float _fDuration)
	{
#ifdef _DEBUG
		m_DebugDrawInfo.push_back(tDebugShapeInfo { _eShape, _vColor, _vPosition, _vScale, _vRotation, _fRadius, _fAngle, _fDuration, 0.f });
#endif
	}

	vector<tDebugShapeInfo>& GetDebutgDrawInfo() { return m_DebugDrawInfo; }

public:
	CRenderMgr();
	~CRenderMgr();
};

