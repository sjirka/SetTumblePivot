#include "SetTumblePivotCtx.h"
#include "SPlane.h"

#include <maya\MFnDependencyNode.h>
#include <maya\MFnMesh.h>
#include <maya\MFnNurbsSurface.h>
#include <maya\MFnSubd.h>
#include <maya\MFnNurbsCurve.h>
#include <maya\M3dView.h>
#include <maya\MFnMeshData.h>
#include <maya\MFnCamera.h>
#include <maya\MMatrix.h>
#include <maya\MFnNurbsCurveData.h>
#include <maya\MFnTransform.h>
#include <maya\MBoundingBox.h>

SetTumblePivotCtx::SetTumblePivotCtx(){
	setTitleString("Set Tumble Pivot");
	setImage("setTumblePivot.xpm", MPxContext::kImage1);
}

SetTumblePivotCtx::~SetTumblePivotCtx()
{
}

void SetTumblePivotCtx::toolOnSetup(MEvent &event) {
	setHelpString("Set tumble pivot by clicking on geometry");

	deleteManipulators();

	MObject manipObject;
	m_manipPtr = (TumblePivotManipContainer*)TumblePivotManipContainer::newManipulator("tumblePivotManipContainer", manipObject);
	if (NULL != m_manipPtr)
		addManipulator(manipObject);
}

void SetTumblePivotCtx::doEnterRegion() {
	setHelpString("Set tumble pivot by clicking on geometry");
}

void SetTumblePivotCtx::toolOffCleanup() {
	deleteManipulators();
	MPxContext::toolOffCleanup();
}

MString SetTumblePivotCtx::stringClassName() const {
	return "setTumblePivotCtx";
}

// VP2
MStatus SetTumblePivotCtx::doPress(MEvent &event, MHWRender::MUIDrawManager &drawMgr, const MHWRender::MFrameContext &context) {
	return doPress(event);
}

MStatus SetTumblePivotCtx::doDrag(MEvent &event, MHWRender::MUIDrawManager &drawMgr, const MHWRender::MFrameContext &context) {
	return doDrag(event);
}

MStatus SetTumblePivotCtx::doRelease(MEvent &event, MHWRender::MUIDrawManager &drawMgr, const MHWRender::MFrameContext &context) {
	return doRelease(event);
}

// Common
MStatus SetTumblePivotCtx::doPress(MEvent &event){
	MStatus status;

	status = m_selectionState.storeCurrentSelection();
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = MGlobal::setHiliteList(MSelectionList());
	CHECK_MSTATUS_AND_RETURN_IT(status);

	// Click select geometry
	short x, y;
	event.getPosition(x, y);
	MGlobal::selectFromScreen(x, y, MGlobal::kReplaceList, MGlobal::kSurfaceSelectMethod);

	MGlobal::getActiveSelectionList(m_activeSelection);

	status = m_selectionState.restoreSelection();
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return doDrag(event);
}

MStatus SetTumblePivotCtx::doDrag(MEvent &event) {
	MStatus status;

	if (NULL != m_manipPtr)
		m_manipPtr->m_drag = true;

	// Get the view based ray
	short x, y;
	event.getPosition(x, y);

	MVector ray;
	MPoint pivot;
	M3dView currentView = M3dView::active3dView(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	status = currentView.viewToWorld(x, y, m_source, ray);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	if (!m_activeSelection.isEmpty()) {
		MDagPath path;
		status = m_activeSelection.getDagPath(0, path);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		status = path.extendToShapeDirectlyBelow(0);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		// Find ray/geometry intersection
		if (path.apiType() == MFn::kMesh) {
			m_hit = meshClosestIntersection(path, m_source, ray, pivot, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
		}
		else if (path.apiType() == MFn::kNurbsSurface) {
			MFnNurbsSurface fnNurbs(path, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Get nurbs intersections and use the closest
			double u, v;
			m_hit = fnNurbs.intersect(m_source, ray, u, v, pivot, 0.01, MSpace::kWorld);
		}
		else if (path.apiType() == MFn::kSubdiv) {
			// First convert SubD to proxy Mesh
			MFnSubd fnSubd(path, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			MFnMeshData dataCreator;
			MObject tesselated = dataCreator.create(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			tesselated = fnSubd.tesselate(true, 1, 1, tesselated, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			MMatrix matrix = path.inclusiveMatrix();

			// Apply world transformations to proxy
			MFnMesh fnMesh(tesselated, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			MPointArray positions;
			status = fnMesh.getPoints(positions);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			for (unsigned int i = 0; i < positions.length(); i++)
				positions[i] *= matrix;
			fnMesh.setPoints(positions);

			// Get closest hit on the proxy mesh
			m_hit = meshClosestIntersection(tesselated, m_source, ray, pivot, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
		}
		else if (path.apiType() == MFn::kNurbsCurve) {
			// Since we can't calculate curve/ray intersection, we'll convert it in screen space and use closest point instead
			MFnNurbsCurve fnOrig(path, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Create proxy curve
			MFnNurbsCurveData dataCreator;
			MObject projected = dataCreator.create(&status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			MFnNurbsCurve fnCurve(projected, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			fnCurve.copy(path.node(), projected, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Convert the proxy curve into screen space
			MPointArray positions;
			status = fnOrig.getCVs(positions, MSpace::kWorld);
			CHECK_MSTATUS_AND_RETURN_IT(status);
			for (unsigned int i = 0; i < positions.length(); i++) {
				short posX, posY;
				currentView.worldToView(positions[i], posX, posY, &status);
				CHECK_MSTATUS_AND_RETURN_IT(status);
				positions[i] = MPoint(posX, posY);
			}
			status = fnCurve.setCVs(positions);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Find parameter of the closest point in screen space
			MPoint flatSource(x, y);
			double param;
			MPoint closest = fnCurve.closestPoint(flatSource, &param, 0.001, MSpace::kObject, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);

			// Use the parameter to get the pivot position on the original curve
			status = fnOrig.getPointAtParam(param, pivot, MSpace::kWorld);
			CHECK_MSTATUS_AND_RETURN_IT(status);
		}
		else {
			// Use center of bounding box for non geometry types
			MFnTransform fnTransform(path.transform());
			pivot = fnTransform.boundingBox().center();
			m_hit = true;
		}
		if (m_hit)
			m_pivot = pivot;
	}
	else
	{
		MDagPath cameraPath;
		status = currentView.getCamera(cameraPath);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		MFnCamera fnCamera(cameraPath, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		
		SPlane plane;
		plane.setNormal((fnCamera.isOrtho())?fnCamera.viewDirection(MSpace::kWorld) : MGlobal::upAxis());
		double param;
		plane.intersect(m_source, ray, m_pivot, param);
		m_hit = true;
	}

	if (m_hit) {
		if (NULL != m_manipPtr)
			m_manipPtr->m_tumblePivot = m_pivot;
		currentView.refresh(false, true);
	}

	return MS::kSuccess;
}

MStatus SetTumblePivotCtx::doRelease(MEvent &event){
	MStatus status;

	if (NULL != m_manipPtr)
		m_manipPtr->m_drag = false;

	M3dView currentView = M3dView::active3dView(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MDagPath cameraPath;
	status = currentView.getCamera(cameraPath);
	CHECK_MSTATUS_AND_RETURN_IT(status);
	MFnCamera fnCamera(cameraPath, &status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnCamera.setCenterOfInterest((m_pivot - m_source).length());
	CHECK_MSTATUS_AND_RETURN_IT(status);

	switch (m_mode)
	{
	case kCenterOfInterest: {
		MGlobal::executeCommand("if(`contextInfo -ex tumbleContext`)tumbleCtx -e -localTumble 1 tumbleContext;");
		MPoint eye = fnCamera.eyePoint(MSpace::kWorld);
		MVector
			up = MGlobal::upAxis(),
			dir = m_pivot - eye;

		status = fnCamera.setCenterOfInterestPoint(m_pivot, MSpace::kWorld);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		status = fnCamera.set(eye, dir, up, fnCamera.horizontalFieldOfView(), fnCamera.aspectRatio());
		CHECK_MSTATUS_AND_RETURN_IT(status);
		break;
	}
	default:
		MGlobal::executeCommand("if(`contextInfo -ex tumbleContext`)tumbleCtx -e -localTumble 0 -autoSetPivot 0 tumbleContext;");
		status = fnCamera.setTumblePivot(m_pivot);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		break;
	}

	return MS::kSuccess;
}

bool SetTumblePivotCtx::meshClosestIntersection(MDagPath &mesh, MPoint &source, MVector &ray, MPoint &intersection, MStatus *status) {
	MFnMesh fnMesh(mesh, status);
	CHECK_MSTATUS_AND_RETURN(*status, false);
	MFloatPoint intersectionFloat;
	MMeshIsectAccelParams accelParams = fnMesh.autoUniformGridParams();
	int hitFace, hitTriangle;
	float hitBary1, hitBary2, hitRayParam;
	bool hit = fnMesh.closestIntersection(source, ray, NULL, NULL, false, MSpace::kWorld, 99999999.9f, false, &accelParams, intersectionFloat, &hitRayParam, &hitFace, &hitTriangle, &hitBary1, &hitBary2, 0.01f, status);
	CHECK_MSTATUS_AND_RETURN(*status, false);
	intersection = intersectionFloat;
	return hit;
}

bool SetTumblePivotCtx::meshClosestIntersection(MObject &mesh, MPoint &source, MVector &ray, MPoint &intersection, MStatus *status) {
	MFnMesh fnMesh(mesh, status);
	CHECK_MSTATUS_AND_RETURN(*status, false);
	MFloatPoint intersectionFloat;
	MMeshIsectAccelParams accelParams = fnMesh.autoUniformGridParams();
	int hitFace, hitTriangle;
	float hitBary1, hitBary2, hitRayParam;
	bool hit = fnMesh.closestIntersection(source, ray, NULL, NULL, false, MSpace::kObject, 99999999.9f, false, &accelParams, intersectionFloat, &hitRayParam, &hitFace, &hitTriangle, &hitBary1, &hitBary2, 0.01f, status);
	CHECK_MSTATUS_AND_RETURN(*status, false);
	intersection = intersectionFloat;
	return hit;
}