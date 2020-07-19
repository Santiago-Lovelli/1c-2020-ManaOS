echo "Comienzo compilacion Broker"
cd /home/utnso/workspace/tp-2020-1c-ManaOS-/Broker/
mkdir Debug
cd Debug
mkdir src
gcc -I"/home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/Broker.d" -MT"src/Broker.o" -o "src/Broker.o" "../src/Broker.c"
gcc -L"/home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos/Debug" -o "Broker"  ./src/Broker.o   -lComunParaTodos -lcommons -lpthread
echo "Termine compilacion Broker"