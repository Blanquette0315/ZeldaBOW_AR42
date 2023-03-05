#include "pch.h"
#include "CEditorCam.h"

void CEditorCam::finaltick()
{
	CalcViewMat();

	CalcProjMat();

	GetFrustum().finaltick();
}