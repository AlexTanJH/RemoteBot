package com.tanal.msp430bluetoothcontroller;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Set;

public class MainActivity extends AppCompatActivity {


    final char FORWARD = 'a';
    final char BACK = 'b';
    final char LEFT = 'c';
    final char RIGHT = 'd';
    final char GETTEMP = 'e';
    final char STOP = 'f';

    private final String TAG = "MainActivity";
    private BluetoothSocket socket = null;
//    private ConnectThread ct;
//    private Handler handler;
    private OutputStream outputStream;
    private Button[] button = new Button[6];
    TextView textView;
    @SuppressLint("ClickableViewAccessibility")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textView = findViewById(R.id.textView);
//        handler = new Handler(Looper.getMainLooper()){
//            @Override
//            public void handleMessage(Message msg) {
//
//
//            }
//        };


        button[0] = findViewById(R.id.forwardButton);
        button[1] = findViewById(R.id.backButton);
        button[2] = findViewById(R.id.rightButton);
        button[3] = findViewById(R.id.leftButton);
        button[4] = findViewById(R.id.getTemp);
        button[5] = findViewById(R.id.breakButton);

        for (final Button b : button){
            b.setOnTouchListener(new View.OnTouchListener() {
                @Override
                public boolean onTouch(View v, MotionEvent event) {
                    switch (event.getAction()){
                        case MotionEvent.ACTION_DOWN: {
                            sendCommand((int)b.getTag());
                            return true;
                        }
                        case MotionEvent.ACTION_CANCEL: {
                            sendToMSP('x');
                            return true;
                        }
                        case MotionEvent.ACTION_UP: {
                            sendToMSP('x');
                            return true;
                        }

                    }
                    return false;
                }
            });
        }
    }
    private void sendCommand(int tag){
        switch (tag){
            case 1:
                sendToMSP(FORWARD);
                break;
            case 2:
                sendToMSP(BACK);
                break;
            case 3:
                sendToMSP(RIGHT);
                break;
            case 4:
                sendToMSP(LEFT);
                break;
            case 5:
                sendToMSP(GETTEMP);
                break;
            case 6:
                sendToMSP(STOP);
                break;
            default:
                break;
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == 1){
            if(resultCode == RESULT_CANCELED) {
                textView.setText("Bluetooth not enabled");
            }
        }
    }

    public void connectBluetooth(View view){
        BluetoothDevice device = null;
        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if(bluetoothAdapter == null){
            textView.setText("Your device does not support bluetooth");
        }
        if(!bluetoothAdapter.isEnabled()){
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, 1);
        }
        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
        if (pairedDevices.size() > 0 ){
            for (BluetoothDevice bDevice: pairedDevices){
                String nameOfDevice = bDevice.getName();
                if(nameOfDevice.equalsIgnoreCase("HC-06")){
                    device = bDevice;
                }
            }
            if (device == null){
                textView.setText("Please pair the HC-06 device with your android device");
            }
        }else{
            textView.setText("Please pair the HC-06 device with your android device");
            return;
        }
        try {
            socket = device.createRfcommSocketToServiceRecord(ConnectThread.id);
            socket.connect();
        } catch (IOException e) {
            socket = null;
            Log.e(TAG, "Socket's create method failed", e);
        }

//        ct = new ConnectThread(device);
//        ct.start();

        try {
             outputStream = socket.getOutputStream();
        } catch (IOException e) {
            Log.e(TAG, "In onResume() and output stream creation failed:" + e.getMessage() + ".");
            return;
        }

        textView.setText("Connected");
        findViewById(R.id.connectButton).setClickable(false);
        for(Button b : button){
            b.setClickable(true);
        }
    }

    private void sendToMSP(char command){
        try {
            textView.setText("Sent: " + command);
            outputStream.write((byte) command);
        }catch (IOException e){
            Log.e(TAG, "Failed to send to MSP" );
        }catch (NullPointerException e){
            Log.e(TAG, "Stream not opened");
        }
    }



}
