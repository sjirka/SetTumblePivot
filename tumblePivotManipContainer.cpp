#include "TumblePivotManipContainer.h"

#include "maya\MFnCamera.h"
#include "..\_library\SCamera.h"
#include "maya\MFnManip3D.h"
#include "maya\MPointArray.h"

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

	MDagPath camPath;
	status = view.getCamera(camPath);
	CHECK_MSTATUS(status);

	MFnCamera fnCam(camPath, &status);
	CHECK_MSTATUS(status);

	m_tumblePivot = fnCam.tumblePivot(&status);
	CHECK_MSTATUS(status);

	m_scaleFactor = m_scaleMultiplier*SCamera::scaleFactor(const_cast<M3dView&>(view), m_tumblePivot);

	view.beginGL();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor3f(0.0f, 1.0f, 1.0f);
	glDisable(GL_LINE_STIPPLE);


	MPointArray circle;
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

	glPopAttrib();
	view.endGL();
}

void TumblePivotManipContainer::preDrawUI(const M3dView &view){
	MStatus status;

	MDagPath camPath;
	status = const_cast<M3dView&>(view).getCamera(camPath);
	CHECK_MSTATUS(status);

	MFnCamera fnCam(camPath, &status);
	CHECK_MSTATUS(status);

	m_tumblePivot = fnCam.tumblePivot(&status);
	CHECK_MSTATUS(status);

	m_scaleFactor = m_scaleMultiplier*SCamera::scaleFactor(const_cast<M3dView&>(view), m_tumblePivot);
}

void TumblePivotManipContainer::drawUI(MHWRender::MUIDrawManager &drawManager, const MHWRender::MFrameContext &frameContext) const {
	drawManager.beginDrawable();
	drawManager.setDepthPriority(7);
	drawManager.setPointSize(7);

	drawManager.setColor(MColor(0, 1, 1));
	drawManager.circle(m_tumblePivot, MVector::xAxis, m_scaleFactor);
	drawManager.circle(m_tumblePivot, MVector::yAxis, m_scaleFactor);
	drawManager.circle(m_tumblePivot, MVector::zAxis, m_scaleFactor);

	drawManager.endDrawable();
}