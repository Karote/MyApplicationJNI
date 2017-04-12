package com.coretronic.myapplicationjni;

import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private final static String TAG = MainActivity.class.getSimpleName();

    private final static int command_port = 1688;

    private JNIBridge mJniBridge;

    private EditText edtInputIP;
    private TextView tvIPStatus;
    private TextView tvSendCmd;
    private TextView tvGetAck;

    private TextView tvMacAddress;

    private String mIP;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (mJniBridge == null) {
            mJniBridge = new JNIBridge();
            mJniBridge.initialJNI();
            mJniBridge.registerCppFunctionCallBack(onCppFunctionCall);
        }

        edtInputIP = (EditText) findViewById(R.id.edt_input_ip);
        findViewById(R.id.btn_ping).setOnClickListener(this);
        tvIPStatus = (TextView) findViewById(R.id.tv_ip_status);
        tvSendCmd = (TextView) findViewById(R.id.tv_send_cmd);
        tvGetAck = (TextView) findViewById(R.id.tv_get_ack);
        tvMacAddress = (TextView)findViewById(R.id.tv_mac_address);
        findViewById(R.id.btn_make_blue_screen).setOnClickListener(this);
        findViewById(R.id.btn_clear_screen).setOnClickListener(this);
        findViewById(R.id.btn_get_mac_address).setOnClickListener(this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (mJniBridge != null) {
            mJniBridge.destroyJNI();
        }
    }

    private JNIBridge.OnCppFunctionCall onCppFunctionCall = new JNIBridge.OnCppFunctionCall() {
        @Override
        public void sendCommandPacketOnJava(byte[] data, int length) {
            new sendCommandTask().execute(data, length);
        }

        @Override
        public void readCommandOnJava(byte[] data, int length) {
            new readCommandTask().execute(data, length);
        }

        @Override
        public void unpackedPacketToJava(byte[] unpackedPacket) {
            Log.d(TAG, "unpackedPacket:" + unpackedPacket);
            Log.d(TAG, "unpackedPacketToJava - unpackedPacket.length:" + unpackedPacket.length);
            if (unpackedPacket.length >= 6) {
                String macAddress = String.format("%02x-%02x-%02x-%02x-%02x-%02x",
                        unpackedPacket[0], unpackedPacket[1], unpackedPacket[2],
                        unpackedPacket[3], unpackedPacket[4], unpackedPacket[5]);
                tvMacAddress.setText("MAC:" + macAddress);
            }
        }
    };

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_ping:
                mIP = edtInputIP.getText().toString();
                new ConnectCheckTask().execute(mIP);
                break;
            case R.id.btn_make_blue_screen:
                mJniBridge.makeBlueScreen(mIP);
                break;
            case R.id.btn_clear_screen:
                mJniBridge.clearScreenColor();
                break;
            case R.id.btn_get_mac_address:
                mJniBridge.getMACAddress(mIP);
                break;
        }
    }

    private class ConnectCheckTask extends AsyncTask<String, Void, Boolean> {

        @Override
        protected Boolean doInBackground(String... params) {
            String strIP = params[0];
            try {
                InetAddress address = InetAddress.getByName(strIP);
                return address.isReachable(5000);
            } catch (UnknownHostException e) {
                e.printStackTrace();
                return false;
            } catch (IOException e) {
                e.printStackTrace();
                return false;
            }
        }

        @Override
        protected void onPostExecute(Boolean aBoolean) {
            super.onPostExecute(aBoolean);

            if (aBoolean) {
                tvIPStatus.setText("Connected");
                tvIPStatus.setTextColor(0xFF00FF00);
            } else {
                tvIPStatus.setText("Un-connected");
                tvIPStatus.setTextColor(0xFFFF0000);
            }
            findViewById(R.id.btn_make_blue_screen).setEnabled(aBoolean);
            findViewById(R.id.btn_clear_screen).setEnabled(aBoolean);
            findViewById(R.id.btn_get_mac_address).setEnabled(aBoolean);
        }
    }

    private class sendCommandTask extends AsyncTask<Object, Void, Void> {

        String strSend = "";
        String strGet = "";

        @Override
        protected Void doInBackground(Object... params) {
            byte[] buffer = (byte[]) params[0];
            int len = (int) params[1];
            try {
                Log.d(TAG, "len:" + len);

                for (int i = 0; i < len; i++) {
                    if (i != 0) {
                        strSend += "-";
                    }
                    strSend += String.format("%02x", buffer[i]);
                }

                Log.d(TAG, "strSend:" + strSend);

                Socket socket = new Socket(mIP, command_port);
                DataOutputStream dos = new DataOutputStream(socket.getOutputStream());
                dos.write(buffer, 0, len);
                dos.flush();

                DataInputStream dis = new DataInputStream(socket.getInputStream());

                int inLength = dis.available();
                while (inLength == 0) {
                    inLength = dis.available();
                }

                byte[] inBuffer = new byte[inLength];
                dis.read(inBuffer);


                dos.close();
                Log.d(TAG, "dos.close()");
                dis.close();
                Log.d(TAG, "dis.close()");


                Log.d(TAG, "inLength:" + inLength);

                for (int i = 0; i < inLength; i++) {
                    if (i != 0) {
                        strGet += "-";
                    }
                    strGet += String.format("%02x", inBuffer[i]);
                }

                Log.d(TAG, "strGet:" + strGet);

                socket.close();
                Log.d(TAG, "socket.close()");

            } catch (UnknownHostException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void aVoid) {
            super.onPostExecute(aVoid);
            tvSendCmd.setText(strSend);
            tvGetAck.setText(strGet);
        }
    }

    private class readCommandTask extends AsyncTask<Object, Void, byte[]> {

        String strSend = "";
        String strGet = "";

        @Override
        protected byte[] doInBackground(Object... params) {
            byte[] outBuffer = (byte[]) params[0];
            int outLength = (int) params[1];
            try {

                Log.d(TAG, "len:" + outLength);

                for (int i = 0; i < outLength; i++) {
                    if (i != 0) {
                        strSend += "-";
                    }
                    strSend += String.format("%02x", outBuffer[i]);
                }

                Log.d(TAG, "strSend:" + strSend);

                Socket socket = new Socket(mIP, command_port);
                DataOutputStream dos = new DataOutputStream(socket.getOutputStream());

                dos.write(outBuffer, 0, outLength);
                dos.flush();

                Log.d(TAG, "dos.flush()");

                DataInputStream dis = new DataInputStream(socket.getInputStream());

                int inLength = dis.available();
                while (inLength == 0) {
                    inLength = dis.available();
                }

                byte[] inBuffer = new byte[inLength];
                dis.read(inBuffer);


                dos.close();
                Log.d(TAG, "dos.close()");
                dis.close();
                Log.d(TAG, "dis.close()");

                socket.close();

                Log.d(TAG, "socket.close()");

                Log.d(TAG, "inLength:" + inLength);

                for (int i = 0; i < inLength; i++) {
                    if (i != 0) {
                        strGet += "-";
                    }
                    strGet += String.format("%02x", inBuffer[i]);
                }

                Log.d(TAG, "strGet:" + strGet);

                return inBuffer;
            } catch (UnknownHostException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(byte[] bytes) {
            super.onPostExecute(bytes);
            tvSendCmd.setText(strSend);
            tvGetAck.setText(strGet);
            mJniBridge.decodePacket(bytes);
        }
    }

}
