/**
* \file DicoSynonymes.cpp
* \brief Le code des opérateurs du DicoSynonymes.
* \author IFT-2008, Étudiant(e)
* \version 0.5
* \date avril 2021
*
* Travail pratique numéro 3.
*
*/

#include "DicoSynonymes.h"
#include <vector>
#include <algorithm>

namespace TP3
{

    /**
     * \fn	DicoSynonymes::DicoSynonymes()
     * \brief constructeur par defaut. initialisation de l'attribut racine, nbRadicaux et groupesSynonymes.
     */
    DicoSynonymes::DicoSynonymes()
    {
        racine = nullptr;
        nbRadicaux = 0;
        groupesSynonymes = std::vector<std::list<NoeudDicoSynonymes*>>();
    }

    /**
     * \fn	DicoSynonymes::DicoSynonymes()
     * \brief constructeur avec parametre a partir d'un fichier. Chargement du dictionnaire a partir du fichier en parametre.
     */
    DicoSynonymes::DicoSynonymes(std::ifstream &fichier)
    {
        racine = nullptr;
        nbRadicaux = 0;
        groupesSynonymes = std::vector<std::list<NoeudDicoSynonymes*>>();
        chargerDicoSynonyme(fichier);
    }

    /**
     * \fn	DicoSynonymes::DicoSynonymes()
     * \brief constructeur avec parametre a partir d'un fichier. Chargement du dictionnaire a partir du fichier en parametre.
     */
    DicoSynonymes::~DicoSynonymes()
    {
        _destructeur(racine);
    }

    void DicoSynonymes::ajouterRadical(const std::string& motRadical)
    {
        _auxAjouterRadical(racine, motRadical);
    }

    void DicoSynonymes::ajouterFlexion(const std::string& motRadical, const std::string& motFlexion)
    {
        if(_auxAppartient(racine, motRadical) == nullptr)
            throw std::logic_error("Le Radical n'existe pas!");

        NoeudDicoSynonymes* noeudRadical = _auxAppartient(racine, motRadical);

        for(std::string flexion : noeudRadical->flexions)
        {
            if(flexion == motFlexion)
                throw std::logic_error("La flexion existe deja !");
        }

        noeudRadical->flexions.push_back(motFlexion);
    }

    void DicoSynonymes::ajouterSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe)
    {
        //Verifier si le radical existe
        if(_auxAppartient(racine, motRadical) == nullptr)
            throw std::logic_error("Le Radical n'existe pas!");

        NoeudDicoSynonymes* noeudRadical = _auxAppartient(racine, motRadical);

        if(numGroupe == -1)
        {
            //Creation du nouveau groupe de synonymes
            groupesSynonymes.push_back(std::list<NoeudDicoSynonymes*>());
            numGroupe = groupesSynonymes.size() - 1;
            noeudRadical->appSynonymes.push_back(numGroupe);
        }

        //Verifier si le numGroupe appartient a appSynonyme du radical
        if(std::find(noeudRadical->appSynonymes.begin(),
                     noeudRadical->appSynonymes.end(), numGroupe) != noeudRadical->appSynonymes.end())
        {
            for(NoeudDicoSynonymes* noeud : groupesSynonymes[numGroupe])
                if(noeud->radical == motSynonyme)
                    throw std::logic_error("Le synonyme pour le motRadical est deja present!");

            //Si le synonyme n'est pas present dans larbre, l'ajouter dans l'arbre pour l'ajouter dans le groupe
            if(_auxAppartient(racine, motSynonyme) == nullptr)
                _auxAjouterRadical(racine, motSynonyme);

            NoeudDicoSynonymes* noeudSynonyme = _auxAppartient(racine, motSynonyme);
            groupesSynonymes[numGroupe].push_back(noeudSynonyme);
            groupesSynonymes[numGroupe].push_back(noeudRadical);
            noeudSynonyme->appSynonymes.push_back(numGroupe);
        }
        else{throw std::logic_error("le numGroupe n'est pas present dans appSynonymes du motRadical!");}
    }

    void DicoSynonymes::supprimerRadical(const std::string& motRadical)
    {
        if(_auxAppartient(racine, motRadical) == nullptr)
            throw std::logic_error("Le Radical n'existe pas!");
    }

    void DicoSynonymes::supprimerFlexion(const std::string& motRadical, const std::string& motFlexion)
    {
        if(_auxAppartient(racine, motRadical) == nullptr || nbRadicaux == 0 || motFlexion.empty())
            throw std::logic_error("motRadical ne fait pas partie de l'arbre!");
        _auxAppartient(racine, motRadical)->flexions.remove(motFlexion);
    }

    void DicoSynonymes::supprimerSynonyme(const std::string& motRadical, const std::string& motSynonyme, int& numGroupe)
    {

    }

    bool DicoSynonymes::estVide() const
    {
        return nbRadicaux == 0;
    }

    int DicoSynonymes::nombreRadicaux() const
    {
        return nbRadicaux;
    }


    // Méthode fournie
    void DicoSynonymes::chargerDicoSynonyme(std::ifstream& fichier)
    {
        if (!fichier.is_open())
            throw std::logic_error("DicoSynonymes::chargerDicoSynonyme: Le fichier n'est pas ouvert !");

        std::string ligne;
        std::string buffer;
        std::string radical;
        int cat = 1;

        while (!fichier.eof()) // tant qu'on peut lire dans le fichier
        {
            std::getline(fichier, ligne);

            if (ligne == "$")
            {
                cat = 3;
                std::getline(fichier, ligne);
            }
            if (cat == 1)
            {
                radical = ligne;
                ajouterRadical(radical);
                cat = 2;
            }
            else if (cat == 2)
            {
                std::stringstream ss(ligne);
                while (ss >> buffer)
                    ajouterFlexion(radical, buffer);
                cat = 1;
            }
            else
            {
                std::stringstream ss(ligne);
                ss >> radical;
                ss >> buffer;
                int position = -1;
                ajouterSynonyme(radical, buffer, position);
                while (ss >> buffer)
                    ajouterSynonyme(radical, buffer, position);
            }
        }
    }

    std::string DicoSynonymes::rechercherRadical(const std::string& mot) const
    {
        return "";
    }

    float DicoSynonymes::similitude(const std::string& mot1, const std::string& mot2) const
    {
        return 0;
    }

    int DicoSynonymes::getNombreSens(std::string radical) const
    {
        return 0;
    }

    std::string DicoSynonymes::getSens(std::string radical, int position) const
    {
        return "";
    }

    std::vector<std::string> DicoSynonymes::getSynonymes(std::string radical, int position) const
    {
        return std::vector<std::string>();
    }

    std::vector<std::string> DicoSynonymes::getFlexions(std::string radical) const
    {
        std::vector<std::string> vecFlexion;
        for(std::string const &flexion : _auxAppartient(racine, radical)->flexions)
            vecFlexion.push_back(flexion);
        return vecFlexion;
    }

    void DicoSynonymes::_destructeur(NoeudDicoSynonymes* & noeud)
    {
        if(noeud != nullptr)
        {
            _destructeur(noeud->gauche);
            _destructeur(noeud->droit);
            delete noeud;
        }
    }

    void DicoSynonymes::_auxAjouterRadical(NoeudDicoSynonymes* & noeud, const std::string &motRadical)
    {
        if(noeud == nullptr)
        {
            noeud = new NoeudDicoSynonymes(motRadical);
            nbRadicaux++;
            return;
        }
        if(motRadical < noeud->radical)
            _auxAjouterRadical(noeud->gauche, motRadical);
        else if(motRadical > noeud->radical)
            _auxAjouterRadical(noeud->droit, motRadical);
        else
            throw std::logic_error("Le radical existe déjà !");

        _balance();
    }

    void DicoSynonymes::_auxSupprimerRadical(NoeudDicoSynonymes *noeud, const std::string &motRadical)
    {
        if(noeud == nullptr)
            throw std::logic_error("Le radical a supprimer est absent!");
        if(motRadical < noeud->radical)
            _auxSupprimerRadical(noeud->gauche, motRadical);
        else if(motRadical > noeud->radical)
            _auxSupprimerRadical(noeud->droit, motRadical);
        else if(noeud->gauche != nullptr && noeud->droit != nullptr)
            _supprimerSuccMinDroite(noeud);
        else
        {
            NoeudDicoSynonymes* vieuxNoeud = noeud;
            noeud = (noeud->gauche != nullptr) ? noeud->gauche : noeud->droit;
            delete vieuxNoeud;
            nbRadicaux--;
        }
        _balance();
    }

    void DicoSynonymes::_supprimerSuccMinDroite(NoeudDicoSynonymes *noeud)
    {
        NoeudDicoSynonymes* temp = noeud->droit;
        NoeudDicoSynonymes* parent = noeud;

        while (temp->gauche != 0)
        {
            parent = temp;
            temp = temp->gauche;
        }

        noeud->radical = temp->radical;
        std::swap(noeud->appSynonymes, temp->appSynonymes);
        std::swap(noeud->flexions, temp->flexions);

        if(temp == parent->gauche)
            _auxSupprimerRadical(parent->gauche, temp->radical);
        else
            _auxSupprimerRadical(parent->droit, temp->radical);
    }

    void DicoSynonymes::_balance()
    {
        if(_debalancementAGauche(racine))
        {
            if(_sousArbrePencheADroite(racine->gauche))
                _zigZagGauche(racine);
            else
                _zigZigGauche(racine);
        } else if(_debalancementADroite(racine))
        {
            if(_sousArbrePencheAGauche(racine->droit))
                _zigZagDroit(racine);
            else
                _zigZigDroit(racine);
        }
        else
        {
            if(racine != nullptr)
                racine->hauteur = 1 + std::max(_hauteur(racine->gauche), _hauteur(racine->droit));
        }
    }

    int DicoSynonymes::_hauteur(NoeudDicoSynonymes *arbre)
    {
        if(arbre == nullptr)
            return -1;
        return arbre->hauteur;
    }

    bool DicoSynonymes::_debalancementAGauche(NoeudDicoSynonymes *&arbre)
    {
        if(arbre == nullptr)
            return false;
        return 1 < _hauteur(arbre->gauche) - _hauteur(arbre->droit);
    }

    bool DicoSynonymes::_debalancementADroite(NoeudDicoSynonymes *&arbre)
    {
        if(arbre == nullptr)
            return false;
        return 1 < _hauteur(arbre->droit) - _hauteur(arbre->gauche);
    }

    bool DicoSynonymes::_sousArbrePencheADroite(NoeudDicoSynonymes *&arbre)
    {
        if(arbre == nullptr)
            return false;
        return _hauteur(arbre->gauche) < _hauteur(arbre->droit);
    }

    bool DicoSynonymes::_sousArbrePencheAGauche(NoeudDicoSynonymes *&arbre)
    {
        if(arbre == nullptr)
            return false;
        return _hauteur(arbre->droit) < _hauteur(arbre->gauche);
    }

    void DicoSynonymes::_zigZigGauche(NoeudDicoSynonymes *&arbre)
    {
        NoeudDicoSynonymes *noeud = arbre->gauche;
        arbre->gauche = noeud->droit;
        noeud->droit = arbre;
        arbre->hauteur = 1 + std::max(_hauteur(arbre->gauche), _hauteur(arbre->droit));
        noeud->hauteur = 1 + std::max(_hauteur(noeud->gauche), arbre->hauteur);
        arbre = noeud;
    }

    void DicoSynonymes::_zigZigDroit(NoeudDicoSynonymes *&arbre)
    {
        NoeudDicoSynonymes *noeud = arbre->droit;
        arbre->droit = noeud->gauche;
        noeud->gauche = arbre;
        arbre->hauteur = 1 + std::max(_hauteur(arbre->droit), _hauteur(arbre->gauche));
        noeud->hauteur = 1 + std::max(_hauteur(noeud->droit), arbre->hauteur);
        arbre = noeud;
    }

    void DicoSynonymes::_zigZagGauche(NoeudDicoSynonymes *&arbre)
    {
        _zigZigDroit(arbre->gauche);
        _zigZigGauche(arbre);
    }

    void DicoSynonymes::_zigZagDroit(NoeudDicoSynonymes *&arbre)
    {
        _zigZigGauche(arbre->droit);
        _zigZigDroit(arbre);
    }

    DicoSynonymes::NoeudDicoSynonymes * DicoSynonymes::_auxAppartient(NoeudDicoSynonymes *arbre, const std::string &radical)
    {
        if(arbre == nullptr)
            return nullptr;

        if(radical < arbre->radical)
            return _auxAppartient(arbre->gauche, radical);
        else if(radical > arbre->radical)
            return _auxAppartient(arbre->droit, radical);
        else
            return arbre;
    }



	//Mettez l'implantation des autres méthodes demandées ici.

}//Fin du namespace