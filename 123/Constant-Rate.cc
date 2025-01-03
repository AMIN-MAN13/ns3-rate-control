#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;



int main() 
{
    // Create nodes
   NodeContainer wifiStaNode;
   wifiStaNode.Create(2);   // Create 2 station node objects
   NodeContainer wifiApNode;
   wifiApNode.Create(1);   // Create 1 access point node object


    // Set up wifi
    WifiHelper wifi;
    wifi.SetStandard(ns3::WIFI_STANDARD_80211n); //wifi standard using for Constant-rate algorithm


    // Set up wifi channels and PHY
    YansWifiChannelHelper wifiChannel; //helping for wifi channel
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel"); // constant speed
    wifiChannel.AddPropagationLoss("ns3::LogDistancePropagationLossModel");  //propagation loss model to the wireless channel
    

    YansWifiPhyHelper wifiPhy;// creating  physical layer
    wifiPhy.SetChannel(wifiChannel.Create());// due to wifi channel

    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                            "DataMode", StringValue("OfdmRate24Mbps"),// using Constatnt-rate model
                            "ControlMode", StringValue("OfdmRate24Mbps"));


   // Set up wifi SSID
    WifiMacHelper wifiMac;
    Ssid ssid = Ssid("network-1");// for dentify  by other devices(difference in MAC layer with other network!)

    // Setup AP
    wifiMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));//set a wifiApNode (0)for access point
    NetDeviceContainer apDevice = wifi.Install(wifiPhy, wifiMac, wifiApNode.Get(0));//and stations can connect it with specified layer,Mac and Node 

    // Setup STAs
    wifiMac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));//type of Mac layer
    NetDeviceContainer staDevices = wifi.Install(wifiPhy, wifiMac, wifiStaNode.Get(0));//speciefied phylayer,Mac and first station (WifiStaNode(0))
    NetDeviceContainer staDevices2 = wifi.Install(wifiPhy, wifiMac, wifiStaNode.Get(1));//speciefied phylayer,Mac and second station (WifiStaNode(1))
    // Create a container for all devices
    NetDeviceContainer allDevices;//network devices
    allDevices.Add(apDevice);//access point
    allDevices.Add(staDevices);//station 1
    allDevices.Add(staDevices2);//station 2
       // Set up mobility
   MobilityHelper mobility;//position of each node
   Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

    // Add positions for three nodes in a row
    positionAlloc->Add(Vector(10.0, 0.0, 0.0)); //position of access point
    positionAlloc->Add(Vector(0.0, 0.0, 0.0)); //position of station 1
    positionAlloc->Add(Vector(20.0, 0.0, 0.0)); //position of station 2

    mobility.SetPositionAllocator(positionAlloc); //sets the position allocator for mobility
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel"); //Constant positions
    mobility.Install(wifiApNode); //install position of access pont
    mobility.Install(wifiStaNode); // install positions of stations
    // Access point position(print the position of access point)
    Vector apPosition = wifiApNode.Get(0)->GetObject<ConstantPositionMobilityModel>()->GetPosition();
    std::cout << "Access Point Position: " << apPosition << std::endl; // print

    // Station Node 0 position(print the position of station 1)
   Vector staPosition0 = wifiStaNode.Get(0)->GetObject<ConstantPositionMobilityModel>()->GetPosition();
   std::cout << "Station Node 0 Position: " << staPosition0 << std::endl; //print
  
   // Station Node 1 position(print the position of station 2)
   Vector staPosition1 = wifiStaNode.Get(1)->GetObject<ConstantPositionMobilityModel>()->GetPosition();
    std::cout << "Station Node 1 Position: " << staPosition1 << std::endl;

    // Set up internet stack 
    InternetStackHelper internet;
    internet.Install(wifiApNode.Get(0)); 
    internet.Install(wifiStaNode.Get(0));
    internet.Install(wifiStaNode.Get(1));



    // Assign IP addresses
    Ipv4AddressHelper ipv4; // using ipv4
    ipv4.SetBase("192.168.1.0", "255.255.255.0"); // ip address and subnet mask
    Ipv4InterfaceContainer apInterface = ipv4.Assign(apDevice); //assign ip of access point
    // Assign IP addresses to the stations
    Ipv4InterfaceContainer staInterface = ipv4.Assign(staDevices);  //assign ip of stations
    Ipv4InterfaceContainer staInterface2 = ipv4.Assign(staDevices2);

    // Print IP addresses
    std::cout << "Access Point IP: " << apInterface.GetAddress(0) << std::endl;
    std::cout << "Station 1 IP: " << staInterface.GetAddress(0) << std::endl;
    std::cout << "Station 2 IP: " << staInterface2.GetAddress(0) << std::endl;


    // Install UDP server on the access point
    UdpServerHelper udpServer(12345);  // udpserver on Port 12345
    ApplicationContainer udpServerApp = udpServer.Install(wifiApNode.Get(0));   //install on access point 
    udpServerApp.Start(Seconds(1.0));  // start time(1 s)
    udpServerApp.Stop(Seconds(100.0));  // stop time(100 s)
    std::cout << "UDP server installed" << std::endl;


    // Install OnOff application on Station 1 *(for sending udp packet to the access point)
    UdpClientHelper udpClientAP(staInterface.GetAddress(0), 12345);   //send packets to node 0 (access point)and using port 12345
    udpClientAP.SetAttribute("MaxPackets", UintegerValue(13000));   //max number of packets send to the access point
    udpClientAP.SetAttribute("Interval", TimeValue(Seconds(0.0001)));  // interval of each packet ...
    udpClientAP.SetAttribute("PacketSize", UintegerValue(65507));  //size of each packet(10084 bytes)

    ApplicationContainer udpClientAppAP = udpClientAP.Install(wifiStaNode.Get(0)); //install udp client on wifistanode 0
    udpClientAppAP.Start(Seconds(1.0));
    udpClientAppAP.Stop(Seconds(100.0));
    std::cout << "UDP client 1 installed" << std::endl;  //print 

    // Install OnOff application on Station 2 (to send to access point)
    UdpClientHelper udpClientAP2(staInterface2.GetAddress(0), 12345);
    udpClientAP2.SetAttribute("MaxPackets", UintegerValue(13000));
    udpClientAP2.SetAttribute("Interval", TimeValue(Seconds(0.0001)));
    udpClientAP2.SetAttribute("PacketSize", UintegerValue(65507));

    ApplicationContainer udpClientAppAP2 = udpClientAP2.Install(wifiStaNode.Get(1));
    udpClientAppAP2.Start(Seconds(1.0));
    udpClientAppAP2.Stop(Seconds(100.0));
    std::cout << "UDP client 2 installed" << std::endl;

    // Install FlowMonitor(before simulation and after assign nodes...)
    FlowMonitorHelper flowMonitor;
    Ptr<FlowMonitor> monitor = flowMonitor.InstallAll();



    // Run the simulation
    Simulator::Stop(Seconds(100.0));  
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