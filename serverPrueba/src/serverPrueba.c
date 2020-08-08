

#include <stdio.h>
#include <stdlib.h>
#include <Logger/Logger.h>
#include <commons/collections/list.h>
#include <Serializacion/Serializacion.h>
#include <Conexiones/Conexiones.h>

int main(void) {
	puts("Superand0 Sac-Server Iniciando..."); /* prints Superand0 Sac-Server Iniciando... */

		t_log* log = iniciar_log("GameBoy");

		char *puerto = "9480";

		log_info(log, "Puerto GameCard: %s", puerto);

		char *ip = "127.0.0.1";

		log_info(log, "Ip GameCard: %s", ip);

		int conexion = iniciar_servidor(ip, puerto, log);
		while(1){
		int cliente = esperar_cliente_con_accept(conexion, log);

		HeaderDelibird headerRecibido = Serialize_RecieveHeader(cliente);

		if (headerRecibido.tipoMensaje == -1) {
			log_error(log, "Se desconecto el Cliente\n");
			return EXIT_SUCCESS;
		}

		switch (headerRecibido.tipoMensaje) {
			case d_LOCALIZED_POKEMON:;
				uint32_t id;
				char* pokemon;
				void* packLocalizedPokemon = Serialize_ReceiveAndUnpack(cliente, headerRecibido.tamanioMensaje);
				t_list *list =list_create();
				Serialize_Unpack_LocalizedPokemon(packLocalizedPokemon, &id, &pokemon, &list);
				log_info(log,"id: %i, pkm: %s",id,pokemon);
				for(int i = 0; i<list->elements_count; i++){
					d_PosCant* asd = list_get(list,i);
					log_info(log,"x: %i, y:%i",asd->posX,asd->posY);
				}

				int server = conectarse_a_un_servidor("127.0.0.1","7269",log);

				d_PosCant** posicionCantidad = malloc(sizeof(d_PosCant**) + sizeof(uint32_t));
				posicionCantidad[0] = NULL;

				for (int i = 0; i < list_size(list); ++i) {
					d_PosCant* posicion = list_get(list,i);
					log_info(log,"x: %i, y: %i", posicion->posX, posicion->posY);
					posicionCantidad = realloc(posicionCantidad, (sizeof(d_PosCant**) + (i+1)*(sizeof(d_PosCant*)) + sizeof(uint32_t) ) );
					posicionCantidad[i] = posicion;
					posicionCantidad[i+1] = NULL;
				}

				Serialize_PackAndSend_LOCALIZED_POKEMON(server,id,pokemon,posicionCantidad);

				HeaderDelibird headerack = Serialize_RecieveHeader(server);
				void* paqueteack = Serialize_ReceiveAndUnpack(server, headerack.tamanioMensaje);

				uint32_t result = Serialize_Unpack_ACK(paqueteack);

				log_info(log,"ack= %i", result);

				break;
			default:;
				log_error(log, "Mensaje no entendido: %i\n", headerRecibido);
				void* packBasura = Serialize_ReceiveAndUnpack(cliente,
						headerRecibido.tamanioMensaje);
				free(packBasura);
				break;
		}
		}
	return EXIT_SUCCESS;
	}

