#OUT_DEFAULT_ARF=out_arf.dat
#OUT_DEFAULT_AARF=out_aarf.dat
#OUT_DEFAULT_CONSTANT=out_constant.dat
RATE_IMG=ratevsthroughput.png
DELAY_IMG=delayvsthroughput.png
#PLOT_DEFAULT_ARF=plot_rate_arf.dat
#PLOT_DEFAULT_AARF=plot_rate_aarf.dat
#PLOT_DEFAULT_CONSTANT=plot_rate_constant.dat
#PLOT_DELAY_ARF=plot_delay_arf.dat
#PLOT_DELAY_AARF=plot_delay_aarf.dat
#PLOT_DELAY_CONSTANT=plot_delay_constant.dat
#
#
#
#
#
#
#
#spinner() {
#    PID=$!
#    i=1
#    sp="/-\|"
#    echo -n ' '
#    while [ -d /proc/$PID ]
#    do
#        printf "\b${sp:i++%${#sp}:1}"
#        sleep .1
#    done
#}
#echo " "
#
#echo "Removing older data files "
#
#if [ -f $OUT_DEFAULT_ARF ];
#then
#   echo "Removed older $OUT_DEFAULT_ARF"
#   rm $OUT_DEFAULT_ARF
#fi
#
#if [ -f $OUT_DEFAULT_AARF ];
#then
#   echo "Removed older $OUT_DEFAULT_AARF"
#   rm $OUT_DEFAULT_AARF
#fi
#if [ -f $OUT_DEFAULT_CONSTANT ];
#then
#   echo "Removed older $OUT_DEFAULT_CONSTANT"
#   rm $OUT_DEFAULT_CONSTANT
#fi
#
#if [ -f $RATE_IMG ];
#then
#   echo "Removed older $RATE_IMG"
#   echo "Removed older $DELAY_IMG"
#   rm $RATE_IMG 
#   rm $DELAY_IMG
#   rm plot_delay_* 
#   rm plot_rate_* 
#fi
#
#echo " "
#echo "Simulating...This might take a while feel free to grab a cup of Coffee :)"
#echo " "
#echo "      --- Initiating simulations for arf rate control with BEB ... "
#for node in  5 10 20 30 40 
#do
#    echo -ne "          Performing simulations for $node nodes ..." 
#    ./waf --run "scratch/wifi-cw --nNodes=$node --Enable_Adaptive_Cw=0 --arf=1"  &>> $OUT_DEFAULT_ARF
#    echo "DONE"
#done
#
#echo " "
#echo "      --- Initiating simulations for aarf rate control with BEB ... "
#for node in  5 10 20 30 40 
#do
#    echo -ne "          Performing simulations for $node nodes ..." 
#    ./waf --run "scratch/wifi-cw --nNodes=$node --Enable_Adaptive_Cw=0 --aarf=1"  &>> $OUT_DEFAULT_AARF
#    echo "DONE"
#done
#echo " "
#echo "      --- Initiating simulations for constant rate control with BEB ... "
#for node in  5 10 20 30 40 
#do
#    echo -ne "          Performing simulations for $node nodes ..." 
#    ./waf --run "scratch/wifi-cw --nNodes=$node --Enable_Adaptive_Cw=0 --constant=1"  &>> $OUT_DEFAULT_CONSTANT
#    echo "DONE"
#done
#
#
#sed -i "/\b\(waf\|build\|Compiling\)\b/d" $OUT_DEFAULT_ARF 
#sed -i "/\b\(waf\|build\|Compiling\)\b/d" $OUT_DEFAULT_AARF 
#sed -i "/\b\(waf\|build\|Compiling\)\b/d" $OUT_DEFAULT_CONSTANT
# 
#cut -f1,2 $OUT_DEFAULT_ARF > $PLOT_DEFAULT_ARF 
#cut -f1,2 $OUT_DEFAULT_AARF > $PLOT_DEFAULT_AARF 
#cut -f1,2 $OUT_DEFAULT_CONSTANT > $PLOT_DEFAULT_CONSTANT 
#cut -f1,5 $OUT_DEFAULT_ARF > $PLOT_DELAY_ARF 
#cut -f1,5 $OUT_DEFAULT_AARF > $PLOT_DELAY_AARF 
#cut -f1,5 $OUT_DEFAULT_CONSTANT > $PLOT_DELAY_CONSTANT 
# 
#
#
#

echo -ne "      --- Plotting results obtained ... "



gnuplot <<- EOF
set xlabel "Number of Nodes"
set ylabel "Throughput (Kbps)"
set term png
set grid ytics
set output "$RATE_IMG"
set style fill solid
plot [-0.5:][0:700] 'plot_rate_constant.dat' u 2: xtic(1) with histogram title "Constant", 'plot_rate_arf.dat' u 2: xtic(1) with histogram title "ARF", 'plot_rate_aarf.dat' u 2: xtic(1) with histogram title "AARF"
EOF
xdg-open $RATE_IMG 


gnuplot <<- EOF
set xlabel "Number of Nodes"
set ylabel "Delay (ms)"
set term png
set grid ytics
set output "$DELAY_IMG"
set style fill solid
plot [-0.5:][0:8000] 'plot_delay_aarf.dat' u 2: xtic(1) with histogram title "AARF", 'plot_delay_arf.dat' u 2: xtic(1) with histogram title "ARF", 'plot_delay_constant.dat' u 2: xtic(1) with histogram title "Constant"
EOF
xdg-open $DELAY_IMG 






#
#
#gnuplot <<- EOF
#set xlabel "Number of Nodes"
#set ylabel "Throughput (Mbps)"
#set term png
#set output "$RATE_IMG"
#plot "plot_rate_arf.dat" w l lc rgb 'black' title "ARF ", \
#     'plot_rate_aarf.dat' w l lc rgb 'green' title "AARF ", \
#     'plot_rate_constant.dat' w l lc rgb 'red' title "Costant rate", \
##plot 'out.dat' using 1:2 with lines,'' using 1:3 with lines,'' using 1:4 with lines
#EOF
#xdg-open $RATE_IMG 
#gnuplot <<- EOF
#set xlabel "Number of Nodes"
#set ylabel "Delay (Nano seconds)"
#set term png
#set output "$DELAY_IMG"
#plot "plot_delay_arf.dat" w l lc rgb 'black' title "ARF ", \
#     'plot_delay_aarf.dat' w l lc rgb 'green' title "AARF ", \
#     'plot_delay_constant.dat' w l lc rgb 'red' title "Costant rate", \
##plot 'out.dat' using 1:2 with lines,'' using 1:3 with lines,'' using 1:4 with lines
#EOF
#xdg-open $DELAY_IMG 
#
echo "DONE"
echo " "
echo "Simulations complete results are present in following files"
echo " "
echo "$RATE_IMG  - Throughput variation comparision"
echo "$DELAY_IMG  - Throughput variation comparision"
