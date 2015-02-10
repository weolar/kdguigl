#ifndef ClipAttrData_h
#define ClipAttrData_h

class KdPath;

class ClipAttrData {
public:
	ClipAttrData();
	~ClipAttrData();
	bool IsURL() const {return m_bURL;}

	const CStdString& GetUrl() const {return m_url;}
	void SetUrl(const CStdString& url);

	KdPath* ToPath(); // 本函数会new出m_path，而GetPath不会
	const KdPath* GetPath() const {return m_path;}

protected:
	bool m_bURL;
	CStdString m_url;
	KdPath* m_path;
};

#endif // CClipAttrData_h