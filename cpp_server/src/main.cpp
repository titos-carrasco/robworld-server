#include <QApplication>

#include "MyPlayground.h"

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
            managedObjectsAliases[&typeid(RobotEPuck)] = &typeid(Enki::EPuck);
            managedObjectsAliases[&typeid(RobotThymio2)] = &typeid(Enki::Thymio2);
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
        std::cout << "Invocar como: MyPlayground archivo.world " << std::endl;
        return -1;
    }

    // requerido por QT5
    QApplication app( argc, argv );

    // preparamos el mundo segun el archivo world
    MyPlayground* mpg;
    try
    {
        mpg = new MyPlayground( argv[1] );
    }
    catch( std::exception& e)
    {
        std::cout << ">> ";
        std::cout << e.what() << std::endl;
        exit( -1 );
    }
    catch( ... )
    {
        std::cout << ">> No se pudo configurar el 'mundo'" << std::endl;
        exit( -1 );
    }

    // preparamos la ventana de despliegue
    Viewer* viewer = new Viewer( "MyPlayground", mpg->getWorld(), mpg->getWalls() );

    // show time
    viewer->show();
    mpg->run();
    int exit_code = app.exec();

    // eso es todo
    mpg->stop();
    delete mpg;
    //delete viewer;        // Segmentation fault

    std::cout << ">> Hasta la vista" << std::endl;
    return exit_code;
}
