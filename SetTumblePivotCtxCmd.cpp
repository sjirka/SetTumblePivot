#include "SetTumblePivotCtxCmd.h"

SetTumblePivotCtxCmd::SetTumblePivotCtxCmd(){
}

SetTumblePivotCtxCmd::~SetTumblePivotCtxCmd(){
}

void *SetTumblePivotCtxCmd::creator() {
	return new SetTumblePivotCtxCmd;
}

MPxContext* SetTumblePivotCtxCmd::makeObj() {
	return m_context = new SetTumblePivotCtx;
}

MStatus SetTumblePivotCtxCmd::appendSyntax() {
	MStatus status(MStatus::kSuccess);

	MSyntax mySyntax = syntax(&status);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	mySyntax.addFlag(MODE_FLAG, MODE_FLAG_LONG, MSyntax::kLong);

	return MS::kSuccess;
}

MStatus SetTumblePivotCtxCmd::doEditFlags() {
	MStatus status(MStatus::kSuccess);

	MArgParser argData = parser();

	if (argData.isFlagSet(MODE_FLAG)) {
		int value;
		status = argData.getFlagArgument(MODE_FLAG, 0, value);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		switch (value){
		case TumbleMode::kCenterOfInterest:
			m_context->m_mode = TumbleMode::kCenterOfInterest;
			break;
		default:
			m_context->m_mode = TumbleMode::kTumblePoint;
			break;
		}
	}

	return status;
}

MStatus SetTumblePivotCtxCmd::doQueryFlags() {
	MStatus status(MStatus::kSuccess);

	MArgParser argData = parser();

	if(argData.isFlagSet(MODE_FLAG))
		setResult(m_context->m_mode);

	return status;
}