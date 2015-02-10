#ifndef KAnim_h
#define KAnim_h

#include "dom/KQueryTweenDef.h"
#include "bindings/KqPropsPushHelp.h"
#include <WTF/Timer.h>

class UINode;
class KdAnimCore;

typedef struct _AnimPropPairExp {
	WCHAR name[20];
	float value;
}AnimPropPairExp;

typedef bool (WINAPI * AnimStepCallback) (float result, const SQChar* attr, int attrLen, void* param1, void* param2);
typedef bool (WINAPI * AnimEndCallback) (void* param1, void* param2);

struct AnimProps {
	int type;
	Vector<AnimPropPair> props;
	int dur;
	KQueryEasing easing;
	KQueryTween tween;
	int stepLen;
	float p0;
	float p1;

	void* stepCallbackParam1;
	void* stepCallbackParam2;
	AnimStepCallback stepCallback;

	void* endparam1;
	void* endparam2;
	AnimEndCallback endCallback;

	CStdString prop;
	float value;

	void CopyProps (Vector<AnimPropPair>& other) {
		props.clear();
		for (size_t i = 0; i < other.size(); ++i) {
			AnimPropPair pair;
			pair.name = other[i].name;
			pair.value = other[i].value;			
			props.push_back(pair);
		}
	}

	AnimProps () {
		type = -1;
		dur = 0;
		KQueryEasing easing = KQE_EASE_IN;
		KQueryTween tween = KQT_LINEAR;
		stepLen = 0;
		p0 = 0;
		p1 = 0;
		value = 0;

		stepCallbackParam1 = 0;
		stepCallbackParam2 = 0;
		stepCallback = 0;

		endparam1 = 0;
		endparam2 = 0;
		endCallback = 0;
	}
};

class NodeAnim {
	WTF_MAKE_FAST_ALLOCATED;

	NodeAnim (UINode* kqueryElem, int parentCacheIdx);
	~NodeAnim();

public:
	NodeAnim* ref();
	NodeAnim* deref();
	void Start();
	NodeAnim* Stop(bool clearQueue); // 只停止动画，而不删除，外部exe慎用
	NodeAnim* Delay(int delayTime);
	NodeAnim* Queue (AnimEndCallback callback, void* param1, void* param2);

	NodeAnim* Set(const CStdString& prop, float value);
	void DoStart();

	NodeAnim* Set(const SQChar* prop, float value);

	NodeAnim* SetAllAnimFinishCallback(AnimEndCallback callback, void* callbackParam1, void* callbackParam2);

	void ReleaseKQueryWhenAnimFinish(int releaseCount);

	NodeAnim* Custom(Vector<AnimPropPair>& props, int dur, KQueryEasing easing, KQueryTween tween);
	NodeAnim* CustomWithCallback(Vector<AnimPropPair>& props, int dur,
		KQueryEasing easing, KQueryTween tween, int stepLen,
		float p0, float p1, AnimStepCallback stepCallback, void* stepCallbackParam1, void* stepCallbackParam2);

	NodeAnim* Custom(KqPropsPushHelp* props, int dur, KQueryEasing easing, KQueryTween tween);
	NodeAnim* CustomWithCallback(KqPropsPushHelp* props, int dur,
		KQueryEasing easing, KQueryTween tween, int stepLen,
		float p0, float p1, AnimStepCallback stepCallback, void* stepCallbackParam1, void* stepCallbackParam2);

	UINode* AttachedNode();
	CPageManager* GetManager() {return m_pManager;}

protected:
	UINode* m_node;
	CPageManager* m_pManager;

	int m_refCounted;

	bool m_bIsDeleteing;
	bool m_bCanDestroy; // 是否能调用delet本对象。在调用强制stop的时候不能delet，由调用方负责delete，
	// 而在动画对象全部结束的自然销毁则可以

	friend class UINode;
	friend class KdAnimCore;

	bool StepAnim(float result, const CStdString* attr, AnimStepCallback stepCallback, void* stepCallbackParam1, void* stepCallbackParam2);
	void EndAnim(bool IsForce, const CStdString& context, int parentCurAnimCoreCacheIdx);

	void Run (Vector<AnimPropPair>& props, int dur, KQueryEasing easing, KQueryTween tween,
		int stepLen, float p0, float p1, AnimStepCallback stepCallback, void* stepCallbackParam1, void* stepCallbackParam2);

	void StopCurAnimCore();

	Vector<AnimProps> m_animPropsQueue; // 每个动画队列的属性表，其实是个二维数组
	int m_parentCacheIdx;

	int m_runingAnimQueueLength; // 本层动画队列有多少个属性项，每个项相当于一个动画
	int m_hadFinishAnimQueue; // endAnim 时候判断所有本层的动画是否完成
	Vector<KdAnimCore*> m_curAnimCore; // 当前的动画列表，在stop的时候有用

	Vector<KdAnimCore*> m_willDestroyAnimCoreQueue; // 在异步回调里要被销毁的数据
	//Timer<NodeAnim> m_LazyDestroytimer;

	AnimEndCallback m_endCallbackWhenAllAnimFinish;
	void* m_endCallbackWhenAllAnimFinishParam1;
	void* m_endCallbackWhenAllAnimFinishParam2;

	void NotifyLazyDestroy(int releaseKQueryCountAtTimer, bool needDestroyMyself);
	int m_releaseKQueryWhenAllAnimFinish;
	Vector<int> m_releaseKQueryAtTimerQueue;
	Vector<bool> m_needDestroyMyselfQueue;

	void LazyDestroy(Timer<NodeAnim>*);

private:
	bool m_needDestroyMyself;
	bool PostWillDestroyAnimCoreQueueNotRepeat(KdAnimCore* animCore);

	bool JuageAllAnimHaveFinishThenClearSource();
};

#endif // KAnim_h