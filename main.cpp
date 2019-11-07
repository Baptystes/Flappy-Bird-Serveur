//Par Baptystes
//Youtube : https://www.youtube.com/user/Baptystes
//© Baptystes 2014

#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include "Structures.h"
#include <iostream>
#include <vector>
using namespace sf;
using namespace std;

#include <ctime>

int main()
{
    srand(0);
    Echange echange;



    sf::Thread thread_AttenteDeClient (&Echange::clientaAccepter, &echange);

    thread_AttenteDeClient.launch();
    while (!(Keyboard::isKeyPressed(Keyboard::LControl) && Keyboard::isKeyPressed(Keyboard::RControl)))
        {
            //cout <<"\n\nRecois";
            echange.donnees_Reception();
            //cout <<"\nEnvoi";
            echange.donnees_Envoi();
        }

    thread_AttenteDeClient.terminate();
}
