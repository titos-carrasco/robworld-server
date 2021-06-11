import socket

from robworld import RobotBase

class RobotThymio2( RobotBase.RobotBase ):
    """
    Clase "wrapper" para acceder a un robot remoto del tipo EPuck

    Parameters
        name: nombre del robot a controlar en el simulador
        host: servidor en donde se encuenra este robot
        port: puerta en donde se encuentra este robot
    """
    tipo = "thymio2"

    def __init__( self, name:str, host:str, port:int  ):
        super().__init__( name, host, port )

    def getSensors( self ):
        """
        Actualiza el valor de los sensores del robot
        """
        resp = super().getSensors()
        self.proximitySensorValues = tuple( resp["proximitySensorValues"] )
        self.proximitySensorDistances = tuple( resp["proximitySensorDistances"] )
        self.groundSensorValues = tuple( resp["groundSensorValues"] )

    def setLedsIntensity( self, leds:list ):
        """
        Cambia la intensidad de los leds del robot

        Parameters
            leds: un arreglo con el valor del tipo float (0 a 1) a
                  asignar como intensidad a cada led. El indice del
                  arreglo corresponde al led a operar
        """
        pkg = { "cmd":"setLedsIntensity", "leds": leds }
        resp = self.sendPkg( pkg )

    def __str__( self ):
        return f"RobotThymio2 >> name:{self.name} - host={self.host} - port={self.port}"
