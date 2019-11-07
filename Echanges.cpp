//Par Baptystes
//Youtube : https://www.youtube.com/user/Baptystes
//© Baptystes 2014

#include <string>
#include <SFML/Network.hpp>
#include "Structures.h"
#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
#include "windows.h"

#define VERSION 1

using namespace sf;
using namespace std;


Echange :: Echange ()
{
    if ((listener.listen(53000) != Socket::Done)){
        cout << "Erreur port\n";
        system("pause");
        exit(-1);
    }
    else{
        cout << "== Started on  | "<< IpAddress::getPublicAddress() <<" |  Quitter : LCtrl+RCtrl ==\nVestion "<<VERSION<<"\n\n";

    }

    clientSocket.empty();
    packet.clear();

    for (int a=0; a<100; a++)
    {
       // tuyaux[a] = a*20;
       tuyaux[a]=rand()%240+45;
    }
    nombreDeClient=0;


//    joueurEnLigne.clear();
    joueursSurLaListe.clear();
    char nomListe[12];
    char motDePasseListe[10];
    if (!(fichier = fopen("data/scores.txt", "r+")))
    {
        fichier = fopen("data/scores.txt", "w");
        fclose(fichier);
        fichier = fopen("data/scores.txt", "r+");
        fputs(" ==  Scores  ==\n&", fichier);
    }
    else
    {
        char caractereEnCours = 0;
        rewind(fichier);
        while ((caractereEnCours=fgetc(fichier)) != '&')
        {
            if (caractereEnCours == '#')
            {
                StructureJoueurScore *nouveauJoueur = new StructureJoueurScore;
                fscanf(fichier, "%s %d %s", nomListe, &(nouveauJoueur->score), motDePasseListe);
                nouveauJoueur->nom = nomListe;
                nouveauJoueur->motDePasse = motDePasseListe;
                //nouveauJoueur->nom = nom;
                cout <<"\n- "<<nouveauJoueur->nom<<", "<<nouveauJoueur->score;
                joueursSurLaListe.push_back(nouveauJoueur);
            }
        }

    }

    fclose (fichier);

    for (int a=0; a<3; a++)
    {
        meilleurJoueur[a].nom = "";
        meilleurJoueur[a].score = 0;
    }
    for (int a=0; a<joueursSurLaListe.size(); a++)
    {
        makeTopTrois(joueursSurLaListe[a]->nom, joueursSurLaListe[a]->score);
    }


}

Echange :: ~Echange ()
{
    fichier = fopen("data/scores.txt", "w+");
    fputs(" ==  Scores  ==", fichier);
    for (int a=0; a<joueursSurLaListe.size(); a++)
        fprintf(fichier, "\n#%s %d %s", joueursSurLaListe[a]->nom.c_str(), joueursSurLaListe[a]->score, joueursSurLaListe[a]->motDePasse.c_str());
    fputs("\n&", fichier);
    fclose(fichier);

    for (int a=0; a<clientSocket.size(); a++)
    {
        delete clientSocket[a];
        delete clientSauvegarde[a];
        delete joueursSurLaListe[a];
    }
}

void Echange :: clientaAccepter ()
{
    Packet packetTemp;
    int connu(0);
    double versionClient (0);


    int a = 0;
    int bestScore = 0;

    while (1)
    {
            TcpSocket *socket = new TcpSocket;
            if (listener.accept(*socket) == Socket::Done)
            {
                StructureEchange* newClient=new StructureEchange;
                newClient->avecTopTrois=0;
                newClient->score = 0;
                newClient->multiple = 0;
                socket->setBlocking(true);




                clientSocket.push_back(socket);
                //clientSocket[nombreDeClient]->setBlocking(true);
                clientSocket[nombreDeClient]->receive(packetTemp);
                StructureJoueurScore *nouveauJoueur = new StructureJoueurScore;

                string motDePasseTemp = "";
                packetTemp >> nouveauJoueur->nom >> versionClient >> motDePasseTemp;
                string nom = nouveauJoueur->nom;
                newClient->nomJoueur = nom;
                clientSauvegarde.push_back(newClient);



                connu = 0;
                a = 0;
                while (connu == 0 && a<joueursSurLaListe.size())
                {
                    //cout <<"\nA="<<a;
                    if (nom == joueursSurLaListe[a]->nom)
                        connu = 1;
                    else
                        a++;
                }

                if (!connu)
                {
                    bestScore = nouveauJoueur->score = 0;                 ///////////////HERE !!!!!!!!!!!!!!!!!!!
                    nouveauJoueur->numeroDuClient = nombreDeClient;
                    nouveauJoueur->motDePasse = motDePasseTemp;
                    joueursSurLaListe.push_back(nouveauJoueur);

                    cout <<"\nNouveau client : ";
                    //cout <<" ("<< joueursSurLaListe[joueursSurLaListe.size()-1]->nom<<") ("<<joueursSurLaListe.size()-1<<") ";

                }
                else
                {
                    cout <<"\nConnection de ";
                    joueursSurLaListe[a]->numeroDuClient = nombreDeClient;
                    bestScore = joueursSurLaListe[a]->score;
                    joueursSurLaListe[a]->nom = nom;
                    //cout <<"2";
                    cout <<" ("<< joueursSurLaListe[a]->nom<<") ";
                    delete nouveauJoueur;
                   // cout <<"4";

                }

                if (versionClient != VERSION || motDePasseTemp != joueursSurLaListe[a]->motDePasse)
                {
                    cout <<nom<<" - Client ejected! (V="<<versionClient << ", Mdp correct="<<(motDePasseTemp != joueursSurLaListe[a]->motDePasse)<<")";
                    expulser(nombreDeClient, 0);
                }
                else
                {
                    packetTemp.clear();
                    for (int a=0; a<100; a++)
                        packetTemp<<tuyaux[a];
                    packetTemp << bestScore;

                    clientSocket[nombreDeClient]->send(packetTemp);
                    cout <<nom<<" | Ip:"<< clientSocket[nombreDeClient]->getRemoteAddress() << "  Online:"<<nombreDeClient+1;

                    packetTemp.clear();
                    for (int a=0; a<3; a++)
                        packetTemp<<meilleurJoueur[a];
                    while (clientSocket[nombreDeClient]->send(packetTemp) != Socket::Done);
                    //cout <<meilleurJoueur[2].nom;


                    nombreDeClient++;

                }
            }
            Sleep(60);
    }
}

void Echange :: expulser (int numeroClient, bool enleverClient)
{
    //cout <<"nombreDeClients : "<<nombreDeClient;
            cout << "\n Supression du client " << numeroClient<<" : "<<clientSauvegarde[numeroClient]->nomJoueur<<"  avec Ip="<<clientSocket[numeroClient]->getRemoteAddress()<<"  restants="<<nombreDeClient-enleverClient;
            clientSocket[numeroClient]->disconnect();
            vector <TcpSocket*> socketBack = clientSocket;
            clientSocket.clear();

            vector <StructureEchange*> clientSauvegardeBack = clientSauvegarde;
            clientSauvegarde.clear();

            //cout << "\nSize= "<<clientSocket.size();
            for (int b=0; b<nombreDeClient; b++)
            {
                if (b != numeroClient)
                {
                    clientSocket.push_back(socketBack[b]);
                    clientSauvegarde.push_back(clientSauvegardeBack[b]);

                }
                else
                {
                    delete clientSauvegardeBack[b];
                    delete socketBack[b];
                }

            }
            nombreDeClient-=enleverClient;


            //delete clientSauvegarde[numeroClient];
            //delete clientSocket[numeroClient];
}

void Echange :: donnees_Envoi ()
{
    for (int clientEnCours=0; clientEnCours<nombreDeClient; clientEnCours++)
    {
        packet.clear();
        packet<<(nombreDeClient-1);
        for (int clientaPartager=0; clientaPartager<nombreDeClient; clientaPartager++)
        {
            if (clientaPartager != clientEnCours)
            {
               packet << *clientSauvegarde[clientaPartager];
            }
        }
        if (clientSauvegarde[clientEnCours]->avecTopTrois ==1)
        {
            clientSauvegarde[clientEnCours]->avecTopTrois = 0;
            packet<< (int) 1;
            for (int a=0; a<3; a++)
                packet << meilleurJoueur[a];
        }
        else
            packet<<0;
        //cout <<"\nPACKET="<<packet.getDataSize();
        clientSocket[clientEnCours]->send(packet);
        //int nombre;
        //packet>>nombre;
        //cout <<"\nn="<<nombre;
    }
}

void Echange :: donnees_Reception ()
{
    Socket::Status status;
    for (int clientRecu=0; clientRecu<nombreDeClient; clientRecu++)
    {
        //cout <<"\BOOOOP";
        status = clientSocket[clientRecu]->receive(packet);
        if (status == Socket :: Disconnected)
        {
                expulser(clientRecu);
        }
        else if (status == Socket::Done)
        {
            StructureEchange *structureToReceive = new StructureEchange;
            packet >> *structureToReceive;
            structureToReceive->nomJoueur = clientSauvegarde[clientRecu]->nomJoueur;
            delete clientSauvegarde[clientRecu];
            clientSauvegarde[clientRecu] = NULL;
            clientSauvegarde[clientRecu] = structureToReceive;
            //clientSauvegarde[clientRecu]->nomJoueur =
            //cout <<"\nSCORE="<<structureToReceive->score;

            if (structureToReceive->score<0)
            {
                int joueurDefilement = 0;
                while (joueursSurLaListe[joueurDefilement]->numeroDuClient != clientRecu  && joueurDefilement < (joueursSurLaListe.size()-1))
                {
                    joueurDefilement++;
                }
                if (joueursSurLaListe[joueurDefilement]->score < (-structureToReceive->score-1))
                    joueursSurLaListe[joueurDefilement]->score = (-structureToReceive->score-1);
                makeTopTrois(structureToReceive->nomJoueur, -structureToReceive->score-1);
                clientSauvegarde[clientRecu]->avecTopTrois = 1;
                //packet.clear();
                //for (int a=0; a<3; a++)
                    /*packet << meilleurJoueur[0];
                clientSocket[clientRecu]->send(packet);*/
               // cout <<"\n===="<<clientRecu;

            }

            //cout <<"\n===== "<< clientRecu<<" =====\n";
            //affInfos (*clientSauvegarde[clientRecu]);

            //cout << "Passed="<<structureos.tuyauxPassed << "  PixelEnCours="<<structureos.pixelEnCours<<"  Hauteur="<<structureos.flappy_Hauteur<<"  Orientation"<<structureos.flappy_orientation<<endl;
        }
    }
}

void Echange :: makeTopTrois(string nom, int score)
{

    if (score > meilleurJoueur[2].score)
    {
        if (meilleurJoueur[2].nom != nom)
        {
            meilleurJoueur[0] = meilleurJoueur[1];
            meilleurJoueur[1] = meilleurJoueur[2];
        }
        meilleurJoueur[2].score = score;
        meilleurJoueur[2].nom = nom;
    }
    else if (score > meilleurJoueur[1].score && meilleurJoueur[2].nom != nom)
    {
        if ( meilleurJoueur[1].nom != nom)
        {
            meilleurJoueur[0] = meilleurJoueur[1];
        }
        meilleurJoueur[1].score = score;
        meilleurJoueur[1].nom = nom;
    }
    else if (score > meilleurJoueur[0].score && meilleurJoueur[1].nom != nom && meilleurJoueur[2].nom != nom)
    {
        meilleurJoueur[0].score = score;
        meilleurJoueur[0].nom = nom;
    }

}





