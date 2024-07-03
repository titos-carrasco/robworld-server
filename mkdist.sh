case "$1" in
    32) ;;
    64) ;;
    *) exit;;
esac

mingw=mingw$1
dist=dist$1
build=build$1

rm -fr $dist
mkdir $dist
cp cpp_server/$build/bin/robworld.exe $dist

files=`ntldd -R $dist/robworld.exe | grep mingw | sed -e "s/.* => //" | sed -e "s/ .*$//"`
for f in $files
do
    cp "$f" "$dist"
done

cp -a /$mingw/share/qt5/plugins/imageformats/ $dist
cp -a /$mingw/share/qt5/plugins/platforms/ $dist
cp -a /$mingw/share/qt5/plugins/styles/ $dist

