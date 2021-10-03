import rcr.robworld.RobotThymio2;

import java.util.Random;

// Requiere servidor en el playground "enjambre.world"
public class TestEnjambre {

    public void run() {
        // Los datos de conexion al playground
        String host = "127.0.0.1";
        int port = 44444;

        // Usamos try/catch para conocer los errores que se produzcan
        Random random = new Random();
        try {
            // Accesamos el robot y configuramos algunos de sus atributos
            RobotThymio2[] robots = new RobotThymio2[36];
            for(int i=0; i<36; i++ )
                robots[i] = new RobotThymio2( "Thymio-" + ( 44444 + i ), host, port );

            // Loop clasico
            long t = System.currentTimeMillis() / 1000;
            while( System.currentTimeMillis() / 1000 - t < 20 ) {
                for(int i=0; i<36; i++ )
                    robots[i].setSpeed( random.nextInt(2000)-1000, random.nextInt(2000)-1000 );
                Thread.sleep( 2000 );
            }

            for(int i= 0; i<36; i++ ) {
                robots[i].setSpeed( 0, 0 );
                robots[i].close();
            }
        }
        catch( Exception e ) {
            System.out.println( e );
        }
    }

    public static void main( String[] args ) {
        TestEnjambre test = new TestEnjambre();
        test.run();
    }
}
