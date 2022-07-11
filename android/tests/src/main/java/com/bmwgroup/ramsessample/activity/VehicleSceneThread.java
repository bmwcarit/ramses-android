//  -------------------------------------------------------------------------
//  Copyright (C) 2022 BMW AG
//  -------------------------------------------------------------------------
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at https://mozilla.org/MPL/2.0/.
//  -------------------------------------------------------------------------

package com.bmwgroup.ramsessample.activity;

import android.content.Context;
import android.util.Log;

import com.bmwgroup.ramses.Property;
import com.bmwgroup.ramses.RamsesThread;

public class VehicleSceneThread extends RamsesThread {

    public VehicleSceneThread(String threadName, Context context) {
        super(threadName, context);
    }

    /* Overrides the base class method which calls this based on thread scheduling
     * This method is executed from the correct thread (the one which talks to ramses)
     */
    @Override
    protected void onUpdate() {
        /*
         * Set the camera yaw and pitch values based on user input (also see input callbacks below)
         */
        m_cameraYaw.set(m_yawValue);
        m_cameraPitch.set(m_pitchValue);

        /*
         * Open or close the doors based on bool value (also see input callbacks below)
         */
        m_doorL1.set(m_allDoorsOpen);
        m_doorL2.set(m_allDoorsOpen);
        m_doorR1.set(m_allDoorsOpen);
        m_doorR2.set(m_allDoorsOpen);
        m_trunk.set(m_allDoorsOpen);
    }

    /* Overrides the base class method which calls this based on thread scheduling
     * This method is executed from the correct thread (the one which talks to ramses)
     */
    @Override
    protected void onSceneLoaded() {
        /// Find the script used to control the logic state (light animation and camera control)
        Property doors = getLogicNodeRootInput("Interface_Doors");
        Property camera = getLogicNodeRootInput("CameraCrane.Logic_CameraCrane");

        if(camera == null || doors == null)
        {
            Log.e("RamsesSampleApp", "Loaded scene does not contain expected interface scripts!");
        }
        else
        {
            m_cameraViewportW = camera.getChild("Viewport").getChild("Width");
            m_cameraViewportH = camera.getChild("Viewport").getChild("Height");
            m_cameraYaw = camera.getChild("CraneGimbal").getChild("Yaw");
            m_cameraPitch = camera.getChild("CraneGimbal").getChild("Pitch");

            m_doorL1 = doors.getChild("Doors_F_L_isOpen");
            m_doorL2 = doors.getChild("Doors_B_L_isOpen");
            m_doorR1 = doors.getChild("Doors_F_R_isOpen");
            m_doorR2 = doors.getChild("Doors_B_R_isOpen");
            m_trunk = doors.getChild("Trunk_U_isOpen");

            /// Initialize values from the scene defaults; in real apps the values should come from the application logic
            m_yawValue = m_cameraYaw.getFloat();
            m_pitchValue = m_cameraPitch.getFloat();
        }
    }

    /* Overrides the base class method which calls this based on thread scheduling
     * This method is executed from the correct thread (the one which talks to ramses)
     */
    @Override
    protected void onSceneLoadFailed() {
        // Implement actions to react to failed scene load
        Log.e("RamsesSampleApp", "Loading Scene failed");
    }

    @Override
    protected void onLogicUpdated() {
        // Here it's possible to read out (but not write!) scene state data, like LogicNode outputs (see RamsesThread::getLogicNodeRootOutput()).
    }

    /* Overrides the base class method which calls this based on thread scheduling
     * This method is executed from the correct thread (the one which talks to ramses)
     */
    @Override
    protected void onDisplayResize(int width, int height) {
        m_cameraViewportW.set(width);
        m_cameraViewportH.set(height);
    }

    void toggleDoors() {
        m_allDoorsOpen = !m_allDoorsOpen;
    }

    // Have to pass user code to addRunnableToThreadQueue to execute from the correct thread (the one which talks to ramses)
    void onTouchDown(final int x, final int y) {
        addRunnableToThreadQueue(new Runnable() {
            @Override
            public void run() {
                m_prevX = x;
                m_prevY = y;
                m_touchDownX = x;
                m_touchDownY = y;
            }
        });
    }

    // Have to pass user code to addRunnableToThreadQueue to execute from the correct thread (the one which talks to ramses)
    void onTouchUp() {
        addRunnableToThreadQueue(new Runnable() {
            @Override
            public void run() {
                m_prevX = -1;
                m_prevY = -1;
            }
        });
    }

    // Have to pass user code to addRunnableToThreadQueue to execute from the correct thread (the one which talks to ramses)
    void onMotion(final int x, final int y) {
        addRunnableToThreadQueue(new Runnable() {
            @Override
            public void run() {
                // Incrementally shift the pitch/yaw values based on change in pixel position relative to the screen size
                final int vpWidth = m_cameraViewportW.getInt();
                final int vpHeight = m_cameraViewportH.getInt();
                final float yawDiff = (100 * (float)(m_prevX - x)) / vpWidth;
                final float pitchDiff = (100 * (float)(y - m_prevY)) / vpHeight;

                m_prevX = x;
                m_prevY = y;
                m_yawValue = m_yawValue + yawDiff;
                m_pitchValue = m_pitchValue + pitchDiff;
            }
        });
    }

    private float m_yawValue;
    private float m_pitchValue;
    private int m_prevX = -1;
    private int m_prevY = -1;
    private int m_touchDownX = -1;
    private int m_touchDownY = -1;
    private Boolean m_allDoorsOpen = false;

    private Property m_cameraViewportW;
    private Property m_cameraViewportH;
    private Property m_cameraYaw;
    private Property m_cameraPitch;
    private Property m_doorL1;
    private Property m_doorL2;
    private Property m_doorR1;
    private Property m_doorR2;
    private Property m_trunk;
}
