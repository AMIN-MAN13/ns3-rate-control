#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"


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
    wifi.SetStandard(ns3::WIFI_STANDARD_80211ac); // Use 802.11ac for Minstrel-HT

    // Set up wifi channels and PHY
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");

    YansWifiPhyHelper wifiPhy;
    wifiPhy.SetChannel(wifiChannel.Create());

    wifi.SetRemoteStationManager("ns3::MinstrelHtWifiManager");

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
    positionAlloc->Add(Vector(380.0, 0.0, 0.0));

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
    internet.Install(wifiApNode.Get(0)); 
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


// Set up a UDP server (PacketSink) on the second station
PacketSinkHelper packetSinkHelperStation2("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), 8080));
ApplicationContainer serverAppsStation2 = packetSinkHelperStation2.Install(wifiStaNode.Get(1));
serverAppsStation2.Start(Seconds(0.0));
serverAppsStation2.Stop(Seconds(10.0));




    // Set up a UDP client (OnOff application) on the first station to send to the second station
    OnOffHelper onoffSta("ns3::UdpSocketFactory", Address(InetSocketAddress(staInterface2.GetAddress(0), 8080)));
    // Adjust the size of packets
    onoffSta.SetAttribute("PacketSize", UintegerValue(1470)); 

    // Adjust the interval between packets
    onoffSta.SetAttribute("DataRate", DataRateValue(DataRate("5Mbps"))); 
    onoffSta.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]")); 
    onoffSta.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]")); 
    ApplicationContainer clientAppsSta = onoffSta.Install(wifiStaNode.Get(0));
    clientAppsSta.Start(Seconds(1.0));
    clientAppsSta.Stop(Seconds(10.0));



// Run the simulation
Simulator::Stop(Seconds(10.0)); 
Simulator::Run();
Simulator::Destroy();


Ptr<PacketSink> sinkStation2 = DynamicCast<PacketSink>(serverAppsStation2.Get(0));
if (sinkStation2) {
    std::cout << "Received packets at the second station: " << sinkStation2->GetTotalRx() << "\n";
    std::cout << "Throughput for the second station: " << sinkStation2->GetTotalRx() * 8.0 / 10.0 / 1000 / 1000 << " Mbps\n";
} else {
    std::cerr << "Error: Unable to retrieve PacketSink object for the second station (DynamicCast failed).\n";
}


    return 0;
}
