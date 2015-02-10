
#include "PaintMgrHook.h"

class DelayCallJsonInitTask : public PageManagerDelayTask {
	WTF_MAKE_FAST_ALLOCATED;
public:
	virtual void Run(CPageManager* manager, Moment moment) {
		if (ePMDTUniniting == moment || !manager->HasSvgInit() || manager->HasJsonReady() || !manager->m_callbacks.m_xmlHaveFinished)
			return;

		manager->SetJonsReady();
		manager->m_callbacks.m_xmlHaveFinished(manager->GetWrap(), NULL, manager->GetPagePtr(), manager->GetHWND());
	}

	virtual void Destroy() {delete this;}
};

class SysPaintMgr {
	CPageManager* m_manager;
	
	IntRect m_clientRect;
	skia::PlatformCanvas* m_memoryCanvas;

	IntRect m_invalidRect;
	SkRegion m_invalidRegion;
	IntRect m_winodwRect;

	IntRect* m_rcHighLight;

	bool m_bHasResize;

public:
	RGBA32 m_bdColor;
	bool m_bUseLayeredBuffer;
	bool m_bFirstPaint;

	SysPaintMgr(CPageManager* manager) 
		: m_manager(manager)
		, m_memoryCanvas(NULL)
		, m_bUseLayeredBuffer(false)
		, m_bHasResize(false)
		, m_bdColor(RGB(0x4c, 0x4c, 0x4c)|0xff000000)
		, m_bFirstPaint(true)
		, m_rcHighLight(NULL) {

	}

	~SysPaintMgr () {
		m_manager = NULL;

		UHDeletePtr(&m_memoryCanvas);
		UHDeletePtr(&m_rcHighLight);
	}

	void Init() {}

	void SchedulePaintEvent() {}

	void ClearPaintWhenLayeredWindow(skia::PlatformCanvas* canvas, const IntRect& rect) {
		if (!m_bUseLayeredBuffer)
			return;

		// When using transparency mode clear the rectangle before painting.
		SkPaint clearPaint;
		clearPaint.setARGB(0,0,0,0);
		clearPaint.setXfermodeMode(SkXfermode::kClear_Mode);

		SkRect skrc = SkRect::Make(SkIRect::MakeXYWH(rect.x(), rect.y(), rect.width(), rect.height()));
		canvas->drawRect(skrc, clearPaint);
	}

	void PostClientPaintMessage() {
		::PostMessage(m_manager->GetHWND(), WM_PAINT, 0, 0);
		PostPaintMessage(&m_clientRect);
	}

	void PostPaintMessage(const IntRect* paintRect) {
		m_invalidRect.unite(*paintRect);
		m_invalidRegion.op((SkIRect)*paintRect, SkRegion::kUnion_Op);

		::InvalidateRect(m_manager->GetHWND(), &(RECT)*paintRect, FALSE);
	}

	void DelayCreateCanvas () {
		if (m_memoryCanvas && !m_bHasResize)
			return;

		m_invalidRect = m_clientRect;

		UHDeletePtr(&m_memoryCanvas);
		m_memoryCanvas = (new skia::PlatformCanvas(m_clientRect.width(), m_clientRect.height(), !m_bUseLayeredBuffer));
		
		m_bHasResize = false;
	}

	void DoPaint(HDC psHdc, const IntRect* invalidateRect) {
		if (invalidateRect->isEmpty())
			return;

		DelayCreateCanvas ();

		IntRect paintRect = *invalidateRect;
		paintRect.intersect(m_clientRect);

		ClearPaintWhenLayeredWindow(m_memoryCanvas, paintRect);

		HDC hMemoryDC = skia::BeginPlatformPaint(m_memoryCanvas);

		PaintMgrPaintHook0();

		DoPaintContents(paintRect, m_memoryCanvas);

		PaintMgrPaintHook1();

		if (m_bUseLayeredBuffer) { // 再把内存dc画到psHdc上
			skia::DrawToNativeLayeredContext(m_memoryCanvas, psHdc, &((RECT)paintRect), &((RECT)m_clientRect));
		} else
			skia::DrawToNativeContext(m_memoryCanvas, psHdc, paintRect.x(), paintRect.y(), &((RECT)paintRect));

		PaintMgrPaintHook2();

Exit0:	
		skia::EndPlatformPaint(m_memoryCanvas);

		if (m_manager->HasSvgInit() && !m_manager->HasJsonReady() && m_manager->m_callbacks.m_xmlHaveFinished)
			m_manager->PostAsysTask(new DelayCallJsonInitTask());
	}

	void DoPaintContents(const IntRect& dirtyRect, skia::PlatformCanvas* pCanvas) {
		PlatformContextSkia context(pCanvas);

		GraphicsContext gc(reinterpret_cast<PlatformGraphicsContext*>(&context));
		//gc.setWindowsIsTransparencyLayer(m_bUseLayeredBuffer);
		gc.save();

		//gc.clip(dirtyRect);
		//gc.clip(m_invalidRegion);
		if (!m_bUseLayeredBuffer)
			gc.fillRect(dirtyRect, KdColor(m_bdColor), ColorSpaceDeviceRGB);

		m_manager->GetRootTemporary()->Paint(&gc, dirtyRect); // 绘制脏矩形

		// 绘制调试高亮
		if (m_rcHighLight)
			gc.strokeRect(*m_rcHighLight, 1);

		gc.restore();
	}

	void OnPaintEvent(HDC dc, const RECT* invalidateRect) {
		if (m_clientRect.isEmpty())
			return;

		IntRect paintRect(*invalidateRect);
		if (m_bFirstPaint || m_bUseLayeredBuffer && paintRect.isEmpty())
			paintRect = m_invalidRect;
		m_bFirstPaint = false;

		if (!m_invalidRect.isEmpty())
			DoPaint(dc, &paintRect);
		else {
			if (m_bUseLayeredBuffer)
				skia::DrawToNativeLayeredContext(m_memoryCanvas, dc, &((RECT)paintRect), &((RECT)m_clientRect));
			else
				skia::DrawToNativeContext(m_memoryCanvas, dc, paintRect.x(), paintRect.y(), &((RECT)paintRect));
		}

		m_invalidRect.clear();
		m_invalidRegion.setEmpty();
	}

	void CopyMemoryDC(HDC dc, const RECT* paintRect) {
		if (!m_memoryCanvas || m_clientRect.isEmpty())
			return;

		skia::DrawToNativeContext(m_memoryCanvas, dc, 0, 0, &((RECT)m_clientRect));
	}

	void SetClientRectAndInvalideta(const IntRect& clientRect) {
		if (clientRect.isEmpty() || m_clientRect == clientRect)
			return;

		m_clientRect = clientRect;
		m_bHasResize = true;
		m_manager->InvalidateAll();
	}

	static bool IsNodeValide(const UINode* parent, const UINode* n) {
		if (parent == n)
			return true;

		WTF::Vector<UINode*>* children = ((UINode*)parent)->GetChilds();
		if (!children)
			return false;

		for(size_t it = 0; it < children->size(); ++it) {
			UINode* child = children->at(it);
			if (child == n || IsNodeValide(child, n))
				return true;
		}

		return false;
	}

	void HighLightRing(UINode* n) {
		if (!IsNodeValide(m_manager->GetRootTemporary(), n))
			return;

		UHDeletePtr(&m_rcHighLight);
		m_rcHighLight = new IntRect(n->BoundingRectAbsolute());

		if (m_rcHighLight->isEmpty())
			UHDeletePtr(&m_rcHighLight);
		else
			PostPaintMessage(m_rcHighLight);
	}
};