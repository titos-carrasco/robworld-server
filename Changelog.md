# ChangeLog

## @v1.2.1 - XXX

### Changed
- Renombra archivos .h a .hpp

### Added
- Agrega (en desarrollo) websocket de manera basica para soportar snap o scratch


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

