/*
 * Copyright (C) 2008 The Android Open Source Project
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

package tests.api.java.net;

import dalvik.annotation.TestTargets;
import dalvik.annotation.TestLevel;
import dalvik.annotation.TestTargetNew;
import dalvik.annotation.TestTargetClass;

import junit.framework.TestCase;

import java.io.IOException;
import java.lang.reflect.Field;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.DatagramSocketImpl;
import java.net.DatagramSocketImplFactory;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketAddress;
import java.net.SocketException;

@TestTargetClass(DatagramSocketImplFactory.class) 
public class DatagramSocketImplFactoryTest extends TestCase {
    
    DatagramSocketImplFactory oldFactory = null;
    Field factoryField = null;
    
    boolean isTestable = false;
    
    boolean isDatagramSocketImplCalled = false;
    boolean isCreateDatagramSocketImpl = false;
    
    @TestTargets({
        @TestTargetNew(
            level = TestLevel.COMPLETE,
            notes = "",
            method = "createDatagramSocketImpl",
            args = {}
        ),
        @TestTargetNew(
            level = TestLevel.PARTIAL_COMPLETE,
            notes = "Verifies SecurityException.",
            clazz = DatagramSocket.class,
            method = "setDatagramSocketImplFactory",
            args = {java.net.DatagramSocketImplFactory.class}
        )
    })
    public void test_createDatagramSocketImpl() throws IllegalArgumentException, 
                                                                    IOException {

        if(isTestable) {
            
            DatagramSocketImplFactory factory = new TestDatagramSocketImplFactory();
            assertFalse(isCreateDatagramSocketImpl);
            DatagramSocket.setDatagramSocketImplFactory(factory);

            try {
                DatagramSocket ds = new java.net.DatagramSocket();
                assertTrue(isCreateDatagramSocketImpl);
                assertTrue(isDatagramSocketImplCalled);
            } catch (Exception e) {
                fail("Exception during test : " + e.getMessage());
            
            }
            
            try {
                DatagramSocket.setDatagramSocketImplFactory(factory);
                fail("SocketException was not thrown.");                
            } catch(SocketException se) {
                //expected
            }
            
            try {
                DatagramSocket.setDatagramSocketImplFactory(null);
                fail("SocketException was not thrown.");                
            } catch(SocketException se) {
                //expected
            }
            
        } else {
            
            TestDatagramSocketImplFactory dsf = new TestDatagramSocketImplFactory();
            DatagramSocketImpl dsi = dsf.createDatagramSocketImpl();
            try {
                assertNull(dsi.getOption(0));
            } catch (SocketException e) {
                fail("SocketException was thrown.");
            }
            
        }
    }
    
    public void setUp() {
        Field [] fields = DatagramSocket.class.getDeclaredFields();
        int counter = 0;
        for (Field field : fields) {
            if (DatagramSocketImplFactory.class.equals(field.getType())) {
                counter++;
                factoryField = field;
            }
        } 
        
        if(counter == 1) {
            
            isTestable = true;
    
            factoryField.setAccessible(true);
            try {
                oldFactory = (DatagramSocketImplFactory) factoryField.get(null);
            } catch (IllegalArgumentException e) {
                fail("IllegalArgumentException was thrown during setUp: " 
                        + e.getMessage());
            } catch (IllegalAccessException e) {
                fail("IllegalAccessException was thrown during setUp: "
                        + e.getMessage());
            }        
        }
    }
    
    public void tearDown() {
        if(isTestable) {
            try {
                factoryField.set(null, oldFactory);
            } catch (IllegalArgumentException e) {
                fail("IllegalArgumentException was thrown during tearDown: " 
                        + e.getMessage());
            } catch (IllegalAccessException e) {
                fail("IllegalAccessException was thrown during tearDown: "
                        + e.getMessage());
            }
        }
    }
    
    class TestDatagramSocketImplFactory implements DatagramSocketImplFactory {
        public DatagramSocketImpl createDatagramSocketImpl() {
            isCreateDatagramSocketImpl = true;
            return new TestDatagramSocketImpl();
        }
    }
    
    class TestDatagramSocketImpl extends DatagramSocketImpl {

        @Override
        protected void bind(int arg0, InetAddress arg1) throws SocketException {
            // TODO Auto-generated method stub
            
        }

        @Override
        protected void close() {
            // TODO Auto-generated method stub
            
        }

        @Override
        protected void create() throws SocketException {
            isDatagramSocketImplCalled = true;
        }

        @Override
        protected byte getTTL() throws IOException {
            // TODO Auto-generated method stub
            return 0;
        }

        @Override
        protected int getTimeToLive() throws IOException {
            // TODO Auto-generated method stub
            return 0;
        }

        @Override
        protected void join(InetAddress arg0) throws IOException {
            // TODO Auto-generated method stub
            
        }

        @Override
        protected void joinGroup(SocketAddress arg0, NetworkInterface arg1) throws IOException {
            // TODO Auto-generated method stub
            
        }

        @Override
        protected void leave(InetAddress arg0) throws IOException {
            // TODO Auto-generated method stub
            
        }

        @Override
        protected void leaveGroup(SocketAddress arg0, NetworkInterface arg1) throws IOException {
            // TODO Auto-generated method stub
            
        }

        @Override
        public int peek(InetAddress arg0) throws IOException {
            // TODO Auto-generated method stub
            return 10;
        }

        @Override
        protected int peekData(DatagramPacket arg0) throws IOException {
            // TODO Auto-generated method stub
            return 0;
        }

        @Override
        protected void receive(DatagramPacket arg0) throws IOException {
            // TODO Auto-generated method stub
            
        }

        @Override
        protected void send(DatagramPacket arg0) throws IOException {
            // TODO Auto-generated method stub
            
        }

        @Override
        protected void setTTL(byte arg0) throws IOException {
            // TODO Auto-generated method stub
            
        }

        @Override
        protected void setTimeToLive(int arg0) throws IOException {
            // TODO Auto-generated method stub
            
        }

        public Object getOption(int arg0) throws SocketException {
            // TODO Auto-generated method stub
            return null;
        }

        public void setOption(int arg0, Object arg1) throws SocketException {
            // TODO Auto-generated method stub
            
        }
    }    
}
