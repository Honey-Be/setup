# (c) 2003-2004 Vajna Miklos <vmiklos@frugalware.org>
# (c)           Patrick J. Volkerding, <volkerdi@slackware.com>

setup="Setup"

# greeting section
welcome="Witaj w instalatorze Frugalware $osver"
greetstring="Witaj w�r�d u�ytkownik�w Frugalware!\n\n \
Tworz�c Frugalware stawiali�my na prostot� i �atw� obs�ug�. Mamy nadziej� �e nasz produkt spodoba ci si�.\n\n \
Tw�rcy Frugalware"

# keyboard section
kbdconf="Konfiguracja Klawiatury"
selmapt="Wybierz zestaw znak�w klawiatury"
selmapd="Mo�esz wybra� map� znak�w dla swojej klawiatury. Je�eli chcesz mie� polsk� klawiatur� wybierz pl2 lub pl"

# hotplug section
hotplug="Wykrywanie sprz�tu"
hotplugt="Prosz� czeka�"
hotplugd="Szukam kart SCSI i PCI"

# selecting media section
searchmedia="Wybieram �r�d�o instalacji"
scanmediat="Przeszukuj�"
scanmediad="Szukam CD/DVD instalacyjnego Frugalware..."
mediafoundt="No�nik CD/DVD znaleziony"
mediafoundd="No�nik instalacyjny znaleziony w "

# partitioning section
partitioning="Tworz� partycj�"
createraid="Tworz� RAID"
selppt="Wybierz program do partycjonowania"
selppd="Z jakiego programu chcesz skorzysta�:"
selhdt="Wybierz dysk do partycjonowania"
selhdd="Wybierz dysk do partycjonowania, dost�pne dyski to:"
fdiskd="Tradycyjny program do partycjonowania"
cfdiskd="�atwiejsza w obs�udz� quasi-graficzna wersja fdisk"

# swap section
setswap="Wyb�r partycji SWAP"
noswaptitle="Nie znaleziono partycji swap"
noswap="Nie stworzy�e� partycji SWAP. Czy chcesz kontynuowa�?"
detectedswapt="Znaleziono partycje SWAP"
detectedswapd="Wybierz partycj� SWAP dla Frugalware:"
formatpartst="Formatowanie partycji SWAP"
formatpartsd1="Formatuj�"
formatpartsd2="jako SWAP i sprwadzam na obecno�� uszkodzonych blok�w"

# rootdev section
rootdevstring="Wyb�r partycji g��wnej"
miscdevstring="Wyb�r partycji"
selrootdevt="Wybierz partycj� g��wn� /"
select="Wybierz"
continue="Kontuuj"
selrootdevd="Wybierz partycj� g��wn� (/) z poni�szej listy."
formatpart="Formatowanie Partycji"
wantformatq="Je�eli partycja nie by�a formatowana to powiniene� zrobi� to teraz\nFormatowanie usunie z niej wszystkie dane. Kontynuowa�?"
formatt="Formatuj"
fromatd="Szybkie formatowanie"
checkt="Sprawd�"
checkd="Wolne formatowanie z poszukiwaniem uszkodzonych blok�w"
nofromatt="Nie"
noformatd="Nie formatuj partycji"
ext2predesc="Ext2 to podstawowy system plik�w linuxa.\n"
reiserpredesc="ReiserFS to nowoczesny system plik�w z ksi�gowaniem.\n"
ext3predesc="Ext3 to system ext2 z ksi�gowaniem.\n"
jfspredesc="JFS to system plik�w z ksi�gowaniem IBMa u�ywany na jego maszynach.\n"
xfspredesc="XFS to system plik�w z ksi�gowaniem SGI.\n"
selectfst="Wyb�r system plik�w dla"
selectfsd="Wybierz system plik�w:\n"
ext2shortdesc="Standardowy system ext2"
reisershortdesc="System z ksi�gowaniem Reisera"
ext3shortdesc="Pospolity system plik�w z ksi�gowaniem"
jfsshortdesc="system IBMa"
xfsshortdesc="system SGI"
duringformatt="Formatuj�"
formatdevicestring="Formatuj� partycj�:"
formatfsstring="System plik�w:"

# linux partitions section

sellinuxpartst="Wybierz inne partycje dla /etc/fstab"
sellinuxpartsd="Chyba masz wi�cej dost�pnych partycji. Mo�esz montowa� je np. jako /home czy te� wybra� zwyk�e montowanie w /mnt. Je�eli nie chcesz nic ustawia� albo ju� to zrobi�e� przejd� dalej poprzez: <$continue>"
inuse="w u�yciu"
askwherelt="Wybierz punkt montowania dla"
askwhereld="Podaj punkt montowania, np. /usr/local lub /mnt/hdaX"

# dos partitions section

seldospartst="Dodanie partycji do /etc/fstab"
seldospartsd="By partycje te by�y widoczne w systemie musz� by� dodane do /etc/fstab. Wybierz interesuj�ce ci� partycje i ustaw punkty montowania.\n\nJe�eli nie chcesz ni� zmienia�, lub ju� to zrobi�e� przejd� dalej poprzez: <$continue>"

# packages section
mirrorconfstring="Wybierz mirror"
mirrorconft="Wybierz mirror pakiet�w"
mirrorconfd="Mo�esz wybra� najbli�szy tobie serwer, co przy�pieszy instalacj�."
categorychk="Szukam kategorii, prosz� czeka�..."
pkgchk="Szukam pakiet�w. Prosz� czeka�..."
categorystring="Wybieram kategori�"
pkgstring="Wybieram pakiety"
instpkgstring="Instalowanie pakiet�w"
sect="grupa" #will be displaied like this: Installing packages (base section)
pleaseselectcategories="Wybierz kategorie pakiet�w do instalacji"
pleaseselectpkgs="Wybierz pakiety do instalacji"

neednextt="Umie�� nast�pny no�nik instalacyjny"
neednextd="Umie�� nast�pny no�nik i wci�nij Enter."
continued="Instaluj pakiety z kolejnego no�nika"
quit="Wyjd�"
quitd="Przerwij instalacj� pakiet�w"

instpkg="Instaluj� wybrane pakiety"
doneinstpkg="Zainstalowano wszystkie pakiety"
errinstpkg="B��dy przy instalacji pakiet�w"
pressenter="Wci�nij Enter by kontynuowa�..."

# configure section
confstring="Konfiguracja zainstalowanego systemu"
confkmodt="Konfiguracja modu��w kernela"
confkmodd="Aktualizacja zale�no�ci modu��w..."

nopasswdt="Brak has�a roota"
nopasswdd="Czy ustawi� has�o roota (ZALECANE)?"

nonormalusert="Brak zwyk�ego u�ytkownika"
nonormaluserd="Obecnie istnieje tylko root. Zaleca si� utworzenie i korzystanie z konta zwyk�ego u�ytkownika. Utworzy� konto zwyk�ego u�ytkownika ?"

endsetupt="Instalacja zako�czona"
erebootd="Wybierz Nie - dostaniesz konsol�, wybierz Tak - restart systemu"
endsetupd="Instalacja Frugalware $osver zako�czona. $erebootd"
ferrort="B��d instalacji"
ferrord="Wyst�pi� b��d krytyczny w czasie instalacji. $erebootd"

# vim: ft=sh
