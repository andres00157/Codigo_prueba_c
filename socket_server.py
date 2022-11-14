
#Se importa el módulo
import socket
 
#instanciamos un objeto para trabajar con el socket
ser = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
 
#Puerto y servidor que debe escuchar
ser.bind(("", 8000))
 
#Aceptamos conexiones entrantes con el metodo listen. Por parámetro las conexiones simutáneas.
ser.listen(1)
 
#Instanciamos un objeto cli (socket cliente) para recibir datos
cli, addr = ser.accept()

while True:

    #Recibimos el mensaje, con el metodo recv recibimos datos. Por parametro la cantidad de bytes para recibir
    try:
        recibido = cli.recv(1024, socket.MSG_DONTWAIT).decode()
        assert len(recibido)>0
        #Si se reciben datos nos muestra la IP y el mensaje recibido
        print("Recibo conexion de la IP: " + str(addr[0]) + " Puerto: " + str(addr[1])+ " el mensaje es:"+recibido)

        #Devolvemos el mensaje al cliente
        cli.send((str(1)+"\n"+str(1.45)+"\n").encode())
    except:
        pass

#Cerramos la instancia del socket cliente y servidor
cli.close()
ser.close()

print("Conexiones cerradas")