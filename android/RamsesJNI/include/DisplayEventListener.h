//  -------------------------------------------------------------------------
//  Copyright (C) 2021 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#pragma once

#include "ramses-renderer-api/IRendererEventHandler.h"
#include <cassert>
#include <thread>

// Only implement callbacks of interest (RendererEventHandlerEmpty provides empty implementations for the rest)
// Ignores display ID for simplicity (we don't handle multi-display setups at this point)
class DisplayEventListener : public ramses::RendererEventHandlerEmpty
{
public:
    void blockUntilDisplayCreated(ramses::displayId_t displayId, ramses::RamsesRenderer& renderer)
    {
        m_displayId = displayId;
        m_displayCreated = false;

        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            renderer.dispatchEvents(*this);
        } while (!m_displayCreated);
    }

private:
    ramses::displayId_t m_displayId;
    ramses::sceneId_t m_sceneId;
    bool m_displayCreated = false;

    void displayCreated(ramses::displayId_t displayId, ramses::ERendererEventResult result) override final
    {
        if (result == ramses::ERendererEventResult::ERendererEventResult_OK)
        {
            assert(m_displayId == displayId && "This code is designed to work with a single display");
            // Avoid compiler warning that displayId is not used
            (void) displayId;
            m_displayCreated = true;
        }
    }
};
