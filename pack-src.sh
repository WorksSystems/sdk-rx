#!/bin/bash
DB=sdk-rx-src
mkdir $DB

cp -a build examples inc src $DB
cp -a arch.png MAINPAGE.md main.md doxygen.conf $DB
cp -a pack.sh $DB
if [ "`which grip`" ] ; then
grip build-sdk.md --export
fi
if [ -e build-sdk.html ] ; then
mv -v build-sdk.html $DB
else
cp -a build-sdk.md $DB
fi
find $DB -name *.git* -exec rm -v \{\} \;

tar zcf $DB.tgz $DB

rm -rf $DB
