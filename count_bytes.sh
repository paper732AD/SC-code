


st_tx=$((`ethtool -S enp33s0np0 | grep tx_bytes_phy | awk '{print $2}'`))
st_rx=$((`ethtool -S enp33s0np0 | grep rx_bytes_phy | awk '{print $2}'`))

total_bytes=$(( ${st_tx} + ${st_rx} ))

echo "bytes = ${total_bytes}"

