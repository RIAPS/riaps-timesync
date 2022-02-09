set -e
rm -rf  package
source version.sh

#AMD64
mkdir -p package/riaps-timesync-amd64/DEBIAN
mkdir -p package/riaps-timesync-amd64/usr/local/lib/
mkdir -p package/riaps-timesync-amd64/usr/arm-linux-gnueabihf/
mkdir -p package/riaps-timesync-amd64/usr/aarch64-linux-gnu/
mkdir -p package/riaps-timesync-amd64/etc/
cp DEBIAN/riaps-timesync-amd64.control package/riaps-timesync-amd64/DEBIAN/control
cp DEBIAN/postinst package/riaps-timesync-amd64/DEBIAN/postinst
cp DEBIAN/prerm package/riaps-timesync-amd64/DEBIAN/prerm
cp DEBIAN/postrm package/riaps-timesync-amd64/DEBIAN/postrm
cp -r timesync/config/common/timesyncd.service package/riaps-timesync-amd64/etc/.
cp -r amd64-opt/* package/riaps-timesync-amd64/.
cp -r armhf-opt/usr/arm-linux-gnueabihf/* package/riaps-timesync-amd64/usr/arm-linux-gnueabihf/.
cp -r arm64-opt/usr/aarch64-linux-gnu/* package/riaps-timesync-amd64/usr/aarch64-linux-gnu/.
sed s/@version@/$timesyncversion/g -i package/riaps-timesync-amd64/DEBIAN/control
fakeroot dpkg-deb --build package/riaps-timesync-amd64/

#ARMHF
mkdir -p package/riaps-timesync-armhf/DEBIAN
mkdir -p package/riaps-timesync-armhf/usr/local/
mkdir -p package/riaps-timesync-armhf/etc/
cp DEBIAN/riaps-timesync-armhf.control package/riaps-timesync-armhf/DEBIAN/control
cp DEBIAN/postinst package/riaps-timesync-armhf/DEBIAN/postinst
cp DEBIAN/prerm package/riaps-timesync-armhf/DEBIAN/prerm
cp DEBIAN/postrm package/riaps-timesync-armhf/DEBIAN/postrm
cp -r timesync/config/common/timesyncd.service package/riaps-timesync-armhf/etc/.
cp -r armhf-opt/usr/local/* package/riaps-timesync-armhf/usr/local/.
cp -r armhf-opt/usr/arm-linux-gnueabihf/* package/riaps-timesync-armhf/usr/local/.
sed s/@version@/$timesyncversion/g -i package/riaps-timesync-armhf/DEBIAN/control
fakeroot dpkg-deb --build package/riaps-timesync-armhf/

#ARM64
mkdir -p package/riaps-timesync-arm64/DEBIAN
mkdir -p package/riaps-timesync-arm64/usr/local/
mkdir -p package/riaps-timesync-arm64/etc/
cp DEBIAN/riaps-timesync-arm64.control package/riaps-timesync-arm64/DEBIAN/control
cp DEBIAN/postinst package/riaps-timesync-arm64/DEBIAN/postinst
cp DEBIAN/prerm package/riaps-timesync-arm64/DEBIAN/prerm
cp DEBIAN/postrm package/riaps-timesync-arm64/DEBIAN/postrm
cp -r timesync/config/common/timesyncd.service package/riaps-timesync-arm64/etc/.
cp -r arm64-opt/usr/local/* package/riaps-timesync-arm64/usr/local/.
cp -r arm64-opt/usr/aarch64-linux-gnu/* package/riaps-timesync-arm64/usr/local/.
sed s/@version@/$timesyncversion/g -i package/riaps-timesync-arm64/DEBIAN/control
fakeroot dpkg-deb --build package/riaps-timesync-arm64/
