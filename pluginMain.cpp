#include "SetTumblePivotCtxCmd.h"
#include "tumblePivotManipContainer.h"

#include <maya\MFnPlugin.h>
#include <maya\MDrawRegistry.h>

MStatus initializePlugin(MObject obj)
{
	MStatus status;

	MFnPlugin fnPlugin(obj, "Stepan Jirka", "1.0", "Any");

	status = fnPlugin.registerContextCommand("setTumblePivot", SetTumblePivotCtxCmd::creator);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.registerNode(
		"tumblePivotManipContainer",
		TumblePivotManipContainer::id,
		TumblePivotManipContainer::creator,
		TumblePivotManipContainer::initialize,
		MPxNode::kManipContainer);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus status;

	MFnPlugin fnPlugin(obj);

	status = fnPlugin.deregisterNode(TumblePivotManipContainer::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.deregisterContextCommand("setTumblePivot");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}
