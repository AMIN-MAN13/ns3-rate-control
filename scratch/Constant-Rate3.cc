#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/traffic-control-module.h"

using namespace ns3;

int main() {
    // Create nodes
    NodeContainer wifiStaNode;
    wifiStaNode.Create(2);   // Create 2 station node objects
    NodeContainer wifiApNode;
    wifiApNode.Create(1);   // Create 1 access point node object

    // Set up wifi, channels, PHY, and SSID
    WifiHelper wifi;
    wifi.SetStandard(ns3::WIFI_STANDARD_80211ac);
   // Set up wifi channels and PHY
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");

    YansWifiPhyHelper wifiPhy;
    wifiPhy.SetChannel(wifiChannel.Create());

     wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                            "DataMode", StringValue("VhtMcs3"), 
                            "ControlMode", StringValue("VhtMcs3")); 
    WifiMacHelper wifiMac;
    Ssid ssid = Ssid("network-1");

 // Setup AP
    wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer apDevice = wifi.Install(wifiPhy, wifiMac, wifiApNode.Get(0));

    // Setup STAs
    wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer staDevices = wifi.Install(wifiPhy, wifiMac, wifiStaNode.Get(0));
    NetDeviceContainer staDevices2 = wifi.Install(wifiPhy, wifiMac, wifiStaNode.Get(1));

    NetDeviceContainer allDevices;
    allDevices.Add(apDevice);
    allDevices.Add(staDevices);
    allDevices.Add(staDevices2);

    // Set up internet stack
    InternetStackHelper internet;
    internet.Install(wifiApNode);
    internet.Install(wifiStaNode);

    // Set up mobility for AP
    MobilityHelper mobilityAP;
    mobilityAP.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityAP.Install(wifiApNode.Get(0));
    wifiApNode.Get(0)->GetObject<ConstantPositionMobilityModel>()->SetPosition(Vector(10.0, 0.0, 0.0));

    // Set up mobility for STA 1
    MobilityHelper mobilitySTA1;
    mobilitySTA1.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilitySTA1.Install(wifiStaNode.Get(0));
    wifiStaNode.Get(0)->GetObject<ConstantPositionMobilityModel>()->SetPosition(Vector(0.0, 0.0, 0.0));

    // Assign IP addresses
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer apInterface = ipv4.Assign(apDevice);
    Ipv4InterfaceContainer staInterface = ipv4.Assign(staDevices);
    Ipv4InterfaceContainer staInterface2 = ipv4.Assign(staDevices2);

    // Print IP addresses
    std::cout << "Access Point IP: " << apInterface.GetAddress(0) << std::endl;
    std::cout << "Station 1 IP: " << staInterface.GetAddress(0) << std::endl;
    std::cout << "Station 2 IP: " << staInterface2.GetAddress(0) << std::endl;

    // Create mobility model for the second station
    MobilityHelper mobilitySTA2;
    mobilitySTA2.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilitySTA2.Install(wifiStaNode.Get(1));
    Ptr<ConstantPositionMobilityModel> staMobilityModel2 = wifiStaNode.Get(1)->GetObject<ConstantPositionMobilityModel>();

    // Define distance range and step size
    double distanceStart = 20.0;
    double distanceEnd = 400.0;
    double distanceStep = 10.0;
    double simulationTime = 100.0;

    for (double distance = distanceStart; distance <= distanceEnd; distance += distanceStep) {
        // Set the position of the second station using MobilityHelper
        staMobilityModel2->SetPosition(Vector(distance, 0.0, 0.0));

        // Dynamically assign a new port for each iteration
        uint16_t port = 8080 + static_cast<uint16_t>(distance);

        // Create new PacketSink for each iteration
        PacketSinkHelper packetSinkHelper("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
        ApplicationContainer sinkApps = packetSinkHelper.Install(wifiStaNode.Get(1));
        sinkApps.Start(Seconds(0.0));

        // Create new application containers for OnOffHelper
        OnOffHelper onoff("ns3::UdpSocketFactory", Address(InetSocketAddress(staInterface2.GetAddress(0), port)));
        onoff.SetAttribute("PacketSize", UintegerValue(1470));
        onoff.SetAttribute("DataRate", DataRateValue(DataRate("150Mbps")));
        onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

        // Set the start time of the OnOffHelper consistently across iterations
        onoff.SetAttribute("StartTime", TimeValue(Seconds(1.0)));

        // Install OnOffHelper on the first station
        ApplicationContainer sourceApps = onoff.Install(wifiStaNode.Get(0));
        sourceApps.Start(Seconds(1.0));
        sourceApps.Stop(Seconds(simulationTime));

        // Run the simulation
        Simulator::Stop(Seconds(simulationTime));
        Simulator::Run();

        // Stop the PacketSink
        sinkApps.Stop(Seconds(simulationTime));

        Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps.Get(0));
        std::ofstream outputFile("output.csv", std::ios::app);
        if (sink) {
            double totalRxBytes = sink->GetTotalRx();
            double throughputMbps = totalRxBytes * 8.0 / simulationTime / 1000 / 1000;
            double expectedThroughputMbps = 150.0; // Assuming the configured data rate in Mbps

            // Calculate packet loss
            double packetLoss = 1.0 - (throughputMbps / expectedThroughputMbps);

            // Print debugging information
            std::cout << "Distance: " << distance << " meters, Port: " << port << std::endl;

            // Print the throughput, packet loss, and distance for the second station
            std::cout << "Station Node 2 Position: " << staMobilityModel2->GetPosition()
                      << ", Throughput STA 2: " << throughputMbps << " Mbps\n"
                      << ", Packet Loss STA 2: " << (packetLoss * 100) << "%\n";
   // Save the throughput data to the throughput CSV file
    std::ofstream throughputFile("throughput_vs_distance_sta2_Constant-655" + std::to_string(distance) + ".csv", std::ios::app);

   throughputFile << distance << "," << throughputMbps << "\n";
   throughputFile.close();

    // Save the packet loss data to the packet loss CSV file
  
   std::ofstream packetLossFile("packet_loss_vs_distance_sta2_Constant-655" + std::to_string(distance) + ".csv", std::ios::app);

   packetLossFile << distance << "," << (packetLoss * 100) << "\n";
   packetLossFile.close();

        } else {
            std::cerr << "Error: Unable to retrieve PacketSink object for the second station (DynamicCast failed).\n";
        }
        outputFile.close();  // Close the CSV file

        // Clear the applications for the next iteration
        sinkApps = ApplicationContainer();
        sourceApps.Stop(Seconds(simulationTime));
    }

    Simulator::Destroy(); 

    return 0;
}