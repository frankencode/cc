/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXMAKE_TESTRUNSTAGE_H
#define FLUXMAKE_TESTRUNSTAGE_H

#include "BuildStage.h"

namespace fluxmake
{

class TestRunStage: public BuildStage
{
public:
	TestRunStage(BuildPlan *plan): BuildStage(plan) {}
	bool run();
};

} // namespace fluxmake

#endif // FLUXMAKE_TESTRUNSTAGE_H
