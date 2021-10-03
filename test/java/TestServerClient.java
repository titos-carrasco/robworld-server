import rcr.robworld.RobotThymio2;
import rcr.robworld.RobotEPuck;

import java.util.Arrays;
import java.util.Random;

// Requiere servidor en el playground "simple.world"
public class TestServerClient {

    public void run() {
        // Los datos de conexion al playground
        String host = "127.0.0.1";
        int port = 44444;

        // Usamos try/catch para conocer los errores que se produzcan
        Random random = new Random();
        try {
            // Accesamos el robot y configuramos algunos de sus atributos
            RobotThymio2 rob01 = new RobotThymio2( "Thymio-01", host, port );
            RobotEPuck rob02 = new RobotEPuck( "Epuck-01", host, port );

            // Loop clasico
            long t = System.currentTimeMillis() / 1000;
            while( System.currentTimeMillis() / 1000 - t < 20 ) {
                rob01.setSpeed( random.nextInt(2000)-1000, random.nextInt(2000)-1000 );
                rob02.setSpeed( random.nextInt(2000)-1000, random.nextInt(2000)-1000 );

                rob02.getSensors();

                System.out.print( "pos: " );
                System.out.println( Arrays.toString( rob02.getPos() ) );

                System.out.print( "speed: " );
                System.out.println( Arrays.toString( rob02.getSpeed() ) );

                System.out.print( "angle: " );
                System.out.println( rob02.getAngle() );

                System.out.print( "proximitySensorValues: " );
                System.out.println( Arrays.toString( rob02.getProximitySensorValues() ) );

                System.out.print( "proximitySensorDistances: " );
                System.out.println( Arrays.toString( rob02.getProximitySensorDistances() ) );

                Thread.sleep( 1000 );
            }

            rob01.setSpeed( 0, 0 );
            rob02.setSpeed( 0, 0 );
            rob01.close();
            rob02.close();
        }
        catch( Exception e ) {
            System.out.println( e );
        }
    }

    public static void main( String[] args ) {
        TestServerClient test = new TestServerClient();
        test.run();
    }

}
