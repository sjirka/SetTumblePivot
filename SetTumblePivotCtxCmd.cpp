#include "SetTumblePivotCtxCmd.h"

SetTumblePivotCtxCmd::SetTumblePivotCtxCmd(){
}

SetTumblePivotCtxCmd::~SetTumblePivotCtxCmd(){
}

void *SetTumblePivotCtxCmd::creator() {
	return new SetTumblePivotCtxCmd;
}

MPxContext* SetTumblePivotCtxCmd::makeObj() {
	return new SetTumblePivotCtx;
}