COMMAND=${1}
# data_len=
echo "COMMAND is \"${COMMAND}\""

type=${2}
st_tx=$((`ethtool -S ens6np0 | grep tx_bytes_phy | awk '{print $2}'`))
st_rx=$((`ethtool -S ens6np0 | grep rx_bytes_phy | awk '{print $2}'`))

start=$(date +%s.%N) 
sudo bash -c "${COMMAND}"
end=$(date +%s.%N)
elapsed=$(echo "$end - $start" | bc)   # 计算时间差，精确到毫秒
echo "程序执行时间为: $elapsed 秒"


ed_tx=$((`ethtool -S ens6np0 | grep tx_bytes_phy | awk '{print $2}'`))
ed_rx=$((`ethtool -S ens6np0 | grep rx_bytes_phy | awk '{print $2}'`))
# if [ "${type}" == "1" ];
# then
    total_bytes_both=$(( ${ed_tx}-${st_tx} + ${ed_rx}-${st_rx} ))
# else
    total_bytes_send=$(( ${ed_tx}-${st_tx}))
# fi
echo "total_phy_bytes of tx and rx is ${total_bytes_both}"
echo "total_phy_bytes of tx and rx is ${total_bytes_both}" >> record0.log
echo "total_phy_bytes of tx is ${total_bytes_send}"
echo "total_phy_bytes of tx is ${total_bytes_send}" >> record0.log
bandwidth_MB=$(echo "scale=2; $total_bytes_both  / 1024.0 / 1024 / $elapsed" | bc)
bandwidth_Gbps=$(echo "scale=2; $total_bytes_both  / 1024.0 / 1024.0 * 8.0 / 1024 / $elapsed" | bc)
bandwidth_MB_send=$(echo "scale=2; $total_bytes_send  / 1024.0 / 1024 / $elapsed" | bc)
bandwidth_Gbps_send=$(echo "scale=2; $total_bytes_send  / 1024.0 / 1024.0 * 8.0 / 1024 / $elapsed" | bc)
echo "程序执行时间为: $elapsed 秒"
echo "程序执行时间为: $elapsed 秒" >> record0.log
echo "程序上下行带宽为: $bandwidth_MB   MB/s"
echo "程序上下行带宽为: $bandwidth_MB   MB/s" >> record0.log
echo "程序上下行带宽为: $bandwidth_Gbps   Gbps/s"
echo "程序上下行带宽为: $bandwidth_Gbps   Gbps/s" >> record0.log
echo "程序上行带宽为: $bandwidth_MB_send   MB/s"
echo "程序上行带宽为: $bandwidth_MB_send   MB/s" >> record0.log
echo "程序上行带宽为: $bandwidth_Gbps_send   Gbps/s"
echo "程序上行带宽为: $bandwidth_Gbps_send   Gbps/s" >> record0.log

size=${3}

if [ "${type}" == "1" ];
then 
    mv record0.log ./log/"record_detail_${size}_12.txt"
    # mv record_detail0.log ./log/protobuf_8/"record_detail_${size}_8_detail.txt"
else
    mv record0.log ./log/"record_detail_${size}.txt"
    # mv record_detail0.log ./log/"record_detail_${size}_detail.txt"    
fi
#client  
#sudo bash ./eRPC-tests/multi_process.sh client 8 102400 10000
#sudo bash execute_multi.sh 'sudo bash ./multi_process.sh client 12 1024000 10000' 1 1024000

#server
#sudo bash  ./test_xrpc/multi_process.sh server 8 102400