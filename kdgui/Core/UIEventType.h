#ifndef UIEventType_H
#define UIEventType_H

class UINode;

// 记得在UHEventNameToEnum添加对应的字符串名
enum UIEventType {
	UIEVENT__FIRST = 0,
	UIEVENT_MOUSEMOVE,
	UIEVENT_MOUSEHOVER,

	UIEVENT_MOUSEENTER,
	UIEVENT_MOUSELEAVE,
	
	UIEVENT_MOUSEDOWN,
	UIEVENT_MOUSEUP,
	UIEVENT_CLICK,

	UIEVENT_KEYDOWN,
	UIEVENT_KEYUP,
	UIEVENT_CHAR,
	UIEVENT_SYSKEY,

	UIEVENT_KILLFOCUS,
	UIEVENT_SETFOCUS,

	UIEVENT_WINDOWSIZE,

	UIEVENT_MOUSEWHEEL,

	UIEVENT_TOUCHBEGIN,
	UIEVENT_TOUCHMOVE,
	UIEVENT_TOUCHEND,
	UIEVENT_TOUCANCEL,
	/*
	UIEVENT_DBLCLICK,
	UIEVENT_CONTEXTMENU,
	UIEVENT_VSCROLL,
	UIEVENT_HSCROLL,
	UIEVENT_SCROLLWHEEL,
	
	UIEVENT_SETCURSOR,
	UIEVENT_MEASUREITEM,
	UIEVENT_DRAWITEM,
	UIEVENT_TIMER,
	UIEVENT_NOTIFY,
	UIEVENT_COMMAND,
	*/
	UIEVENT__LAST
};

// struct UIEventTypeHash {
// 	static unsigned hash(const UIEventType& key) { 
// 		return /*WTF::intHash*/(key);
// 	}
// 	static bool equal(const UIEventType& a, const UIEventType& b) { return a == b; }
// 	static const bool safeToCompareToEmptyOrDeleted = true;
// 	bool isHashTableDeletedValue() const { return false; }
// };
// 
// template<> struct DefaultHash<UIEventType> { typedef UIEventTypeHash Hash; };
// template<> struct HashTraits<UIEventType> {
// 	typedef UIEventType TraitType;
// 	static UIEventType emptyValue() { return UIEVENT__FIRST;}
// 	static const bool emptyValueIsZero = false;
// 	static const bool needsDestruction = false;
// 	static void constructDeletedValue(UIEventType& slot) { slot = UIEVENT__LAST; }
// 	static bool isDeletedValue(const UIEventType& value) { return value == UIEVENT__LAST; }
// };

struct TEventUI {
	UIEventType Type;
	int button; // 左键还是右键，0是左，2是右边。见W3C文档
	UINode* pTarget;
	UINode* pCurrentTarget;
	DWORD dwTimestamp;
	POINT ptMouse;
	TCHAR chKey;
	WORD wKeyState;
	UINT uMsg;
	WPARAM wParam;
	LPARAM lParam;
	int wheelDelta;

	BOOL bNotHandle; // 一般为0，表示已经处理此消息。但控件可以在脚本回调中修改
	BOOL bNotCallDefWindowProc; // 一般为0，表示已经处理此消息。但edit控件有时会为1

	TEventUI() {
		memset(this, 0, sizeof(TEventUI));
	}
};

#endif // UIEventType_H