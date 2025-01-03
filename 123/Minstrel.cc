#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main() {
    // Create nodes
   NodeContainer wifiStaNode;
   wifiStaNode.Create(2);   // Create 2 station node objects
   NodeContainer wifiApNode;
   wifiApNode.Create(1);   // Create 1 access point node object

    // Set up wifi
    WifiHelper wifi;
    wifi.SetStandard(ns3::WIFI_STANDARD_80211b);

    // Set up wifi channels and PHY
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");

    YansWifiPhyHelper wifiPhy;  //creates a helper for setting physical properties.
    wifiPhy.SetChannel(wifiChannel.Create());

    wifi.SetRemoteStationManager("ns3::MinstrelWifiManager");

   // Set up wifi SSID
    WifiMacHelper wifiMac;
    Ssid ssid = Ssid("network-1");

    // Setup AP
    wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer apDevice = wifi.Install(wifiPhy, wifiMac, wifiApNode.Get(0));

    // Setup STAs
    wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer staDevices = wifi.Install(wifiPhy, wifiMac, wifiStaNode.Get(0));
    NetDeviceContainer staDevices2 = wifi.Install(wifiPhy, wifiMac, wifiStaNode.Get(1));
    // Create a container for all devices
    NetDeviceContainer allDevices;
    allDevices.Add(apDevice);
    allDevices.Add(staDevices);
    allDevices.Add(staDevices2);
       // Set up mobility
   MobilityHelper mobility;
   Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

    // Add positions for three nodes in a row
    positionAlloc->Add(Vector(10.0, 0.0, 0.0));
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(20.0, 0.0, 0.0));

    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode);
    mobility.Install(wifiStaNode);
    // Access point position
    Vector apPosition = wifiApNode.Get(0)->GetObject<ConstantPositionMobilityModel>()->GetPosition();
    std::cout << "Access Point Position: " << apPosition << std::endl;

    // Station Node 0 position
   Vector staPosition0 = wifiStaNode.Get(0)->GetObject<ConstantPositionMobilityModel>()->GetPosition();
   std::cout << "Station Node 0 Position: " << staPosition0 << std::endl;

   // Station Node 1 position
   Vector staPosition1 = wifiStaNode.Get(1)->GetObject<ConstantPositionMobilityModel>()->GetPosition();
    std::cout << "Station Node 1 Position: " << staPosition1 << std::endl;

    // Set up internet stack 
    InternetStackHelper internet;
    internet.Install(wifiApNode.Get(0)); // Assuming the access point is the second node 
    internet.Install(wifiStaNode.Get(0));
    internet.Install(wifiStaNode.Get(1));



    // Assign IP addresses
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("192.168.1.0", "255.255.255.0");
     Ipv4InterfaceContainer apInterface = ipv4.Assign(apDevice);
    // Assign IP addresses to the stations
    Ipv4InterfaceContainer staInterface = ipv4.Assign(staDevices);
    Ipv4InterfaceContainer staInterface2 = ipv4.Assign(staDevices2);

    // Print IP addresses
    std::cout << "Access Point IP: " << apInterface.GetAddress(0) << std::endl;
    std::cout << "Station 1 IP: " << staInterface.GetAddress(0) << std::endl;
    std::cout << "Station 2 IP: " << staInterface2.GetAddress(0) << std::endl;


    // Install UDP server on the access point
    UdpServerHelper udpServer(12345);
    ApplicationContainer udpServerApp = udpServer.Install(wifiApNode.Get(0));
    udpServerApp.Start(Seconds(1.0));
    udpServerApp.Stop(Seconds(100.0));
    std::cout << "UDP server installed" << std::endl;


    // Install OnOff application on Station 1 (to send to access point)
    UdpClientHelper udpClientAP(staInterface.GetAddress(0), 12345);
    udpClientAP.SetAttribute("MaxPackets", UintegerValue(2200));
    udpClientAP.SetAttribute("Interval", TimeValue(Seconds(0.0001)));
    udpClientAP.SetAttribute("PacketSize", UintegerValue(65506));

    ApplicationContainer udpClientAppAP = udpClientAP.Install(wifiStaNode.Get(0));
    udpClientAppAP.Start(Seconds(1.0));
    udpClientAppAP.Stop(Seconds(100.0));
    std::cout << "UDP client 1 installed" << std::endl;

    // Install OnOff application on Station 2 (to send to access point)
    UdpClientHelper udpClientAP2(staInterface2.GetAddress(0), 12345);
    udpClientAP2.SetAttribute("MaxPackets", UintegerValue(2200));
    udpClientAP2.SetAttribute("Interval", TimeValue(Seconds(0.0001)));
    udpClientAP2.SetAttribute("PacketSize", UintegerValue(65506));

    ApplicationContainer udpClientAppAP2 = udpClientAP2.Install(wifiStaNode.Get(1));
    udpClientAppAP2.Start(Seconds(1.0));
    udpClientAppAP2.Stop(Seconds(100.0));
    std::cout << "UDP client 2 installed" << std::endl;

        // Install FlowMonitor
    FlowMonitorHelper flowMonitor;
    Ptr<FlowMonitor> monitor = flowMonitor.InstallAll();


    // Run the simulation
    Simulator::Stop(Seconds(100.0));  // Adjusted the stop time
    Simulator::Run();
    Simulator::Destroy();
    std::cout << "Simulation finished" << std::endl;

    // Print throughput
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowMonitor.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    
    double totalThroughput = 0.0;
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        totalThroughput += it->second.rxBytes * 8.0 / (100.0 * 1e6);
    }

    // Print total throughput
    std::cout << "Total Throughput: " << totalThroughput << " Mbps\n";

    return 0;
}