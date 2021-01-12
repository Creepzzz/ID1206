#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* 3.2 Orphans 
int main(){
	
	int pid = fork();
	
	if(pid == 0){
		int child = getpid();
		printf("child:\tparent %d, group %d\n", getppid(), getpgid(child));
		sleep(4);
		printf("child:\tparent %d, group %d (I'm adopted by the upstart process)\n", getppid(), getpgid(child));
		sleep(4);
		printf("child:\tparent %d, group %d (I'm adopted by the upstart process)\n", getppid(), getpgid(child));
		
	} else {
		int mom = getpid();
		printf("mom:\tparent %d in group %d, which I'm the leader of\n", getppid(), getpgid(mom));
		sleep(2);
		int zero = 0;
		int i = 3 / zero;
	}
	
	return 0;
}
*/

/* PRINTOUT:

	mom:	parent 3632 in group 17804, which I'm the leader of
	child:	parent 17804, group 17804
	Floating point exception (core dumped)
	child:	parent 692, group 17804
	child:	parent 692, group 17804
	
$ ps 692
    PID TTY      STAT   TIME COMMAND
    692 ?        Ss     0:01 /lib/systemd/systemd --user

	
$ pstree 692
	systemd─┬─(sd-pam)
		├─at-spi-bus-laun─┬─dbus-daemon
		│                 └─3*[{at-spi-bus-laun}]
		├─at-spi2-registr───2*[{at-spi2-registr}]
		├─dbus-daemon
		├─dconf-service───2*[{dconf-service}]
		├─evolution-addre───5*[{evolution-addre}]
		├─evolution-calen───8*[{evolution-calen}]
		├─evolution-sourc───3*[{evolution-sourc}]
		├─gedit───4*[{gedit}]
		├─gjs───4*[{gjs}]
		├─gnome-session-b─┬─evolution-alarm───5*[{evolution-alarm}]
		│                 ├─gsd-disk-utilit───2*[{gsd-disk-utilit}]
		│                 ├─update-notifier───3*[{update-notifier}]
		│                 └─3*[{gnome-session-b}]
		├─gnome-session-c───{gnome-session-c}
		├─gnome-shell───6*[{gnome-shell}]
		├─gnome-shell-cal───5*[{gnome-shell-cal}]
		├─gnome-terminal-─┬─bash───pstree
		│                 └─4*[{gnome-terminal-}]
		├─goa-daemon───3*[{goa-daemon}]
		├─goa-identity-se───2*[{goa-identity-se}]
		├─gsd-a11y-settin───3*[{gsd-a11y-settin}]
		├─gsd-color───3*[{gsd-color}]
		├─gsd-datetime───3*[{gsd-datetime}]
		├─gsd-housekeepin───3*[{gsd-housekeepin}]
		├─gsd-keyboard───3*[{gsd-keyboard}]
		├─gsd-media-keys───3*[{gsd-media-keys}]
		├─gsd-power───3*[{gsd-power}]
		├─gsd-print-notif───2*[{gsd-print-notif}]
		├─gsd-printer───2*[{gsd-printer}]
		├─gsd-rfkill───2*[{gsd-rfkill}]
		├─gsd-screensaver───2*[{gsd-screensaver}]
		├─gsd-sharing───3*[{gsd-sharing}]
		├─gsd-smartcard───4*[{gsd-smartcard}]
		├─gsd-sound───3*[{gsd-sound}]
		├─gsd-usb-protect───3*[{gsd-usb-protect}]
		├─gsd-wacom───2*[{gsd-wacom}]
		├─gsd-wwan───3*[{gsd-wwan}]
		├─gsd-xsettings───3*[{gsd-xsettings}]
		├─gvfs-afc-volume───3*[{gvfs-afc-volume}]
		├─gvfs-goa-volume───2*[{gvfs-goa-volume}]
		├─gvfs-gphoto2-vo───2*[{gvfs-gphoto2-vo}]
		├─gvfs-mtp-volume───2*[{gvfs-mtp-volume}]
		├─gvfs-udisks2-vo───3*[{gvfs-udisks2-vo}]
		├─gvfsd─┬─gvfsd-trash───2*[{gvfsd-trash}]
		│       └─2*[{gvfsd}]
		├─gvfsd-fuse───5*[{gvfsd-fuse}]
		├─gvfsd-metadata───2*[{gvfsd-metadata}]
		├─ibus-portal───2*[{ibus-portal}]
		├─nautilus───5*[{nautilus}]
		├─pulseaudio───3*[{pulseaudio}]
		├─snap-store───4*[{snap-store}]
		├─tracker-miner-f───4*[{tracker-miner-f}]
		├─xdg-desktop-por───4*[{xdg-desktop-por}]
		├─xdg-desktop-por───3*[{xdg-desktop-por}]
		├─xdg-document-po───5*[{xdg-document-po}]
		└─xdg-permission-───2*[{xdg-permission-}]


*/









/* 3.3 sessions and daemons */
int main(){
	
	int pid = fork();
	
	if(pid == 0){
		int child = getpid();
		printf("child:\tparent %d, group %d, session %d\n", getppid(), getpgid(child), getsid(child));
		
	} else {
		int mom = getpid();
		printf("mom:\tparent %d in group %d, session %d\n", getppid(), getpgid(mom), getsid(mom));
	}
	
	return 0;
}

/* PRINTOUT: 

	mom:	parent 3632 in group 17944, session 3632
	child:	parent 692, group 17944, session 3632

*/
