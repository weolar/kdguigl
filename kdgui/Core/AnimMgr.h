
class AnimMgr {
	Vector<UINode*> m_animatingNodeQueue; // 正在进行动画的节点，方便在关闭时候统一删除动画

public:
	void Init() {
	}

	void AppendAnimNode(UINode* node) {
		size_t size = m_animatingNodeQueue.size();
		for (size_t i = 0; i < size; ++i) {
			if (node == m_animatingNodeQueue[i])
				return;
		}

		node->ref();
		m_animatingNodeQueue.append(node);
	}

	void RemoveAnimNode(UINode* node) {
		size_t size = m_animatingNodeQueue.size();
		for (size_t i = 0; i < size; ++i) {
			if (node == m_animatingNodeQueue[i]) {
				m_animatingNodeQueue.remove(i, 1);
				node->deref();
				return;
			}
		}
	}

	void ForceStopAllAnim() {
		// 因为在强制停止的时候会进入removeAnimNode操作队列，所以需要复制一份
		while (0 != m_animatingNodeQueue.size()) {
			Vector<UINode*> animatingNodeQueueDummy = m_animatingNodeQueue;
			size_t size = animatingNodeQueueDummy.size();
			for (size_t i = 0; i < size; ++i) // 在停止的时候，有可能用户会继续向里面加动画，所以外层需要一个循环
				animatingNodeQueueDummy[i]->ForceStopAllAnimAndDestroy();
		}

		KDASSERT(0 == m_animatingNodeQueue.size());
	}
};