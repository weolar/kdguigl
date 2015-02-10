#ifndef KdPageInfo_h
#define KdPageInfo_h

typedef struct _KdPageDebugInfo {
    unsigned long addr;
    unsigned short port;
} KdPageDebugInfo, *KdPageDebugInfoPtr;

typedef struct _KdPageInfo {
    DWORD dwStyle;
    DWORD dwExStyle;
    int X;
    int Y;
    int nWidth;
    int nHeight;
    //KdParentPageInfo Parent;
    HWND hWndParent;
    void* pageContext;

    KdPageDebugInfo DebugInfo;
} KdPageInfo, *KdPageInfoPtr;

#endif // KdPageInfo_h


