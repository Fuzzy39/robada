package io.github.fuzzy39.robada.pcApp;


import java.nio.ByteBuffer;
import java.util.List;
import java.util.Random;

import org.simplejavable.*;


public class Main extends Thread
{
    // Yes, this is shit code. How could you tell?
    static boolean connected = false;
    static Object lock = new Object();
    static Peripheral esp;

    BluetoothUUID serviceUUID = new BluetoothUUID("00001815-0000-1000-8000-00805f9b34fb");
    //BluetoothUUID characteristicUUID = new BluetoothUUID("00001525-1212-efde-1523-785feabcd123");
    BluetoothUUID motor1 = new BluetoothUUID("bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbb0001");
    BluetoothUUID motor2 = new BluetoothUUID("bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbb0101");


    // This is more or less just the example code from the simpleble docs right now.
    public static void main(String[] args) throws Exception
    {
        if(!Adapter.isBluetoothEnabled())
        {
            System.err.println("Bluetooth is not enabled.");
            return;
        }

        List<Adapter> adapters = Adapter.getAdapters();
        if (adapters.isEmpty())
        {
            System.err.println("No Bluetooth adapters found.");
            return;
        }
        Adapter adapter = adapters.get(0);
        System.out.println("Using adapter: " + adapter.getIdentifier() + " [" + adapter.getAddress() + "]");

        // Adapter.EventListener is an interface. Here, we implement that interface with an anonymous class.
        adapter.setEventListener(new Adapter.EventListener() 
        {
            @Override
            public void onScanFound(Peripheral peripheral)
            {
                /*System.out.println("Found: " + peripheral.getIdentifier()
                    + " [" + peripheral.getAddress() + "] "
                    + peripheral.getRssi() + " dBm");*/
            }
        });

        String deviceName = "Robada";//"ESP_Test";
        System.out.println("Looking for '"+deviceName+"'.");
        // number in ms.
        adapter.scanFor(2000);

        // Let's look for the esp...
        System.out.println("Scan results:");


        Peripheral selected = null;

        for (Peripheral peripheral : adapter.scanGetResults()) 
        {
            if(peripheral.getIdentifier().equals(deviceName))
            {
                selected = peripheral;
            }
        }


        if(selected == null)
        {
            System.out.println("Didn't find ESP...");
            return;
        }

        esp = selected;
        System.out.println("ESP: "+esp.getIdentifier()+" ["+esp.getAddress()+"] Connectable: "+esp.isConnectable());
        if(!esp.isConnectable())
        {
            System.out.println("ESP not connectable.");
            return;
        }

        class PeripheralCallback implements Peripheral.EventListener 
        {
            @Override
            public void onConnected() 
            {
                synchronized(lock)
                {
                    connected = true;
                }

          
                System.out.println("Connected to ESP!");
            }

            @Override
            public void onDisconnected() 
            {
                synchronized(lock)
                {
                    connected = false;
                }

                System.out.println("Disconnected.");
            }
        }

        esp.setEventListener(new PeripheralCallback());
        esp.connect();

        new Main().run();

    }


    public void run() 
    {
        
        while (true)
        {
            synchronized(lock)
            {
                if(connected) break;
            }
            try
            {
                Thread.sleep(100); // I feel like having what sorta ammounts to a spinlock isn't great but idk what else to do.
            }
            catch(InterruptedException e)
            {
                // do nothing.
            }
        }

        // So, we assume we're connected...
        System.out.println("MTU: " + esp.getMtu());
             
        Characteristic testChar = null;

        for (Service service : esp.services())
        {
            // print out everything.
            System.out.println("Service: " + service.uuid());
            for (Characteristic characteristic : service.characteristics())
            {
                System.out.println("  Characteristic: " + characteristic.uuid());
                System.out.println("    read=" + characteristic.canRead()
                    + " notify=" + characteristic.canNotify()
                    + " writeRequest=" + characteristic.canWriteRequest()
                    + " writeCommand=" + characteristic.canWriteCommand());
            }

            // this uuid is a string, actually. sure.
            if(service.uuid().equals(serviceUUID.toString()))
            {
                for (Characteristic characteristic : service.characteristics())
                {
                   if(characteristic.uuid().equals(motor1.toString()))
                   {
                        testChar = characteristic;
                   }
                }
            }

        }

        if(testChar == null)
        {
            System.out.println("Couldn't find characteristic...");
            System.out.println("Disconnecting...");
            esp.disconnect();
            return;
        }

        write(motor1, .5f);
        try{Thread.sleep(1500);}
        catch(InterruptedException e){}
        write(motor1, 0);
        try{Thread.sleep(1000);}
        catch(InterruptedException e){}

        write(motor2, -.2f);
        try{Thread.sleep(1500);}
        catch(InterruptedException e){}
        write(motor2, 0);


        System.out.println("All done! Exiting...");
        esp.disconnect();
     
        
    }

    private void write(BluetoothUUID characteristic, float value)
    {
        byte[] bytes =  ByteBuffer.allocate(4).putFloat(value).array();  
        byte reversed[] = new byte[4];
        for(int i = 0; i<4; i++)
        {
            reversed[i]= bytes[3-i];
        } 

        //printBytes("Write Data: ", reversed);
        System.out.printf("Writing %f to %s\n", value, characteristic);
        esp.writeRequest(serviceUUID, characteristic, reversed);
    }

    private float read(BluetoothUUID characteristic)
    {
    
        byte[] bytes = esp.read(serviceUUID, characteristic);
        
      
        if(bytes.length != 4)
        {
            System.out.println("Got length "+bytes.length+" instead of 4.");
            return 0;
        }
        
        byte reversed[] = new byte[4];
        for(int i = 0; i<4; i++)
        {
            reversed[i]= bytes[3-i];
        }
    
        printBytes("Read Data: ", reversed);
        float speed = ByteBuffer.wrap(reversed).getFloat();
        System.out.printf("Characteristic has value %f.\n", speed);
        return speed;
    }


    private void printBytes(String prefix, byte[] bytes)
    {
        System.out.print(prefix);
        for(int  i = 0; i<bytes.length; i++) System.out.printf("%02X", bytes[i]);
        System.out.println();
     
    }
}