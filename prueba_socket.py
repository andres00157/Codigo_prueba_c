## CLiente

#Se importa el módulo
import socket

#Variables
host = 'localhost'
port = 8050

 
#Creación de un objeto socket (lado cliente)
obj = socket.socket()
 
#Conexión con el servidor. Parametros: IP (puede ser del tipo 192.168.1.1 o localhost), Puerto
obj.connect((host, port))
print("Conectado al servidor")
 
#Creamos un bucle para retener la conexion
while True:
    #Instanciamos una entrada de datos para que el cliente pueda enviar mensajes
    mens = input("Mensaje desde Cliente a Servidor >> ")
 
    #Con el método send, enviamos el mensaje
    obj.send(mens.encode())

#Cerramos la instancia del objeto servidor
obj.close()

#Imprimimos la palabra Adios para cuando se cierre la conexion
print("Conexión cerrada")