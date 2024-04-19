

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
    echo "sudo ./build/Client -request_len=${data_len} -total_request=${request} -port=${server_port} -log_file=record${i}.log"
    sudo ./build/Client -request_len=${data_len} -total_request=${request} -port=${server_port} -log_file=record${i}.log &
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
    echo "sudo ./build/Server -request_len=${data_len}  -port=${server_port} "
    sudo ./build/Server -request_len=${data_len}  -port=${server_port} &
  done
fi

wait