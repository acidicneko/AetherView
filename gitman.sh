#!/bin/bash
install() {
  qmake6
  make -j $(nproc)
  cp AetherView "$GITMAN_BIN"/AetherView
}

uninstall() {
  rm "$GITMAN_BIN"/AetherView
}

update(){
  install
}

if [ $1 = "install" ] ; then 
	install
elif [ $1 = "uninstall" ] ; then
	uninstall
elif [ $1 = "update" ] ; then
	update
else
	echo unknown option
	exit 1
fi
