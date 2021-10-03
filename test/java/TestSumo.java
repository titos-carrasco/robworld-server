import rcr.robworld.RobotThymio2;

import java.util.Arrays;

// Requiere servidor en el playground "sumo.world"
public class TestSumo {

    public void run() {
        // Los datos de conexion al playground
        String host = "127.0.0.1";
        int port = 44444;

        // Usamos try/catch para conocer los errores que se produzcan
        int speed = 100;
        try {
            // Accesamos el robot y configuramos algunos de sus atributos
            RobotThymio2 rob = new RobotThymio2( "Thymio-01", host, port );
            rob.setSpeed( speed, speed );

            // Loop clasico
            long t = System.currentTimeMillis() / 1000;
            while( System.currentTimeMillis() / 1000 - t < 20 ) {
                rob.getSensors();

                System.out.print( "pos: " );
                System.out.println( Arrays.toString( rob.getPos() ) );

                System.out.print( "speed: " );
                System.out.println( Arrays.toString( rob.getSpeed() ) );

                System.out.print( "angle: " );
                System.out.println( rob.getAngle() );

                System.out.print( "proximitySensorValues: " );
                System.out.println( Arrays.toString( rob.getProximitySensorValues() ) );

                System.out.print( "proximitySensorDistances: " );
                System.out.println( Arrays.toString( rob.getProximitySensorDistances() ) );

                System.out.print( "groundSensorValues: " );
                System.out.println( Arrays.toString( rob.getGroundSensorValues() ) );


                double[] gsv = rob.getGroundSensorValues();
                double gl = gsv[0];
                double gr = gsv[1];
                if( gl < 200 || gr < 200 ) {
                    rob.setSpeed( -speed, -speed );
                    Thread.sleep( 2000 );
                    rob.setSpeed( -speed, speed );
                    Thread.sleep( 2000 );
                    rob.setSpeed( speed, speed );
                }

                Thread.sleep( 10 );
            }

            rob.setSpeed( 0, 0 );
            rob.close();
        }
        catch( Exception e ) {
            System.out.println( e );
        }
    }

    public static void main( String[] args ) {
        TestSumo test = new TestSumo();
        test.run();
    }

}
