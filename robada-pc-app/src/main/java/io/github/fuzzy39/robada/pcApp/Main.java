package io.github.fuzzy39.robada.pcApp;

import org.simplejavable.Adapter;
import org.simplejavable.Peripheral;

import java.util.List;

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
        adapter.scanFor(10000);

        System.out.println("Scan results:");
        for (Peripheral peripheral : adapter.scanGetResults()) 
        {
            String state = peripheral.isConnectable() ? "connectable" : "not connectable";
            System.out.println("- " + peripheral.getIdentifier()
                + " [" + peripheral.getAddress() + "] "
                + state);
        }

    }
}