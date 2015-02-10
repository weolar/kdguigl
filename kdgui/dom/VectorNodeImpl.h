
#ifndef VectorNodeImpl_h
#define VectorNodeImpl_h

#include "VectorNode.h"

// 矢量元素共同属性
#define eNRStyleVectorNodeCommAttr \
	eNRStyleStrokeColor,\
	eNRStyleStrokeWidth,\
	eNRStyleStrokeOpacity,\
	eNRStyleFillColor,\
	eNRStyleFillOpacity

class RectNode : public VectorNode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	virtual LPCTSTR GetClass() const {return _SC("rect");}
	virtual UINodeEnum GetClassEnum() const {return eNE_Rect;}

	virtual const KdPath* ToPath();

	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const {
		static NodeAttrDef validAttrs[] = {
			eNRStyleX,
			eNRStyleY, 
			eNRStyleWidth,
			eNRStyleHeight,
			eNRStyleRx,
			eNRStyleRy,
			eNRStyleVectorNodeCommAttr,
			eNRStyleVisualNodeCommAttr,
			eNRStyleEnd
		};
		return validAttrs;
	}
};

class CircleNode : public VectorNode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	virtual LPCTSTR GetClass() const {return _SC("circle");}
	virtual UINodeEnum GetClassEnum() const {return eNE_Circle;}
	virtual const KdPath* ToPath();

	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const {
		static NodeAttrDef validAttrs[] = {
			eNRStyleX,
			eNRStyleY, 

			eNRStyleRx,
			eNRStyleRy,
			eNRStyleCx,
			eNRStyleCy,
			eNRStyleR,

			eNRStyleVectorNodeCommAttr,
			eNRStyleVisualNodeCommAttr,
			eNRStyleEnd
		};
		return validAttrs;
	}
};

class EllipseNode : public VectorNode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	virtual LPCTSTR GetClass() const {return _SC("ellipse");}
	virtual UINodeEnum GetClassEnum() const {return eNE_Ellipse;}
	virtual const KdPath* ToPath();

	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const {
		static NodeAttrDef validAttrs[] = {
			eNRStyleX,
			eNRStyleY, 

			eNRStyleCx,
			eNRStyleCy,
			eNRStyleR,

			eNRStyleVectorNodeCommAttr,
			eNRStyleVisualNodeCommAttr,
			eNRStyleEnd
		};
		return validAttrs;
	}
};

class LineNode : public VectorNode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	virtual LPCTSTR GetClass() const {return _SC("line");}
	virtual UINodeEnum GetClassEnum() const {return eNE_Line;}
	virtual const KdPath* ToPath();

	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const {
		static NodeAttrDef validAttrs[] = {
			eNRStyleX1,
			eNRStyleY1, 

			eNRStyleX2,
			eNRStyleY2,

			eNRStyleVectorNodeCommAttr,
			eNRStyleVisualNodeCommAttr,
			eNRStyleEnd
		};
		return validAttrs;
	}
};

class PathNode : public VectorNode {
	WTF_MAKE_FAST_ALLOCATED;
public:
	virtual LPCTSTR GetClass() const {return _SC("path");}
	virtual UINodeEnum GetClassEnum() const {return eNE_Path;}
	virtual const KdPath* ToPath();

	virtual NodeAttrDef* GetCanRefreshAttrs(NodeAttrDef style) const {
		static NodeAttrDef validAttrs[] = {
			eNRStyleD,

			eNRStyleVectorNodeCommAttr,
			eNRStyleVisualNodeCommAttr,
			eNRStyleEnd
		};
		return validAttrs;
	}
};

#endif // VectorNodeImpl_h