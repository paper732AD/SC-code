

#sudo bash multi_processes client 8 10240 1000
role=${1} #client or server
processes=${2} #processes
data_len=${3} #data_len
request=${4} #number request



if [ ${role} = 'client' ]
then
if [ -z ${processes} ]
then 
  echo "without data_len"
  exit 1
fi
if [ -z ${data_len} ]
then 
  echo "without data_len"
  exit 1
fi
if [ -z ${data_len} ]
then 
  echo "without data_len"
  exit 1
fi
  for (( i=0; i<${processes}; ++i ))
  do 
    client_port=$(( 31820+${i} ))
    server_port=$(( 31860+${i} ))
    echo "sudo ./build/flat_client -data_len=${data_len} -request=${request} -client_port=${client_port} -server_port=${server_port} -log_file=record${i}.log -detail_file=record_detail${i}.log"
    sudo ./build/flat_client -data_len=${data_len} -request=${request} -client_port=${client_port} -server_port=${server_port} -log_file=record${i}.log -detail_file=record_detail${i}.log &
  done
fi


if [ ${role} = 'server' ]
then

if [ -z ${processes} ]
then 
  echo "without data_len"
  exit 1
fi
if [ -z ${data_len} ]
then 
  echo "without data_len"
  exit 1
fi

  for (( i=0; i<${processes}; ++i ))
  do 
    server_port=$(( 31860+${i} ))
    echo "sudo ./build/latency_client -data_len=${data_len}  -server_port=${server_port} "
    sudo ./build/latency_client -data_len=${data_len}  -server_port=${server_port} &
  done
fi

wait