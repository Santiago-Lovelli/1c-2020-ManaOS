echo "
::: Comienzo compilacion Broker :::
"
cd /home/utnso/workspace/tp-2020-1c-ManaOS-/Broker2/
mkdir Debug
cd Debug
mkdir src
gcc -I"/home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/Broker2.d" -MT"src/Broker2.o" -o "src/Broker2.o" "../src/Broker2.c"
gcc -L"/home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos/Debug" -o "Broker2"  ./src/Broker2.o   -lComunParaTodos -lcommons -lpthread
echo "
 ::: Termine compilacion Broker ::: 
 "
