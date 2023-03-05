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

	vector<CCamera*>		m_vecCam;			// ���� ������ �ִ� ��� ī�޶�
	CCamera*				m_EditorCam;		// �����Ϳ� ī�޶�

	vector<CLight2D*>		m_vecLight2D;		// ���� ������ �ִ� ��� 2D ����
	CStructuredBuffer*		m_pLight2DBuffer;	// 2D ���� ������ ������ ����ȭ ����

	vector<CLight3D*>		m_vecLight3D;
	CStructuredBuffer*		m_pLight3DBuffer;

	vector<tDebugShapeInfo> m_DebugDrawInfo;

	Ptr<CTexture>			m_RTCopyTex;		// ���� Ÿ�� ���� �ؽ�ó
												// PostProcess�� �����ϱ� ���ؼ� ���� Ÿ�� �ؽ�ó�� �ʿ��ѵ�, RenderTargetView�̸鼭, SRV�ΰ��� ���� �ȵǱ� ������ ���纻�� �ϳ� �����
												// �ش� �纻�� SRV�� ������ �ϰ�, RTT�� �״�� RTV�� ���ε��� �Ǵ�.

	Ptr<CTexture>			m_PTCopyTex;

	vector<tFont>			m_arrFont;			// ��Ʈ ����� ���� vector

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
		// ������ -1�� ��ȯ�ؼ� �ش� ����Ʈ�� �� ��° �迭 ������� �˷��ش�.
		return m_vecLight3D.size() - 1;
	}

	CCamera* GetMainCam();
	CMRT* GetMRT(MRT_TYPE _Type) { return m_arrMRT[(UINT)_Type]; }
	const vector<CLight3D*>& GetLight3D() { return m_vecLight3D; }

	// ����Ÿ���� ī���ؽ�ó�� ����
	void CopyRenderTarget();

	// ����� �ڵ� PositionTargetTex ����
	void CopyPositionTarget();

	// DebugDraw�� ������ ���������� ���� �Ǿ�� �ϹǷ� ifdef�� �̿��� ó�����־���.
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

