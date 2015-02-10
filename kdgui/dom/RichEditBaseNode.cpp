
#include <richedit.h>
#include <TextServ.h>

#include <WTF/UtilHelp.h>
#include "core/UIManager.h"
#include "rendering/RenderSupport.h"
#include "graphics/GraphicsContext.h"

#include "RichEditBaseNode.h"
#include "RichEditHost.h"

RichEditBaseNode::RichEditBaseNode() {
	m_richeditRender = NULL;
}

RichEditBaseNode::~RichEditBaseNode() {
	UHDeletePtr(&m_richeditRender);
}

void RichEditBaseNode::Paint(GraphicsContext* g, const IntRect &rcInvalid) {
	RenderCommBegin();

	Init();
	m_richeditRender->Paint(g, rcInvalid);

	RenderCommEnd();
}

int RichEditBaseNode::Event(TEventUI& event, BOOL bCapturePhase) {
	Init();

	if (bCapturePhase) // 下发和冒泡，只需要响应一遍即可
		return 0;
	
	return m_richeditRender->Event(&event);
}

void RichEditBaseNode::Init() {
	if (!m_richeditRender)
		m_richeditRender = new RichEditHost(this);

	if (m_richeditRender->HasInit())
		return;

	m_richeditRender->SetCanRepaint(false);

	CREATESTRUCT defaultCs = {0};
	defaultCs.style = ES_MULTILINE /*| WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_NOHIDESEL*/;
	defaultCs.lpszName = L"richedit init..";
	IntRect rc = BoundingRectInLocalCoordinates();
	defaultCs.x = rc.x();
	defaultCs.y = rc.y();
	defaultCs.cx = rc.width();
	defaultCs.cy = rc.height();

	m_richeditRender->Init(GetManager()->GetHWND(), &defaultCs, NULL);
	m_richeditRender->SetTransparent(TRUE);

	m_richeditRender->SetCanRepaint(true);
}