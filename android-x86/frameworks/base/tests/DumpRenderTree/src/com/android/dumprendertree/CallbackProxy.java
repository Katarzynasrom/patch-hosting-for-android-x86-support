/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.dumprendertree;

import android.os.Handler;
import android.os.Message;

import java.util.HashMap;

public class CallbackProxy extends Handler implements EventSender, LayoutTestController {
    
    private EventSender mEventSender;
    private LayoutTestController mLayoutTestController;
    
    private static final int EVENT_DOM_LOG = 1;
    private static final int EVENT_FIRE_KBD = 2;
    private static final int EVENT_KEY_DOWN_1 = 3;
    private static final int EVENT_KEY_DOWN_2 = 4;
    private static final int EVENT_LEAP = 5;
    private static final int EVENT_MOUSE_CLICK = 6;
    private static final int EVENT_MOUSE_DOWN = 7;
    private static final int EVENT_MOUSE_MOVE = 8;
    private static final int EVENT_MOUSE_UP = 9;
    
    private static final int LAYOUT_CLEAR_LIST = 20;
    private static final int LAYOUT_DISPLAY = 21;
    private static final int LAYOUT_DUMP_TEXT = 22;
    private static final int LAYOUT_DUMP_HISTORY = 23;
    private static final int LAYOUT_DUMP_CHILD_SCROLL = 24;
    private static final int LAYOUT_DUMP_EDIT_CB = 25;
    private static final int LAYOUT_DUMP_SEL_RECT = 26;
    private static final int LAYOUT_DUMP_TITLE_CHANGES = 27;
    private static final int LAYOUT_KEEP_WEB_HISTORY = 28;
    private static final int LAYOUT_NOTIFY_DONE = 29;
    private static final int LAYOUT_QUEUE_BACK_NAV = 30;
    private static final int LAYOUT_QUEUE_FWD_NAV = 31;
    private static final int LAYOUT_QUEUE_LOAD = 32;
    private static final int LAYOUT_QUEUE_RELOAD = 33;
    private static final int LAYOUT_QUEUE_SCRIPT = 34;
    private static final int LAYOUT_REPAINT_HORZ = 35;
    private static final int LAYOUT_SET_ACCEPT_EDIT = 36;
    private static final int LAYOUT_MAIN_FIRST_RESP = 37;
    private static final int LAYOUT_SET_WINDOW_KEY = 38;
    private static final int LAYOUT_TEST_REPAINT = 39;
    private static final int LAYOUT_WAIT_UNTIL_DONE = 40;
    
    CallbackProxy(EventSender eventSender, 
            LayoutTestController layoutTestController) {
        mEventSender = eventSender;
        mLayoutTestController = layoutTestController;
    }
    
    public void handleMessage(Message msg) {
        switch (msg.what) {
        case EVENT_DOM_LOG:
            mEventSender.enableDOMUIEventLogging(msg.arg1);
            break;
        case EVENT_FIRE_KBD:
            mEventSender.fireKeyboardEventsToElement(msg.arg1);
            break;
        case EVENT_KEY_DOWN_1:
            HashMap map = (HashMap) msg.obj;
            mEventSender.keyDown((String) map.get("character"), 
                    (String[]) map.get("withModifiers"));
            break;

        case EVENT_KEY_DOWN_2:
            mEventSender.keyDown((String)msg.obj);
            break;

        case EVENT_LEAP:
            mEventSender.leapForward(msg.arg1);
            break;

        case EVENT_MOUSE_CLICK:
            mEventSender.mouseClick();
            break;

        case EVENT_MOUSE_DOWN:
            mEventSender.mouseDown();
            break;

        case EVENT_MOUSE_MOVE:
            mEventSender.mouseMoveTo(msg.arg1, msg.arg2);
            break;

        case EVENT_MOUSE_UP:
            mEventSender.mouseUp();
            break;

        case LAYOUT_CLEAR_LIST:
            mLayoutTestController.clearBackForwardList();
            break;

        case LAYOUT_DISPLAY:
            mLayoutTestController.display();
            break;

        case LAYOUT_DUMP_TEXT:
            mLayoutTestController.dumpAsText();
            break;

        case LAYOUT_DUMP_HISTORY:
            mLayoutTestController.dumpBackForwardList();
            break;

        case LAYOUT_DUMP_CHILD_SCROLL:
            mLayoutTestController.dumpChildFrameScrollPositions();
            break;

        case LAYOUT_DUMP_EDIT_CB:
            mLayoutTestController.dumpEditingCallbacks();
            break;

        case LAYOUT_DUMP_SEL_RECT:
            mLayoutTestController.dumpSelectionRect();
            break;

        case LAYOUT_DUMP_TITLE_CHANGES:
            mLayoutTestController.dumpTitleChanges();
            break;

        case LAYOUT_KEEP_WEB_HISTORY:
            mLayoutTestController.keepWebHistory();
            break;

        case LAYOUT_NOTIFY_DONE:
            mLayoutTestController.notifyDone();
            break;

        case LAYOUT_QUEUE_BACK_NAV:
            mLayoutTestController.queueBackNavigation(msg.arg1);
            break;

        case LAYOUT_QUEUE_FWD_NAV:
            mLayoutTestController.queueForwardNavigation(msg.arg1);
            break;

        case LAYOUT_QUEUE_LOAD:
            HashMap<String, String> loadMap = 
                (HashMap<String, String>) msg.obj;
            mLayoutTestController.queueLoad(loadMap.get("Url"), 
                    loadMap.get("frameTarget"));
            break;

        case LAYOUT_QUEUE_RELOAD:
            mLayoutTestController.queueReload();
            break;

        case LAYOUT_QUEUE_SCRIPT:
            mLayoutTestController.queueScript((String)msg.obj);
            break;

        case LAYOUT_REPAINT_HORZ:
            mLayoutTestController.repaintSweepHorizontally();
            break;

        case LAYOUT_SET_ACCEPT_EDIT:
            mLayoutTestController.setAcceptsEditing(
                    msg.arg1 == 1 ? true : false);
            break;
        case LAYOUT_MAIN_FIRST_RESP:
            mLayoutTestController.setMainFrameIsFirstResponder(
                    msg.arg1 == 1 ? true : false);
            break;

        case LAYOUT_SET_WINDOW_KEY:
            mLayoutTestController.setWindowIsKey(
                    msg.arg1 == 1 ? true : false);
            break;

        case LAYOUT_TEST_REPAINT:
            mLayoutTestController.testRepaint();
            break;

        case LAYOUT_WAIT_UNTIL_DONE:
            mLayoutTestController.waitUntilDone();
            break;
        }
    }

    // EventSender Methods
    
    public void enableDOMUIEventLogging(int DOMNode) {
        obtainMessage(EVENT_DOM_LOG, DOMNode, 0).sendToTarget();
    }

    public void fireKeyboardEventsToElement(int DOMNode) {
        obtainMessage(EVENT_FIRE_KBD, DOMNode, 0).sendToTarget();
    }

    public void keyDown(String character, String[] withModifiers) {
        // TODO Auto-generated method stub
        HashMap map = new HashMap();
        map.put("character", character);
        map.put("withModifiers", withModifiers);
        obtainMessage(EVENT_KEY_DOWN_1, map).sendToTarget();
    }

    public void keyDown(String character) {
        obtainMessage(EVENT_KEY_DOWN_2, character).sendToTarget();
    }

    public void leapForward(int milliseconds) {
        obtainMessage(EVENT_LEAP, milliseconds, 0).sendToTarget(); 
    }

    public void mouseClick() {
        obtainMessage(EVENT_MOUSE_CLICK).sendToTarget();
    }

    public void mouseDown() {
        obtainMessage(EVENT_MOUSE_DOWN).sendToTarget();
    }

    public void mouseMoveTo(int X, int Y) {
        obtainMessage(EVENT_MOUSE_MOVE, X, Y).sendToTarget();
    }

    public void mouseUp() {
        obtainMessage(EVENT_MOUSE_UP).sendToTarget();
    }
    
    // LayoutTestController Methods

    public void clearBackForwardList() {
        obtainMessage(LAYOUT_CLEAR_LIST).sendToTarget();
    }

    public void display() {
        obtainMessage(LAYOUT_DISPLAY).sendToTarget();
    }

    public void dumpAsText() {
        obtainMessage(LAYOUT_DUMP_TEXT).sendToTarget();
    }

    public void dumpBackForwardList() {
        obtainMessage(LAYOUT_DUMP_HISTORY).sendToTarget();
    }

    public void dumpChildFrameScrollPositions() {
        obtainMessage(LAYOUT_DUMP_CHILD_SCROLL).sendToTarget();
    }

    public void dumpEditingCallbacks() {
        obtainMessage(LAYOUT_DUMP_EDIT_CB).sendToTarget(); 
    }

    public void dumpSelectionRect() {
        obtainMessage(LAYOUT_DUMP_SEL_RECT).sendToTarget(); 
    }

    public void dumpTitleChanges() {
        obtainMessage(LAYOUT_DUMP_TITLE_CHANGES).sendToTarget();
    }

    public void keepWebHistory() {
        obtainMessage(LAYOUT_KEEP_WEB_HISTORY).sendToTarget();
    }

    public void notifyDone() {
        obtainMessage(LAYOUT_NOTIFY_DONE).sendToTarget();
    }

    public void queueBackNavigation(int howfar) {
        obtainMessage(LAYOUT_QUEUE_BACK_NAV, howfar, 0).sendToTarget();
    }

    public void queueForwardNavigation(int howfar) {
        obtainMessage(LAYOUT_QUEUE_FWD_NAV, howfar, 0).sendToTarget();
    }

    public void queueLoad(String Url, String frameTarget) {
        HashMap <String, String>map = new HashMap<String, String>();
        map.put("Url", Url);
        map.put("frameTarget", frameTarget);
        obtainMessage(LAYOUT_QUEUE_LOAD, map).sendToTarget();
    }

    public void queueReload() {
        obtainMessage(LAYOUT_QUEUE_RELOAD).sendToTarget();
    }

    public void queueScript(String scriptToRunInCurrentContext) {
        obtainMessage(LAYOUT_QUEUE_SCRIPT, 
                scriptToRunInCurrentContext).sendToTarget();
    }

    public void repaintSweepHorizontally() {
        obtainMessage(LAYOUT_REPAINT_HORZ).sendToTarget();
    }

    public void setAcceptsEditing(boolean b) {
        obtainMessage(LAYOUT_SET_ACCEPT_EDIT, b ? 1 : 0, 0).sendToTarget();
    }

    public void setMainFrameIsFirstResponder(boolean b) {
        obtainMessage(LAYOUT_MAIN_FIRST_RESP, b ? 1 : 0, 0).sendToTarget();
    }

    public void setWindowIsKey(boolean b) {
        obtainMessage(LAYOUT_SET_WINDOW_KEY,b ? 1 : 0, 0).sendToTarget();
    }

    public void testRepaint() {
        obtainMessage(LAYOUT_TEST_REPAINT).sendToTarget(); 
    }

    public void waitUntilDone() {
        obtainMessage(LAYOUT_WAIT_UNTIL_DONE).sendToTarget();
    }

}
