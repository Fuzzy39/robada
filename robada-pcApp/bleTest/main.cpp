#include <iostream>
#include <simpleble/SimpleBLE.h>
#include <bit>
#include <chrono>
#include <thread>



const SimpleBLE::BluetoothUUID serviceUUID("00001815-0000-1000-8000-00805f9b34fb");
const SimpleBLE::BluetoothUUID motor1("bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbb0001");
const SimpleBLE::BluetoothUUID motor2("bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbb0101");

kvn::bytearray floatAsBytes(float f)
{
    static_assert(sizeof(uint32_t) == sizeof(float), "Can't memcpy a float to an int");
    uint32_t copyTo;
    std::memcpy(&copyTo, &f, sizeof(float));
    //copyTo = std::byteswap(copyTo);

    return kvn::bytearray((char *)&copyTo, sizeof(uint32_t));
}

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

    // Scan for peripherals
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

    std::string device ="Robada";
    bool hasConnected = false;
    SimpleBLE::Peripheral peripheral;
    for (std::size_t i = 0; i < peripherals.size(); i++) 
    {
        SimpleBLE::Peripheral p = peripherals[i];
        if(p.identifier() == device)
        {
            std::cout<<"Found Device\n";
            peripheral = peripherals[i];
            hasConnected = true;
            break;
        }
    }

    if(!hasConnected)
    {
        std::cout<<"Didn't Find device '"<<device<<"'.\n";
        return -1;
    }

    peripheral.connect();

    SimpleBLE::Characteristic* Motor1Speed = nullptr;
    SimpleBLE::Characteristic* Motor2Speed = nullptr;

    for (auto& service : peripheral.services())
    {
        if(service.uuid()!=serviceUUID) continue;

        for (SimpleBLE::Characteristic& characteristic : (service.characteristics())) 
        {
            if(characteristic.uuid()==motor1) Motor1Speed = &characteristic;
            if(characteristic.uuid()==motor2) Motor2Speed = &characteristic;
        }
    }

    if(!Motor1Speed || !Motor2Speed)
    {
        std::cout<<"Couldn't find motor characteristics!\n";
        return -2;
    }


    // test some stuff
    peripheral.write_request(serviceUUID, motor1, floatAsBytes(.5f));
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    peripheral.write_request(serviceUUID, motor1, floatAsBytes(0));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    peripheral.write_request(serviceUUID, motor2, floatAsBytes(-.2f));
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    peripheral.write_request(serviceUUID, motor2, floatAsBytes(0));


    peripheral.disconnect();
    std::cout<<"Disconnected!\n";
}
