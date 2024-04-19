ps -ef | grep ./build/flat


sudo kill -9 `ps -ef | grep ./build/flat | awk '{print $2 }'`

echo 'killed'
