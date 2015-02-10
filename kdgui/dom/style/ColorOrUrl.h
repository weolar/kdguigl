
#ifndef ColorOrUrl_h
#define ColorOrUrl_h

enum ColorOrUrlType {
	eCYTColor,
	eCYTUrl,
	eCYTNull,
};

struct ColorOrUrl {
	ColorOrUrl() {
		type = eCYTNull;
		color = 0xffffff;
	}

	ColorOrUrl(SkColor c) {
		color = c;
		type = eCYTColor;
	}

	ColorOrUrl (const ColorOrUrl& other) {
		type = other.type;
		color = other.color;
		url = other.url;
	}

	bool operator == (const ColorOrUrl& other) const {
		return Compare(other);
	}

	bool Compare(const ColorOrUrl& other) const {
		if (type != other.type)
			return false;
		if (eCYTColor == type)
			return (color == other.color);
		else if (eCYTUrl == type)
			return (url == other.url);
		return true;
	}

	SkColor ToColor() {
		if (eCYTColor == type)
			return color;
		return 0xffffff;
	}

	ColorOrUrlType type;
	SkColor color;
	CStdString url;
};

#endif // ColorOrUrl_h