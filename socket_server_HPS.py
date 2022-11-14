
#Se importa el módulo
import socket
import gspread
from oauth2client.service_account import ServiceAccountCredentials
import pandas as pd
from datetime import datetime
import time

import firebase_admin

import firebase_admin
from firebase_admin import credentials as cre_firebase
from firebase_admin import db

import json


class DataBase:

    def __init__(self, Database,sheet_1,sheet_2,name_database):
        self.max_data= 1
        self.cont_out=1
        
        if(name_database == "Google"):
            print("Conectando a drive...")
            self.cond = 0
            self.data = []
            self.Database= Database
            self.sheet_1 = sheet_1
            self.sheet_2 = sheet_2
            scope = ['https://www.googleapis.com/auth/spreadsheets',"https://www.googleapis.com/auth/drive"]
            credentials = ServiceAccountCredentials.from_json_keyfile_name("exoesquelet-eafit-sheet-bcb340f88950.json", scope)
            client = gspread.authorize(credentials)
            self.data_base = client.open(Database)
            self.sheet1 = self.data_base.worksheet(self.sheet_1)
            self.sheet2 = self.data_base.worksheet(self.sheet_2) 
        else:
            print("Conectando a firebase...")
            self.cond=1
            self.data= []
            cred = cre_firebase.Certificate("exoesqueleto-eafit-firebase-adminsdk-ubdid-01f812b4a1.json")
            firebase_admin.initialize_app(cred, 
                {'databaseURL':"https://exoesqueleto-eafit-default-rtdb.firebaseio.com"})
            self.ref_data_soc = db.reference("/data/data_soc")
            self.ref_max_data = db.reference("/data/maximo_dato")
            self.ref_data_web = db.reference("/data/data_web")
            self.ref = db.reference("/data")
    
    
    
    def reset_database(self):
        if(self.cond==0):
            self.sheet1.clear()
            self.cont_out = 1
            self.sheet2.update('A1',[[0,0,0,0,0,1],[0]])
        else:
            ref = db.reference("/")
            self.cont_out = 1
            ref.set({"data":{"data_soc":{},"maximo_dato":{"max":0,"num_data":0},"data_web":{"Emocion":0,"Espasticidad":0,"Dolor":0,"Ambiente_rv":0,"Gender":"Hombre","stop":1}}})
        
    
    def __read_sheet_func(self,x):
        try:
            return float(x)
        except:
            if(x== "Hombre"):
                return 0
            else:
                return 1
    
    # Retorna 1 con hombre y 0 con mujer
    # emocion, espasticidad, dolor, realidad_virtual, genero, stop
    def read_sheet_2(self):
        if(self.cond==0):
            values_list = self.sheet2.row_values(1)
            return list(map(self.__read_sheet_func,values_list))
        else:
            datos = self.ref_data_web.get()
            return list(map(self.__read_sheet_func,list(datos.values())))

    def set_max_data(self,max_data):
        self.max_data = max_data
    
    # data will be: [timestamp, emocion,espasticidad,dolor,ambienteRV, disposicion, intensidad_torque, torque_control, torque_Final, stop]
    def set_data(self,data):
        
        self.data.append(data)
            
        if(len(self.data)>=self.max_data):
            self.send_data()
        
    def send_data(self):
        if(self.cond==0):
            self.sheet1.update('A'+str(self.cont_out),self.data)
        else:
            self.ref_data_soc.push({"id":self.cont_out,"tabla":self.data})
            
        self.cont_out+=self.max_data
        self.send_max_data()
        self.data = []

    def send_max_data(self):
        if(self.cond==0):
            self.sheet2.update('A2',self.cont_out)
        else:
            ## Se actualiza el maximo
            self.ref_max_data.update({"max":self.cont_out,"num_data":self.max_data})


class SocketConnect:
    def __init__(self):
        print("Haciendo conexion con el cliente...")
        #instanciamos un objeto para trabajar con el socket
        self.ser = socket.socket(socket.AF_INET, socket.SOCK_STREAM)       
        
        #Puerto y servidor que debe escuchar
        self.ser.bind(("", 8080))
        
        #Aceptamos conexiones entrantes con el metodo listen. Por parámetro las conexiones simutáneas.
        self.ser.listen(1)
 
        #Instanciamos un objeto cli (socket cliente) para recibir datos
        self.cli, self.addr = self.ser.accept()
        print("Conexion realizada")
    
    def read_data(self):
        try:
            recibido = self.cli.recv(1024, socket.MSG_DONTWAIT).decode()
            recibido = list(map(lambda x:float(x) ,recibido.split("\n")))
            flag = 0
            if(len(recibido)>0):
                flag = 1
        except Exception as e:
            flag = 0
            recibido=[]
        
        return flag, recibido

    def send_data(self, data):
        msg = ""
        for i in data:
            msg+=(str(i)+'\n')
        self.cli.send(msg.encode())
    
    def close_program(self):
        self.cli.close()
        self.ser.close()
        print("Conexiones cerradas")



if __name__== "__main__":
    database_google = DataBase("NewDatabase","Sheet1","sheet2","firebase")
    database_google.reset_database()
    
    database_google.set_max_data(2)
    
    conexion = SocketConnect()
    estado = 0
    tiempo_anterior = datetime.now()
    flag_timer = 0
    
    
    try:
        while(1):
            if(estado==0):
                if(flag_timer):
                    flag_timer=0
                    data_sheet_2= database_google.read_sheet_2()
                    if(data_sheet_2[5]==0):
                        # emocion, espasticidad, dolor, realidad_virtual, genero, stop
                        conexion.send_data(data_sheet_2)
                        estado = 1

            elif(estado==1):
                flag_buffer, mensaje=conexion.read_data()
                if(flag_buffer):
                    # id, ambiente_rv, disposicion, intensidad_torque, torque_control, torque_final, parar
                    print(mensaje)
                    
                    # se envia en set data:
                    #[timestamp, emocion,espasticidad,dolor,ambienteRV, disposicion, intensidad_torque, torque_control, torque_Final, stop]
                    database_google.set_data([ datetime.timestamp(datetime.now()),data_sheet_2[0],data_sheet_2[1],data_sheet_2[2],mensaje[1],mensaje[2],mensaje[3],mensaje[4],mensaje[5],mensaje[6]])
                    
                if(flag_timer):
                    flag_timer=0
                    data_sheet_2= database_google.read_sheet_2()
                    if(data_sheet_2[5]==1):
                        conexion.send_data(data_sheet_2)
                        estado= 0
            
            if((datetime.now()-tiempo_anterior).total_seconds()>2):
                tiempo_anterior = datetime.now()
                flag_timer = 1
                
    except Exception as e:
        print(e)
        conexion.close_program()