OUT_DEFAULT_CW=out_def.dat
OUT_ADAPTIVE_CW=out_adaptive.dat
THROUGHPUT_IMG=throughputvsNodes.png
PLOT_DEFAULT_CW=plot_def_cw.dat
PLOT_ADAPTIVE_CW=plot_adaptive_cw.dat
PLOT_DEFAULT_LOSS=plot_def_loss.dat
PLOT_ADAPTIVE_LOSS=plot_adaptive_loss.dat
PACKET_LOSS_IMG=packetlossvsNodes.png
PLOT_DEFAULT_DELAY=plot_def_delay.dat
PLOT_ADAPTIVE_DELAY=plot_adaptive_delay.dat
DELAY_IMG=delayvsNodes.png







spinner() {
    PID=$!
    i=1
    sp="/-\|"
    echo -n ' '
    while [ -d /proc/$PID ]
    do
        printf "\b${sp:i++%${#sp}:1}"
        sleep .1
    done
}
echo " "

echo "Removing older data files "

if [ -f $OUT_DEFAULT_CW ];
then
   echo "Removed older $OUT_DEFAULT_CW"
   rm $OUT_DEFAULT_CW
fi

if [ -f $OUT_ADAPTIVE_CW ];
then
   echo "Removed older $OUT_ADAPTIVE_CW"
   rm $OUT_ADAPTIVE_CW
fi

if [ -f $THROUGHPUT_IMG ];
then
   echo "Removed older $THROUGHPUT_IMG"
   rm $THROUGHPUT_IMG 
   rm $PACKET_LOSS_IMG 
   rm $DELAY_IMG 
fi

echo "      --- Initiating simulations for default backoff procedure ... "

for node in  5 10 20 30 40 
do
    echo -ne "          Performing simulations for $node nodes ..." 
    ./waf --run "scratch/wifi-cw --nNodes=$node --Enable_Adaptive_Cw=0 --arf=1"  &>> $OUT_DEFAULT_CW
    echo "DONE"
done

echo "      --- Initiating simulations for adaptive contention window ... "
#for node in $(seq 1 10);
#for node in 5 10 15 20 25 30 35 
for node in  5 10 20 30 40 
#for node in 2 4 6 8 10 20 30 40 
do
    echo -ne "          Performing simulations for $node nodes ..." 
    ./waf --run "scratch/wifi-cw --nNodes=$node --Enable_Adaptive_Cw=1 --arf=1"  &>> $OUT_ADAPTIVE_CW
    echo "DONE"
done


sed -i "/\b\(waf\|build\|Compiling\)\b/d" $OUT_DEFAULT_CW 
sed -i "/\b\(waf\|build\|Compiling\)\b/d" $OUT_ADAPTIVE_CW 

cut -f1,2 $OUT_DEFAULT_CW > $PLOT_DEFAULT_CW 
cut -f1,2 $OUT_ADAPTIVE_CW > $PLOT_ADAPTIVE_CW 
cut -f1,3 $OUT_DEFAULT_CW > $PLOT_DEFAULT_LOSS 
cut -f1,3 $OUT_ADAPTIVE_CW > $PLOT_ADAPTIVE_LOSS 
cut -f1,5 $OUT_DEFAULT_CW > $PLOT_DEFAULT_DELAY 
cut -f1,5 $OUT_ADAPTIVE_CW > $PLOT_ADAPTIVE_DELAY 
 




echo -ne "      --- Plotting results obtained ... "

gnuplot <<- EOF
set xlabel "Number of Nodes"
set grid
set ylabel "Throughput (Kbps)"
set term png
set output "$THROUGHPUT_IMG"
plot '$PLOT_DEFAULT_CW' u 1:2 w lp t 'BEB', '$PLOT_ADAPTIVE_CW' u 1:2 w lp t 'Adaptive CW'
#plot '$PLOT_DEFAULT_CW' w l lc rgb 'black' title "BEB",'$PLOT_ADAPTIVE_CW' w l lc rgb 'green' title "Adpative CW" 
#plot 'out.dat' using 1:2 with lines,'' using 1:3 with lines,'' using 1:4 with lines
EOF
xdg-open $THROUGHPUT_IMG 

gnuplot <<- EOF
set xlabel "Number of Nodes"
set ylabel "Packet loss percentage"
set grid
set term png
set output "$PACKET_LOSS_IMG"
plot '$PLOT_DEFAULT_LOSS' u 1:2 w lp t 'BEB', '$PLOT_ADAPTIVE_LOSS' u 1:2 w lp t 'Adaptive CW'
#plot '$PLOT_DEFAULT_LOSS' w l lc rgb 'black' title "BEB",'$PLOT_ADAPTIVE_LOSS' w l lc rgb 'green' title "Adaptive CW" 
#plot 'out.dat' using 1:2 with lines,'' using 1:3 with lines,'' using 1:4 with lines
EOF
xdg-open $PACKET_LOSS_IMG 
#gnuplot <<- EOF
#set grid
#set xlabel "Number of Nodes"
#set ylabel "Aerage delay"
#set term png
#set output "$DELAY_IMG"
#plot '$PLOT_DEFAULT_DELAY' w l lc rgb 'black' title "BEB",'$PLOT_ADAPTIVE_DELAY' w l lc rgb 'green' title "Adaptive CW" 
#EOF
#xdg-open $DELAY_IMG 

echo "DONE"
echo " "
echo "Simulations complete results are present in following files"
echo " "
echo "$THROUGHPUT_IMG  - Throughput variation comparision"
echo "$PACKET_LOSS_IMG - Packet Loss percentage variation comparision"
#echo "$DELAY_IMG       - Aerage delay comparision"
