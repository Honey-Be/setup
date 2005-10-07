# (c) 2003-2004 Vajna Miklos <vmiklos@frugalware.org>
# (c)           Patrick J. Volkerding, <volkerdi@slackware.com>

setup="Setup"

# greeting section
welcome="Bienvenido a Frugalware $osver"
greetstring="Bienvenido a la comunidad de usuarios de Frugalware!\n\n \
Nuestro objetivo creando Frugalware, es ayudar a hacer su trabajo m�s simple y r�pido. Esperamos que disfrute nuestro producto.\n\n \
Los Desarrolladores de Frugalware"

# keyboard section
kbdconf="Configuraci�n del teclado"
selmapt="Selecci�n del mapeo del teclado"
selmapd="Debe elegir uno de los siguientes mapeos de teclado.  Si no selecciona un mapeo de teclado, 'qwerty/us.map.gz' (the US keyboard map) se instala por defecto.  Use las teclas UP/DOWN y PageUp/PageDown para desplazarse en la lista de opciones."

# hotplug section
hotplug="Detectando hardware"
hotplugt="Por favor, aguarde"
hotplugd="Buscando dispositivos SCSI y tarjetas PCI"

# selecting media section
searchmedia="Elegir dispositivo de origen"
scanmediat="Buscando"
scanmediad="Buscando un CD/DVD que contenga el disco de instalaci�n de Frugalware (install disc)"
mediafoundt="Se encontr� el CD/DVD"
mediafoundd="El disco de instalaci�n de Frugalware fue encontrado en el dispositivo"

# partitioning section
partitioning="Crear Partici�n"
createraid="Crear RAID"
selppt="Seleccionar programa de particionamiento"
selppd="Elija el programa que desee utilizar para particionar:"
selhdt="Seleccione el disco r�gido a particionar"
selhdd="Por favor, seleccione el disco r�gido a particionar. Se listan los disponibles:"
fdiskd="El programa para particionar tradicional de Linux"
cfdiskd="Versi�n amigable de fdisk (basada en curses)"

# swap section
setswap="Configurando el espacio de intercambio (swap)"
noswaptitle="No se detect� partici�n de intercambio (swap)"
noswap="Usted no ha creado una partici�n de intercambio (swap). �Desea continuar la instalaci�n sin crearla?"
detectedswapt="Se detect� una partici�n de intercambio (swap)"
detectedswapd="Por favor, elija que partici�n de intercambio (swap) utilizar� Frugalware:"
formatpartst="Formateando la partici�n de intercambio (swap)"
formatpartsd1="Formateando"
formatpartsd2="como partici�n de intercambio (swap) y buscando sectores defectuosos"

# rootdev section
rootdevstring="Configure la partici�n ra�z (root)"
miscdevstring="Configurando particiones"
selrootdevt="Seleccione la partici�n de instalaci�n de Linux"
select="Seleccione"
continue="Contin�e"
selrootdevd="Por favor, seleccione una partici�n de la siguiente lista para ser usada como partici�n ra�z (/)"
formatpart="Formateando partici�n"
wantformatq="Si esta partici�n no ha sido formateada, deber� formatearla. \n NOTA: Se borrar� todos los datos en ella �Desea formatear la partici�n?"
formatt="Formateando"
fromatd="Formato r�pido, sin comprobar sectores defectuosos"
checkt="comprobar"
checkd="Formato lento que comprueba sectores defectuosos"
nofromatt="No"
noformatd="No, no formatear la partici�n"
ext2predesc="Ext2 es el sistema de archivos tradicional de Linux. Es r�pido y estable\n"
reiserpredesc="ReiserFS es un sistema de archivos de tipo \"journaling\" que almacena todos los archivos y sus nombres en una estructura de �rbol balanceado\n"
ext3predesc="Ext3 es la versi�n \"journaling\" del sistema de archivos Ext2.\n"
jfspredesc="JFS es un sistema de archivos de tipo \"journaling\" de IBM, actualmente usado en los enterprise servers de IBM.\n"
xfspredesc="XFS es un sistema de archivos de tipo journaling de SGI originado en IRIX.\n"
selectfst="Elegir sistema de archivos para"
selectfsd="Por favor, elija el tipo de sistema de archivos para usar en el dispositivo especificado. Aqu� hay una descripci�n de los sistemas de archivos disponibles:\n"
ext2shortdesc="Sistema de archivos Linux ext2fs, estandar de Linux"
reisershortdesc="Sistema de archivos (journaling)de Hans Reiser"
ext3shortdesc="Versi�n journaling del sistema de archivos ext2fs"
jfsshortdesc="Sistema de archivos(journaled) de IBM"
xfsshortdesc="Sistema de archivos (journaling) de SGI"
duringformatt="Formateando"
formatdevicestring="Formateando el dispositivo:"
formatfsstring="Sistema de archivos:"

# linux partitions section

sellinuxpartst="Seleccione otras particiones Linux para /etc/fstab"
sellinuxpartsd="Parece que tiene m�s de una partici�n de tipo Linux. Puede distribuir su sistema Linux en m�s de una partici�n. Actualmente usted ha montado solamente la partici�n /. Tal vez desee montar directorios como /home o /usr/local en particiones separadas. No debe intentar montar /etc, /sbin, o /bin en sus propias particiones porque contienen utilidades necesarias para el funcionamiento del sistema y para poder montar particiones. Tampoco use particiones que ya ha asignado antes.Por favor, seleccione una de las particiones Linux listadas abajo, o si desea continuar seleccione <$continue>" 
inuse="en uso"
askwherelt="Seleccione punto de montaje para"
askwhereld="Debe especificar el punto de montaje de la nueva partici�n. Por ejemplo, si desea montarla en /usr/local, entonces responda: /usr/local\n\n �Donde desea montar esta partici�n?"

# dos partitions section

seldospartst="Seleccionar partici�n para agregarla a /etc/fstab"
seldospartsd="Para hacer visibles otras particiones desde Linux, necesitamos agregarlas a /etc/fstab. Por favor elija particiones para ser agregadas a /etc/fstab, si desea continuar, seleccione <$continue>"

# packages section
mirrorconfstring="Elegir un servidor mirror"
mirrorconft="Por favor, elija un servidor mirror"
mirrorconfd="Puede especificar un servidor mirror m�s cercano a su ubicaci�n para acelerar el proceso de descarga. En la mayor�a de los casos la opci�n por defecto (default) es una buena opci�n."
categorychk="Buscando categor�as. Por favor, espere..."
pkgchk="Buscando paquetes. Por favor, espere..."
categorystring="Seleccionando categor�as"
pkgstring="Seleccionando paquetes"
instpkgstring="Instalando paquetes"
sect="(secci�n)" #will be displaied like this: Installing packages (base section)
pleaseselectcategories="Por favor, seleccione que categor�as desea instalar"
pleaseselectpkgs="Por favor, seleccione que paquetes desea instalar"

neednextt="Inserte el disco siguiente"
neednextd="Por favor, inserte el siguiente disco de instalaci�n de Frugalware y presione enter para continuar instalando los paquetes."
continued="Instalando paquetes del siguiente disco"
quit="Salir"
quitd="Salir de la instalaci�n de paquetes y terminar"

instpkg="Instalar los paquetes seleccionados"
doneinstpkg="Se complet� la instalaci�n de los paquetes seleccionados"
errinstpkg="Ocurrieron errores durante la instalaci�n de los paquetes"
pressenter="Presione ENTER para continuar..."

# configure section
confstring="Configurando el sistema instalado"
confkmodt="Configurando los m�dulos del kernel"
confkmodd="Actualizando las dependencias de m�dulos..."

nopasswdt="No se defini� la contrase�a (password) de root"
nopasswdd="No existe actualmente una contrase�a para la cuenta del administrador del sistema (root). Se recomienda que la defina ahora as� estar� activa la primera vez que reinicie la computadora. Esto es de especial importancia si usted utiliza un kernel que habilite conexiones de red y si la computadora se conecta a internet �Desea definir una contrase�a para root?"
nonormalusert="No se encontr� una cuenta de usuario (user)"
nonormaluserd="No existe actualmente una cuenta no-root. Se recomienda crear una. �Desea crear una cuenta de usuario ahora?"

endsetupt="Instalaci�n completada"
erebootd="Si desea realizar algo especial, pulse no, y saldr� a consola (shell). Desea reiniciar su computadora ahora?"
endsetupd="Se complet� satisfactoriamente la instalaci�n y configuraci�n del sistema. Esperamos que Frugalware $osver sea de su agrado. $erebootd"
ferrort="Error en la instalaci�n"
ferrord="Ocurri� un error fatal en la instalaci�n. $erebootd"

# vim: ft=sh
