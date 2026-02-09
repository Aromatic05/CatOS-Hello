# Maintainer: Aromatic <symwww@outlook.com>
pkgname=catos-hello
pkgver=0.1.0
pkgrel=1
pkgdesc="CatOS Hello - welcome application"
arch=('x86_64')
url="https://github.com/Aromatic05/CatOS-Hello"
license=('GPL')
depends=('qt6-base' 'rate-mirrors' 'lshw' 'inxi' 'lsb-release')
makedepends=('cmake' 'gcc' 'make' 'qt6-tools' 'qt5-translations')
source=("${pkgname}::git+$url.git")
sha256sums=('SKIP')

build() {
    cd catos-hello
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
    make -C build
}

package() {
    install -d "$pkgdir/usr/bin"
    # 安装主程序（假定可执行文件输出为 build/CatOS-Hello）
    cd catos-hello
    install -Dm755 scripts/CollectLogs "$pkgdir/usr/bin/CollectLogs"
    install -Dm755 scripts/RunInTerminal "$pkgdir/usr/bin/RunInTerminal"
    if [ -f build/CatOS-Hello ]; then
        install -Dm755 build/CatOS-Hello "$pkgdir/usr/bin/$pkgname"
    fi

    # 安装翻译文件（.qm），优先使用 build 下生成的 qm，然后使用 translations 目录
    install -d "$pkgdir/usr/share/$pkgname/translations"
    if compgen -G "build/*.qm" > /dev/null; then
        for f in build/*.qm; do
            install -Dm644 "$f" "$pkgdir/usr/share/$pkgname/translations/$(basename "$f")"
        done
    fi
    if compgen -G "translations/*.qm" > /dev/null; then
        for f in translations/*.qm; do
            install -Dm644 "$f" "$pkgdir/usr/share/$pkgname/translations/$(basename "$f")"
        done
    fi

    # 安装桌面文件到 /usr/share/applications
    if [ -f "$pkgname.desktop" ]; then
        install -Dm644 "$pkgname.desktop" "$pkgdir/usr/share/applications/$pkgname.desktop"
    fi

    # 将 autostart 桌面文件放到 /etc/skel/.config/autostart，便于新用户使用
    # 使用你提供的路径和模式（注意：PKGBUILD 在项目根目录时 ../$pkgname.desktop 路径请根据实际放置调整）
    install -Dvm644 $pkgname.desktop \
        "$pkgdir/etc/skel/.config/autostart/$pkgname.desktop"
}