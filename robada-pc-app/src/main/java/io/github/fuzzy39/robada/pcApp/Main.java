package io.github.fuzzy39.robada.pcApp;

import java.util.List;

import org.simplejavable.Adapter;
import org.simplejavable.Characteristic;
import org.simplejavable.Peripheral;
import org.simplejavable.Service;

public class Main
{
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
                System.out.println("Found: " + peripheral.getIdentifier()
                    + " [" + peripheral.getAddress() + "] "
                    + peripheral.getRssi() + " dBm");
            }
        });

        // number in ms.
        adapter.scanFor(5000);

        // Let's look for the esp...
        System.out.println("Scan results:");
        String deviceName = "ESP_Test";

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

        final Peripheral esp = selected;
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
                System.out.println("MTU: " + esp.getMtu());
                /*for (Service service : esp.services())
                {
                    System.out.println("Service: " + service.uuid());
                    for (Characteristic characteristic : service.characteristics())
                    {
                        System.out.println("  Characteristic: " + characteristic.uuid());
                        System.out.println("    read=" + characteristic.canRead()
                            + " notify=" + characteristic.canNotify()
                            + " writeRequest=" + characteristic.canWriteRequest()
                            + " writeCommand=" + characteristic.canWriteCommand());
                    }
                }*/
            }

            @Override
            public void onDisconnected() {
                System.out.println("Disconnected.");
                System.exit(0);
            }
        }

        esp.setEventListener(new PeripheralCallback());
        esp.connect();

        // Do... Something.
        // stolen from the connect example
     
        Thread.sleep(2000);
        System.out.println("Disconnecting...");
        esp.disconnect();
        System.exit(0);

    }
}