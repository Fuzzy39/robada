#include <iostream>
#include <simpleble/SimpleBLE.h>


int main() 
{
    if (!SimpleBLE::Adapter::bluetooth_enabled())
    {
        std::cerr << "Bluetooth is not enabled or permission has not been granted." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Bluetooth is available." << std::endl;
    
    auto adapters = SimpleBLE::Adapter::get_adapters();
    if (adapters.empty()) {
        std::cerr << "No Bluetooth adapters found." << std::endl;
        return EXIT_FAILURE;
    }

    auto adapter = adapters[0];
    std::cout << "Using adapter: " << adapter.identifier() << " [" << adapter.address() << "]" << std::endl;

    std::vector<SimpleBLE::Peripheral> peripherals;

    adapter.set_callback_on_scan_start([]() { std::cout << "Scan started." << std::endl; });
    adapter.set_callback_on_scan_stop([]() { std::cout << "Scan stopped." << std::endl; });

    int devicesFound = 0;
    adapter.set_callback_on_scan_found([&](SimpleBLE::Peripheral peripheral) {
        std::cout << "Found device: " << peripheral.identifier() << " [" << peripheral.address() << "]" << std::endl;
        devicesFound++;
        if (peripheral.is_connectable()) {
            peripherals.push_back(peripheral);
        }
    });

    adapter.scan_for(5000);
    std::cout<<devicesFound<<"\n";

    std::string device ="65\" Business TV BED-H";
    bool hasConnected = false;
    SimpleBLE::Peripheral peripheral;
    for (std::size_t i = 0; i < peripherals.size(); i++) 
    {
        SimpleBLE::Peripheral p = peripherals[i];
        if(p.identifier() == device)
        {
            std::cout<<"Found DEvice\n";
            peripheral = peripherals[i];
            hasConnected = true;
            break;
        }
    }

    if(!hasConnected)
    {
        std::cout<<"WFHWOHWOE\n";
        return -1;
    }

    peripheral.connect();

    std::vector<std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID>> readable_characteristics;
    for (auto& service : peripheral.services()) {
        for (SimpleBLE::Characteristic& characteristic : (service.characteristics())) {
            if (characteristic.can_read()) {
                readable_characteristics.emplace_back(service.uuid(), characteristic.uuid());
            }
        }
    }

    if (readable_characteristics.empty()) {
        std::cerr << "The peripheral has no readable characteristics." << std::endl;
        peripheral.disconnect();
        return EXIT_FAILURE;
    }

    std::cout << "Readable characteristics:" << std::endl;
    for (std::size_t i = 0; i < readable_characteristics.size(); i++) {
        std::cout << "[" << i << "] " << readable_characteristics[i].first << " " << readable_characteristics[i].second << std::endl;
    }
}
