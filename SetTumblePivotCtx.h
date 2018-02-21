#pragma once
#include "tumblePivotManipContainer.h"
#include "tumbleMode.h"

#include <maya\MPxContext.h>
#include <maya\MPoint.h>
#include <maya\MVector.h>
#include <maya\MPointArray.h>
#include <maya\MDagPath.h>
#include <maya\MRichSelection.h>
#include <maya\MSelectionList.h>
#include <maya\MGlobal.h>

class SetTumblePivotCtx : public MPxContext
{
public:
	SetTumblePivotCtx();
	virtual ~SetTumblePivotCtx();

	virtual void toolOnSetup(MEvent &);
	virtual void doEnterRegion();
	virtual void toolOffCleanup();
	virtual MString stringClassName() const;

	virtual MStatus doPress(MEvent &event);
	virtual MStatus doDrag(MEvent &event);
	virtual MStatus doRelease(MEvent &event);

	// VP2
	virtual MStatus doPress(MEvent &event, MHWRender::MUIDrawManager &drawMgr, const MHWRender::MFrameContext &context);
	virtual MStatus doDrag(MEvent &event, MHWRender::MUIDrawManager &drawMgr, const MHWRender::MFrameContext &context);
	virtual MStatus doRelease(MEvent &event, MHWRender::MUIDrawManager &drawMgr, const MHWRender::MFrameContext &context);
	
	static bool meshClosestIntersection(MDagPath &path, MPoint &source, MVector &ray, MPoint &intersection, MStatus *status = NULL);
	static bool meshClosestIntersection(MObject &mesh, MPoint &source, MVector &ray, MPoint &intersection, MStatus *status = NULL);
	static MPoint closestPoint(MPointArray &cloud, MPoint &toPoint);

	TumbleMode m_mode = TumbleMode::kTumblePoint;
private:
	TumblePivotManipContainer* m_manipPtr = NULL;

	MSelectionList
		m_activeList,
		m_hiliteList;
	MRichSelection
		m_richList;
	MGlobal::MSelectionMode
		m_selectionMode;
	MSelectionMask
		m_objectMask,
		m_componentMask,
		m_animMask;
	bool
		m_hasRichSelection;
};

