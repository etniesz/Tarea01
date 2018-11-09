/*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "YASL.h"      // For Socket and SocketException
#include "checkArgs.h"
#include <iostream>    // For cerr and cout
#include <cstdlib>     // For atoi()
#include <fstream>   // Librería necesaria para el manejo de archivos

const uint32_t RCVBUFSIZE = 3000;    // Size of receive buffer; 3000 es un tamaño aceptable para la gran mayoria del codigo fuente de una página web.Un valor menor podria cortar el mensaje en pantalla

int main(int argc, char *argv[]) {

	checkArgs* argumentos = new checkArgs(argc, argv); 

	std::string servAddress;		//
	std::string echoString;			// Declaración de variables
	std::string archivoSalida;		//
	uint16_t    echoServPort;		// 


	servAddress   = argumentos->getArgs().SERVER;						//
	echoServPort  = argumentos->getArgs().PORT;						//  Asignación de datos según lo ingresado por consola
	archivoSalida = argumentos->getArgs().ARCHIVOSALIDA;					//
	echoString    = argumentos->getArgs().DATA+ "Host: "+servAddress+"\r\n\r\n";		//


	delete argumentos;

	uint32_t echoStringLen = echoString.length();   // Determine input length ; Largo del mensaje que se le enviará al servidor

	try {

		// Se crea el archivo en donde se almacenará la información rescatada desde la página
		std::ofstream archivoEscritura;
		archivoEscritura.open(archivoSalida,std::ios::out);	// Se le pasa el nombre del archivo ingresado por consola a través de 'archivoSalida'. Está en un estado de escritura
		if(archivoEscritura.fail()){
			std::cout << "Error. No se pudo abrir el archivo";
			exit(EXIT_FAILURE);
		}
		// Establish connection with the echo server
		TCPSocket sock(servAddress, echoServPort);  // Estableciendo conexión con el servidor, por medio de la página y puerto

		// Send the string to the echo server
		sock.send(echoString.c_str(), echoStringLen); // Se le envía el mensaje antes especificado al servidor

		char echoBuffer[RCVBUFSIZE + 1];    // Buffer for echo string + \0
		uint32_t bytesReceived = 0;              // Bytes read on each recv()
		uint32_t totalBytesReceived = 0;         // Total bytes read

		// Receive the same string back from the server
		std::cout << "Recibido: ";               // Setup to print the echoed string
		while (totalBytesReceived < echoStringLen) {
			// Receive up to the buffer size bytes from the sender
			if ((bytesReceived = (sock.recv(echoBuffer, RCVBUFSIZE))) <= 0) {
				std::cerr << "No es posible de leer\n";
				exit(EXIT_FAILURE);
			}
			archivoEscritura << echoBuffer;		 // la información que contiene 'echoBuffer' es enviada a 'archivoEscritura' escribiendo de esta manera el código fuente de la página
			totalBytesReceived += bytesReceived;     // Keep tally of total bytes
			echoBuffer[bytesReceived] = '\0';        // Terminate the string!
			std::cout << echoBuffer;                 // Print the echo buffer
		}
		archivoEscritura.close();
		std::cout << std::endl;

		// Destructor closes the socket

	} catch(SocketException &e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
