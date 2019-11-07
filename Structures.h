//Par Baptystes
//Youtube : https://www.youtube.com/user/Baptystes
//© Baptystes 2014

#ifndef STRUCTURES_H
#define STRUCTURES_H
#include <SFML/Network.hpp>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>

typedef struct StructureEchange StructureEchange;
struct StructureEchange
{
    int multiple, pixelEnCours, flappy_Hauteur, flappy_orientation, score;
    std::string nomJoueur;
    bool avecTopTrois;
};

typedef struct StructureTopTrois StructureTopTrois;
struct StructureTopTrois
{
    std::string nom; int score;
};

typedef struct StructureJoueurScore StructureJoueurScore;
struct StructureJoueurScore
{
    std::string nom, motDePasse; int score; int numeroDuClient;
};

inline sf::Packet& operator <<(sf::Packet& packet, const StructureEchange& structureRecois)
{
    return packet << structureRecois.multiple << structureRecois.pixelEnCours << structureRecois.flappy_Hauteur << structureRecois.flappy_orientation << structureRecois.nomJoueur << structureRecois.score;
}

inline sf::Packet& operator >>(sf::Packet& packet, StructureEchange& structureRecois)
{
    //return packet >> structureRecois.multiple >> structureRecois.pixelEnCours >> structureRecois.flappy_Hauteur >> structureRecois.flappy_orientation >> structureRecois.nomJoueur >> structureRecois.score;
    return packet >> structureRecois.multiple >> structureRecois.pixelEnCours >> structureRecois.flappy_Hauteur >> structureRecois.flappy_orientation >> structureRecois.score;
}
///////
//////////////////////////////////
inline sf::Packet& operator <<(sf::Packet& packet, const StructureTopTrois& structureRecois)
{
    return packet << structureRecois.nom << structureRecois.score;
}

inline sf::Packet& operator >>(sf::Packet& packet, StructureTopTrois& structureRecois)
{
    return packet >> structureRecois.nom >> structureRecois.score;
}


class Echange{
public:
    Echange ();
    ~Echange ();
    //void sendStructJeu (StructureEchange &structure);
    //void recoisStructJeu ();
    void clientaAccepter ();

    int *getTuyaux () {return tuyaux;}

    void expulser(int numeroClient, bool enleverClient=1);
    void donnees_Envoi ();
    void donnees_Reception ();

    void affInfos (const StructureEchange &structure);

    void makeTopTrois (std::string nom, int score);

protected:
    sf::TcpListener listener;
    sf::TcpSocket clientSocketType;
    std::vector <sf::TcpSocket*> clientSocket;
    std::vector <StructureEchange*> clientSauvegarde;
    std::vector <StructureJoueurScore*> joueursSurLaListe;
   //std::vector <StructureJoueurScore*> joueurEnLigne;


    sf::Packet packet;


    int tuyaux[100];
    StructureTopTrois meilleurJoueur[3];
    int nombreDeClient;

    FILE *fichier;



};



#endif // STRUCTURES_H
