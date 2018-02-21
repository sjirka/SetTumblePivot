#include "SetTumblePivotCtxCmd.h"

SetTumblePivotCtxCmd::SetTumblePivotCtxCmd(){
}

SetTumblePivotCtxCmd::~SetTumblePivotCtxCmd(){
}

void *SetTumblePivotCtxCmd::creator() {
	return new SetTumblePivotCtxCmd;
}

MPxContext* SetTumblePivotCtxCmd::makeObj() {
	m_context = new SetTumblePivotCtx;

	if (MGlobal::optionVarExists("setTumblePivotMode"))
		m_context->m_mode = static_cast<TumbleMode>(MGlobal::optionVarIntValue("setTumblePivotMode"));
	
	return m_context;
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

	MStringArray arg;
	argData.getObjects(arg);

	if (argData.isFlagSet(MODE_FLAG)) {
		int value;
		status = argData.getFlagArgument(MODE_FLAG, 0, value);
		CHECK_MSTATUS_AND_RETURN_IT(status);
		m_context->m_mode = static_cast<TumbleMode>(value);
		MGlobal::executeCommand(MString("tumbleCtx -e -localTumble ")+ value +" tumbleContext;");
		MGlobal::setOptionVarValue("setTumblePivotMode", value);
	}

	MGlobal::executeCommand("setTumblePivotCtxValues " + arg[0] + "; refresh;");

	return status;
}

MStatus SetTumblePivotCtxCmd::doQueryFlags() {
	MStatus status(MStatus::kSuccess);

	MArgParser argData = parser();

	if(argData.isFlagSet(MODE_FLAG))
		setResult(m_context->m_mode);

	return status;
}