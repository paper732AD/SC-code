

COMMAND=${1}
# data_len=
echo "COMMAND is \"${COMMAND}\""

st_tx=$((`ethtool -S enp33s0np0 | grep tx_bytes_phy | awk '{print $2}'`))
st_rx=$((`ethtool -S enp33s0np0 | grep rx_bytes_phy | awk '{print $2}'`))

sudo bash -c "${COMMAND}"

ed_tx=$((`ethtool -S enp33s0np0 | grep tx_bytes_phy | awk '{print $2}'`))
ed_rx=$((`ethtool -S enp33s0np0 | grep rx_bytes_phy | awk '{print $2}'`))

total_bytes=$(( ${ed_tx}-${st_tx} + ${ed_rx}-${st_rx} ))

echo "total_phy_bytes of tx and rx is ${total_bytes}" >> latency_test.txt

size=${2}

mv latency_test.txt "latency_test_${size}.txt"
mv latency_test_detail.txt "latency_test_${size}_detail.txt"
