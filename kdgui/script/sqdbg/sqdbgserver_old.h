#ifndef _SQ_DBGSERVER_H_
#define _SQ_DBGSERVER_H_

#define MAX_BP_PATH 512
#define MAX_MSG_LEN 2049

// #include <set>
// #include <map>
// #include <string>
// #include <vector>
#include <wtf/HashMap.h>
#include <wtf/HashSet.h>
#include <wtf/HashMap.h>

#include <winsock.h>

//typedef std::basic_string<SQChar> SQDBGString;
typedef WTF::String SQDBGString;

struct WatchAndBreakPointMem {
    WatchAndBreakPointMem (SQInteger IntData, SQDBGString StringData)
    {
        m1._line = IntData;
        m2._src = StringData;
    }

    WatchAndBreakPointMem(const WatchAndBreakPointMem& Data){ m1._line = Data.m1._line; m2._src=Data.m2._src; }

    union {
        SQInteger _line;
        SQInteger _id;
    }m1;

    union {
        SQDBGString _src;
        SQDBGString _exp;
    }m2;
}

struct BreakPoint{
	BreakPoint(){m._line=0;}
	BreakPoint(SQInteger line, const SQChar *src){ m._line = line; m._src = src; }
	BreakPoint(const BreakPoint& bp){ m._line = bp.m._line; m._src=bp.m._src; }
	bool operator<(const BreakPoint& bp) const
	{
		if(m._line<bp.m._line)
			return true;
		if(m._line==bp.m._line){
// 			if(_src<bp._src){
// 				return true;
// 			}
			return false;
		}
		return false;
	}
	bool operator==(const BreakPoint& other)
	{
		if(m._line==other.m._line && (m._src==other.m._src))
			return true;
		return false;
	}
	//SQInteger _line;
	//SQDBGString _src;
    WatchAndBreakPointMem m;
};

struct Watch {
	Watch() { m._id = 0; }
	Watch(SQInteger id,const SQChar *exp) { m._id = id; m._exp = exp; }
	Watch(const Watch &w) { m._id = w.m._id; m._exp = w.m._exp; }
	bool operator<(const Watch& w) const { return m._id<w.m._id; }
	bool operator==(const Watch& w) const { return m._id == w.m._id; }
	//SQInteger _id;
	//SQDBGString _exp;
    WatchAndBreakPointMem m;
};

struct VMState {
	VMState() { _nestedcalls = 0;}
	SQInteger _nestedcalls;
};
// typedef std::map<HSQUIRRELVM,VMState*> VMStateMap;
// typedef std::set<BreakPoint> BreakPointSet;

typedef WTF::HashMap<HSQUIRRELVM,VMState*> VMStateMap;

typedef WatchAndBreakPointMem* WatchAndBreakPointMemItor;

class WatchAndBreakPointMemSet {
public:
    int size()
    {

    }

    bool insert(const WatchAndBreakPointMem& bp)
    {
        WatchAndBreakPointMem* Data = new WatchAndBreakPointMem(bp);

        m_vecBreakPoints.append(Data);
    }

    bool erase(WatchAndBreakPointMemItor itor)
    {

    }
private:
    WTF::Vector<BreakPoint*> m_vecBreakPoints;
};

//typedef BreakPointSet::iterator BreakPointSetItor;


//typedef std::set<Watch> WatchSet;
typedef WTF::HashSet<Watch> WatchSet;
typedef WatchSet::iterator WatchSetItor;

typedef std::vector<SQChar> SQCharVec;
struct SQDbgServer{
public:
	enum eDbgState{
		eDBG_Running,
		eDBG_StepOver,
		eDBG_StepInto,
		eDBG_StepReturn,
		eDBG_Suspended,
		eDBG_Disabled,
	};

	SQDbgServer(HSQUIRRELVM v);
	~SQDbgServer();
	bool Init();
	//returns true if a message has been received
	bool WaitForClient();
	bool ReadMsg();
	void BusyWait();
	void Hook(HSQUIRRELVM v,SQInteger type,SQInteger line,const SQChar *src,const SQChar *func);
	void ParseMsg(const char *msg);
	bool ParseBreakpoint(const char *msg,BreakPoint &out);
	bool ParseWatch(const char *msg,Watch &out);
	bool ParseRemoveWatch(const char *msg,SQInteger &id);
	void Terminated();
	//
	void BreakExecution();
	void Send(const SQChar *s,...);
	void SendChunk(const SQChar *chunk);
	void Break(HSQUIRRELVM v,SQInteger line,const SQChar *src,const SQChar *type,const SQChar *error=NULL);
	

	void SerializeState(HSQUIRRELVM v);
	//COMMANDS
	void AddBreakpoint(BreakPoint &bp);
	void AddWatch(Watch &w);
	void RemoveWatch(SQInteger id);
	void RemoveBreakpoint(BreakPoint &bp);

	//
	void SetErrorHandlers(HSQUIRRELVM v);
	VMState *GetVMState(HSQUIRRELVM v);

	//XML RELATED STUFF///////////////////////
	#define MAX_NESTING 10
	struct XMLElementState {
		SQChar name[256];
		bool haschildren;
	};

	XMLElementState xmlstate[MAX_NESTING];
	SQInteger _xmlcurrentement;

	void BeginDocument();
	void BeginElement(const SQChar *name);
	void Attribute(const SQChar *name, const SQChar *value);
	void EndElement(const SQChar *name);
	void EndDocument();

	const SQChar *escape_xml(const SQChar *x);
	//////////////////////////////////////////////
	HSQUIRRELVM _v;
	HSQOBJECT _debugroot;
	eDbgState _state;
	SOCKET _accept;
	SOCKET _endpoint;
	BreakPointSet _breakpoints;
	WatchSet _watches;
	//int _recursionlevel; 
	//int _maxrecursion;
	
	bool _ready;
	bool _autoupdate;
	HSQOBJECT _serializefunc;
	SQCharVec _scratchstring;

	SQInteger _line;
	SQDBGString _src;
	SQDBGString _break_type;
	VMStateMap _vmstate;	
};

#ifdef _WIN32
#define sqdbg_closesocket(x) closesocket((x))
#else
#define sqdbg_closesocket(x) close((x))
#endif

#endif //_SQ_DBGSERVER_H_ 