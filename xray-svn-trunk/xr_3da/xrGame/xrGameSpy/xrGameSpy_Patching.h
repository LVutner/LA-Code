#pragma once
#include "xrGameSpy_MainDefs.h"
#include "GameSpy/pt/pt.h"

extern "C"
{
	EXPORT_FN_DECL(bool, ptCheckForPatch, (
//		int productID,  const gsi_char * versionUniqueID,  int distributionID, 
		ptPatchCallback callback, 
		PTBool blocking, 
		void * instance ));
}