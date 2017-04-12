package com.coretronic.myapplicationjni;

import android.util.Log;

/**
 * Created by karot.chuang on 2017/2/6.
 */

public class JNIBridge {
    private final static String TAG = JNIBridge.class.getSimpleName();

    static {
        System.loadLibrary("myNdkDemo");
    }

    public class InnerClass{
        private boolean mBooleanVariable;
        private byte mByteVariable;
        private char mCharVariable;
        private short mShortVariable;
        private int mIntegerVariable;
        private long mLongVariable;
        private float mFloatVariable;
        private double mDoubleVariable;
    }

    InnerClass innerClass = new InnerClass();

    public void makeBlueScreen(String ip) {
        makeBlueScreenOnC(ip);
    }

    public void clearScreenColor() {
        clearScreenColorOnC();
    }

    public void getMACAddress(String ip) {
        getMACAddressOnC(ip);
    }

    public void decodePacket(byte[] packet) {
        decodePacketOnC(packet, packet.length);
    }


    public interface OnCppFunctionCall {
        void sendCommandPacketOnJava(byte[] data, int length);

        void readCommandOnJava(byte[] data, int length);

        void unpackedPacketToJava(byte[] unpackedPacket);
    }

    private OnCppFunctionCall onCppFunctionCall;

    public void registerCppFunctionCallBack(OnCppFunctionCall callback) {
        onCppFunctionCall = callback;
    }

    public void initialJNI() {
        initJNI(this, innerClass);
    }

    public void destroyJNI() {
        releaseJNI();
    }

    public void sendCommandFromC(byte[] data, int length) {
        if (onCppFunctionCall != null) {
            onCppFunctionCall.sendCommandPacketOnJava(data, length);
        }
    }

    public void readCommandFromC(byte[] data, int length) {
        if (onCppFunctionCall != null) {
            onCppFunctionCall.readCommandOnJava(data, length);
        }
    }

    public void unpackedPacketFromC(byte[] unpackedPacket) {
        if (onCppFunctionCall != null) {
            Log.d(TAG, "onCppFunctionCall.unpackedPacketToJava");
            onCppFunctionCall.unpackedPacketToJava(unpackedPacket);
        }
    }

    private static native void initJNI(JNIBridge jniBridge, InnerClass innerClass);

    private static native void releaseJNI();

    private static native void makeBlueScreenOnC(String ip);

    private static native void clearScreenColorOnC();

    private static native void getMACAddressOnC(String ip);

    private static native void decodePacketOnC(byte[] packet, int length);
}
