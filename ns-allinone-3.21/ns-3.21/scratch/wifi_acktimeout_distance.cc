/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * This is a simple example
 *
 * Network topology:
 *
 *  Wifi 10.1.1.0
 *
 *      n1   10.1.1.1
 *      |
 *      |
 *      n2    10.1.1.2  **/

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/gnuplot.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LinkTest2");


class Experiment
{
	public:
		Experiment ();
		void Run (uint32_t dataRate, uint32_t distance, uint32_t packetSize, std::string phyMode,
				uint32_t rtsThreshold, std::string CSVfileName, float acktimeout, uint32_t warmupSeconds, uint32_t runApplicationSeconds, int32_t rssForFixedRssLossModel,
				bool wifiTrace, bool pcapTrace, bool flowMonitor);
		void prepareOutput (std::string);

	private:

		Vector GetPosition (Ptr<Node> node);
		Ptr<Socket> SetupPacketReceive1 (Ipv4Address addr, Ptr<Node> node);
		Ptr<Socket> SetupPacketReceive2 (Ipv4Address addr, Ptr<Node> node);
		NodeContainer GenerateNeighbors(NodeContainer c, uint32_t senderId);

		void ApplicationSetup (Ptr<Node> client, Ptr<Node> server, double start, double stop);
		void AssignNeighbors (NodeContainer c);
		void SelectSrcDest (NodeContainer c);
		void ReceivePacket1 (Ptr<Socket> socket);
		void ReceivePacket2 (Ptr<Socket> socket);
		//void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize, uint32_t pktCount, Time pktInterval );
		void PhyRxOkTrace (std::string context, Ptr<const Packet> packet, double snr, WifiMode mode, enum WifiPreamble preamble);
		void PhyRxErrorTrace (std::string context, Ptr<const Packet> packet, double snr);
		void IpTx (std::string context, Ptr<const Packet> packet, uint32_t interfaceIndex);
		void IpDrop (std::string path,  Ipv4Header const &header, Ptr<const Packet> packet,
		                                         Ipv4L3Protocol::DropReason reason, uint32_t interface);
		void RxDrop (std::string context,Ptr<const Packet> p);
		void TxDrop (std::string context,Ptr<const Packet> p);
		void TxFinalDataFailedTrace (std::string context, Mac48Address address);
		void CheckThroughput ();
		void SendMultiDestinations (Ptr<Node> sender, NodeContainer c);
		void AdvancePosition (Ptr<Node> node);

		void PhyTx1 (std::string context , ns3::Ptr<ns3::Packet const> p, ns3::WifiMode a, ns3::WifiPreamble ab, unsigned char c);
		void PhyRx1 (std::string context , ns3::Ptr<ns3::Packet const> p,double b, ns3::WifiMode a, ns3::WifiPreamble ab);

		void PhyTx2 (std::string context , ns3::Ptr<ns3::Packet const> p, ns3::WifiMode a, ns3::WifiPreamble ab, unsigned char c);
		void PhyRx2 (std::string context , ns3::Ptr<ns3::Packet const> p,double b, ns3::WifiMode a, ns3::WifiPreamble ab);


		Gnuplot2dDataset m_output;

		uint32_t port1;
		uint32_t port2;
		uint32_t bytesTotal1;
		uint32_t bytesTotal2;
		uint32_t packetsReceived1;
		uint32_t packetsReceived2;

		uint32_t p_tx_1;
		uint32_t p_rx_1;
		uint32_t p_tx_2;
		uint32_t p_rx_2;

		uint32_t m_dataRate;
		uint32_t m_distance;
		uint32_t m_packetSize;
		std::string m_phyMode;
		uint32_t m_rtsThreshold;

		std::string m_CSVfileName;
		uint32_t m_warmupSeconds;
		uint32_t m_runApplicationSeconds;

		int32_t m_rssi;

		float m_acktimeout;

};



Experiment::Experiment () :
		port1(4002),
		port2(4000),
		bytesTotal1(0),
		bytesTotal2(0),
		packetsReceived1(0),
		packetsReceived2(0),
		p_tx_1(0),
		p_rx_1(0),
		p_tx_2(0),
		p_rx_2(0),
		m_CSVfileName("output.csv")
{	}


void
Experiment::ReceivePacket1 (Ptr<Socket> socket)
{

  Ptr<Packet> packet;
    while (packet = socket->Recv ())
    {
      bytesTotal1 += packet->GetSize();
      packetsReceived1 += 1;
    }
 }


void
Experiment::ReceivePacket2 (Ptr<Socket> socket)
{

  Ptr<Packet> packet;
    while (packet = socket->Recv ())
    {
      bytesTotal2 += packet->GetSize();
      packetsReceived2 += 1;
    }
 }

  void
 Experiment::CheckThroughput()
  {
    long kbs1 = (bytesTotal1 * 8.0);
    long kbs2 = (bytesTotal2 * 8.0);
    bytesTotal1 = 0;
    bytesTotal2 = 0;


    m_output.Add ((Simulator::Now ()).GetSeconds (), kbs1);

    std::ofstream out(m_CSVfileName.c_str(), std::ios::app);

    out << (Simulator::Now ()).GetSeconds () << ","
    		<< kbs1 << ","
    		<< kbs2 << ","
    		<< kbs1+kbs2 << ","
    		<< p_tx_1    << ","
    		<< p_tx_2    << ","
    		<< p_rx_1    << ","
    		<< p_rx_2    << ","
    		<< packetsReceived1 << ","
    		<< packetsReceived2 << ","
    		<< m_dataRate << ","
    		<< m_distance << ","
    		<< m_packetSize << ","
    		<< m_phyMode << ","
    		<< m_rtsThreshold << ","
			<< (m_acktimeout*1000) << ","
			<< m_rssi
			<< std::endl;

    out.close();
    packetsReceived1 = 0;
    packetsReceived2 = 0;
    p_tx_1=0;
    p_rx_1=0;
    p_tx_2=0;
    p_rx_2=0;
    Simulator::Schedule (Seconds (1.0), &Experiment::CheckThroughput, this);
  }


  void
  Experiment::PhyTx1 (std::string context , ns3::Ptr<ns3::Packet const> p, ns3::WifiMode a, ns3::WifiPreamble ab, unsigned char c)
  {

	WifiMacHeader hdr1;
    if(p->GetSize() > 14){ //solo considero paquetes de datos, ignoro beacons
  	  p_tx_1 += 1;
  	p->PeekHeader(hdr1);

    }
  }

  void
    Experiment::PhyRx1 (std::string context , ns3::Ptr<ns3::Packet const> p,double b, ns3::WifiMode a, ns3::WifiPreamble ab)
    {
	  WifiMacHeader hdr1;
      if(p->GetSize() > 14){ //solo considero paquetes de datos, ignoro beacons
    	  p_rx_1 += 1;
    	  p->PeekHeader(hdr1);

      }
    }


  void
    Experiment::PhyTx2 (std::string context , ns3::Ptr<ns3::Packet const> p, ns3::WifiMode a, ns3::WifiPreamble ab, unsigned char c)
    {

  	WifiMacHeader hdr1;
      if(p->GetSize() > 14){ //solo considero paquetes de datos, ignoro beacons
    	  p_tx_2 += 1;
    	p->PeekHeader(hdr1);

      }
    }

    void
      Experiment::PhyRx2 (std::string context , ns3::Ptr<ns3::Packet const> p,double b, ns3::WifiMode a, ns3::WifiPreamble ab)
      {
  	  WifiMacHeader hdr1;
        if(p->GetSize() > 14){ //solo considero paquetes de datos, ignoro beacons
      	  p_rx_2 += 1;
      	  p->PeekHeader(hdr1);

        }
      }

Ptr<Socket>
Experiment::SetupPacketReceive1 (Ipv4Address addr, Ptr<Node> node)
{

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> sink = Socket::CreateSocket (node, tid);
  InetSocketAddress local = InetSocketAddress (addr, port1);
  sink->Bind (local);
  sink->SetRecvCallback (MakeCallback (&Experiment::ReceivePacket1, this));
  return sink;
}


Ptr<Socket>
Experiment::SetupPacketReceive2 (Ipv4Address addr, Ptr<Node> node)
{

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> sink = Socket::CreateSocket (node, tid);
  InetSocketAddress local = InetSocketAddress (addr, port2);
  sink->Bind (local);
  sink->SetRecvCallback (MakeCallback (&Experiment::ReceivePacket2, this));
  return sink;
}



void
PopulateArpCache ()
{
  Ptr<ArpCache> arp = CreateObject<ArpCache> ();
  arp->SetAliveTimeout (Seconds(3600 * 24 * 365));
  for (NodeList::Iterator i = NodeList::Begin(); i != NodeList::End(); ++i)
  {
    Ptr<Ipv4L3Protocol> ip = (*i)->GetObject<Ipv4L3Protocol> ();
    NS_ASSERT(ip !=0);
    ObjectVectorValue interfaces;
    ip->GetAttribute("InterfaceList", interfaces);
    for(ObjectVectorValue::Iterator j = interfaces.Begin(); j !=
interfaces.End (); j ++)
    {
    	Ptr<Ipv4Interface> ipIface = (j->second)->GetObject<Ipv4Interface> ();
      NS_ASSERT(ipIface != 0);
      Ptr<NetDevice> device = ipIface->GetDevice();
      NS_ASSERT(device != 0);
      Mac48Address addr = Mac48Address::ConvertFrom(device->GetAddress ());
      for(uint32_t k = 0; k < ipIface->GetNAddresses (); k ++)
      {
        Ipv4Address ipAddr = ipIface->GetAddress (k).GetLocal();
        if(ipAddr == Ipv4Address::GetLoopback())
          continue;
        ArpCache::Entry * entry = arp->Add(ipAddr);
        entry->MarkWaitReply(0);
        entry->MarkAlive(addr);
        NS_LOG_UNCOND ("Arp Cache: Adding the pair (" << addr << "," << ipAddr << ")");
      }
    }
  }
  for (NodeList::Iterator i = NodeList::Begin(); i != NodeList::End(); ++i)
  {
    Ptr<Ipv4L3Protocol> ip = (*i)->GetObject<Ipv4L3Protocol> ();
    NS_ASSERT(ip !=0);
    ObjectVectorValue interfaces;
    ip->GetAttribute("InterfaceList", interfaces);
    for(ObjectVectorValue::Iterator j = interfaces.Begin(); j !=interfaces.End (); j ++)
    {
    	Ptr<Ipv4Interface> ipIface = (j->second)->GetObject<Ipv4Interface> ();
      ipIface->SetAttribute("ArpCache", PointerValue(arp));
    }
  }
}



uint32_t p_tx_1=0;
uint32_t p_rx_1=0;
uint32_t p_tx_2=0;
uint32_t p_rx_2=0;
uint32_t dropped=0;

void
MacDrop (std::string context , ns3::Ptr<ns3::Packet const> p)
{

	SnrTag tag;
	WifiMacHeader header;

	//p->PeekPacketTag(tag);
	p->PeekHeader(header);


	if(!header.IsAck()){
	  //NS_LOG_UNCOND (context << " DROPMAC \n " << header. );
	  std::cout << context << " - DROP ACK " << header.GetAddr2() << std::endl;
	}
	//NS_LOG_UNCOND ("Received Packet with SRN = " << tag.Get());
}


void
PhyFail (std::string context , ns3::Mac48Address p)
{
  //Vector position = model->GetPosition ();
  NS_LOG_UNCOND (context << " Tiro " );
  NS_LOG_UNCOND ("TxFailed - " << dropped);
}

void
MacCw (uint32_t oldval, uint32_t newval)
{
	std::cout << Simulator::Now ().GetSeconds () << ": old " << oldval
	<< " new: " << newval << std::endl;

	NS_LOG_UNCOND ("TxFailed");
}

void
PhyFail2 (std::string context , ns3::Mac48Address p)
{
  //Vector position = model->GetPosition ();
  NS_LOG_UNCOND (context << " Máximo de Intentos de retransmisión superado" );

}

void
PhyRxError (std::string context , ns3::Ptr<ns3::Packet const> p, double d)
{
  //Vector position = model->GetPosition ();
  NS_LOG_UNCOND (context << " Recibo Error " << p->GetSize() );
}






int main (int argc, char *argv[])
  {

  	Experiment experiment;
  	std::string CSVfileName;


  	 uint32_t distance_init = 50;
  	 uint32_t packetSize = 1024;
  	 uint32_t rtsThreshold = 5000;
  	 float acktimeout = 0.0005;
  	 uint32_t warmupSeconds = 5;
  	 uint32_t runApplicationSeconds = 35;
  	 int32_t rssi = 0;
  	 bool wifiTrace = true;
  	 bool pcapTrace = true;
  	 bool flowMonitor = true;


  	std::string phyModeL[12] = {"DsssRate1Mbps", "DsssRate2Mbps","DsssRate5_5Mbps","ErpOfdmRate6Mbps",
  		   "ErpOfdmRate9Mbps",
  		   "DsssRate11Mbps",
  		   "ErpOfdmRate12Mbps",
  		   "ErpOfdmRate18Mbps",
  		   "ErpOfdmRate24Mbps",
  		   "ErpOfdmRate36Mbps",
  		   "ErpOfdmRate48Mbps",
  		   "ErpOfdmRate54Mbps"};
  	uint32_t dataRateMax[12] = {1100, 2200, 5500,6600,9900,12100,13200,
  			19800,26400,39600,52800,59400};



     uint32_t pos=0;
  	 uint32_t j;
  	 std::string ref1;
  	 std::string ref2 ;
  	 for (j=0;j<12;j++){
  		CSVfileName = "output_distance_phy_" + phyModeL[j] + ".ods";
  		//blank out the last output file and write the column headers
  		  	std::ofstream out(CSVfileName.c_str());
  		  	out << "SimulationSecond," <<
  		  				"ReceiveRate STA->AP," <<
  		  				"ReceiveRate AP->STA," <<
  		  				"ReceiveRate Duplex," <<
  		  				"PHY PacketsTx/s STA->AP," <<
  		  				"PHY PacketsTx/s AP->STA," <<
  		  				"PHY PacketsRx/s STA->AP," <<
  		  				"PHY PacketsRx/s AP->STA," <<
  		  				"UDP PacketsReceived/s STA->AP," <<
  		  				"UDP PacketsReceived/s AP->STA," <<
  		  				"SendRate," <<
  		  	    		"Distance," <<
  		  	    		"PacketSize," <<
  		  	    		"PhyMode," <<
  		  	    		"RTS/CTS," <<
  		  	    		"AckTimeout (ms)," <<
  		  	    		"RssForFixedRssLossModel"
  		  				<< std::endl;
  		  	 out.close();


  		pos=0;

  	       uint32_t distance=distance_init;
  	       while(distance < 70005)
  	       {

  		     NS_LOG_UNCOND ("\n Rate -> " << dataRateMax[j]);
  		     NS_LOG_UNCOND ("\n Distance -> " << distance);
  		     experiment = Experiment();
  		     experiment.Run(dataRateMax[j], distance, packetSize, phyModeL[j],
  				    rtsThreshold, CSVfileName, acktimeout,warmupSeconds,
  				    runApplicationSeconds,rssi,
  				    wifiTrace, pcapTrace, flowMonitor);



  	 std::ofstream out(CSVfileName.c_str(), std::ios::app);
  	 out << "Pkts tx PHY flujo1," <<
  			"=SUM(E" << pos+12 << ":E" << pos+39<< ")," <<
  	        "Pkts rx PHY flujo1:," <<
  	      "=SUM(G" << pos+12 << ":G" << pos+39<< "),"
  	 << std::endl;
  	 out.close();

  	   std::ofstream out2(CSVfileName.c_str(), std::ios::app);
  	     	 out2 << "Pkts tx PHY flujo2:," <<
  	     			"=SUM(F" << pos+12 << ":F" << pos+39<< ")," <<
  	     	   				"Pkts rx PHY flujo2:," <<
  	     	   			"=SUM(H" << pos+12 << ":H" << pos+39<< "),"
  	     	   				<< std::endl;
  	     out2.close();


  	   std::ofstream out3(CSVfileName.c_str(), std::ios::app);
  	    out3 << "Pkts dropped PHY flujo1:," <<
  	    		"=B" << pos+52 << "-" << "D" << pos+52 << "," <<
  	     	    	  "Pkts dropped PHY flujo2:," <<
  	     	    	    "=B" << pos+53 << "-" << "D" << pos+53 << ","
  	     	    	  << std::endl;
  	    out3.close();
  	  std::ofstream out4(CSVfileName.c_str(), std::ios::app);
  	    	    out4 << " AV. DUPLEX," <<
  	    	    		" ," << " ," <<
  	    	    		" ," << " ," <<
  	    	    		" ," << " ," <<
  	    	    		" ," << " ," <<
  	    	    		" ," << " ," <<
  	    	    		" ," << " ," <<
  	    	    		" ," << " ," <<
  	    	    		" ," << " ," <<
  	    	    		"=AVERAGE(D" << pos+12 << ":D" << pos+38<< "),"
  	    	    	<<	std::endl;
  	    	    out4.close();
     pos=pos+54;
     distance=distance+500;
  	 }

  	}

  	NS_LOG_UNCOND ("Done... ");
  	 return 0;
  }


 void
 Experiment::RxDrop (std::string context, Ptr<const Packet> packet)
  {
  	  NS_LOG_UNCOND (Simulator::Now().GetSeconds () <<  " " << context << " packet=" << packet->GetUid());
  }

  void
  Experiment::TxDrop (std::string context, Ptr<const Packet> packet)
  {
  	  NS_LOG_UNCOND (Simulator::Now().GetSeconds () <<  " " << context << " packet=" << packet->GetUid() << " " << *packet);
  }

  void
 Experiment::IpDrop (std::string context, Ipv4Header const &header, Ptr<const Packet> packet, Ipv4L3Protocol::DropReason reason, uint32_t interface)
  {
  	NS_LOG_UNCOND (Simulator::Now().GetSeconds () <<  " " << context << " packet=" << packet->GetUid());
  }

  void
  Experiment::IpTx (std::string context, Ptr<const Packet> packet, uint32_t interfaceIndex)
  {
  	  NS_LOG_UNCOND (Simulator::Now().GetSeconds () <<  " " << context << " packet=" << packet->GetUid());
  }

  void
 Experiment::PhyRxOkTrace (std::string context, Ptr<const Packet> packet, double snr, WifiMode mode, enum WifiPreamble preamble)
  {
    NS_LOG_UNCOND (Simulator::Now().GetSeconds () <<  " " << context << " packet=" << packet->GetUid() << " mode=" << mode << " snr=" << snr );

  }

  void
  Experiment::PhyRxErrorTrace (std::string context, Ptr<const Packet> packet, double snr)
  {
  	NS_LOG_UNCOND (Simulator::Now().GetSeconds () <<  " " << context << " " << "PHYRXERROR snr=" << snr << " packet=" << packet->GetUid() << " " << *packet);

  }


  void
  Experiment::TxFinalDataFailedTrace (std::string context, Mac48Address address)
  {
    NS_LOG_UNCOND (Simulator::Now().GetSeconds () <<  " "  << context << " " << address);
  }


 void
 Experiment::Run (uint32_t dataRate, uint32_t distance, uint32_t packetSize, std::string phyMode,
		  uint32_t rtsThreshold, std::string CSVfileName, float acktimeout, uint32_t warmupSeconds, uint32_t runApplicationSeconds,
			int32_t rssi, bool wifiTrace, bool pcapTrace, bool flowMonitor)
  {

	    m_dataRate = dataRate;
	 	m_distance = distance;
	 	m_packetSize = packetSize;
	 	m_phyMode = phyMode;
	 	m_rtsThreshold = rtsThreshold;
	 	m_CSVfileName = CSVfileName;
	 	m_acktimeout = acktimeout;
	 	m_warmupSeconds = warmupSeconds;
	 	m_runApplicationSeconds = runApplicationSeconds;

	 	m_rssi = rssi;


        //LogComponentEnable ("MacLow", LOG_LEVEL_ALL);
	 	//LogComponentEnable ("DcaTxop", LOG_LEVEL_ALL);
	 	//LogComponentEnable ("DcfManager", LOG_LEVEL_ALL);
	 	//LogComponentEnable ("ApWifiMac", LOG_LEVEL_ALL);
	 	//LogComponentEnable ("StaWifiMac", LOG_LEVEL_ALL);
	 	//LogComponentEnable ("ApWifiMac", LOG_LEVEL_ALL);
	 	//LogComponentEnable ("WifiMacHeader", LOG_LEVEL_ALL);
	 	//LogComponentEnable ("WifiRemoteStationManager", LOG_LEVEL_ALL);


  	  //topology

	 //modo infraestructura
//  	  NodeContainer wifiApNode;
//  	  wifiApNode.Create(1);
//  	  NodeContainer wifiStaNode;
//  	  wifiStaNode.Create (1);

	 	//modo ad-hoc
  	  NodeContainer tas;
  	  tas.Create (2);



  	  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  	  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  	  phy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

  	  channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel", "Speed", DoubleValue (300000000.0) );

  	  channel.AddPropagationLoss ("ns3::FixedRssLossModel", "Rss", DoubleValue (m_rssi));



  	  phy.SetChannel (channel.Create ());


  	          // turn off RTS/CTS for frames below 2200 bytes
  	  	  	  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", UintegerValue (m_rtsThreshold));
  	  	  	  // disable fragmentation for frames below 2200 bytes
  	  	  	  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", UintegerValue (2200));
  	  	  	  // Fix non-unicast data rate to be the same as that of unicast
  	  	  	  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode",
  	  	  	                        StringValue (phyMode));


  	  WifiHelper wifi = WifiHelper::Default ();


  	  //No llamar si ya se configuran los parámetros mac: acktimeout, Tslot...
  	  wifi.SetStandard (WIFI_PHY_STANDARD_80211g);
  	  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
  	                                  "DataMode",StringValue (phyMode),
  	                                  "ControlMode",StringValue (phyMode));



      //modo ad-hoc
  	  Ssid ssid = Ssid ("ns-3-ssid");
  	  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();


  	  wifiMac.SetType ("ns3::AdhocWifiMac",
  	    		"Ssid", SsidValue (ssid),
  	    								"Sifs", TimeValue (MicroSeconds (10)),
  	    								"Slot", TimeValue (MicroSeconds (20)),
  	    								"EifsNoDifs", TimeValue (MicroSeconds (10+304)),
  	    								"Pifs", TimeValue (MicroSeconds (10+20)),
  	    		      	                "AckTimeout", TimeValue (Seconds (m_acktimeout)),
  	    		      	                "CtsTimeout", TimeValue (Seconds (m_acktimeout)));
  	    		      	                //"MaxPropagationDelay", TimeValue (Seconds (100000.0 / 300000000.0)));

  	  NetDeviceContainer devices = wifi.Install (phy, wifiMac, tas);



  	  //modo infraestructura
  	 /*NqosWifiMacHelper mac = NqosWifiMacHelper::Default ();

  	  Ssid ssid = Ssid ("ns-3-ssid");
  	  mac.SetType ("ns3::ApWifiMac",
  	               "Ssid", SsidValue (ssid),
  	               "BeaconInterval" , TimeValue (Seconds (0.01024)),
  	               "BeaconGeneration", BooleanValue (true),
  	               "AckTimeout", TimeValue (Seconds (0.0006)),
  	               "CtsTimeout", TimeValue (Seconds (0.0006)),
  	               "MaxPropagationDelay", TimeValue (Seconds (200000.0 / 300000000.0)));


  	  NetDeviceContainer apDevice = wifi.Install (phy, mac, wifiApNode);
  	  NetDeviceContainer devices = apDevice;

  	  mac.SetType ("ns3::StaWifiMac",
  	               "Ssid", SsidValue (ssid),
  	                "AckTimeout", TimeValue (Seconds (0.0006)),
  	                "CtsTimeout", TimeValue (Seconds (0.0006)),
  	                "ActiveProbing", BooleanValue (false),
  	                "MaxPropagationDelay", TimeValue (Seconds (200000.0 / 300000000.0)));

  	  NetDeviceContainer staDevice = wifi.Install (phy, mac, wifiStaNode);
  	  devices.Add (staDevice);*/




  	  //Mobility

  	  // Note that with FixedRssLossModel, the positions below are not
  	 // used for received signal strength.
  	  MobilityHelper mobility;
  	  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  	  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  	  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  	  positionAlloc->Add (Vector (distance, 0.0, 0.0));
  	  mobility.SetPositionAllocator (positionAlloc);

  	 mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  	 //modo ad-hoc
  	 mobility.Install (tas);

  	 //modo infraestructura
  	 //mobility.Install (wifiApNode);
  	 //mobility.Install (wifiStaNode);

  	  InternetStackHelper stack;
  	  //modo ad-hoc
  	  stack.Install (tas);
  	  //modo infraestructura
  	  //stack.Install (wifiApNode);
  	  //stack.Install (wifiStaNode);




  	  Ipv4AddressHelper ipv4;
  	  //NS_LOG_INFO ("Assign IP Addresses.");
  	  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  	  Ipv4InterfaceContainer i = ipv4.Assign (devices);

  	  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));

  	  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  	  PopulateArpCache ();

  	  	std::stringstream ss1;
  	  	  ss1 << m_dataRate;
  	  	  std::string rate = ss1.str();

  	rate = rate + "kbps";

  	  OnOffHelper onoff1 ("ns3::UdpSocketFactory", InetSocketAddress (i.GetAddress (1), port1));
  	  onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  	  onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  	  onoff1.SetAttribute ("DataRate", DataRateValue (DataRate (rate)));
  	  onoff1.SetAttribute ("PacketSize", UintegerValue (m_packetSize));

  	    AddressValue remoteAddress1 (InetSocketAddress (i.GetAddress (0), port1));
  	    onoff1.SetAttribute ("Remote", remoteAddress1);
  	    UniformVariable var1;
  	    //modo adhoc
  	    ApplicationContainer temp1 = onoff1.Install (tas.Get(1));
  	    //modo infraestructura
  	    //ApplicationContainer temp1 = onoff1.Install (wifiStaNode);
  	    //temp.Start(Seconds (var.GetValue(m_warmupSeconds,m_warmupSeconds + 1)));
  	    temp1.Start(Seconds (m_warmupSeconds));//evito que haya colisiones al principio
  	    temp1.Stop (Seconds (m_warmupSeconds + m_runApplicationSeconds));

  	    //modo adhoc
  	  Ptr<Socket> sink1 = SetupPacketReceive1 (i.GetAddress (0), tas.Get(0));
  	    //infraestrucutra
  	  //Ptr<Socket> sink1 = SetupPacketReceive1 (i.GetAddress (0), wifiApNode.Get(0));

  	  	  OnOffHelper onoff2 ("ns3::UdpSocketFactory", InetSocketAddress (i.GetAddress (0), port2));
  	  	  onoff2.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  	  	  onoff2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  	  	  onoff2.SetAttribute ("DataRate", DataRateValue (DataRate (rate)));
  	  	  onoff2.SetAttribute ("PacketSize", UintegerValue (m_packetSize));

  	  	    AddressValue remoteAddress2 (InetSocketAddress (i.GetAddress (1), port2));
  	  	    onoff2.SetAttribute ("Remote", remoteAddress2);
  	  	    UniformVariable var2;
  	  	    //modo adhoc
  	  	  	ApplicationContainer temp2 = onoff2.Install (tas.Get(0));
  	  	  	//modo infraestructura
  	  	  	//ApplicationContainer temp2 = onoff2.Install (wifiApNode);
  	  	    //temp.Start(Seconds (var.GetValue(m_warmupSeconds,m_warmupSeconds + 1)));
  	  	    temp2.Start(Seconds (m_warmupSeconds+3));
  	  	    temp2.Stop (Seconds (m_warmupSeconds + m_runApplicationSeconds-3));


  	  	//modo adhoc
  	  	  	  Ptr<Socket> sink2 = SetupPacketReceive2 (i.GetAddress (1), tas.Get(1));
  	  	  	    //infraestrucutra
  	  	  	//  Ptr<Socket> sink2 = SetupPacketReceive2 (i.GetAddress (1), wifiStaNode.Get(0));


  	std::string namecap= "cap";
  	std::stringstream ss;
  	  ss << distance << "m_" << m_dataRate;
  	  std::string dist = ss.str();

  	namecap =  namecap + "_" + dist + "kbps_" + phyMode;

  	//phy.EnablePcap (namecap, devices);

  	  //Simulator::Stop (Seconds (10.0));

  	  FlowMonitorHelper flowmon;
  	  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  	  CheckThroughput ();




  	  //MacDrop
  	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRxDrop",MakeCallback(&MacDrop));


  	  Config::Connect ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::YansWifiPhy/State/Tx",MakeCallback(&Experiment::PhyTx1,this));
  	  Config::Connect ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::YansWifiPhy/State/RxOk",MakeCallback(&Experiment::PhyRx1,this));

  	  Config::Connect ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::YansWifiPhy/State/Tx",MakeCallback(&Experiment::PhyTx2,this));
  	  Config::Connect ("/NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::YansWifiPhy/State/RxOk",MakeCallback(&Experiment::PhyRx2,this));


  	  Simulator::Stop (Seconds (m_warmupSeconds + m_runApplicationSeconds + 10));
  	  Simulator::Run ();

  	  // 10. Print per flow statistics
  	    monitor->CheckForLostPackets ();
  	    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  	    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();

  	    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
  	      {

  	        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
  	        std::cout << "Flow " << i->first - 1 << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";           std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
  	        std::cout << i->first - 1 << "  Rx Bytes:   " << i->second.rxBytes << "\n";
  	        std::cout << i->first - 1 << "  Throughput: " << i->second.rxBytes * 8.0 / m_runApplicationSeconds / 1000 / 1000  << " Mbps\n";
  	        std::cout << i->first - 1 << "  Tx Packets:   " << i->second.txPackets << "\n";
  	        std::cout << i->first - 1 << "  Rx Packets:   " << i->second.rxPackets << "\n";
  	        std::cout << i->first - 1 << "  RxPackets x  Size:   " << (i->second.rxPackets * m_packetSize * 8)/( m_runApplicationSeconds) << " bps \n";
  	        std::cout << i->first - 1 << "  Delay Sum:   " << i->second.delaySum << "\n";
  	        std::cout << i->first - 1 << "  Average Delay:   " << i->second.delaySum / i->second.rxPackets<< "\n";
  	      }



  	  Simulator::Destroy ();
  	  //NS_LOG_UNCOND ("\n DONE3 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");

  }

