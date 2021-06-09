package rcr.robworld;

import java.util.Map;
import java.util.ArrayList;
import javax.json.*;

public class RobotThymio2 extends RobotBase {
    private double[] groundSensorValues = null;

    public RobotThymio2( String name, String host, int port ) throws Exception {
        super( name, host, port, "thymio2" );
    }

    public void getSensors()  throws Exception {
        JsonObject json = retrieveSensors();
        groundSensorValues = getDoubleArray( json.getJsonArray( "groundSensorValues" ) );
    }

    public double[] getGroundSensorValues() {
        return groundSensorValues;
    }

    public void setLedsIntensity( double[] leds ) throws Exception {
        JsonArrayBuilder jleds = Json.createArrayBuilder();
        for( double led : leds )
            jleds.add( led );

        JsonObject jobj = Json.createObjectBuilder()
            .add( "cmd", "setLedsIntensity" )
            .add( "leds", jleds.build() )
            .build();

        String resp = sendCommand( jobj, true );
    }
}
