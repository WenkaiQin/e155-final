#:gcc -Wall -g readInputCoordinate.c -lzmq -o readInputCoordinate.c

sudo mv ~/Documents/e155/e155-final/readInputCoordinate /usr/lib/cgi-bin/readInputCoordinate
#sudo mv ~/Documents/e155/lab6/LEDOFF /usr/lib/cgi-bin/LEDOFF
#sudo mv ~/Documents/e155/lab6/lab6 /usr/lib/cgi-bin/lab6

sudo chown root:www-data /usr/lib/cgi-bin/readInputCoordinate
sudo chmod 010 /usr/lib/cgi-bin/readInputCoordinate
sudo chmod u+s /usr/lib/cgi-bin/readInputCoordinate

