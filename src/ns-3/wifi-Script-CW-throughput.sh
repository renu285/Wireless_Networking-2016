# Script to simulate throughput vs Cwmin

THROUGHPUT_IMG=Contentionwindowvsthroughput.png


echo "Removing older data files "

if [ -f out_5.dat ];
then
   echo "Removed older out_5.dat"
   rm out_5.dat 
   rm plot_5.dat 
fi

if [ -f out_10.dat ];
then
   echo "Removed older out_10.dat"
   rm out_10.dat 
   rm plot_10.dat 
fi

if [ -f out_20.dat ];
then
   echo "Removed older out_10.dat"
   rm out_20.dat 
   rm plot_20.dat 
fi

if [ -f out_30.dat ];
then
   echo "Removed older out_10.dat"
   rm out_30.dat 
   rm plot_30.dat 
fi


if [ -f $THROUGHPUT_IMG ];
then
   echo "Removed older $THROUGHPUT_IMG"
   rm $THROUGHPUT_IMG
fi


echo " "
echo "Simulating...This might take a while feel free to grab a cup of Coffee :)"
echo " "

for node in 5 10 30  
do
    for ((cw=32; cw<=1024 ; cw*=2))
    do
        echo -ne "      --- Initiating simulations for $node stations with CW=$cw ... "
        #./waf --run "scratch/wifi-cw --nNodes=$node --Enable_Adaptive_Cw=1 --minCw=$cw --maxCw=`expr $cw \* 8`"  &>> out_$node.dat
        ./waf --run "scratch/wifi-cw --nNodes=$node --Enable_Adaptive_Cw=1 --minCw=$cw --maxCw=1023"  &>> out_$node.dat
        echo "DONE"
    done 
done


sed -i "/\b\(waf\|build\|Compiling\)\b/d" out_5.dat 
sed -i "/\b\(waf\|build\|Compiling\)\b/d" out_10.dat 
sed -i "/\b\(waf\|build\|Compiling\)\b/d" out_20.dat 
sed -i "/\b\(waf\|build\|Compiling\)\b/d" out_30.dat
 
cut -f4,2 out_5.dat   | awk ' { t = $1; $1 = $2; $2 = t; print; } ' > plot_5.dat 
cut -f4,2 out_10.dat  | awk ' { t = $1; $1 = $2; $2 = t; print; } ' > plot_10.dat 
cut -f4,2 out_20.dat  | awk ' { t = $1; $1 = $2; $2 = t; print; } ' > plot_20.dat 
cut -f4,2 out_30.dat  | awk ' { t = $1; $1 = $2; $2 = t; print; } ' > plot_30.dat 
 
echo -ne "      --- Plotting results obtained ... "

gnuplot <<- EOF
set xlabel "CWmin"
set grid 
set ylabel "Throughput (Kbps)"
set xrange [32:1024]

set term png
set style func linespoints
set output "$THROUGHPUT_IMG"

plot 'plot_5.dat' u 1:2 w lp t '5 stations', 'plot_10.dat' u 1:2 w lp t '10 stations','plot_30.dat' u 1:2 w lp t '30 stations'

#plot "plot_5.dat"  lp t "5 stations", \
#     'plot_10.dat' lp t "10 stations",\
#     'plot_30.dat' lp t "30 stations"
#plot 'out.dat' using 1:2 with lines,'' using 1:3 with lines,'' using 1:4 with lines
EOF
echo "DONE"

echo "Result present in $THROUGHPUT_IMG"
xdg-open $THROUGHPUT_IMG 

