set -e
rm -rf  package
source version.sh

#AMD64
mkdir -p package/riaps-timesync-amd64/DEBIAN
mkdir -p package/riaps-timesync-amd64/usr/local/lib/
mkdir -p package/riaps-timesync-amd64/usr/arm-linux-gnueabihf/lib/
cp DEBIAN/riaps-timesync-amd64.control package/riaps-timesync-amd64/DEBIAN/control
cp DEBIAN/postinst package/riaps-timesync-amd64/DEBIAN/postinst
cp DEBIAN/prerm package/riaps-timesync-amd64/DEBIAN/prerm
cp DEBIAN/postrm package/riaps-timesync-amd64/DEBIAN/postrm
cp -r amd64-opt/* package/riaps-timesync-amd64/.
cp -r armhf-opt/usr/arm-linux-gnueabihf/* package/riaps-timesync-amd64/usr/arm-linux-gnueabihf/.
sed s/@version@/$timesyncversion/g -i package/riaps-timesync-amd64/DEBIAN/control
fakeroot dpkg-deb --build package/riaps-timesync-amd64/

#ARMHF
mkdir -p package/riaps-timesync-armhf/DEBIAN
mkdir -p package/riaps-timesync-armhf/usr/local/lib/
cp DEBIAN/riaps-timesync-armhf.control package/riaps-timesync-armhf/DEBIAN/control
cp DEBIAN/postinst package/riaps-timesync-armhf/DEBIAN/postinst
cp DEBIAN/prerm package/riaps-timesync-armhf/DEBIAN/prerm
cp DEBIAN/postrm package/riaps-timesync-armhf/DEBIAN/postrm
cp -r armhf-opt/usr/local/* package/riaps-timesync-armhf/usr/local/.
cp -r armhf-opt/usr/arm-linux-gnueabihf/* package/riaps-timesync-armhf/usr/local/.
sed s/@version@/$timesyncversion/g -i package/riaps-timesync-armhf/DEBIAN/control
fakeroot dpkg-deb --build package/riaps-timesync-armhf/

#ARM64
mkdir -p package/riaps-timesync-arm64/DEBIAN
mkdir -p package/riaps-timesync-arm64/usr/local/lib/
cp DEBIAN/riaps-timesync-arm64.control package/riaps-timesync-arm64/DEBIAN/control
cp DEBIAN/postinst package/riaps-timesync-arm64/DEBIAN/postinst
cp DEBIAN/prerm package/riaps-timesync-arm64/DEBIAN/prerm
cp DEBIAN/postrm package/riaps-timesync-arm64/DEBIAN/postrm
cp -r arm64-opt/* package/riaps-timesync-arm64/.
sed s/@version@/$timesyncversion/g -i package/riaps-timesync-arm64/DEBIAN/control
fakeroot dpkg-deb --build package/riaps-timesync-arm64/
