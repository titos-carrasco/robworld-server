import time
import random

from robworld.RobotThymio2 import RobotThymio2

# Requiere simulador corriendo el "sumo.world"
class TestSumo():
    def __init__( self ):
        pass

    def run( self ):
        # Los datos de conexion al simulador
        host = "127.0.0.1"
        port = 44444

        # Usamos try/except para conocer los errores que se produzcan
        speed = 100
        try:
            # Accesamos el robot y configuramos algunos de sus atributos
            rob = RobotThymio2( "Thymio-01", host, port )
            rob.setSpeed( speed, speed )

            # Loop clasico
            t = time.time()
            while( time.time() - t < 20 ):
                rob.getSensors()

                print( "pos:", rob.pos )
                print( "speed:", rob.speed )
                print( "angle:", rob.angle )
                print( "proximitySensorValues:", rob.proximitySensorValues )
                print( "proximitySensorDistances:" , rob.proximitySensorDistances )
                print( "groundSensorValues:" , rob.groundSensorValues )

                gl = rob.groundSensorValues[0]
                gr = rob.groundSensorValues[1]
                if( gl < 200 or gr < 200 ):
                    rob.setSpeed( -speed, -speed )
                    time.sleep( 1 )
                    rob.setSpeed( -speed, speed )
                    time.sleep( 1 )
                    rob.setSpeed( speed, speed )
                time.sleep( 0.001 )
            rob.setSpeed( 0, 0 )
            rob.close()
        except ConnectionResetError:
            print( "Conexion abortada" )
        except Exception as e:
            print( e )


# show time
TestSumo().run()
