# Beach_project
latest version: client/server 1.5
TODO:
* INPUT control (client -esteso- & server-ridotto?-)
* Array di mutex(possibilità di scrittura di due ombrelloni diversi simultaneamente)
* Journaling
* Lista x abbonamenti
* Signal Handler per client
* [Quasi fatto] Integrare funzioni di book
* [Quasi fatto] sincronizzazione thread master e thread worker
* [DONE] Signal Handler server
* [DONE] Far fare Fork e uccidere il padre (server modalita daemon)

gcc -o client client15.c -l pthread
gcc -o server server 15.c -l pthread

