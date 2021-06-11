import time
import random
import subprocess

from robworld.RobotEPuck import RobotEPuck
import pygame

# Requiere simulador corriendo el "epuck.world"
class TestCamera():
    def __init__( self ):
        pass

    def run( self ):
        # Levantamos el simulador en otro proceso
        try:
            #pg = subprocess.Popen( [ "robworld", "-m", "../worlds/epuck.world" ], shell=False )
            time.sleep( 1 )
        except Exception as e:
            print( e )
            exit()

        # Los datos de conexion al simulador
        host = "127.0.0.1"
        port = 44444

        # Usamos try/except para conocer los errores que se produzcan
        try:
            # Accesamos el robot y configuramos algunos de sus atributos
            epuck  = RobotEPuck( "Epuck-01" , host, port )
            epuck.setLedRing( 1 )
            epuck.setSpeed( -5, 5 )

            # Obtenemos la primera imagen
            img = epuck.getCameraImage()
            imglen = len( img )

            # Usaremos "pygame" para mostrar la camara lineal del robot
            pygame.init()
            screen = pygame.display.set_mode( ( imglen*10, 80) )

            # Loop clasico
            t = time.time()
            while( time.time() - t < 20 ):
                # requerido por pygame
                for e in pygame.event.get(): pass

                # trazamos la imagen de la camara
                for i in range( imglen ):
                    color = img[ i ]
                    pygame.draw.rect( screen, color, ( i*10, 0, 10, 80) )
                pygame.display.flip()

                # obtenemos la siguiente imagen
                img = epuck.getCameraImage()
                time.sleep( 0.001 )
            epuck.setSpeed( 0, 0 )
            epuck.close()
        except ConnectionResetError:
            print( "Conexion abortada" )
        except Exception as e:
            print( e )

        # Detenemos el simulador
        #pg.send_signal( subprocess.signal.SIGTERM )


# show time
TestCamera().run()
