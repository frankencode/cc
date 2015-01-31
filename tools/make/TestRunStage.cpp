/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include "BuildPlan.h"
#include "JobScheduler.h"
#include "TestRunStage.h"

namespace fluxmake {

bool TestRunStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (outOfScope()) return success_ = true;

    for (int i = 0; i < plan()->prerequisites()->count(); ++i) {
        BuildPlan *prerequisite = plan()->prerequisites()->at(i);
        TestRunStage *stage = prerequisite->testRunStage();
        if (!stage->run()) {
            status_ = stage->status();
            return success_ = false;
        }
    }

    if (!(plan()->options() & BuildPlan::Test)) return success_ = true;

    if (plan()->options() & BuildPlan::Tools) {
        Ref<JobScheduler> scheduler = createScheduler();

        for (int i = 0; i < plan()->modules()->count(); ++i) {
            Module *module = plan()->modules()->at(i);
            String command = module->toolName();
            if (plan()->testArgs() != "") command += " " + plan()->testArgs();
            scheduler->schedule(Job::create(command));
        }

        for (Ref<Job> job; scheduler->collect(&job);) {
            fout() << job->command() << nl;
            ferr() << job->outputText();
            if (job->status() != 0) {
                status_ = job->status();
                return success_ = false;
            }
        }
    }
    else {
        String command = toolChain()->linkName(plan());
        if (plan()->testArgs() != "") command += " " + plan()->testArgs();
        if (!plan()->shell()->run(command))
            return success_ = false;
    }

    return success_ = true;
}

} // namespace fluxmake
