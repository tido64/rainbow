// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_TERMINATEONRETURN_H_
#define DOTNET_TERMINATEONRETURN_H_

#include "Script/GameBase.h"

namespace rainbow
{
    class TerminateOnReturn
    {
    public:
        TerminateOnReturn(GameBase& script, czstring error)
            : error_(error), script_(script)
        {
        }

        ~TerminateOnReturn()
        {
            if (error_ != nullptr)
                script_.terminate(error_);
        }

        void reset(czstring error = nullptr) { error_ = error; }

    private:
        czstring error_;
        GameBase& script_;
    };
}

#endif
