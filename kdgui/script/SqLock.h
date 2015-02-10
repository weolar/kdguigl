/*
 * Copyright (C) 2005, 2008, 2009 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef SqLock_h
#define SqLock_h

#include <wtf/Assertions.h>
#include <wtf/Noncopyable.h>

namespace SQ {
class SqLock {
    WTF_MAKE_NONCOPYABLE(SqLock);
public:
    enum SqLockBehavior { SilenceAssertionsOnly, LockForReal };

    SqLock(SqLockBehavior lockBehavior)
        : m_lockBehavior(lockBehavior)
    {
#ifdef NDEBUG
        // Locking "not for real" is a debug-only feature.
        if (lockBehavior == SilenceAssertionsOnly)
            return;
#endif
        lock(lockBehavior);
    }

    static void lock(SqLockBehavior) {} // TODO_weolar
    static void unlock(SqLockBehavior);

private:
    SqLockBehavior m_lockBehavior;
};

} // namespace

#endif // SqLock_h