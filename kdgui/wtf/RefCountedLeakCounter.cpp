/*
 *  Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

//#include "config.h"
#include <UIlib.h>
#include "RefCountedLeakCounter.h"
#include "Atomics.h"
#include <wtf/HashSet.h>
#include "Dom/UINode.h"
#include "Dom/style/NodeStyle.h"

// #include <wtf/HashCountedSet.h>

//namespace WTF {

#ifdef NDEBUG

void RefCountedLeakCounter::suppressMessages(const char*) { }
void RefCountedLeakCounter::cancelMessageSuppression(const char*) { }

RefCountedLeakCounter::RefCountedLeakCounter(const SQChar*) { }
RefCountedLeakCounter::~RefCountedLeakCounter() { }

void RefCountedLeakCounter::increment() { }
void RefCountedLeakCounter::decrement() { }

#else

// #define LOG_CHANNEL_PREFIX Log
// static WTFLogChannel LogRefCountedLeaks = { 0x00000000, "", WTFLogChannelOn };
// 
// typedef HashCountedSet<const char*, PtrHash<const char*> > ReasonSet;
// static ReasonSet* leakMessageSuppressionReasons;

void RefCountedLeakCounter::suppressMessages(const char* reason) {
//     if (!leakMessageSuppressionReasons)
//         leakMessageSuppressionReasons = new ReasonSet;
//     leakMessageSuppressionReasons->add(reason);
}

void RefCountedLeakCounter::cancelMessageSuppression(const char* reason) {
//     ASSERT(leakMessageSuppressionReasons);
//     ASSERT(leakMessageSuppressionReasons->contains(reason));
//     leakMessageSuppressionReasons->remove(reason);
}

RefCountedLeakCounter::RefCountedLeakCounter(const char* description)
    : m_description(description)
	, m_count(0) {
}


RefCountedLeakCounter::~RefCountedLeakCounter() {
    if (!m_count)
		return;

	CStdString str = m_description;
	if (-1 != str.Find(_SC("PlatformContextSkia")) && 1 == m_count) // 因为这有个全局变量，所以肯定会泄漏一个，不管
		return;
	
	str.Format(_SC("Leak:%d, %s\n"), m_count, m_description);
	OutputDebugString(str.GetBuffer());

	str = m_description;
	if (-1 == str.Find(_SC("UINode")))
		return;
	
	extern WTF::HashSet<UINode*>* g_leaksNodeSet;
	int leakSize = g_leaksNodeSet->size();
	if (0 == leakSize || !g_leaksNodeSet)
		return;

	//OutputDebugStringW(_SC("以下节点发生内存泄漏:\n");

	HashSet<UINode*>::iterator it = g_leaksNodeSet->begin();
	for (; it != g_leaksNodeSet->end(); ++it) {
		UINode * const * node = (UINode * const *)it.get();
		str = (*node)->GetAttrs()->m_id;
		str += _SC("\n");
		//OutputDebugStringW(str);
	}
}

void RefCountedLeakCounter::increment() {
    atomicIncrement(&m_count);
}

void RefCountedLeakCounter::decrement() {
    atomicDecrement(&m_count);
}

#endif

//} // namespace WTF
