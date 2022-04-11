//  -------------------------------------------------------------------------
//  Copyright (C) 2021 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

#include "RamsesBundle.h"
#include "SceneStateListener.h"
#include "DisplayEventListener.h"

#include <android/native_window.h>
#include <android/log.h>
#include <cassert>

#include "ramses-renderer-api/DisplayConfig.h"
#include "ramses-framework-api/RamsesFramework.h"
#include "ramses-client-api/Scene.h"
#include "ramses-client-api/RamsesClient.h"
#include "ramses-utils.h"

#include "ramses-logic/Property.h"
#include "ramses-logic/LuaScript.h"
#include "ramses-logic/RamsesAppearanceBinding.h"
#include "ramses-logic/RamsesNodeBinding.h"
#include "ramses-logic/RamsesCameraBinding.h"
#include "ramses-logic/AnimationNode.h"

namespace ramses_bundle
{
    RamsesBundle::RamsesBundle()
    {
        ramses::RamsesFrameworkConfig frameworkConfig;
        frameworkConfig.setPeriodicLogsEnabled(false);
        m_framework.reset(new ramses::RamsesFramework(frameworkConfig));
        m_client = m_framework->createClient("client-scene-reader");
    }

    bool RamsesBundle::createDisplay(ANativeWindow* nativeWindow, const rlogic::vec4f& clearColor)
    {
        /**
         * Should never create display when already created
         * In this early JNI version, we re-create the whole renderer, not just the display, because it
         * simplifies the code and state handling
         */
        if (m_renderer)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "CreateDisplay failed as the display was already created!");
            return false;
        }

        m_renderer = m_framework->createRenderer(ramses::RendererConfig{});

        ramses::DisplayConfig displayConfig;
        // get the size of the android app window and use it for display creation
        const int width = ANativeWindow_getWidth(nativeWindow);
        const int height = ANativeWindow_getHeight(nativeWindow);
        m_displaySize = { width, height };
        displayConfig.setWindowRectangle(0, 0, width, height);
        displayConfig.setAndroidNativeWindow(nativeWindow);
        displayConfig.setClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
        __android_log_print(ANDROID_LOG_DEBUG, "RamsesNativeInterface", "Clear Color set to: %.1f %.1f %.1f %.1f", m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
        m_displayId = m_renderer->createDisplay(displayConfig);
        m_renderer->flush();

        const float framerateBeforeCreateDisplay = m_renderer->getMaximumFramerate();

        if (framerateBeforeCreateDisplay < 60) {
            m_renderer->setMaximumFramerate(60); // increase fps for faster command processing
        }

        m_renderer->startThread();

        // To avoid lifecycle issues, block until display created event issued by ramses
        DisplayEventListener displayEventListener;
        displayEventListener.blockUntilDisplayCreated(m_displayId, *m_renderer);

        // Thread must be started explicitly by the app to avoid wasting system resources when e.g. paused
        m_renderer->stopThread();

        if (framerateBeforeCreateDisplay != m_renderer->getMaximumFramerate()) {
            m_renderer->setMaximumFramerate(framerateBeforeCreateDisplay); // back to old fps
        }

        __android_log_print(ANDROID_LOG_DEBUG, "RamsesNativeInterface", "Created display %d from native Android window %p", m_displayId.getValue(), nativeWindow);

        m_displayCreated = true;
        return true;
    }

    void RamsesBundle::resizeDisplay(int width, int height)
    {
        //TODO Violin/Vaclav add feature in ramses to tell it when an externally owned native window had its surface size resized (currently not possible without recreating the display)
        __android_log_print(ANDROID_LOG_WARN, "RamsesNativeInterface", "Window resize event received (%d, %d) but not implemented yet in RamsesBundle!", width, height);
    }

    bool RamsesBundle::setMaximumFramerate(float maximumFramerate) {
        if(!m_renderer)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface",  "setMaximumFramerate failed because the display was not created yet!");
            return false;
        }

        const auto result = m_renderer->setMaximumFramerate(maximumFramerate);
        if(result != ramses::StatusOK)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "setMaximumFramerate failed!");
            return false;
        }
        return true;
    }

    bool RamsesBundle::destroyDisplay()
    {
        /**
         * For simplicity, we destroy the entire renderer (which indirectly destroys all displays)
         * Complex applications might need to keep the renderer and only destroy the display
         * (e.g. when using shader caches)
         */
        if (!m_displayId.isValid())
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "DestroyDisplay failed as there is no valid display to destroy!");
            return false;
        }
        //This should never happen
        assert(m_renderer);

        m_framework->destroyRenderer(*m_renderer);
        m_renderer = nullptr;
        m_displayId = ramses::displayId_t::Invalid();
        m_displayCreated = false;
        m_displaySize = std::nullopt;
        return true;
    }

    bool RamsesBundle::loadScene(int fdRamses, long offset, long fdSize,
                                 const void* rlogicBuffer, size_t bufferSize)
    {
        m_ramsesScene = m_client->loadSceneFromFileDescriptor(fdRamses, offset, fdSize);

        if(!m_ramsesScene)
            return false;

        ramses::status_t result = m_ramsesScene->publish(ramses::EScenePublicationMode_LocalOnly);
        if(result != ramses::StatusOK)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Publishing scene after loading failed! Destroying ramses scene to avoid inconsistent state...");
            m_client->destroy(*m_ramsesScene);
            return false;
        }

        if(rlogicBuffer && !loadSceneLogic(rlogicBuffer, bufferSize))
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Loading ramses logic failed! Destroying ramses scene to avoid inconsistent state...");
            return false;
        }

        // Only updateLogic if there is a logicEngine
        if (m_logicEngine)
        {
            if(!updateLogic())
            {
                __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Initial logic update failed!");
                return false;
            }
        }
        if(!flushRamsesScene())
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Initial scene flush failed!");
            return false;
        }

        __android_log_print(ANDROID_LOG_DEBUG, "RamsesNativeInterface", "Successfully loaded ramses contents.");
        return true;
    }

    bool RamsesBundle::loadSceneLogic(const void* rlogicBuffer, size_t bufferSize) {
        auto loadedLogicEngine = std::make_unique<rlogic::LogicEngine>();
        bool success = loadedLogicEngine->loadFromBuffer(rlogicBuffer, bufferSize, m_ramsesScene);
        if (!success)
        {
            return false;
        }
        m_logicEngine = std::move(loadedLogicEngine);
        return m_logicEngine->update();
    }

    bool RamsesBundle::updateLogic()
    {
        if (!m_logicEngine)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "UpdateLogic failed! Can't call updateLogic if no LogicEngine was loaded before!");
            return false;
        }
        return m_logicEngine->update();
    }

    bool RamsesBundle::flushRamsesScene()
    {
        if (!m_ramsesScene)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "flushRamsesScene failed! No scene has been loaded!");
            return false;
        }
        return m_ramsesScene->flush() == ramses::StatusOK;
    }

    bool RamsesBundle::showScene()
    {
        if(!m_renderer || !m_displayId.isValid())
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Showing ramses scene failed! Can't show scene before the renderer and the display are created");
            return false;
        }

        const bool wasRenderingBefore = m_renderer->isThreadRunning();
        if(!wasRenderingBefore)
        {
            m_renderer->startThread();
        }

        const ramses::sceneId_t sceneId = m_ramsesScene->getSceneId();

        ramses::status_t result = m_renderer->getSceneControlAPI()->setSceneMapping(sceneId, m_displayId);
        if(result != ramses::StatusOK)
        {
            return false;
        }

        result = m_renderer->getSceneControlAPI()->setSceneState(sceneId, ramses::RendererSceneState::Rendered);
        if(result != ramses::StatusOK)
        {
            return false;
        }

        result = m_renderer->getSceneControlAPI()->flush();
        if(result != ramses::StatusOK)
        {
            return false;
        }

        SceneStateListener sceneState;
        sceneState.blockUntilSceneRendered(*m_renderer);

        if(!wasRenderingBefore)
        {
            m_renderer->stopThread();
        }

        __android_log_print(ANDROID_LOG_DEBUG, "RamsesNativeInterface", "Successfully displayed scene %lu to display %d.", sceneId.getValue(), m_displayId.getValue());
        return true;
    }

    rlogic::LogicNode* RamsesBundle::findLogicNode(const char* logicNodeName)
    {
        if(!m_logicEngine)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Loaded scene does not contain logic!");
            return nullptr;
        }

        auto* potentialObject = m_logicEngine->findByName<rlogic::LogicObject>(logicNodeName);
        if(!potentialObject)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Loaded scene does not contain a logic object with the name \'%s\'!", logicNodeName);
            return nullptr;
        }

        auto* potentialLogicNode = potentialObject->as<rlogic::LogicNode>();
        if(!potentialLogicNode)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Logic object with the name \'%s\' is not of type LogicNode!", logicNodeName);
            return nullptr;
        }
        return potentialLogicNode;
    }

    rlogic::Property* RamsesBundle::getLogicNodeRootInput(const char* logicNodeName)
    {
        auto* potentialLogicNode = findLogicNode(logicNodeName);
        return potentialLogicNode ? potentialLogicNode->getInputs() : nullptr;
    }

    const rlogic::Property* RamsesBundle::getLogicNodeRootOutput(const char* logicNodeName)
    {
        auto* potentialLogicNode = findLogicNode(logicNodeName);
        return potentialLogicNode ? potentialLogicNode->getOutputs() : nullptr;
    }

    bool RamsesBundle::startRendering()
    {
        if(!m_renderer)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface",  "startRendering failed because the display was not created yet!");
            return false;
        }
        else if(m_renderer->isThreadRunning())
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface",  "startRendering failed because the Renderer is already rendering!");
            return false;
        }

        __android_log_print(ANDROID_LOG_DEBUG, "RamsesNativeInterface", "Starting to render!");
        m_renderer->startThread();
        return true;
    }

    bool RamsesBundle::stopRendering()
    {
        if(!m_renderer)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface",  "stopRendering failed because the display was not created yet!");
            return false;
        }
        else if(!m_renderer->isThreadRunning())
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface",  "stopRendering failed because the Renderer is not rendering!");
            return false;
        }

        __android_log_print(ANDROID_LOG_DEBUG, "RamsesNativeInterface", "Stopping rendering!");
        m_renderer->stopThread();
        return true;
    }

    bool RamsesBundle::isRendering() const
    {
        return m_renderer && m_renderer->isThreadRunning();
    }

    bool RamsesBundle::isDisplayCreated() const
    {
        return m_displayCreated;
    }

    void RamsesBundle::executeRamshCommand(const char *command)
    {
        __android_log_print(ANDROID_LOG_INFO, "RamsesNativeInterface", "Starting to execute ramsh command!");
        if (!command)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Can't execute ramsh command as there is no command!");
            return;
        }
        if (m_framework->executeRamshCommand(command) != ramses::StatusOK)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Executing ramsh command failed!");
        }
        else
        {
            __android_log_print(ANDROID_LOG_INFO, "RamsesNativeInterface", "Finished execution of ramsh command!");
        }
    }

    void RamsesBundle::dumpSceneToFile(const char *filePath)
    {
        __android_log_print(ANDROID_LOG_INFO, "RamsesNativeInterface", "Starting to dump scene to file!");
        if (!m_ramsesScene)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Can't dump scene as there is no scene loaded!");
            return;
        }
        if (m_ramsesScene->saveToFile(filePath, false) != ramses::StatusOK)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Dumping scene to file failed!");
        }
        else
        {
            __android_log_print(ANDROID_LOG_INFO, "RamsesNativeInterface", "Finished dumping scene to file!");
        }
    }

    void RamsesBundle::dumpLogicToFile(const char* filePath)
    {
        __android_log_print(ANDROID_LOG_INFO, "RamsesNativeInterface", "Starting to dump logic to file!");
        if (!filePath)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Can't dump logic to file as there is no file path given!");
            return;
        }
        if (!m_logicEngine)
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Can't dump logic as there is no logic engine!");
            return;
        }
        if (!m_logicEngine->saveToFile(filePath))
        {
            __android_log_print(ANDROID_LOG_ERROR, "RamsesNativeInterface", "Dumping logic to file failed!");
        }
        else
        {
            __android_log_print(ANDROID_LOG_INFO, "RamsesNativeInterface", "Finished dumping logic to file!");
        }
    }

    std::optional<rlogic::vec2i> RamsesBundle::getDisplaySize() const
    {
        return m_displaySize;
    }
}
