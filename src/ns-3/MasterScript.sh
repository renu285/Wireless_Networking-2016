clear
echo "##########################################################"
echo " "
echo " Wireless Networking NS3 Simulation"
echo " "
echo "##########################################################"

./wifi-Script-CW-throughput.sh
./wifi-Script-CW-delay.sh  
./wifi-Script.sh
./wifi-Script-rate-BEB.sh 
echo ""
echo " -- The results of Simulation are present in "
echo "Contentionwindowvsthroughput.png - comparision of contention window with throughput"
echo "Contentionwindowvsdelay.png      - Comparision of contention window with delay"
echo "Contentionwindowvsdelay.png      - Comparision of contention window with delay"
echo "throughputvsNodes.png  - Throughput variation comparision"
echo "packetlossvsNodes.png - Packet Loss percentage variation comparision"
echo "ratevsthroughput.png  - Throughput variation comparision"
echo "delayvsthroughput.png  - Throughput variation comparision"

