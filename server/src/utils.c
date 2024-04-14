#include"utils.h"

t_log* logger;

int iniciar_servidor(void)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PUERTO, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family,
							 servinfo->ai_socktype,
							 servinfo->ai_protocol);

	// Asociamos el socket a un puerto
	int error;
	
	error = bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	if(error == -1) log_error(logger, "Bind Syscall failed on utils.c|iniciar_servidor");

	// Escuchamos las conexiones entrantes

	error = listen(socket_servidor, SOMAXCONN);
	if(error == -1) log_error(logger, "Listen Syscall failed on utils.c|iniciar_servidor");


	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	// Aceptamos un nuevo cliente
	printf("Esperando Cliente...\n");

	int socket_cliente = accept(socket_servidor, NULL, NULL);

	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	size_t bytes;

	int32_t cod_op;

	bytes = recv(socket_cliente, &cod_op, sizeof(int32_t), MSG_WAITALL);

	if(bytes == 0)
	{	
		printf("Exit due to empty handshake message.\n");
		close(socket_cliente);
		return -1;
	}

	printf("Codigo de operacion recibido = %i | retornando handshake. \n", cod_op);
	return cod_op;
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);

	buffer = malloc(*size);

	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Msg: %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
