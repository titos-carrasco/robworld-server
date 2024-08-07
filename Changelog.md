# ChangeLog

## @v1.1.6.1 2024-07-03

### Changed
- Fija resolución de la ventana principal al cargar el mundo de robots
- Crea procedimiento para generar el ejecutable con sus dependencias
- Actualiza README para documentyar todo el proceso de generación del ejecutable
- Mehora CMakeLists.txt


## @v1.1.6 2021-10-07

### Removed
- Cliente Java es movido a otro repositorio
- Cliente Python es movido a otro repositorio
- Directorio SNAP es eliminado del repositorio

### Fixed
- Convierte imagen de fondo a RGB32 para asegurar consistencia con cualquier formato de imagen recibido

### Changed
- Directorio de mundos de prueba se lleva a la raiz del respositorio


## 2021-10-05

### Changed
- Reduce los dpi de las imagenes en los "worlds" de prueba para minimizar uso de memoria (18dpi es suficiente)
- Elimina conversión de imagen de fondo a través de QGLWidget::convertToGLFormat (no es requerido)


## @v1.1.5 - 2021-10-04

### Added
- Ángulo actual de orientación del robot se incluye en los sensores retornados

### Changed
- Corrige uso de la ruta ruta del archivo 'ground' (el la definición del mundo) cuando presenta caracteres especiales
- Modifica librerías y demos para incoporar el atributo 'angle' retornado con los sensores


## @v1.1.4 - 2021-09-29

### Changed
- Corrige uso del nombre del archivo y ruta del archivo 'world' cuando presenta caracteres especiales


## @v1.1.4 - 2021-09-23

### Added
- Cuadro de dialogo para elegir archivo 'world' al ejecutar el programa


## @v1.1.3 - 2021-06-29

### Fixed
- Orden de los include para WIN32
- Fataba definir MSG_NOSIGNAL para WIN32
- Corrige cast requerido por WIN32
- Corrige error en el total de leds recibidos para el robot Thymio2
- Corrige bloques para SNAP ahora construidos sin JS

### Added
- Agrega proxy HTTP para SNAP
- Corrige bloques de SNAP
- Agrega Test en Python para [recorrer laberinto](https://stackoverflow.com/questions/66942322/wall-follower-algorithm-in-prolog)
- Agrega varios Test para snap


## @v1.1.2 - 2021-06-29

### Fixed
- Agrega detección de error en envio de tipo de robot

### Changed
- Renombra archivos .h a .hpp
- Reordena código

### Added
- Agrega (en desarrollo) websocket de manera basica para soportar snap o scratch
- Agrega HTTP GET para soportar snap
- Agrega cliente y demo para [SNAP](https://snap.berkeley.edu/)


## @v1.1.1 - 2021-06-15

### Changed
- Renombramos parámetros l1 y l2 de la definición del elemento BOX
- Cambiamos interpretación de origen (x,y) en componente BOX. Pasa de (center, center) a (left, bottom)


## @v1.1.0 - 2021-06-12

### Changed
- Migramos a jsoncpp
- Corrige primera linea del protocolo para manejarla en formato json


## @v1.0.1 2021-06-11

### Added
- Agregar robot tipo MarxBot

### Fixed
- Corrige timeout en socket


## @@ - 2021-06-09

### Fixed
- Ajusta nombre del proyecto a robworld en varios archivos
- Ignora SIGPIPE al escribir sobre un socket cerrado
- NON BLOCKING socket genera problemas de rendimiento

### Changed
- README.md: instrucciones para generar e instalar la librería para python

### Added
- Archivo ChangeLog
- Cliente para python
- Varios Test en python

