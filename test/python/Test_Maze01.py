import time

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

# Requiere simulador corriendo el "maze01.world"
# utilizamos el algoritmo de https://stackoverflow.com/questions/66942322/wall-follower-algorithm-in-prolog
class TestMaze01():
    def __init__( self ):
        pass

    def run( self ):
        # los datos de conexion al simulador
        host = "127.0.0.1"
        port = 44444

        # usamos try/except para conocer los errores que se produzcan
        try:
            # accesamos el robot
            epuck = RobotEPuck( "Epuck-01", host, port )        # tiene 8 sensores de proximidad
            epuck.setSpeed( 10, 10 )

            t = time.time()
            while( time.time() - t < 60*3 ):
                time.sleep( 0.1 )
                epuck.getSensors()
                s_front = ( epuck.proximitySensorDistances[0] + epuck.proximitySensorDistances[0] )/2
                s_right = epuck.proximitySensorDistances[2]
                s_left = epuck.proximitySensorDistances[5]

                if( s_left >= 12 ):
                    time.sleep( 0.4 )
                    epuck.setSpeed( -10, 10 )
                    time.sleep( 0.4 )
                    epuck.setSpeed( 10, 10 )
                    time.sleep( 1 )
                elif( s_front < 4 ):
                    epuck.setSpeed( 10, -10 )
                    time.sleep( 0.4 )
                    epuck.setSpeed( 10, 10 )

            epuck.setSpeed( 0, 0 )
        except ConnectionResetError:
            print( "Conexion abortada" )
        except Exception as e:
            print( e )


# show time
TestMaze01().run()
