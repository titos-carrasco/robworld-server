import robworld
import robworld.RobotBase
import robworld.RobotEPuck
import robworld.RobotThymio2
import robworld.RobotMarxbot


class TestHelp():
    def __init__( self ):
        pass

    def run( self ):
        # Utilice la tecla "q" para avanzar entre cada pantalla de ayuda
        help( robworld )
        help( robworld.RobotBase )
        help( robworld.RobotEPuck )
        help( robworld.RobotThymio2 )
        help( robworld.RobotMarxbot )


# show time
TestHelp().run()
