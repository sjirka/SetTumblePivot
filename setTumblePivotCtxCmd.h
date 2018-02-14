#pragma once

#include "SetTumblePivotCtx.h"

#include <maya\MPxContextCommand.h>

class SetTumblePivotCtxCmd : public MPxContextCommand
{
public:
	SetTumblePivotCtxCmd();
	virtual ~SetTumblePivotCtxCmd();

	static void *creator();
	virtual MPxContext* makeObj();
};