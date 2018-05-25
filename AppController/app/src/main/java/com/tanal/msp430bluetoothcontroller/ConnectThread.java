package com.tanal.msp430bluetoothcontroller;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

import java.io.IOException;
import java.util.UUID;

class ConnectThread extends Thread{
    private final BluetoothSocket socket;
    private final BluetoothDevice device;
    private final String TAG = "Connect Thread";
    protected static final UUID id = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    public ConnectThread(BluetoothDevice device) {

        // Use a temporary object that is later assigned to mmSocket
        // because mmSocket is final.
        BluetoothSocket tmp = null;
        this.device = device;

        try {
            // Get a BluetoothSocket to connect with the given BluetoothDevice.
            // MY_UUID is the app's UUID string, also used in the server code.
            tmp = device.createRfcommSocketToServiceRecord(id);
        } catch (IOException e) {
            Log.e(TAG, "Socket's create() method failed", e);
        }
        socket = tmp;
    }

    public void run() {
        // Cancel discovery because it otherwise slows down the connection.
        // mBluetoothAdapter.cancelDiscovery();

        try {
            // Connect to the remote device through the socket. This call blocks
            // until it succeeds or throws an exception.
            socket.connect();
        } catch (IOException connectException) {
            // Unable to connect; close the socket and return.
            try {
                socket.close();
            } catch (IOException closeException) {
                Log.e(TAG, "Could not close the client socket", closeException);
            }
            return;
        }
    }

    public BluetoothSocket getSocket(){
        return socket;
    }

    // Closes the client socket and causes the thread to finish.
    public void cancel() {
        try {
            socket.close();
        } catch (IOException e) {
            Log.e(TAG, "Could not close the client socket", e);
        }
    }

}
