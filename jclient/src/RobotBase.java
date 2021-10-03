package rcr.robworld;

import java.util.Map;
import java.util.ArrayList;
import java.math.BigDecimal;

import java.net.Socket;
import java.io.OutputStream;
import java.io.StringReader;
import javax.json.*;

import java.io.PrintWriter;
import java.io.BufferedReader;
import java.io.InputStreamReader;


public class RobotBase {
    private String name;
    private String host;
    private int port;
    private String tipo;
    private Socket sock;
    protected PrintWriter out;
    protected BufferedReader in;

    private double[] pos = null;
    private double[] speed = null;
    private double angle = 0;

    public RobotBase( String name, String host, int port, String tipo ) throws Exception {
        this.host = host;
        this.port = port;
        this.name = name;
        this.tipo = tipo;

        // nos conectamos al simulador
        sock = new Socket( host, port );
        out = new PrintWriter( sock.getOutputStream(), true );
        in = new BufferedReader( new InputStreamReader( sock.getInputStream(), "iso-8859-1" ) );

        // solicitamos acceso enviando el nombre del robot
        JsonObject jobj = Json.createObjectBuilder()
            .add( "connect", name )
            .build();
        String resp = sendCommand( jobj, true );

        // si es aceptado nos envia el tipo de robot que somos
        JsonReader jsonReader = Json.createReader( new StringReader( resp ) );
        JsonObject json = jsonReader.readObject();
        jsonReader.close();

        if( !this.tipo.equals( json.getString( "type" ) ) )
            throw new Exception( "Robot no aceptado" );
    }

    public void close() {
        try {
            if( sock != null ) {
                out.close();
                in.close();
                sock.close();
            }
        }
        catch( Exception e ) {
        }
    }

    public String getName() {
        return name;
    }

    public double[] getPos() {
        return pos;
    }

    public double[] getSpeed() {
        return speed;
    }

    public double getAngle() {
        return angle;
    }

    public void setSpeed( int leftSpeed, int rightSpeed ) throws Exception {
        JsonObject jobj = Json.createObjectBuilder()
            .add( "cmd", "setSpeed" )
            .add( "leftSpeed", leftSpeed )
            .add( "rightSpeed", rightSpeed )
            .build();

        String resp = sendCommand( jobj, true );
    }

    protected JsonObject retrieveSensors() throws Exception {
        JsonObject jobj = Json.createObjectBuilder()
            .add( "cmd", "getSensors" )
            .build();

        String resp = sendCommand( jobj, true );
        JsonReader jsonReader = Json.createReader( new StringReader( resp ) );
        JsonObject json = jsonReader.readObject();
        jsonReader.close();

        speed = getDoubleArray( json.getJsonArray( "speed" ) );
        pos = getDoubleArray( json.getJsonArray( "pos" ) );
        angle = json.getJsonNumber( "angle" ).doubleValue();

        return json;
    }

    protected String sendCommand( JsonObject jobj, boolean response ) throws Exception {
        out.print( jobj );
        out.print( '\n' );
        out.flush();

       return response ? in.readLine() : null;
    }

    protected double[] getDoubleArray( JsonArray arr ) {
        int len = arr.size();
        double[] f = new double[ len ];

        for( int i=0; i<len; i++ ) {
            f[i] = ( arr.getJsonNumber(i) ).doubleValue();
        }

        return f;
    }
}
