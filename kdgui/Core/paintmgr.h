#ifndef PaintMgr_h
#define PaintMgr_h

#define GL_GLEXT_PROTOTYPES

#include <WTF/FastMalloc.h>
#include "PaintMgrHook.h"

#include <GLES2/gl2.h>
#ifdef _MSC_VER
#include <GLES2/gl2ext.h>
#include <GL/gl.h>
#include "graphics/GLext/glext.h"
#endif
#include "graphics/nanovg/nanovg_gl.h"

//////////////////////////////////////////////////////////////////////////

void drawEyes(NVGcontext* vg, float x, float y, float w, float h, float mx, float my, float t)
{
	NVGpaint gloss, bg;
	float ex = w *0.23f;
	float ey = h * 0.5f;
	float lx = x + ex;
	float ly = y + ey;
	float rx = x + w - ex;
	float ry = y + ey;
	float dx,dy,d;
	float br = (ex < ey ? ex : ey) * 0.5f;
	float blink = 1 - pow(sinf(t*0.5f),200)*0.8f;

	bg = nvgLinearGradient(vg, x,y+h*0.5f,x+w*0.1f,y+h, nvgRGBA(0,0,0,32), nvgRGBA(0,0,0,16));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx+3.0f,ly+16.0f, ex,ey);
	nvgEllipse(vg, rx+3.0f,ry+16.0f, ex,ey);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	bg = nvgLinearGradient(vg, x,y+h*0.25f,x+w*0.1f,y+h, nvgRGBA(220,220,220,255), nvgRGBA(128,128,128,255));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx,ly, ex,ey);
	nvgEllipse(vg, rx,ry, ex,ey);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	dx = (mx - rx) / (ex * 10);
	dy = (my - ry) / (ey * 10);
	d = sqrtf(dx*dx+dy*dy);
	if (d > 1.0f) {
		dx /= d; dy /= d;
	}
	dx *= ex*0.4f;
	dy *= ey*0.5f;
	nvgBeginPath(vg);
	nvgEllipse(vg, lx+dx,ly+dy+ey*0.25f*(1-blink), br,br*blink);
	nvgFillColor(vg, nvgRGBA(32,32,32,255));
	nvgFill(vg);

	dx = (mx - rx) / (ex * 10);
	dy = (my - ry) / (ey * 10);
	d = sqrtf(dx*dx+dy*dy);
	if (d > 1.0f) {
		dx /= d; dy /= d;
	}
	dx *= ex*0.4f;
	dy *= ey*0.5f;
	nvgBeginPath(vg);
	nvgEllipse(vg, rx+dx,ry+dy+ey*0.25f*(1-blink), br,br*blink);
	nvgFillColor(vg, nvgRGBA(32,32,32,255));
	nvgFill(vg);

	gloss = nvgRadialGradient(vg, lx-ex*0.25f,ly-ey*0.5f, ex*0.1f,ex*0.75f, nvgRGBA(255,255,255,128), nvgRGBA(255,255,255,0));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx,ly, ex,ey);
	nvgFillPaint(vg, gloss);
	nvgFill(vg);

	gloss = nvgRadialGradient(vg, rx-ex*0.25f,ry-ey*0.5f, ex*0.1f,ex*0.75f, nvgRGBA(255,255,255,128), nvgRGBA(255,255,255,0));
	nvgBeginPath(vg);
	nvgEllipse(vg, rx,ry, ex,ey);
	nvgFillPaint(vg, gloss);
	nvgFill(vg);
}

NVGcontext* gCanvas = 0;

void TestDraw() {
	glViewport(0, 0, 720, 1280);
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	if (!gCanvas)
		gCanvas = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

	nvgBeginFrame(gCanvas, 720, 1280, 1);
	drawEyes(gCanvas, 10, 12, 100, 100, 5, 5, 100);
	nvgEndFrame(gCanvas);

	glEnable(GL_DEPTH_TEST);
}
//////////////////////////////////////////////////////////////////////////

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

class PaintMgr {
public:
	PaintMgr(CPageManager* manager) 
		: m_manager(manager)
		, m_memoryCanvas(NULL)
#ifdef _MSC_VER
		, m_hGLContext(NULL)
#else
		, m_hMainLoopWakeUpEvent(WTF::CreateEvent(TRUE))
#endif
		, m_scheduleMessageCount(0)
		, m_bUseLayeredBuffer(false)
		, m_bIntoPostPaintMsgWhenSchedule(false)
		, m_bHasResize(false)
		, m_bdColor(RGB(0x4c, 0x4c, 0x4c)|0xff000000)
		, m_rcHighLight(0)
		, m_bRequestRender(true)
		, m_schedulePaintCount(0) {}

	~PaintMgr () {
		m_paintMessageQueue.clear();
		m_manager = NULL;

		WTF::CloseEventHandle(m_hMainLoopWakeUpEvent);

		DebugBreak();
		//UHDeletePtr(&m_memoryCanvas);
		UHDeletePtr(&m_rcHighLight);
	}

	void Init() {
#ifndef _MSC_VER
		ResetMemoryCanvas(NULL);
#endif
	}

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

	// Merge any areas that would reduce the total area
	void MergeDirtyList() { while ( DoMergeDirtyList(false) ) {}; }

	bool DoMergeDirtyList(bool forceMerge) {
		int nDirty = (int)m_paintMessageQueue.size();
		if (nDirty < 1)
			return false;

		int bestDelta = forceMerge ? 0x7FFFFFFF : 0;
		int mergeA = 0;
		int mergeB = 0;
		for (int i = 0; i < nDirty-1; i++) {
			for ( int j = i+1; j < nDirty; j++ ) {
				int delta = unionArea(&m_paintMessageQueue[i], &m_paintMessageQueue[j]) -
					m_paintMessageQueue[i].rectArea() - m_paintMessageQueue[j].rectArea();
				if ( bestDelta > delta ) {
					mergeA = i;
					mergeB = j;
					bestDelta = delta;
				}
			}
		}

		if (mergeA != mergeB) {
			m_paintMessageQueue[mergeA].unite((m_paintMessageQueue[mergeB]));
			for ( int i = mergeB+1; i < nDirty; i++ )
				(m_paintMessageQueue[i-1]) = (m_paintMessageQueue[i]);

			m_paintMessageQueue.pop_back();
			return true;
		}

		return false;
	}

	void SchedulePaintEvent() {
		KDASSERT (0 == m_scheduleMessageCount);
		m_scheduleMessageCount++;

		HDC hDC = NULL;
		IntRect lastTimeRect;

#ifndef _MSC_VER	
		PaintBegin(NULL);
		DoPaint(NULL, &m_clientRect);
		PaintEnd();
#else
		if (!m_manager->GetHWND()) 
			return;

		bool bAgainLoop = false;
		do {
			bAgainLoop = false;
			m_bIntoPostPaintMsgWhenSchedule = false;
			lastTimeRect.setSize(IntSize());
			MergeDirtyList();

			size_t queueSize = m_paintMessageQueue.size();
			for (size_t i = 0; i < queueSize; ++i) {
				IntRect* paintRect = &m_paintMessageQueue[i];
				if (lastTimeRect == *paintRect || lastTimeRect.contains(*paintRect))
					continue;

				lastTimeRect = *paintRect;
				if (lastTimeRect.isEmpty()) // 这里可能会重入PostPaintMessage，所以需要重新合并脏矩形，并且小心一些bug
					break;
#ifdef _MSC_VER
				if (!hDC)
					hDC = ::GetDC(m_manager->GetHWND());
#endif
				DoPaint(hDC, &lastTimeRect);
				// 这里还有优化空间，可以把很短间隔的差距不大的矩形忽略掉
				if (queueSize != m_paintMessageQueue.size()) { // 如果在绘制的时候被请求了脏矩形，则重新合并
					m_paintMessageQueue.erase(0, i);
					bAgainLoop = true;
					break;
				}
			}
		} while (bAgainLoop);

		if (hDC)
			::ReleaseDC(m_manager->GetHWND(), hDC);		
#endif
		hDC = NULL;
		m_paintMessageQueue.clear();
		m_scheduleMessageCount--;
	}

	BOOL CreateViewGLContext(HDC hDC) {
#ifdef _MSC_VER
		m_hGLContext = ::wglCreateContext(hDC);//用当前DC产生绘制环境(RC)
		if (!m_hGLContext)
			return FALSE;

		return ::wglMakeCurrent(hDC, m_hGLContext);
#endif
		return TRUE;
	}

	BOOL SetWindowPixelFormat(HDC hDC) {
#ifdef _MSC_VER
		PIXELFORMATDESCRIPTOR pixelDesc = {
			sizeof(PIXELFORMATDESCRIPTOR), 1,
			PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|
			PFD_DOUBLEBUFFER|PFD_SUPPORT_GDI,
			PFD_TYPE_RGBA,
			24, 0,0,0,0,0,0, 0, 0, 0,
			0,0,0,0, 32, 0, 0,
			PFD_MAIN_PLANE, 0, 0,0,0
		};

		m_GLPixelIndex = ::ChoosePixelFormat(hDC,&pixelDesc);
		if(m_GLPixelIndex == 0) {
			m_GLPixelIndex = 1;
			if(0 == ::DescribePixelFormat(hDC,m_GLPixelIndex, sizeof(PIXELFORMATDESCRIPTOR),&pixelDesc))
				return FALSE;
		}

		return ::SetPixelFormat(hDC,m_GLPixelIndex, &pixelDesc);
#endif
		return TRUE;
	}

	int LoadFonts(NVGcontext* vg) {
		int font;
		font = nvgCreateFont(vg, "DroidSansFallback", "/system/fonts/DroidSansFallback.ttf");
		if (font == -1) {
			LOGI("Could not add font regular.\n");
			return -1;
		}

		// 	font = nvgCreateFont(vg, "sans-bold", "../example/Roboto-Bold.ttf");
		// 	if (font == -1) {
		// 		printf("Could not add font bold.\n");
		// 		return -1;
		// 	}
		return 0;
	}

	void ResetMemoryCanvas(HDC hDC) {
		if (m_memoryCanvas && !m_bHasResize)
			return;

		m_invalidRect = m_clientRect;

		if (!SetWindowPixelFormat (hDC))
			return;

		if (!CreateViewGLContext (hDC))
			return;

		//nvgDeleteGL2(m_memoryCanvas);
#ifdef _MSC_VER
		m_memoryCanvas = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);//(NVGcontext*(m_clientRect.width(), m_clientRect.height(), !m_bUseLayeredBuffer));
#else
		m_memoryCanvas = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
		LoadFonts(m_memoryCanvas);
#endif
		m_bHasResize = false;
	}

	void PaintBegin(HDC hDC) {
		ResetMemoryCanvas(hDC);

		glViewport(0, 0, m_clientRect.width(), m_clientRect.height());
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		nvgBeginFrame(m_memoryCanvas, m_clientRect.width(), m_clientRect.height(), 1);
 	}

	void PaintEnd() {
		glEnable(GL_DEPTH_TEST);
		nvgEndFrame(m_memoryCanvas);
	}

	void DoPaint(HDC hDC, const IntRect* paintRect) {
		m_invalidRect = *paintRect;
	
		m_invalidRect.intersect(m_clientRect);
		if (m_invalidRect.isEmpty())
			return;
#ifdef _MSC_VER
		PaintBegin(hDC);
#endif
		//ClearPaintWhenLayeredWindow(m_memoryCanvas, m_invalidRect);
		//HDC hMemoryDC = skia::BeginPlatformPaint(m_memoryCanvas);

		DoPaintContents(m_invalidRect, m_memoryCanvas);

#ifdef _MSC_VER
		::SwapBuffers(hDC);
		PaintEnd();
#endif
		if (m_manager->HasSvgInit() && !m_manager->HasJsonReady() && m_manager->m_callbacks.m_xmlHaveFinished)
			m_manager->PostDelayTask(new DelayCallJsonInitTask());
	}

	void DoPaintContents(const IntRect& dirtyRect, NVGcontext* pCanvas) {
		m_bRequestRender = true;
		PlatformContextNanovg context(pCanvas);

		GraphicsContext gc(reinterpret_cast<PlatformGraphicsContext*>(&context));
		gc.save();

#ifdef _MSC_VER
		//gc.clip(dirtyRect);
		if (!m_bUseLayeredBuffer)
			gc.fillRect(dirtyRect, KdColor(m_bdColor), ColorSpaceDeviceRGB);
#endif
		m_manager->GetRootTemporary()->Paint(&gc, dirtyRect); // 绘制脏矩形

		//gc.strokeRect(dirtyRect, 1);

		// 绘制调试高亮
		if (m_rcHighLight)
			gc.fillRect(*m_rcHighLight,  KdColor(160, 297, 232, 170), ColorSpaceDeviceRGB);
		
		gc.restore();
	}

	void PostClientPaintMessage() {
		PostPaintMessage(&m_clientRect);
	}

	void PostPaintMessage(const IntRect* paintRect) {
		if (!paintRect || paintRect->isEmpty())
			return;
		m_postpaintMessageCount++;

		// 从SchedulePaintEvent->DoPaint，元素的绘制（paint）时候发送过来的，重入了，此时如果队列太满
		// 需要删除，但如果是重入状态，轻易删除元素可能会产生bug，所以直接返回，不绘制这区域
		if (0 != m_scheduleMessageCount)
			m_bIntoPostPaintMsgWhenSchedule = true;
		
		if (m_paintMessageQueue.size() > m_maxPaintMessageQueueSize && m_bIntoPostPaintMsgWhenSchedule)
			return;

		if (m_paintMessageQueue.size() > m_maxPaintMessageQueueSize) {
			IntRect* destroyRect = &m_paintMessageQueue[0];
			m_paintMessageQueue.remove(0);
		}

		// TODO 脏矩形合并
		for (int i = 0; i < (int)m_paintMessageQueue.size(); ++i) {
			IntRect* paintRectFromQueue = &m_paintMessageQueue[i];
			if (paintRectFromQueue == paintRect)
				paintRectFromQueue->setWidth(0);
		}
		
		m_paintMessageQueue.push_back(*paintRect);
		m_postpaintMessageCount--;

		if (0 == m_scheduleMessageCount) // 只有在非重入状态才能合并脏矩形
			MergeDirtyList();
	}

	void OnPaintEvent(HDC hDC, const RECT* paintRect) {
		if (!m_memoryCanvas || m_clientRect.isEmpty())
			return;

		IntRect rc(*paintRect);
		DoPaint(hDC, &rc);

// 		if (m_bUseLayeredBuffer)
// 			skia::DrawToNativeLayeredContext(m_memoryCanvas, dc, &((RECT)*paintRect), &((RECT)m_clientRect));
// 		else
// 			skia::DrawToNativeContext(m_memoryCanvas, dc, 0, 0, &((RECT)m_clientRect));
	}

	void CopyMemoryDC(HDC dc, const RECT* paintRect) {
		if (!m_memoryCanvas || m_clientRect.isEmpty())
			return;

		//skia::DrawToNativeContext(m_memoryCanvas, dc, 0, 0, &((RECT)m_clientRect));
	}

	void SetClientRectAndInvalideta(const IntRect& clientRect) {
		if (clientRect.isEmpty() || m_clientRect == clientRect)
			return;
		
		m_clientRect = clientRect;
		//m_bHasResize = true;
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
			PostPaintMessage(&m_clientRect);
	}

	RGBA32 m_bdColor;
	bool m_bUseLayeredBuffer;
	bool m_bRequestRender;
	OS_EVENT_HANDLE m_hMainLoopWakeUpEvent;
	NVGcontext* m_memoryCanvas;

protected:
	WTF::Vector<IntRect> m_paintMessageQueue;
	static const int m_maxPaintMessageQueueSize = 200;

	int m_postpaintMessageCount;
	int m_scheduleMessageCount;
	bool m_bIntoPostPaintMsgWhenSchedule; // 当调度绘制的时候发生重入post paint

	bool m_bHasResize;

	CPageManager* m_manager;

	IntRect m_clientRect;
	
#ifdef _MSC_VER
	HGLRC m_hGLContext;
#endif
	int m_GLPixelIndex;

	IntRect m_invalidRect;

	IntRect* m_rcHighLight;

	int m_schedulePaintCount;	
};

#endif // PaintMgr_h