#ifndef bitmap_cache_h
#define bitmap_cache_h

namespace skia {

struct BitmapCacheItem {
	int width;
	int height;
	HBITMAP hbitmap;
	void* data;

	void Reset() {
		width = -1;
		height = -1;
		hbitmap = NULL;
		data = NULL;
	}

	bool IsEmpty() {
		return !hbitmap;
	}

	void Set(int width, int height, HBITMAP hbitmap, void* data) {
		this->width = width;
		this->height = height;
		this->data = data;
		this->hbitmap = hbitmap;
	}
};

class BitmapCache {

protected:
	const static int m_nMaxItemSize = 10;
	BitmapCacheItem m_cacheItems[m_nMaxItemSize];
	int m_nInsertPos; // 插入新项的时候，插入的位置

	static void* CreateTLS() {
		return SkNEW(BitmapCache);
	}

	static void DeleteTLS(void* ptr) {
		BitmapCache* self = (BitmapCache*)ptr;
		SkDELETE(self);
	}
	
public:
	BitmapCache () {
		m_nInsertPos = 0;

		for (int i = 0; i < m_nMaxItemSize; ++i) {
			BitmapCacheItem* cacheItem = &m_cacheItems[i];
			cacheItem->Reset();
		}
	}

	static BitmapCache* GetTLS() {
		return (BitmapCache*)SkTLS::Get(CreateTLS, DeleteTLS);
	}

	static void DeleteTLS() { SkTLS::Delete(CreateTLS); }

	// 回收
	bool Recycle(HBITMAP hbitmap) {
		if (!hbitmap)
			return true;

		BITMAP bitmapData = {0};
		if (!::GetObject(hbitmap, sizeof(BITMAP), &bitmapData) || !bitmapData.bmBits) {
			::DeleteObject(hbitmap);
			return false;
		}

		// 先扫描一遍有没重复
		if (ScanForRepeat(hbitmap))
			return true;

		// 如果不是空的，则找个空隙插进去
		if (ScanForEmpty(hbitmap, bitmapData))
			return true;

		// 如果没有空的，则插入
		InsertOldItem(hbitmap, bitmapData);

		return true;
	}

	HBITMAP Create(int width, int height, void** data, HANDLE shared_section) {
		HBITMAP hbitmap = NULL;
		if (!shared_section)
			hbitmap = CreateFromCache(width, height, data);
		
		if (hbitmap)
			return hbitmap;

		HDC screen_dc = ::GetDC(NULL);
		hbitmap = BitmapPlatformDevice::createBitMap(screen_dc, width, height, NULL, data);
		if (!hbitmap) {
			::ReleaseDC(NULL, screen_dc);
			return NULL;
		}

		::ReleaseDC(NULL, screen_dc);
		return hbitmap;
	}

protected:
	void InsertOldItem(HBITMAP hbitmap, const BITMAP& bitmapData) {
		BitmapCacheItem* cacheItem = NULL;

		m_nInsertPos = (m_nInsertPos + 1)%m_nMaxItemSize;
		cacheItem = &m_cacheItems[m_nInsertPos];

		::DeleteObject(cacheItem->hbitmap);
		cacheItem->Set(bitmapData.bmWidth, bitmapData.bmHeight, hbitmap, bitmapData.bmBits);
	}

	bool ScanForRepeat(HBITMAP hbitmap) {
		for (int i = 0; i < m_nMaxItemSize; ++i) {
			BitmapCacheItem* cacheItem = &m_cacheItems[i];
			if (cacheItem->IsEmpty())
				continue;

			if (hbitmap == cacheItem->hbitmap)
				return true;
		}

		return false;
	}

	bool ScanForEmpty(HBITMAP hbitmap, const BITMAP& bitmapData) {
		BitmapCacheItem* cacheItem = NULL;
		for (int i = 0; i < m_nMaxItemSize; ++i) {
			cacheItem = &m_cacheItems[i];
			if (!cacheItem->IsEmpty())
				continue;

			cacheItem->Set(bitmapData.bmWidth, bitmapData.bmHeight, hbitmap, bitmapData.bmBits);
			return true;
		}

		return false;
	}

	HBITMAP CreateFromCache(int width, int height, void** data) {
		for (int i = 0; i < m_nMaxItemSize; ++i) {
			BitmapCacheItem* cacheItem = &m_cacheItems[i];
			if (!cacheItem->hbitmap || cacheItem->height != height || cacheItem->width != width)
				continue;

			// 如果缓存里有大小刚好的，走缓存，并且删掉缓存
			HBITMAP hbitmap = cacheItem->hbitmap;
			*data = cacheItem->data;
			cacheItem->Reset();
			return hbitmap;
		}

		return NULL;
	}
};

} // namespace skia

#endif // bitmap_cache_h