Utiliza librería json obtenida desde https://github.com/stleary/JSON-java

Generar la libreria
-------------------
javac -cp "../dist/javax.json-1.1.4.jar:build/" -d "build/" src/*.java
jar -cf ../dist/robworld-x.y.z.jar -C build/ rcr/

