#include "RobotWorld.hpp"
#include "RobotThymio2.hpp"
#include "RobotEPuck.hpp"
#include "RobotMarxbot.hpp"

#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>

#include <viewer/Viewer.h>

#include <fstream>

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
    // La aplicacion QT
    QApplication app( argc, argv );

    // necesitamnos el archivo que define el mundo de robots
    std::string fname;
    if( argc == 1 )
    {
        QString fn = QFileDialog::getOpenFileName( nullptr, "Abrir Archivo Mundo", "./", "Archivos world (*.world)" );
        QByteArray ba = fn.toLocal8Bit();
        fname = std::string( ba.data() );
    }
    else if( argc == 2 )
    {
        fname = std::string( argv[1] );
    }
    else
    {
        std::cout << "Invocar como: robworld [archivo.world]" << std::endl;
        return -1;
    }

    std::ifstream f( fname );
    if( !f.good() )
    {
        std::cout << "Archivo 'world' es invalido" << std::endl;
        return -1;
    }
    f.close();
    // preparamos el mundo segun el archivo world
    RobWorld::RobotWorld* mpg;
    try
    {
        mpg = new RobWorld::RobotWorld( fname );
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
    }
    catch( ... )
    {
        std::cout << ">> Ooops, esto no deberia haber pasado!!!" << std::endl;
    }


    std::cout << ">> Hasta la vista" << std::endl;
    return exit_code;
}
