#ifndef OffScreenDC_h
#define OffScreenDC_h

class OffScreenDC {
public:
	HDC					m_hdc;
	HBITMAP				m_hBmp;

	OffScreenDC () {
		m_hdc = NULL;
		m_hBmp = NULL;
	}

	~OffScreenDC () {
		if (m_hdc) {
			::DeleteDC(m_hdc);
			m_hdc = NULL;
		}

		if (m_hBmp) {
			::DeleteObject(m_hBmp);
			m_hBmp = NULL;
		}        
	}

	void Init(HDC hdc, LONG xWidth, LONG yHeight) {
		m_hdc = ::CreateCompatibleDC(hdc);
		m_hBmp = ::CreateCompatibleBitmap(m_hdc, xWidth, yHeight);
		::SelectObject(m_hdc, m_hBmp);
	}

	void ZeroMem() {
		RECT rect = {0, 0, 100, 50};
		FillRect(m_hdc, &rect, (HBRUSH) (COLOR_WINDOW+1));

		DWORD dwSize = ::GetBitmapBits(m_hBmp, 0, NULL);
		char* pMem = new char[dwSize];
		::GetBitmapBits(m_hBmp, dwSize, pMem);
		
		memset(pMem, 0, dwSize);

		::SetBitmapBits(m_hBmp, dwSize, pMem);
		delete[] pMem;
		pMem = NULL;
	}

	BOOL Realloc(LONG xWidth, LONG yHeight) {
		HBITMAP hbmpNew = ::CreateCompatibleBitmap(m_hdc, xWidth, yHeight);
		::SelectObject(m_hdc, hbmpNew);
		::DeleteObject(m_hBmp);
		m_hBmp = hbmpNew;

		return TRUE;
	}
};

#endif // OffScreenDC_h