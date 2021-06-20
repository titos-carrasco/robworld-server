#include <QApplication>
#include <viewer/Viewer.h>
#include "RobotWorld.hpp"
#include "RobotThymio2.hpp"
#include "RobotEPuck.hpp"
#include "RobotMarxbot.hpp"

class Viewer : public Enki::ViewerWidget
{
    public:
        Viewer( std::string title, Enki::World* world, double walls ) :
            ViewerWidget( world )
        {
            // título de la ventana
            setWindowTitle( title.c_str() );

            // posición de a camara
            camera.pos.setX( world->w*0.5 );
            camera.pos.setY( -world->h*0.3 );
            camera.altitude = world->h*0.9;
            camera.yaw = 0*(M_PI/180.0);
            camera.pitch = -55*(M_PI/180.0);

            // alto de los muros
            wallsHeight = walls;

            // para desplegar la "piel" de los robots
            managedObjectsAliases[&typeid(RobWorld::RobotEPuck)] = &typeid(Enki::EPuck);
            managedObjectsAliases[&typeid(RobWorld::RobotThymio2)] = &typeid(Enki::Thymio2);
            managedObjectsAliases[&typeid(RobWorld::RobotMarxbot)] = &typeid(Enki::Marxbot);
        }

        ~Viewer()
        {
        }
};

int main(int argc, char* argv[])
{
    // el únio parámetro debe ser el archivo world
    if( argc != 2 )
    {
        std::cout << "Invocar como: robworld archivo.world " << std::endl;
        return -1;
    }

    // requerido por QT5
    QApplication app( argc, argv );

    // preparamos el mundo segun el archivo world
    RobWorld::RobotWorld* mpg;
    try
    {
        mpg = new RobWorld::RobotWorld( argv[1] );
    }
    catch( std::exception& e)
    {
        std::cout << ">> " << std::flush;
        std::cout << e.what() << std::endl;
        return -1;
    }
    catch( ... )
    {
        std::cout << ">> No se pudo configurar el 'mundo'" << std::endl;
        return -1;
    }

    // preparamos la ventana de despliegue
    Viewer* viewer = new Viewer( "Robot World", mpg->getWorld(), mpg->getWalls() );

    // show time
    int exit_code = -1;
    try
    {
        viewer->show();
        mpg->run();
        exit_code = app.exec();

        // eso es todo
        mpg->stop();
        delete mpg;
        //delete viewer;        // Segmentation fault
    }catch( ... )
    {
        std::cout << ">> Ooops, esto no deberia haber pasado!!!" << std::endl;
    }


    std::cout << ">> Hasta la vista" << std::endl;
    return exit_code;
}
