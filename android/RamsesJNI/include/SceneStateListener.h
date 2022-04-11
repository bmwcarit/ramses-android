//  -------------------------------------------------------------------------
//  Copyright (C) 2021 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#pragma once

#include "ramses-renderer-api/IRendererSceneControlEventHandler.h"
#include "ramses-renderer-api/RamsesRenderer.h"
#include "ramses-renderer-api/RendererSceneControl.h"

#include <thread>

// Only implement callbacks of interest (RendererSceneControlEventHandlerEmpty provides empty implementations for the rest)
// Ignores scene ID for simplicity (we don't handle multi-scene setups at this point)
class SceneStateListener : public ramses::RendererSceneControlEventHandlerEmpty
{
public:
    void blockUntilSceneRendered(ramses::RamsesRenderer& renderer)
    {
        m_sceneRendered = false;

        while (!m_sceneRendered)
        {
            renderer.getSceneControlAPI()->dispatchEvents(*this);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

private:
    bool m_sceneRendered = false;

    void sceneStateChanged(ramses::sceneId_t /*sceneId*/, ramses::RendererSceneState state) override final
    {
        if (state == ramses::RendererSceneState::Rendered)
        {
            m_sceneRendered = true;
        }
    }
};
