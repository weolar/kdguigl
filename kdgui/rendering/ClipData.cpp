
#include <UIlib.h>
#include <wtf/UtilHelp.h>
#include "ClipData.h"
#include "Graphics/KdPath.h"

ClipData::~ClipData() {
	if (m_bDeleteIfUninit)
		UHDeletePtr(&m_path);
}