echo "
Pensaste que era un script de deploy, pero era yo dio

"
sleep 5;

echo "
::: Comenzamos deploy :::
"

bash script_commons.sh
bash script_para_shared.sh
bash scriptStand.sh
bash crearCarpetasDePrueba.sh

echo "
::: Terminamos deploy :::
"