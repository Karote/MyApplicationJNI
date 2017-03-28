#include <jni.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include "Packet.h"

typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned long DWORD;
#define COLORRGB(r, g, b) ((long)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GB102CMD_DISPLAY_SOLID_COLOR_W                0x005D
#define CMD_MAC_ADDR_R                0x8020    // get MAC address
#define CMD_FW_VERSION_R            0x801F // read firmware version


static JavaVM *Jvm;
jobject cachedJniBridge;
jobject cachedInnerClass;

unsigned char booleanVar;
signed char byteVar;
unsigned short charVar;
short shortVar;
int intVar;
long longVar;
float floatVar;
double doubleVar;

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    Jvm = vm;
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL
Java_com_coretronic_myapplicationjni_JNIBridge_initJNI(JNIEnv *env, jobject instance,
                                                       jobject jniBridge, jobject innerClass) {
    cachedJniBridge = env->NewGlobalRef(jniBridge);
    cachedInnerClass = env->NewGlobalRef(innerClass);
}


extern "C" JNIEXPORT void JNICALL
Java_com_coretronic_myapplicationjni_JNIBridge_releaseJNI(JNIEnv *env, jobject instance) {

    env->DeleteGlobalRef(cachedJniBridge);
    env->DeleteGlobalRef(cachedInnerClass);
}

struct CommandPacket {
    char cmd[1024];
    double size_cmd;
};

void DrawSolidColor(CommandPacket &packet, char nR, char nG, char nB) {
    packet.size_cmd = 10;
    packet.cmd[0] = 0xfe;
    packet.cmd[1] = 0x00;
    packet.cmd[2] = 0x5d;
    packet.cmd[3] = 0x00;
    packet.cmd[4] = 0x04;
    packet.cmd[5] = nR;//R
    packet.cmd[6] = nG;//G
    packet.cmd[7] = nB;//B
    packet.cmd[8] = 0x01;
    packet.cmd[9] = 0xff;
}

void ClearSolidColor(CommandPacket &packet) {
    packet.size_cmd = 10;
    packet.cmd[0] = 0xfe;
    packet.cmd[1] = 0x00;
    packet.cmd[2] = 0x5d;
    packet.cmd[3] = 0x00;
    packet.cmd[4] = 0x04;
    packet.cmd[5] = 0x00;
    packet.cmd[6] = 0x00;
    packet.cmd[7] = 0x00;
    packet.cmd[8] = 0x00;
    packet.cmd[9] = 0xff;
}

void GetMacAddress(CommandPacket &packet) {
    packet.size_cmd = 12;
    packet.cmd[0] = 0xfe;
    packet.cmd[1] = 0x80;
    packet.cmd[2] = 0x20;
    packet.cmd[3] = 0x00;
    packet.cmd[4] = 0x06;
    packet.cmd[5] = 0x00;
    packet.cmd[6] = 0x00;
    packet.cmd[7] = 0x00;
    packet.cmd[8] = 0x00;
    packet.cmd[9] = 0x00;
    packet.cmd[10] = 0x00;
    packet.cmd[11] = 0xff;
}

void getFromJavaInnerClassVariable(JNIEnv *env) {
    jclass innerClass = env->GetObjectClass(cachedInnerClass);

    jfieldID booleanVariableId = env->GetFieldID(innerClass, "mBooleanVariable", "Z");
    jfieldID byteVariableId = env->GetFieldID(innerClass, "mByteVariable", "B");
    jfieldID charVariableId = env->GetFieldID(innerClass, "mCharVariable", "C");
    jfieldID shortVariableId = env->GetFieldID(innerClass, "mShortVariable", "S");
    jfieldID integerVariableId = env->GetFieldID(innerClass, "mIntegerVariable", "I");
    jfieldID longVariableId = env->GetFieldID(innerClass, "mLongVariable", "J");
    jfieldID floatVariableId = env->GetFieldID(innerClass, "mFloatVariable", "F");
    jfieldID doubleVariableId = env->GetFieldID(innerClass, "mDoubleVariable", "D");

    booleanVar = env->GetBooleanField(cachedInnerClass, booleanVariableId);
    byteVar = env->GetByteField(cachedInnerClass, byteVariableId);
    charVar = env->GetCharField(cachedInnerClass, charVariableId);
    shortVar = env->GetShortField(cachedInnerClass, shortVariableId);
    intVar = env->GetIntField(cachedInnerClass, integerVariableId);
    longVar = env->GetLongField(cachedInnerClass, longVariableId);
    floatVar = env->GetFloatField(cachedInnerClass, floatVariableId);
    doubleVar = env->GetDoubleField(cachedInnerClass, doubleVariableId);

}

void updateJavaInnerClassVariable(JNIEnv *env) {
    jclass innerClass = env->GetObjectClass(cachedInnerClass);

    jfieldID booleanVariableId = env->GetFieldID(innerClass, "mBooleanVariable", "Z");
    jfieldID byteVariableId = env->GetFieldID(innerClass, "mByteVariable", "B");
    jfieldID charVariableId = env->GetFieldID(innerClass, "mCharVariable", "C");
    jfieldID shortVariableId = env->GetFieldID(innerClass, "mShortVariable", "S");
    jfieldID integerVariableId = env->GetFieldID(innerClass, "mIntegerVariable", "I");
    jfieldID longVariableId = env->GetFieldID(innerClass, "mLongVariable", "J");
    jfieldID floatVariableId = env->GetFieldID(innerClass, "mFloatVariable", "F");
    jfieldID doubleVariableId = env->GetFieldID(innerClass, "mDoubleVariable", "D");

    env->SetBooleanField(cachedInnerClass, booleanVariableId, booleanVar);
    env->SetByteField(cachedInnerClass, byteVariableId, byteVar);
    env->SetCharField(cachedInnerClass, charVariableId, charVar);
    env->SetShortField(cachedInnerClass, shortVariableId, shortVar);
    env->SetIntField(cachedInnerClass, integerVariableId, intVar);
    env->SetLongField(cachedInnerClass, longVariableId, longVar);
    env->SetFloatField(cachedInnerClass, floatVariableId, floatVar);
    env->SetDoubleField(cachedInnerClass, doubleVariableId, doubleVar);
}

JNIEXPORT void JNICALL
Java_com_coretronic_myapplicationjni_JNIBridge_getIpCamImageOnC(JNIEnv *env, jclass type) {

    // TODO
//    cv::VideoCapture vcap;
//    cv::Mat image;
}

extern "C" JNIEXPORT void JNICALL
Java_com_coretronic_myapplicationjni_JNIBridge_makeBlueScreenOnC(JNIEnv *env, jclass type, jstring ip_) {

    const char *ip = env->GetStringUTFChars(ip_, 0);

//    int nId = COLORRGB(0, 0, 255);
//    BYTE dat[] = {BYTE(nId & 0xff), BYTE((nId >> 8) & 0xff), BYTE((nId >> 16) & 0xff), BYTE(true)};
//    CPacket send;
//    send.Pack(GB102CMD_DISPLAY_SOLID_COLOR_W, 4, dat);

    CommandPacket cmdPkt;

    DrawSolidColor(cmdPkt, 0, 0, 255);

    struct sockaddr_in server;
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(1688);

    inet_pton(AF_INET, ip, &server.sin_addr.s_addr);

    connect(sock, (struct sockaddr *) &server, sizeof(server));

    write(sock, cmdPkt.cmd, cmdPkt.size_cmd);

    close(sock);

    env->ReleaseStringUTFChars(ip_, ip);

//    Jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
//    jclass mavlinkBridgeClass = env->GetObjectClass(cachedJniBridge);
//    jmethodID mid = env->GetMethodID(mavlinkBridgeClass, "sendCommandFromC", "([BI)V");
////    int len = (int) send.m_pPacket->_used;
//    int len = cmdPkt.size_cmd;
//    jbyteArray byteArray = env->NewByteArray(len);
////    env->SetByteArrayRegion(byteArray, 0, len, (jbyte *) send.m_pPacket->_block);
//    env->SetByteArrayRegion(byteArray, 0, cmdPkt.size_cmd, (jbyte *) cmdPkt.cmd);
//    env->CallVoidMethod(cachedJniBridge, mid, byteArray, len);
//    env->DeleteLocalRef(mavlinkBridgeClass);
//    env->DeleteLocalRef(byteArray);
}

extern "C" JNIEXPORT void JNICALL
Java_com_coretronic_myapplicationjni_JNIBridge_clearScreenColorOnC(JNIEnv *env, jclass type) {

    unsigned int color = 0x00000000;
    bool bEnabled = false;
    CPacket send;
    send.PackHeader(GB102CMD_DISPLAY_SOLID_COLOR_W, 4);
    send.PackData((LPBYTE) &color, 3);
    send.PackData((LPBYTE) &bEnabled, 1);
    send.PackTail();

//    CommandPacket cmdPkt;
//
//    ClearSolidColor(cmdPkt);

    Jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    jclass mavlinkBridgeClass = env->GetObjectClass(cachedJniBridge);
    jmethodID mid = env->GetMethodID(mavlinkBridgeClass, "sendCommandFromC", "([BI)V");
    int len = (int) send.m_pPacket->_used;
//    int len = cmdPkt.size_cmd;
    jbyteArray byteArray = env->NewByteArray(len);
    env->SetByteArrayRegion(byteArray, 0, len, (jbyte *) send.m_pPacket->_block);
//    env->SetByteArrayRegion(byteArray, 0, len, (jbyte *) cmdPkt.cmd);
    env->CallVoidMethod(cachedJniBridge, mid, byteArray, len);
    env->DeleteLocalRef(mavlinkBridgeClass);
    env->DeleteLocalRef(byteArray);
}

//extern "C" JNIEXPORT void JNICALL
//Java_com_coretronic_myapplicationjni_JNIBridge_getMACAddressOnC(JNIEnv *env, jclass type) {
//
//    CPacket send;
//    BYTE mac_addr[6];
//    memset(mac_addr, 0, 6);
//
//    send.Pack(CMD_MAC_ADDR_R, 6, (LPBYTE) mac_addr);
//
//    Jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
//    jclass mavlinkBridgeClass = env->GetObjectClass(cachedJniBridge);
//    jmethodID mid = env->GetMethodID(mavlinkBridgeClass, "readCommandFromC", "([BI)V");
//    int len = (int) send.m_pPacket->_used;
//    jbyteArray byteArray = env->NewByteArray(len);
//    env->SetByteArrayRegion(byteArray, 0, len, (jbyte *) send.m_pPacket->_block);
//    env->CallVoidMethod(cachedJniBridge, mid, byteArray, len);
//    env->DeleteLocalRef(mavlinkBridgeClass);
//    env->DeleteLocalRef(byteArray);
//
//}


extern "C" JNIEXPORT void JNICALL
Java_com_coretronic_myapplicationjni_JNIBridge_getMACAddressOnC(JNIEnv *env, jclass type,
                                                                jstring ip_) {
    const char *ip = env->GetStringUTFChars(ip_, 0);

    // TODO
    CommandPacket cmdPkt;

    GetMacAddress(cmdPkt);

    struct sockaddr_in server;
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(1688);

    inet_pton(AF_INET, ip, &server.sin_addr.s_addr);

    connect(sock, (struct sockaddr *) &server, sizeof(server));

    write(sock, cmdPkt.cmd, cmdPkt.size_cmd);

    close(sock);

    env->ReleaseStringUTFChars(ip_, ip);
}

extern "C" JNIEXPORT void JNICALL
Java_com_coretronic_myapplicationjni_JNIBridge_getFwVersionOnC(JNIEnv *env, jclass type) {

    // TODO
    CPacket send;
    unsigned short fw = 0xffff;

    send.Pack(CMD_FW_VERSION_R, sizeof(fw), (LPBYTE) &fw);

    Jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    jclass mavlinkBridgeClass = env->GetObjectClass(cachedJniBridge);
    jmethodID mid = env->GetMethodID(mavlinkBridgeClass, "readCommandFromC", "([BI)V");
    int len = (int) send.m_pPacket->_used;
    jbyteArray byteArray = env->NewByteArray(len);
    env->SetByteArrayRegion(byteArray, 0, len, (jbyte *) send.m_pPacket->_block);
    env->CallVoidMethod(cachedJniBridge, mid, byteArray, len);
    env->DeleteLocalRef(mavlinkBridgeClass);
    env->DeleteLocalRef(byteArray);
}

extern "C" JNIEXPORT void JNICALL
Java_com_coretronic_myapplicationjni_JNIBridge_decodePacketOnC(JNIEnv *env, jclass type,
                                                               jbyteArray packet_, jint length) {
    jbyte *packet = env->GetByteArrayElements(packet_, NULL);

    // TODO
    CPacket recvPkt(128);
    recvPkt.m_pPacket->_used = length;
    memcpy(recvPkt.m_pPacket->_block, packet, length);

    CPacket::PACKET_HEADER h;
    CPacket::PACKET_TAIL t;

    recvPkt.UnpackAckRead(&h, &t);

    WORD wLen = BYTES2WORD(h.aucLen[0], h.aucLen[1]);

    mem_block *ppData = 0;

    mem_block *b_block;
    b_block = (mem_block *) malloc(sizeof(mem_block));
    assert(b_block);
    b_block->_used = 0;
    b_block->_size = wLen;
    b_block->_allocated = wLen;
    b_block->_block = (void *) malloc(wLen);
    assert(b_block->_block);

    ppData = b_block;

    recvPkt.UnpackAckReadData(&h, (LPBYTE) ppData->_block);
    ppData->_used = wLen;

    BYTE mac[6];
    memset(mac, 0, 6);
    memcpy((LPBYTE) mac, (LPBYTE) ppData->_block, 6);

    Jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    jclass mavlinkBridgeClass = env->GetObjectClass(cachedJniBridge);
    jmethodID mid = env->GetMethodID(mavlinkBridgeClass, "unpackedPacketFromC", "([B)V");
    jbyteArray byteArray = env->NewByteArray(6);
    env->SetByteArrayRegion(byteArray, 0, 6, (jbyte *) mac);
    env->CallVoidMethod(cachedJniBridge, mid, byteArray);
    env->DeleteLocalRef(mavlinkBridgeClass);
    env->DeleteLocalRef(byteArray);


    env->ReleaseByteArrayElements(packet_, packet, 0);
}