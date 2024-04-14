#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	log_trace(logger, "Logger Iniciado");
	//log_debug(logger, "Diebug");
	//log_info(logger, "iInfo");
	//log_warning(logger, "iWarning");
	//log_error(logger, "iError");

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	// if(config_has_property(config, "CLAVE")) log_debug(logger, "Encontrado valor CLAVE en el archivo");
	
	// if(config_has_property(config, "IP")) log_debug(logger, "Encontrado valor IP en el archivo");

	// if(config_has_property(config, "PUERTO")) log_debug(logger, "Encontrado valor PUERTO en el archivo");

	// Loggeamos el valor de config

	valor = config_get_string_value(config, "CLAVE");
	//log_info(logger, "CLAVE = %s", valor);

	ip = config_get_string_value(config, "IP");
	//log_info(logger, "IP = %s", ip);

	puerto = config_get_string_value(config, "PUERTO");
	//log_info(logger, "PUERTO = %s", puerto);


	/* ---------------- LEER DE CONSOLA ---------------- */

	// leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);
	if(conexion == -1) exit(EXIT_FAILURE);
	else log_info(logger, "Conexion con el servidor establecida.");

	// Enviamos al servidor el valor de CLAVE como mensaje

	enviar_mensaje(valor , conexion);

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger = log_create("log.log", "CLIENT", true, LOG_LEVEL_TRACE);
	
	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create("cliente.config");

	if(nuevo_config == NULL) perror("Error al cargar el config");

	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	do
	{
		free(leido);
		leido = readline("> ");
		if(strcmp(leido, "") != 0) log_info(logger, ">> %s", leido);
	} while(strcmp(leido, "") != 0);

	// ¡No te olvides de liberar las lineas antes de regresar!
	free(leido);
}

int32_t handshake(int conexion, int32_t codigo_operacion)
{
	int32_t result;

	send(conexion, &codigo_operacion, sizeof(int32_t), 0);
	recv(conexion, &result, sizeof(int32_t), MSG_WAITALL);

	if(result == 0) printf("Handshake realizado correctamente con codigo = %i\n", codigo_operacion);
	else log_error(logger, "Handshake failed on client.c|handshake");

	return result;
}

void paquete(int conexion)
{
	t_paquete* paquete = crear_paquete();

	char* leido = NULL;
	int size_leido;

	do
	{
		if(leido != NULL) free(leido);
		leido = readline("> ");

		if(strcmp(leido, "") != 0)
		{
			size_leido = strlen(leido) + 1;
			agregar_a_paquete(paquete, leido, size_leido);
		};
	} while(strcmp(leido, "") != 0);

	// Enviar paquete
	enviar_paquete(paquete, conexion);

	// ¡No te olvides de liberar las lineas antes de regresar!
	free(leido);
	eliminar_paquete(paquete);
	
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/*
	  Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado
	*/
	liberar_conexion(conexion);
	log_destroy(logger);
	config_destroy(config);
}
