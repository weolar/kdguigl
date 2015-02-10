
class RenderRichEditTimer : public TimerBase {
public:
	typedef void (RenderRichEdit::*TimerFiredFunction)(RenderRichEditTimer*);

	RenderRichEditTimer(RenderRichEdit* o, TimerFiredFunction f, ThreadTimers* threadTimers)
		: m_object(o), m_function(f), TimerBase(threadTimers) { }

	UINT idTimer;

private:
	virtual void fired() { (m_object->*m_function)(this); }

	RenderRichEdit* m_object;
	TimerFiredFunction m_function;
};

struct RenderRichEditTimeData {
	RenderRichEditTimer* timer;
	UINT idTimer;
};