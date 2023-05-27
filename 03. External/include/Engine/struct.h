#pragma once

// ��Ʈ ����� ���� ����ü
struct tFont
{
	wstring	strFont;
	Vec2	vecPos;
	float	fSize;
	Vec4	vecColor;
};

// �츮�� ����� ���� ������ ���� ����ü
struct Vtx
{
	Vec3 vPos;
	Vec4 vColor;

	// if manage UV with vector -> struct that has Vtx can has different size. 
	// max uv -> 3
	Vec2 vUV0;
	Vec2 vUV1;
	Vec2 vUV2;

	Vec3 vTangent;
	Vec3 vBinormal;
	Vec3 vNormal;

	Vec4 vWeights;
	Vec4 vIndices;
};

struct tEvent
{
	EVENT_TYPE	eType;
	DWORD_PTR	wParam;
	DWORD_PTR	lParam;
};

// ���� ����ü (�ش� ����ü�� 2D�� 3D ��ο��� ����� �����ϴ�.)
struct tLightInfo
{
	Vec4			vDiff;		// ���ݻ� ��		: ���� ��ü���� ����
	Vec4			vSpec;		// �ݻ籤 ���	: ����ŧ���� 3D �������� ����Ѵ�.
	Vec4			vEmb;		// ȯ�汤		: �ֺ� ���� ���� ����

	Vec4			vWorldPos;	// ���� ���� ��ġ
	Vec4			vWorldDir;	// ������ ����
	LIGHT_TYPE		iLightType;	// ������ Ÿ��
	float			fRadius;	// ���� �ݰ� (POINT, SPOT) : ���� ���� �� �� �ִ� �ִ� �Ÿ�
	float			fInAngle;		// ���� ���� (SPOT)
	float			fOutAngle;
};

// Material
struct tMtrlData
{
	Vec4 vDiff;
	Vec4 vSpec;
	Vec4 vAmb;
	Vec4 vEmv;
};

// ���� ����ü
struct tRay
{
	Vec3 vStart;
	Vec3 vDir;
};

// Raycast ����� ���� ����ü
struct tRaycastOut
{
	Vec2 vUV;
	int  iDist;
	int  bSuccess;
};

struct tWeight_4
{
	float arrWeight0[4];
	float arrWeight1[4];
	float arrWeight2[4];
};

// ���̾� �浹 ���� ���� ����ü
struct tLayerCollision
{
	int iLeft;
	int iRight;
};

// ======================
// =   Animation ����ü  =
// ======================
// Pixel ������ ��ǥ�� �ƴ� UV���� ��ǥ�� ����� ������ �� ���ε� �ɾ��ش�.
struct tAnim2DFrm
{
	Vec2  vLeftTop;
	Vec2  vSlice;
	Vec2  vOffset;
	Vec2  vFullsize;
	float fDuration;
};

// ============
// Animation 3D
// ============
struct tFrameTrans
{
	Vec4	vTranslate;
	Vec4	vScale;
	Vec4	qRot;
};

struct tMTKeyFrame
{
	double	dTime;
	int		iFrame;
	Vec3	vTranslate;
	Vec3	vScale;
	Vec4	qRot;
};


struct tMTBone
{
	wstring				strBoneName;
	int					iDepth;
	int					iParentIndx;
	Matrix				matOffset;	// Offset ���(�� -> ��Ʈ ������ ���)
	Matrix				matBone;   // parent to child mat
	// Matrix				matRelative; 
	vector<tMTKeyFrame>	vecKeyFrame;
};

struct tMTAnimClip
{
	wstring			strAnimName;
	int				iStartFrame;
	int				iEndFrame;
	int				iFrameLength;

	double			dStartTime;
	double			dEndTime;
	double			dTimeLength;
	float			fUpdateTime; // �̰� �Ⱦ�

	FbxTime::EMode	eMode;
};

// ===========
// CAM EFFECT
// ===========
struct tCamEffectInfo
{
	float		fAccTime;	// ���� ���� �ð�
	float		fDuration;	// ����Ʈ �� ����ð�
	CAM_EFFECT	eEffect;	// ī�޶� ȿ�� Ÿ��
};

// ===========
// Instancing
// ===========
union uInstID
{
	struct {
		UINT iMesh;
		WORD iMtrl;
		WORD iMtrlIdx;
	};
	ULONG64 llID;
};

class CGameObject;
struct tInstObj
{
	CGameObject* pObj;
	UINT		 iMtrlIdx;
};

struct tInstancingData
{
	Matrix matWorld;
	Matrix matWV;
	Matrix matWVP;
	int    iRowIdx;
	int	   iPadding[3];
};

// ======================
// =   Particle ����ü   =
// ======================
// ��ƼŬ ���� �ϳ��� �����ϴ� ����ü
struct tParticle
{
	Vec4 vRelativePos;	// ��ƼŬ�� ��� ��ġ
	Vec4 vDir;			// ��ƼŬ�� ����
	Matrix vWorldMat;
	Vec4 vSpeed;
	float fMaxTime;		// �ִ� ��ƼŬ �����ð�
	float fCurTime;		// ���� ��ƼŬ �����ð�
	float fSpeed;		// ��ƼŬ�� �ӵ�
	UINT  iActive;		// ��ƼŬ Ȱ��ȭ ���� bool�� �ƴ� UINT�� �� ������ 16����Ʈ ������ ���߱� �����̴�.
};

struct tParticleShare
{
	UINT	iAliveCount;	// �̹� �����ӿ� Ȱ��ȭ ������ ��ƼŬ ����
	UINT	iPadding[3];
};

// ======================
// =   TileMap ����ü   =
// ======================

struct tTile
{
	Vec2 vLeftTop; // UV
	Vec2 vSlice;
};

// ======================
// =  Debug Shape Info  =
// ======================
struct tDebugShapeInfo
{
	DEBUG_SHAPE eShape;
	Vec4		vColor;
	Vec3		vPosition;
	Vec3		vScale;
	Vec3		vRot;
	float		fRadius;
	float		fAngle;
	float		fDuration; // 1�����Ӹ� �����ϰ� ������� �ȵǱ� ������ �ð��� �־��־���.
	float		fCurtime;
};

// ======================
// =  Shader Paramater  =
// ======================
struct tScalarParam
{
	SCALAR_PARAM	eParam;
	string			strName;
};

struct tTextureParam
{
	TEX_PARAM		eParam;
	string			strName;
};

// ======================
// = ��� ���ۿ� ����ü =
// ======================

struct tTransform
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;

	Matrix matWorldInv;
	Matrix matViewInv;

	Matrix matWV;
	Matrix matWVP;
	Matrix matXRot180;
};

extern tTransform g_transform;

struct tMtrlConst
{
	tMtrlData mtrl;
	int iArr[4];
	float fArr[4];
	Vec2 v2Arr[4];
	Vec4 v4Arr[4];
	Matrix matArr[4];

	int HasTex[(UINT)TEX_PARAM::TEX_END]; // �ش� ����� �ش� �ؽ�ó �迭�� ���ε��� �ɸ��� �ִ��� Ȯ���ϱ� ���� ����̴�.
	int	arrAnimData[4];	// 3D Animation ����
	int iUVCount;
	int iTexUVIndex[7]; // UV index that Tex uses 
};

struct tGlobalData
{
	Vec2	vRenderResolution;	// ��� �ػ�
	Vec2	vNoiseResolution;	// ������ �ؽ�ó �ػ�
	int		iLight2DCount;		// ���� ����
	int		iLight3DCount;

	float	fAccTime;			// ���� �ð�
	float	fDT;				// Delta Time
};

extern tGlobalData g_global;

// Animation2D�� ��� ���� ����ü
// ��� ���۴� 16����Ʈ ������ �����־�� �Ѵ�.
struct tAnim2DInfo
{
	Vec2 vLeftTop;		// ���ø��� ��ġ : �츮�� ��Ʋ�� �̹����� ���ε��ϱ� ������ � ��ġ�� �߶���ϴ��� �˾ƾ� �Ѵ�.
	Vec2 vSlice;		// ���� �»���� �������� �󸶳� �ڸ��� Slice ����
	Vec2 vOffset;		// �߽��� �ȸ��� ��� offset ���� ��
	Vec2 vFullSize;		// ��ü ũ�� : 
	int	 iAnim2DUse;	// ���� ���ϸ��̼�2D�� ��������� Ȯ���ϱ� ���� ��. 0�� ��� ������� �ʴ� ���̴�.
						// 0�� ��� t0�� ���ε��� �ؽ�ó�� ����ϰ�, 1�� ��� t12�� ���ε��� ��Ʋ�� �ؽ�ó�� ����Ѵ�.
	int  ipadding[3];	// 16����Ʈ ������ ���߱� ���� �����Ҵ� (�е�)
};

struct tBGRA
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
};

struct tTimeSlow
{
	bool bActive;
	float fRatio;
};

extern tTimeSlow g_timeslow;