#pragma once

// ray
struct tRay
{
	Vec3 vStart;
	Vec3 vDir;
};

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
	Vec2 vUV;

	Vec3 vTangent;
	Vec3 vBinormal;
	Vec3 vNormal;

	// Vec4 vWeights;
	// Vec4 vIndices;
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
	float arrWeight[4];
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

// ======================
// =   Particle ����ü   =
// ======================
// ��ƼŬ ���� �ϳ��� �����ϴ� ����ü
struct tParticle
{
	Vec4 vRelativePos;	// ��ƼŬ�� ��� ��ġ
	Vec4 vDir;			// ��ƼŬ�� ����

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
};

extern tTransform g_transform;

struct tMtrlConst
{
	int iArr[4];
	float fArr[4];
	Vec2 v2Arr[4];
	Vec4 v4Arr[4];
	Matrix matArr[4];

	int HasTex[(UINT)TEX_PARAM::TEX_END]; // �ش� ����� �ش� �ؽ�ó �迭�� ���ε��� �ɸ��� �ִ��� Ȯ���ϱ� ���� ����̴�.
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