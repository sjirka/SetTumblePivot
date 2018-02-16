#pragma once

#include "SetTumblePivotCtx.h"

#include <maya\MPxContextCommand.h>

#define MODE_FLAG "-m"
#define MODE_FLAG_LONG "-mode"

class SetTumblePivotCtxCmd : public MPxContextCommand
{
public:
	SetTumblePivotCtxCmd();
	virtual ~SetTumblePivotCtxCmd();

	static void *creator();
	virtual MPxContext* makeObj();

	virtual MStatus doEditFlags();
	virtual MStatus doQueryFlags();
	virtual MStatus appendSyntax();

private:
	SetTumblePivotCtx* m_context;
};