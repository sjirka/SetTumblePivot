#include "TumblePivotManipContainer.h"

#include "maya\MFnCamera.h"
#include "..\_library\SCamera.h"
#include "maya\MFnManip3D.h"
#include "maya\MPointArray.h"

float tumbleColor[3] = { 0.0, 1.0, 1.0 };
float centerColor[3] = { 1.0, 1.0, 0.0 };


MTypeId TumblePivotManipContainer::id(0x00127899);

TumblePivotManipContainer::TumblePivotManipContainer() {
}

TumblePivotManipContainer::~TumblePivotManipContainer() {
}

void * TumblePivotManipContainer::creator() {
	return new TumblePivotManipContainer;
}

MStatus TumblePivotManipContainer::initialize() {
	return MPxManipContainer::initialize();
}

MStatus TumblePivotManipContainer::createChildren() {
	return MS::kSuccess;
}

MStatus TumblePivotManipContainer::connectToDependNode(const MObject &node) {
	return MStatus::kSuccess;
}

void TumblePivotManipContainer::draw(M3dView &view, const MDagPath &path, M3dView::DisplayStyle style, M3dView::DisplayStatus stat) {
	MPxManipContainer::draw(view, path, style, stat);

	MStatus status;

	int tumbleMode;
	status = MGlobal::executeCommand("tumbleCtx -q -localTumble tumbleContext;", tumbleMode);
	CHECK_MSTATUS(status);
	m_tumbleMode = static_cast<TumbleMode>(tumbleMode);

	if (!m_drag) {
		MDagPath camPath;
		status = view.getCamera(camPath);
		CHECK_MSTATUS(status);

		MFnCamera fnCam(camPath, &status);
		CHECK_MSTATUS(status);

		m_tumblePivot = (m_tumbleMode == TumbleMode::kTumblePoint) ? fnCam.tumblePivot(&status) : fnCam.centerOfInterestPoint(MSpace::kWorld, &status);
		CHECK_MSTATUS(status);
	}

	m_scaleFactor = m_scaleMultiplier*SCamera::scaleFactor(const_cast<M3dView&>(view), m_tumblePivot);

	short x, y;
	view.worldToView(m_tumblePivot, x, y);
	view.viewToWorld(x + 5, y + 5, m_textPoint, MVector());

	view.beginGL();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	MString text = (m_tumbleMode == TumbleMode::kTumblePoint) ? "T" : "C";
	MColor color = (m_tumbleMode == TumbleMode::kTumblePoint) ? MColor(tumbleColor) : MColor(centerColor);

	glColor3f(color.r, color.g, color.b);
	glDisable(GL_LINE_STIPPLE);

	unsigned int divisions = 20;

	glBegin(GL_LINE_STRIP);
	for (unsigned i = 0; i <= divisions; i++) {
		double angle = 2 * M_PI / divisions*i;
		glVertex3d(m_tumblePivot.x+sin(angle)*m_scaleFactor, m_tumblePivot.y+cos(angle)*m_scaleFactor, m_tumblePivot.z);
	}
	glEnd();
	glBegin(GL_LINE_STRIP);
	for (unsigned i = 0; i <= divisions; i++) {
		double angle = 2 * M_PI / divisions*i;
		glVertex3d(m_tumblePivot.x + sin(angle)*m_scaleFactor, m_tumblePivot.y, m_tumblePivot.z + cos(angle)*m_scaleFactor);
	}
	glEnd();
	glBegin(GL_LINE_STRIP);
	for (unsigned i = 0; i <= divisions; i++) {
		double angle = 2 * M_PI / divisions*i;
		glVertex3d(m_tumblePivot.x, m_tumblePivot.y + sin(angle)*m_scaleFactor, m_tumblePivot.z + cos(angle)*m_scaleFactor);
	}
	glEnd();

	view.drawText(text, m_textPoint);

	glPopAttrib();
	view.endGL();
}

void TumblePivotManipContainer::preDrawUI(const M3dView &view){
	MStatus status;

	int tumbleMode;
	status = MGlobal::executeCommand("tumbleCtx -q -localTumble tumbleContext;", tumbleMode);
	CHECK_MSTATUS(status);
	m_tumbleMode = static_cast<TumbleMode>(tumbleMode);

	if (!m_drag) {
		MDagPath camPath;
		status = const_cast<M3dView&>(view).getCamera(camPath);
		CHECK_MSTATUS(status);

		MFnCamera fnCam(camPath, &status);
		CHECK_MSTATUS(status);

		m_tumblePivot = (m_tumbleMode == TumbleMode::kTumblePoint) ? fnCam.tumblePivot(&status) : fnCam.centerOfInterestPoint(MSpace::kWorld, &status);
		CHECK_MSTATUS(status);
	}

	short x, y;
	view.worldToView(m_tumblePivot, x, y);
	m_textPoint.x = x + 5;
	m_textPoint.y = y + 5;

	m_scaleFactor = m_scaleMultiplier*SCamera::scaleFactor(const_cast<M3dView&>(view), m_tumblePivot);
}

void TumblePivotManipContainer::drawUI(MHWRender::MUIDrawManager &drawManager, const MHWRender::MFrameContext &frameContext) const {

	drawManager.beginDrawable();
	drawManager.setDepthPriority(7);

	MString text = (m_tumbleMode == TumbleMode::kTumblePoint) ? "T" : "C";
	MColor color = (m_tumbleMode == TumbleMode::kTumblePoint) ? MColor(tumbleColor) : MColor(centerColor);

	drawManager.setColor(color);
	drawManager.circle(m_tumblePivot, MVector::xAxis, m_scaleFactor);
	drawManager.circle(m_tumblePivot, MVector::yAxis, m_scaleFactor);
	drawManager.circle(m_tumblePivot, MVector::zAxis, m_scaleFactor);
	drawManager.text2d(m_textPoint, text);

	drawManager.endDrawable();
}