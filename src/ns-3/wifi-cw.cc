/*
 # Wireless Networkng Assignment
 # Author Renukaprasad Manjappa
 */


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/random-variable-stream.h"
#include "ns3/gnuplot.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("Adaptive contention window simulations");

/* Callbacks to trace number of collisions */

uint32_t MacTxDropCount, PhyTxDropCount, PhyRxDropCount;

void MacTxDrop(Ptr<const Packet> p)
{
    NS_LOG_INFO("Packet Drop");
    MacTxDropCount++;
}

void PrintDrop()
{
    //std::cout << Simulator::Now().GetSeconds() << "\t" 
    //<< MacTxDropCount << "\t"<< PhyTxDropCount << "\t" 
    //<< PhyRxDropCount << "\n";
    Simulator::Schedule(Seconds(5.0), &PrintDrop);
}

void PhyTxDrop(Ptr<const Packet> p)
{
    NS_LOG_INFO("Packet Drop");
    PhyTxDropCount++;
}

void PhyRxDrop(Ptr<const Packet> p)
{
    NS_LOG_INFO("Packet Drop");
    PhyRxDropCount++;
}


int main (int argc, char *argv[])
{
    double StartTime = 0.0;
    double StopTime = 15.0;
    int nNodes = 20; /*number of node*/
    int minCw = 0;
    int maxCw = 0;
    int arf = 1;
    int aarf = 0;
    int constant = 0;
    uint32_t payloadSize = 1472 /*payload 1472*/;
    uint32_t maxPacket = 1024 /*10000*/ ;
    int Enable_Adaptive_Cw = 0; 


    StringValue DataRate;
    DataRate = StringValue("DsssRate11Mbps");
    // Create randomness based on time
    time_t timex;
    time(&timex);

    /* Introduce randomness in simulations */

    RngSeedManager::SetSeed(timex);
    RngSeedManager::SetRun(1);
    CommandLine cmd;
    

    cmd.AddValue ("nNodes", "number of nodes", nNodes);
    cmd.AddValue ("minCw", "Minimum Contention window size", minCw);
    cmd.AddValue ("maxCw", "Maximum contention window size", maxCw);
    cmd.AddValue ("arf", "arf rate control algorithm", arf);
    cmd.AddValue ("aarf", "aarf rate control algorithm", aarf);
    cmd.AddValue ("constant", "constant rate control ", constant);
    cmd.AddValue ("payloadSize", "offered load size", payloadSize);
    cmd.AddValue ("Enable_Adaptive_Cw", "Enable or disable Adaptive contention window",Enable_Adaptive_Cw);
    cmd.Parse (argc,argv);

    // Create access point
    NodeContainer APNode;
    APNode.Create (1);
   // std::cout << "Access point created.." << '\n';
    NS_LOG_INFO("Access point created..");
    // Create nodes
    NodeContainer StaNodes;
    StaNodes.Create (nNodes);
    NS_LOG_INFO("Nodes created..");
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
    phy.Set ("RxGain", DoubleValue (0) );
    YansWifiChannelHelper channel;
    channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    //channel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
    channel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel");
    phy.SetChannel (channel.Create ());
    WifiHelper wifi = WifiHelper::Default ();
    wifi.SetStandard (WIFI_PHY_STANDARD_80211b);

    
    NS_LOG_INFO("Wifi 802.11b PHY layer initilized");

    // configure MAC parameter

    if (constant) {
        wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", DataRate,
                "ControlMode", DataRate);
    }

    if (aarf) {
        wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
    }
    if (arf) {
        wifi.SetRemoteStationManager ("ns3::ArfWifiManager");
    }
    NqosWifiMacHelper mac = NqosWifiMacHelper::Default ();
    NS_LOG_INFO("Control rate configured..");




    // configure SSID
    Ssid ssid = Ssid ("Wifi_AP");

    mac.SetType ("ns3::StaWifiMac",
            "Ssid", SsidValue (ssid),
            "ActiveProbing", BooleanValue (false));
    NetDeviceContainer staDevices;
    staDevices = wifi.Install (phy, mac, StaNodes);
    mac.SetType ("ns3::ApWifiMac",
            "Ssid", SsidValue (ssid));
    NetDeviceContainer apDevice;
    apDevice = wifi.Install (phy, mac, APNode);
    NS_LOG_INFO("SSID, ApDevice & StaDevice configured..");

    if(Enable_Adaptive_Cw) {

        if (!minCw && !maxCw ) {
        minCw = 8 * nNodes - 6;
        maxCw = 1024;
        }
        Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/DcaTxop/CwMin",UintegerValue(minCw));
        Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/$ns3::RegularWifiMac/DcaTxop/CwMax",UintegerValue(maxCw));
    }
#if 0
    Config::Set ("/NodeList/*/DeviceList/*/Mac/DcaTxop/CwMin",UintegerValue (minCw));
    Config::Set ("/NodeList/*/DeviceList/*/Mac/DcaTxop/CwMax",UintegerValue (maxCw));
#endif



#if 0
    int minCw = 50;
    int maxCw = 1024;
    Ptr<DcaTxop> m_beaconDca = CreateObject<DcaTxop>();
    m_beaconDca->SetMinCw(minCw);
    m_beaconDca->SetMaxCw(maxCw);
    for (int i = 0 ; i < nNodes ; i++) {
        Ptr<Node> node = StaNodes.Get(i);
        Ptr<NetDevice> dev = node->GetDevice(0);
        Ptr<WifiNetDevice> wifi_dev = DynamicCast<WifiNetDevice>(dev);
        Ptr<WifiMac> mac = wifi_dev->GetMac();
        PointerValue ptr;
        mac->GetAttribute("DcaTxop", ptr);

        Ptr<DcaTxop> dca = ptr.Get<DcaTxop>();
        dca->SetMinCw(minCw);
        dca->SetMaxCw(maxCw);
    }

#endif
    // Configure nodes mobility
    MobilityHelper mobility;
    // Constant Position Node Mobility Model
    /* mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
       "MinX", DoubleValue (0.0),
       "MinY", DoubleValue (0.0),
       "DeltaX", DoubleValue (10.0),
       "DeltaY", DoubleValue (10.0),
       "GridWidth", UintegerValue (5),
       "LayoutType", StringValue ("RowFirst")); 
    */
    // Constant Position Node Mobility Model
     //mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel"); 
    // Random Direction 2D Node Mobility Model
    #if 0
     mobility.SetMobilityModel ("ns3::RandomDirection2dMobilityModel",
       "Bounds", RectangleValue (Rectangle (-10, 10, -10, 10)),
       "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=3]"),
       "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=0.4]")); 
#endif
    // Random Walk 2D Node Mobility Model
    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
            "Bounds", RectangleValue (Rectangle (-1000, 1000, -1000, 1000)),
            "Distance", ns3::DoubleValue (300.0));
    mobility.Install (StaNodes);

    // Constant Mobility for Access Point
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (APNode);
    NS_LOG_INFO("Node mobility configured..");
    // Internet stack
    InternetStackHelper stack;
    stack.Install (APNode);
    stack.Install (StaNodes);
    // Configure IPv4 address
    Ipv4AddressHelper address;
    Ipv4Address addr;
    address.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer staNodesInterface;
    Ipv4InterfaceContainer apNodeInterface;
    staNodesInterface = address.Assign (staDevices);
    apNodeInterface = address.Assign (apDevice);
    for(int i = 0 ; i < nNodes; i++)
    {
        addr = staNodesInterface.GetAddress(i);
        //std::cout << " Node " << i+1 << "\t "<< "IP Address "<<addr << std::endl;
    }
    addr = apNodeInterface.GetAddress(0);

    NS_LOG_INFO("Internet Stack & IPv4 address configured..");
    // Create traffic generator (UDP)
    ApplicationContainer serverApp;
    UdpServerHelper myServer (4001); //port 4001
    serverApp = myServer.Install (StaNodes.Get (0));
    serverApp.Start (Seconds(StartTime));
    serverApp.Stop (Seconds(StopTime));
    UdpClientHelper myClient (apNodeInterface.GetAddress (0), 4001); //port 4001
    myClient.SetAttribute ("MaxPackets", UintegerValue (maxPacket));
    myClient.SetAttribute ("Interval", TimeValue (Time ("0.002"))); //packets/s
    myClient.SetAttribute ("PacketSize", UintegerValue (payloadSize));

    //ApplicationContainer clientApp = myClient.Install (StaNodes.Get (0));
    ApplicationContainer clientApp = myClient.Install (StaNodes);

    clientApp.Start (Seconds(StartTime));
    clientApp.Stop (Seconds(StopTime+5));
    // Calculate Throughput & Delay using Flowmonitor
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();




    // Trace Collisions
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTxDrop", MakeCallback(&MacTxDrop));
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop", MakeCallback(&PhyRxDrop));
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDrop", MakeCallback(&PhyTxDrop));

    //Simulator::Schedule(Seconds(5.0), &PrintDrop);




    Simulator::Stop (Seconds (100.0));

    //Simulator::Stop (Seconds(StopTime+2));
    Simulator::Run ();
    monitor->CheckForLostPackets ();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();


    double TotalThroughput = 0.0;
    double psent = 0.0;
    double precv = 0.0;
    Time Delay = Seconds(0.0);
    int64_t Average_Delay = 0.0;
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
        //Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
        //std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
        //std::cout << " Tx Bytes: " << i->second.txBytes << "\n";
       // std::cout << " Rx Bytes: " << i->second.rxBytes << "\n";
        //std::cout << " Average Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - 
        //                                        i->second.timeFirstTxPacket.GetSeconds())/1024/nNodes << " kbps\n";
        double transmitTime = 0.0;  
        transmitTime = (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds());
        if (transmitTime != 0)
            //TotalThroughput += i->second.rxBytes * 8.0 /transmitTime/1024/nNodes;
            TotalThroughput += i->second.rxBytes * 8.0 /transmitTime/1024;
        if (i->second.rxPackets != 0)
            Delay += (i->second.delaySum / i->second.rxPackets); 
        psent +=  i->second.txPackets;
        precv += i->second.rxPackets; 
    }
    
    Average_Delay = Delay.GetMilliSeconds();
    //std::cout << "Average Throughput obtained :" << TotalThroughput/nNodes << "\n";
    //std::cout << "Loss percentage : " << (100.0 * (psent - precv ))/psent << "\n";
    std::cout <<nNodes << "\t";
    std::cout <<TotalThroughput / nNodes << "\t";
    std::cout << (100.0 * (psent - precv )) / psent <<"\t";
    std::cout <<minCw<< "\t";
    std::cout <<Average_Delay/nNodes<< "\n";
    //std::cout << PhyRxDropCount << "\n";
    Simulator::Destroy ();
    return 0;
}
