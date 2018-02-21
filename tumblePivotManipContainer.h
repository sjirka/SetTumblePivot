#pragma once
#include <maya\MPxManipContainer.h>
#include "maya\MDagPath.h"
#include "tumbleMode.h"

class TumblePivotManipContainer : public MPxManipContainer
{
public:
	TumblePivotManipContainer();
	virtual ~TumblePivotManipContainer();
	
	static void * creator();

	static MStatus initialize();
	virtual MStatus createChildren();
	virtual MStatus connectToDependNode(const MObject &node);

	virtual void 	draw(M3dView &view, const MDagPath &path, M3dView::DisplayStyle style, M3dView::DisplayStatus status);
	virtual void 	preDrawUI(const M3dView &view);
	virtual void 	drawUI(MHWRender::MUIDrawManager &drawManager, const MHWRender::MFrameContext &frameContext) const;

	static MTypeId id;
private:
	MPoint
		m_tumblePivot,
		m_textPoint;
	double m_scaleFactor;
	double m_scaleMultiplier = 0.02;
	TumbleMode m_tumbleMode;
};