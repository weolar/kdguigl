
#ifndef LenOrPercent_h
#define LenOrPercent_h

enum LenOrPercentType {
	eLOPLen,
	eLOPPercent,
};

struct LenOrPercent {
	LenOrPercent() {
		type = eLOPLen;
		u.len = 0;
	}

	LenOrPercent (const LenOrPercent& other) {
		type = other.type;
		u.len = other.u.len;
	}

	bool operator == (const LenOrPercent& other) const {
		return Compare(other);
	}

	bool Compare(const LenOrPercent& other) const {
		if (type != other.type)
			return false;
		if (eLOPLen == type)
			return (u.len == other.u.len);
		else if (eLOPPercent == type)
			return (u.percent == other.u.percent);
		return true;
	}

	int ToLen() {
		if (eLOPPercent == type)
			return 0;
		return (int)u.len;
	}

// 	float GetLen(int realLen) {
// 		if (eLOPLen == type)
// 			return (float)u.len;
// 		else
// 			return realLen * (u.percent + 1.0f);
// 	}

	LenOrPercentType type;

	union {
		float len;
		float percent;
	} u;
	
};

#endif // LenOrPercent_h