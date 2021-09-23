import time

from robworld.RobotThymio2 import RobotThymio2
from robworld.RobotEPuck import RobotEPuck

"""
Robot thymio2:
    Sensores de Proximidad:
        0: frontal izquierdo izquierdo
        1: frontal izquierdo
        2: frontal frontal
        3: frontal derecho
        4: frontal derecho derecho
        5: trasero izquierdo
        6: trasero derecho
    Sensores de línea:
        0: izquierdo
        1: derecho

Robot epuck  :
    Sensores de Proximidad:
        0: frontal frontal derecho
        1: frontal derecho
        2: derecho
        3: trasero derecho
        4: trasero izquierdo
        5: izquierdo
        6: frontal izquierdo
        7: frontal frontal izquierdo
"""

# Requiere simulador corriendo el "simple.world"
class TestSensorsObject():
    def __init__( self ):
        pass

    def run( self ):
        # los datos de conexion al simulador
        host = "127.0.0.1"
        port = 44444

        # usamos try/except para conocer los errores que se produzcan
        try:
            # Accesamos los robots
            thymio = MyThymio2( "Thymio-01", host, port )     # tiene 7 sensores de proximidad
            epuck  = MyEPuck( "Epuck-01", host, port )        # tiene 8 sensores de proximidad

            # loop clasico
            t = time.time()
            while( time.time() - t < 20 ):
                distanciaThymio2 = thymio.checkObstacle()
                distanciaEpuck = epuck.checkObstacle()

                # mostramos el valor de los sensores utilizados
                print( "thymio: %10.5f - epuck: %10.5f" % ( distanciaThymio2, distanciaEpuck ) )

                time.sleep( 1 )

            thymio.setSpeed( 0, 0 )
            epuck.setSpeed( 0, 0 )
            thymio.close()
            epuck.close()
        except ConnectionResetError:
            print( "Conexion abortada" )
        except Exception as e:
            print( e )


class MyThymio2( RobotThymio2 ):
    def __init__( self, name, host, port ):
        super().__init__( name, host, port )
        self.s = 10
        self.setSpeed( self.s, self.s )

    def checkObstacle( self ):
        self.getSensors()
        distancia = self.proximitySensorDistances[2]
        if( distancia < 8 ):
            self.setSpeed( -self.s*10, self.s )
        else:
            self.setSpeed( self.s, self.s )
        return distancia


class MyEPuck( RobotEPuck):
    def __init__( self, name, host, port ):
        super().__init__( name, host, port )
        self.s = 20
        self.setSpeed( self.s, self.s )

    def checkObstacle( self ):
        self.getSensors()
        distancia = self.proximitySensorDistances[0]
        if( distancia < 8 ):
            self.setSpeed( -self.s*8, self.s )
        else:
            self.setSpeed( self.s, self.s )
        return distancia

# show time
TestSensorsObject().run()
