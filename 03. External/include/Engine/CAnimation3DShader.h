#pragma once
#include "CComputeShader.h"

class CStructuredBuffer;

class CAnimation3DShader :
	public CComputeShader
{
private:
	CStructuredBuffer* m_pFrameDataBuffer;			// t16
	CStructuredBuffer* m_pOffsetMatBuffer;			// t17
	CStructuredBuffer* m_pBoneCheckBuffer;			// t18
	CStructuredBuffer* m_pOutputBuffer;				// u0

public:
	// g_int_0 : BonCount, g_int_1 : Frame Index

	void SetFrameIndex(int _iFrameIdx) { m_Param.iArr[1] = _iFrameIdx; }
	void SetNextFrameIdx(int _iFrameIdx) { m_Param.iArr[2] = _iFrameIdx; }
	 //void SetExtraAnimBool(int _bHasExtraAnim) { m_Param.iArr[2] = _bHasExtraAnim; }
	 //void SetUpperAnimBool(int _bIsUpperAnim) { m_Param.iArr[3] = _bIsUpperAnim; }
	void SetFrameRatio(float _fFrameRatio) { m_Param.fArr[0] = _fFrameRatio; }


	void SetBoneCount(int _iBoneCount) { m_Param.iArr[0] = _iBoneCount; }
	void SetFrameIdxRatio(int _vIdx, float _fRatio) { m_Param.v2Arr[0] = Vec2((float)_vIdx, _fRatio); }
	void SetFrameIdxRatioLower(int _vIdx, float _fRatio) { m_Param.v2Arr[1] = Vec2((float)_vIdx, _fRatio); }
	void SetBoneBlendCheck(int _iBlendCheck) { m_Param.iArr[1] = _iBlendCheck; }

	void SetSklRootMatrixUpperInv(const Matrix& _matrix) { m_Param.matArr[0] = _matrix; }
	void SetSklRootMatrixUpperNextInv(const Matrix& _matrix) { m_Param.matArr[1] = _matrix; }
	void SetSklRootMatrixLower(const Matrix& _matrix) { m_Param.matArr[2] = _matrix; }
	void SetSklRootMatrixLowerNext(const Matrix& _matrix) { m_Param.matArr[3] = _matrix; }

	void SetEquipableType(int _iOption) { m_Param.iArr[2] = _iOption; }
	void SetBoneUseCheckBuffer(int _iUse) { m_Param.iArr[3] = _iUse; } // use : 1 | not use : 0

	void SetFrameDataBuffer(CStructuredBuffer* _buffer) { m_pFrameDataBuffer = _buffer; }
	void SetOffsetMatBuffer(CStructuredBuffer* _buffer) { m_pOffsetMatBuffer = _buffer; }
	void SetBoneCheckBuffer(CStructuredBuffer* _buffer) { m_pBoneCheckBuffer = _buffer; }
	void SetOutputBuffer(CStructuredBuffer* _buffer) { m_pOutputBuffer = _buffer; }

public:
	virtual void UpdateData();
	virtual void Clear();
	virtual void Execute() override;

public:
	CAnimation3DShader();
	virtual ~CAnimation3DShader();
};


