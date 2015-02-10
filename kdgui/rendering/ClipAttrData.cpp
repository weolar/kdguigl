
#include <UIlib.h>
#include <wtf/UtilHelp.h>
#include "ClipAttrData.h"
#include "Graphics/KdPath.h"

ClipAttrData::ClipAttrData() 
	: m_bURL(false)
	, m_path(NULL) {}

ClipAttrData::~ClipAttrData() {
	UHDeletePtr(&m_path);
}

void ClipAttrData::SetUrl(const CStdString& url) {
	m_bURL = true;
	m_url = url;

	UHDeletePtr(&m_path);
}

KdPath* ClipAttrData::ToPath() {
	m_bURL = true;
	m_url.Empty();
	UHDeletePtr(&m_path);

	m_path = new KdPath();
	return m_path;
}