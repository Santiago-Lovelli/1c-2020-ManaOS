echo "::: Comenzamos deploy :::"

bash script_commons.sh
bash script_para_shared.sh
bash scriptCompilarComunParaTodos.sh
bash scriptCompilarGameBoy.sh
bash scriptCompilarGameCard.sh
bash scriptCompilarTeam.sh
bash crearCarpetasDePrueba.sh

echo "::: Terminamos deploy :::"