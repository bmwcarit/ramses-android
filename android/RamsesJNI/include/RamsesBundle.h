//  -------------------------------------------------------------------------
//  Copyright (C) 2021 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#pragma once

#include <jni.h>
#include <memory>
#include <optional>

#include "ramses-framework-api/RamsesFramework.h"
#include "ramses-renderer-api/Types.h"
#include "ramses-logic/LogicEngine.h"

namespace ramses
{
    class RamsesFramework;
    class RamsesRenderer;
    class RamsesClient;
    class Scene;
}

struct ANativeWindow;

namespace ramses_bundle
{
    class Scene;

    class RAMSES_API_EXPORT RamsesBundle
    {
    public:
        RamsesBundle();

        //display and renderer management
        bool createDisplay(ANativeWindow* nativeWindow, const rlogic::vec4f& clearColor, uint32_t msaaSamples);
        bool destroyDisplay();
        void resizeDisplay(int width, int height);
        bool setMaximumFramerate(float maximumFramerate);
        bool stopRendering();
        bool startRendering();
        bool isRendering() const;
        bool isDisplayCreated() const;
        std::optional<rlogic::vec2i> getDisplaySize() const;

        //scene management
        bool loadSceneLogic(const void* rlogicBuffer, size_t bufferSize);
        uint32_t getFeatureLevel() const;
        bool updateLogic();
        bool flushRamsesScene();
        bool dispatchRendererEvents();
        bool loadScene(int fdRamses, long offset, long fdSize,
                       const void* rlogicBuffer, size_t bufferSize);
        bool showScene();
        rlogic::Property* getInterface(const char* interfaceName);
        rlogic::Property* getLogicNodeRootInput(const char* logicNodeName);
        const rlogic::Property* getLogicNodeRootOutput(const char* logicNodeName);
        bool linkProperties(const rlogic::Property& sourceProperty, const rlogic::Property& targetProperty);
        bool unlinkProperties(const rlogic::Property& sourceProperty, const rlogic::Property& targetProperty);

        //debugging
        void executeRamshCommand(const char* command);
        void dumpSceneToFile(const char* filePath);
        void dumpLogicToFile(const char* filePath);

    private:
        rlogic::LogicNode* findLogicNode(const char* logicNodeName);

        std::unique_ptr<ramses::RamsesFramework> m_framework;
        ramses::RamsesRenderer* m_renderer = nullptr;
        ramses::RamsesClient* m_client = nullptr;
        ramses::Scene* m_ramsesScene = nullptr;
        std::unique_ptr<rlogic::LogicEngine> m_logicEngine;
        ramses::displayId_t m_displayId = ramses::displayId_t::Invalid();
        rlogic::vec4f m_clearColor = {0.f, 0.f, 0.f, 0.f};
        std::optional<rlogic::vec2i> m_displaySize;
        bool m_displayCreated = false;
    };
}
