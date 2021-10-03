import rcr.robworld.RobotMarxbot;

import java.util.Arrays;
import java.util.Random;


// Requiere servidor en el playground "maxbort.world"
public class TestMarxbot {

    public void run() {
        // Los datos de conexion al playground
        String host = "127.0.0.1";
        int port = 44444;

        // Usamos try/catch para conocer los errores que se produzcan
        Random random = new Random();
        try {
            // Accesamos el robot y configuramos algunos de sus atributos
            RobotMarxbot rob = new RobotMarxbot( "Marxbot-01", host, port );

            // Loop clasico
            long t = System.currentTimeMillis() / 1000;
            while( System.currentTimeMillis() / 1000 - t < 20 ) {
                rob.setSpeed( random.nextInt(200)-100, random.nextInt(200)-100 );

                rob.getSensors();

                System.out.print( "pos: " );
                System.out.println( Arrays.toString( rob.getPos() ) );

                System.out.print( "speed: " );
                System.out.println( Arrays.toString( rob.getSpeed() ) );

                System.out.print( "angle: " );
                System.out.println( rob.getAngle() );

                System.out.print( "virtualBumpers: " );
                System.out.println( Arrays.toString( rob.getVirtualBumpers() ) );


                Thread.sleep( 1000 );
            }

            rob.setSpeed( 0, 0 );
            rob.close();
        }
        catch( Exception e ) {
            System.out.println( e );
        }
    }

    public static void main( String[] args ) {
        TestMarxbot test = new TestMarxbot();
        test.run();
    }

}
