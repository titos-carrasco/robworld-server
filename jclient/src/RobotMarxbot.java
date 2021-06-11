package rcr.robworld;

import java.util.Map;
import java.util.ArrayList;
import javax.json.*;

public class RobotMarxbot extends RobotBase {
    private double[] virtualBumpers = null;

    public RobotMarxbot( String name, String host, int port ) throws Exception {
        super( name, host, port, "marxbot" );
    }

    public void getSensors()  throws Exception {
        JsonObject json = retrieveSensors();

        virtualBumpers = getDoubleArray( json.getJsonArray( "virtualBumpers" ) );
    }

    public double[] getVirtualBumpers() {
        return virtualBumpers;
    }
}
