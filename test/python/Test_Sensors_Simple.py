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
class TestSensorsSimple():
    def __init__( self ):
        pass

    def run( self ):
        # los datos de conexion al simulador
        host = "127.0.0.1"
        port = 44444

        # usamos try/except para conocer los errores que se produzcan
        try:
            # Accesamos los robots
            thymio = RobotThymio2( "Thymio-01", host, port )     # tiene 7 sensores de proximidad
            epuck = RobotEPuck( "Epuck-01", host, port )        # tiene 8 sensores de proximidad

            thymio.setSpeed( 10, 10 )
            epuck.setSpeed( 10, 10 )

            # loop clasico
            t = time.time()
            while( time.time() - t < 20 ):
                thymio.getSensors()
                epuck.getSensors()

                distanciaThymio = thymio.proximitySensorDistances[2]
                distanciaEpuck = epuck.proximitySensorDistances[0]

                if( distanciaThymio < 8 ):
                    thymio.setSpeed( -120, 10 )
                else:
                    thymio.setSpeed( 10, 10 )

                if( distanciaEpuck < 8 ):
                    epuck.setSpeed( -10, 10 )
                else:
                    epuck.setSpeed( 10, 10 )

                # mostramos el valor de los sensores utilizados
                print( "thymio: %10.5f - epuck: %10.5f" % ( distanciaThymio, distanciaEpuck ) )

                time.sleep( 0.5 )

            thymio.setSpeed( 0, 0 )
            epuck.setSpeed( 0, 0 )
        except ConnectionResetError:
            print( "Conexion abortada" )
        except Exception as e:
            print( e )


# show time
TestSensorsSimple().run()
