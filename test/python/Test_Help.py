import robworld
import robworld.RobotBase
import robworld.RobotEPuck
import robworld.RobotThymio2


class TestHelp():
    def __init__( self ):
        pass

    def run( self ):
        # Utilice la tecla "q" para avanzar entre cada pantalla de ayuda
        help( robworld )
        help( robworld.RobotBase )
        help( robworld.RobotEPuck )
        help( robworld.RobotThymio2 )


# show time
TestHelp().run()
