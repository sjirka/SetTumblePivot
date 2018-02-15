#include "SetTumblePivotCtxCmd.h"
#include "tumblePivotManipContainer.h"
#include "SetTumblePivotCmd.h"

#include <maya\MFnPlugin.h>
#include <maya\MGlobal.h>


MStatus initializePlugin(MObject obj)
{
	MStatus status;

	MFnPlugin fnPlugin(obj, "Stepan Jirka", "1.0", "Any");

	status = fnPlugin.registerContextCommand("setTumblePivotCtx", SetTumblePivotCtxCmd::creator);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.registerNode(
		"tumblePivotManipContainer",
		TumblePivotManipContainer::id,
		TumblePivotManipContainer::creator,
		TumblePivotManipContainer::initialize,
		MPxNode::kManipContainer);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.registerCommand("setTumblePivot", SetTumblePivotCmd::creator, SetTumblePivotCmd::newSyntax);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	MGlobal::executeCommand("if(!`runTimeCommand -ex setTumblePivot_pressed`)runTimeCommand -ann \"Activate setTumblePivotCtx\" -cat \"Viewport tools\" -d 0 -c \"setTumblePivot;\" setTumblePivot_pressed;");
	MGlobal::executeCommand("if(!`runTimeCommand -ex setTumblePivot_released`)runTimeCommand -ann \"Deactivate setTumblePivotCtx\" -cat \"Viewport tools\" -d 0 -c \"setTumblePivot -ex;\" setTumblePivot_released;");

	return MS::kSuccess;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus status;

	MFnPlugin fnPlugin(obj);

	status = fnPlugin.deregisterNode(TumblePivotManipContainer::id);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.deregisterContextCommand("setTumblePivotCtx");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.deregisterCommand("setTumblePivot");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}
