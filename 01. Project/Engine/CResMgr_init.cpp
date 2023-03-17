#include "pch.h"
#include "CResMgr.h"

#include "CSound.h"

void CResMgr::init()
{
	InitSound();

	CreateDefaultMesh();

	CreateDefaultTexture();

	CreateDefaultGrapicsShader();

	CreateDefaultComputeShader();

	CreateDefaultMaterial();
}

void CResMgr::InitSound()
{
	FMOD::System_Create(&CSound::g_pFMOD);

	if (nullptr == CSound::g_pFMOD)
	{
		assert(nullptr);
	}

	// 32개 채널 생성
	CSound::g_pFMOD->init(32, FMOD_DEFAULT, nullptr);
}

void CResMgr::CreateDefaultMesh()
{
	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;
	Vtx v;
	CMesh* pMesh = nullptr;

	// Point Mesh
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);

	UINT idx = 0;
	
	pMesh = new CMesh(true);
	pMesh->Create(&v, 1, &idx, 1);
	AddRes<CMesh>(L"PointMesh", pMesh);

	// 사각형 메쉬 만들기

	// 지금 Binoraml이 1인데, 사실 DX좌표계에 알맞게 쓰려면 -1이 옳다.
	// 하지만 지금 우리 예제가 PSP게임의 리소스라 OpenGL의 좌표계를 기준으로 만들어져있다.
	// 사실 DX 게임 리소스를 사용해도 문제는 안되는게, 매쉬를 임포트해서 사용하면 해당 매쉬에는 DX기준에 맞게 평면좌표계가 설정되어 있을 것이고,
	// 그 매쉬에 적용될 노말맵 텍스처도 동일한 평면좌표계를 기준으로 만들어 졌기 때문에 문제가 발생하지 않는다.
	// 하지만 지금 우리의 기본 기저 도형이 OpenGL평면 좌표계를 기준으로 Normal, Tangent, Binormal을 잡았기 때문에
	// 통일성이 없어 모호성이 발생한다. 따라서 해당 Binormal값을 -1로 변경해 DX기준에 알맞는 표면 좌표계를 설정해 주어야 한다.
	// 표면 좌표계를 설정하는 것은 어떤 방법을 사용해도 관계 없지만, 암묵적인 룰이 있기 때문에 DX로 맞춰주는게 리소스를 사용할때 모호성이 사라진다.
	v.vPos = Vec3(-0.5f, 0.5f, 0.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vUV = Vec2(1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(1.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, 0.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vUV = Vec2(0.f, 1.f);
	vecVtx.push_back(v);

	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);

	vecIdx.push_back(2);
	vecIdx.push_back(3);
	vecIdx.push_back(0);

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"RectMesh", pMesh);
	vecIdx.clear();

	// DebugRectIndex
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);
	vecIdx.push_back(2);
	vecIdx.push_back(3);
	vecIdx.push_back(0);

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"RectMesh_Debug", pMesh);
	vecVtx.clear();
	vecIdx.clear();

	//원형 메쉬 만들기
	// 중심점
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.5f, 0.5f);

	vecVtx.push_back(v);

	UINT iSlice = 40;
	float fRadius = 0.5f;
	float fTheta = XM_2PI / (float)iSlice;

	for (UINT i = 0; i < iSlice; ++i)
	{
		v.vPos = Vec3(fRadius * cosf(fTheta * (float)i), fRadius * sinf(fTheta * (float)i), 1.f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		v.vUV = Vec2(v.vPos.x + 0.5f, 1.f - (v.vPos.y + 0.5f));
		vecVtx.push_back(v);
	}

	for (UINT i = 0; i < iSlice; ++i)
	{
		vecIdx.push_back(0);
		if (i == iSlice - 1)
		{
			vecIdx.push_back(1);
		}
		else
		{
			vecIdx.push_back(i + 2);
		}
		vecIdx.push_back(i + 1);
	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"CircleMesh", pMesh);
	vecIdx.clear();

	// Circle_debug
	for (int i = 0; i < vecVtx.size(); ++i)
	{
		vecIdx.push_back(i + 1);
	}
	vecIdx.push_back(1);
	
	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"CircleMesh_Debug", pMesh);
	vecVtx.clear();
	vecIdx.clear();

	// Cube Mesh
	Vtx arrCube[24] = {};
	// 윗면
	arrCube[0].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[0].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[0].vUV = Vec2(0.f, 0.f);
	arrCube[0].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[1].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[1].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[1].vUV = Vec2(0.f, 0.f);
	arrCube[1].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[2].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[2].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[2].vUV = Vec2(0.f, 0.f);
	arrCube[2].vNormal = Vec3(0.f, 1.f, 0.f);

	arrCube[3].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[3].vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	arrCube[3].vUV = Vec2(0.f, 0.f);
	arrCube[3].vNormal = Vec3(0.f, 1.f, 0.f);


	// 아랫 면	
	arrCube[4].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[4].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[4].vUV = Vec2(0.f, 0.f);
	arrCube[4].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[5].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[5].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[5].vUV = Vec2(0.f, 0.f);
	arrCube[5].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[6].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[6].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[6].vUV = Vec2(0.f, 0.f);
	arrCube[6].vNormal = Vec3(0.f, -1.f, 0.f);

	arrCube[7].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[7].vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	arrCube[7].vUV = Vec2(0.f, 0.f);
	arrCube[7].vNormal = Vec3(0.f, -1.f, 0.f);

	// 왼쪽 면
	arrCube[8].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[8].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[8].vUV = Vec2(0.f, 0.f);
	arrCube[8].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[9].vPos = Vec3(-0.5f, 0.5f, -0.5f);
	arrCube[9].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[9].vUV = Vec2(0.f, 0.f);
	arrCube[9].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[10].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[10].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[10].vUV = Vec2(0.f, 0.f);
	arrCube[10].vNormal = Vec3(-1.f, 0.f, 0.f);

	arrCube[11].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[11].vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	arrCube[11].vUV = Vec2(0.f, 0.f);
	arrCube[11].vNormal = Vec3(-1.f, 0.f, 0.f);

	// 오른쪽 면
	arrCube[12].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[12].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[12].vUV = Vec2(0.f, 0.f);
	arrCube[12].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[13].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[13].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[13].vUV = Vec2(0.f, 0.f);
	arrCube[13].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[14].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[14].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[14].vUV = Vec2(0.f, 0.f);
	arrCube[14].vNormal = Vec3(1.f, 0.f, 0.f);

	arrCube[15].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[15].vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	arrCube[15].vUV = Vec2(0.f, 0.f);
	arrCube[15].vNormal = Vec3(1.f, 0.f, 0.f);

	// 뒷 면
	arrCube[16].vPos = Vec3(0.5f, 0.5f, 0.5f);
	arrCube[16].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[16].vUV = Vec2(0.f, 0.f);
	arrCube[16].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[17].vPos = Vec3(-0.5f, 0.5f, 0.5f);
	arrCube[17].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[17].vUV = Vec2(0.f, 0.f);
	arrCube[17].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[18].vPos = Vec3(-0.5f, -0.5f, 0.5f);
	arrCube[18].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[18].vUV = Vec2(0.f, 0.f);
	arrCube[18].vNormal = Vec3(0.f, 0.f, 1.f);

	arrCube[19].vPos = Vec3(0.5f, -0.5f, 0.5f);
	arrCube[19].vColor = Vec4(1.f, 1.f, 0.f, 1.f);
	arrCube[19].vUV = Vec2(0.f, 0.f);
	arrCube[19].vNormal = Vec3(0.f, 0.f, 1.f);

	// 앞 면
	arrCube[20].vPos = Vec3(-0.5f, 0.5f, -0.5f);;
	arrCube[20].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[20].vUV = Vec2(0.f, 0.f);
	arrCube[20].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[21].vPos = Vec3(0.5f, 0.5f, -0.5f);
	arrCube[21].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[21].vUV = Vec2(0.f, 0.f);
	arrCube[21].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[22].vPos = Vec3(0.5f, -0.5f, -0.5f);
	arrCube[22].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[22].vUV = Vec2(0.f, 0.f);
	arrCube[22].vNormal = Vec3(0.f, 0.f, -1.f);

	arrCube[23].vPos = Vec3(-0.5f, -0.5f, -0.5f);
	arrCube[23].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	arrCube[23].vUV = Vec2(0.f, 0.f);
	arrCube[23].vNormal = Vec3(0.f, 0.f, -1.f);

	// 인덱스
	for (int i = 0; i < 12; i += 2)
	{
		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 1);
		vecIdx.push_back(i * 2 + 2);

		vecIdx.push_back(i * 2);
		vecIdx.push_back(i * 2 + 2);
		vecIdx.push_back(i * 2 + 3);
	}

	pMesh = new CMesh(true);
	pMesh->Create(arrCube, 24, vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"CubeMesh", pMesh);
	vecIdx.clear();

	// ===========
	// Sphere Mesh
	// ===========
	fRadius = 0.5f;

	// Top
	v.vPos = Vec3(0.f, fRadius, 0.f);
	v.vUV = Vec2(0.5f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, 1.f);
	vecVtx.push_back(v);

	// Body
	UINT iStackCount = 40; // 가로 분할 개수
	UINT iSliceCount = 40; // 세로 분할 개수

	float fStackAngle = XM_PI / iStackCount;
	float fSliceAngle = XM_2PI / iSliceCount;

	float fUVXStep = 1.f / (float)iSliceCount;
	float fUVYStep = 1.f / (float)iStackCount;

	for (UINT i = 1; i < iStackCount; ++i)
	{
		float phi = i * fStackAngle;

		for (UINT j = 0; j <= iSliceCount; ++j)
		{
			float theta = j * fSliceAngle;

			v.vPos = Vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle)
				, fRadius * cosf(i * fStackAngle)
				, fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));
			v.vUV = Vec2(fUVXStep * j, fUVYStep * i);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vNormal = v.vPos;
			v.vNormal.Normalize();

			v.vTangent.x = -fRadius * sinf(phi) * sinf(theta);
			v.vTangent.y = 0.f;
			v.vTangent.z = fRadius * sinf(phi) * cosf(theta);
			v.vTangent.Normalize();

			v.vTangent.Cross(v.vNormal, v.vBinormal);
			v.vBinormal.Normalize();

			vecVtx.push_back(v);
		}
	}

	// Bottom
	v.vPos = Vec3(0.f, -fRadius, 0.f);
	v.vUV = Vec2(0.5f, 1.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();

	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	// 인덱스
	// 북극점
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// 몸통
	for (UINT i = 0; i < iStackCount - 2; ++i)
	{
		for (UINT j = 0; j < iSliceCount; ++j)
		{
			// + 
			// | \
			// +--+
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j)+1);

			// +--+
			//  \ |
			//    +
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i)+(j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
		}
	}

	// 남극점
	UINT iBottomIdx = (UINT)vecVtx.size() - 1;
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(iBottomIdx);
		vecIdx.push_back(iBottomIdx - (i + 2));
		vecIdx.push_back(iBottomIdx - (i + 1));
	}

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"SphereMesh", pMesh);
	vecVtx.clear();
	vecIdx.clear();

	// ===========
	//  Cone Mesh
	// ===========

	CreateConeMesh(0.5f, 1.f, 50);

	// ===============
	//  Cylinder Mesh
	// ===============

	pMesh = new CMesh(true);
	CreateCylinder(0.5f, 0.5f, 1.f, 50, 1, vecVtx, vecIdx);
	pMesh->Create(vecVtx.data(), vecVtx.size(), vecIdx.data(), vecIdx.size());
	AddRes<CMesh>(L"CylinderMesh", pMesh);
}

namespace
{
	inline Vec3 GetCircleVector(size_t i, size_t tessellation) noexcept
	{
		const float angle = float(i) * XM_2PI / float(tessellation);
		float       sin;
		float       cos;

		XMScalarSinCos(&sin, &cos, angle);
		return { cos, sin, 0 };
	}
}

void CResMgr::CreateConeMesh(float radius, float height, size_t tessellation)
{
	if (tessellation < 3)
	{
		throw std::invalid_argument("tessellation parameter must be at least 3");
	}

	vector<Vtx>  vertices;
	vector<UINT> indices;

	vertices.reserve(tessellation + 1); // 1 + x
	indices.reserve(tessellation * 6 - 6);  // 3x + 3(x-2)

	const Vec3   topPosition = Vec3::Zero;
	vertices.push_back(Vtx{ topPosition, {}, {}, {}, {}, {} });

	for (size_t i = 0; i < tessellation; i++)
	{
		const Vec3 circleVec = GetCircleVector(i, tessellation);
		const Vec3 position = circleVec * radius + Vec3(0, 0, height);
		vertices.push_back(Vtx{ position, {}, {}, {}, {}, {} });

		indices.push_back(0);
		indices.push_back((UINT)i + 2);
		indices.push_back((UINT)i + 1);
	}

	indices[indices.size() - 2] = 1;

	for (size_t i = 0; i < tessellation - 2; i++)
	{
		indices.push_back(1);
		indices.push_back(1 + (UINT)i + 1);
		indices.push_back(1 + (UINT)i + 2);
	}


	// 추가
	CMesh* pMesh = new CMesh(true);
	pMesh->Create(vertices.data(), vertices.size(), indices.data(), indices.size());
	AddRes<CMesh>(L"ConeMesh", pMesh);
}

void CResMgr::CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, vector<Vtx>& Vertices, vector<UINT>& Indices)
{
	Vertices.clear();
	Indices.clear();

	//
	// Build Stacks.
	// 

	float stackHeight = height / stackCount;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	UINT ringCount = stackCount + 1;

	// Compute vertices for each stack ring starting at the bottom and moving up.
	for (UINT i = 0; i < ringCount; ++i)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;

		// vertices of ring
		float dTheta = 2.0f * XM_PI / sliceCount;
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			Vtx vertex;

			float c = cosf(j * dTheta);
			float s = sinf(j * dTheta);

			vertex.vPos = XMFLOAT3(r * c, y, r * s);

			vertex.vUV.x = (float)j / sliceCount;
			vertex.vUV.y = 1.0f - (float)i / stackCount;

			// Cylinder can be parameterized as follows, where we introduce v
			// parameter that goes in the same direction as the v tex-coord
			// so that the bitangent goes in the same direction as the v tex-coord.
			//   Let r0 be the bottom radius and let r1 be the top radius.
			//   y(v) = h - hv for v in [0,1].
			//   r(v) = r1 + (r0-r1)v
			//
			//   x(t, v) = r(v)*cos(t)
			//   y(t, v) = h - hv
			//   z(t, v) = r(v)*sin(t)
			// 
			//  dx/dt = -r(v)*sin(t)
			//  dy/dt = 0
			//  dz/dt = +r(v)*cos(t)
			//
			//  dx/dv = (r0-r1)*cos(t)
			//  dy/dv = -h
			//  dz/dv = (r0-r1)*sin(t)

			// This is unit length.
			vertex.vTangent = XMFLOAT3(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			XMFLOAT3 bitangent(dr * c, -height, dr * s);

			XMVECTOR T = XMLoadFloat3(&vertex.vTangent);
			XMVECTOR B = XMLoadFloat3(&bitangent);
			XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
			XMStoreFloat3(&vertex.vNormal, N);

			Vertices.push_back(vertex);
		}
	}

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different.
	UINT ringVertexCount = sliceCount + 1;

	// Compute indices for each stack.
	for (UINT i = 0; i < stackCount; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			Indices.push_back(i * ringVertexCount + j);
			Indices.push_back((i + 1) * ringVertexCount + j);
			Indices.push_back((i + 1) * ringVertexCount + j + 1);

			Indices.push_back(i * ringVertexCount + j);
			Indices.push_back((i + 1) * ringVertexCount + j + 1);
			Indices.push_back(i * ringVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, Vertices, Indices);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, Vertices, Indices);
}

void CResMgr::BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, vector<Vtx>& Vertices, vector<UINT>& Indices)
{
	UINT baseIndex = (UINT)Vertices.size();

	float y = 0.5f * height;
	float dTheta = 2.0f * XM_PI / sliceCount;

	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	Vtx _vtx;
	_vtx.vNormal = Vec3(0.f, 0.f, 1.f);
	_vtx.vBinormal = Vec3(0.0f, 1.0f, 0.0f);
	_vtx.vTangent = Vec3(1.0f, 0.0f, 0.0f);
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius * cosf(i * dTheta);
		float z = topRadius * sinf(i * dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		/*
		* 	Vec3 vPos;
			Vec4 vColor;
			Vec2 vUV;

			Vec3 vTangent;
			Vec3 vBinormal;
			Vec3 vNormal;
		*/
		_vtx.vPos = Vec3(x, y, z);
		_vtx.vUV = Vec2(u, v);
		Vertices.push_back(_vtx);
	}

	// Cap center vertex.
	_vtx.vPos = Vec3(0.0f, y, 0.0f);
	_vtx.vUV = Vec2(0.5f, 0.5f);
	Vertices.push_back(_vtx);

	// Index of center vertex.
	UINT centerIndex = (UINT)Vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		Indices.push_back(centerIndex);
		Indices.push_back(baseIndex + i + 1);
		Indices.push_back(baseIndex + i);
	}
}

void CResMgr::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, vector<Vtx>& Vertices, vector<UINT>& Indices)
{
	// 
	// Build bottom cap.
	//

	UINT baseIndex = (UINT)Vertices.size();
	float y = -0.5f * height;

	// vertices of ring
	float dTheta = 2.0f * XM_PI / sliceCount;
	Vtx _vtx;
	_vtx.vNormal = Vec3(0.f, 0.f, -1.f);
	_vtx.vBinormal = Vec3(0.0f, -1.0f, 0.0f);
	_vtx.vTangent = Vec3(1.0f, 0.0f, 0.0f);
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = bottomRadius * cosf(i * dTheta);
		float z = bottomRadius * sinf(i * dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		_vtx.vPos = Vec3(x, y, z);
		_vtx.vUV = Vec2(u, v);
		Vertices.push_back(_vtx);
	}

	// Cap center vertex.
	_vtx.vPos = Vec3(0.0f, y, 0.0f);
	_vtx.vUV = Vec2(0.5f, 0.5f);
	Vertices.push_back(_vtx);

	// Cache the index of center vertex.
	UINT centerIndex = (UINT)Vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		Indices.push_back(centerIndex);
		Indices.push_back(baseIndex + i);
		Indices.push_back(baseIndex + i + 1);
	}
}

void CResMgr::CreateDefaultTexture()
{
	// 로딩
	// Texture 로딩 
	//CResMgr::GetInst()->Load<CTexture>(L"Player", L"texture\\CharmanderUI.png");
	//CResMgr::GetInst()->Load<CTexture>(L"Smoke", L"texture\\smokeparticle.png");
	//
	//Load<CTexture>(L"SmokeParticle", L"texture\\particle\\smokeparticle.png");
	//Load<CTexture>(L"Bubbles50px", L"texture\\particle\\Bubbles50px.png");
	//
	//Load<CTexture>(L"TILEAtlas", L"texture\\TILE.bmp");
	//Load<CTexture>(L"TerrainTiles", L"texture\\terrainTiles_default.png");
	//Load<CTexture>(L"Alina_Bandit", L"texture\\Alina_Bandit.png");
	//
	//// 애니메이션 로딩 
	//CResMgr::GetInst()->Load<CTexture>(L"link", L"texture\\link.png");
	//Load<CTexture>(L"CardUpgradeEffAtlas", L"texture\\CardUpgradeEffAtlas.png"); // 36, 3, 12 size 164 178
	//Load<CTexture>(L"MonsterAtlas", L"texture\\MonsterAtlas.png");
	//Load<CTexture>(L"KillBossAnim", L"texture\\KillBossAnim.png"); // 130 13 10 size 198 90 
	//
	// NoiseTexture
	// Noise_01은 생성 타입이 원일 시 고르게 분포하지 않는 문제가 있다.
	Load<CTexture>(L"texture\\noise\\noise_01.png", L"texture\\noise\\noise_01.png");
	Load<CTexture>(L"texture\\noise\\noise_02.png", L"texture\\noise\\noise_02.png");
	Load<CTexture>(L"texture\\noise\\noise_03.jpg", L"texture\\noise\\noise_03.jpg");
	
	Load<CTexture>(L"texture\\noise\\TestNoise.png", L"texture\\noise\\TestNoise.png");

	// 생성
	// Texture 생성
	// D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS 로 한 이유
	// : 텍스쳐를 하나 만들어서 최초 검은색으로 출력하다가 ComputeShader를 이용해 색을 바꿔 출력해보기 위함이다.
	CreateTexture(L"UAVTex", 1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM
		, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
}

void CResMgr::CreateDefaultGrapicsShader()
{
	AddInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, "POSITION");
	AddInputLayout(DXGI_FORMAT_R32G32B32A32_FLOAT, "COLOR");
	AddInputLayout(DXGI_FORMAT_R32G32_FLOAT, "TEXCOORD");
	AddInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, "TANGENT");
	AddInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, "BINORMAL");
	AddInputLayout(DXGI_FORMAT_R32G32B32_FLOAT, "NORMAL");

	CGrapicsShader* pShader = nullptr;

	// std2D Shader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D");
	pShader->SetRSType(RS_TYPE::CULL_NONE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	pShader->AddScalarParam(INT_0, "Test Int");
	pShader->AddScalarParam(FLOAT_2, "Test Float");
	pShader->AddScalarParam(VEC2_3, "Test Vec2");
	pShader->AddScalarParam(VEC4_2, "Test Vec4");
	pShader->AddTexureParam(TEX_0, "Output Texture 1");
	pShader->AddTexureParam(TEX_1, "Output Texture 2");
	pShader->AddTexureParam(TEX_2, "Output Texture 3");

	AddRes<CGrapicsShader>(L"Std2DShader", pShader);

	// std2DAlphaBlend Shader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D_AlphaBlend");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDSType(DS_TYPE::NO_WRITE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	pShader->AddScalarParam(INT_0, "Test Int");
	pShader->AddScalarParam(FLOAT_2, "Test Float");
	pShader->AddScalarParam(VEC2_3, "Test Vec2");
	pShader->AddScalarParam(VEC4_2, "Test Vec4");
	pShader->AddTexureParam(TEX_0, "Output Texture 1");
	pShader->AddTexureParam(TEX_1, "Output Texture 2");
	pShader->AddTexureParam(TEX_2, "Output Texture 3");

	AddRes<CGrapicsShader>(L"Std2DAlphaBlendShader", pShader);

	// Grid Shader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\editor.fx", "VS_Grid");
	pShader->CreatePixelShader(L"shader\\editor.fx", "PS_Grid");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDSType(DS_TYPE::NO_WRITE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddRes<CGrapicsShader>(L"GridShader", pShader);

	// DebufDraw Shader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\debugdraw.fx", "VS_DebugDraw");
	pShader->CreatePixelShader(L"shader\\debugdraw.fx", "PS_DebugDraw");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddRes<CGrapicsShader>(L"DebugDrawShader", pShader);

	// Frustum Debug Draw Shader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\Frustum.fx", "VS_Frustum");
	pShader->CreateGeometryShader(L"shader\\Frustum.fx", "GS_Frustum");
	pShader->CreatePixelShader(L"shader\\Frustum.fx", "PS_Frustum");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddRes<CGrapicsShader>(L"FrustumDebugDrawShader", pShader);

	// ParticleRenderShader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\particlerender.fx", "VS_ParticleRender");
	pShader->CreateGeometryShader(L"shader\\particlerender.fx", "GS_ParticleRender");
	pShader->CreatePixelShader(L"shader\\particlerender.fx", "PS_ParticleRender");

	// 이제 점 Mesh를 사용하기 때문에 Topology를 POINTLIST로 전달해준다. 
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetBSType(BS_TYPE::ALPHABLEND);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddRes<CGrapicsShader>(L"ParticleRenderShader", pShader);

	// postprocess
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_PostProcess");
	pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_PostProcess");
	
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	//pShader->SetRSType(RS_TYPE::CULL_NONE);
	//pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POST_PROCESS);
	
	AddRes<CGrapicsShader>(L"PostProcessShader", pShader);

	// TileMap Shader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\tilemap.fx", "VS_TileMap");
	pShader->CreatePixelShader(L"shader\\tilemap.fx", "PS_TileMap");

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);
	pShader->SetRSType(RS_TYPE::CULL_NONE);

	AddRes<CGrapicsShader>(L"TileMapShader", pShader);

	// Std3DShader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\std3d.fx", "VS_Std3D");
	pShader->CreatePixelShader(L"shader\\std3d.fx", "PS_Std3D");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);

	pShader->AddScalarParam(FLOAT_0, "Specular Coefficient");
	pShader->AddTexureParam(TEX_0, "Output Texture      ");
	pShader->AddTexureParam(TEX_1, "Normal Texture      ");

	AddRes<CGrapicsShader>(L"Std3DShader", pShader);

	// Decal Shader (forward)
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\Decal.fx", "VS_Decal");
	pShader->CreatePixelShader(L"shader\\Decal.fx", "PS_Decal");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);		// 볼륨 매시 안에 파고들어도 데칼이 사라지면 안되므로 CULL_FRONT를 넣어주어야 한다.
	pShader->SetBSType(BS_TYPE::ALPHABLEND);		// 색을 남겨 알파 블랜딩을 해주어야 한다.
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);	// 깊이를 남기면 안되고, 테스트도 하면 안된다.
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DECAL);

	pShader->AddTexureParam(TEX_1, "Decal Texture");

	AddRes<CGrapicsShader>(L"DecalShader", pShader);

	// SkyBox Shader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\SkyBox.fx", "VS_SkyBox");
	pShader->CreatePixelShader(L"shader\\SkyBox.fx", "PS_SkyBox");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);			// 스카이 박스 내부에서 보는 거기 떄문에 CULL_BACK을 하면 안된다.
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS_EQUAL);			// 스카이 박스가 카메라의 far끝에 위치하기 때문에 LESS로 하면 짤린다. 자새히 설명하면, TargetClear시 깊이를 1로 셋팅해주는데, 매시의 표면이 1로 걸리기 때문에 해당 부분이 Depth Test를 통과하지 못해 랜더링이 되지 않게 된다.
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);	// 지금 스카이 박스는 Forward 랜더링 방식으로 하기 때문에 DOMAIN_OPAQUE를 넣어준다.

	AddRes<CGrapicsShader>(L"SkyBoxShader", pShader);

	// Tess Shader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\Tess.fx", "VS_TESS");
	pShader->CreateHullShader(L"shader\\tess.fx", "HS_TESS");
	pShader->CreateDomainShader(L"shader\\tess.fx", "DS_TESS");
	pShader->CreatePixelShader(L"shader\\Tess.fx", "PS_TESS");

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	pShader->SetRSType(RS_TYPE::WIER_FRAME);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_OPAQUE);

	pShader->AddScalarParam(INT_0, "Tess Vtx0 Factor  ");
	pShader->AddScalarParam(INT_1, "Tess Vtx1 Factor  ");
	pShader->AddScalarParam(INT_2, "Tess Vtx2 Factor  ");
	pShader->AddScalarParam(INT_3, "Tess Inside Factor");

	AddRes<CGrapicsShader>(L"TessShader", pShader);



	// ====== 지연 렌더링 셰이더 ======
	// Std3D_Deferred Shader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\std3d_deferred.fx", "VS_Std3D_Deferred");
	pShader->CreatePixelShader(L"shader\\std3d_deferred.fx", "PS_Std3D_Deferred");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQIE);

	pShader->AddScalarParam(FLOAT_0, "Specular Coefficient");
	pShader->AddTexureParam(TEX_0, "Output Texture      ");
	pShader->AddTexureParam(TEX_1, "Normal Texture      ");

	AddRes<CGrapicsShader>(L"Std3D_DeferredShader", pShader);

	// DirLight Shader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\light.fx", "VS_DirLightShader");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_DirLightShader");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	// 빛은 계속해서 누적되어야 하기 때문에 OneOne블랜드를 사용해 가산혼합식을 사용한다.
	pShader->SetBSType(BS_TYPE::ONE_ONE);
	// 빛은 깊이 값을 남기면 안되고, 테스트도 하면 안되기 때문에 NTNW를 사용한다.
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHT);

	AddRes<CGrapicsShader>(L"DirLightShader", pShader);

	// PointLight Shader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\light.fx", "VS_PointLightShader");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_PointLightShader");

	// 볼륨 매시 안에 카메라가 파고들어도 빛이 사라지면 안되기 때문에
	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	// 빛은 계속해서 누적되어야 하기 때문에 OneOne블랜드를 사용해 가산혼합식을 사용한다.
	pShader->SetBSType(BS_TYPE::ONE_ONE);
	// 빛은 깊이 값을 남기면 안되고, 테스트도 하면 안되기 때문에 NTNW를 사용한다.
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHT);

	AddRes<CGrapicsShader>(L"PointLightShader", pShader);

	// SpotLight Shader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\light.fx", "VS_SpotLightShader");
	pShader->CreatePixelShader(L"shader\\light.fx", "PS_SpotLightShader");

	// 볼륨 매시 안에 카메라가 파고들어도 빛이 사라지면 안되기 때문에
	pShader->SetRSType(RS_TYPE::CULL_FRONT);
	// 빛은 계속해서 누적되어야 하기 때문에 OneOne블랜드를 사용해 가산혼합식을 사용한다.
	pShader->SetBSType(BS_TYPE::ONE_ONE);
	// 빛은 깊이 값을 남기면 안되고, 테스트도 하면 안되기 때문에 NTNW를 사용한다.
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_LIGHT);

	AddRes<CGrapicsShader>(L"SpotLightShader", pShader);

	// Deferred_Merge Shader
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\deferred_merge.fx", "VS_Merge");
	pShader->CreatePixelShader(L"shader\\deferred_merge.fx", "PS_Merge");

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	// 깊이를 남기지 않는 이유는 Deferred랜더링 이후 포워드 랜더링방식의 물체를 랜더링하는데,
	// 우리가 Merge.fx의 Shader코드에서 정점의 z를 0으로 두었기 때문에 깊이가 0이 남겨질 것이다.
	// 이때, 깊이가 0인것이 제일 가까운 물체이므로 병합이후 물체가 랜더링될 때, 깊이 테스트를 통과하지 못해서 출력되지 않을 것이다.
	// 또 테스트를 안하는 이유는 우연찮게 깊이가 0으로 그려진 물체가 있다면, 의도치 않은 상황이 발생할 수 있기 때문에 확실히 해주기 위함이다.
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);

	// 해당 셰이더는 물체를 그릴 셰이더가 아니므로 도메인은 NONE이다.
	pShader->SetDomain(SHADER_DOMAIN::NONE);

	// 해당 셰이더는 물체를 그릴 셰이더가 아닌 MRT의 랜더 타겟 병합 목적이므로
	// 파라미터를 공개할 필요가 없다.

	AddRes<CGrapicsShader>(L"Deferred_MergeShader", pShader);

	// Deferred_Decal Shader (Deferred)
	pShader = new CGrapicsShader;
	pShader->CreateVertexShader(L"shader\\Decal.fx", "VS_DeferredDecal");
	pShader->CreatePixelShader(L"shader\\Decal.fx", "PS_DeferredDecal");

	pShader->SetRSType(RS_TYPE::CULL_FRONT);		// 볼륨 매시 안에 파고들어도 데칼이 사라지면 안되므로 CULL_FRONT를 넣어주어야 한다.
	pShader->SetBSType(BS_TYPE::ALPHABLEND);		// 색을 남겨 알파 블랜딩을 해주어야 한다.
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);	// 깊이를 남기면 안되고, 테스트도 하면 안된다.
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL);

	pShader->AddTexureParam(TEX_1, "Decal Texture");

	AddRes<CGrapicsShader>(L"Deferred_DecalShader", pShader);
}

#include "CPaintShader.h"
#include "CParticleUpdateShader.h"
void CResMgr::CreateDefaultComputeShader()
{
	CComputeShader* pShader = nullptr;

	pShader = new CPaintShader;
	pShader->CreateComputeShader(L"shader\\compute.fx", "CS_Paint");
	AddRes<CComputeShader>(L"PaintShader", pShader);

	// Particle Update Shader
	pShader = new CParticleUpdateShader;
	pShader->CreateComputeShader(L"shader\\particleupdate.fx", "CS_ParticleUpdate");
	AddRes<CComputeShader>(L"ParticleUpdateShader", pShader);
}

void CResMgr::CreateDefaultMaterial()
{
	CMaterial* pMtrl = nullptr;

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"Std2DShader"));
	AddRes<CMaterial>(L"Std2DMtrl", pMtrl);

	//Load<CMaterial>(L"Std2DMtrl", L"material\\mtrl.mtrl");

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"Std2DAlphaBlendShader"));
	AddRes<CMaterial>(L"Std2DAlphaBlendMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"GridShader"));
	AddRes<CMaterial>(L"GridMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"DebugDrawShader"));
	AddRes<CMaterial>(L"DebugDrawMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"FrustumDebugDrawShader"));
	AddRes<CMaterial>(L"FrustumDebugDrawMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"ParticleRenderShader"));
	AddRes<CMaterial>(L"ParticleRenderMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"PostProcessShader"));
	AddRes<CMaterial>(L"PostProcessMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"TileMapShader"));
	AddRes<CMaterial>(L"TileMapMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"Std3DShader"));
	AddRes<CMaterial>(L"Std3DMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"Std3D_DeferredShader"));
	AddRes<CMaterial>(L"Std3D_DeferredMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"Deferred_MergeShader"));
	AddRes<CMaterial>(L"Deferred_MergeMtrl", pMtrl);
	
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"DirLightShader"));
	AddRes<CMaterial>(L"DirLightMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"PointLightShader"));
	AddRes<CMaterial>(L"PointLightMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"SpotLightShader"));
	AddRes<CMaterial>(L"SpotLightMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"DecalShader"));
	AddRes<CMaterial>(L"DecalMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"Deferred_DecalShader"));
	AddRes<CMaterial>(L"Deferred_DecalMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"SkyBoxShader"));
	AddRes<CMaterial>(L"SkyBoxMtrl", pMtrl);

	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGrapicsShader>(L"TessShader"));
	AddRes<CMaterial>(L"TessMtrl", pMtrl);
}

void CResMgr::AddInputLayout(DXGI_FORMAT _eFormat, const char* _strSemanticName)
{
	D3D11_INPUT_ELEMENT_DESC LayoutDesc = {};
	LayoutDesc.AlignedByteOffset = m_iLayoutOffset;
	LayoutDesc.Format = _eFormat;
	LayoutDesc.InputSlot = 0;
	LayoutDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc.SemanticName = _strSemanticName;
	LayoutDesc.SemanticIndex = 0;
	m_vecLayoutInfo.push_back(LayoutDesc);

	m_iLayoutOffset += GetSizeofFormat(_eFormat);
}

bool CResMgr::DeleteRes(RES_TYPE _Type, const wstring& _strKey)
{
	map<wstring, Ptr<CRes>>::iterator iter = m_arrRes[(UINT)_Type].find(_strKey);

	if (m_arrRes[(UINT)_Type].end() != iter)
	{
		m_arrRes[(UINT)_Type].erase(iter);
		m_bChanged = true;
		return true;
	}

	return false;
}